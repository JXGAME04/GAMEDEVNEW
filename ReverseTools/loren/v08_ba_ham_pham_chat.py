# -*- coding: utf-8 -*-
r"""v08 - DANG KY BA HAM DI KEM GetItemQuality.

Vi sao bat buoc (do duoc trong kich ban may chu dang chay):
  Truoc day GetItemQuality LUON tra 0, nen moi nhanh "pham chat khac 0" la
  nhanh CHET. He lo ren can pham chat that nen phai bat len - va luc do cac
  nhanh do song lai. Chung goi tiep ba ham GetGlodEqIndex /
  GetPlatinaEquipIndex / GetPlatinaLevel, von cung dang la ban vo tra 0.
  De nguyen la HONG bon cho dang chay:
     script\lib\composeex.lua:142                        ghep do
     script\lib\log.lua:39                               ghi nhat ky
     script\missions\yandibaozang\yandiduihuan.lua:135   doi thuong Viem De
     script\event\...\qianqiu_yinglie\head.lua:67

Nen doi ca ba sang ban that trong KItemCompound.cpp.
"""
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
    # (mo ta, chuoi tim, chuoi thay)
    ("khai bao ngoai",
     "extern int LuaCmp_GetItemQuality(Lua_State* L);" + NL,
     "extern int LuaCmp_GetItemQuality(Lua_State* L);" + NL +
     "extern int LuaCmp_GetGlodEqIndex(Lua_State* L);" + NL +
     "extern int LuaCmp_GetPlatinaEquipIndex(Lua_State* L);" + NL +
     "extern int LuaCmp_GetPlatinaLevel(Lua_State* L);" + NL),

    ("dang ky GetGlodEqIndex",
     TAB + TAB + '{"GetGlodEqIndex",' + TAB + "LuaHD3_GetGlodEqIndex}," + NL,
     TAB + TAB + "// [LOREN] ba ham nay phai that cung luc voi GetItemQuality:" + NL +
     TAB + TAB + "// bat pham chat len la cac nhanh goi chung song lai." + NL +
     TAB + TAB + '{"GetGlodEqIndex",' + TAB + "LuaCmp_GetGlodEqIndex}," + NL),

    ("dang ky GetPlatinaEquipIndex",
     TAB + TAB + '{"GetPlatinaEquipIndex",' + TAB + "LuaHD3_GetPlatinaEquipIndex}," + NL,
     TAB + TAB + '{"GetPlatinaEquipIndex",' + TAB + "LuaCmp_GetPlatinaEquipIndex}," + NL),

    ("dang ky GetPlatinaLevel",
     TAB + TAB + '{"GetPlatinaLevel",' + TAB + "LuaHD3_GetPlatinaLevel}," + NL,
     TAB + TAB + '{"GetPlatinaLevel",' + TAB + "LuaCmp_GetPlatinaLevel}," + NL),
]


def main():
    t = doc(P)
    goc = t
    print("DANG KY BA HAM DI KEM GetItemQuality")
    print("=" * 74)
    loi = 0
    for nhan, tim, thay in CAP:
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-26s da co san" % nhan)
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-26s moc neo xuat hien %d lan" % (nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        print("  OK      %-26s" % nhan)

    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        if not os.path.isfile(P + ".truoc_loren"):
            io.open(P + ".truoc_loren", "wb").write(io.open(P, "rb").read())
        io.open(P, "wb").write(t.encode("latin-1"))
        print("  >> da ghi (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    print("=" * 74)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
