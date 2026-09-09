# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG E] Bon dieu chu chi ra, lam theo dung ban tham khao.
#
# 1. O TO NHO KHAC NHAU. Ban tham khao (KgameWorld.cpp:13845) dat ti le rieng:
#       o phu 0..4 (cung TRONG)  scale 1.5   -> to
#       o phu 5..7 (cung NGOAI)  scale 1.2   -> nho hon
#       nut danh chinh           scale 1.2 cua anh to hon han
#    Bo anh VNKU co san bon co: 70 / 100 / 140 / 200. Dung:
#       nut chinh  = assign_skill_140x140.spr
#       o phu 0..4 = assign_skill_100x100.spr
#       o phu 5..7 = assign_skill_70x70.spr
#
# 2. NUT CHINH VA NUT PHU KHAC CHUC NANG.
#       nut chinh (KgameWorld.cpp:3015 mianSkillCallback) -> MainAttack: danh thuong
#         bang ky nang danh trai, TU chon muc tieu.
#       nut phu   (KgameWorld.cpp:4364 auxiliaryskillCallback) -> SetRightSkill(ky nang)
#         roi UseSkill(x, y, ky nang) tai DIEM - tuc danh theo diem/huong, khong phai
#         khoa vao mot con.
#    Ban nay khong co iCoreShell::MainAttack, nen nut chinh dung ky nang danh TRAI +
#    LockSomeoneUseSkill vao con gan nhat - dung y nghia "danh thuong, tu chon muc tieu".
#
# 3. NGAM CO MUI TEN TAI VI TRI CHON, va tha ngon la danh theo huong do.
#    Ban tham khao (KuiMyMenu.cpp:847-912) doi do lech keo thanh mot DIEM trong the gioi:
#       diem = vi tri nhan vat + do_lech_keo * (tam_danh / ban_kinh_can)
#    va ep truc Y con MOT NUA vi goc nhin nghieng (isometric). Roi dat anh mui ten o do
#    (attackSpr_dir) va ve vong tam danh. Tha ngon -> customSkillAttack(ky nang, diem).
#    Ban nay lam y vay: mui ten dung \spr\npcres\direction_arrow.spr cua VNKU.
#
# 4. KIEM TRA NGUA truoc khi danh -> ban va 41 (Core, GDI_KYNANG_MOBILE).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 KYNANG E]"

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# ---------------------------------------------------------------- 1. anh + bien
CU = NL.join([
    "static char\t\ts_szKNAnhChinh[128] = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_100x100.spr\";",
    "static char\t\ts_szKNAnhPhu[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_70x70.spr\";",
    "static char\t\ts_szKNAnhNgam[128]  = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\effect_skill.spr\";",
])
assert s.count(CU) == 1, "khong tim thay ba duong dan anh (%d)" % s.count(CU)
MOI = NL.join([
    "//\t[ANDROID 09/09 KYNANG E] Ba co khung khac nhau - dung nhu ban tham khao:",
    "//\to phu 0..4 (cung TRONG) to hon o phu 5..7 (cung NGOAI), nut chinh to nhat.",
    "static char\t\ts_szKNAnhChinh[128] = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_140x140.spr\";",
    "static char\t\ts_szKNAnhPhu[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_100x100.spr\";",
    "static char\t\ts_szKNAnhPhuNho[128]= \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_70x70.spr\";",
    "static char\t\ts_szKNAnhNgam[128]  = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\effect_skill.spr\";",
    "static char\t\ts_szKNAnhTen[128]   = \"\\\\spr\\\\npcres\\\\direction_arrow.spr\";\t// mui ten chi huong danh",
    "",
    "//\tDiem NGAM trong the gioi (toa do VE) khi dang keo - de ve mui ten va de danh.",
    "static int\t\t\ts_nKNNgamX = 0, s_nKNNgamY = 0;",
    "static int\t\t\ts_nKNCoNgam = 0;\t// 1 = dang co diem ngam",
])
s = s.replace(CU, MOI)

