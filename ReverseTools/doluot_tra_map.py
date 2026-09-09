# -*- coding: utf-8 -*-
"""doluot_tra_map.py - tra ten ham cho dong [DOLUOT] bang tep .map cua linker (khong can dbghelp/pdb).

  python doluot_tra_map.py <CoreClient.map> [<Represent3.map> ...] < jx_paint.log   (loc dong [DOLUOT])
  python doluot_tra_map.py <CoreClient.map> --rva 146D43 146EAB                    (tra le)

Map MSVC: 'Preferred load address is 10000000' + bang ' 0001:00012345  ?ten@@YAXXZ  10013345 f  tep.obj'
=> RVA = dia_chi - base. Ten ham = muc co RVA lon nhat <= RVA hoi (cung section).
"""
import io
import re
import sys
import os


def doc_map(p):
    base = 0x10000000
    muc = []
    rx_base = re.compile(r"Preferred load address is ([0-9A-Fa-f]+)")
    rx = re.compile(r"^\s*([0-9A-Fa-f]{4}):([0-9A-Fa-f]{8})\s+(\S+)\s+([0-9A-Fa-f]{8})\s+(f|F)?")
    trong_bang = False
    for l in io.open(p, encoding="latin-1", errors="replace"):
        m = rx_base.search(l)
        if m:
            base = int(m.group(1), 16); continue
        if "Address" in l and "Publics by Value" in l:
            trong_bang = True; continue
        if not trong_bang:
            continue
        m = rx.match(l)
        if m:
            addr = int(m.group(4), 16)
            if addr >= base:
                muc.append((addr - base, m.group(3)))
    muc.sort()
    return muc


def tra(muc, rva):
    lo, hi = 0, len(muc)
    while lo < hi:
        mid = (lo + hi) // 2
        if muc[mid][0] <= rva:
            lo = mid + 1
        else:
            hi = mid
    if lo == 0:
        return "?", rva
    r, ten = muc[lo - 1]
    return ten, rva - r


def rut_gon(ten):
    # bo trang tri kieu ?FindNpc@KRegion@@QAEHHHHHH@Z -> KRegion::FindNpc
    m = re.match(r"^\?(\w+)@(\w+)@@", ten)
    if m:
        return m.group(2) + "::" + m.group(1)
    m = re.match(r"^\?(\w+)@@", ten)
    if m:
        return m.group(1)
    return ten.lstrip("_")


maps = {}
args = [a for a in sys.argv[1:]]
rvas = []
if "--rva" in args:
    i = args.index("--rva"); rvas = args[i + 1:]; args = args[:i]
for p in args:
    ten_dll = os.path.basename(p).replace(".map", ".dll")
    maps[ten_dll.lower()] = doc_map(p)
    print("map %s: %d muc" % (ten_dll, len(maps[ten_dll.lower()])), file=sys.stderr)
if rvas:
    mp = list(maps.values())[0]
    for r in rvas:
        t, d = tra(mp, int(r, 16)); print("%s -> %s+0x%x  (%s)" % (r, rut_gon(t), d, t))
    sys.exit(0)
rx_muc = re.compile(r"(\d+\.\d)% (\S+?)\+([0-9A-Fa-f]+) \S+\+\d+")
for l in sys.stdin:
    if "[DOLUOT]" not in l:
        continue
    dau = l.split(":", 2)
    out = l[:l.find(":", l.find("pha")) + 1] if "pha" in l else l.strip()
    parts = []
    for m in rx_muc.finditer(l):
        pct, mod, rva = m.group(1), m.group(2), int(m.group(3), 16)
        mp = maps.get(mod.lower())
        if mp:
            t, d = tra(mp, rva); parts.append("%s%% %s+0x%x" % (pct, rut_gon(t), d))
        else:
            parts.append("%s%% %s+%X" % (pct, mod, rva))
    print(out + " " + " | ".join(parts))
