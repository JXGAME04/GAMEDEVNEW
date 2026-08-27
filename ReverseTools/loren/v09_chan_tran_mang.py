# -*- coding: utf-8 -*-
r"""v09 - CHAN TRAN MANG trong Gen_MagicAttrib.

Vong sinh chay toi MAX_ITEM_MAGICLEVEL (16) nhung ghi vao pnaryMA[i], ma
pnaryMA chi co MAX_ITEM_MAGICATTRIB (8) phan tu. Truoc day khong no vi vong
luon `break` som (o thu 7 tro di von la 0). Nay them nhanh bo qua o rong (-1)
nen vong co the di xa hon => phai chan cung.

Day la sua LOI CO SAN, khong doi hanh vi hop le nao: khong the co qua 8 thuoc
tinh phep tren mot mon (chinh mang dich chi co 8 o).
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
    TAB + TAB + TAB + "// [LOREN] ma thu ba cua ban Linux: -1 = O KHAM NAM RONG.",
)

THAY = khoi(
    TAB + TAB + TAB + "// [LOREN] chan cung: pnaryMA[] chi co MAX_ITEM_MAGICATTRIB",
    TAB + TAB + TAB + "// o, trong khi vong nay chay toi MAX_ITEM_MAGICLEVEL. Truoc",
    TAB + TAB + TAB + "// day khong no vi vong luon break som; nay co nhanh bo qua",
    TAB + TAB + TAB + "// o rong nen phai chan.",
    TAB + TAB + TAB + "if (i >= MAX_ITEM_MAGICATTRIB)",
    TAB + TAB + TAB + TAB + "break;",
    TAB + TAB + TAB + "// [LOREN] ma thu ba cua ban Linux: -1 = O KHAM NAM RONG.",
)


def main():
    t = doc(P)
    a = theo_eol(t, TIM)
    b = theo_eol(t, THAY)
    print("CHAN TRAN MANG trong Gen_MagicAttrib")
    print("=" * 70)
    if b in t:
        print("  BO QUA - da co san")
        return 0
    n = t.count(a)
    if n != 1:
        print("  *** LOI: moc neo xuat hien %d lan" % n)
        return 1
    c0, l0 = dem_dong(t)
    t2 = t.replace(a, b, 1)
    c1, l1 = dem_dong(t2)
    if min(c1, l1) > min(c0, l0):
        print("  *** LOI: tang loai xuong dong thieu so")
        return 1
    io.open(P, "wb").write(t2.encode("latin-1"))
    print("  OK  (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
