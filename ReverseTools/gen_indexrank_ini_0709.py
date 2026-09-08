#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_indexrank_ini_0709.py

[XEPHANG 07/09] Sinh lai Ui\\IndexAndRankMapping.ini cho so do ma bang MOI.

Vi sao phai doi: dot [HOASON 01/09] nang MAX_FACTION_NUM 10 -> 13, ma KLadder.h
tinh enum theo MAX_FAC = MAX_FACTION_NUM + 1:
      enumFacTopTenMasterHand = 4
      enumFacTopTenRicher     = 4 + MAX_FAC
      enumLadderEnd           = enumFacTopTenRicher + MAX_FAC
CU  (MAX_FAC=11): Richer = 15, End = 26  -> .ini 25 muc la DUNG
MOI (MAX_FAC=14): Richer = 18, End = 32  -> .ini 25 muc thanh SAI

Client ap cung ma bang = so thu tu dong + 1 (UiStrengthRank.cpp:128
"pIndex[i].usIndexId = i + 1"), va KLadder::GetTopTen (KLadder.cpp:96-103) doc:
      ID < enumFacTopTenRicher -> TopTenFacMasterHand(ID - 4 - 1)
      nguoc lai               -> TopTenFacRich      (ID - 18 - 1)
Nen voi so do moi:
      1        Thap Dai Cao Thu     (LevelStat)
      2        Thap Dai Phu Hao     (MoneyStat)
      3        (duong ke)           (KillerStat - du lieu dang rong)
      4..17    Cao Thu theo phai    fac = ID-5   -> -1..12  (14 muc)
      18..31   Phu Hao theo phai    fac = ID-19  -> -1..12  (14 muc)
      >= 32    ngoai bang -> GetTopTen tra NULL
ListCount PHAI = 31.

Voi ban .ini CU 25 muc, ID 15/16/17 (dan la "Hoang Da/Thieu Lam/Thien Vuong PHU HAO")
that ra hoi CAP cua Hoa Son/Vu Hon/Tieu Dao, va 18..25 lech 3 bac; 6 bang Phu Hao
cuoi khong bao gio hoi duoc.

Tep nay KHONG nam trong git (chi song trong cay chay that) -> sao luu truoc khi ghi.

Chay:  python ReverseTools/gen_indexrank_ini_0709.py [--lui]
"""
import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes, _UNICODE_TO_TCVN3

_T2U = {ord(v): k for k, v in _UNICODE_TO_TCVN3.items()}
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\IndexAndRankMapping.ini"
HAUTO = ".truoc_xephang_0709"

# chi so phai -1..12; -1 = chua nhap mon. Thu tu lay tu GameDataDef.h:497
# ("13 = 10 goc + Hoa Son 10 + Vu Hon 11 + Tieu Dao 12") va tu chinh ban .ini cu.
PHAI = [
    "Hoang Dã",     # -1 chua nhap mon
    "Thiếu Lâm",    # 0
    "Thiên Vương",  # 1
    "Đường Môn",    # 2
    "Ngũ Độc",      # 3
    "Nga My",       # 4
    "Thúy Yên",     # 5
    "Cái Bang",     # 6
    "Thiên Nhẫn",   # 7
    "Võ Đang",      # 8
    "Côn Lôn",      # 9
    "Hoa Sơn",      # 10
    "Vũ Hồn",       # 11
    "Tiêu Dao",     # 12
]


def vn(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


def doc_vn(s):
    return "".join(_T2U.get(ord(c), c) for c in s)


def sinh():
    d = []
    d.append("[Main]")
    d.append("ListCount=%d" % (3 + 2 * len(PHAI)))   # 3 + 28 = 31
    d.append(";MaxCount=10195")
    d.append("; [XEPHANG 07/09] 25 -> 31 muc: MAX_FACTION_NUM 10 -> 13 (dot HOASON 01/09) lam")
    d.append("; KLadder.h doi enumFacTopTenRicher 15 -> 18 va enumLadderEnd 26 -> 32.")
    d.append("; Ma bang = so thu tu dong + 1 (UiStrengthRank.cpp:128).")
    d.append("; 1 Cao Thu / 2 Phu Hao / 3 Sat Thu / 4..17 Cao Thu theo phai / 18..31 Phu Hao theo phai.")
    d.append("")
    d.append("[Index]")
    d.append("")

    def muc(nId, title, unit):
        d.append("%d_Title=%s" % (nId, title))
        d.append("%d_Unit=%s" % (nId, unit))
        d.append("")

    muc(1, vn("Thập Đại Cao Thủ"), vn(" cấp"))
    muc(2, vn("Thập Đại Phú Hào"), vn(" lượng"))
    # ID 3 = KillerStat. Giu nguyen la duong ke nhu ban cu (du lieu Sat Thu dang rong).
    muc(3, "-------------------------", "")

    for k, ten in enumerate(PHAI):          # ID 4..17
        muc(4 + k, vn("%s Cao Thủ" % ten), vn(" cấp"))
    for k, ten in enumerate(PHAI):          # ID 18..31
        muc(18 + k, vn("%s Phú Hào" % ten), vn(" lượng"))

    return "\r\n".join(d) + "\r\n"


def main():
    if not os.path.exists(DICH):
        print("KHONG CO TEP %s" % DICH)
        return 1
    h = DICH + HAUTO

    if "--lui" in sys.argv:
        if os.path.exists(h):
            io.open(DICH, "w", encoding="latin-1", newline="").write(
                io.open(h, "r", encoding="latin-1", newline="").read())
            print("LUI xong tu %s" % h)
            return 0
        print("KHONG CO BAN LUU %s" % h)
        return 1

    goc = io.open(DICH, "r", encoding="latin-1", newline="").read()
    if not os.path.exists(h):
        io.open(h, "w", encoding="latin-1", newline="").write(goc)
        print("Da sao luu -> %s" % os.path.basename(h))
    else:
        print("Ban sao luu da co, giu nguyen: %s" % os.path.basename(h))

    moi = sinh()
    io.open(DICH, "w", encoding="latin-1", newline="").write(moi)
    print("Da ghi %s (%d -> %d byte)" % (os.path.basename(DICH), len(goc), len(moi)))
    print("\n--- doc lai (giai TCVN3) ---")
    for dong in moi.replace("\r\n", "\n").split("\n"):
        if "_Title" in dong or "ListCount" in dong:
            print("   " + doc_vn(dong))
    return 0


if __name__ == "__main__":
    sys.exit(main())
