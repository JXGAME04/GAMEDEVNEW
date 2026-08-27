# -*- coding: utf-8 -*-
r"""v17 - GIU THAM SO SINH KHI NAP VAT PHAM TU CSDL.

Loi do duoc (phan bien vong 2, muc C1):

  KPlayerDBFuns.cpp:572-578  phuc hoi tu CSDL:
        nGeneratorLevel[]  <- pItemData->iparam
        nVersion           <- pItemData->iequipversion
  KPlayerDBFuns.cpp:694     roi moi goi Gen_MagicScript, ma ham nay mo dau bang
        ZeroMemory(&pItem->m_GeneratorParam, ...)
  => XOA SACH nhung gia tri vua phuc hoi.

Hau qua that: khoang thuoc tinh cua he lo ren MAT MA PHEP moi lan dang nhap
lai. atlas.lua doi chieu nguyen lieu bang GetItemParam(idx,1) = nGeneratorLevel[0],
nen sau mot lan thoat game la moi cong thuc Do pho deu truot. Va vi the, va
rieng LuaCmp_AddItemEx la VO NGHIA - do rèn ra dung nhung song khong qua mot
lan dang nhap.

Sua: dat lai ba gia tri NGAY SAU khi bo sinh chay xong. Chi trong nhanh
item_magicscript va item_task (hai nhanh co ZeroMemory), lay tu chinh ban ghi
CSDL nen khong bia them gi.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerDBFuns.cpp"


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
    TAB * 2 + "case item_magicscript:",
    TAB * 3 + "{",
    TAB * 4 + "bGetEquiptResult = ItemGen.Gen_MagicScript(NewItem.m_CommonAttrib.nDetailType, NewItem.m_CommonAttrib.nParticularType, ",
    TAB * 5 + "&NewItem,",
    TAB * 5 + "NewItem.m_CommonAttrib.nLevel,",
    TAB * 5 + "NewItem.m_CommonAttrib.nSeries,",
    TAB * 5 + "NewItem.m_GeneratorParam.nLuck, NewItem.m_CommonAttrib.nStackNum);",
    TAB * 3 + "}",
    TAB * 3 + "break;",
)

THAY = khoi(
    TAB * 2 + "case item_magicscript:",
    TAB * 3 + "{",
    TAB * 4 + "// [LOREN] Gen_MagicScript mo dau bang ZeroMemory ca khoi",
    TAB * 4 + "// m_GeneratorParam, xoa sach nhung gia tri vua phuc hoi tu CSDL",
    TAB * 4 + "// o tren (:572-578). Giu lai truoc, dat lai sau.",
    TAB * 4 + "// Khong lam vay thi khoang thuoc tinh cua he lo ren MAT MA PHEP",
    TAB * 4 + "// moi lan dang nhap, va moi cong thuc Do pho deu truot.",
    TAB * 4 + "int anGiuLevel[MAX_ITEM_MAGICLEVEL];",
    TAB * 4 + "memcpy(anGiuLevel, NewItem.m_GeneratorParam.nGeneratorLevel, sizeof(anGiuLevel));",
    TAB * 4 + "int nGiuVersion = NewItem.m_GeneratorParam.nVersion;",
    TAB * 4 + "UINT uGiuSeed = NewItem.m_GeneratorParam.uRandomSeed;",
    TAB * 4 + "int nGiuLuck = NewItem.m_GeneratorParam.nLuck;",
    "",
    TAB * 4 + "bGetEquiptResult = ItemGen.Gen_MagicScript(NewItem.m_CommonAttrib.nDetailType, NewItem.m_CommonAttrib.nParticularType, ",
    TAB * 5 + "&NewItem,",
    TAB * 5 + "NewItem.m_CommonAttrib.nLevel,",
    TAB * 5 + "NewItem.m_CommonAttrib.nSeries,",
    TAB * 5 + "nGiuLuck, NewItem.m_CommonAttrib.nStackNum);",
    "",
    TAB * 4 + "memcpy(NewItem.m_GeneratorParam.nGeneratorLevel, anGiuLevel, sizeof(anGiuLevel));",
    TAB * 4 + "NewItem.m_GeneratorParam.nVersion = nGiuVersion;",
    TAB * 4 + "NewItem.m_GeneratorParam.uRandomSeed = uGiuSeed;",
    TAB * 4 + "NewItem.m_GeneratorParam.nLuck = nGiuLuck;",
    TAB * 3 + "}",
    TAB * 3 + "break;",
)


def main():
    goc = doc(P)
    t = goc
    print("GIU THAM SO SINH KHI NAP TU CSDL")
    print("=" * 74)
    a = theo_eol(t, TIM)
    b = theo_eol(t, THAY)
    if b in t:
        print("  BO QUA - da co san")
        return 0
    n = t.count(a)
    if n != 1:
        print("  *** LOI: moc neo xuat hien %d lan" % n)
        return 1
    t = t.replace(a, b, 1)
    c0, l0 = dem_dong(goc)
    c1, l1 = dem_dong(t)
    if min(c1, l1) > min(c0, l0):
        print("  *** LOI: tang loai xuong dong thieu so")
        return 1
    if not os.path.isfile(P + ".truoc_loren"):
        io.open(P + ".truoc_loren", "wb").write(io.open(P, "rb").read())
    io.open(P, "wb").write(t.encode("latin-1"))
    print("  OK  (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
