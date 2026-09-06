# -*- coding: utf-8 -*-
"""patch_srv.py [VTCN 06/09] - va cac script may chu cho: co test van tieu, nhanh ca nhan
khong trang thai, ma vat pham dung, event.lua chay tai cho, lenh bai admin gon.
Moi tep: sao luu .truoc_vtcn_0609 (neu chua co), sua qua latin-1 (byte-safe), kiem so lan khop,
kiem khong sinh U+FFFD. Chay: python patch_srv.py [--dry]
"""
import io, os, sys, shutil
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DRY = "--dry" in sys.argv
BAK = ".truoc_vtcn_0609"

def V(u):
    """unicode -> latin-1 str mang byte TCVN3"""
    return unicode_to_tcvn3_bytes(u).decode("latin-1")

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def wr(p, s):
    if DRY:
        print("  (dry) ghi", p)
        return
    if not os.path.isfile(p + BAK):
        shutil.copyfile(p, p + BAK)
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)

def rep(s, old, new, count=1, tag=""):
    n = s.count(old)
    if n != count:
        sys.exit("LOI %s: mong %d khop, thay %d cho: %r" % (tag, count, n, old[:60]))
    return s.replace(old, new)

def patch(rel, fn):
    p = os.path.join(SRV, rel)
    s0 = rd(p)
    s1 = fn(s0)
    if s1 == s0:
        sys.exit("LOI: %s khong doi gi" % rel)
    assert "\ufffd" not in s1
    print("%-58s byte cao %d -> %d" % (rel, hb(s0), hb(s1)))
    wr(p, s1)

# ---------------------------------------------------------------- P1 lib_lmbiaoche.lua
P1 = r'''
-- =========================================================================
-- [VTCN 06/09] CO TEST van tieu - 3 bit trong bien nhiem vu 4169 CUA NGUOI CHOI
-- (chi admin dat qua Lenh bai admin > Bo test van tieu > Co test). Vi JX1 moi
-- tep .lua la mot lua_State rieng nen KHONG dung bien toan cuc de bat co; bien
-- nhiem vu thi moi state deu doc duoc va chi anh huong nguoi bat co.
--   bit 1 (1): bo qua khung gio / thu   (ca nhan 10-23h; bang thu 7, CN 12-23h)
--   bit 2 (2): bo qua "vao bang du 7 ngay"
--   bit 3 (4): coi bang minh la bang dang chiem thanh dang dung
-- 4169 nam ngoai moi dai dang dung (4160-4168 nhanh cu, 4178-4187 nhanh bang).
-- =========================================================================
VT_TEST_TASK = 4169

function VT_TestBoQua(nBit)
	if (GetTask == nil or PlayerIndex == nil or PlayerIndex <= 0) then
		return 0
	end
	local v = GetTask(VT_TEST_TASK)
	if (v == nil or v == 0) then
		return 0
	end
	local nMask = 1
	for i = 2, nBit do
		nMask = nMask * 2
	end
	if (mod(floor(v / nMask), 2) == 1) then
		return 1
	end
	return 0
end

function VT_TestDat(nBit, bOn)
	local v = GetTask(VT_TEST_TASK)
	if (v == nil) then
		v = 0
	end
	local nMask = 1
	for i = 2, nBit do
		nMask = nMask * 2
	end
	local bCo = (mod(floor(v / nMask), 2) == 1)
	if (bOn == 1 and not bCo) then
		v = v + nMask
	elseif (bOn == 0 and bCo) then
		v = v - nMask
	end
	SetTask(VT_TEST_TASK, v)
end
'''.replace("\n", "\r\n")

def p1(s):
    if "VT_TestBoQua" in s:
        sys.exit("lib_lmbiaoche.lua da co VT_TestBoQua")
    if not s.endswith("\r\n"):
        s += "\r\n"
    return s + P1

# ---------------------------------------------------------------- P2 variables.lua
def p2(s):
    old = "szNpcZhangGuiName = "
    new = ("-- [VTCN 06/09] VT_TestBoQua (co test cua admin) cho moi tep config\\129\r\n"
           "Include(\"\\\\script\\\\lib\\\\lib_lmbiaoche.lua\")\r\n\r\n"
           "szNpcZhangGuiName = ")
    return rep(s, old, new, 1, "variables")

