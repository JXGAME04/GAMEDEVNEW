-- zhenyuan_jx1.lua -- [TONGCASTLE 23/08] awardtype nZhenYuanPoint (shenmuling Than Moc Lenh):
-- diem Chan Nguyen kinh mach JX1 = PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, n)
-- (khuon item\event\kinhmach\honnguyenchandon.lua:44)
Include("\\script\\lib\\log.lua")
Include("\\script\\lib\\lib_task.lua")
-- [23/08 vong Include] KHONG Include playerfunlib o day: playerfunlib:4 Include nguoc
-- awardtemplet -> awardtemplet lai Include tep nay = VONG VO HAN (stack Overflow moi state,
-- ScriptError.log phinh hang chuc MB, GameServer boot khong len). Nap luoi luc Give().

ZhenYuanJX1Type = {}

function ZhenYuanJX1Type:Give(tbItem, nAwardCount, tbLogTitle)
	local nPoint = (tbItem.nZhenYuanPoint or 0) * (nAwardCount or 1)
	if nPoint <= 0 then
		return 0
	end
	if not PlayerFunLib then
		Include("\\script\\activitysys\\playerfunlib.lua")	-- nap luoi (xem chu thich dau tep)
	end
	PlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, nPoint)
	WriteLog(format("[TongCastle awardtype zhenyuan] %s +%d diem Chan Nguyen", GetName() or "", nPoint))
	-- [FIX 24/08] ban Linux (lib\awardtype\zhenyuanpoint.lua:13) co thong bao nay;
	-- thieu no thi dung Than Moc Lenh xong khong biet co nhan duoc gi khong.
	Msg2Player(format("Nh­n ®Úîc %d ®iÓm ch©n nguyªn", nPoint))
	return 1
end

if tbAwardTemplet then
	tbAwardTemplet:RegType("nZhenYuanPoint", ZhenYuanJX1Type)
end
