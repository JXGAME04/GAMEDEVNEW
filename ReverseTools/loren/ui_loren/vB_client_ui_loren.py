# -*- coding: ascii -*-
#
# vB_client_ui_loren.py
#
# Va 5 loi UI he lo ren 7 khung (ep do tim + Hoang Kim mon phai) trong
#   D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp
#
# LUAT AN TOAN (tep legacy chua byte TCVN3/GBK):
#   - doc/ghi bang io.open('rb'/'wb') + latin-1, KHONG dung Edit/Write
#   - moi moc neo phai xuat hien DUNG 1 LAN, dem truoc khi sua
#   - lech -> in "LOI TO" va KHONG ghi gi (bo ca luot, khong ghi mot phan)
#   - idempotent: lan 2 bao "DA CO - bo qua"
#   - mac dinh DIEN TAP, chi ghi khi co --ghi
#   - lan ghi dau tao sao luu .truoc_uiloren2
#   - giu nguyen EOL (CRLF), in so CRLF truoc/sau
#
# Cach chay:
#   python vB_client_ui_loren.py          # dien tap (khong ghi gi)
#   python vB_client_ui_loren.py --ghi    # ghi that
#
import io
import os
import sys

DICH = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase\UiCompoundItem.cpp"
SAOLUU_DUOI = ".truoc_uiloren2"

# ------------------------------------------------------------------
# MIENG L1 - the Kham nam: nDetail bi ghim 0 -> case 0 doi nOption[0]==53
# ma nOption da memset 0 (:3183) -> LUON ReturnInfo 16 + return (:3275),
# lenh gui LR_UI_Enchase (:3629) khong bao gio toi.
# Da doc that toan bo switch(nDetail) :3256-3568:
#   (a) co nhanh default :3566-3567, chi "break", KHONG return loi
#   (b) sau switch chi con 3 cong: :3570 (nSeries lech && nDetail > 3),
#       :3588 (!bCanSet), :3605 (thieu tien) - khong cong nao chan them
#   (c) voi nDetail = -1: "-1 > 3" sai -> cong :3570 no-op;
#       bCanSet da la TRUE cung (:3178) -> cong :3588 no-op;
#       cong tien :3605 giu nguyen (la cong that phia client).
# => doi 0 thanh -1 de roi vao default, tra viec kiem cho may chu
#    (KFoundryResDemand loc nguyen lieu khi bam nut).
# ------------------------------------------------------------------
L1_OLD = (
    "\t//bCanSet = g_pCoreShell->GetGameData(GDI_OPTION_CAN_SET,(unsigned int)(&pObj1), nOpProp);\r\n"
    "\t\r\n"
    "\tnDetail = 0;\r\n"
    "\tswitch(nDetail)\r\n"
)
L1_NEW = (
    "\t//bCanSet = g_pCoreShell->GetGameData(GDI_OPTION_CAN_SET,(unsigned int)(&pObj1), nOpProp);\r\n"
    "\t\r\n"
    "\t// [UILOREN-L1] nDetail = 0 (cung) roi vao case 0 doi nOption[0] == 53, ma\r\n"
    "\t// ca mang nOption bi memset 0 va moi dong doc GDI_ITEM_* con bi chu thich\r\n"
    "\t// => nut Kham nam LUON bao ReturnInfo 16 roi return, khong bao gio gui.\r\n"
    "\t// Dat -1 de roi vao default (chi break, khong return): moi cong kiem PHIA\r\n"
    "\t// CLIENT thanh no-op, may chu kiem that khi bam nut (KFoundryResDemand).\r\n"
    "\t// Cac cong con lai sau switch deu an toan voi -1: (-1 > 3) sai nen cong\r\n"
    "\t// nSeries :3570 khong kich hoat; bCanSet da TRUE nen cong :3588 khong kich\r\n"
    "\t// hoat; cong thieu tien giu nguyen.\r\n"
    "\tnDetail = -1;\r\n"
    "\tswitch(nDetail)\r\n"
)

