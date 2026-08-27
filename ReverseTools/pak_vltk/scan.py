# -*- coding: utf-8 -*-
"""Quet moi entry trong pak, giai nen UCL, gom duong dan Include/ScriptFile
va tim chuoi khoa. Dung de KHAM PHA TEN TEP trong pak (pak chi luu id, khong luu ten)."""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pakdump as P
import ucl

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"

RX = re.compile(rb'[\\/][A-Za-z0-9_\\/\.\-]{3,120}\.(?:lua|ini|txt|spr)', re.I)


def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    f.seek(off)
    raw = f.read(cs)
    if fl == 0:
        return raw
    try:
        return ucl.nrv2b_decompress_8(raw, size)
    except Exception:
        return raw


def main():
    paks = sys.argv[1].split(",")
    kws = [k.encode("latin-1") for k in sys.argv[2:]]
    paths = {}
    for pk in paks:
        p = os.path.join(ROOT, pk)
        if not os.path.exists(p):
            print("khong co", pk)
            continue
        try:
            f, es = P.entries(p)
        except Exception as ex:
            print("loi", pk, ex)
            continue
        nhit = 0
        for e in es:
            d = blob_of(f, e)
            for m in RX.finditer(d):
                s = m.group(0).replace(b"/", b"\\")
                paths.setdefault(s, set()).add(pk)
            if kws and any(k.lower() in d.lower() for k in kws):
                nhit += 1
                print("KW  %-22s uid=%08X size=%d" % (pk, e[0], e[2]))
        print("== %s: %d entries, %d kw-hit" % (pk, len(es), nhit))
    # in cac duong dan chua tu khoa
    for s in sorted(paths):
        low = s.lower()
        if any(k.lower() in low for k in kws):
            print("PATH", s.decode("latin-1"), sorted(paths[s]))


main()