CU = '\tGetPrivateProfileString("Cham", "KyNangAnhPhu", s_szKNAnhPhu, s_szKNAnhPhu,'
assert s.count(CU) == 1
MOI = NL.join([
    '\tGetPrivateProfileString("Cham", "KyNangAnhPhuNho", s_szKNAnhPhuNho, s_szKNAnhPhuNho,',
    "\t\tsizeof(s_szKNAnhPhuNho), szCfg);",
    '\tGetPrivateProfileString("Cham", "KyNangAnhTen", s_szKNAnhTen, s_szKNAnhTen,',
    "\t\tsizeof(s_szKNAnhTen), szCfg);",
    CU,
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 2. co tung o
CU = NL.join([
    "static int KyNang_CoPhu()",
    "{",
    "\treturn s_nKNCoPhu > 0 ? s_nKNCoPhu : KyNang_CoAnh(s_szKNAnhPhu, 70);",
    "}",
])
assert s.count(CU) == 1, "khong tim thay KyNang_CoPhu (%d)" % s.count(CU)
MOI = NL.join([
    "//\t[ANDROID 09/09 KYNANG E] O 0..4 nam cung TRONG nen TO hon o 5..7 cung ngoai -",
    "//\tdung ti le rieng cua ban tham khao (1.5 so voi 1.2).",
    "static const char* KyNang_AnhCuaNut(int nNut)",
    "{",
    "\tif (nNut <= 0)\t\treturn s_szKNAnhChinh;",
    "\tif (nNut <= 5)\t\treturn s_szKNAnhPhu;\t\t// o phu 0..4",
    "\treturn s_szKNAnhPhuNho;\t\t\t\t\t\t// o phu 5..7",
    "}",
    "",
    "static int KyNang_CoNut(int nNut)",
    "{",
    "\tif (nNut <= 0)",
    "\t\treturn s_nKNCoChinh > 0 ? s_nKNCoChinh : KyNang_CoAnh(s_szKNAnhChinh, 140);",
    "\tif (nNut <= 5)",
    "\t\treturn s_nKNCoPhu > 0 ? s_nKNCoPhu : KyNang_CoAnh(s_szKNAnhPhu, 100);",
    "\treturn KyNang_CoAnh(s_szKNAnhPhuNho, 70);",
    "}",
])
s = s.replace(CU, MOI)

# --- moi cho goi KyNang_CoChinh()/KyNang_CoPhu() -> KyNang_CoNut(i) ---
for cu, moi in [
    (NL.join([
        "\tif (nNut <= 0)",
        "\t{",
        "\t\tnR = KyNang_CoChinh() / 2;",
        "\t\tnDX = KYNANG_CHINH_DX;",
        "\t\tnDY = KYNANG_CHINH_DY;",
        "\t}",
        "\telse",
        "\t{",
        "\t\tnR = KyNang_CoPhu() / 2;",
        "\t\tnDX = s_nKNDX[nNut - 1];",
        "\t\tnDY = s_nKNDY[nNut - 1];",
        "\t}",
     ]),
     NL.join([
        "\tnR = KyNang_CoNut(nNut) / 2;",
        "\tif (nNut <= 0)",
        "\t{",
        "\t\tnDX = KYNANG_CHINH_DX;",
        "\t\tnDY = KYNANG_CHINH_DY;",
        "\t}",
        "\telse",
        "\t{",
        "\t\tnDX = s_nKNDX[nNut - 1];",
        "\t\tnDY = s_nKNDY[nNut - 1];",
        "\t}",
     ])),
    ("\t\tnR = (i <= 0 ? KyNang_CoChinh() : KyNang_CoPhu()) / 2;\r\n\t\t// khung tron -> do theo BAN KINH, khong phai hinh vuong",
     "\t\tnR = KyNang_CoNut(i) / 2;\r\n\t\t// khung tron -> do theo BAN KINH, khong phai hinh vuong"),
    ("\t\tnR = (i <= 0 ? KyNang_CoChinh() : KyNang_CoPhu()) / 2;\r\n\r\n\t\t// khung tron cua VNKU",
     "\t\tnR = KyNang_CoNut(i) / 2;\r\n\r\n\t\t// khung tron cua VNKU"),
    ("\t\t\tVeAnh(i <= 0 ? s_szKNAnhChinh : s_szKNAnhPhu, nX, nY);",
     "\t\t\tVeAnh(KyNang_AnhCuaNut(i), nX, nY);"),
    ('\t\ts_nKNCoAnh = (CoAnh(s_szKNAnhChinh) && CoAnh(s_szKNAnhPhu)) ? 1 : 0;',
     '\t\ts_nKNCoAnh = (CoAnh(s_szKNAnhChinh) && CoAnh(s_szKNAnhPhu)\r\n'
     '\t\t\t&& CoAnh(s_szKNAnhPhuNho)) ? 1 : 0;'),
]:
    assert s.count(cu) == 1, "khong tim thay: %.60s (%d)" % (cu, s.count(cu))
    s = s.replace(cu, moi)

# --- clamp vao trong khung ve (o ngoai cung tung bi cat mat) ---
CU = NL.join([
    "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
    "\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
])
assert s.count(CU) == 1
MOI = NL.join([
    "\t*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);",
    "\t*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);",
    "\t// [ANDROID 09/09 KYNANG E] giu han trong khung ve: da do thay o ngoai cung bi cat",
    "\t// mat mot nua khi doi cum sang phai. Cung la de man hinh co nao cung khong loi o.",
    "\tif (*px > SCREEN_WIDTH  - nR - 2)\t*px = SCREEN_WIDTH  - nR - 2;",
    "\tif (*px < nR + 2)\t\t\t\t\t*px = nR + 2;",
    "\tif (*py > SCREEN_HEIGHT - nR - 2)\t*py = SCREEN_HEIGHT - nR - 2;",
    "\tif (*py < nR + 2)\t\t\t\t\t*py = nR + 2;",
])
s = s.replace(CU, MOI)

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va (phan 1: co tung o + giu trong khung):", P)
