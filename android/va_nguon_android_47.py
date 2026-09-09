# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 HUONGDI] Mui ten nho duoi chan khi di chuyen + vong tam danh khi ngam.
#
# Chu:
#  - "nguoi choi khi di chuyen duoi chan se co mui ten nho theo huong di chuyen"
#       -> ban tham khao co role_dir = CCSprite::create("ui/dir_role_new_1.png")
#          (KgameWorldVN.cpp:826), va chinh chu da chi ro \spr\npcres\direction_arrow.spr
#          la "huong di chuyen cua player". Ve no duoi chan nhan vat khi dang cam can.
#  - "co them mui ten xanh dai deu chinh theo huong"
#       -> ban tham khao con ve VONG TAM DANH khi ngam:
#          JX1M_AimVeVong(_BackAttackRadius, moveEndPoint, true) (KuiMyMenu.cpp:912).
#          Anh la \spr\attack_radius.spr cua VNKU - vong ELIP xanh nam tren mat dat
#          (elip vi goc nhin nghieng). Ve no o diem ngam, co theo TAM DANH cua ky nang.
#
# Ca hai deu ve o toa do MAN HINH (nhan vat luon o giua khung ve).

import io
import os
import shutil

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 HUONGDI]"

# ---- chep anh vong tam danh ----
SRC = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\spr\Spr\attack_radius.spr"
DST = r"D:\jx1_android_data\spr\attack_radius.spr"
if os.path.isfile(SRC):
    shutil.copyfile(SRC, DST)
    print("chep attack_radius.spr", os.path.getsize(SRC), "byte")

