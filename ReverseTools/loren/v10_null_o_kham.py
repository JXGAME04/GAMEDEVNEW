# -*- coding: utf-8 -*-
r"""v10 - VA HAI LO NULL do o kham nam rong sinh ra (phan bien muc C-7).

Sau khi them nhanh "o rong (-1) thi bo qua", mang pMagicAttrTable[] co the co
KHE NULL o giua. Hai vong duoi day khong luong truoc chuyen do:

  1. Vong kiem TRUNG THUOC TINH (KItemGenerator.cpp ~:815)
         for (k = 0; k < i; k++)
             _ASSERT(pMagicAttrTable[k]);
             if ((pMagicAttrTable[k]->...nPropKind) == ...)   <-- DEREF NULL
     Phai bo qua khe NULL. Vi bo qua bang `continue` se lam bien `k` mat nghia
     "tim thay o vi tri k", nen doi sang co bTrung.

  2. Vong DON CO m_nUseFlag (~:878)
         if (!pMagicAttrTable[i]) break;      <-- DUNG SOM
     Gap khe NULL la dung, cac thuoc tinh phia sau GIU NGUYEN co "dang dung"
     VINH VIEN => tu do ve sau moi lan sinh trang bi deu bo qua chung. Day la
     loi am i toan may chu, kho lan ra. Phai doi sang bo qua khe NULL.

Hien chua co duong du lieu nao sinh ra "-1 roi moi toi so duong", nhung
AddItemEx da dang ky toan cuc nen mot kich ban tuong lai viet
    AddItemEx(..., -1, 3, ...)
la cham vao ca hai lo. Va truoc, 2 cho, re.
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


# --- lo 1: vong kiem trung thuoc tinh -------------------------------------
TIM1 = khoi(
    TAB * 4 + "int k = 0;",
    TAB * 4 + "for (k = 0; k < i; k++)",
    TAB * 4 + "{",
    TAB * 5 + "_ASSERT(pMagicAttrTable[k]);",
    "",
    TAB * 5 + "if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))",
    TAB * 6 + "break;",
    TAB * 4 + "}",
)

THAY1 = khoi(
    TAB * 4 + "// [LOREN] khe NULL = o kham nam rong, bo qua chu khong deref.",
    TAB * 4 + "// Dung co rieng vi `k < i` khong con nghia sau khi bo qua.",
    TAB * 4 + "int k = 0;",
    TAB * 4 + "BOOL bTrungThuocTinh = FALSE;",
    TAB * 4 + "for (k = 0; k < i; k++)",
    TAB * 4 + "{",
    TAB * 5 + "if (!pMagicAttrTable[k])",
    TAB * 6 + "continue;",
    "",
    TAB * 5 + "if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))",
    TAB * 5 + "{",
    TAB * 6 + "bTrungThuocTinh = TRUE;",
    TAB * 6 + "break;",
    TAB * 5 + "}",
    TAB * 4 + "}",
)

TIM2 = khoi(
    TAB * 4 + "if (k < i)",
    TAB * 5 + "continue;",
)

THAY2 = khoi(
    TAB * 4 + "if (bTrungThuocTinh)",
    TAB * 5 + "continue;",
)

# --- lo 2: vong don co m_nUseFlag -----------------------------------------
TIM3 = khoi(
    TAB + "for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)",
    TAB + "{",
    TAB * 2 + "if (!pMagicAttrTable[i])",
    TAB * 3 + "break;",
)

THAY3 = khoi(
    TAB + "for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)",
    TAB + "{",
    TAB * 2 + "// [LOREN] khe NULL = o kham nam rong. Truoc day `break` o day",
    TAB * 2 + "// lam cac thuoc tinh PHIA SAU giu co \"dang dung\" vinh vien,",
    TAB * 2 + "// khien moi lan sinh trang bi ve sau deu bo qua chung.",
    TAB * 2 + "if (!pMagicAttrTable[i])",
    TAB * 3 + "continue;",
)


def main():
    goc = doc(P)
    t = goc
    print("VA HAI LO NULL do o kham nam rong")
    print("=" * 74)
    loi = 0
    for nhan, tim, thay in (
        ("vong kiem trung", TIM1, THAY1),
        ("dieu kien k < i", TIM2, THAY2),
        ("vong don co", TIM3, THAY3),
    ):
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-18s da co san" % nhan)
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-18s moc neo xuat hien %d lan" % (nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        print("  OK      %-18s" % nhan)

    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        io.open(P, "wb").write(t.encode("latin-1"))
        print("  >> da ghi (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    print("=" * 74)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
