# -*- coding: utf-8 -*-
"""anh_nen.py - kiem ANH NEN (jpg) ma cac tep _Scroll.ini tro toi: co trong pak 2.0 va pak du an khong,
kich thuoc bao nhieu, dinh dang gi. Chi doc.
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import ucl  # noqa: E402

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


def co_jpg(d):
    """tra (rong, cao) cua JPEG"""
    if not d or d[:2] != b"\xff\xd8":
        return None
    i = 2
    while i < len(d) - 9:
        if d[i] != 0xFF:
            i += 1
            continue
        m = d[i + 1]
        if m in (0xC0, 0xC1, 0xC2, 0xC3, 0xC5, 0xC6, 0xC7, 0xC9, 0xCA, 0xCB, 0xCD, 0xCE, 0xCF):
            h = struct.unpack(">H", d[i + 5:i + 7])[0]
            w = struct.unpack(">H", d[i + 7:i + 9])[0]
            return (w, h)
        if m in (0xD8, 0xD9) or 0xD0 <= m <= 0xD7:
            i += 2
            continue
        ln = struct.unpack(">H", d[i + 2:i + 4])[0]
        i += 2 + ln
    return (0, 0)


def main():
    k20 = Kho(V20, "2.0")
    kda = Kho(DUAN, "du an")
    anh = {}
    for fn in sorted(os.listdir(SP)):
        m = re.match(r"scroll_(v20|duan)_(\d+)\.ini$", fn)
        if not m:
            continue
        s = io.open(os.path.join(SP, fn), "rb").read().decode("gbk", "replace")
        for l in s.replace("\r\n", "\n").split("\n"):
            mm = re.match(r"^\s*Image\s*=\s*(.+?)\s*$", l)
            if mm:
                anh.setdefault(mm.group(1), set()).add(m.group(2))
    print("So anh nen khac nhau duoc tro toi: %d" % len(anh))
    for ten in sorted(anh):
        b = ten.encode("gbk", "replace")
        d20, p20 = k20.doc(b)
        dda, pda = kda.doc(b)
        c20 = co_jpg(d20)
        cda = co_jpg(dda)
        print("%-52s | 2.0: %-28s | du an: %-28s | map %s" % (
            ten[:52],
            ("%s %d KB %sx%s" % (p20, len(d20) // 1024, c20[0], c20[1])) if d20 else "KHONG CO",
            ("%s %d KB %sx%s" % (pda, len(dda) // 1024, cda[0], cda[1])) if dda else "KHONG CO",
            ",".join(sorted(anh[ten])[:6])))
        if d20 and not os.path.exists(os.path.join(SP, os.path.basename(ten.replace(chr(92), "_")))):
            io.open(os.path.join(SP, "anhnen_" + ten.replace(chr(92), "_")), "wb").write(d20)


if __name__ == "__main__":
    main()
