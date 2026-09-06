-- ============================================================================
-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.
-- Dich nguoc tu ban Linux script\event\beidoulingpai\ (xem dau tep gen).
-- ============================================================================
-- NPC Bac Dau lao nhan. GetNpcValue tra so thu tu thanh (1..7).
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_activity.lua")

function main(NpcIndex)
	local nThanh = GetNpcValue(NpcIndex)
	if (nThanh == nil or nThanh < 1 or nThanh > getn(tbBeidou.THANH)) then
		nThanh = 1
	end
	-- den dung thanh muc tieu = hoan thanh nhiem vu di duong
	tbBeidou:CheckWithCity(nThanh)
	tbBeidou:Menu(nThanh)
end
