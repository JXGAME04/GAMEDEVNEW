# -*- coding: utf-8 -*-
r"""[VATPHAM 12/09 b] Anh cho BANG THONG TIN VAT PHAM cua ban mobile (chu: "cac nut da co san roi" - dung anh nut
kho VNKU nhu hanh trang, khong ve o xam tu che).

Ghi vao lop ghi de android\du_lieu_ghi_de\spr\ui3\uivatpham\ (+ D:\jx1_android_data):
    khung_vatpham.spr    320 x 300 : khung hoa van VNKU (nhu khung hanh trang), khong co tieu de
    nut_vp_<ten>.spr      92 x 30  x2 khung (thuong / bam): nen nut VNKU + chu tieng Viet cua game

Dung: python android\anh_vatpham_vnku.py
"""
import io
import os
import sys

from PIL import Image, ImageDraw, ImageEnhance, ImageFont

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import ghi_spr_nhieu_khung, ghi_moi_noi  # noqa: E402

VNKU_PNG = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\png\Spr\UiNew\UiItem"
#   [VATPHAM 12/09 c] chu: "khong co cai nut nao dep hon a?" -> doi sang kieu NUT NGOC (UiAutoNew) thay dai go.
NUT_NEN = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\png\Spr\UiNew\UiAutoNew\btn_len_f00.png"
NUT_NAP = 46              # be rong hai dau nut (hoa van goc) giu nguyen khi xoa chu
W, H = 320, 300           # khung bang (khong dung nua)
NUT_W, NUT_H = 100, 34

NUT = [
    ("dung",      "Dùng"),
    ("trang_bi",  "Trang bị"),
    ("rao",       "Rao"),
    ("nem",       "Ném"),
    ("dinh",      "Đính"),
    ("thao_dinh", "Tháo đính"),
    ("thao",      "Tháo"),
    ("tach",      "Tách"),
    ("lay_ra",    "Lấy ra"),
    ("cat_ruong", "Cất rương"),
    ("chon",      "Chọn"),
    ("dong",      "Đóng"),
]


def png(ten):
    return Image.open(os.path.join(VNKU_PNG, ten)).convert("RGBA")


def ghi(rel, cac_khung):
    tmp = os.path.join(os.environ.get("TEMP", GOC), "vatpham_tmp.spr")
    msg = ghi_spr_nhieu_khung(cac_khung, tmp)
    du_lieu = io.open(tmp, "rb").read()
    print("%-52s %dx%d x%d  %s" % (rel, cac_khung[0].size[0], cac_khung[0].size[1], len(cac_khung), msg))
    ghi_moi_noi(rel, du_lieu)


def nen_nut_trong(rong=NUT_W, cao=NUT_H):
    """nen NUT NGOC cua kho VNKU (UiAutoNew, btn_len) da XOA CHU: giu hai dau hoa van, keo mot cot sach ra giua"""
    a = Image.open(NUT_NEN).convert("RGBA")
    w, h = a.size
    cot = a.crop((NUT_NAP, 0, NUT_NAP + 6, h)).resize((w - 2 * NUT_NAP, h), Image.BILINEAR)
    a.paste(cot, (NUT_NAP, 0))
    return a.resize((rong, cao), Image.LANCZOS)


def font_chu(co):
    for f in (r"C:\Windows\Fonts\arialbd.ttf", r"C:\Windows\Fonts\tahomabd.ttf", r"C:\Windows\Fonts\arial.ttf"):
        if os.path.isfile(f):
            return ImageFont.truetype(f, co)
    return ImageFont.load_default()


def nut_chu(chu, rong=NUT_W, cao=NUT_H):
    thuong = nen_nut_trong(rong, cao)
    d = ImageDraw.Draw(thuong)
    co = 17
    font = font_chu(co)
    while co > 11:
        x0, y0, x1, y1 = d.textbbox((0, 0), chu, font=font)
        if x1 - x0 <= rong - 10:
            break
        co -= 1
        font = font_chu(co)
    x0, y0, x1, y1 = d.textbbox((0, 0), chu, font=font)
    tx, ty = (rong - (x1 - x0)) // 2 - x0, (cao - (y1 - y0)) // 2 - y0 - 1
    for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (1, 1), (-1, 1), (1, -1)):
        d.text((tx + dx, ty + dy), chu, font=font, fill=(16, 30, 26, 255))
    d.text((tx, ty), chu, font=font, fill=(255, 248, 214, 255))
    bam = ImageEnhance.Brightness(thuong).enhance(0.7)
    return [thuong, bam]


def khung():
    """khung hoa van VNKU (dai hoa van cua main.png xep lop) + vien"""
    main = png("main.png")
    dai = main.crop((10, 990, 664, 1160))
    xep = Image.new("RGBA", (dai.size[0], dai.size[1] * 4), (0, 0, 0, 255))
    for k in range(4):
        xep.paste(dai if k % 2 == 0 else dai.transpose(Image.FLIP_TOP_BOTTOM), (0, k * dai.size[1]))
    nen = xep.resize((W, H), Image.LANCZOS)
    im = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    im.paste(nen, (0, 0))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, W - 1, H - 1), outline=(30, 26, 20, 255), width=2)
    d.rectangle((2, 2, W - 3, H - 3), outline=(120, 104, 66, 255), width=1)
    #   o dat bieu tuong vat pham (goc tren-trai)
    d.rectangle((12, 12, 12 + 44 - 1, 12 + 44 - 1), fill=(12, 12, 12, 255), outline=(120, 104, 66, 255), width=1)
    return im


def main():
    #   khung bang khong dung nua: dai nut nam ngay duoi bang chu giai co san cua game
    for ten, chu in NUT:
        ghi(os.path.join("spr", "ui3", "uivatpham", "nut_vp_%s.spr" % ten), nut_chu(chu))
    print("xong: %d nut + 1 khung" % len(NUT))


if __name__ == "__main__":
    main()
