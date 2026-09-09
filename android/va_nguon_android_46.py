# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG I] De nut la DANH LIEN TUC; keo o xanh toi dau thi tha ky nang o do.
#
# Chu: "cac nut ky nang de vao la tu danh - di chuyen o xanh do cho nao thi tha ky nang
# cho do".
#
# Dung kieu 2 cua ban tham khao - "dieu huong danh LIEN TUC" (KuiMyMenu.cpp:1000
# updateMode): con giu nut thi cu goi customSkillAttack theo diem ngam hien tai, chu
# khong doi den luc nha ngon.
#
# Ban truoc chi danh MOT lan luc nha ngon (kieu 1). Nay:
#   - dat ngon xuong  -> danh ngay phat dau,
#   - con de          -> cu nhip nao den han thi danh tiep, tai DIEM NGAM hien tai,
#   - keo o xanh      -> diem ngam doi theo, phat sau danh vao cho moi,
#   - nha ngon        -> thoi.
# Nhip mac dinh 200 ms (config.ini [Cham] KyNangNhip). Ban tham khao goi moi khung ve;
# o day thua ra vi moi lan goi la mot goi tin len may chu, ma Core/may chu van chan
# theo thoi gian hoi chieu nen goi day hon cung khong danh nhanh hon.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 KYNANG I]"

