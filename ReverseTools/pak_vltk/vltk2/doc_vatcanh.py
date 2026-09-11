# -*- coding: utf-8 -*-
"""doc_vatcanh.py - doc phan VAT CANH (BuildinObj) trong Region_C.dat de xem:
  - vat nao CO HOAT ANH (nAniSpeed != 0) -> khoi bay, co bay, nuoc chay...
  - ten anh cua chung, dac biet tim chu 'yen' (khoi) trong ten.
Dung: python doc_vatcanh.py <ten_map_khong_dau> [2.0|duan]
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
MAPS = {
    "phuongtuong": BS + "maps" + BS + "西北南区" + BS + "凤翔",
    "bienkinh": BS + "maps" + BS + "中原北区" + BS + "汴京",
    "hoason": BS + "maps" + BS + "西北南区" + BS + "华山",
    "maccaoquat": BS + "maps" + BS + "西北北区" + BS + "莫高窟",
    "thanhdo": BS + "maps" + BS + "西南北区" + BS + "成都",
    "laman": BS + "maps" + BS + "江南区" + BS + "临安",
    "daly": BS + "maps" + BS + "西南南区" + BS + "大理",
    "tuongduong": BS + "maps" + BS + "两湖区" + BS + "襄阳",
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
    def __init__(self, thumuc):
        self.paks = {}
        self.chiso = {}
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
                    self.chiso.setdefault(uid, (fn, off, size, cf))
                self.paks[fn] = f
            except Exception:
                pass

    def doc(self, b):
        r = self.chiso.get(name2id(b))
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


BIO = 228          # kich thuoc KBuildinObj (xem SceneDataDef.h)
OFF_W = 4 + 48     # nImgWidth
OFF_IMG = OFF_W + 4
OFF_FLIP = OFF_IMG + 128
OFF_FRAME = OFF_FLIP + 4
OFF_NFRAMES = OFF_FRAME + 2
OFF_ANI = OFF_NFRAMES + 2


def doc_vung(d):
    """tra (danh sach vat canh) tu Region_C.dat"""
    if not d or len(d) < 8:
        return []
    uMax = struct.unpack("<I", d[:4])[0]
    if uMax == 0 or uMax > 64:
        return []
    n = min(uMax, 6)
    secs = []
    for i in range(uMax):
        o, l = struct.unpack("<II", d[4 + i * 8: 12 + i * 8])
        secs.append((o, l))
    head = 4 + 8 * uMax
    if len(secs) < 6:
        return []
    off, ln = secs[5]                      # REGION_BUILDIN_OBJ_FILE_INDEX = 5
    if ln == 0:
        return []
    b = d[head + off: head + off + ln]
    if len(b) < 16:
        return []
    nNum = struct.unpack("<I", b[:4])[0]
    if nNum == 0 or nNum > 4000:
        return []
    ra = []
    p = 16
    for i in range(nNum):
        if p + BIO > len(b):
            break
        rec = b[p:p + BIO]
        ten = rec[OFF_IMG:OFF_IMG + 128].split(b"\x00")[0]
        ani = struct.unpack("<H", rec[OFF_ANI:OFF_ANI + 2])[0]
        nf = struct.unpack("<H", rec[OFF_NFRAMES:OFF_NFRAMES + 2])[0]
        ra.append((ten, ani, nf))
        p += BIO
    return ra


def main(key, ben):
    kho = Kho(V20 if ben == "2.0" else DUAN)
    mp = MAPS[key]
    w = kho.doc(mp.encode("gbk", "replace") + b".wor")
    if not w:
        print("khong co .wor cho %s" % key); return
    m = re.search(r"rect=(\d+),(\d+),(\d+),(\d+)", w.decode("gbk", "replace"))
    x0, y0, x1, y1 = (int(m.group(i)) for i in (1, 2, 3, 4))
    tong = 0
    ani = {}
    yen = {}
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            nb = ("%s%sv_%03d%s%03d_Region_C.dat" % (mp, BS, y, BS, x)).encode("gbk", "replace")
            d = kho.doc(nb)
            if not d:
                continue
            for ten, a, nf in doc_vung(d):
                tong += 1
                if a:
                    ani[ten] = ani.get(ten, 0) + 1
                if b"\xd1\xcc" in ten:
                    yen[ten] = yen.get(ten, 0) + 1
    print("== %s (%s): %d vat canh, %d loai CO HOAT ANH ==" % (key, ben, tong, len(ani)))
    for t, c in sorted(ani.items(), key=lambda kv: -kv[1])[:15]:
        print("   %4d x %s" % (c, t.decode("gbk", "replace")))
    print("   ten co chu 'yen' (khoi): %s" % ({t.decode("gbk", "replace"): c for t, c in yen.items()} if yen else "khong co"))


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "phuongtuong", sys.argv[2] if len(sys.argv) > 2 else "duan")
