# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 GAN] Tu chon ky nang nao vao o nao.
#
# Chu: "kich vao ky nang trong bang ky nang se co chon vao o ky nang".
#
# Ban tham khao co han mot NUT DOI CHE DO GAN - anh \spr\Ui3\UiSkillControl\
# switch_assign_mode.spr cua VNKU (nam ngay canh assign_skill_*.spr ma ta dang dung lam
# khung o). Ten "assign skill" cua ca bo anh cung noi ro y do: cac o la de NGUOI CHOI GAN.
#
# Cach dung:
#   1. Cham nut doi che do (mui ten vong tron) -> vao CHE DO GAN, cac o vien vang.
#   2. Cham mot o -> o do sang len (dang cho gan). Trong che do nay cham o KHONG danh.
#   3. Mo bang ky nang (nut ky nang tren thanh cong cu) roi cham mot ky nang
#      -> ky nang do vao o vua chon, va thoat che do gan.
#   Cham lai nut doi che do de thoat ma khong gan gi.
#
# Gan xong ghi vao UserData\KyNangMobile.ini nen lan sau mo game van con.
# O nao chua gan thi van lay theo danh sach ky nang danh nhu truoc (de nguoi moi choi
# khong phai gan tay moi dung duoc).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 GAN]"

# ================================================================ 1. JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    # --- bien ---
    CU = "static char\t\ts_szKNAnhNum[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\joystick_ctrl.spr\";"
    assert s.count(CU) == 1
    MOI = NL.join([
        CU,
        "//\t[ANDROID 09/09 GAN] nut doi che do GAN ky nang vao o (anh cua VNKU, cung bo)",
        "static char\t\ts_szKNAnhGan[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\switch_assign_mode.spr\";",
        "#define\tKYNANG_NUT_GAN\t\t100\t\t// ma tra ve cua JxKyNang_TrungNut cho nut doi che do",
        "#define\tKYNANG_GAN_DX\t\t300\t\t// cho dat nut doi che do, tinh tu goc phai duoi",
        "#define\tKYNANG_GAN_DY\t\t30",
        "#define\tKYNANG_GAN_CO\t\t40",
        "",
        "static int\t\t\ts_nKNCheDoGan = 0;\t// 1 = dang o che do gan",
        "static int\t\t\ts_nKNOChon = -1;\t// o dang cho gan (0..7), -1 = chua chon",
        "static KUiGameObject s_KNGan[KYNANG_SO_PHU];\t// ky nang nguoi choi tu gan cho tung o",
        "static int\t\t\ts_nKNDaDocGan = 0;",
    ])
    s = s.replace(CU, MOI)

    # --- doc / ghi tep gan ---
    CU = "//\tCo THAT cua mot anh .spr (de neo o theo dung ban kinh cua no)."
    assert s.count(CU) == 1
    MOI = NL.join([
        "//\t[ANDROID 09/09 GAN] Doc / ghi bang gan ky nang: UserData\\KyNangMobile.ini",
        "//\tMoi dong: O<so>=<loai>,<ma ky nang>",
        "static void KyNang_DuongTepGan(char* pszRa, int nCo)",
        "{",
        "\tpszRa[0] = 0;",
        "\tGetCurrentDirectory(nCo, pszRa);",
        "\tstrncat(pszRa, \"\\\\UserData\\\\KyNangMobile.ini\", nCo - strlen(pszRa) - 1);",
        "}",
        "",
        "static void KyNang_DocGan()",
        "{",
        "\tchar szTep[MAX_PATH];",
        "\tchar szDong[128];",
        "\tFILE* pTep;",
        "",
        "\tif (s_nKNDaDocGan)",
        "\t\treturn;",
        "\ts_nKNDaDocGan = 1;",
        "\tmemset(s_KNGan, 0, sizeof(s_KNGan));",
        "\tKyNang_DuongTepGan(szTep, sizeof(szTep));",
        "\tpTep = fopen(szTep, \"rt\");",
        "\tif (pTep == NULL)",
        "\t\treturn;",
        "\twhile (fgets(szDong, sizeof(szDong), pTep))",
        "\t{",
        "\t\tint nO = 0, nLoai = 0, nMa = 0;",
        "",
        "\t\tif (sscanf(szDong, \"O%d=%d,%d\", &nO, &nLoai, &nMa) == 3",
        "\t\t\t&& nO >= 0 && nO < KYNANG_SO_PHU)",
        "\t\t{",
        "\t\t\ts_KNGan[nO].uGenre = (unsigned int)nLoai;",
        "\t\t\ts_KNGan[nO].uId    = (unsigned int)nMa;",
        "\t\t}",
        "\t}",
        "\tfclose(pTep);",
        "}",
        "",
        "static void KyNang_GhiGan()",
        "{",
        "\tchar szTep[MAX_PATH];",
        "\tFILE* pTep;",
        "\tint i;",
        "",
        "\tKyNang_DuongTepGan(szTep, sizeof(szTep));",
        "\tpTep = fopen(szTep, \"wt\");",
        "\tif (pTep == NULL)",
        "\t{",
        "\t\tg_DebugLog(\"[KYNANG] khong ghi duoc %s\", szTep);",
        "\t\treturn;",
        "\t}",
        "\tfprintf(pTep, \"; [ANDROID] Ky nang nguoi choi tu gan cho tung o tren dien thoai.\\n\");",
        "\tfprintf(pTep, \"; Moi dong: O<so o 0..7>=<loai>,<ma ky nang>. Xoa tep = ve mac dinh.\\n\");",
        "\tfor (i = 0; i < KYNANG_SO_PHU; i++)",
        "\t{",
        "\t\tif (s_KNGan[i].uId)",
        "\t\t\tfprintf(pTep, \"O%d=%u,%u\\n\", i, s_KNGan[i].uGenre, s_KNGan[i].uId);",
        "\t}",
        "\tfclose(pTep);",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- vi tri nut doi che do ---
    CU = "//\tTam cua mot o, toa do MAN HINH. nNut: 0 = nut danh chinh, 1..8 = o phu."
    assert s.count(CU) == 1
    MOI = NL.join([
        "//\t[ANDROID 09/09 GAN] Tam nut doi che do gan.",
        "static void KyNang_TamNutGan(int* px, int* py)",
        "{",
        "\tint nR = KYNANG_GAN_CO / 2;",
        "",
        "\t*px = SCREEN_WIDTH  - (nR + KYNANG_GAN_DX * s_nKNGian / 100)",
        "\t\t+ s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
        "\t*py = SCREEN_HEIGHT - (nR + KYNANG_GAN_DY * s_nKNGian / 100)",
        "\t\t- s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
        "\tif (*px < nR + 2) *px = nR + 2;",
        "\tif (*py > SCREEN_HEIGHT - nR - 2) *py = SCREEN_HEIGHT - nR - 2;",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- KyNang_CuaNut: uu tien ky nang da gan ---
    CU = NL.join([
        "\tif (nNut - 1 >= s_nKNCo1)",
        "\t{",
        "\t\tmemset(pRa, 0, sizeof(*pRa));",
        "\t\treturn false;",
        "\t}",
        "\tpRa->uGenre = s_KNBang[nNut - 1].uGenre;",
        "\tpRa->uId    = s_KNBang[nNut - 1].uId;",
        "\treturn (pRa->uGenre != CGOG_NOTHING && pRa->uId != 0);",
    ])
    assert s.count(CU) == 1, "khong tim thay than KyNang_CuaNut (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 GAN] Uu tien ky nang NGUOI CHOI TU GAN cho o nay.",
        "\tKyNang_DocGan();",
        "\tif (s_KNGan[nNut - 1].uId)",
        "\t{",
        "\t\t*pRa = s_KNGan[nNut - 1];",
        "\t\treturn true;",
        "\t}",
        "\t// Chua gan thi lay theo danh sach ky nang danh, de dung duoc ngay khong phai gan tay.",
        "\tif (nNut - 1 >= s_nKNCo1)",
        "\t{",
        "\t\tmemset(pRa, 0, sizeof(*pRa));",
        "\t\treturn false;",
        "\t}",
        "\tpRa->uGenre = s_KNBang[nNut - 1].uGenre;",
        "\tpRa->uId    = s_KNBang[nNut - 1].uId;",
        "\treturn (pRa->uGenre != CGOG_NOTHING && pRa->uId != 0);",
    ])
    s = s.replace(CU, MOI)

    # --- TrungNut: them nut doi che do, va o trong van bat cham khi dang gan ---
    CU = NL.join([
        "\tKyNang_DocBang();",
        "\t// Xet o phu TRUOC roi moi den nut chinh: cung o phu vong sat nut chinh, uu tien",
        "\t// o nho de cham vao ria cung khong bi nut to nuot mat.",
        "\tfor (i = KYNANG_SO_PHU; i >= 0; i--)",
        "\t{",
        "\t\tKUiGameObject o;",
        "",
        "\t\tif (!KyNang_CuaNut(i, &o))",
        "\t\t\tcontinue;\t\t// o trong thi khong bat cham",
    ])
    assert s.count(CU) == 1, "khong tim thay than JxKyNang_TrungNut (%d)" % s.count(CU)
    MOI = NL.join([
        "\tKyNang_DocBang();",
        "\t// [ANDROID 09/09 GAN] nut doi che do gan",
        "\t{",
        "\t\tint nGX, nGY, nGR = KYNANG_GAN_CO / 2;",
        "",
        "\t\tKyNang_TamNutGan(&nGX, &nGY);",
        "\t\tif ((x - nGX) * (x - nGX) + (y - nGY) * (y - nGY) <= nGR * nGR)",
        "\t\t\treturn KYNANG_NUT_GAN;",
        "\t}",
        "\t// Xet o phu TRUOC roi moi den nut chinh: cung o phu vong sat nut chinh, uu tien",
        "\t// o nho de cham vao ria cung khong bi nut to nuot mat.",
        "\tfor (i = KYNANG_SO_PHU; i >= 0; i--)",
        "\t{",
        "\t\tKUiGameObject o;",
        "",
        "\t\t// Trong che do gan thi o TRONG cung phai bat duoc cham, khong thi khong gan",
        "\t\t// duoc vao o trong.",
        "\t\tif (!KyNang_CuaNut(i, &o) && !s_nKNCheDoGan)",
        "\t\t\tcontinue;",
    ])
    s = s.replace(CU, MOI)

    # --- BatDau: xu ly che do gan ---
    CU = NL.join([
        "void JxKyNang_BatDau(int nNut, int x, int y)",
        "{",
        "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    ])
    assert s.count(CU) == 1
    MOI = NL.join([
        "void JxKyNang_BatDau(int nNut, int x, int y)",
        "{",
        "\t// [ANDROID 09/09 GAN] nut doi che do",
        "\tif (nNut == KYNANG_NUT_GAN)",
        "\t{",
        "\t\ts_nKNCheDoGan = !s_nKNCheDoGan;",
        "\t\ts_nKNOChon = -1;",
        "\t\ts_nKNDangCam = -1;",
        "\t\tg_DebugLog(\"[KYNANG] che do gan = %d\", s_nKNCheDoGan);",
        "\t\treturn;",
        "\t}",
        "\t// Dang o che do gan: cham o la CHON o do de gan, khong danh.",
        "\tif (s_nKNCheDoGan)",
        "\t{",
        "\t\ts_nKNOChon = (nNut - 1 > 0) ? (nNut - 2) : -1;\t// chi o phu moi gan duoc",
        "\t\ts_nKNDangCam = -1;",
        "\t\treturn;",
        "\t}",
        "\ts_nKNDangCam = nNut - 1;\t// 0 = nut chinh, 1..8 = o phu",
    ])
    s = s.replace(CU, MOI)

    # --- ham gan tu bang ky nang ---
    CU = "//\t[ANDROID 09/09 KYNANG I] Goi moi vong lap game: con de nut thi cu danh tiep."
    assert s.count(CU) == 1
    MOI = NL.join([
        "//\t[ANDROID 09/09 GAN] Bang ky nang goi vao day khi nguoi choi cham mot ky nang.",
        "//\tTra ve true = da gan vao o dang cho, ben goi khoi lam viec cua no nua.",
        "bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId)",
        "{",
        "\tif (!s_nKNCheDoGan || s_nKNOChon < 0 || s_nKNOChon >= KYNANG_SO_PHU)",
        "\t\treturn false;",
        "\tif (uId == 0)",
        "\t\treturn false;",
        "\tKyNang_DocGan();",
        "\ts_KNGan[s_nKNOChon].uGenre = uGenre;",
        "\ts_KNGan[s_nKNOChon].uId    = uId;",
        "\tKyNang_GhiGan();",
        "\tg_DebugLog(\"[KYNANG] gan ky nang %u vao o %d\", uId, s_nKNOChon);",
        "\ts_nKNOChon = -1;",
        "\ts_nKNCheDoGan = 0;\t\t// gan xong thi ra khoi che do gan luon",
        "\treturn true;",
        "}",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    # --- ve nut doi che do + vien o dang chon ---
    CU = NL.join([
        "\t\t// bieu tuong ky nang nam GON trong khung tron: lui vao ~30% ban kinh",
    ])
    if s.count(CU) == 0:
        CU = "\t\t// [ANDROID 09/09 KYNANG D] KSkill::DrawSkillIcon (KSkills.cpp:2861) BO QUA"
    assert s.count(CU) == 1, "khong tim thay khoi ve bieu tuong (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\t// [ANDROID 09/09 GAN] o dang cho gan: to nen sang cho de nhan",
        "\t\tif (s_nKNCheDoGan && i > 0 && (i - 1) == s_nKNOChon)",
        "\t\t\tOVuong(nX, nY, nR, 0x80FFD24A);",
        "",
        CU,
    ])
    s = s.replace(CU, MOI)

    CU = NL.join([
        "\t// Dang giu mot nut: vong sang ngam",
    ])
    if s.count(CU) == 0:
        CU = "\t// Dang ngam: vach chi huong tu nut toi ngon tay + vong tron duoi chan con dich."
    if s.count(CU) == 0:
        CU = "\tif (s_nKNDangCam >= 0)\r\n\t{\r\n\t\tKyNang_TamNut(s_nKNDangCam, &nX, &nY);"
    assert s.count(CU) == 1, "khong tim thay moc truoc khoi dang giu (%d)" % s.count(CU)
    MOI = NL.join([
        "\t// [ANDROID 09/09 GAN] nut doi che do gan - ve sau cung de nam tren",
        "\tif (CoAnh(s_szKNAnhGan))",
        "\t{",
        "\t\tint nGX, nGY;",
        "",
        "\t\tKyNang_TamNutGan(&nGX, &nGY);",
        "\t\tif (s_nKNCheDoGan)",
        "\t\t\tOVuong(nGX, nGY, KYNANG_GAN_CO / 2, 0x90FFD24A);",
        "\t\tVeAnhCo(s_szKNAnhGan, nGX, nGY, KYNANG_GAN_CO, 0);",
        "\t}",
        "",
        CU,
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
    CU = "void JxKyNang_Nhip();"
    assert s.count(CU) == 1
    MOI = NL.join([
        "// [ANDROID 09/09 GAN] Bang ky nang goi vao day khi nguoi choi cham mot ky nang.",
        "// Tra ve true = da gan vao o dang cho, ben goi khoi lam viec cua no nua.",
        "bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId);",
        CU,
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================ 3. moc o bang ky nang
P = "Sources/S3Client/Ui/UiCase/UiSkillTree.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = NL.join([
        "\t\tif ((nIndex = GetSkillAtPos(x, y)) >= 0)",
        "\t\t{",
        "\t\t\tKUiGameObject\tSkill;",
        "\t\t\tSkill.uGenre = m_Skills[nIndex].uGenre;",
        "\t\t\tSkill.uId = m_Skills[nIndex].uId;",
        "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL,",
        "\t\t\t\t(KUPARAM)&Skill, m_bLeft ? 0 : 1);",
        "\t\t}\t\t",
    ])
    assert s.count(CU) == 1, "khong tim thay nhanh bam ky nang (%d)" % s.count(CU)
    MOI = NL.join([
        "\t\tif ((nIndex = GetSkillAtPos(x, y)) >= 0)",
        "\t\t{",
        "\t\t\tKUiGameObject\tSkill;",
        "\t\t\tSkill.uGenre = m_Skills[nIndex].uGenre;",
        "\t\t\tSkill.uId = m_Skills[nIndex].uId;",
        "#ifdef JX_ANDROID",
        "\t\t\t// [ANDROID 09/09 GAN] Dang o che do gan tren dien thoai: cham ky nang la",
        "\t\t\t// GAN vao o dang cho, khong phai dat lam ky nang danh trai/phai.",
        "\t\t\tif (!JxKyNang_GanKyNang(Skill.uGenre, Skill.uId))",
        "#endif",
        "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL,",
        "\t\t\t\t(KUPARAM)&Skill, m_bLeft ? 0 : 1);",
        "\t\t}\t\t",
    ])
    s = s.replace(CU, MOI)

    CU = '#include "uiskilltree.h"'
    assert s.count(CU) == 1, "khong tim thay include uiskilltree.h (%d)" % s.count(CU)
    s = s.replace(CU, CU + NL + "#ifdef JX_ANDROID" + NL
                  + '#include "../../Platform/JxCanDieuKhien.h"\t// [ANDROID 09/09 GAN]' + NL
                  + "#endif", 1)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)
