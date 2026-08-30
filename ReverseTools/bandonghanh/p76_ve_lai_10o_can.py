# -*- coding: utf-8 -*-
r"""[PETSYS 30/08] Chu: "bop lai o trang bi thi phai bop cho can bang chu
de chen xac nhau". Lan truoc toi dan 10 khung buoc 24 trong khi khung rong
26 -> chong mep.
Do lai khung khu "Trang Bi" tren anh nen: vien ngoai x 317..634, y 68..128
=> vung trong x 322..630 (308px), y 72..124 (52px).
Chia 10 o CAN: buoc 30, o 26x26, bat dau x = 326 (326..596), y = 85.
Anh nen goc lay lai tu pak VLTK (ban tren dia da bi ve de).
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402

CR = chr(13)
BS = chr(92)
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
VL = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
DICH = CL + r"\spr\Ui3\pet\pet_main_1.spr"


def blob(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    raw = f.read(cs if cs else size)
    try:
        if fl == 0 or cs in (0, size):
            return raw[:size] if size > 0 else raw
        if fl == 1:
            return ucl.nrv2b_decompress_8(raw, size)
        import bz2
        import zlib
        return bz2.decompress(raw) if fl == 2 else zlib.decompress(raw)
    except Exception:
        return b""


# ---- 1. lay lai anh nen GOC tu pak VLTK ----
TEN_GOC = [r"\spr\Ui3\pet\pet_main_1.spr", r"\spr\Ui3\ÃÅ¿Í\pet_main_1.spr"]
import glob
lay = None
ids = {P.name2id(t): t for t in TEN_GOC}
for pak in sorted(glob.glob(VL + r"\*.pak")):
    try:
        f, es = P.entries(pak)
    except Exception:
        continue
    for e in es:
        if e[0] in ids:
            d = blob(f, e)
            if d[:3] == b"SPR":
                lay = d
                print("1. lay lai nen goc tu", os.path.basename(pak), len(d))
                break
    f.close()
    if lay:
        break
if lay:
    open(DICH, "wb").write(lay)
else:
    print("1. !! khong tim thay nen goc trong pak - dung ban tren dia")

# ---- 2. ve 10 khung CAN DOI ----
import p15_vietsub_spr as p15  # noqa: E402
from PIL import Image  # noqa: E402

sp = p15.Spr(DICH)
im = sp.imgs[0]
# khung mau = o dau tien cua nen goc (7 o: x 340..556, buoc 36, rong 26)
mau = im.crop((336, 77, 372, 111))          # 36x34 (o 26 + vien)
mau30 = mau.resize((30, 34), Image.LANCZOS)  # buoc 30 -> vua 10 o trong 308px
nenpx = im.getpixel((325, 74))
im.paste(Image.new("RGBA", (300, 40), nenpx), (324, 74))
for k in range(10):
    im.paste(mau30, (324 + k * 30, 76), mau30)
sp.imgs = [im]
sp.path = DICH
bak = DICH + ".truoc_vietsub"
if os.path.exists(bak):
    os.remove(bak)
sp.save()
if os.path.exists(bak):
    os.remove(bak)
print("2. ve 10 khung buoc 30 (x 324..594), khong chong mep")

# ---- 3. INI khop khung ----
import re
p = CL + r"\Ui\Ui3\pet_main.ini"
s = io.open(p, "r", encoding="latin-1", newline="").read()
for k in range(10):
    sec = "[PetEquip_%d]" % (k + 1)
    i = s.find(sec)
    if i < 0:
        continue
    j = s.find("[", i + 1)
    kh = s[i:j] if j > 0 else s[i:]
    m = re.sub(r"Left=\d+", "Left=%d" % (328 + k * 30), kh, 1)
    m = re.sub(r"Top=\d+", "Top=82", m, 1)
    m = re.sub(r"Width=\d+", "Width=24", m, 1)
    m = re.sub(r"Height=\d+", "Height=24", m, 1)
    s = s.replace(kh, m, 1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("3. INI 10 o 24x24 tai y=82, buoc 30 (x 328..598)")

# xuat anh kiem
sp2 = p15.Spr(DICH)
sp2.imgs[0].crop((300, 60, 650, 140)).save(r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\khung10_check.png")
print("4. xuat khung10_check.png de kiem mat")
