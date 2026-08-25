-- exp_jx1.lua - [TONGWAR 23/08 phan bien F10] awardtype nExp / nExp_tl cho tbAwardTemplet:
-- nExp    -> AddOwnExp (engine, ScriptFuns.cpp)  - Qua Dai Hoang Kim 200tr, Than Moc Lenh 10tr
-- nExp_tl -> tl_addPlayerExp (task\task_addplayerexp.lua:60) - Hoang Chan Don 2 ty (co tran cap)
-- KHONG Include lib\awardtype\exp.lua goc (Include nguoc awardtemplet -> vong).
Include("\\script\\lib\\log.lua")
-- [23/08 vong Include] KHONG Include task_addplayerexp o day: keo task_head -> ... ->
-- g_activity -> activity -> awardtemplet -> tep nay = VONG VO HAN (stack Overflow). Nap luoi luc Give().

ExpJX1Type = {}

function ExpJX1Type:Give(tbItem, nAwardCount, tbLogTitle)
	local nExp = (tbItem.nExp or 0) * (nAwardCount or 1)
	if nExp > 0 then
		-- [FIX 24/08] AddOwnExp -> DirectAddExp -> LevelUp() dat m_nExp = 0
		-- (KPlayer.cpp:2629) => MAT SACH exp du khi len cap, va chi len DUNG 1 cap.
		-- AddSumExp (ScriptFuns.cpp:8786) cong TUNG CAP mot nen khong mat gi.
		AddSumExp(nExp)
		WriteLog(format("[awardtype exp_jx1] %s +%d exp", GetName() or "", nExp))
		return 1
	end
	return 0
end

ExpTlJX1Type = {}

function ExpTlJX1Type:Give(tbItem, nAwardCount, tbLogTitle)
	local nExp = (tbItem.nExp_tl or 0) * (nAwardCount or 1)
	if nExp > 0 then
		if not tl_addPlayerExp then
			Include("\\script\\task\\task_addplayerexp.lua")	-- nap luoi (xem chu thich dau tep)
		end
		tl_addPlayerExp(nExp)
		WriteLog(format("[awardtype exp_jx1] %s +%d exp (tl)", GetName() or "", nExp))
		return 1
	end
	return 0
end

if tbAwardTemplet then
	tbAwardTemplet:RegType("nExp", ExpJX1Type)
	tbAwardTemplet:RegType("nExp_tl", ExpTlJX1Type)
end
