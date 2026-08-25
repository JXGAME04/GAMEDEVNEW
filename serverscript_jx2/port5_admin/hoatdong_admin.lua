-- ============================================================================
-- HOATDONG_ADMIN.LUA - [24/08] Menu test 4 hoat dong PORT5 tren Lenh Bai Admin
--   (Bang Chien tongwar / Bach Nhan bairenleitai / Ty Vo bw / Thanh Bao tongcastle)
-- Duoc Include tu lenhbaiadmin.lua; lenhbaiadmin main() dofile lai chinh no moi
-- lan dung => SUA FILE NAY KHONG CAN RESTART GameServer.
-- Cau hinh so lieu: script\header\cauhinh_hoatdong.lua (1 file chinh tat ca).
-- ============================================================================
Include("\\script\\header\\cauhinh_hoatdong.lua")
Include("\\script\\task\\tollgate\\messenger\\messenger_losetask.lua")	-- [25/08] losemessengertask + nt_* cho muc 10

HD_TW_RELAY = "\\script\\missions\\tongwar\\relay\\tongwar.lua"
HD_TC_DRV   = "\\script\\missions\\tongcastle\\tongcastle_driver.lua"
HD_TC_RELAY = "\\script\\mission\\tongcastle\\tongcastle.lua"

function HD_AdminMenu()
	SayEx({"<color=yellow>Ho¹t ®éng 23-24/08<color>: chän môc test (cÊu h×nh: script\\header\\cauhinh_hoatdong.lua)",
	"1. Bang ChiÕn - Vâ L©m ®Ö NhÊt Bang/HD_TW_Menu",
	"2. B¸ch Nh©n L«i ®µi/HD_BR_Menu",
	"3. Tû Vâ ®µi/HD_BW_Menu",
	"4. Bang Héi Thµnh B¶o/HD_TC_Menu",
	"5. NhËn item th­ëng (test)/HD_Item_Menu",
	"6. BOT Tèng Kim: bËt/HD_TK_Bat",
	"7. BOT Tèng Kim: t¾t/HD_TK_Tat",
	"8. N¹p l¹i CONFIG (khi kh«ng cã trËn ch¹y)/HD_ReloadCfg",
	"9. Danh hiÖu & vßng s¸ng (Bang ChiÕn)/HD_TT_Menu",
	"10. Test TÝn Sø (chÈn ®o¸n + gì kÑt)/HD_TS_Menu",
	"KÕt thóc ®èi tho¹i/no"})
end

-- ================= 1) BANG CHIEN =================
function HD_TW_Menu()
	SayEx({"<color=yellow>Bang ChiÕn<color> - mïa 9: 29/10-05/11/2026, trËn "..HD_GioPhut(HD_CFG("TW_GIO_KHAICHIEN", 2000)).."-"..HD_GioPhut(HD_CFG("TW_GIO_KETTHUC", 2130)).." (cÇn >=2 bang chiÕm thµnh ®Ó khëi t¹o)",
	"Xem tr¹ng th¸i (cê trËn? file mïa?)/HD_TW_TrangThai",
	"B1. Khëi t¹o mïa 9 NGAY (tongwar_initmatch)/HD_TW_InitMatch",
	"B2. Ðp tick lÞch NGAY (TaskContent)/HD_TW_Tick",
	"B3. Ðp më pha 2 - b¸o danh 3 cÆp test/HD_TW_Redo",
	"DÞch chuyÓn: NPC b¸o danh Ba L¨ng HuyÖn/HD_TW_Tele53",
	"DÞch chuyÓn: map b¸o danh 608/HD_TW_Tele608",
	"DÞch chuyÓn: map b¸o danh 610/HD_TW_Tele610",
	"DÞch chuyÓn: map b¸o danh 612/HD_TW_Tele612",
	"Quay l¹i/HD_AdminMenu"})
end