# ------------------------------------------------------------------
# MIENG L2 - may trang thai 2 nhip gay o CA 6 pad (chon phuong an (b))
#
# Da doc that: PlayEffect() cua ca 6 lop bi chu thich than, luon "return 1"
#   One    :701-718     Two     :1131-1148   Three   :1607-1624
#   Distill:2159-2174   Forge   :2722-2735   Enchase :3123-3138
# => Breathe ket vinh vien o STATUS_*ING (vi can !PlayEffect()), nhanh
#    STATUS_CHANGING_ITEM (noi thiet ke goc goi Process* de GUI) khong bao
#    gio toi, va 3/2/1 anh Effect da Show thi khong bao gio Hide.
#   Breathe: One :670-697  Two :1100-1127  Three :1576-1603
#            Distill :2131-2156  Forge :2697-2719  Enchase :3095-3120
#
# Da doc that: STATUS_BEGIN_* CHI duoc gan o dung 1 cho moi lop - nhanh
# "nap" trong Process*; ctor gan STATUS_WAITING_MATERIALS (:556 :985 :1462
# :2017 :2598 :2981). Vay bo nhanh "nap" la Breathe khong bao gio vao
# BEGIN_* => khong Show anh (het treo anh) va moi cu bam = 1 yeu cau.
#
# 6 nhanh "nap" (KHONG phai 5: Enchase cung co, o :3623-3627 - hien bi
# mieng L1 chan truoc nen chua lo, nhung VA L1 xong la no song lai):
#   One     :837-841   -> LR_UI_MotOre  (:843)
#   Two     :1313-1317 -> LR_UI_HaiOre  (:1319)
#   Three   :1866-1870 -> LR_UI_BaOre   (:1871)   <-- khong co dong trong
#   Distill :2449-2453 -> LR_UI_Distill (:2455)
#   Forge   :2834-2838 -> LR_UI_Forge   (:2840)
#   Enchase :3623-3627 -> LR_UI_Enchase (:3629)
# Ca 6 khoi giong nhau tung byte (ke ca dong '\t{\t') nen moc neo phai kem
# dong OperationRequest co ten script rieng moi duy nhat.
# ------------------------------------------------------------------
ARM_OLD_TPL = (
    "\tif(m_nStatus == STATUS_WAITING_MATERIALS)\r\n"
    "\t{\t\r\n"
    "\t\tm_nStatus = STATUS_BEGIN_%s;\r\n"
    "\t\treturn;\r\n"
    "\t}\r\n"
)
ARM_NEW_TPL = (
    "\t// [UILOREN-L2-%s] bo nhip \"nap\". PlayEffect() luon tra 1 nen Breathe ket\r\n"
    "\t// o STATUS_%s, nhanh STATUS_CHANGING_ITEM (noi thiet ke goc goi\r\n"
    "\t// ham nay de GUI) khong bao gio toi: cu bam DAU chi \"nap\" roi bi nuot,\r\n"
    "\t// anh hieu ung da Show thi treo vinh vien. Bo nhanh nay thi Breathe khong\r\n"
    "\t// bao gio vao STATUS_BEGIN_%s (day la cho duy nhat gan no) nen vua het\r\n"
    "\t// treo anh vua giu dung \"1 cu bam = 1 lan ep\".\r\n"
    "\t//if(m_nStatus == STATUS_WAITING_MATERIALS)\r\n"
    "\t//{\t\r\n"
    "\t//\tm_nStatus = STATUS_BEGIN_%s;\r\n"
    "\t//\treturn;\r\n"
    "\t//}\r\n"
)
SEND_TPL = "\tg_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 7, (unsigned int)\"%s\");"

