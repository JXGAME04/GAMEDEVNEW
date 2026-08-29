# -*- coding: utf-8 -*-
r"""[BDH 28/08] VIET HOA bo anh giao dien DONG HANH (chu TRUNG -> VIET).

Nguon anh la ban CN (image2.pak jx1m_cdn); client VLTK chi co 3 anh Viet
(tui "Hanh trang" - da rut o buoc truoc). Cac anh con lai tu ve de:
xoa vung chu Trung (fill mau nen lay mau canh vung) + ve chu Viet (PIL).

Format SPR (do hex + KSprite.cpp):
  SPRHEAD 28B | KPAL24[colors] | SPROFFS[frames] (co file ghi RAC) |
  frame: DWORD lenRLE + WORD W,H + WORD offX,offY + RLE [run][alpha](+run idx)
Khi ghi lai: giu header + palette goc, bang offs ghi chuan, alpha 0xFF cho
pixel dac (a<64 bo). Backup .truoc_vietsub. Idempotent (marker file .da_viet).
"""
import io
import os
import struct
import sys

from PIL import Image, ImageDraw, ImageFont

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
SEP = chr(92)


def g(s):
    return s.encode("gbk").decode("latin-1")


GD = os.path.join(CLI, "spr", "UI3", g("同伴界面"))
QB = os.path.join(GD, g("同伴快捷栏"))

FONT_DIR = r"C:\Windows\Fonts"


def font(size, bold=True):
    for name in (["arialbd.ttf", "tahomabd.ttf"] if bold else ["arial.ttf", "tahoma.ttf"]):
        p = os.path.join(FONT_DIR, name)
        if os.path.isfile(p):
            return ImageFont.truetype(p, size)
    return ImageFont.load_default()


