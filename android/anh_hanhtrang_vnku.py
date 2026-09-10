# -*- coding: utf-8 -*-
r"""[HANHTRANG 12/09] Lam lai cua so HANH TRANG cho mobile (chu 02:20: "phai lam toan bo UI hanh trang o to hon, kich vao cac icon
qua nho"): o 28 px -> 44 px (dien thoai 1,75x: 49 px -> 77 px vat ly, ~4,8 mm), khung + nut lay tu kho VNKU (UiItem).
  * Ghi anh (SPR) vao lop ghi de android\du_lieu_ghi_de\spr\ui3\uiitem\ (+ D:\jx1_android_data) bang ghi_moi_noi (nhu anh_wauto_vnku.py):
      uiitemmain_m.spr   316x592 : khung (tieu de VNKU, hoa van, luoi 6x10 o 44 px, hang tien)
      nut_hanhtrang_*.spr 96x35 x2 khung (thuong / bam): loi_rao (quang_ba), dinh_gia (btn_tham_dinh), rao_ban (btn_trung_bay),
                          gui_tien, trang_bi, dong (btn_dong)
  * Ghi ini android\du_lieu_ghi_de\ui\ui3\uiitem.ini: Main 316x592, ItemBox 264x440 (6x10, UnitBorder 2 -> o 44), nut 96x35.
    Cung ten muc / lop KUiItem nhu ini goc -> khong doi C++. Ban PC dung ini goc (lop ghi de chi cho Android).
Dung: python android\anh_hanhtrang_vnku.py
"""
import io
import os
import sys

from PIL import Image, ImageDraw, ImageEnhance

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import ghi_spr_nhieu_khung, ghi_moi_noi  # noqa: E402

VNKU_PNG = r"C:\Users\nguye\Downloads\NHACTAI\VNKU_ui\png\Spr\UiNew\UiItem"
O = 44                  # co o
COT, HANG = 6, 10
VIEN = 2                # UnitBorder
LUOI_X, LUOI_Y = 26, 48
LUOI_W, LUOI_H = O * COT, O * HANG          # 264 x 440
W, H = LUOI_W + 2 * LUOI_X, 592             # 316 x 592
NUT_W, NUT_H = 96, 35
TIEN_Y = LUOI_Y + LUOI_H + 6                # 494
NUT_Y1 = TIEN_Y + 26                        # 520
NUT_Y2 = NUT_Y1 + NUT_H + 4                 # 559
NUT_X = [8, 110, 212]


def ghi(rel, cac_khung):
    tmp = os.path.join(os.environ.get("TEMP", GOC), "hanhtrang_tmp.spr")
    msg = ghi_spr_nhieu_khung(cac_khung, tmp)
    du_lieu = io.open(tmp, "rb").read()
    print("%-48s %dx%d x%d  %s" % (rel, cac_khung[0].size[0], cac_khung[0].size[1], len(cac_khung), msg))
    ghi_moi_noi(rel, du_lieu)


def png(ten):
    return Image.open(os.path.join(VNKU_PNG, ten)).convert("RGBA")


def khung():
    main = png("main.png")                                      # 674 x 1175: tieu de 0..100, than hoa van
    # nen: dai hoa van KHONG co o (y 990..1160 cua main.png) xep 4 lop roi keo ve W x H (crop ca than thi loi luoi 5x6 cua VNKU)
    dai = main.crop((10, 990, 664, 1160))
    xep = Image.new("RGBA", (dai.size[0], dai.size[1] * 4), (0, 0, 0, 255))
    for k in range(4):
        xep.paste(dai if k % 2 == 0 else dai.transpose(Image.FLIP_TOP_BOTTOM), (0, k * dai.size[1]))
    nen = xep.resize((W, H), Image.LANCZOS)
    im = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    im.paste(nen, (0, 0))
    # vien ngoai toi 2 px
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, W - 1, H - 1), outline=(30, 26, 20, 255), width=2)
    # tieu de VNKU "HANH TRANG" (dai 0..100 cua main.png) thu ve 316 x 44
    tieu_de = main.crop((0, 0, 674, 100)).resize((W, 44), Image.LANCZOS)
    im.alpha_composite(tieu_de, (0, 0))
    # luoi 6x10 o 44 px: o toi, vien xam (giong VNKU)
    for r in range(HANG):
        for c in range(COT):
            x0 = LUOI_X + c * O + VIEN
            y0 = LUOI_Y + r * O + VIEN
            x1 = x0 + O - 2 * VIEN - 1
            y1 = y0 + O - 2 * VIEN - 1
            d.rectangle((x0, y0, x1, y1), fill=(12, 12, 12, 255), outline=(78, 78, 78, 255), width=1)
            d.rectangle((x0 + 1, y0 + 1, x1 - 1, y1 - 1), outline=(40, 40, 40, 255), width=1)
    # hang tien: thoi vang + o den (dai 895..975 cua main.png)
    tien = main.crop((25, 895, 660, 975)).resize((W - 16, 22), Image.LANCZOS)
    im.alpha_composite(tien, (8, TIEN_Y))
    return im


def nut(ten_png, rong=NUT_W, cao=NUT_H):
    a = png(ten_png)
    thuong = a.resize((rong, cao), Image.LANCZOS)
    bam = ImageEnhance.Brightness(thuong).enhance(0.7)
    return [thuong, bam]


