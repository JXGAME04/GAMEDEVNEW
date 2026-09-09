# -*- coding: utf-8 -*-
"""Doc vung nho bat ky trong MINIDUMP (Memory64List) - khong nap ca 11 GB.
  python doc_bo_nho.py <dmp> <addr hex> [so byte]
"""
import struct, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

p = sys.argv[1]
f = open(p, "rb")
def rd(off, n):
    f.seek(off); return f.read(n)

sig, ver, nstream, rvastream = struct.unpack("<4sIII", rd(0, 16))
assert sig == b"MDMP"
dirs = {}
for i in range(nstream):
    t, sz, rva = struct.unpack("<III", rd(rvastream + 12 * i, 12))
    dirs[t] = (sz, rva)

# Memory64List: QWORD NumberOfRanges, QWORD BaseRva, then (QWORD start, QWORD size)*
sz, rva = dirs[9]
nranges, baserva = struct.unpack("<QQ", rd(rva, 16))
ranges = []
cur = baserva
raw = rd(rva + 16, 16 * nranges)
for i in range(nranges):
    st, ln = struct.unpack_from("<QQ", raw, 16 * i)
    ranges.append((st, ln, cur))
    cur += ln

def read_mem(addr, n):
    out = b""
    while n > 0:
        hit = None
        for st, ln, fo in ranges:
            if st <= addr < st + ln:
                hit = (st, ln, fo); break
        if not hit:
            break
        st, ln, fo = hit
        take = min(n, st + ln - addr)
        out += rd(fo + (addr - st), take)
        addr += take; n -= take
    return out

addr = int(sys.argv[2], 16)
n = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x40
b = read_mem(addr, n)
print("doc %d/%d byte tai 0x%X" % (len(b), n, addr))
for i in range(0, len(b), 16):
    ch = b[i:i + 16]
    hexs = " ".join("%02X" % c for c in ch)
    txt = "".join(chr(c) if 32 <= c < 127 else "." for c in ch)
    print("  +%03X  %-47s  %s" % (i, hexs, txt))
print("\ndoc theo QWORD:")
for i in range(0, min(len(b), 0x40), 8):
    v = struct.unpack_from("<Q", b, i)[0]
    print("  +%02X = 0x%016X %s" % (i, v, "<-- NULL" if v == 0 else ""))
