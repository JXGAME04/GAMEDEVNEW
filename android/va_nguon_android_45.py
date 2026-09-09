# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG H] Hai cho chu chi ra - lay DUNG so cua nguon tham khao.
#
# 1. O KY NANG QUA TO. Chu dung: dang le phai suy ra co that tu ban tham khao chu khong
#    chon bua theo co anh co san. Nguon ghi ro (KgameWorld.cpp:13821):
#        nor_circleMask->setContentSize(CCSizeMake(40, 40));   // nen tron 40 diem anh
#        RunScaleItem::create(..., scaleGoc)                    // 1.5 cho o 0..4
#                                                               // 1.2 cho o 5..7
#    => o phu 0..4 = 40 * 1.5 = 60 diem anh; o phu 5..7 = 40 * 1.2 = 48 diem anh.
#    Ban truoc toi lay nguyen anh 100x100 va 70x70 -> to gap ruoi, cac vong chong nhau.
#    Nay ve bang RU_T_IMAGE_STRETCH (KItem.cpp:1991 da dung san) nen dat duoc DUNG co,
#    khong con phu thuoc co san cua tep anh. Do lech giua cac o gio khop, bo he so gian.
#
# 2. MUI TEN SAI. Chu: "mui ten do la huong di chuyen cua player - con mui ten dinh huong
#    ky nang se la mui ten xanh va dai".
#        \spr\npcres\direction_arrow.spr    = huong DI CHUYEN (chevron nho)  <- ban truoc dung nham
#        \spr\npcres\attack_direction.spr   = huong DANH (143 KB, nhieu khung theo huong)
#    Doi sang attack_direction.spr va chon KHUNG theo huong ngam, nen mui ten quay theo
#    dung phia dang chi.

import io
import os
import shutil

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
DAU = "[ANDROID 09/09 KYNANG H]"

# ---- chep anh mui ten danh ----
SRC = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\spr\Spr\npcres\attack_direction.spr"
DST = r"D:\jx1_android_data\spr\npcres\attack_direction.spr"
if os.path.isfile(SRC):
    os.makedirs(os.path.dirname(DST), exist_ok=True)
    shutil.copyfile(SRC, DST)
    print("chep attack_direction.spr", os.path.getsize(SRC), "byte")

