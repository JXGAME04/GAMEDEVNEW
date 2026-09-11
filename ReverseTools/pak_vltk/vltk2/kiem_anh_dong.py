# -*- coding: utf-8 -*-
"""kiem_anh_dong.py - kiem cac ANH HOAT ANH cua vat canh (\\游戏资源\\地图动画\\...) co trong pak hai ben khong.
Quet nhieu map, gom ten anh cua vat canh CO HOAT ANH, roi tra tung ten trong pak 2.0 va pak du an.
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
import ucl  # noqa: E402
from doc_vatcanh import Kho, doc_vung, MAPS, V20, DUAN, BS  # noqa: E402


def main():
    k20 = Kho(V20)
    kda = Kho(DUAN)
    ten_anh = {}
    for key in ["phuongtuong", "hoason", "maccaoquat", "thanhdo", "laman", "daly", "tuongduong"]:
        mp = MAPS[key]
        w = kda.doc(mp.encode("gbk", "replace") + b".wor") or k20.doc(mp.encode("gbk", "replace") + b".wor")
        if not w:
            print("  (bo qua %s: khong co .wor)" % key); continue
        m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
        if not m:
            continue
        x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
        dem = 0
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                nb = ("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace")
                d = kda.doc(nb)
                if not d:
                    continue
                for ten, a, nf in doc_vung(d):
                    if a and ten:
                        ten_anh.setdefault(ten, [0, key])
                        ten_anh[ten][0] += 1
                        dem += 1
        print("  %s: %d vat co hoat anh" % (key, dem))
    print("== %d ten anh hoat anh khac nhau ==" % len(ten_anh))
    thieu20 = thieuda = 0
    for ten, (c, key) in sorted(ten_anh.items(), key=lambda kv: -kv[1][0]):
        r20 = k20.chiso.get(__import__("doc_vatcanh").name2id(ten))
        rda = kda.chiso.get(__import__("doc_vatcanh").name2id(ten))
        if not r20:
            thieu20 += 1
        if not rda:
            thieuda += 1
        if not rda or not r20:
            print("  %4d x %-52s | 2.0 %-12s | du an %-12s" % (
                c, ten.decode("gbk", "replace")[:52],
                ("%s %dB" % (r20[0], r20[2])) if r20 else "KHONG CO",
                ("%s %dB" % (rda[0], rda[2])) if rda else "KHONG CO"))
    print("== thieu o 2.0: %d | thieu o du an: %d (tren tong %d) ==" % (thieu20, thieuda, len(ten_anh)))


if __name__ == "__main__":
    main()
