# -*- coding: utf-8 -*-
r"""v04 - THEM KHAI NIEM "O KHAM NAM RONG (-1)" VAO BO SINH VAT PHAM.

Ban Linux dung BA ma cho nGeneratorLevel[i]:
      > 0   cap thuoc tinh phep
     == -1  O KHAM NAM RONG (giu khe, chua co thuoc tinh)
     == 0   het mang
JX1 chi co hai ma (>0 va ==0). Thieu ma -1 thi EP DO TIM khong the chay:
`equip_compound.lua` sinh trang bi voi 1..5 o -1, con `equip_enchase.lua` di
tim o -1 de kham vao.

Do duoc chan o dau: KItemGenerator.cpp Gen_MagicAttrib (ban thu hai)
      pnaryMALevel[i] == 0  -> break
      con lai -> GetCMIT(..., pnaryMALevel[i])
      KBasPropTbl.cpp:424   kep ((nLevel - 1) >= 0)  -> -1 tra NULL
      -> `if (!pCMITItem) break;`  -> DANH SACH BI CAT NGANG tai o do.

Hai thay doi, deu CHI THEM:
  1. Xoa sach pnaryMA[] TRUOC vong sinh. Ban goc dua vao vong don duoi cung
     de xoa phan con lai; khi co o -1 thi vong tren khong `break` nua nen vong
     don khong chay -> sMA[] (mang ngan xep CHUA khoi tao o Gen_ExistEquipment)
     se ro rac vao trang bi. Xoa truoc la an toan tuyet doi.
  2. Them nhanh `< 0` = bo qua o nay, di tiep.

Khong dong vao GetCMIT, khong doi chu ky ham nao.

LUU Y KY THUAT: tep KItemGenerator.cpp co chu thich tieng Trung ma GBK. Doc
bang latin-1 (byte-nguyen) va CHI dung moc neo ASCII.
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


def ghi(p, t):
    if not os.path.isfile(p + ".truoc_loren"):
        io.open(p + ".truoc_loren", "wb").write(io.open(p, "rb").read())
    io.open(p, "wb").write(t.encode("latin-1"))


def dem_dong(t):
    crlf = t.count(CR + NL)
    return crlf, t.count(NL) - crlf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


def khoi(*dong):
    """ghep cac dong (da co tab dat san) thanh mot khoi ket thuc bang xuong dong"""
    return "".join(d + NL for d in dong)


# --- moc neo 1: dau vong sinh -----------------------------------------------
TIM1 = khoi(
    TAB + TAB + "for (i = 0; i < MAX_ITEM_MAGICLEVEL; i++)",
    TAB + TAB + "{",
)

THAY1 = khoi(
    TAB + TAB + "// [LOREN] Xoa sach TRUOC khi sinh.",
    TAB + TAB + "// Ban goc dua vao vong don o cuoi ham de xoa phan con lai, nhung",
    TAB + TAB + "// vong do chi chay khi vong duoi day `break` som. Khi co o kham",
    TAB + TAB + "// nam rong (-1) thi vong duoi day di het MAX_ITEM_MAGICLEVEL,",
    TAB + TAB + "// vong don thanh lenh rong -> mang sMA[] chua khoi tao cua",
    TAB + TAB + "// Gen_ExistEquipment se ro rac vao trang bi.",
    TAB + TAB + "for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)",
    TAB + TAB + "{",
    TAB + TAB + TAB + "pnaryMA[i].nAttribType = 0;",
    TAB + TAB + TAB + "pnaryMA[i].nValue[0] = 0;",
    TAB + TAB + TAB + "pnaryMA[i].nValue[1] = 0;",
    TAB + TAB + TAB + "pnaryMA[i].nValue[2] = 0;",
    TAB + TAB + "}",
    "",
) + TIM1

# --- moc neo 2: nhanh -1, chen ngay truoc SelectedMagicTable.Clear() ---------
TIM2 = khoi(
    TAB + TAB + TAB + TAB + "break;",
    TAB + TAB + TAB + "SelectedMagicTable.Clear();",
)

THAY2 = khoi(
    TAB + TAB + TAB + TAB + "break;",
    TAB + TAB + TAB + "// [LOREN] ma thu ba cua ban Linux: -1 = O KHAM NAM RONG.",
    TAB + TAB + TAB + "// Giu khe (de arynMagLvl[j] va m_aryMagicAttrib[j] cung",
    TAB + TAB + TAB + "// chi so) nhung khong sinh thuoc tinh. Truoc day -1 roi",
    TAB + TAB + TAB + "// vao GetCMIT -> NULL -> break -> cat ngang danh sach.",
    TAB + TAB + TAB + "if (pnaryMALevel[i] < 0)",
    TAB + TAB + TAB + TAB + "continue;",
    TAB + TAB + TAB + "SelectedMagicTable.Clear();",
)


def main():
    p = os.path.join(SRC, "KItemGenerator.cpp")
    goc = doc(p)
    t = goc
    print("THEM O KHAM NAM RONG (-1) VAO Gen_MagicAttrib")
    print("=" * 74)
    loi = 0

    for nhan, tim, thay in (("xoa-truoc", TIM1, THAY1), ("nhanh -1", TIM2, THAY2)):
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-10s da co san" % nhan)
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-10s moc neo xuat hien %d lan" % (nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        print("  OK      %-10s" % nhan)

    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        ghi(p, t)
        print("  >> da ghi (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))
    print("=" * 74)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
