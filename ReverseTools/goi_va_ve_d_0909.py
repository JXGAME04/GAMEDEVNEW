# -*- coding: utf-8 -*-
"""goi_va_ve_d_0909.py - [VE 09/09 d] Represent3: cat chi phi cua chinh bo do [VE] va khoa thua trong luc ve.

Ho so VE 15:5x (DOLUOT, 97 292 mau, moi khung): ntdll 15,7 % trong do RtlQueryPerformanceCounter ~4,4 % (= Rep3VeDpTimer 2 lan QPC
moi DrawPrimitives ~5 000 lenh/khung) + Enter/LeaveCriticalSection ~3,6 % (m_ImageProcessLock moi GetImage ~3 000 lan/khung);
ucrtbase ~4 % (_strnicmp x4 moi don vi trong Rep3VeDem). TextureResMgr chi luong chinh goi (luong nap nen chi dung m_napKhoa; luong
nap vung KScenePlaceC chi doc pak; PrerenderGround chay o Paint luong chinh).
Sua: (1) [VE] lay mau 1/8 khung: Rep3VeDpTimer, Rep3VeDem, cong don DrawPrimitives chi tren khung mau (g_nRep3VeMau); so khung in la
so khung MAU; (2) DrawPrimitives giu khoa NGOAI mot lan (m_dwKhoaNgoai = luong dang giu), GetImage bo khoa khi cung luong (van dung
neu luong khac goi: ho khoa binh thuong, luong chinh giu khoa suot DrawPrimitives); (3) Release sinh Represent3.map de tra ten DOLUOT.
"""
import io
import sys

NL = "\r\n"
T = "\t"
R3 = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
F_SH = R3 + "KRepresentShell3.cpp"
F_TH = R3 + "TextureResMgr.h"
F_TC = R3 + "TextureResMgr.cpp"
F_VC = R3 + "Represent3.vcxproj"
TAG = "[VE 09/09 d]"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- TextureResMgr.h: khoa ngoai
s, c0, l0, h0 = doc(F_TH)
if TAG not in s:
    old = "    KCriticalSection    m_ImageProcessLock;"
    new = NL.join([
        old,
        "public:",
        T + "// " + TAG + " khoa NGOAI: KRepresentShell3::DrawPrimitives giu m_ImageProcessLock mot lan cho ca lo; GetImage tren cung luong bo khoa",
        T + "DWORD" + T + "m_dwKhoaNgoai;" + T + "// id luong dang giu khoa ngoai, 0 = khong",
        T + "void KhoaNgoaiVao() { m_ImageProcessLock.Lock(); m_dwKhoaNgoai = GetCurrentThreadId(); }",
        T + "void KhoaNgoaiRa()  { m_dwKhoaNgoai = 0; m_ImageProcessLock.UnLock(); }",
        T + "struct KhoaTuyChon" + T + "// khoa neu luong nay KHONG dang giu khoa ngoai",
        T + "{",
        T*2 + "KCriticalSection* m_p;",
        T*2 + "KhoaTuyChon(KCriticalSection& k, bool bKhoa) : m_p(bKhoa ? &k : NULL) { if (m_p) m_p->Lock(); }",
        T*2 + "~KhoaTuyChon() { if (m_p) m_p->UnLock(); }",
        T + "};",
    ])
    s = rep(s, old, new, "TH khoa ngoai")
    ghi(F_TH, s, c0, l0, h0, "TextureResMgr.h")
else:
    print("TextureResMgr.h da co")

# ---------------------------------------------------------------- TextureResMgr.cpp
s, c0, l0, h0 = doc(F_TC)
if TAG not in s:
    s = rep(s, T + "m_nNapTruocKip[0] = m_nNapTruocKip[1] = m_nNapTruocKip[2] = 0; m_nNapTruocTre[0] = m_nNapTruocTre[1] = m_nNapTruocTre[2] = 0;",
            T + "m_nNapTruocKip[0] = m_nNapTruocKip[1] = m_nNapTruocKip[2] = 0; m_nNapTruocTre[0] = m_nNapTruocTre[1] = m_nNapTruocTre[2] = 0;" + NL +
            T + "m_dwKhoaNgoai = 0;" + T + "// " + TAG,
            "TC ctor")
    old = "    KAutoCriticalSection AutoLock(m_ImageProcessLock);" + NL + T + "if (m_bVeDangDien) Rep3VeDem(pszImage);" + T + "// [VE 08/09 a] dem don vi ve theo loai"
    new = "    KhoaTuyChon AutoLock(m_ImageProcessLock, m_dwKhoaNgoai != GetCurrentThreadId());" + T + "// " + TAG + " DrawPrimitives dang giu khoa ngoai tren luong nay -> bo" + NL + \
          T + "{ extern int g_nRep3VeMau; if (m_bVeDangDien && g_nRep3VeMau) Rep3VeDem(pszImage); }" + T + "// [VE 08/09 a] dem don vi ve theo loai; " + TAG + " chi khung mau (1/8)"
    s = rep(s, old, new, "TC GetImage")
    ghi(F_TC, s, c0, l0, h0, "TextureResMgr.cpp")
else:
    print("TextureResMgr.cpp da co")

