# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - PHAN LOI (nen cho cac bang phu / nut o wauto9).
#
#  1. VA Sources/Core/Src/CoreShell.cpp (trong khoi #ifdef JX_MOBILE san co): bon ham cho bang WAuto trong game
#     - JxCore_WAutoSoLieu       so lieu nhan vat (sinh luc / noi luc / the luc / dang cap / kinh nghiem / ban do / toa do)
#                                cung nguon KProtocolProcess dien vao IPCMainSync cho WAuto.exe -> cac o "-" cua tab Co ban
#     - JxCore_WAutoViTri        ban do + toa do dang dung (nut "Lay" cua tab Di chuyen)
#     - JxCore_WAutoTenQuanhDay  ten nguoi choi quanh minh (ATYPE_GETAROUNDNAME) - hop "Theo sau", danh sach To doi / Lien dau.
#                                Ban PC phai di vong PRT_GETTEAMAROUND -> PRG_TEAMNAMELIST; trong game goi thang.
#     - JxCore_WAutoTenVatPham   ten vat pham trong hanh trang (ATYPE_GETITEMNAME) - bang "Khong nhat theo ten"
#  2. VA Sources/S3Client/Platform/JxWAutoNoiBo.h/.cpp
#     - nho trang thai BAT/TAT theo nhan vat: tep <APdata>\<id>.bat (khong dung them truong nao cua autoData - luat 0.2)
#     - ep bUseFKey = 0 moi nhip: o "chi PK khi giu phim" khong dung duoc tren dien thoai (Wnd_IsPKKeyDown luon 0),
#       tep .dat chep tu ban PC co the dang bat -> PK se khong bao gio chay ma khong ro vi sao
#
# Chi Android/iOS (JX_MOBILE); ban PC khong doi mot hanh vi nao. Chay lai vo hai.

import io
import os
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")

DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("   bo qua (da va):", p)
        return
    ghi(p, ham(s))
    print("   da va:", p)


