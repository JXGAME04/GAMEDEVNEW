# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B0 b] Hai chinh sua sau lan thu B0 dau tien tren LDPlayer (xem BANGIAO_WAUTO_MOBILE_B0_1109.md):
#
#  1. Sources/Core/Src/KCore.cpp  g_AutoLogOn(): nhanh khong-WIN32 ep s_nAutoLog = 0 -> tren Android KHONG BAO GIO co dong
#     [AUTO-PASS] / [HD-GATE] cua ExtAutoLoop (nhat ky duy nhat chung minh BEN NHAN da tieu thu goi WAuto ma khong gay tac
#     dung phu tren may chu that). Android doc cung khoa [Client] AutoLog cua config.ini nhu ban PC (GetPrivateProfileIntA da
#     co lop gia lap), ghi jx_auto.log o thu muc du lieu. Rao JX_ANDROID: ban Windows (nhanh WIN32) va may chu Linux khong doi.
#
#  2. Sources/S3Client/Platform/JxWAutoNoiBo.cpp  JxWAuto_LuuCauHinh(): g_CreatePath("\APdata") tren Android ghep goc + "\APdata"
#     roi dua thang cho SDL_CreateDirectory (dau '\' khong duoc doi) -> khong tao duoc thu muc, tep .dat ghi that bai
#     ("[WAUTO] KHONG ghi duoc \APdata\<id>.dat"). Dung lop gia lap CreateDirectory (KPosixWin32.cpp: JxPathPosix doi '\' -> '/',
#     ghep thu muc du lieu, ha chu thuong => <du lieu>/apdata), dung duong ma KFile::Create se dung cho tep ben trong.
#
# Chay lai vo hai. Doc/ghi latin-1, giu CRLF, chi them dong ASCII: so byte cao khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 WAUTO B0 b]"


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


# ---------------------------------------------------------------- 1. KCore.cpp
def va_kcore(s):
    return thay(s, ["#else",
                    "\t\ts_nAutoLog = 0;",
                    "#endif",
                    "\t}",
                    "\treturn s_nAutoLog;"],
                   ["#else",
                    "#ifdef JX_ANDROID",
                    "\t\t// %s Android doc cung khoa [Client] AutoLog cua config.ini nhu ban PC (GetPrivateProfileIntA da co lop" % DAU,
                    "\t\t// gia lap), ghi jx_auto.log o thu muc du lieu: la nhat ky duy nhat chung minh ExtAutoLoop (ben nhan cua bang WAuto",
                    "\t\t// trong game) da chay. Cac nen POSIX khac (may chu Linux) van 0 nhu cu.",
                    "\t\ts_nAutoLog = (int)GetPrivateProfileIntA(\"Client\", \"AutoLog\", 0, \".\\\\Config.ini\");",
                    "#else",
                    "\t\ts_nAutoLog = 0;",
                    "#endif",
                    "#endif",
                    "\t}",
                    "\treturn s_nAutoLog;"], "KCore.cpp: g_AutoLogOn Android")


# ---------------------------------------------------------------- 2. JxWAutoNoiBo.cpp
def va_noibo(s):
    s = thay(s, ["#include \"KFilePath.h\"\t\t// g_CreatePath"],
                ["// %s khong dung g_CreatePath (KFilePath.h): tren Android no khong doi dau '\\\\' truoc khi goi SDL_CreateDirectory" % DAU],
                "JxWAutoNoiBo.cpp: include")
    s = thay(s, ["\tchar szThuMuc[32];",
                 "\tstrcpy(szThuMuc, WA_THU_MUC);",
                 "\tg_CreatePath(szThuMuc);"],
                ["\t// %s Dung lop gia lap CreateDirectory (KPosixWin32.cpp): JxPathPosix doi '\\\\' -> '/', ghep thu muc du lieu," % DAU,
                 "\t// ha chu thuong (=> <du lieu>/apdata) - dung duong ma KFile::Create dung cho tep ben trong. Da co thi tra FALSE, vo hai.",
                 "\tCreateDirectory(WA_THU_MUC, NULL);"],
                "JxWAutoNoiBo.cpp: tao thu muc")
    return s


va("Sources/Core/Src/KCore.cpp", DAU, va_kcore)
va("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", DAU, va_noibo)
print("xong")