function HD_TW_TrangThai()
	local nTran = GetGlbValue(850)
	Msg2Player("Cê ®ang më trËn (GlbValue 850) = "..tostring(nTran).." (1 = ®ang cã trËn)")
	local f = openfile("jx2league.txt", "r")
	if f then
		local n = 0
		while 1 do
			local dong = read(f, "*l")
			if not dong then break end
			n = n + 1
			if n <= 8 then Msg2Player("jx2league: "..dong) end
		end
		closefile(f)
		Msg2Player("jx2league.txt tong "..n.." dßng (7 dßng G = ®· khëi t¹o mïa)")
	else
		Msg2Player("CH­A cã jx2league.txt (mïa ch­a khëi t¹o - bÊm B1)")
	end
	HD_TW_Menu()
end

function HD_TW_InitMatch()
	DynamicExecute(HD_TW_RELAY, "tongwar_initmatch", 9)
	Msg2Player("®· gäi tongwar_initmatch(9) - xem jx2league.txt (cÇn >=2 bang ®ang chiÕm thµnh)")
end

function HD_TW_Tick()
	DynamicExecute(HD_TW_RELAY, "TaskContent")
	Msg2Player("®· Ðp TaskContent (tick lÞch Bang ChiÕn)")
end

function HD_TW_Redo()
	local t = HD_CFG("TW_TEST_REDO", {1, 2, 1, 3, 4, 2, 5, 6, 3})
	DynamicExecute("\\script\\gmscript.lua", "HD_TW_RedoStart", t[1], t[2], t[3], t[4], t[5], t[6], t[7], t[8], t[9])
	Msg2Player("®· Ðp pha 2 (b¸o danh) cho 3 cÆp test - id bang söa ë TW_TEST_REDO trong config")
end

function HD_TW_Tele53()  NewWorld(53, 1628, 3173) end
function HD_TW_Tele608() NewWorld(608, 1582, 3174) end
function HD_TW_Tele610() NewWorld(610, 1588, 3160) end
function HD_TW_Tele612() NewWorld(612, 1604, 3147) end

-- ================= 2) BACH NHAN =================
function HD_BR_Menu()
	SayEx({"<color=yellow>B¸ch Nh©n L«i ®µi<color> - më "..HD_GioPhut(HD_CFG("BR_GIO_MO", 1200)).."-24:00 h»ng ngµy, map 960, cÊp >="..HD_CFG("BR_CAP_TOITHIEU", 90),
	"Xem cÊu h×nh ®ang ch¹y/HD_BR_TrangThai",
	"DÞch chuyÓn: NPC lèi vµo (L©m An)/HD_BR_TeleNpc",
	"DÞch chuyÓn: vµo map 960 (®µi 1)/HD_BR_Tele960",
	"Gäi l¹i Init (CHØ khi boot lçi)/HD_BR_Init",
	"Quay l¹i/HD_AdminMenu"})
end

function HD_BR_TrangThai()
	Msg2Player(format("Giê më %s, exp %d/%d phót (L«i Chñ +%d), trÇn %d l­ît/ngµy, buff x2: %d%% ng­êi, mçi %d phót",
		HD_GioPhut(HD_CFG("BR_GIO_MO", 1200)), HD_CFG("BR_EXP_TICK", 1000000), HD_CFG("BR_PHUT_CHUKY_EXP", 5),
		HD_CFG("BR_EXP_LOICHU", 2000000), HD_CFG("BR_TRAN_LUOT_NGAY", 50),
		floor(HD_CFG("BR_TILE_BUFFX2", 0.2)*100), HD_CFG("BR_PHUT_COTHU", 30)))
	-- [24/08 hau phan bien] GetTaskDailyCount la PHUONG THUC cua bang PlayerFunLib, goi
	-- nhu bien toan cuc se luon ra -1. Giai ma tai cho (yymmdd*256 + so_lan); KHONG
	-- Include playerfunlib.lua vi se tao lai vong Include gay tran stack dem 23/08.
	local nRaw2709 = GetTask(2709)
	local nNgayNay = tonumber(GetLocalDate("%y%m%d"))
	local nDaDung = 0
	if (floor(nRaw2709 / 256) == nNgayNay) then
		nDaDung = mod(nRaw2709, 256)
	end
	Msg2Player(format("®· dïng h«m nay (task 2709): %d l­ît", nDaDung))
	HD_BR_Menu()
