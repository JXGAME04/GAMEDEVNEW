# -*- coding: utf-8 -*-
# Doc gopclntab (Go 1.2 - 1.15, magic fffffffb) cua ELF64 de lay bang ten ham -> dia chi
import io, struct, sys

class GoBin:
    def __init__(s, path):
        s.d = io.open(path, "rb").read()
        d = s.d
        e_phoff, = struct.unpack_from("<Q", d, 0x20)
        phe, phn = struct.unpack_from("<HH", d, 0x36)
        s.L = []
        for i in range(phn):
            o = e_phoff + i * phe
            pt, fl, off, va, pa, fsz, msz, al = struct.unpack_from("<IIQQQQQQ", d, o)
            if pt == 1 and fsz:
                s.L.append((off, va, fsz, fl))
        s.funcs = []
        s.find_pclntab()

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

    def find_pclntab(s):
        d = s.d
        magic = b"\xfb\xff\xff\xff\x00\x00"
        i = 0
        while True:
            i = d.find(magic, i)
            if i == -1: break
            if d[i + 6] in (1, 2, 4) and d[i + 7] == 8:
                try:
                    n, = struct.unpack_from("<Q", d, i + 8)
                    if 100 < n < 200000:
                        s.parse(i, n); return
                except Exception:
                    pass
            i += 1
        raise RuntimeError("khong thay pclntab")

    def parse(s, base, n):
        d = s.d
        s.base = base
        ftaboff = base + 16
        for k in range(n):
            entry, funcoff = struct.unpack_from("<QQ", d, ftaboff + k * 16)
            fo = base + funcoff
            va, nameoff = struct.unpack_from("<Qi", d, fo)
            no = base + nameoff
            e = d.index(b"\0", no)
            s.funcs.append((va, d[no:e].decode("latin-1")))
        s.funcs.sort()

    def name_of(s, va):
        lo, hi = 0, len(s.funcs) - 1
        r = None
        while lo <= hi:
            mid = (lo + hi) // 2
            if s.funcs[mid][0] <= va:
                r = s.funcs[mid]; lo = mid + 1
            else:
                hi = mid - 1
        return r

    def addr_of(s, name):
        return [(va, nm) for va, nm in s.funcs if name in nm]

    def frange(s, va):
        for i, (a, nm) in enumerate(s.funcs):
            if a == va:
                end = s.funcs[i + 1][0] if i + 1 < len(s.funcs) else a + 0x2000
                return a, end
        return None

if __name__ == "__main__":
    g = GoBin(sys.argv[1])
    print("so ham:", len(g.funcs))
    q = sys.argv[2] if len(sys.argv) > 2 else None
    if q:
        for va, nm in g.addr_of(q):
            print("%012x %s" % (va, nm))