# (ma_pad, ten_STATUS_BEGIN_*, ten_STATUS_*ING, ten_script, dong_trong_truoc_lenh_gui)
PADS = [
    ("ONE",     "COMPOUND", "COMPOUNDING", "LR_UI_MotOre",  True),
    ("TWO",     "COMPOUND", "COMPOUNDING", "LR_UI_HaiOre",  True),
    ("THREE",   "COMPOUND", "COMPOUNDING", "LR_UI_BaOre",   False),
    ("DISTILL", "DISTILL",  "DISTILLING",  "LR_UI_Distill", True),
    ("FORGE",   "FORGE",    "GORGING",     "LR_UI_Forge",   True),
    ("ENCHASE", "ENCHASE",  "ENCHASING",   "LR_UI_Enchase", True),
]

# ------------------------------------------------------------------
# MIENG L4 - dong cua so chi thu hoi do cua THE DANG MO
# Da doc that CloseWindow :150-187: switch theo nWindow, moi the 1 goi.
# Doi the (ShowWindow :398-505) chi Hide/Show chu KHONG thu hoi do, nen do
# bo o the khac nam phong vo hinh khi dong cua so.
# Da doc that gate phia client: CoreShell.cpp:13023-13188 - GOI_RECOVER_ITEM
# dat BOOL bExistId = FALSE (:13026), quet phong tuong ung (6 case song:
# pos_compone :13047, pos_comptwo :13058, pos_compthree :13069,
# pos_distill :13080, pos_forge :13091, pos_enchase :13102) roi
# "if(!bExistId) break;" (:13182-13185) TRUOC RecoverItem + Send.
# => phong rong khong phat goi nao ra day. Gui du 6 la an toan.
# ------------------------------------------------------------------
L4_OLD = (
    "\t\tswitch(m_pSelf->nWindow)\r\n"
    "\t\t{\t\r\n"
    "\t\t\tcase WINDOWS_COMP:\r\n"
    "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compone, 0); \r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t\tcase WINDOWS_COMP2:\r\n"
    "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_comptwo, 0); \r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t\tcase WINDOWS_COMP3:\r\n"
    "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compthree, 0); \r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t\tcase WINDOWS_DISTILL:\r\n"
    "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_distill, 0);\r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t\tcase WINDOWS_FORGE:\r\n"
    "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_forge, 0); \r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t\tcase WINDOWS_ENCHASE:\r\n"
    "\t\t\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_enchase, 0); \r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t\tdefault:\r\n"
    "\t\t\t\tbreak;\r\n"
    "\t\t}\r\n"
)
L4_NEW = (
    "\t\t// [UILOREN-L4] dong cua so phai thu hoi do o CA 6 PHONG chu khong chi\r\n"
    "\t\t// phong cua the dang mo: doi the (ShowWindow) chi Hide/Show chu khong\r\n"
    "\t\t// thu hoi, nen do bo o the khac se nam ket trong phong vo hinh.\r\n"
    "\t\t// An toan: CoreShell.cpp GOI_RECOVER_ITEM co gate \"if(!bExistId) break;\"\r\n"
    "\t\t// (CoreShell.cpp:13182-13185) quet phong truoc khi gui, nen phong rong\r\n"
    "\t\t// khong phat goi thua ra may chu.\r\n"
    "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compone, 0);\r\n"
    "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_comptwo, 0);\r\n"
    "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_compthree, 0);\r\n"
    "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_distill, 0);\r\n"
    "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_forge, 0);\r\n"
    "\t\tg_pCoreShell->OperationRequest(GOI_RECOVER_ITEM, (unsigned int)pos_enchase, 0);\r\n"
)