end

function HD_BR_TeleNpc() NewWorld(HD_CFG("BR_NPC_MAP", 176), 1464, 3223) end
function HD_BR_Tele960() NewWorld(960, 1784, 3099) end
function HD_BR_Init()
	DynamicExecute("\\script\\missions\\bairenleitai\\bairen_boot.lua", "BairenLeitai_Adm_Init")
	Msg2Player("®· gäi BairenLeitai_Adm_Init (CHó ý: gäi lÆp sÏ t¹o thªm timer)")
end

-- ================= 3) TY VO =================
function HD_BW_Menu()
	SayEx({"<color=yellow>Tû Vâ ®µi<color> - kh«ng cã lÞch, 2 ®éi tr­ëng (mçi ®éi ®ñ 2 ng­êi) b¸o danh ë C«ng B×nh Tö; trËn "..HD_CFG("BW_PHUT_TRAN", 12).." phót (chê "..HD_CFG("BW_PHUT_CHO", 2).." phót)",
	"DÞch chuyÓn: C«ng B×nh Tö D­¬NG CH©U/HD_BW_Tele80",
	"DÞch chuyÓn: C«ng B×nh Tö T­¬NG D­¬NG/HD_BW_Tele78",
	"DÞch chuyÓn: C«ng B×nh Tö THµNH ®«/HD_BW_Tele11",
	"Vµo ®Êu tr­êng 209 (chç kh¸n gi¶)/HD_BW_Tele209",
	"Gì kÑt trËn treo cña T«I (task 200 va 2340-2342)/HD_BW_GoKet",
	"Quay l¹i/HD_AdminMenu"})
end

function HD_BW_Tele80()  NewWorld(80, 1659, 3020) end
function HD_BW_Tele78()  NewWorld(78, 1464, 3183) end
function HD_BW_Tele11()  NewWorld(11, 3071, 5002) end
function HD_BW_Tele209() NewWorld(209, 1598, 3216) end

function HD_BW_GoKet()
	SetTaskTemp(200, 0)
	SetTask(2340, 0)
	SetTask(2341, 0)
	SetTask(2342, 0)
	Msg2Player("®· xãa cê trong-trËn (TaskTemp 200) + vÞ trÝ l­u (task 2340-2342) cña nh©n vËt nµy")
end

-- ================= 4) THANH BAO =================
function HD_TC_Menu()
	SayEx({"<color=yellow>Bang Héi Thµnh B¶o<color> - thø "..HD_CFG("TC_THU", 0).." (0=CN), map 984 (CLIENT CH­A Cã DATA - vµo sÏ ®en mµn)",
	"Håi sinh c©y 1 (Thanh ®ång) NGAY/HD_TC_Cay1",
	"Håi sinh c©y 2 (B¹ch Ng©n) NGAY/HD_TC_Cay2",
	"Håi sinh c©y 3 (Hoµng Kim) NGAY/HD_TC_Cay3",
	"Xãa hÕt c©y + record (dän test)/HD_TC_XoaCay",
	"Xem ®iÓm ThÇn Méc cña t«i/HD_TC_XemDiem",
	"Céng 500 ®iÓm ThÇn Méc (test ®æi lÖnh bµi)/HD_TC_CongDiem",
	"DÞch chuyÓn: NPC Thµnh B¶o (L©m An)/HD_TC_TeleNpc",
	"Vµo map 984 (C¶NH B¸O: client ®en mµn)/HD_TC_Tele984",
	"Quay l¹i/HD_AdminMenu"})
end

