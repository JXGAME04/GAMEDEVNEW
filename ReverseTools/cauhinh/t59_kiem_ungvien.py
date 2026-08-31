# -*- coding: utf-8 -*-
"""t59_kiem_ungvien.py - kiem KY tung ung vien don dep, du 8 duong nap.

VI SAO KHONG QUET HANG LOAT: t57 do duoc 1205/3065 tep "khong ai nhac ten".
Con so do KHONG dung de dua ra quyet dinh xoa hang loat, vi:
  - Nhieu tep duoc nap qua duong ma grep khong thay het (du lieu ban do da nen,
    quy uoc ten theo so hieu, bang trong C++ header).
  - Mot so tep chi duoc nhac o dong DA COMMENT - cong cu bo qua comment nen bao
    la mo coi, nhung do lai chinh la dau hieu "tinh nang bi tat", can doc tay
    moi biet nen bo hay giu.
Nen cach lam dung la: KIEM TUNG UNG VIEN co ten tuoi, doc that tinh nang do la
gi, roi moi quyet.

Cong cu nay kiem 8 duong nap cho tung ung vien va IN RA bang chung, khong tu
quyet dinh gi.

CHI DOC.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = dt.SV
S = dt.S
NGUON = r"D:\GAMEDEVNEW\Sources\Core\Src"
P_MAP = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                     "duongnap_mapdata.txt")

UNG_VIEN = [
    ("header/loidai.lua", "he 'Loi Dai CBT' cu - nghi mo coi"),
    ("startgame/khuvucbang", "lanh dia / khu vuc bang hoi ban cu"),
    ("event/demhuyhoang", "Dem Huy Hoang ban 1"),
    ("event/event_demhuyhoang", "Dem Huy Hoang ban 2"),
    ("global/thanh/npc/add_npc.lua", "bang tbNpcSatThu (boss sat thu 'nua port')"),
    ("item/ib/hoangkim.lua", "the Lien Dau cu"),
    ("item/ib/bachngan.lua", "the Lien Dau cu"),
    ("item/ib/thanhdong.lua", "the Lien Dau cu"),
    ("item/ib/hanthiet.lua", "the Lien Dau cu"),
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan(s):
    return re.sub(r"\\+", "\\\\", s.replace("/", "\\").lower())


def nap_mapdata():
    ra = set()
    if os.path.isfile(P_MAP):
        for l in io.open(P_MAP, encoding="utf-8"):
            l = l.strip().lower()
            if l.endswith(".lua"):
                ra.add(l)
    return ra


def liet_tep(duong):
    p = os.path.join(S, duong.replace("/", os.sep))
    if os.path.isfile(p):
        return [p]
    ra = []
    for goc, _, ts in os.walk(p):
        for t in ts:
            if t.lower().endswith(".lua"):
                ra.append(os.path.join(goc, t))
    return ra


def tim_trong(thu_muc, ten, duoi):
    """Tim ten (khong phan biet hoa thuong) trong cac tep duoi thu_muc."""
    ra = []
    for goc, _, ts in os.walk(thu_muc):
        for t in ts:
            if not t.lower().endswith(duoi):
                continue
            p = os.path.join(goc, t)
            try:
                d = doc(p)
            except OSError:
                continue
            for i, l in enumerate(d.replace("\r\n", "\n").split("\n"), 1):
                if ten in l.lower():
                    ra.append((os.path.basename(p), i,
                               l.strip()[:80],
                               l.lstrip().startswith(("--", "//"))))
                    if len(ra) >= 4:
                        return ra
    return ra


def main():
    md = nap_mapdata()
    print("=== t59 KIEM TUNG UNG VIEN (8 duong nap) ===")
    print("    du lieu ban do: %d ten .lua" % len(md))
    print()
    for duong, mo_ta in UNG_VIEN:
        p0 = os.path.join(S, duong.replace("/", os.sep))
        if not os.path.exists(p0):
            print("## %-34s KHONG TON TAI (da go tu truoc)" % duong)
            print()
            continue
        tep = liet_tep(duong)
        n_dong = 0
        for p in tep:
            try:
                n_dong += doc(p).count("\n") + 1
            except OSError:
                pass
        print("## %s" % duong)
        print("   %s | %d tep, %d dong" % (mo_ta, len(tep), n_dong))

        # (a,b,c) duong dan van ban trong cay script
        goi = dt.ai_goi(duong)
        print("   [a-c] duong dan trong script/settings : %d cho" % len(goi))
        for g in goi[:4]:
            print("         %s:%d | %s" % g)

        # (d) settings da nam trong ai_goi; (f) C++
        ten_co_ban = set(os.path.basename(x).lower() for x in tep)
        cpp = []
        for t in ten_co_ban:
            cpp.extend(tim_trong(NGUON, t, (".cpp", ".h")))
        print("   [f]   ma C++ nhac ten tep            : %d cho" % len(cpp))
        for c in cpp[:3]:
            print("         %s:%d %s| %s"
                  % (c[0], c[1], "(DA COMMENT) " if c[3] else "", c[2]))

        # (h) du lieu ban do
        trong_md = sorted(t for t in ten_co_ban if t in md)
        print("   [h]   du lieu ban do nhac ten tep    : %d/%d tep"
              % (len(trong_md), len(ten_co_ban)))
        if trong_md:
            print("         %s" % ", ".join(trong_md[:5]))
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
