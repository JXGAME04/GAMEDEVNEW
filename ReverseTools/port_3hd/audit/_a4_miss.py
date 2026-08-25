# -*- coding: utf-8 -*-
import os, sys
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_3hd")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from dec2 import decline2
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
p = os.path.join(JX1, "settings", "item", "magicscript.txt")
d = open(p, "rb").read().split(b"\n")
tot = 0; miss = 0; ex = []
BS = chr(92)
for ln in d[1:]:
    c = decline2(ln.rstrip(b"\r")).split("\t")
    if len(c) < 10:
        continue
    s = c[9].strip()
    if not s or s == "0":
        continue
    tot += 1
    rel = s.replace("/", BS).lstrip(BS)
    if not os.path.isfile(os.path.join(JX1, rel)):
        miss += 1
        if len(ex) < 20:
            ex.append((c[0][:26], "%s,%s,%s" % (c[1], c[2], c[3]), s))
print("JX1 magicscript: so muc CO script =", tot, "; THIEU TEP =", miss)
for e in ex:
    print("   ", e)
