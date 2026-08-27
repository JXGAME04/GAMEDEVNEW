# -*- coding: utf-8 -*-
r"""v24 - HAI SUA CUOI TU CONG PHAN BIEN.

(1) DEM O KHAM NAM TRONG BANG SAI TRUONG.
    KItem.cpp ve dong "Chua kham nam" theo dieu kien
        if (i < m_GeneratorParam.nLuck)
    tuc dem bang MAY MAN cua mon do, khong phai so o that.
    Trang bi do lo ren duc ra mang nLuck CHEP TU MON GOC (equip_compound.lua
    truyen g_nEquipLuck), nen so dong hien ra chang lien quan gi toi so o.
    May man 0 => khong hien o nao; may man 8 => hien 8 dong du chi co 3 o.
    Sua: dem dung cai danh dau o trong, tuc nGeneratorLevel[i] == -1.

(2) GIA TRI VAT PHAM DEM SO LUONG THEO CO bStack.
    CalcItemValueByIndex chi nhan theo chong khi bStack bat. Nhung khi xoa
    nguyen lieu thi compound_header.lua xoa CA CHONG bang GetItemStackCount.
    Neu mot mon co bStack tat ma trong CSDL van con chong > 1 (do du lieu cu)
    thi gia tri tinh 1 vien con xoa het ca chong - thiet cho nguoi choi.
    Sua: dem theo so luong THAT, kep giong het GetItemStackCount
    (KJx2WarInfra.cpp:311-316) de hai ben khong bao gio lech.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"


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


CAP = [
    ("KItem.cpp", "dem o trong bang dau -1",
     khoi(TAB * 4 + "if (i < m_GeneratorParam.nLuck)"),
     khoi(TAB * 4 + "// [LOREN] Truoc day dem bang nLuck (may man cua mon do) - khong",
          TAB * 4 + "// lien quan gi toi so o. Trang bi do lo ren duc ra chep nLuck tu",
          TAB * 4 + "// mon goc. Dem dung cai danh dau o trong: nGeneratorLevel == -1.",
          TAB * 4 + "if (i < MAX_ITEM_MAGICLEVEL && m_GeneratorParam.nGeneratorLevel[i] == -1)")),

    ("KItemCompound.cpp", "gia tri dem so luong that",
     khoi(TAB + "int nSoLuong = 1;",
          TAB + "if (pItem->m_CommonAttrib.bStack)",
          TAB + "{",
          TAB * 2 + "int nMax = pItem->m_CommonAttrib.nMaxStack;",
          TAB * 2 + "if (nMax <= 0)",
          TAB * 3 + "nMax = 1;",
          TAB * 2 + "int nCur = pItem->GetStackNum();",
          TAB * 2 + "nSoLuong = (nCur <= nMax) ? nCur : 1;",
          TAB * 2 + "if (nSoLuong < 1)",
          TAB * 3 + "nSoLuong = 1;",
          TAB + "}"),
     khoi(TAB + "// Kep GIONG HET GetItemStackCount (KJx2WarInfra.cpp:311-316), vi do",
          TAB + "// la con so ma compound_header.lua dung de xoa nguyen lieu. Hai ben",
          TAB + "// lech nhau la nguoi choi mat nhieu hon phan duoc tinh.",
          TAB + "int nSoLuong = pItem->GetStackNum();",
          TAB + "if (nSoLuong < 1)",
          TAB * 2 + "nSoLuong = 1;",
          TAB + "{",
          TAB * 2 + "int nMax = pItem->GetMaxStackNum();",
          TAB * 2 + "if (nMax >= 1 && nSoLuong > nMax)",
          TAB * 3 + "nSoLuong = nMax;",
          TAB + "}")),
]


def main():
    print("DEM O KHAM NAM TRONG + DEM SO LUONG THAT")
    print("=" * 76)
    loi = 0
    for ten, nhan, tim, thay in CAP:
        p = os.path.join(SRC, ten)
        goc = doc(p)
        a = theo_eol(goc, tim)
        b = theo_eol(goc, thay)
        if b in goc:
            print("  BO QUA  %-20s %s" % (ten, nhan))
            continue
        n = goc.count(a)
        if n != 1:
            print("  *** LOI %-20s %s: moc neo %d lan" % (ten, nhan, n))
            loi += 1
            continue
        t = goc.replace(a, b, 1)
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI %-20s tang loai xuong dong thieu so" % ten)
            loi += 1
            continue
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  OK      %-20s %s (CRLF %d->%d)" % (ten, nhan, c0, c1))
    print("=" * 76)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
