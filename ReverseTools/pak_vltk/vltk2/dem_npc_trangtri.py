# -*- coding: utf-8 -*-
"""dem_npc_trangtri.py - dem NPC trang tri (phan 2) va OBJ (phan 3) trong Region_C.dat cua MOI map,
va dem so ban ghi LON NHAT trong mot vung 3x3 (dung bang so khe NPC client can dung khi bat lai).
Chi doc.
"""
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, BS  # noqa: E402

NPC_REC, OBJ_REC, HEAD = 60, 24, 12


def dem(b, rec):
    if len(b) <= HEAD:
        return 0
    n = struct.unpack("<I", b[:4])[0]
    if n > 20000:
        return 0
    p = HEAD
    c = 0
    for i in range(n):
        if p + rec > len(b):
            break
        slen, = struct.unpack("<H", b[p + rec - 2:p + rec])
        p += rec + slen
        c += 1
    return c


def cat(d):
    uMax = struct.unpack("<I", d[:4])[0]
    if uMax == 0 or uMax > 64 or len(d) < 4 + 8 * uMax:
        return None, 0
    return [struct.unpack("<II", d[4 + i * 8: 12 + i * 8]) for i in range(uMax)], 4 + 8 * uMax


def main():
    kho = Kho(DUAN)
    d = kho.doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    duong = {}
    for l in d.decode("gbk", "replace").replace("\r\n", "\n").split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(.+?)\s*$", l)
        if m and (BS in m.group(2)) and not m.group(2).lower().endswith((".ini", ".txt")):
            mp = m.group(2)
            if not mp.lower().startswith(BS + "maps"):
                mp = BS + "maps" + (mp if mp.startswith(BS) else BS + mp)
            duong[int(m.group(1))] = mp
    ra = []
    tong_npc = tong_obj = 0
    max_o = (0, 0, 0)
    for idx in sorted(duong):
        mp = duong[idx]
        w = kho.doc(mp.encode("gbk", "replace") + b".wor")
        if not w:
            continue
        m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
        if not m:
            continue
        x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
        if (x1 - x0) > 60 or (y1 - y0) > 60:
            continue
        o = {}
        n_npc = n_obj = 0
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                dd = kho.doc(("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace"))
                if not dd:
                    continue
                secs, head = cat(dd)
                if not secs or len(secs) < 6:
                    continue
                off, ln = secs[2]
                a = dem(dd[head + off: head + off + ln], NPC_REC) if ln else 0
                off, ln = secs[3]
                b2 = dem(dd[head + off: head + off + ln], OBJ_REC) if ln else 0
                o[(x, y)] = (a, b2)
                n_npc += a
                n_obj += b2
        # cua so 3x3 nang nhat (client giu 9 vung mot luc)
        for (x, y) in o:
            s9 = sum(o.get((x + dx, y + dy), (0, 0))[0] for dx in (-1, 0, 1) for dy in (-1, 0, 1))
            if s9 > max_o[0]:
                max_o = (s9, idx, (x, y))
        if n_npc or n_obj:
            ra.append((idx, mp, n_npc, n_obj))
            tong_npc += n_npc
            tong_obj += n_obj
    ra.sort(key=lambda r: -r[2])
    print("%-5s %-44s %-7s %s" % ("id", "map", "npc", "obj"))
    for idx, mp, a, b2 in ra[:25]:
        print("%-5d %-44s %-7d %d" % (idx, mp[:44], a, b2))
    print("\n== %d map co NPC/OBJ trang tri; tong NPC %d, tong OBJ %d ==" % (len(ra), tong_npc, tong_obj))
    print("== cua so 3x3 nang nhat: %d NPC (map %d, vung %s) ==" % max_o)


if __name__ == "__main__":
    main()
