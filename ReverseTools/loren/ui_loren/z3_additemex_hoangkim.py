# -*- coding: utf-8 -*-
"""z3_additemex_hoangkim.py - VA "ep xong mat het do khong ra gi" (LOI CUA TOI).

CHUOI SU CO (do bang chung, khong doan):
  1. ScriptError 16:23:56: `ScriptError 10. (\\script\\item\\compound\\atlas.lua)`
     dung luc chu game bam Ket hop; client bao "Lo ren gap loi khong ro"
     (= nResult nil/-1, ngoai bang LR_UI_TBKQ).
  2. Cong thuc Do pho hang 2: DES_QUALITY=1 (Hoang Kim), DES_GENRE=0,
     DES_DETAILTYPE=1, con DES_PARTICULAR / DES_LEVEL / DES_SERIES DE TRONG.
     `LuaTabFile_GetCell` (ScriptFuns.cpp:3130) tra GIA TRI MAC DINH -1 cho o
     trong => genDesItemsInfo dung info = {ver,0, 1, 0, 0(=1-1), -1, -1, -1, ...}.
  3. defFinalCompound (compound_header.lua:108):
        writeCompoundLog  -> chay qua (toan so, -1 in binh thuong)
        removeItems x2    -> NGUYEN LIEU BI XOA (dung thu tu ban goc)
        addItemByInfo     -> AddItemEx(..., quality=1, genre=0, detail=0,
                             particular=-1, level=-1, ...)
  4. AddItemEx vuong CHINH CHOT "ma am" toi them truoc do:
        if (nDetailType < 0 || nParticular < 0 || nLevel < 0) -> tu choi
     => tra 0 => khong tao mon => "khong ro" => DO MAT TRANG.

VI SAO -1 LA HOP LE o day: `Gen_Equipment(nItemNature,...)`
(KItemGenerator.CPP:1611) voi NATURE_GOLD goi thang
`Gen_GoldEquipment(nDetailType, ...)` - danh tinh trang bi Hoang Kim la CHI SO
BANG goldequip nam trong nDetailType (atlas.lua:176 da tru 1 san),
particular/level/series KHONG DUOC DUNG. Chot "ma am" cua toi viet cho do
thuong, chan oan duong Hoang Kim/Bach Kim.

MIENG VA (toi thieu): voi trang bi HOANG KIM / BACH KIM (genre equip + quality
GOLD/PLATINA theo ma Linux), kep cac ma am ve 0 TRUOC chot; moi duong khac giu
nguyen chot nhu cu.

Tep dich: Sources\\Core\\Src\\KItemCompound.cpp => build Core CA HAI cau hinh.
Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_hkfix lan dau).
"""
import io
import os
import shutil
import sys

T = "\t"
NHAN = "[LOREN 28/08] HOANG KIM: ma am la hop le"
DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp"
HAU_TO = ".truoc_hkfix"

CU = [
    T + "if (nDetailType < 0 || nParticular < 0 || nLevel < 0)",
    T + "{",
    T*2 + "printf(\"[LOREN] AddItemEx: ma am (%d/%d/%d), tu choi\\n\",",
    T*2 + "\t   nDetailType, nParticular, nLevel);",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
]

MOI = [
    T + "// " + NHAN + ".",
    T + "// Cong thuc Do pho ghi mon RA bang CHI SO BANG goldequip trong",
    T + "// DES_DETAILTYPE (atlas.lua:176 da tru 1); particular/level/series de",
    T + "// trong = -1 va KHONG duoc dung: Gen_Equipment (KItemGenerator.CPP:1611)",
    T + "// voi NATURE_GOLD goi thang Gen_GoldEquipment(nDetailType, ...).",
    T + "// Chot \"ma am\" phia duoi viet cho do thuong, tung chan oan duong nay:",
    T + "// defFinalCompound da removeItems XONG moi goi den day, tu choi o day la",
    T + "// nguyen lieu mat trang ma khong ra mon (su co 28/08 cua chu game).",
    T + "if (nGenre == item_equip &&",
    T*2 + "(nQuality == ITEMQUALITY_GOLD || nQuality == ITEMQUALITY_PLATINA))",
    T + "{",
    T*2 + "if (nParticular < 0)",
    T*3 + "nParticular = 0;",
    T*2 + "if (nLevel < 0)",
    T*3 + "nLevel = 0;",
    T*2 + "if (nSeries < 0)",
    T*3 + "nSeries = 0;",
    T + "}",
    T + "if (nDetailType < 0 || nParticular < 0 || nLevel < 0)",
    T + "{",
    T*2 + "printf(\"[LOREN] AddItemEx: ma am (%d/%d/%d), tu choi\\n\",",
    T*2 + "\t   nDetailType, nParticular, nLevel);",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== z3_additemex_hoangkim - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n") - raw.count("\r\n")) else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)
    vt = [i for i in range(len(dong) - len(CU) + 1) if dong[i:i + len(CU)] == CU]
    if len(vt) != 1:
        print("!!! LOI TO: chot ma am khop %d lan (can 1)" % len(vt))
        for i, l in enumerate(dong):
            if "ma am" in l:
                print("    dong %d: %r" % (i + 1, l[:88]))
        return 1
    i = vt[0]
    print("  ok  noi long chot cho HOANG KIM/BACH KIM (dong %d, +%d dong)"
          % (i + 1, len(MOI) - len(CU)))
    dong = dong[:i] + MOI + dong[i + len(CU):]

    nd = eol.join(dong)
    if sum(1 for c in nd if ord(c) > 127) != hi0:
        print("!!! LOI TO: byte cao doi")
        return 1
    if nd.count("{") - raw.count("{") != nd.count("}") - raw.count("}"):
        print("!!! LOI TO: ngoac lech")
        return 1
    # chot: chot "ma am" van con dung 1 lan (khong bi xoa)
    if nd.count("AddItemEx: ma am") != 1:
        print("!!! LOI TO: chot ma am phai con dung 1 lan")
        return 1
    print("  chot: chot 'ma am' van con cho moi duong khac | byte cao %d (khong doi)" % hi0)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
    with io.open(DICH, "wb") as f:
        f.write(nd.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI. => build Core CA HAI cau hinh, dat CoreServer.dll + CoreClient.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
