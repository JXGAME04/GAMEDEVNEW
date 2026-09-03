-- simcity_admin.lua - Menu admin cho tinh nang SimCity (bot nguoi choi gia lap)
-- Port SimCity JX2 -> JX1. Xem BANGIAO_SIMCITY_JX1.md.
--
-- Goi tu lenhbaiadmin.lua: muc "SimCity - bot gi¶ lËp/SC_Menu"
-- Cac ham C dung o day dang ky trong ScriptFuns.cpp (KSimCity.cpp, server-only):
--   SC_AddBot(nSex,nLevel,nSubWorldIdx,nMpsX,nMpsY,nSeries[,szName][,nFaction][,nLifeMax]) -> nNpcIdx/0
--   SC_DelBot(nNpcIdx) / SC_ClearBots() -> so bot da xoa
--   SC_MoveOn() / SC_MoveOff()
--   SC_Goto(nNpcIdx,nMpsX,nMpsY)
--   SC_PatrolBox(nNpcIdx[,nHalfMps])
--   SC_LoadPreset(szPresetPath,szPathName) -> nRouteId/-1
--   SC_SetBotRoute(nNpcIdx,nRouteId[,bLoop])
--   SC_SetBotFlag / SC_GetBotFlag
--
-- LUU Y duong dan preset: PHAI dung gach xuoi "/" vi Lua 4.0 nuot escape "\"
--   ("\t" -> TAB, "\1" -> ky tu 0x01). g_GetFullPath nhan ca "/".

SC_END_SAY   = "KÕt thóc ®èi tho¹i./no"
SC_PRESETDIR = "/settings/simcity/maps/thanhthi/"
SC_CHATFILE  = "/settings/simcity/chat.txt"
SC_CHATRATE  = 40                 -- 40/1000 moi giay moi bot (~1 cau / 25 giay / bot)
SC_ChatLoaded = 0
SC_ChatState  = 0

-- bo nho tam cua phien admin (moi lan mo menu)
SC_LastBot   = 0    -- bot vua sinh
SC_LastRoute = -1   -- lo trinh vua nap
SC_MoveState = 0    -- 1 = driver di chuyen dang BAT (chi de hien thi tren menu)

-- ================= MENU CHINH =================
function SC_Menu()
	local nMove = "T¾t"
	if SC_MoveState == 1 then
		nMove = "BËt"
	end
	-- LUU Y: KHONG duoc dung ky tu "|" trong bat ky chuoi nao.
	-- sUiAppendAnswer (ScriptFuns.cpp:548) dung "|" lam DAU PHAN CACH giua tieu de
	-- va tung lua chon, tat ca trong 1 buffer 512 byte (MAX_SCIRPTACTION_BUFFERNUM).
	-- Co "|" trong tieu de -> client tach sai -> mat het cac dong chon.
	local nChat = "T¾t"
	if SC_ChatState == 1 then
		nChat = "BËt"
	end
	SayEx({format("<color=yellow>SimCity - bot gi¶ lËp<color>\nDi chuyÓn: <color=green>%s<color>  Nãi chuyÖn: <color=green>%s<color>", nMove, nChat),
	"Sinh bot - tuÇn tra - lé tr×nh/SC_SinhMenu",
	"Driver di chuyÓn: BËt/SC_On",
	"Driver di chuyÓn: T¾t/SC_Off",
	"Bot nãi chuyÖn: BËt/SC_ChatOn",
	"Bot nãi chuyÖn: T¾t/SC_ChatOff",
	"Trang trÝ thµnh thÞ (map nµy)/SC_City",
	"Xo¸ hÕt bot/SC_Clear",
	"Bot ng­êi ch¬i thËt (KPlayer)/PB_Menu",
	SC_END_SAY})
end

-- (21/08) tach ra menu con: SC_Menu cu ~581 byte, VUOT bo dem 512 cua
-- sUiAppendAnswer nen bi cat cut, dong chon cuoi mat ten ham -> bam vao la loi.
function SC_SinhMenu()
	SayEx({format("<color=yellow>SimCity - sinh bot<color>\nBot võa sinh: <color=gold>%d<color>  Lé tr×nh: <color=gold>%d<color>", SC_LastBot, SC_LastRoute),
	"Sinh 1 bot t¹i chç t«i ®øng/SC_SpawnHere",
	"Sinh 5 bot t¹i chç t«i ®øng/SC_Spawn5",
	"Cho bot võa sinh ®i tuÇn tra/SC_PatrolLast",
	"§i theo lé tr×nh b¶n ®å nµy/SC_RouteMenu",
	"Quay l¹i/SC_Menu",
	SC_END_SAY})
end

-- ================= TRANG TRI THANH THI (18/08) =================
-- Khuon ban goc pthanhthi.lua: sap hang NGOI BAN tum quanh dia danh (tiem thuoc,
-- tap hoa, Da Tau...) + dan di dao rai tren luoi node cua map. Du lieu:
--   settings/simcity/maps/thanhthi.txt (chi muc WorldID -> file node)
--   settings/simcity/maps/attractions.txt (dia danh) + stall_adv.txt (bien sap)
--   settings/simcity/names.txt (737 ten ban goc)
SC_CitySeq = 0
function SC_TenThanhThi()
	local s = SC_RandomName()
	if s and s ~= "" then
		SC_CitySeq = SC_CitySeq + 1
		return format("%s%d", s, SC_CitySeq)
	end
	return SC_MakeName()
