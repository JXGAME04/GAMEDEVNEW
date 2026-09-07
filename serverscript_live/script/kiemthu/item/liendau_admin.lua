-- liendau_admin.lua - Bé test Liªn ®Êu cho LÖnh bµi Admin (21/08/2026)
-- §­îc Include bëi \script\item\lenhbaiadmin.lua; lÖnh bµi dofile l¹i mçi
-- lÇn më nªn söa file nµy xong bÊm l¹i lÖnh bµi lµ ch¹y ngay (kh«ng restart).
-- LÖnh Ðp pha ®i ®­êng DynamicExecute -> gsdriver -> "dw" (trÔ chõng 1 gi©y).

LD_GSDRIVER = "\\script\\leaguematch\\gsdriver.lua"
LD_GLB_PHASE = 820
LD_GLB_SID = 821
LD_GLB_MATCHID = 822
LD_GLB_TYPE = 823
LD_GLB_NEXT = 824
LD_GLB_CLOSE = 826
LD_LGTYPE = 5
LD_T_WIN = 1720
LD_T_TIE = 1721
LD_T_TOTAL = 1722
LD_T_POINT = 2500
LD_T_HONOUR = 37
LD_T_RESPECT = 39

LD_TEN_PHA = {
	"1 - nghØ gi÷a mïa (®ang tr¶ th­ëng xÕp h¹ng)",
	"2 - trong mïa, ngoµi giê më trËn",
	"3 - gi÷ chç cho ng­êi vµo l¹i trËn",
	"4 - ®ang më trËn (b¸o danh ®­îc)",
}
LD_TEN_LOAI = {"Song ®Êu", "ChiÕn ®éi m«n ph¸i", "S­ ®å", "Tam nh©n", "§¬n ®Êu", "Liªn hoµn", "Nam n÷ phèi hîp"}

function LD_AdminMenu()
	SayEx({"Bé test Liªn ®Êu (vâ l©m liªn ®Êu WLLS):",
	"Xem tr¹ng th¸i hiÖn t¹i/LD_TrangThai",
	"H­íng dÉn test nhanh (®äc tr­íc)/LD_HuongDan",
	"Më trËn NGAY - test bÊt kú lóc nµo (Ðp pha 4)/LD_EpPha4",
	"§ãng trËn, vÒ trong-mïa (Ðp pha 2)/LD_EpPha2",
	"VÒ nghØ gi÷a mïa - tr¶ th­ëng h¹ng (Ðp pha 1)/LD_EpPha1",
	"Tr¶ vÒ pha thËt theo ®ång hå/LD_PhaThat",
	"Khëi t¹o l¹i driver (nh­ lóc boot)/LD_KhoiTaoLai",
	"Th­ëng vµ chiÕn tÝch (menu con)/LD_MenuThuong",
	"Di chuyÓn nhanh (menu con)/LD_MenuMove",
	"KÕt thóc ®èi tho¹i/no"})
end

function LD_TrangThai()
	local n_pha = GetGlbValue(LD_GLB_PHASE)
	local n_sid = GetGlbValue(LD_GLB_SID)
	local n_mid = GetGlbValue(LD_GLB_MATCHID)
	local n_type = GetGlbValue(LD_GLB_TYPE)
	local n_next = GetGlbValue(LD_GLB_NEXT)
	local n_close = GetGlbValue(LD_GLB_CLOSE)
	local str_pha = LD_TEN_PHA[n_pha]
	if (not str_pha) then str_pha = n_pha.." - ch­a khëi t¹o (driver ch­a ch¹y?)" end
	local str = "Tr¹ng th¸i liªn ®Êu:"
	str = str.."\nPha: "..str_pha
	str = str.."\nMïa gi¶i sè: "..n_sid.."  |  Lo¹i mïa nµy: "..(LD_TEN_LOAI[n_type] or n_type)
	str = str.."\nLo¹i mïa sau: "..(LD_TEN_LOAI[n_next] or n_next)
	str = str.."\nM· trËn hiÖn t¹i: "..n_mid.."  (0 = ch­a cã trËn)"
	local str_kx = "®ang më"
	local str_vl = "®ang më"
	if (GetBit(n_close, 1) ~= 0) then str_kx = "®ang ®ãng" end
	if (GetBit(n_close, 2) ~= 0) then str_vl = "®ang ®ãng" end
	str = str.."\nH¹ng KiÖt xuÊt (80-119): "..str_kx.."  |  H¹ng Vâ l©m (120+): "..str_vl
	str = str.."\n\n§iÓm cña ta:  Danh dù(37) = "..GetTask(LD_T_HONOUR).."  |  Uy danh(39) = "..GetTask(LD_T_RESPECT)
	str = str.."\nTÝch lòy(2500) = "..GetTask(LD_T_POINT)
	str = str.."\nChiÕn tÝch c¸ nh©n: th¾ng "..GetTask(LD_T_WIN).." / hßa "..GetTask(LD_T_TIE).." / tæng "..GetTask(LD_T_TOTAL)
	Talk(1, "LD_AdminMenu", str)
