# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG G] He so GIAN cho cum nut.
#
# Do lech tung o lay dung cua ban tham khao, nhung ban tham khao ve khung bang anh
# 70x70 nhan ti le 1.5 / 1.2, con bo anh VNKU cua ta co san bon co that (70/100/140/200)
# va ham ve o day KHONG phong to thu nho anh duoc. Ket qua: khung to hon khoang cach
# giua cac o -> cac vong tron chong len nhau, kho cham dung o.
#
# Them KyNangGian (phan tram) nhan vao do lech, giu nguyen HINH CUNG cua ban tham khao
# ma noi rong ra cho vua bo anh. Mac dinh 120%.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 KYNANG G]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "static int\t\t\ts_nKNBanKinhKeo = 60;"
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        "//\t[ANDROID 09/09 KYNANG G] noi rong cum nut: do lech goc tinh cho khung 70x70 nhan",
        "//\tti le, con ta ve anh o co that (70/100/140) nen phai gian ra keo cac vong chong nhau.",
        "static int\t\t\ts_nKNGian = 120;\t// phan tram",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nKNBanKinhKeo = GetPrivateProfileInt("Cham", "KyNangBanKinhKeo", 60, szCfg);'
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        '\ts_nKNGian = GetPrivateProfileInt("Cham", "KyNangGian", 120, szCfg);',
        "\tif (s_nKNGian < 50)  s_nKNGian = 50;",
        "\tif (s_nKNGian > 300) s_nKNGian = 300;",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nKNSangPhai = GetPrivateProfileInt("Cham", "KyNangSangPhai", 60, szCfg);'
    assert s.count(CU) == 1
    s = s.replace(CU, '\ts_nKNSangPhai = GetPrivateProfileInt("Cham", "KyNangSangPhai", 24, szCfg);')

    CU = "static int\t\t\ts_nKNSangPhai = 60;\t// doi ca cum sang phai bao nhieu diem anh"
    assert s.count(CU) == 1
    s = s.replace(CU, "static int\t\t\ts_nKNSangPhai = 24;\t// doi ca cum sang phai bao nhieu diem anh")

    CU = NL.join([
        "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
        "\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
    ])
    assert s.count(CU) == 1
    MOI = NL.join([
        "\t// [ANDROID 09/09 KYNANG G] gian do lech ra cho vua bo anh (giu nguyen hinh cung)",
        "\tnDX = nDX * s_nKNGian / 100;",
        "\tnDY = nDY * s_nKNGian / 100;",
        "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
        "\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
