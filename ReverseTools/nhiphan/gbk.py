# -*- coding: utf-8 -*-
# Trich chuoi GBK (2 byte) trong ELF, chi giu doan toan chu Han/dau cau CJK
import io, struct, sys

path = sys.argv[1]
outp = sys.argv[2]
lo = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0
hi = int(sys.argv[4], 16) if len(sys.argv) > 4 else 1 << 62
d = io.open(path, "rb").read()
eph, = struct.unpack_from("<I", d, 0x1C)
phe, phn = struct.unpack_from("<HH", d, 0x2A)
L = []
for i in range(phn):
    o = eph + i * phe
    pt, off, va, pa, fsz, msz, fl, al = struct.unpack_from("<8I", d, o)
    if pt == 1 and fsz: L.append((off, va, fsz))

def o2v(off):
    for o0, v0, f in L:
        if o0 <= off < o0 + f: return v0 + (off - o0)
    return 0

def cjk(c):
    o = ord(c)
    return 0x4e00 <= o <= 0x9fff or 0x3000 <= o <= 0x303f or 0xff00 <= o <= 0xffef

res = []
i = max(lo, 0); n = min(len(d), hi)
while i < n:
    if 0x81 <= d[i] <= 0xfe and i + 1 < n and 0x40 <= d[i+1] <= 0xfe and d[i+1] != 0x7f:
        j = i; cnt = 0
        while j + 1 < n and 0x81 <= d[j] <= 0xfe and 0x40 <= d[j+1] <= 0xfe and d[j+1] != 0x7f:
            j += 2; cnt += 1
        if cnt >= 2:
            try:
                s = d[i:j].decode("gbk")
                if sum(1 for c in s if cjk(c)) >= max(2, int(len(s) * 0.9)):
                    res.append((i, o2v(i), s))
            except Exception:
                pass
        i = j
    else:
        i += 1
with io.open(outp, "w", encoding="utf-8") as f:
    f.write("tong: %d\n" % len(res))
    for off, va, s in res:
        f.write("%08x %08x %s\n" % (off, va, s))
print("tong:", len(res), "->", outp)
