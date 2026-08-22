#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Liet ke ten tep thu muc settings/battles/maps/woods theo BYTE tho, thu giai ma GBK de biet tep nao la ban dung."""
import os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
D = b"D:\\ServerLinux\\server1\\settings\\battles\\maps\\woods"
want = [u"后营2.txt", u"大营2.txt", u"大营2野外.txt", u"后营2trap.txt", u"大营2to后营2trap.txt", u"大营2to野外trap.txt", u"帅旗2.txt",
        u"后营4.txt", u"大营4.txt", u"大营4野外.txt", u"后营4trap.txt", u"大营4to后营4trap.txt", u"大营4to野外trap.txt", u"帅旗4.txt"]
wantb = {w.encode("gbk"): w for w in want}
found = {}
for n in os.listdir(D):
    p = os.path.join(D, n)
    sz = os.path.getsize(p)
    tag = ""
    if n in wantb:
        tag = "<== GBK DUNG: " + wantb[n]
        found[n] = sz
    print("%-60s %6d %s" % (n.decode("latin-1"), sz, tag))
print()
print("Tim thay %d/%d tep ten GBK dung" % (len(found), len(want)))
for w in want:
    b = w.encode("gbk")
    print("  %-28s %s" % (w, "OK %d byte" % found[b] if b in found else ">>> THIEU"))
# in noi dung cac tep GBK dung
for w in want:
    b = w.encode("gbk")
    if b in found:
        data = open(os.path.join(D, b), "rb").read()
        lines = data.split(b"\n")
        print("--- %s (%d dong): %s" % (w, len([l for l in lines if l.strip()]), b" | ".join(l.strip() for l in lines[:4]).decode("latin-1")))