# ---------------------------------------------------------------- 1. CoreShell.cpp
CORESHELL = [
    "",
    "// %s So lieu nhan vat cho bang WAuto trong game (tab Co ban: sinh luc / noi luc / the luc / dang cap / kinh nghiem /" % DAU,
    "// ban do / toa do). CUNG nguon ma KProtocolProcess:2073 dien vao IPCMainSync gui cho WAuto.exe, nen so hien tren dien",
    "// thoai trung voi so WAuto.exe hien ben PC. pnSo can >= 10 phan tu:",
    "//   0 sinh luc, 1 sinh luc toi da, 2 noi luc, 3 noi luc toi da, 4 the luc, 5 the luc toi da, 6 dang cap, 7 ma ban do, 8 x, 9 y",
    "// Kinh nghiem tra bang CHU (m_nExp la double, cap cao vuot qua int). Tra 1 khi da vao game. Chi mobile.",
    "extern \"C\" int JxCore_WAutoSoLieu(int* pnSo, int nSoMax, char* szMap, int nMapMax, char* szKN, int nKNMax)",
    "{",
    "\tif (!pnSo || nSoMax < 10)",
    "\t\treturn 0;",
    "\tint nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;",
    "\tif (nIdx <= 0)",
    "\t\treturn 0;",
    "\tchar szTen[64] = { 0 };",
    "\tint nId = 0, nX = 0, nY = 0;",
    "\tg_ScenePlace.GetSceneNameAndFocus(szTen, nId, nX, nY);",
    "\tpnSo[0] = Npc[nIdx].m_CurrentLife;",
    "\tpnSo[1] = Npc[nIdx].m_CurrentLifeMax;",
    "\tpnSo[2] = Npc[nIdx].m_CurrentMana;",
    "\tpnSo[3] = Npc[nIdx].m_CurrentManaMax;",
    "\tpnSo[4] = Npc[nIdx].m_CurrentStamina;",
    "\tpnSo[5] = Npc[nIdx].m_CurrentStaminaMax;",
    "\tpnSo[6] = Npc[nIdx].m_Level;",
    "\tpnSo[7] = nId;",
    "\tNpc[nIdx].GetMpsPos(&pnSo[8], &pnSo[9]);",
    "\tif (szMap && nMapMax > 0)",
    "\t{",
    "\t\tstrncpy(szMap, szTen, nMapMax - 1);",
    "\t\tszMap[nMapMax - 1] = 0;",
    "\t}",
    "\tif (szKN && nKNMax > 0)",
    "\t{",
    "\t\tdouble dE = Player[CLIENT_PLAYER_INDEX].m_nExp;",
    "\t\tdouble dF = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;",
    "\t\tif (dF > 0.0)",
    "\t\t\tsnprintf(szKN, nKNMax, \"%.0f/%.0f\", dE, dF);",
    "\t\telse",
    "\t\t\tsnprintf(szKN, nKNMax, \"%.0f\", dE);",
    "\t}",
    "\treturn 1;",
    "}",
    "",
    "// %s Ban do + toa do DANG DUNG cho hai nut \"Lay\" cua tab Di chuyen (WAuto.exe lay tu gnode.player, cung mot nguon)." % DAU,
    "// Tra 1 khi da vao game. Chi mobile.",
    "extern \"C\" int JxCore_WAutoViTri(int* pnMapId, char* szMap, int nMapMax, int* pnX, int* pnY)",
    "{",
    "\tint nIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;",
    "\tif (nIdx <= 0)",
    "\t\treturn 0;",
    "\tchar szTen[64] = { 0 };",
    "\tint nId = 0, nX = 0, nY = 0;",
    "\tg_ScenePlace.GetSceneNameAndFocus(szTen, nId, nX, nY);",
    "\tif (pnMapId)",
    "\t\t*pnMapId = nId;",
    "\tif (szMap && nMapMax > 0)",
    "\t{",
    "\t\tstrncpy(szMap, szTen, nMapMax - 1);",
    "\t\tszMap[nMapMax - 1] = 0;",
    "\t}",
    "\tint x = 0, y = 0;",
    "\tNpc[nIdx].GetMpsPos(&x, &y);",
    "\tif (pnX)",
    "\t\t*pnX = x;",
    "\tif (pnY)",
    "\t\t*pnY = y;",
    "\treturn 1;",
    "}",
    "",
    "// %s Ten nhan vat NGUOI CHOI dang dung quanh minh - dung ATYPE_GETAROUNDNAME, chinh viec ma ban PC phai di vong" % DAU,
    "// PRT_GETTEAMAROUND -> PRG_TEAMNAMELIST qua vung nho chung. pOut = mang nMax x 32 byte. Tra so ten. Chi mobile.",
    "extern \"C\" int JxCore_WAutoTenQuanhDay(char* pOut, int nMax)",
    "{",
    "\tstatic char s_szTam[100 * 32];\t// ATYPE_GETAROUNDNAME dung lai o 100 ten, moi ten 32 byte",
    "\tif (!pOut || nMax <= 0)",
    "\t\treturn 0;",
    "\tmemset(pOut, 0, (size_t)nMax * 32);",
    "\tif (Player[CLIENT_PLAYER_INDEX].m_nIndex <= 0)",
    "\t\treturn 0;",
    "\tmemset(s_szTam, 0, sizeof(s_szTam));",
    "\tint n = g_CoreShell.OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_GETAROUNDNAME, (KNPARAM)s_szTam);",
    "\tif (n < 0)",
    "\t\tn = 0;",
    "\tif (n > 100)",
    "\t\tn = 100;",
    "\tif (n > nMax)",
    "\t\tn = nMax;",
    "\tmemcpy(pOut, s_szTam, (size_t)n * 32);",
    "\treturn n;",
    "}",
    "",
    "// %s Ten cac mon KHAC NHAU dang co trong hanh trang - ATYPE_GETITEMNAME (bang \"Khong nhat theo ten\")." % DAU,
    "// pOut = mang nMax x 80 byte. Tra so ten (toi da 60 = so o hanh trang). Chi mobile.",
    "extern \"C\" int JxCore_WAutoTenVatPham(char* pOut, int nMax)",
    "{",
    "\tstatic char s_szTam[MAX_EQUIPMENT_ITEM * 80];",
    "\tif (!pOut || nMax <= 0)",
    "\t\treturn 0;",
    "\tmemset(pOut, 0, (size_t)nMax * 80);",
    "\tif (Player[CLIENT_PLAYER_INDEX].m_nIndex <= 0)",
    "\t\treturn 0;",
    "\tmemset(s_szTam, 0, sizeof(s_szTam));",
    "\tint n = g_CoreShell.OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_GETITEMNAME, (KNPARAM)s_szTam);",
    "\tif (n < 0)",
    "\t\tn = 0;",
    "\tif (n > MAX_EQUIPMENT_ITEM)",
    "\t\tn = MAX_EQUIPMENT_ITEM;",
    "\tif (n > nMax)",
    "\t\tn = nMax;",
    "\tmemcpy(pOut, s_szTam, (size_t)n * 80);",
    "\treturn n;",
    "}",
]


