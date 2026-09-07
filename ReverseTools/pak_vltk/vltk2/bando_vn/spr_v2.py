# -*- coding: utf-8 -*-
"""Giai ma SPR -> PNG, TU DONG chon offset bang mau (28 = ban goc, 32 = ban VNG Viet hoa).
Dung: python spr_v2.py <spr> <png>
"""
import struct, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from PIL import Image

src, out = sys.argv[1], sys.argv[2]
b = open(src, "rb").read()
com, w, h, cx, cy, frames, colors, dirs, itv = struct.unpack_from("<4sHHHHHHHH", b, 0)
base = 28 + colors * 3 + 8 * frames        # = 804 cho ca hai bien the
flen, fw, fh, ox, oy = struct.unpack_from("<IHHHH", b, base)

idx = bytearray(w * h)
p = base + 12
end = min(len(b), p + flen)
x = y = 0
while p + 1 < end and y < h:
    run = b[p]; alpha = b[p + 1]; p += 2
    if alpha == 0:
        x += run
    else:
        for k in range(run):
            if p >= len(b): break
            if x < w and y < h:
                idx[y * w + x] = b[p]
            p += 1; x += 1
    while x >= w:
        x -= w; y += 1

used = set(idx)

def build(o):
    return [tuple(b[o + i * 3: o + i * 3 + 3]) if len(b[o + i * 3: o + i * 3 + 3]) == 3 else (0, 0, 0)
            for i in range(256)]

def smooth_of(pal):
    s = n = 0
    for yy in range(0, h, 9):
        row = yy * w
        for xx in range(0, w - 1, 4):
            a = pal[idx[row + xx]]; c = pal[idx[row + xx + 1]]
            s += abs(a[0]-c[0]) + abs(a[1]-c[1]) + abs(a[2]-c[2]); n += 1
    return s / max(1, n)

cands = []
for o in (28, 32):
    pal = build(o)
    cands.append((smooth_of(pal), o, pal))
cands.sort()
sc, o, pal = cands[0]
print("%s: %dx%d | bang mau offset=%d (muot %.1f) | %d chi so" %
      (src.split("\\")[-1].split("/")[-1], w, h, o, sc, len(used)))

img = Image.new("RGB", (w, h)); px = img.load()
for yy in range(h):
    row = yy * w
    for xx in range(w):
        px[xx, yy] = pal[idx[row + xx]]
img.save(out)
print("   -> %s" % out)
