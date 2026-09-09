# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 GANTOADO] Chu: "van chua thay doi ky nang cac o duoc" va "chua chinh toa do
# cac nut ky nang duoc". Thu lai tren may ao thi ra BA loi that, khong phai chu lam sai:
#
# 1. CHINH TOA DO - nut ky nang NUOT cu cham. KSdlApp bat cham tren nut ky nang ngay luc dat
#    ngon (CHAM_KYNANG) truoc khi he chinh toa do kip nhin thay. Dang o che do sua thi phai
#    de cu cham di xuong UiToaDo. Ca duong ngon thu nhat lan duong ngon thu hai.
#
# 2. CHINH TOA DO - do doi AM bi chan. s_nKNX/Y dung -1 lam "chua dat" va moi cho deu
#    "(s_nKNX >= 0 ? s_nKNX : 0)", nen keo cum sang TRAI hay LEN TREN (do doi am) la mat.
#    Doi thanh do doi thuong, mac dinh 0, am duong deu duoc. config.ini [Cham] KyNangX/Y
#    gio la do doi so voi goc phai duoi (0 = dung cho cua ban tham khao).
#
# 3. GAN KY NANG - chi moc vao bang NHO (KUiSkillTree, mo bang cach bam o ky nang tren thanh
#    trang thai). Cua so ky nang LON (KUiSkills / KUiSkillsNew, mo tu thanh cong cu) thi bam
#    vao o ky nang la NHAC LEN de keo (WND_N_ITEM_PICKDROP, WndObjContainer.cpp:288), khong
#    phai bam thuong -> moc dat o dau OnSkillPickDrop: dang o che do gan thi gan luon va
#    KHONG nhac len.
#
# Kem theo: dang o che do gan thi hien dong chu huong dan ngay canh cum nut, de chu biet
# buoc tiep theo phai lam gi.

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 10/09 GANTOADO]"


def vn(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


# ================================================================ 1. KSdlApp.cpp
P = "Sources/S3Client/Platform/KSdlApp.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t\t\t{",
        "\t\t\t\tint nNutKN = JxKyNang_TrungNut(m_nChamX0, m_nChamY0);",
        "\t\t\t\tif (nNutKN > 0)",
    ])
    assert s.count(CU) == 1, "khong tim thay cho bat nut ky nang ngon 1 (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t\t// [ANDROID 10/09 GANTOADO] Dang sua giao dien thi KHONG cho nut ky nang nuot",
        "\t\t\t// cu cham - phai de no di xuong UiToaDo thi moi keo cum nut di duoc.",
        "\t\t\tif (!UiToaDo_DangSua())",
        "\t\t\t{",
        "\t\t\t\tint nNutKN = JxKyNang_TrungNut(m_nChamX0, m_nChamY0);",
        "\t\t\t\tif (nNutKN > 0)",
    ])
    s = s.replace(CU, MOI)

    CU = NL.join([
        "\t\t\t// Ngon thu hai tro di: uu tien nut ky nang, roi den can dieu khien.",
        "\t\t\tif (m_nNgonKyNang < 0)",
    ])
    assert s.count(CU) == 1, "khong tim thay cho bat nut ky nang ngon 2 (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t\t// Ngon thu hai tro di: uu tien nut ky nang, roi den can dieu khien.",
        "\t\t\t// [ANDROID 10/09 GANTOADO] dang sua giao dien thi khong bat nut ky nang",
        "\t\t\tif (m_nNgonKyNang < 0 && !UiToaDo_DangSua())",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 2. JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- do doi: mac dinh 0, am duong deu duoc ---
    CU = NL.join([
        "static int\t\t\ts_nKNX   = -1;\t\t// -1 = neo goc phai duoi nhu ban tham khao",
        "static int\t\t\ts_nKNY   = -1;",
    ])
    assert s.count(CU) == 1, "khong tim thay khai bao s_nKNX/Y (%d)" % s.count(CU)
    MOI = NL.join([
        "// [ANDROID 10/09 GANTOADO] Do DOI ca cum so voi cho neo goc phai duoi cua ban tham khao.",
        "// Am duong deu duoc (truoc day -1 = \"chua dat\" nen keo sang trai/len tren la mat).",
        "// Che do sua giao dien keo cum di thi doi hai so nay, va luu vao UserData\\UiToaDo.ini.",
        "static int\t\t\ts_nKNX   = 0;",
        "static int\t\t\ts_nKNY   = 0;",
    ])
    s = s.replace(CU, MOI)

    for cu, moi in [
        ('\ts_nKNX       = GetPrivateProfileInt("Cham", "KyNangX", -1, szCfg);',
         '\ts_nKNX       = GetPrivateProfileInt("Cham", "KyNangX", 0, szCfg);'),
        ('\ts_nKNY       = GetPrivateProfileInt("Cham", "KyNangY", -1, szCfg);',
         '\ts_nKNY       = GetPrivateProfileInt("Cham", "KyNangY", 0, szCfg);'),
        ("\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
         "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + s_nKNX;"),
        ("\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
         "\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + s_nKNY;"),
        ("\t\t+ s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
         "\t\t+ s_nKNSangPhai + s_nKNX;"),
        ("\t\t- s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
         "\t\t- s_nKNLenTren + s_nKNY;"),
        (NL.join([
            "\t// s_nKNX/Y la do DOI so voi cho neo goc phai duoi, nen cong them phan chenh.",
            "\tif (s_nKNX < 0) s_nKNX = 0;",
            "\tif (s_nKNY < 0) s_nKNY = 0;",
            "\ts_nKNX += (x - nX);",
            "\ts_nKNY += (y - nY);",
         ]),
         NL.join([
            "\t// s_nKNX/Y la do DOI so voi cho neo goc phai duoi, nen cong them phan chenh.",
            "\t// [ANDROID 10/09 GANTOADO] am duong deu duoc - keo sang trai / len tren la do doi am.",
            "\ts_nKNX += (x - nX);",
            "\ts_nKNY += (y - nY);",
         ])),
    ]:
        assert s.count(cu) == 1, "khong tim thay: %.70s (%d)" % (cu, s.count(cu))
        s = s.replace(cu, moi)

    # --- dong chu huong dan khi dang o che do gan ---
    CU = "static bool CoAnh(const char* pszAnh)"
    assert s.count(CU) == 1
    MOI = NL.join([
        "//\t[ANDROID 10/09 GANTOADO] Ve mot dong chu (toa do man hinh) - de huong dan ngay tren man.",
        "static void KyNang_VeChu(const char* pszChu, int nX, int nY, unsigned int uMau)",
        "{",
        "\tint nDai = (int)strlen(pszChu);",
        "",
        "\tif (nDai <= 0 || g_pRepresentShell == NULL)",
        "\t\treturn;",
        "\tif (nX < 8) nX = 8;",
        "\tg_pRepresentShell->OutputText(12, (char*)pszChu, nDai, nX, nY, uMau,",
        "\t\t0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    CU = NL.join([
        "\t\tKyNang_TamNutGan(&nGX, &nGY);",
        "\t\tif (s_nKNCheDoGan)",
        "\t\t\tOVuong(nGX, nGY, KYNANG_GAN_CO / 2, 0x90FFD24A);",
        "\t\tVeAnhCo(s_szKNAnhGan, nGX, nGY, KYNANG_GAN_CO, 0);",
    ])
    assert s.count(CU) == 1, "khong tim thay cho ve nut doi che do (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\tKyNang_TamNutGan(&nGX, &nGY);",
        "\t\tif (s_nKNCheDoGan)",
        "\t\t{",
        "\t\t\tOVuong(nGX, nGY, KYNANG_GAN_CO / 2, 0x90FFD24A);",
        "\t\t\t// [ANDROID 10/09 GANTOADO] huong dan ngay tren man hinh, tung buoc mot",
        "\t\t\tKyNang_VeChu(s_nKNOChon < 0",
        '\t\t\t\t? "%s"' % vn(u"Chế độ gán: chạm vào ô kỹ năng muốn đổi"),
        '\t\t\t\t: "%s",' % vn(u"Giờ mở bảng kỹ năng, chạm một kỹ năng để gán vào ô đang sáng"),
        "\t\t\t\tnGX - 300, nGY - 38, 0xFFFFD24A);",
        "\t\t}",
        "\t\tVeAnhCo(s_szKNAnhGan, nGX, nGY, KYNANG_GAN_CO, 0);",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 3. cua so ky nang LON
