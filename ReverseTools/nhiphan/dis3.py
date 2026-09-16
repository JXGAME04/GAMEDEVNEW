# -*- coding: utf-8 -*-
# Tang tren dis2: giai chuoi tham chieu kieu PIC  lea reg,[ebx +/- off]  voi ebx = _GLOBAL_OFFSET_TABLE_
import sys, re, struct
sys.path.insert(0,".")
from dis2 import Lib as _Lib
from capstone import *
class Lib(_Lib):
    def gotbase(s):
        return s.pltgot
    def strat(s, va, maxlen=160):
        o = s.e.v2o(va)
        if o is None or o >= len(s.d): return None
        e = o
        while e < len(s.d) and s.d[e] != 0 and e-o < maxlen: e += 1
        t = s.d[o:e]
        if len(t) >= 4 and all(9 <= c < 127 for c in t): return t.decode("latin-1")
        return None
    def full(s, fn, maxi=2000):
        ins = s.dis(fn, maxi)
        out = []
        G = s.gotbase()
        for i in ins:
            t = "%s %s" % (i.mnemonic, i.op_str); ann = ""
            if i.mnemonic == "call":
                try:
                    nm = s.name(int(i.op_str, 16))
                    if nm: ann = "  ; -> %s" % nm
                except ValueError: pass
            m = re.search(r"\[ebx ([+-]) (0x[0-9a-f]+)\]", i.op_str)
            if m:
                off = int(m.group(2), 16) * (1 if m.group(1) == "+" else -1)
                sv = s.strat((G + off) & 0xffffffff)
                if sv: ann = '  ; "%s"' % sv.replace("\n", "\n")
            m2 = re.search(r"\[ebx ([+-]) (0x[0-9a-f]+)\]", i.op_str)
            out.append((i.address, t, ann))
        return out
def dump(lib, fn, note="", maxi=2000, only=None):
    rows = lib.full(fn, maxi)
    if not rows: print("  !! khong co", fn); return
    print("\n--- %s %s ---" % (fn, note))
    for a, t, ann in rows:
        if only and not (ann or any(k in t for k in only)): continue
        print("   %06x  %-44s%s" % (a, t, ann))
