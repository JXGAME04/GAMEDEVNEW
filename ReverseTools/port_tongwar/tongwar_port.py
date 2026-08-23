# -*- coding: utf-8 -*-
r"""[TONGWAR 23/08] Thi cong Bang Chien / Vo Lam De Nhat Bang theo SPEC_PORT_TONGWAR.md.
- Chep 28 tep Linux (23 cay + give_support_item + relay + huangzhendan + BeiDou head + biggoldenseed)
- Va co chu dich (muc H, TRU cac va da duoc engine JX2COMPAT xu ly: H9 GetTeamMember,
  H11 AddSkillState rank, H13 GetGameTime*18, H14 OnPlayerDeath bridge - xem b97735d0)
- 3 tep MOI: tongwar_driver.lua / match\newworld.lua / npc_tongwar.lua
- Settings: missions.txt:34, TimerTask 61/62/54, signup_trap.txt, woods 15 tep (ten cp1252),
  MapList 605-607 NewWorldScript, gmscript.lua, startgame.lua, item_jx1.lua, magicscript.txt
- Mua 9: 261029-261111 (TRUNG khoa ngay 29,30,31,1,2,3,5 cua SCHEDULE_TABLE -> TAB_CALENDAR giu byte)
Chay lai an toan (idempotent): moi va deu assert truoc khi thay.
"""
import io, os, sys, shutil, re

sys.path.insert(0, r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts")
from vn_edit import vn  # unicode -> TCVN3 (latin-1 str)

LINUX = r"D:\ServerLinux\server1"
RELAY = r"D:\ServerLinux\gateway\s3relay"
E     = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR   = r"D:\GAMEDEVNEW\serverscript_jx2\tongwar"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    d = os.path.dirname(p)
    if d and not os.path.isdir(d): os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)

def put_both(rel, content):
    wr(os.path.join(E, rel), content)
    wr(os.path.join(MIR, rel), content)

def copy_both(src, rel):
    for root in (E, MIR):
        dst = os.path.join(root, rel)
        d = os.path.dirname(dst)
        if not os.path.isdir(d): os.makedirs(d)
        shutil.copyfile(src, dst)

def mirror_edit(rel):
    """chep trang thai SAU KHI SUA cua tep du an vao jx1_edits"""
    src = os.path.join(E, rel)
    dst = os.path.join(MIR, "jx1_edits", rel)
    d = os.path.dirname(dst)
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(src, dst)

def rep1(s, old, new, tag=""):
    assert s.count(old) == 1, "rep1 %s: %d lan" % (tag or old[:60], s.count(old))
    return s.replace(old, new, 1)

def repn(s, old, new, n, tag=""):
    assert s.count(old) == n, "repn %s: %d lan (can %d)" % (tag or old[:60], s.count(old), n)
    return s.replace(old, new)

MARK = "-- [TONGWAR 23/08] "

# ============================================================
# 1) CHEP NGUYEN BYTE (khong va)
# ============================================================
RAW = [
    (LINUX + r"\script\missions\tongwar\head.lua",              r"script\missions\tongwar\head.lua"),
    (LINUX + r"\script\missions\tongwar\tongwar_main.lua",      r"script\missions\tongwar\tongwar_main.lua"),
    (LINUX + r"\script\missions\tongwar\match\head.lua",        r"script\missions\tongwar\match\head.lua"),
    (LINUX + r"\script\missions\tongwar\match\playerdeath.lua", r"script\missions\tongwar\match\playerdeath.lua"),
    (LINUX + r"\script\missions\tongwar\match\smalltimer.lua",  r"script\missions\tongwar\match\smalltimer.lua"),
    (LINUX + r"\script\missions\tongwar\match\totaltimer.lua",  r"script\missions\tongwar\match\totaltimer.lua"),
    (LINUX + r"\script\missions\tongwar\match\hometrap1.lua",   r"script\missions\tongwar\match\hometrap1.lua"),
    (LINUX + r"\script\missions\tongwar\match\hometrap2.lua",   r"script\missions\tongwar\match\hometrap2.lua"),
    (LINUX + r"\script\missions\tongwar\match\homeouttrap1.lua",r"script\missions\tongwar\match\homeouttrap1.lua"),
    (LINUX + r"\script\missions\tongwar\match\homeouttrap2.lua",r"script\missions\tongwar\match\homeouttrap2.lua"),
    (LINUX + r"\script\missions\tongwar\npc\chefu.lua",         r"script\missions\tongwar\npc\chefu.lua"),
    (LINUX + r"\script\missions\tongwar\npc\openbox.lua",       r"script\missions\tongwar\npc\openbox.lua"),
    (LINUX + r"\script\event\tongwar\score.lua",                r"script\event\tongwar\score.lua"),
    (LINUX + r"\script\event\tongwar\event.lua",                r"script\event\tongwar\event.lua"),
    (LINUX + r"\script\event\tongwar\tongwar_gmscript.lua",     r"script\event\tongwar\tongwar_gmscript.lua"),
    (LINUX + r"\script\vng_event\give_support_item.lua",        r"script\vng_event\give_support_item.lua"),
    (LINUX + r"\script\event\BeiDouChuanGong\head.lua",         r"script\event\BeiDouChuanGong\head.lua"),
]
for src, rel in RAW:
    assert os.path.isfile(src), src
    copy_both(src, rel)
