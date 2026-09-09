# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 LUAN]  -  chu (10/09 14:2x), chi mobile:
#   "O phu bo ky nang vong sang thi tu luan chuyen doi voi phai Nga My": ky nang hao quang/trang thai (bat len la co
#   vong sang quanh nguoi) - de 3 ky nang vong sang o o phu se TU DOI QUA LUAN PHIEN lien tuc + hieu ung vong tron
#   xoay tai cac o do (ban tham khao co san: CCRotateBy 360/giay tren o aura).
#   "cac nut nho lai va nam ben canh thong tin - khong can hien thi ten ky nang cua ban che".
#
# Lam:
#   1. Core GDI_KYNANG_MOBILE: nParam 1 = CHI HOI (khong xu len/xuong ngua) - de hoi "aura?" moi khung an toan.
#   2. Luan chuyen: moi [Cham] LuanChuyenMs (mac dinh 3000) doi sang vong sang ke tiep trong cac o phu (SetRightSkill
#      -> SetAuraSkill). Mot vong sang thi chi giu cho no bat.
#   3. BO SetRightSkill khi danh/buff tu o phu: KPlayer::SetRightSkill(ky nang thuong) goi SetAuraSkill(0) = TAT vong
#      sang moi lan danh! UseSkill/LockSomeoneUseSkill nhan ma ky nang truc tiep nen khong can.
#   4. Vong xoay 16 khung (vong_xoay.spr, lam_vong_xoay.py) ve tren o phu dang giu aura, ~1 vong/giay.
#   5. Bang 4 nut: nho (84x31), khong ten, khong nen; dat NGAY CANH khung thong tin ky nang (KMouseOver::JxLayKhung).
#   6. Go phu mot vong sang dang bat -> tat (dat ky nang phai = ky nang danh chinh -> SetAuraSkill(0)).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 LUAN]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    for nl in ("\r\n", "\n"):
        c = nl.join(cu)
        if s.count(c) == so:
            return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


# ============================================================ 1. Core: nParam 1 = chi hoi
P = "Sources/Core/Src/CoreShell.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, ["\t\tint nHan = pKN->GetHorseLimit();"], [
        "\t\t// %s nParam 1 = CHI HOI (bo dem aura, tam danh...), khong xu len/xuong ngua" % DAU,
        "\t\tint nHan = (nParam == 1) ? 0 : pKN->GetHorseLimit();",
    ], "nHan")
    ghi(P, s)
    print("da va:", P)

# ============================================================ 2. MouseHover.h: lay khung tip dang hien
P = "Sources/S3Client/Ui/Elem/MouseHover.h"
s = doc(P)
if "JxLayKhung" in s:
    print("da co JxLayKhung, bo qua:", P)
else:
    s = thay(s, ["\tvoid\tOnWndClosed(void* pWnd);"], [
        "\tvoid\tOnWndClosed(void* pWnd);",
        "\t//\t%s khung thong tin dang hien (de dat 4 nut gan ky nang ngay ben canh). 0 = khong hien." % DAU,
        "\tint\t\tJxLayKhung(int* pL, int* pT, int* pW, int* pH) const",
        "\t{",
        "\t\tif (!m_bShow) return 0;",
        "\t\tif (pL) *pL = m_nLeft;",
        "\t\tif (pT) *pT = m_nTop;",
        "\t\tif (pW) *pW = m_nWndWidth;",
        "\t\tif (pH) *pH = m_nWndHeight;",
        "\t\treturn 1;",
        "\t}",
    ], "MouseHover OnWndClosed")
    ghi(P, s)
    print("da va:", P)

# ============================================================ 3. JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 3a. include
s = thay(s, ['#include "../Ui/UiCase/UiSkills.h"'], [
    '#include "../Ui/UiCase/UiSkills.h"',
    '#include "../Ui/Elem/MouseHover.h"\t// %s dat 4 nut canh khung thong tin ky nang' % DAU,
], "include UiSkills")

