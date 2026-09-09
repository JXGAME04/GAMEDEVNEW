# -*- coding: utf-8 -*-
#
# [UITOADO 09/09 F] Nhieu icon KHONG keo rieng duoc, keo thi di CA CUM.
#
# Chu: "fix luon cai chinh toa do co nhieu icon toi khong kich di chuyen duoc - co di
# chuyen duoc thi no cung di ca cum".
#
# Nguyen nhan: khoa cua mot o la "<ten lop cua so goc>|<ten muc ini>", ma ten muc ini chi
# co khi o do duoc Init tu mot muc trong tep .ini. Rat nhieu o duoc tao THANG bang ma
# (AddChild) nen khong co ten muc -> TaoKhoa tra false -> TimODuoiChuot LEO LEN CHA
# (UiToaDo.cpp:686) va tra ve cua so cha. Ket qua dung nhu chu ta: o do khong keo rieng
# duoc, ma keo thi di ca cum.
#
# Sua: o nao khong co ten muc ini thi dat cho no ten THAY THE theo VI TRI trong cay cua so:
#     <ten lop cua so goc>|#<so thu tu cua cha>.<so thu tu cua no>
# Thu tu nay do trinh tu dung giao dien quyet dinh, ma trinh tu do co dinh (dung tu ini
# theo dung mot thu tu moi lan chay), nen khoa on dinh giua cac lan mo game.
#
# Kem theo: [ANDROID 09/09 NGHIENG B] mui ten huong di van nam tren nguoi khi di LEN vi
# toi ep truc Y con mot nua (dung ve hinh hoc cho vat nam tren MAT DAT, nhung lam dau
# hieu huong len/xuong gan nhu khong nhin thay). Bo phan ep do.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"

# ================================================================ 1. UiToaDo
P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
DAU = "[UITOADO 09/09 F]"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "static bool TaoKhoaTuOCon(KWndWindow* pWnd, char* pszRa, int nCo)",
        "{",
        "\tchar szLop[64];",
        "",
        "\tpszRa[0] = 0;",
        "\tif (pWnd == NULL)",
        "\t\treturn false;",
        "\tLayTenLop(pWnd->GetOwner(), szLop, sizeof(szLop));",
        "\treturn TaoKhoa(szLop, pWnd, pszRa, nCo);",
        "}",
    ])
    assert s.count(CU) == 1, "khong tim thay TaoKhoaTuOCon (%d)" % s.count(CU)
    MOI = NL.join([
        "//\t[UITOADO 09/09 F] So thu tu cua mot o trong danh sach con cua cha no (-1 = khong ro).",
        "static int SoThuTuCon(KWndWindow* pWnd)",
        "{",
        "\tKWndWindow*\tpCha;",
        "\tKWndWindow*\tp;",
        "\tint\t\t\ti = 0;",
        "",
        "\tif (pWnd == NULL)",
        "\t\treturn -1;",
        "\tpCha = pWnd->GetParent();",
        "\tif (pCha == NULL)",
        "\t\treturn -1;",
        "\tfor (p = pCha->GetFirstChild(); p; p = p->GetNextWnd(), i++)",
        "\t{",
        "\t\tif (p == pWnd)",
        "\t\t\treturn i;",
        "\t}",
        "\treturn -1;",
        "}",
        "",
        "static bool TaoKhoaTuOCon(KWndWindow* pWnd, char* pszRa, int nCo)",
        "{",
        "\tchar szLop[64];",
        "",
        "\tpszRa[0] = 0;",
        "\tif (pWnd == NULL)",
        "\t\treturn false;",
        "\tLayTenLop(pWnd->GetOwner(), szLop, sizeof(szLop));",
        "\tif (TaoKhoa(szLop, pWnd, pszRa, nCo))",
        "\t\treturn true;",
        "",
        "\t//\t[UITOADO 09/09 F] O nay khong nap tu ini nen khong co ten muc. Truoc day tra",
        "\t//\tfalse -> ben goi LEO LEN CHA -> keo mot icon thanh keo CA CUM, va nhieu icon",
        "\t//\tkhong keo rieng duoc. Nay dat ten thay the theo VI TRI trong cay cua so:",
        "\t//\t\t<ten lop cua so goc>|#<thu tu cua cha>.<thu tu cua no>",
        "\t//\tTrinh tu dung giao dien co dinh moi lan chay nen khoa nay on dinh.",
        "\tif (szLop[0])",
        "\t{",
        "\t\tint nToi = SoThuTuCon(pWnd);",
        "\t\tint nCha = SoThuTuCon(pWnd->GetParent());",
        "",
        "\t\tif (nToi >= 0)",
        "\t\t{",
        "\t\t\t_snprintf(pszRa, nCo, \"%s|#%d.%d\", szLop, nCha, nToi);",
        "\t\t\tpszRa[nCo - 1] = 0;",
        "\t\t\treturn true;",
        "\t\t}",
        "\t}",
        "\treturn false;",
        "}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 2. mui ten huong di
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 NGHIENG B]"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\t\tint nY = SCREEN_HEIGHT / 2 + s_nHuongDiThap - (int)(sin(fGocD) * nDay / 2.0);"
    assert s.count(CU) == 1, "khong tim thay dong tinh Y cua mui ten (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t// [ANDROID 09/09 NGHIENG B] KHONG ep truc Y con mot nua nua. Ep 1/2 la dung ve",
        "\t\t// hinh hoc cho vat nam tren MAT DAT, nhung lam dau hieu len/xuong gan nhu khong",
        "\t\t// nhin thay: do duoc khi di LEN mui ten van roi ngay tren nguoi (tam (521,301)",
        "\t\t// trong khi giua man hinh la (520,302)) - dung nhu chu ta \"van nam sau dit\".",
        "\t\tint nY = SCREEN_HEIGHT / 2 + s_nHuongDiThap - (int)(sin(fGocD) * nDay);",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