print("chep nguyen byte: %d tep" % len(RAW))

# ============================================================
# 2) CHEP + VA
# ============================================================

# --- 2.1 relay tongwar.lua: + mua 9 ---
s = rd(RELAY + r"\relaysetting\task\tongwar.lua")
assert "function tongwar_GetCurSeason" in s, "relay thieu tongwar_GetCurSeason"
NL = "\r\n" if "\r\n" in s else "\n"
old8 = "\t[8] = {140329, \t\t140329,\t\t140405,\t\t140406,\t\t140411}," + NL
s = rep1(s, old8, old8 +
    "\t--Season 9 - JX1 port 23/08/2026: TRUNG khoa ngay 29,30,31,1,2,3,5 cua SCHEDULE_TABLE (nhu mua 8)" + NL +
    "\t[9] = {261029, \t\t261029,\t\t261105,\t\t261106,\t\t261111}," + NL, "season9")
put_both(r"script\missions\tongwar\relay\tongwar.lua", s)
print("relay tongwar.lua + mua 9 ok")

# --- 2.2 missions\tongwar\tongwar_autoexec.lua: thay khoi ngay 2014 bang NPC loi vao ---
s = rd(LINUX + r"\script\missions\tongwar\tongwar_autoexec.lua")
NL = "\r\n" if "\r\n" in s else "\n"
i0 = s.find('\tlocal nDate = tonumber(GetLocalDate("%Y%m%d"));')
assert i0 > 0, "autoexec: khong thay khoi nDate"
i1 = s.rfind(NL + "end")
assert i1 > i0, "autoexec: khong thay end cuoi"
szNpcName = vn("Võ Lâm Truyền Nhân")
new_block = (
    "\t" + MARK + "goc: NPC \"Su gia Thien Ha De Nhat Bang\" chi hien 07-18/04/2014 (su kien hau mua" + NL +
    "\t-- VNG; npc_shizhe.lua giu nguyen trang). Loi vao thay the: NPC \"Vo Lam Truyen Nhan\" tpl 53" + NL +
    "\t-- (Linux KHONG co loi vao - tongWar_Start 0 call site), dat Ba Lang Huyen canh Su Gia Cong Thanh." + NL +
    "\tlocal sidx = SubWorldID2Idx(53);" + NL +
    "\tif (sidx >= 0) then" + NL +
    "\t\tSubWorld = sidx;" + NL +
    "\t\tlocal npcidx = AddNpc(53, 1, sidx, 1628*32, 3173*32, 1, \"" + szNpcName + "\");" + NL +
    "\t\tif (npcidx > 0) then" + NL +
    "\t\t\tSetNpcScript(npcidx, \"\\\\script\\\\event\\\\tongwar\\\\npc_tongwar.lua\");" + NL +
    "\t\tend;" + NL +
    "\t\tSubWorld = oldSubWorld;" + NL +
    "\tend;")
s = s[:i0] + new_block + s[i1:]
put_both(r"script\missions\tongwar\tongwar_autoexec.lua", s)
print("tongwar_autoexec.lua ok (NPC loi vao map 53)")

