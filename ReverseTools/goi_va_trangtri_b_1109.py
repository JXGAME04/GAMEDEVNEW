# -*- coding: utf-8 -*-
r"""goi_va_trangtri_b_1109.py - [TRANGTRI 11/09 b] sua goc "bat roi van khong co gi":
KSubWorld::m_szMapPath CHI duoc gan trong khoi da bi chu thich (KSubWorld.cpp ~2332, nam trong
/* ... */ tu dong 2323 den 2344) nen LUON RONG. KRegion::LoadObject nhan lpszPath rong va
tra FALSE ngay dong dau -> phan 2/3 cua Region_C.dat khong bao gio duoc doc.

Sua: gan m_szMapPath ngay sau khi doc m_szPathName tu MapList.ini (dong 2362, ma DANG CHAY).
MapList.ini cua du an ghi khong co tien to \maps\ (do duoc: "1=西北南区\凤翔") con ban 2.0 thi co,
nen chap ca hai kieu. Kem chan tran dem.

Va them: ghi duong dan map vao dong [TRANGTRI] va in dong do MOI 10 s ke ca khi so dem = 0,
de phan biet "map khong co du lieu trang tri" voi "duong dan sai nen khong doc duoc gi".
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[TRANGTRI 11/09 b]"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten)
        sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong))
        sys.exit(1)
    return s.replace(old, new)


# ------------------------------------------------------------------ KSubWorld.cpp: gan m_szMapPath
P = D + "KSubWorld.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    neo = T*2 + 'IniFile.GetString("List", szKeyName, "", m_szPathName, sizeof(m_szPathName));' + NL
    s = rep(s, neo, neo + NL.join([
        T*2 + "{" + T + "// " + TAG + " m_szMapPath cu chi duoc gan trong khoi DA BI CHU THICH ben tren",
        T*2 + "// nen LUON RONG -> KRegion::LoadObject tra FALSE ngay dong dau va phan NPC/OBJ cua",
        T*2 + "// Region_C.dat khong bao gio duoc doc. Lay thang tu m_szPathName vua doc xong.",
        T*3 + "extern char g_szTTMap[80];",
        T*3 + "const char* pTT = m_szPathName;",
        T*3 + "while (*pTT == '\\\\') pTT++;",
        T*3 + "int nTT = (int)strlen(pTT);",
        T*3 + "m_szMapPath[0] = 0;",
        T*3 + "if (nTT > 0 && nTT + 7 < (int)sizeof(m_szMapPath))",
        T*3 + "{",
        T*4 + "// MapList.ini cua du an ghi KHONG co tien to \\maps\\ (\"1=...\"), ban 2.0 thi co.",
        T*4 + "if ((pTT[0] == 'm' || pTT[0] == 'M') && (pTT[1] == 'a' || pTT[1] == 'A')",
        T*4 + " && (pTT[2] == 'p' || pTT[2] == 'P') && (pTT[3] == 's' || pTT[3] == 'S') && pTT[4] == '\\\\')",
        T*5 + "sprintf(m_szMapPath, \"\\\\%s\", pTT);",
        T*4 + "else",
        T*5 + "sprintf(m_szMapPath, \"\\\\maps\\\\%s\", pTT);",
        T*3 + "}",
        T*3 + "strncpy(g_szTTMap, m_szMapPath, sizeof(g_szTTMap) - 1);",
        T*3 + "g_szTTMap[sizeof(g_szTTMap) - 1] = 0;",
        T*2 + "}", ""]), "S gan duong")
    ghi(P, s, h0, lf0, crlf0, "KSubWorld.cpp")
else:
    print("KSubWorld.cpp da co")

# ------------------------------------------------------------------ KRegion.cpp: khai bao g_szTTMap
P = D + "KRegion.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, "char     g_szTTTen[3][40] = { \"\", \"\", \"\" };" + NL,
            "char     g_szTTTen[3][40] = { \"\", \"\", \"\" };" + NL +
            "char     g_szTTMap[80] = \"\";" + T + "// " + TAG + " duong dan map dang mo, de doi chieu trong log" + NL, "R bien map")
    ghi(P, s, h0, lf0, crlf0, "KRegion.cpp")
else:
    print("KRegion.cpp da co")

# ------------------------------------------------------------------ KSubWorldSet.cpp: in ke ca khi 0
P = D + "KSubWorldSet.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T*3 + "extern char g_szTTTen[3][40]; extern int g_nTTSo;" + NL +
            T*3 + "if (g_uTTNpcThem || g_uTTNpcXoa || g_uTTObjThem || g_uTTNpcHong)" + NL +
            T*3 + "{" + NL,
            T*3 + "extern char g_szTTTen[3][40]; extern int g_nTTSo; extern char g_szTTMap[80];" + NL +
            T*3 + "{" + T + "// " + TAG + " in ke ca khi bang 0: phan biet \"map khong co du lieu\" voi \"duong dan sai\"" + NL, "SW in luon")
    s = rep(s, T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem, g_szTTTen[0], g_szTTTen[1], g_szTTTen[2]);" + NL,
            T*6 + "g_uTTNpcThem, g_uTTNpcHong, g_uTTNpcXoa, g_uTTObjThem, g_szTTMap, g_szTTTen[0], g_szTTTen[1], g_szTTTen[2]);" + NL, "SW tham so")
    s = rep(s, T*5 + "fprintf(pT, \"[TRANGTRI] 10s: npc them %u hong %u xoa %u | obj them %u | ten: %s %s %s\\n\"," + NL,
            T*5 + "fprintf(pT, \"[TRANGTRI] 10s: npc them %u hong %u xoa %u | obj them %u | map %s | ten: %s %s %s\\n\"," + NL, "SW dinh dang")
    ghi(P, s, h0, lf0, crlf0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")

print("XONG " + TAG)
