# -*- coding: utf-8 -*-
"""vP_distill_3nhom.py - VA "bo du nguyen lieu van bao Nguyen lieu khong dung
luat ghep" o the TRICH LAY (Distill).

TRIEU CHUNG (chu game, kem anh): the "Lay" da bo du Trang bi + Huyen Tinh +
Khoang thach, bam nut van ra "Nguyen lieu khong dung luat ghep"
(= FOUNDRY_RESULT ma 8, compound_ui.lua:30).

GOC (doc ban Linux roi doi chieu):
  `D:\\ServerLinux\\server1\\script\\item\\compound\\magic_distill.lua:28-49`
  duyet nguyen lieu va doi **CA HAI** thu:
      dong 30-37: nEquipIdx  <- mon co nGenre == 0            (TRANG BI)
      dong 38-45: g_nDistillMagicPos <- mon 6/1/149..154      (NGUYEN KHOANG)
      dong 48   : if( nEquipIdx <= 0 or g_nDistillMagicPos <= 0 )
                      then return RESULT_LACK_RESOURCE
  => trang bi VA nguyen khoang deu BAT BUOC, khong phai "cai nay hoac cai kia".
  Cong them Huyen Tinh la phi bat buoc theo foundryresdemand.ini
  (`Distill_Cryolite=Cryolite`) => DUNG BA mon chinh, khop `s_anSoO[3] = 3`.

CHO SAI TRONG JX1 (KItemCompound.cpp:213 + :228):
      static const int s_g3a[] = { FRK_Distill_Equip, FRK_Distill_OrgMine };
              // chu thich cu: "nguon: do HOAC khoang"  <-- HIEU SAI BAN GOC
      static const KNhomKhoa s_aryNhom3[] = { {s_g3a,2}, {s_g3b,1} };
  Cac khoa trong CUNG mot nhom la LUA CHON THAY THE NHAU (chu thich :198-199),
  va moi nguyen lieu chi phuc vu duoc MOT nhom (`abDaDung`, Check :468-...).
  Nen khi nguoi choi bo du 3 mon:
      trang bi     -> khop nhom 1 (qua khoa Distill_Equip), nhom 1 coi nhu du
      Huyen Tinh   -> khop nhom 2 (Distill_Cryolite)
      NGUYEN KHOANG-> KHONG CON NHOM NAO -> bi tinh la "o thua"
  ma `s_anKhoaThua[3] = -1` (:245) = khong cho phep o thua nao
  => tra FOUNDRY_RESULT_RULE_ERROR (ma 8) = dung cau bao loi chu game thay.

MIENG VA: tach thanh BA nhom bat buoc rieng, dung nhu ban goc doi:
      {Distill_Equip} + {Distill_OrgMine} + {Distill_Cryolite}
Khong dong den s_anSoO (van 3, dung), khong dong den o tu chon
(s_anKhoaTuChon[3] = FRK_Distill_EnhanceItem, giu nguyen), khong dong nhom khac.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_distill3 lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp"
HAU_TO = ".truoc_distill3"

CU_KHOA = (
    "static const int s_g3a[] = { FRK_Distill_Equip, FRK_Distill_OrgMine };\t// nguon: do HOAC khoang\n"
    "static const int s_g3b[] = { FRK_Distill_Cryolite };\n"
)
MOI_KHOA = (
    "// [LOREN 27/08] Ban goc doi CA HAI, khong phai \"hoac\":\n"
    "//   magic_distill.lua:30-37 tim TRANG BI (nGenre == 0)\n"
    "//   magic_distill.lua:38-45 tim NGUYEN KHOANG (6/1/149..154 ban Linux)\n"
    "//   magic_distill.lua:48    if( nEquipIdx <= 0 or g_nDistillMagicPos <= 0 )\n"
    "//                               then return RESULT_LACK_RESOURCE\n"
    "// Gop chung mot nhom la sai: cac khoa cung nhom THAY THE nhau va moi nguyen\n"
    "// lieu chi phuc vu duoc mot nhom, nen trang bi chiem cho roi thi nguyen\n"
    "// khoang thanh \"o thua\" -> s_anKhoaThua[3] = -1 -> RULE_ERROR (ma 8).\n"
    "static const int s_g3a[] = { FRK_Distill_Equip };\n"
    "static const int s_g3c[] = { FRK_Distill_OrgMine };\n"
    "static const int s_g3b[] = { FRK_Distill_Cryolite };\n"
)

CU_NHOM = "static const KNhomKhoa s_aryNhom3[] = { {s_g3a,2}, {s_g3b,1} };\n"
MOI_NHOM = ("static const KNhomKhoa s_aryNhom3[] = "
            "{ {s_g3a,1}, {s_g3c,1}, {s_g3b,1} };\t// trang bi + nguyen khoang + Huyen Tinh\n")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vP_distill_3nhom - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH); return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi_truoc = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] Ban goc doi CA HAI" in raw:
        print("  DA CO - bo qua (idempotent)"); return 0

    moi = raw
    for ten, cu, thay in (("M1 tach khoa", CU_KHOA, MOI_KHOA),
                          ("M2 ba nhom bat buoc", CU_NHOM, MOI_NHOM)):
        cu_e = cu.replace("\n", eol)
        dem = moi.count(cu_e)
        if dem != 1:
            print("!!! LOI TO: %s - moc neo xuat hien %d lan (can 1) - KHONG ghi gi" % (ten, dem))
            return 1
        moi = moi.replace(cu_e, thay.replace("\n", eol), 1)
        print("  %s: moc neo trung 1 lan" % ten)

    # chot: so nhom cua type 3 phai la 3
    if "{ {s_g3a,1}, {s_g3c,1}, {s_g3b,1} }" not in moi:
        print("!!! LOI TO: khong tim thay khoi 3 nhom sau khi thay"); return 1
    if "aryBang" in moi:
        # sLayNhom dem so nhom bang so phan tu mang -> kiem dong khai bao
        import re
        m = re.search(r"\{\s*s_aryNhom3,\s*(\d+)\s*\}", moi)
        if m:
            print("  ! CANH BAO: sLayNhom khai so nhom type 3 = %s - PHAI la 3" % m.group(1))
            if m.group(1) != "3":
                moi = moi[:m.start()] + "{ s_aryNhom3, 3 }" + moi[m.end():]
                print("    -> da sua thanh 3")

    hi_sau = sum(1 for c in moi if ord(c) > 127)
    if hi_sau != hi_truoc:
        print("!!! LOI TO: byte cao %d -> %d" % (hi_truoc, hi_sau)); return 1
    print("  byte cao %d (khong doi) | CRLF %d -> %d"
          % (hi_truoc, raw.count("\r\n"), moi.count("\r\n")))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that."); return 0

    sao = DICH + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(DICH, sao); print("  sao luu -> %s" % sao)
    with io.open(DICH, "wb") as f:
        f.write(moi.encode("latin-1"))
    if io.open(DICH, "rb").read().decode("latin-1") != moi:
        print("!!! LOI TO: doc lai KHONG khop"); return 1
    print("  DA GHI + doc lai tu dia: khop.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
