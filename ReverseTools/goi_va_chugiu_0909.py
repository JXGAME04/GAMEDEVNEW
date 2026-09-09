# -*- coding: utf-8 -*-
"""goi_va_chugiu_0909.py - [CHUGIU 09/09] Giu vi tri MAN HINH cua chu (ten / danh hieu / so sat thuong)
toi thieu Rep3ChuGiuMs mili-giay; the gioi van ve moi khung.

Chu thu ba cach:
  - cap nhat moi khung (7 ms)        : am mau
  - giu 2 khung (14 ms, NhipTheGioi) : HET AM, nhung the gioi giat
  - loc thoi gian (doi xung / chon loc): "van toi den dam khi di chuyen" - lam cham tin hieu = mat dinh sang
=> Thu duy nhat vua het am vua khong toi la GIU 14 ms. Nhung giu ca the gioi thi giat. Chu am nang nhat
   la CHU (net 1 px, tuong phan cao). Vay: chi giu CHU, ngay sau khi doi toa do the gioi -> man hinh
   trong KRepresentShell3::OutputText; than nguoi, dat, sprite van muot moi khung.
Theo THOI GIAN (khong theo khung) nen tu thich nghi: 60 Hz (16,7 ms) khong bao gio giu; 143 Hz giu 1 khung
(=2 khung/vi tri); 240 Hz giu 2 khung. Chu lech than toi da 1 khung (~6 px khi chay).
Khoa: [Client] Rep3ChuGiuMs = 12 (mac dinh; 0 = tat). Rep3LocMs mac dinh -> 0 (giu lam cong tac).
Nhan dien "cung mot dong chu": bam chuoi + font, va vi tri man hinh moi cach vi tri dang giu <= 24 px
(cung ten o hai NPC khac nhau thi cach xa nhau). Bang vong 512 muc, tim tuyen tinh.
"""
import io
import sys

NL = "\r\n"
T = "\t"
P = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)
lf0 = s.count("\n") - s.count("\r\n")


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


if "[CHUGIU 09/09]" in s:
    print("da va roi"); sys.exit(0)

# 1) bien + bang giu, dat truoc OutputText
old = "void KRepresentShell3::OutputText(int nFontId, const char* psText, int nCount, int nX, "
new = NL.join([
    "// [CHUGIU 09/09] Giu vi tri MAN HINH cua chu toi thieu Rep3ChuGiuMs ms (xem ReverseTools/goi_va_chugiu_0909.py).",
    "// Chu A/B: giu 14 ms thi het am ma khong toi; giu ca the gioi thi giat => chi giu CHU. Theo thoi gian nen",
    "// 60 Hz khong giu, 143 Hz giu 1 khung, 240 Hz giu 2 khung. Nhan dien dong chu = bam chuoi+font, va vi tri",
    "// man hinh moi cach vi tri dang giu <= 24 px.",
    "int      g_nRep3ChuGiuMs = 12;",
    "unsigned g_uRep3ChuGiu = 0, g_uRep3ChuVe = 0;",
    "struct KRep3ChuGiu { unsigned uBam; int nX, nY; double dLuc; };",
    "static KRep3ChuGiu s_ChuGiu[512];",
    "static int         s_nChuGiuKe = 0;",
    "static double      s_dChuGiuF = 0.0;",
    "static inline unsigned Rep3BamChu(const char* p, int n, int nFont)",
    "{",
    T + "unsigned h = 2166136261u ^ (unsigned)nFont;",
    T + "if (n < 0) { for (; *p; p++) { h ^= (unsigned char)*p; h *= 16777619u; } }",
    T + "else       { for (int i = 0; i < n && p[i]; i++) { h ^= (unsigned char)p[i]; h *= 16777619u; } }",
    T + "return h;",
    "}",
    "// tra ve true neu (nX, nY) da duoc thay bang vi tri dang giu",
    "static bool Rep3ChuGiu(const char* psText, int nCount, int nFont, int& nX, int& nY)",
    "{",
    T + "if (g_nRep3ChuGiuMs <= 0) return false;",
    T + "if (s_dChuGiuF == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dChuGiuF = (double)f.QuadPart / 1000.0; }",
    T + "LARGE_INTEGER q; QueryPerformanceCounter(&q);",
    T + "const double dNow = (double)q.QuadPart / s_dChuGiuF;",
    T + "const unsigned uBam = Rep3BamChu(psText, nCount, nFont);",
    T + "for (int i = 0; i < 512; i++)",
    T + "{",
    T + T + "KRep3ChuGiu& e = s_ChuGiu[i];",
    T + T + "if (e.uBam != uBam || e.dLuc == 0.0) continue;",
    T + T + "int dx = nX - e.nX; if (dx < 0) dx = -dx;",
    T + T + "int dy = nY - e.nY; if (dy < 0) dy = -dy;",
    T + T + "if (dx > 24 || dy > 24) continue;",
    T + T + "if (dNow - e.dLuc < (double)g_nRep3ChuGiuMs) { nX = e.nX; nY = e.nY; g_uRep3ChuGiu++; return true; }",
    T + T + "e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++; return false;",
    T + "}",
    T + "KRep3ChuGiu& e = s_ChuGiu[s_nChuGiuKe]; s_nChuGiuKe = (s_nChuGiuKe + 1) & 511;",
    T + "e.uBam = uBam; e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++;",
    T + "return false;",
    "}",
    "",
    old,
])
s = rep(s, old, new, "S bang giu")

# 2) ap ngay sau doi toa do, truoc khi ve
old = T + "m_FontTable[i].pFontObj->SetBorderColor(BorderColor);"
new = NL.join([
    T + "if (nZ != TEXT_IN_SINGLE_PLANE_COORD)",
    T + T + "Rep3ChuGiu(psText, nCount, nFontId, nX, nY);" + T + "// [CHUGIU 09/09] chi chu neo vao the gioi",
    old,
])
s = rep(s, old, new, "S ap dung")

# 3) config: Rep3ChuGiuMs; Rep3LocMs mac dinh ve 0
s = rep(s, T + "g_nRep3LocMs     = Rep3Ini(\"Rep3LocMs\", 8);" + T + "// [LOCTG 09/09]",
        T + "g_nRep3LocMs     = Rep3Ini(\"Rep3LocMs\", 0);" + T + "// [LOCTG 09/09] [CHUGIU] mac dinh TAT: chu che toi; giu lam cong tac" + NL +
        T + "g_nRep3ChuGiuMs  = Rep3Ini(\"Rep3ChuGiuMs\", 12);" + T + "// [CHUGIU 09/09]" + NL +
        T + "if (g_nRep3ChuGiuMs < 0) g_nRep3ChuGiuMs = 0;" + NL +
        T + "if (g_nRep3ChuGiuMs > 100) g_nRep3ChuGiuMs = 100;",
        "S config")
s = rep(s, "int  g_nRep3LocMs     = 8;", "int  g_nRep3LocMs     = 0;", "S mac dinh loc")

# 4) log
s = rep(s, T*3 + "g_uRep3LocKhung = 0;",
        T*3 + "g_uRep3LocKhung = 0;" + NL +
        T*3 + "Rep3Log(\"[CHUGIU] giu %d ms | dong chu giu %u, ve moi %u\", g_nRep3ChuGiuMs, g_uRep3ChuGiu, g_uRep3ChuVe);" + NL +
        T*3 + "g_uRep3ChuGiu = 0; g_uRep3ChuVe = 0;",
        "S log")

if sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s or s.count("\n") - s.count("\r\n") != lf0:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp [CHUGIU] (byte cao %d giu nguyen)" % h0)
