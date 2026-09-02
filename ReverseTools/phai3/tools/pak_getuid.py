# -*- coding: utf-8 -*-
"""pak_getuid.py <pak> <outdir> <uid1> [uid2 ...] : rut dung cac entry theo uid (hex) tu 1 pak, giai nen UCL, ghi outdir/<uid>.bin"""
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
        return raw
    return ucl.nrv2b_decompress_8(raw, size)

def main():
    pak, outdir = sys.argv[1], sys.argv[2]
    want = set(int(x, 16) for x in sys.argv[3:])
    os.makedirs(outdir, exist_ok=True)
    f, es = P.entries(pak)
    got = 0
    for e in es:
        if e[0] in want:
            d = blob_of(f, e)
            open(os.path.join(outdir, "%08X.bin" % e[0]), "wb").write(d)
            print("  %08X %d byte" % (e[0], len(d)))
            got += 1
    print("lay %d/%d tu %s" % (got, len(want), os.path.basename(pak)))

main()
