# -*- coding: utf-8 -*-
"""Nhin 96 byte dau cua MOI entry trong pak (giai nen som) de phan loai; entry dang chu
thi giai nen tron va ghi ra outdir/<uid>.txt. Dung: python pakpeek.py <pak> <outdir> [maxtext]
"""
import os, sys, struct, time, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import nrv

pak, outdir = sys.argv[1], sys.argv[2]
maxtext = int(sys.argv[3]) if len(sys.argv) > 3 else 8 * 1024 * 1024
os.makedirs(outdir, exist_ok=True)

f = open(pak, "rb")
sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
f.seek(ioff)
raw = f.read(count * 16)


def classify(h):
    if h.startswith(b"SPR"):
        return "SPR"
    if h.startswith(b"RIFF"):
        return "WAV"
    if h.startswith(b"ID3") or h[:2] == b"\xff\xfb":
        return "MP3"
    if h[:2] == b"BM":
        return "BMP"
    if h.startswith(b"\x89PNG"):
        return "PNG"
    if h.startswith(b"MZ"):
        return "EXE"
    if h.startswith(b"\xff\xfe") or h.startswith(b"\xfe\xff"):
        return "UTF16"
    printable = sum(1 for c in h if 32 <= c < 127 or c in (9, 10, 13) or c >= 0x80)
    ctrl = sum(1 for c in h if c < 9 or (13 < c < 32) or c == 127)
    if ctrl == 0 and printable == len(h):
        return "TEXT"
    if ctrl * 20 < len(h):
        return "TEXT?"
    return "BIN"


types = collections.Counter()
t0 = time.time()
ntext = 0
fails = 0
listing = open(os.path.join(outdir, "_list.txt"), "w", encoding="utf-8")
for i in range(count):
    uid, off, size, cf = struct.unpack_from("<IIiI", raw, i * 16)
    csize = cf & 0xFFFFFF
    flag = cf >> 24
    f.seek(off)
    if flag == 0 or csize in (0, size):
        head = f.read(min(96, size))
        comp = None
    else:
        comp = f.read(csize)
        try:
            head = nrv.nrv2b_8(comp, limit=96)
        except Exception:
            head = b""
            fails += 1
    t = classify(head) if head else "FAIL"
    types[t] += 1
    listing.write("%08X\t%d\t%d\t%s\t%r\n" % (uid, size, flag, t, head[:24]))
    if t in ("TEXT", "TEXT?", "UTF16") and size <= maxtext:
        try:
            if comp is None:
                f.seek(off)
                data = f.read(size)
            else:
                data = nrv.nrv2b_8(comp)
            open(os.path.join(outdir, "%08X.txt" % uid), "wb").write(data)
            ntext += 1
        except Exception as ex:
            fails += 1
listing.close()
print("pak", pak, "entries", count, "types", dict(types), "text dumped", ntext, "fails", fails, "%.1fs" % (time.time() - t0))