# --- 2.3 match\mission.lua: AddSkillState(661,5,0,0) -> RemoveSkillState(661) ---
s = rd(LINUX + r"\script\missions\tongwar\match\mission.lua")
m = re.search(r"\tAddSkillState\(661,5,0,0\)[^\r\n]*", s)
assert m, "mission.lua: khong thay AddSkillState 661"
s = s[:m.start()] + "\tRemoveSkillState(661)\t" + MARK + "JX1: AddSkillState(661,5,0,0) = buff VO HAN (nTime<=0 -> -1); y do goc Linux la go trang thai quan ham" + s[m.end():]
put_both(r"script\missions\tongwar\match\mission.lua", s)
print("match/mission.lua ok")

# --- 2.4 npc\doctor.lua: Sale(131,1) -> Sale(12,1) ---
s = rd(LINUX + r"\script\missions\tongwar\npc\doctor.lua")
s = rep1(s, "Sale(131, 1)", "Sale(12, 1) " + MARK + "buysell.txt du an 102 dong (khong co shop 131); 12 = Hieu thuoc Thanh Thi", "doctor")
put_both(r"script\missions\tongwar\npc\doctor.lua", s)
print("npc/doctor.lua ok")

# --- 2.5 trap\tongwar_trap.lua: bo khoi 2014 + trung sinh -> cap 90 ---
s = rd(LINUX + r"\script\missions\tongwar\trap\tongwar_trap.lua")
NL = "\r\n" if "\r\n" in s else "\n"
sz90 = vn("Phải đạt cấp 90 trở lên mới được vào đấu trường.")
# khoi 2014 (65-71): tu 'local nTongJoinTime' den 'end' cua no
blk2014 = re.search(r"\tlocal nTongJoinTime = GetJoinTongTime\(\)\*60\r?\n.*?\r?\n\tend\r?\n", s, re.S)
assert blk2014, "trap: khong thay khoi 2014"
s = s[:blk2014.start()] + "\t" + MARK + "bo dieu kien 'vao bang truoc 0h 29/03/2014' (DIEUKIEN #18 - moc mua VNG)" + NL + s[blk2014.end():]
blkTL = re.search(r"\tif ST_GetTransLifeCount\(\) < 4 then\r?\n.*?\r?\n\tend\r?\n", s, re.S)
assert blkTL, "trap: khong thay khoi trung sinh"
s = (s[:blkTL.start()] +
    "\t" + MARK + "chinh sach chu game: bo trung sinh, cong cap >= 90 (DIEUKIEN #20)" + NL +
    "\tif (GetLevel() < 90) then" + NL +
    "\t\tTalk(1, \"\", \"" + sz90 + "\")" + NL +
    "\t\treturn" + NL +
    "\tend" + NL + s[blkTL.end():])
put_both(r"script\missions\tongwar\trap\tongwar_trap.lua", s)
print("trap/tongwar_trap.lua ok")

# --- 2.6 event\tongwar\tongwar_signup.lua: 2014 + trung sinh + bang mat na ---
s = rd(LINUX + r"\script\event\tongwar\tongwar_signup.lua")
NL = "\r\n" if "\r\n" in s else "\n"
blk2014 = re.search(r"\tlocal nTongJoinTime = GetJoinTongTime\(\)\*60\r?\n.*?\r?\n\tend\r?\n", s, re.S)
assert blk2014, "signup: khong thay khoi 2014"
s = s[:blk2014.start()] + "\t" + MARK + "bo dieu kien 'vao bang truoc 0h 29/03/2014' (DIEUKIEN #18 - moc mua VNG)" + NL + s[blk2014.end():]
blkTL = re.search(r"\tif ST_GetTransLifeCount\(\) < 4 then\r?\n.*?\r?\n\tend\r?\n", s, re.S)
assert blkTL, "signup: khong thay khoi trung sinh"
s = (s[:blkTL.start()] +
    "\t" + MARK + "chinh sach chu game: bo trung sinh, cong cap >= 90 (DIEUKIEN #19)" + NL +
    "\tif (GetLevel() < 90) then" + NL +
    "\t\tTalk(1, \"\", \"" + sz90 + "\")" + NL +
    "\t\treturn" + NL +
    "\tend" + NL + s[blkTL.end():])
