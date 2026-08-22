#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Doi chieu NPC template (id = dong-2) giua Linux va du an cho Bach Nhan Loi Dai."""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
LNX = "D:" + BS + "ServerLinux" + BS + "server1" + BS + "settings" + BS + "npcs.txt"
SRV = "E:" + BS + "SourceTuanLe" + BS + "SourceVs22" + BS + "TESTLOFFF_ONLINE" + BS + "bin" + BS + "server" + BS + "settings" + BS + "npcs.txt"
CLI = "E:" + BS + "SourceTuanLe" + BS + "SourceVs22" + BS + "TESTLOFFF_ONLINE" + BS + "bin" + BS + "client" + BS + "settings" + BS + "npcs.txt"
IDS = [1786,1787,1788,1789,1790,1791,1792,1793,1794,1795,1571,393,625,389,1747]

def tcvn(b):
    # hien thi bang latin-1 (chi de so sanh byte)
    return b.decode("latin-1")

def load(p):
    rows = io.open(p, "rb").read().split(b"\n")
    hdr = rows[0].rstrip(b"\r").split(b"\t")
    return hdr, rows

for label, p in [("LINUX", LNX), ("DUAN-SRV", SRV), ("DUAN-CLI", CLI)]:
    try:
        hdr, rows = load(p)
    except Exception as e:
        print(label, "LOI", e); continue
    print("=== %s  cot=%d dong=%d" % (label, len(hdr), len(rows)))
    hl = [h.decode("latin-1") for h in hdr]
    def col(name):
        return hl.index(name) if name in hl else -1
    ci, cn, ck, cc, cl = col("Id"), col("Name"), col("Kind"), col("Camp"), col("Level")
    cs = col("Series")
    for i in IDS:
        ln = i + 1  # dong = id+2 -> chi so 0-based = id+1
        if ln >= len(rows):
            print("  %5d: NGOAI BANG" % i); continue
        f = rows[ln].rstrip(b"\r").split(b"\t")
        def g(c):
            return tcvn(f[c]) if 0 <= c < len(f) else "?"
        print("  %5d: Id=%s Name=%r Kind=%s Camp=%s Level=%s Series=%s" % (i, g(ci), f[cn] if cn>=0 else b"?", g(ck), g(cc), g(cl), g(cs)))
