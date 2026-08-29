-- ============================================================================
-- TEST_HOATDONG_ADMIN.LUA - BO TEST HOAT DONG VIEM DE BAO TANG
-- SINH TU DONG boi ReverseTools\viemde\v39_gen_test_viemde.py - DUNG SUA TAY
-- (sua tay bang trinh soan thao UTF-8 se lam hong toan bo dau tieng Viet TCVN3)
--
-- Include tu lenhbaiadmin.lua; main() cua lenh bai dofile lai chinh no moi lan
-- dung => SUA TEP NAY KHONG CAN KHOI DONG LAI GameServer.
--
-- HAI LUAT CUA MENU:
--   * CAM dau "/" trong NHAN (ScriptFuns.cpp:717 cat o dau "/" dau tien)
--   * Client CHI VE 6 NUT mot luc - moi man duoi day deu <= 6 nut
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
-- Mot dong nay keo theo ca chuoi: driver -> yandibaozang_trigger (YDBZ_OnTrigger)
-- -> readymap\ready.lua (tbReady) -> head.lua (YDBZ_MAP_MAP, YDBZ_BOAT_POS...).
-- Thieu no la bam "bat dau bao danh" se bao "Khong thay YDBZ_OnTrigger".
Include("\\script\\tinhnang\\viemde\\ydbz_driver.lua")

TTHD_PHIEN = "28/08/2026"

-- ---------------------------------------------------------------------------
-- Tien ich
-- ---------------------------------------------------------------------------
function TTHD_In(szDong)
	Msg2Player(szDong)
end

function TTHD_CoKhong(bDung)
	if bDung then
		return "<color=green>Cã<color>"
	end
	return "<color=red>Kh«ng<color>"
end

function TTHD_CoHam(szTen)
	return getglobal(szTen) ~= nil
end

function TTHD_CoMap(nMapId)
	if SubWorldID2Idx == nil then
		return 0
	end
	if SubWorldID2Idx(nMapId) >= 0 then
		return 1
	end
	return 0
end

-- ---------------------------------------------------------------------------
-- MENU GOC  (5 muc + thoat = 6 nut)
-- ---------------------------------------------------------------------------
function TTHD_Root()
	SayEx({"<color=yellow>Bé test Viªm §Õ B¶o Tµng<color> (b¶n " .. TTHD_PHIEN .. ") - chän môc:",
	"1. ChÈn ®o¸n Viªm §Õ (mét l­ît quÐt)/TTHD_ChanDoan",
	"2. B¸o danh vµ vµo trËn/TTHD_BaoDanh",
	"3. Test mét m×nh, kh«ng cÇn tæ ®éi/TTHD_MotMinh",
	"4. VËt phÈm vµ hÖ xóc x¾c/TTHD_VatPham",
	"5. B¶n ®å, NPC vµ cÊu h×nh/TTHD_BanDo",
	"KÕt thóc ®èi tho¹i./no"})
end

