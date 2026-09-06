# -*- coding: ascii -*-
"""[TKINFO 05/09 + SPRFIX 06/09] Ve nen cua so "thong tin tran" Tong Kim theo phong cach nen 2.0
(than gan den 20,20,20; dai tieu de nau vang 48,42,28; vien vang manh) va ghi SPR bang ghi_spr.py (co kiem lai).

Sinh HAI anh:
  thongtin20.spr      221x268  - cua so mo day du
  thongtin20_thu.spr  221x27   - chi dai tieu de, dung khi nguoi choi bam nut THU GON ([TKINFO 06/09]).
      Phai co anh rieng vi KWndImage::PaintWindow ve NGUYEN tam SPR, KHONG cat theo m_Height.

Ban dau tien (05/09) ghi bang doan ma noi tuyen bi thieu dem bang mau -> client SAP luc ve (xem ghi_spr.py).
"""
import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ghi_spr import ghi_spr_tu_anh, kiem_spr

W, H = 221, 268
H_THU = 27
S = 3  # ve lon roi thu nho cho min

LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\spr\Ui3\UiGameMain\UiTongKim"
MIR = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\tongkim_chat\client\spr\Ui3\UiGameMain\UiTongKim"


def _dai_tieu_de(d, w_px, h_px):
    """Ve dai tieu de + hoa van goc (dung chung cho ca hai anh)."""
    d.rounded_rectangle((1 * S, 1 * S, w_px - 2 * S, 26 * S), radius=4 * S, fill=(48, 42, 28, 255))
    d.rectangle((1 * S, 20 * S, w_px - 2 * S, 26 * S), fill=(48, 42, 28, 255))
    d.line((1 * S, 26 * S, w_px - 2 * S, 26 * S), fill=(150, 122, 62, 255), width=1 * S)
    # [TKINFO 06/09] chi ve hoa van BEN PHAI: goc trai dai tieu de danh cho nut thu gon [BtnFold]
    # (dat ben phai thi than bang Chien Bao 600x350 tai x=100 che mat nut khi bang do dang mo).
    d.rectangle(((W - 14) * S, 10 * S, (W - 8) * S, 16 * S), outline=(160, 130, 70, 255), width=1 * S)


def ve_day_du():
    img = Image.new("RGBA", (W * S, H * S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((0, 0, W * S - 1, H * S - 1), radius=4 * S, fill=(20, 20, 20, 232), outline=(112, 92, 52, 255), width=1 * S)
    _dai_tieu_de(d, W * S, H * S)
    d.line((1 * S, 27 * S, W * S - 2 * S, 27 * S), fill=(70, 58, 34, 255), width=1 * S)
    for y in (90, 226):  # duong ke tren/duoi bang xep hang
        d.line((8 * S, y * S, (W - 8) * S, y * S), fill=(96, 80, 46, 255), width=1 * S)
    return img.resize((W, H), Image.LANCZOS)


def ve_thu_gon():
    img = Image.new("RGBA", (W * S, H_THU * S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((0, 0, W * S - 1, H_THU * S - 1), radius=4 * S, fill=(20, 20, 20, 232), outline=(112, 92, 52, 255), width=1 * S)
    _dai_tieu_de(d, W * S, H_THU * S)
    return img.resize((W, H_THU), Image.LANCZOS)


if __name__ == "__main__":
    here = os.path.dirname(os.path.abspath(__file__))
    for ten, img in [("thongtin20.spr", ve_day_du()), ("thongtin20_thu.spr", ve_thu_gon())]:
        img.save(os.path.join(here, ten.replace(".spr", "_preview.png")))
        for thu_muc in (LIVE, MIR):
            p = os.path.join(thu_muc, ten)
            n = ghi_spr_tu_anh(img, p)
            ok, mo_ta = kiem_spr(p)
            print("ghi %-90s %7d byte -> %s" % (p, n, mo_ta))
