# -*- coding: utf-8 -*-
"""Quet chuoi trong anh nhi phan da giai nen. Dung:
python strscan.py <img.bin> <baseRVA hex> <imagebase hex> <regex> [maxhits]
In: RVA, VA, chuoi (ASCII) ; ngoai ra quet GBK doan co ky tu Trung."""
import re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
img = open(sys.argv[1], "rb").read()
base = int(sys.argv[2], 16)
ib = int(sys.argv[3], 16)
rx = re.compile(sys.argv[4].encode("latin-1"), re.I)
maxhits = int(sys.argv[5]) if len(sys.argv) > 5 else 400
# ASCII strings >= 4
n = 0
for m in re.finditer(rb"[\x20-\x7e]{4,}", img):
    s = m.group()
    if rx.search(s):
        off = m.start()
        print("%08X  %08X  %s" % (base + off, ib + base + off, s.decode("latin-1")))
        n += 1
        if n >= maxhits:
            print("... (cat)")
            break
print("ascii hits", n)
