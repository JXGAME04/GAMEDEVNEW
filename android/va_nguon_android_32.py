# -*- coding: utf-8 -*-
#
# [UITOADO 09/09] THANH NUT CHAM cho che do sua giao dien - chay ca PC lan mobile.
#
# Chu: "lam them tinh nang chinh toa do icon trong game o ban mobile / chinh to nho
# icon - di chuyen toa do - xoa icon - lam ca 2 ban pc va mobile luon".
#
# Ma da co san UiToaDo (Ctrl+U) lam DU ca ba viec, nhung bam hoan toan vao chuot:
#     lan chuot   = to / nho          -> dien thoai KHONG CO
#     chuot phai  = doi ca khoi       -> dien thoai KHONG CO
#     chuot giua  = an / hien lai     -> dien thoai KHONG CO
#     Ctrl+U / Ctrl+K = luu / xoa het -> dien thoai KHONG CO ban phim
# Nen tren mobile chi con moi keo trai = doi cho mot o.
#
# Them: mot THANH NUT ve ngay tren man hinh. Chon cong cu roi cham vao o can sua.
# Ban PC dung duoc y het (bam trai), va MOI phim chuot cu VAN GIU NGUYEN - thanh nut
# la duong THU HAI, khong thay the duong cu.
#
# Vao che do sua tren dien thoai: them nut "Sua GD" nho, chi hien khi config.ini co
# [Ui] SuaToaDo=1 (nhu truoc gio) va dang KHONG sua. Ban PC van bam Ctrl+U duoc.
#
# Vi tri nut mo lay tu config.ini [Ui] SuaToaDoNutX / SuaToaDoNutY (mac dinh: mep phai,
# giua man hinh), de chu doi cho neu no che mat thu gi.

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
H = "Sources/S3Client/Ui/Elem/UiToaDo.h"
DAU = "[UITOADO 09/09]"
NL = "\r\n"


def vn(s):
    """Chuoi Viet -> TCVN3, tra ve dang str latin-1 de ghep vao ma nguon."""
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# ---------------------------------------------------------------- 1. include
CU = '#include "../../../Represent/iRepresent/iRepresentShell.h"'
assert s.count(CU) == 1
s = s.replace(CU, CU + NL + '#include "../../../Represent/iRepresent/KRepresentUnit.h"')

# ---------------------------------------------------------------- 2. hang so + bien
CU = "static void DatThongBao(const char* pszChu);"
assert s.count(CU) == 1
KHOI_BIEN = NL.join([
    "",
    "//--------------------------------------------------------------------------",
    "//\t[UITOADO 09/09] Thanh nut cham",
    "//",
    "//\tDien thoai khong co lan chuot / chuot phai / chuot giua / ban phim, ma ba",
    "//\tthu do lai chinh la duong duy nhat de to nho, doi ca khoi, an va luu.",
    "//\tThanh nut nay lam duong THU HAI cho ca bon viec ay, chi can bam trai.",
    "//\tMoi phim chuot cu van giu nguyen cho ban PC.",
    "//--------------------------------------------------------------------------",
    "#define\tUITOADO_NUT_CAO\t\t34",
    "#define\tUITOADO_NUT_Y\t\t4",
    "#define\tUITOADO_NUT_CACH\t2",
    "#define\tUITOADO_MO_RONG\t\t92\t\t// nut \"mo che do sua\" luc chua sua",
    "#define\tUITOADO_MO_CAO\t\t30",
    "",
    "//\tCong cu dang chon: quyet dinh cham / keo vao mot o thi lam gi",
    "enum",
    "{",
    "\tCONGCU_DOI = 0,\t\t// keo = doi cho MOT o   (= keo trai cu)",
    "\tCONGCU_KHOI,\t\t// keo = doi cho CA KHOI (= keo phai cu)",
    "\tCONGCU_TO,\t\t\t// cham = to them mot nac (= lan chuot len)",
    "\tCONGCU_NHO,\t\t\t// cham = nho di mot nac  (= lan chuot xuong)",
    "\tCONGCU_AN,\t\t\t// cham = an / hien lai   (= bam chuot giua)",
    "\tCONGCU_SO",
    "};",
    "",
    "//\tMa lenh cua nut khong phai cong cu",
    "#define\tNUT_BANG\t\t100\t\t// bat / tat bang danh sach cua so",
    "#define\tNUT_LUU\t\t\t101\t\t// luu va thoat (= Ctrl+U)",
    "#define\tNUT_XOAHET\t\t102\t\t// xoa het (= Ctrl+K)",
    "#define\tUITOADO_SO_NUT\t8",
    "",
    "static const struct",
    "{",
    "\tconst char*\tpszTen;",
    "\tint\t\t\tnMa;",
    "} s_Nut[UITOADO_SO_NUT] =",
    "{",
    '\t{ "%s",\tCONGCU_DOI },' % vn(u"Dời ô"),
    '\t{ "%s",\tCONGCU_KHOI },' % vn(u"Dời khối"),
    '\t{ "%s",\t\tCONGCU_TO },' % vn(u"To hơn"),
    '\t{ "%s",\tCONGCU_NHO },' % vn(u"Nhỏ lại"),
    '\t{ "%s",\tCONGCU_AN },' % vn(u"Giấu/hiện"),
    '\t{ "%s",\tNUT_BANG },' % vn(u"Danh sách"),
    '\t{ "%s",\t\tNUT_LUU },' % vn(u"Lưu"),
    '\t{ "%s",\tNUT_XOAHET },' % vn(u"Xoá hết"),
    "};",
    "",
    "static int\t\t\ts_nCongCu\t\t= CONGCU_DOI;",
    "static bool\t\t\ts_bDaDocViTriNut = false;",
    "static int\t\t\ts_nMoX\t\t\t= -1;\t// -1 = tu tinh (mep phai, giua man hinh)",
    "static int\t\t\ts_nMoY\t\t\t= -1;",
    "",
    "extern int SCREEN_WIDTH;\t\t// S3Client.cpp / KSdlApp.cpp: co khung ve that",
    "extern int SCREEN_HEIGHT;",
    "",
])
s = s.replace(CU, KHOI_BIEN + NL + CU)

