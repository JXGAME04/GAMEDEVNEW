#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""chay_dll.py - [LUA54] Chay mot tep Lua qua DUNG API lua4_* cua Lua54Dll.dll (x64) - con duong ma
GameServer se dung (lua4_open -> lua4_baselibopen -> lua4_dofile). print() ghi ra tep de diff voi lua4.exe.

Chay:  python chay_dll.py <tep.lua> <tep_ra.txt> [GOC=<thu muc script>] [--selftest]
"""
import ctypes, io, os, sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass
DLL = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "Lib", "lua54", "x64", "Lua54Dll.dll")
D = ctypes.CDLL(os.path.abspath(DLL))
D.lua4_open.restype = ctypes.c_void_p; D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_close.argtypes = [ctypes.c_void_p]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dostring.restype = ctypes.c_int
D.lua4_dofile.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dofile.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]
D.lua4_selftest.argtypes = [ctypes.c_void_p]; D.lua4_selftest.restype = ctypes.c_int

PRINT_RA_TEP = (
    'io.output(RA_TEP) '
    'print = function(...) local t = {} for i = 1, select("#", ...) do t[i] = tostring((select(i, ...))) end '
    'io.write(table.concat(t, string.char(9)), string.char(10)) end'
)


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    tep, ra = sys.argv[1], sys.argv[2]
    goc = None
    for a in sys.argv[3:]:
        if a.startswith("GOC="):
            goc = a[4:]
    L = D.lua4_open(100)
    D.lua4_baselibopen(L)
    if "--selftest" in sys.argv:
        print("lua4_selftest:", D.lua4_selftest(L))
    ok = True
    def chay(s):
        nonlocal ok
        r = D.lua4_dostring(L, s.encode("latin-1"))
        D.lua4_settop(L, 0)
        if r != 0:
            ok = False
            print("LOI dostring ma", r)
    chay('RA_TEP = "%s"' % ra.replace("\\", "/"))
    chay(PRINT_RA_TEP)
    if goc is not None:
        chay('GOC = "%s"' % goc.replace("\\", "/"))
    r = D.lua4_dofile(L, tep.encode("latin-1"))
    D.lua4_settop(L, 0)
    if r != 0:
        ok = False
        print("LOI dofile ma", r, "(xem stderr / _ALERT)")
    chay("io.output():close()")
    D.lua4_close(L)
    print("chay_dll:", "OK" if ok else "LOI", "->", ra)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
