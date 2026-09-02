# -*- coding: utf-8 -*-
"""khung_vhtd_make.py : ve khung ky nang Vu Hon / Tieu Dao 366x500 (cung bo cuc khung_hs.spr: XS/YS o 36x36 theo [Skill_N_i]) + ma hoa SPR.
  python khung_vhtd_make.py <wh|xy> <out.spr> [preview.png]
"""
import os, struct, sys, io
from PIL import Image, ImageDraw, ImageFont
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
W, H = 366, 500
XS = [18, 88, 158, 230, 299]
YS = [15, 81, 148, 216, 284, 350]
FX, FY = 7, 57

CFG = {
    "wh": dict(title="KỸ NĂNG VŨ HỒN", labels=["Thuẫn Pháp", "Đao Pháp", "Hỗ Trợ", "Cấp 120+"], rows=[5, 6, 6, 2],
               notes={(0, 4): "Kỹ 150", (1, 4): "Kỹ 90", (1, 5): "Kỹ 150", (2, 5): "Trấn phái", (3, 0): "Cấp 120", (3, 1): "Tiến giai"}),
    "xy": dict(title="KỸ NĂNG TIÊU DAO", labels=["Kiếm Pháp", "Cầm Pháp", "Hỗ Trợ", "Cấp 120+"], rows=[5, 6, 5, 2],
               notes={(0, 4): "Kỹ 150", (1, 3): "Kỹ 90", (1, 4): "Kỹ 150", (1, 5): "Trấn phái", (3, 0): "Cấp 120", (3, 1): "Tiến giai"}),
}

def font(size, bold=True):
    for n in (["arialbd.ttf", "tahomabd.ttf"] if bold else ["arial.ttf", "tahoma.ttf"]):
        p = os.path.join(r"C:\Windows\Fonts", n)
        if os.path.isfile(p): return ImageFont.truetype(p, size)
    return ImageFont.load_default()

def ve(cfg):
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for y in range(H):
        t = y / H
        d.line([(0, y), (W - 1, y)], fill=(int(46 - 14 * t), int(36 - 10 * t), int(26 - 6 * t), 255))
    d.rectangle([0, 0, W - 1, H - 1], outline=(150, 118, 60, 255), width=2)
    d.rectangle([3, 3, W - 4, H - 4], outline=(90, 70, 36, 255), width=1)
    d.rectangle([6, 6, W - 7, 40], fill=(28, 22, 16, 255), outline=(120, 95, 48, 255))
    f = font(19); tt = cfg["title"]; tw = d.textlength(tt, font=f)
    for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
        d.text(((W - tw) / 2 + dx, 12 + dy), tt, font=f, fill=(0, 0, 0, 255))
    d.text(((W - tw) / 2, 12), tt, font=f, fill=(255, 222, 120, 255))
    f2 = font(12)
    for i, lb in enumerate(cfg["labels"]):
        cx = FX + XS[i] + 18; tw = d.textlength(lb, font=f2)
        d.text((cx - tw / 2, 47), lb, font=f2, fill=(230, 210, 150, 255))
    d.line([(8, 63), (W - 9, 63)], fill=(110, 88, 44, 255), width=1)
    for c, nrows in enumerate(cfg["rows"]):
        for r in range(nrows):
            ax, ay = FX + XS[c], FY + YS[r]
            d.rectangle([ax - 4, ay - 4, ax + 39, ay + 39], fill=(22, 18, 12, 255), outline=(160, 128, 66, 255), width=2)
            d.rectangle([ax - 1, ay - 1, ax + 36, ay + 36], fill=(40, 34, 26, 255), outline=(70, 56, 30, 255), width=1)
    f3 = font(10, False)
    for (c, r), txt in cfg["notes"].items():
        d.text((FX + XS[c] - 4, FY + YS[r] + 40), txt, font=f3, fill=(200, 180, 120, 255))
    d.rectangle([FX + 200, FY + 386, FX + 352, FY + 406], fill=(28, 22, 16, 255), outline=(110, 88, 44, 255))
    d.text((FX + 206, FY + 390), "Điểm kỹ năng còn:", font=font(12, False), fill=(230, 210, 150, 255))
    d.line([(8, H - 34), (W - 9, H - 34)], fill=(110, 88, 44, 255), width=1)
    return img

def encode_spr(img, path):
    q = img.convert("RGB").quantize(colors=256, method=Image.Quantize.MEDIANCUT)
    pal = q.getpalette()[:768]
    idx = q.load(); a = img.split()[3].load()
    rle = bytearray()
    for y in range(H):
        x = 0
        while x < W:
            n = 0
            while x + n < W and a[x + n, y] < 64 and n < 255: n += 1
            if n: rle += bytes((n, 0)); x += n; continue
            n = 0
            while x + n < W and a[x + n, y] >= 64 and n < 255: n += 1
            rle += bytes((n, 255))
            for k in range(n): rle.append(idx[x + k, y])
            x += n
    out = bytearray()
    out += struct.pack("<4sHHHHHHHH", b"SPR\x00", W, H, 0, 0, 1, 256, 1, 0)
    out += bytes(12)
    out += bytes(pal)
    out += struct.pack("<II", 0, len(rle) + 8)
    out += struct.pack("<HHHH", W, H, 0, 0) + bytes(rle)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    print("ghi", path, len(out), "byte")

if __name__ == "__main__":
    cfg = CFG[sys.argv[1]]
    img = ve(cfg)
    if len(sys.argv) > 3: img.save(sys.argv[3]); print("preview", sys.argv[3])
    encode_spr(img, sys.argv[2])
