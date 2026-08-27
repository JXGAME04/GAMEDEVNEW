# -*- coding: utf-8 -*-
r"""v14 - DANG KY FoundryCompound (cua vao cua he lo ren tu kich ban)."""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
P = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"


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


CAP = [
    ("khai bao ngoai",
     "extern int LuaCmp_ITEM_CalcItemValue(Lua_State* L);" + NL,
     "extern int LuaCmp_ITEM_CalcItemValue(Lua_State* L);" + NL +
     "extern int LuaCmp_FoundryCompound(Lua_State* L);" + NL),
    ("dang ky",
     TAB + TAB + '{"ITEM_CalcItemValue",' + TAB + "LuaCmp_ITEM_CalcItemValue}," + NL,
     TAB + TAB + '{"ITEM_CalcItemValue",' + TAB + "LuaCmp_ITEM_CalcItemValue}," + NL +
     TAB + TAB + "// [LOREN] cua vao cua he lo ren: kich ban NPC gom nguyen lieu" + NL +
     TAB + TAB + "// roi goi ham nay -> Compound() cua kich ban tuong ung." + NL +
     TAB + TAB + '{"FoundryCompound",' + TAB + "LuaCmp_FoundryCompound}," + NL),
]


def main():
    goc = doc(P)
    t = goc
    print("DANG KY FoundryCompound")
    print("=" * 70)
    loi = 0
    for nhan, tim, thay in CAP:
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-16s da co san" % nhan)
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-16s moc neo %d lan" % (nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        print("  OK      %-16s" % nhan)
    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        io.open(P, "wb").write(t.encode("latin-1"))
        print("  >> da ghi (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    print("=" * 70)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
