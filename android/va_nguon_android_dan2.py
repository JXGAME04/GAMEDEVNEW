# -*- coding: utf-8 -*-
r"""[DAN 11/09 b] Sua "tut FPS khi dong nguoi choi" - buoc 2, CHI Android, giu nguyen hanh vi game (khong cat hieu ung / NPC / dan).

Ket qua [DAN] tren may ao (va_nguon_android_dan1.py): moi CheckCollision goi ~55 lan FindNpc (quet o 7x7 quanh dan, moi o qua GetOffsetAxis),
va GetOffsetAxis mo dau bang AUTOLOG_EVERY. Voi [Client] AutoLog=1 (bo tai dien thoai dang bat) moi site AUTOLOG_EVERY = 1 lan
timeGetTime() = SDL_GetTicks() (PLT + clock_gettime, ~50-80 ns); moi vien moi tick ~60 lan (49 o + ~12 site trong Activate/OnFly)
-> 700 vien x 60 x ~70 ns ~ 3 ms/tick = dung co so do Fold 7 (dan 4-5 ms/tick luc dong).

Hai sua, ket qua game y het:
  (B) KCore.h: AUTOLOG_EVERY / AUTOLOG_IDX_EVERY tren Android so voi g_uAutoLogNow (cap nhat moi tick o KSubWorldSet::MainLoop va moi lan
      g_AutoLog ghi) thay vi goi timeGetTime() o MOI site. Nhip ghi log lech toi da 1 tick (~55 ms) so voi chu ky >= 500 ms cua cac site.
  (C) KMissle.cpp: ba vong quet o (CheckCollision, CheckNearestCollision, ProcessCollision) bo qua o TRONG VUNG khong co NPC
      (KRegion::JxSoNpcO() doc bo dem m_pNpcRef) TRUOC khi goi GetOffsetAxis/FindNpc - FindNpc von tra 0 cho o trong nen ket qua y het;
      o o bien vung (sang vung ke) van di duong cu. Khoa [Client] DanToiUu=0 tat (C) de doi chung.
Them do: [DAN] ... boqua (so o bo qua) | vacham (ms/tick trong ProcessCollision + DoCollision, RAII, khong dem long nhau).

Doc/ghi latin-1 (TCVN3/GBK, CRLF), moc lay nguyen van tu tep (khop tren ban LF), moi moc dung 1 cho, so byte cao khong doi; idempotent.
Dung:  python android\va_nguon_android_dan2.py   (SAU va_nguon_android_dan1.py)
"""
import io
import os
import re

GOC = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DAN 11/09 b]"


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
    t = s.replace(chr(13) + chr(10), chr(10))
    ds = [m.group(0) for m in re.finditer(mau, t, re.M)]
    if len(ds) != 1:
        raise SystemExit("moc '%s' khop %d dong (can 1)" % (ten, len(ds)))
    return ds[0]


def thay(s, cu, moi, ten):
    n = s.count(cu)
    if n != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (n, ten))
    return s.replace(cu, moi)


