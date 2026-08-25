# -*- coding: utf-8 -*-
"""In cac dong Lua goi cac ham NHOM B (co ngu canh) de doi chieu ngu nghia."""
import io, os, re, sys, json

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
HERE = os.path.dirname(os.path.abspath(__file__))
LNX = r"D:\ServerLinux\server1\script"

raw = json.load(io.open(os.path.join(HERE, "api_gap_raw.json"), encoding="utf-8"))
names = sorted(n for n, v in raw.items() if v["nhom"] == "B")
if len(sys.argv) > 1:
    names = [n for n in names if n in set(sys.argv[1:])]

for n in names:
    print("\n########## %s  (%s)" % (n, raw[n]["dia_chi_linux"] or "khong xuat ra Lua"))
    shown = 0
    for site in raw[n]["vi_tri_goi"]:
        rel, ln = site.rsplit(":", 1)
        ln = int(ln)
        p = os.path.join(LNX, rel.replace("/", os.sep))
        if not os.path.isfile(p):
            continue
        lines = io.open(p, "rb").read().decode("gbk", "replace").split("\n")
        print("  --- %s:%d" % (rel, ln))
        for i in range(max(0, ln - 3), min(len(lines), ln + 3)):
            mark = ">>" if i + 1 == ln else "  "
            print("   %s %5d| %s" % (mark, i + 1, lines[i].rstrip()[:150]))
        shown += 1
        if shown >= 3:
            break
