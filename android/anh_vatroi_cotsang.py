# -*- coding: utf-8 -*-
r"""[VATROI 14/09] Sinh anh hieu ung VAT PHAM ROI (chu: "lam muc 1 truoc: hieu ung anh sang tuy theo loai vat pham" - chep cach game 3D
Kiem Vong Giang Ho: StillObject.mDropLightObj cot sang theo pham chat + cmn_drop_flash loe luc cham dat):
  * spr\vatroi\cotsang.spr : 1 khung 48x120, cot sang TRANG mo dan len tren + vung sang o chan; neo (cx, cy) = (12, 100): chan cot (24, 112)
                             roi vao TAM icon vat pham 24x24 (14/09 c: icon dung thang len tren diem do) (icon co cx=cy=0, goc trai tren tai diem dat). Mau: KObj::Draw ve bang
                             IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST nhan mau ten vat pham (pham chat) -> mot anh trang dung cho moi mau.
  * spr\vatroi\loe.spr     : 6 khung 64x48, vong elip 2:1 dan ra + loe giua (khung dau), tam (32, 32), neo (20, 20) = tam vong tai tam icon.
Dinh dang SPR (theo bo_cuc_vnku_mobile.doc_spr): "SPR\0" + W H cx cy frames colors dirs itv (8 x u16) + 12 byte 0, bang mau colors x 3,
bang (offset, len) moi khung, moi khung: fw fh ox oy (4 x u16) + RLE tung dong: (run, alpha 8 bit 0..255) [+ run chi so neu alpha > 0].
Ghi vao lop ghi de android\du_lieu_ghi_de\spr\vatroi\, D:\jx1_android_data\spr\vatroi\ (may ao) va tuy chon goi may chu tai (tham so 1,
nho --chi-manifest). Xem truoc: scratchpad/vatroi_xemtruoc.png (tham so --xem <duong dan png>).
Dung: python android\anh_vatroi_cotsang.py [D:\jx1_android_data_dt_v4] [--xem out.png]
"""
import math
import os
import struct
import sys

from PIL import Image, ImageDraw, ImageFilter

GOC = os.path.dirname(os.path.abspath(__file__))
DICH = [os.path.join(GOC, "du_lieu_ghi_de", "spr", "vatroi"), os.path.join(r"D:\jx1_android_data", "spr", "vatroi")]
XEM = None
args = [a for a in sys.argv[1:]]
if "--xem" in args:
    i = args.index("--xem"); XEM = args[i + 1]; del args[i:i + 2]
if args:
    DICH.append(os.path.join(args[0], "spr", "vatroi"))


def cot_sang(w=48, h=120):
    """Cot sang trang: ngang = Gauss quanh tam (rong hon o chan), doc = mo dan len tren; chan co vung sang elip."""
    im = Image.new("RGBA", (w, h), (255, 255, 255, 0))
    px = im.load()
    cx = (w - 1) / 2.0
    chan = h - 8                     # y cua diem dat (neo cy)
    for y in range(h):
        t = 1.0 - min(1.0, max(0.0, (chan - y) / float(chan)))     # 0 tren dinh .. 1 o chan
        sigma = 4.5 + 6.0 * t                                      # cot loe rong dan xuong chan
        doc = t ** 1.6                                             # mo manh o tren
        for x in range(w):
            g = math.exp(-((x - cx) ** 2) / (2 * sigma * sigma))
            a = 230 * g * doc
            if y > chan:
                a *= max(0.0, 1.0 - (y - chan) / 8.0)
            px[x, y] = (255, 255, 255, int(a))
    # vung sang o chan (elip 2:1)
    glow = Image.new("RGBA", (w, h), (255, 255, 255, 0))
    dg = ImageDraw.Draw(glow)
    for r in range(6):
        rx, ry = 22 - r * 3, 7 - r
        dg.ellipse((cx - rx, chan - ry, cx + rx, chan + ry), fill=(255, 255, 255, 40 + r * 30))
    glow = glow.filter(ImageFilter.GaussianBlur(1.5))
    im.alpha_composite(glow)
    return im


