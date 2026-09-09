# -*- coding: utf-8 -*-
#
# [UITOADO 09/09 C] Ten lop tren Android co tien to so -> khoa luu KHAC ban PC.
#
# Do duoc tren may ao: cham vao khung noi dung hop thu, thanh bao hien
#     14KUiMailManager|MailContentValue  105%
# trong khi tren Windows phai la
#       KUiMailManager|MailContentValue  105%
#
# LayTenLop lay typeid(*pWnd).name(). MSVC tra "class KUiMailManager" nen cat o
# dau cach cuoi la ra ten. GCC / Itanium ABI (Android) tra ten MA HOA dang
# <do dai><ten>, vi du "16KUiMailManager" - khong co dau cach nao de cat.
#
# Hau qua that: khoa luu trong UserData\UiToaDo.ini khac nhau giua hai ban, nen
# giao dien chu tu dat o may tinh KHONG dung lai duoc tren dien thoai va nguoc lai.
#
# Sua: bo qua cac chu so dau ten. Ten lop C++ khong bao gio bat dau bang chu so
# nen viec nay an toan; ten long nhau (dang N3Foo3BarE) thi giu nguyen, cac lop
# giao dien o day deu la lop muc ngoai cung.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
DAU = "[UITOADO 09/09 C]"
NL = "\r\n"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\tconst char*\tpszTen\t= typeid(*pWnd).name();",
        "\tconst char*\tp\t\t= strrchr(pszTen, ' ');",
        "",
        "\tif (p)",
        "\t\tpszTen = p + 1;",
    ])
    assert s.count(CU) == 1, "khong tim thay than LayTenLop (thay %d)" % s.count(CU)
    MOI = NL.join([
        "\tconst char*\tpszTen\t= typeid(*pWnd).name();",
        "\tconst char*\tp\t\t= strrchr(pszTen, ' ');",
        "",
        "\tif (p)",
        "\t\tpszTen = p + 1;\t\t\t\t// MSVC: \"class KUiFoo\" -> \"KUiFoo\"",
        "",
        "\t//\t[UITOADO 09/09 C] GCC / Itanium ABI (Android) tra ten MA HOA dang",
        "\t//\t<do dai><ten>, vi du \"16KUiMailManager\" - khong co dau cach de cat.",
        "\t//\tBo cac chu so dau thi khoa luu giong het ban PC, nho vay tep",
        "\t//\tUserData\\UiToaDo.ini dung chung duoc cho ca hai ban.",
        "\twhile (*pszTen >= '0' && *pszTen <= '9')",
        "\t\tpszTen++;",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
