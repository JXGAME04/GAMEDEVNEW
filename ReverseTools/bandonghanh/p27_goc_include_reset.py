# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] GOC THAT "moi nut im": Include = lua_dofile KHONG guard
=> 7 module deu Include head.lua => PetSys = {} bi RESET 8 lan trong state
protocol_process_gs => bang cuoi chi con TransferExp + ProtocolProcess,
Summon/ChangeName/... = nil => dispatch im tuyet doi.
Kem: callback "#PetSys:Fn()" khong chay tren JX1 (KPlayer::ExecuteScript chi
parse ham PHANG + tham so SO) => doi sang CreateNewSayEx (dailogsys, bang ham).
Kem: hang task Linux giam len save JX1 (task 1 = RANK MON PHAI!).

A. head.lua: guard PetSys (khong reset khi Include lap)
B. jx1_compat.lua: + unpack (state petsys khong co lib\common.lua)
C. common.lua: doi 7 hang task 1/3001/3002/3009/3010/3007/3008 -> 5132..5138
D. petcard.lua + bdh_admin.lua: SetTask(3061) -> 5124 (PET_TV_SKILL0)
E. feed.lua: not PET_IsCreate() (0 la truthy!) -> PET_IsCreate() ~= 1
F. delete.lua: Say "#PetSys:..." -> CreateNewSayEx bang
G. feature.lua: 3 menu Say "#PetSys:..." -> CreateNewSayEx bang
H. petcard.lua: + Include summon.lua (main goi PetSys:Summon)
I. lenhbaiadmin.lua: + ReLoadScript petcard/feed
K. C KPlayerPet.cpp: Pet_RunProtocol -> KPlayer::ExecuteScript (tu set
   m_ActionScriptID + inject PlayerIndex/PlayerID/SubWorld => Say/AskString
   callback ve dung state petsys)
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
BS = chr(92)


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def sua(duong, cu, moi, nhan, goc=SV):
    p = os.path.join(goc, duong)
    s = doc(p)
    if moi in s:
        print("da co:", nhan)
        return True
    if cu not in s:
        print("!! khong thay anchor:", nhan)
        return False
    if s.count(cu) != 1:
        print("!! anchor khong duy nhat (%d):" % s.count(cu), nhan)
        return False
    ghi(p, s.replace(cu, moi, 1))
    print("VA:", nhan)
    return True


# ---------- A. head.lua guard ----------
sua(r"script\petsys\head.lua",
    "PetSys = {}\r\nPetSys.tbFeature = {}\r\nPetSys.tbLevelUp = {}",
    "-- [JX1 28/08] Include khong guard -> moi module Include head lai la\r\n"
    "-- PetSys={} RESET, xoa sach ham cac module nap truoc (goc 'moi nut im')\r\n"
    "if PetSys == nil then\r\n"
    "\tPetSys = {}\r\n"
    "\tPetSys.tbFeature = {}\r\n"
    "\tPetSys.tbLevelUp = {}\r\n"
    "end",
    "A. head guard PetSys")

# ---------- B. unpack vao jx1_compat ----------
p = os.path.join(SV, r"script\petsys\jx1_compat.lua")
s = doc(p)
if "function unpack" not in s:
    s += "\r\n".join([
        "",
        "if (unpack == nil) then",
        "\t-- ban giong het script\\lib\\common.lua:16 (state petsys khong Include file do)",
        "\tfunction unpack(tb, n)",
        "\t\tif (not n) then",
        "\t\t\tn = 1",
        "\t\tend",
        "\t\tif (n >= getn(tb)) then",
        "\t\t\treturn tb[n]",
        "\t\tend",
        "\t\treturn tb[n], unpack(tb, n + 1)",
        "\tend",
        "end",
        "",
    ])
    ghi(p, s)
    print("VA: B. unpack compat")
else:
    print("da co: B. unpack")

# ---------- C. doi hang task ----------
p = os.path.join(SV, r"script\petsys\common.lua")
s = doc(p)
DOI = [
    ("TSK_EVENT_FINISHED = 3007", "TSK_EVENT_FINISHED = 5137"),
    ("TSK_EVENT_DATE = 3008", "TSK_EVENT_DATE = 5138"),
    ("TSK_COUNT_TRANSFER_EXP = 1", "TSK_COUNT_TRANSFER_EXP = 5132"),
    ("TSK_APPLE_DAILY = 3001", "TSK_APPLE_DAILY = 5133"),
    ("TSK_MAIZE_DAILY = 3009", "TSK_MAIZE_DAILY = 5135"),
    ("TSK_SUGARCANE_DAILY = 3010", "TSK_SUGARCANE_DAILY = 5136"),
    ("TSK_SWEET_POTATO_DAILY = 3002", "TSK_SWEET_POTATO_DAILY = 5134"),
]
n = 0
for cu, moi in DOI:
    if moi in s:
        n += 1
        continue
    if cu not in s:
        print("!! C. thieu anchor:", cu)
        continue
    s = s.replace(cu, moi, 1)
    n += 1
