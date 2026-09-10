# -*- coding: utf-8 -*-
"""so_canh.py - so du lieu CANH (scene) cua mot map giua client VLTK 2.0 va client du an (JX1).
Chi doc, khong sua gi.

Dung: python so_canh.py <ten_map_khong_dau>
  vd: python so_canh.py hoason
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

MAPS = {
    "hoason": BS + "maps" + BS + "西北南区" + BS + "华山",
    "hoasontuyetdinh": BS + "maps" + BS + "西北南区" + BS + "华山绝顶",
    "maccaoquat": BS + "maps" + BS + "西北北区" + BS + "莫高窟",
    "minhnguyettran": BS + "maps" + BS + "特殊用地" + BS + "明月镇",
    "phuongtuong": BS + "maps" + BS + "西北南区" + BS + "凤翔",
    "bienkinh": BS + "maps" + BS + "中原北区" + BS + "汴京",
}


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
    """tap cac pak cua mot client"""

    def __init__(self, thumuc, ten):
        self.ten = ten
        self.paks = {}
        if not os.path.isdir(thumuc):
            print("(khong co thu muc %s)" % thumuc)
            return
        for fn in sorted(os.listdir(thumuc)):
            if not fn.lower().endswith(".pak"):
                continue
            p = os.path.join(thumuc, fn)
            try:
                f = open(p, "rb")
                sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
                f.seek(ioff)
                raw = f.read(count * 16)
                d = {}
                for i in range(count):
                    uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
                    d[uid] = (off, size, cf)
                self.paks[fn] = (f, d)
            except Exception as e:
                print("(bo qua %s: %s)" % (fn, e))

    def tim(self, b):
        uid = name2id(b)
        for fn, (f, d) in self.paks.items():
            if uid in d:
                return (fn,) + d[uid]
        return None

    def doc(self, b):
        r = self.tim(b)
        if not r:
            return None
        fn, off, size, cf = r
        f = self.paks[fn][0]
        csize = cf & 0xFFFFFF
        f.seek(off)
        if csize == 0 or csize == size:
            return f.read(size)
        raw = f.read(csize)
        try:
            return bytes(ucl.nrv2b_decompress_8(raw, size))
        except Exception:
            try:
                import zlib
                return zlib.decompress(raw)
            except Exception:
                return None


def rect_cua(kho, mp):
    w = kho.doc(mp.encode("gbk", "replace") + b".wor")
    if not w:
        return None, None
    m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
    return (tuple(int(m.group(i)) for i in (1, 2, 3, 4)) if m else None), w


def kham(d, ten):
    """doc dau Region_C.dat: dinh dang tep"""
    if not d:
        return "khong co"
    head = d[:64]
    txt = "".join(chr(c) if 32 <= c < 127 else "." for c in head[:32])
    return "%d byte | dau: %s | hex %s" % (len(d), txt, " ".join("%02X" % c for c in head[:16]))


def main(key):
    mp = MAPS[key]
    print("=== MAP %s : %s ===" % (key, mp))
    k20 = Kho(V20, "2.0")
    kda = Kho(DUAN, "du an")
    print("pak 2.0: %d | pak du an: %d" % (len(k20.paks), len(kda.paks)))
    for kho in (k20, kda):
        r, w = rect_cua(kho, mp)
        print("-- %s: rect=%s, .wor %s" % (kho.ten, r, "co" if w else "KHONG"))
        if w:
            io.open(os.path.join(SP, "%s_%s.wor" % (key, kho.ten.replace(" ", ""))), "wb").write(w)
    r20, _ = rect_cua(k20, mp)
    rda, _ = rect_cua(kda, mp)
    r = r20 or rda
    if not r:
        print("khong co rect o ca hai ben -> dung")
        return
    x0, y0, x1, y1 = r
    # dem o vung + do lon tep, hai ben
    for kho in (k20, kda):
        n = 0
        tong = 0
        maus = []
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                b = ("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace")
                t = kho.tim(b)
                if t:
                    n += 1
                    tong += t[2]
                    if len(maus) < 2:
                        maus.append((y, x, kho.doc(b)))
        print("-- %s: %d o Region_C.dat, tong %.1f MB" % (kho.ten, n, tong / 1048576.0))
        for (y, x, d) in maus:
            print("     v_%03d/%03d: %s" % (y, x, kham(d, kho.ten)))
            io.open(os.path.join(SP, "%s_%s_v%03d_%03d_Region_C.dat" % (key, kho.ten.replace(" ", ""), y, x)), "wb").write(d or b"")


if __name__ == "__main__":
    os.makedirs(SP, exist_ok=True)
    main(sys.argv[1] if len(sys.argv) > 1 else "hoason")
