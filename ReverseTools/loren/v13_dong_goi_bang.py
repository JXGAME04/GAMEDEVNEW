# -*- coding: utf-8 -*-
r"""v13 - DONG GOI BO BANG DU LIEU cua he lo ren.

Ban Linux to chuc bang vat pham THEO PHIEN BAN:
      \settings\item\<so 3 chu so>\<ten bang>
Moi mon do mang mot so phien ban, tra dung bang cua doi minh. JX1 co
ITEM_VERSION = 1 nen phai co ca thu muc 000 va 001.

Bo bang gom:
  settings\item\compoundscript.txt        COMPOUND_TYPE 0..8 -> kich ban
  settings\item\itemvaluescript.txt       tra gia tri vat pham (da nan ma)
  settings\item\{000,001}\atlas_compound.txt          (da nan ma)
  settings\item\{000,001}\magicattriblevel.txt        <- lay CUA JX1
  settings\item\{000,001}\magicattriblevel_index.txt  <- lay CUA JX1
  settings\item\{000,001}\itemvalue\*.txt             10 bang cua ban Linux

Vi sao magicattriblevel lay CUA JX1: hai ban cung so dong (481/49) va cung so
do cot, nhung khac byte. Bang nay mo ta thuoc tinh phep CUA CHINH kho vat pham,
nen phai dung ban cua JX1 - chep ban Linux de len la mo ta sai kho cua minh.

CHOT AN TOAN: truoc khi ghi, kiem tung bang.
  - dong nao co NHIEU cot hon dong tieu de la CHAN LAI. Do dung kieu loi da
    lam sap GameServer 26/08 (dong dinh doi -> KTabFile lech luoi -> Load
    FALSE -> vong nap bo ngang -> 8 bang cuoi rong).
  - dong THIEU cot thi khong sao: CreateTabOffset tu dem o rong roi sang dong
    moi (KTabFile.cpp:159-168).
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
RA = os.path.join(HERE, "ra", "settings", "item")
LNX = r"D:\ServerLinux\server1\settings\item"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"

TAB = chr(9)
CR = chr(13)
NL = chr(10)

PHIEN_BAN = ("000", "001")

# 10 bang gia tri: 7 cai co o moi phien ban Linux, 3 cai chi co o ban 004
BANG_ITEMVALUE = [
    ("000", "equip_enchasable_level.txt"),
    ("000", "equip_enchasable_magic_pos.txt"),
    ("000", "equip_enchasable_socket.txt"),
    ("000", "equip_enchasable_type.txt"),
    ("000", "equip_normal_magic.txt"),
    ("000", "magicattrib_combine.txt"),
    ("000", "ore.txt"),
    ("004", "equip_gold.txt"),
    ("004", "fantasygold.txt"),
    ("004", "fantasygoldessence.txt"),
]


def kiem_luoi(b, ten):
    """tra ve (ok, thong bao). Chan dong THUA cot."""
    d = b.decode("latin-1")
    dong = [x for x in d.replace(CR + NL, NL).split(NL)]
    if not dong or not dong[0].strip():
        return False, "tep rong"
    ncot = len(dong[0].split(TAB))
    thua = []
    for i, x in enumerate(dong[1:], 2):
        if not x.strip():
            continue
        n = len(x.split(TAB))
        if n > ncot:
            thua.append((i, n))
    if thua:
        return False, "%d dong THUA cot (vd dong %d co %d cot, tieu de %d)" % (
            len(thua), thua[0][0], thua[0][1], ncot)
    crlf = d.count(CR + NL)
    lf = d.count(NL) - crlf
    return True, "%d cot, %d dong, CRLF=%d LF=%d" % (ncot, len(dong), crlf, lf)


def chep(nguon, dich, nhan):
    if not os.path.isfile(nguon):
        print("  *** THIEU NGUON  %s" % nguon)
        return False
    b = io.open(nguon, "rb").read()
    ok, msg = kiem_luoi(b, nhan)
    if not ok:
        print("  *** LUOI HONG    %-44s %s" % (nhan, msg))
        return False
    d = os.path.dirname(dich)
    if not os.path.isdir(d):
        os.makedirs(d)
    io.open(dich, "wb").write(b)
    print("  OK  %-46s %s" % (nhan, msg))
    return True


def main():
    print("DONG GOI BO BANG CUA HE LO REN")
    print("=" * 92)
    loi = 0

    # 1. compoundscript.txt - chep nguyen tu ban Linux (duong dan kich ban
    #    cua JX1 trung khuon: \script\item\compound\*.lua)
    if not chep(os.path.join(LNX, "compoundscript.txt"),
                os.path.join(RA, "compoundscript.txt"),
                "compoundscript.txt"):
        loi += 1

    # 2. itemvaluescript.txt - ban DA NAN MA (v03)
    if not chep(os.path.join(HERE, "ra", "itemvaluescript.txt"),
                os.path.join(RA, "itemvaluescript.txt"),
                "itemvaluescript.txt"):
        loi += 1

    # 3. bang theo phien ban
    for pb in PHIEN_BAN:
        print()
        print("--- phien ban %s ---" % pb)
        if not chep(os.path.join(HERE, "ra", "atlas_compound.txt"),
                    os.path.join(RA, pb, "atlas_compound.txt"),
                    "%s\\atlas_compound.txt" % pb):
            loi += 1
        for f in ("magicattriblevel.txt", "magicattriblevel_index.txt"):
            if not chep(os.path.join(SRV, f),
                        os.path.join(RA, pb, f),
                        "%s\\%s  (cua JX1)" % (pb, f)):
                loi += 1
        for nguon_pb, f in BANG_ITEMVALUE:
            if not chep(os.path.join(LNX, nguon_pb, "itemvalue", f),
                        os.path.join(RA, pb, "itemvalue", f),
                        "%s\\itemvalue\\%s" % (pb, f)):
                loi += 1

    print()
    print("=" * 92)
    if loi == 0:
        n = 0
        for d, _, fs in os.walk(RA):
            n += len([f for f in fs if f.endswith(".txt")])
        print("  XONG - %d bang o %s" % (n, RA))
    else:
        print("  CO %d MUC LOI" % loi)
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
