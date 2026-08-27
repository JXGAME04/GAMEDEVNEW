# -*- coding: utf-8 -*-
r"""v22 - HAI SUA C++ TU PHAN BIEN VONG 4.

(1) NGU HANH BI BOC NGAU NHIEN ROI GHI DE VINH VIEN.
    Gen_MagicScript:  if (nSeries < series_metal) SetSeries(GetRandomNumber(0,4));
    Kho vat pham hien tai deu co iseries = -1 (do ma cu ep). Truoc day khong sao
    vi ngay duoi co SetSeries(-1) ep lai. Ban va cua toi bo phep ep do cho 15 ma
    lo ren => so ngau nhien SONG SOT, roi KPlayerDBFuns ghi xuong CSDL.
    Moi lan dang nhap boc mot so khac, va lan nao cung ghi de. KHONG HOI DUOC.

    Sua: voi nguyen lieu lo ren, nSeries am thi GIU NGUYEN -1, khong boc ngau
    nhien. Viec gan ngu hanh that de cho buoc di tru CSDL lam sau, co kiem soat.

(2) MA PHEP CUA KHOANG DAT SAI TRUONG.
    JX1 mo ta ma phep cua khoang 199..204 qua m_GeneratorParam.nLuck:
        KItem.cpp:2372   if (m_GeneratorParam.nLuck) { ... MAGICATTRIB_LVINDEX_FILE ... }
    con kich ban lo ren (ban Linux) doc GetItemParam(idx, 1) = nGeneratorLevel[0].
    Hai ben canh hai cua khac nhau:
      - vien do lo ren duc ra KHONG hien dong "Thuoc tinh:" nao
      - hai chot chong mat do (CanStack / FindSameItemToSort) so nGeneratorLevel[0]
        nen voi do CU (ma phep nam o nLuck) chung canh nham cua

    Sua: LuaCmp_AddItemEx ghi ma phep vao CA HAI truong cho khoang 199..204, va
    hai chot xep chong so them nLuck. Nhat quan ca hai phia, khong phai chon ben.
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


CAP = []

# --- (1) KItemGenerator.cpp: khong boc ngau nhien ngu hanh cho nguyen lieu ----
CAP.append((
    "KItemGenerator.cpp", "ngu hanh khong boc ngau nhien",
    khoi(
        TAB + "if (nSeries < series_metal)",
        TAB * 2 + "pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));",
        TAB + "else",
        TAB * 2 + "pItem->SetSeries(nSeries);",
    ),
    khoi(
        TAB + "// [LOREN] Nguyen lieu lo ren: ngu hanh am nghia la CHUA CO, phai giu",
        TAB + "// nguyen. Boc ngau nhien o day thi so do se bi ghi xuong CSDL",
        TAB + "// (KPlayerDBFuns) va khong hoi lai duoc; moi lan dang nhap lai boc",
        TAB + "// mot so khac. Viec gan ngu hanh that de buoc di tru CSDL lam.",
        TAB + "if (nSeries < series_metal)",
        TAB + "{",
        TAB * 2 + "if (sLaNguyenLieuLoRen(nDetailType, nParticularType))",
        TAB * 3 + "pItem->SetSeries(nSeries);",
        TAB * 2 + "else",
        TAB * 3 + "pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));",
        TAB + "}",
        TAB + "else",
        TAB * 2 + "pItem->SetSeries(nSeries);",
    ),
))

# --- (2a) KItemCompound.cpp: ghi ma phep vao ca nLuck ------------------------
CAP.append((
    "KItemCompound.cpp", "ma phep ghi ca vao nLuck",
    khoi(
        TAB + "memcpy(pNew->m_GeneratorParam.nGeneratorLevel, nMagicLevel,",
        TAB * 2 + "   sizeof(int) * MAX_ITEM_MAGICLEVEL);",
    ),
    khoi(
        TAB + "memcpy(pNew->m_GeneratorParam.nGeneratorLevel, nMagicLevel,",
        TAB * 2 + "   sizeof(int) * MAX_ITEM_MAGICLEVEL);",
        "",
        TAB + "// [LOREN] Khoang thuoc tinh: JX1 va ban Linux canh HAI CUA khac nhau",
        TAB + "// cho cung mot thu. JX1 doc ma phep o m_GeneratorParam.nLuck",
        TAB + "// (KItem.cpp:2372, de ve dong \"Thuoc tinh:\" trong chu giai), con kich",
        TAB + "// ban lo ren doc GetItemParam(idx,1) = nGeneratorLevel[0].",
        TAB + "// Ghi vao ca hai thi ca hai phia deu dung, khoi phai chon ben.",
        TAB + "if (nGenre == item_magicscript && nDetailType == 1 &&",
        TAB * 2 + "nParticular >= 199 && nParticular <= 204 && nMagicLevel[0] > 0)",
        TAB + "{",
        TAB * 2 + "pNew->m_GeneratorParam.nLuck = nMagicLevel[0];",
        TAB + "}",
    ),
))

# --- (2b) KItem.cpp CanStack: so them nLuck ---------------------------------
CAP.append((
    "KItem.cpp", "CanStack so them nLuck",
    khoi(
        TAB * 3 + "&& m_GeneratorParam.nGeneratorLevel[0] == Item[nOldIdx].m_GeneratorParam.nGeneratorLevel[0]",
    ),
    khoi(
        TAB * 3 + "&& m_GeneratorParam.nGeneratorLevel[0] == Item[nOldIdx].m_GeneratorParam.nGeneratorLevel[0]",
        TAB * 3 + "// nLuck la cua ma JX1 dung cho ma phep cua khoang (KItem.cpp:2372)",
        TAB * 3 + "&& m_GeneratorParam.nLuck == Item[nOldIdx].m_GeneratorParam.nLuck",
    ),
))

# --- (2c) KInventory.cpp FindSameItemToSort: so them nLuck -------------------
CAP.append((
    "KInventory.cpp", "FindSameItemToSort so them nLuck",
    khoi(
        TAB * 3 + "&& Item[*pArray].m_GeneratorParam.nGeneratorLevel[0] == Item[nIdx].m_GeneratorParam.nGeneratorLevel[0]",
    ),
    khoi(
        TAB * 3 + "&& Item[*pArray].m_GeneratorParam.nGeneratorLevel[0] == Item[nIdx].m_GeneratorParam.nGeneratorLevel[0]",
        TAB * 3 + "// nLuck la cua ma JX1 dung cho ma phep cua khoang (KItem.cpp:2372)",
        TAB * 3 + "&& Item[*pArray].m_GeneratorParam.nLuck == Item[nIdx].m_GeneratorParam.nLuck",
    ),
))


def main():
    print("NGU HANH KHONG BOC NGAU NHIEN + MA PHEP DAT DUNG CUA")
    print("=" * 78)
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
            print("  *** LOI %-20s %s: tang loai xuong dong thieu so" % (ten, nhan))
            loi += 1
            continue
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  OK      %-20s %s (CRLF %d->%d)" % (ten, nhan, c0, c1))
    print("=" * 78)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