-- ---------------------------------------------------------------------------
-- 1) CHAN DOAN
-- ---------------------------------------------------------------------------
function TTHD_ChanDoan()
	TTHD_In("<color=yellow>===== ChÈn ®o¸n Viªm §Õ =====<color>")
	TTHD_In("Giê m¸y chñ: <color=gold>" .. GetLocalDate("%y-%m-%d %H:%M:%S") .. "<color>")

	TTHD_In("<color=yellow>[1] KÞch b¶n<color>")
	TTHD_In("    BËt trong cÊu h×nh: " .. TTHD_CoKhong(HD_CFG("YDBZ_BAT", 1) == 1)
		.. " | Giê ch¹y: " .. HD_CFG("YDBZ_GIO", "?"))
	TTHD_In("    YDBZ_DriverInit: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_DriverInit"))
		.. " | YDBZ_Tick: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_Tick"))
		.. " | YDBZ_OnTrigger: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_OnTrigger")))
	TTHD_In("    tbReady (ruét b¸o danh): " .. TTHD_CoKhong(tbReady ~= nil)
		.. " | YDBZ_restore: " .. TTHD_CoKhong(TTHD_CoHam("YDBZ_restore")))
	TTHD_In("    lib:DoFunInWorld (v¸ 28/08): "
		.. TTHD_CoKhong(lib ~= nil and lib.DoFunInWorld ~= nil))

	TTHD_In("<color=yellow>[2] B¶n ®å<color>")
	local nMapNpc = HD_CFG("YDBZ_NPC_MAP", 37)
	TTHD_In("    B¶n ®å NPC b¸o danh " .. nMapNpc .. ": " .. TTHD_CoKhong(TTHD_CoMap(nMapNpc) == 1))
	local nCo = 0
	local i
	for i = 1003, 1017 do
		nCo = nCo + TTHD_CoMap(i)
	end
	TTHD_In("    Phßng chê 1003-1017: <color=gold>" .. nCo .. "/15<color>")
	nCo = 0
	for i = 853, 862 do
		nCo = nCo + TTHD_CoMap(i)
	end
	TTHD_In("    B¶n ®å trËn 853-862: <color=gold>" .. nCo .. "/10<color>")

	TTHD_In("<color=yellow>[3] HÖ xóc x¾c trong ®éng c¬<color>")
	TTHD_In("    ApplyItemDice: " .. TTHD_CoKhong(TTHD_CoHam("ApplyItemDice"))
		.. " | AddDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("AddDiceItemInfo"))
		.. " | RollItem: " .. TTHD_CoKhong(TTHD_CoHam("RollItem")))
	if not TTHD_CoHam("ApplyItemDice") then
		TTHD_In("    <color=red>Ch­a cã trong DLL - thay CoreServer.dll råi khëi ®éng l¹i.<color>")
	end

	TTHD_In("<color=yellow>[4] §iÒu kiÖn tham gia<color>")
	TTHD_In("    CÊp tèi thiÓu " .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. " | Tæ ®éi cÇn " .. YDBZ_TEAM_COUNT_LIMIT .. " tíi " .. YDBZ_TEAM_COUNT_MAXLIMIT .. " ng­êi")
	TTHD_In("    CÊp cña b¹n: <color=gold>" .. GetLevel() .. "<color>"
		.. " | Tæ ®éi hiÖn t¹i: <color=gold>" .. GetTeamSize() .. "<color> ng­êi")
	TTHD_In("<color=yellow>===== HÕt chÈn ®o¸n =====<color>")
	TTHD_Root()
end

-- ---------------------------------------------------------------------------
-- 2) BAO DANH  (5 muc + quay lai = 6 nut - VUA CHAM tran client)
-- ---------------------------------------------------------------------------
function TTHD_BaoDanh()
	SayEx({"<color=yellow>B¸o danh vµ vµo trËn<color> - ®­êng ®i b×nh th­êng cña ng­êi ch¬i",
	"1. Më ®ît b¸o danh ngay, kh«ng chê tíi giê/TTHD_BD_Mo",
	"2. DÞch chuyÓn tíi NPC b¸o danh ë BiÖn Kinh/TTHD_BD_Tele",
	"3. Xem ®iÒu kiÖn tham gia/TTHD_BD_YeuCau",
	"4. V× sao mét tæ ®éi ch­a ®ñ ®Ó më trËn/TTHD_BD_ViSao",
	"5. §Æt l¹i NPC b¸o danh, hiÖn kÕt qu¶/TTHD_BD_DatNpc",
	"Quay l¹i/TTHD_Root"})
end

function TTHD_BD_Mo()
	if not TTHD_CoHam("YDBZ_OnTrigger") then
		TTHD_In("<color=red>Kh«ng thÊy YDBZ_OnTrigger.<color>")
		TTHD_In("KiÓm dßng Include ydbz_driver.lua ë ®Çu tÖp test_hoatdong_admin.lua.")
		TTHD_BaoDanh()
		return
	end
	YDBZ_OnTrigger()
	TTHD_In("<color=green>§· më ®ît b¸o danh Viªm §Õ.<color>")
	TTHD_In("Thêi gian b¸o danh " .. HD_CFG("YDBZ_PHUT_BAODANH", 5)
		.. " phót. Tíi gÆp B×nh B×nh c« n­¬ng ë BiÖn Kinh.")
	TTHD_In("Nhí lµ cÇn <color=yellow>Ýt nhÊt hai tæ ®éi<color> míi më ®­îc trËn -")
	TTHD_In("muèn thö mét m×nh th× dïng môc 3 ë menu gèc.")
	TTHD_BaoDanh()
