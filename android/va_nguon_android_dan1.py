# -*- coding: utf-8 -*-
r"""[DAN 11/09] Bo do chi phi DAN (KMissle::Activate) tren Android - buoc 1 cua viec "khong tut FPS khi dong nguoi choi".

Fold 7 00:31-00:43 (BANGIAO_DONHIP_MOBILE_1209.md muc 9): luc dam dong toi, "dan" 690-790 vien/tick ton 4,3-5,3 ms/tick tren luong chinh
(bucket g_dKhacMs[1] = vong KRegion::Activate goi Missle[i].Activate() cho m_MissleList cua 9 vung) -> tick 10-15 ms -> 53-95 fps.
7 us/vien la qua lon so voi ma da doc -> phai DO tung phan truoc khi sua (chu quyet: co so do roi moi chon cach sua).

Chi JX_ANDROID (KMissle.cpp / KSubWorldSet.cpp dich chung cho PC: moi doi ma nam trong #ifdef JX_ANDROID, nhanh #else giu nguyen van).
Chi do khi [Client] PaintLog > 0 (g_nCorePaintLog) - moi vien 4-6 lan QueryPerformanceCounter (~30 ns) = ~0,1 ms/tick khi 700 vien.
In moi 10 s vao jx_paint.log, ngay sau [WORLD b]:
  [DAN] 10s: act A (a/tick) tong T ms/tick = u us/vien | onfly F (barrier B xNb, beyond Y, col C xNc: findnpc Nf hit Nh) | move M | khac K

Doc/ghi latin-1 (GBK/TCVN3 trong chu thich, CRLF), moc lay NGUYEN VAN tu tep bang regex (khong lech khoang trang cuoi dong),
moi moc khop dung 1 cho, so byte cao khong doi; chay lai nhieu lan khong sao.
Dung:  python android\va_nguon_android_dan1.py
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DAN 11/09]"


def doc(p):
    s = io.open(p, encoding="latin-1", newline="").read()
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    return s, nl


def ghi(p, s, cao):
    if sum(1 for c in s if ord(c) >= 0x80) != cao:
        raise SystemExit("so byte cao doi - khong ghi: " + p)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da va:", os.path.relpath(p, GOC))


def dong(s, mau, ten):
    """tra ve NGUYEN VAN dong (khong ke ket dong) khop regex `mau`, phai dung 1 dong; tep CRLF: `$` cua re.M dung truoc LF, dong con CR -> bo"""
    t = s.replace(chr(13) + chr(10), chr(10))   # khop tren ban LF de `$` cua re.M dung cuoi dong; dong tra ve khong chua CR
    ds = [m.group(0) for m in re.finditer(mau, t, re.M)]
    if len(ds) != 1:
        raise SystemExit("moc '%s' khop %d dong (can 1)" % (ten, len(ds)))
    return ds[0]


def thay(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(cu, moi)


# ---------------------------------------------------------------- 1. KMissle.cpp
p = os.path.join(GOC, "Sources", "Core", "Src", "KMissle.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    # 1a. bo dem + ham do, ngay sau #define VHLOG
    d = dong(s, r"^#define VHLOG\(\.\.\.\).*$", "VHLOG")
    s = thay(s, d, d + nl + nl.join([
        "#ifdef JX_ANDROID",
        "// " + DAU + " do chi phi tung phan cua KMissle::Activate (in [DAN] moi 10 s o KSubWorldSet.cpp, chi khi [Client] PaintLog > 0).",
        "// Xem android/va_nguon_android_dan1.py + BANGIAO_DONHIP_MOBILE_1209.md muc 9 (Fold 7: 690-790 vien/tick ton 4-5 ms/tick luc dong).",
        "extern int g_nCorePaintLog;",
        "unsigned g_uDanAct = 0, g_uDanCol = 0, g_uDanFind = 0, g_uDanHit = 0, g_uDanBar = 0;",
        "double   g_dDanTong = 0.0, g_dDanOnFly = 0.0, g_dDanBar = 0.0, g_dDanBeyond = 0.0, g_dDanCol = 0.0, g_dDanMove = 0.0;",
        "static inline double DanMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)",
        "{",
        "\tstatic double s_dTanSo = 0.0;",
        "\tif (s_dTanSo <= 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dTanSo = (double)f.QuadPart; if (s_dTanSo <= 0.0) s_dTanSo = 1.0; }",
        "\treturn (double)(b.QuadPart - a.QuadPart) * 1000.0 / s_dTanSo;",
        "}",
        "#endif"]), "bo dem")
    # 1b. Activate: bat dau do sau cua chan id/region
    cu = nl.join(["\tif (m_nMissleId <= 0 || m_nRegionId < 0)", "\t{", "\t\treturn  0 ;", "\t}"])
    s = thay(s, cu, cu + nl + nl.join([
        "#ifdef JX_ANDROID",
        "\tconst bool bDanDo = g_nCorePaintLog > 0; LARGE_INTEGER liDan0, liDanA, liDanB;\t// " + DAU,
        "\tif (bDanDo) { QueryPerformanceCounter(&liDan0); g_uDanAct++; }",
        "#endif"]), "Activate bat dau")
    # 1c. OnFly() trong switch
    cu = nl.join(["\t\t\tOnFly();", "\t\t\tif (m_bFlyEvent)"])
    s = thay(s, cu, nl.join([
        "#ifdef JX_ANDROID",
        "\t\t\tif (bDanDo) QueryPerformanceCounter(&liDanA);\t// " + DAU,
        "#endif",
        "\t\t\tOnFly();",
        "#ifdef JX_ANDROID",
        "\t\t\tif (bDanDo) { QueryPerformanceCounter(&liDanB); g_dDanOnFly += DanMs(liDanA, liDanB); }",
        "#endif",
        "\t\t\tif (m_bFlyEvent)"]), "OnFly")
    # 1d. MoveObject cuoi Activate + tong
    cu = nl.join(["\t\tif (m_usLightRadius && m_eMissleStatus != MS_DoWait)",
                  "\t\t\tg_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_LIGHT_PROP );",
                  "\t\telse",
                  "\t\t\tg_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT);",
                  "\t}",
                  "\t", "#endif",
                  "\tm_nCurrentLife ++;",
                  "\treturn 1;"])
    if s.count(cu) != 1:
        cu = cu.replace("\t}" + nl + "\t" + nl + "#endif", "\t}" + nl + nl + "#endif")
    s = thay(s, cu, nl.join(["#ifdef JX_ANDROID",
                             "\t\tif (bDanDo) QueryPerformanceCounter(&liDanA);\t// " + DAU,
                             "#endif",
                             "\t\tif (m_usLightRadius && m_eMissleStatus != MS_DoWait)",
                             "\t\t\tg_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_LIGHT_PROP );",
                             "\t\telse",
                             "\t\t\tg_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT);",
                             "#ifdef JX_ANDROID",
                             "\t\tif (bDanDo) { QueryPerformanceCounter(&liDanB); g_dDanMove += DanMs(liDanA, liDanB); }",
                             "#endif",
                             "\t}",
                             "\t", "#endif",
                             "#ifdef JX_ANDROID",
                             "\tif (bDanDo) { QueryPerformanceCounter(&liDanB); g_dDanTong += DanMs(liDan0, liDanB); }\t// " + DAU,
                             "#endif",
                             "\tm_nCurrentLife ++;",
                             "\treturn 1;"]), "MoveObject + tong")
    # 1e. OnFly: bien do
    cu = nl.join(["void KMissle::OnFly()", "{"])
    s = thay(s, cu, cu + nl + nl.join(["#ifdef JX_ANDROID",
                                       "\tconst bool bDanDoB = g_nCorePaintLog > 0; LARGE_INTEGER liDb0, liDb1;\t// " + DAU,
                                       "#endif"]), "OnFly dau")
    # 1f. TestBarrier
    d = dong(s, r"^\tif \(TestBarrier\(\)\)[ \t]*\r?$", "TestBarrier")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID",
                            "\tif (bDanDoB) QueryPerformanceCounter(&liDb0);\t// " + DAU,
                            "\tconst BOOL bDanBarrier = TestBarrier();",
                            "\tif (bDanDoB) { QueryPerformanceCounter(&liDb1); g_dDanBar += DanMs(liDb0, liDb1); g_uDanBar++; }",
                            "\tif (bDanBarrier)",
                            "#else",
                            d,
                            "#endif"]), "TestBarrier")
    # 1g. CheckBeyondRegion + CheckCollision
    d1 = dong(s, r"^\tif \(CheckBeyondRegion\(nDOffsetX, nDOffsetY\)\)[ \t]*\r?$", "CheckBeyondRegion")
    d2 = dong(s, r"^\t\tif \(CheckCollision\(\) == -1\)[ \t]*\r?$", "CheckCollision == -1")
    cu = nl.join([d1, "\t{", d2])
    s = thay(s, cu, nl.join(["#ifdef JX_ANDROID",
                             "\tif (bDanDoB) QueryPerformanceCounter(&liDb0);\t// " + DAU,
                             "\tconst BOOL bDanBeyond = CheckBeyondRegion(nDOffsetX, nDOffsetY);",
                             "\tif (bDanDoB) { QueryPerformanceCounter(&liDb1); g_dDanBeyond += DanMs(liDb0, liDb1); }",
                             "\tif (bDanBeyond)",
                             "\t{",
                             "\t\tif (bDanDoB) QueryPerformanceCounter(&liDb0);",
                             "\t\tconst int nDanCol = CheckCollision();",
                             "\t\tif (bDanDoB) { QueryPerformanceCounter(&liDb1); g_dDanCol += DanMs(liDb0, liDb1); g_uDanCol++; }",
                             "\t\tif (nDanCol == -1)",
                             "#else",
                             d1, "\t{", d2,
                             "#endif"]), "Beyond + Col")
    # 1h. dem FindNpc (2 cho) + va cham (DoCollision)
    d = dong(s, r"^\t\t\tnNpcIdx = SubWorld\[m_nSubWorldId\]\.m_Region\[nSearchRegion\]\.FindNpc\(nRMx, nRMy, m_nLauncher, m_eRelation, m_nFollowNpcIdx\);$", "FindNpc gan")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID", "\t\t\tg_uDanFind++;\t// " + DAU, "#endif", d]), "FindNpc gan")
    d = dong(s, r"^\t\t\t\tnNpcIdx = SubWorld\[m_nSubWorldId\]\.m_Region\[nSearchRegion\]\.FindNpc\(nRMx, nRMy, m_nLauncher, m_eRelation, nVhPrefer\);$", "FindNpc rong")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID", "\t\t\t\tg_uDanFind++;\t// " + DAU, "#endif", d]), "FindNpc rong")
    cu = nl.join(["void KMissle::DoCollision()", "{"])
    s = thay(s, cu, cu + nl + nl.join(["#ifdef JX_ANDROID", "\tg_uDanHit++;\t// " + DAU, "#endif"]), "DoCollision")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 2. KSubWorldSet.cpp: in [DAN] sau [WORLD b]
p = os.path.join(GOC, "Sources", "Core", "Src", "KSubWorldSet.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    d = dong(s, r"^\t\tg_dWorldNhac = 0\.0; g_dNpcPha\[0\] = .*$", "reset WORLD b")
    s = thay(s, d, nl.join([
        "#ifdef JX_ANDROID",
        "\t\t{\t// " + DAU + " chi phi tung phan cua dan (KMissle.cpp), cung ky 10 s voi [WORLD b]",
        "\t\t\textern unsigned g_uDanAct, g_uDanCol, g_uDanFind, g_uDanHit, g_uDanBar;",
        "\t\t\textern double g_dDanTong, g_dDanOnFly, g_dDanBar, g_dDanBeyond, g_dDanCol, g_dDanMove;",
        "\t\t\tFILE* pD = fopen(\"jx_paint.log\", \"a\");",
        "\t\t\tif (pD && g_uWorldTick)",
        "\t\t\t{",
        "\t\t\t\tconst double dT = (double)g_uWorldTick;",
        "\t\t\t\tfprintf(pD, \"[DAN] 10s: act %u (%.1f/tick) tong %.2f ms/tick = %.1f us/vien | onfly %.2f ms/tick (barrier %.3f x%u, beyond %.3f, col %.3f x%u: findnpc %u hit %u) | move %.3f | khac %.3f\\n\",",
        "\t\t\t\t\tg_uDanAct, g_uDanAct / dT, g_dDanTong / dT, g_uDanAct ? g_dDanTong * 1000.0 / g_uDanAct : 0.0,",
        "\t\t\t\t\tg_dDanOnFly / dT, g_dDanBar / dT, g_uDanBar, g_dDanBeyond / dT, g_dDanCol / dT, g_uDanCol, g_uDanFind, g_uDanHit,",
        "\t\t\t\t\tg_dDanMove / dT, (g_dDanTong - g_dDanOnFly - g_dDanMove) / dT);",
        "\t\t\t}",
        "\t\t\tif (pD) fclose(pD);",
        "\t\t\tg_uDanAct = g_uDanCol = g_uDanFind = g_uDanHit = g_uDanBar = 0;",
        "\t\t\tg_dDanTong = g_dDanOnFly = g_dDanBar = g_dDanBeyond = g_dDanCol = g_dDanMove = 0.0;",
        "\t\t}",
        "#endif",
        d]), "in [DAN]")
    ghi(p, s, cao)

print("xong")
