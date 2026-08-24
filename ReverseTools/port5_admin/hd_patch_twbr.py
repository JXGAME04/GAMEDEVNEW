# -*- coding: utf-8 -*-
r"""[HD CONFIG 24/08] Phan 1: noi TONGWAR + BAIREN vao config trung tam
script\header\cauhinh_hoatdong.lua qua HD_CFG(khoa, macdinh).
Latin-1 (giu nguyen byte TCVN3/GBK), CRLF/LF giu nguyen, moi patch co guard idempotent.
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MTW = r"D:\GAMEDEVNEW\serverscript_jx2\tongwar\jx1_edits"
MBR = r"D:\GAMEDEVNEW\serverscript_jx2\bairenleitai\jx1_edits"

INC = 'Include("\\\\script\\\\header\\\\cauhinh_hoatdong.lua")'

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def nl(s): return "\r\n" if "\r\n" in s else "\n"
def mir(p_abs, mroot, rel):
    d = os.path.dirname(os.path.join(mroot, rel))
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(p_abs, os.path.join(mroot, rel))

def prepend_inc(s, tag):
    if "cauhinh_hoatdong" in s: return s
    return INC + "\t-- [HD CONFIG 24/08] " + tag + nl(s) + s

def sub1(s, old, new, tag):
    assert s.count(old) == 1, "anchor '%s' = %d" % (tag, s.count(old))
    return s.replace(old, new, 1)

# ============ 1) relay\tongwar.lua ============
p = os.path.join(E, r"script\missions\tongwar\relay\tongwar.lua")
s = rd(p)
if "HD_CFG" not in s:
    NL = nl(s)
    s = prepend_inc(s, "lich mua / gio tran chinh duoc")
    s = sub1(s, "TONGWAR_STATETIME = 2000",
             'TONGWAR_STATETIME = HD_CFG("TW_GIO_KHAICHIEN", 2000)', "statetime")
    s = sub1(s, "TONGWAR_ENDTIME = 2130",
             'TONGWAR_ENDTIME = HD_CFG("TW_GIO_KETTHUC", 2130)', "endtime")
    old = "261111},"
    assert s.count(old) == 1
    i = s.index(old) + len(old)
    j = s.index("}", i) + 1          # dau } dong bang SEASON_TB
    ins = (NL + "-- [HD CONFIG 24/08] de mua 9 tu config (doi ngay PHAI giu trung khoa ngay 29,30,31,1,2,3,5" + NL +
           "-- cua SCHEDULE_TABLE - xem chu thich trong cauhinh_hoatdong.lua)" + NL +
           "if (tbCHD and tbCHD.TW_MUA9) then" + NL +
           "\tTONGWAR_SEASON_TB[9] = tbCHD.TW_MUA9" + NL +
           "end")
    s = s[:j] + ins + s[j:]
    wr(p, s); print("relay/tongwar.lua OK")
else:
    print("relay/tongwar.lua da co")
mir(p, MTW, r"script\missions\tongwar\relay\tongwar.lua")

# ============ 2) event\tongwar\tongwar_signup.lua ============
p = os.path.join(E, r"script\event\tongwar\tongwar_signup.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "cap toi thieu chinh duoc")
    s = sub1(s, "if (GetLevel() < 90) then",
             'if (GetLevel() < HD_CFG("TW_CAP_TOITHIEU", 90)) then', "lv-signup")
    # text: ...c�p 90 tr�... -> noi chuoi dong
    b = s.encode("latin-1")
    olb = "p 90 tr".encode("latin-1")
    assert b.count(olb) == 1
    b = b.replace(olb, ('p "..HD_CFG("TW_CAP_TOITHIEU", 90).." tr').encode("latin-1"), 1)
    s = b.decode("latin-1")
    wr(p, s); print("tongwar_signup.lua OK")
else:
    print("tongwar_signup.lua da co")
mir(p, MTW, r"script\event\tongwar\tongwar_signup.lua")

# ============ 3) missions\tongwar\trap\tongwar_trap.lua ============
p = os.path.join(E, r"script\missions\tongwar\trap\tongwar_trap.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "cap + so mang chinh duoc")
    s = sub1(s, "if (GetLevel() < 90) then",
             'if (GetLevel() < HD_CFG("TW_CAP_TOITHIEU", 90)) then', "lv-trap")
    b = s.encode("latin-1")
    olb = "p 90 tr".encode("latin-1")
    assert b.count(olb) == 1
    b = b.replace(olb, ('p "..HD_CFG("TW_CAP_TOITHIEU", 90).." tr').encode("latin-1"), 1)
    s = b.decode("latin-1")
    s = sub1(s, "tongwar_setdata(TONGWAR_RLTASK_MAXDEATH, 10)",
             'tongwar_setdata(TONGWAR_RLTASK_MAXDEATH, HD_CFG("TW_SO_MANG", 10))', "maxdeath")
    wr(p, s); print("tongwar_trap.lua OK")
else:
    print("tongwar_trap.lua da co")
mir(p, MTW, r"script\missions\tongwar\trap\tongwar_trap.lua")

# ============ 4) missions\tongwar\head.lua ============
p = os.path.join(E, r"script\missions\tongwar\head.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "hang so tran chinh duoc (RESTART moi ap dung)")
    s = sub1(s, "TIMER_2 = 15* 6 * 60  * FRAME2TIME;",
             'TIMER_2 = HD_CFG("TW_PHUT_TRAN", 90) * 60 * FRAME2TIME;', "timer2")
    s = sub1(s, "RUNGAME_TIME = 30 * 60 * FRAME2TIME / TIMER_1;",
             'RUNGAME_TIME = HD_CFG("TW_PHUT_CHUANBI", 30) * 60 * FRAME2TIME / TIMER_1;', "rungame")
    s = sub1(s, "MAX_MEMBERCOUNT = 150",
             'MAX_MEMBERCOUNT = HD_CFG("TW_NGUOI_TOIDA", 150)', "maxmem")
    s = sub1(s, "MIN_MEMBERCOUNT = 5",
             'MIN_MEMBERCOUNT = HD_CFG("TW_NGUOI_TOITHIEU", 5)', "minmem")
    s = sub1(s, "TIME_PLAYER_STAY = 120",
             'TIME_PLAYER_STAY = HD_CFG("TW_GIAY_HAUDOANH", 120)', "stay")
    s = sub1(s, "BONUS_KILLPLAYER = 75",
             'BONUS_KILLPLAYER = HD_CFG("TW_DIEM_KILL", 75)', "killp")
    s = sub1(s, "BONUS_MAXSERIESKILL = 150",
             'BONUS_MAXSERIESKILL = HD_CFG("TW_DIEM_LIENTRAM", 150)', "series")
    wr(p, s); print("missions/tongwar/head.lua OK")
else:
    print("missions/tongwar/head.lua da co")
mir(p, MTW, r"script\missions\tongwar\head.lua")

# ============ 5) missions\bairenleitai\head.lua ============
p = os.path.join(E, r"script\missions\bairenleitai\head.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "nhip dau chinh duoc (RESTART moi ap dung)")
    s = sub1(s, "\tnApply = 30*18,",
             '\tnApply = HD_CFG("BR_GIAY_CHO_KHIEUCHIEN", 30)*18,', "napply")
    s = sub1(s, "\tnPrepare = 3*18,",
             '\tnPrepare = HD_CFG("BR_GIAY_DEM_NGUOC", 3)*18,', "nprepare")
    s = sub1(s, "\tnFight = 3*60*18,--3*60*18,",
             '\tnFight = HD_CFG("BR_PHUT_MOI_LUOT", 3)*60*18,--3*60*18,', "nfight")
    s = sub1(s, "HA_MAXSTAYTIME = 90*60",
             'HA_MAXSTAYTIME = HD_CFG("BR_PHUT_DUNG_YEN", 90)*60', "stay")
    s = sub1(s, "HA_DOUBLETIME = 30*60",
             'HA_DOUBLETIME = HD_CFG("BR_PHUT_BUFF_X2", 30)*60', "double")
    wr(p, s); print("bairenleitai/head.lua OK")
else:
    print("bairenleitai/head.lua da co")
mir(p, MBR, r"script\missions\bairenleitai\head.lua")

# ============ 6) missions\bairenleitai\hundred_arena.lua ============
p = os.path.join(E, r"script\missions\bairenleitai\hundred_arena.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "gio mo / exp / nguong chinh duoc (LIVE)")
    s = sub1(s, "if (ntime >= 1200) then",
             'if (ntime >= HD_CFG("BR_GIO_MO", 1200)) then', "giomo")
    s = sub1(s, "if (GetLevel() >= 90) then",
             'if (GetLevel() >= HD_CFG("BR_CAP_TOITHIEU", 90)) then', "lv")
    # text "C�p 90 v� 90 tr�" -> dong
    b = s.encode("latin-1")
    olb = "p 90 v".encode("latin-1")
    assert b.count(olb) == 1
    b = b.replace(olb, ('p "..HD_CFG("BR_CAP_TOITHIEU", 90).." v').encode("latin-1"), 1)
    s = b.decode("latin-1")
    b = s.encode("latin-1")
    olb = " 90 tr".encode("latin-1")
    assert b.count(olb) == 1, "van con %d ' 90 tr'" % b.count(olb)
    b = b.replace(olb, (' "..HD_CFG("BR_CAP_TOITHIEU", 90).." tr').encode("latin-1"), 1)
    s = b.decode("latin-1")
    # text gio mo "12:00 ~ 24:00" -> dong theo config
    s = sub1(s, "12:00 ~ 24:00",
             '"..HD_GioPhut(HD_CFG("BR_GIO_MO", 1200)).." ~ 24:00', "textgio")
    s = sub1(s, "if (nHour == 0) then",
             'if (nHour == HD_CFG("BR_GIO_DONG_H", 0)) then', "close")
    s = sub1(s, "if (nHour >= 0 and nHour <= 11) then",
             'if (nHour >= HD_CFG("BR_GIO_DONG_H", 0) and nHour < floor(HD_CFG("BR_GIO_MO", 1200)/100)) then', "notime")
    s = sub1(s, "if (nMin == 0 or mod(nMin, 5) == 0) then",
             'if (nMin == 0 or mod(nMin, HD_CFG("BR_PHUT_CHUKY_EXP", 5)) == 0) then', "tickexp")
    s = sub1(s, "local nExp = 1e6",
             'local nExp = HD_CFG("BR_EXP_TICK", 1000000)', "exp")
    s = sub1(s, "local nExpEx = 2e6;",
             'local nExpEx = HD_CFG("BR_EXP_LOICHU", 2000000);', "expex")
    old = "PlayerFunLib:GetTaskDailyCount(self.nTaskExpLimit) < 50"
    assert s.count(old) == 3, "tran ngay = %d cho" % s.count(old)
    s = s.replace(old, 'PlayerFunLib:GetTaskDailyCount(self.nTaskExpLimit) < HD_CFG("BR_TRAN_LUOT_NGAY", 50)')
    s = sub1(s, "if (nMin == 0 or mod(nMin, 30) == 0) then",
             'if (nMin == 0 or mod(nMin, HD_CFG("BR_PHUT_COTHU", 30)) == 0) then', "cothu")
    s = sub1(s, "self.nDoubleCount = floor(nPlayerCount*0.2)--0.2;",
             'self.nDoubleCount = floor(nPlayerCount * HD_CFG("BR_TILE_BUFFX2", 0.2))--0.2;', "tile")
    s = sub1(s, "tb_npc.nLevel = 90+nLevel;",
             'tb_npc.nLevel = HD_CFG("BR_CAP_NPC_GOC", 90) + nLevel;', "npclv")
    s = sub1(s, "if (n_last_grade < 100) then",
             'if (n_last_grade < HD_CFG("BR_TRAN_CHUOI", 100)) then', "chuoi")
    s = sub1(s, "self.Master.szPlayerName, 100 );",
             'self.Master.szPlayerName, HD_CFG("BR_TRAN_CHUOI", 100) );', "chuoitext")
    wr(p, s); print("hundred_arena.lua OK")
else:
    print("hundred_arena.lua da co")
mir(p, MBR, r"script\missions\bairenleitai\hundred_arena.lua")

# ============ 7) missions\bairenleitai\bairen_boot.lua ============
p = os.path.join(E, r"script\missions\bairenleitai\bairen_boot.lua")
s = rd(p)
if "HD_CFG" not in s:
    s = prepend_inc(s, "NPC loi vao chinh duoc")
    s = sub1(s, "nNpcId = 1747, nLevel = 95, nMapId = 176,",
             'nNpcId = HD_CFG("BR_NPC_ID", 1747), nLevel = HD_CFG("BR_NPC_LEVEL", 95), nMapId = HD_CFG("BR_NPC_MAP", 176),', "npc")
    wr(p, s); print("bairen_boot.lua OK")
else:
    print("bairen_boot.lua da co")
mir(p, MBR, r"script\missions\bairenleitai\bairen_boot.lua")

print("XONG PHAN 1 (tongwar + bairen)")
