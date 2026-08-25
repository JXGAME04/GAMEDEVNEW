# -*- coding: utf-8 -*-
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

LNX = r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt"
JX1I = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"

def load(path):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType"); iP = hdr.index("ParticularType")
    out = {}
    for n, ln in enumerate(d[1:], start=2):
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP):
            continue
        try:
            key = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError:
            continue
        out[key] = (c[0].strip(), n, c)
    return out, hdr

lnx, hl = load(LNX)
jx1, hj = load(JX1I)
remap = json.load(io.open(r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong\remap_resolved.json", encoding="utf-8"))

def norm(s):
    return re.sub(r"\s+", " ", s.strip().lower())

print("SRC\tTEN_LINUX\tDST\tTEN_JX1_DICH\tTEN_JX1_TAI_SO_NGUON\tKHOP")
prob = []
for s, d in remap.items():
    ks = tuple(map(int, s.split(","))); kd = tuple(map(int, d.split(",")))
    sn = lnx.get(ks, ("<KHONG CO O LINUX>", 0, []))[0]
    dn = jx1.get(kd, ("<KHONG CO O JX1>", 0, []))[0]
    on = jx1.get(ks, ("<trong>", 0, []))[0]
    ok = "OK" if norm(sn) == norm(dn) else "LECH"
    if ok == "LECH":
        prob.append((s, sn, d, dn))
    print("%s\t%s\t%s\t%s\t%s\t%s" % (s, sn, d, dn, on, ok))
print()
print("== LECH TEN:", len(prob))
for p in prob:
    print("  ", p)
