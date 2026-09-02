# -*- coding: utf-8 -*-
"""khung_hs_make.py : ve khung ky nang Hoa Son 366x500 bang PIL (cung bo cuc voi khung_cl.spr do duoc:
   cot khung o: x 19-63 / 89-133 / 159-203 / 231-275 / 301-345 ; hang: y 66-110 / 133-177 / 200-244 / 267-311 / 335-379 / 402-446
   o 36x36 tai (Fly 7 + Skill 18, Fly 57 + Skill 15) = (25,72)... ) roi ma hoa SPR (header 32, KPAL24, RLE [run][alpha][idx..], alpha 255).
  python khung_hs_make.py <out.spr> <preview.png>
"""
import os, struct, sys, io
from PIL import Image, ImageDraw, ImageFont
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
W, H = 366, 500
XS = [18, 88, 158, 230, 299]           # Skill_*_i Left (trong Fly)
YS = [15, 81, 148, 216, 284, 350]      # Skill_*_i Top
FX, FY = 7, 57                          # UiSkillFly.ini [Main] Left/Top

def font(size, bold=True):
    for n in (["arialbd.ttf", "tahomabd.ttf"] if bold else ["arial.ttf", "tahoma.ttf"]):
        p = os.path.join(r"C:\Windows\Fonts", n)
        if os.path.isfile(p): return ImageFont.truetype(p, size)
    return ImageFont.load_default()

def ve():
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # nen: nau sam co vien vang dong
    for y in range(H):
        t = y / H
        c = (int(46 - 14 * t), int(36 - 10 * t), int(26 - 6 * t), 255)
        d.line([(0, y), (W - 1, y)], fill=c)
    d.rectangle([0, 0, W - 1, H - 1], outline=(150, 118, 60, 255), width=2)
    d.rectangle([3, 3, W - 4, H - 4], outline=(90, 70, 36, 255), width=1)
    # thanh tieu de
    d.rectangle([6, 6, W - 7, 40], fill=(28, 22, 16, 255), outline=(120, 95, 48, 255))
    f = font(19)
    tt = "KỸ NĂNG HOA SƠN"
    tw = d.textlength(tt, font=f)
    for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
        d.text(((W - tw) / 2 + dx, 12 + dy), tt, font=f, fill=(0, 0, 0, 255))
    d.text(((W - tw) / 2, 12), tt, font=f, fill=(255, 222, 120, 255))
    # nhan cot
    f2 = font(12)
    labels = ["Kiếm Tông", "Khí Tông", "Hỗ Trợ", "Cấp 120+", "Khinh Công"]
    for i, lb in enumerate(labels):
        cx = FX + XS[i] + 18
        tw = d.textlength(lb, font=f2)
        d.text((cx - tw / 2, 47), lb, font=f2, fill=(230, 210, 150, 255))
    d.line([(8, 63), (W - 9, 63)], fill=(110, 88, 44, 255), width=1)
    # khung o 36x36 (vien 4px ngoai)
    for r, y in enumerate(YS):
        for c, x in enumerate(XS):
            if c == 3 and r > 2: continue      # cot 120+: 3 o (Khi Quan Truong Hong / 120 / tien giai)
            if c == 4 and r > 0: continue      # khinh cong: 1 o
            ax, ay = FX + x, FY + y
            d.rectangle([ax - 4, ay - 4, ax + 39, ay + 39], fill=(22, 18, 12, 255), outline=(160, 128, 66, 255), width=2)
            d.rectangle([ax - 1, ay - 1, ax + 36, ay + 36], fill=(40, 34, 26, 255), outline=(70, 56, 30, 255), width=1)
    # chu thich nho cot Khi Tong hang 6 = tran phai
    f3 = font(10, False)
    d.text((FX + XS[1] - 2, FY + YS[5] + 40), "Trấn phái", font=f3, fill=(200, 180, 120, 255))
    d.text((FX + XS[3] - 4, FY + YS[0] + 40), "Kỹ 50", font=f3, fill=(200, 180, 120, 255))
    d.text((FX + XS[3] - 4, FY + YS[1] + 40), "Cấp 120", font=f3, fill=(200, 180, 120, 255))
    d.text((FX + XS[3] - 4, FY + YS[2] + 40), "Tiến giai", font=f3, fill=(200, 180, 120, 255))
    # dong diem con lai (RemainPoint_10: Left 310 Top 390 trong Fly -> (317,447))
    d.rectangle([FX + 200, FY + 386, FX + 352, FY + 406], fill=(28, 22, 16, 255), outline=(110, 88, 44, 255))
    d.text((FX + 206, FY + 390), "Điểm kỹ năng còn:", font=font(12, False), fill=(230, 210, 150, 255))
    # chan
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
    out += bytes(12)                                   # WORD Reserved[6]
    out += bytes(pal)
    out += struct.pack("<II", 0, len(rle) + 8)
    out += struct.pack("<HHHH", W, H, 0, 0) + bytes(rle)
    open(path, "wb").write(bytes(out))
    print("ghi", path, len(out), "byte")

if __name__ == "__main__":
    img = ve()
    img.save(sys.argv[2]); print("preview", sys.argv[2])
    encode_spr(img, sys.argv[1])