# ------------------------------------------------------------------
# MIENG L5 - NPC mo lai khi cua so dang mo -> the sang lech noi dung
# Da doc that OpenWindow :119-146: ep CheckButton the Tinh luyen + UpdateData
# pad One (:130-135) nhung KHONG reset nWindow va KHONG goi ShowWindow, ma
# ShowWindow moi la cho Show/Hide than cac pad (:398-505).
# Cach it dong nhat va it rui ro nhat: CHEN THEM 2 dong sau :135, khong xoa
# dong nao (giu nguyen ca m_AtlasPadBtn.CheckButton(FALSE) - ShowWindow(0)
# khong dung toi nut Atlas vi 2 dong do bi chu thich :413-414).
# ShowWindow(WINDOWS_COMP) = case 0: Show pad One, CheckButton dung, Hide 5
# pad con lai, UpdateData.
# ------------------------------------------------------------------
L5_OLD = (
    "\t\tm_pSelf->m_CompoundOnePad.UpdateData();\r\n"
    "\t\tm_pSelf->BringToTop();\r\n"
)
L5_NEW = (
    "\t\tm_pSelf->m_CompoundOnePad.UpdateData();\r\n"
    "\t\t// [UILOREN-L5] NPC mo lai luc cua so dang mo: cu chi ep CheckButton the\r\n"
    "\t\t// Tinh luyen ma khong Show/Hide than pad va khong reset nWindow => dang\r\n"
    "\t\t// o the Trich lay thi nut sang \"Tinh luyen\" ma than van la Trich lay.\r\n"
    "\t\tKUiCompoundItem::ShowWindow(WINDOWS_COMP);\r\n"
    "\t\tm_pSelf->nWindow = WINDOWS_COMP;\r\n"
    "\t\tm_pSelf->BringToTop();\r\n"
)

# ------------------------------------------------------------------
# MIENG L6 - loi cung ho SetContainerId con 1 con
# Da doc that: KUiCompoundThree::LoadScheme (:1519) dung CtrlCompTwoItemMap
# tai :1529. Cac cho khac cua lop Three da dung dung CtrlCompThreeItemMap
# (:1948, :1990). Hai bang hien trung tung byte (:56-68) nen chua co hau qua
# - la min cho ngay ai do doi mot trong hai bang.
# ------------------------------------------------------------------
L6_OLD = (
    "\t\tKWndImage::Init(&Ini, \"Main\");\r\n"
    "\t\tfor (int i = 0; i < _ITEM_COMP_COUNT; i ++)\r\n"
    "\t\t{\r\n"
    "\t\t\tm_ItemBox[i].Init(&Ini, CtrlCompTwoItemMap[i].pIniSection);\r\n"
    "\t\t}\r\n"
    "\t\tm_Compound.Init(&Ini,\"CompoundBtn\");\r\n"
    "\t\tm_Cancel.Init(&Ini,\"CancleBtn\");\r\n"
    "\t\tm_Guide.Init(&Ini,\"GuideList\");\r\n"
    "\t\tm_ListScroll.Init(&Ini,\"GuideList_Scroll\");\r\n"
    "\t\t\r\n"
    "\t\tint nX,nY,nColor;\r\n"
    "\t\tIni.GetInteger2(\"Box_0\",\"Pos\",&nX,&nY);\r\n"
    "\r\n"
    "\t\tif (Ini.GetString(\"TextColor\", \"Font\", \"\", Buffer, sizeof(Buffer)))\r\n"
    "\t\t{\r\n"
    "\t\t\tnColor = (::GetColor(Buffer) & 0xFFFFFF);\r\n"
    "\t\t}\r\n"
    "\t\tm_nSelect = 0;\r\n"
)
L6_NEW = (
    "\t\tKWndImage::Init(&Ini, \"Main\");\r\n"
    "\t\tfor (int i = 0; i < _ITEM_COMP_COUNT; i ++)\r\n"
    "\t\t{\r\n"
    "\t\t\t// [UILOREN-L6] loi cung ho: lop Three ma tra bang cua lop Two. Hai bang\r\n"
    "\t\t\t// dang trung tung byte nen chua no, nhung day la min cho ngay doi bang.\r\n"
    "\t\t\tm_ItemBox[i].Init(&Ini, CtrlCompThreeItemMap[i].pIniSection);\r\n"
    "\t\t}\r\n"
    "\t\tm_Compound.Init(&Ini,\"CompoundBtn\");\r\n"
    "\t\tm_Cancel.Init(&Ini,\"CancleBtn\");\r\n"
    "\t\tm_Guide.Init(&Ini,\"GuideList\");\r\n"
    "\t\tm_ListScroll.Init(&Ini,\"GuideList_Scroll\");\r\n"
    "\t\t\r\n"
    "\t\tint nX,nY,nColor;\r\n"
    "\t\tIni.GetInteger2(\"Box_0\",\"Pos\",&nX,&nY);\r\n"
    "\r\n"
    "\t\tif (Ini.GetString(\"TextColor\", \"Font\", \"\", Buffer, sizeof(Buffer)))\r\n"
    "\t\t{\r\n"
    "\t\t\tnColor = (::GetColor(Buffer) & 0xFFFFFF);\r\n"
    "\t\t}\r\n"
    "\t\tm_nSelect = 0;\r\n"
)

