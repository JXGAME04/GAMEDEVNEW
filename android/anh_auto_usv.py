# -*- coding: utf-8 -*-
r"""[AUTO 12/09 HOA] Chu 17:55 gui anh va bao: "ban lay icon nay thay vao Auto duoc", "ban da thay icon Auto
thanh hinh BONG HOA cho toi dau", "anh nay ne".

Anh chu gui la BONG HOA DO la xanh, 18x20, co san trong kho VNKU:
    VNKU_ui\png\_khong_ro_ten\uid2DA73294_0284.png   (trung y het uidE776E440_1757.png - cung mot anh, hai ma)
Lay NGUYEN anh do lam icon Auto tren thanh cong cu, chi PHONG DOI (2x, kieu diem anh - khong lam nhoe net ve
tay), roi can giua o 48x48:
    spr\uinew\uitoolscontrolbar\auto_m.spr   48x48, 2 khung
      [0] auto TAT : bong hoa lam MO di (nhu moi nut khac cua thanh cong cu: xam = dang tat)
      [1] auto BAT : bong hoa nguyen ban
Muon hai khung y het nhau (khong phan biet bat / tat) thi dat MO_KHI_TAT = 0.

Truoc do toi tu GHEP vong tron kiem cheo VNKU voi chu "Auto" - do la CHE, chu khong nhan; roi doan nham sang nut
tron do "CONFIG AUTO" cua kho GHM. Lan nay lay dung anh chu gui.
Nguon CHI DOC. Dung: python android\anh_auto_usv.py
"""
import io
import os
import sys

from PIL import Image

GOC = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, GOC)
from bo_cuc_vnku_mobile import ghi_spr_nhieu_khung, ghi_moi_noi  # noqa: E402

#   kho tham khao CHI DOC (xem MEMORY: jx1-mobile-nguon-tham-khao)
KHO = os.path.join(os.path.expanduser("~"), "Downloads", "NHACTAI", "VNKU_ui", "png", "_khong_ro_ten")
E_HOA = "uid2DA73294_0284.png"
ICON = 48
PHONG = 2               # phong theo LAN nguyen (diem anh sac net), 18x20 -> 36x40
MO_KHI_TAT = 55         # khung "auto tat" sang bao nhieu % (0 = tat han buoc lam mo, hai khung y het nhau)


def doc_hoa():
    p = os.path.join(KHO, E_HOA)
    if not os.path.isfile(p):
        raise SystemExit("khong thay %s" % p)
    im = Image.open(p).convert("RGBA")
    bb = im.getchannel("A").point(lambda a: 255 if a > 20 else 0).getbbox()
    if bb:
        im = im.crop(bb)
    return im.resize((im.size[0] * PHONG, im.size[1] * PHONG), Image.NEAREST)


def vao_khung(im):
    o = Image.new("RGBA", (ICON, ICON), (0, 0, 0, 0))
    if im.size[0] > ICON or im.size[1] > ICON:
        im = im.copy()
        im.thumbnail((ICON, ICON), Image.NEAREST)
    o.alpha_composite(im, ((ICON - im.size[0]) // 2, (ICON - im.size[1]) // 2))
    return o


def lam_mo(im, pt):
    """Khung 'dang tat': giu nguyen hinh, chi ha do sang - giong cach cac nut khac tren thanh cong cu."""
    o = im.copy()
    px = o.load()
    for y in range(o.size[1]):
        for x in range(o.size[0]):
            r, g, b, a = px[x, y]
            px[x, y] = (r * pt // 100, g * pt // 100, b * pt // 100, a)
    return o


def main():
    hoa = vao_khung(doc_hoa())
    khung = [lam_mo(hoa, MO_KHI_TAT) if MO_KHI_TAT else hoa, hoa]
    rel = os.path.join("spr", "uinew", "uitoolscontrolbar", "auto_m.spr")
    tmp = os.path.join(os.environ.get("TEMP", GOC), "auto_hoa_tmp.spr")
    msg = ghi_spr_nhieu_khung(khung, tmp)
    print("%-48s %dx%d x%d  %s" % (rel, ICON, ICON, len(khung), msg))
    ghi_moi_noi(rel, io.open(tmp, "rb").read())
    for i, k in enumerate(khung):
        k.save(os.path.join(os.environ.get("TEMP", GOC), "auto_hoa_%d.png" % i))
    print("xong")


if __name__ == "__main__":
    main()
