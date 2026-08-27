# -*- coding: utf-8 -*-
# ============================================================================
# v39_client_ui.py - Va UiCompoundItem.cpp (S3Client\Ui\UiCase) cho he lo ren
#
# 6 nhom mieng va:
#   A. Bug KUiCompoundThree::Initialize dung nham UOC_COMPTWO_ITEM
#      -> UOC_COMPTHREE_ITEM (doi chieu OnItemPickDrop dong ~1922/1941).
#   B. Bug KUiForge::LoadScheme lap i < _ITEM_COMP_COUNT (=3) tren mang
#      CtrlForgeItemMap[_ITEM_FORGE_COUNT] (=2) -> tran chi so -> _ITEM_FORGE_COUNT.
#   C. 6 dong chu thich GOI_LOAD_BUTTON_SCRIPT -> goi that
#      GOI_ADD_UI_CMD_SCRIPT voi uParam=7 va ten trang theo lop
#      (khop danh sach case 7 cua v37: LR_UI_MotOre/HaiOre/BaOre/Distill/Forge/Enchase).
#   D. Bo comment 6 dong GOI_RECOVER_ITEM trong KUiCompoundItem::CloseWindow
#      (neo theo dong "case WINDOWS_xxx:" ngay tren de moc duy nhat).
#   E. Bo comment 6 dong GOI_RECOVER_ITEM trong OnCancel cua 6 lop
#      (neo theo "if(g_pCoreShell)" ngay tren; ca file chi co dung 6 cho nhu vay).
#   F. Bo comment 6 khoi UpdateAllItem /*...*/ - ca 6 khoi chi dung
#      GDI_COMPONE_ITEM..GDI_ENCHASE_ITEM (v38 khai bao) + Celar()/HoldObject
#      (co that trong WndObjContainer.h) nen mo het.
#      Cac dong doc GDI_ITEM_SERIES/GDI_ITEM_DETAIL/GDI_OPTION_CAN_SET/GDI_ITEM_MAGICTYPE
#      (~3202-3208, ~3250-3253) KHONG dung den vi enum khong ton tai - DE NGUYEN.
#   G. bCanSet trong KUiEnchase::ProcessEnchase dang gan cung FALSE (~3178)
#      ma duong doc GDI_OPTION_CAN_SET (~3253) khong ton tai -> gan TRUE,
#      server kiem that khi bam nut.
#
# Mac dinh DIEN TAP (khong ghi). Ghi that: python v39_client_ui.py --ghi
# Sao luu lan dau: UiCompoundItem.cpp.truoc_uiloren
# Doc/ghi latin-1 giu nguyen byte TCVN3; moc neo toan ASCII.
# ============================================================================
import io
import os
import sys

TEP = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
SAO_LUU = TEP + ".truoc_uiloren"


def theo_eol(s, eol):
    """Doi moc neo viet bang \\n sang EOL troi cua tep."""
    return s.replace("\n", eol)


def mo_khoi(cu):
    """Bo /* sau tab dau va */ o cuoi cua mot khoi comment \t/*...\t}*/"""
    if not (cu.startswith("\t/*") and cu.endswith("\t}*/")):
        raise SystemExit("LOI NOI BO: khoi khong dung dang \\t/*...\\t}*/")
    return "\t" + cu[3:-2]


# ---------------------------------------------------------------------------
# Danh sach mieng va: (ten, chuoi_cu, chuoi_moi)
# ---------------------------------------------------------------------------
VA = []

# --- A. UOC_COMPTWO_ITEM -> UOC_COMPTHREE_ITEM trong KUiCompoundThree ------
# (KUiCompoundTwo::Initialize co khoi giong het nhung mo bang "{" thuong,
#  con KUiCompoundThree mo bang "{\t" - van phai neo ca ten ham cho chac)
VA.append((
    "A. CompThree SetContainerId dung nham UOC_COMPTWO_ITEM",
    "void KUiCompoundThree::Initialize()\n"
    "{\t\n"
    "\tfor (int i = 0; i < _ITEM_COMP_COUNT; i ++)\n"
    "\t{\n"
    "\t\tm_ItemBox[i].SetObjectGenre(CGOG_ITEM);\n"
    "\t\tAddChild(&m_ItemBox[i]);\n"
    "\t\tm_ItemBox[i].SetContainerId((int)UOC_COMPTWO_ITEM);",
    "void KUiCompoundThree::Initialize()\n"
    "{\t\n"
    "\tfor (int i = 0; i < _ITEM_COMP_COUNT; i ++)\n"
    "\t{\n"
    "\t\tm_ItemBox[i].SetObjectGenre(CGOG_ITEM);\n"
    "\t\tAddChild(&m_ItemBox[i]);\n"
    "\t\tm_ItemBox[i].SetContainerId((int)UOC_COMPTHREE_ITEM);",
))

