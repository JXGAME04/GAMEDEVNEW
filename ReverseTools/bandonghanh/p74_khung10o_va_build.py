# -*- coding: utf-8 -*-
r"""[PETSYS 30/08]
A. Sua loi bien dich: KPlayerPet.cpp dung ItemSet nhung thieu include
   KItemSet.h (extern KItemSet ItemSet - KItemSet.h:142).
B. Chu: "7 o gio 10 mon ... che chu dong duoi va khong co o cac mon them".
   Do anh nen: khu trang bi CHI co 7 khung (y 81..107, x 340..556 buoc 36),
   khong con cho hang 2 (nhan "Ky Nang" ngay duoi, y~118).
   => VE LAI khu do thanh 10 KHUNG lien mach: cat 1 khung mau tu chinh anh
   nen, xoa dai cu, dan 10 khung buoc 24 tu x=340 (340..556, vua khung cu).
   INI: 10 o PetEquip 24x24 dung toa do moi.
"""
import io
import os
import re
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")

CR = chr(13)
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc_lf(p):
    return io.open(p, "r", encoding="latin-1", newline="").read().replace(CR + "\n", "\n")


def ghi_crlf(p, lf):
    io.open(p, "w", encoding="latin-1", newline="").write(lf.replace("\n", CR + "\n"))


# ---------- A. include KItemSet.h ----------
p = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerPet.cpp"
lf = doc_lf(p)
if '#include "KItemSet.h"' not in lf:
    neo = '#include "KNpcSet.h"'
    assert lf.count(neo) == 1
    lf = lf.replace(neo, neo + '\n#include "KItemSet.h"\t// [30/08] extern KItemSet ItemSet (PET_ClearHand)', 1)
    ghi_crlf(p, lf)
    print("A. them include KItemSet.h")
else:
    print("A. da co")

# ---------- B. ve lai 10 khung o ----------
import p15_vietsub_spr as p15  # noqa: E402
from PIL import Image  # noqa: E402

nen = CL + r"\spr\Ui3\pet\pet_main_1.spr"
sp = p15.Spr(nen)
im = sp.imgs[0]
# khung mau: o dau tien (x 336..372, y 77..111) - lay ca vien
mau = im.crop((336, 77, 372, 111))          # 36x34
mau24 = mau.resize((24, 30), Image.LANCZOS)  # thu hep ngang, giu chieu doc
# lay mau nen (diem trong khu, ngoai khung) de xoa dai cu
nenpx = im.getpixel((330, 75))
vung = Image.new("RGBA", (260, 36), nenpx)
im.paste(vung, (335, 76))
# dan 10 khung buoc 24 tu x=338
for k in range(10):
    im.paste(mau24, (338 + k * 24, 78), mau24)
sp.imgs = [im]
sp.path = nen
bak = nen + ".truoc_vietsub"
if os.path.exists(bak):
    os.remove(bak)
sp.save()
if os.path.exists(bak):
    os.remove(bak)
print("B1. ve lai 10 khung o (buoc 24, x 338..554)")

# INI: 10 o 24x24 khop khung moi
p = CL + r"\Ui\Ui3\pet_main.ini"
s = io.open(p, "r", encoding="latin-1", newline="").read()
for k in range(10):
    sec = "[PetEquip_%d]" % (k + 1)
    i = s.find(sec)
    if i < 0:
        continue
    j = s.find("[", i + 1)
    kh = s[i:j] if j > 0 else s[i:]
    m = re.sub(r"Left=\d+", "Left=%d" % (341 + k * 24), kh, 1)
    m = re.sub(r"Top=\d+", "Top=81", m, 1)
    m = re.sub(r"Width=\d+", "Width=24", m, 1)
    m = re.sub(r"Height=\d+", "Height=24", m, 1)
    s = s.replace(kh, m, 1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("B2. INI 10 o 24x24 tai y=81, buoc 24")
print("XONG p74")
