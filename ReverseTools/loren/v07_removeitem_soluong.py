# -*- coding: utf-8 -*-
r"""v07 - RemoveItemByIndex nhan them so luong (doi thu hai).

Vi sao: ITEM_CalcItemValue cua ban Linux NHAN gia tri theo so luong chong
(do duoc o 0x08153F21-0x08153F6A, da chep vao KItemCompound.cpp). Nghia la mot
chong 20 vien khoang duoc tinh gia tri cua ca 20 vien. Vay khi lo ren an
nguyen lieu thi phai an CA CHONG - neu chi tru mot vien thi nguoi choi tra gia
bang mot vien ma huong gia tri cua ca chong. Do la lo hong kinh te, khong phai
loi hien thi.

Ban JX1 hien dong cung nNum = 1. Them doi thu hai, KHONG doi hanh vi cu:
goi mot doi van tru 1 nhu truoc.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KJx2WarInfra.cpp"


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
    "int LuaRemoveItemByIndex(Lua_State* L)",
    "{",
    TAB + "int nOk = 0;",
    TAB + "int nPlayerIndex = GetPlayerIndex(L);",
    TAB + "if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))",
    TAB + "{",
    TAB + TAB + "int nIdx = (int)Lua_ValueToNumber(L, 1);",
    TAB + TAB + "if (nIdx > 0 && nIdx < MAX_ITEM &&",
    TAB + TAB + TAB + "Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nIdx, 1))",
    TAB + TAB + TAB + "nOk = 1;",
    TAB + "}",
)

THAY = khoi(
    "// [LOREN 26/08] Them doi THU HAI = so luong can tru (mac dinh 1).",
    "// He lo ren tinh gia tri nguyen lieu theo CA CHONG nen khi an nguyen lieu",
    "// cung phai an ca chong; compound_header.lua goi",
    "//     RemoveItemByIndex( idx, GetItemStackCount(idx) )",
    "// Goi mot doi van giu nguyen hanh vi cu (tru 1 don vi).",
    "int LuaRemoveItemByIndex(Lua_State* L)",
    "{",
    TAB + "int nOk = 0;",
    TAB + "int nPlayerIndex = GetPlayerIndex(L);",
    TAB + "if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))",
    TAB + "{",
    TAB + TAB + "int nIdx = (int)Lua_ValueToNumber(L, 1);",
    TAB + TAB + "int nNum = 1;",
    TAB + TAB + "if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))",
    TAB + TAB + "{",
    TAB + TAB + TAB + "nNum = (int)Lua_ValueToNumber(L, 2);",
    TAB + TAB + TAB + "if (nNum < 1)",
    TAB + TAB + TAB + TAB + "nNum = 1;",
    TAB + TAB + "}",
    TAB + TAB + "if (nIdx > 0 && nIdx < MAX_ITEM &&",
    TAB + TAB + TAB + "Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nIdx, nNum))",
    TAB + TAB + TAB + "nOk = 1;",
    TAB + "}",
)


def main():
    t = doc(P)
    a = theo_eol(t, TIM)
    b = theo_eol(t, THAY)
    print("RemoveItemByIndex nhan them so luong")
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
    if not os.path.isfile(P + ".truoc_loren"):
        io.open(P + ".truoc_loren", "wb").write(io.open(P, "rb").read())
    io.open(P, "wb").write(t2.encode("latin-1"))
    print("  OK  (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
