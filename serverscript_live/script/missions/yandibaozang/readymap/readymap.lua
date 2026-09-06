Include("\\script\\missions\\basemission\\dungeon.lua")
Include("\\script\\missions\\yandibaozang\\readymap\\ready.lua")
Include("\\script\\global\\autoexec_head.lua")

ReadyMap = Dungeon:new_type("yandibaozang_ready")

function ReadyMap:_init()
	self.nCount = 0
	self:AddTimer(18*60, self.OnTime, {self})
	
	local nMapIndex = SubWorldID2Idx(self.nMapId)
	
	local nNpcIndex1 = AddNpc(389, 80, nMapIndex, 1577*32, 3184*32, 0, "Chñ d­îc ®iÕm")
	if nNpcIndex1 > 0 then
		SetNpcScript(nNpcIndex1, "\\script\\missions\\yandibaozang\\npc\\yaoshang.lua")
	end
	
	local nNpcIndex2 = AddNpc(389, 80, nMapIndex, 1607*32, 3178*32, 0, "Chñ d­îc ®iÕm")
	if nNpcIndex2 > 0 then
		SetNpcScript(nNpcIndex2, "\\script\\missions\\yandibaozang\\npc\\yaoshang.lua")
	end
	
	
	return 1
end

function ReadyMap:OnEnterMap()
	self.nCount = self.nCount + 1
	LeaveTeam()
	SetTmpCamp(1);
	SetFightState(0)
	SetLogoutRV(1);
	SetPunish(0);
	SetCreateTeam(0)
	SetPKFlag(1)
	ForbidChangePK(1)
	DisabledUseTownP(1)
	ForbitTrade(1)
	ForbidEnmity(1)	
	SetTaskTemp(200,1)
end

function ReadyMap:OnLeaveMap()
	tbReady:DelPartyMember(GetName())
	self.nCount = self.nCount - 1
	SetTmpCamp(0);
	SetFightState(0)
	SetLogoutRV(0);
	SetPunish(1);
	SetCreateTeam(1)
	SetPKFlag(0)
	ForbidChangePK(0)
	DisabledUseTownP(0)
	ForbitTrade(0)
	ForbidEnmity(0)	
	SetTaskTemp(200,0)
end

function ReadyMap:OnTime()
	if self.nCount == 0 then
		self:close()
		return 0
	end
	return 18*10
end

function ReadyMap:OnClose()
	tbReady.tbMapId[self.nMapId] = nil
	tbReady.nMapCount = tbReady.nMapCount - 1
	tbReady:BroadCast(format("cã ®¹i hiÖp l©m trËn träng th­¬ng, hiÖn t¹i cã %d chi ®éi tham gia.", tbReady.nMapCount))
end

function ReadyMap:GetFreeMap()
	if tbReady.nMapCount >= 15 then
		return 0
	end
	local pDungeon = self:new_dungeon(tbReady.nTemplateMapId)
	if pDungeon == nil then
		return 0
	end
	tbReady.tbMapId[pDungeon.nMapId] = 1
	tbReady.nMapCount = tbReady.nMapCount + 1
	return pDungeon.nMapId
end

local _autoexec = function()
	PreApplyDungeonMap(852,0,0)
end

AutoFunctions:Add(_autoexec)