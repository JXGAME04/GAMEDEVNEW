# -*- coding: utf-8 -*-
#
# [ANDROID 09/09 KYNANG] Nut chon ky nang danh cho ban mobile.
#
# Chu: "Thieu phan cac nut chon ky nang danh ban mobile - cham la danh, dieu chinh
# huong danh - ma nguon mobile tham khao da co san dua vao viet lai cho phu hop".
#
# Nguon tham khao: D:\USVOLAM\Jx1mClientMobile\Classes\gamescene\KSkillRocker.cpp
# (dat ngon len nut roi keo de ngam huong; tha ngon la danh). Ban ay viet cho Cocos
# nen chi lay CACH LAM, phan ve va phan cham viet lai theo bo cua ban nay.
#
# Ba viec:
#   1. Bang ky nang danh - lay tu GDI_LEFT_ENABLE_SKILLS (dung danh sach ma ban PC
#      dung cho o danh chuot TRAI), ve bang chinh ham ve bieu tuong cua game
#      (iCoreShell::DrawGameObj) nen bieu tuong giong het ban PC, khong can them anh.
#   2. CHAM mot nut = chon ky nang do lam ky nang danh trai (GOI_SET_IMMDIA_SKILL,
#      y het ban PC) roi danh ngay con dich GAN NHAT.
#   3. GIU roi KEO = ngam: huong keo quyet dinh danh con nao (Core loc trong non
#      +-60 do quanh huong keo, xem ban va 35). Trong luc keo co vach chi huong va
#      vong tron duoi chan con dang ngam. Tha ngon la danh.
#
# Danh = iCoreShell::LockSomeoneUseSkill(chi so NPC, ma ky nang) - dung ham ma ban PC
# dung o ShortcutKey.cpp:1712, khong dat them duong danh rieng cho mobile.
#
# config.ini [Cham]:
#   KyNang     1 = bat (mac dinh)
#   KyNangSo   so nut hien ra (mac dinh 6)
#   KyNangCot  so cot (mac dinh 2)
#   KyNangCo   canh mot nut, diem anh (mac dinh 56)
#   KyNangX/Y  goc trai tren cua bang nut; -1 = tu tinh (goc phai duoi, tren thanh cong cu)

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[ANDROID 09/09 KYNANG]"

# ================================================================= 1. header
P = "Sources/S3Client/Platform/JxCanDieuKhien.h"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    CU = "// Goi cuoi moi khung ve: ve icon \"noi chuyen\" tren dau NPC doi thoai gan nhat.\r\nvoid JxIconNpc_Ve();"
    assert s.count(CU) == 1, "khong tim thay khai bao JxIconNpc_Ve (%d)" % s.count(CU)
    MOI = NL.join([
        CU,
        "",
        "//---------------------------------------------------------------------------",
        "// [ANDROID 09/09 KYNANG] Nut chon ky nang danh.",
        "//",
        "// Cham mot nut = chon ky nang do lam ky nang danh trai roi danh con dich gan nhat.",
        "// Giu roi keo = ngam: huong keo quyet dinh danh con nao; tha ngon la danh.",
        "// Cach lam theo lop KSkillRocker cua ban JX1 Mobile (D:\\USVOLAM\\Jx1mClientMobile).",
        "//---------------------------------------------------------------------------",
        "",
        "// (x, y) co trung mot nut ky nang khong. Tra ve so thu tu nut + 1, hoac 0.",
        "int  JxKyNang_TrungNut(int x, int y);",
        "// Dat ngon len nut thu (nNut - 1).",
        "void JxKyNang_BatDau(int nNut, int x, int y);",
        "// Ngon di chuyen - ngam huong danh.",
        "void JxKyNang_Keo(int x, int y);",
        "// Nha ngon = danh. Tra ve true neu da danh.",
        "bool JxKyNang_Nha();",
        "// Goi cuoi moi khung ve: ve bang nut + vach ngam + vong tron duoi chan con dang ngam.",
        "void JxKyNang_Ve();",
    ])
    s = s.replace(CU, MOI)
    io.open(P, "w", encoding="latin-1", newline="").write(s)
    print("da va:", P)

