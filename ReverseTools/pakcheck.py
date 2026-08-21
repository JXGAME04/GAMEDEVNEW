#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Kiem tra mot duong dan tai nguyen co nam trong .pak khong (bam FileNameToId).

  pakcheck.py <pak> <path1> [path2 ...]     duong dan kieu  \\maps\\xxx\\yyy.wor
"""
import io, os, struct, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)


def name2id(s):
    """KPakList::FileNameToId - s la chuoi latin-1 (byte tho)."""
    uid = 0
    idx = 0
    for ch in s:
        c = ord(ch)
        if 65 <= c <= 90:          # 'A'..'Z' -> thuong
            c = c + 32
        elif c > 127:              # char CO DAU tren MSVC/GCC x86 => am
            c = c - 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


def load_pak(p):
    d = open(p, "rb").read()
    magic, count, ioff, hsize = struct.unpack("<IIII", d[:16])
    ids = set()
    for i in range(count):
        o = ioff + i * 16
        if o + 16 > len(d):
            break
        uid, off, size, flg = struct.unpack("<IIII", d[o:o + 16])
        ids.add(uid)
    return ids, count


if __name__ == "__main__":
    pak = sys.argv[1]
    ids, count = load_pak(pak)
    print("### %s : %d muc" % (os.path.basename(pak), count))
    for p in sys.argv[2:]:
        raw = p.encode("utf-8").decode("utf-8")
        try:
            b = raw.encode("gbk")
        except Exception:
            b = raw.encode("latin-1", "replace")
        s = b.decode("latin-1")
        while BS + BS in s:
            s = s.replace(BS + BS, BS)
        # FindElemFile: them '\' o dau roi bam (ham bam tu ha thuong A-Z)
        uid = name2id(BS + s.lstrip(BS))
        print("  %-8s %08X  %s" % ("CO" if uid in ids else "KHONG", uid, p))