# ================================================================ cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- bien ---
    CU = "static char\t\ts_szKNAnhTen[128]   = \"\\\\spr\\\\npcres\\\\attack_direction.spr\";"
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        "//\t[ANDROID 09/09 HUONGDI] vong ELIP xanh chi TAM DANH, ve tren mat dat o diem ngam",
        "//\t(ban tham khao: JX1M_AimVeVong, KuiMyMenu.cpp:912).",
        "static char\t\ts_szKNAnhTam[128]   = \"\\\\spr\\\\attack_radius.spr\";",
        "//\t[ANDROID 09/09 HUONGDI] mui ten nho duoi chan nhan vat khi dang di chuyen.",
        "//\tChinh chu chi ro day la anh \"huong di chuyen cua player\".",
        "static char\t\ts_szHuongDiAnh[128] = \"\\\\spr\\\\npcres\\\\direction_arrow.spr\";",
        "static int\t\t\ts_nHuongDiBat = 1;",
        "static int\t\t\ts_nHuongDiCoAnh = -1;",
        "static int\t\t\ts_nHuongDiThap = 30;\t// ve thap hon giua man hinh bao nhieu (duoi chan)",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nKNNhip = GetPrivateProfileInt("Cham", "KyNangNhip", 200, szCfg);'
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        '\ts_nHuongDiBat  = GetPrivateProfileInt("Cham", "HuongDi", 1, szCfg);',
        '\ts_nHuongDiThap = GetPrivateProfileInt("Cham", "HuongDiThap", 30, szCfg);',
        '\tGetPrivateProfileString("Cham", "HuongDiAnh", s_szHuongDiAnh, s_szHuongDiAnh,',
        "\t\tsizeof(s_szHuongDiAnh), szCfg);",
        '\tGetPrivateProfileString("Cham", "KyNangAnhTam", s_szKNAnhTam, s_szKNAnhTam,',
        "\t\tsizeof(s_szKNAnhTam), szCfg);",
    ])
    s = s.replace(CU, MOI)

    # --- ve vong tam danh o diem ngam ---
    CU = NL.join([
        "\t\t// [ANDROID 09/09 KYNANG H] MUI TEN DINH HUONG DANH tai vi tri dang ngam.",
    ])
    assert s.count(CU) == 1
    MOI = NL.join([
        "\t\t// [ANDROID 09/09 HUONGDI] VONG TAM DANH tren mat dat o diem ngam - ban tham",
        "\t\t// khao ve bang JX1M_AimVeVong(_BackAttackRadius, moveEndPoint). Anh la vong ELIP",
        "\t\t// (det theo truc Y) vi goc nhin nghieng, nen ve rong gap doi chieu cao.",
        "\t\tif (s_nKNCoNgam && CoAnh(s_szKNAnhTam))",
        "\t\t{",
        "\t\t\tKUiGameObject oT;",
        "\t\t\tint nTamVe = 120;",
        "",
        "\t\t\tif (KyNang_CuaNut(s_nKNDangCam, &oT))",
        "\t\t\t{",
        "\t\t\t\tKJxKyNangHoi oH;",
        "\t\t\t\tmemset(&oH, 0, sizeof(oH));",
        "\t\t\t\toH.nSkillId = (int)oT.uId;",
        "\t\t\t\tif (g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oH, 0)",
        "\t\t\t\t\t&& oH.nTamDanh > 0)",
        "\t\t\t\t\tnTamVe = oH.nTamDanh;",
        "\t\t\t}",
        "\t\t\tif (nTamVe < 48)  nTamVe = 48;",
        "\t\t\tif (nTamVe > 400) nTamVe = 400;",
        "\t\t\tVeAnhKhung(s_szKNAnhTam, s_nKNNgamX, s_nKNNgamY, nTamVe, nTamVe / 2, 0);",
        "\t\t}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- ham ve theo khung chu nhat (rong x cao) ---
    CU = "//\t[ANDROID 09/09 KYNANG H] nhu VeAnh nhung chon duoc KHUNG (anh nhieu huong)."
    assert s.count(CU) == 1
    MOI = NL.join([
        "//\t[ANDROID 09/09 HUONGDI] Ve anh vao mot khung chu nhat (tam nX,nY; rong nR, cao nC).",
        "//\tDung cho vong elip tam danh - rong gap doi cao vi goc nhin nghieng.",
        "static void VeAnhKhung(const char* pszAnh, int nX, int nY, int nR, int nC, int nKhung)",
        "{",
        "\tKRUImage a;",
        "",
        "\tif (g_pRepresentShell == NULL || nR < 2 || nC < 2)",
        "\t\treturn;",
        "\tmemset(&a, 0, sizeof(a));",
        "\ta.nType = ISI_T_SPR;",
        "\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
        "\ta.Color.Color_dw = 0xffffffff;",
        "\ta.nISPosition = IMAGE_IS_POSITION_INIT;",
        "\ta.nFrame = nKhung;",
        "\tstrncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);",
        "\ta.oPosition.nX = nX - nR / 2;",
        "\ta.oPosition.nY = nY - nC / 2;",
        "\ta.oPosition.nZ = 0;",
        "\ta.oEndPos.nX = a.oPosition.nX + nR;",
        "\ta.oEndPos.nY = a.oPosition.nY + nC;",
        "\ta.oEndPos.nZ = 0;",
        "\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- mui ten huong di duoi chan ---
    CU = "void JxKyNang_Ve()\r\n{"
    assert s.count(CU) == 1
    MOI = NL.join([
        "//---------------------------------------------------------------------------",
        "// [ANDROID 09/09 HUONGDI] MUI TEN NHO DUOI CHAN THEO HUONG DI CHUYEN",
        "//",
        "// Chu: \"nguoi choi khi di chuyen duoi chan se co mui ten nho theo huong di chuyen\".",
        "// Ban tham khao co role_dir rieng cho viec nay (KgameWorldVN.cpp:826).",
        "// Nhan vat luon o giua khung ve nen ve ngay duoi giua man hinh.",
        "//---------------------------------------------------------------------------",
        "void JxHuongDi_Ve()",
        "{",
        "\tint nSo, nKhung;",
        "",
        "\tDocCaiDat();",
        "\tif (!s_nHuongDiBat || !s_bCam || g_pRepresentShell == NULL)",
        "\t\treturn;\t\t// chi hien khi dang cam can dieu khien (tuc dang di)",
        "\tif (s_nHuongDiCoAnh < 0)",
        "\t{",
        "\t\ts_nHuongDiCoAnh = CoAnh(s_szHuongDiAnh) ? 1 : 0;",
        "\t\tg_DebugLog(\"[HUONGDI] anh mui ten: %s -> co anh=%d\",",
        "\t\t\ts_szHuongDiAnh, s_nHuongDiCoAnh);",
        "\t}",
        "\tif (!s_nHuongDiCoAnh)",
        "\t\treturn;",
        "\t// s_nHuong la 0..63 theo loi cua game (0 = xuong, 16 = trai, 32 = len, 48 = phai).",
        "\tnSo = KyNang_SoKhung(s_szHuongDiAnh);",
        "\tnKhung = (nSo > 1) ? (s_nHuong * nSo / 64) % nSo : 0;",
        "\tVeAnh2(s_szHuongDiAnh, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + s_nHuongDiThap, nKhung);",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ header
P = "Sources/S3Client/Platform/JxCanDieuKhien.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "void JxKyNang_Ve();"
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        "// [ANDROID 09/09 HUONGDI] Goi cuoi moi khung ve: mui ten nho duoi chan nhan vat",
        "// theo huong dang di chuyen.",
        "void JxHuongDi_Ve();",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ UiShell
P = "Sources/S3Client/Ui/UiShell.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "\tJxKyNang_Ve();\t// [ANDROID 09/09 KYNANG] bang nut ky nang + vach ngam + vong duoi chan dich"
    assert s.count(CU) == 1
    MOI = NL.join([
        "\tJxHuongDi_Ve();\t// [ANDROID 09/09 HUONGDI] mui ten nho duoi chan theo huong di chuyen",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
