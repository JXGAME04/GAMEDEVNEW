# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 HUONGDI B] Mui ten huong di phai nam PHIA TRUOC nhan vat.
#
# Chu: "van lech - di xuong thi dung, di len mui ten no nam sau lung".
# Dung: ban truoc toi ve mui ten o MOT CHO co dinh (duoi chan, lech xuong 16 diem anh).
# Di xuong thi tinh co dung phia truoc; di len thi cho do thanh sau lung.
#
# Sua: dat mui ten o chan nhan vat RO I DAY RA THEO HUONG DANG DI - tuc no chay vong
# quanh chan theo huong, luon nam phia truoc.
#
#   goc = (48 - huong) * 5.625 do     (bang huong cua game: 48 = phai, 32 = len,
#                                      16 = trai, 0 = xuong - dung bang da dung cho can)
#   x = giua man hinh + cos(goc) * R
#   y = chan nhan vat  - sin(goc) * R / 2      (chia doi vi goc nhin nghieng, va truc Y
#                                              cua man hinh huong XUONG nen doi dau)

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 HUONGDI B]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t// Ve theo KHUNG co dinh thay vi de anh tu neo: anh .spr nay co khung rong hon",
        "\t// phan nhin thay nen neo theo co khung lam mui ten roi thap hon chan (da do).",
        "\tVeAnhKhung(s_szHuongDiAnh, SCREEN_WIDTH / 2,",
        "\t\tSCREEN_HEIGHT / 2 + s_nHuongDiThap, 34, 38, nKhung);",
    ])
    assert s.count(CU) == 1, "khong tim thay cho ve mui ten (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 HUONGDI B] Mui ten phai nam PHIA TRUOC nhan vat, khong phai mot",
        "\t// cho co dinh: day no ra theo dung huong dang di, chia doi truc Y vi goc nhin",
        "\t// nghieng. (Truoc day ve o mot cho nen di len thi mui ten nam sau lung.)",
        "\t{",
        "\t\tdouble fGocD = (48.0 - (double)s_nHuong) * 5.625 * 3.14159265358979 / 180.0;",
        "\t\tint nDay = s_nHuongDiXa;",
        "\t\tint nX = SCREEN_WIDTH  / 2 + (int)(cos(fGocD) * nDay);",
        "\t\tint nY = SCREEN_HEIGHT / 2 + s_nHuongDiThap - (int)(sin(fGocD) * nDay / 2.0);",
        "",
        "\t\t// Ve theo KHUNG co dinh thay vi de anh tu neo: anh .spr nay co khung rong hon",
        "\t\t// phan nhin thay nen neo theo co khung lam mui ten roi thap hon chan (da do).",
        "\t\tVeAnhKhung(s_szHuongDiAnh, nX, nY, 34, 38, nKhung);",
        "\t}",
    ])
    s = s.replace(CU, MOI)

    CU = "static int\t\t\ts_nHuongDiThap = 16;\t// ve thap hon giua man hinh bao nhieu (duoi chan)"
    assert s.count(CU) == 1
    MOI = NL.join([
        "static int\t\t\ts_nHuongDiThap = 10;\t// chan nhan vat thap hon giua man hinh bao nhieu",
        "static int\t\t\ts_nHuongDiXa = 38;\t\t// day mui ten ra phia truoc bao nhieu diem anh",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nHuongDiThap = GetPrivateProfileInt("Cham", "HuongDiThap", 16, szCfg);'
    assert s.count(CU) == 1
    MOI = NL.join([
        '\ts_nHuongDiThap = GetPrivateProfileInt("Cham", "HuongDiThap", 10, szCfg);',
        '\ts_nHuongDiXa   = GetPrivateProfileInt("Cham", "HuongDiXa", 38, szCfg);',
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
