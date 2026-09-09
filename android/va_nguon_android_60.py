# -*- coding: utf-8 -*-
#
# [UITOADO 10/09 H] Hop thu / dau gia "mo len bi loi vi tri, khong dieu chinh duoc" (chu, 10/09 11:5x).
#
# Anh chup: khung hop thu mo dung cho chu keo (146,43) nhung RUOT (danh sach thu, trang dau gia) bi
# doi them dung (146,43) / (154,59) nua -> danh sach de len to giay, trang dau gia lech khoi khung.
#
# Nguyen nhan: khoa cua bang toa do la "<lop cua so goc>|<ten muc ini>". Cua so goc doc [Main] cua
# mail_manager.ini -> khoa "KUiMailManager|Main". Nhung con ben trong (m_List) doc [Main] cua
# mail_list.ini -> CUNG khoa "KUiMailManager|Main" -> ApChoCay dat con vao (146,43) TUONG DOI VOI CHA.
# Keo cai gi cung dinh vao ruot -> "khong dieu chinh duoc".
#
# Sua: o CON ma muc ini ten "Main" thi coi nhu chua dat ten -> dung ten thay the theo vi tri trong
# cay (#cha.con, da co tu [UITOADO 09/09 F]). Va ApChoCay cung phai dung ten thay the do (truoc day
# chi dung TaoKhoa nen o dat ten thay the ghi duoc ma KHONG ap lai luc mo game).
# Ca PC lan Android (UiToaDo dung chung).

import io
import os

os.chdir(r"D:\GAMEDEVNEW_wt_mobile")
NL = "\n"      # hai doan sua la ma goc cua tep, ket dong LF (tep nay lan ket dong LF/CRLF)
DAU = "[UITOADO 10/09 H]"
P = "Sources/S3Client/Ui/Elem/UiToaDo.cpp"
s = io.open(P, encoding="latin-1", newline="").read()
if DAU in s:
    print("da va roi, bo qua:", P)
    raise SystemExit

CU = NL.join([
    "\tif (pWnd->GetMucIni()[0] == 0)",
    "\t\treturn false;",
    "",
    "\t_snprintf(pszRa, nCo, \"%s|%s\", pszLop, pWnd->GetMucIni());",
])
assert s.count(CU) == 1, "khong tim thay than TaoKhoa (%d)" % s.count(CU)
MOI = NL.join([
    "\tif (pWnd->GetMucIni()[0] == 0)",
    "\t\treturn false;",
    "\t//\t%s O CON ma muc ini cung ten \"Main\" (ini con: mail_list.ini, auction_item_*.ini...)" % DAU,
    "\t//\tthi khoa \"<lop>|Main\" TRUNG voi khoa cua cua so goc -> keo goc la RUOT bi doi theo (hop thu,",
    "\t//\tdau gia). Coi nhu chua dat ten -> ben goi dat ten thay the theo vi tri trong cay.",
    "\tif (pWnd->GetOwner() != pWnd && strcmp(pWnd->GetMucIni(), \"Main\") == 0)",
    "\t\treturn false;",
    "",
    "\t_snprintf(pszRa, nCo, \"%s|%s\", pszLop, pWnd->GetMucIni());",
])
s = s.replace(CU, MOI)

CU = NL.join([
    "\twhile (pWnd)",
    "\t{",
    "\t\tif (TaoKhoa(pszLop, pWnd, szKhoa, sizeof(szKhoa)))",
    "\t\t\tApMotO(pWnd, TimKhoa(szKhoa));",
    "\t\tApChoCay(pszLop, pWnd->GetFirstChild());",
])
assert s.count(CU) == 1, "khong tim thay ApChoCay (%d)" % s.count(CU)
MOI = NL.join([
    "\twhile (pWnd)",
    "\t{",
    "\t\t//\t%s dung ca ten thay the (#cha.con) - truoc day o khong ten ghi duoc ma khong ap lai" % DAU,
    "\t\tif (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)))",
    "\t\t\tApMotO(pWnd, TimKhoa(szKhoa));",
    "\t\tApChoCay(pszLop, pWnd->GetFirstChild());",
])
s = s.replace(CU, MOI)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("da va:", P)
