# -*- coding: utf-8 -*-
"""vO_dongbo_nature_tim.py - Giu PHAM CHAT TIM tren ba duong dong bo cua client,
de vong sang tim hien ca khi XEM DO NGUOI KHAC / XEM SAP / KHOE DO TRONG CHAT.

BOI CANH: vong sang cua JX1 ve 100% BANG MA (S3Client\\Ui\\Elem\\vongsang.cpp -
DrawBorder chay quanh + DrawBorder2 nhap nhay), khong dung anh spr nao; nhanh mau
tim (`ehuyenkim`, bang l_PurpleBorder) da co san. Cong bat la
`GetNatureItem(...) == purple_item` -> `KItem::GetColorItem()` doc
`m_CommonAttrib.nItemNature`. Dot truoc da them nhanh NATURE_VIOLET vao
GetColorItem/GetKind nen TUI DO CUA CHINH MINH da hien vong sang.

CON THIEU: ba duong client DUNG LAI vat pham cua NGUOI KHAC deu danh roi
nItemNature, nen sang tui minh thi co vong sang ma sang mat nguoi khac thi mat:

    KViewItem.cpp:133   xem trang bi nguoi khac
    KSellItem.cpp:171   xem sap bay ban
    CoreShell.cpp:2283  khoe do trong chat (GDI_ITEM_CHAT)

Du lieu DA CO SAN tren duong truyen, may chu da dien day du:
    SViewItemInfo.m_nNature (KProtocol.h:2195) - dien tai KPlayer.cpp:6540, :10646
    ChatItem.m_nNature      (GameDataDef.h:613) - dien tai CoreShell.cpp:2335
Chi phia client quen nap lai.

🔴 VI SAO KHONG DOI DIEU KIEN `>= NATURE_GOLD` THANH `> NATURE_NORMAL`
(cach nhin thi tuong la don gian hon, nhung SAI):
Duong GUI ma hoa truong `m_btDetail` KHAC NHAU theo pham chat -
CoreShell.cpp:2319:
    if (GetNature() >= NATURE_GOLD && m_btDetail != 99)
        pInfo->m_btDetail = Item[nIdx].GetRow();     // CHI SO DONG goldequip
    else
        pInfo->m_btDetail = Item[nIdx].GetDetailType();  // LOAI vat pham
Do tim (NATURE_VIOLET = 1 < NATURE_GOLD = 2) duoc gui bang GetDetailType().
Neu noi dieu kien ben nhan, do tim se roi vao nhanh `ItemSet.Add(nature, ...)` -
nhanh danh cho do Hoang Kim, noi tham so `detail` duoc hieu la CHI SO DONG -
=> dung sai vat pham. Vi vay GIU NGUYEN duong tao, chi GAN LAI pham chat sau khi
vat pham da dung xong. Vong sang chi doc nItemNature nen the la du.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_natureim lan dau).
"""
import io
import os
import shutil
import sys

HAU_TO = ".truoc_naturetim"
CHU = (
    "\t// [LOREN 27/08] Giu PHAM CHAT de vong sang tim hien ca o day. Duong tao\n"
    "\t// tren khong mang nItemNature sang (do tim di nhanh AddItemSet2), nhung\n"
    "\t// may chu DA gui du trong m_nNature. Khong noi dieu kien >= NATURE_GOLD\n"
    "\t// vi truong m_btDetail duoc ma hoa khac nhau theo pham chat\n"
    "\t// (CoreShell.cpp:2319) - noi ra se dung sai vat pham.\n"
)

MIENG = [
    ("M1 KViewItem (xem do nguoi khac)",
     r"D:\GAMEDEVNEW\Sources\Core\Src\KViewItem.cpp",
     "\t\tItem[nItemIdx].SetID(pView->m_sInfo[i].m_nID);\n",
     CHU.replace("\n\t//", "\n\t\t//").replace("\t// [LOREN", "\t\t// [LOREN")
     + "\t\tItem[nItemIdx].SetNature(pView->m_sInfo[i].m_nNature);\n"
       "\t\tItem[nItemIdx].SetID(pView->m_sInfo[i].m_nID);\n"),

    ("M2 KSellItem (xem sap bay ban)",
     r"D:\GAMEDEVNEW\Sources\Core\Src\KSellItem.cpp",
     "\t\t\tItem[nItemIdx].SetID(pView->m_sInfo[i].m_nID);\n",
     CHU.replace("\n\t//", "\n\t\t\t//").replace("\t// [LOREN", "\t\t\t// [LOREN")
     + "\t\t\tItem[nItemIdx].SetNature(pView->m_sInfo[i].m_nNature);\n"
       "\t\t\tItem[nItemIdx].SetID(pView->m_sInfo[i].m_nID);\n"),

    ("M3 CoreShell (khoe do trong chat)",
     r"D:\GAMEDEVNEW\Sources\Core\Src\CoreShell.cpp",
     "\t\t\t\t\t\tItem[nItemIdx].SetID(m_sInfo->m_nID);\n",
     CHU.replace("\n\t//", "\n\t\t\t\t\t\t//").replace("\t// [LOREN", "\t\t\t\t\t\t// [LOREN")
     + "\t\t\t\t\t\tItem[nItemIdx].SetNature(m_sInfo->m_nNature);\n"
       "\t\t\t\t\t\tItem[nItemIdx].SetID(m_sInfo->m_nID);\n"),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vO_dongbo_nature_tim - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    ke_hoach = []

    for ten, dich, cu, thay in MIENG:
        print("\n--- %s" % ten)
        if not os.path.isfile(dich):
            print("!!! LOI TO: khong thay %s" % dich); return 1
        raw = io.open(dich, "rb").read().decode("latin-1")
        eol = "\r\n" if "\r\n" in raw else "\n"
        if "[LOREN 27/08] Giu PHAM CHAT de vong sang tim" in raw:
            print("  DA CO - bo qua"); continue
        cu_e = cu.replace("\n", eol)
        dem = raw.count(cu_e)
        if dem != 1:
            print("!!! LOI TO: moc neo xuat hien %d lan (can 1) - KHONG ghi gi" % dem); return 1
        moi = raw.replace(cu_e, thay.replace("\n", eol), 1)
        hi0 = sum(1 for c in raw if ord(c) > 127)
        hi1 = sum(1 for c in moi if ord(c) > 127)
        if hi0 != hi1:
            print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1)); return 1
        print("  moc neo trung 1 lan | byte cao %d (khong doi) | CRLF %d -> %d"
              % (hi0, raw.count("\r\n"), moi.count("\r\n")))
        ke_hoach.append((dich, moi))

    if not ke_hoach:
        print("\nKhong co gi de ap."); return 0
    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that."); return 0

    for dich, noi_dung in ke_hoach:
        sao = dich + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(dich, sao); print("  sao luu -> %s" % sao)
        with io.open(dich, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        if io.open(dich, "rb").read().decode("latin-1") != noi_dung:
            print("!!! LOI TO: doc lai KHONG khop: %s" % dich); return 1
        print("  DA GHI %s" % os.path.basename(dich))
    print("\n  => build Core CA HAI cau hinh")
    return 0


if __name__ == "__main__":
    sys.exit(main())
