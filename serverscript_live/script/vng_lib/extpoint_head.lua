-- ================================================================================================
-- [HE THONG] script/vng_lib/extpoint_head.lua
-- Muc dich  : Dinh nghia diem mo rong.
-- Duoc nap  : Include tu 1 tep (vd extpoint.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham       : (khong co - tep du lieu/cau hinh)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
if not tbExtPointLib then 
	tbExtPointLib = {}
end

tbExtPointLib.tbList = {
	[3] = {
		[1] = 4, --Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[2] = 4, --Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[3] = 4, --Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[4] = 4, --Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[5] = 4,--Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[6] = 4,--Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[7] = 4,--Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[8] = 4,--Ver 4[22/08/2013 Æ’n 24h ngµy 04/03/2015] Hµo PhÛ Tri ¢n
		[9] = 1,
		[10] = 1,
		[11] = 1,
		[12] = 1,
		[13] = 1,
		[14] = 1,
		[15] = 1,
		--[20/06/2013 - 24h 24/06/2013]: 1 trao gi∂i THDNB 13 
	},
	[4] = {
		[1] = 1,		
		[2] = 2,
		[3] = 2,
		[4] = 2,
		[5] = 2,
		--Trao ≠u Æ∑i ingame ch≠¨ng tr◊nh VIP 2013
		--Ver 2: Trao ≠u Æ∑i ingame ch≠¨ng tr◊nh VIP 2013 - ThanhLD
		[6] = 2, 
		--Reset pass security, kh´ng h’t hπn, by ThanhLD 20131115
		[7] = 2,
		--Ver 3 [from 2013-12-26 00:00 to 2013-12-29 24:00] awards playground Fanpage
		[8] = 1,
		[9] = 1,
		[10] = 1,
		[11] = 1,
		[12] = 2,
		-- [21/10/2013 - 24h 27/08/2014] Trao th≠Îng item VLMC 5 - ThanhLD
		[13] = 1,
		-- [17/10/2013 - 24h 31/08/2014] Trao vﬂng hµo quang VLMC 5 - ThanhLD
		[14] = 1,
		[15] = 1,
	},
	[5] = {
		[1] = 2,
		--Ver 2 [_/_/2014 --> 24h _/_/2014] Ghi nhÀn nhÀn Æ≠Óc ThÀp Ni™n L÷nh Bµi
		[2] = 4,
		--Ver 4 [ch≠a c„ ngµy k’t thÛc] K›ch hoπt khi tπo account mÌi sau ngay 9/4/2015
		[3] = 4,
		--Ver 4 [ch≠a c„ ngµy k’t thÛc] NhÀn th≠Îng g„i hÁ trÓ 30 ngµy ch¨i 1role/1acc
		[4] = 5,
		--Ver 5 Het han ngay 9-4-2015
		[5] = 3,
		--Ver 3 [03/07/2014 --> 24h 31/12/2014] NhÀn th≠Îng Tri ¢n g„i VIP vµng ho∆c bπc, ThanhLD
		[6] = 6, 	
		--Ver 6  [20150422- 20150610]  NhÀn th≠Îng Nhan Thuong Vo Lam Hoi Ngo- Van Loi Tri An_ Moc 1
		[7] = 5,
		--Ver 5 [20150422- 20150610]  NhÀn th≠Îng Nhan Thuong Vo Lam Hoi Ngo- Van Loi Tri An_ Moc 2
		[8] = 4,
		--Ver 4 [20150422- 20150610]  NhÀn th≠Îng Nhan Thuong Vo Lam Hoi Ngo- Van Loi Tri An_ Moc 3
		[9] = 3,
		--Ver 3 [20150422- 20150610]  NhÀn th≠Îng Nhan Thuong Vo Lam Hoi Ngo- Van Loi Tri An_ Moc 4
		[10] = 5,
		--Ver5 [18/012/2014 --> 24h 21/12/2014] Trao th≠Îng gi∂i Æ u CCTalk 
		[11] = 6,
		--Ver 6 [20150115 --> 24h 20150121] Trao th≠Îng VLMC6 - ThanhLD
		[12] = 6,
		--Ver 6 [29/08/2016 --> 24h 11/09/2016] Trao th≠Îng Tham gia gi∂i VLNB 08/2016 - NgaVN
		[13] = 4,
		--Ver 4 [24h 31/03/2015]  Ho Tro Clicker - ThanhLD	
		[14] = 4,
		--Ver 4 [24h 31/03/2015]  Ho Tro Clicker - ThanhLD	
		[15] = 5,
		--Ver 3 [07/01/2015 --> 24h 13/01/2015] TNhÀn th≠Îng Fanpage th∏ng 12-2014 - Thanhld
		--Ver 5 [29/01/2014 --> 24h 04/02/2015] Trao th≠Îng Lπc Tuy’t Thanh Bπch - NgaVN
	},
}