# bang mat na: id p Linux -> khoa "p_lv" du an (D.3)
old_mask = ('["482"] = 1,')
assert s.count(old_mask) == 1
mask_old = re.search(r"\tlocal tbForbidMaskInTongWar = \{.*?\}\r?\n", s, re.S)
assert mask_old, "signup: khong thay bang mat na"
mask_new = (
    "\t" + MARK + "id mat na Linux (p 482/447/450/446/647/806) -> du an khoa (particular, level)" + NL +
    "\tlocal tbForbidMaskInTongWar = {" + NL +
    "\t\t\t\t[\"48_3\"] = 1," + NL +
    "\t\t\t\t[\"44_8\"] = 1," + NL +
    "\t\t\t\t[\"44_7\"] = 1," + NL +
    "\t\t\t\t[\"45_1\"] = 1," + NL +
    "\t\t\t\t[\"64_8\"] = 1,\t\t" + NL +
    "\t\t\t\t[\"80_7\"] = 1,\t\t" + NL +
    "\t\t\t}" + NL)
s = s[:mask_old.start()] + mask_new + s[mask_old.end():]
s = rep1(s, "\t\tlocal nG, nD, nP = GetItemProp(tbEquip[i])" + NL + "\t\tif tbForbidMaskInTongWar[tostring(nP)] then",
            "\t\tlocal nG, nD, nP, nLv = GetItemProp(tbEquip[i])\t" + MARK + "GetItemProp JX1 tra (g,d,p,lv,...)" + NL +
            "\t\tif nG == 0 and nD == 11 and tbForbidMaskInTongWar[nP..\"_\"..nLv] then", "mask-check")
put_both(r"script\event\tongwar\tongwar_signup.lua", s)
print("event/tongwar_signup.lua ok")

# --- 2.7 event\tongwar\head.lua: Include treasure_head + 1500->1485 + ngay hien thi ---
s = rd(LINUX + r"\script\event\tongwar\head.lua")
s = rep1(s, 'Include("\\\\script\\\\task\\\\random\\\\treasure_head.lua");',
            '--Include("\\\\script\\\\task\\\\random\\\\treasure_head.lua");\t' + MARK + '0/21 ky hieu duoc dung; task\\random khong port (tien le Tin Su)', "treasure")
s = rep1(s, "GetSkillState(1500)", "GetSkillState(1485)", "skill1500a")
s = rep1(s, "RemoveSkillState(1500)", "RemoveSkillState(1485)", "skill1500b")
s = rep1(s, "04/04/2014", "04/11/2026", "detail-date")
put_both(r"script\event\tongwar\head.lua", s)
print("event/head.lua ok")

# --- 2.8 event\tongwar\headinfo.lua: item id + ngay lich ---
s = rd(LINUX + r"\script\event\tongwar\headinfo.lua")
s = rep1(s, "tbProp={6,1,30438 ,1}", "tbProp={6,1,4864 ,1}", "qua-dai-hk")       # 30438 -> 4864 (item moi)
s = rep1(s, "tbProp={6,1,2264,1}", "tbProp={6,1,2273,1}", "hoang-chan-don")     # 2264 -> 2273
s = repn(s, "tbProp={0,4862}", "tbProp={0,4491}", 2, "cuong-lan")               # ca dong comment lan dong that
s = rep1(s, "tbProp={6,1,30301,1}", "tbProp={6,1,4857,1}", "hon-nguyen")        # 30301 -> 4857 (Tin Su)
s = rep1(s, "tbProp={0,3477}", "tbProp={0,3476}", "phi-phong")                  # 3477 -> 3476
for old, new in (('"29/03"', '"29/10"'), ('"30/03"', '"30/10"'), ('"31/03"', '"31/10"'),
                 ('"01/04"', '"01/11"'), ('"02/04"', '"02/11"'), ('"03/04"', '"03/11"'), ('"05/04"', '"05/11"')):
    s = rep1(s, old, new, "lich" + old)
put_both(r"script\event\tongwar\headinfo.lua", s)
print("event/headinfo.lua ok")

# --- 2.9 event\tongwar\npc_shizhe.lua: bo GetJoinTongTime + 1500->1485 ---
s = rd(LINUX + r"\script\event\tongwar\npc_shizhe.lua")
s = rep1(s, " or n_titletype == 1 or GetJoinTongTime() < 10080) then",
            " or n_titletype == 1) then\t" + MARK + "bo 'vao bang >= 7 ngay' (DIEUKIEN #21)", "jointime")