function HD_TC_Cay1() DynamicExecute(HD_TC_DRV, "TONGCASTLE_Adm_ReviveTree", 1) Msg2Player("®· Ðp håi sinh c©y lo¹i 1 ë map 984") end
function HD_TC_Cay2() DynamicExecute(HD_TC_DRV, "TONGCASTLE_Adm_ReviveTree", 2) Msg2Player("®· Ðp håi sinh c©y lo¹i 2 ë map 984") end
function HD_TC_Cay3() DynamicExecute(HD_TC_DRV, "TONGCASTLE_Adm_ReviveTree", 3) Msg2Player("®· Ðp håi sinh c©y lo¹i 3 ë map 984") end

function HD_TC_XoaCay()
	DynamicExecute(HD_TC_RELAY, "tbS3TongCastle:DelAllTreeData", 1)
	DynamicExecute(HD_TC_RELAY, "tbS3TongCastle:DelAllTreeData", 2)
	DynamicExecute(HD_TC_RELAY, "tbS3TongCastle:DelAllTreeData", 3)
	Msg2Player("®· xãa record c©y c¶ 3 lo¹i ë relay (c©y ®ang ®øng trong map sÏ bÞ dän ë tick sau)")
end

function HD_TC_XemDiem()
	Msg2Player("®iÓm ThÇn Méc (task 3399) = "..GetTask(3399))
	HD_TC_Menu()
end

function HD_TC_CongDiem()
	SetTask(3399, GetTask(3399) + 500)
	Msg2Player("®· céng 500 ®iÓm ThÇn Méc -> hiÖn cã "..GetTask(3399))
end

function HD_TC_TeleNpc() NewWorld(176, 1663, 3262) end
function HD_TC_Tele984() NewWorld(984, 1311, 3515) end

-- ================= 5) ITEM THUONG TEST =================
-- [FIX 24/08] Engine doi AddItem TOI THIEU 7 THAM SO (ScriptFuns.cpp:4764: neu
-- nParamNum < 7 thi return luon, KHONG tao item). Truoc day o day goi 6 tham so nen
-- bam "nhan item" khong duoc gi. Chuan cua du an: AddItem(genre, detail, particular,
-- nLevel, nSeries, nLuck, 0) voi nLevel = 1 (xem lib\awardtype\item_jx1.lua:30).
function HD_Item_Menu()
	SayEx({"NhËn item th­ëng ®Ó test (genre 6/detail 1)",
	"Qu¶ ®¹i Hoµng Kim 4864 ("..floor(HD_CFG("QDHK_EXP", 200000000)/1e6).."tr exp) x1/HD_It4864",
	"Hoµng Ch©n ®¬n 2273 ("..floor(HD_CFG("HCD_EXP", 2000000000)/1e6).."tr exp cã trÇn) x1/HD_It2273",
	"Bïa triÖu Thñ VÖ 3204 x5/HD_It3204",
	"Thanh ®ång ThÇn Méc LÖnh 3205 x1/HD_It3205",
	"B¹ch Ng©n ThÇn Méc LÖnh 3206 x1/HD_It3206",
	"Hoµng Kim ThÇn Méc LÖnh 3207 x1/HD_It3207",
	"Hµn nguyªn ch©n ®an 4857 x1/HD_It4857",
	"Quay l¹i/HD_AdminMenu"})
end

function HD_It4864()
	local nIdx = AddItem(6, 1, 4864, 1, 0, 0, 0)
	if (nIdx and nIdx > 0) then
		Msg2Player("®· nhËn Qu¶ ®¹i Hoµng Kim")
	else
		Msg2Player("Khong nhan duoc item - hanh trang day hoac id sai.")
	end
end
function HD_It2273()
	local nIdx = AddItem(6, 1, 2273, 1, 0, 0, 0)
	if (nIdx and nIdx > 0) then
		Msg2Player("®· nhËn Hoµng Ch©n ®¬n")
	else
		Msg2Player("Khong nhan duoc item - hanh trang day hoac id sai.")
	end
end
function HD_It3204()
	for i = 1, 5 do AddItem(6, 1, 3204, 1, 0, 0, 0) end
	Msg2Player("®· nhËn 5 Bïa triÖu Thñ VÖ")
