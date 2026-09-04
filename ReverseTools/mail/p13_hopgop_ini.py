# -*- coding: utf-8 -*-
"""[DAUGIA 04/09 A6] Them 3 muc vao UiGiveItem.ini (hop dua vat pham) cho che do KY GUI:
o nhan "Gia ban", o nhap gia, nut doi loai tien. Chi hien khi KUiAffairItem::SetAuctionMode(1).
Dat de len phan duoi cua o mo ta (96..182) vi luc ky gui mo ta rat ngan. Idempotent.
Chay: python p13_hopgop_ini.py"""
import io, os, sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
MIRROR_C = r"D:\GAMEDEVNEW_wt_mail\serverscript_jx2\mail\client"
REL = r"ui\Ui3\UiGiveItem.ini"
BS = chr(92)
E = "\r\n"


def V(s):
    return unicode_to_tcvn3_bytes(s).decode("latin-1")


BLOCK = E.join([
    "",
    "; [DAUGIA 04/09 A6] che do KY GUI: hien them o nhap gia + nut doi loai tien ngay trong hop nay",
    "; (chu 04/09: \"box co the tuy chon nhap loai tien va bo do vao chung 1\"). Cac muc nay chi hien",
    "; khi KUiAffairItem::SetAuctionMode(1); hop giao nop khac khong dung toi.",
    "[AucPriceLabel]",
    "Left=16",
    "Top=138",
    "Width=52",
    "Height=16",
    "Font=12",
    "HAlign=0",
    "VAlign=1",
    "Color=255,217,78",
    "Text=" + V("Giá bán"),
    "",
    "[AucPriceEdit]",
    "Left=70",
    "Top=137",
    "Width=110",
    "Height=16",
    "MultiLine=0",
    "Password=0",
    "HAlign=0",
    "VAlign=1",
    "Type=0",
    "MaxLen=10",
    "Font=12",
    "Color=255,255,255",
    "BorderColor=120,120,120",
    "",
    "[AucCurBtn]",
    "Left=16",
    "Top=158",
    "Width=90",
    "Height=19",
    "Font=12",
    "HAlign=1",
    "VAlign=1",
    "LabelYOffset=4",
    "Color=169,211,190",
    "OverColor=124,255,125",
    "SelectColor=77,77,77",
    "Up=0",
    "Down=1",
    "Over=2",
    "OverFrame=2",
    "Trans=0",
    "Image=" + BS + "spr" + BS + "Ui4" + BS + "common" + BS + unicode_to_tcvn3_bytes("").decode("latin-1") + "\xd0\xa1\xb0\xb4\xc5\xa5\xcb\xc4\xd7\xd6.spr",
    "Label=" + V("Ngân lượng"),
    "",
])


def main():
    for root in (CL, MIRROR_C):
        p = os.path.join(root, REL)
        if not os.path.exists(p):
            print("  bo qua (khong co):", p)
            continue
        s = io.open(p, "r", encoding="latin-1", newline="").read()
        if "[AucPriceLabel]" in s:
            print("  da co:", p)
            continue
        s = s.rstrip("\r\n") + E + BLOCK
        io.open(p, "w", encoding="latin-1", newline="").write(s)
        print("  ghi:", p, len(s))


if __name__ == "__main__":
    main()
