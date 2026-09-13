# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - cac sua sau khi dich thu bang clang cua NDK.
# Gom cac sua nho de wauto9/10/11 dich duoc; moi sua ghi ro vi sao. Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def V(s):
    return re.sub(r"@@(.*?)@@", lambda m: vn(m.group(1)), s, flags=re.S)


# moi muc: (tep, dau nhan biet da va, chuoi cu, chuoi moi, ten)
SUA = [
    # WA_MENU_TOI_DA dung trong khai bao thanh vien cua UiWAutoTrang.h nen phai dinh nghia o HEADER, khong phai .cpp
    ("Sources/S3Client/Ui/UiCase/UiWAutoTrang.h", "#define WA_MENU_TOI_DA",
     "#define WA_TR_AN_TOI_DA\t(WA_TR_TICK + WA_TR_NHAP + WA_TR_CHON + WA_TR_NHAN + WA_TR_NUT)",
     "#define WA_TR_AN_TOI_DA\t(WA_TR_TICK + WA_TR_NHAP + WA_TR_CHON + WA_TR_NHAN + WA_TR_NUT)\r\n"
     "// %s so dong toi da cua mot hop chon (danh sach chieu 72, ten nguoi quanh day 100) va nguong mo BANG PHU\r\n"
     "// thay cho KPopupMenu (menu khong cuon: cao = so dong x 26 px, man 604 px la mat tu dong 24)\r\n"
     "#define WA_MENU_TOI_DA\t100\r\n#define WA_MENU_DAI\t\t10" % DAU,
     "UiWAutoTrang.h: dinh nghia WA_MENU_TOI_DA"),
    ("Sources/S3Client/Ui/UiCase/UiWAutoTrang.cpp", "// WA_MENU_TOI_DA / WA_MENU_DAI",
     "#define WA_MENU_TOI_DA\t100\t// %s du cho danh sach chieu (72) va ten nguoi quanh day (100)\r\n"
     "#define WA_MENU_DAI\t\t10\t// tu so dong nay tro len thi mo BANG PHU (cuon duoc) thay cho KPopupMenu (khong cuon)" % DAU,
     "// WA_MENU_TOI_DA / WA_MENU_DAI: dinh nghia trong UiWAutoTrang.h (dung trong khai bao thanh vien)",
     "UiWAutoTrang.cpp: bo dinh nghia trung"),
    # Nut [?] la widget THU 16 cua khung nhung hai mang "an duoi menu" moi chua 15 (4 nhom + 8 tab + 3) -> ghi tran
    # mot o. Noi ca hai len 4 (nhom + tab + BatTat + TrangThai + Dong + TroGiup = 16).
    ("Sources/S3Client/Ui/UiCase/UiWAuto.h", "WA_UI_TAB_MOI_NHOM + 4",
     "\tKWndWindow*\tm_apAnTam[WA_UI_SO_NHOM + WA_UI_TAB_MOI_NHOM + 3];",
     "\tKWndWindow*\tm_apAnTam[WA_UI_SO_NHOM + WA_UI_TAB_MOI_NHOM + 4];\t// %s + 1 cho nut [?]" % DAU,
     "UiWAuto.h: noi mang an tam"),
    ("Sources/S3Client/Ui/UiCase/UiWAuto.cpp", "WA_UI_TAB_MOI_NHOM + 4",
     "\tKWndWindow* ap[WA_UI_SO_NHOM + WA_UI_TAB_MOI_NHOM + 3];",
     "\tKWndWindow* ap[WA_UI_SO_NHOM + WA_UI_TAB_MOI_NHOM + 4];\t// %s + 1 cho nut [?]" % DAU,
     "UiWAuto.cpp: noi mang ap"),
]


def main():
    for p, dau, cu, moi, ten in SUA:
        s = doc(p)
        if dau in s:
            print("   bo qua (da va):", ten)
            continue
        cu = V(cu)
        moi = V(moi)
        if s.count(cu) != 1:
            raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(cu), ten))
        ghi(p, s.replace(cu, moi))
        print("   da va:", ten)
    print("xong wauto12")


if __name__ == "__main__":
    main()