end

function LD_EpPha4()
	DynamicExecute(LD_GSDRIVER, "WLLS_Adm_ForcePhase", 4)
	Msg2Player("§· göi lÖnh më trËn (pha 4). Chê 1-2 gi©y råi bÊm NPC Sø gi¶ / Quan viªn héi tr­êng ®Ó b¸o danh.")
end
function LD_EpPha2()
	DynamicExecute(LD_GSDRIVER, "WLLS_Adm_ForcePhase", 2)
	Msg2Player("§· göi lÖnh vÒ pha 2 (trong mïa, ngoµi giê trËn).")
end
function LD_EpPha1()
	DynamicExecute(LD_GSDRIVER, "WLLS_Adm_ForcePhase", 1)
	Msg2Player("§· göi lÖnh vÒ pha 1 (nghØ gi÷a mïa - l·nh ®­îc th­ëng xÕp h¹ng).")
end
function LD_PhaThat()
	DynamicExecute(LD_GSDRIVER, "WLLS_Adm_PhaThat")
	Msg2Player("§· tr¶ vÒ pha thËt theo ®ång hå m¸y chñ.")
end
function LD_KhoiTaoLai()
	DynamicExecute(LD_GSDRIVER, "WLLS_Adm_KhoiTaoLai")
	Msg2Player("§· khëi t¹o l¹i driver liªn ®Êu (TaskShedule + GameSvrReady).")
end

--=========== menu con: th­ëng vµ chiÕn tÝch ===========
function LD_MenuThuong()
	SayEx({"Th­ëng vµ chiÕn tÝch (test):",
	"Céng 1000 ®iÓm Danh dù - task 37/LD_CongDanhDu",
	"Céng 1000 ®iÓm Uy danh - task 39/LD_CongUyDanh",
	"Céng 10000 ®iÓm tÝch lòy - task 2500/LD_CongTichLuy",
	"§Æt chiÕn tÝch c¸ nh©n: 5 th¾ng, 1 hßa/LD_DatChienTich",
	"Xãa s¹ch chiÕn tÝch vµ ®iÓm tÝch lòy/LD_XoaChienTich",
	"Xem chiÕn ®éi cña ta/LD_DoiCuaTa",
	"Xem h¹ng 1-10 hai b¶ng xÕp h¹ng/LD_Top10",
	"Trë l¹i/LD_AdminMenu",
	"KÕt thóc ®èi tho¹i/no"})
end

function LD_CongDanhDu()
	SetTask(LD_T_HONOUR, GetTask(LD_T_HONOUR) + 1000)
	Msg2Player("Danh dù (task 37) hiÖn cã: "..GetTask(LD_T_HONOUR).." ®iÓm. GÆp Sø gi¶ kiÖt xuÊt ®æi th­ëng (shop 93).")
end
function LD_CongUyDanh()
	SetTask(LD_T_RESPECT, GetTask(LD_T_RESPECT) + 1000)
	Msg2Player("Uy danh (task 39) hiÖn cã: "..GetTask(LD_T_RESPECT).." ®iÓm.")
end
function LD_CongTichLuy()
	SetTask(LD_T_POINT, GetTask(LD_T_POINT) + 10000)
	Msg2Player("§iÓm tÝch lòy (task 2500) hiÖn cã: "..GetTask(LD_T_POINT)..".")
end
function LD_DatChienTich()
	SetTask(LD_T_WIN, 5)
	SetTask(LD_T_TIE, 1)
	SetTask(LD_T_TOTAL, 6)
	Msg2Player("§· ®Æt chiÕn tÝch c¸ nh©n: 5 th¾ng, 1 hßa, 6 tæng trËn.")
end
function LD_XoaChienTich()
	SetTask(LD_T_WIN, 0)
	SetTask(LD_T_TIE, 0)
	SetTask(LD_T_TOTAL, 0)
	SetTask(LD_T_POINT, 0)
	Msg2Player("§· xãa chiÕn tÝch vµ ®iÓm tÝch lòy c¸ nh©n (danh dù, uy danh gi÷ nguyªn).")
end

function LD_DoiCuaTa()
	local n_lid = LG_GetLeagueObjByRole(LD_LGTYPE, GetName())
	if (n_lid == 0 or n_lid == nil) then
		Talk(1, "LD_MenuThuong", "B¹n ch­a ë trong chiÕn ®éi nµo. GÆp Sø gi¶ trong 7 thµnh ®Ó lËp ®éi.")
		return
	end
	local str_lgname, _, n_mem = LG_GetLeagueInfo(n_lid)
	local str = "ChiÕn ®éi: "..str_lgname.."  ("..n_mem.." thµnh viªn)"
	str = str.."\n§iÓm ®éi: "..LG_GetLeagueTask(n_lid, 4).."  |  H¹ng: "..LG_GetLeagueTask(n_lid, 5)
	str = str.."\nTh¾ng "..LG_GetLeagueTask(n_lid, 6).." / hßa "..LG_GetLeagueTask(n_lid, 7).." / tæng "..LG_GetLeagueTask(n_lid, 8)
	Talk(1, "LD_MenuThuong", str)
