# -*- coding: utf-8 -*-
"""Kiem: cot ParticularType trong magicscript.txt co BANG chi so dong khong?

Ly do: KItemGenerator.CPP:1660-1661 dat `const int i = nParticularType;` roi
GetMagicScript(i) -> KBasPropTbl.cpp:1058 tra ve `m_pBuf + i` = phan tu THU i.
Tuc may tra theo CHI SO, khong tra theo cot. Neu cot lech chi so o dau do thi
moi cong cu nan ma dua vao cot deu SAI tu diem do tro di.
"""
import io
import os
import sys

sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
from bangtxt import tcvn2uni  # noqa: E402

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
     r"\settings\item\magicscript.txt")

d = io.open(P, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")
hdr = d[0].split("\t")
print("so cot:", len(hdr))
for i, h in enumerate(hdr[:6]):
    print("  cot %d = %s" % (i, h))

# LoadRecord bo dong tieu de; chi so 0 = dong du lieu dau tien (d[1])
lech = []
n = 0
for k, l in enumerate(d[1:]):
    c = l.split("\t")
    if len(c) < 4 or not c[3].strip().isdigit():
        continue
    n += 1
    p = int(c[3])
    if p != k:
        lech.append((k, p, tcvn2uni(c[0])))

print()
print("so dong du lieu doc duoc:", n)
print("so dong co cot ParticularType != chi so:", len(lech))
if lech:
    print()
    print("  chi_so  cot_P   ten")
    for k, p, ten in lech[:15]:
        print("  %-7d %-7d %s" % (k, p, ten[:50]))
    if len(lech) > 15:
        print("  ... con %d dong" % (len(lech) - 15))
else:
    print("=> cot ParticularType TRUNG KHOP chi so dong tren toan bang.")
    print("   Vay tra theo cot hay theo chi so deu ra cung ket qua.")
