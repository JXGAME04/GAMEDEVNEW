# -*- coding: utf-8 -*-
"""Liet ke TUNG dong da doi giua ban Linux goc va ban trien khai JX1 (chi .lua manifest)."""
import io, os, re, sys, json
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2

LNX = r"D:\ServerLinux\server1"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HERE = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong"
remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
manifest = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
NUM = re.compile(r"\d+")
tot = 0
for rel in manifest:
    if not rel.lower().endswith(".lua"):
        continue
    a = os.path.join(LNX, rel.replace("/", os.sep)); b = os.path.join(JX1, rel.replace("/", os.sep))
    if not (os.path.isfile(a) and os.path.isfile(b)):
        continue
    la = open(a, "rb").read().split(b"\n"); lb = open(b, "rb").read().split(b"\n")
    if len(la) != len(lb):
        print("### %s SO DONG KHAC: %d -> %d" % (rel, len(la), len(lb)))
    n = 0
    for i, (x, y) in enumerate(zip(la, lb), 1):
        if x == y:
            continue
        sx = decline2(x.rstrip(b"\r")); sy = decline2(y.rstrip(b"\r"))
        # bo qua dong chi doi do AddNpc->AddNpcEx / Include
        if "AddNpc(" in sx and "AddNpcEx(" in sy and NUM.findall(sx) == [t for t in NUM.findall(sy)][:0] + NUM.findall(sx):
            pass
        n += 1
        tot += 1
        print("%s:%d" % (rel, i))
        print("   -%s" % sx.strip()[:140])
        print("   +%s" % sy.strip()[:140])
print("TONG dong khac:", tot)