# --- B. KUiForge::LoadScheme lap _ITEM_COMP_COUNT (3) tren mang 2 phan tu ---
VA.append((
    "B. Forge LoadScheme lap _ITEM_COMP_COUNT -> _ITEM_FORGE_COUNT",
    "\t\tKWndImage::Init(&Ini, \"Main\");\n"
    "\t\tfor (int i = 0; i < _ITEM_COMP_COUNT; i ++)\n"
    "\t\t{\n"
    "\t\t\tm_ItemBox[i].Init(&Ini, CtrlForgeItemMap[i].pIniSection);",
    "\t\tKWndImage::Init(&Ini, \"Main\");\n"
    "\t\tfor (int i = 0; i < _ITEM_FORGE_COUNT; i ++)\n"
    "\t\t{\n"
    "\t\t\tm_ItemBox[i].Init(&Ini, CtrlForgeItemMap[i].pIniSection);",
))

# --- C. 6 dong GOI_LOAD_BUTTON_SCRIPT chu thich -> GOI_ADD_UI_CMD_SCRIPT ----
_SCRIPT = [
    ("COMPOUND_SCRIPT_FILE", "CompOneItem",   "LR_UI_MotOre"),
    ("COMPOUND_SCRIPT_FILE", "CompTwoItem",   "LR_UI_HaiOre"),
    ("COMPOUND_SCRIPT_FILE", "CompThreeItem", "LR_UI_BaOre"),
    ("DISTILL_SCRIPT_FILE",  "DistillItem",   "LR_UI_Distill"),
    ("FORGE_SCRIPT_FILE",    "ForgeItem",     "LR_UI_Forge"),
    ("ENCHASE_SCRIPT_FILE",  "EnchaseItem",   "LR_UI_Enchase"),
]
for _file, _cu_ten, _moi_ten in _SCRIPT:
    VA.append((
        "C. Goi script trang %s -> %s" % (_cu_ten, _moi_ten),
        "\t//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, "
        "(unsigned int)%s, (unsigned int)\"%s\");" % (_file, _cu_ten),
        "\tg_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 7, "
        "(unsigned int)\"%s\");" % _moi_ten,
    ))

# --- D. CloseWindow: bo comment 6 dong GOI_RECOVER_ITEM ---------------------
# Neo kem dong "case WINDOWS_xxx:" ngay tren de moc duy nhat (dong 2 tab
# trong OnCancel co cung noi dung, chi khac muc lui dong).
# CHU Y: pos_distill KHONG co dau cach cuoi dong, 5 dong kia CO - giu nguyen byte.
_CLOSE = [
    ("WINDOWS_COMP",    "pos_compone",   " "),
    ("WINDOWS_COMP2",   "pos_comptwo",   " "),
    ("WINDOWS_COMP3",   "pos_compthree", " "),
    ("WINDOWS_DISTILL", "pos_distill",   ""),
    ("WINDOWS_FORGE",   "pos_forge",     " "),
    ("WINDOWS_ENCHASE", "pos_enchase",   " "),
]
for _case, _pos, _duoi in _CLOSE:
    VA.append((
        "D. CloseWindow tra lai do o %s" % _pos,
        "\t\t\tcase %s:\n"
        "\t\t\t\t//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, "
        "(unsigned int)%s, 0);%s" % (_case, _pos, _duoi),
        "\t\t\tcase %s:\n"
        "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, "
        "(unsigned int)%s, 0);%s" % (_case, _pos, _duoi),
    ))

# --- E. OnCancel 6 lop: bo comment GOI_RECOVER_ITEM -------------------------
# Neo kem "if(g_pCoreShell)" + "{" ngay tren (ca file dung 6 cho nhu vay).
# Ca 6 dong deu co dau cach cuoi dong.
_CANCEL = [
    "pos_compone", "pos_comptwo", "pos_compthree",
    "pos_distill", "pos_forge", "pos_enchase",
]
for _pos in _CANCEL:
    VA.append((
        "E. OnCancel tra lai do o %s" % _pos,
        "\tif(g_pCoreShell)\n"
        "\t{\n"
        "\t\t//g_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, "
        "(unsigned int)%s, 0); " % _pos,
        "\tif(g_pCoreShell)\n"
        "\t{\n"
        "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, "
        "(unsigned int)%s, 0); " % _pos,
    ))

