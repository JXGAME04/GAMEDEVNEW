# -*- coding: utf-8 -*-
"""pak_tim.py - tra TEN tep trong cac pak cua client VLTK 2.0 bang bam ten (KPakList::FileNameToId),
va rut tep ra dia. Chi doc, khong sua gi trong cay client.

Dung:
  python pak_tim.py tim  <ten1> [ten2 ...]          -> cho biet ten nam trong pak nao, co bao nhieu byte
  python pak_tim.py rut  <ten> <duong_dan_ra>       -> giai nen va ghi ra tep
  python pak_tim.py list <pak> [tien_to_uid]        -> in so muc cua pak
"""
import io
import os
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import ucl  # noqa: E402

CLIENT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data"
PAKS = ["common.pak", "slistcl.pak", "update.pak", "minimap.pak", "freeresource.pak",
        "skills.pak", "spr.pak", "resource.pak", "map.pak", "font.pak", "sound.pak"]


def name2id(s):
    """Bam ten tep JX: A-Z -> thuong, byte > 127 coi la am, chi so tu 1."""
    uid = 0
    idx = 0
    for ch in s:
        c = ch if isinstance(ch, int) else ord(ch)
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


def ten_byte(s):
    """Ten co the la str (ascii) hoac bytes (GBK) -> danh sach ma byte."""
    if isinstance(s, bytes):
        return list(s)
    return [ord(c) for c in s]


def chi_muc(p):
    f = open(p, "rb")
    sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
    f.seek(ioff)
    raw = f.read(count * 16)
    d = {}
    for i in range(count):
        uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
        d[uid] = (off, size, cf)
    return f, d, count


_CACHE = {}


def mo_tat_ca():
    if _CACHE:
        return _CACHE
    for name in PAKS:
        p = os.path.join(CLIENT, name)
        if not os.path.exists(p):
            continue
        try:
            _CACHE[name] = chi_muc(p)
        except Exception as e:
            print("  (bo qua %s: %s)" % (name, e))
    return _CACHE


def doc_muc(f, off, size, cf):
    """cf: 24 bit thap = so byte da nen (0 = khong nen), byte cao = co."""
    csize = cf & 0xFFFFFF
    f.seek(off)
    if csize == 0 or csize == size:
        return f.read(size)
    raw = f.read(csize)
    try:
        return bytes(ucl.nrv2b_decompress_8(raw, size))
    except Exception as e:
        print("  (giai nen hong: %s)" % e)
        return raw


def rut(ten, ra):
    paks = mo_tat_ca()
    uid = name2id(ten_byte(ten))
    for name, (f, d, cnt) in paks.items():
        if uid in d:
            off, size, cf = d[uid]
            data = doc_muc(f, off, size, cf)
            io.open(ra, "wb").write(data)
            print("%s -> %s (%s, %d byte)" % (ten if isinstance(ten, str) else ten.decode("latin-1"), ra, name, len(data)))
            return data
    print("KHONG THAY: %s" % ten)
    return None


def tim(tens):
    paks = mo_tat_ca()
    for ten in tens:
        uid = name2id(ten_byte(ten))
        hien = ten.decode("latin-1") if isinstance(ten, bytes) else ten
        thay = []
        for name, (f, d, cnt) in paks.items():
            if uid in d:
                off, size, cf = d[uid]
                thay.append("%s (off %d, size %d, cf 0x%X)" % (name, off, size, cf))
        print("%-60s uid %08X -> %s" % (hien[:60], uid, "; ".join(thay) if thay else "KHONG THAY"))


if __name__ == "__main__":
    lenh = sys.argv[1] if len(sys.argv) > 1 else "tim"
    if lenh == "tim":
        tim(sys.argv[2:])
    elif lenh == "list":
        f, d, cnt = chi_muc(os.path.join(CLIENT, sys.argv[2]))
        print("%s: %d muc" % (sys.argv[2], cnt))
    else:
        print(__doc__)
