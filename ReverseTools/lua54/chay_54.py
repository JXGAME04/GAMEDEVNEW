#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""chay_54.py - [LUA54 05/09] Chay MOT tep Lua (da chuyen) tren Lua 5.4 that (lua54.dll) voi lop lua4compat,
print() ghi ra tep de doi chieu voi lua4.exe.

Chay:  python chay_54.py <tep.lua> <tep_ra.txt> [GOC=<thu muc script>] [--dll ...]
Bien toan cuc GOC duoc dat truoc khi chay (dump_cfg.lua dung de Include).
"""
import ctypes, io, os, sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass
HERE = os.path.dirname(os.path.abspath(__file__))
DLL = r"C:\Program Files\Wireshark\lua54.dll"
if "--dll" in sys.argv:
    DLL = sys.argv[sys.argv.index("--dll") + 1]
L = ctypes.CDLL(DLL)
L.luaL_newstate.restype = ctypes.c_void_p
L.luaL_openlibs.argtypes = [ctypes.c_void_p]
L.lua_close.argtypes = [ctypes.c_void_p]
L.luaL_loadbufferx.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p, ctypes.c_char_p]
L.luaL_loadbufferx.restype = ctypes.c_int
L.lua_pcallk.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p]
L.lua_pcallk.restype = ctypes.c_int
L.lua_tolstring.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
L.lua_tolstring.restype = ctypes.c_char_p
L.lua_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]

# print -> tep (stdout cua DLL khong di qua Python); \t va \n viet bang string.char de khong dinh escape
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
    S = L.luaL_newstate(); L.luaL_openlibs(S)

    def chay(src, ten):
        r = L.luaL_loadbufferx(S, src, len(src), ten.encode("utf-8", "replace"), b"t")
        if r == 0:
            r = L.lua_pcallk(S, 0, 0, 0, None, None)
        if r != 0:
            msg = L.lua_tolstring(S, -1, None)
            print("LOI", ten, (msg or b"?").decode("latin-1", "replace"))
            L.lua_settop(S, 0)
            return False
        L.lua_settop(S, 0)
        return True

    ok = chay(('RA_TEP = "%s"' % ra.replace("\\", "/")).encode("latin-1"), "=ra")
    ok = ok and chay(PRINT_RA_TEP.encode("latin-1"), "=print")
    ok = ok and chay(io.open(os.path.join(HERE, "..", "..", "Sources", "Library", "Lua54", "lua4compat.lua"), "rb").read(), "=lua4compat.lua")
    if goc is not None:
        ok = ok and chay(('GOC = "%s"' % goc.replace("\\", "/")).encode("latin-1"), "=goc")
    ok = ok and chay(io.open(tep, "rb").read(), "=" + os.path.basename(tep))
    chay(b"io.output():close()", "=close")
    L.lua_close(S)
    print("chay_54:", "OK" if ok else "LOI", "->", ra)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
