# -*- coding: utf-8 -*-
r"""[BW 23/08] Thi cong Loi dai ty vo (missions/bw) theo SPEC_PORT_BW.md.
- 10 tep chep + va H1-H3, H5, H7-H10, H12 (H4 SetPunish / H6 GetTeamMember / H11 OnMissionTimer
  DA duoc engine JX2COMPAT xu ly - KHONG va script de khoi vá kep)
- 1 tep MOI bw_addnpc.lua (12 NPC Cong Binh Tu 3 thanh - engine bo NPC thoai map-data)
- H13 doi Hon Chien sang map 210; H14 missions.txt:12; H15 TimerTask 20/21; H16 startgame
"""
import io, os, re, shutil, sys

sys.path.insert(0, r"C:/Users/nguye/.claude/skills/swordonline-dev/scripts")
from vn_edit import vn

LINUX = r"D:\ServerLinux\server1"
E     = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR   = r"D:\GAMEDEVNEW\serverscript_jx2\bw"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    d = os.path.dirname(p)
    if d and not os.path.isdir(d): os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def put_both(rel, content):
    wr(os.path.join(E, rel), content); wr(os.path.join(MIR, rel), content)
def copy_both(src, rel):
    for root in (E, MIR):
        dst = os.path.join(root, rel); d = os.path.dirname(dst)
        if not os.path.isdir(d): os.makedirs(d)
        shutil.copyfile(src, dst)
def mirror_edit(rel):
    src = os.path.join(E, rel); dst = os.path.join(MIR, "jx1_edits", rel)
    d = os.path.dirname(dst)
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(src, dst)
def rep1(s, old, new, tag=""):
    assert s.count(old) == 1, "rep1 %s: %d" % (tag or old[:60], s.count(old))
    return s.replace(old, new, 1)

MARK = "-- [BW 23/08] "
SZ90 = vn("Phải đạt cấp 90 trở lên mới được tham gia Lôi đài thi đấu.")

# ============================================================
# 1) chep nguyen byte
# ============================================================
for f in ["bwmanager_chengdu.lua", "bwmanager_xiangyang.lua", "bwmission.lua", "bwtimer.lua",
          "bwtotaltimer.lua", "bwdeath.lua", "bwcamper.lua"]:
    copy_both(LINUX + r"\script\missions\bw" + "\\" + f, r"script\missions\bw" + "\\" + f)
print("chep nguyen: 7 tep")

