#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Nhan dang tep woods theo chuoi Unicode cua NTFS: thu cac chuoi giai ma de quy ve ten GBK goc.
Quy uoc du an (ACP 1252): ten tep tren dia = byte GBK giai ma cp1252 (vd settings/maps/great_night/³¤°×É½±±Â´.txt)."""
import os, sys, hashlib
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
D = u"D:\\ServerLinux\\server1\\settings\\battles\\maps\\woods"
want = [u"后营2.txt", u"大营2.txt", u"大营2野外.txt", u"后营2trap.txt", u"大营2to后营2trap.txt", u"大营2to野外trap.txt", u"帅旗2.txt",
        u"后营4.txt", u"大营4.txt", u"大营4野外.txt", u"后营4trap.txt", u"大营4to后营4trap.txt", u"大营4to野外trap.txt", u"帅旗4.txt"]

def chains(name):
    """tra ve danh sach (mo ta, ten GBK giai ma) cho cac chuoi giai ma kha di"""
    out = []
    tries = [
        ("cp1252->gbk", lambda s: s.encode("cp1252").decode("gbk")),
        ("cp437->gbk", lambda s: s.encode("cp437").decode("gbk")),
        ("cp1252->utf8->cp1252->gbk", lambda s: s.encode("cp1252").decode("utf-8").encode("cp1252").decode("gbk")),
        ("cp1252->utf8->cp437->gbk", lambda s: s.encode("cp1252").decode("utf-8").encode("cp437").decode("gbk")),
        ("cp1252->utf8->utf8->cp1252->gbk", lambda s: s.encode("cp1252").decode("utf-8").encode("cp1252").decode("utf-8").encode("cp1252").decode("gbk")),
    ]
    for d, f in tries:
        try:
            out.append((d, f(name)))
        except Exception:
            pass
    return out

bywant = {}
for n in sorted(os.listdir(D)):
    p = os.path.join(D, n)
    data = open(p, "rb").read()
    h = hashlib.md5(data).hexdigest()[:8]
    hit = None
    for d, g in chains(n):
        if g in want:
            hit = (d, g)
            break
    print("%-50s %6d %s %s" % (n[:50], len(data), h, ("=> %s (%s)" % hit) if hit else ""))
    if hit:
        bywant.setdefault(hit[1], []).append((n, h, len(data)))
print()
for w in want:
    lst = bywant.get(w, [])
    hs = set(x[1] for x in lst)
    print("%-24s %d ban, md5 %s %s" % (w, len(lst), ",".join(sorted(hs)), "<<< NOI DUNG KHAC NHAU" if len(hs) > 1 else ""))
    # ten tren dia theo quy uoc du an
    print("      ten tren dia (ACP1252): %s" % w.encode("gbk").decode("cp1252"))
