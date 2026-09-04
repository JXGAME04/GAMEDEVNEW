Include("\\script\\chienlenh\\cl_def.lua")	-- [CL 04/09] so hieu nhiem vu Chien Lenh

Include("\\script\\missions\\basemission\\dungeon.lua")
Include("\\script\\missions\\yandibaozang\\head.lua")
Include("\\script\\missions\\yandibaozang\\include.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\lib\\log.lua")
-- [LUA4 28/08] JX1 Include = dofile vao state dang goi (moi state Lua rieng, khong
-- co state dung chung nhu Linux) => 'lib' (DoFunInWorld/ShuffleTable) phai
-- Include tuong minh; thieu no la "index global 'lib' (nil)" tai :324.
Include("\\script\\activitysys\\functionlib.lua")
local nPak = curpack()
local Party = {}
Party.nId = 0
Party.nMapId = 0
Party.nPosX = 0
Party.nPosY = 0
Party.tbMember = {}

function Party:new()
	local tb = {}
	for k, v in self do
		tb[k] = v
	end
	return tb
end

function Party:Add(szName)
	self.tbMember[szName] = {}
end

function Party:Get(szName)
	return self.tbMember[szName]
end

function Party:Del(szName)
	self.tbMember[szName] = nil
end

tbReady = {}
tbReady.nTemplateMapId = 852
tbReady.nIntervalTime = 60
tbReady.nReadyTime = 5*60

tbReady.tbPos =
{
	{1590,3195},
	{1590,3170},
	{1601,3184}, 
}

function tbReady:Init()
	self:close()
	self.nMaxPartyId = 0
	self.tbPlayer = {}
	self.tbMapId = {}
	self.nMapCount = 0
	self.nCurTime = 0
	self.nState = 1
	local nOldPak = usepack(%nPak)
	self._timer_ = AddTimer(self.nIntervalTime * 18, "tbReady:OnTime", 0)
	usepack(nOldPak)
end

function tbReady:close()
	if self.OnClose then
		self:OnClose()
	end
	
	if self._timer_ then
		DelTimer(self._timer_)
		self._timer_ = nil
	end
end

function tbReady:BroadCast(szMsg)
	for nMapId, _ in self.tbMapId do
		Msg2Map(nMapId, szMsg)
	end
end

function tbReady:OnTime(nParam, nTimerId)
	print("tbReady:OnTime")
	-- [LUA4] dong ho mo coi (chua Init hoac da close): tu tat, khong cong nil
	if self.nCurTime == nil then
		return 0, nParam
	end
	self.nCurTime = self.nCurTime + self.nIntervalTime
	if self.nState == 1 then
		if self.nCurTime >= self.nReadyTime then
			self.nState = 2
			self:BroadCast(format("Thêi gian b¸o danh v­ît ¶i b¶o tµng viªm ®Õ ®· kÕt thóc, xin mêi c¸c vâ l©m nh©n sü h·y chuÈn bÞ lÇn cuèi, cßn %s b¾t ®Çu.", 30))
			return 30*18, nParam
		else
			self:BroadCast(format("V­ît ¶i b¶o tµng viªm ®Õ cßn <color=yellow>%s phót<color> kÕt thóc thêi gian b¸o danh, h·y nhanh nhanh b¸o danh.",floor((self.nReadyTime -self.nCurTime)/60)))
		end
	elseif self.nState == 2 then
		self.nState = 3
		self:close()
		return 0, nParam
	end
	
	return self.nIntervalTime * 18, nParam
end

function tbReady:NewPartyId()
	self.nMaxPartyId = self.nMaxPartyId + 1
	return self.nMaxPartyId
end

function tbReady:AddPartyMember(nPartyId, szName)
	self.tbPlayer[szName] = nPartyId
end

function tbReady:DelPartyMember(szName)
	print(szName, self.tbPlayer[szName])
	self.tbPlayer[szName] = nil
end


function tbReady:GetParty(szName)
	local nPartyId = self.tbPlayer[szName].nPartyId
	return self.tbParty[nPartyId]
end

