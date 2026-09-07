-- ================================================================================================
-- [HE THONG] script/startgame/lmbj_addnpc.lua
-- Muc dich  : Dat NPC VAN TIEU luc khoi dong.
-- Duoc nap  : Include tu 2 tep (vd test_vantieu_admin.lua, startgame.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : npcfunlib.lua
-- Ham (dong): lmbj_addnpc_bang (80), lmbj_addnpc_canhan (93), lmbj_addnpc (101)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- ============================================================
-- [LMBC 06/09] KHOI TAO NPC LONG MON TIEU CUC (van tieu).
--
-- VI SAO CAN TEP NAY: khung activitysys cua JX1 dang CHET - G_ACTIVITY:AddActivity
-- va LoadActivitys co dinh nghia nhung KHONG CHO NAO GOI, va JX1 khong co autoexec.lua.
-- Chep du 15 tep config\129 van se KHONG NPC NAO XUAT HIEN, va trieu chung la
-- 'khong loi gi ca nhung khong thay NPC' - rat kho truy. Nen sinh NPC o day, theo dung
-- loi nha JX1 (khuon script\task\tollgate\tinsu_addnpc.lua).
--
-- Toa do chep nguyen van tu ban Linux config\129\variables.lua:4,8,12-27.
-- Ten NPC doc tu chinh npcs.txt nen khop tung byte voi bang.
-- startgame.lua goi lmbj_addnpc().
-- ============================================================
Include("\\script\\activitysys\\npcfunlib.lua")

LMBJ_TEN_CONSIGNER = "TiÕp DÉn ¸p Tiªu Bang"
LMBJ_TEN_RECEIVER  = "NhËn Hµng ¸p Tiªu Bang"
LMBJ_TEN_BIAOSHI   = "Long M«n Tiªu S­"
LMBJ_TEN_ZHANGGUI  = "¤ng chñ Tiªu côc Lôc Tam C©n"

-- {mapID, o x, o y}
LMBJ_POS_CONSIGNER = {
	{80, 1678, 3147},
	{176, 1399, 3140},
	{37, 1703, 3019},
	{11, 3205, 5078},
	{78, 1569, 3280},
	{1, 1559, 3258},
	{162, 1650, 3247},
}

LMBJ_POS_RECEIVER = {
	{116, 1741, 3031},
	{93, 1620, 3182},
	{94, 1638, 3106},
	{181, 1627, 3052},
	{180, 1548, 3176},
	{182, 2107, 3412},
	{42, 1642, 3029},
	{45, 1702, 3076},
	{204, 1621, 3375},
	{103, 1638, 2930},
	{10, 1778, 3065},
	{23, 1854, 3123},
	{22, 1865, 3274},
	{5, 1597, 3592},
	{91, 1602, 2810},
	{83, 1660, 2924},
	{205, 1587, 3221},
	{201, 1736, 3235},
	{2, 2406, 3713},
	{4, 1672, 3096},
	{168, 1653, 3138},
	{171, 1494, 2982},
	{203, 1505, 3102},
}

LMBJ_POS_BIAOSHI = {
	{176, 1448, 3335},
	{176, 1534, 2974},
	{11, 3061, 4992},
	{78, 1537, 3182},
	{1, 1597, 3093},
	{162, 1639, 3215},
	{37, 1690, 3150},
	{80, 1786, 3037},
	{20, 3516, 6164},
	{99, 1673, 3229},
	{100, 1622, 3113},
	{101, 1625, 3209},
	{153, 1677, 3240},
	{174, 1596, 3271},
}

LMBJ_POS_ZHANGGUI = {
	{121, 1989, 4476},
}

-- Nhanh BANG (ap Tieu Bang 10 sao): NPC tiep dan + NPC nhan hang.
function lmbj_addnpc_bang()
	NpcFunLib:AddObjNpc(LMBJ_TEN_CONSIGNER, 2230, LMBJ_POS_CONSIGNER, "\\script\\activitysys\\config\\129\\npc_consigner.lua")
	NpcFunLib:AddObjNpc(LMBJ_TEN_RECEIVER,  2231, LMBJ_POS_RECEIVER,  "\\script\\activitysys\\config\\129\\npc_receiver.lua")
end

-- Nhanh CA NHAN (1-9 sao): Long Mon Tieu Su + ong chu tieu cuc.
-- [VTCN 06/09] Script NPC = npc_canhan.lua (thoai noi vao nhanh ca nhan GOC LINUX khong
-- trang thai trong config\129\extend.lua). tasknpc.lua (lop LongMenBiaoJu.*) KHONG chay
-- duoc tren JX1: goi GetDlgClass() ma khong Include dialog.lua, va so dang ky xe nam trong
-- state cua tasknpc.lua con OnTimer/OnDeath cua xe chay o state biaoche.lua (moi tep .lua
-- mot lua_State - Engine\Src\KLuaScript.cpp:23). Muon lui: doi hang duoi ve tasknpc.lua.
LMBJ_SCRIPT_CANHAN = "\\script\\activitysys\\config\\129\\npc_canhan.lua"
-- LMBJ_SCRIPT_CANHAN = "\\script\\event\\longmenbiaoju\\tasknpc.lua"	-- ban cu (lop LongMenBiaoJu)
function lmbj_addnpc_canhan()
	NpcFunLib:AddObjNpc(LMBJ_TEN_BIAOSHI,  2145, LMBJ_POS_BIAOSHI,  LMBJ_SCRIPT_CANHAN)
	NpcFunLib:AddObjNpc(LMBJ_TEN_ZHANGGUI, 2157, LMBJ_POS_ZHANGGUI, LMBJ_SCRIPT_CANHAN)
end

-- Bat/tat tung nhanh o day. De 0 la nhanh do khong sinh NPC nao.
LMBJ_BAT_BANG   = 1
LMBJ_BAT_CANHAN = 1

function lmbj_addnpc()
	if (LMBJ_BAT_BANG == 1) then
		lmbj_addnpc_bang()
	end
	if (LMBJ_BAT_CANHAN == 1) then
		lmbj_addnpc_canhan()
	end
end