# ---------------------------------------------------------------- P3 npc_consigner.lua
def p3(s):
    s = rep(s, "    if nWeekIdx ~= 0 and nWeekIdx ~= 6 then",
               "    if (nWeekIdx ~= 0 and nWeekIdx ~= 6) and VT_TestBoQua(1) ~= 1 then	-- [VTCN] co test 1", 4, "consigner thu")
    s = rep(s, "    if nCurTime < 1200 or nCurTime > 2300 then",
               "    if (nCurTime < 1200 or nCurTime > 2300) and VT_TestBoQua(1) ~= 1 then	-- [VTCN] co test 1", 5, "consigner gio")
    return s

# ---------------------------------------------------------------- P4 extend.lua
def p4(s):
    # (a) award.lua: self.tbAward / tbAllCountCell / tbPickCountCell dung o ReceiveAward,
    #     HandTongBiaoChe, GetTongAwardCallBack ma tep nay khong Include -> nil. Dat NGAY SAU
    #     variables.lua vi award.lua Include lai head.lua (ActivityClass:new() tao pActivity MOI)
    #     nen phai nam TRUOC moi dinh nghia pActivity.* cua tep nay.
    old = 'Include("\\\\script\\\\activitysys\\\\config\\\\129\\\\variables.lua")\r\n'
    new = (old +
           '-- [VTCN 06/09] award.lua dinh nghia tbAward/tbAllCountCell/tbPickCountCell ma tep nay dung\r\n'
           '-- (self.tbAward...). JX1 moi tep mot lua_State nen phai Include o day; dat truoc moi\r\n'
           '-- dinh nghia pActivity.* vi award.lua Include lai head.lua (ActivityClass:new() = doi tuong moi).\r\n'
           'Include("\\\\script\\\\activitysys\\\\config\\\\129\\\\award.lua")\r\n')
    s = rep(s, old, new, 1, "extend award")
    # (b) CheckJoinTongDays
    old = "function pActivity:CheckJoinTongDays(nDay)\r\n    local nDayTime = nDay * 24 * 60;"
    new = ("function pActivity:CheckJoinTongDays(nDay)\r\n"
           "    if VT_TestBoQua(2) == 1 then	-- [VTCN] co test 2: bo qua 7 ngay\r\n"
           "        return 1;\r\n"
           "    end\r\n"
           "    local nDayTime = nDay * 24 * 60;")
    s = rep(s, old, new, 1, "extend 7ngay")
    # (c) GetOccupyTongId
    old = "    local nMapID = GetWorldPos();\r\n    if not tbMainCity[nMapID] then"
    new = ("    local nMapID = GetWorldPos();\r\n"
           "    if VT_TestBoQua(3) == 1 and tbMainCity[nMapID] then	-- [VTCN] co test 3: coi bang minh chiem thanh\r\n"
           "        local szMyTong, nMyTong = GetTongName();\r\n"
           "        if nMyTong ~= 0 then\r\n"
           "            return nMyTong, szMyTong;\r\n"
           "        end\r\n"
           "    end\r\n"
           "    if not tbMainCity[nMapID] then")
    s = rep(s, old, new, 1, "extend chiemthanh")
    # (d) khung gio nhanh ca nhan (2 cho, khac nhau mot dau '=')
    s = rep(s, "    if (nCurTime>2300 or nCurTime<1000) then",
               "    if (nCurTime>2300 or nCurTime<1000) and VT_TestBoQua(1) ~= 1 then	-- [VTCN] co test 1", 1, "extend gio1")
    s = rep(s, "    if (nCurTime>=2300 or nCurTime < 1000) then",
               "    if (nCurTime>=2300 or nCurTime < 1000) and VT_TestBoQua(1) ~= 1 then	-- [VTCN] co test 1", 1, "extend gio2")
    return s

# ---------------------------------------------------------------- P5 taskclass.lua (nhanh cu, giu de lui)
def p5(s):
    old = "\tif nCurTime < StartTime[1]*100 + StartTime[2] or ( nCurTime >= EndTime[1]*100 + EndTime[2]) then"
    new = "\tif (nCurTime < StartTime[1]*100 + StartTime[2] or ( nCurTime >= EndTime[1]*100 + EndTime[2])) and VT_TestBoQua(1) ~= 1 then	-- [VTCN] co test 1"
    return rep(s, old, new, 3, "taskclass gio")

