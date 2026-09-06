# -*- coding: latin-1 -*-
"""
p01_noi_hatang.py  [RELAYHT 06/09]

Noi ha tang kich ban Linux vao S3Relay: RelayScript + RelayShareData + RelayTaskCentre.
Va cac tep CO SAN (doc/ghi latin-1 de KHONG lam hong byte TCVN3/GBK):

  DoScript.h        + extern GameScriptFuns  (RelayScript.cpp can dung lai)
  Global.cpp        + include 3 header, + goi *_Init() sau InitScript(), + *_Uninit()
  S3Relay.h         + timer_relaytask = 7
  S3Relay.cpp       + SetTimer 1 giay, + case timer_relaytask -> TaskCentre_Tick()
  S3Relay.vcxproj   + 3 cap tep nguon

Chay lai duoc nhieu lan (idempotent): moi cho vá deu kiem dau [RELAYHT 06/09] truoc.
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SRC = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay")
MARK = "[RELAYHT 06/09]"


def read(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def write(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


def patch(path, anchor, insert, before=False, name=""):
    d = read(path)
    if MARK in d and insert.strip().split("\n")[0].strip() in d:
        print("  = %s: da vá roi" % name)
        return
    n = d.count(anchor)
    if n != 1:
        print("  ! %s: NEO khong duy nhat (%d) -> DUNG" % (name, n))
        sys.exit(1)
    d = d.replace(anchor, (insert + anchor) if before else (anchor + insert), 1)
    write(path, d)
    print("  + %s" % name)


# ---------------------------------------------------------------- DoScript.h
p = os.path.join(SRC, "DoScript.h")
d = read(p)
if "extern TLua_Funcs GameScriptFuns" not in d:
    d = d.replace(
        "BOOL InitScript();",
        "// " + MARK + " RelayScript.cpp dang ky lai bang ham nay cho MOI kich ban relay\n"
        "extern TLua_Funcs GameScriptFuns[];\n"
        "int g_GetGameScriptFunNum();\n"
        "\n"
        "BOOL InitScript();", 1)
    write(p, d)
    print("  + DoScript.h: extern GameScriptFuns")
else:
    print("  = DoScript.h: da co")

# ---------------------------------------------------------------- Global.cpp
p = os.path.join(SRC, "Global.cpp")
d = read(p)
if "RelayTaskCentre.h" not in d:
    d = d.replace(
        '#include "OfflineMsgDB.h"',
        '#include "RelayScript.h"\t\t// ' + MARK + ' ha tang kich ban kieu Linux\n'
        '#include "RelayShareData.h"\n'
        '#include "RelayTaskCentre.h"\n'
        '#include "OfflineMsgDB.h"', 1)
    print("  + Global.cpp: 3 include")
if "RelayScript_Init()" not in d:
    old = "\tInitScript();"
    assert d.count(old) == 1, "Global.cpp: neo InitScript() khong duy nhat"
    new = (
        "\tInitScript();\n"
        "\n"
        "\t// " + MARK + " HA TANG KICH BAN KIEU LINUX. Thu tu BAT BUOC:\n"
        "\t// dang ky het bang ham -> roi moi nap kich ban (TaskCentre nap kich ban).\n"
        "\tRelayScript_Init();\n"
        "\tRelayScript_AddFuncTable(g_ShareDataFuns, g_GetShareDataFunNum());\n"
        "\tRelayScript_AddFuncTable(g_TaskCentreFuns, g_GetTaskCentreFunNum());\n"
        "\tShareData_Init();\t\t// loi MySQL -> tu tat, relay van chay\n"
        "\tTaskCentre_Init();")
    d = d.replace(old, new, 1)
    print("  + Global.cpp: goi *_Init()")
if "TaskCentre_Uninit()" not in d:
    old = "\t\tOfflineMsg_Uninit();"
    assert d.count(old) == 1, "Global.cpp: neo OfflineMsg_Uninit() khong duy nhat"
    new = ("\t\tTaskCentre_Uninit();\t// " + MARK + "\n"
           "\t\tShareData_Uninit();\n"
           "\t\tRelayScript_Uninit();\n"
           "\t\tOfflineMsg_Uninit();")
    d = d.replace(old, new, 1)
    print("  + Global.cpp: goi *_Uninit()")
write(p, d)

# ---------------------------------------------------------------- S3Relay.h
p = os.path.join(SRC, "S3Relay.h")
d = read(p)
if "timer_relaytask" not in d:
    old = "const UINT timer_tongjx2 = 6;"
    assert d.count(old) == 1, "S3Relay.h: neo timer_tongjx2 khong duy nhat"
    d = d.replace(old, old + "\nconst UINT timer_relaytask = 7;\t// " + MARK + " nhip TaskCentre, 1 giay", 1)
    write(p, d)
    print("  + S3Relay.h: timer_relaytask")
else:
    print("  = S3Relay.h: da co")

# ---------------------------------------------------------------- S3Relay.cpp
p = os.path.join(SRC, "S3Relay.cpp")
d = read(p)
if 'SetTimer(g_mainwnd, timer_relaytask' not in d:
    old = "\tSetTimer(g_mainwnd, timer_tongjx2, 30000, NULL);"
    assert d.count(old) == 1, "S3Relay.cpp: neo SetTimer tongjx2 khong duy nhat"
    d = d.replace(old, old + "\n\tSetTimer(g_mainwnd, timer_relaytask, 1000, NULL);\t// " + MARK + " TaskCentre", 1)
    print("  + S3Relay.cpp: SetTimer")
if "TaskCentre_Tick()" not in d:
    old = "\t\t\tcase timer_tongjx2: JX2_TimerTick(); break;"
    assert d.count(old) == 1, "S3Relay.cpp: neo case timer_tongjx2 khong duy nhat"
    d = d.replace(old, old + "\n\t\t\tcase timer_relaytask: TaskCentre_Tick(); break;\t// " + MARK, 1)
    print("  + S3Relay.cpp: case WM_TIMER")
if '#include "RelayTaskCentre.h"' not in d:
    old = '#include "S3Relay.h"'
    assert d.count(old) >= 1, "S3Relay.cpp: khong thay include S3Relay.h"
    d = d.replace(old, old + '\n#include "RelayTaskCentre.h"\t// ' + MARK, 1)
    print("  + S3Relay.cpp: include")
write(p, d)

# ---------------------------------------------------------------- vcxproj
p = os.path.join(SRC, "S3Relay.vcxproj")
d = read(p)
if "RelayTaskCentre.cpp" not in d:
    old = '<ClCompile Include="OfflineMsgDB.cpp" />'
    if d.count(old) != 1:
        # thu kieu the co con (khong tu dong dong)
        old = '<ClCompile Include="OfflineMsgDB.cpp">'
    assert d.count(old) == 1, "vcxproj: khong tim thay neo OfflineMsgDB.cpp"
    add = ('<ClCompile Include="RelayScript.cpp" />\r\n'
           '    <ClCompile Include="RelayShareData.cpp" />\r\n'
           '    <ClCompile Include="RelayTaskCentre.cpp" />\r\n'
           '    ')
    d = d.replace(old, add + old, 1)
    print("  + vcxproj: 3 ClCompile")
if "RelayTaskCentre.h" not in d:
    old = '<ClInclude Include="OfflineMsgDB.h" />'
    if d.count(old) != 1:
        old = '<ClInclude Include="OfflineMsgDB.h">'
    assert d.count(old) == 1, "vcxproj: khong tim thay neo OfflineMsgDB.h"
    add = ('<ClInclude Include="RelayScript.h" />\r\n'
           '    <ClInclude Include="RelayShareData.h" />\r\n'
           '    <ClInclude Include="RelayTaskCentre.h" />\r\n'
           '    ')
    d = d.replace(old, add + old, 1)
    print("  + vcxproj: 3 ClInclude")
write(p, d)

print("XONG.")
