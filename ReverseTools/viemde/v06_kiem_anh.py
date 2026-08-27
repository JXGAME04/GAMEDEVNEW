# -*- coding: utf-8 -*-
"""VIEM DE - buoc 4c: 5 vat pham phai TAO MOI. Kiem xem duong dan anh (.spr) cua
ban Linux CO SAN trong pak client JX1 khong.

Neu co -> dung Y NGUYEN duong dan cua Linux, vat pham giong 100%.
Neu khong -> phai muon anh cua mot vat pham JX1 da co (guide buoc 4).

Ham bam ten tep pak = KPakList::FileNameToId (da kiem dinh 300/300, xem hashtest.py).
"""
import os
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SEP = "\\"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

# (ma Linux, ten, duong dan anh cua ban Linux)
CAN_TAO = [
    ("6,1,30350", "Hộp Mặt Nạ Chiến Trường", r"\spr\item\script\item_chrismasbox.spr"),
    ("6,1,30538", "Đằng Long Thạch - Hạ", r"\spr\vng\item\longha.spr"),
    ("6,1,30008", "Đồ Phổ Tinh Sương Yêu Đái", r"\spr\item\citydefence\fragment.spr"),
    ("6,1,30011", "Đồ Phổ Tinh Sương Ngọc Bội", r"\spr\item\citydefence\fragment.spr"),
    ("6,1,30534", "Đồ Phổ Đằng Long Bội", r"\spr\vng\item\dophodanglong.spr"),
]


def pid(name):
    n = name[1:] if name[:1] in (SEP, "/") else name
    pb = bytes(b + 32 if 65 <= b <= 90 else b for b in (SEP + n).encode("latin-1"))
    i = 0
    idx = 0
    for b in pb:
        idx += 1
        c = b + 32 if 65 <= b <= 90 else b
        i = ((((i + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF) & 0xFFFFFFFF
    return i ^ 0x12345678


def load_index(p):
    try:
        f = open(p, "rb")
    except Exception:
        return None
    head = f.read(32)
    if head[:4] != b"PACK":
        f.close()
        return None
    cnt, itoff, datoff = struct.unpack_from("<III", head, 4)
    f.seek(itoff)
    raw = f.read(16 * cnt)
    f.close()
    return set(struct.unpack_from("<I", raw, 16 * k)[0] for k in range(cnt))


def main():
    paks = []
    root = CLI + r"\data"
    for f in sorted(os.listdir(root)):
        if f.lower().endswith(".pak"):
            s = load_index(os.path.join(root, f))
            if s:
                paks.append((f, s))
    print("da nap %d pak client" % len(paks))
    print()
    for ma, ten, anh in CAN_TAO:
        u = pid(anh)
        o = [f for f, s in paks if u in s]
        tren_dia = os.path.isfile(CLI + anh)
        trang_thai = ("CO trong " + o[0]) if o else ("CO tren dia" if tren_dia else "<< KHONG CO")
        print("  %-11s %-30s %-44s %s" % (ma, ten[:29], anh[-42:], trang_thai))


main()