# ---------------------------------------------------------------- P6 lmbj_config.lua
def p6(s):
    for a, b in ((4200, 4771), (4201, 4772), (4202, 4773), (4203, 4774), (4204, 4775), (4205, 4776), (4207, 4778)):
        s = rep(s, "tbProp = {6, 1, %d, 1, 0, 0}," % a, "tbProp = {6, 1, %d, 1, 0, 0},	-- [VTCN 06/09] ma JX1 (magicscript.txt), Linux la %d" % (b, a), 1, "lmbj_config %d" % a)
    s = rep(s, "tbProp = {6, 1, 4534, 1, 0, 0}, nCount = 1, nBindState = -2},",
               "tbProp = {6, 1, 4809, 1, 0, 0}, nCount = 1, nBindState = -2},	-- [VTCN 06/09] 4534 la dong test rac trong magicscript JX1; 4809 = Ho Tieu Le Hop (mon gan nhat, chua co script su dung)", 1, "lmbj_config lebao")
    old = "LongMenBiaoJu.ItemList = {"
    new = ("-- [VTCN 06/09] SUA MA: bang Linux 4200-4207 KHONG phai ma JX1. Trong magicscript.txt cua may chu\r\n"
           "-- nay (dong = ParticularType + 1): 4771 Tieu Ky, 4772 Hoan Tieu Chi, 4773 uy nhiem trang cao cap,\r\n"
           "-- 4774 Ho Tieu Lenh, 4775 Khoai Ma Gia Tien, 4776 Kien Bat Kha Toa, 4777 Thiet Xa Mat Bo,\r\n"
           "-- 4778 Tieu Xa Di Vi. Dong 4200-4207 la 'Thuong Long Van Tinh - Binh' - phat nham mon.\r\n"
           "LongMenBiaoJu.ItemList = {")
    return rep(s, old, new, 1, "lmbj_config ghi chu")

# ---------------------------------------------------------------- P7 item.lua
def p7(s):
    s = rep(s, 'if szGDP == "6,1,4204" then', 'if szGDP == "6,1,4775" then	-- [VTCN 06/09] ma JX1 (Linux 4204)', 1, "item 4204")
    s = rep(s, 'elseif szGDP == "6,1,4205" then', 'elseif szGDP == "6,1,4776" then	-- [VTCN] Linux 4205', 1, "item 4205")
    s = rep(s, 'elseif szGDP == "6,1,4207" then', 'elseif szGDP == "6,1,4778" then	-- [VTCN] Linux 4207', 1, "item 4207")
    s = rep(s, "\tif (nP == 4204) then", "\tif (nP == 4775) then	-- [VTCN] Linux 4204", 1, "item nP4204")
    s = rep(s, "\telseif (nP == 4205) then", "\telseif (nP == 4776) then	-- [VTCN] Linux 4205", 1, "item nP4205")
    s = rep(s, "\telseif (nP == 4207) then", "\telseif (nP == 4778) then	-- [VTCN] Linux 4207", 1, "item nP4207")
    old = 'Include("\\\\script\\\\activitysys\\\\config\\\\129\\\\variables.lua")\r\n'
    new = (old + '-- [VTCN 06/09] getSkillLevel doc pActivity.tbBJPathLevel (extend.lua) - JX1 moi tep mot state\r\n'
                 'Include("\\\\script\\\\activitysys\\\\config\\\\129\\\\extend.lua")\r\n')
    return rep(s, old, new, 1, "item extend")

# ---------------------------------------------------------------- P8 npc_lmbiaowu.lua / npc_lmbiaobox.lua
def p8(s):
    old = 'Include("\\\\script\\\\activitysys\\\\config\\\\129\\\\variables.lua")\r\n'
    new = (old + '-- [VTCN 06/09] pActivity.tbPickCountCell/tbAward (award.lua) + AddTaskDailyA/CheckJoinTongDays/\r\n'
                 '-- WriteTongYBLog (extend.lua): JX1 moi tep mot lua_State nen phai Include o day\r\n'
                 'Include("\\\\script\\\\activitysys\\\\config\\\\129\\\\extend.lua")\r\n')
    return rep(s, old, new, 1, "lmbiaowu/box extend")

