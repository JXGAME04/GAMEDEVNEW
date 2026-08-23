# -*- coding: utf-8 -*-
r"""[BAIREN 23/08] Thi cong Bach Nhan Loi Dai theo SPEC_PORT_BAIRENLEITAI.md.
- 12 tep chep (9 nguyen byte + 3 DIFF: npc_death / player_death / npc_enter) + hundred_arena va H + cau noi A.4
- 3 tep MOI (P1): bairen_boot.lua / npc_quan_jx1.lua / newworld.lua (chuoi TCVN3 trich tu nguon Linux)
- Settings: 11 tep bairenleitai + maplist 960_NewWorldScript + startgame.lua
Engine C.1-C.7 da vao CoreServer (port5_engine_patch.py).
"""
import io, os, re, shutil, sys

LINUX = r"D:\ServerLinux\server1"
E     = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR   = r"D:\GAMEDEVNEW\serverscript_jx2\bairenleitai"

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

MARK = "-- [JX1 23/08] "
HA = "\\\\script\\\\missions\\\\bairenleitai\\\\hundred_arena.lua"

# ============================================================
# 1) chep nguyen byte
# ============================================================
RAW = ["head.lua", "npc_beauty.lua", "npc_chuwuxiang.lua", "npc_store.lua", "npc_trans.lua", "trap_arena.lua"]
for f in RAW:
    copy_both(LINUX + r"\script\missions\bairenleitai" + "\\" + f, r"script\missions\bairenleitai" + "\\" + f)
copy_both(LINUX + r"\script\missions\basemission\lib.lua", r"script\missions\basemission\lib.lua")
copy_both(LINUX + r"\script\event\msg2allworld.lua", r"script\event\msg2allworld.lua")
print("chep nguyen: %d tep" % (len(RAW) + 2))

# ============================================================
# 2) hundred_arena.lua: H (cap 90) + cau noi A.4
# ============================================================
s = rd(LINUX + r"\script\missions\bairenleitai\hundred_arena.lua")
NL = "\r\n" if "\r\n" in s else "\n"
s = rep1(s, "if (GetLevel() >= 120) then", "if (GetLevel() >= 90) then\t" + MARK + "chinh sach cap 90", "level120")
m = re.search(r'^\t{3}Talk\(1, "", "<color=yellow>C[^\r\n]*', s, re.M)
assert m, "khong thay dong Talk cap 120"
line = m.group(0)
assert line.count("120") == 2
s = s[:m.start()] + line.replace("120", "90") + s[m.end():]
bridge = NL.join([
"",
"-- ===== " + MARK.strip() + " cau noi 1 state/tep: npc_death.lua & player_death.lua goi sang state nay",
"-- qua DynamicExecuteByPlayer (Linux 1 state nen doc truc tiep HundredArena.*) =====",
"function HundredArena:JX1_NpcDeath(nNpcIndex)",
"\tlocal nArenaID = ArenaNpc:GetNpcArena(nNpcIndex);",
"\tif (nArenaID and self.tbArenaList[nArenaID]) then",
"\t\tself.tbArenaList[nArenaID]:NpcDeath();",
"\tend",
"end",
"function HundredArena:JX1_PlayerDeath(nType)",
"\tlocal tb = self.tbPlayerList[GetName()];",
"\tif (not tb or not tb.nArenaId or tb.nArenaId == 0 or not self.tbArenaList[tb.nArenaId]) then",
"\t\treturn",
"\tend",
"\tif (nType == 0) then",
"\t\tif (tb.nRight == 1) then",
"\t\t\tnType = 2",
"\t\telse",
"\t\t\tnType = 1",
"\t\tend",
"\tend",
"\tself.tbArenaList[tb.nArenaId]:PlayerDeath(nType);",
"end",
""])
if not s.endswith(NL): s += NL
s += bridge
put_both(r"script\missions\bairenleitai\hundred_arena.lua", s)
print("hundred_arena.lua ok")

