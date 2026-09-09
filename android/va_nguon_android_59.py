# -*- coding: utf-8 -*-
#
# [UITOADO 10/09 G] Bo cuc MAC DINH: "luu config toa do hien tai lam mac dinh, co xoa cung
#                    quay tro lai nhu nay".
#
# Truoc: chi co MOT tep UserData\UiToaDo.ini (nguoi choi tu dat). Xoa het / xoa tep = ve
# dung \Ui\ui3\*.ini goc cua ban PC - tuc bo cuc chu vua sap cho dien thoai mat sach.
#
# Nay co HAI lop:
#   \Ui\UiToaDo_MacDinh.ini      bo cuc mac dinh cua GAME (chu chot lai) - nap TRUOC
#   \UserData\UiToaDo.ini        nguoi choi tu dat - nap SAU, khoa trung thi de len
# "Xoa het" chi xoa lop nguoi choi roi nap lai lop mac dinh -> quay ve dung bo cuc chu chot.
# Them nut "Mac dinh" tren thanh nut che do sua: ghi bang hien tai vao tep mac dinh.
#
# Tep mac dinh dau tien = chinh bo cuc chu vua sap (17 muc), da chep san vao du lieu Android.

import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes  # noqa: E402

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\r\n"
DAU = "[UITOADO 10/09 G]"


