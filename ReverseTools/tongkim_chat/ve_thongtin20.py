# -*- coding: ascii -*-
"""[TKINFO 05/09 + SPRFIX 06/09 + 06/09 toi] Ve nen cua so "thong tin tran" Tong Kim theo phong cach nen 2.0
va ghi SPR bang ghi_spr.py (co kiem lai).

Sinh HAI anh:
  thongtin20.spr      221x232  - cua so mo day du
  thongtin20_thu.spr   27x27   - CHI o nut, dung khi nguoi choi bam nut THU GON ([TKINFO 06/09 toi]).
      Phai co anh rieng vi KWndImage::PaintWindow ve NGUYEN tam SPR, KHONG cat theo m_Width/m_Height.

Lich su:
  05/09      ban dau tien ghi bang doan ma noi tuyen bi thieu dem bang mau -> client SAP luc ve (xem ghi_spr.py).
  06/09      them anh thu gon 221x27 (chi ha chieu cao).
  06/09 toi  chu bao 3 viec:
    1. "nut thu nho chua thu nho duoc hoan toan" -> anh thu gon xuong 27x27, vua dung o nut [BtnFold]
       (ini Left=6 Top=8 Width=15 Height=11) nen nut van nam dung cho cu, khong phai doi toa do.
    2. "nen mau toi qua fix lai mau sang hon ti cho co the nhin xuyen" -> than 20,20,20 alpha 232 (do tren
       anh chup that ra ~(26,25,21), gan nhu dac) -> 42,40,34 alpha 168 (~66%, ra ~(65,62,53) tren nen dat).
       Dai tieu de 48,42,28 alpha 255 -> 72,63,42 alpha 216 (van du dac de doc chu tieu de).
    3. bo nut "Nhan xem Chien Bao" -> khung thap lai 268 -> 232 (duong ke duoi bang xep hang o y=226 giu nguyen).

Alpha nam trong RLE cua SPR ([run][alpha] moi doan) va ghi_spr._luong_tu_alpha() lam tron ve (a//16)*16+8,
nen 168 / 216 / 255 deu ra dung so do, khong lech.
"""
import os
import sys

from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ghi_spr import ghi_spr_tu_anh, kiem_spr

W, H = 221, 232
W_THU, H_THU = 27, 27
CAO_TIEUDE = 27          # chieu cao dai tieu de trong anh day du
S = 3                    # ve lon roi thu nho cho min

# [TKINFO 06/09 toi] bang mau: than sang hon va cho nhin xuyen, vien giu dac cho ro khung
NEN_THAN = (42, 40, 34, 168)
NEN_TIEUDE = (72, 63, 42, 216)
VIEN = (150, 124, 70, 255)
VIEN_DUOI_TIEUDE = (170, 142, 82, 255)
KE_MO = (96, 82, 50, 255)
HOA_VAN = (176, 146, 84, 255)

HERE = os.path.dirname(os.path.abspath(__file__))
LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\spr\Ui3\UiGameMain\UiTongKim"
# guong trong git: <goc cay>\serverscript_jx2\tongkim_chat\client\spr\... (tinh theo vi tri script, khong go cung)
MIR = os.path.normpath(os.path.join(
    HERE, "..", "..", "serverscript_jx2", "tongkim_chat", "client", "spr", "Ui3", "UiGameMain", "UiTongKim"))


def ve_day_du():
    img = Image.new("RGBA", (W * S, H * S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((0, 0, W * S - 1, H * S - 1), radius=4 * S, fill=NEN_THAN, outline=VIEN, width=1 * S)
    # dai tieu de: dac hon than cho de doc chu
    d.rounded_rectangle((1 * S, 1 * S, (W - 2) * S, (CAO_TIEUDE - 1) * S), radius=4 * S, fill=NEN_TIEUDE)
    d.rectangle((1 * S, 20 * S, (W - 2) * S, (CAO_TIEUDE - 1) * S), fill=NEN_TIEUDE)
    d.line((1 * S, (CAO_TIEUDE - 1) * S, (W - 2) * S, (CAO_TIEUDE - 1) * S), fill=VIEN_DUOI_TIEUDE, width=1 * S)
    # [TKINFO 06/09] chi ve hoa van BEN PHAI: goc trai dai tieu de danh cho nut thu gon [BtnFold]
    # (dat ben phai thi than bang Chien Bao 600x350 tai x=100 che mat nut khi bang do dang mo).
    d.rectangle(((W - 14) * S, 10 * S, (W - 8) * S, 16 * S), outline=HOA_VAN, width=1 * S)
    for y in (90, 226):  # duong ke tren/duoi bang xep hang
        d.line((8 * S, y * S, (W - 8) * S, y * S), fill=KE_MO, width=1 * S)
    return img.resize((W, H), Image.LANCZOS)


def ve_thu_gon():
    """[TKINFO 06/09 toi] chi con o nut: mot o vuong 27x27 cung mau dai tieu de, nut ve de len tren."""
    img = Image.new("RGBA", (W_THU * S, H_THU * S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((0, 0, W_THU * S - 1, H_THU * S - 1), radius=4 * S, fill=NEN_TIEUDE,
                        outline=VIEN, width=1 * S)
    return img.resize((W_THU, H_THU), Image.LANCZOS)


if __name__ == "__main__":
    for ten, img in [("thongtin20.spr", ve_day_du()), ("thongtin20_thu.spr", ve_thu_gon())]:
        img.save(os.path.join(HERE, ten.replace(".spr", "_preview.png")))
        for thu_muc in (LIVE, MIR):
            p = os.path.join(thu_muc, ten)
            n = ghi_spr_tu_anh(img, p)
            ok, mo_ta = kiem_spr(p)
            print("ghi %-90s %7d byte -> %s" % (p, n, mo_ta))