s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# ---------------------------------------------------------------- 1. co that + anh
CU = NL.join([
    "//\t[ANDROID 09/09 KYNANG E] Ba co khung khac nhau - dung nhu ban tham khao:",
    "//\to phu 0..4 (cung TRONG) to hon o phu 5..7 (cung NGOAI), nut chinh to nhat.",
    "static char\t\ts_szKNAnhChinh[128] = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_140x140.spr\";",
    "static char\t\ts_szKNAnhPhu[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_100x100.spr\";",
    "static char\t\ts_szKNAnhPhuNho[128]= \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_70x70.spr\";",
    "static char\t\ts_szKNAnhNgam[128]  = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\effect_skill.spr\";",
    "static char\t\ts_szKNAnhTen[128]   = \"\\\\spr\\\\npcres\\\\direction_arrow.spr\";\t// mui ten chi huong danh",
])
assert s.count(CU) == 1, "khong tim thay khoi anh (%d)" % s.count(CU)
MOI = NL.join([
    "//\t[ANDROID 09/09 KYNANG H] CO THAT lay tu ban tham khao (KgameWorld.cpp:13821):",
    "//\tnen tron 40 diem anh, nhan 1.5 cho o 0..4 va 1.2 cho o 5..7 => 60 va 48.",
    "//\tVe bang RU_T_IMAGE_STRETCH nen dat duoc dung co, khong phu thuoc co san cua tep anh;",
    "//\tnho vay chi can MOT tep khung cho ca ba cap.",
    "#define\tKYNANG_CO_CHINH\t92\t// nut danh chinh (mr-1_new nhan 1.2 cua ban tham khao)",
    "#define\tKYNANG_CO_TRONG\t60\t// o phu 0..4 = 40 * 1.5",
    "#define\tKYNANG_CO_NGOAI\t48\t// o phu 5..7 = 40 * 1.2",
    "",
    "static char\t\ts_szKNAnhChinh[128] = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_100x100.spr\";",
    "static char\t\ts_szKNAnhPhu[128]   = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_100x100.spr\";",
    "static char\t\ts_szKNAnhPhuNho[128]= \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\assign_skill_100x100.spr\";",
    "static char\t\ts_szKNAnhNgam[128]  = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\effect_skill.spr\";",
    "//\t[ANDROID 09/09 KYNANG H] direction_arrow.spr la huong DI CHUYEN cua nhan vat -",
    "//\tkhong phai cai nay. Mui ten dinh huong DANH la attack_direction.spr (nhieu khung",
    "//\ttheo huong) - chon khung theo goc ngam thi mui ten quay dung phia dang chi.",
    "static char\t\ts_szKNAnhTen[128]   = \"\\\\spr\\\\npcres\\\\attack_direction.spr\";",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 2. ham co / anh
CU = NL.join([
    "static int KyNang_CoNut(int nNut)",
    "{",
    "\tif (nNut <= 0)",
    "\t\treturn s_nKNCoChinh > 0 ? s_nKNCoChinh : KyNang_CoAnh(s_szKNAnhChinh, 140);",
    "\tif (nNut <= 5)",
    "\t\treturn s_nKNCoPhu > 0 ? s_nKNCoPhu : KyNang_CoAnh(s_szKNAnhPhu, 100);",
    "\treturn KyNang_CoAnh(s_szKNAnhPhuNho, 70);",
    "}",
])
assert s.count(CU) == 1, "khong tim thay KyNang_CoNut (%d)" % s.count(CU)
MOI = NL.join([
    "static int KyNang_CoNut(int nNut)",
    "{",
    "\tif (nNut <= 0)",
    "\t\treturn s_nKNCoChinh > 0 ? s_nKNCoChinh : KYNANG_CO_CHINH;",
    "\tif (nNut <= 5)",
    "\t\treturn s_nKNCoPhu > 0 ? s_nKNCoPhu : KYNANG_CO_TRONG;\t// o phu 0..4",
    "\treturn KYNANG_CO_NGOAI;\t\t\t\t\t\t\t\t\t// o phu 5..7",
    "}",
    "",
    "//\t[ANDROID 09/09 KYNANG H] Ve mot anh .spr vao DUNG o vuong (tam nX,nY canh nCo).",
    "//\tDung RU_T_IMAGE_STRETCH nhu KItem.cpp:1991 - nho vay co nut khong bi buoc theo",
    "//\tco san cua tep anh, va man hinh nao cung dat duoc dung co mong muon.",
    "static void VeAnhCo(const char* pszAnh, int nX, int nY, int nCo, int nKhung)",
    "{",
    "\tKRUImage a;",
    "",
    "\tif (g_pRepresentShell == NULL || nCo < 2)",
    "\t\treturn;",
    "\tmemset(&a, 0, sizeof(a));",
    "\ta.nType = ISI_T_SPR;",
    "\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
    "\ta.Color.Color_dw = 0xffffffff;",
    "\ta.nISPosition = IMAGE_IS_POSITION_INIT;",
    "\ta.nFrame = nKhung;",
    "\tstrncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);",
    "\ta.oPosition.nX = nX - nCo / 2;",
    "\ta.oPosition.nY = nY - nCo / 2;",
    "\ta.oPosition.nZ = 0;",
    "\ta.oEndPos.nX = a.oPosition.nX + nCo;",
    "\ta.oEndPos.nY = a.oPosition.nY + nCo;",
    "\ta.oEndPos.nZ = 0;",
    "\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);",
    "}",
    "",
    "//\tSo khung cua mot anh .spr (mui ten danh co mot khung cho moi huong).",
    "static int KyNang_SoKhung(const char* pszAnh)",
    "{",
    "\tKImageParam oTs;",
    "",
    "\tmemset(&oTs, 0, sizeof(oTs));",
    "\tif (g_pRepresentShell && g_pRepresentShell->GetImageParam(pszAnh, &oTs, ISI_T_SPR)",
    "\t\t&& oTs.nNumFrames > 0)",
    "\t\treturn (int)oTs.nNumFrames;",
    "\treturn 1;",
    "}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 3. ve khung bang co that
CU = NL.join([
    "\t\tif (s_nKNCoAnh)",
    "\t\t\tVeAnh(KyNang_AnhCuaNut(i), nX, nY);",
    "\t\telse",
    "\t\t\tOVuong(nX, nY, nR, (i == s_nKNDangCam) ? 0xB03A8A3A : 0x80202020);",
])
assert s.count(CU) == 1, "khong tim thay khoi ve khung (%d)" % s.count(CU)
MOI = NL.join([
    "\t\tif (s_nKNCoAnh)",
    "\t\t\tVeAnhCo(KyNang_AnhCuaNut(i), nX, nY, nR * 2, 0);",
    "\t\telse",
    "\t\t\tOVuong(nX, nY, nR, (i == s_nKNDangCam) ? 0xB03A8A3A : 0x80202020);",
])
s = s.replace(CU, MOI)

# vong ngam cung ve theo co nut dang giu
CU = "\t\tif (CoAnh(s_szKNAnhNgam))\r\n\t\t\tVeAnh(s_szKNAnhNgam, nX, nY);\t// effect_skill.spr cua VNKU"
assert s.count(CU) == 1
MOI = NL.join([
    "\t\tif (CoAnh(s_szKNAnhNgam))\t// vong sang effect_skill.spr cua VNKU, vua khit nut",
    "\t\t\tVeAnhCo(s_szKNAnhNgam, nX, nY, KyNang_CoNut(s_nKNDangCam) * 3 / 2, 0);",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 4. mui ten theo huong
CU = NL.join([
    "\t\t// [ANDROID 09/09 KYNANG F] MUI TEN tai vi tri dang ngam - chu: \"huong do se co",
    "\t\t// mui ten theo vi tri chon\". Ban tham khao dat anh attackSpr_dir o dung diem nay.",
    "\t\tif (s_nKNCoNgam && CoAnh(s_szKNAnhTen))",
    "\t\t\tVeAnh(s_szKNAnhTen, s_nKNNgamX, s_nKNNgamY);",
])
assert s.count(CU) == 1, "khong tim thay khoi ve mui ten (%d)" % s.count(CU)
MOI = NL.join([
    "\t\t// [ANDROID 09/09 KYNANG H] MUI TEN DINH HUONG DANH tai vi tri dang ngam.",
    "\t\t// attack_direction.spr co mot khung cho moi huong - chon khung theo goc ngam",
    "\t\t// thi mui ten quay dung phia dang chi.",
    "\t\tif (s_nKNCoNgam && CoAnh(s_szKNAnhTen))",
    "\t\t{",
    "\t\t\tint nSo = KyNang_SoKhung(s_szKNAnhTen);",
    "\t\t\tint nKhung = 0;",
    "",
    "\t\t\tif (nSo > 1)",
    "\t\t\t{",
    "\t\t\t\tint dx = s_nKNNgamX - SCREEN_WIDTH / 2;",
    "\t\t\t\tint dy = s_nKNNgamY - SCREEN_HEIGHT / 2;",
    "\t\t\t\t// goc 0 = sang phai, tang nguoc chieu kim dong ho (truc Y man hinh huong xuong)",
    "\t\t\t\tdouble fGoc = atan2((double)(-dy), (double)dx) * 180.0 / 3.14159265358979;",
    "\t\t\t\tif (fGoc < 0) fGoc += 360.0;",
    "\t\t\t\tnKhung = (int)((fGoc * nSo + 180.0) / 360.0) % nSo;",
    "\t\t\t}",
    "\t\t\tVeAnh2(s_szKNAnhTen, s_nKNNgamX, s_nKNNgamY, nKhung);",
    "\t\t}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 5. VeAnh co chon khung
CU = "static bool CoAnh(const char* pszAnh)"
assert s.count(CU) == 1
MOI = NL.join([
    "//\t[ANDROID 09/09 KYNANG H] nhu VeAnh nhung chon duoc KHUNG (anh nhieu huong).",
    "static void VeAnh2(const char* pszAnh, int nX, int nY, int nKhung)",
    "{",
    "\tKRUImage a;",
    "\tKRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };",
    "",
    "\tif (g_pRepresentShell == NULL)",
    "\t\treturn;",
    "\tmemset(&a, 0, sizeof(a));",
    "\ta.nType = ISI_T_SPR;",
    "\ta.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
    "\ta.Color.Color_dw = 0xffffffff;",
    "\ta.nISPosition = IMAGE_IS_POSITION_INIT;",
    "\ta.nFrame = nKhung;",
    "\tstrncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);",
    "\tif (g_pRepresentShell->GetImageFrameParam(a.szImage, nKhung, &oOff, &oCo, a.nType)",
    "\t\t&& oCo.nX > 0)",
    "\t{",
    "\t\ta.oPosition.nX = nX - oCo.nX / 2;",
    "\t\ta.oPosition.nY = nY - oCo.nY / 2;",
    "\t}",
    "\telse",
    "\t{",
    "\t\ta.oPosition.nX = nX;",
    "\t\ta.oPosition.nY = nY;",
    "\t}",
    "\tg_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE, true);",
    "}",
    "",
    CU,
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 6. bo he so gian
CU = '\ts_nKNGian = GetPrivateProfileInt("Cham", "KyNangGian", 120, szCfg);'
assert s.count(CU) == 1
s = s.replace(CU, '\ts_nKNGian = GetPrivateProfileInt("Cham", "KyNangGian", 100, szCfg);')
CU = "static int\t\t\ts_nKNGian = 120;\t// phan tram"
assert s.count(CU) == 1
s = s.replace(CU, "static int\t\t\ts_nKNGian = 100;\t// phan tram (co nut da dung roi nen khong can gian)")

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
