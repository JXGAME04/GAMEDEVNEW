# -*- coding: utf-8 -*-
"""Tim MOI anh SPR co kich thuoc lon (>=600x400) trong cac pak cua mot client.
Chi doc 32 byte dau cua moi muc (giai nen som) => nhanh.
Dung: python bigspr.py <thu muc data> <thu muc ket qua> [--dump]
"""
import os, sys, struct
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk\vltk2")
import nrv

ROOT = sys.argv[1]
OUT = sys.argv[2]
DUMP = "--dump" in sys.argv
os.makedirs(OUT, exist_ok=True)

MINW, MINH = 600, 400
found = []
for fn in sorted(os.listdir(ROOT)):
    if not fn.lower().endswith(".pak"):
        continue
    p = os.path.join(ROOT, fn)
    f = open(p, "rb")
    hdr = f.read(16)
    if len(hdr) < 16:
        f.close(); continue
    sig, count, ioff, doff = struct.unpack("<IIII", hdr)
    f.seek(ioff)
    raw = f.read(count * 16)
    n = 0
    for i in range(count):
        uid, off, size, cf = struct.unpack_from("<IIiI", raw, i * 16)
        if size < 4096:
            continue
        csize = cf & 0xFFFFFF
        flag = cf >> 24
        try:
            f.seek(off)
            if flag == 0 or csize in (0, size):
                head = f.read(32)
            else:
                blob = f.read(min(csize, 4096))
                head = nrv.nrv2b_8(blob, limit=32)
        except Exception:
            continue
        if head[:3] != b"SPR":
            continue
        try:
            com, w, h, cx, cy, frames, colors, dirs, itv = struct.unpack_from("<4sHHHHHHHH", head, 0)
        except Exception:
            continue
        if w >= MINW and h >= MINH:
            found.append((fn, uid, size, w, h, frames, colors))
            n += 1
            if DUMP:
                f.seek(off)
                blob = f.read(csize if csize else size)
                if flag == 0 or csize in (0, size):
                    data = blob[:size]
                else:
                    try:
                        data = nrv.nrv2b_8(blob, size)
                    except Exception:
                        continue
                open(os.path.join(OUT, "%s_%08X_%dx%d.spr" % (fn.replace(".pak", ""), uid, w, h)), "wb").write(data)
    f.close()
    if n:
        print("  %-18s %d anh lon" % (fn, n))

print("---- tong:", len(found))
for r in sorted(found, key=lambda x: -(x[3] * x[4])):
    print("   %-16s uid=%08X %8d B  %dx%d frames=%d colors=%d" % r)