# ================================================================ 1. JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- bien nhip ---
    CU = "static int\t\t\ts_nKNBanKinhKeo = 60;"
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        "//\t[ANDROID 09/09 KYNANG I] de nut = danh lien tuc; day la khoang cach giua hai phat.",
        "static int\t\t\ts_nKNNhip = 200;\t\t// mili giay",
        "static unsigned int\ts_uKNDanhLuc = 0;\t// luc danh phat gan nhat",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nKNBanKinhKeo = GetPrivateProfileInt("Cham", "KyNangBanKinhKeo", 60, szCfg);'
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        '\ts_nKNNhip = GetPrivateProfileInt("Cham", "KyNangNhip", 200, szCfg);',
        "\tif (s_nKNNhip < 60) s_nKNNhip = 60;",
    ])
    s = s.replace(CU, MOI)

    # --- tach phan danh ra thanh ham rieng, dung chung cho nhip va cho luc nha ---
    CU = NL.join([
        "bool JxKyNang_Nha()",
        "{",
        "\tint nNut = s_nKNDangCam;",
        "\tint nDich = s_nKNDichIdx;",
        "\tint bCoNgam = s_nKNCoNgam;",
        "\tint nNgamX = s_nKNNgamX, nNgamY = s_nKNNgamY;",
        "\tint nTam = 0, nAura = 0;",
        "\tKUiGameObject o;",
        "",
        "\ts_nKNDangCam = -1;",
        "\ts_nKNDichIdx = 0;",
        "\ts_nKNCoNgam = 0;",
        "\tif (nNut < 0 || g_pCoreShell == NULL)",
        "\t\treturn false;",
    ])
    assert s.count(CU) == 1, "khong tim thay dau JxKyNang_Nha (%d)" % s.count(CU)
    MOI = NL.join([
        "//\t[ANDROID 09/09 KYNANG I] Danh MOT phat bang nut dang giu. Dung chung cho nhip",
        "//\t(dang de nut) va cho luc nha ngon.",
        "static void KyNang_DanhMotPhat()",
        "{",
        "\tint nNut = s_nKNDangCam;",
        "\tint nDich = s_nKNDichIdx;",
        "\tint bCoNgam = s_nKNCoNgam;",
        "\tint nNgamX = s_nKNNgamX, nNgamY = s_nKNNgamY;",
        "\tint nTam = 0, nAura = 0;",
        "\tKUiGameObject o;",
        "",
        "\tif (nNut < 0 || g_pCoreShell == NULL)",
        "\t\treturn;",
    ])
    s = s.replace(CU, MOI)

    # phan than con lai: doi cac 'return true/false' cho khop kieu void
    CU = NL.join([
        "\tif (!KyNang_CuaNut(nNut, &o))",
        "\t\treturn false;",
        "",
        "\t// [ANDROID 09/09 KYNANG F] Khau kiem tra ngua - dung nhu he tu danh lam.",
        "\t// Neu ky nang chi dung duoc duoi ngua ma dang cuoi thi Core da gui lenh xuong ngua,",
        "\t// lan nay khong danh; nguoi choi bam lai la danh.",
        "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura))",
        "\t\treturn true;",
        "\tif (nAura)",
        "\t{",
        "\t\t// Ky nang TRO (noi cong / trang thai): khong phai danh. Chi dat lam ky nang",
        "\t\t// dang dung; bat/tat aura chua noi duoc tu day - xem muc \"con lai\" ban giao.",
        "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
        "\t\treturn true;",
        "\t}",
        "",
        "\tif (nNut > 0)",
        "\t{",
        "\t\t// [ANDROID 09/09 KYNANG F] NUT PHU: ban tham khao dat lam ky nang danh PHAI roi",
        "\t\t// danh THEO DIEM (KgameWorld.cpp:4437 SetRightSkill + UseSkill). Lam y vay - nho",
        "\t\t// the nut phu va nut chinh khong dam chan nhau: nut chinh van giu ky nang danh trai.",
        "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
        "\t\tif (bCoNgam)",
        "\t\t{",
        "\t\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// danh theo huong da ngam",
        "\t\t\treturn true;",
        "\t\t}",
        "\t}",
        "\telse if (bCoNgam)",
        "\t{",
        "\t\t// NUT CHINH co ngam: danh thuong theo huong do.",
        "\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);",
        "\t\treturn true;",
        "\t}",
        "",
        "\t// Khong ngam: cham la danh - tu chon con gan nhat (kieu 0 cua ban tham khao).",
        "\tif (nDich == 0)",
        "\t\tnDich = KyNang_TimDich(0, 0, NULL, NULL);",
        "\tif (nDich == 0)",
        "\t\treturn true;\t\t// khong co dich: van coi la da xu ly",
        "\tg_pCoreShell->LockSomeoneUseSkill(nDich, (int)o.uId);",
        "\treturn true;",
        "}",
    ])
    assert s.count(CU) == 1, "khong tim thay than JxKyNang_Nha (%d)" % s.count(CU)
    MOI = NL.join([
        "\tif (!KyNang_CuaNut(nNut, &o))",
        "\t\treturn;",
        "",
        "\t// Khau kiem tra ngua - dung nhu he tu danh lam. Neu ky nang chi dung duoc duoi",
        "\t// ngua ma dang cuoi thi Core da gui lenh xuong ngua, phat nay khong danh.",
        "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura))",
        "\t\treturn;",
        "\tif (nAura)",
        "\t{",
        "\t\t// Ky nang TRO (noi cong / trang thai): khong phai danh. Chi dat lam ky nang",
        "\t\t// dang dung; bat/tat aura chua noi duoc tu day - xem muc \"con lai\" ban giao.",
        "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
        "\t\treturn;",
        "\t}",
        "",
        "\tif (nNut > 0)",
        "\t{",
        "\t\t// NUT PHU: ban tham khao dat lam ky nang danh PHAI roi danh THEO DIEM",
        "\t\t// (KgameWorld.cpp:4437 SetRightSkill + UseSkill). Nho the nut phu va nut chinh",
        "\t\t// khong dam chan nhau: nut chinh van giu ky nang danh trai.",
        "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
        "\t\tif (bCoNgam)",
        "\t\t{",
        "\t\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// tha ky nang o cho o xanh",
        "\t\t\treturn;",
        "\t\t}",
        "\t}",
        "\telse if (bCoNgam)",
        "\t{",
        "\t\tg_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);\t// nut chinh, danh theo huong",
        "\t\treturn;",
        "\t}",
        "",
        "\t// Khong ngam: de nut la danh con gan nhat (kieu 0 cua ban tham khao).",
        "\tif (nDich == 0)",
        "\t\tnDich = KyNang_TimDich(0, 0, NULL, NULL);",
        "\tif (nDich == 0)",
        "\t\treturn;",
        "\tg_pCoreShell->LockSomeoneUseSkill(nDich, (int)o.uId);",
        "}",
        "",
        "//\t[ANDROID 09/09 KYNANG I] Goi moi vong lap game: con de nut thi cu danh tiep.",
        "void JxKyNang_Nhip()",
        "{",
        "\tunsigned int uNay;",
        "",
        "\tif (s_nKNDangCam < 0)",
        "\t\treturn;",
        "\tuNay = (unsigned int)GetTickCount();",
        "\tif (s_uKNDanhLuc && uNay - s_uKNDanhLuc < (unsigned int)s_nKNNhip)",
        "\t\treturn;",
        "\ts_uKNDanhLuc = uNay;",
        "\tKyNang_DanhMotPhat();",
        "}",
        "",
        "bool JxKyNang_Nha()",
        "{",
        "\tbool bCo = (s_nKNDangCam >= 0);",
        "",
        "\ts_nKNDangCam = -1;",
        "\ts_nKNDichIdx = 0;",
        "\ts_nKNCoNgam = 0;",
        "\ts_uKNDanhLuc = 0;",
        "\treturn bCo;",
        "}",
    ])
    s = s.replace(CU, MOI)

    # dat ngon xuong: danh ngay phat dau
    CU = NL.join([
        "void JxKyNang_BatDau(int nNut, int x, int y)",
        "{",
        "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
        "\ts_nKNNgonX = x;",
        "\ts_nKNNgonY = y;",
        "\ts_nKNDichIdx = 0;",
        "}",
    ])
    assert s.count(CU) == 1, "khong tim thay JxKyNang_BatDau (%d)" % s.count(CU)
    MOI = NL.join([
        "void JxKyNang_BatDau(int nNut, int x, int y)",
        "{",
        "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
        "\ts_nKNNgonX = x;",
        "\ts_nKNNgonY = y;",
        "\ts_nKNDichIdx = 0;",
        "\ts_nKNCoNgam = 0;",
        "\ts_uKNDanhLuc = 0;\t\t// = danh ngay phat dau o nhip ke tiep",
        "}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 2. header
P = "Sources/S3Client/Platform/JxCanDieuKhien.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "// Nha ngon = danh. Tra ve true neu da danh.\r\nbool JxKyNang_Nha();"
    assert s.count(CU) == 1, "khong tim thay khai bao JxKyNang_Nha (%d)" % s.count(CU)
    MOI = NL.join([
        "// [ANDROID 09/09 KYNANG I] Goi moi vong lap game: con de nut thi cu danh tiep,",
        "// tai cho o xanh dang dung.",
        "void JxKyNang_Nhip();",
        "// Nha ngon = thoi danh. Tra ve true neu vua nha mot nut ky nang.",
        "bool JxKyNang_Nha();",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 3. goi tu vong lap
P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\t\tJxCan_Nhip();\t// [ANDROID 09/09 CAN] dang cam can thi day nhan vat di theo huong"
    assert s.count(CU) == 1, "khong tim thay loi goi JxCan_Nhip (%d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "\t\tJxKyNang_Nhip();\t// [ANDROID 09/09 KYNANG I] dang de nut ky nang thi cu danh tiep",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
