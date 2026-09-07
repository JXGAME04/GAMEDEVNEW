-- ============================================================================
-- ST_LENHBAI_INFO.LUA - SINH boi ReverseTools/port_3hd/thicong/c52_chidan_satthulenh.py
-- Chi dan khi bam chuot phai Sat Thu lenh (398) / Sat thu gian (399).
-- KHONG tru item. Hai ma nay von de cot Script = 0 o ca ban Linux lan du an,
-- nguoi choi bam khong thay gi nen tuong item hong.
-- ============================================================================

function main(nItemIdx)
	local g, d, p, lv = GetItemProp(nItemIdx)
	if (p == 399) then
		Say("S¸t thñ gi¶n kh«ng dïng trùc tiÕp. §©y lµ <color=yellow>vÐ vµo V­ît ¶i<color> - mang theo råi ghi danh ë DÞch Quan, mçi l­ît trõ 1 c¸i.", 0)
		return
	end
	local nSo = CalcItemCount(3, 6, 1, 398, -1)
	Say("S¸t Thñ lÖnh kh«ng dïng trùc tiÕp. Mang tíi <color=yellow>NhiÕp ThÝ TrÇn<color> chän môc hîp thµnh, bá ®óng <color=yellow>5<color> S¸t Thñ lÖnh cïng cÊp sÏ ®­îc 1 S¸t thñ gi¶n (ngò hµnh ngÉu nhiªn).", 0)
	Say("§ang gi÷ tÊt c¶ <color=yellow>"..nSo.."<color> S¸t Thñ lÖnh. S¸t thñ gi¶n dïng lµm vÐ vµo V­ît ¶i.", 0)
end
