# -*- coding: utf-8 -*-
"""goi_va_cay_am_do_0909.py - [CAY 09/09 do] + [AM 09/09 do]: hai bo do (PaintLog=1) cho 2 nghi van con lai cua tick.

CAY: KIpotBranch::AddPointLeafToList = 44-48 % tick (DOLUOT). Dem moi 10 s: so lan chen, so nut duyet vong 1 (tim vat tinh xep
theo duong chua diem) va vong 2 (chen theo y), so lan khop vat tinh, so lan chen vao danh sach con (duoi vat tinh) ->
[WORLD b] 'cay: chen N duyet1 N duyet2 N khop N con N'. Quyet dinh cach cat (bo qua vong 1 khi nhanh khong co duong / doi cho tai cho).
AM: 2 tick 29 va 107 ms luc vao map (16:0x) o vong DAN (171-218 dan) va ST (idx 49 dung yen 40 ms) - nghi nap AM THANH dong bo
(g_SoundCache.GetNode -> KWavSound tu sound.pak). Boc 6 diem goi bang AmThanhLay(): do thoi gian, dem lan >= 0,5 ms ->
[WORLD-TICK] 'am thanh N lan X ms', [WORLD b] 'am thanh N lan X ms'.
"""
import io
import re
import sys

NL = "\r\n"
T = "\t"
CORE = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"
F_BR = CORE + "Scene/KIpotBranch.cpp"
F_SW = CORE + "KSubWorld.cpp"
F_SWS = CORE + "KSubWorldSet.cpp"
F_AM = [CORE + "KMissleRes.cpp", CORE + "KNpcRes.cpp", CORE + "KNpcSet.cpp", CORE + "CoreShell.cpp"]
TAG_C = "[CAY 09/09 do]"
TAG_A = "[AM 09/09 do]"


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


# ================================================================ KIpotBranch.cpp: bo dem cay
s, c0, l0, h0 = doc(F_BR)
if TAG_C not in s:
    s = rep(s, "#include \"SceneMath.h\"" + NL,
            "#include \"SceneMath.h\"" + NL + "#ifndef _SERVER" + NL +
            "unsigned g_uCayChen = 0, g_uCayDuyet1 = 0, g_uCayDuyet2 = 0, g_uCayKhop = 0, g_uCayCon = 0;" + T + "// " + TAG_C + " in o [WORLD b]" + NL +
            "#endif" + NL,
            "BR bien")
    old = T + "POINT" + T + "lp, op1,op2;" + NL + T + "lp = pLeaf->oPosition;"
    new = "#ifndef _SERVER" + NL + T + "g_uCayChen++; if (pParentLeaf) g_uCayCon++;" + T + "// " + TAG_C + NL + "#endif" + NL + old
    s = rep(s, old, new, "BR chen")
    old = T + "for(pL = pFirst; pL; pL = pL->pBrother)" + NL + T + "{" + NL + T*2 + "if (pL->eLeafType != KIpotLeaf::IPOTL_T_BUILDIN_OBJ)"
    new = T + "for(pL = pFirst; pL; pL = pL->pBrother)" + NL + T + "{" + NL + "#ifndef _SERVER" + NL + T*2 + "g_uCayDuyet1++;" + T + "// " + TAG_C + NL + "#endif" + NL + T*2 + "if (pL->eLeafType != KIpotLeaf::IPOTL_T_BUILDIN_OBJ)"
    s = rep(s, old, new, "BR vong 1")
    old = T*3 + "pMatchL = pL;" + NL + T*3 + "eMatchRelate = eRelate;"
    new = T*3 + "pMatchL = pL;" + NL + T*3 + "eMatchRelate = eRelate;" + NL + "#ifndef _SERVER" + NL + T*3 + "g_uCayKhop++;" + T + "// " + TAG_C + NL + "#endif"
    s = rep(s, old, new, "BR khop")
    old = T + "for(pL = pFirst; pL; pPreLeaf = pL, pL = pL->pBrother)" + NL + T + "{"
    new = old + NL + "#ifndef _SERVER" + NL + T*2 + "g_uCayDuyet2++;" + T + "// " + TAG_C + NL + "#endif"
    s = rep(s, old, new, "BR vong 2")
    ghi(F_BR, s, c0, l0, h0, "KIpotBranch.cpp")
