#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""chay_stub.py - [LUA54] Chay MOT script (da chuyen) qua Lua54Dll voi ham engine GIA (moi bien toan cuc thieu
tra ve ham rong) de loi ra LOI LUA THAT (khong phai loi thieu ham C++). Include duoc gia lap tu goc script.

Chay:  python chay_stub.py <thu muc script> <tep tuong doi> [<tep> ...]
"""
import ctypes, os, sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass
HERE = os.path.dirname(os.path.abspath(__file__))
DLL = os.path.abspath(os.path.join(HERE, "..", "..", "Lib", "lua54", "x64", "Lua54Dll.dll"))
os.environ["LUA54_BO_KIEM"] = "1"
D = ctypes.CDLL(DLL)
D.lua4_open.restype = ctypes.c_void_p; D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_close.argtypes = [ctypes.c_void_p]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dostring.restype = ctypes.c_int
D.lua4_dofile.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dofile.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]

STUB = r'''
LOI_DS = {}
_ALERT = function(m) LOI_DS[getn(LOI_DS) + 1] = tostring(m) end
_ERRORMESSAGE = function(m) _ALERT("error: " .. tostring(m)) end
setmetatable(_G, {__index = function(t, k) return function() return nil end end})
function Include(p)
    local q = gsub(p, strchar(92), "/")
    q = gsub(q, "^/script", "")
    local fn, e = loadfile(GOC .. q)
    if not fn then _ALERT("Include " .. q .. ": " .. tostring(e)) return end
    local ok, er = pcall(fn)
    if not ok then _ALERT("Include " .. q .. ": " .. tostring(er)) end
end
'''


def main():
    goc = sys.argv[1].replace("\\", "/").rstrip("/")
    for rel in sys.argv[2:]:
        L = D.lua4_open(100); D.lua4_baselibopen(L)
        D.lua4_dostring(L, ('GOC = "%s"' % goc).encode("latin-1")); D.lua4_settop(L, 0)
        r = D.lua4_dostring(L, STUB.encode("latin-1")); D.lua4_settop(L, 0)
        if r != 0:
            print("stub loi", r); return 1
        r = D.lua4_dofile(L, (goc + "/" + rel).encode("latin-1")); D.lua4_settop(L, 0)
        print("== %s -> ma %d" % (rel, r))
        # in danh sach loi
        D.lua4_dostring(L, b'RA = "" for i = 1, getn(LOI_DS) do RA = RA .. "  " .. LOI_DS[i] .. string.char(10) end'); D.lua4_settop(L, 0)
        D.lua4_getglobal = D.lua4_getglobal; D.lua4_getglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        D.lua4_tostring.argtypes = [ctypes.c_void_p, ctypes.c_int]; D.lua4_tostring.restype = ctypes.c_char_p
        D.lua4_getglobal(L, b"RA")
        s = D.lua4_tostring(L, -1)
        print((s or b"").decode("latin-1", "replace")[:3000])
        D.lua4_settop(L, 0)
        D.lua4_close(L)
    return 0


if __name__ == "__main__":
    sys.exit(main())
