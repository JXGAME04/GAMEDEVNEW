-- simcity_admin.lua - Menu admin cho tinh nang SimCity (bot nguoi choi gia lap)
-- Port SimCity JX2 -> JX1. Xem BANGIAO_SIMCITY_JX1.md.
--
-- Goi tu lenhbaiadmin.lua: muc "SimCity - bot gia lap/SC_Menu"
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

SC_END_SAY   = "Ket thuc doi thoai./no"
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
	local nMove = "TAT"
	if SC_MoveState == 1 then
		nMove = "BAT"
	end
	-- LUU Y: KHONG duoc dung ky tu "|" trong bat ky chuoi nao.
	-- sUiAppendAnswer (ScriptFuns.cpp:548) dung "|" lam DAU PHAN CACH giua tieu de
	-- va tung lua chon, tat ca trong 1 buffer 512 byte (MAX_SCIRPTACTION_BUFFERNUM).
	-- Co "|" trong tieu de -> client tach sai -> mat het cac dong chon.
	local nChat = "TAT"
	if SC_ChatState == 1 then
		nChat = "BAT"
	end
	SayEx({format("<color=yellow>SimCity - bot gia lap<color>\nDi chuyen: <color=green>%s<color>  Noi chuyen: <color=green>%s<color>\nBot: <color=gold>%d<color>  Lo trinh: <color=gold>%d<color>", nMove, nChat, SC_LastBot, SC_LastRoute),
	"Sinh 1 bot tai cho toi dung/SC_SpawnHere",
	"Sinh 5 bot tai cho toi dung/SC_Spawn5",
	"Cho bot vua sinh di tuan tra (vuong)/SC_PatrolLast",
	"Di theo lo trinh ban do nay/SC_RouteMenu",
	"Bat driver di chuyen/SC_On",
	"Tat driver di chuyen/SC_Off",
	"Bat bot noi chuyen/SC_ChatOn",
	"Tat bot noi chuyen/SC_ChatOff",
	"Xoa het bot/SC_Clear",
	"Bot NGUOI CHOI THAT (KPlayer)/PB_Menu",
	SC_END_SAY})
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
	SayEx({format("<color=yellow>Bot nguoi choi THAT<color>\nDang song: <color=gold>%d<color> / toi da <color=gold>%d<color>\nBot nap tu roledb, co trang bi va chi so that.", nCo, nTran),
	"Goi 1 bot (tai khoan 1)/#PB_Call(1,1)",
	"Goi 5 bot (tai khoan 1-5)/#PB_Call(1,5)",
	"Goi 20 bot (tai khoan 1-20)/#PB_Call(1,20)",
	"Cho bot VAO PHAI (theo ngu hanh)/PB_Join",
	"BAT danh quai/#PB_Fight(1)",
	"TAT danh quai/#PB_Fight(0)",
	"BAT bot noi chuyen/#PB_Chat(40)",
	"TAT bot noi chuyen/#PB_Chat(0)",
	"Go het bot nguoi choi/PB_Clear",
	"Quay lai/SC_Menu",
	SC_END_SAY})
end

function PB_Call(nTu, nDen)
	local nXep, nTran = PB_AddBot(nTu, nDen)
	if nXep and nXep > 0 then
		Msg2Player(format("Da xep %d tai khoan vao hang doi (toi da %d bot).", nXep, nTran))
		Msg2Player("Bot sinh DAN theo nhip, doi vai giay roi xem lai so bot.")
	else
		Msg2Player("Khong xep duoc. Co the da cham tran bot, hoac hang doi day.")
	end
	PB_Menu()
end

function PB_Clear()
	local n = PB_ClearBot()
	Msg2Player(format("Da go %d bot khoi the gioi.", n))
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
		Msg2Player(format("Da BAT danh quai cho %d bot.", n))
		Msg2Player("Bot tu tim quai quanh no; xem console dong [BotDanh] de biet chieu da chon.")
	else
		Msg2Player(format("Da TAT danh quai cho %d bot.", n))
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
			Msg2Player(format("Da BAT bot noi chuyen (kho %d cau, muc %d/1000 moi giay).", n, nRate))
		else
			Msg2Player("Nap kho cau THAT BAI - kiem settings/simcity/chat.txt.")
		end
	else
		Msg2Player("Da TAT bot noi chuyen.")
	end
	PB_Menu()
end

function PB_Join()
	local n = PB_JoinFaction()
	if n and n > 0 then
		Msg2Player(format("Da ra lenh vao phai cho %d bot.", n))
		Msg2Player("Bot tu chay bo toi NPC mon phai; xem console de biet ket qua.")
	else
		Msg2Player("Khong co bot nao nhan lenh. Goi bot ra truoc da.")
	end
	PB_Menu()
end

