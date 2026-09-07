-- ============================================================================
-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.
-- Dich nguoc tu ban Linux script\event\beidoulingpai\ (xem dau tep gen).
-- ============================================================================
-- Bac Dau Huyet Linh Don (Linux beidouxuelingdan.lua): lan nhan nhiem vu ke
-- tiep duoc thuong GAP DOI. Gioi han 15 lan/ngay (task 2999) - y het Linux.
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")

function main(nItemIndex)
	if (GetBitTask(tbBeidou.TSK_AWARD_STATE, tbBeidou.TSK_BIT_XUELINGDAN, 1) == 1) then
		Talk(1, "", MSG_ALREADY_DOUBLE_AWARD)
		return 1
	end
	-- dem theo NGAY: task 2999 luu ngay + so lan
	local nNgay = tonumber(FormatTime2Date(GetCurServerTime()))
	local nLuu = GetTask(tbBeidou.TSK_LIMIT_TIMES)
	local nNgayLuu = floor(nLuu / 100)
	local nLan = mod(nLuu, 100)
	if (nNgayLuu ~= mod(nNgay, 100000)) then
		nLan = 0
	end
	local nMax = HD_CFG("HD3_BD_LINHDAN_NGAY", 15)
	if (nLan >= nMax) then
		Talk(1, "", format("H«m nay ®· dïng %d lÇn, kh«ng thÓ dïng thªm.", nMax))
		return 1
	end
	if (ConsumeItem(3, 1, 6, 1, 4141, -1) ~= 1) then
		return 1
	end
	SetTask(tbBeidou.TSK_LIMIT_TIMES, mod(nNgay, 100000) * 100 + nLan + 1)
	SetBitTask(tbBeidou.TSK_AWARD_STATE, tbBeidou.TSK_BIT_XUELINGDAN, 1, 1)
	Msg2Player(MSG_DOUBLE_AWARD_TIP)
end