# ============================================================
# 3) npc_death.lua (DIFF)
# ============================================================
s = rd(LINUX + r"\script\missions\bairenleitai\npc_death.lua")
NL = "\r\n" if "\r\n" in s else "\n"
old = ("\tlocal nArenaID = ArenaNpc:GetNpcArena(nNpcIndex);" + NL +
       "\tif (HundredArena.tbArenaList[nArenaID]) then" + NL +
       "\t\tHundredArena.tbArenaList[nArenaID]:NpcDeath();" + NL +
       "\tend" + NL)
new = ("\t" + MARK + "HundredArena song trong state cua hundred_arena.lua (engine ta 1 state/tep)" + NL +
       "\tDynamicExecuteByPlayer(PlayerIndex, \"" + HA + "\", \"HundredArena:JX1_NpcDeath\", nNpcIndex)" + NL)
s = rep1(s, old, new, "npc_death")
put_both(r"script\missions\bairenleitai\npc_death.lua", s)
print("npc_death.lua ok")

# ============================================================
# 4) player_death.lua (DIFF)
# ============================================================
s = rd(LINUX + r"\script\missions\bairenleitai\player_death.lua")
NL = "\r\n" if "\r\n" in s else "\n"
# 4a. wrapper OnPlayerDeath (truoc OnDeath; engine cung co fallback - giu ca hai nhu citywar_arena)
s = rep1(s, "function OnDeath(Launcher)",
    MARK + "engine JX1 goi OnPlayerDeath(nVictimPIdx, nLauncherNpcIdx) (KNpc.cpp) - Linux goi OnDeath(Launcher)" + NL +
    "function OnPlayerDeath(nVictimPIdx, Launcher)" + NL +
    "\tOnDeath(Launcher)" + NL +
    "end" + NL + NL +
    "function OnDeath(Launcher)", "wrapper")
# 4b. khoi tra cuu xuyen state (17-36) -> DynamicExecuteByPlayer
i0 = s.find("\tlocal szKilledName = CallPlayerFunction(nKilledIndex, GetName);")
assert i0 > 0
i1 = s.find("\t--Modified by DinhHQ - 20110714")
assert i1 > i0
new_mid = (
    "\t" + MARK + "tbPlayerList/tbArenaList song trong state hundred_arena.lua -> goi sang state do;" + NL +
    "\t-- nType 3 = bi NPC giet, 0 = bi nguoi giet (JX1_PlayerDeath tu suy 1|2 theo nRight cua nan nhan)" + NL +
    "\tSetCurCamp(GetCamp());" + NL +
    "\t" + NL +
    "\tlocal nKillerIndex = NpcIdx2PIdx(Launcher);" + NL +
    "\tlocal szHA = \"" + HA + "\";" + NL +
    "\t" + NL +
    "\tif (nKillerIndex <= 0) then" + NL +
    "\t\tprint(Launcher, GetNpcName(Launcher));" + NL +
    "\t\tDelNpc(Launcher);" + NL +
    "\t\tDynamicExecuteByPlayer(nKilledIndex, szHA, \"HundredArena:JX1_PlayerDeath\", 3);" + NL +
    "\telse" + NL +
    "\t\tDynamicExecuteByPlayer(nKilledIndex, szHA, \"HundredArena:JX1_PlayerDeath\", 0);" + NL +
    "\tend" + NL +
    "\t" + NL)
s = s[:i0] + new_mid + s[i1:]
# 4c. khoi self:DelPlayer + NewWorld (49-53) -> giu dung hanh vi that cua Linux (nem loi, khong chay)
i0 = s.find("\tself:DelPlayer(PlayerIndex)")
assert i0 > 0
i1 = s.find("\tNewWorld(nMapId, nX, nY);")
assert i1 > i0
i1 = s.find(NL, i1) + len(NL)
new_tail = (
    "\t" + MARK + "goc :49-53 `self:DelPlayer(PlayerIndex)` + NewWorld: `self` la GLOBAL CHUA GAN ->" + NL +
    "\t-- tren Linux NEM LOI runtime tai day, NewWorld khong bao gio chay; hanh vi thuc te = hoi sinh" + NL +
    "\t-- tai diem dang nhap (SetRevPos o tren), roi map 960 -> OnLeaveMap don. Giu DUNG hanh vi do." + NL)