def vn(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

# ---------------------------------------------------------------- 1. hang so
CU = '#define\tUITOADO_TEP\t\t\t"\\\\UserData\\\\UiToaDo.ini"'
assert s.count(CU) == 1, "khong tim thay UITOADO_TEP (%d)" % s.count(CU)
MOI = NL.join([
    CU,
    "//\t[UITOADO 10/09 G] bo cuc MAC DINH cua game (chu chot lai) - nap truoc tep nguoi choi",
    '#define\tUITOADO_TEP_MACDINH\t"\\\\Ui\\\\UiToaDo_MacDinh.ini"',
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 2. nap: tach thanh NapTep
CU = NL.join([
    "void UiToaDo_Nap()",
    "{",
    "\tchar\tszDuongDan[MAX_PATH];",
    "\tchar\tszDong[320];",
    "\tFILE*\tpTep;",
    "",
    "\ts_nSo   = 0;",
    "\ts_bTran = 0;",
    "\tg_GetFullPath(szDuongDan, (char*)UITOADO_TEP);",
    "\tpTep = fopen(szDuongDan, \"rt\");",
    "\tif (pTep == NULL)",
    "\t\treturn;",
])
assert s.count(CU) == 1, "khong tim thay dau UiToaDo_Nap (%d)" % s.count(CU)
MOI = NL.join([
    "//\t[UITOADO 10/09 G] Nap MOT tep vao bang - cong don, khoa trung thi tep nap sau de len.",
    "static void NapTep(const char* pszTep)",
    "{",
    "\tchar\tszDuongDan[MAX_PATH];",
    "\tchar\tszDong[320];",
    "\tFILE*\tpTep;",
    "",
    "\tg_GetFullPath(szDuongDan, (char*)pszTep);",
    "\tpTep = fopen(szDuongDan, \"rt\");",
    "\tif (pTep == NULL)",
    "\t\treturn;",
])
s = s.replace(CU, MOI)

CU = NL.join([
    "\tfclose(pTep);",
    "\tg_DebugLog(\"[UITOADO] nap %d muc tu %s\", s_nSo, szDuongDan);",
    "}",
])
assert s.count(CU) == 1, "khong tim thay duoi UiToaDo_Nap (%d)" % s.count(CU)
MOI = NL.join([
    "\tfclose(pTep);",
    "\tg_DebugLog(\"[UITOADO] nap xong %s -> bang co %d muc\", szDuongDan, s_nSo);",
    "}",
    "",
    "//\t[UITOADO 10/09 G] Hai lop: mac dinh cua game truoc, nguoi choi tu dat de len sau.",
    "void UiToaDo_Nap()",
    "{",
    "\ts_nSo   = 0;",
    "\ts_bTran = 0;",
    "\tNapTep(UITOADO_TEP_MACDINH);",
    "\tNapTep(UITOADO_TEP);",
    "}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 3. ghi: GhiTepVao(tep)
CU = NL.join([
    "static int GhiTep()",
    "{",
    "\tchar\tszDuongDan[MAX_PATH];",
    "\tFILE*\tpTep;",
    "\tint\t\ti;",
    "",
    "\tg_GetFullPath(szDuongDan, (char*)UITOADO_TEP);",
])
assert s.count(CU) == 1, "khong tim thay dau GhiTep (%d)" % s.count(CU)
MOI = NL.join([
    "//\t[UITOADO 10/09 G] ghi bang hien tai vao mot tep (tep nguoi choi hoac tep mac dinh)",
    "static int GhiTepVao(const char* pszTep)",
    "{",
    "\tchar\tszDuongDan[MAX_PATH];",
    "\tFILE*\tpTep;",
    "\tint\t\ti;",
    "",
    "\tg_GetFullPath(szDuongDan, (char*)pszTep);",
])
s = s.replace(CU, MOI)

CU = NL.join([
    "\tg_DebugLog(\"[UITOADO] ghi %d muc vao %s\", s_nSo, szDuongDan);",
    "\treturn s_nSo;",
    "}",
])
assert s.count(CU) == 1, "khong tim thay duoi GhiTep (%d)" % s.count(CU)
MOI = NL.join([
    "\tg_DebugLog(\"[UITOADO] ghi %d muc vao %s\", s_nSo, szDuongDan);",
    "\treturn s_nSo;",
    "}",
    "",
    "static int GhiTep()",
    "{",
    "\treturn GhiTepVao(UITOADO_TEP);",
    "}",
])
s = s.replace(CU, MOI)

# ---------------------------------------------------------------- 4. xoa het -> nap lai mac dinh
CU = "\tremove(szDuongDan);"
assert s.count(CU) == 1, "khong tim thay remove(szDuongDan) (%d)" % s.count(CU)
MOI = NL.join([
    "\tremove(szDuongDan);",
    "\t// [UITOADO 10/09 G] xoa xong thi nap lai lop MAC DINH va ap ngay - \"co xoa cung quay",
    "\t// tro lai nhu nay\" (chu). O nao khong co trong tep mac dinh thi giu cho hien tai.",
    "\tUiToaDo_Nap();",
    "\tUiToaDo_ApChoTatCa();",
])
s = s.replace(CU, MOI)
CU = '\tDatThongBao("%s");' % vn(u"Đã xoá hết. Cỡ và ô ẩn về nguyên trạng; toạ độ về gốc sau khi mở lại game")
if s.count(CU) == 1:
    s = s.replace(CU, '\tDatThongBao("%s");' % vn(u"Đã xoá phần tự đặt, quay về bố cục mặc định của game"))
    print("  (da doi cau bao cua Xoa het)")

# ---------------------------------------------------------------- 5. nut "Mac dinh"
for cu, moi in [
    ("#define\tNUT_XOAHET\t\t102\t\t// xoa het (= Ctrl+K)",
     "#define\tNUT_XOAHET\t\t102\t\t// xoa het (= Ctrl+K)" + NL
     + "#define\tNUT_MACDINH\t\t103\t\t// [UITOADO 10/09 G] ghi bo cuc hien tai lam MAC DINH cua game"),
    ("#define\tUITOADO_SO_NUT\t8", "#define\tUITOADO_SO_NUT\t9"),
    ('\t{ "%s",\tNUT_XOAHET },' % vn(u"Xoá hết"),
     '\t{ "%s",\tNUT_XOAHET },' % vn(u"Xoá hết") + NL + '\t{ "%s",\tNUT_MACDINH },' % vn(u"Mặc định")),
    (NL.join(["\tcase NUT_XOAHET:", "\t\tUiToaDo_XoaHet();", "\t\tbreak;"]),
     NL.join([
        "\tcase NUT_XOAHET:", "\t\tUiToaDo_XoaHet();", "\t\tbreak;",
        "\tcase NUT_MACDINH:",
        "\t\t// [UITOADO 10/09 G] chot bo cuc dang co lam mac dinh cua game (cho moi may)",
        "\t\tif (GhiTepVao(UITOADO_TEP_MACDINH) >= 0)",
        '\t\t\tDatThongBao("%s");' % vn(u"Đã lưu bố cục hiện tại làm mặc định của game"),
        "\t\telse",
        '\t\t\tDatThongBao("%s");' % vn(u"Lỗi: không ghi được Ui\\\\UiToaDo_MacDinh.ini"),
        "\t\tbreak;",
     ])),
]:
    assert s.count(cu) == 1, "khong tim thay: %.60s (%d)" % (cu, s.count(cu))
    s = s.replace(cu, moi)

io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
