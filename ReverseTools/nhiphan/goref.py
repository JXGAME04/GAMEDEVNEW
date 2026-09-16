# -*- coding: utf-8 -*-
# Tim ham nao nhac toi mot dia chi (quet byte 4-byte LE trong .text)
import sys, struct
sys.path.insert(0, ".")
from godis import G

g = G(sys.argv[1]); g.init_cs()
o0, v0, f0, fl = g.L[0]
blk = g.d[o0:o0 + f0]
for a in sys.argv[2:]:
    tgt = int(a, 16)
    print("### %x" % tgt)
    pat = struct.pack("<I", tgt)
    i = blk.find(pat)
    while i != -1:
        va = v0 + i
        f = g.name_of(va)
        print("   %08x  %s" % (va, f[1] if f else "?"))
        i = blk.find(pat, i + 1)
