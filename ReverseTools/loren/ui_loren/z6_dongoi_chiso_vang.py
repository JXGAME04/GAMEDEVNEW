# -*- coding: utf-8 -*-
"""z6_dongoi_chiso_vang.py - VA "xep do / qua tran / restart xong mon vang-tim ep
thanh ITEM LA (doi sach dong phep, chi so)".

GOC (doc tan ma, da loai 2 gia thuyet truoc bang doi chieu):
  - He do vang GOLD2 tai lap chi so qua CO "custom magic":
    Gen_GoldEquipment (KItemGenerator.CPP:1818) va Gen_PlatinaEquipment (:1931)
    CHI doc lai chi so that khi nGeneratorLevel[MAX_ITEM_MAGICATTRIB] != 0
    (type o [0..5], MAKELONG(value0, value2) o [6..11]); bang khong thi
    RANDOM lai tu bang goldequipmagic (:1830-1867).
  - Do vang HE THONG (award/Lua) sau khi cap deu duoc dong goi qua
    LuaSetMagicAttrib (ScriptFuns.cpp:6455-6474) => ben qua save/load.
  - LuaCmp_AddItemEx (lo ren) KHONG dong goi => o [6] = 0:
      * TRONG PHIEN: xep do / doi map lam client huy + dung lai item tu goi
        ITEM_SYNC -> client chay Gen voi chuoi random RIENG cua client
        (g_RandomSeed nhanh #else :1779) -> hien dong phep KHAC server
        => "xep do xong ra item la" (su co thienho 28/08 dem, sau tran TK).
      * QUA RESTART: load roledb (KPlayerDBFuns :641) cung roi nhanh random
        -> doi ruot lan nua.
  - Ten/hinh mon van dung (Row luu ben, hang 0 = Mong Long Tang Mao da doi
    chieu) - chi phan ruot khong duoc ghim.

MIENG VA: trong LuaCmp_AddItemEx, SAU khi item da co m_aryMagicAttrib cuoi
cung (sau nhanh Gen_ExistEquipment tai sinh theo seed), voi trang bi
HOANG KIM / BACH KIM: chep nguoc chi so that vao nGeneratorLevel[0..11]
y khuon LuaSetMagicAttrib. Duong kham nam / do thuong (quality khac) giu
nguyen - chung tai lap bang seed, khong dung co nay.

Tep dich: Sources\\Core\\Src\\KItemCompound.cpp => build CA HAI cau hinh.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_dongoi lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[XEPDO2 28/08]"
DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp"
HAU_TO = ".truoc_dongoi"

CU = [
    T + "if (nGenre == item_equip && uRandSeed != 0)",
    T + "{",
    T*2 + "// Hat khac 0 = \"tai sinh dung mon nay\". Ban Linux cung re nhanh theo",
    T*2 + "// dung dieu kien nay (0x0811F46C test eax,eax / 0x0811F50A jne).",
    T*2 + "ItemGen.Gen_ExistEquipment(nNature, nDetailType, nParticular, nSeries,",
    T*3 + "\t\t\t\t   nLevel, nMagicLevel, nLuck, nItemVer, pNew);",
    T*2 + "pNew->m_GeneratorParam.nVersion = nItemVer;",
    T*2 + "pNew->m_GeneratorParam.uRandomSeed = uRandSeed;",
    T + "}",
]

MOI = CU + [
    "",
    T + "// " + NHAN + " DONG GOI chi so that cho trang bi HOANG KIM / BACH KIM.",
    T + "// He GOLD2 chi tai lap duoc dong phep khi nGeneratorLevel[6] != 0 (co",
    T + "// \"custom magic\": type o [0..5], MAKELONG(value0, value2) o [6..11] -",
    T + "// Gen_GoldEquipment KItemGenerator.CPP:1818 / Platina :1931; do vang he",
    T + "// thong duoc dong goi qua LuaSetMagicAttrib ScriptFuns.cpp:6455 nen ben).",
    T + "// Thieu buoc nay: moi lan item duoc DUNG LAI (client xep do / doi map,",
    T + "// server load roledb sau restart) la roi ve nhanh RANDOM-tu-bang ->",
    T + "// mon ep giu ten/hinh nhung doi sach ruot (\"item la\" - su co 28/08).",
    T + "if (nGenre == item_equip &&",
    T*2 + "(nQuality == ITEMQUALITY_GOLD || nQuality == ITEMQUALITY_PLATINA))",
    T + "{",
    T*2 + "int nGoi;",
    T*2 + "for (nGoi = 0; nGoi < MAX_ITEM_MAGICATTRIB; nGoi++)",
    T*2 + "{",
    T*3 + "if (pNew->m_aryMagicAttrib[nGoi].nAttribType <= 0)",
    T*4 + "break;",
    T*3 + "pNew->m_GeneratorParam.nGeneratorLevel[nGoi] =",
    T*4 + "pNew->m_aryMagicAttrib[nGoi].nAttribType;",
    T*3 + "pNew->m_GeneratorParam.nGeneratorLevel[nGoi + MAX_ITEM_MAGICATTRIB] =",
    T*4 + "MAKELONG(pNew->m_aryMagicAttrib[nGoi].nValue[0],",
    T*5 + "\t pNew->m_aryMagicAttrib[nGoi].nValue[2]);",
    T*2 + "}",
    T*2 + "for (; nGoi < MAX_ITEM_MAGICATTRIB; nGoi++)",
    T*2 + "{",
    T*3 + "pNew->m_GeneratorParam.nGeneratorLevel[nGoi] = 0;",
    T*3 + "pNew->m_GeneratorParam.nGeneratorLevel[nGoi + MAX_ITEM_MAGICATTRIB] = 0;",
    T*2 + "}",
    T + "}",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== z6_dongoi_chiso_vang - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    d = raw.split(eol)
    # so bang strip() (dong :1466 goc can le bang tab+space pha tron)
    vt = [i for i in range(len(d) - len(CU) + 1)
          if [x.strip() for x in d[i:i + len(CU)]] == [x.strip() for x in CU]]
    if len(vt) != 1:
        print("!!! LOI TO: neo khop %d lan (can 1)" % len(vt))
        for i, l in enumerate(d):
            if "Gen_ExistEquipment(nNature" in l:
                print("    dong %d: %r" % (i + 1, l[:90]))
        return 1
    i = vt[0]
    d[i:i + len(CU)] = MOI
    print("  ok  chen khoi dong goi sau nhanh tai sinh (dong %d, +%d dong)"
          % (i + 1, len(MOI) - len(CU)))

    nd = eol.join(d)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if (nd.count("{") - raw.count("{")) != (nd.count("}") - raw.count("}")):
        print("!!! LOI TO: ngoac lech")
        return 1
    print("  chot: byte cao %d (khong doi), ngoac can" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % os.path.basename(sao))
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. => build CA HAI cau hinh, dat canh cho chu swap.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