end

function TTHD_BD_Tele()
	NewWorld(HD_CFG("YDBZ_NPC_MAP", 37), 1714, 3173)
	TTHD_In("§· dÞch chuyÓn tíi NPC b¸o danh thø nhÊt.")
	TTHD_In("Ba chç cßn l¹i: (1642,3145) (1622,3019) (1857,2968).")
end

-- Goi lai YDBZ_DriverInit: no TU DON NPC cu roi dat lai (idempotent san trong
-- driver) va TRA VE so NPC dat duoc => vua "dem" vua "sua" mot nut, khong can
-- khoi dong lai. Loi in ra console (print) khong doc duoc tu client, nen o day
-- thuat lai ket qua qua Msg2Player.
function TTHD_BD_DatNpc()
	if not TTHD_CoHam("YDBZ_DriverInit") then
		TTHD_In("<color=red>Kh«ng thÊy YDBZ_DriverInit - kiÓm Include ydbz_driver.lua.<color>")
		TTHD_BaoDanh()
		return
	end
	local nDat = YDBZ_DriverInit()
	TTHD_In("Dän NPC cò vµ ®Æt l¹i: <color=gold>" .. nDat .. "/4<color> ®iÓm trªn b¶n ®å "
		.. HD_CFG("YDBZ_NPC_MAP", 37) .. ".")
	if nDat == 0 then
		TTHD_In("<color=red>0 NPC - b¶n ®å ch­a n¹p hoÆc AddNpc lçi, xem console GameServer.<color>")
	else
		TTHD_In("Dïng môc 2 ®Ó dÞch chuyÓn tíi NPC xem tËn m¾t.")
	end
	TTHD_BaoDanh()
end

function TTHD_BD_YeuCau()
	TTHD_In("<color=yellow>===== §iÒu kiÖn tham gia =====<color>")
	TTHD_In("CÊp tèi thiÓu: <color=gold>" .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. "<color> - cÊp cña b¹n: <color=gold>" .. GetLevel() .. "<color>")
	TTHD_In("Tæ ®éi: <color=gold>" .. YDBZ_TEAM_COUNT_LIMIT .. " tíi "
		.. YDBZ_TEAM_COUNT_MAXLIMIT .. "<color> ng­êi - hiÖn cã: <color=gold>"
		.. GetTeamSize() .. "<color>")
	TTHD_In("TrÇn l­ît: " .. HD_CFG("YDBZ_LAN_NGAY", 4) .. " lÇn mét ngµy, "
		.. HD_CFG("YDBZ_LAN_TUAN", 10) .. " lÇn mét tuÇn.")
	TTHD_In("Mang Viªm §Õ LÖnh th× ®­îc miÔn trÇn l­ît.")
	TTHD_BaoDanh()
end

function TTHD_BD_ViSao()
	TTHD_In("<color=yellow>===== V× sao mét tæ ®éi ch­a ®ñ =====<color>")
	TTHD_In("Hµm ghÐp nhãm tbReady:Group nhËn danh s¸ch <color=yellow>tæ ®éi<color>,")
	TTHD_In("kh«ng ph¶i ng­êi ch¬i. Nã cã chèt: nÕu chØ cã <color=yellow>mét<color> tæ ®éi")
	TTHD_In("th× tr¶ vÒ b¶ng rçng, nªn kh«ng ai vµo ®­îc trËn.")
	TTHD_In("Mét b¶n ®å trËn chøa <color=gold>ba<color> tæ ®éi tranh nhau, nªn cÇn Ýt nhÊt hai.")
	TTHD_In("Muèn thö mét m×nh th× dïng môc 3 ë menu gèc - nã ®i th¼ng vµo ruét,")
	TTHD_In("bá qua kh©u ghÐp nhãm.")
	TTHD_BaoDanh()
end

