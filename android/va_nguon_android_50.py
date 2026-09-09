# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 NGHIENG] Bo vach do o nut; thay bang NUM NGHIENG theo huong dang chinh.
#                          Va day mui ten huong di ra xa hon cho khoi nam tren nguoi.
#
# Chu:
#  - "may nut ky nang khi dieu chinh huong bo cai duong mau do noi nut, thay vao hieu ung
#     nghieng theo huong dieu chinh"
#       -> Dung cach cua ban tham khao: nut ky nang chinh la mot CAN (KuiMyMenu) - no co
#          jsSprite (num) chay trong ban kinh theo huong ngon tay (setSpritePiont,
#          KuiMyMenu.cpp:271 va :820). Tuc nut "nghieng" ve phia dang chi, khong co vach.
#          Dung luon anh num co san cua VNKU: \spr\Ui3\UiSkillControl\joystick_ctrl.spr
#          (cung bo voi khung nut, da dung cho can dieu khien).
#  - "van lech - di xuong thi dung, di len mui ten no nam sau lung" (da sua o ban va 49,
#     nhung day ra chua du): do duoc mui ten roi ngay tren nguoi (tam (522,301) trong khi
#     giua man hinh la (520,302)). Day xa hon: 56 diem anh.

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 NGHIENG]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # ---- 1. day mui ten huong di ra xa hon ----
    CU = "static int\t\t\ts_nHuongDiXa = 38;\t\t// day mui ten ra phia truoc bao nhieu diem anh"
    assert s.count(CU) == 1
    MOI = NL.join([
        "//\t[ANDROID 09/09 NGHIENG] do duoc: day 38 thi mui ten van roi NGAY TREN nguoi",
        "//\t(tam (522,301) trong khi giua man hinh la (520,302)). Day xa hon.",
        "static int\t\t\ts_nHuongDiXa = 56;\t\t// day mui ten ra phia truoc bao nhieu diem anh",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nHuongDiXa   = GetPrivateProfileInt("Cham", "HuongDiXa", 38, szCfg);'
    assert s.count(CU) == 1
    s = s.replace(CU, '\ts_nHuongDiXa   = GetPrivateProfileInt("Cham", "HuongDiXa", 56, szCfg);')

    # ---- 2. bo vach do, them num nghieng ----
    CU = NL.join([
        "\tif (s_nKNDangCam >= 0)",
        "\t{",
        "\t\tKRULine oVach;",
        "",
        "\t\tKyNang_TamNut(s_nKNDangCam, &nX, &nY);",
    ])
    assert s.count(CU) == 1, "khong tim thay dau khoi ve khi dang giu (%d)" % s.count(CU)
    MOI = NL.join([
        "\tif (s_nKNDangCam >= 0)",
        "\t{",
        "\t\tKyNang_TamNut(s_nKNDangCam, &nX, &nY);",
    ])
    s = s.replace(CU, MOI)

    CU = NL.join([
        "\t\toVach.oPosition.nX = nX;",
        "\t\toVach.oPosition.nY = nY;",
        "\t\toVach.oEndPos.nX   = s_nKNNgonX;",
        "\t\toVach.oEndPos.nY   = s_nKNNgonY;",
        "\t\toVach.Color.Color_dw = s_nKNDichIdx ? 0xFFFF6666 : 0xA0FFFFFF;",
        "\t\tg_pRepresentShell->DrawPrimitives(1, &oVach, RU_T_LINE, true);",
        "",
    ])
    assert s.count(CU) == 1, "khong tim thay khoi ve vach (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t// [ANDROID 09/09 NGHIENG] Khong ve vach nua. Thay bang NUM chay trong ban kinh",
        "\t\t// nut theo huong ngon tay - dung cach cua ban tham khao (KuiMyMenu jsSprite,",
        "\t\t// setSpritePiont): nut \"nghieng\" ve phia dang chi. Anh num la joystick_ctrl.spr",
        "\t\t// cua VNKU, cung bo voi khung nut.",
        "\t\t{",
        "\t\t\tint dx = s_nKNNgonX - nX;",
        "\t\t\tint dy = s_nKNNgonY - nY;",
        "\t\t\tint nDai = (int)sqrt((double)(dx * dx + dy * dy));",
        "\t\t\tint nBK = KyNang_CoNut(s_nKNDangCam) / 2;\t// num chay trong long nut",
        "\t\t\tint nNumX = nX, nNumY = nY;",
        "",
        "\t\t\tif (nDai > nBK && nDai > 0)",
        "\t\t\t{",
        "\t\t\t\tnNumX = nX + dx * nBK / nDai;",
        "\t\t\t\tnNumY = nY + dy * nBK / nDai;",
        "\t\t\t}",
        "\t\t\telse",
        "\t\t\t{",
        "\t\t\t\tnNumX = nX + dx;",
        "\t\t\t\tnNumY = nY + dy;",
        "\t\t\t}",
        "\t\t\tif (CoAnh(s_szKNAnhNum))",
        "\t\t\t\tVeAnhCo(s_szKNAnhNum, nNumX, nNumY, nBK, 0);",
        "\t\t}",
        "",
    ])
    s = s.replace(CU, MOI)

    # ---- 3. anh num ----
    CU = "static char\t\ts_szKNAnhNgam[128]  = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\effect_skill.spr\";"
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        "//\t[ANDROID 09/09 NGHIENG] num chay trong long nut theo huong ngon tay",
        "static char\t\ts_szKNAnhNum[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\joystick_ctrl.spr\";",
    ])
    s = s.replace(CU, MOI)

    CU = '\tGetPrivateProfileString("Cham", "KyNangAnhNgam", s_szKNAnhNgam, s_szKNAnhNgam,'
    assert s.count(CU) == 1
    MOI = NL.join([
        '\tGetPrivateProfileString("Cham", "KyNangAnhNum", s_szKNAnhNum, s_szKNAnhNum,',
        "\t\tsizeof(s_szKNAnhNum), szCfg);",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
