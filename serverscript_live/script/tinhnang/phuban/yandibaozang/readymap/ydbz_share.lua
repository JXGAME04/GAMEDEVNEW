-- [LOCAL54 06/09 toi] 3 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local YDBZ_GetRV, YDBZ_SetRV, YDBZ_MoMissionNeo
-- ==========================================================================
-- VIEM DE - LOP DUNG CHUNG TRANG THAI (ban JX1)
-- SINH TU DONG boi ReverseTools\viemde\v10_gen_share.py - DUNG SUA TAY
--
-- Ban Linux giu hang doi bao danh trong bang Lua `tbReady` va dung chung duoc
-- giua cac tep vi may chu do chay MOT state Lua. JX1 cho MOI TEP mot state
-- rieng, nen o day moi thu dung chung deu nam trong KHO CUA ENGINE:
--   * so lieu  -> bien mission cua ban do NEO (khuon 852, khong ai vao)
--   * danh sach nguoi bao danh -> AddMSPlayer/GetNextPlayer, dung y het co che
--     ma chinh ban goc dung cho pha danh tran (ready.lua:214)
-- ==========================================================================

Include("\\script\\missions\\basemission\\dungeon.lua")

YDBZ_NEO_MAPID        = 852     -- ban do khuon, dung lam noi giu so lieu chung
-- !! KHONG dung 50/51: ban Linux da danh 50 cho mission TRAN DANH va 51 cho
-- mission phong cho cu (settings	ask\missions.txt) - da khai dung nhu vay o JX1.
-- Khe 60 chi la O CHUA NGUOI (khong can dong script trong missions.txt: engine
-- doc GetString(id+1,2) tra rong thi khong chay script nao, van tao o binh thuong).
YDBZ_MISSION_READY    = 60      -- mission giu danh sach nguoi dang bao danh
YDBZ_RV_STATE         = 90      -- 0 chua mo, 1 dang bao danh, 2 chuan bi, 3 da bat dau
YDBZ_RV_MAXPARTY      = 91      -- bo dem id to doi
YDBZ_RV_CURTIME       = 92      -- so giay da troi qua trong pha bao danh

-- Hai hang so nay GIU DUNG SO CUA BAN LINUX:
--   200  = co TaskTemp "dang trong mot hoat dong" - JX1 dung chung cho 17 tinh nang
--          (leaguematch/head.lua:519 chu thich ro: cam doi tran doanh). Bo soat id
--          bao "200 da bi dung" la BAO DONG GIA: day la quy uoc dung chung, khong
--          phai xung dot - doi so moi la lam sai khac ban goc.
--   2618 = task dem so lan tham gia trong ngay (bo soat da xac nhan JX1 chua ai dung).
YDBZ_TASKTEMP_TRONGTRAN = 200
YDBZ_TASK_DEMNGAY       = 2618


-- ---------------------------------------------------------------- so lieu
function YDBZ_GetRV(nIdx)
	return DG_GetVar(YDBZ_NEO_MAPID, nIdx)
end

function YDBZ_SetRV(nIdx, nValue)
	return DG_SetVar(YDBZ_NEO_MAPID, nIdx, nValue)
end

-- Mo mission tren ban do neo (bay #4: GetMissionV tra 0 ca khi mission chua ton
-- tai, nen phai hoi IsMission chu khong hoi gia tri).
function YDBZ_MoMissionNeo()
	local nIdx = SubWorldID2Idx(YDBZ_NEO_MAPID)
	if nIdx < 0 then
		print("[YDBZ] ban do neo "..YDBZ_NEO_MAPID.." CHUA NAP")
		return 0
	end
	local nOld = SubWorld
	SubWorld = nIdx
	if IsMission(YDBZ_MISSION_READY) ~= 1 then
		OpenMission(YDBZ_MISSION_READY)
	end
	SubWorld = nOld
	return 1
end


