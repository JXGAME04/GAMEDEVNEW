# -*- coding: utf-8 -*-
"""scroll_v20.py - rut cac tep \\Maps\\ScrollSetting\\<id>_Scroll.ini cua client VLTK 2.0
(lop anh nen cuon - parallax) va doi chieu voi client du an. Chi doc.

Dung: python scroll_v20.py [id_toi_da]
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import ucl  # noqa: E402

BS = chr(92)
V20 = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data"
DUAN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\data"
SP = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\4001d2df-3b1a-4386-9730-72e4d6ac7cf7\scratchpad\v20canh"


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
    def __init__(self, thumuc, ten):
        self.ten = ten
        self.paks = {}
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
                d = {}
                for i in range(count):
                    uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
                    d[uid] = (off, size, cf)
                self.paks[fn] = (f, d)
            except Exception:
                pass

    def doc(self, b):
        uid = name2id(b)
        for fn, (f, d) in self.paks.items():
            if uid in d:
                off, size, cf = d[uid]
                csize = cf & 0xFFFFFF
                f.seek(off)
                if csize == 0 or csize == size:
                    return f.read(size), fn
                raw = f.read(csize)
                try:
                    return bytes(ucl.nrv2b_decompress_8(raw, size)), fn
                except Exception:
                    return None, fn
        return None, None


def maplist(kho):
    d, _ = kho.doc((BS + "settings" + BS + "maplist.ini").encode("ascii"))
    if not d:
        return {}
    s = d.decode("gbk", "replace").replace("\r\n", "\n")
    ten = {}
    for l in s.split("\n"):
        m = re.match(r"^\s*(\d+)\s*=\s*(" + re.escape(BS) + r"maps" + re.escape(BS) + r".+?)\s*$", l)
        if m:
            ten[int(m.group(1))] = m.group(2)
    return ten


def main(maxid=1100):
    os.makedirs(SP, exist_ok=True)
    k20 = Kho(V20, "2.0")
    kda = Kho(DUAN, "du an")
    ten20 = maplist(k20)
    tenda = maplist(kda)
    print("MapList: 2.0 %d muc | du an %d muc" % (len(ten20), len(tenda)))
    co20, coda = [], []
    for i in range(0, maxid + 1):
        b = ("%sMaps%sScrollSetting%s%d_Scroll.ini" % (BS, BS, BS, i)).encode("ascii")
        d20, p20 = k20.doc(b)
        dda, pda = kda.doc(b)
        if d20:
            co20.append((i, len(d20), p20))
        if dda:
            coda.append((i, len(dda), pda))
        if d20:
            io.open(os.path.join(SP, "scroll_v20_%d.ini" % i), "wb").write(d20)
        if dda:
            io.open(os.path.join(SP, "scroll_duan_%d.ini" % i), "wb").write(dda)
    print("2.0   : %d tep _Scroll.ini -> %s" % (len(co20), [x[0] for x in co20]))
    print("du an : %d tep _Scroll.ini -> %s" % (len(coda), [x[0] for x in coda]))
    for i, n, p in co20:
        m = ten20.get(i, "?")
        print("  2.0 id %-5d %-30s %6d byte (%s)" % (i, m, n, p))


if __name__ == "__main__":
    main(int(sys.argv[1]) if len(sys.argv) > 1 else 1100)
