# -*- coding: utf-8 -*-
"""w1_nhandien_chisodong.py - VA "kham vien da X vao lai ra dong thuoc tinh Y".

CHU GAME: "vien da sat thuong vat ly ngoai cong cua toi hien 2, ep vao vu khi
ra thanh dong noi luc toi da".

GOC THAT - CACH NHAN DIEN "CHI SO DONG" THAT BAI O DAI 1..10.

Trang bi tim luu trong `nGeneratorLevel[i]` CHI SO DONG cua
`magicattriblevel.txt`, con trang bi thuong luu CAP 1..MATF_LEVEL (=10).
Duong dong bo tu may chu ve client KHONG mang pham chat xuong
(`KItemSet::AddItemSet2` -> `Gen_Equipment` ban 10 tham so, KItemSet.cpp:228;
ban 11 tham so co `nItemNature` chi dung o `KItemSet::Add`, dong 337), nen
`Gen_MagicAttrib` phai TU DOAN, va dang doan bang CHINH GIA TRI:

    if (nItemNature == NATURE_VIOLET || pnaryMALevel[i] > MATF_LEVEL)   // :709
        -> hieu la chi so dong, dich qua sLoRenDichThuocTinh (DUNG)
    else
        -> hieu la CAP 1..10, di duong GetCMIT SINH THUOC TINH NGAU NHIEN (SAI)

Phep doan nay HONG khi chi so dong nam trong 1..10. Tra
`settings/item/magicattriblevel_index.txt` (khoa = MAGIC_ID, kem dai chi so dong):

    MAGIC_ID 126  dai   1..10   "Tang sat thuong vat ly he Ngoai cong"   <-- VUNG CHET
    MAGIC_ID 166  dai  11..20   "Tang cong kich chinh xac"
    MAGIC_ID 115  dai  21..30   "Tang toc do xuat chieu he Ngoai cong"
    MAGIC_ID 136  dai 121..130  "Hut sinh luc doi phuong"
    MAGIC_ID 137  dai 131..140  "Hut Noi luc doi phuong"

Dung mot MAGIC_ID roi vao 1..10, va do chinh la "sat thuong vat ly Ngoai cong"
- vien da chu game dung.

BANG CHUNG SO tu `Logs/KSG_CompoundLog_20260827.txt` (khong doan):
    trang bi cua chu game: <29, 236, 10, 340, -1, 0>
                                    ^^ o thu 3 = "Hien 2" = dung o chu game noi
    o 3 mang chi so dong 10 -> dong 10 co MAGIC_ID 126 = "Tang sat thuong vat ly
    he Ngoai cong" - dung ten vien da. Vi 10 <= MATF_LEVEL nen o nay bi hieu la
    "cap 10" -> GetCMIT sinh thuoc tinh NGAU NHIEN -> hien ra dong khac.
Doi chieu: lan kham ghi trong log dung vien MAGIC_ID 136 (dai 121..130 > 10) nen
KHONG dinh loi - dung nhu quan sat "co lan dung co lan sai".

MIENG VA (toi thieu, KHONG doi giao thuc, KHONG doi chu ky ham):
Trang bi tim dung chi so dong cho CA SAU O. Nen thay vi xet TUNG O roi le, hay
xet CA MANG mot lan: chi can MOT o vuot MATF_LEVEL hoac bang -1 (o kham nam
rong - chi trang bi tim moi co) thi CA MANG la chi so dong.
Voi truong hop tren: <29, 236, 10, 340, -1, 0> co 29/236/340 > 10 va co -1,
nen o mang gia tri 10 cung duoc hieu DUNG la chi so dong.

    BOOL bChiSoDong = (nItemNature == NATURE_VIOLET);
    if (!bChiSoDong)
        for (k...) if (pnaryMALevel[k] > MATF_LEVEL || pnaryMALevel[k] == -1)
            { bChiSoDong = TRUE; break; }
    ...
    if (bChiSoDong)          // thay cho dieu kien cu

Chi con sot khi trang bi tim co CA SAU o deu nam trong 1..10, tuc ca sau deu la
MAGIC_ID 126 - ma `equip_enchase.lua:76-80` da chan kham trung MagicID, nen
truong hop do khong ton tai.

Tep dich: Sources\\Core\\Src\\KItemGenerator.CPP (dung chung client + server)
=> build CA HAI cau hinh, dat lai CoreClient.dll + CoreServer.dll.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_chisodong lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemGenerator.CPP"
HAU_TO = ".truoc_chisodong"

T = "\t"
NHAN = "[LOREN 27/08] NHAN DIEN CHI SO DONG THEO CA MANG"

# Neo 1: dieu kien cu (dung 1 lan)
CU_DK = T*3 + "if (nItemNature == NATURE_VIOLET || pnaryMALevel[i] > MATF_LEVEL)"
MOI_DK = T*3 + "if (bChiSoDong)"

# Neo 2: dat khoi tinh bChiSoDong ngay truoc vong `for` chinh.
CU_FOR = T*2 + "for (i = 0; i < MAX_ITEM_MAGICLEVEL; i++)"

KHOI = [
    T*2 + "// " + NHAN + ".",
    T*2 + "// Truoc day xet TUNG O: `pnaryMALevel[i] > MATF_LEVEL`. Phep doan do",
    T*2 + "// HONG khi chi so dong roi vao 1..10 - va co that: bang",
    T*2 + "// magicattriblevel_index.txt cho MAGIC_ID 126 (\"Tang sat thuong vat ly",
    T*2 + "// he Ngoai cong\") dai chi so dong dung bang 1..10. O do bi hieu nham la",
    T*2 + "// \"cap 10\" cua trang bi thuong -> di duong GetCMIT -> SINH THUOC TINH",
    T*2 + "// NGAU NHIEN, nen kham vien da nay vao lai hien ra mot dong khac han.",
    T*2 + "// (Do that: trang bi <29,236,10,340,-1,0>, o thu 3 mang chi so dong 10.)",
    T*2 + "//",
    T*2 + "// Trang bi tim dung chi so dong cho CA SAU O, nen xet CA MANG mot lan:",
    T*2 + "// chi can mot o vuot MATF_LEVEL hoac bang -1 (o kham nam rong - rieng",
    T*2 + "// trang bi tim moi co) thi ca mang la chi so dong. Nho vay o mang gia tri",
    T*2 + "// nho (1..10) cung duoc hieu dung.",
    T*2 + "// Van uu tien nItemNature khi duong goi co truyen (KItemSet::Add); duong",
    T*2 + "// dong bo ve client (AddItemSet2 -> Gen_Equipment ban 10 tham so) khong",
    T*2 + "// mang pham chat xuong nen moi phai suy tu chinh mang.",
    T*2 + "BOOL bChiSoDong = (nItemNature == NATURE_VIOLET);",
    T*2 + "if (!bChiSoDong)",
    T*2 + "{",
    T*3 + "for (int k = 0; k < MAX_ITEM_MAGICATTRIB; k++)",
    T*3 + "{",
    T*4 + "if (pnaryMALevel[k] > MATF_LEVEL || pnaryMALevel[k] == -1)",
    T*4 + "{",
    T*5 + "bChiSoDong = TRUE;",
    T*5 + "break;",
    T*4 + "}",
    T*3 + "}",
    T*2 + "}",
    T*2 + "",
]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w1_nhandien_chisodong - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH)
        return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if NHAN in raw:
        print("  DA CO - bo qua (idempotent)")
        return 0

    dong = raw.split(eol)

    vt_dk = [i for i, l in enumerate(dong) if l == CU_DK]
    if len(vt_dk) != 1:
        print("!!! LOI TO: dieu kien cu xuat hien %d lan (can 1)" % len(vt_dk))
        for i, l in enumerate(dong):
            if "nItemNature == NATURE_VIOLET" in l:
                print("     dong %d: %r" % (i + 1, l[:96]))
        return 1
    i_dk = vt_dk[0]

    vt_for = [i for i, l in enumerate(dong) if l == CU_FOR]
    if len(vt_for) != 1:
        print("!!! LOI TO: vong for chinh xuat hien %d lan (can 1)" % len(vt_for))
        return 1
    i_for = vt_for[0]

    if not (i_for < i_dk):
        print("!!! LOI TO: vong for (%d) khong nam TRUOC dieu kien (%d)" % (i_for + 1, i_dk + 1))
        return 1
    print("  vong for dong %d | dieu kien dong %d (dung thu tu)" % (i_for + 1, i_dk + 1))

    dong[i_dk] = MOI_DK
    dong = dong[:i_for] + KHOI + dong[i_for:]
    moi = eol.join(dong)

    # ---------------- chot an toan ----------------
    hi1 = sum(1 for c in moi if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1))
        return 1
    if moi.count("{") != raw.count("{") + 3 or moi.count("}") != raw.count("}") + 3:
        print("!!! LOI TO: ngoac lech - mo %d->%d, dong %d->%d"
              % (raw.count("{"), moi.count("{"), raw.count("}"), moi.count("}")))
        return 1
    if moi.count("BOOL bChiSoDong = ") != 1 or moi.count("if (bChiSoDong)") != 1:
        print("!!! LOI TO: so lan khai bao/dung bChiSoDong khong dung 1")
        return 1
    if "pnaryMALevel[i] > MATF_LEVEL" in moi.replace(T*2 + "// ", ""):
        # chi con trong chu thich la duoc
        con = [l for l in moi.split(eol)
               if "pnaryMALevel[i] > MATF_LEVEL" in l and not l.strip().startswith("//")]
        if con:
            print("!!! LOI TO: van con dieu kien cu ngoai chu thich: %r" % con[0][:90])
            return 1
    print("  byte cao %d (khong doi) | ngoac {%d }%d (deu +3) | CRLF %d -> %d"
          % (hi0, moi.count("{"), moi.count("}"), raw.count("\r\n"), moi.count("\r\n")))

    if not ghi:
        print("\n  --- KHOI SE CHEN (truoc vong for) ---")
        for l in KHOI:
            print("   |%s" % l.replace("\t", "    ").rstrip())
        print("   ... va dong %d: %s" % (i_dk + 1 + len(KHOI), MOI_DK.strip()))
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao)
        print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI + doc lai tu dia: khop.")
    print("\n  => build Core CA HAI cau hinh, dat CoreClient.dll + CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