else:
    print("KIpotBranch.cpp da co")

# ================================================================ KSubWorld.cpp: AmThanhLay + dem
s, c0, l0, h0 = doc(F_SW)
if TAG_A not in s:
    # bien dem: trong khoi client (truoc WorldTickXong); ham boc: NGOAI khoi client (truoc KSubWorld::Activate) de may chu cung link duoc
    old = "void WorldTickXong(double dQuet)" + NL + "{"
    new = NL.join([
        "// " + TAG_A + " do nap AM THANH dong bo (g_SoundCache.GetNode -> KWavSound doc sound.pak khi chua co trong cache):",
        "// boc 6 diem goi trong KMissleRes/KNpcRes/KNpcSet/CoreShell bang AmThanhLay (dinh nghia truoc KSubWorld::Activate);",
        "// chi dem lan >= 0,5 ms (= that su nap). In o [WORLD-TICK] va [WORLD b].",
        "unsigned g_uAmNap = 0; double g_dAmNapMs = 0.0; unsigned t_uAmNap = 0; double t_dAmNapMs = 0.0;",
        "void WorldTickXong(double dQuet)",
        "{",
    ])
    s = rep(s, old, new, "SW bien am")
    old = "void KSubWorld::Activate()" + NL + "{"
    new = NL.join([
        "// " + TAG_A + " boc g_SoundCache.GetNode: do thoi gian (client, PaintLog=1); may chu goi thang",
        "KCacheNode* AmThanhLay(KCache& cache, char* szTen, KCacheNode* pNode)",
        "{",
        "#ifndef _SERVER",
        T + "extern int g_nCorePaintLog; extern unsigned g_uAmNap, t_uAmNap; extern double g_dAmNapMs, t_dAmNapMs;",
        T + "if (g_nCorePaintLog <= 0) return cache.GetNode(szTen, pNode);",
        T + "static double s_dTanSo = 0.0;",
        T + "if (s_dTanSo == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dTanSo = (double)f.QuadPart / 1000.0; }",
        T + "LARGE_INTEGER a, b; QueryPerformanceCounter(&a);",
        T + "KCacheNode* p = cache.GetNode(szTen, pNode);",
        T + "QueryPerformanceCounter(&b);",
        T + "const double d = (double)(b.QuadPart - a.QuadPart) / s_dTanSo;",
        T + "if (d >= 0.5) { g_uAmNap++; g_dAmNapMs += d; t_uAmNap++; t_dAmNapMs += d; }",
        T + "return p;",
        "#else",
        T + "return cache.GetNode(szTen, pNode);",
        "#endif",
        "}",
        "void KSubWorld::Activate()",
        "{",
    ])
    s = rep(s, old, new, "SW AmThanhLay")
    # in [WORLD-TICK] + reset
    s = rep(s, "| object %u %.1f ms | dan %u %.1f ms | nguoi choi %.1f ms\\n\",",
            "| object %u %.1f ms | dan %u %.1f ms | nguoi choi %.1f ms | am thanh %u lan %.1f ms\\n\",",
            "SW tick fmt")
    s = rep(s, T*4 + "t_uKhacSo[0], t_dKhacMs[0], t_uKhacSo[1], t_dKhacMs[1], t_dKhacMs[2]);",
            T*4 + "t_uKhacSo[0], t_dKhacMs[0], t_uKhacSo[1], t_dKhacMs[1], t_dKhacMs[2], t_uAmNap, t_dAmNapMs);",
            "SW tick args")
    s = rep(s, T + "t_dKhacMs[0] = t_dKhacMs[1] = t_dKhacMs[2] = 0.0; t_uKhacSo[0] = t_uKhacSo[1] = t_uKhacSo[2] = 0;" + T + "// [WORLD 09/09 c]",
            T + "t_dKhacMs[0] = t_dKhacMs[1] = t_dKhacMs[2] = 0.0; t_uKhacSo[0] = t_uKhacSo[1] = t_uKhacSo[2] = 0;" + T + "// [WORLD 09/09 c]" + NL +
            T + "t_uAmNap = 0; t_dAmNapMs = 0.0;" + T + "// " + TAG_A,
            "SW tick reset")
    # KCache.h
    s = rep(s, "#include \"KDoLuot.h\"" + T + "// [DOLUOT 09/09 c]" + NL,
            "#include \"KDoLuot.h\"" + T + "// [DOLUOT 09/09 c]" + NL + "#include \"KCache.h\"" + T + "// " + TAG_A + NL,
            "SW include")
    ghi(F_SW, s, c0, l0, h0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")

# ================================================================ KSubWorldSet.cpp: in [WORLD b]
s, c0, l0, h0 = doc(F_SWS)
if TAG_C not in s:
    s = rep(s, T*2 + "extern unsigned g_uVungSo, g_uVungLech, g_uVungXay;" + T + "// [VUNG 09/09]",
            T*2 + "extern unsigned g_uVungSo, g_uVungLech, g_uVungXay;" + T + "// [VUNG 09/09]" + NL +
            T*2 + "extern unsigned g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon; extern unsigned g_uAmNap; extern double g_dAmNapMs;" + T + "// " + TAG_C + " " + TAG_A,
            "SWS extern")
    s = rep(s, "| vung: xay %u, hoi %u, lech %u\\n\",",
            "| vung: xay %u, hoi %u, lech %u | cay: chen %u duyet1 %u duyet2 %u khop %u con %u | am thanh %u lan %.2f ms\\n\",",
            "SWS fmt")
    s = rep(s, T*4 + "g_uVungXay, g_uVungSo, g_uVungLech);",
            T*4 + "g_uVungXay, g_uVungSo, g_uVungLech," + NL +
            T*4 + "g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon, g_uAmNap, g_dAmNapMs);",
            "SWS args")
    s = rep(s, T*2 + "g_uVungSo = 0; g_uVungLech = 0; g_uVungXay = 0;" + T + "// [VUNG 09/09]",
            T*2 + "g_uVungSo = 0; g_uVungLech = 0; g_uVungXay = 0;" + T + "// [VUNG 09/09]" + NL +
            T*2 + "g_uCayChen = g_uCayDuyet1 = g_uCayDuyet2 = g_uCayKhop = g_uCayCon = 0; g_uAmNap = 0; g_dAmNapMs = 0.0;" + T + "// " + TAG_C + " " + TAG_A,
            "SWS reset")
    ghi(F_SWS, s, c0, l0, h0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")

# ================================================================ 4 tep goi g_SoundCache.GetNode -> AmThanhLay
for p in F_AM:
    s, c0, l0, h0 = doc(p)
    if TAG_A in s:
        print(p.split("/")[-1], "da co"); continue
    n = s.count("g_SoundCache.GetNode(")
    if n == 0:
        print("FAIL", p, "khong co g_SoundCache.GetNode"); sys.exit(1)
    s = s.replace("g_SoundCache.GetNode(", "AmThanhLay(g_SoundCache, ")
    # khai bao extern sau include dau tien cua tep
    m = re.search(r"#include [^\r\n]*\r\n", s)
    if not m:
        print("FAIL", p, "khong thay include"); sys.exit(1)
    khai = "extern KCacheNode* AmThanhLay(KCache& cache, char* szTen, KCacheNode* pNode);" + T + "// " + TAG_A + " (KSubWorld.cpp) boc g_SoundCache.GetNode de do nap dong bo" + NL
    s = s[:m.end()] + khai + s[m.end():]
    print("  %s: %d diem goi" % (p.split("/")[-1], n))
    ghi(p, s, c0, l0, h0, p.split("/")[-1])
print("XONG CAY do + AM do")
