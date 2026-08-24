# -*- coding: utf-8 -*-
r"""[23/08 dem] CAT VONG INCLUDE lam GameServer boot stack-Overflow moi state:
awardtemplet:66/67 -> {zhenyuan_jx1:6 -> playerfunlib:4} / {exp_jx1:6 -> task_addplayerexp:10 -> task_head}
-> ... -> g_activity -> activity:300 -> awardtemplet -> LAP VO HAN.
Sua: 2 tep awardtype cua toi thanh LA (leaf) - bo Include nang top-level, nap luoi luc Give().
Ap dung: E:\bin\server + mirror serverscript_jx2 + 2 generator (tc_port.py / tongwar_fix_phanbien.py).
"""
import io, os, shutil

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MZ = r"D:\GAMEDEVNEW\serverscript_jx2\tongcastle\script\lib\awardtype\zhenyuan_jx1.lua"
ME = r"D:\GAMEDEVNEW\serverscript_jx2\tongwar\script\lib\awardtype\exp_jx1.lua"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)

BS2 = "\\\\"  # 2 backslash trong file lua

# ---- 1) zhenyuan_jx1.lua ----
p = os.path.join(E, r"script\lib\awardtype\zhenyuan_jx1.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
inc_pf = 'Include("' + BS2 + 'script' + BS2 + 'activitysys' + BS2 + 'playerfunlib.lua")'
if inc_pf + NL in s.replace(inc_pf + "\t", inc_pf + NL):  # co the co tab sau
    pass
if NL + inc_pf in s:
    s = s.replace(NL + inc_pf,
        NL + "-- [23/08 vong Include] KHONG Include playerfunlib o day: playerfunlib:4 Include nguoc" + NL +
        "-- awardtemplet -> awardtemplet lai Include tep nay = VONG VO HAN (stack Overflow moi state," + NL +
        "-- ScriptError.log phinh hang chuc MB, GameServer boot khong len). Nap luoi luc Give().", 1)
    old_call = "\tPlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, nPoint)"
    assert s.count(old_call) == 1, "zy call"
    s = s.replace(old_call,
        "\tif not PlayerFunLib then" + NL +
        '\t\tInclude("' + BS2 + "script" + BS2 + "activitysys" + BS2 + 'playerfunlib.lua")\t-- nap luoi (xem chu thich dau tep)' + NL +
        "\tend" + NL + old_call, 1)
    wr(p, s)
    print("zhenyuan_jx1.lua: DA CAT VONG")
else:
    print("zhenyuan_jx1.lua: da xu ly truoc do")
shutil.copyfile(p, MZ)

# ---- 2) exp_jx1.lua ----
p = os.path.join(E, r"script\lib\awardtype\exp_jx1.lua")
s = rd(p)
NL = "\r\n" if "\r\n" in s else "\n"
inc_tl = 'Include("' + BS2 + 'script' + BS2 + 'task' + BS2 + 'task_addplayerexp.lua")'
if NL + inc_tl in s:
    s = s.replace(NL + inc_tl,
        NL + "-- [23/08 vong Include] KHONG Include task_addplayerexp o day: keo task_head -> ... ->" + NL +
        "-- g_activity -> activity -> awardtemplet -> tep nay = VONG VO HAN (stack Overflow). Nap luoi luc Give().", 1)
    old_call = "\t\ttl_addPlayerExp(nExp)"
    assert s.count(old_call) == 1, "exp call"
    s = s.replace(old_call,
        "\t\tif not tl_addPlayerExp then" + NL +
        '\t\t\tInclude("' + BS2 + "script" + BS2 + "task" + BS2 + 'task_addplayerexp.lua")\t-- nap luoi (xem chu thich dau tep)' + NL +
        "\t\tend" + NL + old_call, 1)
    wr(p, s)
    print("exp_jx1.lua: DA CAT VONG")
else:
    print("exp_jx1.lua: da xu ly truoc do")
shutil.copyfile(p, ME)

# ---- 3) generator tc_port.py ----
p = r"D:\GAMEDEVNEW\ReverseTools\port_tongcastle\tc_port.py"
s = io.open(p, "r", encoding="utf-8").read()
g_old = '"Include(\\"\\\\\\\\script\\\\\\\\activitysys\\\\\\\\playerfunlib.lua\\")",'
if g_old in s:
    s = s.replace(g_old,
        '"-- [23/08 vong Include] playerfunlib:4 Include nguoc awardtemplet = vong vo han - nap luoi luc Give()",', 1)
    g_call = '"\\tPlayerFunLib:AddTask(TASK_CHANGNGUYENDAN, nPoint)",'
    assert s.count(g_call) == 1, "tc gen call"
    s = s.replace(g_call,
        '"\\tif not PlayerFunLib then",\n'
        '"\\t\\tInclude(\\"\\\\\\\\script\\\\\\\\activitysys\\\\\\\\playerfunlib.lua\\")\\t-- nap luoi (xem chu thich dau tep)",\n'
        '"\\tend",\n' + g_call, 1)
    io.open(p, "w", encoding="utf-8").write(s)
    print("tc_port.py: generator DA SUA")
else:
    print("tc_port.py: generator da xu ly / khong thay")

# ---- 4) generator tongwar_fix_phanbien.py ----
p = r"D:\GAMEDEVNEW\ReverseTools\port_tongwar\tongwar_fix_phanbien.py"
s = io.open(p, "r", encoding="utf-8").read()
g_old = '"Include(\\"\\\\\\\\script\\\\\\\\task\\\\\\\\task_addplayerexp.lua\\")",'
if g_old in s:
    s = s.replace(g_old,
        '"-- [23/08 vong Include] task_addplayerexp keo task_head -> g_activity -> awardtemplet = vong - nap luoi luc Give()",', 1)
    g_call = '"\\t\\ttl_addPlayerExp(nExp)",'
    assert s.count(g_call) == 1, "tw gen call"
    s = s.replace(g_call,
        '"\\t\\tif not tl_addPlayerExp then",\n'
        '"\\t\\t\\tInclude(\\"\\\\\\\\script\\\\\\\\task\\\\\\\\task_addplayerexp.lua\\")\\t-- nap luoi (xem chu thich dau tep)",\n'
        '"\\t\\tend",\n' + g_call, 1)
    io.open(p, "w", encoding="utf-8").write(s)
    print("tongwar_fix_phanbien.py: generator DA SUA")
else:
    print("tongwar_fix_phanbien.py: generator da xu ly / khong thay")

print("XONG")
