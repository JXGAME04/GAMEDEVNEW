# -*- coding: utf-8 -*-
"""goi_va_vatdong_1009.py - [VATDONG 10/09] vat canh CO HOAT ANH (khoi bep, nuoc chay, co bay...).

Do duoc trong du lieu map cua DU AN (doc thang Region_C.dat, phan BuildinObj):
  - Phuong Tuong: 4.716 vat canh, 253 vat CO HOAT ANH, gom \\游戏资源\\地图动画\\熔岩\\短烟.spr (KHOI) x2,
    \\...\\水波\\水_3.spr (song nuoc) x211, \\...\\城市村镇\\city-moc*.spr...
  - Hoa Son: 54 vat co hoat anh. Mac Cao Quat: 0.
  - CA 27 anh hoat anh deu CO trong pak ca hai ben (khong thieu tep nao).
  - Toan bo 253 vat hoat anh o Phuong Tuong deu kieu sap xep POINT (Props bit 8-9 = 0).
Vay du lieu va anh deu du; viec con lai la client co ve va co chay khung hinh khong.

Lam hai viec:
  1. SUA: KIpotBranch::PaintABranchObject dang BO QUA buoc chuyen khung (dong BuildinObjNextFrame bi chu thich)
     -> vat canh kieu LINE/TREE co hoat anh se dung hinh. Bat lai (IR_NextFrame di theo dong ho nen goi
     nhieu lan trong mot khung khong lam nhanh hon).
  2. DO: dem vat canh co hoat anh da VE trong 10 giay va ghi 3 ten dau + toa do o (canh/256, canh/512)
     -> [VATDONG] trong jx_paint.log. Chi ghi khi PaintLog=1.
"""
import io
import sys

NL = "\r\n"
T = "\t"
TAG = "[VATDONG 10/09]"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/Scene/"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


# ---------------------------------------------------------------- KIpotLeaf.cpp: dem + bat khung
P = D + "KIpotLeaf.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, "void  BuildinObjNextFrame(KBuildinObj* pBio)" + NL + "{" + NL,
            NL.join([
                "// " + TAG + " dem vat canh co hoat anh da ve (khoi bep, nuoc chay, co bay) de biet client co chay hay khong",
                "unsigned g_uVatDongVe = 0;",
                "char g_szVatDong[3][96] = { \"\", \"\", \"\" };",
                "int  g_nVatDongX[3] = { 0, 0, 0 }, g_nVatDongY[3] = { 0, 0, 0 };",
                "int  g_nVatDongSo = 0;",
                "",
                "void  BuildinObjNextFrame(KBuildinObj* pBio)",
                "{",
                T + "g_uVatDongVe++;" + T + "// " + TAG,
                T + "if (g_nVatDongSo < 3 && pBio->szImage[0])",
                T + "{",
                T*2 + "int k = g_nVatDongSo;",
                T*2 + "int nTrung = 0;",
                T*2 + "for (int i = 0; i < g_nVatDongSo; i++) if (strcmp(g_szVatDong[i], pBio->szImage) == 0) nTrung = 1;",
                T*2 + "if (!nTrung)",
                T*2 + "{",
                T*3 + "strncpy(g_szVatDong[k], pBio->szImage, sizeof(g_szVatDong[k]) - 1);",
                T*3 + "g_nVatDongX[k] = pBio->oPos1.x / 256;" + T + "// doi ra toa do o nhu trong game",
                T*3 + "g_nVatDongY[k] = pBio->oPos1.y / 512;",
                T*3 + "g_nVatDongSo++;",
                T*2 + "}",
                T + "}",
                ""]), "L dem")
    ghi(P, s, h0, lf0, crlf0, "KIpotLeaf.cpp")
else:
    print("KIpotLeaf.cpp da co")

# ---------------------------------------------------------------- KIpotBranch.cpp: bat lai buoc chuyen khung
P = D + "KIpotBranch.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, T + "//BuildinObjNextFrame(pBio);" + NL,
            T + "BuildinObjNextFrame(pBio);" + T + "// " + TAG + " truoc day bi chu thich: vat canh kieu LINE/TREE co hoat anh bi dung hinh" + NL, "B bat khung")
    ghi(P, s, h0, lf0, crlf0, "KIpotBranch.cpp")
else:
    print("KIpotBranch.cpp da co")

# ---------------------------------------------------------------- KSubWorldSet.cpp: in [VATDONG] moi 10 s
P = "D:/GAMEDEVNEW_wt_delta/Sources/Core/Src/KSubWorldSet.cpp"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    neo = T*2 + "extern unsigned g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon;"
    s = rep(s, neo, T*2 + "{" + T + "// " + TAG + " vat canh co hoat anh: dem lan ve trong 10 s + 3 ten dau" + NL +
            T*3 + "extern unsigned g_uVatDongVe; extern char g_szVatDong[3][96]; extern int g_nVatDongX[3], g_nVatDongY[3], g_nVatDongSo;" + NL +
            T*3 + "FILE* pV = fopen(\"jx_paint.log\", \"a\");" + NL +
            T*3 + "if (pV)" + NL +
            T*3 + "{" + NL +
            T*4 + "fprintf(pV, \"[VATDONG] 10s: %u lan ve vat canh co hoat anh | %d ten: %s(%d,%d) %s(%d,%d) %s(%d,%d)\\n\"," + NL +
            T*5 + "g_uVatDongVe, g_nVatDongSo, g_szVatDong[0], g_nVatDongX[0], g_nVatDongY[0]," + NL +
            T*5 + "g_szVatDong[1], g_nVatDongX[1], g_nVatDongY[1], g_szVatDong[2], g_nVatDongX[2], g_nVatDongY[2]);" + NL +
            T*4 + "fclose(pV);" + NL +
            T*3 + "}" + NL +
            T*3 + "g_uVatDongVe = 0; g_nVatDongSo = 0; g_szVatDong[0][0] = g_szVatDong[1][0] = g_szVatDong[2][0] = 0;" + NL +
            T*2 + "}" + NL + neo, "S in")
    ghi(P, s, h0, lf0, crlf0, "KSubWorldSet.cpp")
else:
    print("KSubWorldSet.cpp da co")
print("XONG " + TAG)