s = s[:i0] + new_tail + s[i1:]
put_both(r"script\missions\bairenleitai\player_death.lua", s)
print("player_death.lua ok")

# ============================================================
# 5) npc_enter.lua (DIFF 1 dong)
# ============================================================
s = rd(LINUX + r"\script\missions\bairenleitai\npc_enter.lua")
s = rep1(s, 'Include("\\\\script\\\\activitysys\\\\npcdailog.lua")',
            'Include("\\\\script\\\\dailogsys\\\\dailogsay.lua")\t' + MARK + 'npcdailog keo 18 tep bonusvlmc/vng khong port; chi can CreateNewSayEx', "include")
put_both(r"script\missions\bairenleitai\npc_enter.lua", s)
print("npc_enter.lua ok")

# ============================================================
# 6) 3 tep MOI (P1) - chuoi TCVN3 trich tu nguon Linux
# ============================================================
src_np = rd(LINUX + r"\script\missions\zhaojingling\enternpc.lua")
m = re.search(r'szName = "([^"]+)"', src_np)
SZ_QUANNAME = m.group(1)                        # "Quan nhac nho Hoang Thanh Tu" (TCVN3)
m = re.search(r'szTitleMsg = "([^"]+)"', src_np)
SZ_TITLE = m.group(1)                           # cau gioi thieu dai (TCVN3)
src_h9 = rd(LINUX + r"\script\activitysys\config\9\head.lua")
m = re.search(r'szName = "([^"]+)"', src_h9)
SZ_LOIDAI = m.group(1)                          # "Loi Dai Hoang Thanh Tu" (TCVN3)

NL = "\n"
boot = NL.join([
"-- [BAIREN 23/08] Thay activitysys\\config\\9 (ServerStart) + zhaojingling\\enternpc.lua (dat NPC).",
"-- Engine ta moi tep mot lua_State nen G_ACTIVITY khong dang ky duoc nhu Linux (SPEC 1.2);",
"-- Linux con XOA NPC nay luc boot (autoexec.lua:214 ClearNPCNewVersion) - chu game quyet dat lai.",
"Include(\"\\\\script\\\\missions\\\\basemission\\\\lib.lua\")\t-- basemission_CallNpc",
"BAIREN_HA = \"" + HA + "\"",
"",
"function BairenLeitai_Init()",
"\t-- = config\\9\\config.lua:12-16: Include hundred_arena.lua roi HundredArena.IniEnvironment(HundredArena)",
"\tDynamicExecute(BAIREN_HA, \"HundredArena:IniEnvironment\")",
"\t-- = zhaojingling\\enternpc.lua:7-15 (toa do doc tu settings\\maps\\chrismas\\enternpc.txt)",
"\tlocal szPos = \"\\\\settings\\\\maps\\\\chrismas\\\\enternpc.txt\"",
"\tif (TabFile_Load(szPos, szPos) == 0) then",
"\t\tOutputMsg(\"[BAIREN] thieu \"..szPos)",
"\t\treturn 0",
"\tend",
"\tbasemission_CallNpc({",
"\t\tszName = \"" + SZ_QUANNAME + "\", nNpcId = 1747, nLevel = 95, nMapId = 176,",
"\t\tnPosX = tonumber(TabFile_GetCell(szPos, 2, \"POSX\")),",
"\t\tnPosY = tonumber(TabFile_GetCell(szPos, 2, \"POSY\")),",
"\t\tszScriptPath = \"\\\\script\\\\missions\\\\bairenleitai\\\\npc_quan_jx1.lua\",",
"\t})",
"\tOutputMsg(\"[BAIREN] Bach Nhan Loi Dai khoi dong\")",
"\treturn 1",
"end",
"",
"-- Lenh bai Admin: khoi tao lai (CHI dung khi boot loi - goi lan 2 se tao them timer 1 phut)",
"function BairenLeitai_Adm_Init()",
"\treturn BairenLeitai_Init()",
"end",
""])
put_both(r"script\missions\bairenleitai\bairen_boot.lua", boot)

