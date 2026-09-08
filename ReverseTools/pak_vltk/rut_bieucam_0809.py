# -*- coding: utf-8 -*-
"""rut_bieucam_0809.py - [BIEUCAM 08/09] ChatPics.ini khai bao 911 bieu cam (\\spr\\Ui3\\<GBK bieu cam>\\N.spr) nhung pak game chi co 0..139
-> LoadImage FAIL 140..910. Tim cac tep thieu trong pak cua game, client tham khao JX1 (Level Up) va client 2.0, rut ra thu muc
bin\\client\\spr\\Ui3\\<GBK> (tep roi tren dia duoc engine uu tien truoc pak).
Chay: python rut_bieucam_0809.py [--rut]   (khong co --rut = chi liet ke)"""
import os, sys, io
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pakdump as P
import ucl
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
GBK = b"\xb1\xed\xc7\xe9"                      # "bieu cam" (GBK) - dung byte y het ChatPics.ini
DIR_LAT = GBK.decode("latin-1")               # de bam name2id (ord = byte)
GAME = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
PAKDIRS = [
    ("game", GAME + r"\data"),
    ("jx1-levelup", r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"),
    ("vltk2.0", r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data"),
]
N0, N1 = 0, 911
want = {}
for n in range(N0, N1):
    for nm in (BS + "spr" + BS + "Ui3" + BS + DIR_LAT + BS + ("%d.spr" % n), BS + "spr" + BS + "Ui3" + BS + DIR_LAT + BS + ("%02d.spr" % n)):
        want[P.name2id(nm)] = (n, nm)

def read_raw(f, e):
    uid, off, size, cf = e
    csize = cf & 0xFFFFFF
    flag = cf >> 24
    f.seek(off)
    blob = f.read(csize if csize else size)
    if flag in (0x01, 0x20) and csize and csize != size:
        return ucl.nrv2b_decompress_8(blob, size)
    return blob

found = {}   # n -> (src, data)
for tag, d in PAKDIRS:
    if not os.path.isdir(d):
        print("bo qua (khong co):", tag, d); continue
    for fn in sorted(os.listdir(d)):
        if not fn.lower().endswith((".pak", ".mps")): continue
        try:
            f, es = P.entries(os.path.join(d, fn))
        except Exception as ex:
            print("  loi doc", fn, ex); continue
        hit = 0
        for e in es:
            if e[0] in want:
                n, nm = want[e[0]]
                if n in found: continue
                try:
                    data = read_raw(f, e)
                except Exception as ex:
                    print("  loi giai nen", fn, n, ex); continue
                if len(data) >= 32 and data[:4] in (b"SPR\x00", b"SPR ", b"SPRX", b"SPRA", b"SPRC") or len(data) >= 32:
                    found[n] = (tag + "/" + fn, data); hit += 1
        if hit: print("%-12s %-24s: %d bieu cam" % (tag, fn, hit))
have = sorted(found)
print("tong thay %d / %d; thieu %d" % (len(have), N1 - N0, N1 - N0 - len(have)))
missing = [n for n in range(N0, N1) if n not in found]
print("thieu (dau/cuoi):", missing[:10], "...", missing[-5:] if missing else "")
by_src = {}
for n, (src, data) in found.items(): by_src[src] = by_src.get(src, 0) + 1
for s in sorted(by_src): print("  nguon %-40s %d" % (s, by_src[s]))
if "--rut" in sys.argv:
    # Game mo tep bang API ANSI voi byte GBK -> ten thu muc tren dia phai la cac byte do giai theo code page ANSI cua may
    import ctypes
    acp = ctypes.windll.kernel32.GetACP()
    dirname = GBK.decode("cp%d" % acp, "replace")
    outdir = GAME + BS + "spr" + BS + "Ui3" + BS + dirname
    if not os.path.isdir(outdir): os.makedirs(outdir)
    nWrote = 0
    for n in have:
        if n < 140: continue          # 0..139 da co trong pak game (da mount)
        src, data = found[n]
        p = outdir + BS + ("%d.spr" % n)
        if os.path.exists(p): continue
        open(p, "wb").write(data); nWrote += 1
    print("ACP=%d, thu muc %r, da ghi %d tep" % (acp, outdir, nWrote))
