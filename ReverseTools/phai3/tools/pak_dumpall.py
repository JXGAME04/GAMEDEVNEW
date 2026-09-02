# -*- coding: utf-8 -*-
"""pak_dumpall.py <pak> <outdir> : giai nen MOI entry (UCL NRV2B) ra outdir/<uid>.bin + index.tsv"""
import os, sys, io
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P
import ucl

def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    f.seek(off)
    raw = f.read(cs if cs else size)
    if fl == 0:
        return raw, "raw"
    try:
        return ucl.nrv2b_decompress_8(raw, size), "ucl"
    except Exception as ex:
        return raw, "ERR:%s" % ex

def sniff(d):
    h = d[:8]
    if h.startswith(b"SPR"): return "spr"
    if h.startswith(b"WAV") or h.startswith(b"RIFF"): return "wav"
    if h.startswith(b"MPS"): return "mps"
    n = len(d)
    if n == 0: return "empty"
    s = d[:4096]
    ok = sum(1 for b in s if 9 <= b <= 13 or 32 <= b < 127 or b >= 0x80)
    if ok >= len(s) * 0.97 and b"\x00" not in s:
        return "txt"
    return "bin"

def main():
    pak, outdir = sys.argv[1], sys.argv[2]
    os.makedirs(outdir, exist_ok=True)
    f, es = P.entries(pak)
    idx = io.open(os.path.join(outdir, "index.tsv"), "w", encoding="utf-8")
    idx.write("uid\tsize\tcsize\tflag\thow\tkind\thead\n")
    for e in es:
        d, how = blob_of(f, e)
        k = sniff(d)
        open(os.path.join(outdir, "%08X.bin" % e[0]), "wb").write(d)
        head = d[:60].decode("latin-1").replace("\t", " ").replace("\r", " ").replace("\n", " ")
        idx.write("%08X\t%d\t%d\t%d\t%s\t%s\t%s\n" % (e[0], e[2], e[3] & 0xFFFFFF, e[3] >> 24, how, k, head))
    idx.close()
    print("xong %s: %d entry -> %s" % (pak, len(es), outdir))

main()