-- ---------------------------------------------------------------------------
-- 3) TEST MOT MINH  (4 muc + quay lai = 5 nut)
-- ---------------------------------------------------------------------------
function TTHD_MotMinh()
	SayEx({"<color=yellow>Test mét m×nh<color> - ®i th¼ng vµo trËn, bá qua b¸o danh vµ ghÐp nhãm",
	"1. Vµo th¼ng trËn mét m×nh, b¶n ®å 853/TTHD_MM_Vao853",
	"2. Chän b¶n ®å trËn kh¸c/TTHD_MM_ChonMap",
	"3. Tho¸t trËn vµ phôc håi tr¹ng th¸i/TTHD_MM_Thoat",
	"4. ChÕ ®é nµy lµm g×, cã an toµn kh«ng/TTHD_MM_GiaiThich",
	"Quay l¹i/TTHD_Root"})
end

-- Ruot cua che do mot minh. Dung DUNG hai ham ma duong bao danh that dung,
-- chi khac la tu dung san mot "nhom" gom mot nguoi.
function TTHD_MM_Vao(nMapId)
	if tbReady == nil then
		TTHD_In("<color=red>Kh«ng thÊy tbReady - kiÓm Include ydbz_driver.lua.<color>")
		return
	end
	if TTHD_CoMap(nMapId) == 0 then
		TTHD_In("<color=red>B¶n ®å " .. nMapId .. " ch­a n¹p vµo m¸y chñ.<color>")
		return
	end
	local tbGroup = {}
	tbGroup[1] = {}
	tbGroup[1][1] = GetName()

	tbReady:InitMatchMission(nMapId, tbGroup)
	tbReady:ToMatch(nMapId, tbGroup)

	TTHD_In("<color=green>§· vµo trËn Viªm §Õ mét m×nh, b¶n ®å " .. nMapId .. ".<color>")
	TTHD_In("B¹n ®ang ë phe 1. §i t×m ¶i ®Ó ®¸nh; boss cuèi lµ L­¬ng Mi Nhi.")
	TTHD_In("H×nh nh©n r¬i qua <color=yellow>hÖ xóc x¾c<color> ë mäi boss - ®ã lµ chç cÇn soi.")
	TTHD_In("Xong th× quay l¹i lÖnh bµi, môc 3 råi môc 3, ®Ó tho¸t cho s¹ch tr¹ng th¸i.")
end

function TTHD_MM_Vao853() TTHD_MM_Vao(853) end
function TTHD_MM_Vao854() TTHD_MM_Vao(854) end
function TTHD_MM_Vao855() TTHD_MM_Vao(855) end
function TTHD_MM_Vao856() TTHD_MM_Vao(856) end
function TTHD_MM_Vao857() TTHD_MM_Vao(857) end

function TTHD_MM_ChonMap()
	SayEx({"<color=yellow>Chän b¶n ®å trËn<color> - m­êi b¶n ®å 853 tíi 862 ®Òu dïng ®­îc",
	"B¶n ®å 854/TTHD_MM_Vao854",
	"B¶n ®å 855/TTHD_MM_Vao855",
	"B¶n ®å 856/TTHD_MM_Vao856",
	"B¶n ®å 857/TTHD_MM_Vao857",
	"Quay l¹i/TTHD_MotMinh"})
end

function TTHD_MM_Thoat()
	if not TTHD_CoHam("YDBZ_restore") then
		TTHD_In("<color=red>Kh«ng thÊy YDBZ_restore - kiÓm Include ydbz_driver.lua.<color>")
		TTHD_MotMinh()
		return
	end
	-- Dung ham don dep CUA CHINH tinh nang (include.lua:34), khong tu bia lai:
	-- no go co chien dau, phe, kich ban chet, cam giao dich, SetTaskTemp(200,0)...
	YDBZ_restore(PlayerIndex, YDBZ_MISSION_MATCH, 0)
	NewWorld(HD_CFG("YDBZ_NPC_MAP", 37), 1714, 3173)
	TTHD_In("<color=green>§· tho¸t trËn vµ phôc håi tr¹ng th¸i, ®­a vÒ BiÖn Kinh.<color>")
	TTHD_In("NÕu cßn kÑt cê chiÕn ®Êu th× tho¸t ra vµo l¹i nh©n vËt.")
