# -*- coding: utf-8 -*-
#
# [KYNANG 12/09 O] Chu 13:50:
#  (1) "cac o ky nang khi moi vao phai no tu add ky nang vao luc go khong duoc": tep nho o ky nang la
#      UserData\KyNangMobile.ini - MOT tep dung CHUNG cho MOI nhan vat. Nhan vat moi (moi vao phai) doc phai
#      cac ma ky nang cua nhan vat KHAC -> o hien ky nang la, ma trong so ky nang cua minh khong co ma do nen
#      "Go khoi o" tim khong ra -> go khong duoc. Sua: moi nhan vat mot tep UserData\KyNangMobile_<ma>.ini
#      (ma nhan vat lay bang JxCore_WAutoNhanVat nhu ben WAuto), doi nhan vat thi doc lai.
#  (2) "lam cac icon ky nang khi bo vao o ky nang to len bang o tron": bieu tuong ky nang dang ve CO CO DINH
#      (KyNangCoIcon = 32) trong khi nut to 92 / 60 / 48 -> nut chinh chi lap 35 % vong tron. Sua: ve theo
#      PHAN TRAM duong kinh nut ([Cham] KyNangIconPhanTram, mac dinh 78).
import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[KYNANG 12/09 O]"
TEP = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(c), ten))
    return s.replace(c, nl.join(moi))


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


s = doc(TEP)
if DAU in s:
    raise SystemExit("da va roi")
s0 = s

# ---- (1) tep nho o ky nang theo TUNG NHAN VAT ----
s = thay(s, ["static void KyNang_DuongTepGan(char* pszRa, int nCo)",
             "{",
             "\tpszRa[0] = 0;",
             "\tGetCurrentDirectory(nCo, pszRa);",
             "\tstrncat(pszRa, \"\\\\UserData\\\\KyNangMobile.ini\", nCo - strlen(pszRa) - 1);",
             "}"],
            ["extern \"C\" int JxCore_WAutoNhanVat(unsigned int* puId, int* pnLifeMax, int* pnManaMax);\t// %s CoreShell.cpp" % DAU,
             "static unsigned int s_uKNNhanVat = 0;\t// ma nhan vat cua tep dang nho (0 = chua vao game)",
             "",
             "//\t%s Ma nhan vat dang choi (0 = chua vao the gioi)." % DAU,
             "static unsigned int KyNang_MaNhanVat()",
             "{",
             "\tunsigned int uId = 0;",
             "\tif (!JxCore_WAutoNhanVat(&uId, NULL, NULL))",
             "\t\treturn 0;",
             "\treturn uId;",
             "}",
             "",
             "//\tTep nho o ky nang cua RIENG mot nhan vat. Truoc day moi nhan vat dung chung mot tep nen nhan vat",
             "//\tmoi (moi vao phai) doc phai ma ky nang cua nhan vat khac: o hien ky nang la ma \"Go khoi o\" tim",
             "//\tkhong ra -> go khong duoc (chu 13:50).",
             "static void KyNang_DuongTepGan(char* pszRa, int nCo)",
             "{",
             "\tunsigned int uId = KyNang_MaNhanVat();",
             "",
             "\tpszRa[0] = 0;",
             "\tGetCurrentDirectory(nCo, pszRa);",
             "\tif (uId)",
             "\t{",
             "\t\tchar szTen[64];",
             "\t\t_snprintf(szTen, sizeof(szTen), \"\\\\UserData\\\\KyNangMobile_%u.ini\", uId);",
             "\t\tszTen[sizeof(szTen) - 1] = 0;",
             "\t\tstrncat(pszRa, szTen, nCo - strlen(pszRa) - 1);",
             "\t}",
             "\telse",
             "\t\tstrncat(pszRa, \"\\\\UserData\\\\KyNangMobile.ini\", nCo - strlen(pszRa) - 1);",
             "}"], "duong tep")

s = thay(s, ["static void KyNang_DocGan()",
             "{",
             "\tchar szTep[MAX_PATH];",
             "\tchar szDong[128];",
             "\tFILE* pTep;",
             "",
             "\tif (s_nKNDaDocGan)",
             "\t\treturn;"],
            ["static void KyNang_DocGan()",
             "{",
             "\tchar szTep[MAX_PATH];",
             "\tchar szDong[128];",
             "\tFILE* pTep;",
             "\tunsigned int uId = KyNang_MaNhanVat();\t// %s doi nhan vat thi doc lai tep cua nhan vat do" % DAU,
             "",
             "\tif (s_nKNDaDocGan && uId == s_uKNNhanVat)",
             "\t\treturn;",
             "\ts_uKNNhanVat = uId;"], "doc gan")

# ---- (2) bieu tuong ky nang to bang o tron ----
s = thay(s, ["static int\t\t\ts_nKNCoIcon = 32;\t// co THAT cua bieu tuong ky nang (DrawSkillIcon bo qua Width/Height)"],
            ["static int\t\t\ts_nKNCoIcon = 32;\t// co THAT cua bieu tuong ky nang (DrawSkillIcon bo qua Width/Height)",
             "static int\t\t\ts_nKNIconPT = 78;\t// %s bieu tuong = bao nhieu %% duong kinh nut (chu: \"icon to len bang o tron\")" % DAU], "bien phan tram")

s = thay(s, ["\ts_nKNCoIcon  = GetPrivateProfileInt(\"Cham\", \"KyNangCoIcon\", 32, szCfg);"],
            ["\ts_nKNCoIcon  = GetPrivateProfileInt(\"Cham\", \"KyNangCoIcon\", 32, szCfg);",
             "\ts_nKNIconPT  = GetPrivateProfileInt(\"Cham\", \"KyNangIconPhanTram\", 78, szCfg);\t// %s" % DAU,
             "\tif (s_nKNIconPT < 30) s_nKNIconPT = 30;",
             "\tif (s_nKNIconPT > 100) s_nKNIconPT = 100;"], "doc phan tram")

s = thay(s, ["\t\tif (bCo)",
             "\t\t{",
             "\t\t\tnIcon = s_nKNCoIcon;",
             "\t\t\tg_pCoreShell->DrawGameObj(o.uGenre, o.uId,",
             "\t\t\t\tnX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);",
             "\t\t}"],
            ["\t\tif (bCo)",
             "\t\t{",
             "\t\t\t//\t%s bieu tuong ve theo PHAN TRAM duong kinh nut (truoc day co dinh 32 px nen nut chinh 92 px" % DAU,
             "\t\t\t//\tchi lap 35 %% vong tron - chu: \"lam cac icon ky nang bo vao o to len bang o tron\").",
             "\t\t\tnIcon = nR * 2 * s_nKNIconPT / 100;",
             "\t\t\tif (nIcon < 8)",
             "\t\t\t\tnIcon = 8;",
             "\t\t\tg_pCoreShell->DrawGameObj(o.uGenre, o.uId,",
             "\t\t\t\tnX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);",
             "\t\t}"], "ve icon")

if cao(s) != cao(s0):
    raise SystemExit("byte cao doi")
ghi(TEP, s)
print("da va:", TEP)