# 3b. trang thai + hang so bang chon nho
s = thay(s, [
    "#define\tBC_RONG\t\t\t\t130",
    "#define\tBC_CAO_NUT\t\t\t46",
    "#define\tBC_CAO_TEN\t\t\t22",
    "#define\tBC_SO_NUT\t\t\t4",
    "#define\tBC_NUT_RONG\t\t\t118",
    "#define\tBC_NUT_CAO\t\t\t43",
], [
    "//\t%s nut nho (84x31), khong ten, khong nen, nam ngay canh khung thong tin ky nang" % DAU,
    "#define\tBC_RONG\t\t\t\t88",
    "#define\tBC_CAO_NUT\t\t\t35",
    "#define\tBC_CAO_TEN\t\t\t0",
    "#define\tBC_SO_NUT\t\t\t4",
    "#define\tBC_NUT_RONG\t\t\t84",
    "#define\tBC_NUT_CAO\t\t\t31",
], "BC_ hang so 67")
s = thay(s, ["static unsigned int\ts_uKNBaoLuc = 0;"], [
    "static unsigned int\ts_uKNBaoLuc = 0;",
    "//\t%s vong sang luan chuyen: 3 ky nang vong sang o o phu tu doi qua luan phien (chu, Nga My)" % DAU,
    "static int\t\t\ts_nKNLuanMs = 3000;\t\t\t// [Cham] LuanChuyenMs",
    "static unsigned int\ts_uKNLuanLuc = 0;",
    "static int\t\t\ts_nKNLuanK = 0;",
    "static unsigned int\ts_uKNAuraId[KYNANG_SO_PHU + 1];\t// bo dem: ma ky nang cua o -> co phai vong sang",
    "static int\t\t\ts_nKNAuraLa[KYNANG_SO_PHU + 1];",
    "static char\t\t\ts_szKNAnhXoay[128] = \"\\\\spr\\\\Ui3\\\\UiSkillControl\\\\vong_xoay.spr\";\t// 16 khung, lam_vong_xoay.py",
    "#define\tKYNANG_XOAY_KHUNG\t16",
], "khai bao s_uKNBaoLuc")

# 3c. DocCaiDat
s = thay(s, ["\ts_nKNNhip = GetPrivateProfileInt(\"Cham\", \"KyNangNhip\", 200, szCfg);"], [
    "\ts_nKNNhip = GetPrivateProfileInt(\"Cham\", \"KyNangNhip\", 200, szCfg);",
    "\ts_nKNLuanMs = GetPrivateProfileInt(\"Cham\", \"LuanChuyenMs\", 3000, szCfg);\t// %s" % DAU,
    "\tif (s_nKNLuanMs < 500) s_nKNLuanMs = 500;",
    "\tGetPrivateProfileString(\"Cham\", \"KyNangAnhXoay\", s_szKNAnhXoay, s_szKNAnhXoay, sizeof(s_szKNAnhXoay), szCfg);",
], "DocCaiDat KyNangNhip")

# 3d. HoiCore: bChiHoi
s = thay(s, ["\tint* pCanDiem = NULL)"], ["\tint* pCanDiem = NULL, int bChiHoi = 0)"], "HoiCore chu ky 67")
s = thay(s, ["\tif (!g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oHoi, 0))"], [
    "\tif (!g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oHoi, bChiHoi ? 1 : 0))\t// %s" % DAU,
], "HoiCore GetGameData")
# hai cho hoi tam danh de ve (khong duoc xu ngua o day)
s = thay(s, ["GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oH, 0)"], ["GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oH, 1)"], "oH chi hoi", so=2)

# 3e. DanhMotPhat: bo SetRightSkill khi buff / danh tu o phu
s = thay(s, [
    "\t\tif (nNut > 0)",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
    "\t\tg_pCoreShell->UseSkill(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (int)o.uId);",
], [
    "\t\t// %s KHONG SetRightSkill nua: KPlayer::SetRightSkill(ky nang thuong) goi SetAuraSkill(0) = tat" % DAU,
    "\t\t// vong sang moi lan dung; UseSkill nhan ma ky nang truc tiep nen khong can.",
    "\t\tg_pCoreShell->UseSkill(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (int)o.uId);",
], "buff SetRightSkill")
s = thay(s, [
    "\t\t// NUT PHU: ban tham khao dat lam ky nang danh PHAI roi danh THEO DIEM",
    "\t\t// (KgameWorld.cpp:4437 SetRightSkill + UseSkill). Nho the nut phu va nut chinh",
    "\t\t// khong dam chan nhau: nut chinh van giu ky nang danh trai.",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);",
], [
    "\t\t// NUT PHU: truoc day dat lam ky nang danh PHAI roi danh theo diem (nhu ban tham khao).",
    "\t\t// %s BO buoc SetRightSkill: KPlayer::SetRightSkill(ky nang thuong) goi SetAuraSkill(0)" % DAU,
    "\t\t// = TAT vong sang moi lan danh. UseSkill / LockSomeoneUseSkill nhan ma ky nang truc tiep",
    "\t\t// (SetActiveSkill theo ma) nen nut phu van khong dam chan nut chinh.",
], "aux SetRightSkill")

