# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 SUAKEO] Dang sua giao dien thi VUOT phai la KEO, khong phai cuon.
#
# Chu: "co nhieu icon toi khong kich di chuyen duoc". Ban va 51 da sua duoc phan dat ten
# (o khong nap tu ini van co khoa rieng), nhung do tay tren may ao van khong keo duoc.
#
# Nguyen nhan thu hai, nam o bo nhan cham: vuot DOC tren mot o giao dien bi doi thanh
# CUON DANH SACH (KSdlApp.cpp, nhanh CHAM_CUON - lam tu 09/09 cho khung thoai NPC).
# Trong che do sua giao dien thi y dinh luon luon la KEO, khong bao gio la cuon.
#
# Sua: dang sua giao dien thi bo qua ca hai nhanh CAN DIEU KHIEN va CUON, di thang vao
# nhanh keo. Chi anh huong khi dang o che do sua nen khong dung toi thao tac choi binh thuong.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/KSdlApp.cpp"
DAU = "[ANDROID 09/09 SUAKEO]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t\t\t// [ANDROID 09/09 CAN] Keo o vung ben trai (ngoai giao dien) = CAN DIEU KHIEN;",
    ])
    assert s.count(CU) == 1, "khong tim thay dau nhanh xe dich (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t\t// [ANDROID 09/09 SUAKEO] Dang sua giao dien: y dinh luon la KEO o giao dien,",
        "\t\t\t// khong bao gio la cuon danh sach hay cam can. Khong co dong nay thi vuot doc",
        "\t\t\t// tren mot o se thanh CUON (nhanh ngay duoi) va o do khong bao gio doi cho duoc.",
        "\t\t\tif (UiToaDo_DangSua())",
        "\t\t\t{",
        "\t\t\t\tm_nCham = CHAM_KEO;",
        "\t\t\t\tGhiChuot(MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));",
        "\t\t\t\tMsgProc(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(m_nChamX0, m_nChamY0));",
        "\t\t\t}",
        "\t\t\telse",
        CU,
    ])
    s = s.replace(CU, MOI)

    # nhanh CAN nam ngay sau -> phai bo vao else o tren, nen doi thanh if lien tiep
    CU2 = NL.join([
        "\t\t\telse",
        "\t\t\t// [ANDROID 09/09 CAN] Keo o vung ben trai (ngoai giao dien) = CAN DIEU KHIEN;",
        "\t\t\t// keo o cho khac = giu chuot trai roi re nhu ban PC (di lien tuc, keo tha vat pham).",
        "\t\t\tif (JxCan_TrongVung(m_nChamX0, m_nChamY0) && !JxUi_CoGiaoDienTaiDiem(m_nChamX0, m_nChamY0))",
    ])
    assert s.count(CU2) == 1, "khong ghep duoc else voi nhanh CAN (%d)" % s.count(CU2)

    # them include
    CU3 = '#include "JxCanDieuKhien.h"'
    if CU3 in s and '#include "../Ui/Elem/UiToaDo.h"' not in s:
        s = s.replace(CU3, CU3 + NL + '#include "../Ui/Elem/UiToaDo.h"\t// [ANDROID 09/09 SUAKEO] UiToaDo_DangSua', 1)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
