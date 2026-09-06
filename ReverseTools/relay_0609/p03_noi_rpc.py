# -*- coding: latin-1 -*-
"""p03_noi_rpc.py  [RELAYHT 06/09]

Noi RelayRpc (goi kich ban GameServer <-> relay) vao S3Relay:
  Global.cpp      + include, + dang ky bang ham, + RelayRpc_Init()/Uninit()
  S3Relay.h       + timer_relayrpc = 8
  S3Relay.cpp     + SetTimer 100 ms, + case timer_relayrpc -> RelayRpc_Tick()
  HostConnect.cpp + bat goi "da toi noi" co byte dau s2s_script -> xep hang
  S3Relay.vcxproj + RelayRpc.cpp/.h

Doc/ghi latin-1, GIU NGUYEN xuong dong CRLF cua tep goc. Chay lai duoc nhieu lan.
"""
import io
import os

ROOT = r"D:\GAMEDEVNEW_wt_relay"
SRC = os.path.join(ROOT, "Sources", "MultiServer", "S3Relay")
MARK = "[RELAYHT 06/09]"
TAB = chr(9)
NL = chr(13) + chr(10)


def read(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def write(p, d):
    io.open(p, "w", encoding="latin-1", newline="").write(d)


def after_line(d, anchor, addition):
    """Chen addition thanh dong moi NGAY SAU dong chua anchor."""
    assert d.count(anchor) == 1, "neo khong duy nhat: " + anchor[:60]
    i = d.find(NL, d.find(anchor))
    assert i > 0
    return d[:i] + NL + addition + d[i:]


# ---------------------------------------------------------------- Global.cpp
p = os.path.join(SRC, "Global.cpp")
d = read(p)
n = 0
if '#include "RelayRpc.h"' not in d:
    d = after_line(d, '#include "RelayTaskCentre.h"', '#include "RelayRpc.h"')
    n += 1
if "RelayRpc_Init()" not in d:
    d = after_line(d,
                   "RelayScript_AddFuncTable(g_TaskCentreFuns, g_GetTaskCentreFunNum());",
                   TAB + "RelayScript_AddFuncTable(g_RelayRpcFuns, g_GetRelayRpcFunNum());" + NL
                   + TAB + "RelayRpc_Init();" + TAB + TAB + "// " + MARK + " goi kich ban GS<->relay")
    n += 1
if "RelayRpc_Uninit()" not in d:
    old = TAB + TAB + "TaskCentre_Uninit();"
    assert d.count(old) == 1
    d = d.replace(old, TAB + TAB + "RelayRpc_Uninit();" + NL + old, 1)
    n += 1
if n:
    write(p, d)
print("  Global.cpp: %d cho" % n)

# ---------------------------------------------------------------- S3Relay.h
p = os.path.join(SRC, "S3Relay.h")
d = read(p)
if "timer_relayrpc" not in d:
    d = after_line(d, "const UINT timer_relaytask = 7;",
                   "const UINT timer_relayrpc = 8;" + TAB + "// " + MARK + " nhip goi kich ban, 100 ms")
    write(p, d)
    print("  + S3Relay.h: timer_relayrpc")
else:
    print("  = S3Relay.h: da co")

# ---------------------------------------------------------------- S3Relay.cpp
p = os.path.join(SRC, "S3Relay.cpp")
d = read(p)
n = 0
if '#include "RelayRpc.h"' not in d:
    d = after_line(d, '#include "RelayTaskCentre.h"', '#include "RelayRpc.h"' + TAB + "// " + MARK)
    n += 1
if "SetTimer(g_mainwnd, timer_relayrpc" not in d:
    d = after_line(d, "SetTimer(g_mainwnd, timer_relaytask, 1000, NULL);",
                   TAB + "SetTimer(g_mainwnd, timer_relayrpc, 100, NULL);" + TAB + "// " + MARK)
    n += 1
if "RelayRpc_Tick()" not in d:
    d = after_line(d, "case timer_relaytask: TaskCentre_Tick(); break;",
                   TAB * 3 + "case timer_relayrpc: RelayRpc_Tick(); break;" + TAB + "// " + MARK)
    n += 1
if n:
    write(p, d)
print("  S3Relay.cpp: %d cho" % n)

# ---------------------------------------------------------------- HostConnect.cpp
p = os.path.join(SRC, "HostConnect.cpp")
d = read(p)
n = 0
if '#include "RelayRpc.h"' not in d:
    d = after_line(d, '#include "HostConnect.h"', '#include "RelayRpc.h"' + TAB + "// " + MARK)
    n += 1
if "RelayRpc_OnPacket" not in d:
    old = (TAB + "if (pRelayData->nToIP == 0)" + NL
           + TAB + "{" + NL
           + TAB * 2 + "//arrived" + NL
           + TAB * 2 + "return;" + NL
           + TAB + "}")
    assert d.count(old) == 1, "HostConnect.cpp: khong thay nhanh 'arrived'"
    new = (TAB + "if (pRelayData->nToIP == 0)" + NL
           + TAB + "{" + NL
           + TAB * 2 + "//arrived" + NL
           + TAB * 2 + "// " + MARK + " GameServer goi kich ban tren relay: than goi bat dau" + NL
           + TAB * 2 + "// bang s2s_script. Day la LUONG MANG nen chi xep hang, RelayRpc_Tick()" + NL
           + TAB * 2 + "// (luong chinh) moi chay Lua." + NL
           + TAB * 2 + "if (pRelayData->routeDateLength > 1 &&" + NL
           + TAB * 3 + "*((BYTE*)(pRelayData + 1)) == (BYTE)s2s_script)" + NL
           + TAB * 2 + "{" + NL
           + TAB * 3 + "RelayRpc_OnPacket((BYTE*)(pRelayData + 1) + 1," + NL
           + TAB * 4 + "pRelayData->routeDateLength - 1, GetID());" + NL
           + TAB * 2 + "}" + NL
           + TAB * 2 + "return;" + NL
           + TAB + "}")
    d = d.replace(old, new, 1)
    n += 1
if n:
    write(p, d)
print("  HostConnect.cpp: %d cho" % n)

# ---------------------------------------------------------------- vcxproj
p = os.path.join(SRC, "S3Relay.vcxproj")
d = read(p)
n = 0
if "RelayRpc.cpp" not in d:
    old = '<ClCompile Include="RelayScript.cpp" />'
    assert d.count(old) == 1
    d = d.replace(old, '<ClCompile Include="RelayRpc.cpp" />' + NL + '    ' + old, 1)
    n += 1
if "RelayRpc.h" not in d:
    old = '<ClInclude Include="RelayScript.h" />'
    assert d.count(old) == 1
    d = d.replace(old, '<ClInclude Include="RelayRpc.h" />' + NL + '    ' + old, 1)
    n += 1
if n:
    write(p, d)
print("  vcxproj: %d cho" % n)
print("XONG p03.")