# 3f. bo dem aura + luan chuyen, dat truoc JxKyNang_Nhip
s = thay(s, ["void JxKyNang_Nhip()", "{", "\tunsigned int uNay;", "", "\tif (s_nKNDangCam < 0)", "\t\treturn;"], [
    "//\t%s o phu i (1..8) dang giu VONG SANG (aura)? Bo dem theo ma ky nang; hoi Core kieu \"chi hoi\"." % DAU,
    "static int KyNang_OLaAura(int i, KUiGameObject* pRa)",
    "{",
    "\tif (i < 1 || i > KYNANG_SO_PHU || !KyNang_CuaNut(i, pRa))",
    "\t\treturn 0;",
    "\tif (s_uKNAuraId[i] != pRa->uId)",
    "\t{",
    "\t\tint nAura = 0;",
    "",
    "\t\tKyNang_HoiCore((int)pRa->uId, NULL, &nAura, NULL, NULL, NULL, 1);",
    "\t\ts_uKNAuraId[i] = pRa->uId;",
    "\t\ts_nKNAuraLa[i] = nAura;",
    "\t}",
    "\treturn s_nKNAuraLa[i];",
    "}",
    "",
    "//\t%s Vong sang luan chuyen: cac o phu dang giu vong sang thay nhau bat, moi s_nKNLuanMs." % DAU,
    "//\tMot vong sang: chi giu cho no bat (bi tat thi bat lai). Chu: \"3 ky nang vong sang o o phu se tu doi",
    "//\tqua luan phien lien tuc\" (Nga My). Ban tham khao: AUTO goi lai nut vong sang lien tuc.",
    "static void KyNang_LuanChuyen()",
    "{",
    "\tKUiGameObject aAura[KYNANG_SO_PHU];",
    "\tKUiGameObject oMuon;",
    "\tKUiPlayerImmedItemSkill oTay;",
    "\tunsigned int uNay;",
    "\tint i, n = 0;",
    "",
    "\tif (g_pCoreShell == NULL)",
    "\t\treturn;",
    "\tfor (i = 1; i <= KYNANG_SO_PHU; i++)",
    "\t{",
    "\t\tKUiGameObject o;",
    "",
    "\t\tif (KyNang_OLaAura(i, &o))",
    "\t\t\taAura[n++] = o;",
    "\t}",
    "\tif (n == 0)",
    "\t\treturn;",
    "\tuNay = (unsigned int)GetTickCount();",
    "\tif (s_uKNLuanLuc && uNay - s_uKNLuanLuc < (unsigned int)s_nKNLuanMs)",
    "\t\treturn;",
    "\ts_uKNLuanLuc = uNay;",
    "\toMuon = aAura[s_nKNLuanK % n];",
    "\ts_nKNLuanK++;",
    "\tmemset(&oTay, 0, sizeof(oTay));",
    "\tg_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (KNPARAM)&oTay, 0);",
    "\tif (n == 1 && oTay.IMmediaSkill[1].uId == oMuon.uId)",
    "\t\treturn;\t\t// mot vong sang va dang bat dung no",
    "\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&oMuon, 1);\t// SetRightSkill -> SetAuraSkill",
    "\tg_DebugLog(\"[KYNANG] luan chuyen vong sang -> %u (%d/%d)\", oMuon.uId, (s_nKNLuanK - 1) % n + 1, n);",
    "}",
    "",
    "void JxKyNang_Nhip()",
    "{",
    "\tunsigned int uNay;",
    "",
    "\tKyNang_LuanChuyen();\t// %s chay moi khung, khong phu thuoc dang giu nut hay khong" % DAU,
    "\tif (s_nKNDangCam < 0)",
    "\t\treturn;",
], "JxKyNang_Nhip dau")