s = rep1(s, "GetSkillState(1500)", "GetSkillState(1485)", "skill1500a")
s = rep1(s, "RemoveSkillState(1500)", "RemoveSkillState(1485)", "skill1500b")
put_both(r"script\event\tongwar\npc_shizhe.lua", s)
print("event/npc_shizhe.lua ok")

# --- 2.10 item\huangzhendan.lua: bo 3 Include + AddSkillState p3 1 ---
s = rd(LINUX + r"\script\item\huangzhendan.lua")
for inc in ('Include("\\\\script\\\\lib\\\\pay.lua");',
            'Include("\\\\script\\\\task\\\\task_addplayerexp.lua");',
            'Include("\\\\script\\\\event\\\\BeiDouChuanGong\\\\lib\\\\addaward.lua");'):
    s = rep1(s, inc, "--" + inc + "\t" + MARK + "tep khong co tren JX1, khong dung", "inc")
s = rep1(s, "AddSkillState( 509, 1, 0, 180);",
            "AddSkillState( 509, 1, 1, 180);\t" + MARK + "JX1: p3=1 ap skill that (Linux luon ap that)", "skill509")
put_both(r"script\item\huangzhendan.lua", s)
print("item/huangzhendan.lua ok")

# --- 2.11 vng_event\item\biggoldenseed.lua: cap 90 + 2 task global + bo han su dung ---
s = rd(LINUX + r"\script\vng_event\item\biggoldenseed.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "TSK_DAILY_LIMIT = 3062",
            "TSK_DAILY_LIMIT = 3062" + NL +
            MARK + "2 global Linux nam o item\\goldenseed.lua (khong port) - dinh nghia tai cho" + NL +
            "TASKIDDAY = 2321" + NL + "TASKEATCOUNT = 2322", "taskids")
s = rep1(s, "if (GetLevel() < 120) then", "if (GetLevel() < 90) then\t" + MARK + "chinh sach cap 90", "level")
blk = re.search(r"\tif \(nItemdate == 0\) then\r?\n\t\tif \(ITEM_GetExpiredTime\(ItemIndex\) <= 0\) then\r?\n\t\t\treturn -1\r?\n\t\tend\r?\n\t\treturn 1;\r?\n\tend", s)
assert blk, "biggoldenseed: khong thay khoi ExpiredTime"
s = (s[:blk.start()] +
    "\tif (nItemdate == 0) then" + NL +
    "\t\t" + MARK + "khong dat han (ITEM_GetExpiredTime khong co tren JX1) -> luon hop le (lech co chu dich, nhu 'GetItemLife' Dot E)" + NL +
    "\t\treturn 1;" + NL +
    "\tend" + s[blk.end():])
put_both(r"script\vng_event\item\biggoldenseed.lua", s)
print("vng_event/item/biggoldenseed.lua ok")

