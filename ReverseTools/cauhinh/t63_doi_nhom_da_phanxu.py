# -*- coding: utf-8 -*-
"""t63_doi_nhom_da_phanxu.py - doi cac tep trong 5 nhom DA QUA PHAN XU.

BOI CANH: bo khao sat xep 8 nhom la "AN_TOAN". Toi doi chieu voi DUONG NAP THU 8
(du lieu ban do trong Pak, t58/t62) thi 3 nhom SAI:
    9 cay thu muc goc Trung Quoc : 454/1288 tep DANG duoc ban do dung
    script\\skill\\ (ten ASCII)    : 253/417  tep DANG duoc ban do dung
    task\\tollgate\\messenger      :   1/61   tep DANG duoc ban do dung
Neu lam theo bo khao sat thi da doi 708 tep DANG SONG ra khoi cay.

Con 5 nhom qua duoc ca hai phep kiem (0 tham chieu van ban + 0 ten trong du lieu
ban do). Cong cu nay kiem LAI TUNG TEP bang doi_tep.ai_goi (quet script\\ va
settings\\ theo duong dan, chuan hoa hai dau gach, khop theo ranh gioi) roi moi
doi. Tep nao con cho goi thi CHAN, khong doi.

    maps\\khac\\trap        36 tep - trap dich chuyen giua ban do, loi goi duy
                                    nhat o startgame\\khac\\othermap.lua:323 DA COMMENT
    maps\\*\\obj            19 tep - lop obj (cao thi / dau hieu / bang niem yet)
    header\\                 tep bang du lieu cua cac he da go
    tong\\map               16 tep - ban sao chet cua cay bang hoi JX2; ban that
                                    o scriptjx2\\tong_vn (KTongJX2.cpp:3826)
    global\\thanh\\npc        5 tep - ban "nua port" boss Sat Thu; ban that o
                                    task\\tollgate\\killbosshead.lua

Mac dinh DIEN TAP; --ghi moi doi that.
"""
import io
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = dt.S
P_MD = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                    "duongnap_mapdata.txt")

NHOM = [
    ("maps/khac/trap", "trap dich chuyen giua ban do - loi goi duy nhat o"
     " startgame\\khac\\othermap.lua:323 DA COMMENT"),
    ("maps/*/obj", "lop obj (cao thi / dau hieu / bang niem yet) - khong thanh"
     " nao dat"),
    ("header", "bang du lieu cua cac he da go"),
    ("tong/map", "ban sao chet cua cay bang hoi JX2 - ban that o"
     " scriptjx2\\tong_vn (KTongJX2.cpp:3826)"),
    ("global/thanh/npc", "ban 'nua port' boss Sat Thu - ban that o"
     " task\\tollgate\\killbosshead.lua"),
]


def nap_md():
    return set(l.strip().lower()
               for l in io.open(P_MD, encoding="utf-8")
               if l.strip().lower().endswith(".lua"))


def liet(mau):
    """Tra ve danh sach duong dan tuong doi (so voi script\\)."""
    ra = []
    if mau == "maps/*/obj":
        base = os.path.join(S, "maps")
        for d in sorted(os.listdir(base)):
            p = os.path.join(base, d, "obj")
            if os.path.isdir(p):
                for t in sorted(os.listdir(p)):
                    if t.lower().endswith(".lua"):
                        ra.append("maps/%s/obj/%s" % (d, t))
        return ra
    p0 = os.path.join(S, mau.replace("/", os.sep))
    if not os.path.isdir(p0):
        return ra
    for g, _, ts in os.walk(p0):
        for t in sorted(ts):
            if t.lower().endswith(".lua") and ".truoc_" not in t.lower():
                rel = os.path.relpath(os.path.join(g, t), S)
                ra.append(rel.replace(os.sep, "/"))
    return ra


def main():
    ghi = "--ghi" in sys.argv[1:]
    md = nap_md()
    print("=== t63 doi 5 nhom da qua phan xu - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    print("    duong nap thu 8: %d ten .lua" % len(md))
    print()
    tong = 0
    for mau, ly_do in NHOM:
        ds = liet(mau)
        if not ds:
            print("## %-22s (khong con trong cay)" % mau)
            continue
        print("## %s  (%d tep)" % (mau, len(ds)))
        n_ok = n_chan = 0
        for rel in ds:
            ten = os.path.basename(rel).lower()
            if ten in md:
                print("   [CHAN-BANDO] %s" % rel)
                n_chan += 1
                continue
            goi = dt.ai_goi(rel)
            if goi:
                print("   [CHAN-GOI  ] %-46s %s:%d"
                      % (rel, goi[0][0], goi[0][1]))
                n_chan += 1
                continue
            ok, msg = dt.doi(rel, ly_do, ghi=ghi)
            if ok:
                n_ok += 1
            else:
                print("   [LOI] %s: %s" % (rel, msg))
                n_chan += 1
        print("   -> %d doi duoc, %d bi chan" % (n_ok, n_chan))
        tong += n_ok
        print()
    print("=> %d tep %s" % (tong, "da doi" if ghi else "se doi"))
    if not ghi:
        print()
        print("DIEN TAP - chua doi gi. Chay lai voi --ghi de lam that.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
