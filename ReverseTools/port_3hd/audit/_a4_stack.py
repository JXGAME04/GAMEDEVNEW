# -*- coding: utf-8 -*-
import os, sys, io, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

def load(path, cols):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType"); iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max([iG, iD, iP] + cols):
            continue
        try:
            k = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError:
            continue
        out[k] = [c[0].strip()] + [c[i].strip() for i in cols]
    return out

# Linux: [12]=是否叠放 (stack), [17]=参数4 co cap do
L = load(r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt", [12, 17, 11])
# JX1 : [12]=nMaxStack
J = load(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt", [12, 11])
remap = json.load(io.open(r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong\remap_resolved.json", encoding="utf-8"))
print("%-10s %-28s %-10s %-8s | %-9s %-10s %s" % ("LINUX", "ten", "stack_L", "coCapDo", "JX1", "stack_J", "canh bao"))
for s in sorted(remap, key=lambda x: int(x.split(",")[2])):
    dd = remap[s]
    ks = tuple(map(int, s.split(","))); kd = tuple(map(int, dd.split(",")))
    a = L.get(ks); b = J.get(kd)
    if not a or not b:
        print("%-10s KHONG TRA DUOC" % s); continue
    w = ""
    if a[1] != b[1]:
        w += "STACK LECH(%s->%s) " % (a[1], b[1])
    print("%-10s %-28s %-10s %-8s | %-9s %-10s %s" % (s, a[0][:28], a[1], a[2], dd, b[1], w))