class Spr:
    def __init__(self, path):
        self.path = path
        b = open(path, "rb").read()
        (com, self.w, self.h, self.cx, self.cy, self.frames, self.colors,
         self.dirs, self.itv) = struct.unpack_from("<4sHHHHHHHH", b, 0)
        assert com[:3] == b"SPR"
        self.reserved = b[20:28]
        off = 28
        self.pal = []
        for i in range(self.colors):
            self.pal.append((b[off], b[off+1], b[off+2]))
            off += 3
        # CHUAN DO HEX 28/08: sau palette co 4 BYTE DEM, roi bang SPROFFS
        # {Offset,Length} (Offset tinh tu base -> header 8B cua frame; Length =
        # 8 + do dai RLE). Frame header CHI 8 byte {W,H,offX,offY}.
        self.pad = b[off:off+4]
        off += 4
        offs = []
        for i in range(self.frames):
            o, ln = struct.unpack_from("<II", b, off)
            offs.append((o, ln))
            off += 8
        base = off
        self.imgs = []
        for fi in range(self.frames):
            fo, fl = offs[fi]
            if fo >= len(b) - base:
                fo = 0
            p = base + fo
            fw, fh, ox, oy = struct.unpack_from("<HHHH", b, p)
            p += 8
            flen = (fl - 8) if fl >= 8 else (len(b) - p)
            img = Image.new("RGBA", (self.w, self.h), (0, 0, 0, 0))
            px = img.load()
            x = y = 0
            end = min(len(b), p + flen)
            while p + 1 < end and y < fh:
                run, alpha = b[p], b[p+1]
                p += 2
                if alpha == 0:
                    x += run
                else:
                    a = 255 if alpha >= 31 else max(8, alpha * 255 // 31)
                    for k in range(run):
                        idx = b[p]
                        p += 1
                        if x < fw and ox + x < self.w and oy + y < self.h:
                            r, gg, bb = self.pal[idx]
                            px[ox + x, oy + y] = (r, gg, bb, a)
                        x += 1
                while x >= fw and fw > 0:
                    x -= fw
                    y += 1
            self.imgs.append(img)
        # bang tra nguoc mau -> index (gan nhat)
        self._cache = {}

    def near(self, rgb):
        if rgb in self._cache:
            return self._cache[rgb]
        r, gg, bb = rgb
        best, bd = 0, 1 << 30
        for i, (pr, pg, pb) in enumerate(self.pal):
            d = (pr-r)*(pr-r) + (pg-gg)*(pg-gg) + (pb-bb)*(pb-bb)
            if d < bd:
                bd, best = d, i
        self._cache[rgb] = best
        return best

    def save(self):
        frames_blob = []
        for img in self.imgs:
            px = img.load()
            rle = bytearray()
            for y in range(self.h):
                x = 0
                while x < self.w:
                    # dem run trong suot
                    n = 0
                    while x + n < self.w and px[x+n, y][3] < 64 and n < 255:
                        n += 1
                    if n:
                        rle += bytes((n, 0))
                        x += n
                        continue
                    n = 0
                    while x + n < self.w and px[x+n, y][3] >= 64 and n < 255:
                        n += 1
                    rle += bytes((n, 255))  # [29/08] alpha thang 0..255 (do goc: dac=255; 31 = mo 12%)
                    for k in range(n):
                        r, gg, bb, a = px[x+k, y]
                        rle.append(self.near((r, gg, bb)))
                    x += n
            frames_blob.append(bytes(rle))
        out = bytearray()
        out += struct.pack("<4sHHHHHHHH", b"SPR\x00", self.w, self.h, self.cx,
                           self.cy, self.frames, self.colors, self.dirs, self.itv)
        out += self.reserved
        for r, gg, bb in self.pal:
            out += bytes((r, gg, bb))
        out += self.pad                              # 4 byte dem nhu goc
        off = 0
        for blob in frames_blob:
            out += struct.pack("<II", off, len(blob) + 8)
            off += len(blob) + 8
        for blob in frames_blob:
            out += struct.pack("<HHHH", self.w, self.h, 0, 0)
            out += blob
        if not os.path.exists(self.path + ".truoc_vietsub"):
            open(self.path + ".truoc_vietsub", "wb").write(open(self.path, "rb").read())
        open(self.path, "wb").write(bytes(out))


def ve(spr, jobs):
    """jobs: list (x, y, w, h, text, size, color, align) - xoa vung + ve chu.
    align: 'l'/'c'. Fill nen = mau pixel ngay ben trai vung (hoac trong vung goc)."""
    for img in spr.imgs:
        d = ImageDraw.Draw(img)
        px = img.load()
        for job in jobs:
            if len(job) == 9:
                (x, y, w, h, text, size, color, align, bg) = job
            else:
                (x, y, w, h, text, size, color, align) = job
                bx = max(0, x - 3)
                bg = px[bx, min(spr.h-1, y + h // 2)]
                if bg[3] < 64:
                    bg = (0, 0, 0, 0)
            d.rectangle([x, y, x + w - 1, y + h - 1], fill=bg)
            if not text:
                continue
            f = font(size)
            tw = d.textlength(text, font=f)
            tx = x + (w - tw) // 2 if align == "c" else x
            ty = y + (h - size) // 2 - 1
            # vien den mong cho de doc
            for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                d.text((tx + dx, ty + dy), text, font=f, fill=(10, 10, 10, 255))
            d.text((tx, ty), text, font=f, fill=color)


VANG = (255, 220, 120, 255)
TRANG = (230, 235, 230, 255)
XANH = (150, 230, 150, 255)


def lam(path, jobs, ghi_chu=""):
    marker = path + ".da_viet"
    if os.path.exists(marker):
        print("  da viet:", os.path.basename(path))
        return
    s = Spr(path)
    ve(s, jobs)
    s.save()
    open(marker, "wb").write(b"1")
    print("  VIET HOA:", ghi_chu or os.path.basename(path), "(%d frame)" % s.frames)


# ============ 1. hai title 321x27 ============
lam(os.path.join(GD, g("同伴属性title") + ".spr"),
    [(60, 3, 200, 21, "THUỘC TÍNH ĐỒNG HÀNH", 13, VANG, "c")], "title thuoc tinh")
lam(os.path.join(GD, g("同伴技能title") + ".spr"),
    [(60, 3, 200, 22, "KỸ NĂNG ĐỒNG HÀNH", 13, VANG, "c")], "title ky nang")

# ============ 2. nen trang THUOC TINH 411x376 ============
# toa do do tren anh png (goc trai anh nen; cua so dat nen tai x=37 -> khong lien quan)
jobs_attr = [
    (140, 6, 130, 20, "THUỘC TÍNH", 13, VANG, "c"),
    (4, 36, 192, 17, "", 10, TRANG, "c", (22, 28, 28, 255)),   # xoa nguyen dai header 3 cot
    (198, 36, 96, 17, "", 10, TRANG, "c", (22, 28, 28, 255)),
    (40, 344, 340, 19, "", 10, TRANG, "c", (22, 28, 28, 255)), # dai nut duoi (nut anh de len)
    (6, 37, 48, 15, "Võ công", 10, TRANG, "c", (22, 28, 28, 255)),
    (58, 37, 48, 15, "Kháng", 10, TRANG, "c", (22, 28, 28, 255)),
    (110, 37, 66, 15, "Ra chiêu", 10, TRANG, "c", (22, 28, 28, 255)),
    (200, 37, 90, 15, "Thuộc tính", 10, TRANG, "c", (22, 28, 28, 255)),
    (310, 37, 92, 15, "", 10, TRANG, "c", (22, 28, 28, 255)),   # 遗忘技能 - nut anh de len
    (206, 66, 64, 13, "Tên:", 10, XANH, "l"),
    (206, 81, 68, 13, "Kinh nghiệm:", 10, XANH, "l"),
    (206, 96, 64, 13, "Tinh lực:", 10, XANH, "l"),
    (206, 111, 64, 13, "Sát thương:", 10, XANH, "l"),
    (283, 128, 46, 13, "K.thường:", 9, TRANG, "l"),
    (283, 143, 46, 13, "K.băng:", 9, TRANG, "l"),
    (283, 158, 46, 13, "K.lôi:", 9, TRANG, "l"),
    (283, 173, 46, 13, "K.hỏa:", 9, TRANG, "l"),
    (283, 188, 46, 13, "K.độc:", 9, TRANG, "l"),
    (206, 205, 40, 13, "Cấp:", 10, TRANG, "l"),
    (283, 205, 56, 13, "Ngũ hành:", 10, TRANG, "l"),
    (206, 220, 56, 13, "Tính cách:", 10, TRANG, "l"),
    (283, 220, 56, 13, "Thân mật:", 10, TRANG, "l"),
    (196, 237, 46, 13, "Sinh lực:", 10, TRANG, "l"),
    (196, 252, 46, 13, "Sức đánh:", 10, TRANG, "l"),
    (196, 267, 46, 13, "Chính xác:", 10, TRANG, "l"),
    (196, 282, 46, 13, "Né tránh:", 10, TRANG, "l"),
    (196, 297, 46, 13, "Tốc độ:", 10, TRANG, "l"),
    (196, 312, 46, 13, "May mắn:", 10, TRANG, "l"),
    (345, 237, 44, 13, "Tư chất:", 9, TRANG, "l"),
    (345, 252, 44, 13, "Tư chất:", 9, TRANG, "l"),
    (345, 267, 44, 13, "Tư chất:", 9, TRANG, "l"),
    (345, 282, 44, 13, "Tư chất:", 9, TRANG, "l"),
    (345, 297, 44, 13, "Tư chất:", 9, TRANG, "l"),
    (345, 312, 44, 13, "Tư chất:", 9, TRANG, "l"),
    (60, 345, 60, 16, "", 10, VANG, "c"),   # 保存 (nut anh de)
    (163, 345, 60, 16, "", 10, VANG, "c"),  # 删除
    (270, 345, 60, 16, "", 10, VANG, "c"),  # 关闭
]
lam(os.path.join(GD, g("同伴-人物") + ".spr"), jobs_attr, "nen thuoc tinh")

# ============ 3. nen trang KY NANG 411x376 ============
jobs_skill = [
    (140, 6, 130, 20, "KỸ NĂNG", 13, VANG, "c"),
    (4, 36, 192, 17, "", 10, TRANG, "c", (22, 28, 28, 255)),
    (198, 36, 96, 17, "", 10, TRANG, "c", (22, 28, 28, 255)),
    (6, 37, 48, 15, "Võ công", 10, TRANG, "c", (22, 28, 28, 255)),
    (58, 37, 48, 15, "Kháng", 10, TRANG, "c", (22, 28, 28, 255)),
    (110, 37, 66, 15, "Ra chiêu", 10, TRANG, "c", (22, 28, 28, 255)),
    (200, 37, 90, 15, "Kỹ năng tự học", 10, TRANG, "c", (22, 28, 28, 255)),
    (310, 37, 92, 15, "", 10, TRANG, "c", (22, 28, 28, 255)),
]
lam(os.path.join(GD, g("同伴-技能") + ".spr"), jobs_skill, "nen ky nang")

# ============ 4. nut chu ============
lam(os.path.join(GD, g("选定") + ".spr"), [(10, 4, 82, 19, "Lưu", 12, VANG, "c")], "nut Luu")
lam(os.path.join(GD, g("解散") + ".spr"), [(10, 4, 82, 19, "Giải tán", 12, VANG, "c")], "nut Giai tan")
lam(os.path.join(GD, g("关闭") + ".spr"), [(10, 4, 82, 19, "Đóng", 12, VANG, "c")], "nut Dong")
lam(os.path.join(GD, g("切换") + ".spr"), [(6, 2, 90, 16, "Chuyển trang", 10, VANG, "c")], "nut Chuyen")
lam(os.path.join(GD, g("遗忘") + ".spr"), [(4, 1, 96, 15, "Quên kỹ năng", 10, VANG, "c")], "nut Quen")

# ============ 5. the doc 标签1..5 (37x45, chu 同伴 doc) ============
for i in range(1, 6):
    p = os.path.join(GD, g("标签%d" % i) + ".spr")
    marker = p + ".da_viet"
    if os.path.exists(marker):
        print("  da viet: the", i)
        continue
    s = Spr(p)
    for img in s.imgs:
        d = ImageDraw.Draw(img)
        px = img.load()
        # vung chu: cot giua the (rong 20 tu x=17 theo frame off) - xoa ca vung giua
        bg = px[18, 22]
        d.rectangle([15, 2, 36, 42], fill=bg if bg[3] >= 64 else (0, 0, 0, 0))
        f = font(11)
        d.text((21, 4), "Đ", font=f, fill=VANG)
        d.text((21, 16), "H", font=f, fill=VANG)
        f2 = font(13)
        d.text((22, 28), str(i), font=f2, fill=TRANG)
    s.save()
    open(marker, "wb").write(b"1")
    print("  VIET HOA: the", i)

print("XONG p15")
