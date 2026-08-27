# -*- coding: utf-8 -*-
r"""v38 - CLIENT CORE cho UI lo ren (6 khu chua do: compone..enchase).

Tep dich (deu doc/ghi latin-1, byte-nguyen - cay nay co TCVN3/GBK):
  1. CoreShell.h              : them 6 enum GDI_COMPONE_ITEM..GDI_ENCHASE_ITEM
                                o CUOI enum GAMEDATA_INDEX (sau GDI_GAMBLE_OPER_DATA,
                                truoc dau dong '};') de KHONG xe dich gia tri cu.
  2. CoreShell.cpp GetGameData: 6 nhanh moi theo dung khuon GDI_TREMBLE_ITEM
                                (CoreShell.cpp:2381) - doc getter cua m_ItemList
                                (GetCompOneItem.. do v38_itemlist/v36 khai bao),
                                Region.v = chi so o (khong can bang doi nhu
                                PartTrembleConvert vi bang do la bang dong nhat).
  3. CoreShell.cpp GOI_SWITCH_OBJECT: 6 case UOC_COMPONE_ITEM..UOC_ENCHASE_ITEM
                                cho ca P1 (pObject1, ~13153) va P2 (pObject2,
                                ~13293) theo khuon case UOC_TREMBLE_ITEM song.
  4. CoreShell.cpp GOI_RECOVER_ITEM (~12825): 6 case pos_compone..pos_enchase
                                SONG, chep khuon case pos_tremble SONG ngay tren;
                                khoi chu thich /*case pos_compone..*/ cua VLTK
                                GIU NGUYEN (khong mo) - ten getter lay dung theo
                                khoi do: GetCompOneItem/GetCompTwoItem/
                                GetCompThreeItem/GetDistillItem/GetForgeItem/
                                GetEnchaseItem; sau switch da co san doan
                                RecoverItem(uParam) + SendClientCmdRecoverItem.
  5. GameSpaceChangedNotify.cpp GDCNI_OBJECT_CHANGED (~241): 6 nhanh container
                                theo khuon UOC_TREMBLE_ITEM - cua so lo ren la
                                MOT lop KUiCompoundItem (da co san UiCompoundItem.h
                                duoc include o dong 62, co GetIfVisible +
                                UpdateItem(KUiObjAtRegion*, int)).

Cac dinh danh nen da CO SAN trong GameDataDef.h (kiem 27/08):
  UOC_COMPONE_ITEM..UOC_ENCHASE_ITEM (dong 751-756),
  pos_compone..pos_enchase (dong 335-340),
  compoundpart_num / outinpart_num / forgepart_num (dong 247/263/270).
Chi getter GetCompOneItem.. trong KItemList.h la do mieng va ITEMLIST khai -
script nay chi CANH BAO neu chua thay (build se can no).

Mac dinh DIEN TAP (khong ghi). Them --ghi moi ghi that; lan ghi dau tao
ban sao <tep>.truoc_uiloren.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CR = chr(13)
NL = chr(10)

H = r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.h"
C = r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp"
G = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\GameSpaceChangedNotify.cpp"
KIL = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemList.h"
DUOI_SAOLUU = ".truoc_uiloren"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dem_dong(t):
    crlf = t.count(CR + NL)
    return crlf, t.count(NL) - crlf


def theo_eol(t, s):
    """doi xuong dong cua chuoi cho khop EOL troi cua tep dich"""
    crlf, lf = dem_dong(t)
    s = s.replace(CR + NL, NL)
    if crlf > lf:
        s = s.replace(NL, CR + NL)
    return s


# ---------------------------------------------------------------------------
# 6 khu chua do: (TEN_UOC, pos_, getter, hang_so_so_o, mo ta ascii)
# getter + hang so lay DUNG theo khoi chu thich /*case pos_compone..*/ co san
# trong GOI_RECOVER_ITEM cua CoreShell.cpp (12850..12914).
# ---------------------------------------------------------------------------
KHU = [
    ("COMPONE",   "pos_compone",   "GetCompOneItem",   "3 /* _ITEM_COMP_COUNT - UI chi co Ore1..3, quet 8 la tran mang UpdateAllItem */", "luyen huyen tinh khoang thach"),
    ("COMPTWO",   "pos_comptwo",   "GetCompTwoItem",   "3 /* _ITEM_COMP_COUNT */", "nang cap huyen tinh khoang thach"),
    ("COMPTHREE", "pos_compthree", "GetCompThreeItem", "3 /* _ITEM_COMP_COUNT */", "nang cap khoang thach thuoc tinh"),
    ("DISTILL",   "pos_distill",   "GetDistillItem",   "outinpart_num",    "rut option trang bi"),
    ("FORGE",     "pos_forge",     "GetForgeItem",     "forgepart_num",    "che tao trang bi tim"),
    ("ENCHASE",   "pos_enchase",   "GetEnchaseItem",   "outinpart_num",    "kham nam trang bi"),
]


def khoi(*d):
    return "".join(x + NL for x in d)


def the(mau, k):
    ten, pos, getter, num, mota = k
    return (mau.replace("@TEN@", ten).replace("@POS@", pos)
                .replace("@GETTER@", getter).replace("@NUM@", num)
                .replace("@MOTA@", mota))


# ---------------------------------------------------------------------------
# 1. CoreShell.h - 6 enum o CUOI GAMEDATA_INDEX (moc = dong //Return cuoi + };)
# ---------------------------------------------------------------------------
H1_TIM = (
    "\t//Return The specific meaning is determined by the value of uParam,"
    " see the description of UI_TRADE_OPER_DATA\n};"
)
H1_THAY = (
    "\t//Return The specific meaning is determined by the value of uParam,"
    " see the description of UI_TRADE_OPER_DATA\n"
    "\n"
    "\t// [UILOREN] 6 khu chua do cua he lo ren - them CUOI enum de khong\n"
    "\t// xe dich gia tri cac muc cu; doi ung UOC_*/pos_* trong GameDataDef.h\n"
    "\tGDI_COMPONE_ITEM,\t\t//Lo ren - luyen huyen tinh khoang thach\n"
    "\tGDI_COMPTWO_ITEM,\t\t//Lo ren - nang cap huyen tinh khoang thach\n"
    "\tGDI_COMPTHREE_ITEM,\t\t//Lo ren - nang cap khoang thach thuoc tinh\n"
    "\tGDI_DISTILL_ITEM,\t\t//Lo ren - rut option trang bi\n"
    "\tGDI_FORGE_ITEM,\t\t\t//Lo ren - che tao trang bi tim\n"
    "\tGDI_ENCHASE_ITEM,\t\t//Lo ren - kham nam trang bi\n"
    "};"
)

# ---------------------------------------------------------------------------
# 2. CoreShell.cpp GetGameData - 6 case theo khuon GDI_TREMBLE_ITEM, chen ngay
#    sau nhanh tremble (truoc case GDI_CHAT_ITEM_IMAGE, moc duy nhat).
# ---------------------------------------------------------------------------
MAU_GDI = khoi(
    "\tcase GDI_@TEN@_ITEM:\t\t// [UILOREN] lo ren - @MOTA@ (khuon GDI_TREMBLE_ITEM)",
    "\t\tnRet = 0;",
    "\t\tif (uParam)",
    "\t\t{",
    "\t\t\tif (nParam == 1)",
    "\t\t\t\tbreak;",
    "",
    "\t\t\tint nCount = 0;",
    "\t\t\tKUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;",
    "",
    "\t\t\tfor (int i = 0; i < @NUM@; i++)",
    "\t\t\t{",
    "\t\t\t\tpInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.@GETTER@(i);",
    "\t\t\t\tif (pInfo->Obj.uId)",
    "\t\t\t\t{",
    "\t\t\t\t\tpInfo->Obj.uGenre = CGOG_ITEM;",
    "",
    "\t\t\t\t\tpInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();",
    "\t\t\t\t\tpInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();",
    "\t\t\t\t\tpInfo->Region.h = 0;",
    "\t\t\t\t\tpInfo->Region.v = i;",
    "\t\t\t\t}",
    "\t\t\t\telse",
    "\t\t\t\t{",
    "\t\t\t\t\tpInfo->Obj.uGenre = CGOG_NOTHING;",
    "\t\t\t\t}",
    "\t\t\t\tnCount++;",
    "\t\t\t\tpInfo++;",
    "\t\t\t}",
    "\t\t\tnRet = nCount;",
    "\t\t}",
    "\t\tbreak;",
)
C1_TIM = "\tcase GDI_CHAT_ITEM_IMAGE:"
C1_THAY = "".join(the(MAU_GDI, k) for k in KHU) + "\tcase GDI_CHAT_ITEM_IMAGE:"

# ---------------------------------------------------------------------------
# 3a. GOI_SWITCH_OBJECT phia P1 (pObject1) - chen sau case UOC_TREMBLE_ITEM song.
#     Region.v cua cac khu nay la chi so o thang (bang doi la bang dong nhat
#     nen khong can PartConvert rieng).
# ---------------------------------------------------------------------------
MAU_P1 = khoi(
    "\t\t\t\tcase UOC_@TEN@_ITEM:\t// [UILOREN] khuon UOC_TREMBLE_ITEM",
    "\t\t\t\t\t{",
    "\t\t\t\t\t\tif (pObject1->Region.h == 1)",
    "\t\t\t\t\t\t\tbreak;",
    "\t\t\t\t\t\tP1.nPlace = @POS@;",
    "\t\t\t\t\t\tP1.nX = pObject1->Region.v;",
    "\t\t\t\t\t}",
    "\t\t\t\t\tbreak;",
)
C2_TIM = khoi(
    "\t\t\t\tcase UOC_TREMBLE_ITEM: ",
    "\t\t\t\t\t{",
    "\t\t\t\t\t\tif (pObject1->Region.h == 1)",
    "\t\t\t\t\t\t\tbreak;",
    "\t\t\t\t\t\tP1.nPlace = pos_tremble;",
    "\t\t\t\t\t\tP1.nX = PartTrembleConvert[pObject1->Region.v];",
    "\t\t\t\t\t}",
    "\t\t\t\t\tbreak;",
)
C2_THAY = C2_TIM + "".join(the(MAU_P1, k) for k in KHU)

# ---------------------------------------------------------------------------
# 3b. GOI_SWITCH_OBJECT phia P2 (pObject2) - y het, doi P1/pObject1 -> P2/pObject2.
# ---------------------------------------------------------------------------
MAU_P2 = MAU_P1.replace("pObject1", "pObject2").replace("P1.", "P2.")
C3_TIM = C2_TIM.replace("pObject1", "pObject2").replace("P1.", "P2.")
C3_THAY = C3_TIM + "".join(the(MAU_P2, k) for k in KHU)

# ---------------------------------------------------------------------------
# 4. GOI_RECOVER_ITEM - 6 case song, chep khuon case pos_tremble SONG ngay tren,
#    chen TRUOC khoi chu thich /*case pos_compone..*/ (giu nguyen khoi cu).
#    Bien i da duoc khai 'int i;' o dau switch(uParam) - dung lai nhu tremble.
#    Sau switch da co san: if(!bExistId) break; RecoverItem(uParam);
#    SendClientCmdRecoverItem(uParam); -> khong phai them gi o duoi.
# ---------------------------------------------------------------------------
MAU_RECOVER = khoi(
    "\t\t\t\tcase @POS@:\t// [UILOREN] khuon case pos_tremble song o tren",
    "\t\t\t\t\t{",
    "\t\t\t\t\t\tfor(i = 0; i < @NUM@; i++)",
    "\t\t\t\t\t\t{",
    "\t\t\t\t\t\t\tif(Player[CLIENT_PLAYER_INDEX].m_ItemList.@GETTER@(i))",
    "\t\t\t\t\t\t\t{",
    "\t\t\t\t\t\t\t\tbExistId = TRUE;",
    "\t\t\t\t\t\t\t}",
    "\t\t\t\t\t\t}",
    "\t\t\t\t\t}",
    "\t\t\t\t\tbreak;",
)
C4_TIM = "\t\t\t\t/*case pos_compone:"
C4_THAY = "".join(the(MAU_RECOVER, k) for k in KHU) + "\t\t\t\t/*case pos_compone:"

# ---------------------------------------------------------------------------
# 5. GameSpaceChangedNotify.cpp - 6 nhanh container sau nhanh UOC_TREMBLE_ITEM.
#    UiCase/UiCompoundItem.h da duoc include san (dong 62); ca 6 khu dung chung
#    MOT cua so KUiCompoundItem.
# ---------------------------------------------------------------------------
MAU_NOTIFY = khoi(
    "\t\t\telse if (pObject->eContainer == UOC_@TEN@_ITEM)\t// [UILOREN] khuon UOC_TREMBLE_ITEM",
    "\t\t\t{",
    "\t\t\t\tKUiCompoundItem* pCompItem = KUiCompoundItem::GetIfVisible();",
    "\t\t\t\tif (pCompItem)",
    "\t\t\t\t\tpCompItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);",
    "\t\t\t}",
)
G1_TIM = khoi(
    "\t\t\telse if (pObject->eContainer == UOC_TREMBLE_ITEM)",
    "\t\t\t{",
    "\t\t\t\tKUiTrembleItem* pItem = KUiTrembleItem::GetIfVisible();",
    "\t\t\t\tif (pItem)",
    "\t\t\t\t\tpItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);",
    "\t\t\t}",
)
G1_THAY = G1_TIM + "".join(the(MAU_NOTIFY, k) for k in KHU)

# ---------------------------------------------------------------------------
MIENG = [
    (H, "H1 enum GDI_*",           H1_TIM, H1_THAY),
    (C, "C1 GetGameData 6 nhanh",  C1_TIM, C1_THAY),
    (C, "C2 SWITCH_OBJECT P1",     C2_TIM, C2_THAY),
    (C, "C3 SWITCH_OBJECT P2",     C3_TIM, C3_THAY),
    (C, "C4 RECOVER_ITEM 6 case",  C4_TIM, C4_THAY),
    (G, "G1 Notify 6 container",   G1_TIM, G1_THAY),
]


def ap_mieng(t, ten, tim, thay):
    """ap mot mieng va vao noi dung t (trong bo nho). tra (t_moi, ok, msg)"""
    tim = theo_eol(t, tim)
    thay = theo_eol(t, thay)
    if thay in t:
        return t, True, "  BO QUA  %-26s da co san" % ten
    n = t.count(tim)
    if n != 1:
        return t, False, ("  *** LOI %-26s moc neo xuat hien %d lan (can dung 1)"
                          % (ten, n))
    return t.replace(tim, thay, 1), True, "  OK      %-26s" % ten


def main():
    ghi_that = "--ghi" in sys.argv
    print("v38 - CLIENT CORE cho UI lo ren (6 khu compone..enchase)")
    print("=" * 78)
    print("  che do: %s" % ("GHI THAT" if ghi_that else "DIEN TAP (them --ghi de ghi)"))

    # canh bao som: getter do mieng ITEMLIST khai - thieu thi build se gay
    kil = doc(KIL)
    thieu = [k[2] for k in KHU if (k[2] + "(") not in kil]
    if thieu:
        print("  ! CANH BAO: KItemList.h CHUA co getter: %s" % ", ".join(thieu))
        print("    (mieng va ITEMLIST phai chay truoc khi BUILD, khong chan mieng nay)")

    goc = {}     # duong dan -> noi dung goc
    moi = {}     # duong dan -> noi dung sau va
    loi = 0
    for p, ten, tim, thay in MIENG:
        if p not in goc:
            goc[p] = doc(p)
            moi[p] = goc[p]
        moi[p], ok, msg = ap_mieng(moi[p], ten, tim, thay)
        print(msg)
        if not ok:
            loi += 1

    # canh gac EOL: khong duoc de lot dong LF le loi vao tep CRLF
    for p in goc:
        c0, l0 = dem_dong(goc[p])
        c1, l1 = dem_dong(moi[p])
        if l1 > l0:
            print("  *** LOI %s: tang %d dong LF le loi" % (os.path.basename(p), l1 - l0))
            loi += 1

    print("=" * 78)
    if loi:
        print("  CO %d MUC LOI - KHONG GHI GI CA" % loi)
        return 1

    doi = [p for p in goc if moi[p] != goc[p]]
    if not doi:
        print("  XONG - khong co gi phai doi (da va het tu truoc)")
        return 0

    if not ghi_that:
        for p in doi:
            c0, l0 = dem_dong(goc[p])
            c1, l1 = dem_dong(moi[p])
            print("  [DIEN TAP] se ghi %-28s CRLF %d->%d LF-le %d->%d"
                  % (os.path.basename(p), c0, c1, l0, l1))
        print("  DIEN TAP DAT - chua ghi gi; chay lai voi --ghi de ghi that")
        return 0

    for p in doi:
        if not os.path.isfile(p + DUOI_SAOLUU):
            io.open(p + DUOI_SAOLUU, "wb").write(io.open(p, "rb").read())
        io.open(p, "wb").write(moi[p].encode("latin-1"))
        print("  DA GHI %s (sao luu %s)" % (p, os.path.basename(p) + DUOI_SAOLUU))
    print("  XONG - da ghi %d tep" % len(doi))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