end
function HD_It3205()
	local nIdx = AddItem(6, 1, 3205, 1, 0, 0, 0)
	if (nIdx and nIdx > 0) then
		Msg2Player("®· nhËn Thanh ®ång ThÇn Méc LÖnh")
	else
		Msg2Player("Khong nhan duoc item - hanh trang day hoac id sai.")
	end
end
function HD_It3206()
	local nIdx = AddItem(6, 1, 3206, 1, 0, 0, 0)
	if (nIdx and nIdx > 0) then
		Msg2Player("®· nhËn B¹ch Ng©n ThÇn Méc LÖnh")
	else
		Msg2Player("Khong nhan duoc item - hanh trang day hoac id sai.")
	end
end
function HD_It3207()
	local nIdx = AddItem(6, 1, 3207, 1, 0, 0, 0)
	if (nIdx and nIdx > 0) then
		Msg2Player("®· nhËn Hoµng Kim ThÇn Méc LÖnh")
	else
		Msg2Player("Khong nhan duoc item - hanh trang day hoac id sai.")
	end
end
function HD_It4857()
	local nIdx = AddItem(6, 1, 4857, 1, 0, 0, 0)
	if (nIdx and nIdx > 0) then
		Msg2Player("®· nhËn Hµn nguyªn ch©n ®an")
	else
		Msg2Player("Khong nhan duoc item - hanh trang day hoac id sai.")
	end
end

-- ================= 6/7) BOT TONG KIM =================
function HD_TK_Bat() PB_SetTongKim(1) Msg2Player("®· BËT bot tù tham gia Tèng Kim (PB_SetTongKim 1)") end
function HD_TK_Tat() PB_SetTongKim(0) Msg2Player("®· T¾T bot tù tham gia Tèng Kim (PB_SetTongKim 0)") end

-- ================= 8) NAP LAI CONFIG =================
-- ReLoadScript = chay lai ca file trong state cua no => moi HD_CFG/%upvalue
-- deu nhan gia tri moi tu cauhinh_hoatdong.lua vua sua tren dia.
-- CHU Y: KHONG bam khi dang co tran chay (bang trang thai trong file se bi reset).
HD_RELOAD_LIST = {
	"\\script\\missions\\tongwar\\relay\\tongwar.lua",
	"\\script\\event\\tongwar\\tongwar_signup.lua",
	"\\script\\missions\\tongwar\\trap\\tongwar_trap.lua",
	"\\script\\missions\\bairenleitai\\hundred_arena.lua",
	"\\script\\missions\\bairenleitai\\bairen_boot.lua",
	"\\script\\missions\\bw\\bwmanager.lua",
	"\\script\\missions\\tongcastle\\tongcastle.lua",
	"\\script\\mission\\tongcastle\\tongcastle.lua",
	"\\script\\missions\\tongcastle\\guideperson.lua",
	"\\script\\missions\\tongcastle\\treedeath.lua",
	"\\script\\missions\\tongcastle\\guard.lua",
	"\\script\\missions\\tongcastle\\shenmuling.lua",
	"\\script\\item\\huangzhendan.lua",
	"\\script\\vng_event\\item\\biggoldenseed.lua",
	-- [CFG TS 25/08] file doc TS_*; posthouse con BAN SAO trong state wagoner/turenpc/
	-- turerukou nen nap lai o day chi cap nhat state goc - muon an moi cua van can
	-- restart GS (vi vay config danh dau [RESTART])
	"\\script\\task\\tollgate\\messenger\\posthouse.lua",
	"\\script\\task\\tollgate\\messenger\\qianbaoku\\90\\tureboss90.lua",
	"\\script\\task\\tollgate\\messenger\\qianbaoku\\90\\turebug90.lua",
}