# --- F. Bo comment 6 khoi UpdateAllItem /*...*/ -----------------------------
# Moi khoi chi dung GDI_xxx_ITEM ma v38 khai bao + Celar()/HoldObject co that.
_KHOI = [
    ("GDI_COMPONE_ITEM",   "_ITEM_COMP_COUNT"),
    ("GDI_COMPTWO_ITEM",   "_ITEM_COMP_COUNT"),
    ("GDI_COMPTHREE_ITEM", "_ITEM_COMP_COUNT"),
    ("GDI_DISTILL_ITEM",   "_ITEM_OUTIN_COUNT"),
    ("GDI_FORGE_ITEM",     "_ITEM_FORGE_COUNT"),
    ("GDI_ENCHASE_ITEM",   "_ITEM_OUTIN_COUNT"),
]
for _gdi, _dem in _KHOI:
    _cu = (
        "\t/*KUiObjAtRegion\tItem[%s];\n"
        "\tint nCount = g_pCoreShell->GetGameData(%s, (unsigned int)&Item, 0);\n"
        "\tint\ti;\n"
        "\tfor (i = 0; i < %s; i++)\n"
        "\t\tm_ItemBox[i].Celar();\n"
        "\tfor (i = 0; i < nCount; i++)\n"
        "\t{\n"
        "\t\tif (Item[i].Obj.uGenre != CGOG_NOTHING)\n"
        "\t\t\tUpdateItem(&Item[i], true);\n"
        "\t}*/" % (_dem, _gdi, _dem)
    )
    VA.append((
        "F. Mo khoi UpdateAllItem doc %s" % _gdi,
        _cu,
        mo_khoi(_cu),
    ))

# --- G. bCanSet gan cung FALSE trong ProcessEnchase -------------------------
# Duong doc that (GDI_OPTION_CAN_SET dong ~3253) khong ton tai va van de
# nguyen dang comment; gan TRUE de khong chan nghiep vu phia client.
VA.append((
    "G. Enchase bCanSet FALSE cung -> TRUE (server kiem that)",
    "\tBOOL bCanSet = FALSE;",
    "\tBOOL bCanSet = TRUE; // server kiem that khi bam nut",
))


# ---------------------------------------------------------------------------
def main():
    ghi = ("--ghi" in sys.argv)

    if not os.path.isfile(TEP):
        print("LOI: khong thay tep dich: %s" % TEP)
        sys.exit(2)

    f = io.open(TEP, "rb")
    goc = f.read().decode("latin-1")
    f.close()

    eol = "\r\n" if goc.count("\r\n") * 2 >= goc.count("\n") else "\n"
    print("Tep dich : %s" % TEP)
    print("EOL troi : %s | %d mieng va" % ("CRLF" if eol == "\r\n" else "LF", len(VA)))
    print("Che do   : %s" % ("GHI THAT" if ghi else "DIEN TAP (khong ghi; muon ghi them --ghi)"))
    print("-" * 74)

    data = goc
    loi = []
    so_ap = 0
    so_bo = 0

    for ten, cu, moi in VA:
        cu2 = theo_eol(cu, eol)
        moi2 = theo_eol(moi, eol)
        if moi2 in data:
            print("BO QUA (da va tu truoc): %s" % ten)
            so_bo += 1
            continue
        n = data.count(cu2)
        if n != 1:
            loi.append("  - '%s': moc neo xuat hien %d lan (can dung 1 lan)" % (ten, n))
            continue
        data = data.replace(cu2, moi2, 1)
        so_ap += 1
        print("AP: %s" % ten)

    print("-" * 74)

    if loi:
        print("!" * 74)
        print("LOI TO - MOC NEO KHONG KHOP, KHONG GHI GI CA:")
        for d in loi:
            print(d)
        print("!" * 74)
        sys.exit(2)

    print("Tong ket: ap %d, bo qua %d, loi 0." % (so_ap, so_bo))

    if not ghi:
        print("DIEN TAP xong - tep dich CHUA bi dong den.")
        return

    if so_ap == 0:
        print("Khong co gi moi de ghi.")
        return

    if not os.path.exists(SAO_LUU):
        f = io.open(SAO_LUU, "wb")
        f.write(goc.encode("latin-1"))
        f.close()
        print("Da sao luu: %s" % SAO_LUU)

    f = io.open(TEP, "wb")
    f.write(data.encode("latin-1"))
    f.close()
    print("DA GHI %d mieng va vao %s" % (so_ap, TEP))


if __name__ == "__main__":
    main()
