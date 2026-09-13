# -*- coding: utf-8 -*-
#
# [WAUTO 13/09] Danh bong o danh sach sau khi DO THAT tren LDPlayer 11:10 (ban wauto30 da chay tot:
# thanh cuon that + vuot len xuong da cuon dung, bam dung dong sau khi cuon). Con ba cho chua dep:
#
#  1. Dong HUONG DAN duoi bang phu bi cat ngang: "... Ten trong danh sach thi aut" - khong phai do
#     be ngang o (660 px chua duoc 110 byte) ma do m_Huong la KWndText80: dem chu chi 80 byte,
#     cac cau huong dan dai 90-102 byte. Doi sang KWndText256.
#
#  2. Chu trong trang huong dan xuong dong SOM: XuongDong lay nMax = (rong - 12) / 6 = 108 byte nhung
#     bi chan boi WA_DS_CHU - 1 = 95 -> thua ~90 px ben phai moi dong. Nang WA_DS_CHU 96 -> 112.
#
#  3. Long o qua toi (14,13,11 gan nhu den) nen VIEN LOM ve roi ma khong nhin ra: bong den hat vao
#     tren-trai trung mau nen. Nang nen len (30,26,20) va lam ro hai mep -> o moi thay "lom xuong".
#
# Chi mobile (JX_MOBILE). Chay lai vo hai.

import io
import os
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")

DAU = "[WAUTO 13/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(p, cu, moi, ten):
    s = doc(p)
    if moi in s:
        print("   bo qua (da va):", ten)
        return
    if s.count(cu) != 1:
        raise SystemExit("khong tim thay dung 1 cho (%d): %s" % (s.count(cu), ten))
    ghi(p, s.replace(cu, moi))
    print("   da va:", ten)


H = "Sources/S3Client/Ui/UiCase/UiWAutoDsach.h"
C = "Sources/S3Client/Ui/UiCase/UiWAutoDsach.cpp"


def main():
    # 1. dem chu cua dong huong dan: 80 -> 256 byte
    thay(H,
         "\tKWndText80\t\t\tm_Huong;\r\n",
         "\tKWndText256\t\t\tm_Huong;\t// %s cau huong dan dai 90-102 byte, KWndText80 cat con 79\r\n" % DAU,
         "UiWAutoDsach.h: m_Huong KWndText80 -> KWndText256")

    # 2. do dai mot dong: 96 -> 112 byte (o rong 660 px chua duoc 108 byte)
    thay(H,
         "#define WA_DS_CHU\t\t96\t\t// do dai moi dong (o rong 560 px, font 12 = 6 px/byte -> 93 byte)\r\n",
         "#define WA_DS_CHU\t\t112\t\t// %s do dai moi dong (o rong 660 px, font 12 = 6 px/byte -> 108 byte)\r\n" % DAU,
         "UiWAutoDsach.h: WA_DS_CHU 96 -> 112")

    # 3. long o sang hon de vien lom nhin ra duoc
    thay(C,
         "\tWAD_Bong(x0, y0, x1, y1, 14, 13, 11, 2);\r\n",
         "\tWAD_Bong(x0, y0, x1, y1, 30, 26, 20, 2);\t\t// %s nen nau rat toi (truoc la 14,13,11 gan den:\r\n" % DAU +
         "\t\t\t\t\t\t\t\t\t\t\t\t// bong den hat vao cua vien lom trung mau nen nen khong thay o lom xuong)\r\n",
         "UiWAutoDsach.cpp: nen long o sang hon")

    print("xong wauto31")


if __name__ == "__main__":
    main()
