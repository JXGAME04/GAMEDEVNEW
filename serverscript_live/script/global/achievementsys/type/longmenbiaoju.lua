Include("\\script\\global\\achievementsys\\head.lua")
Include("\\script\\global\\achievementsys\\simple_detail.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")


--成功运送X星以上的镖车
local LMBJ_Finish = AchievementDetailBase:Derive()
LMBJ_Finish.nParamCount = 1

function LMBJ_Finish:Init(tbParam)
	self.nMinLevel = tonumber(tbParam[1])
	EventSys:GetType("LongMenExpress"):Reg("OnFinish", self.OnMessage, self)
	return 1
end

function LMBJ_Finish:OnMessage(nType)
	if nType >= self.nMinLevel then
		self.tbAchieveType:AchieveTarget(1)
	end
end

AchievementSys:RegDetailType("LMBJ_Finish", LMBJ_Finish)
