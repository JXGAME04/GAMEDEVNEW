import ctypes, os, sys
os.environ["LUA54_BO_KIEM"] = "1"
D = ctypes.CDLL("D:/GAMEDEVNEW/Lib/lua54/x64/Lua54Dll.dll")
D.lua4_open.restype = ctypes.c_void_p; D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_close.argtypes = [ctypes.c_void_p]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dostring.restype = ctypes.c_int
D.lua4_dofile.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dofile.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]
L = D.lua4_open(100); D.lua4_baselibopen(L)
ra = sys.argv[2].replace(chr(92), "/")
D.lua4_dostring(L, ('RA_TEP = "%s"' % ra).encode()); D.lua4_settop(L, 0)
r = D.lua4_dofile(L, sys.argv[1].encode()); D.lua4_settop(L, 0)
D.lua4_close(L)
print("ma dofile =", r)
print(open(ra, encoding="latin-1").read())
