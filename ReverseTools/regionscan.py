#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Dem so tep <x>_Region_S.dat / _Region_C.dat cua mot ban do trong cac pak."""
import os, struct, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)


def name2id(s):
    uid = 0
    idx = 0
    for ch in s:
        c = ord(ch)
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


def load(p):
    d = open(p, "rb").read()
    sig, count, ioff, doff = struct.unpack("<IIII", d[:16])
    return set(struct.unpack("<I", d[ioff + i * 16: ioff + i * 16 + 4])[0] for i in range(count))


def scan(pakdir, mappath, rng=260):
    ids = set()
    for f in os.listdir(pakdir):
        if f.lower().endswith((".pak", ".mps")):
            try:
                ids |= load(os.path.join(pakdir, f))
            except Exception:
                pass
    base = BS + "maps" + BS + mappath
    nS = nC = 0
    for y in range(rng):
        for x in range(rng):
            for suf, tag in ((("_Region_S.dat"), "S"), (("_Region_C.dat"), "C")):
                k = "%s%sv_%03d%s%03d%s" % (base, BS, y, BS, x, suf)
                if name2id(k) in ids:
                    if tag == "S":
                        nS += 1
                    else:
                        nC += 1
    return nS, nC


if __name__ == "__main__":
    pakdir = sys.argv[1]
    for mp in sys.argv[2:]:
        b = mp.encode("utf-8").decode("utf-8").encode("gbk", "replace").decode("latin-1")
        s, c = scan(pakdir, b)
        print("  S=%-5d C=%-5d  %s" % (s, c, mp))
