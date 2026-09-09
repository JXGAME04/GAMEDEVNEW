# -*- coding: utf-8 -*-
"""goi_va_nhiptrinh1_do_khoang_present_0809.py - [TRINH 08/09] Con dung MOT gia thuyet chua do: NHIP TRINH KHUNG.
Da biet chac: tung khung ve ra deu DUNG (anh chup dung), chuoi vi tri noi suy deu va don dieu (do 900 khung),
mau theo phe dung, khong ai bi ve hai lan, he chieu sang tat van bi. Tat noi suy thi HET, ha xuong 72 khung/giay
voi nhip deu thi NHE HAN. Nghia la muc do bi ti le voi so lan hinh dich tren man hinh moi giay.
Neu cac khung KHONG cach deu nhau (khung nay 6,9 ms, khung sau 13,9 ms) thi vat dang di chuyen se bi NHAN DOI /
NHOE tren mat du tung khung deu sac net - dung trieu chung. Dong [REP3] hien chi in 'present TB x ms' la trung binh,
khong thay do lech.
Ban nay do KHOANG CACH GIUA HAI LAN TRINH KHUNG bang dong ho hieu nang (khong phai timeGetTime 1 ms), va in vao
dong [REP3] co san: 'nhip trinh: TB x.xx ms, min x.xx, max x.xx, lech chuan x.xx, so khung tre > 1,5 lan: N'.
O 144 Hz co vsync, so dung phai la TB 6,94 va lech chuan gan 0. Neu lech chuan lon hoac co khung tre thi nhip
trinh chinh la nguyen nhan, va sua o do. Khong them khoa cau hinh, khong doi hanh vi ve."""
import io, re, sys

P = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
NL = "\r\n"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


if "g_dTrinhTong" in s:
    print("da va roi"); sys.exit(0)

# bien do
s = rep(s, "int  g_nRep3Vsync     = 0;" + NL,
    "int  g_nRep3Vsync     = 0;" + NL +
    "// [TRINH 08/09] do khoang cach giua hai lan trinh khung bang dong ho hieu nang" + NL +
    "static LARGE_INTEGER s_liTrinhTruoc = {0};" + NL +
    "static double g_dTrinhTong = 0.0, g_dTrinhBinh = 0.0, g_dTrinhMin = 0.0, g_dTrinhMax = 0.0;" + NL +
    "static unsigned g_uTrinhDem = 0, g_uTrinhTre = 0;" + NL)

# do ngay sau Present
s = rep(s, "\tPD3DDEVICE->Present(NULL,NULL,NULL,NULL);" + NL,
    "\tPD3DDEVICE->Present(NULL,NULL,NULL,NULL);" + NL +
    "\t{\t// [TRINH 08/09] khoang cach hai lan trinh khung (chinh xac hon timeGetTime 1 ms)" + NL +
    "\t\tLARGE_INTEGER liNay; QueryPerformanceCounter(&liNay);" + NL +
    "\t\tif (s_liTrinhTruoc.QuadPart)" + NL +
    "\t\t{" + NL +
    "\t\t\tconst double dMs = Rep3NapMs(s_liTrinhTruoc, liNay);" + NL +
    "\t\t\tif (dMs > 0.0 && dMs < 200.0)" + NL +
    "\t\t\t{" + NL +
    "\t\t\t\tg_dTrinhTong += dMs; g_dTrinhBinh += dMs * dMs; g_uTrinhDem++;" + NL +
    "\t\t\t\tif (g_dTrinhMin <= 0.0 || dMs < g_dTrinhMin) g_dTrinhMin = dMs;" + NL +
    "\t\t\t\tif (dMs > g_dTrinhMax) g_dTrinhMax = dMs;" + NL +
    "\t\t\t\tif (g_uTrinhDem > 8 && dMs > (g_dTrinhTong / g_uTrinhDem) * 1.5) g_uTrinhTre++;" + NL +
    "\t\t\t}" + NL +
    "\t\t}" + NL +
    "\t\ts_liTrinhTruoc = liNay;" + NL +
    "\t}" + NL)

# in vao dong [REP3]
s = rep(s, "\t\t\tg_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0;",
    "\t\t\tif (g_uTrinhDem > 1)" + NL +
    "\t\t\t{\t// [TRINH 08/09] nhip trinh khung: o 144 Hz co vsync phai la TB 6,94 va lech chuan gan 0" + NL +
    "\t\t\t\tconst double dTb = g_dTrinhTong / g_uTrinhDem;" + NL +
    "\t\t\t\tdouble dPhuongSai = g_dTrinhBinh / g_uTrinhDem - dTb * dTb;" + NL +
    "\t\t\t\tif (dPhuongSai < 0.0) dPhuongSai = 0.0;" + NL +
    "\t\t\t\tRep3Log(\"[REP3-TRINH] %u khung: nhip trinh TB %.2f ms, min %.2f, max %.2f, lech chuan %.2f | khung tre (> 1,5 lan TB): %u\"," + NL +
    "\t\t\t\t\tg_uTrinhDem, dTb, g_dTrinhMin, g_dTrinhMax, sqrt(dPhuongSai), g_uTrinhTre);" + NL +
    "\t\t\t}" + NL +
    "\t\t\tg_dTrinhTong = g_dTrinhBinh = g_dTrinhMin = g_dTrinhMax = 0.0; g_uTrinhDem = g_uTrinhTre = 0;" + NL +
    "\t\t\tg_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0;")

if sum(1 for c in s if ord(c) >= 0x80) != h0:
    print("FAIL byte cao"); sys.exit(1)
if re.search(r"[^\r]\n", s):
    print("FAIL LF"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp"); print("XONG TRINH")