# ------------------------------------------------------------------
# MIENG L7 - KUiDistill::ProcessDistill khoi "case 2:" thieu break
# Da doc that: case 2 :2314-2333 het khoi la roi THANG vao case 3 (:2334),
# trong khi ban Enchase tuong ung CO break (:3387-3389) va cac case anh em
# trong chinh switch nay deu co break (:2313, :2358) voi thut le 3 tab.
# Hien la ma chet vi nDetail bi ghim 0 (:2288 -> case 0: break), min chi no
# khi khoi phuc cac dong doc GDI_ITEM_DETAIL.
# ------------------------------------------------------------------
L7_OLD = (
    "\t\t\t\t\treturn;\r\n"
    "\t\t\t\t}\r\n"
    "\t\t\t}\r\n"
    "\t\tcase 3:\r\n"
)
L7_NEW = (
    "\t\t\t\t\treturn;\r\n"
    "\t\t\t\t}\r\n"
    "\t\t\t}\r\n"
    "\t\t\t// [UILOREN-L7] them break bi thieu (ban Enchase tuong ung co break).\r\n"
    "\t\t\tbreak;\r\n"
    "\t\tcase 3:\r\n"
)


def tao_danh_sach_mieng():
    ds = []
    ds.append(dict(
        ma="L1",
        mota="Kham nam: nDetail ghim 0 -> nut luon ra loi, khong bao gio gui",
        nhan="[UILOREN-L1]",
        cu=L1_OLD, moi=L1_NEW))

    for ma_pad, begin, ing, script, co_dong_trong in PADS:
        gap = "\t\r\n" if co_dong_trong else ""
        gui = SEND_TPL % script
        cu = (ARM_OLD_TPL % begin) + gap + gui
        moi = (ARM_NEW_TPL % (ma_pad, ing, begin, begin)) + gap + gui
        ds.append(dict(
            ma="L2-" + ma_pad,
            mota="pad %s: bo nhip 'nap' (cu bam dau bi nuot + treo anh hieu ung)" % ma_pad,
            nhan="[UILOREN-L2-%s]" % ma_pad,
            cu=cu, moi=moi))

    ds.append(dict(
        ma="L4",
        mota="CloseWindow: thu hoi do o ca 6 phong, khong chi the dang mo",
        nhan="[UILOREN-L4]",
        cu=L4_OLD, moi=L4_NEW))
    ds.append(dict(
        ma="L5",
        mota="OpenWindow: NPC mo lai -> the sang lech noi dung",
        nhan="[UILOREN-L5]",
        cu=L5_OLD, moi=L5_NEW))
    ds.append(dict(
        ma="L6",
        mota="KUiCompoundThree::LoadScheme tra nham CtrlCompTwoItemMap",
        nhan="[UILOREN-L6]",
        cu=L6_OLD, moi=L6_NEW))
    ds.append(dict(
        ma="L7",
        mota="KUiDistill::ProcessDistill case 2 thieu break",
        nhan="[UILOREN-L7]",
        cu=L7_OLD, moi=L7_NEW))
    return ds


def dem_eol(s):
    crlf = s.count("\r\n")
    lf_le = s.count("\n") - crlf
    cr_le = s.count("\r") - crlf
    return crlf, lf_le, cr_le


