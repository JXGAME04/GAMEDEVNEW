Include("\\script\\global\\achievementsys\\head.lua")
Include("\\script\\global\\achievementsys\\simple_detail.lua")
Include("\\script\\misc\\eventsys\\type\\func.lua")

-- [SAPXEP 06/09] JX1 khong co he thanh tuu (thieu global/achievementsys/head.lua) -> thoat som, khoi ScriptError 4 luc boot
if AchievementDetailBase == nil then return end


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
