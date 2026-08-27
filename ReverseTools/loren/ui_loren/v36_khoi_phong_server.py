# -*- coding: utf-8 -*-
r"""v36 - HOI SINH 6 PHONG DO LO REN phia may chu (KItemList.h / KItemList.cpp).

6 phong: compone (luyen huyen tinh khoang thach) / comptwo (nang cap huyen tinh)
/ compthree (nang cap khoang thach thuoc tinh) / distill (rut option trang bi)
/ enchase (kham nam trang bi) / forge (che tao trang bi tim).

Khuon chep tu phong pos_tremble dang song trong KItemList.cpp:
  H1  KItemList.h  : 6 mang canh m_TrembleItem (dong ~33)
  H2  KItemList.h  : 6 getter + 18 khai bao Check/Add/Un (canh GetTrembleItem ~148)
  C1  AddKIL ~325  : 6 case dat cho (khuon case pos_tremble)
  C2  AddKIL ~358  : 6 hook goi AddXxxItem (khuon CheckTrembleItem)
  C3  AddKIL ~466  : 6 case anh xa container client UOC_*_ITEM
                     (v = nX truc tiep, KHONG qua bang doi PartTrembleConvert)
  C4  Remove ~624  : 6 case goi UnXxxItem
  C5  Remove ~730  : 6 case anh xa container client
  C6  ExchangeItem ~3284 : 6 case theo khuon tremble
  C7  ExchangeItem ~3492 : 6 case anh xa container client
  C8  ClearAll ~3752     : ZeroMemory 6 mang canh m_TrembleItem
  C9..C16 RecoverItem ~5416..5535 : mo khoi /* */ va sua trong do
          ItemSet.Add( -> ItemSet.AddI( ; Add(nIndex, pos_equiproom -> AddKIL(
          (doi chung case pos_tremble song ngay ben tren)
  C17 sau UnTrembleItem ~5669 : 18 than ham (6 phong x Check/Add/Un).
          CheckXxx dot dau CHO QUA moi vat pham (kiem tra nguyen lieu that su
          do KFoundryResDemand lam khi nguoi choi bam nut) - chi chan chi so
          o khong hop le va o da co do.

Kich thuoc mang (GameDataDef.h:238-270): compoundpart_num=8, outinpart_num=11,
forgepart_num=2. Container UOC_COMPONE_ITEM..UOC_ENCHASE_ITEM co san
(GameDataDef.h:751-756). pos_compone..pos_enchase co san (GameDataDef.h:335-340).

Mac dinh DIEN TAP (khong ghi). Chi ghi khi co --ghi.
Sao luu <tep>.truoc_uiloren truoc khi ghi lan dau.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)
TAB = chr(9)

H = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemList.h"
C = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemList.cpp"


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
# Bang 6 phong: (ten ham, ten mang, kich thuoc, pos, ten container UOC)
# ---------------------------------------------------------------------------
PHONG = [
    ("CompOne",   "m_CompOneItem",   "compoundpart_num", "pos_compone",   "UOC_COMPONE_ITEM"),
    ("CompTwo",   "m_CompTwoItem",   "compoundpart_num", "pos_comptwo",   "UOC_COMPTWO_ITEM"),
    ("CompThree", "m_CompThreeItem", "compoundpart_num", "pos_compthree", "UOC_COMPTHREE_ITEM"),
    ("Distill",   "m_DistillItem",   "outinpart_num",    "pos_distill",   "UOC_DISTILL_ITEM"),
    ("Enchase",   "m_EnchaseItem",   "outinpart_num",    "pos_enchase",   "UOC_ENCHASE_ITEM"),
    ("Forge",     "m_ForgeItem",     "forgepart_num",    "pos_forge",     "UOC_FORGE_ITEM"),
]


def moi_phong(mau):
    """Nhan ban mau cho 6 phong, thay cac the @HAM@ @MANG@ @NUM@ @POS@ @UOC@."""
    ra = []
    for ham, mang, num, pos, uoc in PHONG:
        ra.append(mau.replace("@HAM@", ham).replace("@MANG@", mang)
                     .replace("@NUM@", num).replace("@POS@", pos)
                     .replace("@UOC@", uoc))
    return "".join(ra)


# ===========================================================================
# KItemList.h
# ===========================================================================
H1_TIM = khoi(
    TAB + "int" + TAB*3 + "m_TrembleItem[compoundpart_num];",
)
H1_THAY = H1_TIM + moi_phong(
    TAB + "int" + TAB*3 + "@MANG@[@NUM@];" + NL
)

H2_TIM = khoi(
    TAB + "int" + TAB*3 + "GetTrembleItem(int nIdx) { return m_TrembleItem[nIdx]; }",
)
H2_THAY = H2_TIM + khoi(
    TAB + "// [LOREN] 6 phong do lo ren - khuon chep tu tremble",
) + moi_phong(
    TAB + "int" + TAB*3 + "Get@HAM@Item(int nIdx) { return @MANG@[nIdx]; }" + NL
) + moi_phong(
    TAB + "BOOL" + TAB*2 + "Check@HAM@Item(int nIdx, int nPlace = -1);" + NL
    + TAB + "BOOL" + TAB*2 + "Add@HAM@Item(int nIdx, int nPlace = -1);" + NL
    + TAB + "void" + TAB*2 + "Un@HAM@Item(int nIdx, int nPlace = -1);" + NL
)

# ===========================================================================
# KItemList.cpp
# ===========================================================================
# --- C1: AddKIL - dat cho (khuon case pos_tremble ~325) ---------------------
C1_TIM = khoi(
    TAB + "case pos_tremble:",
    TAB*2 + "if (nX < 0 || nX >= tremblepart_num)",
    TAB*3 + "return 0;",
    TAB*2 + "if (m_TrembleItem[nX])",
    TAB*3 + "return 0;",
    TAB*2 + "m_Items[i].nPlace = pos_tremble;",
    TAB*2 + "m_Items[i].nX = nX;",
    TAB*2 + "m_Items[i].nY = 0;",
    TAB*2 + "break;",
)
C1_THAY = C1_TIM + moi_phong(khoi(
    TAB + "case @POS@:" + TAB + "// [LOREN]",
    TAB*2 + "if (nX < 0 || nX >= @NUM@)",
    TAB*3 + "return 0;",
    TAB*2 + "if (@MANG@[nX])",
    TAB*3 + "return 0;",
    TAB*2 + "m_Items[i].nPlace = @POS@;",
    TAB*2 + "m_Items[i].nX = nX;",
    TAB*2 + "m_Items[i].nY = 0;",
    TAB*2 + "break;",
))

# --- C2: AddKIL - hook sau khi dat (khuon CheckTrembleItem ~358) ------------
C2_TIM = khoi(
    TAB + "if (m_Items[i].nPlace == pos_tremble)" + TAB*2,
    TAB + "{",
    TAB*2 + "CheckTrembleItem(m_Items[i].nIdx, nX);",
    TAB + "}",
)
C2_THAY = C2_TIM + moi_phong(khoi(
    TAB + "if (m_Items[i].nPlace == @POS@)" + TAB + "// [LOREN]",
    TAB + "{",
    TAB*2 + "Add@HAM@Item(m_Items[i].nIdx, nX);",
    TAB + "}",
))

# --- C3: AddKIL - anh xa container client (~466) ----------------------------
C3_TIM = khoi(
    TAB + "case pos_tremble: ",
    TAB*2 + "pInfo.Region.h = 0;",
    TAB*2 + "pInfo.Region.v = PartTrembleConvert[nX];",
    TAB*2 + "pInfo.eContainer = UOC_TREMBLE_ITEM;",
    TAB*2 + "break;",
)
C3_THAY = C3_TIM + moi_phong(khoi(
    TAB + "case @POS@:" + TAB + "// [LOREN] khong can bang doi, v = nX truc tiep",
    TAB*2 + "pInfo.Region.h = 0;",
    TAB*2 + "pInfo.Region.v = nX;",
    TAB*2 + "pInfo.eContainer = @UOC@;",
    TAB*2 + "break;",
))

# --- C4: Remove - go do (~624) ----------------------------------------------
C4_TIM = khoi(
    TAB + "case pos_tremble:",
    TAB*2 + "UnTrembleItem(m_Items[nIdx].nIdx);",
    TAB*2 + "break;",
)
C4_THAY = C4_TIM + moi_phong(khoi(
    TAB + "case @POS@:" + TAB + "// [LOREN]",
    TAB*2 + "Un@HAM@Item(m_Items[nIdx].nIdx);",
    TAB*2 + "break;",
))

# --- C5: Remove - anh xa container client (~730) ----------------------------
C5_TIM = khoi(
    TAB + "case pos_tremble: ",
    TAB*2 + "pInfo.Region.h = 0;",
    TAB*2 + "pInfo.Region.v = PartTrembleConvert[m_Items[nIdx].nX];",
    TAB*2 + "pInfo.eContainer = UOC_TREMBLE_ITEM;",
    TAB*2 + "break;",
)
C5_THAY = C5_TIM + moi_phong(khoi(
    TAB + "case @POS@:" + TAB + "// [LOREN]",
    TAB*2 + "pInfo.Region.h = 0;",
    TAB*2 + "pInfo.Region.v = m_Items[nIdx].nX;",
    TAB*2 + "pInfo.eContainer = @UOC@;",
    TAB*2 + "break;",
))

# --- C6: ExchangeItem - 6 case theo khuon tremble (~3284) -------------------
# Moc neo = duoi cua case pos_tremble (nhanh else + break + dau } dong switch).
# Cac dong co khoang trang thua ({\t, #endif\t\t, dong chi co \t\t\t) lay
# DUNG BYTE tu tep that - dung "sua dep" keo moc truot.
C6_DUOI = khoi(
    TAB*2 + "else",
    TAB*2 + "{" + TAB,
    TAB*3 + "if (nEquipIdx1)",
    TAB*3 + "{",
    TAB*4 + "UnTrembleItem(nEquipIdx1, SrcPos->nX);",
    TAB*3 + "}",
    TAB*3 + "m_Hand = nEquipIdx1;",
    TAB*3 + "m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;",
    "#ifdef _SERVER",
    TAB*3 + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));",
    "#endif" + TAB*2,
    TAB*3,
    TAB*2 + "}",
    TAB*2 + "break;",
)
C6_TIM = C6_DUOI + khoi(TAB + "}")
C6_THAY = C6_DUOI + khoi(
    TAB + "// [LOREN] 6 phong do lo ren - khuon chep tu case pos_tremble",
) + moi_phong(khoi(
    TAB + "case @POS@:",
    TAB*2 + "if (Player[this->m_PlayerIdx].CheckTrading())",
    TAB*3 + "return;",
    TAB*2 + "if (SrcPos->nX < 0 || SrcPos->nX >= @NUM@ || DesPos->nX < 0 || DesPos->nX >= @NUM@)",
    TAB*3 + "return;",
    TAB*2 + "nEquipIdx1 = @MANG@[SrcPos->nX];",
    TAB*2 + "if (m_Hand)",
    TAB*2 + "{",
    TAB*3 + "if(Add@HAM@Item(m_Hand, DesPos->nX) == TRUE)",
    TAB*3 + "{",
    TAB*4 + "if (nEquipIdx1)",
    TAB*4 + "{",
    TAB*5 + "Un@HAM@Item(nEquipIdx1, SrcPos->nX);",
    TAB*4 + "}",
    TAB*4 + "m_Hand = nEquipIdx1;",
    TAB*4 + "m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;",
    "#ifdef _SERVER",
    TAB*4 + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));",
    "#endif",
    TAB*3 + "}",
    TAB*3 + "else if (nEquipIdx1)",
    TAB*3 + "{",
    TAB*4 + "Add@HAM@Item(nEquipIdx1, SrcPos->nX);",
    TAB*3 + "}",
    TAB*2 + "}",
    TAB*2 + "else",
    TAB*2 + "{",
    TAB*3 + "if (nEquipIdx1)",
    TAB*3 + "{",
    TAB*4 + "Un@HAM@Item(nEquipIdx1, SrcPos->nX);",
    TAB*3 + "}",
    TAB*3 + "m_Hand = nEquipIdx1;",
    TAB*3 + "m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;",
    "#ifdef _SERVER",
    TAB*3 + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));",
    "#endif",
    TAB*2 + "}",
    TAB*2 + "break;",
)) + khoi(TAB + "}")

# --- C7: ExchangeItem - anh xa container client (~3492) ---------------------
C7_TIM = khoi(
    TAB*2 + "case pos_tremble: ",
    TAB*3 + "pInfo1.Region.h = 0;",
    TAB*3 + "pInfo1.Region.v = PartTrembleConvert[SrcPos->nX];",
    TAB*3 + "pInfo2.Region.h = 0;",
    TAB*3 + "pInfo2.Region.v = PartTrembleConvert[DesPos->nX];",
    TAB*3 + "pInfo1.eContainer = UOC_TREMBLE_ITEM;",
    TAB*3 + "pInfo2.eContainer = UOC_TREMBLE_ITEM;",
    TAB*3 + "break;",
)
C7_THAY = C7_TIM + moi_phong(khoi(
    TAB*2 + "case @POS@:" + TAB + "// [LOREN]",
    TAB*3 + "pInfo1.Region.h = 0;",
    TAB*3 + "pInfo1.Region.v = SrcPos->nX;",
    TAB*3 + "pInfo2.Region.h = 0;",
    TAB*3 + "pInfo2.Region.v = DesPos->nX;",
    TAB*3 + "pInfo1.eContainer = @UOC@;",
    TAB*3 + "pInfo2.eContainer = @UOC@;",
    TAB*3 + "break;",
))

# --- C8: ClearAll - khoi tao 6 mang = 0 (~3752) -----------------------------
C8_TIM = khoi(
    TAB + "ZeroMemory(m_TrembleItem, sizeof(m_TrembleItem));",
)
C8_THAY = C8_TIM + moi_phong(
    TAB + "ZeroMemory(@MANG@, sizeof(@MANG@));" + NL
)

# --- C9: RecoverItem - mo dau khoi comment ----------------------------------
C9_TIM = khoi(TAB*2 + "/*case pos_compone:")
C9_THAY = khoi(TAB*2 + "case pos_compone:" + TAB + "// [LOREN] mo khoi")

# --- C10..C15: RecoverItem - sua ruot tung phong Add -> AddI / AddKIL -------
REC_TIM_MAU = khoi(
    TAB*5 + "int nIndex = ItemSet.Add(&Item[@MANG@[i]]);",
    TAB*5 + "int nX, nY;",
    TAB*5 + "if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))",
    TAB*5 + "{",
    TAB*6 + "Add(nIndex, pos_equiproom, nX, nY);",
    TAB*6 + "Remove(@MANG@[i]);",
)
REC_THAY_MAU = khoi(
    TAB*5 + "int nIndex = ItemSet.AddI(&Item[@MANG@[i]]);",
    TAB*5 + "int nX, nY;",
    TAB*5 + "if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))",
    TAB*5 + "{",
    TAB*6 + "AddKIL(nIndex, pos_equiproom, nX, nY);",
    TAB*6 + "Remove(@MANG@[i]);",
)

# --- C16: RecoverItem - dong khoi comment -----------------------------------
C16_TIM = khoi(
    "#else",
    TAB*3 + "memset(&m_EnchaseItem, 0, sizeof(m_EnchaseItem));",
    "#endif",
    TAB*3 + "break;*/",
)
C16_THAY = khoi(
    "#else",
    TAB*3 + "memset(&m_EnchaseItem, 0, sizeof(m_EnchaseItem));",
    "#endif",
    TAB*3 + "break;",
)

# --- C17: 18 than ham sau UnTrembleItem (~5669) -----------------------------
C17_TIM = khoi(
    TAB + "m_TrembleItem[i] = 0;",
    "",
    TAB + "i = 0;",
    TAB + "return;",
    "}",
)
C17_THAY = C17_TIM + khoi(
    "",
    "// ---------------------------------------------------------------------------",
    "// [LOREN] 6 phong do lo ren (compone/comptwo/compthree/distill/enchase/forge)",
    "// Khuon chep tu CheckTrembleItem/UnTrembleItem. CheckXxx cho qua MOI vat pham:",
    "// kiem tra nguyen lieu that su do KFoundryResDemand lam khi nguoi choi bam nut.",
    "// Chi chan chi so o khong hop le va o da co do.",
    "// ---------------------------------------------------------------------------",
) + moi_phong(khoi(
    "BOOL KItemList::Check@HAM@Item(int nIdx, int nPlace /* = -1 */)",
    "{",
    TAB + "if (m_PlayerIdx <= 0 || nIdx <= 0)",
    TAB*2 + "return FALSE;",
    "",
    TAB + "int nItemListIdx = FindSame(nIdx);",
    TAB + "if (!nItemListIdx)",
    TAB*2 + "return FALSE;",
    "",
    TAB + "if (nPlace < 0 || nPlace >= @NUM@)",
    TAB*2 + "return FALSE;",
    "",
    TAB + "if (@MANG@[nPlace])",
    TAB*2 + "return FALSE;",
    "",
    TAB + "return TRUE;",
    "}",
    "",
    "BOOL KItemList::Add@HAM@Item(int nIdx, int nPlace /* = -1 */)",
    "{",
    TAB + "if (Check@HAM@Item(nIdx, nPlace) == FALSE)",
    TAB*2 + "return FALSE;",
    "",
    TAB + "int nItemListIdx = FindSame(nIdx);",
    TAB + "@MANG@[nPlace] = nIdx;",
    TAB + "m_Items[nItemListIdx].nPlace = @POS@;",
    TAB + "m_Items[nItemListIdx].nX = nPlace;",
    TAB + "m_Items[nItemListIdx].nY = 0;",
    TAB + "return TRUE;",
    "}",
    "",
    "void KItemList::Un@HAM@Item(int nIdx, int nPos /* = -1 */)",
    "{",
    TAB + "int i = 0;",
    TAB + "if (m_PlayerIdx <= 0)",
    TAB*2 + "return;",
    "",
    TAB + "if (nIdx <= 0)",
    TAB*2 + "return;",
    "",
    TAB + "if (nPos < 0)",
    TAB + "{",
    TAB*2 + "for (i = 0; i < @NUM@; i++)",
    TAB*2 + "{",
    TAB*3 + "if (@MANG@[i] == nIdx)",
    TAB*3 + "{",
    TAB*4 + "break;",
    TAB*3 + "}",
    TAB*2 + "}",
    TAB*2 + "if (i == @NUM@)",
    TAB*3 + "return;",
    TAB + "}",
    TAB + "else",
    TAB + "{",
    TAB*2 + "if (nPos >= @NUM@)",
    TAB*3 + "return;",
    TAB*2 + "if (@MANG@[nPos] != nIdx)",
    TAB*3 + "return;",
    TAB*2 + "i = nPos;",
    TAB + "}",
    TAB + "@MANG@[i] = 0;",
    "}",
    "",
))


def gom_mieng():
    """Tra ve [(tep, ten mieng, TIM, THAY)] theo thu tu ap dung."""
    mieng = [
        (H, "H1 mang thanh vien", H1_TIM, H1_THAY),
        (H, "H2 getter + khai bao", H2_TIM, H2_THAY),
        (C, "C1 AddKIL dat cho", C1_TIM, C1_THAY),
        (C, "C2 AddKIL hook Add", C2_TIM, C2_THAY),
        (C, "C3 AddKIL container", C3_TIM, C3_THAY),
        (C, "C4 Remove go do", C4_TIM, C4_THAY),
        (C, "C5 Remove container", C5_TIM, C5_THAY),
        (C, "C6 ExchangeItem 6 case", C6_TIM, C6_THAY),
        (C, "C7 Exchange container", C7_TIM, C7_THAY),
        (C, "C8 ClearAll ZeroMemory", C8_TIM, C8_THAY),
        (C, "C9 Recover mo comment", C9_TIM, C9_THAY),
    ]
    for ham, mang, num, pos, uoc in PHONG:
        mieng.append((C, "C10+ Recover ruot " + ham,
                      REC_TIM_MAU.replace("@MANG@", mang),
                      REC_THAY_MAU.replace("@MANG@", mang)))
    mieng.append((C, "C16 Recover dong comment", C16_TIM, C16_THAY))
    mieng.append((C, "C17 18 than ham", C17_TIM, C17_THAY))
    return mieng


def va_mot(t, ten, a, b):
    """Tra (t_moi, 'VA'|'DACO'|None)."""
    if b in a:
        # THAY nam gon trong TIM (vd bo dau comment) -> phai kiem TIM truoc
        if a in t:
            n = t.count(a)
            if n != 1:
                print("  *** LOI [%s]: moc neo xuat hien %d lan" % (ten, n))
                return t, None
            return t.replace(a, b, 1), "VA"
        if b in t:
            return t, "DACO"
        print("  *** LOI [%s]: khong thay moc neo lan chuoi THAY" % ten)
        return t, None
    else:
        if b in t:
            return t, "DACO"
        n = t.count(a)
        if n != 1:
            print("  *** LOI [%s]: moc neo xuat hien %d lan" % (ten, n))
            return t, None
        return t.replace(a, b, 1), "VA"


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("v36 - HOI SINH 6 PHONG DO LO REN phia may chu")
    print("Che do: %s" % ("GHI THAT" if ghi else "DIEN TAP (khong ghi)"))
    print("=" * 70)

    noidung = {}
    goc = {}
    for p in (H, C):
        if not os.path.isfile(p):
            print("*** LOI: khong thay tep %s" % p)
            return 1
        noidung[p] = doc(p)
        goc[p] = noidung[p]

    loi = 0
    so_va = 0
    so_daco = 0
    for p, ten, a, b in gom_mieng():
        t = noidung[p]
        a2 = theo_eol(t, a)
        b2 = theo_eol(t, b)
        t2, kq = va_mot(t, ten, a2, b2)
        if kq is None:
            loi += 1
            continue
        noidung[p] = t2
        nhan = "SE VA " if not ghi else "DA VA "
        if kq == "DACO":
            nhan = "DA CO - bo qua"
            so_daco += 1
        else:
            so_va += 1
        print("  %-14s %s" % (nhan, ten))

    print("-" * 70)
    for p in (H, C):
        c0, l0 = dem_dong(goc[p])
        c1, l1 = dem_dong(noidung[p])
        if min(c1, l1) > min(c0, l0):
            print("*** LOI: %s tang loai xuong dong thieu so (LF le %d->%d)"
                  % (os.path.basename(p), min(c0, l0), min(c1, l1)))
            loi += 1
        print("  %s: CRLF %d->%d, LF le %d->%d"
              % (os.path.basename(p), c0, c1, min(c0, l0), min(c1, l1)))

    if loi:
        print("*** CO %d LOI - KHONG GHI GI." % loi)
        return 1

    print("Tong: %d mieng se va, %d mieng da co san." % (so_va, so_daco))
    if not ghi:
        print("DIEN TAP XONG - chua ghi gi. Them --ghi de ghi that.")
        return 0

    for p in (H, C):
        if noidung[p] == goc[p]:
            continue
        luu = p + ".truoc_uiloren"
        if not os.path.isfile(luu):
            shutil.copyfile(p, luu)
            print("Sao luu: %s" % luu)
        io.open(p, "wb").write(noidung[p].encode("latin-1"))
        print("DA GHI: %s" % p)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