# ---------------------------------------------------------------- 1. KCore.h: macro _EVERY dung moc moi tick
p = os.path.join(GOC, "Sources", "Core", "Src", "KCore.h")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    d1 = dong(s, r"^#define AUTOLOG_EVERY\(ms, \.\.\.\).*$", "AUTOLOG_EVERY")
    d2 = dong(s, r"^#define AUTOLOG_IDX_EVERY\(idx, ms, \.\.\.\).*$", "AUTOLOG_IDX_EVERY")
    s = thay(s, d1, nl.join([
        "#ifdef JX_ANDROID",
        "// " + DAU + " Android: timeGetTime() = SDL_GetTicks() (PLT + clock_gettime ~50-80 ns) goi o MOI site moi lan qua (GetOffsetAxis: 49 o/vien/tick)",
        "// -> so voi g_uAutoLogNow (cap nhat moi tick KSubWorldSet::MainLoop + moi lan g_AutoLog). Nhip ghi lech toi da 1 tick. Xem android/va_nguon_android_dan2.py",
        "extern DWORD g_uAutoLogNow;",
        "#define AUTOLOG_EVERY(ms, ...)    do { static DWORD s_uAutoLogT = 0; if (g_AutoLogOn()) { if ((DWORD)(g_uAutoLogNow - s_uAutoLogT) >= (DWORD)(ms)) { s_uAutoLogT = g_uAutoLogNow; g_AutoLog(__VA_ARGS__); } } } while (0)",
        "#else",
        d1,
        "#endif"]), "AUTOLOG_EVERY")
    s = thay(s, d2, nl.join([
        "#ifdef JX_ANDROID",
        "#define AUTOLOG_IDX_EVERY(idx, ms, ...)   do { static DWORD s_uAutoLogTI = 0; if (g_AutoLogWhoIdx(idx)) { if ((DWORD)(g_uAutoLogNow - s_uAutoLogTI) >= (DWORD)(ms)) { s_uAutoLogTI = g_uAutoLogNow; g_AutoLog(__VA_ARGS__); } } } while (0)\t// " + DAU,
        "#else",
        d2,
        "#endif"]), "AUTOLOG_IDX_EVERY")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 2. KCore.cpp: dinh nghia + cap nhat khi ghi
p = os.path.join(GOC, "Sources", "Core", "Src", "KCore.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    d = dong(s, r"^void g_AutoLog\(const char\* szFmt, \.\.\.\)$", "g_AutoLog")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID",
                            "DWORD g_uAutoLogNow = 0;\t// " + DAU + " moc thoi gian cho AUTOLOG_EVERY (KCore.h), cap nhat moi tick + moi lan ghi",
                            "#endif",
                            d]), "g_AutoLog dau")
    cu = nl.join(["\tif (!g_AutoLogOn())", "\t\treturn;", "\tDWORD uNow = timeGetTime();"])
    s = thay(s, cu, cu + nl + nl.join(["#ifdef JX_ANDROID", "\tg_uAutoLogNow = uNow;\t// " + DAU, "#endif"]), "g_AutoLog uNow")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 3. KSubWorldSet.cpp: cap nhat moc moi tick + in boqua/vacham
