# -*- coding: utf-8 -*-
import sys
sys.path.insert(0, ".")
from godis import G

g = G(sys.argv[1]); g.init_cs()
a = int(sys.argv[2], 16); e = int(sys.argv[3], 16)
o = g.v2o(a)
for I in g.md.disasm(g.d[o:o + (e - a)], a):
    ann = ""
    if I.mnemonic == "call":
        try:
            t = int(I.op_str, 16)
            f = g.name_of(t)
            if f: ann = "  ; -> " + f[1].split("bishop.")[-1]
        except ValueError:
            pass
    print("  %08x  %-42s%s" % (I.address, I.mnemonic + " " + I.op_str, ann))
