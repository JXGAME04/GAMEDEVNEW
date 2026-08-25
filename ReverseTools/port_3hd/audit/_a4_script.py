# -*- coding: utf-8 -*-
import re, sys, io, json, os
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

def load(path, scol):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType"); iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP, scol):
            continue
        try:
            k = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError:
            continue
        out[k] = (c[0].strip(), c[scol].strip())
    return out

L = load(r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt", 13)
J = load(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt", 9)
remap = json.load(io.open(r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong\remap_resolved.json", encoding="utf-8"))
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

def nrm(s):
    return s.lower().replace("/", "\\").strip("\\")

print("%-10s %-28s %-44s | %-9s %-44s %s" % ("LINUX", "ten", "script Linux", "JX1", "script JX1", "trang thai"))
for s in sorted(remap, key=lambda x: int(x.split(",")[2])):
    dd = remap[s]
    ks = tuple(map(int, s.split(","))); kd = tuple(map(int, dd.split(",")))
    ln, lsc = L.get(ks, ("?", ""))
    jn, jsc = J.get(kd, ("?", ""))
    if not (lsc or jsc):
        continue
    st = "khop" if nrm(lsc) == nrm(jsc) else "LECH"
    if lsc and not jsc:
        st = "JX1 KHONG CO SCRIPT"
    ex = ""
    if lsc:
        ex = "co tep" if os.path.isfile(os.path.join(JX1, nrm(lsc))) else "TEP THIEU"
    print("%-10s %-28s %-44s | %-9s %-44s %s / %s" % (s, ln[:28], lsc[:44], dd, jsc[:44], st, ex))
