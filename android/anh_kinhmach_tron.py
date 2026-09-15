# -*- coding: utf-8 -*-
r"""[KMTRON 14/09] Chu: "icon kinh mach o man hinh dang hinh vuong, toi muon lam lai hinh tron".

Icon kinh_mach_m.spr (48x48, 2 khung) la anh VUONG nen dung canh cac icon TRON khac (Ngoi / To doi / Tong kim)
thi choi. Kich ban nay doc chinh tep do, cat thanh HINH TRON va them vong vien vang giong cac icon kia, ghi de
lai (giu nguyen ten, co, so khung nen khong phai dong ini).

  - cat tron: ban kinh 23 tren anh 48x48, mem bien (4 muc alpha) cho khoi rang cua;
  - vong vien: vong vang #F0D070 day 2 px + vien trong toi #2A1E0A day 1 px, dung mau lay tu icon tron san co;
  - giu 2 khung goc (khung 0 = thuong, khung 1 = dang bam).

Luon chay lai duoc: neu tep da tron (goc anh da trong suot) thi bao va khong ghi de.
Ghi vao lop ghi de android\du_lieu_ghi_de, D:\jx1_android_data (may ao) va tuy chon thu muc dt_v4 (tham so 1).
Dung: python android\anh_kinhmach_tron.py [D:\jx1_android_data_dt_v4]    (--xem: ghi PNG xem thu)
"""
import io
import os
import struct
import sys

from PIL import Image, ImageDraw

GOC = os.path.dirname(os.path.abspath(__file__))
REL = os.path.join("spr", "UiNew", "UiToolsControlBar", "kinh_mach_m.spr")
DICH = [os.path.join(GOC, "du_lieu_ghi_de"), r"D:\jx1_android_data"]
for a in sys.argv[1:]:
    if not a.startswith("--"):
        DICH.append(a)
XEM = "--xem" in sys.argv

VANG = (240, 208, 112)
TOI = (42, 30, 10)


def doc_spr(path):
    """-> (W, H, cx, cy, itv, [anh RGBA moi khung])"""
    d = open(path, "rb").read()
    assert d[:3] == b"SPR", path
    W, H, cx, cy, frames, colors, dirs, itv = struct.unpack("<HHHHHHHH", d[4:20])
    pos = 32
    pal = [(d[pos + 3 * i], d[pos + 3 * i + 1], d[pos + 3 * i + 2]) for i in range(colors)]
    pos += colors * 3
    offs = [struct.unpack("<II", d[pos + 8 * i:pos + 8 * i + 8]) for i in range(frames)]
    base = pos + frames * 8
    ra = []
    for fo, fl in offs:
        p = base + fo
        fw, fh, ox, oy = struct.unpack("<HHHH", d[p:p + 8])
        p += 8
        img = Image.new("RGBA", (fw, fh), (0, 0, 0, 0))
        px = img.load()
        x = y = 0
        end = base + fo + fl
        while y < fh and p < end:
            run, alpha = d[p], d[p + 1]
            p += 2
            if alpha == 0:
                x += run
            else:
                for _ in range(run):
                    idx = d[p]
                    p += 1
                    if x < fw:
                        r, g, b = pal[idx] if idx < len(pal) else (255, 0, 255)
                        # alpha trong tep nay la 8 bit (xem MEMORY spr-alpha-8bit); ban cu ghi 0..31 thi >= 31 coi la dac
                        px[x, y] = (r, g, b, 255 if alpha >= 248 else (255 if alpha == 31 else alpha))
                    x += 1
            if x >= fw:
                x, y = 0, y + 1
        ra.append(img)
    return W, H, cx, cy, itv, ra


def rle_khung(im, idx):
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


def ghi_spr(cac_khung, path, W, H, cx, cy, itv):
    ghep = Image.new("RGB", (W, H * len(cac_khung)))
    for i, k in enumerate(cac_khung):
        ghep.paste(k.convert("RGB"), (0, H * i))
    q = ghep.quantize(colors=255, method=Image.Quantize.FASTOCTREE)
    pal = (q.getpalette() or [])[:765]
    pal += [0] * (768 - len(pal))
    kb = []
    for i, k in enumerate(cac_khung):
        kb.append(struct.pack("<HHHH", W, H, 0, 0) + rle_khung(k, q.crop((0, H * i, W, H * (i + 1)))))
    out = bytearray(struct.pack("<4sHHHHHHHH", b"SPR\x00", W, H, cx, cy, len(kb), 256, 1, itv) + b"\x00" * 12)
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


def lam_tron(im):
    w, h = im.size
    r = min(w, h) / 2.0 - 1.0
    cx, cy = w / 2.0, h / 2.0
    ra = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    src = im.load()
    dst = ra.load()
    for y in range(h):
        for x in range(w):
            d = ((x + 0.5 - cx) ** 2 + (y + 0.5 - cy) ** 2) ** 0.5
            if d > r:
                continue
            p = src[x, y]
            a = p[3]
            if d > r - 1.5:                       # mem bien cho khoi rang cua
                a = int(a * (r - d) / 1.5)
            if a <= 0:
                continue
            dst[x, y] = (p[0], p[1], p[2], min(255, a))
    d = ImageDraw.Draw(ra)
    d.ellipse((1, 1, w - 2, h - 2), outline=VANG + (255,), width=2)
    d.ellipse((3, 3, w - 4, h - 4), outline=TOI + (150,), width=1)
    return ra


def main():
    goc = os.path.join(DICH[0], REL)
    if not os.path.isfile(goc):
        raise SystemExit("khong thay " + goc)
    W, H, cx, cy, itv, ra = doc_spr(goc)
    if ra[0].getpixel((0, 0))[3] == 0 and ra[0].getpixel((W - 1, 0))[3] == 0:
        print("bo qua: icon da tron (bon goc trong suot)")
        return
    tron = [lam_tron(k) for k in ra]
    for g in DICH:
        p = os.path.join(g, REL)
        if not os.path.isdir(os.path.dirname(p)):
            print("khong thay thu muc:", os.path.dirname(p))
            continue
        n = ghi_spr(tron, p, W, H, cx, cy, itv)
        print("da ghi: %s (%d byte, %d khung %dx%d)" % (p, n, len(tron), W, H))
    if XEM:
        xem = Image.new("RGBA", (W * 2 + 30, H + 10), (60, 60, 60, 255))
        xem.alpha_composite(ra[0], (5, 5))
        xem.alpha_composite(tron[0], (W + 25, 5))
        t = os.path.join(os.environ.get("TEMP", "."), "kinhmach_tron.png")
        xem.resize((xem.width * 4, xem.height * 4), Image.NEAREST).save(t)
        print("xem (trai = cu, phai = moi):", t)


if __name__ == "__main__":
    main()
