# -*- coding: utf-8 -*-
"""vI_da_trong_co_he.py - Bo test lo ren: khoang thuoc tinh phai ra DUNG NHU BAN GOC.

CHU GAME CHOT (27/08 chieu):
  "cac vien thuoc tinh an phai co he"
  "hien tai da ban viet add xong lenh bai admin khong co thuoc tinh ma da co pham chat"
  "thuong se add da chua co gi moi lay thuoc tinh moi ep vao"

=> Vien khoang phat ra phai:
     - CO NGU HANH (he) that, moi loai mot he;
     - KHONG co pham chat  (dong hien "Pham chat thuoc tinh: %d" doc
       m_CommonAttrib.nLevel - KItem.cpp:1520 - nen nLevel phai = 0);
     - KHONG co thuoc tinh (nGeneratorLevel = 0; thuoc tinh chi co duoc khi RUT
       tu trang bi xanh qua the Trich lay roi kham sang).

BAN CU SAI O DAU: `AddItemEx( nVer, 0, 0, 6, 1, ptc, lv, 2, 0, ... )` voi lv = 6..9
truyen vao vi tri nLevel => da chua co thuoc tinh nao ma da hien "Pham chat thuoc
tinh: 6", dung nhu chu game noi. Ngu hanh cung bi ghim cung = 2 cho ca sau loai.

Chu ky that (KItemCompound.cpp:1240):
  AddItemEx( nItemVer, nRandSeed, nQuality, nGenre, nDetailType, nParticular,
             nLevel, nSeries, nLuck, nMagLvl1..nMagLvl6 )

NGU HANH: engine JX1 dung series_metal..series_earth = 1..5 (Kim Moc Thuy Hoa Tho).
Sau loai khoang ung voi sau O KHAM, khong phai sau ngu hanh, nen bo test phat
MOI LOAI DU CA 5 HE de thu duoc moi to hop khi kham vao trang bi.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_datrong lan dau).
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\viemde")
from bangtxt import uni2tcvn, tcvn2uni  # noqa: E402

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\test_loren_admin.lua"
HAU_TO = ".truoc_datrong"

CHO_PHEP_HOA = set("\u0102\u00c2\u0110\u00ca\u00d4\u01a0\u01af")


def tcvn(u):
    for ch in u:
        if ord(ch) > 127 and ch.isupper() and ch not in CHO_PHEP_HOA:
            raise SystemExit("!!! LOI TO: chu HOA co dau khong ma hoa duoc TCVN3: %r" % ch)
    t = uni2tcvn(u)
    if tcvn2uni(t) != u:
        raise SystemExit("!!! LOI TO: cham vong tron that bai voi %r" % u)
    return t


NEO = (
    "\tfor i = 1, getn( TBKHOANG_TEST ) do\n"
    "\t\tfor lv = 6, 9 do\n"
    "\t\t\tif( AddItemEx( nVer, 0, 0, 6, 1, TBKHOANG_TEST[i][1], lv, 2, 0, 0 ) > 0 ) then\n"
    "\t\t\t\tnDem = nDem + 1;\n"
    "\t\t\tend\n"
    "\t\tend\n"
    "\tend\n"
)

MOI = (
    "\t-- Sau loai khoang, moi loai DU CA 5 NGU HANH (Kim Moc Thuy Hoa Tho).\n"
    "\t-- Dung ban goc: da phat ra CHUA CO GI - khong thuoc tinh, khong pham chat -\n"
    "\t-- chi mang ngu hanh. Thuoc tinh chi co khi RUT tu trang bi xanh qua the\n"
    "\t-- Trich lay roi kham sang. Vi vay:\n"
    "\t--   nLevel  = 0  (dong \"Pham chat thuoc tinh\" doc chinh o nay)\n"
    "\t--   nSeries = 1..5 ngu hanh that\n"
    "\t--   nLuck   = 0 va sau o nMagLvl = 0 (chua co thuoc tinh nao)\n"
    "\tfor i = 1, getn( TBKHOANG_TEST ) do\n"
    "\t\tfor he = 1, 5 do\n"
    "\t\t\tif( AddItemEx( nVer, 0, 0, 6, 1, TBKHOANG_TEST[i][1], 0, he, 0, 0 ) > 0 ) then\n"
    "\t\t\t\tnDem = nDem + 1;\n"
    "\t\t\tend\n"
    "\t\tend\n"
    "\tend\n"
)

# Dong thong bao cu noi "sau loai khoang cap 6 toi 9" - phai sua theo.
CAU_CU = tcvn(u"G\u1ed3m Huy\u1ec1n Tinh c\u1ea5p 1 t\u1edbi 10 v\u00e0 s\u00e1u lo\u1ea1i "
              u"kho\u00e1ng c\u1ea5p 6 t\u1edbi 9.")
CAU_MOI = tcvn(u"G\u1ed3m Huy\u1ec1n Tinh c\u1ea5p 1 t\u1edbi 10 v\u00e0 s\u00e1u lo\u1ea1i "
               u"kho\u00e1ng tr\u1ed1ng \u0111\u1ee7 5 ng\u0169 h\u00e0nh.")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vI_da_trong_co_he - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)

    if "moi loai DU CA 5 NGU HANH" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    neo = NEO.replace("\n", eol)
    moi_khoi = MOI.replace("\n", eol)
    if raw.count(neo) != 1:
        print("!!! LOI TO: moc neo vong add xuat hien %d lan (can 1)" % raw.count(neo))
        return 1
    moi = raw.replace(neo, moi_khoi, 1)
    print("  M1 vong add khoang: moc neo trung 1 lan")

    if moi.count(CAU_CU) == 1:
        moi = moi.replace(CAU_CU, CAU_MOI, 1)
        print("  M2 cau thong bao: da sua theo")
    else:
        print("  M2 cau thong bao: khong thay (bo qua, khong chan)")

    hi_sau = sum(1 for c in moi if ord(c) > 127)
    print("  byte cao %d -> %d | CRLF %d -> %d" %
          (hi_truoc, hi_sau, raw.count("\r\n"), moi.count("\r\n")))
    try:
        moi.encode("latin-1")
    except UnicodeEncodeError as e:
        print("!!! LOI TO: ky tu ngoai latin-1: %s" % e)
        return 1

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai tu dia KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