# ---------------------------------------------------------------- KRepresentShell3.cpp
s, c0, l0, h0 = doc(F_SH)
if TAG not in s:
    old = "struct Rep3VeDpTimer { LARGE_INTEGER a; Rep3VeDpTimer() { QueryPerformanceCounter(&a); } ~Rep3VeDpTimer() { LARGE_INTEGER b; QueryPerformanceCounter(&b); g_dRep3VeDpKhung += Rep3NapMs(a, b); } };"
    new = NL.join([
        "int g_nRep3VeMau = 0;" + T + "// " + TAG + " 1 = khung nay la khung MAU (1/8): moi do [VE] theo don vi/lenh chi chay tren khung mau (QPC 2 lan/lenh x 5 000 lenh/khung = 4 % thoi gian ve)",
        "struct Rep3VeDpTimer { LARGE_INTEGER a; Rep3VeDpTimer() { if (g_nRep3VeMau) QueryPerformanceCounter(&a); } ~Rep3VeDpTimer() { if (g_nRep3VeMau) { LARGE_INTEGER b; QueryPerformanceCounter(&b); g_dRep3VeDpKhung += Rep3NapMs(a, b); } } };",
    ])
    s = rep(s, old, new, "SH timer")
    # RepresentBegin: chon khung mau
    old = T + "QueryPerformanceCounter(&g_liRep3VeBegin);" + T + "// [VE 08/09 b]"
    new = old + NL + T + "{ static unsigned s_uKhung = 0; g_nRep3VeMau = ((++s_uKhung) & 7) == 0; }" + T + "// " + TAG
    s = rep(s, old, new, "SH begin")
    # RepresentEnd: chi cong don tren khung mau
    old = NL.join([
        T + "g_uRep3VeKhung++;" + T + "// [VE 08/09 a]",
        T + "{" + T + "// [VE 08/09 b] CPU pass ve khung nay",
        T*2 + "LARGE_INTEGER liNow; QueryPerformanceCounter(&liNow);",
        T*2 + "const double dKhung = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, liNow) : 0.0;",
        T*2 + "g_dRep3VeKhungTong += dKhung; if (dKhung > g_dRep3VeKhungMax) g_dRep3VeKhungMax = dKhung;",
        T*2 + "g_dRep3VeDpTong += g_dRep3VeDpKhung; if (g_dRep3VeDpKhung > g_dRep3VeDpMax) g_dRep3VeDpMax = g_dRep3VeDpKhung;",
        T*2 + "g_dRep3VeDpKhung = 0.0;",
        T + "}",
    ])
    new = NL.join([
        T + "if (g_nRep3VeMau)" + T + "// " + TAG + " chi khung mau: dem don vi + thoi gian DrawPrimitives + thoi gian khung (TB/khung khong doi, so khung in = khung mau)",
        T + "{",
        T*2 + "g_uRep3VeKhung++;" + T + "// [VE 08/09 a]",
        T*2 + "LARGE_INTEGER liNow; QueryPerformanceCounter(&liNow);" + T + "// [VE 08/09 b] CPU pass ve khung nay",
        T*2 + "const double dKhung = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, liNow) : 0.0;",
        T*2 + "g_dRep3VeKhungTong += dKhung; if (dKhung > g_dRep3VeKhungMax) g_dRep3VeKhungMax = dKhung;",
        T*2 + "g_dRep3VeDpTong += g_dRep3VeDpKhung; if (g_dRep3VeDpKhung > g_dRep3VeDpMax) g_dRep3VeDpMax = g_dRep3VeDpKhung;",
        T*2 + "g_dRep3VeDpKhung = 0.0;",
        T + "}",
    ])
    s = rep(s, old, new, "SH end")
    s = rep(s, "khac %.0f (khung %u)", "khac %.0f (khung mau %u)", "SH fmt khung")
    # DrawPrimitives: khoa ngoai
    old = "void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)" + NL + "{" + NL + T + "Rep3VeDpTimer veDp;" + T + "// [VE 08/09 b]"
    new = NL.join([
        "// " + TAG + " giu khoa TextureResMgr mot lan cho ca lo lenh (GetImage tren cung luong bo khoa; ~3 000 cap Enter/Leave/khung)",
        "struct Rep3KhoaNgoai { TextureResMgr& m; Rep3KhoaNgoai(TextureResMgr& t) : m(t) { m.KhoaNgoaiVao(); } ~Rep3KhoaNgoai() { m.KhoaNgoaiRa(); } };",
        "void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)",
        "{",
        T + "Rep3VeDpTimer veDp;" + T + "// [VE 08/09 b]",
        T + "Rep3KhoaNgoai khoaNgoai(m_TextureResMgr);" + T + "// " + TAG,
    ])
    s = rep(s, old, new, "SH khoa ngoai")
    ghi(F_SH, s, c0, l0, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

# ---------------------------------------------------------------- Represent3.vcxproj: map cho Release|Win32
s = io.open(F_VC, "r", encoding="utf-8", newline="").read()
if "GenerateMapFile" not in s:
    moc = "<ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|Win32'\">"
    a = s.find(moc); k = s.find("</ItemDefinitionGroup>", a)
    old = "      <GenerateDebugInformation>true</GenerateDebugInformation>" + NL
    b = s.find(old, a)
    if a < 0 or b < 0 or b > k:
        print("FAIL vcxproj neo"); sys.exit(1)
    BS = chr(92)
    new = old + "      <GenerateMapFile>true</GenerateMapFile>" + NL + "      <MapFileName>." + BS + "Release" + BS + "Represent3.map</MapFileName>" + NL
    s = s[:b] + new + s[b + len(old):]
    io.open(F_VC, "w", encoding="utf-8", newline="").write(s)
    print("OK Represent3.vcxproj map")
else:
    print("vcxproj da co")
print("XONG VE d")
