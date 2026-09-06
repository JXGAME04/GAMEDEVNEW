import ctypes, os, sys
os.environ["LUA54_BO_KIEM"] = "1"
D = ctypes.CDLL(sys.argv[1])
D.lua4_open.restype = ctypes.c_void_p; D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_close.argtypes = [ctypes.c_void_p]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_selftest.argtypes = [ctypes.c_void_p]; D.lua4_selftest.restype = ctypes.c_int
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dostring.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]
D.lua4_getglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
D.lua4_tostring.argtypes = [ctypes.c_void_p, ctypes.c_int]; D.lua4_tostring.restype = ctypes.c_char_p
for name in ("lua4_perf_set", "lua4_perf_read", "lua4_inc_stats", "lua4_inc_set", "lua4_dofile"):
    print(name, "export:", hasattr(D, name))
L = D.lua4_open(100); D.lua4_baselibopen(L)
print("selftest loi =", D.lua4_selftest(L))
D.lua4_dostring(L, b"collectgarbage() KQ = tostring(gcinfo()) .. '|' .. tostring(collectgarbage(10))"); D.lua4_settop(L, 0)
D.lua4_getglobal(L, b"KQ"); print("gcinfo/collectgarbage:", D.lua4_tostring(L, -1)); D.lua4_settop(L, 0)
D.lua4_close(L)
