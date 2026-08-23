#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Liet ke TOAN BO muc cua map 984 (\maps\<GBK 特殊用地>\banghuichengbao) trong mot .pak.

Cach tra: bam FileNameToId (KPakList.cpp:72, & 0xFFFFFFFF TRUOC %, khong .lower()).
Quet region TUYET DOI 0..999 x 0..999 cho 10 mau ten tep region (SceneDataDef.h:12-47)
+ cac mau ten o cap thu muc map (.wor, .ini, .txt, Trap*, ObstacleTable*, *_srv.fp ...).

  python scan_984.py <pak> [pak2 ...]
"""
import os, struct, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
# 特殊用地 (GBK) - lay nguyen byte tu settings\MapList.ini dong 984
MAPDIR = b"\xcc\xd8\xca\xe2\xd3\xc3\xb5\xd8".decode("latin-1") + BS + "banghuichengbao"
BASE = BS + "maps" + BS + MAPDIR

REGION_FILES = ["Region_S.dat", "Region_C.dat", "OBSTACLE.DAT", "Trap.dat", "Npc_S.dat",
                "Npc_C.dat", "Obj_S.dat", "Obj_C.dat", "Ground.dat", "BuildinObj.Dat"]
# ten tep cap thu muc map (mau doan; .wor la chac chan - KSubWorld.cpp:1780)
DIR_FILES = [".wor", ".ini", ".txt", ".map", ".mps", ".wor.bak"]
DIR_SUB = ["Trap.txt", "Trap.ini", "ObstacleTable.txt", "Obstacle.txt", "NpcList.txt",
           "Npc.txt", "map.ini", "MapInfo.ini", "minimap.spr", "Ground.dat", "BuildinObj.Dat",
           "bg.ini", "weather.ini", "Weather.ini", "Region.ini", "Sound.ini", "sound.ini"]


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
    sig, count, ioff, doff = struct.unpack("<4sIII", d[:16])
    idx = {}
    for i in range(count):
        uid, off, size, cf = struct.unpack("<IIiI", d[ioff + i * 16: ioff + i * 16 + 16])
        idx[uid] = (off, size, cf)
    return sig, count, idx


def candidates(rng=1000):
    out = {}
    for suf in DIR_FILES:
        out[BASE + suf] = name2id(BASE + suf)
    for f in DIR_SUB:
        k = BASE + BS + f
        out[k] = name2id(k)
    for y in range(rng):
        vdir = "%s%sv_%03d" % (BASE, BS, y)
        for x in range(rng):
            for f in REGION_FILES:
                k = "%s%s%03d_%s" % (vdir, BS, x, f)
                out[k] = name2id(k)
    return out


if __name__ == "__main__":
    cand = candidates()
    for pak in sys.argv[1:]:
        sig, count, idx = load(pak)
        print("### %s  sig=%r  count=%d" % (pak, sig, count))
        hits = []
        tot = 0
        for k, uid in cand.items():
            if uid in idx:
                off, size, cf = idx[uid]
                hits.append((k, uid, off, size, cf))
                tot += size
        hits.sort(key=lambda t: t[2])
        for k, uid, off, size, cf in hits:
            kk = k.encode("latin-1").decode("gbk", "replace")
            print("  %08X off=%-9d size=%-7d flag=%08X  %s" % (uid, off, size, cf, kk))
        print("  => %d muc khop, tong %d byte goc; con %d muc trong pak KHONG thuoc map nay" % (len(hits), tot, count - len(hits)))