# ---------------------------------------------------------------- P9 event.lua (chay TAI CHO tren GameServer)
def p9(s):
    old = ("    local nMapID, nX, nY, nState = GetBiaoChePos(szPlayerName);\r\n"
           "    tbCarPos[1] = nMapID;\r\n"
           "    tbCarPos[2] = nX;\r\n"
           "    tbCarPos[3] = nY;\r\n"
           "    tbCarPos[4] = nState;\r\n")
    new = ("    -- [VTCN 06/09] JX1: RemoteExecute chay TAI CHO (KJx2SharedStore.cpp LuaJX2_RemoteExecute)\r\n"
           "    -- trong state cua tep nay, KHONG dat PlayerIndex; va GetBiaoChePos cua engine JX1 la ban\r\n"
           "    -- GameServer: khong tham so, tra (x32, y32, subworldIdx) cua NGUOI GOI (KBiaoChe.cpp).\r\n"
           "    -- Ban Linux o day chay tren relay voi GetBiaoChePos(szName) tra (mapId, oX, oY, coChienDau).\r\n"
           "    -- -> tra xe theo TEN qua CallPlayerFunction roi doi ve dang relay de 6 ham goi lai\r\n"
           "    -- (getBiaoChePosCallBack, showTongBiaoCheCallBack, ...) giu nguyen: NewWorld(map, oX, oY).\r\n"
           "    local nPIdx = SearchPlayer(szPlayerName);\r\n"
           "    if (nPIdx == nil or nPIdx <= 0) then\r\n"
           "        ObjBuffer:PushObject(ResultHandle, tbCarPos);\r\n"
           "        return nil;\r\n"
           "    end\r\n"
           "    local a, b, c, d = CallPlayerFunction(nPIdx, GetBiaoChePos);\r\n"
           "    if (d ~= nil) then\r\n"
           "        tbCarPos = {a, b, c, d};			-- dang relay 4 gia tri (neu sau nay co)\r\n"
           "    elseif (c ~= nil and c >= 0) then\r\n"
           "        tbCarPos = {SubWorldIdx2ID(c), floor(a / 32), floor(b / 32), CallPlayerFunction(nPIdx, GetFightState)};\r\n"
           "    end\r\n")
    return rep(s, old, new, 1, "event GetBiaoChePos")

# ---------------------------------------------------------------- P10 lmbj_addnpc.lua
def p10(s):
    old = "-- Nhanh CA NHAN (1-9 sao): Long Mon Tieu Su + ong chu tieu cuc.\r\n-- Ca hai deu dung chung tasknpc.lua cua ban Linux.\r\n"
    new = ("-- Nhanh CA NHAN (1-9 sao): Long Mon Tieu Su + ong chu tieu cuc.\r\n"
           "-- [VTCN 06/09] Script NPC = npc_canhan.lua (thoai noi vao nhanh ca nhan GOC LINUX khong\r\n"
           "-- trang thai trong config\\129\\extend.lua). tasknpc.lua (lop LongMenBiaoJu.*) KHONG chay\r\n"
           "-- duoc tren JX1: goi GetDlgClass() ma khong Include dialog.lua, va so dang ky xe nam trong\r\n"
           "-- state cua tasknpc.lua con OnTimer/OnDeath cua xe chay o state biaoche.lua (moi tep .lua\r\n"
           "-- mot lua_State - Engine\\Src\\KLuaScript.cpp:23). Muon lui: doi hang duoi ve tasknpc.lua.\r\n"
           "LMBJ_SCRIPT_CANHAN = \"\\\\script\\\\activitysys\\\\config\\\\129\\\\npc_canhan.lua\"\r\n"
           "-- LMBJ_SCRIPT_CANHAN = \"\\\\script\\\\event\\\\longmenbiaoju\\\\tasknpc.lua\"	-- ban cu (lop LongMenBiaoJu)\r\n")
    s = rep(s, old, new, 1, "addnpc ghi chu")
    s = rep(s, '"\\\\script\\\\event\\\\longmenbiaoju\\\\tasknpc.lua")', "LMBJ_SCRIPT_CANHAN)", 2, "addnpc script")
    return s

# ---------------------------------------------------------------- P11 lenhbaiadmin.lua
MENU_MOI = V(u'''SayEx({format(PLAYER_INFOMATION,nName,nW,nX*32,nY*32,nX,nY,PlayerIndex),
		-- [VTCN 06/09] menu chinh gom lai 8 muc (truoc 19): cac bo test xep vao 2 nhom
		"Quản lý máy chủ/system",
		"Chức năng chính: nhận trang bị, điểm, vật phẩm, thử nghiệm/manager",
		"BOT người chơi/PB_Menu",
		"Chiến Lệnh (admin)/CL_MenuAdmin",	-- [CL 04/09]
		"Gửi thư thử: có vật phẩm/mailtest2",	-- [MAIL 03/09]
		"Bộ test hoạt động: Vận tiêu, Bang hội, Dã Tẩu, Liên đấu, Sát Thủ, Viêm Đế.../ADM_TestHoatDong",
		"Bộ test hệ thống nhân vật: Lò rèn, Kinh mạch, Đồng hành, Phi Phong, Dung luyện/ADM_TestNhanVat",
		"Kết thúc đối thoại/no"})''')

