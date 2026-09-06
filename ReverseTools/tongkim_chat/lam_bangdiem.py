# -*- coding: utf-8 -*-
"""[TKDIEM 04/09] Ve sprite BANG DIEM "Tong VS Kim" (300x64) bang Pillow roi ghi SPR (8-bit palette + RLE alpha).
Bo cuc theo bang diem cua client 2.0 (KUiHuaihehepan): TONG trai (vang cam), VS giua (tim hong), KIM phai (xanh);
so diem va thanh can bang do client ve (KWndText / KWndShadow) o phan duoi.

Format SPR (Sources/Engine/Src/KSprite.h): SPRHEAD 32 byte {"SPR\0", W, H, CenterX, CenterY, Frames, Colors,
Directions, Interval, Reserved[6]} | KPAL24[Colors] | SPROFFS[Frames]{Offset, Length} | SPRFRAME{W,H,OffX,OffY}+RLE.
RLE moi dong: [run][alpha] (+ run chi so palette neu alpha > 0), alpha 0..255 (255 = dac).
(Tool cu p15_vietsub_spr.py doc header 28 byte + "4 byte dem" sau palette = lech 4 byte so voi struct that.)
Chay: python lam_bangdiem.py [dich.spr]  -> ghi SPR + PNG xem truoc.
"""
import os, struct, sys
from PIL import Image, ImageDraw, ImageFont, ImageFilter

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
OUT_SPR = sys.argv[1] if len(sys.argv) > 1 else r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\spr\Ui3\UiGameMain\UiTongKim\bangdiem.spr"
OUT_PNG = os.path.join(os.path.dirname(os.path.abspath(__file__)), "png", "bangdiem_preview.png")
W, H = 300, 64
SCALE = 3          # ve lon roi thu nho -> chu min
FONT_DIR = r"C:\Windows\Fonts"


def font(name, size):
    p = os.path.join(FONT_DIR, name)
    return ImageFont.truetype(p, size) if os.path.isfile(p) else ImageFont.truetype(os.path.join(FONT_DIR, "arialbd.ttf"), size)


def rounded(draw, box, r, fill, outline=None, width=1):
    draw.rounded_rectangle(box, radius=r, fill=fill, outline=outline, width=width)