end

function SC_City()
	local nW, nX, nY = GetWorldPos()
	local nSwIdx = SubWorldID2Idx(nW)
	local nSet = SC_CityNodes(nW)
	if not nSet or nSet < 0 then
		Msg2Player(format("Map %d kh«ng cã d÷ liÖu node (maps/thanhthi.txt).", nW))
		SC_Menu()
		return
	end
	local nSap = 0
	local nDi = 0
	-- SAP BAN truoc (diem nhan thi giac): ngoi + bien hieu, quanh dia danh
	local nMuonSap = 20 + random(0, 10)
	for i = 1, nMuonSap do
		local sx, sy = SC_PickSpawn(nSet, 1, nW, nSwIdx)
		if sx and sx > 0 then
			local nIdx = 0
			for t = 1, 5 do
				nIdx = SC_AddBot(random(0, 1), 85 + random(0, 15), nSwIdx, sx, sy,
				                 random(0, 4), SC_TenThanhThi())
				if nIdx and nIdx > 0 then
					break
				end
			end
			if nIdx and nIdx > 0 then
				SC_DressBot(nIdx, 0)
				SC_SetBotFaction(nIdx, random(0, 9))
				SC_SetBotInfo(nIdx, 0, random(0, 8000), random(0, 300), random(0, 2))
				-- stall + sit dat NGAY sau khi sinh - m_BaiTan chi di trong goi sync
				-- luc client VAO region, khong co goi cap nhat rieng
				SC_SetBotStall(nIdx, 1)
				SC_SetBotSit(nIdx)
				nSap = nSap + 1
			end
		end
	end
	-- DAN DI DAO: rai deu, tuan tra quanh cho dung (PatrolBox 16 o)
	for i = 1, 40 do
		local sx, sy = SC_PickSpawn(nSet, 0, nW, nSwIdx)
		if sx and sx > 0 then
			local nIdx = 0
			for t = 1, 5 do
				nIdx = SC_AddBot(random(0, 1), 80 + random(0, 20), nSwIdx, sx, sy,
				                 random(0, 4), SC_TenThanhThi())
				if nIdx and nIdx > 0 then
					break
				end
			end
			if nIdx and nIdx > 0 then
				SC_GiveIdentity(nIdx)
				SC_PatrolBox(nIdx, 512)
				nDi = nDi + 1
			end
		end
	end
	SC_MoveOn()
	SC_LastBot = SC_LastBot + nSap + nDi
	Msg2Player(format("Trang trÝ map %d: %d s¹p ngåi b¸n + %d d©n ®i d¹o.", nW, nSap, nDi))
	SC_Menu()
end

-- ================= BOT NGUOI CHOI THAT (KPlayerBot) =================
-- KHAC HAN bot SimCity o tren:
--   bot SimCity = KNpc kind_player, khong co KPlayer -> chi di lai + noi chuyen.
--   bot nay     = KPlayer THAT nap tu roledb qua Goddess -> co trang bi that, chi so that,
--                 xem tin tuc / PM / vao phai / luyen cap deu di duong chinh thong.
-- Dieu kien: tai khoan "1".."1000" phai co san nhan vat trong roledb
-- (da tao bang tools\taobot_bdb, nhan ban tu tai khoan mau hinodl).
function PB_Menu()
	local nCo, nTran = PB_BotCount()
	SayEx({format("<color=yellow>Bot ng­êi ch¬i thËt<color>\n§ang sèng: <color=gold>%d<color> / tèi ®a <color=gold>%d<color>", nCo, nTran),
	"Gäi bot - Vµo ph¸i - L­u - Gì/PB_GoiMenu",
	"ChÕ ®é: ®¸nh qu¸i - chat - buff/PB_CheDoMenu",
	"NhiÖm vô D· TÈu/PB_DaTauMenu",
	"Ra thµnh b¸n s¹p/PB_SapMenu",
	"Gäi vÒ thµnh - th«n/PB_VtMenu",
	"Tham gia Tèng Kim/PB_TkMenu",
	"Bang héi bot/PB_BangMenu",
	"Quay l¹i SimCity/SC_Menu",
	SC_END_SAY})
end

-- (21/08) BA MENU CON. PB_Menu cu gom 18 dong ~764 byte, vuot xa bo dem 512 cua
-- sUiAppendAnswer (ScriptFuns.cpp:540-554) nen bi cat cut giua chung: dong chon
-- cuoi mat ten ham va bam vao la loi. Moi menu duoi day <= ~350 byte.
function PB_GoiMenu()
	local nCo, nTran = PB_BotCount()
	SayEx({format("<color=yellow>Gäi bot / qu¶n lý <color>\n§ang sèng: <color=gold>%d<color> / tèi ®a <color=gold>%d<color>", nCo, nTran),
	"Gäi 1 bot/#PB_Call(1,1)",
	"Gäi 100 bot/#PB_Call(1,100)",
	"Gäi 1000 bot/#PB_Call(1,1000)",
	"Cho bot vµo ph¸i (theo ngò hµnh)/PB_Join",
	"L­u d÷ liÖu bot ngay/PB_Save",
	"Gì hÕt bot ng­êi ch¬i/PB_Clear",
	"Quay l¹i/PB_Menu",
	SC_END_SAY})
end

