# -*- coding: utf-8 -*-
"""Quet cac tep vua chep (b1_manifest) tim cho dung 42 bo so item TRUNG NANG."""
import io, os, re, csv, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
P3 = os.path.dirname(HERE)
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

rows = list(csv.reader(io.open(os.path.join(P3, "id_dungdo.csv"), encoding="utf-8")))
remap = {}
for r in rows[1:]:
    if r[0].strip("\ufeff") != "ITEM" or "NANG" not in r[4]:
        continue
    m = re.search(r"->\s*([0-9]+,[0-9]+,[0-9]+)", r[5])
    if m:
        remap[r[1].strip()] = m.group(1)

manifest = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
hit = {}
for rel in manifest:
    if not rel.lower().endswith(".lua"):
        continue
    p = os.path.join(JX1, rel.replace("/", os.sep))
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    for k in remap:
        g, dd, pt = k.split(",")
        pat = re.compile(r"\b%s\s*,\s*%s\s*,\s*%s\b" % (g, dd, pt))
        if pat.search(d):
            hit.setdefault(rel, set()).add(k)

print("42 bo so remap:", len(remap))
print("So tep can remap:", len(hit))
for rel in sorted(hit):
    name = rel.rsplit("/", 1)[-1] if "/" in rel else rel
    print("  %-40s %s" % (name, sorted(hit[rel])))
