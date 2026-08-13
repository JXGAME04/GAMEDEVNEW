-- jx2compat.lua - lop tuong thich de chay script bang hoi JX2 (Linux) tren JX1.
-- VI TRI: bin\server\scriptjx2\jx2compat.lua (NGOAI thu muc script\ vi GameServer
-- tu quet chay moi .lua trong script\ luc khoi dong -> lib JX2 se bao loi mo coi)
-- NAP TRUOC khi goi script tong_jx2: dofile("scriptjx2/jx2compat.lua")
-- Cac ham C da co san (dot 3): GetCurServerTime, OutputMsg, WriteLog, String2Id,
-- GetContribution/AddContribution/..., GetTong, GetTongFigure, GetTongMTask...

-- ==== IncludeLib: JX2 la ham engine map theo TEN MODULE dang ky.
-- JX1 dung bang tra duoi day; thieu module thi bao ro de biet duong bo sung.
TJX_LIBROOT = "scriptjx2/lib/"
TJX_TONGROOT = "scriptjx2/tong/"

TJX_LIBMAP = {
	TONG = { TJX_TONGROOT.."tong_header.lua" },
	FILE = { TJX_LIBROOT.."file.lua" },
	LOG = { TJX_LIBROOT.."log.lua" },
	STRING = { TJX_LIBROOT.."string.lua" },
	BASIC = { TJX_LIBROOT.."basic.lua" },
	COMMON = { TJX_LIBROOT.."common.lua" },
	SAY = { TJX_LIBROOT.."say.lua" },
	PLAYER = { TJX_LIBROOT.."player.lua" },
	AWARD = { TJX_LIBROOT.."award.lua" },
	TIMERLIST = { TJX_LIBROOT.."timerlist.lua" },
	TOPLIST = { TJX_LIBROOT.."toplist.lua" },
	MAPDB = { TJX_LIBROOT.."mapdb.lua" },
	GB_TASK = { TJX_LIBROOT.."gb_taskfuncs.lua" },
}

TJX_LIBLOADED = {}

function IncludeLib(szName)
	if (szName == nil) then
		return 0
	end
	if (TJX_LIBLOADED[szName] == 1) then
		return 1
	end
	local tFiles = TJX_LIBMAP[szName]
	if (tFiles == nil) then
		OutputMsg("IncludeLib: CHUA DANG KY module ["..szName.."] trong jx2compat.lua")
		return 0
	end
	local i = 1
	while (tFiles[i] ~= nil) do
		dofile(tFiles[i])
		i = i + 1
	end
	TJX_LIBLOADED[szName] = 1
	return 1
end

-- ==== Include kieu JX2 (duong dan tuyet doi \script\...) -> tro ve cay JX1
function Include(szPath)
	if (szPath == nil) then
		return 0
	end
	-- doi \script\tong\... -> script/tong_jx2/...
	local s = gsub(szPath, "\\", "/")
	s = gsub(s, "^/", "")
	s = gsub(s, "^script/tong/", TJX_TONGROOT)
	s = gsub(s, "^script/lib/", TJX_LIBROOT)
	return dofile(s)
end

-- ==== gb_* (kho bien toan cuc tren relay cua JX2, chay qua he League) ====
-- JX1 mot GameServer: giu bang Lua toan cuc + luu file de song qua restart.
-- CHUA dong bo lien-GS (he League lam o giai doan sau).
TJX_GBTASK = {}
TJX_GBFILE = "logs/gbtask_jx2.txt"

function gb_GetTask(szModule, szName)
	local k = szModule.."|"..szName
	if (TJX_GBTASK[k] == nil) then
		return 0
	end
	return TJX_GBTASK[k]
end

function gb_SetTask(szModule, szName, nValue)
	local k = szModule.."|"..szName
	TJX_GBTASK[k] = nValue
	WriteStringToFile(TJX_GBFILE, k.."="..nValue.."\n")
	return 1
end

function gb_AppendTask(szModule, szName, nDelta)
	return gb_SetTask(szModule, szName, gb_GetTask(szModule, szName) + nDelta)
end

-- ==== cac ham phu con thieu -> tam thoi bao va tra gia tri an toan ====
function Msg2PlayerByName(szName, szMsg)
	OutputMsg("Msg2PlayerByName("..szName..") - chua ho tro, dung Msg2Player khi co ngu canh")
	return 0
end

function GetPlayerRev()
	return 0
end

function WriteInf(sz)      WriteLog(sz) return 1 end
function WriteInfTB(t, sz) WriteLog(sz) return 1 end
function WriteErr(sz)      WriteLog("[ERR] "..sz) return 1 end
function WriteErrTB(t, sz) WriteLog("[ERR] "..sz) return 1 end
function WriteWar(sz)      WriteLog("[WAR] "..sz) return 1 end
function WriteWarTB(t, sz) WriteLog("[WAR] "..sz) return 1 end
function WriteTongMoneyChangeLog(sz) WriteLog("[TIEN-BANG] "..sz) return 1 end
