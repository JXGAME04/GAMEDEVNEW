# -*- coding: utf-8 -*-
"""rut_anh_khoi.py - rut cac .spr hoat anh dang nghi la KHOI tu pak client ra dia de xem."""
import os
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, BS  # noqa: E402

RA = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\4001d2df-3b1a-4386-9730-72e4d6ac7cf7\scratchpad"
GOC = BS + "\u6e38\u620f\u8d44\u6e90" + BS + "\u5730\u56fe\u52a8\u753b" + BS + "\u7194\u5ca9" + BS
TEN = ["10160.spr", "\u77ed\u70df.spr"]

os.makedirs(RA, exist_ok=True)
k = Kho(DUAN)
for i, t in enumerate(TEN):
    d = k.doc((GOC + t).encode("gbk", "replace"))
    if not d:
        print("%s: KHONG CO" % t)
        continue
    p = os.path.join(RA, "khoi%d.spr" % i)
    open(p, "wb").write(d)
    com, w, h, cx, cy, frames, colors, dirs, itv = struct.unpack_from("<4sHHHHHHHH", d, 0)
    print("%-12s -> %s | sig=%s %dx%d tam=%d,%d khung=%d mau=%d huong=%d nhip=%d ms" % (
        t, p, com[:3].decode("ascii", "replace"), w, h, cx, cy, frames, colors, dirs, itv))
