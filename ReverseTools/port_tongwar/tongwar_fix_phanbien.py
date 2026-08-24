# -*- coding: utf-8 -*-
r"""[TONGWAR 23/08 - hau phan bien] F9-F12 + don trung lap awardtemplet:
F9  (CAO): npc_tongwar.lua thieu Include gb_taskfuncs.lua -> gb_GetTask nil ngay click dau
     (Linux 1 state co san qua event.lua; JX1 moi tep mot state).
F10 (CAO): tbAwardTemplet.TYPE thieu 'nExp'/'nExp_tl' -> Hoang Chan Don 2273 (2 ty exp),
     Qua Dai Hoang Kim 4864 (200tr), Than Moc Lenh (10tr/60tr) TRU DO ma khong trao exp.
     Tao lib\awardtype\exp_jx1.lua (nExp -> AddOwnExp; nExp_tl -> tl_addPlayerExp cua
     task\task_addplayerexp.lua - tep CO tren du an, chu thich cu cua tongwar_port sai).
F11 (TRUNG): match\newworld.lua thieu Include common.lua -> split nil moi luot vao/ra 605-607.
F12 (THAP): head.lua:492 hai tham so format van 29/03/2014 / 05/04/2014.
+ don 3 dong Include zhenyuan_jx1 lap trong awardtemplet.lua (tc_port chay 3 luot).
Idempotent - chay lai an toan.
"""
import io, os, shutil

E    = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRT = r"D:\GAMEDEVNEW\serverscript_jx2\tongwar"
MIRC = r"D:\GAMEDEVNEW\serverscript_jx2\tongcastle"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s):
    d = os.path.dirname(p)
    if d and not os.path.isdir(d): os.makedirs(d)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def mir(p_abs, root_mir, rel):
    d = os.path.dirname(os.path.join(root_mir, rel))
    if not os.path.isdir(d): os.makedirs(d)
    shutil.copyfile(p_abs, os.path.join(root_mir, rel))

# ---- F9: npc_tongwar.lua + gb_taskfuncs ----
p = os.path.join(E, r"script\event\tongwar\npc_tongwar.lua")
s = rd(p)
if "gb_taskfuncs" not in s:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = 'Include("\\\\script\\\\event\\\\tongwar\\\\head.lua")'
    assert s.count(old) == 1
    s = s.replace(old,
        'Include("\\\\script\\\\gb_taskfuncs.lua")\t-- [TONGWAR 23/08 phan bien F9] gb_GetTask cho get_tongwar_season (Linux co qua event.lua - 1 state)' + NL + old, 1)
    wr(p, s)
    print("F9 ok")
else:
    print("F9 da co")
mir(p, MIRT, r"script\event\tongwar\npc_tongwar.lua")