function PB_CheDoMenu()
	SayEx({"<color=yellow>ChÕ ®é bot<color>",
	"BËt ®¸nh qu¸i/#PB_Fight(1)",
	"T¾t ®¸nh qu¸i/#PB_Fight(0)",
	"BËt bot nãi chuyÖn/#PB_Chat(40)",
	"T¾t bot nãi chuyÖn/#PB_Chat(0)",
	"ChÊm TTL + QuÕ Hoa Töu: BËt/#PB_Buff(1)",
	"ChÊm TTL + QuÕ Hoa Töu: T¾t/#PB_Buff(0)",
	"Quay l¹i/PB_Menu",
	SC_END_SAY})
end

function PB_VtMenu()
	SayEx({"<color=yellow>Gäi bot vÒ thµnh - th«n<color>",
	"BËt (bot vÒ 8 thµnh, chia ®Òu)/#PB_VeThanhBat(1)",
	"T¾t (bot vÒ b·i luyÖn)/#PB_VeThanhBat(0)",
	"Quay l¹i/PB_Menu",
	SC_END_SAY})
end

-- ================= BANG HOI BOT (02/09) =================
-- 5 bang toan bot (settings\simcity\botbang.txt): tao bang - tuyen thanh vien - dung 18h ngay bao danh
-- dau thau cong thanh - admin nap quy. Ham C (KPlayerBot.cpp): PB_BangTao / PB_BangTuyen / PB_BangCTC /
-- PB_BangNap / PB_BangSo / PB_BangTen / PB_BangTT. Xem bot.log muc [BotBang5].
function PB_BangMenu()
	local nBat = PB_BangCTC(-1)
	local sBat = "T¾t"
	if nBat == 1 then
		sBat = "BËt"
	end
	SayEx({format("<color=yellow>Bang héi bot<color>\n§Êu thÇu c«ng thµnh theo giê (18h ngµy b¸o danh): <color=green>%s<color>", sBat),
	"Xem tr¹ng th¸i c¸c bang bot/PB_BangXem",
	"T¹o 5 bang (bot cÊp cao nhÊt lµm bang chñ)/PB_BangTaoGo",
	"TuyÓn thµnh viªn/PB_BangTuyenMenu",
	"BËt ®Êu thÇu theo giê/#PB_BangCTCBat(1)",
	"T¾t ®Êu thÇu theo giê/#PB_BangCTCBat(0)",
	"N¹p quü bang/PB_BangNapMenu",
	"N¹p l¹i botbang.txt/PB_BangNapLai",
	"Quay l¹i menu bot/PB_Menu",
	SC_END_SAY})
end

function PB_BangXem()
	local s = PB_BangTT()
	if (s == nil or s == "") then
		s = "(ch­a n¹p ®­îc botbang.txt)"
	end
	SayEx({"<color=yellow>Bang bot (stt.tªn tt ng­êi quü thµnh chñ)<color>\n"..s,
	"Quay l¹i/PB_BangMenu",
	SC_END_SAY})
end

function PB_BangTaoGo()
	local n = PB_BangTao(5)
	Msg2Player(format("§· giao %d bot ®i lËp bang (bot vÒ NPC m«n ph¸i xuÊt s­ råi lËp bang; xem bot.log [BotBang5]).", n))
	PB_BangMenu()
end

function PB_BangTuyenMenu()
	local t = {"<color=yellow>TuyÓn thµnh viªn<color>\nChän bang (môc tiªu theo botbang.txt; bot ®ñ cÊp, ch­a bang tù vÒ xin)"}
	local n = PB_BangSo()
	for i = 1, n do
		tinsert(t, format("%s/#PB_BangTuyenGo(%d)", PB_BangTen(i), i))
	end
	tinsert(t, "Quay l¹i/PB_BangMenu")
	tinsert(t, SC_END_SAY)
	SayEx(t)
end

function PB_BangTuyenGo(k)
	local n = PB_BangTuyen(k, 0)
	if (n > 0) then
		Msg2Player(format("Bang %s: b¾t ®Çu tuyÓn tíi %d thµnh viªn (xem bot.log [BotBang5]).", PB_BangTen(k), n))
	elseif (n == -2) then
		Msg2Player("Bang nµy ch­a cã trªn m¸y chñ (ch­a t¹o, hoÆc relay ch­a ®ång bé - chê 30 gi©y råi thö l¹i).")
	else
		Msg2Player("Sè thø tù bang kh«ng hîp lÖ.")
	end
	PB_BangMenu()
end

function PB_BangCTCBat(n)
	PB_BangCTC(n)
	if (n == 1) then
		Msg2Player("§· bËt: 18h00-18h55 ngµy b¸o danh cña thµnh môc tiªu, bang chñ bot tíi Sø Gi¶ C«ng Thµnh ®Êu thÇu (cÇn ®ñ 37 ng­êi, quü > 1 triÖu ®ñ 750 gi©y, quü ®ñ phÝ).")
	else
		Msg2Player("§· t¾t ®Êu thÇu theo giê.")
	end
	PB_BangMenu()
end

function PB_BangNapLai()
	local n = PB_BangSo(1)
	-- [BOTBANG5b] -1 = con bot dang di lam viec bang
	if (n < 0) then
		Msg2Player("Ch­a n¹p l¹i ®­îc: cßn bot ®ang ®i lËp bang / xin bang / ®Êu thÇu. Thö l¹i sau.")
	else
		Msg2Player(format("§· n¹p l¹i botbang.txt: %d bang (chiÕn dÞch tuyÓn bÞ ®Æt l¹i).", n))
	end
	PB_BangMenu()