def va_coreshell(s):
    return thay(s, [
        "\treturn Npc[nIdx].m_SkillList.GetAllSkillByType(pOut);",
        "}",
        "#endif\t// JX_MOBILE",
    ], [
        "\treturn Npc[nIdx].m_SkillList.GetAllSkillByType(pOut);",
        "}",
    ] + CORESHELL + [
        "#endif\t// JX_MOBILE",
    ], "CoreShell.cpp: bon ham WAuto")


# ---------------------------------------------------------------- 2. JxWAutoNoiBo
def va_noibo_h(s):
    return thay(s, [
        "int\t\t\t\tJxWAuto_DangBat();",
    ], [
        "int\t\t\t\tJxWAuto_DangBat();",
        "// %s Trang thai bat/tat duoc NHO theo nhan vat (tep <APdata>\\<id>.bat). autoData khong co truong nao cho viec nay" % DAU,
        "// va KHONG duoc them truong vao giua struct (lech offset tep .dat cua moi nguoi choi PC - luat 0.2 cua lo trinh).",
    ], "JxWAutoNoiBo.h: chu thich nho bat/tat")


def va_noibo_cpp(s):
    # a) ten tep nho bat/tat + doc/ghi
    s = thay(s, [
        "static void WA_TenTep(char* sz, int nMax, unsigned int uId)",
        "{",
        "\tsnprintf(sz, nMax, WA_THU_MUC \"\\\\%u.dat\", uId);",
        "}",
    ], [
        "static void WA_TenTep(char* sz, int nMax, unsigned int uId)",
        "{",
        "\tsnprintf(sz, nMax, WA_THU_MUC \"\\\\%u.dat\", uId);",
        "}",
        "",
        "// %s Tep NHO TRANG THAI BAT/TAT cua rieng nhan vat nay (1 byte). Khong dung truong nao cua autoData: them truong" % DAU,
        "// vao giua struct la lech offset moi tep .dat cua nguoi choi PC (canh bao ghi 5 cho trong ipc_shared.h).",
        "static void WA_TenTepBat(char* sz, int nMax, unsigned int uId)",
        "{",
        "\tsnprintf(sz, nMax, WA_THU_MUC \"\\\\%u.bat\", uId);",
        "}",
        "",
        "static int WA_DocBat(unsigned int uId)",
        "{",
        "\tchar sz[128];",
        "\tBYTE b = 0;",
        "\tKFile f;",
        "\tWA_TenTepBat(sz, sizeof(sz), uId);",
        "\tif (!f.Open(sz))",
        "\t\treturn -1;\t\t\t\t// chua co tep -> giu cong tac config.ini",
        "\tif (f.Read(&b, 1) != 1)",
        "\t\tb = 0;",
        "\tf.Close();",
        "\treturn b ? 1 : 0;",
        "}",
        "",
        "static void WA_GhiBat(unsigned int uId, int bBat)",
        "{",
        "\tchar sz[128];",
        "\tBYTE b = (BYTE)(bBat ? 1 : 0);",
        "\tKFile f;",
        "\tif (!uId)",
        "\t\treturn;",
        "\tCreateDirectory(WA_THU_MUC, NULL);",
        "\tWA_TenTepBat(sz, sizeof(sz), uId);",
        "\tif (!f.Create(sz))",
        "\t\treturn;",
        "\tf.Write(&b, 1);",
        "\tf.Close();",
        "}",
    ], "JxWAutoNoiBo.cpp: tep nho bat/tat")

    # b) nap trang thai khi nap cau hinh cua nhan vat
    s = thay(s, [
        "\tg_DebugLog(\"[WAUTO] nap %s: %u/%u byte; fight=%d vis=%d pick=%d life=%d TK=%d DT=%d\", sz, (unsigned)dwDoc, (unsigned)sizeof(autoData),",
        "\t\ts_CauHinh.bFight, s_CauHinh.nVision, s_CauHinh.bPickUp, s_CauHinh.bCheckiLife, s_CauHinh.bTongKim, s_CauHinh.bDaTau);",
        "\treturn 1;",
    ], [
        "\tg_DebugLog(\"[WAUTO] nap %s: %u/%u byte; fight=%d vis=%d pick=%d life=%d TK=%d DT=%d\", sz, (unsigned)dwDoc, (unsigned)sizeof(autoData),",
        "\t\ts_CauHinh.bFight, s_CauHinh.nVision, s_CauHinh.bPickUp, s_CauHinh.bCheckiLife, s_CauHinh.bTongKim, s_CauHinh.bDaTau);",
        "\t{\t// %s nho trang thai bat/tat cua lan choi truoc (chua co tep thi giu cong tac config.ini)" % DAU,
        "\t\tint nBatCu = WA_DocBat(uId);",
        "\t\tif (nBatCu >= 0)",
        "\t\t{",
        "\t\t\ts_nBat = nBatCu;",
        "\t\t\tg_DebugLog(\"[WAUTO] nho trang thai lan truoc: Bat=%d\", s_nBat);",
        "\t\t}",
        "\t}",
        "\treturn 1;",
    ], "JxWAutoNoiBo.cpp: nap trang thai bat")

    # c) ghi lai khi doi trang thai
    s = thay(s, [
        "int JxWAuto_Bat(int bBat)\t\t\t{ s_nBat = bBat ? 1 : 0; return s_nBat; }",
    ], [
        "int JxWAuto_Bat(int bBat)",
        "{",
        "\ts_nBat = bBat ? 1 : 0;",
        "\tWA_GhiBat(s_uId, s_nBat);\t\t// %s nho cho lan mo app sau" % DAU,
        "\treturn s_nBat;",
        "}",
    ], "JxWAutoNoiBo.cpp: ghi trang thai bat")

    # d) ep bUseFKey = 0 moi nhip
    s = thay(s, [
        "\tpGL->setting.uACTuoi = 0;",
    ], [
        "\tpGL->setting.uACTuoi = 0;",
        "\t// %s Dien thoai khong co ban phim: Wnd_IsPKKeyDown() luon 0 nen bUseFKey = 1 la may PK KHONG BAO GIO chay" % DAU,
        "\t// (S3Client.cpp:1182). Tep .dat chep tu ban PC co the dang bat -> ep 0 o day, o tick da bo khoi giao dien mobile.",
        "\tpGL->setting.bUseFKey = 0;",
    ], "JxWAutoNoiBo.cpp: ep bUseFKey = 0")
    return s


def main():
    va("Sources/Core/Src/CoreShell.cpp", "JxCore_WAutoSoLieu", va_coreshell)
    va("Sources/S3Client/Platform/JxWAutoNoiBo.h", "tep <APdata>", va_noibo_h)
    va("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", "WA_TenTepBat", va_noibo_cpp)
    print("xong wauto8")


if __name__ == "__main__":
    main()
