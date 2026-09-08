# -*- coding: utf-8 -*-
"""Hai mieng va Engine cho x64 (byte-safe): KNetServerNode.cpp '(int) this' trong g_DebugLog; KStepLuaScript.cpp:286 dia chi lua_State
di qua CallFunction 'd' (va_arg int) -> tren x64 dung 'n' (double) de khong cat cut con tro; Win32 giu nguyen (#ifdef _WIN64)."""
import io, re, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_mobile"
E = ROOT + r"\Sources\Engine\Src"

def wr(p, b, nb):
    assert sum(1 for c in b if c > 127) == sum(1 for c in nb if c > 127), p
    io.open(p, "wb").write(nb)

# 1. KNetServerNode.cpp
p = E + r"\KNetServerNode.cpp"; b = io.open(p, "rb").read()
if b"[X64 08/09]" in b:
    print("KNetServerNode.cpp: da va")
else:
    rx = re.compile(rb"(connection accepted from %s\.[^\n]*?)\(int\) this,")
    nb, n = rx.subn(lambda m: m.group(1) + b"(int)(KNPARAM) this,	// [X64 08/09] chi in so nhan dang", b, count=1)
    assert n == 1, "khong thay neo KNetServerNode"
    wr(p, b, nb); print("KNetServerNode.cpp: OK")

# 2. KStepLuaScript.cpp
p = E + r"\KStepLuaScript.cpp"; b = io.open(p, "rb").read()
if b"[X64 08/09]" in b:
    print("KStepLuaScript.cpp: da va")
else:
    old = b'		if (!CallFunction(MsgFuncName, 0, "ds", (unsigned int)pNode->StateAddr,  pNode->szMsgData))'
    assert b.count(old) == 1, "neo KStepLuaScript"
    nl = b"\r\n" if b"\r\n" in b else b"\n"
    new = nl.join([
        b"#ifdef _WIN64	// [X64 08/09] dia chi lua_State di qua so Lua: 'd' doc va_arg int (cat cut tren x64) -> dung 'n' (double)",
        b'		if (!CallFunction(MsgFuncName, 0, "ns", (double)(KUPARAM)pNode->StateAddr,  pNode->szMsgData))',
        b"#else",
        old,
        b"#endif",
    ])
    wr(p, b, b.replace(old, new, 1)); print("KStepLuaScript.cpp: OK")
