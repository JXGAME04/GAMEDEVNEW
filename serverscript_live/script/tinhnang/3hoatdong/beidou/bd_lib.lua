-- ============================================================================
-- SINH TU DONG boi ReverseTools/gen_beidou.py - DUNG SUA TAY.
-- Dich nguoc tu ban Linux script\event\beidoulingpai\ (xem dau tep gen).
-- ============================================================================
-- 3 ham ban Linux dung ma JX1 khong dang ky - viet lai bang ham co san.
Include("\\script\\tinhnang\\3hoatdong\\beidou\\bd_head.lua")

-- Linux: GetServerDate(fmt). JX1 co GetLocalDate cung y nghia.
function BD_ServerDate(fmt)
	return GetLocalDate(fmt)
end

-- Linux: GetItemLevel(idx) / GetItemSeries(idx).
-- JX1: GetItemProp tra DU 6 gia tri genre,detail,particular,LEVEL,SERIES,luck
-- (KJx2WarInfra.cpp:771-793) nen lay thang khong can them ham engine.
function BD_ItemLevel(nItemIdx)
	local g, d, p, lv = GetItemProp(nItemIdx)
	return lv
end
function BD_ItemSeries(nItemIdx)
	local g, d, p, lv, se = GetItemProp(nItemIdx)
	return se
end

-- so lenh bai Bac Dau dang mang (hanh trang)
function BD_DemLenhBai()
	return CalcItemCount(3, 6, 1, 4126, -1)
end