# ============================================================
# 3) 3 TEP MOI
# ============================================================
NL = "\n"
driver = NL.join([
"-- ============================================================",
"-- TONGWAR DRIVER " + MARK.strip() + " - thay task centre relay cho Bang Chien / Vo Lam De Nhat Bang",
"-- (khuon y het \\script\\missions\\tong\\tong_driver.lua cua dot Hoat dong Phuong 21/08).",
"-- Relay Linux: tasklist.ini [Task_64] tongwar.lua - TaskShedule() luc boot roi TaskContent()",
"-- moi 30 phut; cua so +-5 phut cua 20:00/21:30 chi khop tick :00/:30 -> tick 15 phut tuong duong.",
"-- ============================================================",
"",
"TONGWAR_DRV_MSKEY   = 29\t-- khoa missionId rieng cho GlbTimer (27 = WLLS, 28 = TONG)",
"TONGWAR_DRV_TIMERID = 54\t-- settings\\TimerTask.txt khoa 54 tro nguoc ve tep nay",
"TONGWAR_DRV_LASTQ   = -1",
"TONGWAR_DRV_RELAY   = \"\\\\script\\\\missions\\\\tongwar\\\\relay\\\\tongwar.lua\"",
"",
"function TONGWAR_Drv_CurQ()",
"\treturn tonumber(date(\"%y%m%d%H\")) * 4 + floor(tonumber(date(\"%M\")) / 15)",
"end",
"",
"function TONGWAR_DriverInit(szParam)",
"\tDynamicExecute(TONGWAR_DRV_RELAY, \"TaskShedule\")",
"\tTONGWAR_DRV_LASTQ = TONGWAR_Drv_CurQ()",
"\tStartGlbMSTimer(TONGWAR_DRV_MSKEY, TONGWAR_DRV_TIMERID, 30 * 18)",
"\tOutputMsg(\"[TONGWAR] Driver Bang Chien khoi dong (quy \"..TONGWAR_DRV_LASTQ..\")\")",
"\treturn 1",
"end",
"",
"function OnTimer()",
"\tlocal q = TONGWAR_Drv_CurQ()",
"\tif (q ~= TONGWAR_DRV_LASTQ) then",
"\t\tTONGWAR_DRV_LASTQ = q",
"\t\tDynamicExecute(TONGWAR_DRV_RELAY, \"TaskContent\")",
"\tend",
"end",
"",
"-- ====== Lenh bai Admin: ep chay ngay TaskContent / khoi tao mua de test ======",
"function TONGWAR_Adm_TaskContent()",
"\tDynamicExecute(TONGWAR_DRV_RELAY, \"TaskContent\")",
"\treturn 1",
"end",
"function TONGWAR_Adm_InitMatch(nSeason)",
"\tDynamicExecute(TONGWAR_DRV_RELAY, \"tongwar_initmatch\", nSeason or 9)",
"\treturn 1",
"end",
""])
put_both(r"script\missions\tongwar\tongwar_driver.lua", driver)

newworld = NL.join([
"-- " + MARK.strip() + " map chien truong 605-607: KNpc::ChangeWorld JX1 KHONG go nguoi choi khoi mission",
"-- khi doi map (m_MissionArray.RemovePlayer bi chu thich, chi go luc logout) -> OnLeave cua",
"-- mission.lua khong bao gio chay khi bi day ra / dung Tho Dia Phu; tu go o day.",
"-- Khuon: missions\\leaguematch\\combat\\newworld.lua",
"Include(\"\\\\script\\\\maps\\\\newworldscript_default.lua\")",
"",
"function OnNewWorld(szParam)",
"\tOnNewWorldDefault(szParam)",
"end",
"",
"function OnLeaveWorld(szParam)",
"\tDelMSPlayer(33, 0)\t-- mission 33 Bang Chien",
"\tOnLeaveWorldDefault(szParam)",
"end",
""])
put_both(r"script\missions\tongwar\match\newworld.lua", newworld)

npc_entry = NL.join([
"-- " + MARK.strip() + " NPC loi vao \"Vo Lam Truyen Nhan\" (MOI - ban Linux KHONG co loi vao:",
"-- tongWar_Start 0 call site, DIEUKIEN da chung minh). Moi muc con theo pha 1/2/3 da nam san",
"-- trong tongWar_Start (event\\tongwar\\head.lua:169-207).",
"Include(\"\\\\script\\\\event\\\\tongwar\\\\head.lua\")",
"Include(\"\\\\script\\\\event\\\\tongwar\\\\npc_shizhe.lua\")",
"",
"function main()",
"\ttongWar_Start()",
"end",
""])
put_both(r"script\event\tongwar\npc_tongwar.lua", npc_entry)
print("3 tep moi ok")

# ============================================================
# 4) SETTINGS
# ============================================================

# 4.1 signup_trap.txt
copy_both(LINUX + r"\settings\maps\tongwar\signup_trap.txt", r"settings\maps\tongwar\signup_trap.txt")