function tbReady:Group(tbParty)
	local nGroupCount = getn(tbParty)
	tbParty = lib:ShuffleTable(tbParty, 1, nGroupCount)
	local tbMapGroup = {}
	if nGroupCount == 1 then
		return tbMapGroup
	end
	if 1 < nGroupCount and nGroupCount <= 3 then
		tbMapGroup[1] = {}
		tbMapGroup[1]= tbParty
	elseif nGroupCount > 3 and mod(nGroupCount, 3) == 0 then
		local nMapCount = floor(nGroupCount / 3)
		for i=1, nMapCount do
			tbMapGroup[i] = {}
		end
		for i=1, nGroupCount do
			tinsert(tbMapGroup[floor((i-1) / 3) + 1], tbParty[i])
		end
	elseif nGroupCount > 3 and mod(nGroupCount, 3) ~= 0 then
		local nSpGroup = 2 * (3 - mod(nGroupCount, 3))
		local nMapCount =  floor(nGroupCount / 3) + 1
		for i=1, nMapCount do
			tbMapGroup[i] = {}
		end
		for i=1, nGroupCount - nSpGroup do
			tinsert(tbMapGroup[floor((i-1) / 3) + 1], tbParty[i])
		end
		local offset = floor((nGroupCount - nSpGroup - 1)/3) + 2
		for i=nGroupCount - nSpGroup + 1, nGroupCount do			
			tinsert(tbMapGroup[floor((i - nGroupCount + nSpGroup - 1)/ 2) + offset], tbParty[i])
		end
	end
	return tbMapGroup
end

function tbReady:ShowGroup(tbMapGroup)
	for i=1, getn(tbMapGroup) do 
		print("group", i)
		for j=1, getn(tbMapGroup[i]) do
			print(tbMapGroup[i][j])
		end
	end
end

function tbReady:OnClose()
	if not self.tbPlayer then
		return
	end
	local tbMaplist = {}
	for i=1, getn(YDBZ_MAP_MAP) do
		if SubWorldID2Idx(YDBZ_MAP_MAP[i]) >= 0 then
			tinsert(tbMaplist, YDBZ_MAP_MAP[i])
		end
	end
	local tbPartyMap = {}
	local tbParty = {}
	for szName, nPartyId in self.tbPlayer do
		tbPartyMap[nPartyId] = tbPartyMap[nPartyId] or {}
		tinsert(tbPartyMap[nPartyId], szName)
	end
	self.tbPlayer = {}
	for nPartyId, v in tbPartyMap do
		tinsert(tbParty, v) 
	end
	local nMapCount = getn(tbMaplist)
	local tbMapGroup = self:Group(tbParty)	
	self:ShowGroup(tbMapGroup)
	if getn(tbMapGroup) == 0 then
		self:KickOutGroup(tbParty)
		return
	end
	for i=1, getn(tbMapGroup) do
		if i <= nMapCount then
			self:InitMatchMission(tbMaplist[i], tbMapGroup[i])
		end
	end
	for i=1, getn(tbMapGroup) do
		if i <= nMapCount then
			self:ToMatch(tbMaplist[i], tbMapGroup[i])
		else
			self:KickOutGroup(tbMapGroup[i])
		end
	end
end

function tbReady:ToMatch(nMapId, tbGroup)
	local nMapIndex = SubWorldID2Idx(nMapId)
	if nMapIndex < 0 then
		return
	end
	for i=1, getn(tbGroup) do
		local tbParty = tbGroup[i]
		lib:DoFunInWorld(nMapIndex, SetMissionS, YDBZ_TEAM_NAME[i], tbParty[1])
		for j=1, getn(tbParty) do
			local szName = tbParty[j]
			local nPlayerIndex = SearchPlayer(szName)
			if nPlayerIndex > 0 then
				lib:DoFunInWorld(nMapIndex, AddMSPlayer, YDBZ_MISSION_MATCH, nPlayerIndex, i)
				CallPlayerFunction(nPlayerIndex, self.PlayerEnterMatch, self, i, nMapId)
			end
			
		end
	end
	Msg2Map(nMapId, format("khu vùc ®ã cã Kho B¸u Viªm §Õ %d chi ®éi, ®¹i hiÖp ph¶i cè g¾ng nhÐ!!!", getn(tbGroup)))
end