function HD_ReloadCfg()
	Include("\\script\\header\\cauhinh_hoatdong.lua")
	local nOk = 0
	for i = 1, getn(HD_RELOAD_LIST) do
		ReLoadScript(HD_RELOAD_LIST[i])
		nOk = nOk + 1
	end
	Msg2Player("®· n¹p l¹i CONFIG + "..nOk.." file tÝnh n¨ng. C¸c môc [RESTART] trong config vÉn cÇn restart GS.")
end

-- ================= 9) DANH HIEU & VONG SANG =================
-- CHI Bang Chien co danh hieu/vong sang. Da doi chieu ban Linux: Bach Nhan, Thanh Bao,
-- Ty Vo KHONG he co Title_AddTitle/AddSkillState nao => khong phai thieu khi port.
--   quan ham 1..5 = title 100..104  (missions\tongwar\match\head.lua:430)
--   skill 661 cap (rank-1) = hieu ung MAY MAN tren DAU (StateSpecialId 67 ->
--     \spr\skill\others\lucky.spr, cot 3 = "Head"), KHONG phai vong sang.
--     Ban Linux van goi (match\head.lua:432) nen GIU de dung 100% Linux.
--   VONG SANG THAT: bang settings\PlayerTitle.txt cot ExtSkill1, engine tu cast
--     trong KNpc::SetCurPlayerTitle (KNpc.cpp:11016-11023). Dong 288-292 (id 287-291)
--     = skill 830..834 cap 2/4/6/8/10 (Status136..140, cot "Foot" = duoi chan);
--     dong 293-296 (id 292-295) = skill 1169 cap 1 (Status81, giong id 286 THDNB).
--     => chi can SetPlayerTitle la vong sang TU HIEN, khong phai cast tay.
--   105/106 Vo Lam De Nhat Bang     (event\tongwar\head.lua:273, han 30 ngay)
--   199 Cao Cap De Nhat Bang        (npc_shizhe.lua:355, han 90 ngay)
--   3000 Vo Lam Minh Chu            (head.lua:266)
-- Chu ky: Title_AddTitle(nId, nTimeType, nTime) - nTimeType 1 = co han, nTime tinh FRAME
-- (18 frame = 1 giay); Title_ActiveTitle(nId) = DEO len nguoi.
TT_QUANHAM = {"Binh SÜ", "HiÖu óy", "Thèng LÜnh", "Phã T­íng", "®¹i T­íng"}

function HD_TT_Menu()
	local nCur = Title_GetActiveTitle()
	SayEx({"<color=yellow>Danh hiÖu & vßng s¸ng<color> - danh hiÖu ®ang ®eo: ".. tostring(nCur),
	"Qu©n hµm 1 - Binh SÜ (kÌm vßng s¸ng)/HD_TT_R1",
	"Qu©n hµm 2 - HiÖu óy (kÌm vßng s¸ng)/HD_TT_R2",
	"Qu©n hµm 3 - Thèng LÜnh (kÌm vßng s¸ng)/HD_TT_R3",
	"Qu©n hµm 4 - Phã T­íng (kÌm vßng s¸ng)/HD_TT_R4",
	"Qu©n hµm 5 - ®¹i T­íng (kÌm vßng s¸ng)/HD_TT_R5",
	"Danh hiÖu 105 - Vâ L©m ®Ö NhÊt Bang/HD_TT_105",
	"Danh hiÖu 106 - Vâ L©m ®Ö NhÊt Bang (bang viªn)/HD_TT_106",
	"Danh hiÖu 199 - Cao CÊp ®Ö NhÊt Bang/HD_TT_199",
	"Danh hiÖu 3000 - Vâ L©m Minh Chñ/HD_TT_3000",
	"Gì hÕt danh hiÖu vµ vßng s¸ng/HD_TT_Clear",
	"Quay l¹i/HD_AdminMenu"})
end

