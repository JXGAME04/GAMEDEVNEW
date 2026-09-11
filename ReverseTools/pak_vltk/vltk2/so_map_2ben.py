# -*- coding: utf-8 -*-
"""so_map_2ben.py - so TOAN BO du lieu canh cua tung map giua client 2.0 va client du an:
so o vung Region_C.dat va tong byte. Map nao 2.0 nhieu hon = canh cua ho co them thu (khoi, dong vat...).
Chi doc.
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW_wt_delta\ReverseTools\pak_vltk")
import ucl  # noqa: E402

BS = chr(92)
V20 = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data"
DUAN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\data"


def name2id(b):
    uid = 0
    idx = 0
    for c in b:
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


class Kho(object):
    def __init__(self, thumuc):
        self.paks = {}
        self.chiso = {}
        if not os.path.isdir(thumuc):
            return
        for fn in sorted(os.listdir(thumuc)):
            if not fn.lower().endswith(".pak"):
                continue
            try:
                f = open(os.path.join(thumuc, fn), "rb")
                sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
                f.seek(ioff)
                raw = f.read(count * 16)
                for i in range(count):
                    uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
                    if uid not in self.chiso:
                        self.chiso[uid] = (fn, off, size, cf)
                self.paks[fn] = f
            except Exception:
                pass

    def co(self, b):
        return self.chiso.get(name2id(b))

    def doc(self, b):
        r = self.co(b)
        if not r:
            return None
        fn, off, size, cf = r
        f = self.paks[fn]
        cs = cf & 0xFFFFFF
        f.seek(off)
        if cs == 0 or cs == size:
            return f.read(size)
        raw = f.read(cs)
        try:
            return bytes(ucl.nrv2b_decompress_8(raw, size))
        except Exception:
            return None


def maplist(kho):
    d = kho.doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    if not d:
        return {}
    out = {}
    for l in d.decode("gbk", "replace").replace("\r\n", "\n").split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(" + re.escape(BS) + r"maps" + re.escape(BS) + r".+?)\s*$", l)
        if m:
            out.setdefault(m.group(2), int(m.group(1)))
    return out


def main():
    k20 = Kho(V20)
    kda = Kho(DUAN)
    print("2.0: %d pak %d muc | du an: %d pak %d muc" % (len(k20.paks), len(k20.chiso), len(kda.paks), len(kda.chiso)))
    ml = maplist(k20)
    print("map trong MapList 2.0: %d" % len(ml))
    ds = []
    for mp, idx in sorted(ml.items(), key=lambda kv: kv[1]):
        b = mp.encode("gbk", "replace")
        w20 = k20.doc(b + b".wor")
        wda = kda.doc(b + b".wor")
        if not w20 or not wda:
            continue
        m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w20.decode("gbk", "replace"))
        if not m:
            continue
        x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
        if (x1 - x0) > 60 or (y1 - y0) > 60:
            continue
        n20 = n_da = 0
        s20 = s_da = 0
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                nb = ("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace")
                r20 = k20.co(nb)
                rda = kda.co(nb)
                if r20:
                    n20 += 1; s20 += r20[2]
                if rda:
                    n_da += 1; s_da += rda[2]
        if n20 or n_da:
            ds.append((mp, idx, n20, s20, n_da, s_da))
    print("%-46s %-6s | %-7s %-10s | %-7s %-10s | chenh" % ("map", "id", "o 2.0", "byte 2.0", "o du an", "byte du an"))
    kh = 0
    for mp, idx, n20, s20, n_da, s_da in ds:
        if s20 != s_da:
            kh += 1
            print("%-46s %-6d | %-7d %-10d | %-7d %-10d | %+d byte" % (mp[:46], idx, n20, s20, n_da, s_da, s20 - s_da))
    print("== %d/%d map co du lieu canh KHAC nhau ==" % (kh, len(ds)))


if __name__ == "__main__":
    main()