# ================================================================= 2. cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# --- include ---
CU = '#include "../../Core/src/GameDataDef.h"'
assert s.count(CU) == 1
s = s.replace(CU, CU + NL + '#include "../../Core/src/CoreObjGenreDef.h"\t// [ANDROID 09/09 KYNANG] CGOG_NOTHING')

# --- cai dat doc them tu config ---
CU = '\tGetPrivateProfileString("Cham", "IconNpcAnh", s_szIconAnh, s_szIconAnh, sizeof(s_szIconAnh), szCfg);'
assert s.count(CU) == 1
MOI = NL.join([
    CU,
    "\t// [ANDROID 09/09 KYNANG]",
    '\ts_nKNBat = GetPrivateProfileInt("Cham", "KyNang", 1, szCfg);',
    '\ts_nKNSo  = GetPrivateProfileInt("Cham", "KyNangSo", 6, szCfg);',
    '\ts_nKNCot = GetPrivateProfileInt("Cham", "KyNangCot", 2, szCfg);',
    '\ts_nKNCo  = GetPrivateProfileInt("Cham", "KyNangCo", 56, szCfg);',
    '\ts_nKNX   = GetPrivateProfileInt("Cham", "KyNangX", -1, szCfg);',
    '\ts_nKNY   = GetPrivateProfileInt("Cham", "KyNangY", -1, szCfg);',
    "\tif (s_nKNSo  < 1)  s_nKNSo  = 1;",
    "\tif (s_nKNSo  > KYNANG_TOI_DA) s_nKNSo = KYNANG_TOI_DA;",
    "\tif (s_nKNCot < 1)  s_nKNCot = 1;",
    "\tif (s_nKNCo  < 24) s_nKNCo  = 24;",
])
s = s.replace(CU, MOI)

# --- bien trang thai ---
CU = "static bool\ts_bCam = false;"
assert s.count(CU) == 1
MOI = NL.join([
    "// [ANDROID 09/09 KYNANG] bang nut ky nang",
    "#define\tKYNANG_TOI_DA\t\t12\t\t// so nut nhieu nhat cho hien",
    "#define\tKYNANG_DS_TOI_DA\t65\t\t// bang GDI_LEFT_ENABLE_SKILLS tra ve toi da 65 muc",
    "#define\tKYNANG_CACH\t\t\t6\t\t// khe ho giua hai nut",
    "#define\tKYNANG_LAM_MOI_MS\t2000\t// bao lau doc lai danh sach ky nang mot lan",
    "",
    "static int\t\t\ts_nKNBat = 1;",
    "static int\t\t\ts_nKNSo  = 6;",
    "static int\t\t\ts_nKNCot = 2;",
    "static int\t\t\ts_nKNCo  = 56;",
    "static int\t\t\ts_nKNX   = -1;\t\t// -1 = tu tinh",
    "static int\t\t\ts_nKNY   = -1;",
    "",
    "static KUiSkillData\ts_KNBang[KYNANG_DS_TOI_DA];",
    "static int\t\t\ts_nKNCo1 = 0;\t\t// so ky nang doc duoc",
    "static unsigned int\ts_uKNDocLuc = 0;",
    "static int\t\t\ts_nKNDangCam = -1;\t// nut dang giu (0-based), -1 = khong",
    "static int\t\t\ts_nKNNgonX = 0, s_nKNNgonY = 0;",
    "static int\t\t\ts_nKNDichIdx = 0;\t// chi so NPC dang ngam",
    "static int\t\t\ts_nKNDichX = 0, s_nKNDichY = 0;\t// vi tri VE cua no",
    "",
    CU,
])
s = s.replace(CU, MOI)

