# -*- coding: utf-8 -*-
"""mo_khoi_map53.py - in TOAN BO ban ghi KBuildinObj cua vat canh khoi o map 53 (10160.spr)
va so sanh voi vat khoi DANG CHAY o Phuong Tuong (短烟.spr) de tim cho khac.
Chi doc.
"""
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk\vltk2")
from doc_vatcanh import Kho, DUAN, BS, BIO  # noqa: E402

SORT = {0x0000: "POINT", 0x0100: "LINE", 0x0200: "TREE", 0x0300: "?"}


def duong_map(kho, idx):
    d = kho.doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    for l in d.decode("gbk", "replace").replace("\r\n", "\n").split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(.+?)\s*$", l)
        if m and int(m.group(1)) == idx:
            mp = m.group(2)
            if not mp.lower().startswith(BS + "maps"):
                mp = BS + "maps" + (mp if mp.startswith(BS) else BS + mp)
            return mp
    return None


def quet(kho, idx, loc):
    mp = duong_map(kho, idx)
    w = kho.doc(mp.encode("gbk", "replace") + b".wor")
    m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
    x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
    ra = []
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            d = kho.doc(("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace"))
            if not d:
                continue
            uMax = struct.unpack("<I", d[:4])[0]
            if uMax == 0 or uMax > 64:
                continue
            secs = [struct.unpack("<II", d[4 + i * 8: 12 + i * 8]) for i in range(uMax)]
            if len(secs) < 6:
                continue
            head = 4 + 8 * uMax
            off, ln = secs[5]
            if not ln:
                continue
            b = d[head + off: head + off + ln]
            hd = struct.unpack("<IHHHHHH", b[:16])
            n = hd[0]
            p = 16
            for i in range(n):
                if p + BIO > len(b):
                    break
                r = b[p:p + BIO]
                p += BIO
                ten = r[56:184].split(b"\x00")[0]
                if loc not in ten:
                    continue
                props, = struct.unpack("<I", r[0:4])
                ip = [struct.unpack("<iii", r[4 + k * 12: 16 + k * 12]) for k in range(4)]
                wdt, hgt = struct.unpack("<hh", r[52:56])
                flip, = struct.unpack("<I", r[184:188])
                nfr, nnum, nani, nord = struct.unpack("<HHHH", r[188:196])
                o1 = struct.unpack("<iii", r[196:208])
                o2 = struct.unpack("<iii", r[208:220])
                ra.append(dict(vung=(x, y), n=n, dau=hd[1:], ten=ten.decode("gbk", "replace"),
                               props=props, sort=SORT.get(props & 0x0300, "?"),
                               ip=ip, w=wdt, h=hgt, flip=flip, nfr=nfr, nnum=nnum,
                               nani=nani, nord=nord, o1=o1, o2=o2, idx=i))
    return ra


def inra(nhan, ds):
    print("\n===== %s: %d vat =====" % (nhan, len(ds)))
    for v in ds:
        print(" vung %s  #%d/%d (tree %d line %d point %d above %d)" % (v["vung"], v["idx"], v["n"], v["dau"][0], v["dau"][1], v["dau"][2], v["dau"][3]))
        print("   ten=%s  Props=0x%08X sap_xep=%s" % (v["ten"], v["props"], v["sort"]))
        print("   ImgPos1=%s ImgPos2=%s" % (v["ip"][0], v["ip"][1]))
        print("   ImgPos3=%s ImgPos4=%s" % (v["ip"][2], v["ip"][3]))
        print("   anh %dx%d | flip=%u nFrame=%d nImgNumFrames=%d nAniSpeed=%d nOrder=%d" % (
            v["w"], v["h"], v["flip"], v["nfr"], v["nnum"], v["nani"], v["nord"]))
        print("   oPos1=%s (o %d,%d)  oPos2=%s" % (v["o1"], v["o1"][0] // 256, v["o1"][1] // 512, v["o2"]))


if __name__ == "__main__":
    k = Kho(DUAN)
    inra("map 53 - 10160.spr (khoi chua hien)", quet(k, 53, b"10160"))
    inra("map 1 Phuong Tuong - \u77ed\u70df.spr (khoi DA HIEN)", quet(k, 1, "\u77ed\u70df".encode("gbk")))
