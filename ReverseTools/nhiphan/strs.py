# -*- coding: utf-8 -*-
# Trich chuoi ASCII + GBK tu ELF, kem file-offset va VA
import io, struct, sys, re

def loads(path):
    d = io.open(path, "rb").read()
    eph, = struct.unpack_from("<I", d, 0x1C)
    phe, phn = struct.unpack_from("<HH", d, 0x2A)
    L = []
    for i in range(phn):
        o = eph + i * phe
        pt, off, va, pa, fsz, msz, fl, al = struct.unpack_from("<8I", d, o)
        if pt == 1 and fsz:
            L.append((off, va, fsz))
    return d, L

def o2v(L, off):
    for o0, va, fsz in L:
        if o0 <= off < o0 + fsz:
            return va + (off - o0)
    return None

def scan(path, minlen=4):
    d, L = loads(path)
    out = []
    cur = bytearray(); start = 0
    for i, c in enumerate(d):
        if 0x20 <= c < 0x7f or c in (9,):
            if not cur: start = i
            cur.append(c)
        else:
            if len(cur) >= minlen:
                out.append((start, o2v(L, start), cur.decode("latin-1")))
            cur = bytearray()
    if len(cur) >= minlen:
        out.append((start, o2v(L, start), cur.decode("latin-1")))
    return out

if __name__ == "__main__":
    path = sys.argv[1]
    pat = sys.argv[2] if len(sys.argv) > 2 else None
    ml = int(sys.argv[3]) if len(sys.argv) > 3 else 4
    rx = re.compile(pat, re.I) if pat else None
    for off, va, s in scan(path, ml):
        if rx and not rx.search(s):
            continue
        print("%08x %s %s" % (off, ("%08x" % va) if va else "--------", s))