# ============================================================
# 2) bwhead.lua: H1 (mission 11) + H2 (timer 20/21) + H3 (task 2340-2342) + H5 (LeaveGame) + H9 (Ly Tu)
# ============================================================
s = rd(LINUX + r"\script\missions\bw\bwhead.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "BW_MISSIONID = 4;", "BW_MISSIONID = 11;\t" + MARK + "khe mission: 4 cua JX1 = Phong Lang Do; 11 = mission_trong", "H1")
s = rep1(s, "BW_SMALLTIME_ID = 10;", "BW_SMALLTIME_ID = 20;\t" + MARK + "khe timer: 10 cua JX1 dang song (task10.lua)", "H2a")
s = rep1(s, "BW_TOTALTIME_ID = 11;", "BW_TOTALTIME_ID = 21;\t" + MARK, "H2b")
s = rep1(s, "BW_SIGNPOSWORLD = 300;", "BW_SIGNPOSWORLD = 2340;\t" + MARK + "300-302 cua JX1 = task luu/khoi phuc Tong Kim", "H3a")
s = rep1(s, "BW_SIGNPOSX = 301;", "BW_SIGNPOSX = 2341;", "H3b")
s = rep1(s, "BW_SIGNPOSY = 302;", "BW_SIGNPOSY = 2342;", "H3c")
s = rep1(s, "\t\tPlayerIndex = PTab[i];" + NL + "\t\tNewWorld(GetTask(BW_SIGNPOSWORLD), GetTask(BW_SIGNPOSX), GetTask(BW_SIGNPOSY));",
            "\t\tPlayerIndex = PTab[i];" + NL +
            "\t\tLeaveGame();\t" + MARK + "JX1 khong go mission khi doi map (C11) - giong citywar_arena/head.lua:75" + NL +
            "\t\tNewWorld(GetTask(BW_SIGNPOSWORLD), GetTask(BW_SIGNPOSX), GetTask(BW_SIGNPOSY));", "H5")
s = rep1(s, '{1608, 3211, "L?T?"},', '{1608, 3211, "' + vn("Lý Tư") + '"},\t' + MARK + 'ten hong san o ban VNG', "H9")
put_both(r"script\missions\bw\bwhead.lua", s)
print("bwhead.lua ok")

# ============================================================
# 3) bwmanager.lua: H7 (mo khoa) + H8 (cap 90) + H10 (prompt 30 byte)
# ============================================================
s = rd(LINUX + r"\script\missions\bw\bwmanager.lua")
NL = "\r\n" if "\r\n" in s else "\n"
# H7: xoa dong khoa
m = re.search(r'^do Talk\(1, "", "[^\r\n]*return end\r?\n', s, re.M)
assert m, "H7: khong thay dong khoa"
s = s[:m.start()] + MARK + "bo dong khoa 'Chuc nang da dong' (DIEUKIEN #1)" + NL + s[m.end():]
# H8a: OnRegister - sau kiem IsCaptain
s = rep1(s, "\tif (IsCaptain() ~= 1) then " + NL + "\t\tErrorMsg(5)" + NL + "\t\treturn" + NL + "\tend;",
            "\tif (IsCaptain() ~= 1) then " + NL + "\t\tErrorMsg(5)" + NL + "\t\treturn" + NL + "\tend;" + NL +
            NL +
            "\t" + MARK + "chinh sach cap 90 (DIEUKIEN #2)" + NL +
            "\tif (GetLevel() < 90) then" + NL +
            "\t\tSay(\"" + SZ90 + "\", 0)" + NL +
            "\t\treturn" + NL +
            "\tend;", "H8a")
# H8b: SignUpFinal - kiem CA 2 nguoi TRUOC khi dung SubWorld/OpenMission (hau phan bien F2:
# khoi MemberCount dong bang 'end' KHONG cham phay -> khong duoc neo '.*?end;')
old_mc = ("\tif (MemberCount <= 0 or MemberCount > 8) then" + NL +
          "\t\treturn" + NL +
          "\tend" + NL)
assert s.count(old_mc) == 1, "H8b: khoi MemberCount %d lan" % s.count(old_mc)
ins = (
    "\t" + MARK + "chinh sach cap 90 cho CA HAI doi truong (GetTeamMember JX2COMPAT: 1 = doi truong, 2 = thanh vien);" + NL +
    "\t-- dat TRUOC OpenMission de fail khong mo mission nua chung (phan bien F2)" + NL +
    "\tlocal nOldPlv = PlayerIndex;" + NL +
    "\tfor i = 1, 2 do" + NL +
    "\t\tPlayerIndex = GetTeamMember(i);" + NL +
    "\t\tif (PlayerIndex <= 0 or GetLevel() < 90) then" + NL +
    "\t\t\tPlayerIndex = nOldPlv;" + NL +
    "\t\t\tSay(\"" + SZ90 + "\", 0)" + NL +
    "\t\t\treturn" + NL +
    "\t\tend;" + NL +
    "\tend;" + NL +
    "\tPlayerIndex = nOldPlv;" + NL)
s = s.replace(old_mc, old_mc + NL + ins, 1)
# H8c: OnJoin - chan group ~= 3
s = rep1(s, "function OnJoin(group)" + NL + "\tidx = SubWorldID2Idx(BW_COMPETEMAP[1]);",
            "function OnJoin(group)" + NL +
            "\t" + MARK + "chinh sach cap 90 (khan gia group 3 khong chan)" + NL +
            "\tif (group ~= 3 and GetLevel() < 90) then" + NL +
            "\t\tSay(\"" + SZ90 + "\", 0)" + NL +
            "\t\treturn" + NL +
            "\tend;" + NL +
            "\tidx = SubWorldID2Idx(BW_COMPETEMAP[1]);", "H8c")
# H10: prompt <= 31 byte (S2C_INPUT_BOX.Value[32])
m = re.search(r'AskClientForNumber\("OnEnterKey1", 0, 10000, "([^"]+)"\);', s)
assert m, "H10: khong thay AskClientForNumber"
old_prompt = m.group(1)
assert old_prompt.startswith("Xin nh")
new_prompt = "N" + old_prompt[5:]        # "Xin nhap..." -> "Nhap..." (cat 4 byte, viet hoa N)
assert len(new_prompt) <= 31, "prompt %d byte" % len(new_prompt)
s = s.replace('AskClientForNumber("OnEnterKey1", 0, 10000, "%s");' % old_prompt,
              'AskClientForNumber("OnEnterKey1", 0, 10000, "%s");\t%sS2C_INPUT_BOX.Value[32] cat o 31 byte' % (new_prompt, MARK), 1)
put_both(r"script\missions\bw\bwmanager.lua", s)
print("bwmanager.lua ok (prompt %d byte)" % len(new_prompt))

# ============================================================
# 4) branch_bwsj.lua: H12 cat battlehead
# ============================================================
s = rd(LINUX + r"\script\task\newtask\branch\branch_bwsj.lua")
s = rep1(s, 'Include("\\\\script\\\\battles\\\\battlehead.lua")',
            '--Include("\\\\script\\\\battles\\\\battlehead.lua")\t' + MARK + 'chi phuc vu branchTask_JoinSJend1 (0 call site), keo 26 tep Tong Kim JX2', "H12")
put_both(r"script\task\newtask\branch\branch_bwsj.lua", s)
print("branch_bwsj.lua ok")

# ============================================================
# 5) bw_addnpc.lua MOI (12 Cong Binh Tu - bw_regionnpc.py, 2 cay trung byte)
# ============================================================
NLn = "\n"
SZ_CBT = vn("Công Bình Tử")
rows = [
    (80, 1659, 3020, "bwmanager.lua"), (80, 1852, 3049, "bwmanager.lua"),
    (80, 1627, 3208, "bwmanager.lua"), (80, 1709, 3251, "bwmanager.lua"),
    (78, 1464, 3183, "bwmanager_xiangyang.lua"), (78, 1705, 3235, "bwmanager_xiangyang.lua"),
    (78, 1474, 3272, "bwmanager_xiangyang.lua"), (78, 1577, 3376, "bwmanager_xiangyang.lua"),
    (11, 3071, 5002, "bwmanager_chengdu.lua"), (11, 3225, 5042, "bwmanager_chengdu.lua"),
    (11, 3040, 5096, "bwmanager_chengdu.lua"), (11, 3166, 5195, "bwmanager_chengdu.lua"),
]
lines = [
"-- [BW 23/08] 12 NPC Cong Binh Tu (tpl 309) doc tu region maps.pak (port_bw\\bw_regionnpc.py,",
"-- trung byte 2 cay) - engine ta bo NPC thoai trong map-data (NotAddNpcNormal=1) nen tu AddNpc.",
"-- Khuon: task\\tollgate\\tinsu_addnpc.lua",
"bw_dialognpc = {"]
for (mid, x, y, sc) in rows:
    lines.append("\t{309, %d, %d, %d, \"\\\\script\\\\missions\\\\bw\\\\%s\"}," % (mid, x, y, sc))
lines += [
"}",
"function bw_addnpc()",
"\tlocal nOld = SubWorld",
"\tfor i = 1, getn(bw_dialognpc) do",
"\t\tlocal t = bw_dialognpc[i]",
"\t\tlocal SId = SubWorldID2Idx(t[2])",
"\t\tif (SId >= 0) then",
"\t\t\tSubWorld = SId",
"\t\t\tlocal idx = AddNpc(t[1], 1, SId, t[3] * 32, t[4] * 32, 0, \"" + SZ_CBT + "\")",
"\t\t\tif (idx > 0) then",
"\t\t\t\tSetNpcScript(idx, t[5])",
"\t\t\tend",
"\t\t\tSubWorld = nOld",
"\t\tend",
"\tend",
"end",
""]
put_both(r"script\missions\bw\bw_addnpc.lua", NLn.join(lines))
print("bw_addnpc.lua ok")

# ============================================================
# 6) H13: Hon Chien doi sang map 210 (tep DU AN tren E)
# ============================================================
p = os.path.join(E, r"script\tinhnang\loidaihonchien\mainloidai.lua")
s = rd(p)
if "NewWorld(210, 1628 , 3213);" not in s:
    s = rep1(s, "NewWorld(209, 1628 , 3213);",
                "NewWorld(210, 1628 , 3213);\t" + MARK + "nhuong 209 cho Loi dai ty vo (bw); 210 cung map data, Tho Dia Phu da chan", "H13")
wr(p, s)
mirror_edit(r"script\tinhnang\loidaihonchien\mainloidai.lua")
print("mainloidai.lua -> map 210 ok")

# ============================================================
# 7) H14 missions.txt dong 12 (id 11) + H15 TimerTask 20/21 + H16 startgame
# ============================================================
p = os.path.join(E, r"settings\task\missions.txt")
s = rd(p)
NLm = "\r\n" if "\r\n" in s else "\n"
ls = s.split(NLm)
if ls[11] != "11\t\\script\\missions\\bw\\bwmission.lua":
    assert ls[11] == "11\t\\script\\missions\\mission_trong.lua", "missions.txt dong 12: %r" % ls[11]
    ls[11] = "11\t\\script\\missions\\bw\\bwmission.lua"
wr(p, NLm.join(ls))
mirror_edit(r"settings\task\missions.txt")

p = os.path.join(E, r"settings\TimerTask.txt")
s = rd(p)
NLt = "\r\n" if "\r\n" in s else "\n"
if not re.search(r"^20\t", s, re.M):
    if not s.endswith(NLt): s += NLt
    s += "20\t\\script\\missions\\bw\\bwtimer.lua" + NLt
    s += "21\t\\script\\missions\\bw\\bwtotaltimer.lua" + NLt
wr(p, s)
mirror_edit(r"settings\TimerTask.txt")

p = os.path.join(E, r"script\startgame.lua")
s = rd(p)
NLs = "\r\n" if "\r\n" in s else "\n"
if "bw_addnpc" in s:
    wr(p, s)
    mirror_edit(r"script\startgame.lua")
    print("settings + startgame ok (da co)")
    print("TAT CA XONG")
    sys.exit(0)
old = 'Include("\\\\script\\\\missions\\\\bairenleitai\\\\bairen_boot.lua")'
i = s.find(old); assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + 'Include("\\\\script\\\\missions\\\\bw\\\\bw_addnpc.lua")\t-- [BW 23/08] 12 NPC Cong Binh Tu (loi dai ty vo)' + s[j:]
old = "\ttongwar_addsignnpc()"
i = s.find(old); assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + "\tbw_addnpc()\t-- [BW 23/08]" + s[j:]
wr(p, s)
mirror_edit(r"script\startgame.lua")
print("settings + startgame ok")
print("TAT CA XONG")