function tbReady:PlayerEnterMatch(nCamp, nMapId)
	
	NewWorld(nMapId, floor(YDBZ_BOAT_POS[nCamp][1]/32), floor(YDBZ_BOAT_POS[nCamp][2]/32))
	local nCountState = GetTask(2618)
	local nCurDate = tonumber(GetLocalDate("%y%m%d"))
	if floor(nCountState/256) ~= nCurDate then
		
		nCountState = nCurDate * 256
		SetTask(2618, nCountState)
	end
	SetTask(2618, nCountState + 1)
	YDBZ_UpdateTask()
	if YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1,1) ~= 1 then
		YDBZ_sdl_addTaskByte(YDBZ_PLAY_LIMIT_COUNT,2,1)
		YDBZ_sdl_addTaskByte(YDBZ_PLAY_LIMIT_COUNT,4,1)
	end
	SetTask(YDBZ_TEAMS_TASKID,0)
	SetTempRevPos(nMapId, YDBZ_BOAT_POS[nCamp][1], YDBZ_BOAT_POS[nCamp][2])
	YDBZ_sdl_setTaskByte(YDBZ_ITEM_YANDILING,2, nCamp)
	ForbitTrade(0)
	LeaveTeam()
	SetTmpCamp(nCamp);
	SetFightState(1);
	SetLogoutRV(1);
	SetPunish(0);
	SetCreateTeam(0);
	SetPKFlag(1)
	ForbidChangePK(1);
	SetDeathScript(YDBZ_SCRIPT_PLAYER_DEATH);
	DisabledUseTownP(1)
	ForbidEnmity(1)	
	SetTaskTemp(200,1)
	SetDeathType(-1)
	EventSys:GetType("YanDiBaoZang"):OnPlayerEvent("OnEnter", PlayerIndex)
	CL_Xong(CL_NV.VIEMDE)	-- [CL 04/09] Viem De Bao Tang: vao 1 lan
	G_ACTIVITY:OnMessage("SignUpYDBZ", PlayerIndex)
end

function tbReady:KickOutGroup(tbGroup)
	for i=1, getn(tbGroup) do
		local tbParty = tbGroup[i]
		local nMapId, nX, nY = self:GetSignUpPos()
		for j=1, getn(tbParty) do
			local szName = tbParty[j]
			local nPlayerIndex = SearchPlayer(szName)
			if nPlayerIndex > 0 then
				
				CallPlayerFunction(nPlayerIndex, self.KickOut, self, nMapId, nX, nY)
			end
		end
	end
end

function tbReady:GiveItem(tbItem)
	if CalcFreeItemCellCount() >= tbItem.nCount then
		tbAwardTemplet:Give(tbItem, 1, {"yandibaozang", "kick out give item"})
	else
		tbLog:PlayerActionLog("yandibaozang", "kick out give item fail", tbItem.nCount, tbItem.tbProp[1], tbItem.tbProp[2], tbItem.tbProp[3], tbItem.tbProp[4])
	end	
end

function tbReady:KickOut(nMapId, nPosX, nPosY)
	
	local tbItem = {}
	if YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1,1) == 1 then
		tbItem.tbProp={YDBZ_LIMIT_DOUBEL_ITEM[1][1], YDBZ_LIMIT_DOUBEL_ITEM[1][2], YDBZ_LIMIT_DOUBEL_ITEM[1][3], 1, 0, 0}
		tbItem.nCount = YDBZ_LIMIT_DOUBEL_ITEM[2]
	else
		tbItem.tbProp={YDBZ_LIMIT_ITEM[1][1], YDBZ_LIMIT_ITEM[1][2], YDBZ_LIMIT_ITEM[1][3], 1, 0, 0}
		tbItem.nCount = YDBZ_LIMIT_ITEM[2]
	end
	self:GiveItem(tbItem)
	NewWorld(nMapId, nPosX, nPosY)
end

function tbReady:GetSignUpPos()
	local tbPos = 
	{
		{37,1714,3173},
		{37,1642,3145},
		{37,1622,3019},
		{37,1857,2968},
	}
	local nMapIndex = SubWorldID2Idx(37)
	if nMapIndex >= 0 then
		local nIdx = random(1, getn(tbPos))
		-- [LUA4] unpack la ham Lua 5, JX1 dung Lua 4.0.1 nen unpack = nil
		local tb = tbPos[nIdx]
		return tb[1], tb[2], tb[3]
	end	
end

function tbReady:InitMatchMission(nMapId, tbGroup)
	local nMapIndex = SubWorldID2Idx(nMapId)
	if nMapIndex < 0 then
		return
	end
	local nMapId, nX, nY = self:GetSignUpPos()
	lib:DoFunInWorld(nMapIndex, OpenMission, YDBZ_MISSION_MATCH)
	lib:DoFunInWorld(nMapIndex, SetMissionV, YDBZ_SIGNUP_WORLD, nMapId)
	lib:DoFunInWorld(nMapIndex, SetMissionV, YDBZ_SIGNUP_POSX, nX)
	lib:DoFunInWorld(nMapIndex, SetMissionV, YDBZ_SIGNUP_POSY, nY)
	lib:DoFunInWorld(nMapIndex, SetMissionV, YDBZ_TEAM_COUNT, getn(tbGroup))
	lib:DoFunInWorld(nMapIndex, SetMissionV, YDBZ_TEAM_SUM, getn(tbGroup))
end