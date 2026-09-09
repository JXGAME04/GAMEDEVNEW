# -*- coding: utf-8 -*-
#
# [UITOADO 10/09 F] TUNG NUT ky nang doi cho RIENG duoc trong che do sua giao dien.
#
# Chu: "nut ky nang phai dieu chinh tung nut duoc". Ban truoc chi dang ky CA CUM lam mot o
# ("CumKyNang") nen keo nut nao ca cum cung di theo.
#
# Cach lam - dung luon hai cong cu san co cua thanh nut che do sua:
#   - "Doi o"   : keo mot nut -> CHI nut do dich. Moi nut co do doi rieng s_nKNDoiX/Y[i],
#                 luu vao UserData\UiToaDo.ini voi khoa KyNang0 (nut chinh), KyNang1..8
#                 (o phu 0..7), KyNangGan (nut doi che do gan).
#   - "Doi khoi": keo bat ky nut nao -> CA CUM dich (s_nKNX/Y), luu khoa CumKyNang.
# Nut nao chua keo rieng thi van nam theo hinh cung cua ban tham khao. Do doi rieng tinh
# SO VOI cum, nen doi cum sau do thi cac nut van giu cach sap xep da chinh.
#
# De ham dat/lay/do trung biet no dang noi ve nut nao, bang o ve tay cua UiToaDo them mot
# tham so ngu canh (void*) cho ca ba ham - nguoi dang ky duy nhat hien nay la cum nut ky
# nang nen doi chu ky khong dung ai khac. Them UiToaDo_CongCuKhoi() de ben ngoai biet dang
# dung cong cu "Doi khoi".
#
# Trong che do sua, o phu TRONG cung duoc ve (chi khung) de con thay ma keo.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[UITOADO 10/09 F]"
DASH = "//---------------------------------------------------------------------------"

# ================================================================ 1. UiToaDo.h
P = "Sources/S3Client/Ui/Elem/UiToaDo.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    for cu, moi in [
        ("typedef bool (*PFN_UITOADO_TRUNG)(int x, int y);\t\t\t// (x,y) co trung o nay khong",
         "typedef bool (*PFN_UITOADO_TRUNG)(void* pNgu, int x, int y);\t\t// (x,y) co trung o nay khong"),
        ("typedef void (*PFN_UITOADO_LAYVITRI)(int* px, int* py);\t// vi tri hien tai",
         "typedef void (*PFN_UITOADO_LAYVITRI)(void* pNgu, int* px, int* py);\t// vi tri hien tai"),
        ("typedef void (*PFN_UITOADO_DATVITRI)(int x, int y);\t\t// dat vi tri moi",
         "typedef void (*PFN_UITOADO_DATVITRI)(void* pNgu, int x, int y);\t\t// dat vi tri moi"),
        (NL.join([
            "void\tUiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,",
            "\t\t\tPFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat);",
         ]),
         NL.join([
            "//\t[UITOADO 10/09 F] pNgu = ngu canh tuy y, duoc dua nguyen ve ca ba ham (vi du: so thu",
            "//\ttu cua nut, de mot bo ham dung chung cho nhieu nut).",
            "void\tUiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,",
            "\t\t\tPFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat, void* pNgu);",
            "",
            "//\t[UITOADO 10/09 F] Dang sua va cong cu dang chon la \"Doi khoi\" (keo = ca khoi).",
            "bool\tUiToaDo_CongCuKhoi();",
         ])),
    ]:
        assert s.count(cu) == 1, "UiToaDo.h: khong tim thay %.60s (%d)" % (cu, s.count(cu))
        s = s.replace(cu, moi)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 2. UiToaDo.cpp