for P, HAM in [
    ("Sources/S3Client/Ui/UiCase/UiSkills.cpp",
     "void KUiFightSkillSubPage::OnSkillPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)"),
    ("Sources/S3Client/Ui/UiCase/UiSkillsNew.cpp",
     "void KUiFightSkillSubPageNew::OnSkillPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)"),
]:
    s = io.open(P, encoding="latin-1", newline="").read()
    if DAU in s:
        print("da va roi, bo qua:", P)
        continue
    CU = NL.join([
        HAM,
        "{",
        "\tKUiObjAtContRegion\tPick, Drop;",
        "\tKUiDraggedObject\tObj;",
        "",
    ])
    assert s.count(CU) == 1, "khong tim thay dau OnSkillPickDrop trong %s (%d)" % (P, s.count(CU))
    MOI = NL.join([
        HAM,
        "{",
        "\tKUiObjAtContRegion\tPick, Drop;",
        "\tKUiDraggedObject\tObj;",
        "",
        "#ifdef JX_ANDROID",
        "\t// [ANDROID 10/09 GANTOADO] Tren dien thoai, bam vao o ky nang o day la NHAC LEN de",
        "\t// keo. Dang o che do GAN (nut mui ten vong tron canh cum nut ky nang) thi thay vi",
        "\t// nhac len, gan luon ky nang do vao o dang cho - va khong bat dau keo.",
        "\tif (pPickPos && !pDropPos && pPickPos->pWnd)",
        "\t{",
        "\t\tKUiDraggedObject oGan;",
        "\t\t((KWndObjectBox*)(pPickPos->pWnd))->GetObject(oGan);",
        "\t\tif (oGan.uGenre != CGOG_NOTHING && JxKyNang_GanKyNang(oGan.uGenre, oGan.uId))",
        "\t\t\treturn;",
        "\t}",
        "#endif",
        "",
    ])
    s = s.replace(CU, MOI)
    CU = '#include "UiSysMsgCentre.h"'
    assert s.count(CU) >= 1
    s = s.replace(CU, CU + NL + "#ifdef JX_ANDROID" + NL
                  + '#include "../../Platform/JxCanDieuKhien.h"\t// [ANDROID 10/09 GANTOADO] gan ky nang vao o' + NL
                  + "#endif", 1)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