p = os.path.join(GOC, "Sources", "Core", "Src", "KSubWorldSet.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    cu = nl.join(["void KSubWorldSet::MainLoop()", "{"])
    s = thay(s, cu, cu + nl + nl.join(["#ifdef JX_ANDROID",
                                       "\t{ extern DWORD g_uAutoLogNow; g_uAutoLogNow = timeGetTime(); }\t// " + DAU + " moc cho AUTOLOG_EVERY, 1 lan/tick",
                                       "#endif"]), "MainLoop")
    cu = "\t\t\textern double g_dDanTong, g_dDanOnFly, g_dDanBar, g_dDanBeyond, g_dDanCol, g_dDanMove;"
    s = thay(s, cu, cu + nl + "\t\t\textern unsigned g_uDanBoQua; extern double g_dDanVaCham;\t// " + DAU, "extern DAN")
    cu = "col %.3f x%u: findnpc %u hit %u) | move %.3f | khac %.3f\\n\","
    s = thay(s, cu, "col %.3f x%u: findnpc %u hit %u boqua %u vacham %.3f) | move %.3f | khac %.3f\\n\",", "format DAN")
    cu = "\t\t\t\t\tg_dDanOnFly / dT, g_dDanBar / dT, g_uDanBar, g_dDanBeyond / dT, g_dDanCol / dT, g_uDanCol, g_uDanFind, g_uDanHit,"
    s = thay(s, cu, "\t\t\t\t\tg_dDanOnFly / dT, g_dDanBar / dT, g_uDanBar, g_dDanBeyond / dT, g_dDanCol / dT, g_uDanCol, g_uDanFind, g_uDanHit, g_uDanBoQua, g_dDanVaCham / dT,", "doi so DAN")
    cu = "\t\t\tg_uDanAct = g_uDanCol = g_uDanFind = g_uDanHit = g_uDanBar = 0;"
    s = thay(s, cu, cu + " g_uDanBoQua = 0; g_dDanVaCham = 0.0;", "reset DAN")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 4. KRegion.h: JxSoNpcO
p = os.path.join(GOC, "Sources", "Core", "Src", "KRegion.h")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    d = dong(s, r"^\tint\t\t\tFindNpc\(int nMapX, int nMapY, int nNpcIdx, int nRelation, int nPreferIdx = 0\);$", "FindNpc khai bao")
    s = thay(s, d, d + nl + nl.join([
        "#ifdef JX_ANDROID",
        "\t// " + DAU + " so NPC dang dung tren mot o (bo dem m_pNpcRef, cung du lieu FindNpc dung de thoat som). KMissle bo qua o trong",
        "\t// TRONG VUNG truoc khi goi GetOffsetAxis/FindNpc - FindNpc von tra 0 khi o trong nen ket qua y het. Ngoai bien -> 0 (khong dung de bo qua).",
        "\tinline int\tJxSoNpcO(int nMapX, int nMapY) const { return (m_pNpcRef && nMapX >= 0 && nMapY >= 0 && nMapX < m_nWidth && nMapY < m_nHeight) ? (int)m_pNpcRef[nMapY * m_nWidth + nMapX] : 0; }",
        "\tinline bool\tJxOTrongVung(int nMapX, int nMapY) const { return nMapX >= 0 && nMapY >= 0 && nMapX < m_nWidth && nMapY < m_nHeight; }",
        "#endif"]), "JxSoNpcO")
    ghi(p, s, cao)

# ---------------------------------------------------------------- 5. KMissle.cpp: khoa + bo qua o trong + do vacham
p = os.path.join(GOC, "Sources", "Core", "Src", "KMissle.cpp")
s, nl = doc(p)
if DAU in s:
    print("da va roi, bo qua:", os.path.relpath(p, GOC))
else:
    cao = sum(1 for c in s if ord(c) >= 0x80)
    if "[DAN 11/09]" not in s:
        raise SystemExit("chay android/va_nguon_android_dan1.py truoc")
    cu = "double   g_dDanTong = 0.0, g_dDanOnFly = 0.0, g_dDanBar = 0.0, g_dDanBeyond = 0.0, g_dDanCol = 0.0, g_dDanMove = 0.0;"
    s = thay(s, cu, cu + nl + nl.join([
        "// " + DAU + " (C) bo qua o trong trong vung truoc GetOffsetAxis/FindNpc ([Client] DanToiUu, mac dinh 1) + do ProcessCollision/DoCollision",
        "unsigned g_uDanBoQua = 0; double g_dDanVaCham = 0.0; static int g_nDanVcDepth = 0; static int g_nJxDanToiUu = -1;",
        "struct JxDanDo",
        "{",
        "\tLARGE_INTEGER t0; double* pd; int* pDepth;",
        "\tJxDanDo(double* d, int* depth) : pd(d), pDepth(depth) { t0.QuadPart = 0; if (++*pDepth == 1 && g_nCorePaintLog > 0) QueryPerformanceCounter(&t0); }",
        "\t~JxDanDo() { if (--*pDepth == 0 && t0.QuadPart) { LARGE_INTEGER t1; QueryPerformanceCounter(&t1); *pd += DanMs(t0, t1); } }",
        "};",
        "#define JX_DAN_BO_QUA_O(vung, x, y) ((g_nJxDanToiUu > 0) && (vung).JxOTrongVung((x), (y)) && (vung).JxSoNpcO((x), (y)) == 0)"]), "khai bao dan2")
    # DanMs nam trong #ifdef JX_ANDROID o tren; JxDanDo dung DanMs -> phai nam trong cung khoi: khoi tren ket thuc bang "#endif" ngay sau "}" cua DanMs
    # -> chen truoc "#endif" do: dong khai bao vua them nam TRUOC DanMs (sau g_dDanTong...), DanMs chua khai bao -> dua JxDanDo xuong sau DanMs
    cu_struct_start = "struct JxDanDo" + nl
    a = s.index(cu_struct_start); b = s.index("#define JX_DAN_BO_QUA_O", a)
    khoi = s[a:b]  # struct ... }; + nl
    s = s[:a] + s[b:]
    cu = nl.join(["\treturn (double)(b.QuadPart - a.QuadPart) * 1000.0 / s_dTanSo;", "}"])
    s = thay(s, cu, cu + nl + khoi.rstrip(nl), "JxDanDo sau DanMs")
    # khoa doc 1 lan (trong Activate, sau dong dem act)
    cu = "\tif (bDanDo) { QueryPerformanceCounter(&liDan0); g_uDanAct++; }"
    s = thay(s, cu, cu + nl + "\tif (g_nJxDanToiUu < 0) g_nJxDanToiUu = (int)GetPrivateProfileIntA(\"Client\", \"DanToiUu\", 1, \".\\\\config.ini\");\t// " + DAU, "khoa DanToiUu")
    # (C1) CheckCollision vong CollideRange
    d = dong(s, r"^\t\t\t\tif \(!GetOffsetAxis\(m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, i , j , nSearchRegion, nRMx, nRMy\)\)$", "GetOffsetAxis CheckCollision")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID",
                            "\t\t\t\tif (JX_DAN_BO_QUA_O(CurRegion, m_nCurrentMapX + i, m_nCurrentMapY + j)) { g_uDanBoQua++; continue; }\t// " + DAU,
                            "#endif",
                            d]), "C1")
    # (C2) CheckNearestCollision 3x3
    d = dong(s, r"^\t\t\tif \(!KMissle::GetOffsetAxis\($", "GetOffsetAxis Nearest")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID",
                            "\t\t\tif (JX_DAN_BO_QUA_O(CurRegion, m_nCurrentMapX + i, m_nCurrentMapY + j)) { g_uDanBoQua++; continue; }\t// " + DAU,
                            "#endif",
                            d]), "C2")
    # (C3) ProcessCollision(int...) vong nRange
    d = dong(s, r"^\t\t\tif \(!GetOffsetAxis\(nSubWorld, nRegionId, nMapX, nMapY, i , j , nSearchRegion, nRMx, nRMy\)\)$", "GetOffsetAxis Process")
    s = thay(s, d, nl.join(["#ifdef JX_ANDROID",
                            "\t\t\tif (JX_DAN_BO_QUA_O(SubWorld[nSubWorld].m_Region[nRegionId], nMapX + i, nMapY + j)) { g_uDanBoQua++; continue; }\t// " + DAU,
                            "#endif",
                            d]), "C3")
    # do vacham: RAII o dau ProcessCollision(int...) va DoCollision
    d = dong(s, r"^int KMissle::ProcessCollision\(int nLauncherIdx, int nRegionId, int nMapX, int nMapY, int nRange , int eRelation, int nPreferIdx\)$", "ProcessCollision int")
    s = thay(s, d + nl + "{", d + nl + "{" + nl + nl.join(["#ifdef JX_ANDROID", "\tJxDanDo jxDanDo(&g_dDanVaCham, &g_nDanVcDepth);\t// " + DAU, "#endif"]), "RAII Process")
    cu = "\tg_uDanHit++;\t// [DAN 11/09]"
    s = thay(s, cu, cu + nl + "\tJxDanDo jxDanDo(&g_dDanVaCham, &g_nDanVcDepth);\t// " + DAU, "RAII DoCollision")
    ghi(p, s, cao)

print("xong")
