import os, ctypes, sys, time
os.environ["LUA54_BO_KIEM"] = "1"
D = ctypes.CDLL("D:/GAMEDEVNEW/Lib/lua54/x64/Lua54Dll.dll")
D.lua4_open.restype = ctypes.c_void_p; D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_close.argtypes = [ctypes.c_void_p]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dostring.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]
D.lua4_getglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
D.lua4_tonumber.argtypes = [ctypes.c_void_p, ctypes.c_int]; D.lua4_tonumber.restype = ctypes.c_double
D.lua4_setgcthreshold.argtypes = [ctypes.c_void_p, ctypes.c_int]
D.lua4_getgccount.argtypes = [ctypes.c_void_p]; D.lua4_getgccount.restype = ctypes.c_int
def num(L, name):
    D.lua4_getglobal(L, name.encode()); v = D.lua4_tonumber(L, -1); D.lua4_settop(L, 0); return v
def run(L, s):
    r = D.lua4_dostring(L, s.encode()); D.lua4_settop(L, 0); return r
def gc_full(L):
    last = -1
    for i in range(40000):
        D.lua4_setgcthreshold(L, 0)
    return D.lua4_getgccount(L)
ds = sys.argv[1].replace(chr(92), "/")
L = D.lua4_open(100); D.lua4_baselibopen(L)
run(L, "DS = \"%s\"" % ds)
run(L, "DSL = {} for line in io.lines(DS) do DSL[#DSL+1] = line end N_DS = #DSL")
n = int(num(L, "N_DS"))
k0 = gc_full(L)
t0 = time.perf_counter()
run(L, "KEEP = {} for i = 1, #DSL do KEEP[i] = loadfile(DSL[i]) end")
t1 = time.perf_counter()
k1 = gc_full(L)
print("tep: %d; heap truoc %d KB, sau khi bien dich va giu song toan bo (da GC): %d KB => proto = %.1f MB; bien dich %.0f ms" % (n, k0, k1, (k1 - k0) / 1024.0, (t1 - t0) * 1000))
# chay than chunk? khong (co the goi I/O). Do them: bien dich lai lan 2 giu song -> tang gap doi? (kiem tinh tuyen tinh)
run(L, "KEEP2 = {} for i = 1, #DSL do KEEP2[i] = loadfile(DSL[i]) end")
k2 = gc_full(L)
print("bien dich lan 2 giu song: %d KB => tang %.1f MB (tuyen tinh neu ~ bang lan 1)" % (k2, (k2 - k1) / 1024.0))
# so voi nap BYTECODE (string.dump -> load): toc do
run(L, "DUMP = {} for i = 1, #DSL do local f = KEEP[i] if f then DUMP[i] = string.dump(f) end end TB = 0 for i = 1, #DSL do if DUMP[i] then TB = TB + #DUMP[i] end end")
tb = num(L, "TB")
t0 = time.perf_counter()
run(L, "KEEP3 = {} for i = 1, #DSL do if DUMP[i] then KEEP3[i] = load(DUMP[i], DSL[i], \"b\") end end")
t1 = time.perf_counter()
print("bytecode: tong %.1f MB; nap lai tu bytecode toan cay: %.0f ms (so voi bien dich nguon o tren)" % (tb / 1048576.0, (t1 - t0) * 1000))
D.lua4_close(L)
