# -*- coding: utf-8 -*-
#
# [UITOADO 09/09 B] Noi thanh nut cham vao mach nhan chuot va mach ve.
#
# Phan 1 (ban va 32) chi dung hinh hoc + ham ve. Phan nay:
#   - UiToaDo_NhanChuot: nut "mo che do sua" (luc chua sua), thanh nut, va cong cu
#     dang chon quyet dinh cham/keo vao mot o thi lam gi;
#   - UiToaDo_Ve: ve thanh nut, ve nut mo, va doi dong huong dan cho khoi de len nhau.
#
# Nguyen tac: KHONG dong den duong chuot cu. Lan chuot / chuot phai / chuot giua /
# Ctrl+U / Ctrl+K cua ban PC van y nguyen; thanh nut chi them duong bam TRAI.

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")

P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
DAU = "[UITOADO 09/09 B]"
NL = "\r\n"


def vn(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# ============================================================ 1. UiToaDo_NhanChuot
CU = NL.join([
    "bool UiToaDo_NhanChuot(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)",
    "{",
    "\tint\t\tx, y;",
    "\tchar\tszKhoa[UITOADO_CO_KHOA];",
    "\tchar\tszChu[192];",
    "",
    "\tif (s_bDangSua == false)",
    "\t\treturn false;",
])
assert s.count(CU) == 1, "khong tim thay dau UiToaDo_NhanChuot (%d)" % s.count(CU)

MOI = NL.join([
    "//\t[UITOADO 09/09 B] Cham vao mot o khi dang chon cong cu to / nho / giau.",
    "//\tLam dung viec ma lan chuot va chuot giua van lam, chi khac duong vao.",
    "static void CongCuChamO(int x, int y)",
    "{",
    "\tchar\t\tszKhoa[UITOADO_CO_KHOA];",
    "\tchar\t\tszChu[192];",
    "\tKWndWindow*\tpO = TimODuoiChuot(x, y, false, szKhoa, sizeof(szKhoa));",
    "",
    "\tif (pO == NULL)",
    "\t{",
    '\t\tDatThongBao("%s");' % vn(u"Ô này không đặt tên được (không nạp từ ini)"),
    "\t\treturn;",
    "\t}",
    "",
    "\tif (s_nCongCu == CONGCU_AN)",
    "\t{",
    "\t\tif (pO->UiDangAn() == 0 && CamAn(pO))",
    "\t\t{",
    '\t\t\tDatThongBao("%s");' % vn(u"Không cho giấu nút đóng / huỷ, giấu xong sẽ kẹt cửa sổ"),
    "\t\t\treturn;",
    "\t\t}",
    "\t\tpO->UiDatAn(pO->UiDangAn() ? 0 : 1);",
    "\t\tGhiLaiO(pO, szKhoa);",
    '\t\t_snprintf(szChu, sizeof(szChu), "%s  %s", szKhoa,',
    '\t\t\tpO->UiDangAn() ? "%s" : "%s");' % (vn(u"đã giấu"), vn(u"hiện lại")),
    "\t\tszChu[sizeof(szChu) - 1] = 0;",
    "\t\tDatThongBao(szChu);",
    "\t\treturn;",
    "\t}",
    "",
    "\t//\tCONGCU_TO / CONGCU_NHO",
    "\tif (CamCoGian(pO))",
    "\t{",
    '\t\tDatThongBao("%s");' % vn(u"Ô này không co giãn được (ô danh sách / thanh cuộn)"),
    "\t\treturn;",
    "\t}",
    "\tpO->UiDatTiLe(pO->UiLayTiLe()",
    "\t\t+ ((s_nCongCu == CONGCU_TO) ? UITOADO_TILE_NAC : -UITOADO_TILE_NAC));",
    "\tGhiLaiO(pO, szKhoa);",
    '\t_snprintf(szChu, sizeof(szChu), "%s  %d%%", szKhoa, pO->UiLayTiLe() / 10);',
    "\tszChu[sizeof(szChu) - 1] = 0;",
    "\tDatThongBao(szChu);",
    "}",
    "",
    "//\t[UITOADO 09/09 B] Bam mot nut tren thanh. Tra ve true = da xu ly.",
    "static void BamNutThanh(int i)",
    "{",
    "\tint nMa = s_Nut[i].nMa;",
    "\tchar szChu[192];",
    "",
    "\tif (nMa < CONGCU_SO)",
    "\t{",
    "\t\ts_nCongCu = nMa;",
    '\t\t_snprintf(szChu, sizeof(szChu), "%s %%s", s_Nut[i].pszTen);'
    % vn(u"Công cụ:"),
    "\t\tszChu[sizeof(szChu) - 1] = 0;",
    "\t\tDatThongBao(szChu);",
    "\t\treturn;",
    "\t}",
    "\tswitch (nMa)",
    "\t{",
    "\tcase NUT_BANG:",
    "\t\ts_bHienBang = !s_bHienBang;",
    "\t\tif (s_bHienBang)",
    "\t\t\tDungBang();",
    "\t\tbreak;",
    "\tcase NUT_LUU:",
    "\t\tUiToaDo_BatTat();\t\t// dang sua -> tat = tu dong ghi tep",
    "\t\tbreak;",
    "\tcase NUT_XOAHET:",
    "\t\tUiToaDo_XoaHet();",
    "\t\tbreak;",
    "\t}",
    "}",
    "",
    CU.replace(
        NL.join([
            "\tif (s_bDangSua == false)",
            "\t\treturn false;",
        ]),
        NL.join([
            "\t//\t[UITOADO 09/09 B] Chua sua: chi bat cu bam vao nut \"mo che do sua\".",
            "\t//\tDien thoai khong co ban phim nen khong bam Ctrl+U duoc.",
            "\tif (s_bDangSua == false)",
            "\t{",
            "\t\tif (uMsg != WM_LBUTTONDOWN)",
            "\t\t\treturn false;",
            "\t\tWnd_GetCursorPos(&x, &y);",
            "\t\tif (TrongNutMo(x, y) == false)",
            "\t\t\treturn false;",
            "\t\tUiToaDo_BatTat();",
            "\t\treturn true;",
            "\t}",
        ]),
    ),
])
s = s.replace(CU, MOI)

# ---- chen xu ly thanh nut + cong cu vao nhanh WM_LBUTTONDOWN ----
CU2 = NL.join([
    "\tcase WM_LBUTTONDOWN:",
    "\tcase WM_RBUTTONDOWN:",
    "\t\t{",
    "\t\t\tKWndWindow*\tpO;",
    "",
    "\t\t\ts_pKeo = NULL;",
    "\t\t\ts_szKhoaKeo[0] = 0;",
    "",
])
assert s.count(CU2) == 1, "khong tim thay nhanh LBUTTONDOWN (%d)" % s.count(CU2)
MOI2 = NL.join([
    "\tcase WM_LBUTTONDOWN:",
    "\tcase WM_RBUTTONDOWN:",
    "\t\t{",
    "\t\t\tKWndWindow*\tpO;",
    "\t\t\tint\t\t\tnNut;",
    "",
    "\t\t\ts_pKeo = NULL;",
    "\t\t\ts_szKhoaKeo[0] = 0;",
    "",
    "\t\t\t//\t[UITOADO 09/09 B] thanh nut an tren het",
    "\t\t\tnNut = (uMsg == WM_LBUTTONDOWN) ? NutTrung(x, y) : -1;",
    "\t\t\tif (nNut >= 0)",
    "\t\t\t{",
    "\t\t\t\tBamNutThanh(nNut);",
    "\t\t\t\tbreak;",
    "\t\t\t}",
    "",
    "\t\t\t//\t[UITOADO 09/09 B] cong cu to / nho / giau: cham la lam ngay,",
    "\t\t\t//\tkhong bat dau keo.",
    "\t\t\tif (uMsg == WM_LBUTTONDOWN && s_nCongCu >= CONGCU_TO",
    "\t\t\t\t&& TrongBang(x, y) == false)",
    "\t\t\t{",
    "\t\t\t\tCongCuChamO(x, y);",
    "\t\t\t\tbreak;",
    "\t\t\t}",
    "",
])
s = s.replace(CU2, MOI2)

# ---- cong cu "doi ca khoi" cho bam TRAI ----
CU3 = "\t\t\tpO = TimODuoiChuot(x, y, uMsg == WM_RBUTTONDOWN, szKhoa, sizeof(szKhoa));"
assert s.count(CU3) == 1, "khong tim thay dong TimODuoiChuot cua nhanh keo (%d)" % s.count(CU3)
MOI3 = NL.join([
    "\t\t\t//\t[UITOADO 09/09 B] bam trai + cong cu \"Doi khoi\" = y nhu bam phai",
    "\t\t\tpO = TimODuoiChuot(x, y,",
    "\t\t\t\tuMsg == WM_RBUTTONDOWN || s_nCongCu == CONGCU_KHOI,",
    "\t\t\t\tszKhoa, sizeof(szKhoa));",
])
s = s.replace(CU3, MOI3)

# ============================================================ 2. UiToaDo_Ve
CU4 = NL.join([
    "\tif (s_bDangSua)",
    "\t{",
    '\t\tVeChu("%s", 8, 8, 0xFFFFD24A);' % vn(u"Sửa giao diện: kéo trái = một ô, kéo phải = cả khối, lăn = to nhỏ, bấm giữa = xoá / hiện lại"),
    '\t\tVeChu("%s", 8, 24, 0xFFFFD24A);' % vn(u"Bấm giữa lên chỗ trống = bảng danh sách cửa sổ. Ctrl+U = lưu và thoát. Ctrl+K = xoá hết"),
])
assert s.count(CU4) == 1, "khong tim thay hai dong huong dan (%d)" % s.count(CU4)
MOI4 = NL.join([
    "\tif (s_bDangSua)",
    "\t{",
    "\t\tint i, nX, nY, nW, nH;",
    "",
    "\t\t//\t[UITOADO 09/09 B] thanh nut cham - ve truoc, chu huong dan tut xuong duoi",
    "\t\tfor (i = 0; i < UITOADO_SO_NUT; i++)",
    "\t\t{",
    "\t\t\tbool bDangChon = (s_Nut[i].nMa < CONGCU_SO && s_Nut[i].nMa == s_nCongCu);",
    "",
    "\t\t\tNutHinh(i, &nX, &nY, &nW, &nH);",
    "\t\t\tOKhoi(nX, nY, nX + nW, nY + nH,",
    "\t\t\t\tbDangChon ? 0xC02A6E2A : 0xC0202020);",
    "\t\t\tVeChu(s_Nut[i].pszTen, nX + 5, nY + 10,",
    "\t\t\t\tbDangChon ? 0xFFAAFFAA : 0xFFDDDDDD);",
    "\t\t}",
    "",
    '\t\tVeChu("%s", 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 4, 0xFFFFD24A);'
    % vn(u"Chọn công cụ ở trên rồi chạm vào ô cần sửa. Kéo = dời chỗ."),
    '\t\tVeChu("%s", 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 18, 0xFFFFD24A);'
    % vn(u"Chuột: kéo phải = cả khối, lăn = to nhỏ, bấm giữa = giấu / bảng. Ctrl+U lưu, Ctrl+K xoá hết"),
])
s = s.replace(CU4, MOI4)

# ---- doi toa do cua ba dong con lai cho khoi de len thanh nut ----
for cu4b, moi4b in [
    ('VeChu(szChu, 8, 40, 0xFF66FF66);',
     'VeChu(szChu, 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 32, 0xFF66FF66);'),
    ('VeChu(s_szThongBao, 8, 56, 0xFFFF8080);',
     'VeChu(s_szThongBao, 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 46, 0xFFFF8080);'),
]:
    assert s.count(cu4b) == 1, "khong tim thay %r" % cu4b
    s = s.replace(cu4b, moi4b)

# ---- ve nut "mo che do sua" khi chua sua ----
CU5 = NL.join([
    "\tif (s_szThongBao[0] && GetTickCount() < s_uHetThongBao)",
])
assert s.count(CU5) == 1
MOI5 = NL.join([
    "\t//\t[UITOADO 09/09 B] Chua sua ma config da cho phep: hien nut nho de mo.",
    "\t//\tDien thoai khong co Ctrl+U. Doi cho bang [Ui] SuaToaDoNutX / SuaToaDoNutY.",
    "\tif (s_bDangSua == false && UiToaDo_ChoPhep())",
    "\t{",
    "\t\tint nX, nY, nW, nH;",
    "",
    "\t\tNutMoHinh(&nX, &nY, &nW, &nH);",
    "\t\tOKhoi(nX, nY, nX + nW, nY + nH, 0xA0202020);",
    '\t\tVeChu("%s", nX + 6, nY + 8, 0xFFFFD24A);' % vn(u"Sửa giao diện"),
    "\t}",
    "",
    CU5,
])
s = s.replace(CU5, MOI5)

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va (phan 2: noi vao mach chuot va mach ve):", P)
