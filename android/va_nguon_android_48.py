# -*- coding: utf-8 -*-
#
# [UITOADO 09/09 E] Cho cac o VE TAY (khong phai cua so KWnd) doi cho duoc trong che do sua.
#
# Chu: "cac nut ky nang moi them khong co trong phan di chuyen toa do - toi khong di
# chuyen sap xep lai duoc".
#
# Vi sao thieu: UiToaDo lam viec tren cac cua so KWnd (duyet cay cua so, lay ten lop +
# ten muc ini lam khoa). Cum nut ky nang va can dieu khien KHONG phai cua so KWnd - chung
# duoc ve thang bang DrawPrimitives - nen he do khong nhin thay.
#
# Them mot bang O RIENG: moi muc gom mot khoa + ba ham (do trung / lay vi tri / dat vi tri).
# Trong che do sua, keo mot o rieng thi goi ham dat vi tri cua no; luc luu thi ghi vao
# CUNG tep UserData\UiToaDo.ini nhu moi o khac, va luc nap thi goi ham dat lai.
#
# Nho vay bat cu thu gi ve tay sau nay (nut auto, nut nhat do...) chi can dang ky mot dong
# la doi cho duoc, khong phai sua lai he sua giao dien.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[UITOADO 09/09 E]"

# ================================================================ 1. UiToaDo.h
P = "Sources/S3Client/Ui/Elem/UiToaDo.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "//\tCua so sap bi xoa - bo moi con tro dang giu toi no (goi tu Wnd_OnWindowDelete)."
    assert s.count(CU) == 1
    MOI = NL.join([
        "//---------------------------------------------------------------------------",
        "//\t[UITOADO 09/09 E] O VE TAY (khong phai cua so KWnd)",
        "//",
        "//\tCum nut ky nang, can dieu khien... duoc ve thang bang DrawPrimitives nen he sua",
        "//\tgiao dien khong nhin thay chung. Dang ky o day thi chung doi cho duoc y nhu moi",
        "//\to giao dien khac, va vi tri cung duoc luu vao UserData\\UiToaDo.ini.",
        "//---------------------------------------------------------------------------",
        "typedef bool (*PFN_UITOADO_TRUNG)(int x, int y);\t\t\t// (x,y) co trung o nay khong",
        "typedef void (*PFN_UITOADO_LAYVITRI)(int* px, int* py);\t// vi tri hien tai",
        "typedef void (*PFN_UITOADO_DATVITRI)(int x, int y);\t\t// dat vi tri moi",
        "",
        "//\tpszKhoa: ten rieng, dung lam khoa trong tep ini (vi du \"CumKyNang\").",
        "void\tUiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,",
        "\t\t\tPFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat);",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 2. UiToaDo.cpp
P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- bang o rieng ---
    CU = "static void DatThongBao(const char* pszChu);"
    assert s.count(CU) == 1
    MOI = NL.join([
        "//--------------------------------------------------------------------------",
        "//\t[UITOADO 09/09 E] Bang O VE TAY (khong phai cua so KWnd) - xem UiToaDo.h",
        "//--------------------------------------------------------------------------",
        "#define\tUITOADO_ORIENG_MAX\t8",
        "",
        "struct KORieng",
        "{",
        "\tchar\t\t\t\t\tszKhoa[64];",
        "\tPFN_UITOADO_TRUNG\t\tpfnTrung;",
        "\tPFN_UITOADO_LAYVITRI\tpfnLay;",
        "\tPFN_UITOADO_DATVITRI\tpfnDat;",
        "};",
        "",
        "static KORieng\t\ts_ORieng[UITOADO_ORIENG_MAX];",
        "static int\t\t\ts_nSoORieng = 0;",
        "static int\t\t\ts_nKeoORieng = -1;\t// o rieng dang keo, -1 = khong",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- ham dang ky + ap lai ---
    CU = "bool UiToaDo_ChoPhep()"
    assert s.count(CU) == 1
    MOI = NL.join([
        "void UiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,",
        "\t\tPFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat)",
        "{",
        "\tint i;",
        "",
        "\tif (pszKhoa == NULL || pszKhoa[0] == 0 || pfnTrung == NULL",
        "\t\t|| pfnLay == NULL || pfnDat == NULL)",
        "\t\treturn;",
        "\tfor (i = 0; i < s_nSoORieng; i++)",
        "\t{",
        "\t\tif (strcmp(s_ORieng[i].szKhoa, pszKhoa) == 0)",
        "\t\t\treturn;\t\t// da dang ky roi",
        "\t}",
        "\tif (s_nSoORieng >= UITOADO_ORIENG_MAX)",
        "\t\treturn;",
        "\ti = s_nSoORieng++;",
        "\tstrncpy(s_ORieng[i].szKhoa, pszKhoa, sizeof(s_ORieng[i].szKhoa) - 1);",
        "\ts_ORieng[i].szKhoa[sizeof(s_ORieng[i].szKhoa) - 1] = 0;",
        "\ts_ORieng[i].pfnTrung = pfnTrung;",
        "\ts_ORieng[i].pfnLay   = pfnLay;",
        "\ts_ORieng[i].pfnDat   = pfnDat;",
        "",
        "\t//\tCo vi tri da luu thi ap lai ngay",
        "\t{",
        "\t\tint n = TimKhoa(pszKhoa);",
        "\t\tif (n >= 0)",
        "\t\t\tpfnDat(s_Bang[n].nLeft, s_Bang[n].nTop);",
        "\t}",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- ap lai khi nap xong ---
    CU = "void\tUiToaDo_ApChoTatCa()"
    if s.count(CU) == 0:
        CU = "void UiToaDo_ApChoTatCa()"
    assert s.count(CU) == 1, "khong tim thay UiToaDo_ApChoTatCa (%d)" % s.count(CU)
    MOI = NL.join([
        "//\t[UITOADO 09/09 E] Ap lai vi tri da luu cho cac o ve tay.",
        "static void ApChoORieng()",
        "{",
        "\tfor (int i = 0; i < s_nSoORieng; i++)",
        "\t{",
        "\t\tint n = TimKhoa(s_ORieng[i].szKhoa);",
        "\t\tif (n >= 0)",
        "\t\t\ts_ORieng[i].pfnDat(s_Bang[n].nLeft, s_Bang[n].nTop);",
        "\t}",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # goi ApChoORieng trong ApChoTatCa
    CU2 = CU + NL + "{"
    assert s.count(CU2) == 1
    s = s.replace(CU2, CU2 + NL + "\tApChoORieng();\t// [UITOADO 09/09 E] ca cac o ve tay")

    # --- bat cham: o rieng xet TRUOC cua so KWnd ---
    CU = NL.join([
        "\t\t\t//\t[UITOADO 09/09 B] bam trai + cong cu \"Doi khoi\" = y nhu bam phai",
    ])
    assert s.count(CU) == 1
    MOI = NL.join([
        "\t\t\t//\t[UITOADO 09/09 E] O VE TAY xet TRUOC: chung nam tren cung man hinh nen",
        "\t\t\t//\tphai duoc uu tien, khong thi cua so phia duoi cuop mat cu keo.",
        "\t\t\ts_nKeoORieng = -1;",
        "\t\t\t{",
        "\t\t\t\tint k;",
        "\t\t\t\tfor (k = 0; k < s_nSoORieng; k++)",
        "\t\t\t\t{",
        "\t\t\t\t\tif (s_ORieng[k].pfnTrung(x, y))",
        "\t\t\t\t\t{",
        "\t\t\t\t\t\ts_nKeoORieng = k;",
        "\t\t\t\t\t\ts_nKeoX = x;",
        "\t\t\t\t\t\ts_nKeoY = y;",
        "\t\t\t\t\t\tbreak;",
        "\t\t\t\t\t}",
        "\t\t\t\t}",
        "\t\t\t}",
        "\t\t\tif (s_nKeoORieng >= 0)",
        "\t\t\t\tbreak;",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- keo ---
    CU = NL.join([
        "\tcase WM_MOUSEMOVE:",
        "\t\tif (s_pKeo)",
        "\t\t{",
    ])
    assert s.count(CU) == 1
    MOI = NL.join([
        "\tcase WM_MOUSEMOVE:",
        "\t\t//\t[UITOADO 09/09 E] dang keo mot o ve tay",
        "\t\tif (s_nKeoORieng >= 0)",
        "\t\t{",
        "\t\t\tint nX = 0, nY = 0;",
        "",
        "\t\t\ts_ORieng[s_nKeoORieng].pfnLay(&nX, &nY);",
        "\t\t\ts_ORieng[s_nKeoORieng].pfnDat(nX + (x - s_nKeoX), nY + (y - s_nKeoY));",
        "\t\t\ts_nKeoX = x;",
        "\t\t\ts_nKeoY = y;",
        "\t\t\tbreak;",
        "\t\t}",
        "\t\tif (s_pKeo)",
        "\t\t{",
    ])
    s = s.replace(CU, MOI)

    # --- nha: ghi lai vi tri ---
    CU = NL.join([
        "\tcase WM_LBUTTONUP:",
        "\tcase WM_RBUTTONUP:",
        "\t\tif (s_pKeo && s_szKhoaKeo[0])",
        "\t\t\tGhiLaiO(s_pKeo, s_szKhoaKeo);",
        "\t\ts_pKeo = NULL;",
        "\t\tbreak;",
    ])
    assert s.count(CU) == 1, "khong tim thay nhanh nha chuot (%d)" % s.count(CU)
    MOI = NL.join([
        "\tcase WM_LBUTTONUP:",
        "\tcase WM_RBUTTONUP:",
        "\t\t//\t[UITOADO 09/09 E] o ve tay: ghi vi tri moi vao bang de luc luu co trong tep",
        "\t\tif (s_nKeoORieng >= 0)",
        "\t\t{",
        "\t\t\tint nX = 0, nY = 0;",
        "\t\t\tchar szB[192];",
        "",
        "\t\t\ts_ORieng[s_nKeoORieng].pfnLay(&nX, &nY);",
        "\t\t\tDatKhoa(s_ORieng[s_nKeoORieng].szKhoa, nX, nY, 1000, 0);",
        "\t\t\t_snprintf(szB, sizeof(szB), \"%s  =  %d,%d\",",
        "\t\t\t\ts_ORieng[s_nKeoORieng].szKhoa, nX, nY);",
        "\t\t\tszB[sizeof(szB) - 1] = 0;",
        "\t\t\tDatThongBao(szB);",
        "\t\t\ts_nKeoORieng = -1;",
        "\t\t\tbreak;",
        "\t\t}",
        "\t\tif (s_pKeo && s_szKhoaKeo[0])",
        "\t\t\tGhiLaiO(s_pKeo, s_szKhoaKeo);",
        "\t\ts_pKeo = NULL;",
        "\t\tbreak;",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 3. dang ky cum nut ky nang
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "int JxKyNang_TrungNut(int x, int y)"
    assert s.count(CU) == 1
    MOI = NL.join([
        "//---------------------------------------------------------------------------",
        "//\t[UITOADO 09/09 E] Cho cum nut ky nang DOI CHO duoc bang che do sua giao dien.",
        "//\tChu: \"cac nut ky nang moi them khong co trong phan di chuyen toa do\".",
        "//\tVi tri cum = s_nKNX / s_nKNY (goc trai tren cua ca cum, tinh theo do doi).",
        "//---------------------------------------------------------------------------",
        "static bool KyNang_ORiengTrung(int x, int y)",
        "{",
        "\treturn JxKyNang_TrungNut(x, y) > 0;",
        "}",
        "",
        "static void KyNang_ORiengLay(int* px, int* py)",
        "{",
        "\tKyNang_TamNut(0, px, py);\t// lay tam nut danh chinh lam moc cua ca cum",
        "}",
        "",
        "static void KyNang_ORiengDat(int x, int y)",
        "{",
        "\tint nX = 0, nY = 0;",
        "",
        "\tKyNang_TamNut(0, &nX, &nY);",
        "\t// s_nKNX/Y la do DOI so voi cho neo goc phai duoi, nen cong them phan chenh.",
        "\tif (s_nKNX < 0) s_nKNX = 0;",
        "\tif (s_nKNY < 0) s_nKNY = 0;",
        "\ts_nKNX += (x - nX);",
        "\ts_nKNY += (y - nY);",
        "}",
        "",
        "static void KyNang_DangKySuaToaDo()",
        "{",
        "\tstatic bool s_bDaDangKy = false;",
        "",
        "\tif (s_bDaDangKy)",
        "\t\treturn;",
        "\ts_bDaDangKy = true;",
        "\tUiToaDo_DangKyORieng(\"CumKyNang\", KyNang_ORiengTrung,",
        "\t\tKyNang_ORiengLay, KyNang_ORiengDat);",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # goi dang ky trong JxKyNang_Ve (chay moi khung, ham tu chan lai)
    CU = NL.join([
        "\tDocCaiDat();",
        "\tif (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)",
        "\t\treturn;",
    ])
    assert s.count(CU) == 1, "khong tim thay dau JxKyNang_Ve (%d)" % s.count(CU)
    MOI = NL.join([
        "\tDocCaiDat();",
        "\tif (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)",
        "\t\treturn;",
        "\tKyNang_DangKySuaToaDo();\t// [UITOADO 09/09 E] cho doi cho duoc trong che do sua",
    ])
    s = s.replace(CU, MOI)

    # include
    CU = '#include "JxCanDieuKhien.h"'
    assert s.count(CU) == 1
    s = s.replace(CU, CU + NL + '#include "../Ui/Elem/UiToaDo.h"\t// [UITOADO 09/09 E] dang ky o ve tay')
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
