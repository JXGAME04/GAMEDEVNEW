#!/usr/bin/env python
# -*- coding: ascii -*-
"""bw_cellcheck.py - kiem tra o (x,y) cua mot map co region trong pak khong + gia tri vat can.
Region: nx = x*32 // 512, ny = y*32 // 1024 (REGION_GRID_WIDTH 16 o, HEIGHT 32 o).
Muc vat can (REGION_OBSTACLE_FILE_INDEX 0) = 16*32 dword, chi so = (cy*16 + cx).
Dung: python bw_cellcheck.py <pakdir> <duong-dan-map-utf8> x,y [x,y ...]
"""
import os, sys, struct
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
import gen_datau_spots as G

pakdir = sys.argv[1]
duongdan = sys.argv[2].encode("gbk")
paks = []
for f in sorted(os.listdir(pakdir)):
    if f.lower().endswith((".pak", ".mps")):
        try:
            paks.append(G.Pak(os.path.join(pakdir, f)))
        except Exception:
            pass
for cell in sys.argv[3:]:
    x, y = [int(v) for v in cell.split(",")]
    mx, my = x * 32, y * 32
    nx, ny = mx // 512, my // 1024
    cx, cy = (mx % 512) // 32, (my % 1024) // 32
    k = b"\\maps\\" + duongdan + b"\\v_%03d\\%03d_region_s.dat" % (ny, nx)
    uid = G.name_to_id(k)
    found = None
    for p in paks:
        if p.info(uid):
            found = p; break
    if not found:
        print("o (%d,%d) region(%d,%d): KHONG CO region trong pak" % (x, y, nx, ny)); continue
    blob = found.doc(uid)
    num_sect, = struct.unpack_from("<I", blob, 0)
    head = 4 + 8 * num_sect
    off, length = struct.unpack_from("<II", blob, 4)
    val = struct.unpack_from("<I", blob, head + off + (cy * 16 + cx) * 4)[0] if length >= 2048 else None
    print("o (%d,%d) region(%d,%d) cell(%d,%d) pak=%s size=%d vatcan=%s" % (x, y, nx, ny, cx, cy, found.name, len(blob), hex(val) if val is not None else "?"))
