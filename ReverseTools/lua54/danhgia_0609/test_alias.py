"""test_alias.py <Lua54Dll.dll> - kiem bi danh duong dan trong DLL: tep cu khong co -> doc script\\_duongdan_cu.txt -> nap tep moi."""
import ctypes, os, sys, tempfile
os.environ["LUA54_BO_KIEM"] = "1"
DLL = os.path.abspath(sys.argv[1])
tmp = tempfile.mkdtemp(prefix="l4alias_")
os.makedirs(os.path.join(tmp, "script", "moi"), exist_ok=True)
open(os.path.join(tmp, "script", "moi", "tep.lua"), "w").write("KQ_ALIAS = (KQ_ALIAS or 0) + 1\n")
open(os.path.join(tmp, "script", "_duongdan_cu.txt"), "w").write(
    "-- chu thich\n--@ script\\cu\\tep.lua=script\\moi\\tep.lua\n--@ SCRIPT/cu2/Tep2.LUA = script/moi/tep.lua\n")
os.chdir(tmp)
D = ctypes.CDLL(DLL)
vp, ci, cp = ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p
D.lua4_open.restype = vp; D.lua4_open.argtypes = [ci]
D.lua4_baselibopen.argtypes = [vp]; D.lua4_close.argtypes = [vp]
D.lua4_dofile.argtypes = [vp, cp]; D.lua4_dofile.restype = ci
D.lua4_dostring.argtypes = [vp, cp]; D.lua4_dostring.restype = ci
D.lua4_settop.argtypes = [vp, ci]
D.lua4_getglobal.argtypes = [vp, cp]
D.lua4_tonumber.argtypes = [vp, ci]; D.lua4_tonumber.restype = ctypes.c_double
D.lua4_alias_count.restype = ci
L = D.lua4_open(100); D.lua4_baselibopen(L)
r1 = D.lua4_dofile(L, b"script\\cu\\tep.lua"); D.lua4_settop(L, 0)
r2 = D.lua4_dofile(L, (tmp + "\\script\\CU2\\tep2.lua").encode()); D.lua4_settop(L, 0)
r3 = D.lua4_dofile(L, b"script\\khongco\\x.lua"); D.lua4_settop(L, 0)
D.lua4_dostring(L, b"dofile('script/cu/tep.lua')"); D.lua4_settop(L, 0)       # shim dofile -> L4_DuongDanMoi
D.lua4_getglobal(L, b"KQ_ALIAS"); n = D.lua4_tonumber(L, -1); D.lua4_settop(L, 0)
print("alias count =", D.lua4_alias_count(), "| dofile cu ->", r1, "| tuyet doi CU2 ->", r2, "| khong co ->", r3, "| KQ_ALIAS =", n, "(mong 3)")
ok = (r1 == 0 and r2 == 0 and r3 != 0 and n == 3.0)
print("TEST ALIAS:", "OK" if ok else "SAI")
D.lua4_close(L)
sys.exit(0 if ok else 1)
