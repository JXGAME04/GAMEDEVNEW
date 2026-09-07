-- ============================================================================
-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.
-- Dich nguoc tu ban Linux script\event\beidoulingpai\ (xem dau tep gen).
-- ============================================================================
-- Bac Dau Chi Bao (Linux beidouzhibao.lua): dung de HOAN THANH ngay nhiem vu.
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")

function main(nItemIndex)
	local nId = GetBitTask(tbBeidou.TSK_TASK_STATE, tbBeidou.TSK_BIT_TASK_ID, tbBeidou.TSK_BIT_ID_LEN)
	if (nId == 0) then
		Talk(1, "", MSG_ERR_NO_TASK)
		return 1
	end
	if (tbBeidou:CheckOverTime() ~= 1) then
		SetTask(tbBeidou.TSK_AWARD_STATE, 0)
		SetTask(tbBeidou.TSK_TASK_STATE, 0)
		Talk(1, "", MSG_TASK_FAIL)
		return 1
	end
	if (tbBeidou:GiveAward() ~= 1) then
		return 1
	end
	if (ConsumeItem(3, 1, 6, 1, 4140, -1) ~= 1) then
		return 1
	end
	SetTask(tbBeidou.TSK_TASK_STATE, 0)
	SetTask(tbBeidou.TSK_FINISH_COUNT, GetTask(tbBeidou.TSK_FINISH_COUNT) + 1)
	SetTask(tbBeidou.TSK_HOUR_COUNT, GetTask(tbBeidou.TSK_HOUR_COUNT) + 1)
	SetBitTask(tbBeidou.TSK_AWARD_STATE, tbBeidou.TSK_BIT_DOUBLE_AWARD, 1, 0)
	Msg2Player(MSG_BEIDOUZHIBAO)
end
