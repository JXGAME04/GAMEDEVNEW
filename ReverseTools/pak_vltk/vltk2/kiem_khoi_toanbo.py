# -*- coding: utf-8 -*-
"""kiem_khoi_toanbo.py - quet TOAN BO map cua client du an, liet ke cac vat canh co hoat anh thuoc nhom KHOI
(ten anh chua chu 'yen' hoac nam trong thu muc 熔岩/地图动画) kem toa do o (canh/256, canh/512).
Chi doc.
"""
import io
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, BS, BIO  # noqa: E402

YEN = b"\xd1\xcc"          # 'yen' = khoi
DONG = "地图动画".encode("gbk")


def main():
    kho = Kho(DUAN)
    d = kho.doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    duong = {}
    for l in d.decode("gbk", "replace").replace("\r\n", "\n").split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(.+?)\s*$", l)
        if m and ("\\" in m.group(2)) and not m.group(2).lower().endswith((".ini", ".txt")):
            mp = m.group(2)
            if not mp.lower().startswith(BS + "maps"):
                mp = BS + "maps" + (mp if mp.startswith(BS) else BS + mp)
            duong.setdefault(mp, int(m.group(1)))
    print("quet %d duong dan map" % len(duong))
    tong_map = 0
    ra = []
    for mp, idx in sorted(duong.items(), key=lambda kv: kv[1]):
        w = kho.doc(mp.encode("gbk", "replace") + b".wor")
        if not w:
            continue
        m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
        if not m:
            continue
        x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
        if (x1 - x0) > 60 or (y1 - y0) > 60:
            continue
        tong_map += 1
        khoi = []
        nAni = 0
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                dd = kho.doc(("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace"))
                if not dd or len(dd) < 8:
                    continue
                uMax = struct.unpack("<I", dd[:4])[0]
                if uMax == 0 or uMax > 64 or len(dd) < 4 + 8 * uMax:
                    continue
                secs = [struct.unpack("<II", dd[4 + i * 8: 12 + i * 8]) for i in range(uMax)]
                if len(secs) < 6:
                    continue
                off, ln = secs[5]
                if not ln:
                    continue
                head = 4 + 8 * uMax
                b = dd[head + off: head + off + ln]
                if len(b) < 16:
                    continue
                nNum = struct.unpack("<I", b[:4])[0]
                if nNum > 4000:
                    continue
                p = 16
                for i in range(nNum):
                    if p + BIO > len(b):
                        break
                    rec = b[p:p + BIO]
                    p += BIO
                    ani = struct.unpack("<H", rec[192:194])[0]
                    if not ani:
                        continue
                    nAni += 1
                    ten = rec[56:184].split(b"\x00")[0]
                    if YEN in ten:
                        o1 = struct.unpack("<iii", rec[196:208])
                        khoi.append((ten.decode("gbk", "replace"), o1[0] // 256, o1[1] // 512))
        if khoi or nAni:
            ra.append((idx, mp, nAni, khoi))
    print("%-5s %-42s %-7s %s" % ("id", "map", "vat dong", "khoi (ten, o)"))
    for idx, mp, nAni, khoi in ra:
        if khoi:
            print("%-5d %-42s %-7d %s" % (idx, mp[:42], nAni, "; ".join("%s (%d,%d)" % (t.split(BS)[-1], x, y) for t, x, y in khoi[:6])))
    print("== %d map co vat canh dong; %d map co KHOI ==" % (len([r for r in ra if r[2]]), len([r for r in ra if r[3]])))


if __name__ == "__main__":
    main()