-- [FIX 25/08] Title_ActiveTitle (he JX2) KHONG dat Npc[].m_btPlayerTitle - bien DUY NHAT
-- client dung de VE danh hieu (KNpc.cpp:5647 PlayerSync.PlayerTitle). Phai goi THEM
-- SetPlayerTitle cua he JX1 (ScriptFuns.cpp:10702 -> KNpc::SetCurPlayerTitle:11006).
-- Id HAI BANG mang nghia KHAC NHAU (105: JX2 = De Nhat Bang, JX1 = Hieu uy) nen phai
-- tra bang duoi day; 9 dong 287-295 la dong MOI them vao settings\PlayerTitle.txt.
TT_JX1ID = {[100]=287, [101]=288, [102]=289, [103]=290, [104]=291,
			[105]=292, [106]=293, [199]=294, [3000]=295}

function HD_TT_Hien(nIdJx2, nTimeFrame)
	local nJx1 = TT_JX1ID[nIdJx2]
	if nJx1 then
		SetPlayerTitle(nJx1, nTimeFrame or 9999999, 0)
	end
end

function HD_TT_Rank(nRank)
	local nId = 100 + nRank - 1
	Title_AddTitle(nId, 0, 9999999)
	Title_ActiveTitle(nId)
	HD_TT_Hien(nId)	-- [FIX 25/08] de danh hieu HIEN tren dau nhan vat
	AddSkillState(661, nRank - 1, 0, 999999)
	Msg2Player("®· nhËn qu©n hµm ".. TT_QUANHAM[nRank] ..
		" (danh hiÖu ".. nId .. ", vßng s¸ng kü n¨ng ".. (829 + nRank) .." cÊp ".. (nRank * 2) .. ")")
end
function HD_TT_R1() HD_TT_Rank(1) end
function HD_TT_R2() HD_TT_Rank(2) end
function HD_TT_R3() HD_TT_Rank(3) end
function HD_TT_R4() HD_TT_Rank(4) end
function HD_TT_R5() HD_TT_Rank(5) end

-- han dung y het ban goc: 105/106 = 30 ngay, 199 = 90 ngay (tinh bang FRAME)
function HD_TT_Cap(nId, nNgay, szTen)
	Title_AddTitle(nId, 1, nNgay * 24 * 60 * 60 * 18)
	Title_ActiveTitle(nId)
	HD_TT_Hien(nId, nNgay * 24 * 60 * 60 * 18)	-- [FIX 25/08]
	Msg2Player("®· nhËn danh hiÖu ".. szTen .. " (id ".. nId ..
		", h¹n ".. nNgay .. " ngµy)")
end
function HD_TT_105()  HD_TT_Cap(105,  30, "Vâ L©m ®Ö NhÊt Bang") end
function HD_TT_106()  HD_TT_Cap(106,  30, "Vâ L©m ®Ö NhÊt Bang") end
function HD_TT_199()  HD_TT_Cap(199,  90, "Cao CÊp ®Ö NhÊt Bang") end
function HD_TT_3000() HD_TT_Cap(3000, 30, "Vâ L©m Minh Chñ") end

function HD_TT_Clear()
	Title_ActiveTitle(0)
	local tb = {100, 101, 102, 103, 104, 105, 106, 199, 3000}
	for i = 1, getn(tb) do
		Title_RemoveTitle(tb[i])
	end
	RemovePlayerTitle()	-- [FIX 25/08] go danh hieu he JX1 (m_btPlayerTitle)
	RemoveSkillState(661)
	RemoveSkillState(1485)
	-- [FIX 25/08] go ca VONG SANG that (ExtSkill1 cua 9 dong PlayerTitle.txt moi).
	-- RemovePlayerTitle o tren da goi CastStateSkill overlook=1 nen thuong da go,
	-- go lai cho chac vi menu co the cap nhieu danh hieu lien tiep.
	local tbAura = {830, 831, 832, 833, 834, 1169}
	for i = 1, getn(tbAura) do
		RemoveSkillState(tbAura[i])
	end
	Msg2Player("®· gì hÕt danh hiÖu Bang ChiÕn vµ vßng s¸ng qu©n hµm.")
end


