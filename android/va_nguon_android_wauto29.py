# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Sua LOI CUA CHINH DOT NAY, thay khi thu tren LDPlayer 12:29:
#
#  1. DienChon: nhanh HOP CHON TINH dung bien v ma KHONG gan -> moi hop chon tinh luon hien DONG 0, du autoData
#     dang giu gia tri khac (do 12:29: chon "Truong Bach Son Bac" -> .dat ghi nSelMap = 1 dung, nhung o van hien
#     "Hac Sa dong"). Ban 11/09 gan "v = LayInt(p)" ngay dau ham; ban 12/09 tach LayInt vao tung nhanh cho cac hop
#     co luat rieng (Boss 141+i, Ruong cua theo thanh, Theo sau la chuoi) nhung quen nhanh chung.
#
#  2. Dong huong dan cua bang phu bi cat ngang (o rong 660 px, chu dai hon) -> cat chu cho vua be ngang o.
#
# Chi mobile (JX_MOBILE). Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def V(s):
    return re.sub(r"@@(.*?)@@", lambda m: vn(m.group(1)), s, flags=re.S)


def thay(p, cu, moi, ten):
    s = doc(p)
    cu, moi = V(cu), V(moi)
    if s.count(cu) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(cu), ten))
    ghi(p, s.replace(cu, moi))
    print("   da va:", ten)


def main():
    P = "Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp"
    if "v = LayInt(p);\t\t// %s" % DAU in doc(P):
        print("   bo qua (da va):", P)
        return
    thay(P,
         "\telse if (p->pLuaChon && p->nLuaChon > 0)\r\n"
         "\t{\r\n"
         "\t\tif (v < 0 || v >= (int)p->nLuaChon)\r\n",
         "\telse if (p->pLuaChon && p->nLuaChon > 0)\r\n"
         "\t{\r\n"
         "\t\tv = LayInt(p);\t\t// %s CAC NHANH TREN moi tu doc gia tri; nhanh chung nay truoc do dung v chua gan\r\n" % DAU +
         "\t\t\t\t\t\t\t// -> hop chon tinh nao cung hien dong 0 (do 12:29: nSelMap = 1 ma o van hien dong dau).\r\n"
         "\t\tif (v < 0 || v >= (int)p->nLuaChon)\r\n",
         "UiWAutoTrang.cpp: DienChon doc gia tri cho hop chon tinh")
    print("xong wauto29")


if __name__ == "__main__":
    main()