# --- than ham: chen truoc JxCan_Ve (cuoi tep) ---
CU = "void JxIconNpc_Ve()"
assert s.count(CU) == 1, "khong tim thay JxIconNpc_Ve (%d)" % s.count(CU)

THAN = NL.join([
    "//---------------------------------------------------------------------------",
    "// [ANDROID 09/09 KYNANG] NUT CHON KY NANG DANH",
    "//---------------------------------------------------------------------------",
    "",
    "// Doc lai danh sach ky nang danh trai. Khong doc moi khung: GDI nay quet ca cay",
    "// vo cong nen goi lien tuc la phi.",
    "static void KyNang_DocBang()",
    "{",
    "\tunsigned int uNay = (unsigned int)GetTickCount();",
    "",
    "\tif (g_pCoreShell == NULL)",
    "\t\treturn;",
    "\tif (s_uKNDocLuc && uNay - s_uKNDocLuc < KYNANG_LAM_MOI_MS)",
    "\t\treturn;",
    "\ts_uKNDocLuc = uNay;",
    "\tmemset(s_KNBang, 0, sizeof(s_KNBang));",
    "\ts_nKNCo1 = g_pCoreShell->GetGameData(GDI_LEFT_ENABLE_SKILLS, (KUPARAM)&s_KNBang, 0);",
    "\tif (s_nKNCo1 < 0)",
    "\t\ts_nKNCo1 = 0;",
    "\tif (s_nKNCo1 > KYNANG_DS_TOI_DA)",
    "\t\ts_nKNCo1 = KYNANG_DS_TOI_DA;",
    "}",
    "",
    "// Goc trai tren cua bang nut. Mac dinh: goc phai duoi, chua len tren thanh cong cu.",
    "static void KyNang_GocBang(int* px, int* py)",
    "{",
    "\tint nHang = (s_nKNSo + s_nKNCot - 1) / s_nKNCot;",
    "\tint nRong = s_nKNCot * s_nKNCo + (s_nKNCot - 1) * KYNANG_CACH;",
    "\tint nCao  = nHang * s_nKNCo + (nHang - 1) * KYNANG_CACH;",
    "",
    "\t// SCREEN_WIDTH / SCREEN_HEIGHT = co khung ve that (KSdlApp dat), dung nhu JxCan_TrongVung.",
    "\t*px = (s_nKNX >= 0) ? s_nKNX : (SCREEN_WIDTH - nRong - 12);",
    "\t*py = (s_nKNY >= 0) ? s_nKNY : (SCREEN_HEIGHT - nCao - 96);",
    "}",
    "",
    "static void KyNang_HinhNut(int i, int* px, int* py)",
    "{",
    "\tint nX0, nY0;",
    "",
    "\tKyNang_GocBang(&nX0, &nY0);",
    "\t*px = nX0 + (i % s_nKNCot) * (s_nKNCo + KYNANG_CACH);",
    "\t*py = nY0 + (i / s_nKNCot) * (s_nKNCo + KYNANG_CACH);",
    "}",
    "",
    "// So nut thuc su ve ra: khong nhieu hon so ky nang dang co.",
    "static int KyNang_SoNutHien()",
    "{",
    "\tint n = s_nKNSo;",
    "",
    "\tif (n > s_nKNCo1)",
    "\t\tn = s_nKNCo1;",
    "\treturn n;",
    "}",
    "",
    "int JxKyNang_TrungNut(int x, int y)",
    "{",
    "\tint i, nX, nY, n;",
    "",
    "\tDocCaiDat();",
    "\tif (!s_nKNBat)",
    "\t\treturn 0;",
    "\tKyNang_DocBang();",
    "\tn = KyNang_SoNutHien();",
    "\tfor (i = 0; i < n; i++)",
    "\t{",
    "\t\tKyNang_HinhNut(i, &nX, &nY);",
    "\t\tif (x >= nX && x < nX + s_nKNCo && y >= nY && y < nY + s_nKNCo)",
    "\t\t\treturn i + 1;",
    "\t}",
    "\treturn 0;",
    "}",
    "",
    "// Hoi Core: con dich hop nhat theo huong ngam (0,0 = khong ngam -> gan nhat).",
    "// Tra ve chi so NPC, 0 neu khong co con nao.",
    "static int KyNang_TimDich(int nHuongX, int nHuongY, int* pVeX, int* pVeY)",
    "{",
    "\tKUiTargetDetailInfo tt;",
    "",
    "\tif (g_pCoreShell == NULL)",
    "\t\treturn 0;",
    "\tmemset(&tt, 0, sizeof(tt));",
    "\ttt.nViTriVeX = nHuongX;",
    "\ttt.nViTriVeY = nHuongY;",
    "\tif (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&tt, 2))",
    "\t\treturn 0;",
    "\tif (pVeX) *pVeX = tt.nViTriVeX;",
    "\tif (pVeY) *pVeY = tt.nViTriVeY;",
    "\treturn tt.nChiSoNpc;",
    "}",
    "",
    "void JxKyNang_BatDau(int nNut, int x, int y)",
    "{",
    "\ts_nKNDangCam = nNut - 1;",
    "\ts_nKNNgonX = x;",
    "\ts_nKNNgonY = y;",
    "\ts_nKNDichIdx = 0;",
    "}",
    "",
    "void JxKyNang_Keo(int x, int y)",
    "{",
    "\tint nX, nY, dx, dy;",
    "",
    "\tif (s_nKNDangCam < 0)",
    "\t\treturn;",
    "\ts_nKNNgonX = x;",
    "\ts_nKNNgonY = y;",
    "\tKyNang_HinhNut(s_nKNDangCam, &nX, &nY);",
    "\tdx = x - (nX + s_nKNCo / 2);",
    "\tdy = y - (nY + s_nKNCo / 2);",
    "\t// keo chua du xa thi coi nhu chua ngam - tranh rung tay lam doi muc tieu",
    "\tif (dx * dx + dy * dy < 18 * 18)",
    "\t{",
    "\t\ts_nKNDichIdx = 0;",
    "\t\treturn;",
    "\t}",
    "\ts_nKNDichIdx = KyNang_TimDich(dx, dy, &s_nKNDichX, &s_nKNDichY);",
    "}",
    "",
    "bool JxKyNang_Nha()",
    "{",
    "\tint nChon = s_nKNDangCam;",
    "\tint nDich = s_nKNDichIdx;",
    "",
    "\ts_nKNDangCam = -1;",
    "\ts_nKNDichIdx = 0;",
    "\tif (nChon < 0 || nChon >= s_nKNCo1 || g_pCoreShell == NULL)",
    "\t\treturn false;",
    "\tif (s_KNBang[nChon].uGenre == CGOG_NOTHING)",
    "\t\treturn false;",
    "",
    "\t// Chon lam ky nang danh TRAI - y het ban PC (UiSkillTree.cpp:156), nho vay o",
    "\t// ky nang tren thanh trang thai cung doi theo va lan sau cham thang vao dich",
    "\t// la danh bang dung ky nang nay.",
    "\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNBang[nChon], 0);",
    "",
    "\t// Khong ngam duoc con nao trong huong keo thi danh con gan nhat.",
    "\tif (nDich == 0)",
    "\t\tnDich = KyNang_TimDich(0, 0, NULL, NULL);",
    "\tif (nDich == 0)",
    "\t\treturn true;\t\t// khong co dich: van coi la da xu ly (da doi ky nang)",
    "\tg_pCoreShell->LockSomeoneUseSkill(nDich, (int)s_KNBang[nChon].uId);",
    "\treturn true;",
    "}",
    "",
    "// Vong tron duoi chan con dang ngam - dung anh vong DICH nhu JxVongChon_Ve.",
    "static void KyNang_VeVongDich(int nVeX, int nVeY)",
    "{",
    "\tstatic KRUImage s_VongNgam;",
    "\tKRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };",
    "\tint nLuiX = 0, nLuiY = 0;",
    "",
    "\tif (s_nVongCoAnh < 0)",
    "\t\ts_nVongCoAnh = (CoAnh(s_szVongAnh) && CoAnh(s_szVongAnhDich)) ? 1 : 0;",
    "\tif (!s_nVongCoAnh)",
    "\t\treturn;",
    "\tif (s_VongNgam.szImage[0] == 0)",
    "\t{",
    "\t\tmemset(&s_VongNgam, 0, sizeof(s_VongNgam));",
    "\t\ts_VongNgam.nType = ISI_T_SPR;",
    "\t\ts_VongNgam.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;",
    "\t\ts_VongNgam.Color.Color_dw = 0xffffffff;",
    "\t\ts_VongNgam.nISPosition = IMAGE_IS_POSITION_INIT;",
    "\t\ts_VongNgam.nFrame = 0;",
    "\t\tstrncpy(s_VongNgam.szImage, s_szVongAnhDich, sizeof(s_VongNgam.szImage) - 1);",
    "\t}",
    "\tif (g_pRepresentShell->GetImageFrameParam(s_VongNgam.szImage, 0, &oOff, &oCo, s_VongNgam.nType)",
    "\t\t&& oCo.nX > 0)",
    "\t{",
    "\t\tnLuiX = oCo.nX / 2;",
    "\t\tnLuiY = oCo.nY / 2;",
    "\t}",
    "\ts_VongNgam.oPosition.nX = nVeX - nLuiX;",
    "\ts_VongNgam.oPosition.nY = nVeY - nLuiY;",
    "\t// FALSE = toa do THE GIOI",
    "\tg_pRepresentShell->DrawPrimitives(1, &s_VongNgam, RU_T_IMAGE, false);",
    "}",
    "",
    "void JxKyNang_Ve()",
    "{",
    "\tint i, n, nX, nY;",
    "",
    "\tDocCaiDat();",
    "\tif (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)",
    "\t\treturn;",
    "\tKyNang_DocBang();",
    "\tn = KyNang_SoNutHien();",
    "\tfor (i = 0; i < n; i++)",
    "\t{",
    "\t\tKyNang_HinhNut(i, &nX, &nY);",
    "\t\t// nen mo cho thay ranh o nut, dam hon khi dang giu",
    "\t\tOVuong(nX + s_nKNCo / 2, nY + s_nKNCo / 2, s_nKNCo / 2,",
    "\t\t\t(i == s_nKNDangCam) ? 0xB03A8A3A : 0x80202020);",
    "\t\tg_pCoreShell->DrawGameObj(s_KNBang[i].uGenre, s_KNBang[i].uId,",
    "\t\t\tnX, nY, s_nKNCo, s_nKNCo, 0);",
    "\t}",
    "",
    "\t// Dang ngam: vach chi huong tu nut toi ngon tay + vong tron duoi chan con dich.",
    "\tif (s_nKNDangCam >= 0)",
    "\t{",
    "\t\tKRULine oVach;",
    "",
    "\t\tKyNang_HinhNut(s_nKNDangCam, &nX, &nY);",
    "\t\toVach.oPosition.nX = nX + s_nKNCo / 2;",
    "\t\toVach.oPosition.nY = nY + s_nKNCo / 2;",
    "\t\toVach.oEndPos.nX   = s_nKNNgonX;",
    "\t\toVach.oEndPos.nY   = s_nKNNgonY;",
    "\t\toVach.Color.Color_dw = s_nKNDichIdx ? 0xFFFF6666 : 0xA0FFFFFF;",
    "\t\tg_pRepresentShell->DrawPrimitives(1, &oVach, RU_T_LINE, true);",
    "",
    "\t\tif (s_nKNDichIdx)",
    "\t\t\tKyNang_VeVongDich(s_nKNDichX, s_nKNDichY);",
    "\t}",
    "}",
    "",
    CU,
])
s = s.replace(CU, THAN)

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
