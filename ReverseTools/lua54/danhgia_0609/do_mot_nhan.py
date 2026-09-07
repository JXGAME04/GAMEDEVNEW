r"""do_mot_nhan.py - DO SUC MOT NHAN theo dung kieu viec cua GameServer, de quy doi giua may nay va may chu that.
Ba phep do, deu la mot luong:
  1) QUET VUNG   : duyet 1.500 doi tuong cach nhau 6 KB (giong KRegion 6 KB/vung, truot cache L2)
  2) LUA         : goi ham Lua qua Lua54Dll neu co (giong 13.000 loi goi script moi tick), khong co thi bo qua
  3) SO HOC      : vong tinh so nguyen + so thuc (giong tinh sat thuong/toa do)
In "DIEM MOT NHAN" - chay tren may chu that roi lay ti so de quy doi tran nguoi choi.
  python do_mot_nhan.py [duong_dan_Lua54Dll.dll]
"""
import sys, time, ctypes, os
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

def do(ten, f, lap):
    t0 = time.perf_counter()
    f(lap)
    dt = time.perf_counter() - t0
    print("  %-12s %8.0f ms cho %d vong  -> %9.0f vong/giay" % (ten, dt * 1000, lap, lap / dt if dt else 0))
    return lap / dt if dt else 0

# 1) quet vung: mang lon, buoc nhay 6 KB
BUOC = 6144 // 8
N_VUNG = 1500
mang = bytearray(N_VUNG * 6144)
mv = memoryview(mang)
def quet(lap):
    s = 0
    for _ in range(lap):
        for i in range(0, N_VUNG * 6144, 6144):
            s += mang[i]
    return s

# 3) so hoc
def sohoc(lap):
    s = 0.0
    for i in range(lap):
        s += (i * 7 % 1000) * 1.5 + (i >> 3)
    return s

print("DO SUC MOT NHAN (chay tren may chu that de lay ti so)")
print("Python:", sys.version.split()[0], "|", os.environ.get("PROCESSOR_IDENTIFIER", "?"))
d1 = do("QUET VUNG", quet, 2000)
d3 = do("SO HOC", sohoc, 2000000)

# 2) Lua qua Lua54Dll
dll = sys.argv[1] if len(sys.argv) > 1 else None
if dll is None:
    for c in (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\Lua54Dll.dll",
              r"D:\GAMEDEVNEW_wt_lua54b\Lib\lua54\x64\Lua54Dll.dll"):
        if os.path.exists(c): dll = c; break
d2 = 0
if dll and os.path.exists(dll):
    try:
        os.environ["LUA54_BO_KIEM"] = "1"; os.environ["LUA54_PROF"] = "0"
        D = ctypes.CDLL(dll)
        vp, ci, cp = ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p
        D.lua4_open.restype = vp; D.lua4_open.argtypes = [ci]
        D.lua4_baselibopen.argtypes = [vp]
        D.lua4_dostring.argtypes = [vp, cp]; D.lua4_dostring.restype = ci
        D.lua4_close.argtypes = [vp]
        L = D.lua4_open(100); D.lua4_baselibopen(L)
        code = b"local s = 0 for i = 1, 200000 do s = s + floor(i / 7) + mod(i, 13) end KQ = s"
        t0 = time.perf_counter()
        for _ in range(20): D.lua4_dostring(L, code)
        dt = time.perf_counter() - t0
        d2 = 20 * 200000 / dt
        print("  %-12s %8.0f ms cho 4.000.000 vong Lua -> %9.0f vong/giay" % ("LUA", dt * 1000, d2))
        D.lua4_close(L)
    except Exception as e:
        print("  LUA: khong do duoc:", e)
else:
    print("  LUA: khong thay Lua54Dll.dll (bo qua)")

diem = (d1 / 1000.0) * 0.35 + (d3 / 1000000.0) * 0.25 + (d2 / 1000000.0) * 0.40
print("\nDIEM MOT NHAN = %.2f   (cang cao cang manh; lay diem may chu / diem may nay = he so quy doi)" % diem)
