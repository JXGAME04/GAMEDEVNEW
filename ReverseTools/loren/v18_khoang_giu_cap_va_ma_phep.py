# -*- coding: utf-8 -*-
r"""v18 - CHO NGUYEN LIEU LO REN GIU DUOC CAP, NGU HANH VA MA PHEP.

GOC CUA LOI (phan bien vong 2, muc C1 - da tu do lai):

  Gen_MagicScript (KItemGenerator.cpp) ep MOI vat pham xep chong ve
        SetSeries(-1)   va   SetLevel(10)
  vi JX1 coi "xep chong duoc" dong nghia "khong co cap, khong co ngu hanh"
  (neu khong thi hai chong khac cap se nhap lam mot va mat du lieu).

  Do bang so tren magicscript.txt cua JX1: ca 9 nguyen lieu lo ren deu co
  nMaxStack khac 0 (Huyen Tinh 50, sau khoang 50, Huyen Kim 60, Chi Tinh 10)
  => tat ca deu bi ep ve cap 10, ngu hanh -1.

  Hau qua do duoc:
   - ore_upgrade / fantasygold_upgrade kiem "nLevel >= 10" -> CHET CUNG
   - equip_enchase so ngu hanh voi -1 -> SAI VINH VIEN
   - xuanjing_compound quay ra "cap 1" nhung vat pham thanh cap 10, ma bang gia
     ore.txt cho cap 10 la 1.849.000.000 -> MAY IN

BAN GOC LINUX KHONG BI: magicscript cua Linux co HAI cot rieng -
     cot 12 "xep chong"      va   cot 17 "co khai niem cap"
  Sau khoang thuoc tinh: xep chong = 0, co cap = 0
  Huyen Tinh / Huyen Kim  : xep chong = 1, co cap = 1   <- JX1 khong dien ta duoc

HAI THAY DOI, deu HEP va CHI THEM:

  1. Gen_MagicScript: bo qua phep ep tren cho DUNG 9 ma nguyen lieu lo ren,
     va khong xoa tham so sinh cua chung (ma phep nam o do).
  2. KItem::CanStack(nOldIdx, Dest): so them CAP va MA PHEP truoc khi cho nhap
     chong. Day la lo ma phep ep tren vua mo ra: hai vien khoang khac cap hoac
     khac ma phep von se nhap lam mot va mat du lieu.
     Them dieu kien chi lam viec NHAP CHONG KHO HON, khong the lam hong do dang
     co: nhung chong da nhap thi van la mot mon.
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


# ---------------------------------------------------------------------------
# 1. KItemGenerator.cpp - ham nhan dien nguyen lieu lo ren + bo qua phep ep
# ---------------------------------------------------------------------------
GEN_TIM = khoi(
    "BOOL KItemGenerator::Gen_MagicScript(IN int nDetailType, IN int nParticularType, IN OUT KItem* pItem, IN int nLevel, IN int nSeries, IN int nLuck, IN int nStackNum)",
    "{",
)

GEN_THAY = khoi(
    "// [LOREN] Nguyen lieu cua he lo ren: chung co CAP va NGU HANH that, va",
    "// khoang thuoc tinh con mang MA PHEP o nGeneratorLevel[0].",
    "// Ban Linux dien ta bang mot cot rieng trong magicscript (cot 17",
    "// \"co khai niem cap\"); bang cua JX1 khong co cot do nen phai liet ke.",
    "//   6/1/146        Huyen Tinh Khoang Thach",
    "//   6/1/199..204   sau khoang thuoc tinh",
    "//   6/1/3769,3770  Huyen Kim va Huyen Kim Chi Tinh",
    "static BOOL sLaNguyenLieuLoRen(int nDetailType, int nParticularType)",
    "{",
    TAB + "if (nDetailType != 1)",
    TAB * 2 + "return FALSE;",
    TAB + "if (nParticularType == 146)",
    TAB * 2 + "return TRUE;",
    TAB + "if (nParticularType >= 199 && nParticularType <= 204)",
    TAB * 2 + "return TRUE;",
    TAB + "if (nParticularType == 3769 || nParticularType == 3770)",
    TAB * 2 + "return TRUE;",
    TAB + "return FALSE;",
    "}",
    "",
    "BOOL KItemGenerator::Gen_MagicScript(IN int nDetailType, IN int nParticularType, IN OUT KItem* pItem, IN int nLevel, IN int nSeries, IN int nLuck, IN int nStackNum)",
    "{",
)

# hai cho ep - moi cho them mot dieu kien
EP1_TIM = khoi(
    TAB + "if(pMagicScript->m_nMaxStack) // x" + chr(0xEE) + "\xac l" + chr(0xFD) + " x" + chr(0xEC) + "p ch" + chr(0xED) + "ng n" + chr(0xEF) + "u l" + chr(0xE0) + " v" + chr(0xEC) + "t ph" + chr(0xE8) + "m x" + chr(0xEC) + "p ch" + chr(0xED) + "ng kh" + chr(0xF4) + "ng ph" + chr(0xE2) + "n bi" + chr(0xEB) + "t ng" + chr(0xFB) + " h" + chr(0xE0) + "nh xep chong",
    TAB * 2 + "pItem->SetSeries(-1);",
)


def va_ep(t):
    """Them dieu kien vao hai dong ep. Moc neo lay bang cach tim CHINH XAC hai
    dong lenh (ASCII), khong dung dong chu thich tieng Viet co dau."""
    so = 0
    for cu, moi in (
        (TAB * 2 + "pItem->SetSeries(-1);",
         TAB * 2 + "if (!sLaNguyenLieuLoRen(nDetailType, nParticularType))" + NL +
         TAB * 3 + "pItem->SetSeries(-1);"),
        (TAB * 2 + "pItem->SetLevel(10);",
         TAB * 2 + "if (!sLaNguyenLieuLoRen(nDetailType, nParticularType))" + NL +
         TAB * 3 + "pItem->SetLevel(10);"),
    ):
        a = theo_eol(t, cu + NL)
        b = theo_eol(t, moi + NL)
        if b in t:
            so += 1
            continue
        if t.count(a) != 1:
            return None, "moc neo %r xuat hien %d lan" % (cu.strip(), t.count(a))
        t = t.replace(a, b, 1)
        so += 1
    return t, so


# ZeroMemory: giu lai tham so sinh cho nguyen lieu lo ren
ZERO_TIM = khoi(
    TAB + "BOOL bEC = FALSE;",
    TAB + "ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));",
    TAB,
    TAB + "const KBASICPROP_MAGICSCRIPT* pMagicScript = NULL;",
)
ZERO_THAY = khoi(
    TAB + "BOOL bEC = FALSE;",
    TAB + "// [LOREN] Ma phep cua khoang nam o nGeneratorLevel[0]; xoa la mat.",
    TAB + "KItemGeneratorParam sGiuParam = pItem->m_GeneratorParam;",
    TAB + "ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));",
    TAB + "if (sLaNguyenLieuLoRen(nDetailType, nParticularType))",
    TAB * 2 + "pItem->m_GeneratorParam = sGiuParam;",
    TAB,
    TAB + "const KBASICPROP_MAGICSCRIPT* pMagicScript = NULL;",
)

# ---------------------------------------------------------------------------
# 2. KItem.cpp - CanStack so them cap va ma phep
# ---------------------------------------------------------------------------
STACK_TIM = khoi(
    TAB * 3 + "&& m_CommonAttrib.nSeries == Item[nOldIdx].GetSeries()",
)
STACK_THAY = khoi(
    TAB * 3 + "&& m_CommonAttrib.nSeries == Item[nOldIdx].GetSeries()",
    TAB * 3 + "// [LOREN] So them CAP va MA PHEP. Nguyen lieu lo ren co cap that",
    TAB * 3 + "// va mang ma phep rieng; thieu hai phep so nay thi hai vien khac",
    TAB * 3 + "// nhau se nhap lam mot chong va MAT du lieu. Them dieu kien chi",
    TAB * 3 + "// lam viec nhap chong kho hon, khong dung toi do da nhap tu truoc.",
    TAB * 3 + "&& m_CommonAttrib.nLevel == Item[nOldIdx].GetLevel()",
    TAB * 3 + "&& m_GeneratorParam.nGeneratorLevel[0] == Item[nOldIdx].m_GeneratorParam.nGeneratorLevel[0]",
)


def va_tep(duong, cap):
    goc = doc(duong)
    t = goc
    ten = os.path.basename(duong)
    loi = 0
    for nhan, tim, thay in cap:
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-22s %s" % (ten, nhan))
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-22s %s: moc neo %d lan" % (ten, nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        print("  OK      %-22s %s" % (ten, nhan))
    return t, goc, loi


def main():
    print("CHO NGUYEN LIEU LO REN GIU CAP, NGU HANH VA MA PHEP")
    print("=" * 80)
    loi = 0

    # --- KItemGenerator.cpp ---
    p = os.path.join(SRC, "KItemGenerator.cpp")
    t, goc, l = va_tep(p, [("them ham nhan dien", GEN_TIM, GEN_THAY),
                           ("giu tham so sinh", ZERO_TIM, ZERO_THAY)])
    loi += l
    if l == 0:
        t2, kq = va_ep(t)
        if t2 is None:
            print("  *** LOI KItemGenerator.cpp hai dong ep: %s" % kq)
            loi += 1
        else:
            t = t2
            print("  OK      %-22s bo qua phep ep (%d cho)" % ("KItemGenerator.cpp", kq))
    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  >> ghi KItemGenerator.cpp (CRLF %d->%d)" % (c0, c1))

    # --- KItem.cpp ---
    p = os.path.join(SRC, "KItem.cpp")
    t, goc, l = va_tep(p, [("CanStack so cap + ma phep", STACK_TIM, STACK_THAY)])
    loi += l
    if l == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  >> ghi KItem.cpp (CRLF %d->%d)" % (c0, c1))

    print("=" * 80)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
