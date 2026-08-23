-- zhenyuan_jx1.lua -- [TONGCASTLE 23/08] awardtype nZhenYuanPoint (shenmuling Than Moc Lenh):
-- diem Chan Nguyen kinh mach JX1 = PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, n)
-- (khuon item\event\kinhmach\honnguyenchandon.lua:44)
Include("\\script\\lib\\log.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\activitysys\\playerfunlib.lua")

ZhenYuanJX1Type = {}

function ZhenYuanJX1Type:Give(tbItem, nAwardCount, tbLogTitle)
	local nPoint = (tbItem.nZhenYuanPoint or 0) * (nAwardCount or 1)
	if nPoint <= 0 then
		return 0
	end
	PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, nPoint)
	WriteLog(format("[TongCastle awardtype zhenyuan] %s +%d diem Chan Nguyen", GetName() or "", nPoint))
	return 1
end

if tbAwardTemplet then
	tbAwardTemplet:RegType("nZhenYuanPoint", ZhenYuanJX1Type)
end
