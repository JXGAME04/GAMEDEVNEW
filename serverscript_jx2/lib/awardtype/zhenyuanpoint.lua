Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\lib\\log.lua")

ZhenYuanPointType = {}
ZhenYuanPointType.nPak = curpack()
ZhenYuanPointType.TASK_ID = 4000


function ZhenYuanPointType:AddZhenYuanPoint(nAmount, tbLogTitle)
	
	SetTask(self.TASK_ID, GetTask(self.TASK_ID) + nAmount)
	SyncTaskValue(self.TASK_ID)
	Msg2Player(format("NhËn ®­îc %d ®iÓm ch©n nguyªn", nAmount))
	self:WriteLog(nAmount, tbLogTitle)
end

function ZhenYuanPointType:Give(tbItem, nAwardCount, tbLogTitle)
	if not tbItem.nZhenYuanPoint then
		return 
	end
	local nPlayerIndex = PlayerIndex
	local nAmount = tbItem.nZhenYuanPoint * (nAwardCount or 1) * (tbItem.nCount or 1)
	local nPak = usepack(self.nPak)
	CallPlayerFunction(nPlayerIndex, self.AddZhenYuanPoint, self, nAmount, tbLogTitle)
	usepack(nPak)
	
end


function ZhenYuanPointType:WriteLog(nAmount, tbLogTitle)
	local szCode = ""
	if tbLogTitle then
		local szEventName = tbLogTitle[1] or ""
		local szAction = tbLogTitle[2] or "award"
		local szName = "nZhenYuanPoint"
		call(tbLog.PlayerAwardLog, {tbLog, szEventName, szAction, szName, szCode, nAmount}, "x")
	end
end
		
tbAwardTemplet:RegType("nZhenYuanPoint", ZhenYuanPointType)