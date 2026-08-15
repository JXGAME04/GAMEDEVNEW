# -*- coding: utf-8 -*-
# Quet CU PHAP toan bo cay script bang chinh Lua 4.0 cua game (lualibdll.dll).
import ctypes, io, os, sys

B = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DLL = os.path.join(B, "lualibdll.dll")
os.add_dll_directory(os.path.dirname(DLL))
lua = ctypes.CDLL(DLL)
lua.lua_open.restype = ctypes.c_void_p
lua.lua_open.argtypes = [ctypes.c_int]
lua.lua_dobuffer.restype = ctypes.c_int
lua.lua_dobuffer.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_char_p]

files = []
for base in ("script", "scriptjx2"):
    for root, d, fs in os.walk(os.path.join(B, base)):
        for fn in fs:
            if fn.lower().endswith(".lua"):
                files.append(os.path.join(root, fn))

bad = []
for p in files:
    try:
        data = io.open(p, 'rb').read()
    except Exception:
        continue
    L = lua.lua_open(0)
    if not L:
        continue
    rc = lua.lua_dobuffer(L, data, len(data), b"chk")
    if rc == 3:
        bad.append(p)

log = io.open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "lint_result.txt"), "w", encoding="utf-8")
log.write("tong %d file .lua, LOI CU PHAP: %d\n" % (len(files), len(bad)))
for p in bad:
    log.write("SYNTAX: " + p.replace(B, "") + "\n")
log.close()
print("tong %d file, loi cu phap: %d (chi tiet: lint_result.txt)" % (len(files), len(bad)))
