# -*- coding: ascii -*-
"""[SPRFIX 06/09] Ghi tep SPR tu anh RGBA (Pillow) VA KIEM LAI bang cach doc nguoc.

GOC SU CO 06/09 09:48 (client sap 0xC0000005 tai TextureResSpr::LoadSprFile+0x1F9, TextureRes.cpp:522):
  thongtin20.spr do toi ve co phan dau khai Colors = 256 nhung chi ghi 138 mau (414 byte) vi
  `pal_img.getpalette()[:768]` cua Pillow CHI tra ve so mau anh that su dung. Bo nap doc bang mau
  theo Colors = 256 (768 byte) nen bang SPROFFS bi lech 354 byte, roi vao giua du lieu RLE:
  Offset doc duoc = 0x4B4B4B4B -> pFrame = pSprite + 1,26 ti -> doc dia chi khong hop le -> SAP.
  (bangdiem.spr ve truoc do khong sap vi anh do dung du 256 mau.)

LUAT: bang mau LUON ghi du Colors * 3 byte (dem 0), va PHAI kiem lai tep sau khi ghi.

Format (Engine/Src/KSprite.h + Represent3/TextureRes.cpp:458-535):
  SPRHEAD 32 byte {"SPR\0", Width, Height, CenterX, CenterY, Frames, Colors, Directions, Interval, Reserved[6]}
  KPAL24[Colors]           (3 byte moi mau: R, G, B)
  SPROFFS[Frames]          {DWORD Offset, DWORD Length}  - Offset tinh TU SAU bang SPROFFS
  SPRFRAME moi khung       {WORD Width, Height, OffsetX, OffsetY} + du lieu RLE
  RLE moi dong: [run][alpha] (+ run byte chi so bang mau khi alpha > 0)
Dung: from ghi_spr import ghi_spr_tu_anh; ghi_spr_tu_anh(img, "duong/dan.spr")
"""
import os
import struct

from PIL import Image

HEADER_LEN = 32
SPROFFS_LEN = 8
FRAME_HEAD_LEN = 8


def _rle_mot_khung(img, idx):
    """Ma hoa RLE mot khung tu anh RGBA + bang chi so mau (idx[x, y])."""
    W, H = img.size
    px = img.load()
    rle = bytearray()
    for y in range(H):
        x = 0
        while x < W:
            a0 = _luong_tu_alpha(px[x, y][3])
            n = 0
            while x + n < W and n < 255:
                if _luong_tu_alpha(px[x + n, y][3]) != a0:
                    break
                n += 1
            rle += bytes((n, a0))
            if a0 > 0:
                for k in range(n):
                    rle.append(idx[x + k, y])
            x += n
    return bytes(rle)


def _luong_tu_alpha(a):
    if a < 8:
        return 0
    if a >= 248:
        return 255
    return (a // 16) * 16 + 8


def ghi_spr_tu_anh(img, path, center=(0, 0)):
    """Ghi anh RGBA thanh SPR mot khung. Tra ve so byte da ghi. Nem AssertionError neu kiem lai hong."""
    img = img.convert("RGBA")
    W, H = img.size
    px = img.load()

    rgb = Image.new("RGB", (W, H), (0, 0, 0))
    prgb = rgb.load()
    for y in range(H):
        for x in range(W):
            r, g, b, a = px[x, y]
            prgb[x, y] = (r, g, b) if a > 0 else (0, 0, 0)
    pal_img = rgb.quantize(colors=256, method=Image.MEDIANCUT, dither=Image.NONE)
    idx = pal_img.load()

    # [SPRFIX 06/09] getpalette() chi tra ve so mau THAT SU dung -> PHAI dem cho du 256*3 byte
    pal = list(pal_img.getpalette() or [])[: 256 * 3]
    pal += [0] * (256 * 3 - len(pal))
    assert len(pal) == 256 * 3

    frame = struct.pack("<HHHH", W, H, 0, 0) + _rle_mot_khung(img, idx)

    out = bytearray()
    out += struct.pack("<4sHHHHHHHH", b"SPR\x00", W, H, center[0], center[1], 1, 256, 1, 1)
    out += b"\x00" * 12                      # Reserved[6]
    out += bytes(pal)                        # KPAL24[256]
    out += struct.pack("<II", 0, len(frame))  # SPROFFS[0]
    out += frame

    if os.path.dirname(path):
        os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, "wb").write(bytes(out))
    kiem_spr(path, bat_buoc=True)
    return len(out)


def kiem_spr(path, bat_buoc=False):
    """Doc nguoc tep SPR va kiem cau truc. Tra ve (True, mo_ta) hoac (False, ly_do)."""
    d = open(path, "rb").read()
    ten = os.path.basename(path)

    def hong(ly_do):
        if bat_buoc:
            raise AssertionError("%s: %s" % (ten, ly_do))
        return False, ly_do

    if len(d) < HEADER_LEN or d[:3] != b"SPR":
        return hong("khong phai tep SPR")
    W, H, cx, cy, frames, colors, dirs, itv = struct.unpack("<HHHHHHHH", d[4:20])
    if frames == 0:
        return hong("Frames = 0 (LoadSprFile bo qua)")
    off_tbl = HEADER_LEN + colors * 3
    if off_tbl + frames * SPROFFS_LEN > len(d):
        return hong("bang mau khai %d mau nhung tep chi %d byte" % (colors, len(d)))
    goc = off_tbl + frames * SPROFFS_LEN     # Offset tinh tu sau bang SPROFFS
    for i in range(frames):
        o, l = struct.unpack("<II", d[off_tbl + i * SPROFFS_LEN: off_tbl + (i + 1) * SPROFFS_LEN])
        if goc + o + FRAME_HEAD_LEN > len(d):
            return hong("khung %d: Offset %d vuot tep (%d byte) - DAY LA LOI LAM SAP CLIENT" % (i, o, len(d)))
        if l < FRAME_HEAD_LEN or goc + o + l > len(d):
            return hong("khung %d: Length %d khong hop le" % (i, l))
        fw, fh, ox, oy = struct.unpack("<HHHH", d[goc + o: goc + o + FRAME_HEAD_LEN])
        if fw == 0 or fh == 0 or fw > 4096 or fh > 4096:
            return hong("khung %d: kich thuoc %dx%d vo ly" % (i, fw, fh))
        # duyet RLE: phai phu dung fh dong x fw diem
        p = goc + o + FRAME_HEAD_LEN
        het = goc + o + l
        x = y = 0
        while y < fh and p + 1 < het:
            run, alpha = d[p], d[p + 1]
            p += 2
            if alpha > 0:
                p += run
            x += run
            if x >= fw:
                x = 0
                y += 1
        if y != fh:
            return hong("khung %d: RLE phu %d/%d dong" % (i, y, fh))
    return True, "%s OK: %dx%d, %d khung, %d mau, %d byte" % (ten, W, H, frames, colors, len(d))


if __name__ == "__main__":
    import sys
    for p in sys.argv[1:]:
        ok, mo_ta = kiem_spr(p)
        print(("OK  " if ok else "HONG") + "  " + mo_ta if ok else "HONG  %s: %s" % (os.path.basename(p), mo_ta))
