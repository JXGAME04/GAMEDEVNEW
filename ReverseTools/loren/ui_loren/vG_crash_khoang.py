# -*- coding: utf-8 -*-
"""vG_crash_khoang.py - VA GOC "re chuot vao khoang thuoc tinh = sap client".

TRIEU CHUNG (chu game bao 27/08 chieu): lay khoang bang bo test lo ren, re chuot
vao vien Chu Sa (6/1/203) la Game.exe sap ngay. Sua tham so Lua (nMagLvl1 1 -> 0)
KHONG chua duoc - dung luat "trieu chung lap sau va = gia thuyet sai".

GOC THAT (doc ma, khong doan):
  KItemSet::AddItemSet2 (KItemSet.cpp:202-209) lay khe vat pham bang FindFree()
  roi CHI dat 2 truong:
        pItem->m_GeneratorParam.nVersion   = nVersion;
        pItem->m_GeneratorParam.uRandomSeed = nRandomSeed;
  Phan con lai cua m_GeneratorParam (nGeneratorLevel[0..5], nLuck, ...) van GIU
  NGUYEN gia tri cua vat pham CU tung nam o khe do - tuc la RAC.

  Truoc dot lo ren dieu do vo hai vi Gen_MagicScript mo dau bang
        ZeroMemory(&pItem->m_GeneratorParam, sizeof(...));
  xoa sach tat ca.

  Dot lo ren 27/08 (v17_giu_tham_so_khi_nap_csdl) them vao Gen_MagicScript:
        KItemGeneratorParam sGiuParam = pItem->m_GeneratorParam;   // <-- chep RAC
        ZeroMemory(&pItem->m_GeneratorParam, ...);
        if (sLaNguyenLieuLoRen(nDetailType, nParticularType))
            pItem->m_GeneratorParam = sGiuParam;                   // <-- ghi RAC tro lai
  Y dinh dung (giu ma phep cua khoang khi NAP TU CSDL - KPlayerDBFuns.cpp:694,
  cho do NewItem co du lieu that), nhung no ap cho MOI duong goi, ke ca duong
  SINH MOI qua AddItemSet2 noi khe chua he duoc lam sach.

  => khoang 146 / 148..153 / 199..204 / 3769,3770 sinh ra mang nGeneratorLevel[]
  la rac cua mon do truoc do. Ve mo ta doc rac -> sap. Giai thich dung ca ba
  dieu quan sat duoc: chi khoang moi sap (chi 3 dai ma do duoc hoi rac), sap
  ngau nhien (tuy khe tai su dung chua gi), va sua tham so Lua khong an thua.

MIENG VA (toi thieu, dung gocmo): lam sach m_GeneratorParam ngay trong
AddItemSet2 TRUOC khi dat nVersion/uRandomSeed. Sau mieng nay:
  - duong SINH MOI: khe sach -> sGiuParam = 0 -> hoi lai 0 -> y het hanh vi
    truoc dot lo ren, khong con rac;
  - duong NAP TU CSDL (KPlayerDBFuns): KHONG di qua AddItemSet2 nen v17 van
    giu duoc ma phep that - dung y dinh ban dau, khong pha gi.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_crashkhoang lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemSet.cpp"
HAU_TO = ".truoc_crashkhoang"

NEO = (
    "\tKItem*\tpItem = &Item[i];\n"
    "\tpItem->m_GeneratorParam.nVersion = nVersion;\n"
    "\tpItem->m_GeneratorParam.uRandomSeed = nRandomSeed;\n"
)

MOI = (
    "\tKItem*\tpItem = &Item[i];\n"
    "\t// [LOREN 27/08] FindFree() tra ve mot khe DUNG LAI: m_GeneratorParam con\n"
    "\t// nguyen so lieu cua mon do truoc do. Truoc day vo hai vi Gen_* deu\n"
    "\t// ZeroMemory khoi nay, nhung Gen_MagicScript nay GIU LAI khoi do cho\n"
    "\t// nguyen lieu lo ren (de khong mat ma phep khi nap tu CSDL), nen rac se\n"
    "\t// chay thang vao vat pham moi va lam client sap luc ve mo ta.\n"
    "\t// Lam sach o day: duong nap tu CSDL (KPlayerDBFuns) khong di qua ham nay\n"
    "\t// nen van giu duoc so lieu that cua no.\n"
    "\tZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));\n"
    "\tpItem->m_GeneratorParam.nVersion = nVersion;\n"
    "\tpItem->m_GeneratorParam.uRandomSeed = nRandomSeed;\n"
)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vG_crash_khoang - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)
    crlf_truoc = raw.count("\r\n")

    if "[LOREN 27/08] FindFree() tra ve mot khe DUNG LAI" in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    neo = NEO.replace("\n", eol)
    moi_khoi = MOI.replace("\n", eol)

    dem = raw.count(neo)
    if dem != 1:
        print("!!! LOI TO: moc neo xuat hien %d lan (can dung 1) - KHONG ghi gi" % dem)
        return 1
    print("  moc neo trung 1 lan (KItemSet::AddItemSet2)")

    moi = raw.replace(neo, moi_khoi, 1)

    hi_sau = sum(1 for c in moi if ord(c) > 127)
    if hi_sau != hi_truoc:
        print("!!! LOI TO: byte cao %d -> %d (le ra khong doi)" % (hi_truoc, hi_sau))
        return 1
    print("  byte cao %d -> %d (khong doi)   CRLF %d -> %d   EOL=%s"
          % (hi_truoc, hi_sau, crlf_truoc, moi.count("\r\n"),
             "CRLF" if eol == "\r\n" else "LF"))

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
    print("\n  => PHAI BUILD LAI CA HAI: Core 'Server Release|x64' VA 'Client Release|Win32'")
    print("     (KItemSet.cpp la tep DUNG CHUNG client/server)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
