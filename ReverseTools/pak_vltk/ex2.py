# -*- coding: utf-8 -*-
"""ex2 - Rut tep khoi pak VLTK, GIAI NEN DUNG BANG UCL NRV2B.

Khac ex1.py: ex1 dung read_entry cua pakdump (zlib) nen tep nen ra RAC CO LAN
CHU DOC DUOC - rat de tuong nham la "sai bang ma". Pak cua VLTK nen bang UCL.

Moi ban tim thay duoc ghi rieng theo ten pak, vi cung mot duong dan xuat hien
trong NHIEU pak (update01/03/04...) voi noi dung KHAC NHAU - pak nap sau de len
pak nap truoc, nen ban cuoi moi la ban dang chay.

Dung:  python ex2.py /settings/item/foundryresdemand.ini [...]
       (dau / se duoc doi thanh dau nguoc; nho dat MSYS2_ARG_CONV_EXCL=* khi
        goi tu Git Bash, khong thi duong dan bi bien thanh C:\\Program Files\\Git\\...)
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pakdump as P
import ucl

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

BS = chr(92)
ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "ra_vltk")


def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    f.seek(off)
    raw = f.read(cs)
    if fl == 0:
        return raw
    return ucl.nrv2b_decompress_8(raw, size)


def main():
    names = [a.replace("/", BS) for a in sys.argv[1:]]
    if not names:
        print("thieu tham so: duong dan trong pak")
        return 1
    want = {P.name2id(n): n for n in names}
    if not os.path.isdir(OUT):
        os.makedirs(OUT)
    found = {}
    for fn in sorted(os.listdir(ROOT)):
        if not fn.lower().endswith((".pak", ".mps")):
            continue
        try:
            f, es = P.entries(os.path.join(ROOT, fn))
        except Exception:
            continue
        for e in es:
            if e[0] not in want:
                continue
            nm = want[e[0]]
            try:
                d = blob_of(f, e)
                tt = "UCL"
            except Exception as ex:
                d = None
                tt = "LOI giai nen: %s" % ex
            base = nm.split(BS)[-1]
            tep = "%s__%s" % (fn.replace(".pak", ""), base)
            if d is not None:
                open(os.path.join(OUT, tep), "wb").write(d)
                # do do doc duoc: ty le byte in duoc
                doc = sum(1 for b in d if 9 <= b <= 13 or 32 <= b < 127 or b >= 160)
                tt = "UCL, doc duoc %d%%" % (100 * doc // max(1, len(d)))
            print("  %-22s %-46s %7d byte  %s" % (fn, nm, len(d or b""), tt))
            found.setdefault(e[0], []).append(tep)
    print()
    for uid, nm in want.items():
        if uid not in found:
            print("  KHONG THAY  %-46s uid=%08X" % (nm, uid))
    print("  thu muc ra: %s" % OUT)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
