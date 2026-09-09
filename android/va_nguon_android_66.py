# -*- coding: utf-8 -*-
#
# [ANDROID 10/09 NUTVNKU] + [ANDROID 10/09 KHINHCONG]  -  chu (10/09 14:xx):
#   "cac nut khi kich vao ky nang co san o VNKU roi ban them vao thoi - lam gi che them vay nhin khong ro chu"
#      -> bang chon dung dung 4 nut anh cua VNKU (UiNew\UiSkills\nut_phim_chinh / nut_phim_phu /
#         nut_go_phim_chinh / nut_go_phim_phu, 236x86, ve thu nua = 118x43, RU_T_IMAGE_STRETCH), bo chu tu ve.
#   "ky nang khinh cong bo phim phu su dung khong duoc"
#      -> Khinh cong (210): TargetEnemy/Self/Ally deu 0, AttackRadius 400 = ky nang can DIEM. Cham (khong ngam)
#         truoc day di tim ke dich -> khong co -> khong lam gi. Nay Core tra nCanDiem; khong ngam thi tha ve
#         phia dang di / quay mat (s_nHuong cua can dieu khien) cach nhan vat min(tam, 360) - giong ban tham
#         khao (UseSkill theo huong joystick khi dang chay).

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
DAU = "[ANDROID 10/09 NUTVNKU]"


def vn(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten):
    for nl in ("\r\n", "\n"):
        c = nl.join(cu)
        if s.count(c) == 1:
            return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung 1 cho: %s" % ten)


# ============================================================ Core
P = "Sources/Core/Src/GameDataDef.h"
s = doc(P)
if "nCanDiem" in s:
    print("da co nCanDiem, bo qua:", P)
else:
    s = thay(s, ["\tchar szTen[64];\t// RA  : ten ky nang (ghi tren bang chon)", "};"], [
        "\tchar szTen[64];\t// RA  : ten ky nang (ghi tren bang chon)",
        "\tint\tnCanDiem;\t// RA  : 1 = ky nang can mot DIEM (khinh cong...): khong nham ai ca (khong dich/minh/dong doi)",
        "};",
    ], "struct nCanDiem")
    ghi(P, s)
    print("da va:", P)

P = "Sources/Core/Src/CoreShell.cpp"
s = doc(P)
if "nCanDiem" in s:
    print("da co nCanDiem, bo qua:", P)
else:
    s = thay(s, ["\t\tpHoi->szTen[0] = 0;"], ["\t\tpHoi->szTen[0] = 0;", "\t\tpHoi->nCanDiem = 0;\t// [ANDROID 10/09 KHINHCONG]"], "khoi tao nCanDiem")
    s = thay(s, ["\t\tif (pKN->GetSkillName())"], [
        "\t\t// [ANDROID 10/09 KHINHCONG] ky nang khong nham AI (khinh cong 210, dat bay...): can mot diem tren dat",
        "\t\tpHoi->nCanDiem = (!pKN->IsAura() && !pKN->IsTargetEnemy() && !pKN->IsTargetSelf() && !pKN->IsTargetAlly()) ? 1 : 0;",
        "\t\tif (pKN->GetSkillName())",
    ], "tinh nCanDiem")
    ghi(P, s)
    print("da va:", P)

# ============================================================ JxCanDieuKhien.cpp
P = "Sources/S3Client/Platform/JxCanDieuKhien.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# 1. kich thuoc bang + anh 4 nut VNKU
s = thay(s, [
    "#define\tBC_RONG\t\t\t\t176",
    "#define\tBC_CAO_NUT\t\t\t34",
    "#define\tBC_CAO_TEN\t\t\t22",
], [
    "//\t%s 4 nut anh cua VNKU (236x86, ve thu nua) thay cho chu tu ve - chu: \"nhin khong ro chu\"" % DAU,
    "#define\tBC_RONG\t\t\t\t130",
    "#define\tBC_CAO_NUT\t\t\t46",
    "#define\tBC_CAO_TEN\t\t\t22",
    "#define\tBC_SO_NUT\t\t\t4",
    "#define\tBC_NUT_RONG\t\t\t118",
    "#define\tBC_NUT_CAO\t\t\t43",
    "static const char* s_szBCAnh[BC_SO_NUT] =",
    "{",
    "\t\"\\\\Spr\\\\UiNew\\\\UiSkills\\\\nut_phim_chinh.spr\",",
    "\t\"\\\\Spr\\\\UiNew\\\\UiSkills\\\\nut_phim_phu.spr\",",
    "\t\"\\\\Spr\\\\UiNew\\\\UiSkills\\\\nut_go_phim_chinh.spr\",",
    "\t\"\\\\Spr\\\\UiNew\\\\UiSkills\\\\nut_go_phim_phu.spr\",",
    "};",
], "BC_ hang so")