end

function PB_BangNapMenu()
	local t = {"<color=yellow>N¹p quü bang<color>\nChän bang nhËn tiÒn"}
	local n = PB_BangSo()
	for i = 1, n do
		tinsert(t, format("%s/#PB_BangNapChon(%d)", PB_BangTen(i), i))
	end
	tinsert(t, "Bang cña t«i/#PB_BangNapChon(0)")
	tinsert(t, "Quay l¹i/PB_BangMenu")
	tinsert(t, SC_END_SAY)
	SayEx(t)
end

function PB_BangNapChon(k)
	PB_BangNapK = k
	local sTen = "bang cña t«i"
	if (k > 0) then
		sTen = PB_BangTen(k)
	end
	SayEx({format("<color=yellow>N¹p quü cho %s<color>\nChän sè tiÒn", sTen),
	"1 triÖu l­îng/#PB_BangNapGo(1000000)",
	"5 triÖu l­îng/#PB_BangNapGo(5000000)",
	"20 triÖu l­îng/#PB_BangNapGo(20000000)",
	"100 triÖu l­îng/#PB_BangNapGo(100000000)",
	"Quay l¹i/PB_BangNapMenu",
	SC_END_SAY})
end

function PB_BangNapGo(nLuong)
	local k = PB_BangNapK or 0
	local nTongID = 0
	if (k == 0) then
		local szTen, nID = GetTongName()
		if (nID == nil or nID == 0) then
			Msg2Player("B¹n ch­a cã bang héi.")
			PB_BangNapMenu()
			return
		end
		nTongID = nID
	end
	local r = PB_BangNap(k, nLuong, nTongID)
	if (r == 1) then
		Msg2Player(format("§· göi lÖnh n¹p %d l­îng vµo ng©n quü (bot.log [BotBang5]). Më l¹i Xem tr¹ng th¸i sau vµi gi©y.", nLuong))
	else
		Msg2Player("Kh«ng n¹p ®­îc: bang ch­a cã trªn m¸y chñ hoÆc sè tiÒn kh«ng hîp lÖ.")
	end
	PB_BangMenu()
end

-- ================= TONG KIM (21/08) =================
-- Bot tu tham gia Tong Kim y het nguoi choi: ve map bao danh 324, thoat party,
-- di bo toi NPC bao danh, bam gia nhap, cho 90 giay trong hau doanh roi ra tran,
-- chay sang doanh trai doi phuong tim nguoi danh. Het tran tu dung Than Hanh Phu
-- len lai bai luyen dung cap; bot dang chay Da Tau tu chay Da Tau lai.
-- Ham C dang ky o ScriptFuns.cpp: PB_SetTongKim / PB_SetTongKimTran / PB_TongKimGoi
function PB_TkMenu()
	local nBat = PB_SetTongKim(-1)
	local nTran = PB_SetTongKimTran(-1)
	local sBat = "T¾t"
	if nBat == 1 then
		sBat = "BËt"
	end
	local sTran = format("%d bot", nTran)
	if nTran <= 0 then
		sTran = "kh«ng giíi h¹n"
	end
	SayEx({format("<color=yellow>Bot tù tham gia Tèng Kim<color>\nTr¹ng th¸i: <color=green>%s<color>  TrÇn mçi trËn: <color=gold>%s<color>", sBat, sTran),
	"Gäi bot vµo trËn ngay (®ang më tay)/PB_TkGoiNgay",
	"BËt tù ®éng theo giê/#PB_TkBat(1)",
	"T¾t tù ®éng theo giê/#PB_TkBat(0)",
	"TrÇn: kh«ng giíi h¹n/#PB_TkTran(0)",
	"TrÇn: 100 bot/#PB_TkTran(100)",
	"TrÇn: 200 bot/#PB_TkTran(200)",
	"TrÇn: 500 bot/#PB_TkTran(500)",
	"Quay l¹i menu bot/PB_Menu",
	SC_END_SAY})
end

-- Goi quan NGAY. Dung khi chu game MO Tong Kim bang tay de test: bot chi duoc goi
-- o dung khoanh khac tran chuyen dong->mo, nen mo tran truoc roi moi bat tinh nang
-- thi se khong con nao vao. Muc nay ep goi lai, va goi bao nhieu lan cung an toan
-- (bot dang trong tran khong bi dong den).
function PB_TkGoiNgay()
	local nUng = PB_TongKimGoi()
	if nUng and nUng > 0 then
		Msg2Player(format("§· ra lÖnh gäi qu©n: %d bot ®ñ t­ c¸ch (cÊp >= 80, ®· vµo ph¸i, ®ang luyÖn c«ng, kh«ng b¸n s¹p).", nUng))
		Msg2Player("Bot b¾t ®Çu vÒ map b¸o danh trong vµi gi©y. Xem bot.log môc [BotTK].")
	else
		Msg2Player("Kh«ng cã bot nµo ®ñ t­ c¸ch. CÇn: cÊp >= 80, ®· vµo m«n ph¸i, ®ang ®¸nh qu¸i, kh«ng b¸n s¹p, camp kh¸c 4.")
	end
	PB_TkMenu()
end

