# -*- coding: ascii -*-
"""[TKINFO 05/09 + SPRFIX 06/09] Ve nen cua so "thong tin tran" Tong Kim (221x268) theo phong cach nen 2.0
(than gan den 20,20,20; dai tieu de nau vang 48,42,28; vien vang manh) va ghi SPR bang ghi_spr.py (co kiem lai).
Ban dau tien (05/09) ghi bang doan ma noi tuyen bi thieu dem bang mau -> client SAP luc ve (xem ghi_spr.py).
"""
import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ghi_spr import ghi_spr_tu_anh, kiem_spr

W, H = 221, 268
S = 3  # ve lon roi thu nho cho min

DICH = [
    r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\spr\Ui3\UiGameMain\UiTongKim\thongtin20.spr",
    r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\tongkim_chat\client\spr\Ui3\UiGameMain\UiTongKim\thongtin20.spr",
]


def ve():
    img = Image.new("RGBA", (W * S, H * S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # than cua so + vien vang
    d.rounded_rectangle((0, 0, W * S - 1, H * S - 1), radius=4 * S, fill=(20, 20, 20, 232), outline=(112, 92, 52, 255), width=1 * S)
    # dai tieu de
    d.rounded_rectangle((1 * S, 1 * S, W * S - 2 * S, 26 * S), radius=4 * S, fill=(48, 42, 28, 255))
    d.rectangle((1 * S, 20 * S, W * S - 2 * S, 26 * S), fill=(48, 42, 28, 255))
    d.line((1 * S, 26 * S, W * S - 2 * S, 26 * S), fill=(150, 122, 62, 255), width=1 * S)
    d.line((1 * S, 27 * S, W * S - 2 * S, 27 * S), fill=(70, 58, 34, 255), width=1 * S)
    # duong ke tren/duoi bang xep hang
    for y in (90, 226):
        d.line((8 * S, y * S, (W - 8) * S, y * S), fill=(96, 80, 46, 255), width=1 * S)
    # hoa van goc
    for x in (8, W - 14):
        d.rectangle((x * S, 10 * S, (x + 6) * S, 16 * S), outline=(160, 130, 70, 255), width=1 * S)
    return img.resize((W, H), Image.LANCZOS)


if __name__ == "__main__":
    img = ve()
    png = os.path.join(os.path.dirname(os.path.abspath(__file__)), "thongtin20_preview.png")
    img.save(png)
    for p in DICH:
        n = ghi_spr_tu_anh(img, p)
        ok, mo_ta = kiem_spr(p)
        print("ghi %s (%d byte) -> %s" % (p, n, mo_ta))
    print("xem truoc:", png)
