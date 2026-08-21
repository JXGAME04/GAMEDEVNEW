#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Quet MapList cua du an: map nao co .wor trong pak nao."""
import io, os, struct, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


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


def load_pak(p):
    d = open(p, "rb").read()
    sig, count, ioff, doff = struct.unpack("<IIII", d[:16])
    ids = {}
    for i in range(count):
        o = ioff + i * 16
        uid, off, size, cf = struct.unpack("<IIiI", d[o:o + 16])
        ids[uid] = size
    return ids


def maplist():
    d = {}
    for ln in io.open(os.path.join(SRV, "settings", "MapList.ini"), encoding="latin-1"):
        ln = ln.strip()
        if "=" in ln:
            k, v = ln.split("=", 1)
            if k.strip().isdigit():
                d[int(k)] = v.strip()
    return d


paks = {}
pdir = os.path.join(SRV, "Pak")
for f in os.listdir(pdir):
    if f.lower().endswith((".pak", ".mps")):
        try:
            paks[f] = load_pak(os.path.join(pdir, f))
        except Exception as e:
            print("loi", f, e)
print("pak:", {k: len(v) for k, v in paks.items()})

ml = maplist()
want = [int(x) for x in sys.argv[1:]] or sorted(ml)
nfound = 0
for i in want:
    v = ml.get(i)
    if not v:
        print("%4d  (khong co trong MapList)" % i)
        continue
    hits = []
    variant = v.replace(BS + BS, BS)
    key = BS + "maps" + BS + variant + ".wor"
    uid = name2id(key)
    for pn, ids in paks.items():
        if uid in ids:
            hits.append("%s(%dB)" % (pn, ids[uid]))
    disk = os.path.exists(os.path.join(SRV, "Maps", v.replace(BS + BS, BS) + ".wor"))
    if disk:
        hits.append("LOOSE")
    if hits:
        nfound += 1
    try:
        vv = v.encode("latin-1").decode("gbk", "replace")
    except Exception:
        vv = v
    print("%4d  %-45s %s" % (i, vv, ", ".join(hits) if hits else "-- KHONG THAY --"))
print("\nco du lieu: %d / %d" % (nfound, len(want)))