HAM_MOI = V(u'''

-- ============================================================
-- [VTCN 06/09] Menu chinh gom lai: 2 nhom bo test (menu cu 19 muc -> 8 muc).
-- Cac ham goc (TX_Root, DT_AdminMenu, ...) giu nguyen, chi doi cho bam.
-- ============================================================
function ADM_Main()
	if (admincheck() ~= 1) then
		return
	end
	chucnangadmin(-1)
end

function ADM_TestHoatDong()
	SayEx({"<color=yellow>Bộ test hoạt động<color>: chọn mục",
	"Vận tiêu Long Môn Tiêu Cục (cá nhân + bang hội)/VT_TestRoot",	-- [VTCN 06/09]
	"Bang hội - công thành/TX_Root",
	"Dã Tẩu: xoá phạt - thêm lượt/DT_AdminMenu",
	"Liên đấu/LD_AdminMenu",
	"Bang Chiến - Bách Nhân - Tỷ Võ - Thành Bảo/HD_AdminMenu",
	"Hoạt động Linux: Sát Thủ - Phong Lăng Độ - Vượt ải/HD3_AdminMenu",
	"Viêm Đế/TTHD_Root",
	"Quay lại lệnh bài/ADM_Main",
	"Kết thúc đối thoại/no"})
end

function ADM_TestNhanVat()
	SayEx({"<color=yellow>Bộ test hệ thống nhân vật<color>: chọn mục",
	"Lò rèn/LR_Root",
	"Kinh Mạch/KM_TestRoot",
	"Đồng hành/BDH_Root",
	"Phi Phong/PP_Root",
	"Dung luyện/DL_TestRoot",
	"Quay lại lệnh bài/ADM_Main",
	"Kết thúc đối thoại/no"})
end
''').replace("\n", "\r\n")

def p11(s):
    a = s.find("SayEx({format(PLAYER_INFOMATION")
    b = s.find('/no"})', a)
    if a < 0 or b < 0 or s.count("SayEx({format(PLAYER_INFOMATION") != 1:
        sys.exit("lenhbaiadmin: khong tim thay khoi menu")
    b += len('/no"})')
    old_block = s[a:b]
    print("  khoi menu cu %d byte, %d byte cao -> moi %d byte, %d byte cao" % (len(old_block), hb(old_block), len(MENU_MOI), hb(MENU_MOI)))
    s = s[:a] + MENU_MOI.replace("\n", "\r\n") + s[b:]
    old = 'Include("\\\\script\\\\item\\\\test_dungluyen_admin.lua")'
    i = s.find(old)
    if i < 0:
        sys.exit("lenhbaiadmin: khong thay Include test_dungluyen_admin")
    j = s.find("\r\n", i)
    s = s[:j] + '\r\nInclude("\\\\script\\\\item\\\\test_vantieu_admin.lua")	-- [VTCN 06/09] bo test VAN TIEU Long Mon Tieu Cuc' + s[j:]
    if "function ADM_TestHoatDong" in s:
        sys.exit("lenhbaiadmin da co ADM_TestHoatDong")
    if not s.endswith("\r\n"):
        s += "\r\n"
    return s + HAM_MOI

# ---------------------------------------------------------------- chay
patch(r"script\lib\lib_lmbiaoche.lua", p1)
patch(r"script\activitysys\config\129\variables.lua", p2)
patch(r"script\activitysys\config\129\npc_consigner.lua", p3)
patch(r"script\activitysys\config\129\extend.lua", p4)
patch(r"script\event\longmenbiaoju\taskclass.lua", p5)
patch(r"script\event\longmenbiaoju\lmbj_config.lua", p6)
patch(r"script\event\longmenbiaoju\item.lua", p7)
patch(r"script\activitysys\config\129\npc_lmbiaowu.lua", p8)
patch(r"script\activitysys\config\129\npc_lmbiaobox.lua", p8)
patch(r"script\event\longmenbiaoju\event.lua", p9)
patch(r"script\startgame\lmbj_addnpc.lua", p10)
patch(r"script\item\lenhbaiadmin.lua", p11)
print("XONG" + (" (dry)" if DRY else ""))
