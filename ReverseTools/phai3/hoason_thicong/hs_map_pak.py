# -*- coding: utf-8 -*-
"""hs_map_pak.py : dong goi du lieu MAY CHU ban do 987 (Hoa Son 2013) tu D:\ServerLinux\server1\pak\maps.pak
thanh pak JX1 (XPackFile: header 'PACK', bang index SAP XEP theo uid, du lieu RAW flag 0).
  python hs_map_pak.py <out.pak>
"""
import os, struct, sys, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P
import ucl
BS = chr(92)
def gb(s): return s.encode("gbk").decode("latin-1")
SRC = r"D:\ServerLinux\server1\pak\maps.pak"

def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF; fl = cf >> 24
    f.seek(off); raw = f.read(cs if cs else size)
    if fl == 0: return raw
    return ucl.nrv2b_decompress_8(raw, size)

def main():
    out = sys.argv[1]
    base = BS + "maps" + BS + gb("西北南区") + BS + gb("华山派2013")
    names = [base + ".wor"]
    for y in range(87, 106):
        for x in range(74, 100):
            names.append(base + BS + "v_%03d" % y + BS + "%03d_region_s.dat" % x)
            names.append(base + BS + "v_%03d" % y + BS + "%03d_region_c.dat" % x)
    want = {P.name2id(n): n for n in names}
    f, es = P.entries(SRC)
    got = {}
    for e in es:
        if e[0] in want:
            got[e[0]] = blob_of(f, e)
    f.close()
    print("tim thay %d/%d ten (wor+region_s+region_c)" % (len(got), len(want)))
    ns = sum(1 for u in got if want[u].endswith("_region_s.dat")); nc = sum(1 for u in got if want[u].endswith("_region_c.dat"))
    print("  region_s:", ns, " region_c:", nc, " wor:", sum(1 for u in got if want[u].endswith(".wor")))
    # ghi pak: header 16 + index 16*n + data
    uids = sorted(got)
    n = len(uids)
    hdr_size = 16; idx_size = 16 * n
    data = bytearray(); index = bytearray()
    off = hdr_size + idx_size
    for u in uids:
        d = got[u]
        index += struct.pack("<IIiI", u, off, len(d), len(d))   # flag 0 (RAW), csize = size
        data += d; off += len(d)
    with open(out, "wb") as w:
        w.write(struct.pack("<4sIII", b"PACK", n, hdr_size, hdr_size + idx_size))
        w.write(index); w.write(data)
    # doi chung: doc lai bang pakdump
    f2, es2 = P.entries(out)
    ok = 0
    for e in es2:
        d = f2.read(0) if False else None
        f2.seek(e[1]); raw = f2.read(e[2])
        if raw == got[e[0]]: ok += 1
    f2.close()
    print("ghi %s: %d entry, %d byte, doc lai khop %d/%d" % (out, n, os.path.getsize(out), ok, n))

main()