# 2. HoiCore: tra them nCanDiem
s = thay(s, [
    "static bool KyNang_HoiCore(int nSkillId, int* pTamDanh, int* pLaAura, int* pTuDung = NULL, char* pszTen = NULL)",
], [
    "static bool KyNang_HoiCore(int nSkillId, int* pTamDanh, int* pLaAura, int* pTuDung = NULL, char* pszTen = NULL,",
    "\tint* pCanDiem = NULL)",
], "HoiCore chu ky")
s = thay(s, ["\tif (pszTen)   pszTen[0] = 0;"], ["\tif (pszTen)   pszTen[0] = 0;", "\tif (pCanDiem) *pCanDiem = 0;\t// [ANDROID 10/09 KHINHCONG]"], "HoiCore khoi tao")
s = thay(s, ["\tif (pTuDung)  *pTuDung  = oHoi.nTuDung;\t// [ANDROID 10/09 BUFF]"], [
    "\tif (pTuDung)  *pTuDung  = oHoi.nTuDung;\t// [ANDROID 10/09 BUFF]",
    "\tif (pCanDiem) *pCanDiem = oHoi.nCanDiem;\t// [ANDROID 10/09 KHINHCONG]",
], "HoiCore ra nCanDiem")

# 3. DanhMotPhat: ky nang can diem, khong ngam -> tha ve phia dang di
s = thay(s, [
    "\tint nTuDung = 0;",
    "",
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura, &nTuDung))",
    "\t\treturn;",
], [
    "\tint nTuDung = 0, nCanDiem = 0;",
    "",
    "\tif (!KyNang_HoiCore((int)o.uId, &nTam, &nAura, &nTuDung, NULL, &nCanDiem))",
    "\t\treturn;",
], "DanhMotPhat HoiCore 66")
s = thay(s, [
    "\tif (nNut > 0)",
    "\t{",
    "\t\t// NUT PHU: ban tham khao dat lam ky nang danh PHAI roi danh THEO DIEM",
], [
    "\tif (nCanDiem && !bCoNgam)",
    "\t{",
    "\t\t// [ANDROID 10/09 KHINHCONG] ky nang can mot DIEM (khinh cong 210: khong nham dich/minh/dong doi):",
    "\t\t// cham khong ngam thi tha ve PHIA DANG DI / QUAY MAT (s_nHuong cua can dieu khien, 0..63,",
    "\t\t// 0 = xuong, 16 = trai, 32 = len, 48 = phai) cach nhan vat min(tam, 360) - nhu ban tham khao",
    "\t\t// (UseSkill theo huong joystick khi dang chay). Truoc day di tim ke dich -> khong co -> bo qua.",
    "\t\tint nXa = (nTam > 0) ? nTam : 300;",
    "\t\tdouble fGoc = (48.0 - (double)s_nHuong) * 5.625 * 3.14159265358979 / 180.0;",
    "",
    "\t\tif (nXa > 360) nXa = 360;",
    "\t\tif (nXa < 120) nXa = 120;",
    "\t\tnNgamX = SCREEN_WIDTH / 2 + (int)(cos(fGoc) * nXa);",
    "\t\tnNgamY = SCREEN_HEIGHT / 2 - (int)(sin(fGoc) * nXa);",
    "\t\tbCoNgam = 1;",
    "\t\tg_DebugLog(\"[KYNANG] ky nang can diem %u: tha ve huong %d -> (%d,%d)\", o.uId, s_nHuong, nNgamX, nNgamY);",
    "\t}",
    "\tif (nNut > 0)",
    "\t{",
    "\t\t// NUT PHU: ban tham khao dat lam ky nang danh PHAI roi danh THEO DIEM",
], "DanhMotPhat nut phu")

