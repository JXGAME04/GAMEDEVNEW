# -*- coding: utf-8 -*-
r"""v25 - CHO DUONG CLIENT CUNG DICH DUOC CHI SO DONG.

Do duoc: khi may chu dong bo mot vat pham moi, client chay
    KProtocolProcess::s2cSyncItem   (m_Nature < 2, ma NATURE_VIOLET = 1)
    -> KItemSet::AddItemSet2 (KItemSet.cpp:213)
    -> KItemGenerator::Gen_Equipment ban 10 THAM SO (khong co nItemNature)
    -> Gen_MagicAttrib(..., nGameVersion)   <- nItemNature lay mac dinh -1
=> chi so dong 1..480 lai roi vao GetCMIT -> NULL -> break -> do tim hien
   TRANG TRON o may khach, du may chu tinh dung.

Hai duong kia (Gen_ExistEquipment va Gen_Equipment co nature) da truyen du.

CACH SUA, chon duong KHONG dung toi giao thuc:
  Them phep nhan dien theo CHINH GIA TRI: cap cua JX1 chi chay 1..MATF_LEVEL
  (=10), nen con so LON HON 10 khong the la cap - chac chan la chi so dong.
  Dieu kien moi:  nItemNature == NATURE_VIOLET  HOAC  gia tri > MATF_LEVEL

  Con lai mot khe 2%: dong 1..10 (deu la MAGIC_ID 126) di qua duong client thi
  van bi hieu la cap 1..10. Khi do no roi ve hanh vi cu (boc ngau nhien trong
  nhom cap do) chu khong mat thuoc tinh. Muon dung tuyet doi thi phai truyen
  nItemNature xuong AddItemSet2 va Gen_Equipment ban 10 tham so - viec do dung
  toi KProtocolProcess nen de chu du an quyet.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemGenerator.cpp"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dem_dong(t):
    crlf = t.count(CR + NL)
    return crlf, t.count(NL) - crlf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


def khoi(*d):
    return "".join(x + NL for x in d)


TIM = khoi(
    TAB * 3 + "if (nItemNature == NATURE_VIOLET)",
)
THAY = khoi(
    TAB * 3 + "// Duong dong bo tu may chu ve client khong mang pham chat xuong",
    TAB * 3 + "// (KItemSet::AddItemSet2 -> Gen_Equipment ban 10 tham so), nen nhan",
    TAB * 3 + "// dien them bang CHINH GIA TRI: cap cua JX1 chi chay 1..MATF_LEVEL,",
    TAB * 3 + "// con so lon hon the chac chan la chi so dong cua bang thuoc tinh.",
    TAB * 3 + "if (nItemNature == NATURE_VIOLET || pnaryMALevel[i] > MATF_LEVEL)",
)


def main():
    print("CHO DUONG CLIENT CUNG DICH DUOC CHI SO DONG")
    print("=" * 74)
    goc = doc(P)
    a = theo_eol(goc, TIM)
    b = theo_eol(goc, THAY)
    if b in goc:
        print("  BO QUA - da co san")
        return 0
    n = goc.count(a)
    if n != 1:
        print("  *** LOI: moc neo xuat hien %d lan" % n)
        return 1
    t = goc.replace(a, b, 1)
    c0, l0 = dem_dong(goc)
    c1, l1 = dem_dong(t)
    if min(c1, l1) > min(c0, l0):
        print("  *** LOI: tang loai xuong dong thieu so")
        return 1
    io.open(P, "wb").write(t.encode("latin-1"))
    print("  OK  (CRLF %d->%d)" % (c0, c1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