function PB_TkBat(n)
	local nMoi = PB_SetTongKim(n)
	if nMoi == 1 then
		Msg2Player("§· bËt: tíi giê Tèng Kim bot tù vÒ map b¸o danh vµ gia nhËp nh­ ng­êi ch¬i.")
		Msg2Player("NÕu trËn ®ang më s½n th× qu©n sÏ ®­îc gäi ngay ë nhÞp kÕ tiÕp.")
	else
		Msg2Player("§· t¾t. Bot ®ang trong trËn ®­îc tr¶ l¹i tr¹ng th¸i cò råi vÒ b·i luyÖn.")
	end
	PB_TkMenu()
end

function PB_TkTran(n)
	local nMoi = PB_SetTongKimTran(n)
	if nMoi and nMoi > 0 then
		Msg2Player(format("TrÇn mçi trËn = %d bot.", nMoi))
	else
		Msg2Player("TrÇn mçi trËn = kh«ng giíi h¹n (mäi bot ®ñ t­ c¸ch ®Òu vµo).")
	end
	PB_TkMenu()
end

-- (19/08 toi #4) goi het bot ve 15 map thanh thi/thon, chia deu theo chi so
function PB_VeThanhBat(n)
	local nMoi = PB_SetVeThanh(n)
	if nMoi == 1 then
		Talk(1,"","§· bËt chÕ ®é gäi bot vÒ thµnh thÞ - th«n (chia ®Òu).\nBot ®æi map so le trong 60 gi©y - ®õng lo thÊy chËm.\nBot b¸n s¹p gi÷ nguyªn s¹p; bot D· TÈu t¹m ng­ng, kh«ng mÊt nhiÖm vô.")
	else
		Talk(1,"","§· t¾t chÕ ®é vÒ thµnh - bot tù vÒ b·i luyÖn ®¸nh qu¸i nh­ cò.")
	end
end

function PB_Call(nTu, nDen)
	local nXep, nTran = PB_AddBot(nTu, nDen)
	if nXep and nXep > 0 then
		Msg2Player(format("§· xÕp %d tµi kho¶n vµo hµng ®îi (tèi ®a %d bot).", nXep, nTran))
		Msg2Player("Bot sinh dÇn theo nhÞp, ®îi vµi gi©y råi xem l¹i sè bot.")
	else
		Msg2Player("Kh«ng xÕp ®­îc. Cã thÓ ®· ch¹m trÇn bot, hoÆc hµng ®îi ®Çy.")
	end
	PB_Menu()
end

function PB_Clear()
	local n = PB_ClearBot()
	Msg2Player(format("§· gì %d bot khái thÕ giíi (mäi con ®Òu ®­îc l­u tr­íc khi gì).", n))
	PB_Menu()
end

-- (18/08) Luu NGAY du lieu moi bot (cap/do/vi tri) truoc khi tat server.
-- Binh thuong bot tu luu 10 phut/lan va luu khi bi go; muc nay de chot lan cuoi.
function PB_Save()
	local n = PB_SaveAll()
	if n and n > 0 then
		Msg2Player(format("§· xÕp hµng l­u %d bot (~15 gi©y).", n))
		Msg2Player("Muèn t¾t server: ®îi dßng [BotLuu] xong trong bot.log, ®îi thªm ~10 gi©y cho Goddess ghi nèt råi h·y t¾t.")
	else
		Msg2Player("Kh«ng cã bot nµo ®ang sèng ®Ó l­u.")
	end
	PB_Menu()
end

-- Ra lenh cho bot di vao phai.
-- TACH RIENG khoi luc goi bot LA CO Y: sinh xong bot chi TU DI BO TAN RA roi dung cho,
-- bam muc nay thi chung moi lu luot keo toi NPC mon phai dung ngu hanh cua tung con.
-- Bot nao tung bo cuoc (khong tim duoc duong) thi bam lai lan nua la no di lai.
-- Bat/tat danh quai. Bot tu tim quai gan nhat quanh no roi ra chieu hop vu khi dang cam
-- (hoac hop duong quyen neu khong cam gi). Chi bot DA VAO PHAI moi co ky nang de danh.
function PB_Fight(nOn)
	local n = PB_SetFight(nOn)
	if nOn == 1 then
		Msg2Player(format("§· bËt ®¸nh qu¸i cho %d bot.", n))
		Msg2Player("Bot tù t×m qu¸i quanh nã; xem console dßng [BotDanh] ®Ó biÕt chiªu ®· chän.")
	else
		Msg2Player(format("§· t¾t ®¸nh qu¸i cho %d bot.", n))
	end
	PB_Menu()
end

-- Bat/tat bot noi chuyen. nRate = so lan/1000 moi giay moi bot (40 ~ mot cau / 25 giay / bot).
-- Kho cau la settings/simcity/chat.txt (2745 dong) - DA CO SAN tren may chu.
-- LUU Y duong dan phai dung gach XUOI: Lua 4.0 nuot escape "\".
function PB_Chat(nRate)
	local n = PB_SetChat(nRate, "/settings/simcity/chat.txt", "general")
	if nRate > 0 then
		if n and n > 0 then
			Msg2Player(format("§· bËt bot nãi chuyÖn (kho %d c©u, møc %d/1000 mçi gi©y).", n, nRate))
		else
			Msg2Player("N¹p kho c©u thÊt b¹i - kiÓm settings/simcity/chat.txt.")
		end
	else
		Msg2Player("§· t¾t bot nãi chuyÖn.")
	end
	PB_Menu()
end

function PB_Join()
	local n = PB_JoinFaction()
	if n and n > 0 then
		Msg2Player(format("§· ra lÖnh vµo ph¸i cho %d bot.", n))
		Msg2Player("Bot tù ch¹y bé tíi NPC m«n ph¸i; xem console ®Ó biÕt kÕt qu¶.")
	else
		Msg2Player("Kh«ng cã bot nµo nhËn lÖnh. Gäi bot ra tr­íc ®·.")
	end
	PB_Menu()
end

-- ================= GD3: BOT NOI CHUYEN =================
-- Nap kho cau thoai 1 lan roi bat xac suat noi.
-- chat.txt: cot 1 = Type, cot 2 = Chat. Type nhieu nhat: general (1690), fighting (928).
function PB_Buff(nOn)
	local n = PB_SetBuff(nOn)
	if (nOn == 1) then
		Talk(1,"","§· bËt chÊm Tiªn Th¶o Lé (x2 exp) + QuÕ Hoa Töu (+20 may m¾n) cho "..n.." bot. HÕt h¹n sÏ tù chÊm l¹i.")
	else
		Talk(1,"","§· t¾t chÊm buff cho bot. Buff ®ang cã sÏ hÕt h¹n dÇn.")
	end
end

function PB_DaTauMenu()
	local nDang = PB_SetDaTau(-1)
	Say("Bot tù ch¹y nhiÖm vô D· TÈu (nhËn - lµm - tr¶ - nhËn th­ëng, trÇn 40/ngµy/bot).\n§ang cho phÐp: "..nDang.." bot. Chän sè bot ®­îc lµm:",6,
	"T¾t hÕt/#PB_DaTauSet(0)",
	"20 bot/#PB_DaTauSet(20)",
	"50 bot/#PB_DaTauSet(50)",
	"100 bot/#PB_DaTauSet(100)",
	"200 bot/#PB_DaTauSet(200)",
	"TÊt c¶ 1000/#PB_DaTauSet(1000)")
end

function PB_DaTauSet(n)
	local nMoi = PB_SetDaTau(n)
	Talk(1,"","§· ®Æt giíi h¹n "..nMoi.." bot lµm D· TÈu (lÊy c¸c bot chØ sè thÊp nhÊt). Bot ®ang ®¸nh qu¸i sÏ tù ®i gÆp D· TÈu; ®ñ 40/ngµy tù vÒ b·i luyÖn.")
end

function PB_SapMenu()
	local nDang = PB_SetBanSap(-1)
	Say("Bot ra thµnh ngåi b¸n s¹p thËt (ng­êi ch¬i xem + mua ®­îc; hµng trang søc tr¾ng, gi¸ gèc x2).\n§ang cho phÐp: "..nDang.." bot. Chän sè bot:",6,
	"T¾t hÕt (®ãng s¹p, vÒ b·i)/#PB_SapSet(0)",
	"10 bot/#PB_SapSet(10)",
	"20 bot/#PB_SapSet(20)",
	"50 bot/#PB_SapSet(50)",
	"100 bot/#PB_SapSet(100)",
	"200 bot/#PB_SapSet(200)")
end

function PB_SapSet(n)
	local nMoi = PB_SetBanSap(n)
	Talk(1,"","§· bèc ngÉu nhiªn "..nMoi.." bot ra thµnh ngåi b¸n s¹p. Bot sÏ tù ch¹y vÒ khu trung t©m vµ më s¹p trong vµi gi©y.")
end

function SC_ChatOn()
	if SC_ChatLoaded ~= 1 then
		local n = SC_LoadChat(SC_CHATFILE, "general")
		if n and n > 0 then
			SC_ChatLoaded = 1
			Msg2Player(format("§· n¹p %d c©u tho¹i (general).", n))
		else
			Msg2Player("N¹p c©u tho¹i thÊt b¹i. KiÓm tÖp settings/simcity/chat.txt ®· chÐp ch­a.")
			SC_Menu()
			return
		end
	end
	SC_ChatChance(SC_CHATRATE)
	SC_ChatState = 1
	Msg2Player(format("Bot sÏ nãi chuyÖn (%d/1000 mçi gi©y mçi bot).", SC_CHATRATE))
	SC_Menu()
end

function SC_ChatOff()
	SC_ChatChance(0)
	SC_ChatState = 0
	Msg2Player("§· t¾t bot nãi chuyÖn.")
	SC_Menu()
end

-- ================= SINH BOT =================
-- TEN BOT: BAT BUOC truyen. O Npc[] duoc TAI SU DUNG va truong Name khong bi xoa khi sinh
-- (KNpc::Init khong reset Name, nhanh sentinel cua KNpc::Load khong ghi Name) nen bot khong
-- ten se mang ten chu cu cua o. Client tim nguoi noi bong thoai THEO TEN va lay ket qua trung
-- DAU TIEN -> bong thoai co the nhay sang quai hoac nguoi choi that.
SC_HO  = { "Tran", "Le", "Nguyen", "Pham", "Hoang", "Vo", "Dang", "Bui", "Do", "Ngo",
           "Duong", "Ly", "Ho", "Truong", "Dinh", "Mai" }
SC_TEN = { "Phong", "Vu", "Long", "Ha", "Minh", "Tuan", "Khoa", "Nam", "Lam", "Hai",
           "Anh", "Bao", "Thang", "Quan", "Hung", "Son", "Trung", "Kiet", "Dat", "Vinh" }

-- ten duy nhat: ghep Ho + Ten + so thu tu (Name[32] nen thoai mai)
-- SC_AddBot TU CHOI ten trung (chu game chot cam trung tuyet doi) va tra 0 kem -1.
SC_NameSeq = 0
function SC_MakeName()
	SC_NameSeq = SC_NameSeq + 1
	return format("%s%s%d", SC_HO[random(1, getn(SC_HO))], SC_TEN[random(1, getn(SC_TEN))], SC_NameSeq)
end

-- ================= NHAN THAN BOT (GD4) =================
-- Cap day du thong tin cho bot vua sinh, de nhin nhu nguoi choi that.
-- Ban goc JX2 lam dung viec nay trong SimCityNgoaiTrang:makeup (plugins/pngoaitrang.lua)
-- bang ChangeNpcFeature; o JX1 ta ghi thang cac truong tuong ung qua SC_DressBot.
SC_ResReported = 0
function SC_GiveIdentity(nIdx)
	if not nIdx or nIdx <= 0 then
		return
	end
	-- ngoai trang: lay ngau nhien tu kho res THAT cua ban JX1 nay (SC_DressBot tu do bang
	-- g_ItemChangeRes nen khong bao gio ra mon do khong ton tai). 1/6 bot cuoi ngua.
	local nRide = 0
	if random(1, 6) == 1 then
		nRide = 1
	end
	local nOk, nHelm, nArmor, nWeapon, nHorse = SC_DressBot(nIdx, nRide)

	-- bao MOT LAN cho GM biet du lieu res co nap duoc khong
	if SC_ResReported == 0 and nHelm then
		SC_ResReported = 1
		if nHelm == 0 and nArmor == 0 then
			Msg2Player("C¶nh b¸o: kho ngo¹i trang rçng - kiÓm tÖp settings res cña client/server.")
		else
			Msg2Player(format("Kho ngo¹i trang: nãn %d, ¸o %d, vò khÝ %d, ngùa %d.", nHelm, nArmor, nWeapon, nHorse))
		end
	end

	-- mon phai 0..9 (5 he x 2 phai). SC_SetBotFaction tu dat camp theo phai,
	-- neu khong bot se mang camp_free = "mÉu ®å s¸t thñ", sai han y do dan thanh thi.
	SC_SetBotFaction(nIdx, random(0, 9))

	-- chi so trong bang Tin tuc: PK 0 (hien hoa binh), danh vong / phuc duyen / trung sinh
	-- o muc hop ly cua nguoi choi lau nam.
	SC_SetBotInfo(nIdx, 0, random(0, 8000), random(0, 300), random(0, 2))
end

-- sinh 1 bot ngay tai vi tri admin dang dung (khoi phai doan toa do)
function SC_SpawnHere()
	local nW, nX, nY = GetWorldPos()          -- nX,nY = O LUOI
	local nSwIdx = SubWorldID2Idx(nW)
	local nSex   = random(0, 1)
	local nIdx = 0
	-- thu lai vai lan phong khi ten vua boc bi trung (SC_AddBot tu choi ten trung)
	for i = 1, 5 do
		nIdx = SC_AddBot(nSex, 100, nSwIdx, (nX + 1) * 32, (nY + 1) * 32, random(0, 4), SC_MakeName())
		if nIdx and nIdx > 0 then
			break
		end
	end
	if nIdx and nIdx > 0 then
		SC_LastBot = nIdx
		SC_GiveIdentity(nIdx)
		Msg2Player(format("§· sinh bot idx = %d t¹i map %d (%d,%d).", nIdx, nW, nX, nY))
	else
		Msg2Player("Sinh bot thÊt b¹i. KiÓm to¹ ®é / subworld (hoÆc tªn bÞ trïng liªn tiÕp).")
	end
	SC_Menu()
end

function SC_Spawn5()
	local nW, nX, nY = GetWorldPos()
	local nSwIdx = SubWorldID2Idx(nW)
	local nOk = 0
	for i = 1, 5 do
		local nIdx = SC_AddBot(random(0, 1), 100, nSwIdx, (nX + i + 1) * 32, (nY + 1) * 32, random(0, 4), SC_MakeName())
		if nIdx and nIdx > 0 then
			nOk = nOk + 1
			SC_LastBot = nIdx
			SC_GiveIdentity(nIdx)
		end
	end
	Msg2Player(format("§· sinh %d/5 bot quanh chç b¹n ®øng.", nOk))
	SC_Menu()
end

-- ================= DI CHUYEN =================
function SC_PatrolLast()
	if SC_LastBot <= 0 then
		Msg2Player("Ch­a cã bot nµo. H·y sinh bot tr­íc.")
		SC_Menu()
		return
	end
	-- SC_PatrolBox nay kiem TestBarrier tung goc va tu thu nho ban kinh; tra 0 khi cho qua chat
	-- (duoi 2 goc di duoc). Truoc day 4 goc khong he duoc kiem nen goc nam trong nha -> bot ket.
	if SC_PatrolBox(SC_LastBot) == 1 then
		Msg2Player(format("Bot %d ®· nhËn lé tr×nh tuÇn tra (vu«ng). Nhí bËt driver.", SC_LastBot))
	else
		Msg2Player("Chç nµy qu¸ chËt ®Ó tuÇn tra (kh«ng ®ñ 2 gãc ®i ®­îc). H·y ra chç réng h¬n.")
	end
	SC_Menu()
end

function SC_On()
	SC_MoveOn()
	SC_MoveState = 1
	Msg2Player("§· bËt driver di chuyÓn. (Bot chØ b­íc khi cã ng­êi ch¬i ®øng gÇn.)")
	SC_Menu()
end

function SC_Off()
	SC_MoveOff()
	SC_MoveState = 0
	Msg2Player("§· t¾t driver di chuyÓn.")
	SC_Menu()
end

function SC_Clear()
	local n = SC_ClearBots()
	SC_LastBot   = 0
	SC_LastRoute = -1
	Msg2Player(format("§· xo¸ %d bot.", n))
	SC_Menu()
end

-- ================= LO TRINH BAN DO THAT =================
-- Bang: {WorldID, ten file preset (khong duong dan), ten tuyen}
-- Ten tuyen o cot 1 cua tep preset (da doi chieu du lieu that, phan biet hoa/thuong).
SC_ROUTES = {
	{  1, "1_phuongtuong_preset.txt",  "phuongtuong1"          },
	{  1, "1_phuongtuong_preset.txt",  "phuongtuong2"          },
	{ 78, "78_tuongduong_preset.txt",  "tuongduong_daochoi"    },
	{ 78, "78_tuongduong_preset.txt",  "tuongduong_trongthanh" },
	{ 78, "78_tuongduong_preset.txt",  "tuongduong_full"       },
	{ 37, "37_bienkinh_preset.txt",    "bienkinh1"             },
	{ 37, "37_bienkinh_preset.txt",    "bienkinh2"             },
	{176, "176_laman_preset.txt",      "lamAn_cungChinh"       },
	{176, "176_laman_preset.txt",      "lamAn_cungPhu1"        },
}

-- CHI liet ke tuyen CUNG MAP: vua dung nhu cau, vua khong vuot buffer 512 byte
-- (tieu de + moi option cong them 1 byte "|" - xem sUiAppendAnswer ScriptFuns.cpp:538).
function SC_RouteMenu()
	local nW = GetWorldPos()
	local tb = {}
	local nFound = 0
	for i = 1, getn(SC_ROUTES) do
		if SC_ROUTES[i][1] == nW then
			tinsert(tb, format("%s/#SC_UseRoute(%d)", SC_ROUTES[i][3], i))
			nFound = nFound + 1
		end
	end
	if nFound == 0 then
		tinsert(tb, 1, format("<color=yellow>Chän tuyÕn ®­êng<color>\nMap <color=gold>%d<color> ch­a khai b¸o tuyÕn nµo.\nThªm vµo b¶ng SC_ROUTES trong simcity_admin.lua.", nW))
	else
		tinsert(tb, 1, format("<color=yellow>Chän tuyÕn ®­êng<color>\nMap <color=gold>%d<color> cã <color=green>%d<color> tuyÕn.\nBot sÏ ®i vßng theo tuyÕn chän.", nW, nFound))
	end
	tinsert(tb, "Quay l¹i/SC_Menu")
	tinsert(tb, SC_END_SAY)
	SayEx(tb)
end

function SC_UseRoute(nSel)
	local r = SC_ROUTES[nSel]
	if not r then
		Msg2Player("TuyÕn kh«ng hîp lÖ.")
		SC_Menu()
		return
	end
	if SC_LastBot <= 0 then
		Msg2Player("Ch­a cã bot. H·y sinh bot tr­íc (menu chÝnh).")
		SC_Menu()
		return
	end
	local nW = GetWorldPos()
	if r[1] ~= nW then
		Msg2Player(format("TuyÕn nµy thuéc map %d, b¹n ®ang ë map %d. Bot sÏ kh«ng ®i ®­îc.", r[1], nW))
		SC_Menu()
		return
	end
	local nRid = SC_LoadPreset(SC_PRESETDIR..r[2], r[3])
	if not nRid or nRid < 0 then
		Msg2Player(format("N¹p tuyÕn thÊt b¹i: %s / %s. KiÓm d÷ liÖu ®· chÐp sang settings/simcity ch­a.", r[2], r[3]))
		SC_Menu()
		return
	end
	SC_LastRoute = nRid
	-- SC_SetBotRoute tra 2 gia tri khi TU CHOI: 0 va khoang cach (o luoi) toi node gan nhat.
	-- Bot luon bat dau tu node GAN NHAT chu khong phai node dau tep, nen ban phai dung trong
	-- pham vi 32 o cua tuyen. Truoc day bot bi ban toi node dau tep (do duoc toi 1009 o) nen
	-- no di duong thang xuyen thanh va ket goc ngay.
	local nOk, nCell = SC_SetBotRoute(SC_LastBot, nRid, 1)
	if nOk == 1 then
		Msg2Player(format("Bot %d ®· nhËn tuyÕn %s (routeId %d). Nhí bËt driver.", SC_LastBot, r[3], nRid))
	else
		if nCell then
			Msg2Player(format("Bot c¸ch tuyÕn %s tíi %d « (tèi ®a 32). H·y ®øng gÇn tuyÕn råi sinh bot l¹i.", r[3], nCell))
		else
			Msg2Player("G¸n tuyÕn cho bot thÊt b¹i.")
		end
	end
	SC_Menu()
end
