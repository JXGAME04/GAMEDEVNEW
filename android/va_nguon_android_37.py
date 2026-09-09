# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG B] Noi nut ky nang vao bo nhan cham va mach ve.
#
# Them mot trang thai cham moi: CHAM_KYNANG. Bat NGAY luc dat ngon (khong doi xe dich
# nhu can dieu khien), vi nut ky nang la mot o cu the - dat ngon trung no la chac chan
# muon dung no, khong the la thao tac gi khac. Nho vay cham vao nut cung KHONG lot mot
# cu bam chuot xuong duoi game.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 KYNANG]"

# ================================================================= 1. KSdlApp.h
P = "Sources/S3Client/Platform/KSdlApp.h"
s = io.open(P, encoding="latin-1", newline="").read()
if "CHAM_KYNANG" in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tenum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN, CHAM_CUON };"
    assert s.count(CU) == 1, "khong tim thay enum trang thai cham (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 KYNANG] CHAM_KYNANG = dang giu mot nut ky nang (keo = ngam huong danh)",
        "\tenum { CHAM_KHONG = 0, CHAM_CHO, CHAM_KEO, CHAM_PHAI, CHAM_RE, CHAM_CAN, CHAM_CUON,",
        "\t\tCHAM_KYNANG };",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================= 2. KSdlApp.cpp
P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- dat ngon: nut ky nang bat ngay ---
    CU = NL.join([
        "\t\t\tm_nCham = CHAM_CHO;",
        "\t\t\tm_nChamX0 = m_nChamX = (int)fx; m_nChamY0 = m_nChamY = (int)fy;",
        "\t\t\tm_uChamDat = (unsigned int)SDL_GetTicks();",
    ])
    assert s.count(CU) == 1, "khong tim thay doan dat ngon (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t\tm_nCham = CHAM_CHO;",
        "\t\t\tm_nChamX0 = m_nChamX = (int)fx; m_nChamY0 = m_nChamY = (int)fy;",
        "\t\t\tm_uChamDat = (unsigned int)SDL_GetTicks();",
        "\t\t\t// [ANDROID 09/09 KYNANG] Dat ngon trung mot nut ky nang thi bat NGAY, khong doi",
        "\t\t\t// xe dich: nut la mot o cu the nen dat trung no la chac chan muon dung no.",
        "\t\t\t// Nho vay cham vao nut cung khong lot mot cu bam chuot xuong duoi game.",
        "\t\t\t{",
        "\t\t\t\tint nNutKN = JxKyNang_TrungNut(m_nChamX0, m_nChamY0);",
        "\t\t\t\tif (nNutKN > 0)",
        "\t\t\t\t{",
        "\t\t\t\t\tm_nCham = CHAM_KYNANG;",
        "\t\t\t\t\tJxKyNang_BatDau(nNutKN, m_nChamX0, m_nChamY0);",
        "\t\t\t\t\treturn true;",
        "\t\t\t\t}",
        "\t\t\t}",
    ])
    s = s.replace(CU, MOI)

    # --- nha ngon ---
    CU = NL.join([
        "\t\tif (nTruoc == CHAM_CAN)",
        "\t\t{",
        "\t\t\tJxCan_Nha();",
        "\t\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay nhanh nha ngon cua can (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\tif (nTruoc == CHAM_KYNANG)",
        "\t\t{",
        "\t\t\tJxKyNang_Nha();\t\t// [ANDROID 09/09 KYNANG] nha ngon = danh",
        "\t\t}",
        "\t\telse if (nTruoc == CHAM_CAN)",
        "\t\t{",
        "\t\t\tJxCan_Nha();",
        "\t\t}",
    ])
    s = s.replace(CU, MOI)

    # --- ngon di chuyen ---
    CU = NL.join([
        "\t\tif (m_nCham == CHAM_CAN)",
        "\t\t{",
        "\t\t\tJxCan_Keo(m_nChamX, m_nChamY);",
        "\t\t\treturn true;",
        "\t\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay nhanh keo cua can (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\tif (m_nCham == CHAM_KYNANG)",
        "\t\t{",
        "\t\t\tJxKyNang_Keo(m_nChamX, m_nChamY);\t// [ANDROID 09/09 KYNANG] ngam huong danh",
        "\t\t\treturn true;",
        "\t\t}",
        CU,
    ])
    s = s.replace(CU, MOI)

    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================= 3. UiShell.cpp
P = "Sources/S3Client/Ui/UiShell.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tJxCan_Ve();\t// [ANDROID 09/09 CAN] ve can len tren cung, ngay truoc khi ket khung"
    assert s.count(CU) == 1, "khong tim thay loi goi JxCan_Ve (%d)" % s.count(CU)
    MOI = NL.join([
        "\tJxKyNang_Ve();\t// [ANDROID 09/09 KYNANG] bang nut ky nang + vach ngam + vong duoi chan dich",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
