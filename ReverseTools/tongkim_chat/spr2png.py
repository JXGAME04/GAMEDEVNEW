# -*- coding: ascii -*-
# Giai ma SPR (SPRHEAD 32 B + KPAL24 + SPROFFS + frame RLE [run][alpha](+idx)) -> PNG frame 0. Dung: python spr2png.py in.spr out.png
import struct, sys
from PIL import Image

def decode(path):
    d = open(path, "rb").read()
    assert d[:3] == b"SPR", "khong phai SPR"
    W, H, cx, cy, frames, colors, dirs, interval = struct.unpack("<HHHHHHHH", d[4:20])
    pos = 32
    pal = []
    for i in range(colors):
        r, g, b = d[pos], d[pos+1], d[pos+2]; pal.append((r, g, b)); pos += 3
    offs = []
    for i in range(frames):
        o, l = struct.unpack("<II", d[pos:pos+8]); offs.append((o, l)); pos += 8
    base = pos
    fo, fl = offs[0]
    p = base + fo
    fw, fh, ox, oy = struct.unpack("<HHHH", d[p:p+8]); p += 8
    img = Image.new("RGBA", (fw, fh), (0, 0, 0, 0)); px = img.load()
    y = 0; x = 0
    end = base + fo + fl
    while y < fh and p < end:
        run = d[p]; alpha = d[p+1]; p += 2
        if alpha == 0:
            x += run
        else:
            for k in range(run):
                idx = d[p]; p += 1
                if x < fw:
                    r, g, b = pal[idx] if idx < len(pal) else (255, 0, 255)
                    px[x, y] = (r, g, b, 255 if alpha >= 31 else int(alpha * 255 / 31))
                x += 1
        if x >= fw:
            x = 0; y += 1
    return img, (W, H, frames, colors, fw, fh)

if __name__ == "__main__":
    img, info = decode(sys.argv[1])
    img.save(sys.argv[2])
    print(sys.argv[1], "->", sys.argv[2], info)