# 3g. Ve: vong xoay tren o phu dang giu aura
s = thay(s, [
    "\t\tif (bCo)",
    "\t\t{",
    "\t\t\tnIcon = s_nKNCoIcon;",
    "\t\t\tg_pCoreShell->DrawGameObj(o.uGenre, o.uId,",
    "\t\t\t\tnX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);",
    "\t\t}",
], [
    "\t\tif (bCo)",
    "\t\t{",
    "\t\t\tnIcon = s_nKNCoIcon;",
    "\t\t\tg_pCoreShell->DrawGameObj(o.uGenre, o.uId,",
    "\t\t\t\tnX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);",
    "\t\t}",
    "\t\t// %s o phu giu VONG SANG: vong xoay (16 khung, ~1 vong/giay) nhu ban tham khao (CCRotateBy 360/1s)" % DAU,
    "\t\tif (bCo && i > 0 && KyNang_OLaAura(i, &o) && CoAnh(s_szKNAnhXoay))",
    "\t\t\tVeAnhCo(s_szKNAnhXoay, nX, nY, nR * 2 + 12, (int)(((unsigned int)GetTickCount() / 62) % KYNANG_XOAY_KHUNG));",
], "Ve vong xoay")

# 3h. Go phu: vong sang dang bat thi tat
s = thay(s, [
    "\tif (nGo)",
    "\t{",
    "\t\tKyNang_GhiGan();",
], [
    "\tif (nGo)",
    "\t{",
    "\t\tint nAura = 0;",
    "",
    "\t\tKyNang_GhiGan();",
    "\t\tmemset(s_uKNAuraId, 0, sizeof(s_uKNAuraId));\t// %s bo dem aura cua cac o phai tinh lai" % DAU,
    "\t\t// go mot vong sang -> tat no: dat ky nang phai = ky nang danh chinh (SetRightSkill -> SetAuraSkill(0))",
    "\t\tKyNang_HoiCore((int)p->uId, NULL, &nAura, NULL, NULL, NULL, 1);",
    "\t\tif (nAura && s_KNChinh.uId && g_pCoreShell)",
    "\t\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNChinh, 1);",
], "GoKhoiO nGo")

# 3i. VeBangChon: nut nho canh khung thong tin, khong ten / nen
i0 = s.index("static void KyNang_VeBangChon()")
i1 = s.index("void JxKyNang_Ve()")
nl = "\r\n" if "\r\n" in s[i0:i1] else "\n"
VE = [
    "static void KyNang_VeBangChon()",
    "{",
    "\tint i, nY, nL, nT, nW, nH;",
    "",
    "\tif (!s_nBCBat || g_pRepresentShell == NULL)",
    "\t\treturn;",
    "\t// cua so ky nang dong roi thi bang cung dong",
    "\tif (KUiSkillsNew::GetIfVisible() == NULL && KUiSkills::GetIfVisible() == NULL)",
    "\t{",
    "\t\ts_nBCBat = 0;",
    "\t\treturn;",
    "\t}",
    "\t// %s dat NGAY CANH khung thong tin ky nang dang hien (ben phai; het cho thi ben trai)" % DAU,
    "\tif (g_MouseOver.JxLayKhung(&nL, &nT, &nW, &nH))",
    "\t{",
    "\t\ts_nBCX = nL + nW + 4;",
    "\t\tif (s_nBCX + BC_RONG > SCREEN_WIDTH - 2)",
    "\t\t\ts_nBCX = nL - 4 - BC_RONG;",
    "\t\ts_nBCY = nT;",
    "\t\tif (s_nBCX < 2)\t\t\t\t\t\t\t\t\t\t\ts_nBCX = 2;",
    "\t\tif (s_nBCY + BC_CAO_NUT * BC_SO_NUT > SCREEN_HEIGHT - 2)\ts_nBCY = SCREEN_HEIGHT - 2 - BC_CAO_NUT * BC_SO_NUT;",
    "\t\tif (s_nBCY < 2)\t\t\t\t\t\t\t\t\t\t\ts_nBCY = 2;",
    "\t}",
    "\tnY = s_nBCY + BC_CAO_TEN;",
    "\t// 4 nut anh cua VNKU (khung 0 = binh thuong), 236x86 -> 84x31",
    "\tfor (i = 0; i < BC_SO_NUT; i++, nY += BC_CAO_NUT)",
    "\t{",
    "\t\tif (CoAnh(s_szBCAnh[i]))",
    "\t\t\tVeAnhKhung(s_szBCAnh[i], s_nBCX + BC_RONG / 2, nY + BC_CAO_NUT / 2, BC_NUT_RONG, BC_NUT_CAO, 0);",
    "\t\telse",
    "\t\t\tHopChuNhat(s_nBCX + 2, nY + 2, s_nBCX + BC_RONG - 2, nY + BC_CAO_NUT - 2, 0xC0304868);",
    "\t}",
    "}",
    "",
]
s = s[:i0] + nl.join(VE) + s[i1:]
ghi(P, s)
print("da va:", P)
