# -*- coding: utf-8 -*-
r"""v19 - SAU KHOANG THUOC TINH KHONG XEP CHONG (dung ban goc Linux).

Do duoc tren magicscript cua Linux 004, cot 12 "xep chong" va cot 17 "co khai
niem cap":

    6/1/147  Huyen Tinh Khoang Thach   xep chong = 1   co cap = 1
    6/1/200..205  sau khoang thuoc tinh xep chong = 0   co cap = 0
    6/1/3151 Huyen Kim                 xep chong = 1   co cap = 1
    6/1/3152 Huyen Kim Chi Tinh        xep chong = 1   co cap = 1

Bang cua JX1 dat nMaxStack = 50 cho CA SAU khoang. Do la cho lech, va no keo
theo hai chuyen:

  1. Moi vien khoang thuoc tinh mang MOT MA PHEP RIENG o nGeneratorLevel[0].
     Xep chong duoc nghia la hai vien khac ma phep co the nhap lam mot chong -
     mat du lieu. (Da va CanStack de so them ma phep, nhung khong xep chong
     ngay tu dau moi la dung ban goc.)
  2. ore_upgrade doi DUNG BA VIEN khoang. Neu ba vien nam chung mot chong thi
     duong giao vat pham chi thay MOT mon -> cong thuc truot vinh vien.

Doi nMaxStack ve 0 cho dung sau ma 199..204.

RUI RO DI CHUYEN DU LIEU: da quet toan bo cay kich ban may chu, KHONG mot cho
nao dang phat sau khoang nay (grep "6,1,19x" / "6,1,20x" = 0 ket qua thuc).
Nen khong co nguoi choi nao dang giu chong de bi anh huong. Huyen Tinh (146)
thi CO nguoi giu (thuong cong thanh, doi thuong Viem De) nen GIU NGUYEN xep
chong - dung nhu ban goc.
"""
import io
import os
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location(
    "bangtxt", os.path.join(HERE, "..", "viemde", "bangtxt.py"))
bangtxt = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(bangtxt)

TAB = chr(9)
CR = chr(13)
NL = chr(10)

P = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
MA_DOI = [str(x) for x in range(199, 205)]     # sau khoang thuoc tinh


def main():
    doi_that = "--ghi" in sys.argv
    d = io.open(P, "rb").read().decode("latin-1")
    crlf = d.count(CR + NL)
    lf = d.count(NL) - crlf
    kieu = CR + NL if crlf > lf else NL
    dong = d.replace(CR + NL, NL).split(NL)
    b = [x.split(TAB) for x in dong]
    hdr = [c.strip() for c in b[0]]
    if "nMaxStack" not in hdr:
        print("KHONG thay cot nMaxStack; cot hien co: %s" % hdr[:16])
        return 1
    iS = hdr.index("nMaxStack")
    ncot = len(b[0])

    print("SAU KHOANG THUOC TINH KHONG XEP CHONG")
    print("=" * 84)
    print("tep: %s" % P)
    print("cot nMaxStack o vi tri %d, tong %d cot, %d dong, xuong dong %s"
          % (iS, ncot, len(dong), "CRLF" if kieu != NL else "LF"))
    print()

    doi = 0
    for i in range(1, len(b)):
        c = b[i]
        if len(c) <= iS:
            continue
        if c[1].strip() == "6" and c[2].strip() == "1" and c[3].strip() in MA_DOI:
            cu = c[iS].strip()
            if cu == "0":
                print("  BO QUA  6,1,%-5s %-30s da la 0" % (c[3], bangtxt.tcvn2uni(c[0])[:30]))
                continue
            c[iS] = "0"
            doi += 1
            print("  DOI     6,1,%-5s %-30s nMaxStack %s -> 0"
                  % (c[3], bangtxt.tcvn2uni(c[0])[:30], cu))

    # CHOT LUOI: khong dong nao duoc thua cot so voi tieu de
    thua = [i + 1 for i, c in enumerate(b) if len(c) > ncot]
    if thua:
        print("  *** LOI: %d dong thua cot (vd dong %d)" % (len(thua), thua[0]))
        return 1

    print()
    print("=" * 84)
    if doi == 0:
        print("  Khong co gi de doi.")
        return 0
    if not doi_that:
        print("  DIEN TAP - da doi %d dong trong bo nho, chua ghi. Them --ghi de ghi." % doi)
        return 0

    ra = kieu.join(TAB.join(c) for c in b)
    if len(ra.replace(CR + NL, NL).split(NL)) != len(dong):
        print("  *** LOI: so dong doi tu %d thanh %d"
              % (len(dong), len(ra.replace(CR + NL, NL).split(NL))))
        return 1
    # KHONG ghi thang len cay van hanh. magicscript.txt chinh la tep da lam
    # SAP GameServer hom qua (mot dong dinh doi -> lech luoi -> vong nap bo
    # ngang). Xep vao bo dat chung de di cung mot dot, co dien tap va co soat
    # luoi mot lan nua truoc khi len may chu.
    dich = os.path.join(HERE, "ra", "settings", "item", "magicscript.txt")
    d2 = os.path.dirname(dich)
    if not os.path.isdir(d2):
        os.makedirs(d2)
    io.open(dich, "wb").write(ra.encode("latin-1"))
    print("  >> da ghi vao BO DAT: %s" % dich)
    print("     (doi %d dong, %d byte - chua len may chu)" % (doi, len(ra)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