if n == len(DOI):
    if "5132" in s:
        ghi(p, s)
    print("VA: C. 7 hang task -> 5132..5138 (task 1 = RANK mon phai, THOAT NAN)")
else:
    print("!! C. chi duoc %d/7" % n)

# ---------- D. 3061 -> 5124 ----------
sua(r"script\petsys\petcard.lua", "SetTask(3061, nRet)",
    "SetTask(5124, nRet)\t-- [JX1] PET_TV_SKILL0; 3061 la o Linux",
    "D1. petcard 3061->5124")
p = os.path.join(SV, r"script\item\bdh_admin.lua")
s = doc(p)
if "SetTask(5124" not in s:
    s = s.replace("SetTask(3061", "SetTask(5124").replace("GetTask(3061", "GetTask(5124")
    ghi(p, s)
    print("VA: D2. bdh_admin 3061->5124")
else:
    print("da co: D2")

# ---------- E. feed not PET_IsCreate ----------
sua(r"script\petsys\feed.lua",
    "\tif not PET_IsCreate() then",
    "\t-- [JX1] PET_IsCreate tra 0/1; 0 van la TRUE trong Lua nen 'not' sai\r\n"
    "\tif PET_IsCreate() ~= 1 then",
    "E. feed IsCreate ~= 1")

# ---------- F. delete.lua -> CreateNewSayEx ----------
sua(r"script\petsys\delete.lua",
    '\tSay(%DELETE_TITLE, 2,\r\n'
    '\t\t\tformat("%s/#PetSys:ConfirmDelete()", %CONFIRM),\r\n'
    '\t\t\t%CANCEL)',
    '\t-- [JX1 28/08] "#PetSys:Fn()" khong chay (CallFunction chi nhan ham phang\r\n'
    '\t-- + tham so so - KPlayer.cpp:7106) -> dung CreateNewSayEx cua dailogsys\r\n'
    '\tlocal tbOpt = {}\r\n'
    '\ttinsert(tbOpt, {%CONFIRM, self.ConfirmDelete, {self}})\r\n'
    '\ttinsert(tbOpt, {%CANCEL})\r\n'
    '\tCreateNewSayEx(%DELETE_TITLE, tbOpt)',
    "F. delete CreateNewSayEx")

# ---------- G. feature.lua 3 menu ----------
p = os.path.join(SV, r"script\petsys\feature.lua")
s = doc(p)
if "CreateNewSayEx" in s:
    print("da co: G. feature")
