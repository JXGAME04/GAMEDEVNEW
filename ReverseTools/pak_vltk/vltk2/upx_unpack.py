# -*- coding: utf-8 -*-
"""Giai nen anh (image) cua tep PE bi nen UPX (NRV2B/2D/2E LE32) — chi de doc chuoi/ma,
KHONG dung lai import/reloc. Dung: python upx_unpack.py <exe> <out.bin> [src_off]
Ghi: <out.bin> = anh giai nen bat dau tu RVA cua section dau (UPX0)."""
import os, struct, sys, time
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import nrv

exe = sys.argv[1]
out = sys.argv[2]
d = open(exe, "rb").read()
pe = struct.unpack_from("<I", d, 0x3C)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]
optsz = struct.unpack_from("<H", d, pe + 20)[0]
ep = struct.unpack_from("<I", d, pe + 40)[0]
ib = struct.unpack_from("<I", d, pe + 52)[0]
sec = pe + 24 + optsz
secs = []
for i in range(nsec):
    s = d[sec + i * 40:sec + i * 40 + 40]
    name = s[:8].rstrip(b"\0").decode("latin-1")
    vsz, va, rsz, roff = struct.unpack_from("<IIII", s, 8)
    secs.append((name, va, vsz, roff, rsz))


def rva2off(r):
    for n, va, vsz, roff, rsz in secs:
        if va <= r < va + max(vsz, rsz):
            return roff + (r - va)
    return None


i = d.find(b"UPX!")
ph = d[i:i + 32]
ver, fmt, meth, lvl = ph[4], ph[5], ph[6], ph[7]
u_adler, c_adler, u_len, c_len, u_fsize = struct.unpack_from("<IIIII", ph, 8)
filt, cto = ph[28], ph[29]
print("packheader ver=%d fmt=%d method=%d u_len=%d c_len=%d u_file_size=%d filter=%#x cto=%#x" % (ver, fmt, meth, u_len, c_len, u_fsize, filt, cto))

if len(sys.argv) > 3:
    src_off = int(sys.argv[3], 0)
else:
    epo = rva2off(ep)
    stub = d[epo:epo + 16]
    src_off = None
    if stub[0] == 0x60 and stub[1] == 0xBE:
        src_va = struct.unpack_from("<I", stub, 2)[0]
        src_off = rva2off(src_va - ib)
        print("stub: pushad; mov esi,%#x -> file off %#x" % (src_va, src_off))
    if src_off is None:
        src_off = secs[1][3]
        print("stub khong nhan ra, dung dau UPX1:", hex(src_off))

fn = nrv.UPX_METHOD.get(meth)
if fn is None:
    print("method %d chua ho tro (14=LZMA)" % meth)
    sys.exit(1)
blob = d[src_off:src_off + c_len + 64]
t = time.time()
img = fn(blob)
print("giai nen xong %d byte trong %.1fs (u_len=%d)" % (len(img), time.time() - t, u_len))
if len(img) != u_len:
    print("!! kich thuoc lech: %d vs %d" % (len(img), u_len))
open(out, "wb").write(img)
print("da ghi", out, "base RVA", hex(secs[0][1]), "imagebase", hex(ib))