def main():
    ghi_that = "--ghi" in sys.argv[1:]
    print("=" * 78)
    print("vB_client_ui_loren.py - va 5 loi UI he lo ren")
    print("Che do: %s" % ("GHI THAT" if ghi_that else "DIEN TAP (khong ghi gi)"))
    print("Tep dich: %s" % DICH)
    print("=" * 78)

    if not os.path.isfile(DICH):
        print("LOI TO: khong thay tep dich. Khong ghi gi.")
        return 2

    goc = io.open(DICH, "rb").read().decode("latin-1")
    crlf0, lf0, cr0 = dem_eol(goc)
    print("Truoc : %d byte | CRLF=%d | LF le=%d | CR le=%d"
          % (len(goc), crlf0, lf0, cr0))
    print("-" * 78)

    ds = tao_danh_sach_mieng()
    noi_dung = goc
    so_ap = 0
    so_bo_qua = 0
    loi = []

    for m in ds:
        da_co = m["nhan"] in noi_dung
        n = noi_dung.count(m["cu"])
        if da_co:
            print("[%-11s] DA CO - bo qua   (nhan %s da nam trong tep)"
                  % (m["ma"], m["nhan"]))
            so_bo_qua += 1
            continue
        if n != 1:
            print("[%-11s] LOI TO: moc neo trung %d lan (can dung 1) - %s"
                  % (m["ma"], n, m["mota"]))
            loi.append(m["ma"])
            continue
        print("[%-11s] moc neo trung 1 lan - SE AP: %s" % (m["ma"], m["mota"]))
        noi_dung = noi_dung.replace(m["cu"], m["moi"], 1)
        so_ap += 1

    print("-" * 78)
    if loi:
        print("LOI TO: %d mieng khong neo duoc (%s). KHONG GHI GI CA LUOT."
              % (len(loi), ", ".join(loi)))
        return 3

    crlf1, lf1, cr1 = dem_eol(noi_dung)
    print("Sau   : %d byte | CRLF=%d | LF le=%d | CR le=%d"
          % (len(noi_dung), crlf1, lf1, cr1))
    print("Tong  : %d mieng se ap, %d mieng da co san" % (so_ap, so_bo_qua))

    if lf1 != lf0 or cr1 != cr0:
        print("LOI TO: EOL bi lech (LF le %d->%d, CR le %d->%d). KHONG GHI GI."
              % (lf0, lf1, cr0, cr1))
        return 4

    try:
        noi_dung.encode("latin-1")
    except UnicodeEncodeError:
        print("LOI TO: noi dung moi co ky tu ngoai latin-1 - se pha byte TCVN3."
              " KHONG GHI GI.")
        return 5

    if so_ap == 0:
        print("Khong co gi de ap. Khong ghi.")
        return 0

    if not ghi_that:
        print("DIEN TAP: khong ghi gi. Them --ghi de ghi that.")
        return 0

    sao = DICH + SAOLUU_DUOI
    if not os.path.exists(sao):
        f = io.open(sao, "wb")
        f.write(goc.encode("latin-1"))
        f.close()
        print("Da tao sao luu: %s" % sao)
    else:
        print("Sao luu da co san, giu nguyen: %s" % sao)

    f = io.open(DICH, "wb")
    f.write(noi_dung.encode("latin-1"))
    f.close()
    print("DA GHI %d mieng vao %s" % (so_ap, DICH))

    # doc lai tu dia de xac nhan (bay driver ghi dut nua chung)
    lai = io.open(DICH, "rb").read().decode("latin-1")
    if lai != noi_dung:
        print("LOI TO: doc lai tu dia KHAC voi noi dung dinh ghi!"
              " Phuc hoi tu %s ngay." % sao)
        return 6
    crlf2, lf2, cr2 = dem_eol(lai)
    print("Doc lai: %d byte | CRLF=%d | LF le=%d | CR le=%d - khop."
          % (len(lai), crlf2, lf2, cr2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