-- ================= 10) TEST TIN SU =================
-- [25/08 v2] Tieng Viet co dau (TCVN3, bang ma tu kiem 4/4); BO '|' khoi tieu de -
-- '|' la ky tu PHAN CACH option cua goi thoai (sUiAppendAnswer ScriptFuns.cpp:604,
-- client tach KPlayer.cpp:7683/7717); co '|' trong noi dung la AnswerCount=0, mat het nut.
-- Y nghia 1203: 0=chua nhan, 10=da nhan chua bat dau, 20=dang lam, 21=tam ngat, 25/30=xong.
function HD_TS_Menu()
	SayEx({"<color=yellow>Test TÝn Sø <color>- 1203="..nt_getTask(1203)..", rót vò khÝ="..GetFightState().." (0 = kh«ng ®¸nh ®­îc qu¸i, kh«ng thÓ chÕt)",
	"1. Xem tr¹ng th¸i chi tiÕt".."/HD_TS_Xem",
	"2. Gì kÑt: 20 sang 21, råi tíi DÞch Quan bÊm TiÕp tôc".."/HD_TS_GoKet",
	"3. BËt tr¹ng th¸i chiÕn ®Êu NGAY (test nhanh trong ¶i)".."/HD_TS_BatFight",
	"4. Hñy s¹ch nhiÖm vô TÝn Sø (nh­ NPC)".."/HD_TS_Huy",
	"5. ®Õn ¶i 3 - Thiªn B¶o Khè (map 395)".."/HD_TS_DenAi",
	"6. VÒ Ba L¨ng HuyÖn (dän tr¹ng th¸i nh­ NPC)".."/HD_TS_VeThanh",
	"KÕt thóc ®èi tho¹i".."/no"})
end

function HD_TS_Xem()
	Msg2Player(format("[TÝn Sø] 1203=%d 1201=%d 1202=%d 1204=%d 1205=%d 1206=%d 1211=%d",
		nt_getTask(1203), nt_getTask(1201), nt_getTask(1202), nt_getTask(1204),
		nt_getTask(1205), nt_getTask(1206), nt_getTask(1211)))
	local nMap, nX, nY = GetWorldPos()
	Msg2Player(format("[TÝn Sø] map=%d täa ®é (%d,%d), rót vò khÝ=%d (0 = kh«ng ®¸nh ®­îc qu¸i, kh«ng thÓ chÕt)", nMap, nX, nY, GetFightState()))
end

function HD_TS_GoKet()
	if (nt_getTask(1203) == 20) then
		nt_setTask(1203, 21)
		Msg2Player("[TÝn Sø] 1203: 20 -> 21. Tíi DÞch Quan bÊm 'TiÕp tôc nhiÖm vô' ®Ó bËt l¹i ®Çy ®ñ tr¹ng th¸i (®ång hå, chiÕn ®Êu, håi sinh...).")
	else
		Msg2Player("[TÝn Sø] 1203 = "..nt_getTask(1203).." - chØ gì kÑt khi ®ang 20.")
	end
end

function HD_TS_BatFight()
	SetFightState(1)
	Msg2Player("[TÝn Sø] ®· RóT Vò KHÝ (bËt chiÕn ®Êu). Thö ®¸nh qu¸i ngay. L­u ý: giê cã thÓ CHÕT thËt.")
end

function HD_TS_Huy()
	losemessengertask()
	Msg2Player("[TÝn Sø] ®· gäi hñy nhiÖm vô (chØ xãa khi ®ang 20/21). NhËn l¹i ë DÞch Quan thµnh Ba L¨ng hoÆc ®¹i Lý.")
end

function HD_TS_DenAi()
	SetRevPos(11,10)
	NewWorld(395,1417,3207)
end

function HD_TS_VeThanh()
	DisabledUseTownP(0)
	SetFightState(0)
	SetPunish(1)
	SetCreateTeam(1)
	SetPKFlag(0)
	ForbidChangePK(0)
	SetDeathScript("")
	SetLogoutRV(0)
	NewWorld(11,3021,5090)
end
