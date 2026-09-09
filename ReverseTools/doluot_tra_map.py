# -*- coding: utf-8 -*-
"""doluot_tra_map.py - tra ten ham cho log [DOLUOT] bang tep .map cua linker (khong can dbghelp/pdb).

  python doluot_tra_map.py <CoreClient.map> [<Represent3.map> ...] < jx_paint.log
      - dong [DOLUOT-EIP] pha X mau N: mod+rva:so ...  -> gom theo HAM (qua map), in top 20 moi dong
      - dong [DOLUOT] ... | x% mod+rva ...             -> tra ten tung muc (dinh dang cu)
  python doluot_tra_map.py <CoreClient.map> --rva 146D43 146EAB      (tra le)
  python doluot_tra_map.py <CoreClient.map> --gop < jx_paint.log      (GOP moi dong [DOLUOT-EIP] cung pha lai thanh 1 bang)

Map MSVC: 'Preferred load address is 10000000' + ' 0001:00012345  ?ten@@YAXXZ  10013345 f  tep.obj' => RVA = dia chi - base.
Ten ham = muc co RVA lon nhat <= RVA hoi. Module khong co map (ntdll, driver, vcruntime) giu 'mod+rva'.
"""
import io
import os
import re
import sys


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
        return None, rva
    r, ten = muc[lo - 1]
    return ten, rva - r


def rut_gon(ten):
    m = re.match(r"^\?(\w+)@(\w+)@@", ten)
    if m:
        return m.group(2) + "::" + m.group(1)
    m = re.match(r"^\?\?0(\w+)@@", ten)
    if m:
        return m.group(1) + "::ctor"
    m = re.match(r"^\?(\w+)@@", ten)
    if m:
        return m.group(1)
    return ten.lstrip("_")


maps = {}
args = list(sys.argv[1:])
rvas = []
gop = "--gop" in args
if gop:
    args.remove("--gop")
if "--rva" in args:
    i = args.index("--rva"); rvas = args[i + 1:]; args = args[:i]
for p in args:
    ten_dll = os.path.basename(p).replace(".map", ".dll").lower()
    ten_dll = re.sub(r"_[0-9a-f]{8}\.dll$", ".dll", ten_dll)     # CoreClient_ed9e6799.map -> coreclient.dll
    maps[ten_dll] = doc_map(p)
    print("map %s: %d muc" % (ten_dll, len(maps[ten_dll])), file=sys.stderr)
if rvas:
    mp = list(maps.values())[0]
    for r in rvas:
        t, d = tra(mp, int(r, 16)); print("%s -> %s+0x%x  (%s)" % (r, rut_gon(t) if t else "?", d, t))
    sys.exit(0)


def gom_dong(l, bang, tong):
    """gom mot dong [DOLUOT-EIP] vao bang {(mod, ham): so}; tra so mau cua dong"""
    m = re.search(r"mau (\d+):", l)
    n = int(m.group(1)) if m else 0
    for mod, rva, so in re.findall(r" (\S+?)\+([0-9A-Fa-f]+):(\d+)", l):
        so = int(so); mp = maps.get(mod.lower())
        if mp:
            t, d = tra(mp, int(rva, 16)); ham = rut_gon(t) if t else ("+%s" % rva)
        else:
            ham = "+" + rva
        bang[(mod, ham)] = bang.get((mod, ham), 0) + so
        tong[mod] = tong.get(mod, 0) + so
    return n


def in_bang(nhan, bang, tong, n, top=20):
    tt = sum(tong.values()) or 1
    print("%s: %d mau da gom (%.0f%% cua %d) | module: %s" % (nhan, tt, tt * 100.0 / max(n, 1), n,
          ", ".join("%s %.1f%%" % (k, v * 100.0 / tt) for k, v in sorted(tong.items(), key=lambda kv: -kv[1]))))
    for (mod, ham), so in sorted(bang.items(), key=lambda kv: -kv[1])[:top]:
        print("   %5.1f%%  %s!%s" % (so * 100.0 / tt, mod, ham))


rx_cu = re.compile(r"(\d+\.\d)% (\S+?)\+([0-9A-Fa-f]+)")
gop_bang = {}; gop_tong = {}; gop_n = {}
for l in sys.stdin:
    if "[DOLUOT-EIP]" in l:
        pha = "TICK" if "pha TICK" in l else "VE"
        if gop:
            b = gop_bang.setdefault(pha, {}); t = gop_tong.setdefault(pha, {})
            gop_n[pha] = gop_n.get(pha, 0) + gom_dong(l, b, t)
        else:
            b = {}; t = {}; n = gom_dong(l, b, t)
            in_bang("[DOLUOT] %s" % pha, b, t, n)
    elif "[DOLUOT]" in l and "%" in l and not gop:
        out = l[:l.find(":", l.find("pha")) + 1] if "pha" in l else l.strip()
        parts = []
        for m in rx_cu.finditer(l):
            pct, mod, rva = m.group(1), m.group(2), int(m.group(3), 16)
            mp = maps.get(mod.lower())
            if mp:
                t, d = tra(mp, rva); parts.append("%s%% %s+0x%x" % (pct, rut_gon(t) if t else "?", d))
            else:
                parts.append("%s%% %s+%X" % (pct, mod, rva))
        print(out + " " + " | ".join(parts))
    elif "[DOLUOT]" in l and not gop:
        print(l.rstrip())
if gop:
    for pha in ("TICK", "VE"):
        if pha in gop_bang:
            in_bang("[DOLUOT GOP] %s" % pha, gop_bang[pha], gop_tong[pha], gop_n[pha], top=30)
