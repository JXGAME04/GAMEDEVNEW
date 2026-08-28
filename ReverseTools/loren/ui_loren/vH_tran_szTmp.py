# -*- coding: utf-8 -*-
"""vH_tran_szTmp.py - VA DIEM SAP THAT cua "re chuot vao khoang Chu Sa = dis client".

DIEM SAP (doc ma, KItem.cpp:1500-1506, trong KItem::GetDesc ban 4 tham so :1228):

    char szIntor[64] = "!";
    KTabFile MagicTab;
    MagicTab.Load("\\Settings\\Item\\magicattriblevel_index.txt");
    char szTmp[8];                                        // <-- 8 BYTE
    sprintf(szTmp, "%d", m_aryBaseAttrib[0].nValue[0]);   // <-- int, toi da 11 ky tu

`%d` cua mot int co the dai 11 ky tu ("-2147483648") + NUL = 12 byte, TRAN bo dem
8 byte tren NGAN XEP -> pha khung stack -> sap CUNG, khong kip chay bo bat loi
(dung trieu chung: jx_crash.log chi co dong "game khoi dong", khong co ban ghi nao).

VI SAO GIA TRI LA RAC (goc thu hai, da va rieng o vG):
KItemSet::AddItemSet2 lay khe bang FindFree() ma KHONG lam sach KItem. Trong
Gen_MagicScript, `*pItem = *pMagicScript` chi gan phan CO BAN tu bang magicscript;
ba mang thuoc tinh (m_aryBaseAttrib[7], m_aryRequireAttrib[6], m_aryMagicAttrib[])
KHONG he bi dung toi, nen chung giu nguyen so lieu cua mon do TRUOC DO o khe do.
Voi khoang 199..204, duong ve mo ta doc thang m_aryBaseAttrib[0].nValue[0] ->
in ra so rac -> tran.
(vG da lam sach m_GeneratorParam nhung KHONG cham 3 mang nay, nen chua du.)

HAI MIENG:
  1. KItem.cpp    : noi szTmp[8] -> szTmp[16] (du cho moi gia tri int).
                    Day la lo hong TRAN THAT, phai va bat ke du lieu tu dau toi -
                    client khong duoc sap vi mot mon do co so lieu la.
  2. KItemSet.cpp : lam sach 3 mang thuoc tinh khi cap khe moi trong AddItemSet2,
                    dung cho ngay canh mieng ZeroMemory m_GeneratorParam cua vG.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_tranbuf lan dau).
"""
import io
import os
import shutil
import sys

HAU_TO = ".truoc_tranbuf"

MIENG = [
    (
        "M1  noi bo dem szTmp 8 -> 16 (KItem.cpp, cho tran that)",
        r"D:\GAMEDEVNEW\Sources\Core\Src\KItem.cpp",
        "\t\tchar szTmp[8];\n"
        "\t\tsprintf(szTmp,\"%d\",m_aryBaseAttrib[0].nValue[0]);\n",
        "\t\t// [LOREN 27/08] 8 byte KHONG du: \"%d\" cua mot int co the dai 11 ky tu\n"
        "\t\t// (\"-2147483648\") + NUL = 12. Khi m_aryBaseAttrib[0].nValue[0] mang so\n"
        "\t\t// la (khe vat pham dung lai, xem KItemSet::AddItemSet2) thi sprintf pha\n"
        "\t\t// khung stack ngay tai day -> client sap CUNG, bo bat loi khong kip ghi.\n"
        "\t\tchar szTmp[16];\n"
        "\t\tsprintf(szTmp,\"%d\",m_aryBaseAttrib[0].nValue[0]);\n",
    ),
    (
        "M2  lam sach 3 mang thuoc tinh khi cap khe moi (KItemSet.cpp)",
        r"D:\GAMEDEVNEW\Sources\Core\Src\KItemSet.cpp",
        "\tZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));\n",
        "\tZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));\n"
        "\t// [LOREN 27/08] Ba mang thuoc tinh cung phai sach: Gen_MagicScript chi lam\n"
        "\t// `*pItem = *pMagicScript` (gan phan co ban tu bang), KHONG dung toi chung,\n"
        "\t// nen chung se giu so lieu cua mon do truoc do o khe nay. Duong ve mo ta\n"
        "\t// khoang 199..204 doc thang m_aryBaseAttrib[0].nValue[0] (KItem.cpp:1504).\n"
        "\tZeroMemory(pItem->m_aryBaseAttrib, sizeof(pItem->m_aryBaseAttrib));\n"
        "\tZeroMemory(pItem->m_aryRequireAttrib, sizeof(pItem->m_aryRequireAttrib));\n"
        "\tZeroMemory(pItem->m_aryMagicAttrib, sizeof(pItem->m_aryMagicAttrib));\n",
    ),
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vH_tran_szTmp - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))
    ke_hoach = []

    for ten, dich, cu, moi_khoi in MIENG:
        print("\n--- %s" % ten)
        if not os.path.isfile(dich):
            print("!!! LOI TO: khong thay %s" % dich)
            return 1
        raw = io.open(dich, "rb").read().decode("latin-1")
        eol = "\r\n" if "\r\n" in raw else "\n"
        neo = cu.replace("\n", eol)
        moi = moi_khoi.replace("\n", eol)

        if moi.split(eol)[0].strip().startswith("// [LOREN 27/08]") and moi.split(eol)[1] in raw:
            pass
        if "[LOREN 27/08] 8 byte KHONG du" in raw and dich.endswith("KItem.cpp"):
            print("  DA CO - bo qua"); continue
        if "[LOREN 27/08] Ba mang thuoc tinh cung phai sach" in raw and dich.endswith("KItemSet.cpp"):
            print("  DA CO - bo qua"); continue

        dem = raw.count(neo)
        if dem != 1:
            print("!!! LOI TO: moc neo xuat hien %d lan (can dung 1) - KHONG ghi gi" % dem)
            return 1
        hi_truoc = sum(1 for c in raw if ord(c) > 127)
        moi_raw = raw.replace(neo, moi, 1)
        hi_sau = sum(1 for c in moi_raw if ord(c) > 127)
        if hi_sau != hi_truoc:
            print("!!! LOI TO: byte cao %d -> %d" % (hi_truoc, hi_sau))
            return 1
        print("  moc neo trung 1 lan | byte cao %d (khong doi) | CRLF %d -> %d"
              % (hi_truoc, raw.count("\r\n"), moi_raw.count("\r\n")))
        ke_hoach.append((dich, moi_raw))

    if not ke_hoach:
        print("\nKhong co gi de ap.")
        return 0
    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for dich, noi_dung in ke_hoach:
        sao = dich + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(dich, sao)
            print("  sao luu -> %s" % sao)
        with io.open(dich, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        if io.open(dich, "rb").read().decode("latin-1") != noi_dung:
            print("!!! LOI TO: doc lai tu dia KHONG khop: %s" % dich)
            return 1
        print("  DA GHI %s" % dich)
    print("\n  => build lai Core CA HAI cau hinh (tep dung chung client/server)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
