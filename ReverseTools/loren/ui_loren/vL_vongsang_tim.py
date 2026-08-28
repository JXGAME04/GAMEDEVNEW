# -*- coding: utf-8 -*-
"""vL_vongsang_tim.py - BAT VONG SANG TIM cho trang bi do lo ren duc ra.

TRIEU CHUNG (chu game): "do tim khong hien thi vong sang mau tim chay quanh
trang bi nhu ban linux".

DA DOC MA - HE VONG SANG CUA CLIENT DA CO DAY DU, khong phai viet moi:
  S3Client\\Ui\\Elem\\WndObjContainer.cpp:475 `KWndObjectMatrix::PaintWindow()`
  (chu thich san: "edit by phong kieu vong sang item"), doan :553-574 ve vong
  sang theo `g_pCoreShell->GetNatureItem(uId, uGenre)`:
        gold_item     -> DrawBorder2/DrawBorder(... ehoangkim)
        purple_item   -> ... ehuyenkim      <-- NHANH CHO DO TIM, DA CO SAN
        green_item    -> ... eblue
        platinum_item -> ... ebachkim

MAT XICH THIEU: `KCoreShell::GetNatureItem` (CoreShell.cpp:20287) goi
`KItem::GetColorItem()` (KItem.cpp:3350), ma ham do nhan biet do tim bang:
        else if (IsPurple())            // KItem.cpp:3360
va `KItem::IsPurple()` (KItem.cpp:3229) chi la:
        return m_CommonAttrib.nPoint;
tuc do tim KIEU CU cua JX1 (nhan bang truong nPoint). Trang bi do LO REN duc ra
mang `m_CommonAttrib.nItemNature == NATURE_VIOLET` va nPoint = 0 => khong lot
dieu kien => khong ve vong sang.

VI SAO KHONG SUA IsPurple(): no duoc dung o 18 cho, trong do
`KPlayerDBFuns.cpp:1061  pItemData->ipoint = Item[nItemIndex].IsPurple();`
GHI THANG VAO CO SO DU LIEU (truong ipoint). Doi no la doi du lieu nguoi choi -
tuyet doi khong dung toi.

MIENG VA (toi thieu, dung cho): them nhanh NATURE_VIOLET vao HAI ham MAU:
  1. KItem::GetColorItem()  - chi phuc vu mau/vong sang:
       CoreShell.cpp:20284/20292/20302/20306 (GetNatureItem -> ve vong sang) va
       6 cho kiem `GetColorItem() > green_item` (CoreShell.cpp:3663, 3863, 4604,
       15878, 15987, 16450) de CHAN vut/ban nham do quy - do tim duoc bao ve
       theo, dung nhu mong doi.
  2. KItem::GetKind() - cung ho, dung khi xep tui (KInventory.cpp:513).
Dat nhanh moi SAU kiem gold/platina va TRUOC IsPurple() de khong doi hanh vi cu.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_vongsang lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItem.cpp"
HAU_TO = ".truoc_vongsang"

NHANH = (
    "\t// [LOREN 27/08] Trang bi do LO REN duc ra danh dau bang nItemNature =\n"
    "\t// NATURE_VIOLET chu khong phai nPoint (do tim kieu cu). Thieu nhanh nay\n"
    "\t// thi GetNatureItem tra ve normal_item va KWndObjectMatrix::PaintWindow\n"
    "\t// (WndObjContainer.cpp:560) khong ve vong sang tim.\n"
    "\t// KHONG sua IsPurple(): no ghi vao CSDL qua KPlayerDBFuns.cpp:1061.\n"
    "\telse if (GetNature() == NATURE_VIOLET)\n"
    "\t{\n"
    "\t\treturn purple_item;\n"
    "\t}\n"
)

# GetColorItem: chen truoc `else if (IsPurple())` cua ham nay
NEO_COLOR = (
    "\tif (GetNature() == NATURE_PLATINA)\n"
    "\t{\n"
    "\t\treturn platinum_item;\n"
    "\t}\n"
    "\telse if (IsPurple())\n"
)
MOI_COLOR = (
    "\tif (GetNature() == NATURE_PLATINA)\n"
    "\t{\n"
    "\t\treturn platinum_item;\n"
    "\t}\n"
    + NHANH +
    "\telse if (IsPurple())\n"
)

# GetKind: chen sau nhanh gold
NEO_KIND = (
    "\tif (GetGoldId() || GetNature() >= NATURE_GOLD)\n"
    "\t{\n"
    "\t\treturn gold_item;\n"
    "\t}\n"
    "\telse if (IsPurple())\n"
)
MOI_KIND = (
    "\tif (GetGoldId() || GetNature() >= NATURE_GOLD)\n"
    "\t{\n"
    "\t\treturn gold_item;\n"
    "\t}\n"
    + NHANH +
    "\telse if (IsPurple())\n"
)

MIENG = [
    ("M1 GetColorItem (vong sang + bao ve do quy)", NEO_COLOR, MOI_COLOR),
    ("M2 GetKind (xep tui)", NEO_KIND, MOI_KIND),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vL_vongsang_tim - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] Trang bi do LO REN duc ra danh dau" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    moi = raw
    for ten, cu, thay in MIENG:
        cu_e = cu.replace("\n", eol)
        thay_e = thay.replace("\n", eol)
        dem = moi.count(cu_e)
        if dem != 1:
            print("!!! LOI TO: %s - moc neo xuat hien %d lan (can 1) - KHONG ghi gi" % (ten, dem))
            return 1
        moi = moi.replace(cu_e, thay_e, 1)
        print("  %s: moc neo trung 1 lan" % ten)

    hi_sau = sum(1 for c in moi if ord(c) > 127)
    if hi_sau != hi_truoc:
        print("!!! LOI TO: byte cao %d -> %d" % (hi_truoc, hi_sau))
        return 1
    print("  byte cao %d (khong doi) | CRLF %d -> %d"
          % (hi_truoc, raw.count("\r\n"), moi.count("\r\n")))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai tu dia KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build Core CA HAI cau hinh (KItem.cpp dung chung client/server)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