# 4. bang chon: 4 nut, cao theo BC_SO_NUT
s = s.replace("\tint nCao = BC_CAO_TEN + BC_CAO_NUT * 3;", "\tint nCao = BC_CAO_TEN + BC_CAO_NUT * BC_SO_NUT;")
assert s.count("BC_CAO_NUT * BC_SO_NUT") == 2, "so cho nCao khong dung 2"

# 4a. go phim chinh: ve ky nang danh mac dinh (ky nang dau danh sach danh trai)
s = thay(s, ["int JxKyNang_ChamBangChon(int x, int y)", "{"], [
    "//\t%s \"Go phim chinh\": tra o chinh ve ky nang dau danh sach danh trai (ky nang co ban)," % DAU,
    "//\tva quen \"Chinh=\" da nho. (Core luon can mot ky nang danh trai nen khong \"go\" thanh trong duoc.)",
    "static void KyNang_GoChinh()",
    "{",
    "\tKyNang_DocGan();",
    "\tmemset(&s_KNChinhGan, 0, sizeof(s_KNChinhGan));",
    "\tKyNang_GhiGan();",
    "\tif (g_pCoreShell && s_nKNCo1 > 0 && s_KNBang[0].uId && s_KNBang[0].uId != s_KNChinh.uId)",
    "\t{",
    "\t\tKUiGameObject o;",
    "",
    "\t\tmemset(&o, 0, sizeof(o));",
    "\t\to.uGenre = s_KNBang[0].uGenre;",
    "\t\to.uId    = s_KNBang[0].uId;",
    "\t\tg_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);",
    "\t}",
    "\tKyNang_Bao(\"%s\");" % vn(u"Ô chính về kỹ năng cơ bản"),
    "}",
    "",
    "int JxKyNang_ChamBangChon(int x, int y)",
    "{",
], "dau ChamBangChon")
s = thay(s, [
    "\telse",
    "\t\tKyNang_GoKhoiO(&o);",
    "\treturn 1;",
    "}",
], [
    "\telse if (nNut == 2)",
    "\t\tKyNang_GoChinh();\t\t// %s" % DAU,
    "\telse",
    "\t\tKyNang_GoKhoiO(&o);",
    "\treturn 1;",
    "}",
], "ChamBangChon go")

# 4b. ve bang: 4 nut anh VNKU
i0 = s.index("static void KyNang_VeBangChon()")
i1 = s.index("void JxKyNang_Ve()")
nl = "\r\n" if "\r\n" in s[i0:i1] else "\n"
VE = [
    "static void KyNang_VeBangChon()",
    "{",
    "\tint i, nY;",
    "",
    "\tif (!s_nBCBat || g_pRepresentShell == NULL)",
    "\t\treturn;",
    "\t// cua so ky nang dong roi thi bang cung dong",
    "\tif (KUiSkillsNew::GetIfVisible() == NULL && KUiSkills::GetIfVisible() == NULL)",
    "\t{",
    "\t\ts_nBCBat = 0;",
    "\t\treturn;",
    "\t}",
    "\tHopChuNhat(s_nBCX, s_nBCY, s_nBCX + BC_RONG, s_nBCY + BC_CAO_TEN + BC_CAO_NUT * BC_SO_NUT, 0xD0101820);",
    "\tKyNang_VeChu(s_szBCTen[0] ? s_szBCTen : \"%s\", s_nBCX + 8, s_nBCY + 4, 0xFFFFD24A);" % vn(u"Kỹ năng"),
    "\tnY = s_nBCY + BC_CAO_TEN;",
    "\t// %s 4 nut anh cua VNKU (khung 0 = binh thuong), ve thu nua co goc (236x86 -> 118x43)" % DAU,
    "\tfor (i = 0; i < BC_SO_NUT; i++, nY += BC_CAO_NUT)",
    "\t{",
    "\t\tif (CoAnh(s_szBCAnh[i]))",
    "\t\t\tVeAnhKhung(s_szBCAnh[i], s_nBCX + BC_RONG / 2, nY + BC_CAO_NUT / 2, BC_NUT_RONG, BC_NUT_CAO, 0);",
    "\t\telse",
    "\t\t\tHopChuNhat(s_nBCX + 4, nY + 3, s_nBCX + BC_RONG - 4, nY + BC_CAO_NUT - 3, 0xC0304868);",
    "\t}",
    "}",
    "",
]
s = s[:i0] + nl.join(VE) + s[i1:]
ghi(P, s)
print("da va:", P)