-- ================= GD3: BOT NOI CHUYEN =================
-- Nap kho cau thoai 1 lan roi bat xac suat noi.
-- chat.txt: cot 1 = Type, cot 2 = Chat. Type nhieu nhat: general (1690), fighting (928).
function SC_ChatOn()
	if SC_ChatLoaded ~= 1 then
		local n = SC_LoadChat(SC_CHATFILE, "general")
		if n and n > 0 then
			SC_ChatLoaded = 1
			Msg2Player(format("Da nap %d cau thoai (general).", n))
		else
			Msg2Player("Nap cau thoai THAT BAI. Kiem tep settings/simcity/chat.txt da chep chua.")
			SC_Menu()
			return
		end
	end
	SC_ChatChance(SC_CHATRATE)
	SC_ChatState = 1
	Msg2Player(format("Bot se noi chuyen (%d/1000 moi giay moi bot).", SC_CHATRATE))
	SC_Menu()
end

function SC_ChatOff()
	SC_ChatChance(0)
	SC_ChatState = 0
	Msg2Player("Da TAT bot noi chuyen.")
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
			Msg2Player("CANH BAO: kho ngoai trang RONG - kiem tep settings res cua client/server.")
		else
			Msg2Player(format("Kho ngoai trang: non %d, ao %d, vu khi %d, ngua %d.", nHelm, nArmor, nWeapon, nHorse))
		end
	end

	-- mon phai 0..9 (5 he x 2 phai). SC_SetBotFaction tu dat camp theo phai,
	-- neu khong bot se mang camp_free = "mau do sat thu", sai han y do dan thanh thi.
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
		Msg2Player(format("Da sinh bot idx = %d tai map %d (%d,%d).", nIdx, nW, nX, nY))
	else
		Msg2Player("Sinh bot THAT BAI. Kiem toa do / subworld (hoac ten bi trung lien tiep).")
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
	Msg2Player(format("Da sinh %d/5 bot quanh cho ban dung.", nOk))
	SC_Menu()
end

-- ================= DI CHUYEN =================
function SC_PatrolLast()
	if SC_LastBot <= 0 then
		Msg2Player("Chua co bot nao. Hay sinh bot truoc.")
		SC_Menu()
		return
	end
	-- SC_PatrolBox nay kiem TestBarrier tung goc va tu thu nho ban kinh; tra 0 khi cho qua chat
	-- (duoi 2 goc di duoc). Truoc day 4 goc khong he duoc kiem nen goc nam trong nha -> bot ket.
	if SC_PatrolBox(SC_LastBot) == 1 then
		Msg2Player(format("Bot %d da nhan lo trinh tuan tra (vuong). Nho BAT driver.", SC_LastBot))
	else
		Msg2Player("Cho nay qua chat de tuan tra (khong du 2 goc di duoc). Hay ra cho rong hon.")
	end
	SC_Menu()
end

function SC_On()
	SC_MoveOn()
	SC_MoveState = 1
	Msg2Player("Da BAT driver di chuyen. (Bot chi buoc khi co nguoi choi dung gan.)")
	SC_Menu()
end

function SC_Off()
	SC_MoveOff()
	SC_MoveState = 0
	Msg2Player("Da TAT driver di chuyen.")
	SC_Menu()
end

function SC_Clear()
	local n = SC_ClearBots()
	SC_LastBot   = 0
	SC_LastRoute = -1
	Msg2Player(format("Da xoa %d bot.", n))
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
		tinsert(tb, 1, format("<color=yellow>Chon tuyen duong<color>\nMap <color=gold>%d<color> CHUA khai bao tuyen nao.\nThem vao bang SC_ROUTES trong simcity_admin.lua.", nW))
	else
		tinsert(tb, 1, format("<color=yellow>Chon tuyen duong<color>\nMap <color=gold>%d<color> co <color=green>%d<color> tuyen.\nBot se di vong theo tuyen chon.", nW, nFound))
	end
	tinsert(tb, "Quay lai/SC_Menu")
	tinsert(tb, SC_END_SAY)
	SayEx(tb)
end

function SC_UseRoute(nSel)
	local r = SC_ROUTES[nSel]
	if not r then
		Msg2Player("Tuyen khong hop le.")
		SC_Menu()
		return
	end
	if SC_LastBot <= 0 then
		Msg2Player("Chua co bot. Hay sinh bot truoc (menu chinh).")
		SC_Menu()
		return
	end
	local nW = GetWorldPos()
	if r[1] ~= nW then
		Msg2Player(format("Tuyen nay thuoc map %d, ban dang o map %d. Bot se khong di duoc.", r[1], nW))
		SC_Menu()
		return
	end
	local nRid = SC_LoadPreset(SC_PRESETDIR..r[2], r[3])
	if not nRid or nRid < 0 then
		Msg2Player(format("Nap tuyen THAT BAI: %s / %s. Kiem du lieu da chep sang settings/simcity chua.", r[2], r[3]))
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
		Msg2Player(format("Bot %d da nhan tuyen %s (routeId %d). Nho BAT driver.", SC_LastBot, r[3], nRid))
	else
		if nCell then
			Msg2Player(format("Bot cach tuyen %s toi %d o (toi da 32). Hay dung gan tuyen roi sinh bot lai.", r[3], nCell))
		else
			Msg2Player("Gan tuyen cho bot THAT BAI.")
		end
	end
	SC_Menu()
end
