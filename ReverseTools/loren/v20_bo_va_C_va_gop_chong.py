# -*- coding: utf-8 -*-
r"""v20 - BO BAN VA C (KPlayerDBFuns) + VA DUNG CUA GOP CHONG.

Phan bien vong 3 bat duoc hai cho, ca hai deu dung:

1) BAN VA C KHONG KHOANH VUNG.
   No dat lai nGeneratorLevel[] cho MOI vat pham item_magicscript khi nap tu
   CSDL, trong khi nGeneratorLevel[0..5] chinh la o ma SetSpecItemParam dang
   dung cho nhieu tinh nang khac (yeu bai Tin Su dem so lan dung, ngan phieu
   Tien Trang giu SO TIEN, han dung cua mot so vat pham...). Truoc day
   Gen_MagicScript xoa sach moi lan dang nhap, nhung duong LUU thi van ghi
   xuong CSDL - nen CSDL dang chua san so khac 0. Bat chung song lai cung mot
   luc la doi hanh vi cua tinh nang dang chay, va co cho la nerf hoi to vao do
   nguoi choi dang cam.

   => BO HAN ban va C. Khong can no nua: ban va A da giu va dat lai
      m_GeneratorParam ngay TRONG Gen_MagicScript cho dung 9 ma lo ren, ma
      duong nap CSDL cung di qua chinh ham do (KPlayerDBFuns.cpp:694).
      Mot cho, khoanh dung vung, khong dung toi vat pham nao khac.

2) VA NHAM CUA GOP CHONG.
   KItem::CanStack(nOldIdx, Dest) chi duoc goi o 4 cho keo tha bang tay.
   Duong gop chong THAT khi NHAN do la
        KItemList::AddKIL (KItemList.cpp:195)
        -> KInventory::FindSameItemToSort (KInventory.cpp:502)
   Ham nay da so Series va Level (tot), nhung KHONG so nGeneratorLevel[0] -
   tuc MA PHEP cua khoang thuoc tinh. Hai vien cung cap cung ngu hanh nhung
   khac ma phep se nhap lam mot va mat mot ma.

   Hien chua no vi sau khoang 199..204 vua duoc dat nMaxStack = 0 nen dieu
   kien GetStackNum() < GetMaxStackNum() khong bao gio dung. Nhung de nguyen
   la de lai mot cai bay: ai chinh nMaxStack ve khac 0 la mat du lieu ngay.
   Them phep so cho khop voi CanStack da va.
"""
import io
import os
import shutil
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


GOP_TIM = khoi(
    TAB * 3 + "&& Item[*pArray].GetLevel() == Item[nIdx].GetLevel()",
)
GOP_THAY = khoi(
    TAB * 3 + "&& Item[*pArray].GetLevel() == Item[nIdx].GetLevel()",
    TAB * 3 + "// [LOREN] So them MA PHEP. Khoang thuoc tinh cua he lo ren mang",
    TAB * 3 + "// ma phep rieng o nGeneratorLevel[0]; thieu phep so nay thi hai",
    TAB * 3 + "// vien khac ma phep se nhap lam mot chong va MAT mot ma.",
    TAB * 3 + "&& Item[*pArray].m_GeneratorParam.nGeneratorLevel[0] == Item[nIdx].m_GeneratorParam.nGeneratorLevel[0]",
)


def main():
    print("BO BAN VA C + VA DUNG CUA GOP CHONG")
    print("=" * 78)
    loi = 0

    # --- 1. tra KPlayerDBFuns.cpp ve nguyen goc ---
    p = os.path.join(SRC, "KPlayerDBFuns.cpp")
    sao = p + ".truoc_loren"
    if not os.path.isfile(sao):
        print("  BO QUA  KPlayerDBFuns.cpp: khong co ban sao (chua tung va?)")
    elif "[LOREN]" not in doc(p):
        print("  BO QUA  KPlayerDBFuns.cpp: da tra ve nguyen goc roi")
    else:
        goc = io.open(sao, "rb").read()
        hien = io.open(p, "rb").read()
        io.open(p, "wb").write(goc)
        print("  OK      KPlayerDBFuns.cpp tra ve nguyen goc (%d -> %d byte)"
              % (len(hien), len(goc)))

    # --- 2. FindSameItemToSort so them ma phep ---
    p = os.path.join(SRC, "KInventory.cpp")
    goc = doc(p)
    t = goc
    a = theo_eol(t, GOP_TIM)
    b = theo_eol(t, GOP_THAY)
    if b in t:
        print("  BO QUA  KInventory.cpp: da co san")
    else:
        n = t.count(a)
        if n != 1:
            print("  *** LOI KInventory.cpp: moc neo xuat hien %d lan" % n)
            loi += 1
        else:
            t = t.replace(a, b, 1)
            c0, l0 = dem_dong(goc)
            c1, l1 = dem_dong(t)
            if min(c1, l1) > min(c0, l0):
                print("  *** LOI KInventory.cpp: tang loai xuong dong thieu so")
                return 1
            if not os.path.isfile(p + ".truoc_loren"):
                shutil.copy2(p, p + ".truoc_loren")
            io.open(p, "wb").write(t.encode("latin-1"))
            print("  OK      KInventory.cpp (CRLF %d->%d, LF %d->%d)" % (c0, c1, l0, l1))

    print("=" * 78)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