end

function TTHD_MM_GiaiThich()
	TTHD_In("<color=yellow>===== ChÕ ®é mét m×nh lµm g× =====<color>")
	TTHD_In("Nã gäi ®óng hai hµm mµ ®­êng b¸o danh thËt vÉn gäi:")
	TTHD_In("   <color=gold>tbReady:InitMatchMission<color> - më nhiÖm vô 50 trªn b¶n ®å trËn")
	TTHD_In("   <color=gold>tbReady:ToMatch<color> - ghi b¹n vµo nhiÖm vô råi ®­a vµo trËn")
	TTHD_In("ChØ kh¸c mét chç: nã tù dùng s½n mét nhãm gåm m×nh b¹n, nªn kh«ng")
	TTHD_In("ph¶i qua kh©u ghÐp nhãm vèn ®ßi Ýt nhÊt hai tæ ®éi.")
	TTHD_In("Mäi thø kh¸c gi÷ nguyªn: cê chiÕn ®Êu, phe, kÞch b¶n chÕt, giíi h¹n l­ît.")
	TTHD_In("<color=red>ChØ nªn dïng trªn m¸y thö.<color> Nã cã tÝnh mét l­ît tham gia trong ngµy.")
	TTHD_MotMinh()
end

-- ---------------------------------------------------------------------------
-- 4) VAT PHAM VA XUC XAC  (3 muc + quay lai = 4 nut)
-- ---------------------------------------------------------------------------
function TTHD_VatPham()
	SayEx({"<color=yellow>VËt phÈm vµ hÖ xóc x¾c<color>",
	"1. NhËn vËt phÈm Viªm §Õ ®Ó thö/TTHD_VP_Nhan",
	"2. Më cöa sæ xóc x¾c ngay, kh«ng cÇn ®¸nh boss/TTHD_VP_XucXac",
	"3. KiÓm t¸m hµm xóc x¾c trong ®éng c¬/TTHD_VP_KiemDLL",
	"Quay l¹i/TTHD_Root"})
end

function TTHD_VP_Nhan()
	AddItem(6, 1, 1614, 1, 0, 0, 0)
	AddItem(6, 1, 1614, 1, 0, 0, 0)
	AddItem(6, 1, 1615, 1, 0, 0, 0)
	AddItem(6, 1, 1626, 1, 0, 0, 0)
	TTHD_In("§· nhËn: 2 H×nh nh©n, 1 Viªm §Õ §å §»ng, 1 Viªm §Õ LÖnh.")
	TTHD_In("H×nh nh©n lµ mãn r¬i qua hÖ xóc x¾c ë mäi boss.")
	TTHD_In("Viªm §Õ LÖnh cho miÔn trÇn l­ît tham gia.")
	TTHD_VatPham()
end

-- Test end-to-end ca he xuc xac. szFile phai la tep ma engine DA NAP va la noi
-- hai ham goi lai ton tai: lenhbaiadmin.lua (no Include tep nay, ma Include cua
-- JX1 = lua_dofile vao trang thai cua tep GOI - ScriptFuns.cpp:1969).
TTHD_XX_TEP = "\\script\\item\\lenhbaiadmin.lua"

function TTHD_VP_XucXac()
	if not TTHD_CoHam("ApplyItemDice") then
		TTHD_In("<color=red>§éng c¬ ch­a cã hµm ApplyItemDice.<color>")
		TTHD_In("Ph¶i thay CoreServer.dll míi råi khëi ®éng l¹i m¸y chñ.")
		TTHD_VatPham()
		return
	end
	local nId = ApplyItemDice(1, 100, 30, TTHD_XX_TEP, "TTHD_XX_Chot", "TTHD_XX_Goi", 1)
	if nId == nil or nId <= 0 then
		TTHD_In("<color=red>ApplyItemDice tr¶ vÒ " .. tostring(nId) .. ".<color>")
		TTHD_VatPham()
		return
	end
	-- Hinh nhan {6,1,1614} - ma cua JX1. Ban Linux la 1605, ma do o JX1 lai la
	-- "Thiep chuc su de" nen truyen nham la hien ra cai phong thu.
	local nItem = AddDiceItemInfo(nId, 0, 6, 1, 1614, 1, 1, 1, 1)
	if nItem == nil or nItem <= 0 then
		TTHD_In("<color=red>AddDiceItemInfo tr¶ vÒ " .. tostring(nItem) .. ".<color>")
		TTHD_VatPham()
		return
	end
	RollItem(nId)
	TTHD_In("<color=green>§· më cöa sæ xóc x¾c - m· phiªn " .. nId .. ".<color>")
	TTHD_In("Soi bèn thø: biÓu t­îng H×nh nh©n, chó gi¶i khi rª chuét,")
	TTHD_In("®ång hå ®Õm lïi, vµ hai nót CÇn víi Bá qua.")
