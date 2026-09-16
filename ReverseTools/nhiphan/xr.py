# -*- coding: utf-8 -*-
# xref theo hang so tuyet doi (ET_EXEC) + thao ma quanh diem tham chieu
import io, struct, sys
from capstone import *

class Bin:
    def __init__(s, path):
        s.d = io.open(path, "rb").read()
        eph, = struct.unpack_from("<I", s.d, 0x1C)
        phe, phn = struct.unpack_from("<HH", s.d, 0x2A)
        s.L = []
        for i in range(phn):
            o = eph + i * phe
            pt, off, va, pa, fsz, msz, fl, al = struct.unpack_from("<8I", s.d, o)
            if pt == 1 and fsz:
                s.L.append((off, va, fsz, fl))
        s.md = Cs(CS_ARCH_X86, CS_MODE_32)
        s.md.detail = False

    def v2o(s, va):
        for off, v0, fsz, fl in s.L:
            if v0 <= va < v0 + fsz:
                return off + (va - v0)
        return None

    def o2v(s, off):
        for o0, v0, fsz, fl in s.L:
            if o0 <= off < o0 + fsz:
                return v0 + (off - o0)
        return None

    def cstr(s, va, maxlen=300):
        o = s.v2o(va)
        if o is None: return None
        e = o
        while e < len(s.d) and s.d[e] and e - o < maxlen: e += 1
        t = s.d[o:e]
        try: return t.decode("latin-1")
        except Exception: return None

    def xrefs(s, va, execonly=True):
        pat = struct.pack("<I", va)
        res = []
        for off, v0, fsz, fl in s.L:
            if execonly and not (fl & 1): continue
            blk = s.d[off:off + fsz]
            i = blk.find(pat)
            while i != -1:
                res.append(v0 + i)
                i = blk.find(pat, i + 1)
        return res

    def dis(s, va, n=60, back=0):
        o = s.v2o(va - back)
        if o is None: return []
        return list(s.md.disasm(s.d[o:o + n * 8], va - back))[:n]

def show(b, va, n=40, back=0, tag=""):
    print("=== %08x %s ===" % (va, tag))
    for i in b.dis(va, n, back):
        ann = ""
        for op in i.op_str.replace(",", " ").replace("[", " ").replace("]", " ").split():
            if op.startswith("0x") and len(op) >= 8:
                try: v = int(op, 16)
                except ValueError: continue
                sv = b.cstr(v, 120)
                if sv and len(sv) >= 4 and all(9 <= ord(c) < 127 for c in sv):
                    ann += '  ; "%s"' % sv
        print("  %08x  %-40s%s" % (i.address, i.mnemonic + " " + i.op_str, ann))
