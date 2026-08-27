# -*- coding: utf-8 -*-
"""Doi chieu THU TU KHAI BAO (KProtocolProcess.h) voi THU TU DANG KY
(KProtocolProcess.cpp) cho tung trinh xu ly giao thuc.

Vi sao can: vi tri khai bao khong anh huong bien dich, nhung cay nay co quy uoc
moi tep phai cung thu tu (memory 'protocol-add-order'). Sai thu tu lam nguoi doc
sau hieu nham thu tu giao thuc - va da tung bi bat loi.

Chay:  python kiem_thutu_handler.py
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = r"D:\GAMEDEVNEW"
F_H = os.path.join(CAY, "Sources", "Core", "Src", "KProtocolProcess.h")
F_C = os.path.join(CAY, "Sources", "Core", "Src", "KProtocolProcess.cpp")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dang_ky(txt, tien_to):
    """[(ten_giao_thuc, ten_ham, dong)] theo thu tu xuat hien, BO dong bi chu thich"""
    ra = []
    for i, dong in enumerate(txt.split("\r\n")):
        if dong.lstrip().startswith("//"):
            continue
        m = re.search(r"ProcessFunc\[\s*(" + tien_to + r"_\w+)\s*\]\s*=\s*&KProtocolProcess::(\w+)", dong)
        if m:
            ra.append((m.group(1), m.group(2), i + 1))
    return ra


def khai_bao(txt):
    """{ten_ham: dong} + thu tu xuat hien"""
    thu_tu = []
    for i, dong in enumerate(txt.split("\r\n")):
        if dong.lstrip().startswith("//"):
            continue
        m = re.match(r"\s*void\s+(\w+)\s*\(", dong)
        if m:
            thu_tu.append((m.group(1), i + 1))
    return thu_tu


def kiem(nhan, dk, kb):
    print("=" * 76)
    print("  " + nhan)
    print("=" * 76)
    vt = {}
    for k, (ten, d) in enumerate(kb):
        vt.setdefault(ten, (k, d))          # lan khai bao DAU TIEN
    truoc_ten, truoc_k = None, -1
    loi = 0
    thieu = 0
    for gt, ham, d in dk:
        if ham not in vt:
            print("  !! %-26s -> %-26s KHONG CO KHAI BAO trong .h" % (gt, ham))
            thieu += 1
            continue
        k, dh = vt[ham]
        if k < truoc_k:
            print("  !! %-26s -> %-26s khai bao o .h:%d, DUNG TRUOC '%s' (.h:%d)"
                  % (gt, ham, dh, truoc_ten, kb[truoc_k][1]))
            loi += 1
        else:
            truoc_ten, truoc_k = ham, k
    print("  -> %d muc dang ky | %d sai thu tu | %d thieu khai bao"
          % (len(dk), loi, thieu))
    return loi, thieu


def main():
    th = doc(F_H)
    tc = doc(F_C)
    kb = khai_bao(th)
    tong_loi = 0
    for nhan, tien_to in (("s2c  (may chu -> client)", "s2c"),
                          ("c2s  (client -> may chu)", "c2s")):
        dk = dang_ky(tc, tien_to)
        l, t = kiem(nhan, dk, kb)
        tong_loi += l
        print()
    # in rieng phan duoi cung de mat thuong doi chieu
    for tien_to in ("s2c", "c2s"):
        dk = dang_ky(tc, tien_to)[-6:]
        print("  6 muc %s cuoi cung, dang ky (.cpp)  ->  khai bao (.h):" % tien_to)
        vt = {}
        for k, (ten, d) in enumerate(kb):
            vt.setdefault(ten, (k, d))
        for gt, ham, d in dk:
            k, dh = vt.get(ham, (None, None))
            print("     .cpp:%-5d %-26s -> %-26s .h:%s" % (d, gt, ham, dh))
        print()
    print("=" * 76)
    print("  TONG: %s" % ("CO %d cho sai thu tu" % tong_loi if tong_loi else "thu tu khop"))
    print("=" * 76)
    return 1 if tong_loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