P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    for cu, moi, n in [
        ("#define\tUITOADO_ORIENG_MAX\t8", "#define\tUITOADO_ORIENG_MAX\t16\t// [UITOADO 10/09 F] cum ky nang + 10 nut rieng", 1),
        ("\tPFN_UITOADO_DATVITRI\tpfnDat;",
         "\tPFN_UITOADO_DATVITRI\tpfnDat;" + NL + "\tvoid*\t\t\t\t\tpNgu;\t// [UITOADO 10/09 F] ngu canh dua ve ca ba ham", 1),
        (NL.join([
            "void UiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,",
            "\t\tPFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat)",
         ]),
         NL.join([
            "void UiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,",
            "\t\tPFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat, void* pNgu)",
         ]), 1),
        ("\ts_ORieng[i].pfnDat   = pfnDat;",
         "\ts_ORieng[i].pfnDat   = pfnDat;" + NL + "\ts_ORieng[i].pNgu     = pNgu;", 1),
        ("\t\t\tpfnDat(s_Bang[n].nLeft, s_Bang[n].nTop);",
         "\t\t\tpfnDat(pNgu, s_Bang[n].nLeft, s_Bang[n].nTop);", 1),
        ("\t\t\ts_ORieng[i].pfnDat(s_Bang[n].nLeft, s_Bang[n].nTop);",
         "\t\t\ts_ORieng[i].pfnDat(s_ORieng[i].pNgu, s_Bang[n].nLeft, s_Bang[n].nTop);", 1),
        ("\t\t\t\t\tif (s_ORieng[k].pfnTrung(x, y))",
         "\t\t\t\t\tif (s_ORieng[k].pfnTrung(s_ORieng[k].pNgu, x, y))", 1),
        ("\t\t\ts_ORieng[s_nKeoORieng].pfnLay(&nX, &nY);",
         "\t\t\ts_ORieng[s_nKeoORieng].pfnLay(s_ORieng[s_nKeoORieng].pNgu, &nX, &nY);", 2),
        ("\t\t\ts_ORieng[s_nKeoORieng].pfnDat(nX + (x - s_nKeoX), nY + (y - s_nKeoY));",
         "\t\t\ts_ORieng[s_nKeoORieng].pfnDat(s_ORieng[s_nKeoORieng].pNgu,"
         + NL + "\t\t\t\tnX + (x - s_nKeoX), nY + (y - s_nKeoY));", 1),
        (NL.join(["bool UiToaDo_DangSua()", "{", "\treturn s_bDangSua;", "}"]),
         NL.join([
            "bool UiToaDo_DangSua()", "{", "\treturn s_bDangSua;", "}", "",
            "//\t[UITOADO 10/09 F] dang sua VA cong cu dang chon la \"Doi khoi\"",
            "bool UiToaDo_CongCuKhoi()", "{", "\treturn s_bDangSua && s_nCongCu == CONGCU_KHOI;", "}",
         ]), 1),
    ]:
        assert s.count(cu) == n, "UiToaDo.cpp: %.60s -> thay %d, can %d" % (cu, s.count(cu), n)
        s = s.replace(cu, moi)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 3. JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- bang do doi rieng ---
    CU = NL.join(["static int\t\t\ts_nKNX   = 0;", "static int\t\t\ts_nKNY   = 0;"])
    assert s.count(CU) == 1, "khong tim thay s_nKNX/Y (%d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "// [UITOADO 10/09 F] Do doi RIENG cua tung nut (0 = nut chinh, 1..8 = o phu 0..7,",
        "// 9 = nut doi che do gan), cong them vao vi tri theo cung. Keo bang cong cu \"Doi o\".",
        "static int\t\t\ts_nKNDoiX[KYNANG_SO_PHU + 2];",
        "static int\t\t\ts_nKNDoiY[KYNANG_SO_PHU + 2];",
    ])
    s = s.replace(CU, MOI)

    # --- ap do doi rieng vao vi tri ---
    for cu, moi in [
        ("\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + s_nKNX;",
         "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + s_nKNX + s_nKNDoiX[nNut < 0 ? 0 : nNut];"),
        ("\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + s_nKNY;",
         "\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + s_nKNY + s_nKNDoiY[nNut < 0 ? 0 : nNut];"),
        ("\t\t+ s_nKNSangPhai + s_nKNX;",
         "\t\t+ s_nKNSangPhai + s_nKNX + s_nKNDoiX[KYNANG_SO_PHU + 1];"),
        ("\t\t- s_nKNLenTren + s_nKNY;",
         "\t\t- s_nKNLenTren + s_nKNY + s_nKNDoiY[KYNANG_SO_PHU + 1];"),
        ("\t\tif (!bCo && i > 0)" + NL + "\t\t\tcontinue;\t\t// o phu trong thi khong ve gi ca",
         "\t\tif (!bCo && i > 0 && !UiToaDo_DangSua())" + NL
         + "\t\t\tcontinue;\t\t// o phu trong thi khong ve; tru luc dang sua (de con keo duoc)"),
    ]:
        assert s.count(cu) == 1, "JxCan: khong tim thay %.60s (%d)" % (cu, s.count(cu))
        s = s.replace(cu, moi)

    # --- thay ca khoi dang ky o rieng ---
    i0 = s.index(DASH + NL + "//\t[UITOADO 09/09 E] Cho cum nut ky nang DOI CHO duoc bang che do sua giao dien.")
    i1 = s.index("int JxKyNang_TrungNut(int x, int y)")
    KHOI = NL.join([
        DASH,
        "//\t[UITOADO 10/09 F] TUNG NUT ky nang doi cho RIENG duoc trong che do sua giao dien.",
        "//\tChu: \"nut ky nang phai dieu chinh tung nut duoc\".",
        "//\t  - cong cu \"Doi o\"   : keo mot nut -> CHI nut do dich (do doi rieng s_nKNDoiX/Y[i]),",
        "//\t                        luu khoa KyNang0 (nut chinh), KyNang1..8 (o phu 0..7), KyNangGan.",
        "//\t  - cong cu \"Doi khoi\": keo bat ky nut nao -> CA CUM dich (s_nKNX/Y), luu khoa CumKyNang.",
        "//\tNut chua keo rieng bao gio thi van nam theo hinh cung cua ban tham khao.",
        DASH,
        "#define\tKYNANG_ORIENG_GAN\t(KYNANG_SO_PHU + 1)\t// chi so cua nut doi che do trong bang do doi",
        "",
        "static void KyNang_TamORieng(int i, int* px, int* py, int* pR)",
        "{",
        "\tif (i == KYNANG_ORIENG_GAN)",
        "\t{",
        "\t\tKyNang_TamNutGan(px, py);",
        "\t\t*pR = KYNANG_GAN_CO / 2;",
        "\t}",
        "\telse",
        "\t{",
        "\t\tKyNang_TamNut(i, px, py);",
        "\t\t*pR = KyNang_CoNut(i) / 2;",
        "\t}",
        "}",
        "",
        "static bool KyNang_ORiengTrungMot(void* pNgu, int x, int y)",
        "{",
        "\tint i = (int)(intptr_t)pNgu;",
        "\tint nX, nY, nR;",
        "",
        "\tif (UiToaDo_CongCuKhoi())",
        "\t\treturn false;\t\t\t// dang \"Doi khoi\" thi de muc CumKyNang bat",
        "\tKyNang_TamORieng(i, &nX, &nY, &nR);",
        "\treturn (x - nX) * (x - nX) + (y - nY) * (y - nY) <= nR * nR;",
        "}",
        "",
        "static void KyNang_ORiengLayMot(void* pNgu, int* px, int* py)",
        "{",
        "\tint nR;",
        "",
        "\tKyNang_TamORieng((int)(intptr_t)pNgu, px, py, &nR);",
        "}",
        "",
        "static void KyNang_ORiengDatMot(void* pNgu, int x, int y)",
        "{",
        "\tint i = (int)(intptr_t)pNgu;",
        "\tint nX = 0, nY = 0, nR;",
        "",
        "\tKyNang_TamORieng(i, &nX, &nY, &nR);",
        "\ts_nKNDoiX[i] += (x - nX);",
        "\ts_nKNDoiY[i] += (y - nY);",
        "}",
        "",
        "//\tCa cum - chi bat khi dang dung cong cu \"Doi khoi\".",
        "static bool KyNang_ORiengTrungCum(void* pNgu, int x, int y)",
        "{",
        "\tint i;",
        "",
        "\tif (!UiToaDo_CongCuKhoi())",
        "\t\treturn false;",
        "\tfor (i = 0; i <= KYNANG_ORIENG_GAN; i++)",
        "\t{",
        "\t\tint nX, nY, nR;",
        "",
        "\t\tKyNang_TamORieng(i, &nX, &nY, &nR);",
        "\t\tif ((x - nX) * (x - nX) + (y - nY) * (y - nY) <= nR * nR)",
        "\t\t\treturn true;",
        "\t}",
        "\treturn false;",
        "}",
        "",
        "static void KyNang_ORiengLayCum(void* pNgu, int* px, int* py)",
        "{",
        "\tKyNang_TamNut(0, px, py);\t// lay tam nut danh chinh lam moc cua ca cum",
        "}",
        "",
        "static void KyNang_ORiengDatCum(void* pNgu, int x, int y)",
        "{",
        "\tint nX = 0, nY = 0;",
        "",
        "\tKyNang_TamNut(0, &nX, &nY);",
        "\t// s_nKNX/Y la do DOI cua ca cum so voi cho neo goc phai duoi; am duong deu duoc.",
        "\ts_nKNX += (x - nX);",
        "\ts_nKNY += (y - nY);",
        "}",
        "",
        "static void KyNang_DangKySuaToaDo()",
        "{",
        "\tstatic bool s_bDaDangKy = false;",
        "\tstatic const char* s_szKhoa[KYNANG_SO_PHU + 2] =",
        "\t{",
        "\t\t\"KyNang0\", \"KyNang1\", \"KyNang2\", \"KyNang3\", \"KyNang4\",",
        "\t\t\"KyNang5\", \"KyNang6\", \"KyNang7\", \"KyNang8\", \"KyNangGan\"",
        "\t};",
        "\tint i;",
        "",
        "\tif (s_bDaDangKy)",
        "\t\treturn;",
        "\ts_bDaDangKy = true;",
        "\t// Ca cum dang ky TRUOC: vi tri da luu cua cum ap truoc, roi moi den do doi rieng tung",
        "\t// nut (do doi rieng tinh so voi cum) - thu tu nay phai giu.",
        "\tUiToaDo_DangKyORieng(\"CumKyNang\", KyNang_ORiengTrungCum,",
        "\t\tKyNang_ORiengLayCum, KyNang_ORiengDatCum, NULL);",
        "\tfor (i = 0; i < KYNANG_SO_PHU + 2; i++)",
        "\t\tUiToaDo_DangKyORieng(s_szKhoa[i], KyNang_ORiengTrungMot,",
        "\t\t\tKyNang_ORiengLayMot, KyNang_ORiengDatMot, (void*)(intptr_t)i);",
        "}",
        "",
    ])
    s = s[:i0] + KHOI + s[i1:]

    CU = "#include <math.h>"
    assert s.count(CU) >= 1
    if "#include <stdint.h>" not in s:
        s = s.replace(CU, CU + NL + "#include <stdint.h>\t// [UITOADO 10/09 F] intptr_t", 1)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
