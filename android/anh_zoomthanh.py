# -*- coding: utf-8 -*-
"""[ZOOMTHANH 14/09] Sinh anh thanh keo zoom (KUiZoomThanh) bang PIL, ghi SPR 8 bit (bang mau xam, alpha 8 bit theo TextureRes.cpp):
  spr\\ui3\\uizoomthanh\\thanh.spr  28x160, 1 khung: rai doc mo toi, dau "+" o tren (phong to), dau "-" o duoi (nhin rong), vach nac
  spr\\ui3\\uizoomthanh\\nut.spr    28x28, 2 khung: nut tron (khung 0 thuong, khung 1 dang bam - sang hon)
Ghi vao lop ghi de android\\du_lieu_ghi_de, D:\\jx1_android_data (may ao) va tuy chon thu muc dt_v4 (tham so 1).
Dung: python android\\anh_zoomthanh.py [D:\\jx1_android_data_dt_v4]    (--xem: ghi them PNG xem thu vao scratch)
"""
import os
import struct
import sys

from PIL import Image, ImageDraw

GOC = os.path.dirname(os.path.abspath(__file__))
DICH = [os.path.join(GOC, "du_lieu_ghi_de"), r"D:\jx1_android_data"]
for a in sys.argv[1:]:
    if not a.startswith("--"):
        DICH.append(a)
XEM = "--xem" in sys.argv


def rle_khung_mau(im, idx):
    """RLE theo TextureRes.cpp: (run, a) ; a == 0 = trong suot, a = alpha 8 bit roi run byte chi so bang mau."""
    w, h = im.size
    px = im.load()
    ix = idx.load()
    out = bytearray()
    for y in range(h):
        x = 0
        while x < w:
            a = px[x, y][3]
            q = 0 if a < 8 else a
            n = 1
            while x + n < w and n < 255:
                a2 = px[x + n, y][3]
                if (0 if a2 < 8 else a2) != q:
                    break
                n += 1
            out += bytes((n, q))
            if q:
                out += bytes(ix[x + k, y] for k in range(n))
            x += n
    return bytes(out)


def ghi_spr(cac_khung, path, cx=0, cy=0, itv=0):
    """cac_khung: danh sach anh RGBA cung co. Bang mau chung: luong tu hoa 255 mau tu tat ca khung (xam nen R=G=B, khong lo thu tu RGB/BGR)."""
    w, h = cac_khung[0].size
    ghep = Image.new("RGB", (w, h * len(cac_khung)))
    for i, k in enumerate(cac_khung):
        ghep.paste(k.convert("RGB"), (0, h * i))
    q = ghep.quantize(colors=255, method=Image.Quantize.FASTOCTREE)
    pal = q.getpalette()[:765] + [0] * (768 - min(765, len(q.getpalette()[:765])))
    kb = []
    for i, k in enumerate(cac_khung):
        idx = q.crop((0, h * i, w, h * (i + 1)))
        kb.append(struct.pack("<HHHH", w, h, 0, 0) + rle_khung_mau(k, idx))
    out = bytearray(struct.pack("<4sHHHHHHHH", b"SPR\x00", w, h, cx, cy, len(kb), 256, 1, itv) + b"\x00" * 12)
    out += bytes(pal[:768])
    o = 0
    for k in kb:
        out += struct.pack("<II", o, len(k))
        o += len(k)
    for k in kb:
        out += k
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    return len(out)


def anh_thanh(w=28, h=160):
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    # rai doc mo toi (rong 12, bo tron), vien sang nhe
    d.rounded_rectangle((8, 18, 19, 141), radius=6, fill=(20, 20, 20, 170), outline=(120, 120, 120, 200), width=1)
    # vach nac (14 nac -> 15 vach) tren rai
    for i in range(15):
        y = 22 + i * (138 - 22) // 14
        d.line((11, y, 16, y), fill=(150, 150, 150, 150), width=1)
    # dau + (tren) va - (duoi), tron nen mo
    d.ellipse((5, 1, 22, 18), fill=(30, 30, 30, 160), outline=(140, 140, 140, 200), width=1)
    d.line((9, 9, 18, 9), fill=(235, 235, 235, 255), width=2)
    d.line((13, 5, 13, 14), fill=(235, 235, 235, 255), width=2)
    d.ellipse((5, 141, 22, 158), fill=(30, 30, 30, 160), outline=(140, 140, 140, 200), width=1)
    d.line((9, 149, 18, 149), fill=(235, 235, 235, 255), width=2)
    return im


def anh_nut(sang, w=28, h=28):
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    if sang:
        d.ellipse((2, 2, 25, 25), fill=(245, 245, 245, 255), outline=(60, 60, 60, 255), width=2)
        d.ellipse((9, 9, 18, 18), fill=(120, 120, 120, 255))
    else:
        d.ellipse((3, 3, 24, 24), fill=(200, 200, 200, 245), outline=(40, 40, 40, 255), width=2)
        d.ellipse((10, 10, 17, 17), fill=(90, 90, 90, 255))
    return im


def main():
    thanh = anh_thanh()
    nut = [anh_nut(0), anh_nut(1)]
    for goc in DICH:
        if not os.path.isdir(goc):
            print("khong thay thu muc:", goc)
            continue
        tm = os.path.join(goc, "spr", "ui3", "uizoomthanh")
        n1 = ghi_spr([thanh], os.path.join(tm, "thanh.spr"))
        n2 = ghi_spr(nut, os.path.join(tm, "nut.spr"))
        print("da ghi: %s (thanh %d B, nut %d B)" % (tm, n1, n2))
    if XEM:
        xem = Image.new("RGBA", (28 * 3 + 20, 160), (60, 90, 60, 255))
        xem.alpha_composite(thanh, (5, 0))
        xem.alpha_composite(nut[0], (45, 40))
        xem.alpha_composite(nut[1], (75, 40))
        p = os.path.join(os.environ.get("TEMP", "."), "zoomthanh_xem.png")
        xem.resize((xem.width * 3, xem.height * 3), Image.NEAREST).save(p)
        print("xem:", p)


if __name__ == "__main__":
    main()