end

function LD_Top10()
	local str = "B¶ng xÕp h¹ng (10 h¹ng ®Çu):"
	str = str.."\n* KiÖt xuÊt (ladder 10196):"
	local n_co = 0
	for i = 1, 10 do
		local str_ten = Ladder_GetLadderInfo(10196, i)
		if (str_ten ~= nil and str_ten ~= "") then
			str = str.."\n   "..i..". "..str_ten
			n_co = 1
		end
	end
	if (n_co == 0) then str = str.."\n   (trèng)" end
	str = str.."\n* Vâ l©m (ladder 10197):"
	n_co = 0
	for i = 1, 10 do
		local str_ten = Ladder_GetLadderInfo(10197, i)
		if (str_ten ~= nil and str_ten ~= "") then
			str = str.."\n   "..i..". "..str_ten
			n_co = 1
		end
	end
	if (n_co == 0) then str = str.."\n   (trèng)" end
	Talk(1, "LD_MenuThuong", str)
end

--=========== menu con: di chuyÓn nhanh ===========
function LD_MenuMove()
	SayEx({"Di chuyÓn nhanh (täa ®é chuÈn cña hÖ liªn ®Êu):",
	"§Õn héi tr­êng KiÖt xuÊt - map 396/LD_Move396",
	"§Õn héi tr­êng Vâ l©m - map 540/LD_Move540",
	"§Õn khu chuÈn bÞ KiÖt xuÊt - map 560/LD_Move560",
	"§Õn khu chuÈn bÞ Vâ l©m - map 570/LD_Move570",
	"VÒ Ba L¨ng HuyÖn/LD_MoveVe",
	"Trë l¹i/LD_AdminMenu",
	"KÕt thóc ®èi tho¹i/no"})
end
function LD_Move396()
	SetLogoutRV(0)
	NewWorld(396, 1523, 3024)
end
function LD_Move540()
	SetLogoutRV(0)
	NewWorld(540, 1523, 3024)
end
function LD_Move560()
	SetLogoutRV(0)
	NewWorld(560, 1596, 2977)
end
function LD_Move570()
	SetLogoutRV(0)
	NewWorld(570, 1596, 2977)
end
function LD_MoveVe()
	SetLogoutRV(0)
	NewWorld(53, 1626, 3260)
end

function LD_HuongDan()
	local str = "Tr×nh tù test mét trËn (cÇn 2 tµi kho¶n ®Ó ra trËn thËt):"
	str = str.."\n1. BÊm [Më trËn NGAY], chê 2 gi©y; vµo [Xem tr¹ng th¸i] thÊy pha 4 lµ ®­îc."
	str = str.."\n2. GÆp Sø gi¶ ë thµnh ®óng d¶i cÊp (KiÖt xuÊt 80-119, Vâ l©m 120 trë lªn), chän [ChiÕn ®éi h¹ng ... cña ta] ®Ó lËp ®éi vµ ®Æt tªn."
	str = str.."\n3. Muèn ®ñ 2 ng­êi: tµi kho¶n 2 lËp tæ ®éi víi ®éi tr­ëng, råi ®éi tr­ëng vµo l¹i môc trªn, chän thªm ®éi h÷u vµo chiÕn ®éi."
	str = str.."\n4. Chän [Ta muèn ®Õn khu thi ®Êu h¹ng ...] ®Ó vµo héi tr­êng. Ch­a cã chiÕn ®éi th× Quan viªn sÏ lÆng lÏ ®­a vÒ thµnh - kh«ng ph¶i lçi."
	str = str.."\n5. GÆp Quan viªn héi tr­êng, chän [Ta muèn tham chiÕn], x¸c nhËn 2 b­íc, c¶ ®éi ®­îc kÐo vµo khu chuÈn bÞ."
	str = str.."\n6. CÇn 2 ®éi cïng h¹ng trë lªn ®ang chê; hÖ tù ghÐp cÆp råi kÐo vµo ®Êu tr­êng (ThÞ vÖ b¸o sè phót chuÈn bÞ)."
	str = str.."\n7. §¸nh xong xem ®iÓm ë Sø gi¶ [Xem chiÕn tÝch cña ®éi]. Test xong bÊm [Tr¶ vÒ pha thËt theo ®ång hå]."
	Talk(1, "LD_AdminMenu", str)
end