# 4.2 woods: 15 tep, nguon = ten cp437 giai duoc GBK; dich = byte GBK giai cp1252
wsrc = LINUX + r"\settings\battles\maps\woods"
count = 0
seen = set()
for fn in os.listdir(wsrc):
    if fn == "mapinfo.txt":
        copy_both(os.path.join(wsrc, fn), r"settings\battles\maps\woods\mapinfo.txt")
        count += 1
        continue
    try:
        gbk_bytes = fn.encode("cp437")
        han = gbk_bytes.decode("gbk")  # phai giai duoc GBK -> ung vien
    except (UnicodeEncodeError, UnicodeDecodeError):
        continue
    # bien the chuan: giai GBK ra dung bo chu ten goc (loai mojibake kep van ra CJK rac)
    HAN_OK = set(u"大营后野外帅旗")  # dai/doanh/hau/da/ngoai/soai/ky
    if not all(ord(c) < 128 or c in HAN_OK for c in han):
        continue
    dest_name = gbk_bytes.decode("cp1252", "strict")
    assert dest_name not in seen
    seen.add(dest_name)
    rel = os.path.join(r"settings\battles\maps\woods", dest_name)
    copy_both(os.path.join(wsrc, fn), rel)
    count += 1
assert count == 15, "woods: %d tep (can 15)" % count
print("woods: 15 tep ok")

# 4.3 missions.txt dong 34 (id 33)
p = os.path.join(E, r"settings\task\missions.txt")
s = rd(p)
NLm = "\r\n" if "\r\n" in s else "\n"
ls = s.split(NLm)
assert ls[33] == "33\t\\script\\missions\\mission_trong.lua", "missions.txt dong 34: %r" % ls[33]
ls[33] = "33\t\\script\\missions\\tongwar\\match\\mission.lua"
wr(p, NLm.join(ls))
mirror_edit(r"settings\task\missions.txt")

# 4.4 TimerTask.txt + 61/62/54
p = os.path.join(E, r"settings\TimerTask.txt")
s = rd(p)
NLt = "\r\n" if "\r\n" in s else "\n"
for k in ("54", "61", "62"):
    assert not re.search(r"^%s\t" % k, s, re.M), "TimerTask da co khoa " + k
if not s.endswith(NLt): s += NLt
s += "61\t\\script\\missions\\tongwar\\match\\smalltimer.lua" + NLt
s += "62\t\\script\\missions\\tongwar\\match\\totaltimer.lua" + NLt
s += "54\t\\script\\missions\\tongwar\\tongwar_driver.lua" + NLt
wr(p, s)
mirror_edit(r"settings\TimerTask.txt")

# 4.5 MapList.ini: 605-607 NewWorldScript
p = os.path.join(E, r"settings\MapList.ini")
s = rd(p)
for mid in ("605", "606", "607"):
    s = rep1(s, mid + "_NewWorldScript=\\script\\maps\\newworldscript.lua",
                mid + "_NewWorldScript=\\script\\missions\\tongwar\\match\\newworld.lua", "maplist" + mid)
wr(p, s)
mirror_edit(r"settings\MapList.ini")

# 4.6 gmscript.lua + Include tongwar_gmscript
p = os.path.join(E, r"script\gmscript.lua")
s = rd(p)
NLg = "\r\n" if "\r\n" in s else "\n"
old = 'Include("\\\\script\\\\missions\\\\leaguematch\\\\wlls_gmscript.lua");'
s = rep1(s, old, old + NLg + 'Include("\\\\script\\\\event\\\\tongwar\\\\tongwar_gmscript.lua");\t' + MARK + 'dw tongwar_start/tongwar_gw_say ha canh state nay (Linux gmscript.lua:9)', "gmscript")
wr(p, s)
mirror_edit(r"script\gmscript.lua")

# 4.7 startgame.lua: Include + 2 loi goi
p = os.path.join(E, r"script\startgame.lua")
s = rd(p)
NLs = "\r\n" if "\r\n" in s else "\n"
old = 'Include("\\\\script\\\\task\\\\tollgate\\\\tinsu_addnpc.lua")'
i = s.find(old)
assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + 'Include("\\\\script\\\\missions\\\\tongwar\\\\tongwar_autoexec.lua")\t' + MARK + 'NPC/trap 6 map bao danh + NPC loi vao Bang Chien' + s[j:]
old = "\ttinsu_addnpc()"
i = s.find(old)
assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + "\ttongwar_addsignnpc()\t" + MARK + "Bang Chien" + s[j:]
old = '\tDynamicExecute("\\\\script\\\\missions\\\\tong\\\\tong_driver.lua", "TONG_DriverInit")'
i = s.find(old)
assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + '\tDynamicExecute("\\\\script\\\\missions\\\\tongwar\\\\tongwar_driver.lua", "TONGWAR_DriverInit")\t' + MARK + 'lich mua Bang Chien (thay relay)' + s[j:]
wr(p, s)
mirror_edit(r"script\startgame.lua")