end

function TTHD_XX_Goi(nId, nSo)
	if nSo > 0 then
		TTHD_In("<color=gold>[Xóc x¾c] Phiªn " .. nId .. ": b¹n gieo ®­îc " .. nSo .. " ®iÓm.<color>")
	else
		TTHD_In("<color=gold>[Xóc x¾c] Phiªn " .. nId .. ": b¹n ®· bá qua.<color>")
	end
end

function TTHD_XX_Chot(nId, nThang, nSo)
	TTHD_In("<color=yellow>===== Xóc x¾c ®· chèt =====<color>")
	TTHD_In("M· phiªn " .. nId .. " | Ng­êi th¾ng (chØ sè) " .. nThang .. " | §iÓm " .. nSo)
	if nThang > 0 then
		TTHD_In("<color=green>VËt phÈm ®· trao cho ng­êi th¾ng - kiÓm tói ®å.<color>")
	else
		TTHD_In("Kh«ng ai nhËn, vËt phÈm ®· ®­îc thu håi (®óng thiÕt kÕ, kh«ng rß).")
	end
end

function TTHD_VP_KiemDLL()
	TTHD_In("<color=yellow>===== T¸m hµm xóc x¾c trong ®éng c¬ =====<color>")
	TTHD_In("   ApplyItemDice: " .. TTHD_CoKhong(TTHD_CoHam("ApplyItemDice"))
		.. " | AddDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("AddDiceItemInfo")))
	TTHD_In("   RollItem: " .. TTHD_CoKhong(TTHD_CoHam("RollItem"))
		.. " | GetItemDiceState: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceState")))
	TTHD_In("   DiceLootItem: " .. TTHD_CoKhong(TTHD_CoHam("DiceLootItem"))
		.. " | GetItemDiceRollInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceRollInfo")))
	TTHD_In("   GetItemDiceItemInfo: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDiceItemInfo"))
		.. " | GetItemDicePlayerList: " .. TTHD_CoKhong(TTHD_CoHam("GetItemDicePlayerList")))
	TTHD_In("Cã hµm nµo ghi Kh«ng tøc DLL ®ang ch¹y lµ b¶n cò.")
	TTHD_VatPham()
end

-- ---------------------------------------------------------------------------
-- 5) BAN DO, NPC, CAU HINH  (4 muc + quay lai = 5 nut)
-- ---------------------------------------------------------------------------
function TTHD_BanDo()
	SayEx({"<color=yellow>B¶n ®å, NPC vµ cÊu h×nh<color>",
	"1. §Æt l¹i bèn NPC b¸o danh, gäi lÆp an toµn/TTHD_BanDo_Npc",
	"2. KiÓm mäi b¶n ®å Viªm §Õ ®· n¹p ch­a/TTHD_BanDo_Kiem",
	"3. Xem cÊu h×nh khèi 7 ®ang hiÖu lùc/TTHD_BanDo_Cfg",
	"4. Xem vÞ trÝ hiÖn t¹i cña b¹n/TTHD_BanDo_ViTri",
	"Quay l¹i/TTHD_Root"})
end

function TTHD_BanDo_Npc()
	if not TTHD_CoHam("YDBZ_DriverInit") then
		TTHD_In("<color=red>Kh«ng thÊy YDBZ_DriverInit.<color>")
		TTHD_BanDo()
		return
	end
	local nDat = YDBZ_DriverInit()
	TTHD_In("§· ®Æt <color=gold>" .. nDat .. "<color> NPC b¸o danh.")
	TTHD_In("Hµm nµy tù dän NPC cò tr­íc khi ®Æt l¹i nªn gäi lÆp kh«ng nh©n b¶n.")
	TTHD_BanDo()
