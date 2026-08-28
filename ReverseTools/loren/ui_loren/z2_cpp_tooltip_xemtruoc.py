# -*- coding: utf-8 -*-
"""z2_cpp_tooltip_xemtruoc.py - Ba mieng C++ cho hai yeu cau cua chu game:
  (a) "item do pho thieu noi dung can nhung nguyen lieu nao pham chat bao nhieu"
      -> KItem.cpp: chu giai do pho liet ke 6 nguyen lieu + cap + ten phep.
  (b) "tab do pho thieu nut xem truoc"
      -> UiCompoundItem: nut PreviewBtn (anh da rut o x7) goi LR_UI_AtlasPreview.
  (c) danh sach trang case 7 += "LR_UI_AtlasPreview" (thieu la may chu im lang).

CHU GIAI DO PHO (a) - vi sao lan nay DUNG (lan truoc x2 sai va da go):
  - Chuoi tieng Viet: SINH bang bangtxt.uni2tcvn, nhung vao patcher duoi dang
    escape PYTHON \\xNN -> tep .cpp nhan BYTE TCVN3 THAT, khong go tay octal.
  - Tra DUNG DONG ma isAtlas (atlas.lua:110) dung khi ghep that: dong DAU TIEN
    khop ma do pho -> tooltip va luat kiem luon khop nhau.
  - Ten phep tra tu magicattriblevel_index.txt (khoa MAGIC_ID, cot DESC) - y
    het cach chu giai khoang 199..204 dang lam (KItem.cpp:1515).
  - Moi dong ket bang "  \\n  " ASCII va KHONG dong the <color> ngay sau chuoi
    TCVN3 -> khong dinh luat nuot dau `<` cua TEncodeText.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_z2 lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] chu giai + xem truoc Do pho"
HAU_TO = ".truoc_z2"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"

# chuoi TCVN3 (sinh bang sinh_chuoi_z.py - bangtxt.uni2tcvn)
NL_CAN = "Nguy\xaan li\xd6u c\xc7n:"
CAP = "c\xcap"
KEM_HT = "K\xccm: Huy\xd2n Tinh Kho\xb8ng Th\xb9ch + 100000 l\xad\xeeng"


def K(*d):
    return list(d)


# ====================== KItem.cpp : tooltip do pho ======================
NEO_KITEM = [
    T + "if (m_CommonAttrib.nItemGenre == item_magicscript && (m_CommonAttrib.nParticularType >= 199 && m_CommonAttrib.nParticularType <= 204))",
    T + "{",
    T*2 + "char szLevel[128];",
]

KHOI_KITEM = [
    T + "// " + NHAN + ".",
    T + "// Chu game: \"item do pho thieu noi dung can nhung nguyen lieu nao pham",
    T + "// chat bao nhieu\". Cong thuc doc tu atlas_compound.txt - LAY DONG DAU",
    T + "// TIEN khop ma do pho, dung dong ma isAtlas (atlas.lua:110) dung khi ghep",
    T + "// that, nen tooltip va luat kiem luon khop nhau.",
    T + "// Dai 238..390 = dai ma do pho that sau khi nan bang (132 ma).",
    T + "if (m_CommonAttrib.nItemGenre == item_magicscript && m_CommonAttrib.nDetailType == 1 &&",
    T*2 + "m_CommonAttrib.nParticularType >= 238 && m_CommonAttrib.nParticularType <= 390)",
    T + "{",
    T*2 + "KTabFile AtlasTab;",
    T*2 + "if (AtlasTab.Load(\"\\\\Settings\\\\Item\\\\atlas_compound.txt\"))",
    T*2 + "{",
    T*3 + "int nHang = 0;",
    T*3 + "for (int r = 2; r <= AtlasTab.GetHeight(); r++)",
    T*3 + "{",
    T*4 + "int nG = -1, nD = -1, nP = -1;",
    T*4 + "AtlasTab.GetInteger(r, (LPSTR)\"ATLAS_GENRE\", -1, &nG);",
    T*4 + "AtlasTab.GetInteger(r, (LPSTR)\"ATLAS_DETAILTYPE\", -1, &nD);",
    T*4 + "AtlasTab.GetInteger(r, (LPSTR)\"ATLAS_PARTICULAR\", -1, &nP);",
    T*4 + "if (nG == 6 && nD == 1 && nP == m_CommonAttrib.nParticularType)",
    T*4 + "{",
    T*5 + "nHang = r;",
    T*5 + "break;",
    T*4 + "}",
    T*3 + "}",
    T*3 + "if (nHang >= 2)",
    T*3 + "{",
    T*4 + "KTabFile MagicIdx;",
    T*4 + "BOOL bMagic = MagicIdx.Load(\"\\\\Settings\\\\Item\\\\magicattriblevel_index.txt\");",
    T*4 + "strcat(pszMsg, \" <color=Fire>" + NL_CAN + "  \\n  \");",
    T*4 + "for (int k = 1; k <= 6; k++)",
    T*4 + "{",
    T*5 + "char szCot[32];",
    T*5 + "char szTenNl[64];",
    T*5 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_NAME\", k);",
    T*5 + "szCot[sizeof(szCot) - 1] = 0;",
    T*5 + "szTenNl[0] = 0;",
    T*5 + "AtlasTab.GetString(nHang, szCot, (LPSTR)\"\", szTenNl, sizeof(szTenNl));",
    T*5 + "if (szTenNl[0] == 0)",
    T*6 + "continue;",
    T*5 + "int nCap = -1, nMg = -1;",
    T*5 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_LEVEL\", k);",
    T*5 + "szCot[sizeof(szCot) - 1] = 0;",
    T*5 + "AtlasTab.GetInteger(nHang, szCot, -1, &nCap);",
    T*5 + "_snprintf(szCot, sizeof(szCot) - 1, \"%d_MAGIC_ID\", k);",
    T*5 + "szCot[sizeof(szCot) - 1] = 0;",
    T*5 + "AtlasTab.GetInteger(nHang, szCot, -1, &nMg);",
    T*5 + "char szDescNl[64];",
    T*5 + "szDescNl[0] = 0;",
    T*5 + "if (bMagic && nMg > 0)",
    T*5 + "{",
    T*6 + "char szKey[16];",
    T*6 + "_snprintf(szKey, sizeof(szKey) - 1, \"%d\", nMg);",
    T*6 + "szKey[sizeof(szKey) - 1] = 0;",
    T*6 + "MagicIdx.GetString(szKey, (LPSTR)\"DESC\", (LPSTR)\"\", szDescNl, sizeof(szDescNl));",
    T*5 + "}",
    T*5 + "char szDongNl[200];",
    T*5 + "if (nCap > 0 && szDescNl[0])",
    T*6 + "_snprintf(szDongNl, sizeof(szDongNl) - 1, \" <color=Green>%s (" + CAP + " %d) - %s  \\n  \", szTenNl, nCap, szDescNl);",
    T*5 + "else if (nCap > 0)",
    T*6 + "_snprintf(szDongNl, sizeof(szDongNl) - 1, \" <color=Green>%s (" + CAP + " %d)  \\n  \", szTenNl, nCap);",
    T*5 + "else",
    T*6 + "_snprintf(szDongNl, sizeof(szDongNl) - 1, \" <color=Green>%s  \\n  \", szTenNl);",
    T*5 + "szDongNl[sizeof(szDongNl) - 1] = 0;",
    T*5 + "strcat(pszMsg, szDongNl);",
    T*4 + "}",
    T*4 + "strcat(pszMsg, \" <color=White>" + KEM_HT + "  \\n  \");",
    T*3 + "}",
    T*2 + "}",
    T + "}",
    "",
]

# ====================== KProtocolProcess.cpp : danh sach trang ======================
NEO_WL = [T*5 + '"LR_UI_Atlas",']
KHOI_WL = [
    T*5 + '"LR_UI_Atlas",',
    T*5 + '"LR_UI_AtlasPreview",' + T + "// " + NHAN + " - nut Xem truoc",
]

# ====================== UiCompoundItem.h / .cpp : nut Xem truoc ======================
UIH = [
    ("khai bao m_Preview",
     K(T + "KWndButton		m_Atlas;"),
     K(T + "KWndButton		m_Atlas;",
       T + "KWndButton		m_Preview;	// " + NHAN)),
]
UIC = [
    ("AddChild m_Preview",
     K(T + "AddChild(&m_Atlas);"),
     K(T + "AddChild(&m_Atlas);",
       T + "AddChild(&m_Preview);	// " + NHAN)),
    ("Init PreviewBtn",
     K(T*2 + "m_Atlas.Init(&Ini, \"AtlasBtn\");"),
     K(T*2 + "m_Atlas.Init(&Ini, \"AtlasBtn\");",
       T*2 + "m_Preview.Init(&Ini, \"PreviewBtn\");	// " + NHAN)),
    ("WndProc nut Xem truoc",
     K(T*4 + "m_nStatus = STATUS_BEGIN_ATLAS;",
       T*4 + "return 1;",
       T*3 + "}",
       T*2 + "}",
       T*2 + "break;"),
     K(T*4 + "m_nStatus = STATUS_BEGIN_ATLAS;",
       T*4 + "return 1;",
       T*3 + "}",
       T*2 + "}",
       T*2 + "else if (uParam == (unsigned int)&m_Preview)",
       T*2 + "{",
       T*3 + "// " + NHAN + ": nho may chu doi chieu o voi cong thuc va bao ro",
       T*3 + "// mon nao con thieu (LR_UI_AtlasPreview trong test_loren_admin.lua).",
       T*3 + "sDoPhoLog(\"[DOPHO] WndProc: bam nut Xem truoc\");",
       T*3 + "if (g_pCoreShell)",
       T*4 + "g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 7, (unsigned int)\"LR_UI_AtlasPreview\");",
       T*2 + "}",
       T*2 + "break;")),
]


def ap(dong, cu, moi, ten):
    vt = [i for i in range(len(dong) - len(cu) + 1) if dong[i:i + len(cu)] == cu]
    if len(vt) != 1:
        print("   !!! %-28s khop %d lan (can 1)" % (ten, len(vt)))
        return None
    print("   ok  %-28s dong %d (+%d)" % (ten, vt[0] + 1, len(moi) - len(cu)))
    return dong[:vt[0]] + moi + dong[vt[0] + len(cu):]


def xuly(p, mieng, delta_mong, ghi):
    raw = io.open(p, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    print("--- %s ---" % os.path.basename(p))
    if NHAN in raw:
        print("   DA CO - bo qua")
        return True
    dong = raw.split(eol)
    for ten, cu, moi in mieng:
        dong = ap(dong, cu, moi, ten)
        if dong is None:
            return False
    nd = eol.join(dong)
    hi1 = sum(1 for c in nd if ord(c) > 127)
    if hi1 - hi0 != delta_mong:
        print("   !!! byte cao %d -> %d (them %d, mong %d)" % (hi0, hi1, hi1 - hi0, delta_mong))
        return False
    print("   byte cao %d -> %d (+%d dung nhu mong)" % (hi0, hi1, hi1 - hi0))
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("   !!! ngoac lech")
        return False
    try:
        nd.encode("latin-1")
    except UnicodeEncodeError as e:
        print("   !!! ngoai latin-1: %s" % e)
        return False
    if ghi:
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != nd:
            print("   !!! doc lai KHONG khop")
            return False
        print("   DA GHI")
    return True


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== z2_cpp_tooltip_xemtruoc - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    # delta byte cao cua khoi KItem = tong byte cao trong 3 chuoi TCVN3 (CAP dung 2 lan)
    delta = sum(1 for c in NL_CAN + KEM_HT + CAP + CAP if ord(c) > 127)
    ok = xuly(os.path.join(CORE, "KItem.cpp"),
              [("chu giai do pho", NEO_KITEM, KHOI_KITEM + NEO_KITEM, )], delta, ghi)
    if not ok:
        return 1
    ok = xuly(os.path.join(CORE, "KProtocolProcess.cpp"),
              [("danh sach trang", NEO_WL, KHOI_WL)], 0, ghi)
    if not ok:
        return 1
    ok = xuly(os.path.join(UI, "UiCompoundItem.h"), UIH, 0, ghi)
    if not ok:
        return 1
    ok = xuly(os.path.join(UI, "UiCompoundItem.cpp"), UIC, 0, ghi)
    if not ok:
        return 1

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
    else:
        print("\n=> build Core CA HAI cau hinh + S3Client, thay 3 nhi phan")
    return 0


if __name__ == "__main__":
    sys.exit(main())