else:
    ok = True
    # G1: menu 1 (label TCVN3 giu nguyen bang regex capture)
    m = re.search(
        r'\ttinsert\(tbOpt, format\("%s/#PetSys:ChangeFeatureChooseFeatureLevel\(\)", ("[^"]*")\)\)\r\n'
        r'\ttinsert\(tbOpt, ("[^"]*")\)\r\n'
        r'\t\r\n'
        r'\tSay\(%CHANGE_FEATURE_DLG1_TITLE, getn\(tbOpt\), unpack\(tbOpt\)\)', s)
    if m:
        s = s[:m.start()] + (
            '\ttinsert(tbOpt, {' + m.group(1) + ', self.ChangeFeatureChooseFeatureLevel, {self}})\r\n'
            '\ttinsert(tbOpt, {' + m.group(2) + '})\r\n'
            '\tCreateNewSayEx(%CHANGE_FEATURE_DLG1_TITLE, tbOpt)') + s[m.end():]
    else:
        ok = False
        print("!! G1 khong khop")
    # G2: menu chon cap
    cu2 = ('\t\ttinsert(tbOpt, format("%s/#%s(%d)",szFeatureRange, "PetSys:ChangeFeatureChooseFeature", nFeatureLevel))\r\n'
           '\tend\r\n'
           '\ttinsert(tbOpt, %END_DLG)\r\n'
           '\tSay(szTitle, getn(tbOpt), unpack(tbOpt))')
    moi2 = ('\t\ttinsert(tbOpt, {szFeatureRange, self.ChangeFeatureChooseFeature, {self, nFeatureLevel}})\r\n'
            '\tend\r\n'
            '\ttinsert(tbOpt, {%END_DLG})\r\n'
            '\tCreateNewSayEx(szTitle, tbOpt)')
    if cu2 in s:
        s = s.replace(cu2, moi2, 1)
    else:
        ok = False
        print("!! G2 khong khop")
    # G3: menu chon ngoai quan
    cu3 = ('\t\ttinsert(tbOpt, format("%s/#%s(%d)", tbFeatureGroup[i][1], "PetSys:ConfirmChangeFeature", tbFeatureGroup[i][2]))\r\n'
           '\tend\r\n'
           '\ttinsert(tbOpt, format("%s/#%s()",%BACK, "PetSys:ChangeFeatureChooseFeatureLevel"))\r\n'
           '\ttinsert(tbOpt, %END_DLG)\r\n'
           '\tSay(szTitle, getn(tbOpt), unpack(tbOpt))')
    moi3 = ('\t\ttinsert(tbOpt, {tbFeatureGroup[i][1], self.ConfirmChangeFeature, {self, tbFeatureGroup[i][2]}})\r\n'
            '\tend\r\n'
            '\ttinsert(tbOpt, {%BACK, self.ChangeFeatureChooseFeatureLevel, {self}})\r\n'
            '\ttinsert(tbOpt, {%END_DLG})\r\n'
            '\tCreateNewSayEx(szTitle, tbOpt)')
    if cu3 in s:
        s = s.replace(cu3, moi3, 1)
    else:
        ok = False
        print("!! G3 khong khop")
    if ok:
        ghi(p, s)
        print("VA: G. feature 3 menu -> CreateNewSayEx")

# ---------- H. petcard Include summon ----------
sua(r"script\petsys\petcard.lua",
    'Include("' + BS * 2 + 'script' + BS * 2 + 'activitysys' + BS * 2 + 'playerfunlib.lua")',
    'Include("' + BS * 2 + 'script' + BS * 2 + 'activitysys' + BS * 2 + 'playerfunlib.lua")\r\n'
    'Include("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'summon.lua")\t'
    '-- [JX1] main() goi PetSys:Summon()',
    "H. petcard + summon")

# ---------- I. lenhbai + petcard/feed ----------
p = os.path.join(SV, r"script\item\lenhbaiadmin.lua")
s = doc(p)
if "petcard.lua" not in s:
    neo = ('ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'lang.lua")')
    if s.count(neo) == 1:
        s = s.replace(neo, neo + '\r\n'
                      'ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'petcard.lua")\r\n'
                      'ReLoadScript("' + BS * 2 + 'script' + BS * 2 + 'petsys' + BS * 2 + 'feed.lua")', 1)
        ghi(p, s)
        print("VA: I. lenhbai + petcard/feed")
    else:
        print("!! I. anchor lang.lua =", s.count(neo))
else:
    print("da co: I")

# ---------- K. C Pet_RunProtocol ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
s = doc(p)
cu = ('\tif (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return;\r\n'
      '\tKLuaScript* pScript = (KLuaScript*)g_GetScript(\r\n'
      '\t\t"\\\\script\\\\petsys\\\\protocol_process_gs.lua");\r\n'
      '\tif (!pScript || !pScript->m_LuaState) return;\r\n'
      '\tLua_PushNumber(pScript->m_LuaState, nPlayerIdx);\r\n'
      '\tpScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);\r\n'
      '\tpScript->CallFunction((LPSTR)"PetSys_Protocol", 0, (LPSTR)"d", nOp);')
moi = ('\tif (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return;\r\n'
       '\t// [28/08] chay qua KPlayer::ExecuteScript de m_ActionScriptID tro dung\r\n'
       '\t// script petsys (KPlayer.cpp:7092) - Say chon muc / AskClientForString\r\n'
       '\t// moi callback ve dung state; no cung tu inject PlayerIndex/PlayerID.\r\n'
       '\tPlayer[nPlayerIdx].ExecuteScript(\r\n'
       '\t\t(char*)"\\\\script\\\\petsys\\\\protocol_process_gs.lua",\r\n'
       '\t\t(char*)"PetSys_Protocol", nOp);')
if moi.splitlines()[1] in s:
    print("da co: K")
elif cu in s:
    ghi(p, s.replace(cu, moi, 1))
    print("VA: K. Pet_RunProtocol -> KPlayer::ExecuteScript")
else:
    print("!! K. khong thay anchor - kiem tay")
print("XONG p27")
