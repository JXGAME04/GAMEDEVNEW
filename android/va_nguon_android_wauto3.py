# -*- coding: utf-8 -*-
#
# [ANDROID 11/09 WAUTO B0 c] Sua sau lan thu B0 thu hai tren LDPlayer: tep APdata\<id>.dat co RAC trong mang ten to doi
# (wauto_dat.py xem: szIJPtName = ['r8v', 'r8v', 'u8v']). Goc: JxWAuto_NapCauHinh() lay ban mac dinh tu "autoData mac;" tren
# NGAN XEP - constructor cua autoData chi gan tung truong so, KHONG xoa trang cac mang chuoi (szIJPtName[24][32],
# szNOPName[60][80], szLDPtName...), nen rac ngan xep lot vao s_CauHinh roi ghi ra tep. WAuto.exe khong dinh vi gameNode cua no
# nam trong std::vector (bo nho duoc xoa trang). Sua: lay tu doi tuong TINH (bo nho tinh duoc xoa trang truoc khi constructor
# chay) - chi tep Android-only JxWAutoNoiBo.cpp, ban PC khong lien quan.
# Ten kich ban: wauto3 (ban dau 85 roi 89, deu trung voi va TTMT cua phien khac -> WAuto dat ten rieng, khong dua so). Chay lai vo hai. Doc/ghi latin-1, giu CRLF, chi them dong ASCII.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[ANDROID 11/09 WAUTO B0 c]"


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


def va_noibo(s):
    return thay(s, ["\tautoData mac;",
                    "\ts_CauHinh = mac;\t\t\t\t\t\t// ve mac dinh cua struct truoc"],
                   ["\t// %s Ban mac dinh lay tu doi tuong TINH: bo nho tinh duoc xoa trang truoc khi constructor chay, nen cac" % DAU,
                    "\t// mang chuoi (szIJPtName, szNOPName, szLDPtName...) la 0. Bien tam tren ngan xep thi constructor KHONG xoa mang -> rac",
                    "\t// ('r8v') lot vao tep .dat (thay o lan thu B0 thu hai).",
                    "\tstatic const autoData s_MacDinh;",
                    "\ts_CauHinh = s_MacDinh;\t\t\t\t\t// ve mac dinh cua struct truoc"],
                   "JxWAutoNoiBo.cpp: mac dinh tu doi tuong tinh")


va("Sources/S3Client/Platform/JxWAutoNoiBo.cpp", DAU, va_noibo)
print("xong")
