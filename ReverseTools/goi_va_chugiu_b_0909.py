# -*- coding: utf-8 -*-
"""goi_va_chugiu_b_0909.py - [CHUGIU 09/09 b] Rep3ChuGiu: bang bam 2048 o (do tuyen tinh <= 16) thay duyet tuyen tinh 512 o
moi dong chu, va lay gio MOT LAN moi khung (g_liRep3VeBegin, da co o RepresentBegin) thay QueryPerformanceCounter moi dong chu.
Ho so VE 16:0x: Rep3ChuGiu 2,1 % + QPC (ntdll +62F0E 2,4 %) - vai tram dong chu/khung x 512 so sanh + 1 QPC.
Hanh vi giu chu (12 ms) y nguyen; chi doi cach tim muc va nguon gio (gio dau khung, cung mot gia tri cho moi chu trong khung).
"""
import io
import sys

NL = "\r\n"
T = "\t"
P = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/KRepresentShell3.cpp"
TAG = "[CHUGIU 09/09 b]"


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


def rep_khoang(s, dau, cuoi, new, ten):
    if s.count(dau) != 1:
        print("FAIL neo %s: dau %d" % (ten, s.count(dau))); sys.exit(1)
    a = s.find(dau); b = s.find(cuoi, a + len(dau))
    if b < 0:
        print("FAIL neo %s: khong thay cuoi" % ten); sys.exit(1)
    return s[:a] + new + s[b + len(cuoi):]


s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80); lf0 = s.count("\n") - s.count("\r\n")
if TAG in s:
    print("da co"); sys.exit(0)
s = rep(s, "static KRep3ChuGiu s_ChuGiu[512];" + NL + "static int         s_nChuGiuKe = 0;",
        "static KRep3ChuGiu s_ChuGiu[2048];" + T + "// " + TAG + " bang bam mo, do tuyen tinh toi da 16 o (truoc: vong 512 o duyet tuyen tinh moi dong chu)" + NL +
        "static int         s_nChuGiuKe = 0;",
        "bang")
dau = "static bool Rep3ChuGiu(const char* psText, int nCount, int nFont, int& nX, int& nY)" + NL + "{"
cuoi = T + "e.uBam = uBam; e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++;" + NL + T + "return false;" + NL + "}"
new = NL.join([
    "static bool Rep3ChuGiu(const char* psText, int nCount, int nFont, int& nX, int& nY)",
    "{",
    T + "if (g_nRep3ChuGiuMs <= 0) return false;",
    T + "if (s_dChuGiuF == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dChuGiuF = (double)f.QuadPart / 1000.0; }",
    T + "// " + TAG + " gio dau khung (RepresentBegin) - cung mot gia tri cho moi dong chu trong khung, khong QPC moi dong",
    T + "const double dNow = (double)g_liRep3VeBegin.QuadPart / s_dChuGiuF;",
    T + "const unsigned uBam = Rep3BamChu(psText, nCount, nFont);",
    T + "int nTrong = -1; int nCu = -1; double dCuNhat = 0.0;",
    T + "for (int k = 0; k < 16; k++)",
    T + "{",
    T*2 + "const int i = (int)((uBam + (unsigned)k) & 2047);",
    T*2 + "KRep3ChuGiu& e = s_ChuGiu[i];",
    T*2 + "if (e.dLuc == 0.0) { if (nTrong < 0) nTrong = i; break; }" + T + "// o trong: chuoi do ket thuc",
    T*2 + "if (nCu < 0 || e.dLuc < dCuNhat) { nCu = i; dCuNhat = e.dLuc; }",
    T*2 + "if (e.uBam != uBam) continue;",
    T*2 + "int dx = nX - e.nX; if (dx < 0) dx = -dx;",
    T*2 + "int dy = nY - e.nY; if (dy < 0) dy = -dy;",
    T*2 + "if (dx > 24 || dy > 24) continue;",
    T*2 + "if (dNow - e.dLuc < (double)g_nRep3ChuGiuMs) { nX = e.nX; nY = e.nY; g_uRep3ChuGiu++; return true; }",
    T*2 + "e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++; return false;",
    T + "}",
    T + "KRep3ChuGiu& e = s_ChuGiu[nTrong >= 0 ? nTrong : (nCu >= 0 ? nCu : (int)(uBam & 2047))];" + T + "// o trong, khong thi o cu nhat trong chuoi do",
    T + "e.uBam = uBam; e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++;",
    T + "return false;",
    "}",
])
s = rep_khoang(s, dau, cuoi, new, "ham")
if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp " + TAG)
