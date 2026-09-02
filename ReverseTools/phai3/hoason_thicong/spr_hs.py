# -*- coding: utf-8 -*-
"""spr_hs.py : bo doc/ghi SPR theo DUNG KSprite.h (header 32 byte: 20 + WORD Reserved[6]),
palette KPAL24 tai 32, bang SPROFFS tai 32+colors*3, frame {W,H,offX,offY}+RLE [run][alpha](+idx*run).
  python spr_hs.py dump <in.spr> <out.png>
  python spr_hs.py make <in.spr> <out.spr> <out.png>   : tao khung Hoa Son tu khung Con Lon
"""
import os, struct, sys, io
from PIL import Image, ImageDraw, ImageFont
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
FONT_DIR = r"C:\Windows\Fonts"

def font(size, bold=True):
    for name in (["arialbd.ttf", "tahomabd.ttf"] if bold else ["arial.ttf", "tahoma.ttf"]):
        p = os.path.join(FONT_DIR, name)
        if os.path.isfile(p):
            return ImageFont.truetype(p, size)
    return ImageFont.load_default()

class Spr:
    def __init__(self, path):
        b = open(path, "rb").read()
        (com, self.w, self.h, self.cx, self.cy, self.frames, self.colors, self.dirs, self.itv) = struct.unpack_from("<4sHHHHHHHH", b, 0)
        assert com[:3] == b"SPR", "khong phai SPR"
        self.reserved = b[20:32]                      # WORD Reserved[6]
        off = 32
        self.pal = [tuple(b[off + 3 * i: off + 3 * i + 3]) for i in range(self.colors)]
        off += 3 * self.colors
        self.offs = [struct.unpack_from("<II", b, off + 8 * i) for i in range(self.frames)]
        base = off + 8 * self.frames
        self.imgs = []
        self.frame_hdr = []
        for fi in range(self.frames):
            fo, fl = self.offs[fi]
            p = base + fo
            fw, fh, ox, oy = struct.unpack_from("<HHHH", b, p)
            self.frame_hdr.append((fw, fh, ox, oy))
            p += 8
            end = min(len(b), base + fo + fl)
            img = Image.new("RGBA", (self.w, self.h), (0, 0, 0, 0))
            px = img.load()
            x = y = 0
            while p + 1 < end and y < fh:
                run, alpha = b[p], b[p + 1]
                p += 2
                if alpha == 0:
                    x += run
                else:
                    a = 255 if alpha >= 31 else max(8, alpha * 255 // 31)
                    for k in range(run):
                        idx = b[p]; p += 1
                        if x < fw and ox + x < self.w and oy + y < self.h:
                            r, g, bb = self.pal[idx]
                            px[ox + x, oy + y] = (r, g, bb, a)
                        x += 1
                while x >= fw and fw > 0:
                    x -= fw; y += 1
            self.imgs.append(img)
        self._cache = {}

    def near(self, rgb):
        if rgb in self._cache: return self._cache[rgb]
        r, g, bb = rgb
        best, bd = 0, 1 << 30
        for i, (pr, pg, pb) in enumerate(self.pal):
            d = (pr - r) ** 2 + (pg - g) ** 2 + (pb - bb) ** 2
            if d < bd: bd, best = d, i
        self._cache[rgb] = best
        return best

    def save(self, path):
        blobs = []
        for img in self.imgs:
            px = img.load()
            rle = bytearray()
            for y in range(self.h):
                x = 0
                while x < self.w:
                    n = 0
                    while x + n < self.w and px[x + n, y][3] < 64 and n < 255: n += 1
                    if n:
                        rle += bytes((n, 0)); x += n; continue
                    n = 0
                    while x + n < self.w and px[x + n, y][3] >= 64 and n < 255: n += 1
                    rle += bytes((n, 255))
                    for k in range(n):
                        r, g, bb, a = px[x + k, y]
                        rle.append(self.near((r, g, bb)))
                    x += n
            blobs.append(bytes(rle))
        out = bytearray()
        out += struct.pack("<4sHHHHHHHH", b"SPR\x00", self.w, self.h, self.cx, self.cy, self.frames, self.colors, self.dirs, self.itv)
        out += self.reserved
        for r, g, bb in self.pal: out += bytes((r, g, bb))
        off = 0
        for blob in blobs:
            out += struct.pack("<II", off, len(blob) + 8); off += len(blob) + 8
        for blob in blobs:
            out += struct.pack("<HHHH", self.w, self.h, 0, 0) + blob
        open(path, "wb").write(bytes(out))
        print("ghi", path, len(out), "byte")

def ve_chu(img, x, y, w, h, text, size, color, bg=None, align="c"):
    d = ImageDraw.Draw(img)
    px = img.load()
    if bg is None:
        bg = px[max(0, x - 2), min(img.height - 1, y + h // 2)]
    d.rectangle([x, y, x + w - 1, y + h - 1], fill=bg)
    if not text: return
    f = font(size)
    tw = d.textlength(text, font=f)
    tx = x + (w - tw) // 2 if align == "c" else x
    ty = y + (h - size) // 2 - 1
    for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
        d.text((tx + dx, ty + dy), text, font=f, fill=(10, 10, 10, 255))
    d.text((tx, ty), text, font=f, fill=color)

if __name__ == "__main__":
    mode = sys.argv[1]
    s = Spr(sys.argv[2])
    print("size", s.w, s.h, "frames", s.frames, "colors", s.colors, "frame hdr", s.frame_hdr[:1], "pal[0..3]", s.pal[:4])
    if mode == "dump":
        s.imgs[0].save(sys.argv[3]); print("png", sys.argv[3])
    elif mode == "make":
        # cac vung chu do tren anh sau khi dump (xem out/spr/khung_cl_ok.png)
        img = s.imgs[0]
        jobs = eval(open(sys.argv[5], encoding="utf-8").read()) if len(sys.argv) > 5 else []
        for j in jobs: ve_chu(img, *j)
        img.save(sys.argv[4]); print("png xem truoc", sys.argv[4])
        s.save(sys.argv[3])
