# -*- coding: utf-8 -*-
r"""[VATPHAM 12/09 e] Chu 15:30: "tren dien thoai vao 4 o den phim 1 2 3 4 bo item van lech".

Bon o den do KHONG phai cua so o phim: chung la HINH VE SAN trong anh nen cua thanh duoi
(\spr\UiNew\UiPlayerBar\khung_chat_mobile.spr, 1040x604, o tai 932,205 / 972,205 / 932,245 / 972,245).
Anh nen do duoc ve theo cua so 800x600 nen tren khung ve rong (dien thoai 1371x617) no bi keo lech:
do duoc o den ra 1099,241 trong khi cua so o phim nam 1098,219 -> bo item vao thi anh vat pham mot noi,
o den mot noi.

Bo sinh nay:
  1. Cat mot o (36x36) trong anh nen ra thanh anh rieng  -> spr\ui3\uivatpham\o_phim_nen.spr
  2. XOA bon o khoi anh nen (de trong suot)              -> spr\uinew\uiplayerbar\khung_chat_mobile.spr (lop ghi de)
Roi KImmediaItem::PaintWindow (chi Android) tu ve anh o_phim_nen vao DUNG khung cua so cua tung o phim,
nen o den va vat pham luon trung nhau tren moi kich co man hinh.

Dung: python android\anh_ophim_nen.py
"""
import io
import os
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import doc_spr, ghi_spr_nhieu_khung, ghi_moi_noi  # noqa: E402

NEN = r"D:\jx1_android_data\spr\UiNew\UiPlayerBar\khung_chat_mobile.spr"
O = [(932, 205), (972, 205), (932, 245), (972, 245)]     # goc tren-trai bon o trong anh nen
CO = 36


def ghi(rel, cac_khung):
    tmp = os.path.join(os.environ.get("TEMP", GOC), "ophim_tmp.spr")
    msg = ghi_spr_nhieu_khung(cac_khung, tmp)
    du_lieu = io.open(tmp, "rb").read()
    print("%-52s %dx%d x%d  %s" % (rel, cac_khung[0].size[0], cac_khung[0].size[1], len(cac_khung), msg))
    ghi_moi_noi(rel, du_lieu)


def main():
    W, H, khung, lech = doc_spr(NEN)
    im = khung[0].copy()
    print("anh nen: %dx%d, %d khung" % (W, H, len(khung)))

    #   1. cat mot o ra lam anh rieng
    o = im.crop((O[0][0], O[0][1], O[0][0] + CO, O[0][1] + CO))
    ghi(os.path.join("spr", "ui3", "uivatpham", "o_phim_nen.spr"), [o])

    #   2. xoa bon o khoi anh nen (trong suot)
    trong = Image.new("RGBA", (CO, CO), (0, 0, 0, 0))
    for (x, y) in O:
        im.paste(trong, (x, y))
    ghi(os.path.join("spr", "uinew", "uiplayerbar", "khung_chat_mobile.spr"), [im])
    print("xong")


if __name__ == "__main__":
    main()
