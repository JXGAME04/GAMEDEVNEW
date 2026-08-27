# -*- coding: utf-8 -*-
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pakdump as P

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
OUT = os.path.dirname(os.path.abspath(__file__))

names = [
    BS + "script" + BS + "ui" + BS + "LianSaiGamble.lua",
]
if len(sys.argv) > 1:
    names = [a.replace("/", BS) for a in sys.argv[1:]]

want = {P.name2id(n): n for n in names}
found = set()
for fn in sorted(os.listdir(ROOT)):
    if not fn.lower().endswith((".pak", ".mps")):
        continue
    try:
        f, es = P.entries(os.path.join(ROOT, fn))
    except Exception:
        continue
    for e in es:
        if e[0] in want:
            d = P.read_entry(f, e)
            nm = want[e[0]]
            base = nm.split(BS)[-1]
            p = os.path.join(OUT, base)
            open(p, "wb").write(d)
            print("%-24s %-55s size=%d -> %s" % (fn, nm, len(d), base))
            found.add(e[0])
for uid, nm in want.items():
    if uid not in found:
        print("KHONG THAY  %-55s uid=%08X" % (nm, uid))
