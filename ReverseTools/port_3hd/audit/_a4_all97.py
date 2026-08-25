# -*- coding: utf-8 -*-
"""Kiem lai TAT CA bo so vat pham xuat hien trong 101 tep port (khong chi 42 bo NANG):
so ten Linux vs ten JX1 CUNG SO. Neu ten khac nhau ma KHONG duoc anh xa -> BO SOT."""
import io, os, re, sys, json, csv
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

LNXI = r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt"
JX1I = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
LNX = r"D:\ServerLinux\server1"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"

def load(path):
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType"); iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP):
            continue
        try:
            k = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError:
            continue
        out[k] = c[0].strip()
    return out

lnx = load(LNXI); jx1 = load(JX1I)
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
man = [x for x in io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n") if x.strip()]
TRIP = re.compile(r"(?<![0-9])(\d{1,2})(\s*,\s*)(\d{1,2})(\s*,\s*)(\d{1,5})(?![0-9])")
def norm(s): return re.sub(r"\s+", " ", s.strip().lower())

seen = {}
for rel in man:
    if not rel.lower().endswith(".lua"):
        continue
    p = os.path.join(LNX, rel.replace("/", os.sep))
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    for i, ln in enumerate(d.split("\n"), 1):
        # chi xet dong CO ve la vat pham
        if not re.search(r"tbProp|AddItem|DropItem|ConsumeItem|CalcItemCount|GetItemCount|ItemCount|GiveAward|ForbidItem|ItemProp|\[\"\d+,\d+,\d+\"\]", ln, re.I):
            continue
        for m in TRIP.finditer(ln):
            k = (int(m.group(1)), int(m.group(3)), int(m.group(5)))
            if k[0] not in (0, 1, 2, 3, 4, 5, 6):
                continue
            if k not in lnx:
                continue
            seen.setdefault(k, []).append((rel, i, decline2(ln.strip().encode("latin-1"))[:100]))

print("So bo so vat pham (co trong bang Linux) xuat hien:", len(seen))
print()
print("== NHOM 1: khong nam trong 42 bo remap, NHUNG ten JX1 cung so KHAC ten Linux -> NGHI BO SOT ==")
n1 = 0
for k in sorted(seen):
    s = "%d,%d,%d" % k
    if s in remap:
        continue
    ln_name = lnx.get(k, "")
    j_name = jx1.get(k, "<KHONG CO O JX1>")
    if norm(ln_name) != norm(j_name):
        n1 += 1
        print("  %-12s Linux=%-34s | JX1=%-34s  (%d cho)" % (s, ln_name[:34], j_name[:34], len(seen[k])))
        for rel, i, t in seen[k][:3]:
            print("      %s:%d %s" % (rel, i, t))
print("  => %d bo" % n1)
print()
print("== NHOM 2: khong remap va ten TRUNG (an toan) ==")
n2 = sum(1 for k in seen if "%d,%d,%d" % k not in remap and norm(lnx.get(k, "")) == norm(jx1.get(k, "\x00")))
print("  => %d bo" % n2)
