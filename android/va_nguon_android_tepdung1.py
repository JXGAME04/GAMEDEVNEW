# -*- coding: utf-8 -*-
#
# [ANDROID 12/09 TEPDUNG] Nhat ky TEP DUNG tren Android (buoc "rut gon toi da" - chi giu du lieu game thuc su mo):
#   Sources/Engine/Src/KFile.cpp    KFile::Open  : mo duoc tep ROI  -> ghi 'R <duong dan>'      (moi ten mot lan)
#   Sources/Engine/Src/KPakList.cpp FindElemFile : tim thay trong pak -> ghi 'P <uid hex> <ten>' (moi uid mot lan)
# vao <thu muc du lieu>\jx_tep_dung.log (ghi them, khong ghi de; tat bang [Client] TepDung=0 trong config.ini).
# Chi JX_ANDROID (ban Windows / Linux khong doi). Bo nho: std::set uid + set chuoi (vai tram KB). Chay lai vo hai.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 12/09 TEPDUNG]"


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


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("da va roi, bo qua:", p)
        return
    s2 = ham(s)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o " + p)
    ghi(p, s2)
    print("da va:", p)


KHOI = [
    "#ifdef JX_ANDROID",
    "// %s nhat ky tep dung (rut gon du lieu): 'R <tep roi>' / 'P <uid> <ten xin>' vao jx_tep_dung.log, moi ten mot lan" % DAU,
    "#include <set>",
    "#include <string>",
    "#include <stdio.h>",
    "extern const char* JxPosix_DataDir(void);",
    "static int s_nTepDungBat = -1;",
    "static std::set<std::string> s_TepDungDaGhi;",
    "void JxTepDung_Ghi(char cLoai, unsigned int uId, const char* pszTen)",
    "{",
    "\tif (s_nTepDungBat < 0)",
    "\t\ts_nTepDungBat = GetPrivateProfileIntA(\"Client\", \"TepDung\", 1, \".\\\\Config.ini\") ? 1 : 0;",
    "\tif (!s_nTepDungBat || !pszTen)",
    "\t\treturn;",
    "\tchar szKhoa[600];",
    "\tif (cLoai == 'P')",
    "\t\tsnprintf(szKhoa, sizeof(szKhoa), \"P %08x %s\", uId, pszTen);",
    "\telse",
    "\t\tsnprintf(szKhoa, sizeof(szKhoa), \"R %s\", pszTen);",
    "\tif (!s_TepDungDaGhi.insert(std::string(szKhoa)).second)",
    "\t\treturn;",
    "\tchar szTep[1200];",
    "\tsnprintf(szTep, sizeof(szTep), \"%s/jx_tep_dung.log\", JxPosix_DataDir());",
    "\tFILE* f = fopen(szTep, \"ab\");",
    "\tif (!f)",
    "\t\treturn;",
    "\tfprintf(f, \"%s\\n\", szKhoa);",
    "\tfclose(f);",
    "}",
    "#endif",
    "",
]


def va_kfile(s):
    # 1. khoi ham sau cac #include dau tep (truoc dinh nghia KFile::Open)
    s = thay(s, ["BOOL KFile::Open(LPSTR FileName)", "{"],
                KHOI + ["BOOL KFile::Open(LPSTR FileName)", "{"], "KFile.cpp: khoi JxTepDung_Ghi")
    # 2. mo duoc tep roi -> ghi (nhanh POSIX: fopen lcasePathName)
    s = thay(s, ["\t\tif (NULL == (m_hFile = fopen(lcasePathName, \"rb\")))"],
                ["\t\tif (NULL != (m_hFile = fopen(lcasePathName, \"rb\")))",
                 "\t\t{",
                 "#ifdef JX_ANDROID",
                 "\t\t\tJxTepDung_Ghi('R', 0, lcasePathName);\t// %s" % DAU,
                 "#endif",
                 "\t\t}",
                 "\t\telse"], "KFile.cpp: ghi tep roi")
    return s


def va_paklist(s):
    s = thay(s, ["\t\tunsigned long uId = FileNameToId(szPackName);",
                 "\t\tbFounded = FindElemFile(uId, ElemRef);"],
                ["\t\tunsigned long uId = FileNameToId(szPackName);",
                 "\t\tbFounded = FindElemFile(uId, ElemRef);",
                 "#ifdef JX_ANDROID",
                 "\t\tif (bFounded)",
                 "\t\t{",
                 "\t\t\textern void JxTepDung_Ghi(char cLoai, unsigned int uId, const char* pszTen);\t// %s (KFile.cpp)" % DAU,
                 "\t\t\tJxTepDung_Ghi('P', (unsigned int)uId, szPackName);",
                 "\t\t}",
                 "#endif"], "KPakList.cpp: ghi id pak")
    return s


va("Sources/Engine/Src/KFile.cpp", DAU, va_kfile)
va("Sources/Engine/Src/KPakList.cpp", DAU, va_paklist)
print("xong")