def loe_khung(i, w=64, h=48):
    """Khung i (0..5): vong elip 2:1 dan ra + loe giua o khung dau."""
    im = Image.new("RGBA", (w, h), (255, 255, 255, 0))
    d = ImageDraw.Draw(im)
    cx, cy = w / 2.0, 32.0            # tam vong o (32, 32): neo (20, 8) = tam vong tai giua-duoi icon 24x24
    rx = 7 + i * 5
    ry = rx / 2.0
    a = 235 - i * 36
    day = max(1, 4 - i // 2)
    d.ellipse((cx - rx, cy - ry, cx + rx, cy + ry), outline=(255, 255, 255, a), width=day)
    if i < 2:
        b = 200 - i * 90
        d.ellipse((cx - 9 + i * 2, cy - 4 + i, cx + 9 - i * 2, cy + 4 - i), fill=(255, 255, 255, b))
    return im.filter(ImageFilter.GaussianBlur(0.8))


def rle_khung(im):
    """RGBA -> (bang mau la trang thuan: chi so 0) + RLE (run, alpha 8 bit 0..255)[+ chi so]. Anh trang nen bang mau 1 mau."""
    w, h = im.size
    px = im.load()
    out = bytearray()
    for y in range(h):
        x = 0
        while x < w:
            a = px[x, y][3]
            q = 0 if a < 8 else a          # [SUA 14/09] alpha SPR la 8 bit 0..255 (TextureRes.cpp RenderToA8R8G8B8: a << 24), khong phai 0..31
            n = 1
            while x + n < w and n < 255:
                a2 = px[x + n, y][3]
                q2 = 0 if a2 < 8 else a2
                if q2 != q:
                    break
                n += 1
            out += bytes((n, q))
            if q:
                out += b"\x00" * n          # chi so bang mau 0 = trang
            x += n
    return bytes(out)


def ghi_spr(cac_khung, path, cx, cy):
    """Ghi SPR: bang mau 256 (mau 0 trang), tung khung cung co, neo (cx, cy)."""
    w, h = cac_khung[0].size
    pal = bytes([255, 255, 255]) + b"\x00" * (768 - 3)
    kb = [struct.pack("<HHHH", w, h, 0, 0) + rle_khung(k) for k in cac_khung]
    out = bytearray(struct.pack("<4sHHHHHHHH", b"SPR\x00", w, h, cx, cy, len(kb), 256, 1, 1) + b"\x00" * 12)
    out += pal
    o = 0
    for k in kb:
        out += struct.pack("<II", o, len(k)); o += len(k)
    for k in kb:
        out += k
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    return len(out)


def doc_spr(path):
    """Doc lai de kiem (theo bo_cuc_vnku_mobile.doc_spr)."""
    d = open(path, "rb").read()
    assert d[:3] == b"SPR"
    W, H, cx, cy, frames, colors, dirs, itv = struct.unpack("<HHHHHHHH", d[4:20])
    pos = 32
    pal = [(d[pos + 3 * i], d[pos + 3 * i + 1], d[pos + 3 * i + 2]) for i in range(colors)]
    pos += colors * 3
    offs = [struct.unpack("<II", d[pos + 8 * i:pos + 8 * i + 8]) for i in range(frames)]
    base = pos + frames * 8
    ra = []
    for fo, fl in offs:
        p = base + fo
        fw, fh, ox, oy = struct.unpack("<HHHH", d[p:p + 8]); p += 8
        img = Image.new("RGBA", (fw, fh), (0, 0, 0, 0)); px = img.load()
        x = y = 0; end = base + fo + fl
        while y < fh and p < end:
            run, alpha = d[p], d[p + 1]; p += 2
            if alpha == 0:
                x += run
            else:
                for _ in range(run):
                    idx = d[p]; p += 1
                    if x < fw:
                        r, g, b = pal[idx]
                        px[x, y] = (r, g, b, alpha)
                    x += 1
            if x >= fw:
                x, y = 0, y + 1
        ra.append(img)
    return (W, H, cx, cy), ra


def main():
    cot = cot_sang()
    loe = [loe_khung(i) for i in range(6)]
    for dich in DICH:
        # neo (cx, cy): Represent3 REF_SPOT dat anh tai (pos - cx, pos - cy); anh vat pham roi la icon 24x24 cx=cy=0 (goc trai tren tai
        # diem dat) -> chan cot (24, 112) va tam vong (32, 32) phai roi vao (12, 24) cua icon -> cx = 24-12, cy = 112-24 / 32-12, 32-24
        n1 = ghi_spr([cot], os.path.join(dich, "cotsang.spr"), 12, 100)   # [VATROI 14/09 c] chan cot tai TAM icon (x+12, y+12)
        n2 = ghi_spr(loe, os.path.join(dich, "loe.spr"), 20, 20)          # tam vong loe tai tam icon
        print("da ghi:", dich, "cotsang.spr %d B, loe.spr %d B" % (n1, n2))
    # kiem doc lai
    hdr, ks = doc_spr(os.path.join(DICH[0], "cotsang.spr"))
    hdr2, ks2 = doc_spr(os.path.join(DICH[0], "loe.spr"))
    print("kiem: cotsang", hdr, ks[0].size, "| loe", hdr2, len(ks2), "khung", ks2[0].size)
    if XEM:
        # xem truoc: nen dat toi + cot sang nhuom 4 mau (xanh, hoang kim, do tim, bach kim) + 6 khung loe
        mau = [(120, 220, 120), (255, 215, 80), (230, 120, 255), (200, 240, 255)]
        W, H = 48 * 4 + 64 * 6 + 60, 150
        nen = Image.new("RGBA", (W, H), (40, 36, 30, 255))
        for i, m in enumerate(mau):
            t = ks[0].copy(); r, g, b, a = t.split()
            t = Image.merge("RGBA", (r.point(lambda v, m=m: m[0]), g.point(lambda v, m=m: m[1]), b.point(lambda v, m=m: m[2]), a))
            nen.alpha_composite(t, (10 + i * 48, 15))
        for i, k in enumerate(ks2):
            nen.alpha_composite(k, (10 + 48 * 4 + 20 + i * 64, 60))
        nen.convert("RGB").save(XEM)
        print("xem truoc:", XEM)


if __name__ == "__main__":
    main()
