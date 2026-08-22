# -*- coding: utf-8 -*-
# Doc .wor cua map 975 (\maps\特殊用地\jingjichang) trong pak du an + pak Linux,
# in rect va kiem tra toa do readypos/battlepos/NPC cua rule.lua nam trong rect.
import os, sys, struct
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\port_tongcastle")
import extract_984 as E

MAPDIR_B = b"\xcc\xd8\xca\xe2\xd3\xc3\xb5\xd8" + b"\\jingjichang"   # 特殊用地\jingjichang (GBK)
BASE_B = b"\\maps\\" + MAPDIR_B
PAKS = [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\pak\jingjichang.mps",
        r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\pak\maps.pak",
        r"D:\ServerLinux\server1\pak\maps.pak"]

def read_tab(p):
    rows = [ln.split() for ln in open(p, "rb").read().decode("latin-1").splitlines()[1:] if ln.strip()]
    return [(int(a), int(b)) for a, b in rows]

for pak in PAKS:
    if not os.path.isfile(pak):
        print("KHONG CO", pak); continue
    f, tbl, order, meta = E.load_index(pak)
    uid = E.name2id(BASE_B + b".wor")
    print("== %s : %d muc, .wor %s" % (pak, len(tbl), "CO" if uid in tbl else "KHONG"))
    if uid not in tbl:
        continue
    wor = E.read_elem(f, *tbl[uid])
    rect = E.parse_rect(wor)
    print("   rect (region tuyet doi) =", rect)
    # region 512x1024 px (KSubWorld.cpp: m_nRegionWidth=512/32, Height=1024/32) -> toa do pixel
    if rect:
        l, t, r, b = rect
        px = (l * 512, t * 1024, r * 512, b * 1024)
        print("   pixel rect = x[%d..%d] y[%d..%d]  => o (x/32) [%d..%d] (y/32) [%d..%d]" % (px[0], px[2], px[1], px[3], px[0]//32, px[2]//32, px[1]//32, px[3]//32))
        # dem region S co trong pak
        nS = 0
        for y in range(t, b + 1):
            for x in range(l, r + 1):
                k = BASE_B + b"\\v_%03d\\%03d_Region_S.dat" % (y, x)
                if E.name2id(k) in tbl: nS += 1
        print("   so tep _Region_S.dat trong cua so rect:", nS)
        def inside(X, Y):
            return px[0] <= X <= px[2] and px[1] <= Y <= px[3]
        for name, p in (("readypos", r"D:\ServerLinux\server1\settings\missions\arena\readypos.txt"),
                        ("battlepos", r"D:\ServerLinux\server1\settings\missions\arena\battlepos.txt")):
            pts = read_tab(p)
            bad = [pt for pt in pts if not inside(*pt)]
            print("   %s: %d diem, ngoai rect: %d" % (name, len(pts), len(bad)))
        for nm, X, Y in (("Ruong chua do", 50112, 102848), ("Chu duoc diem", 50496, 102528)):
            print("   NPC %-14s (%d,%d) o=(%d,%d) trong rect: %s" % (nm, X, Y, X//32, Y//32, inside(X, Y)))
    # in vai dong dau cua .wor
    print("   .wor dau:", wor[:200].decode("latin-1").replace("\r", "").split("\n")[:8])
    f.close()
