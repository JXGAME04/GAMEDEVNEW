# -*- coding: utf-8 -*-
import io
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\npcs.txt"
d = io.open(P, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")
co = 0
mau = []
for l in d[1:]:
    c = l.split("\t")
    if len(c) > 32:
        for k in (31, 32):
            v = c[k].strip()
            if v and v != "0":
                co += 1
                if len(mau) < 6:
                    mau.append(v)
print("so o ActionScript/LevelScript co gia tri:", co)
print("vi du:")
for m in mau:
    print("   ", repr(m))
