# -*- coding: utf-8 -*-
# Tim ham Go tham chieu toi mot chuoi (qua go.string header hoac lea truc tiep)
import sys, struct, re
sys.path.insert(0, ".")
from godis import G

g = G(sys.argv[1]); g.init_cs()
needle = sys.argv[2].encode("latin-1")
# 1) tim vi tri byte cua chuoi
i = g.d.find(needle)
targets = set()
while i != -1:
    va = g.o2v(i)
    if va: targets.add(va)
    i = g.d.find(needle, i + 1)
print("chuoi o VA:", [hex(v) for v in targets])
# 2) tim header (ptr,len) tro toi chuoi
hdr = set()
for va in targets:
    pat = struct.pack("<Q", va)
    j = g.d.find(pat)
    while j != -1:
        hv = g.o2v(j)
        if hv:
            ln, = struct.unpack_from("<Q", g.d, j + 8)
            if ln == len(needle) or 0 < ln < 200:
                hdr.add(hv)
        j = g.d.find(pat, j + 1)
print("header o VA:", [hex(v) for v in hdr])
want = targets | hdr
# 3) quet .text tim lea rip-rel toi cac VA do
o0, v0, f0, fl = g.L[0]
for va, nm in g.funcs:
    r = g.frange(va)
    if not r: continue
    a, e = r
    if e - a > 0x8000: e = a + 0x8000
    o = g.v2o(a)
    if o is None: continue
    for I in g.md.disasm(g.d[o:o + (e - a)], a):
        if I.mnemonic == "lea" and "rip" in I.op_str:
            m = re.search(r"\[rip ([+-]) (0x[0-9a-f]+)\]", I.op_str)
            if m:
                t = I.address + I.size + (1 if m.group(1) == "+" else -1) * int(m.group(2), 16)
                if t in want:
                    print("  %-70s %08x  %s" % (nm, I.address, I.op_str))
