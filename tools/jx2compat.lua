-- jx2compat.lua - lop tuong thich de chay script bang hoi JX2 (Linux) tren JX1.
-- VI TRI: bin\server\scriptjx2\jx2compat.lua (NGOAI thu muc script\ vi GameServer
-- tu quet chay moi .lua trong script\ luc khoi dong -> lib JX2 se bao loi mo coi)
-- NAP TRUOC khi goi script tong_jx2: dofile("scriptjx2/jx2compat.lua")
-- Cac ham C da co san (dot 3): GetCurServerTime, OutputMsg, WriteLog, String2Id,
-- GetContribution/AddContribution/..., GetTong, GetTongFigure, GetTongMTask...

-- ==== IncludeLib: JX2 la ham engine map theo TEN MODULE dang ky.
-- JX1 dung bang tra duoi day; thieu module thi bao ro de biet duong bo sung.
TJX_LIBROOT = "scriptjx2/lib/"
TJX_TONGROOT = "scriptjx2/tong_vn/"	-- ban VN hoa (Patch); ban Trung goc o scriptjx2/tong/

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

-- ==== He LEAGUE JX2 (lien-GS) thu gon cho mot GS ====
-- obj tam dung de dang ky; so dang ky that TJX_LG_REG[mod|ten]; gia tri nhiem vu
-- persist qua gb_SetTask/gb_GetTask (module "LG"/"LGM") de song qua restart.
TJX_LG_TMP = {}
TJX_LG_NEXT = 1
TJX_LGM_TMP = {}
TJX_LGM_NEXT = 1
TJX_LG_REG = {}

function LG_CreateLeagueObj()
	local id = TJX_LG_NEXT
	TJX_LG_NEXT = id + 1
	TJX_LG_TMP[id] = {nMod = 0, szName = "", members = {}}
	return id
end

function LG_FreeLeagueObj(id)
	TJX_LG_TMP[id] = nil
	return 1
end

function LG_SetLeagueInfo(id, nMod, szName)
	local o = TJX_LG_TMP[id]
	if (o == nil) then
		return 0
	end
	o.nMod = nMod
	o.szName = szName
	return 1
end

function LG_GetLeagueInfo(lid)
	local r = TJX_LG_REG[lid]
	if (r == nil) then
		return 0, ""
	end
	return r.nMod, r.szName
end

function LGM_CreateMemberObj()
	local id = TJX_LGM_NEXT
	TJX_LGM_NEXT = id + 1
	TJX_LGM_TMP[id] = {szName = ""}
	return id
end

function LGM_SetMemberInfo(mid, szName, nP, nMod, szModName)
	local m = TJX_LGM_TMP[mid]
	if (m == nil) then
		return 0
	end
	m.szName = szName
	return 1
end

function LG_AddMemberToObj(id, mid)
	local o = TJX_LG_TMP[id]
	local m = TJX_LGM_TMP[mid]
	if (o == nil or m == nil) then
		return 0
	end
	tinsert(o.members, m.szName)
	return 1
end

-- dang ky league: chuyen obj tam vao so; callback OnCreate (neu co) goi ngay
function LG_ApplyAddLeague(id, szScript, szFunc)
	local o = TJX_LG_TMP[id]
	if (o == nil) then
		return 0
	end
	local key = o.nMod.."|"..o.szName
	if (TJX_LG_REG[key] == nil) then
		TJX_LG_REG[key] = {nMod = o.nMod, szName = o.szName, members = {}}
	end
	local r = TJX_LG_REG[key]
	local i = 1
	while (o.members[i] ~= nil) do
		tinsert(r.members, o.members[i])
		i = i + 1
	end
	return 1
end

function LG_ApplyRemoveLeague(nMod, szName)
	TJX_LG_REG[nMod.."|"..szName] = nil
	return 1
end

function LG_GetLeagueObj(nMod, szName)
	local key = nMod.."|"..szName
	if (TJX_LG_REG[key] ~= nil) then
		return key
	end
	return -1
end

function LG_GetLeagueTask(lid, nTaskID)
	if (lid == -1 or lid == nil) then
		return 0
	end
	return gb_GetTask("LG", lid.."|"..nTaskID)
end

function LG_ApplySetLeagueTask(nMod, szName, nTaskID, nValue)
	return gb_SetTask("LG", nMod.."|"..szName.."|"..nTaskID, nValue)
end

function LG_ApplyAppendLeagueTask(nMod, szName, nTaskID, nDelta)
	local k = nMod.."|"..szName.."|"..nTaskID
	return gb_SetTask("LG", k, gb_GetTask("LG", k) + nDelta)
end

function LG_GetFirstLeague(nMod)
	local k, v = next(TJX_LG_REG, nil)
	while (k ~= nil) do
		if (v.nMod == nMod) then
			return k
		end
		k, v = next(TJX_LG_REG, k)
	end
	return -1
end

function LG_GetNextLeague(nMod, lid)
	local bFound = 0
	local k, v = next(TJX_LG_REG, nil)
	while (k ~= nil) do
		if (bFound == 1 and v.nMod == nMod) then
			return k
		end
		if (k == lid) then
			bFound = 1
		end
		k, v = next(TJX_LG_REG, k)
	end
	return -1
end

function LGM_ApplyAddMember(lid, szMember)
	local r = TJX_LG_REG[lid]
	if (r == nil) then
		return 0
	end
	tinsert(r.members, szMember)
	return 1
end

function LG_GetMemberTask(lid, szMember, nTaskID)
	return gb_GetTask("LGM", lid.."|"..szMember.."|"..nTaskID)
end

function LG_ApplySetMemberTask(lid, szMember, nTaskID, nValue)
	return gb_SetTask("LGM", lid.."|"..szMember.."|"..nTaskID, nValue)
end

function LG_ApplyAppendMemberTask(lid, szMember, nTaskID, nDelta)
	local k = lid.."|"..szMember.."|"..nTaskID
	return gb_SetTask("LGM", k, gb_GetTask("LGM", k) + nDelta)
end

-- ==== Phuong tho: cua so duc JX2 chua co tren JX1 -> bao ro ====
CURRENCYTYPE_CONTRIBUTION = 2

function FoundryItem(nType)
	Say("Chuc nang DUC/CHE cua phuong tho se mo o ban cap nhat sau.", 0)
	return 0
end