def glow_text(base, xy, text, fnt, fill, glow, glow_r, outline=(0, 0, 0, 255), ow=2, anchor="mm"):
    """chu co vien den + quang sang mem."""
    layer = Image.new("RGBA", base.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    d.text(xy, text, font=fnt, fill=glow, anchor=anchor)
    layer = layer.filter(ImageFilter.GaussianBlur(glow_r))
    base.alpha_composite(layer)
    d = ImageDraw.Draw(base)
    d.text(xy, text, font=fnt, fill=outline, anchor=anchor, stroke_width=ow, stroke_fill=outline)
    d.text(xy, text, font=fnt, fill=fill, anchor=anchor)


def ve():
    w, h = W * SCALE, H * SCALE
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # nen: tam mau dam mo, vien vang
    rounded(d, (2 * SCALE, 2 * SCALE, w - 2 * SCALE - 1, h - 2 * SCALE - 1), 10 * SCALE, (12, 14, 26, 190), (196, 150, 60, 255), 2 * SCALE)
    rounded(d, (5 * SCALE, 5 * SCALE, w - 5 * SCALE - 1, h - 5 * SCALE - 1), 8 * SCALE, None, (120, 90, 30, 200), 1 * SCALE)
    # hai o tieu de Tong / Kim (tam mau nhe)
    rounded(d, (8 * SCALE, 6 * SCALE, 116 * SCALE, 31 * SCALE), 6 * SCALE, (90, 30, 20, 170), (220, 150, 60, 220), 1 * SCALE)
    rounded(d, (184 * SCALE, 6 * SCALE, 292 * SCALE, 31 * SCALE), 6 * SCALE, (20, 40, 90, 170), (100, 170, 255, 220), 1 * SCALE)
    f_big = font("arialbd.ttf", 23 * SCALE)   # Arial Bold co du dau tieng Viet (Georgia thieu glyph "o dau")
    f_vs = font("impact.ttf", 26 * SCALE)
    glow_text(img, (62 * SCALE, 18 * SCALE), "Tống", f_big, (255, 214, 90, 255), (255, 90, 20, 200), 4 * SCALE)
    glow_text(img, (238 * SCALE, 18 * SCALE), "Kim", f_big, (170, 220, 255, 255), (40, 120, 255, 200), 4 * SCALE)
    glow_text(img, (150 * SCALE, 19 * SCALE), "VS", f_vs, (255, 110, 230, 255), (200, 40, 220, 220), 5 * SCALE, ow=2 * SCALE)
    # ranh thanh can bang (client ve hai KWndShadow len tren): x 112..188, y 42..50
    rounded(d, (110 * SCALE, 40 * SCALE, 190 * SCALE - 1, 52 * SCALE - 1), 3 * SCALE, (0, 0, 0, 200), (196, 150, 60, 255), 1 * SCALE)
    # vien mo cho hai o so (client ve so len)
    rounded(d, (14 * SCALE, 35 * SCALE, 110 * SCALE, 57 * SCALE), 5 * SCALE, (0, 0, 0, 120), (140, 100, 40, 160), 1 * SCALE)
    rounded(d, (190 * SCALE, 35 * SCALE, 286 * SCALE, 57 * SCALE), 5 * SCALE, (0, 0, 0, 120), (60, 110, 180, 160), 1 * SCALE)
    img = img.resize((W, H), Image.LANCZOS)
    return img


def ghi_spr(img, path):
    W_, H_ = img.size
    px = img.load()
    # palette 256 mau tu cac diem co alpha > 0 (nen mo van can mau dung)
    rgb = Image.new("RGB", (W_, H_), (0, 0, 0))
    prgb = rgb.load()
    for y in range(H_):
        for x in range(W_):
            r, g, b, a = px[x, y]
            prgb[x, y] = (r, g, b) if a > 0 else (0, 0, 0)
    pal_img = rgb.quantize(colors=256, method=Image.MEDIANCUT, dither=Image.NONE)
    # [SPRFIX 06/09] getpalette() CHI tra ve so mau anh that su dung; header khai Colors = 256 nen thieu byte
    # -> bo nap doc lech vao du lieu RLE -> Offset rac -> SAP client (06/09 09:48). Xem ghi_spr.py.
    pal = list(pal_img.getpalette() or [])[:768]
    pal += [0] * (768 - len(pal))
    idx = pal_img.load()
    # RLE tung dong: nhom theo (alpha luong tu 16 muc) - alpha 0 = trong suot
    rle = bytearray()
    for y in range(H_):
        x = 0
        while x < W_:
            a0 = px[x, y][3]
            a0 = 0 if a0 < 8 else (255 if a0 >= 248 else (a0 // 16) * 16 + 8)
            n = 0
            while x + n < W_ and n < 255:
                a = px[x + n, y][3]
                a = 0 if a < 8 else (255 if a >= 248 else (a // 16) * 16 + 8)
                if a != a0:
                    break
                n += 1
            rle += bytes((n, a0))
            if a0 > 0:
                for k in range(n):
                    rle.append(idx[x + k, y])
            x += n
    frame = struct.pack("<HHHH", W_, H_, 0, 0) + bytes(rle)
    out = bytearray()
    out += struct.pack("<4sHHHHHHHH", b"SPR\x00", W_, H_, 0, 0, 1, 256, 1, 1)
    out += b"\x00" * 12                                  # Reserved[6] WORD
    out += bytes(pal)                                    # KPAL24[256]
    out += struct.pack("<II", 0, len(frame))             # SPROFFS[1]
    out += frame
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    return len(out)


if __name__ == "__main__":
    img = ve()
    os.makedirs(os.path.dirname(OUT_PNG), exist_ok=True)
    img.save(OUT_PNG)
    n = ghi_spr(img, OUT_SPR)
    print("ghi", OUT_SPR, n, "byte; xem truoc", OUT_PNG)