end

function TTHD_BanDo_Kiem()
	TTHD_In("<color=yellow>===== B¶n ®å Viªm §Õ =====<color>")
	local nMapNpc = HD_CFG("YDBZ_NPC_MAP", 37)
	TTHD_In("NPC b¸o danh, b¶n ®å " .. nMapNpc .. ": " .. TTHD_CoKhong(TTHD_CoMap(nMapNpc) == 1))
	local szThieu = ""
	local nCo = 0
	local i
	for i = 1003, 1017 do
		if TTHD_CoMap(i) == 1 then
			nCo = nCo + 1
		else
			szThieu = szThieu .. i .. " "
		end
	end
	TTHD_In("Phßng chê 1003-1017: <color=gold>" .. nCo .. "/15<color>")
	if szThieu ~= "" then
		TTHD_In("<color=red>ThiÕu: " .. szThieu .. "<color>")
	end
	szThieu = ""
	nCo = 0
	for i = 853, 862 do
		if TTHD_CoMap(i) == 1 then
			nCo = nCo + 1
		else
			szThieu = szThieu .. i .. " "
		end
	end
	TTHD_In("B¶n ®å trËn 853-862: <color=gold>" .. nCo .. "/10<color>")
	if szThieu ~= "" then
		TTHD_In("<color=red>ThiÕu: " .. szThieu .. "<color>")
		TTHD_In("Khai trong settings\\MapList.ini vµ maps\\WorldSet_GameServer.ini råi khëi ®éng l¹i.")
	end
	TTHD_BanDo()
end

function TTHD_BanDo_Cfg()
	TTHD_In("<color=yellow>===== CÊu h×nh khèi 7 =====<color>")
	TTHD_In("Söa ë script\\header\\cauhinh_hoatdong.lua")
	TTHD_In("YDBZ_BAT = " .. HD_CFG("YDBZ_BAT", 1) .. " | YDBZ_GIO = " .. HD_CFG("YDBZ_GIO", "?"))
	TTHD_In("YDBZ_CAP_TOITHIEU = " .. HD_CFG("YDBZ_CAP_TOITHIEU", 120)
		.. " | YDBZ_PHONG_TOIDA = " .. HD_CFG("YDBZ_PHONG_TOIDA", 15))
	TTHD_In("YDBZ_LAN_NGAY = " .. HD_CFG("YDBZ_LAN_NGAY", 4)
		.. " | YDBZ_LAN_TUAN = " .. HD_CFG("YDBZ_LAN_TUAN", 10))
	TTHD_In("YDBZ_PHUT_BAODANH = " .. HD_CFG("YDBZ_PHUT_BAODANH", 5)
		.. " | YDBZ_PHUT_TRAN = " .. HD_CFG("YDBZ_PHUT_TRAN", 30))
	TTHD_In("<color=red>L­u ý<color>: sè ng­êi tæ ®éi (" .. YDBZ_TEAM_COUNT_LIMIT .. " tíi "
		.. YDBZ_TEAM_COUNT_MAXLIMIT .. ") lµ sè ghi cøng")
	TTHD_In("ë readymap\\include.lua dßng 25-26, kh«ng ®äc tõ cÊu h×nh.")
	TTHD_BanDo()
end

function TTHD_BanDo_ViTri()
	local nW, nX, nY = GetWorldPos()
	TTHD_In("B¶n ®å: <color=gold>" .. nW .. "<color> (" .. GetMapName(nW) .. ")")
	TTHD_In("Täa ®é «: <color=gold>" .. nX .. ", " .. nY .. "<color> | ChØ sè ng­êi ch¬i: " .. PlayerIndex)
	TTHD_In("Tæ ®éi: <color=gold>" .. GetTeamSize() .. "<color> ng­êi | CÊp: <color=gold>"
		.. GetLevel() .. "<color>")
	TTHD_BanDo()
end