quan = NL.join([
"-- [BAIREN 23/08] main() NPC \"Quan nhac nho Hoang Thanh Tu\" = zhaojingling\\enternpc.lua:17-30 (thoai)",
"-- + config\\9 ClickNpc detail 3 -> bairen_dialog. Muc \"Hoang Thanh Tu huan luyen cung\" (Truy tim",
"-- Moc Nhan - zhaojingling) KHONG dua vao vi chua port.",
"Include(\"\\\\script\\\\dailogsys\\\\g_dialog.lua\")",
"Include(\"\\\\script\\\\missions\\\\bairenleitai\\\\npc_enter.lua\")",
"",
"function main()",
"\tlocal nNpcIndex = GetLastDiagNpc()",
"\tlocal tbDailog = DailogClass:new(GetNpcName(nNpcIndex))",
"\ttbDailog.szTitleMsg = \"" + SZ_TITLE + "\"",
"\ttbDailog:AddOptEntry(\"" + SZ_LOIDAI + "\", bairen_dialog)",
"\ttbDailog:Show()",
"end",
""])
put_both(r"script\missions\bairenleitai\npc_quan_jx1.lua", quan)

nw = NL.join([
"-- [BAIREN 23/08] map 960: thay EventSys EnterMap/LeaveMap (hundred_arena.lua:160-172 dang ky trong",
"-- state rieng cua no - newworldscript khong thay duoc) - khuon missions\\leaguematch\\combat\\newworld.lua",
"Include(\"\\\\script\\\\maps\\\\newworldscript_default.lua\")",
"BAIREN_HA = \"" + HA + "\"",
"",
"function OnNewWorld(szParam)",
"\tDynamicExecuteByPlayer(PlayerIndex, BAIREN_HA, \"HundredArena:OnEnterMap\")",
"\tOnNewWorldDefault(szParam)",
"end",
"",
"function OnLeaveWorld(szParam)",
"\tDynamicExecuteByPlayer(PlayerIndex, BAIREN_HA, \"HundredArena:OnLeaveMap\")",
"\tOnLeaveWorldDefault(szParam)",
"end",
""])
put_both(r"script\missions\bairenleitai\newworld.lua", nw)
print("3 tep moi ok")

# ============================================================
# 7) settings: 11 tep + maplist + startgame
# ============================================================
sdir = LINUX + r"\settings\maps\missions\bairenleitai"
n = 0
for f in os.listdir(sdir):
    copy_both(os.path.join(sdir, f), r"settings\maps\missions\bairenleitai" + "\\" + f)
    n += 1
assert n == 11, "settings bairenleitai: %d tep" % n

p = os.path.join(E, r"settings\MapList.ini")
s = rd(p)
s = rep1(s, "960_NewWorldScript=\\script\\maps\\newworldscript.lua",
            "960_NewWorldScript=\\script\\missions\\bairenleitai\\newworld.lua", "maplist960")
wr(p, s)
mirror_edit(r"settings\MapList.ini")

p = os.path.join(E, r"script\startgame.lua")
s = rd(p)
NLs = "\r\n" if "\r\n" in s else "\n"
old = 'Include("\\\\script\\\\missions\\\\tongwar\\\\tongwar_autoexec.lua")'
i = s.find(old); assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + 'Include("\\\\script\\\\missions\\\\bairenleitai\\\\bairen_boot.lua")\t-- [BAIREN 23/08] Bach Nhan Loi Dai' + s[j:]
old = '\tDynamicExecute("\\\\script\\\\missions\\\\tongwar\\\\tongwar_driver.lua", "TONGWAR_DriverInit")'
i = s.find(old); assert i >= 0
j = s.find(NLs, i)
s = s[:j] + NLs + '\tDynamicExecute("\\\\script\\\\missions\\\\bairenleitai\\\\bairen_boot.lua", "BairenLeitai_Init")\t-- [BAIREN 23/08] thay activitysys/config/9 ServerStart' + s[j:]
wr(p, s)
mirror_edit(r"script\startgame.lua")
print("settings + startgame ok")
print("TAT CA XONG")
