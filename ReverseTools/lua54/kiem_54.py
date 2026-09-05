#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""kiem_54.py - [LUA54 05/09] KIEM script da chuyen bang Lua 5.4 THAT (lua54.dll qua ctypes).

Khong can tai gi: dung C:\\Program Files\\Wireshark\\lua54.dll (Lua 5.4) co san tren may.
  1. Kiem cu phap lua4compat.lua roi tung tep .lua da chuyen (luaL_loadbufferx, khong chay).
  2. (--chay) Chay thu cac tep DU LIEU thuan (khong goi ham engine): nap shim + tep, in bang ket qua
     de doi chieu voi lua4.exe (lua4 -f cung tep). Dung cho cauhinh/*.lua, cfgw_meta.lua, header/cauhinh_hoatdong.lua.

Chay:  python kiem_54.py <thu muc script da chuyen> [--dll duong_dan] [--chay tep1.lua tep2.lua ...]
Ghi:   kiem_54_baocao.txt
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
L.lua_tolstring.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_void_p]
L.lua_tolstring.restype = ctypes.c_char_p
L.lua_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]
L.lua_pcallk.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p]
L.lua_pcallk.restype = ctypes.c_int
L.lua_gettop.argtypes = [ctypes.c_void_p]
L.lua_gettop.restype = ctypes.c_int


def loi_tren_dinh(S):
    s = L.lua_tolstring(S, -1, None)
    return s.decode("latin-1", "replace") if s else "?"


def kiem_cu_phap(S, path, ten):
    b = io.open(path, "rb").read()
    r = L.luaL_loadbufferx(S, b, len(b), ("=" + ten).encode("utf-8", "replace"), b"t")
    msg = None if r == 0 else loi_tren_dinh(S)
    L.lua_settop(S, 0)
    return r, msg


def chay_chunk(S, path, ten):
    b = io.open(path, "rb").read()
    r = L.luaL_loadbufferx(S, b, len(b), ("=" + ten).encode("utf-8", "replace"), b"t")
    if r != 0:
        msg = loi_tren_dinh(S); L.lua_settop(S, 0); return r, msg
    r = L.lua_pcallk(S, 0, 0, 0, None, None)
    msg = None if r == 0 else loi_tren_dinh(S)
    L.lua_settop(S, 0)
    return r, msg


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    goc = sys.argv[1]
    bc = []
    S = L.luaL_newstate(); L.luaL_openlibs(S)
    r, msg = kiem_cu_phap(S, os.path.join(HERE, "lua4compat.lua"), "lua4compat.lua")
    bc.append("lua4compat.lua cu phap: %s" % ("OK" if r == 0 else "LOI " + msg))
    so, loi = 0, []
    for d, _, fs in os.walk(goc):
        for f in fs:
            if not f.lower().endswith(".lua"):
                continue
            p = os.path.join(d, f); rel = os.path.relpath(p, goc).replace("\\", "/")
            so += 1
            r, msg = kiem_cu_phap(S, p, rel)
            if r != 0:
                loi.append((rel, msg))
    bc.append("cu phap 5.4: %d tep, %d LOI" % (so, len(loi)))
    for rel, msg in loi[:300]:
        bc.append("  %s  %s" % (rel, msg[:160]))
    if "--chay" in sys.argv:
        ds = sys.argv[sys.argv.index("--chay") + 1:]
        bc.append("")
        bc.append("CHAY THU (shim + tep du lieu):")
        S2 = L.luaL_newstate(); L.luaL_openlibs(S2)
        r, msg = chay_chunk(S2, os.path.join(HERE, "lua4compat.lua"), "lua4compat.lua")
        bc.append("  nap shim: %s" % ("OK" if r == 0 else "LOI " + msg))
        # Include() gia: nap tep tuong doi tu goc
        stub = ("function Include(p) local q = string.gsub(p, string.char(92), '/') local f, e = loadfile(%r .. q) "
                "if not f then _ALERT('Include loi: ' .. tostring(e)) return end return f() end" % (goc.replace("\\", "/"),))
        r = L.luaL_loadbufferx(S2, stub.encode("latin-1"), len(stub), b"=stub", b"t"); L.lua_pcallk(S2, 0, 0, 0, None, None); L.lua_settop(S2, 0)
        for t in ds:
            r, msg = chay_chunk(S2, os.path.join(goc, t), t)
            bc.append("  %-45s %s" % (t, "OK" if r == 0 else "LOI " + msg[:200]))
        L.lua_close(S2)
    L.lua_close(S)
    io.open(os.path.join(HERE, "kiem_54_baocao.txt"), "w", encoding="utf-8").write("\n".join(bc) + "\n")
    print("\n".join(bc[:40]))
    if len(bc) > 40:
        print("... (xem kiem_54_baocao.txt)")


if __name__ == "__main__":
    main()
