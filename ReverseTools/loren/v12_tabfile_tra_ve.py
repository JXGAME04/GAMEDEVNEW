# -*- coding: utf-8 -*-
r"""v12 - VA TabFile_GetRowCount / TabFile_UnLoad tra ve SO GIA TRI SAI.

Ma hien tai:
    Lua_PushNumber(L, 0);
    return  0;          <-- day gia tri len roi bao "khong tra gi"

Lua doc theo SO GIA TRI TRA VE, nen day 0 ma khai bao 0 la ben Lua nhan
**nil**, khong phai so 0. Hau qua that (phan bien muc B-4):

    atlas.lua:  local nRowCount = TabFile_GetRowCount( strFullPath )
                for j = 2, nRowCount do          -->  for j = 2, nil  --> NO

Bang chua nap (thieu tep) la kich ban hong ngay luc nap, bi loai khoi cay.
Doi thanh `return 1` la moi cho goi deu nhan duoc so 0 dang hoang.

Day la loi co san cua du an, khong phai do dot port sinh ra - nhung dot port
la cho dau tien cham vao no.
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


def khoi(*d):
    return "".join(x + NL for x in d)


TIM1 = khoi(
    TAB + "Lua_PushNumber(L, 0);",
    TAB + "return  0;",
    "}",
    "",
    "int LuaTabFile_UnLoad(Lua_State* L)",
)
THAY1 = khoi(
    TAB + "// [LOREN] Truoc day `return 0` khien Lua nhan NIL thay vi so 0:",
    TAB + "// atlas.lua chay `for j = 2, TabFile_GetRowCount(...)` la no ngay",
    TAB + "// khi bang chua nap. Bang chua nap phai tra 0 dang hoang.",
    TAB + "Lua_PushNumber(L, 0);",
    TAB + "return 1;",
    "}",
    "",
    "int LuaTabFile_UnLoad(Lua_State* L)",
)

TIM2 = khoi(
    TAB + "Lua_PushNumber(L, 0);",
    TAB + "return 0;",
    "}",
)
THAY2 = khoi(
    TAB + "Lua_PushNumber(L, 0);",
    TAB + "return 1;",
    "}",
)


def main():
    goc = doc(P)
    t = goc
    print("VA TabFile_GetRowCount / UnLoad tra ve so gia tri")
    print("=" * 76)
    loi = 0

    a = theo_eol(t, TIM1)
    b = theo_eol(t, THAY1)
    if b in t:
        print("  BO QUA  GetRowCount: da co san")
    else:
        n = t.count(a)
        if n != 1:
            print("  *** LOI GetRowCount: moc neo %d lan" % n)
            loi += 1
        else:
            t = t.replace(a, b, 1)
            print("  OK      GetRowCount")

    # UnLoad: chi doi trong pham vi ham do (tim moc neo ngay sau GetRowCount)
    i = t.find(theo_eol(t, "int LuaTabFile_UnLoad(Lua_State* L)"))
    if i < 0:
        print("  *** LOI UnLoad: khong thay ham")
        loi += 1
    else:
        a2 = theo_eol(t, TIM2)
        b2 = theo_eol(t, THAY2)
        j = t.find(a2, i)
        if j < 0:
            print("  BO QUA  UnLoad: da co san hoac khac khuon")
        else:
            t = t[:j] + b2 + t[j + len(a2):]
            print("  OK      UnLoad")

    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        io.open(P, "wb").write(t.encode("latin-1"))
        print("  >> da ghi (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    print("=" * 76)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
