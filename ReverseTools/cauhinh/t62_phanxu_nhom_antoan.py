# -*- coding: utf-8 -*-
"""t62_phanxu_nhom_antoan.py - PHAN XU cac nhom ma bo khao sat xep "AN_TOAN".

Bo khao sat quet script + settings + C++ va ket luan nhieu nhom la 0 tham chieu.
NHUNG no KHONG kiem DUONG NAP THU 8 (NPC/bay/obj gan bang du lieu ban do trong
bin\\server\\Pak\\*.pak, duong dan nam trong khoi DA NEN).

Do thu: 454/1.288 tep trong 9 cay thu muc goc Trung Quoc CO trong du lieu ban do
-> ket luan "AN_TOAN" cua no cho nhom do la SAI, va neu lam theo thi da doi 454
tep dang song ra khoi cay.

Cong cu nay doi chieu TUNG NHOM voi danh sach duong nap thu 8 (duongnap_mapdata.txt,
sinh boi t58 - nho dung ucl.nrv2b_decompress_8 chu KHONG dung pakdump.read_entry).

CHI DOC.
"""
import io
import os
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
P_MD = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                    "duongnap_mapdata.txt")

# (nhan, danh sach duong dan tuong doi, hang bo khao sat da xep)
NHOM = [
    ("9 cay thu muc goc Trung Quoc", None, "AN_TOAN"),   # None = tu do
    ("skill/ (ten ASCII)", ["skill"], "AN_TOAN"),
    ("maps/khac/trap", ["maps/khac/trap"], "AN_TOAN"),
    ("maps/*/obj", None, "AN_TOAN"),                     # xu ly rieng
    ("header/", ["header"], "AN_TOAN"),
    ("tong/map (ban sao JX2)", ["tong/map"], "AN_TOAN"),
    ("global/thanh/npc (5 tep con lai)", ["global/thanh/npc"], "AN_TOAN"),
    ("task/tollgate/messenger", ["task/tollgate/messenger"], "CAN_THAN"),
]


def nap_md():
    return set(l.strip().lower()
               for l in io.open(P_MD, encoding="utf-8")
               if l.strip().lower().endswith(".lua"))


def duyet(goc):
    ra = []
    for g, _, ts in os.walk(goc):
        for t in ts:
            if t.lower().endswith(".lua") and ".truoc_" not in t.lower():
                ra.append(os.path.join(g, t))
    return ra


def main():
    md = nap_md()
    print("=== t62 PHAN XU cac nhom bo khao sat xep AN_TOAN ===")
    print("    duong nap thu 8 (du lieu ban do): %d ten .lua" % len(md))
    print()
    print("%-38s %6s %6s %6s  %s"
          % ("NHOM", "TONG", "TRONG", "NGOAI", "KET LUAN"))
    print("-" * 82)

    for nhan, duong, hang in NHOM:
        tep = []
        if nhan.startswith("9 cay"):
            for d in os.listdir(S):
                p = os.path.join(S, d)
                if os.path.isdir(p) and any(ord(c) > 127 for c in d):
                    tep.extend(duyet(p))
        elif nhan.startswith("maps/*/obj"):
            base = os.path.join(S, "maps")
            for d in os.listdir(base):
                p = os.path.join(base, d, "obj")
                if os.path.isdir(p):
                    tep.extend(duyet(p))
        else:
            for x in duong:
                p = os.path.join(S, x.replace("/", os.sep))
                if os.path.isdir(p):
                    tep.extend(duyet(p))
                elif os.path.isfile(p):
                    tep.append(p)
        if not tep:
            print("%-38s %6s  (khong con trong cay)" % (nhan, "-"))
            continue
        trong = [p for p in tep if os.path.basename(p).lower() in md]
        ngoai = len(tep) - len(trong)
        if trong:
            kl = "!! SAI - %d tep DANG DUOC BAN DO DUNG" % len(trong)
        else:
            kl = "dung - 0 tep trong du lieu ban do"
        print("%-38s %6d %6d %6d  %s"
              % (nhan, len(tep), len(trong), ngoai, kl))
    print()
    print("LUU Y: 'ngoai' (khong co trong du lieu ban do) VAN CHUA du de goi la")
    print("chet - con quy uoc ten theo so hieu va cac tep chi duoc nhac o dong")
    print("da comment. Phai doc tay tung cum truoc khi doi.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
