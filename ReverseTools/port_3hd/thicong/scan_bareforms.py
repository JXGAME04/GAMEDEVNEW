# -*- coding: utf-8 -*-
"""Voi 42 particular remap, tim MOI cho dung dang KHAC 6,1,<pt> trong tep manifest:
   - "== <pt>"  /  "==<pt>"   (so sanh parttype/particular)
   - GetItemProp / tbProp cua GENRE khac (2,1,<pt>)
De biet cho nao regex 6,1,<pt> BO SOT -> phai va tay."""
import io, os, re, csv, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
P3 = os.path.dirname(HERE)
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

rows = list(csv.reader(io.open(os.path.join(P3, "id_dungdo.csv"), encoding="utf-8")))
parts = set()
for r in rows[1:]:
    if r[0].strip("\ufeff") != "ITEM" or "NANG" not in r[4]:
        continue
    parts.add(r[1].strip().split(",")[2])

manifest = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
for rel in manifest:
    if not rel.lower().endswith(".lua"):
        continue
    p = os.path.join(JX1, rel.replace("/", os.sep))
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    for i, ln in enumerate(d.split("\n"), 1):
        for pt in parts:
            # dang "== 399" hoac "==399" (khong phai 6,1,399)
            if re.search(r"==\s*%s(?![0-9])" % pt, ln) and not re.search(r"[0-9],\s*[0-9],\s*%s" % pt, ln):
                print("%-46s :%d  %s" % (rel.rsplit("/", 1)[-1], i, ln.strip()[:80]))