def main():
    ghi(os.path.join("spr", "ui3", "uiitem", "uiitemmain_m.spr"), [khung()])
    for ten, tep in (("loi_rao", "quang_ba_f00.png"), ("dinh_gia", "btn_tham_dinh_f00.png"), ("rao_ban", "btn_trung_bay_f00.png"),
                     ("gui_tien", "gui_tien_f00.png"), ("trang_bi", "trang_bi_f00.png"), ("dong", "btn_dong_f00.png")):
        ghi(os.path.join("spr", "ui3", "uiitem", "nut_hanhtrang_%s.spr" % ten), nut(tep))
    # ini
    goc = io.open(r"D:\jx1_android_data\ui\ui3\uiitem.ini", encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in goc else "\n"
    doan = [
        "; [HANHTRANG 12/09] Hanh trang mobile: o 44 px (goc 28), khung + nut VNKU (sinh boi android/anh_hanhtrang_vnku.py). Chi Android.",
        "[Main]", "Left=400", "Top=10", "Width=%d" % W, "Height=%d" % H, "Moveable=1", "StartPos=800,100", "Trans=1",
        "Image=\\spr\\ui3\\uiitem\\uiitemmain_m.spr",
        "[Main1024]", "Left=400", "Top=10", "Width=%d" % W, "Height=%d" % H, "Moveable=1", "StartPos=800,100", "Trans=1",
        "Image=\\spr\\ui3\\uiitem\\uiitemmain_m.spr",
        "[CloseBtn]", "Left=%d" % NUT_X[2], "Top=%d" % NUT_Y2, "Width=%d" % NUT_W, "Height=%d" % NUT_H, "Up=0", "Down=1", "Over=0", "OverFrame=0",
        "Image=\\spr\\ui3\\uiitem\\nut_hanhtrang_dong.spr",
        "[GetMoneyBtn]", "Left=%d" % NUT_X[0], "Top=%d" % NUT_Y2, "Width=%d" % NUT_W, "Height=%d" % NUT_H, "Up=0", "Down=1", "Over=0", "OverFrame=0",
        "Image=\\spr\\ui3\\uiitem\\nut_hanhtrang_gui_tien.spr",
        "[Money]", "Left=%d" % (LUOI_X + 30), "Top=%d" % (TIEN_Y + 4), "Width=200", "Height=14", "Text=", "Font=14", "HAlign=1", "Color=255,217,78",
        "[FkCoinLable]", "Left=-30", "Top=%d" % (TIEN_Y + 30), "Width=138", "Height=14", "Text=Xu", "Font=14", "HAlign=1", "Color=255,0,0",
        "[FkCoinValue]", "Left=%d" % (LUOI_X + 30), "Top=%d" % (TIEN_Y + 30), "Width=200", "Height=14", "Text=0 Xu", "Font=14", "HAlign=1", "Color=255,0,0",
        "[OpenStatus]", "Left=%d" % NUT_X[1], "Top=%d" % NUT_Y2, "Width=%d" % NUT_W, "Height=%d" % NUT_H, "Up=0", "Down=1", "Over=0", "OverFrame=0",
        "Image=\\spr\\ui3\\uiitem\\nut_hanhtrang_trang_bi.spr",
        "[ItemBox]", "Width=%d" % LUOI_W, "Height=%d" % LUOI_H, "Left=%d" % LUOI_X, "Top=%d" % LUOI_Y, "HUnits=%d" % COT, "VUnits=%d" % HANG, "UnitBorder=%d" % VIEN,
        "[MakeAdvBtn]", "Left=%d" % NUT_X[0], "Top=%d" % NUT_Y1, "Width=%d" % NUT_W, "Height=%d" % NUT_H, "Up=0", "Down=1", "Over=0", "OverFrame=0",
        "Image=\\spr\\ui3\\uiitem\\nut_hanhtrang_loi_rao.spr",
        "[MarkPriceBtn]", "Left=%d" % NUT_X[1], "Top=%d" % NUT_Y1, "Width=%d" % NUT_W, "Height=%d" % NUT_H, "Up=0", "Down=1", "Over=0", "OverFrame=0",
        "Image=\\spr\\ui3\\uiitem\\nut_hanhtrang_dinh_gia.spr",
        "[MakeStallBtn]", "Left=%d" % NUT_X[2], "Top=%d" % NUT_Y1, "Width=%d" % NUT_W, "Height=%d" % NUT_H, "Up=0", "Down=1", "CheckBox=1",
        "Image=\\spr\\ui3\\uiitem\\nut_hanhtrang_rao_ban.spr",
    ]
    # giu nguyen [Settings] cua ini goc
    i = goc.find("[Settings]")
    settings = goc[i:] if i >= 0 else ""
    noi_dung = nl.join(doan) + nl + settings
    for p in (os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uiitem.ini"), r"D:\jx1_android_data\ui\ui3\uiitem.ini"):
        os.makedirs(os.path.dirname(p), exist_ok=True)
        io.open(p, "w", encoding="latin-1", newline="").write(noi_dung)
    print("ini:", os.path.join(GOC, "du_lieu_ghi_de", "ui", "ui3", "uiitem.ini"))
    khung().save(os.path.join(os.environ.get("TEMP", GOC), "hanhtrang_xem.png"))


if __name__ == "__main__":
    main()
