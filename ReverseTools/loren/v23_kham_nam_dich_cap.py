# -*- coding: utf-8 -*-
r"""v23 - TANG DICH CAP THUOC TINH CHO KHAM NAM (manh cuoi).

VAN DE (phan bien vong 4 muc 2, da tu do lai):
  Ban Linux: arynMagLvl[i] la CHI SO DONG trong magicattriblevel.txt (480 dong).
             Moi dong la mot cap (MAGIC_ID, LEVEL) kem ba dai gia tri MAG_P1..P3.
             magicattriblevel_index.txt chia 48 nhom: 1-10, 11-20, ... 471-480.
  JX1:       nGeneratorLevel[i] la CAP 1..10, dua vao GetCMIT(tienTo, loai,
             nguHanh, cap) roi BOC NGAU NHIEN mot thuoc tinh trong nhom do.
             KBasPropTbl.h:173  MATF_LEVEL 10  -> ngoai 1..10 tra NULL
             KItemGenerator.cpp -> gap NULL thi break -> CAT SACH thuoc tinh.
  => 47/48 nhom vuot bien. Kham nam khong the chay.

CACH LAM:
  Chi so dong DA DAC TA DU (biet chinh xac thuoc tinh nao, cap nao, gia tri bao
  nhieu) - con day du hon cai JX1 can. Nen chi can DOC BANG roi ghi thang thuoc
  tinh, khong qua GetCMIT nua.

  Phan biet bang PHAM CHAT, khong bang gia tri: chi trang bi NATURE_VIOLET (do
  tim, tuc quality 2 cua ban Linux) moi hieu chi so dong. Moi trang bi khac giu
  nguyen duong cu 1..10. Nho vay dong 1..10 (MAGIC_ID 126) khong bi lan voi cap
  1..10 cua JX1.

  KHONG dung nGeneratorLevel[8..15] (nhanh ghi thang cua JX1) - o do phien
  "cuong hoa 4 the" dang nham toi, va dung no thi mat luon dau -1 danh o rong.
  Chi so dong van nam nguyen o nGeneratorLevel[0..5] nhu ban Linux ghi, nen
  equip_enchase.lua doc lai bang GetItemAllParams van thay dung.

  Ham dich dat trong KItemGenerator.cpp (bien dich cho CA may chu lan client) de
  hai ben sinh ra cung mot ket qua tu cung mot hat. Keo theo: bang
  magicattriblevel.txt phai co o CA hai ben.
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

# --- 1. Ham dich, dat ngay truoc ban Gen_MagicAttrib thu hai -----------------
CAP.append(("KItemGenerator.cpp", "them ham dich chi so dong",
    khoi("BOOL KItemGenerator::Gen_MagicAttrib(",
         TAB + "int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,",
         TAB + "KItemNormalAttrib* pnaryMA, int nGameVersion",
         ")"),
    khoi(
        "// [LOREN] Dich CHI SO DONG cua bang magicattriblevel.txt thanh mot thuoc",
        "// tinh cu the. Ban Linux ghi chi so dong vao arynMagLvl, ban JX1 thi cho",
        "// rang do la cap 1..10 - hai quy uoc khac han nhau.",
        "// Bang nam o  \\settings\\item\\%03d\\magicattriblevel.txt, cot:",
        "//   0 NAME  1 PREFIX  2 SERIES  3 LEVEL  4 MAGIC_ID",
        "//   5 MAG_P1_MIN  6 MAG_P1_MAX  7 MAG_P2_MIN  8 MAG_P2_MAX",
        "//   9 MAG_P3_MIN 10 MAG_P3_MAX 11 BASE_VALUE",
        "// Nap mot lan cho moi phien ban roi giu lai.",
        "#define LOREN_MAX_ITEM_VER 8",
        "static KTabFile   s_LoRenMagTab[LOREN_MAX_ITEM_VER];",
        "static BOOL       s_LoRenMagTabDaNap[LOREN_MAX_ITEM_VER] = { FALSE };",
        "",
        "static KTabFile* sLoRenBangThuocTinh(int nVersion)",
        "{",
        TAB + "if (nVersion < 0 || nVersion >= LOREN_MAX_ITEM_VER)",
        TAB * 2 + "return NULL;",
        TAB + "if (!s_LoRenMagTabDaNap[nVersion])",
        TAB + "{",
        TAB * 2 + "char szPath[MAX_PATH];",
        TAB * 2 + "_snprintf(szPath, sizeof(szPath) - 1,",
        TAB * 3 + "  \"\\\\settings\\\\item\\\\%03d\\\\magicattriblevel.txt\", nVersion);",
        TAB * 2 + "szPath[sizeof(szPath) - 1] = 0;",
        TAB * 2 + "s_LoRenMagTab[nVersion].Load(szPath);",
        TAB * 2 + "s_LoRenMagTabDaNap[nVersion] = TRUE;",
        TAB + "}",
        TAB + "if (s_LoRenMagTab[nVersion].GetHeight() < 2)",
        TAB * 2 + "return NULL;",
        TAB + "return &s_LoRenMagTab[nVersion];",
        "}",
        "",
        "// Tra ve TRUE neu dich duoc. nRow dem tu 1 (dong tieu de la dong 1 cua",
        "// KTabFile nen dong du lieu thu nRow nam o nRow + 1).",
        "static BOOL sLoRenDichThuocTinh(int nVersion, int nRow, KItemNormalAttrib* pOut)",
        "{",
        TAB + "if (!pOut || nRow <= 0)",
        TAB * 2 + "return FALSE;",
        TAB + "KTabFile* pTab = sLoRenBangThuocTinh(nVersion);",
        TAB + "if (!pTab)",
        TAB * 2 + "return FALSE;",
        TAB + "if (nRow + 1 > pTab->GetHeight())",
        TAB * 2 + "return FALSE;",
        "",
        TAB + "int nMagicId = 0;",
        TAB + "pTab->GetInteger(nRow + 1, (LPSTR)\"MAGIC_ID\", 0, &nMagicId);",
        TAB + "if (nMagicId <= 0)",
        TAB * 2 + "return FALSE;",
        "",
        TAB + "const char* szMin[3] = { \"MAG_P1_MIN\", \"MAG_P2_MIN\", \"MAG_P3_MIN\" };",
        TAB + "const char* szMax[3] = { \"MAG_P1_MAX\", \"MAG_P2_MAX\", \"MAG_P3_MAX\" };",
        TAB + "pOut->nAttribType = nMagicId;",
        TAB + "for (int k = 0; k < 3; k++)",
        TAB + "{",
        TAB * 2 + "int nMin = 0, nMax = 0;",
        TAB * 2 + "pTab->GetInteger(nRow + 1, (LPSTR)szMin[k], 0, &nMin);",
        TAB * 2 + "pTab->GetInteger(nRow + 1, (LPSTR)szMax[k], 0, &nMax);",
        TAB * 2 + "// dai am (vd -1,-1) nghia la o nay khong dung - giu nguyen so do",
        TAB * 2 + "if (nMin < 0 || nMax < nMin)",
        TAB * 3 + "pOut->nValue[k] = nMin;",
        TAB * 2 + "else if (nMax == nMin)",
        TAB * 3 + "pOut->nValue[k] = nMin;",
        TAB * 2 + "else",
        TAB * 3 + "pOut->nValue[k] = GetRandomNumber(nMin, nMax);",
        TAB + "}",
        TAB + "return TRUE;",
        "}",
        "",
        "BOOL KItemGenerator::Gen_MagicAttrib(",
        TAB + "int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,",
        TAB + "KItemNormalAttrib* pnaryMA, int nGameVersion, int nItemNature",
        ")",
    )))

# --- 2. Header: them tham so co mac dinh ------------------------------------
CAP.append(("KItemGenerator.h", "chu ky them nItemNature",
    khoi(TAB + "BOOL Gen_MagicAttrib(int, const int*, int, int, KItemNormalAttrib*, int nVersion);"),
    khoi(TAB + "// [LOREN] nItemNature cuoi: NATURE_VIOLET thi hieu nGeneratorLevel la",
         TAB + "// CHI SO DONG cua magicattriblevel.txt (quy uoc ban Linux), khong phai",
         TAB + "// cap 1..10. Mac dinh -1 = giu nguyen duong cu.",
         TAB + "BOOL Gen_MagicAttrib(int, const int*, int, int, KItemNormalAttrib*, int nVersion, int nItemNature = -1);")))

# --- 3. Nhanh dich trong vong sinh ------------------------------------------
CAP.append(("KItemGenerator.cpp", "nhanh dich trong vong sinh",
    khoi(TAB * 3 + "if (pnaryMALevel[i] < 0)",
         TAB * 4 + "continue;",
         TAB * 3 + "SelectedMagicTable.Clear();"),
    khoi(TAB * 3 + "if (pnaryMALevel[i] < 0)",
         TAB * 4 + "continue;",
         TAB * 3 + "// [LOREN] Trang bi tim: con so la CHI SO DONG cua bang thuoc tinh,",
         TAB * 3 + "// da dac ta du ca thuoc tinh lan cap lan dai gia tri. Ghi thang,",
         TAB * 3 + "// khong qua GetCMIT (GetCMIT chi nhan cap 1..10 nen se tra NULL).",
         TAB * 3 + "if (nItemNature == NATURE_VIOLET)",
         TAB * 3 + "{",
         TAB * 4 + "if (i < MAX_ITEM_MAGICATTRIB &&",
         TAB * 5 + "sLoRenDichThuocTinh(nGameVersion, pnaryMALevel[i], &pnaryMA[i]))",
         TAB * 5 + "continue;",
         TAB * 4 + "// khong tra duoc thi bo qua o nay, dung cat ca danh sach",
         TAB * 4 + "continue;",
         TAB * 3 + "}",
         TAB * 3 + "SelectedMagicTable.Clear();")))

# --- 4. Hai noi goi co pham chat -------------------------------------------
CAP.append(("KItemGenerator.cpp", "truyen nature o Gen_ExistEquipment/Gen_Equipment",
    None, None))


def va_chuoi(p, tim, thay, nhan, ten):
    goc = doc(p)
    a = theo_eol(goc, tim)
    b = theo_eol(goc, thay)
    if b in goc:
        print("  BO QUA  %-20s %s" % (ten, nhan))
        return 0, goc
    n = goc.count(a)
    if n != 1:
        print("  *** LOI %-20s %s: moc neo %d lan" % (ten, nhan, n))
        return 1, goc
    return 0, goc.replace(a, b, 1)


def main():
    print("TANG DICH CAP THUOC TINH CHO KHAM NAM")
    print("=" * 80)
    loi = 0

    # gom cac ban va cua KItemGenerator.cpp lai lam mot lan ghi
    p = os.path.join(SRC, "KItemGenerator.cpp")
    t = doc(p)
    goc = t
    for ten, nhan, tim, thay in CAP:
        if ten != "KItemGenerator.cpp" or tim is None:
            continue
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-20s %s" % (ten, nhan))
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-20s %s: moc neo %d lan" % (ten, nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        print("  OK      %-20s %s" % (ten, nhan))

    # hai noi goi: them tham so nature
    for cu, moi, nhan in (
        ("bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);",
         "bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion, nItemNature);",
         "noi goi co nature"),
    ):
        # chi doi HAI noi goi nam trong ham co nItemNature (dong ~1331 va ~1636)
        vt = []
        i = 0
        while True:
            j = t.find(cu, i)
            if j < 0:
                break
            vt.append(j)
            i = j + len(cu)
        if len(vt) == 0:
            print("  BO QUA  KItemGenerator.cpp %s: da doi" % nhan)
            continue
        # bo noi goi dau tien (Gen_Equipment khong co nature)
        doi = 0
        for j in reversed(vt[1:]):
            t = t[:j] + moi + t[j + len(cu):]
            doi += 1
        print("  OK      KItemGenerator.cpp %s (%d/%d noi goi)" % (nhan, doi, len(vt)))

    if loi == 0 and t != goc:
        c0, l0 = dem_dong(goc)
        c1, l1 = dem_dong(t)
        if min(c1, l1) > min(c0, l0):
            print("  *** LOI: tang loai xuong dong thieu so")
            return 1
        io.open(p, "wb").write(t.encode("latin-1"))
        print("  >> ghi KItemGenerator.cpp (CRLF %d->%d)" % (c0, c1))

    # header
    for ten, nhan, tim, thay in CAP:
        if ten != "KItemGenerator.h":
            continue
        p2 = os.path.join(SRC, ten)
        l, t2 = va_chuoi(p2, tim, thay, nhan, ten)
        loi += l
        if l == 0 and t2 != doc(p2):
            io.open(p2, "wb").write(t2.encode("latin-1"))
            print("  OK      %-20s %s" % (ten, nhan))

    print("=" * 80)
    print("  %s" % ("XONG" if loi == 0 else "CO %d MUC LOI" % loi))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
