# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 NGUA] Chu: "loi ngua khong hien hinh o ban client mobile, ban pc hien day du".
#
# Nguyen nhan (do 11/09 23:30): anh ngua cua nhan vat nu duoc xin theo duong dan TUONG DOI qua thu muc nam:
#   \spr\npcres\woman\..\man\MA_HH_002_RD01.spr   (jx_rep3.log: "anh_null top: ... x8878")
# Tren PC, g_GetFullPath (KFilePath.cpp) gap "\..\" thi RemoveTwoPointPath rut gon thanh \spr\npcres\man\... roi moi bam id tim
# trong pak (spr.pak co id 30c8b46d = \spr\npcres\man\ma_hh_002_rd01.spr). Ban Android dich khong co WIN32 (JX_POSIX) nen
# RemoveTwoPointPath / RemoveOnePointPath chi tim "/../" va "/./" (nhanh Linux), trong khi ten tai nguyen van dung dau nguoc
# -> khong rut gon -> id 33ef6bd0 khong co trong pak -> anh null -> ngua tang hinh (ca thu nhan vat nam khi lay anh cheo).
#
# Sua (chi khi JX_ANDROID; ban PC WIN32 va ban Linux server giu nguyen ma): sau khi tim dau xuoi, tim them "\..\" va "\.\".
# Nguon KFilePath.cpp co byte cao (chu thich tieng Trung) -> sua bang latin-1. Chay lai vo hai.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 NGUA]"


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


def va_kfilepath(s):
    # RemoveTwoPointPath: sau nhanh "/../" (khong WIN32) them nhanh "\..\" cho Android
    s = thay(s, ["int RemoveTwoPointPath(LPTSTR szPath, int nLength)",
                 "{",
                 "\tint nRemove = 0;",
                 "\tKASSERT(szPath);",
                 "#ifdef WIN32",
                 "\tLPCTSTR lpszOld = \"\\\\..\\\\\";",
                 "#else",
                 "\tLPCTSTR lpszOld = \"/../\";",
                 "#endif",
                 "\tLPTSTR lpszTarget = strstr(szPath, lpszOld);"],
                ["int RemoveTwoPointPath(LPTSTR szPath, int nLength)",
                 "{",
                 "\tint nRemove = 0;",
                 "\tKASSERT(szPath);",
                 "#ifdef WIN32",
                 "\tLPCTSTR lpszOld = \"\\\\..\\\\\";",
                 "#else",
                 "\tLPCTSTR lpszOld = \"/../\";",
                 "#endif",
                 "\tLPTSTR lpszTarget = strstr(szPath, lpszOld);",
                 "#ifdef JX_ANDROID",
                 "\t// %s ten tai nguyen tren Android van dung dau nguoc (\"\\\\spr\\\\npcres\\\\woman\\\\..\\\\man\\\\...\"): khong rut gon thi id bam" % DAU,
                 "\t// khac id trong pak -> anh ngua null. Ban PC (WIN32) da rut gon san; ban Linux server chi co dau xuoi - khong dong den.",
                 "\tif (!lpszTarget)",
                 "\t{",
                 "\t\tlpszOld = \"\\\\..\\\\\";",
                 "\t\tlpszTarget = strstr(szPath, lpszOld);",
                 "\t}",
                 "#endif"], "KFilePath.cpp: RemoveTwoPointPath")
    s = thay(s, ["int RemoveOnePointPath(LPTSTR szPath, int nLength)",
                 "{",
                 "\tint nRemove = 0;",
                 "\tKASSERT(szPath);",
                 "#ifdef WIN32",
                 "\tLPCTSTR lpszOld = \"\\\\.\\\\\";",
                 "#else",
                 "\tLPCTSTR lpszOld = \"/./\";",
                 "#endif",
                 "\tLPTSTR lpszTarget = strstr(szPath, lpszOld);"],
                ["int RemoveOnePointPath(LPTSTR szPath, int nLength)",
                 "{",
                 "\tint nRemove = 0;",
                 "\tKASSERT(szPath);",
                 "#ifdef WIN32",
                 "\tLPCTSTR lpszOld = \"\\\\.\\\\\";",
                 "#else",
                 "\tLPCTSTR lpszOld = \"/./\";",
                 "#endif",
                 "\tLPTSTR lpszTarget = strstr(szPath, lpszOld);",
                 "#ifdef JX_ANDROID",
                 "\tif (!lpszTarget)\t// %s nhu RemoveTwoPointPath: them dang dau nguoc \"\\\\.\\\\\"" % DAU,
                 "\t{",
                 "\t\tlpszOld = \"\\\\.\\\\\";",
                 "\t\tlpszTarget = strstr(szPath, lpszOld);",
                 "\t}",
                 "#endif"], "KFilePath.cpp: RemoveOnePointPath")
    return s


va("Sources/Engine/Src/KFilePath.cpp", DAU, va_kfilepath)
print("xong")