-- ------------------------------------------------- danh sach nguoi bao danh
-- Ban Linux: tbReady.tbPlayer[szName] = nPartyId
-- Ban JX1  : nguoi choi duoc ghi vao mission YDBZ_MISSION_READY, nhom = nPartyId
function YDBZ_ThemNguoi(nPlayerIndex, nPartyId)
	if nPlayerIndex <= 0 then
		return 0
	end
	YDBZ_MoMissionNeo()
	local nIdx = SubWorldID2Idx(YDBZ_NEO_MAPID)
	if nIdx < 0 then
		return 0
	end
	local nOld = SubWorld
	SubWorld = nIdx
	AddMSPlayer(YDBZ_MISSION_READY, nPlayerIndex, nPartyId)
	SubWorld = nOld
	local nCu = PlayerIndex
	PlayerIndex = nPlayerIndex
	SetTask(YDBZ_TEAMS_TASKID, nPartyId)
	PlayerIndex = nCu
	return 1
end

function YDBZ_BoNguoi(nPlayerIndex)
	if nPlayerIndex <= 0 then
		return 0
	end
	local nIdx = SubWorldID2Idx(YDBZ_NEO_MAPID)
	if nIdx < 0 then
		return 0
	end
	local nOld = SubWorld
	SubWorld = nIdx
	DelMSPlayer(YDBZ_MISSION_READY, nPlayerIndex, 0)
	SubWorld = nOld
	return 1
end

-- Tra ve bang {[nPartyId] = {chi so nguoi choi, ...}} - thay cho vong lap
-- `for szName, nPartyId in self.tbPlayer` cua ban Linux (ready.lua:173).
-- Bay #3: GetNextPlayer(id, idx, 0) coi 0 la TAT CA, con GetMSPlayerCount(id, 0)
-- lai dem RIENG nhom 0 - nen o day chi dung GetNextPlayer voi nhom 0.
function YDBZ_LayDanhSach()
	local tbParty = {}
	local nIdx = SubWorldID2Idx(YDBZ_NEO_MAPID)
	if nIdx < 0 then
		return tbParty
	end
	local nOld = SubWorld
	local nCu = PlayerIndex
	SubWorld = nIdx
	local i = 0
	local p = 0
	local nVong = 0
	while nVong < 500 do
		nVong = nVong + 1
		i, p = GetNextPlayer(YDBZ_MISSION_READY, i, 0)
		if p and p > 0 then
			PlayerIndex = p
			local nPartyId = GetTask(YDBZ_TEAMS_TASKID)
			tbParty[nPartyId] = tbParty[nPartyId] or {}
			tinsert(tbParty[nPartyId], p)
		end
		if not i or i <= 0 then
			break
		end
	end
	PlayerIndex = nCu
	SubWorld = nOld
	return tbParty
end

function YDBZ_XoaDanhSach()
	local nIdx = SubWorldID2Idx(YDBZ_NEO_MAPID)
	if nIdx < 0 then
		return 0
	end
	local nOld = SubWorld
	SubWorld = nIdx
	if IsMission(YDBZ_MISSION_READY) == 1 then
		CloseMission(YDBZ_MISSION_READY)
	end
	OpenMission(YDBZ_MISSION_READY)
	SubWorld = nOld
	return 1
end


-- ------------------------------------------------------------- be phong cho
-- Ban Linux: tbReady.nMapCount / tbReady.tbMapId
-- Ban JX1  : suy thang tu co ban cua be pho ban
function YDBZ_DemPhong()
	local tbPool = DUNGEON_POOL[YDBZ_NEO_MAPID]
	if not tbPool then
		return 0
	end
	local n = 0
	local i
	for i = 1, getn(tbPool) do
		if DG_GetVar(tbPool[i], DG_V_BUSY) ~= 0 then
			n = n + 1
		end
	end
	return n
end

function YDBZ_MoiPhong(szMsg)
	local tbPool = DUNGEON_POOL[YDBZ_NEO_MAPID]
	if not tbPool then
		return
	end
	local i
	for i = 1, getn(tbPool) do
		if DG_GetVar(tbPool[i], DG_V_BUSY) ~= 0 then
			Msg2Map(tbPool[i], szMsg)
		end
	end
end


-- ---------------------------------------------------------------- id to doi
function YDBZ_IdToDoiMoi()
	local n = YDBZ_GetRV(YDBZ_RV_MAXPARTY) + 1
	YDBZ_SetRV(YDBZ_RV_MAXPARTY, n)
	return n
end
