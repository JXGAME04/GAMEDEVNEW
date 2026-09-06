-- ==========================================================================
-- KHUNG PHO BAN DONG (dungeon) - ban JX1
-- SINH TU DONG boi ReverseTools\viemde\v03_gen_dungeon.py - DUNG SUA TAY
--
-- Dich nguoc tu \script\missions\basemission\dungeon.lua cua ban Linux,
-- viet lai cho JX1 vi 2 khac biet co ban:
--   * JX1 moi tep Lua mot state rieng => trang thai chung phai nam o BIEN
--     MISSION cua chinh ban do (SubWorld[i].m_MissionArray), khong dung bien Lua.
--   * JX1 nap ban do mot lan luc khoi dong => dung BE ban do tinh khai truoc
--     trong MapList.ini + WorldSet_GameServer.ini (1003-1017).
-- ==========================================================================

-- --- chi so bien mission dung cho quan ly be (KMission.h: 1..99; Viem De
-- --- dung 1..49 tren ban do TRAN nen 90.. la vung trong) ---
DG_V_BUSY   = 90    -- 0 = ranh, khac 0 = mot to doi dang dung
DG_V_TPL    = 91    -- id ban do khuon ma phong nay nhan ban tu do
DG_V_COUNT  = 92    -- so nguoi dang dung trong phong
DG_V_TYPE   = 93    -- ma loai pho ban (xem DG_TYPE_*)
DG_V_SERIAL = 94    -- so thu tu lan cap phat, de phan biet hai luot dung chung id

DG_TYPE_YDBZ_READY = 1   -- phong cho Viem De Bao Tang

-- --- be ban do theo tung khuon ---
DUNGEON_POOL = {}
DUNGEON_POOL[852] = {1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017}


-- ==========================================================================
-- Doc/ghi bien mission cua MOT ban do bat ky (tu dong tra lai SubWorld cu)
-- ==========================================================================
function DG_GetVar(nMapId, nIdx)
	local nIdxMap = SubWorldID2Idx(nMapId)
	if nIdxMap < 0 then
		return 0
	end
	local nOld = SubWorld
	SubWorld = nIdxMap
	local nRet = GetMissionV(nIdx)
	SubWorld = nOld
	return nRet
end

function DG_SetVar(nMapId, nIdx, nValue)
	local nIdxMap = SubWorldID2Idx(nMapId)
	if nIdxMap < 0 then
		return 0
	end
	local nOld = SubWorld
	SubWorld = nIdxMap
	SetMissionV(nIdx, nValue)
	SubWorld = nOld
	return 1
end


-- ==========================================================================
-- Cap phat / thu hoi ban do trong be  (thay cho 3 ham engine cua ban Linux)
-- ==========================================================================

-- Ban Linux: PreApplyDungeonMap(nTpl, 0, 0) - dat truoc mot so ban sao.
-- Ban JX1: be la TINH, ham nay chi don sach trang thai thua cua lan chay truoc
-- (vi du may chu vua khoi dong lai giua chung mot dot).
function PreApplyDungeonMap(nTpl, nUnused1, nUnused2)
	local tbPool = DUNGEON_POOL[nTpl]
	if not tbPool then
		print("[DUNGEON] PreApplyDungeonMap: khuon "..nTpl.." chua khai be")
		return 0
	end
	local i
	local nOk = 0
	for i = 1, getn(tbPool) do
		local nMapId = tbPool[i]
		if SubWorldID2Idx(nMapId) >= 0 then
			DG_SetVar(nMapId, DG_V_BUSY, 0)
			DG_SetVar(nMapId, DG_V_COUNT, 0)
			DG_SetVar(nMapId, DG_V_TPL, nTpl)
			DG_SetVar(nMapId, DG_V_TYPE, 0)
			nOk = nOk + 1
		else
			print("[DUNGEON] ban do be "..nMapId.." CHUA NAP - kiem MapList.ini va WorldSet_GameServer.ini")
		end
	end
	print("[DUNGEON] be khuon "..nTpl..": "..nOk.."/"..getn(tbPool).." ban do san sang")
	return nOk
end

-- Tra ve id ban do ranh trong be, hoac 0 neu het.
function ApplyDungeonMap(nTpl)
	local tbPool = DUNGEON_POOL[nTpl]
	if not tbPool then
		return 0
	end
	local i
	for i = 1, getn(tbPool) do
		local nMapId = tbPool[i]
		if SubWorldID2Idx(nMapId) >= 0 and DG_GetVar(nMapId, DG_V_BUSY) == 0 then
			DG_SetVar(nMapId, DG_V_BUSY, 1)
			DG_SetVar(nMapId, DG_V_COUNT, 0)
			DG_SetVar(nMapId, DG_V_TPL, nTpl)
			DG_SetVar(nMapId, DG_V_SERIAL, DG_GetVar(nMapId, DG_V_SERIAL) + 1)
			return nMapId
		end
	end
	return 0
end

-- Chu y CHINH TA: ban Linux viet sai thanh 'Dungenon' (thieu chu). Giu nguyen
-- ten do de kich ban goc goi duoc, va them mot ten viet dung lam bi danh.
function ReturnDungenonMap(nTpl, nMapId)
	if not nMapId or nMapId <= 0 then
		return 0
	end
	DG_SetVar(nMapId, DG_V_BUSY, 0)
	DG_SetVar(nMapId, DG_V_COUNT, 0)
	DG_SetVar(nMapId, DG_V_TYPE, 0)
	return 1
end

function ReturnDungeonMap(nTpl, nMapId)
	return ReturnDungenonMap(nTpl, nMapId)
