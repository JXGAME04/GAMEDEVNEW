# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG D] Ba cho chinh sau khi do tan mat tren may ao.
#
# 1. BIEU TUONG KY NANG LECH LEN GOC TRAI TREN cua khung tron.
#    Do KSkill::DrawSkillIcon (KSkills.cpp:2861) BO QUA Width/Height - no ve anh
#    .spr o co THAT, lay (x, y) lam goc TRAI TREN. Ban truoc truyen x = tam - nIcon/2
#    voi nIcon tu tinh, nen anh 32 diem anh bi day lech len goc.
#    Sua: canh giua theo co THAT cua bieu tuong (KyNangCoIcon, mac dinh 32).
#
# 2. CUM NUT DE LEN "BAO VAT" VA COT ICON PHAI.
#    Ban tham khao thay HAN thanh cong cu duoi day nen dat cum sat goc phai duoi
#    duoc; ban nay CON GIU thanh cong cu + qua cau Bao Vat + cot icon phai cua ban
#    PC, nen phai day cum len va sang phai. Mac dinh moi: doi phai 60, nang 80.
#    Van neo goc phai duoi va giu nguyen hinh cung cua ban tham khao.
#
# 3. NUT "SUA GIAO DIEN" NAM DUNG VUNG NGON CAI PHAI - da bi cham nham that trong
#    luc do (che do sua bat len giua tran danh). Doi len duoi tieu ban do.

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"

# ================================================================ 1 + 2
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 KYNANG D]"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- bien moi + doi mac dinh ---
    CU = "static int\t\t\ts_nKNLenTren = 60;\t// nang ca cum len, khoi thanh cong cu duoi day"
    assert s.count(CU) == 1
    MOI = NL.join([
        "// [ANDROID 09/09 KYNANG D] Ban tham khao thay HAN thanh cong cu duoi day nen dat",
        "// cum sat goc phai duoi duoc. Ban nay CON GIU thanh cong cu + qua cau Bao Vat +",
        "// cot icon phai cua ban PC, nen phai day cum len va sang phai cho khoi de len.",
        "static int\t\t\ts_nKNLenTren = 80;\t// nang ca cum len bao nhieu diem anh",
        "static int\t\t\ts_nKNSangPhai = 60;\t// doi ca cum sang phai bao nhieu diem anh",
        "static int\t\t\ts_nKNCoIcon = 32;\t// co THAT cua bieu tuong ky nang (DrawSkillIcon bo qua Width/Height)",
    ])
    s = s.replace(CU, MOI)

    CU = '\ts_nKNLenTren = GetPrivateProfileInt("Cham", "KyNangLenTren", 60, szCfg);'
    assert s.count(CU) == 1
    MOI = NL.join([
        '\ts_nKNLenTren = GetPrivateProfileInt("Cham", "KyNangLenTren", 80, szCfg);',
        '\ts_nKNSangPhai = GetPrivateProfileInt("Cham", "KyNangSangPhai", 60, szCfg);',
        '\ts_nKNCoIcon  = GetPrivateProfileInt("Cham", "KyNangCoIcon", 32, szCfg);',
        "\tif (s_nKNCoIcon < 8) s_nKNCoIcon = 8;",
    ])
    s = s.replace(CU, MOI)

    # --- vi tri: cong them do doi sang phai ---
    CU = "\t*px = SCREEN_WIDTH  - (nR + nDX) + (s_nKNX >= 0 ? s_nKNX : 0);"
    assert s.count(CU) == 1
    s = s.replace(CU, "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);")

    # --- canh giua bieu tuong theo co THAT ---
    CU = NL.join([
        "\t\t// bieu tuong ky nang nam GON trong khung tron: lui vao ~30% ban kinh",
        "\t\tif (bCo)",
        "\t\t{",
        "\t\t\tnIcon = nR * 7 / 5;\t\t// = 1.4 * ban kinh = 70% duong kinh",
        "\t\t\tg_pCoreShell->DrawGameObj(o.uGenre, o.uId,",
        "\t\t\t\tnX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);",
        "\t\t}",
    ])
    assert s.count(CU) == 1, "khong tim thay khoi ve bieu tuong (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t// [ANDROID 09/09 KYNANG D] KSkill::DrawSkillIcon (KSkills.cpp:2861) BO QUA",
        "\t\t// Width/Height - no ve anh o co THAT, lay (x, y) lam goc TRAI TREN. Nen phai",
        "\t\t// tu canh giua theo co that (KyNangCoIcon), khong the nho ham do co lai.",
        "\t\tif (bCo)",
        "\t\t{",
        "\t\t\tnIcon = s_nKNCoIcon;",
        "\t\t\tg_pCoreShell->DrawGameObj(o.uGenre, o.uId,",
        "\t\t\t\tnX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);",
        "\t\t}",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 3
P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
DAU = "[UITOADO 09/09 D]"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t*px = (s_nMoX >= 0) ? s_nMoX",
        "\t\t: (SCREEN_WIDTH > 0 ? SCREEN_WIDTH : 1024) - UITOADO_MO_RONG - 6;",
        "\t*py = (s_nMoY >= 0) ? s_nMoY",
        "\t\t: (SCREEN_HEIGHT > 0 ? SCREEN_HEIGHT : 768) / 2;",
    ])
    assert s.count(CU) == 1, "khong tim thay vi tri nut mo (%d)" % s.count(CU)
    MOI = NL.join([
        "\t//\t[UITOADO 09/09 D] Mac dinh CU la mep phai GIUA man hinh - dung ngay vung",
        "\t//\tngon cai phai, va tu 09/09 cum nut ky nang cung nam do. Do tren may ao da",
        "\t//\tcham nham that: che do sua bat len giua tran danh. Doi xuong ngay duoi tieu",
        "\t//\tban do (goc phai tren), cho do khong co nut nao khac.",
        "\t*px = (s_nMoX >= 0) ? s_nMoX",
        "\t\t: (SCREEN_WIDTH > 0 ? SCREEN_WIDTH : 1024) - UITOADO_MO_RONG - 6;",
        "\t*py = (s_nMoY >= 0) ? s_nMoY : 172;",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