# ---- F10: awardtype exp_jx1.lua + Include vao awardtemplet + don trung lap ----
zy = "\n".join([
"-- exp_jx1.lua - [TONGWAR 23/08 phan bien F10] awardtype nExp / nExp_tl cho tbAwardTemplet:",
"-- nExp    -> AddOwnExp (engine, ScriptFuns.cpp)  - Qua Dai Hoang Kim 200tr, Than Moc Lenh 10tr",
"-- nExp_tl -> tl_addPlayerExp (task\\task_addplayerexp.lua:60) - Hoang Chan Don 2 ty (co tran cap)",
"-- KHONG Include lib\\awardtype\\exp.lua goc (Include nguoc awardtemplet -> vong).",
"Include(\"\\\\script\\\\lib\\\\log.lua\")",
"-- [23/08 vong Include] task_addplayerexp keo task_head -> g_activity -> awardtemplet = vong - nap luoi luc Give()",
"",
"ExpJX1Type = {}",
"",
"function ExpJX1Type:Give(tbItem, nAwardCount, tbLogTitle)",
"\tlocal nExp = (tbItem.nExp or 0) * (nAwardCount or 1)",
"\tif nExp > 0 then",
"\t\tAddOwnExp(nExp)",
"\t\tWriteLog(format(\"[awardtype exp_jx1] %s +%d exp\", GetName() or \"\", nExp))",
"\t\treturn 1",
"\tend",
"\treturn 0",
"end",
"",
"ExpTlJX1Type = {}",
"",
"function ExpTlJX1Type:Give(tbItem, nAwardCount, tbLogTitle)",
"\tlocal nExp = (tbItem.nExp_tl or 0) * (nAwardCount or 1)",
"\tif nExp > 0 then",
"\t\tif not tl_addPlayerExp then",
"\t\t\tInclude(\"\\\\script\\\\task\\\\task_addplayerexp.lua\")\t-- nap luoi (xem chu thich dau tep)",
"\t\tend",
"\t\ttl_addPlayerExp(nExp)",
"\t\tWriteLog(format(\"[awardtype exp_jx1] %s +%d exp (tl)\", GetName() or \"\", nExp))",
"\t\treturn 1",
"\tend",
"\treturn 0",
"end",
"",
"if tbAwardTemplet then",
"\ttbAwardTemplet:RegType(\"nExp\", ExpJX1Type)",
"\ttbAwardTemplet:RegType(\"nExp_tl\", ExpTlJX1Type)",
"end",
""])
p = os.path.join(E, r"script\lib\awardtype\exp_jx1.lua")
wr(p, zy)
mir(p, MIRT, r"script\lib\awardtype\exp_jx1.lua")

p = os.path.join(E, r"script\lib\awardtemplet.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
ZLINE = 'Include("\\\\script\\\\lib\\\\awardtype\\\\zhenyuan_jx1.lua")\t-- [TONGCASTLE 23/08] diem Chan Nguyen (shenmuling)'
n = s.count(ZLINE)
if n > 1:
    first = s.find(ZLINE)
    head = s[:first + len(ZLINE)]
    tail = s[first + len(ZLINE):].replace(ZLINE + NL, "").replace(NL + ZLINE, "")
    s = head + tail
    print("don %d dong zhenyuan lap" % (n - 1))
if "exp_jx1.lua" not in s:
    s = s.replace(ZLINE, ZLINE + NL + 'Include("\\\\script\\\\lib\\\\awardtype\\\\exp_jx1.lua")\t-- [TONGWAR 23/08 phan bien F10] nExp/nExp_tl (Hoang Chan Don, Qua Dai HK, Than Moc Lenh)', 1)
    print("F10 Include ok")
wr(p, s)
mir(p, MIRT, r"jx1_edits\script\lib\awardtemplet.lua")
mir(p, MIRC, r"jx1_edits\script\lib\awardtemplet.lua")

# ---- F11: match\newworld.lua + common.lua ----
p = os.path.join(E, r"script\missions\tongwar\match\newworld.lua")
s = rd(p)
if "common.lua" not in s:
    NL = "\r\n" if "\r\n" in s else "\n"
    old = 'Include("\\\\script\\\\maps\\\\newworldscript_default.lua")'
    assert s.count(old) == 1
    s = s.replace(old,
        'Include("\\\\script\\\\lib\\\\common.lua")\t-- [TONGWAR 23/08 phan bien F11] split() cho PraseParam' + NL + old, 1)
    wr(p, s)
    print("F11 ok")
else:
    print("F11 da co")
mir(p, MIRT, r"script\missions\tongwar\match\newworld.lua")

# ---- F12: head.lua:492 hai moc ngay dau/cuoi ----
p = os.path.join(E, r"script\event\tongwar\head.lua")
s = rd(p)
c1, c2 = s.count('"29/03/2014"'), s.count('"05/04/2014"')
if c1 or c2:
    assert c1 == 1 and c2 == 1, (c1, c2)
    s = s.replace('"29/03/2014"', '"29/10/2026"').replace('"05/04/2014"', '"05/11/2026"')
    wr(p, s)
    print("F12 ok")
else:
    print("F12 da doi (29/03: %d, 05/04: %d, 29/10: %d)" % (c1, c2, s.count('"29/10/2026"')))
mir(p, MIRT, r"script\event\tongwar\head.lua")
print("XONG")
