# -*- coding: utf-8 -*-
r"""Decode SPR (Kingsoft Sword3) -> PNG. Bo cuc theo KSprite.cpp::Load:
SPRHEAD(28B) -> KPAL24[Colors](3B/mau) -> SPROFFS[Frames](8B) -> frames.
Frame: WORD W,H,OffX,OffY + RLE: tung token 2 byte [run_len][alpha];
alpha==0 -> run_len pixel trong suot; alpha>0 -> run_len byte index mau theo sau
(alpha 0..31 la do dam - nhan len 255/31).
Dung: python spr2png.py <file.spr> <out.png> [frame]
"""
import struct
import sys

from PIL import Image


def decode(path, out, frame_no=0):
    b = open(path, "rb").read()
    (com, w, h, cx, cy, frames, colors, dirs, itv) = struct.unpack_from("<4sHHHHHHHH", b, 0)
    assert com[:3] == b"SPR", com
    off = 28
    pal = []
    for i in range(colors):
        r, g, bl = b[off], b[off+1], b[off+2]
        pal.append((r, g, bl))
        off += 3
    offs = []
    for i in range(frames):
        o, ln = struct.unpack_from("<II", b, off)
        offs.append((o, ln))
        off += 8
    base = off
    fo, fl = offs[frame_no]
    if fo >= len(b) - base:
        fo = 0          # mot so file ghi RAC vao bang offs; frame nam ngay tai base
    p = base + fo
    # frame that (do hex file VNG): DWORD LenRLE + WORD W,H + WORD OffX,OffY
    flen, fw, fh, ox, oy = struct.unpack_from("<IHHHH", b, p)
    p += 12
    img = Image.new("RGBA", (max(w, fw + ox), max(h, fh + oy)), (0, 0, 0, 0))
    px = img.load()
    x = y = 0
    end = min(len(b), p + (flen if flen > 0 else (fl if fl > 0 else len(b) - p)))
    while p + 1 < end and y < fh:
        run = b[p]
        alpha = b[p+1]
        p += 2
        if alpha == 0:
            x += run
        else:
            a = min(255, alpha * 255 // 31)
            for k in range(run):
                idx = b[p]
                p += 1
                if x < fw:
                    r, g, bl = pal[idx] if idx < len(pal) else (255, 0, 255)
                    px[ox + x, oy + y] = (r, g, bl, a)
                x += 1
        while x >= fw:
            x -= fw
            y += 1
            if x == 0:
                break
    img.save(out)
    print("%s: %dx%d frames=%d colors=%d -> frame %d (%dx%d off %d,%d) -> %s" %
          (path.split(chr(92))[-1], w, h, frames, colors, frame_no, fw, fh, ox, oy, out))


if __name__ == "__main__":
    decode(sys.argv[1], sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 0)