end

-- Ban Linux con co ApplyDelDungeonMap (xoa han ban sao). Be tinh khong xoa duoc,
-- nen anh xa ve thu hoi cho khoi vo kich ban nao lo goi.
function ApplyDelDungeonMap(nTpl, nMapId)
	return ReturnDungenonMap(nTpl, nMapId)
end


-- ==========================================================================
-- Lop Dungeon (giu nguyen mat ngoai cua ban Linux de kich ban goc goi duoc)
-- ==========================================================================
DungeonList = {}
DungeonType = {}
DungeonTypeCount = {}
Dungeon = {}

function Dungeon:new_type(szKey)
	local tb = {}
	local k, v
	for k, v in pairs(self) do
		tb[k] = v
	end
	tb.szDungeonType = szKey
	DungeonType[szKey] = tb
	return tb
end

function Dungeon:new(nMapId, nTemplateMapId, tbInitParam)
	if DungeonList[nMapId] then
		DungeonList[nMapId]:close()
	end
	local tb = {}
	local k, v
	for k, v in pairs(self) do
		tb[k] = v
	end
	tb.nTemplateMapId = nTemplateMapId
	tb.nMapId = nMapId
	tb.tbNpc = {}
	if SubWorldID2Idx(nMapId) < 0 then
		return nil
	end
	DungeonTypeCount[self.szDungeonType] = (DungeonTypeCount[self.szDungeonType] or 0) + 1
	DungeonList[nMapId] = tb
	-- ban Linux goi tb:_init(unpack(tbInitParam)); Lua 4.0 cua JX1 khong co unpack
	-- nen truyen thang bang table (kich ban Viem De khong dung tham so nao).
	local bRet = tb:_init(tbInitParam)
	if not bRet then
		tb:free()
		return nil
	end
	return tb
end

function Dungeon:new_dungeon(nTemplateMapId, tbInitParam)
	local nMapId = ApplyDungeonMap(nTemplateMapId)
	if nMapId > 0 then
		local p = self:new(nMapId, nTemplateMapId, tbInitParam)
		if not p then
			ReturnDungenonMap(nTemplateMapId, nMapId)
		end
		return p
	end
end

function Dungeon:_init(tbParam)
	return 1
end

function Dungeon:OnEnterMap()
end

function Dungeon:OnLeaveMap()
end

-- --- so nguoi trong phong: doc/ghi qua bien mission nen MOI TEP deu thay ---
function Dungeon:GetCount()
	return DG_GetVar(self.nMapId, DG_V_COUNT)
end

function Dungeon:AddCount(nDelta)
	local n = DG_GetVar(self.nMapId, DG_V_COUNT) + nDelta
	if n < 0 then
		n = 0
	end
	DG_SetVar(self.nMapId, DG_V_COUNT, n)
	return n
end

-- --- NPC do pho ban sinh ra: nho lai de don sach luc dong ---
function Dungeon:RememberNpc(nNpcIndex)
	if nNpcIndex and nNpcIndex > 0 then
		tinsert(self.tbNpc, nNpcIndex)
	end
	return nNpcIndex
end

function Dungeon:ClearNpc()
	local i
	for i = 1, getn(self.tbNpc) do
		if self.tbNpc[i] and self.tbNpc[i] > 0 then
			DelNpc(self.tbNpc[i])
		end
	end
	self.tbNpc = {}
end

function Dungeon:AddTimer(nFrame, fn, tbParam)
	local nTimerId = AddTimer(nFrame, "Dungeon:OnTime", self.nMapId)
	if nTimerId and nTimerId > 0 then
		self._timer_ = self._timer_ or {}
		self._timer_[nTimerId] = {fn, tbParam}
	end
	return nTimerId
end

function Dungeon:OnTime(nMapId, nTimerId)
	local pDungeon = DungeonList[nMapId]
	if not pDungeon or not pDungeon._timer_ then
		return 0, 0
	end
	local tb = pDungeon._timer_[nTimerId]
	if not tb then
		return 0, 0
	end
	local nFrame = call(tb[1], tb[2])
	if nFrame and nFrame > 0 then
		return nFrame, nMapId
	end
	if pDungeon._timer_ then
		pDungeon._timer_[nTimerId] = nil
	end
	return 0, 0
end

function Dungeon:free()
	local szType = self.szDungeonType
	if DungeonTypeCount[szType] then
		DungeonTypeCount[szType] = DungeonTypeCount[szType] - 1
		if DungeonTypeCount[szType] <= 0 then
			DungeonTypeCount[szType] = nil
		end
	end
	if self._timer_ then
		local nTimerId, tb
		for nTimerId, tb in pairs(self._timer_) do
			DelTimer(nTimerId)
		end
		self._timer_ = nil
	end
	self:ClearNpc()
	DungeonList[self.nMapId] = nil
	if self.nTemplateMapId then
		ReturnDungenonMap(self.nTemplateMapId, self.nMapId)
	end
end

function Dungeon:close()
	if self.OnClose then
		self:OnClose()
	end
	self:free()
end

-- Ban Linux con co Dungeon:SetForbitItem (cam mang vat pham theo loai ban do).
-- JX1 khong co he set_MapType/add_Item2Map, de rong cho kich ban goc goi khong vo.
function Dungeon:SetForbitItem(nMapId)
end

-- Ban Linux: SubWorldName(nIdx) tra ten ban do. JX1 chua co ham engine nay,
-- kich ban chi dung de in log go roi (player_death.lua:20).
function SubWorldName(nIdxSubWorld)
	return "map#"..nIdxSubWorld
end
