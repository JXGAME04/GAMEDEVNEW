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
	SayEx({format("<color=yellow>SimCity - bot nguoi choi gia lap<color>\nDriver di chuyen: <color=green>%s<color>\nBot vua sinh: <color=gold>%d<color>  |  Lo trinh vua nap: <color=gold>%d<color>", nMove, SC_LastBot, SC_LastRoute),
	"Sinh 1 bot tai cho toi dung/SC_SpawnHere",
	"Sinh 5 bot tai cho toi dung/SC_Spawn5",
	"Cho bot vua sinh di tuan tra (vuong)/SC_PatrolLast",
	"Di theo lo trinh ban do nay/SC_RouteMenu",
	"Bat driver di chuyen/SC_On",
	"Tat driver di chuyen/SC_Off",
	"Xoa het bot/SC_Clear",
	SC_END_SAY})
end

-- ================= SINH BOT =================
-- sinh 1 bot ngay tai vi tri admin dang dung (khoi phai doan toa do)
function SC_SpawnHere()
	local nW, nX, nY = GetWorldPos()          -- nX,nY = O LUOI
	local nSwIdx = SubWorldID2Idx(nW)
	local nSex   = random(0, 1)
	local nIdx   = SC_AddBot(nSex, 100, nSwIdx, (nX + 1) * 32, (nY + 1) * 32, random(0, 4))
	if nIdx and nIdx > 0 then
		SC_LastBot = nIdx
		Msg2Player(format("Da sinh bot idx = %d tai map %d (%d,%d).", nIdx, nW, nX, nY))
	else
		Msg2Player("Sinh bot THAT BAI (SC_AddBot tra 0). Kiem toa do / subworld.")
	end
	SC_Menu()
end

function SC_Spawn5()
	local nW, nX, nY = GetWorldPos()
	local nSwIdx = SubWorldID2Idx(nW)
	local nOk = 0
	for i = 1, 5 do
		local nIdx = SC_AddBot(random(0, 1), 100, nSwIdx, (nX + i + 1) * 32, (nY + 1) * 32, random(0, 4))
		if nIdx and nIdx > 0 then
			nOk = nOk + 1
			SC_LastBot = nIdx
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
	if SC_PatrolBox(SC_LastBot) == 1 then
		Msg2Player(format("Bot %d da nhan lo trinh tuan tra (vuong). Nho BAT driver.", SC_LastBot))
	else
		Msg2Player(format("Gan tuan tra cho bot %d THAT BAI.", SC_LastBot))
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

function SC_RouteMenu()
	local nW = GetWorldPos()
	local tb = {}
	tinsert(tb, format("<color=yellow>Chon tuyen duong<color>\nBan dang o map <color=gold>%d<color>.\nChi tuyen CUNG MAP moi dung duoc.", nW))
	for i = 1, getn(SC_ROUTES) do
		local r = SC_ROUTES[i]
		local sMark = ""
		if r[1] == nW then
			sMark = " <color=green>(map nay)<color>"
		end
		tinsert(tb, format("Map %d - %s%s/#SC_UseRoute(%d)", r[1], r[3], sMark, i))
	end
	tinsert(tb, "Xem cac tuyen co trong map nay/SC_ListLocal")
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
	if SC_SetBotRoute(SC_LastBot, nRid, 1) == 1 then
		Msg2Player(format("Bot %d da nhan tuyen %s (routeId %d). Nho BAT driver.", SC_LastBot, r[3], nRid))
	else
		Msg2Player("Gan tuyen cho bot THAT BAI.")
	end
	SC_Menu()
end

-- liet ke tuyen khai bao san cho map hien tai (tien tra cuu)
function SC_ListLocal()
	local nW = GetWorldPos()
	local nFound = 0
	Msg2Player(format("--- Tuyen khai bao cho map %d ---", nW))
	for i = 1, getn(SC_ROUTES) do
		if SC_ROUTES[i][1] == nW then
			Msg2Player(format("  %s  (tep %s)", SC_ROUTES[i][3], SC_ROUTES[i][2]))
			nFound = nFound + 1
		end
	end
	if nFound == 0 then
		Msg2Player("  (khong co) - map nay chua khai bao tuyen trong SC_ROUTES.")
		Msg2Player("  Co the them vao bang SC_ROUTES trong simcity_admin.lua.")
	end
	SC_RouteMenu()
end