# 4.8 lib\awardtype\item_jx1.lua: nhanh goldequip AddItem2
p = os.path.join(E, r"script\lib\awardtype\item_jx1.lua")
s = rd(p)
NLa = "\r\n" if "\r\n" in s else "\n"
old = ("\tfor i = 1, nAmount do" + NLa +
    "\t\tlocal nItemIdx = AddItem(tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0," + NLa +
    "\t\t\ttbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0, 0)" + NLa)
new = ("\tfor i = 1, nAmount do" + NLa +
    "\t\tlocal nItemIdx" + NLa +
    "\t\t" + MARK + "goldequip (nQuality == 1, tbProp = {0, record goldequip.txt}): AddItem2" + NLa +
    "\t\t-- NATURE_GOLD nhu event\\30thang4\\EventLib.lua:176; nExpiredTime bo qua (lech co chu dich)" + NLa +
    "\t\tif tbItem.nQuality == 1 and (tbProp[1] or 0) == 0 then" + NLa +
    "\t\t\tnItemIdx = AddItem2(2, 0, tbProp[2] or 0, 0, 0, 0)" + NLa +
    "\t\telse" + NLa +
    "\t\t\tnItemIdx = AddItem(tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0," + NLa +
    "\t\t\t\ttbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0, 0)" + NLa +
    "\t\tend" + NLa)
s = rep1(s, old, new, "item_jx1-give")
wr(p, s)
mirror_edit(r"script\lib\awardtype\item_jx1.lua")

# 4.9 magicscript.txt: 2273 gan script + them dong 4864
p = os.path.join(E, r"settings\item\magicscript.txt")
s = rd(p)
NLi = "\r\n" if "\r\n" in s else "\n"
ls = s.split(NLi)
# dong 2275 (index 2274) = id 2273 Hoang Chan Don
parts = ls[2274].split("\t")
assert parts[3] == "2273", "magicscript dong 2275: id %r" % parts[3]
assert parts[9] == "0", "magicscript 2273 cot 10: %r" % parts[9]
parts[9] = "\\script\\item\\huangzhendan.lua"
ls[2274] = "\t".join(parts)
# dong moi 4866 = id 4864 Qua Dai Hoang Kim (ten + spr + desc byte tu Linux 30438)
lx = rd(LINUX + r"\settings\item\004\magicscript.txt").split("\n")
src = None
for l in lx:
    if "\t30438\t" in l:
        src = l.rstrip("\r").split("\t"); break
assert src, "Linux magicscript khong thay 30438"
new_line = "\t".join([src[0], "6", "1", "4864", src[4], src[5], "1", "1", src[8],
                      "\\script\\vng_event\\item\\biggoldenseed.lua", "0", "1", "0", "0", ""])
assert len(ls) in (4865, 4866), "magicscript %d dong" % len(ls)
if ls[-1] == "":
    ls.insert(len(ls) - 1, new_line)
else:
    ls.append(new_line)
    ls.append("")  # ket thuc bang newline
wr(p, NLi.join(ls))
mirror_edit(r"settings\item\magicscript.txt")
print("settings ok")

# ============================================================
# 5) KIEM TRA PHU
# ============================================================
# map 53 = Ba Lang Huyen?
s = rd(os.path.join(E, r"settings\MapList.ini"))
m = re.search(r"^53_name=([^\r\n]*)", s, re.M)
print("map 53 =", repr(m.group(1)) if m else "???")
# task 2321/2322 va cham? (quet .lua toan cay, bao cao thoi - biggoldenseed dung lam mo dem ngay)
hits = []
for root, dirs, files in os.walk(os.path.join(E, "script")):
    for f in files:
        if not f.endswith(".lua"): continue
        fp = os.path.join(root, f)
        try:
            t = rd(fp)
        except Exception:
            continue
        if "Task(2321" in t or "Task(2322" in t:
            if "biggoldenseed" not in fp:
                hits.append(fp)
print("task 2321/2322 dung o ngoai biggoldenseed:", hits[:5] if hits else "KHONG")
print("TAT CA XONG")