# ---------------------------------------------------------------- 3. ham ve + do trung
CU = "//--------------------------------------------------------------------------\r\n//\tNhan chuot khi dang sua\r\n//--------------------------------------------------------------------------"
assert s.count(CU) == 1, "khong tim thay moc truoc UiToaDo_NhanChuot (%d)" % s.count(CU)

KHOI_HAM = NL.join([
    "//--------------------------------------------------------------------------",
    "//\t[UITOADO 09/09] Thanh nut cham - hinh hoc, do trung, ve",
    "//--------------------------------------------------------------------------",
    "//\tKhoi mau dac. Dung KRUShadow y nhu JxCanDieuKhien.cpp - o day khong co san",
    "//\tnguyen thuy ve hinh nao khac ngoai OutputText.",
    "static void OKhoi(int x0, int y0, int x1, int y1, unsigned int uMau)",
    "{",
    "\tKRUShadow o;",
    "",
    "\tif (g_pRepresentShell == NULL)",
    "\t\treturn;",
    "\to.oPosition.nX = x0;",
    "\to.oPosition.nY = y0;",
    "\to.oEndPos.nX   = x1;",
    "\to.oEndPos.nY   = y1;",
    "\to.Color.Color_dw = uMau;",
    "\tg_pRepresentShell->DrawPrimitives(1, &o, RU_T_SHADOW, true);",
    "}",
    "",
    "//\tBe rong mot nut: chia deu ca hang theo be ngang man hinh, co chan tren duoi",
    "//\tde man hinh rat hep van bam duoc, man hinh rong khong bi nut dai ngoang.",
    "static int NutRong()",
    "{",
    "\tint nRong = (SCREEN_WIDTH > 0 ? SCREEN_WIDTH : 1024);",
    "",
    "\tnRong = (nRong - 2 * UITOADO_NUT_CACH) / UITOADO_SO_NUT - UITOADO_NUT_CACH;",
    "\tif (nRong < 56)\t\tnRong = 56;",
    "\tif (nRong > 124)\tnRong = 124;",
    "\treturn nRong;",
    "}",
    "",
    "static void NutHinh(int i, int* px, int* py, int* pw, int* ph)",
    "{",
    "\tint nRong = NutRong();",
    "",
    "\t*pw = nRong;",
    "\t*ph = UITOADO_NUT_CAO;",
    "\t*px = UITOADO_NUT_CACH + i * (nRong + UITOADO_NUT_CACH);",
    "\t*py = UITOADO_NUT_Y;",
    "}",
    "",
    "//\t-1 = khong trung nut nao",
    "static int NutTrung(int x, int y)",
    "{",
    "\tint i, nX, nY, nW, nH;",
    "",
    "\tfor (i = 0; i < UITOADO_SO_NUT; i++)",
    "\t{",
    "\t\tNutHinh(i, &nX, &nY, &nW, &nH);",
    "\t\tif (x >= nX && x < nX + nW && y >= nY && y < nY + nH)",
    "\t\t\treturn i;",
    "\t}",
    "\treturn -1;",
    "}",
    "",
    "//\tNut \"mo che do sua\" - chi co khi config cho phep va dang khong sua.",
    "//\tVi tri lay tu config.ini [Ui] SuaToaDoNutX / SuaToaDoNutY, de chu doi cho",
    "//\tneu no che mat thu gi tren man hinh cua minh.",
    "static void NutMoHinh(int* px, int* py, int* pw, int* ph)",
    "{",
    "\tif (s_bDaDocViTriNut == false)",
    "\t{",
    "\t\tchar szDuongDan[MAX_PATH];",
    "",
    "\t\ts_bDaDocViTriNut = true;",
    "\t\tg_GetFullPath(szDuongDan, (char*)UITOADO_CAUHINH);",
    '\t\ts_nMoX = GetPrivateProfileInt("Ui", "SuaToaDoNutX", -1, szDuongDan);',
    '\t\ts_nMoY = GetPrivateProfileInt("Ui", "SuaToaDoNutY", -1, szDuongDan);',
    "\t}",
    "\t*pw = UITOADO_MO_RONG;",
    "\t*ph = UITOADO_MO_CAO;",
    "\t*px = (s_nMoX >= 0) ? s_nMoX",
    "\t\t: (SCREEN_WIDTH > 0 ? SCREEN_WIDTH : 1024) - UITOADO_MO_RONG - 6;",
    "\t*py = (s_nMoY >= 0) ? s_nMoY",
    "\t\t: (SCREEN_HEIGHT > 0 ? SCREEN_HEIGHT : 768) / 2;",
    "}",
    "",
    "static bool TrongNutMo(int x, int y)",
    "{",
    "\tint nX, nY, nW, nH;",
    "",
    "\tif (s_bDangSua || UiToaDo_ChoPhep() == false)",
    "\t\treturn false;",
    "\tNutMoHinh(&nX, &nY, &nW, &nH);",
    "\treturn (x >= nX && x < nX + nW && y >= nY && y < nY + nH);",
    "}",
    "",
    CU,
])
s = s.replace(CU, KHOI_HAM)

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va (phan 1: hang so + ham ve):", P)
