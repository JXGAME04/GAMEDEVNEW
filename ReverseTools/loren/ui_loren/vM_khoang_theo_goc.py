# -*- coding: utf-8 -*-
"""vM_khoang_theo_goc.py - Ba cho JX1 LECH BAN GOC ve vien khoang. Sua theo dung
ban Linux, khong tu bien.

NEN TANG (tra tu ban Linux `D:\\ServerLinux\\server1\\script\\item\\compound\\`):
  "Hien"/"An" (明/暗) la TEN CUA 6 O KHAM, danh so theo vi tri:
      pos 1 HIEN1  pos 2 AN1  pos 3 HIEN2  pos 4 AN2  pos 5 HIEN3  pos 6 AN3
  Khac biet DUY NHAT: **o AN (pos CHAN) bat buoc ngu hanh vien khoang = ngu hanh
  trang bi**; o HIEN thi khong.
      equip_enchase.lua:75 (Linux) / :67 (JX1):
        elseif( mod( nOreMagLvlPos, 2 ) == 0 and g_nEquipSeries ~= nOreSeries )
            then return RESULT_SERIES_ERROR;
      magic_distill.lua:51 (ca hai ban): y het.
  Vi the vien AN moi ton tai DU 5 NGU HANH, con vien HIEN chi mot
  (`trangbitim.lua:136-157` - NPC thu nghiem ban goc phat 1 vien HIEN va 5 vien
  AN cho moi cap o).

  HAI DAY vat pham, deu lech -1 khi sang JX1:
      nguyen khoang RONG : Linux 149..154 -> JX1 148..153  (pos = ptc - 147)
      khoang THUOC TINH  : Linux 200..205 -> JX1 199..204  (pos = ptc - 198)
  => o AN cua JX1:  nguyen khoang ptc LE (149,151,153)
                    khoang thuoc tinh ptc CHAN (200,202,204)
  (Tinh chan/le DAO khi sang JX1 vi lech -1; so vi tri pos thi khong doi.)

BA MIENG:

M1 - **Intro bi cat giua the mau** (chu game thay "thuoc tinh an 1<" du mot dau '<').
     Ban goc Linux/VLTK viet Intro dai 186..191 byte va client VLTK chay tot;
     JX1 dang de 164..183 byte, tuc NGAN HON goc. Ben lech la CAU TRUC cua JX1:
     `KBasPropTbl.h` KBASICPROP_MAGICSCRIPT.m_szIntro[SZBUFLEN_1] = 128 byte
     => KTabFile cat o byte 127, cat trung giua "<color=...>".
     Sua: noi rieng truong nay len SZBUFLEN_256 (hang da co san :6).
     An toan da kiem:
       - nap dung `sizeof(pBuf->m_szIntro)` (KBasPropTbl.CPP:1266) nen tu dong
         nap du hon, khong phai sua cho nap;
       - dich chep sang la `KItem.h:64 szIntro[256]` - du cho (KItem.cpp:888);
       - `TEnterTextFromCharArray` (Text.cpp:1558-1592) KEP CUNG 256, khong tran;
       - KBASICPROP_MAGICSCRIPT chi song trong bo nho (bang nap tu .txt), KHONG
         ghi ra CSDL hay goi tin => noi khong pha du lieu nguoi choi.

M2 - **Doc sai truong MAGIC_ID khi tra bang DESC/FIT_EQUIP** (chu game thay
     "Thuoc tinh:" va "Yeu cau trang bi:" RONG).
     Ban goc doc `GetItemParam(idx, 1)` = `m_GeneratorParam.nGeneratorLevel[0]`
     (ScriptFuns.cpp:5811-5818; dung o equip_enchase.lua:68, prop_ore.lua:32).
     Duong GHI cung ghi vao do: KItemCompound.cpp:1364-1365.
     Nhung KItem.cpp:1508 lai doc `m_aryBaseAttrib[0].nValue[0]` - truong ma
     KHONG AI GHI (KItem.cpp:890 ZeroMemory no trong operator=). Nap chong kia
     (KItem.cpp:2378) doc `m_GeneratorParam.nLuck` thi lai dung.
     Sua: doc `m_GeneratorParam.nGeneratorLevel[0]` cho khop ban goc.

M3 - **Khong hien dong NGU HANH cho vien khoang** (chu game: "chua co he").
     Ban goc IN ngu hanh cho vien AN:
       prop_ore.lua:21-23      (day khoang thuoc tinh) mod(ptc,2)==1 -> in he
       primitive_ore.lua:14-16 (day nguyen khoang)     mod(ptc,2)==0 -> in he
     JX1 (KItem.cpp:1463-1470) ep `nfkSerial = series_nil` cho MOI vat pham
     genre 6 tru 398/399 => khong vien khoang nao hien ngu hanh.
     Sua: giu nguyen ngu hanh cho o AN cua ca hai day (theo parity DA DOI CHIEU
     o phan nen tang). O HIEN van khong hien - dung nhu ban goc.

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_khoanggoc lan dau).
"""
import io
import os
import shutil
import sys

HAU_TO = ".truoc_khoanggoc"

M1_DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KBasPropTbl.h"
M1_CU = (
    "\tint\t\t\tm_nHeight;\t\t\t\t\t\n"
    "\tchar\t\tm_szIntro[SZBUFLEN_1];\t\t\n"
    "\tchar\t\tm_szScript[SZBUFLEN_1];\t\t\n"
)
M1_MOI = (
    "\tint\t\t\tm_nHeight;\t\t\t\t\t\n"
    "\t// [LOREN 27/08] 128 byte KHONG du: Intro cua khoang thuoc tinh o ban goc\n"
    "\t// (Linux 004 + client VLTK) dai 186..191 byte. KTabFile cat o byte 127,\n"
    "\t// cat trung giua mot the \"<color=...>\" -> the ho -> bo phan tich the di\n"
    "\t// tim '>' ra ngoai pham vi. Dich chep sang la szIntro[256] (KItem.h:64)\n"
    "\t// nen du cho; nap dung sizeof() nen tu dong nap du hon.\n"
    "\tchar\t\tm_szIntro[SZBUFLEN_256];\t\n"
    "\tchar\t\tm_szScript[SZBUFLEN_1];\t\t\n"
)

M2_DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItem.cpp"
M2_CU = "\t\tsprintf(szTmp,\"%d\",m_aryBaseAttrib[0].nValue[0]);\n"
M2_MOI = (
    "\t\t// [LOREN 27/08] Ban goc doc MAGIC_ID cua vien khoang bang\n"
    "\t\t// GetItemParam(idx,1) = m_GeneratorParam.nGeneratorLevel[0]\n"
    "\t\t// (ScriptFuns.cpp:5811; equip_enchase.lua:68, prop_ore.lua:32), va\n"
    "\t\t// duong ghi cung ghi vao do (KItemCompound.cpp:1364).\n"
    "\t\t// m_aryBaseAttrib[0].nValue[0] KHONG AI GHI (bi ZeroMemory o\n"
    "\t\t// KItem.cpp:890) nen tra bang luon truot -> dong \"Thuoc tinh:\" rong.\n"
    "\t\tsprintf(szTmp,\"%d\",m_GeneratorParam.nGeneratorLevel[0]);\n"
)

M3_DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItem.cpp"
M3_CU = (
    "\t\tint nPar = m_CommonAttrib.nParticularType;\n"
)
M3_MOI = (
    "\t\tint nPar = m_CommonAttrib.nParticularType;\n"
    "\t\t// [LOREN 27/08] Vien khoang O AN co ngu hanh that va PHAI hien ra:\n"
    "\t\t// ban goc in dong ngu hanh cho chung (prop_ore.lua:21-23 voi day\n"
    "\t\t// khoang thuoc tinh, primitive_ore.lua:14-16 voi day nguyen khoang),\n"
    "\t\t// vi o AN (pos chan) bat buoc cung ngu hanh voi trang bi\n"
    "\t\t// (equip_enchase.lua:67, magic_distill.lua:51). O HIEN thi khong hien.\n"
    "\t\t// JX1 lech -1 so voi Linux nen parity DAO:\n"
    "\t\t//   nguyen khoang    148..153 -> o AN la ptc LE   (149,151,153)\n"
    "\t\t//   khoang thuoc tinh 199..204 -> o AN la ptc CHAN (200,202,204)\n"
    "\t\tBOOL bKhoangAn =\n"
    "\t\t\t(nPar >= 148 && nPar <= 153 && (nPar % 2) == 1) ||\n"
    "\t\t\t(nPar >= 199 && nPar <= 204 && (nPar % 2) == 0);\n"
)
M3_CU2 = (
    "\t\tif(nPar == 398 || nPar == 399) //s\xb8t th\xf1 l\xd6nh, s\xb8t th\xf1 gi\xb6n\n"
)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vM_khoang_theo_goc - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    ke_hoach = {}

    # ---- M1 ----
    raw = io.open(M1_DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    if "[LOREN 27/08] 128 byte KHONG du" in raw:
        print("  M1: DA CO - bo qua")
    else:
        cu = M1_CU.replace("\n", eol)
        if raw.count(cu) != 1:
            print("!!! LOI TO: M1 moc neo xuat hien %d lan (can 1)" % raw.count(cu)); return 1
        ke_hoach[M1_DICH] = raw.replace(cu, M1_MOI.replace("\n", eol), 1)
        print("  M1 noi m_szIntro 128 -> 256: moc neo trung 1 lan")

    # ---- M2 + M3 (cung tep KItem.cpp) ----
    raw2 = ke_hoach.get(M2_DICH) or io.open(M2_DICH, "rb").read().decode("latin-1")
    eol2 = "\r\n" if "\r\n" in raw2 else "\n"
    if "[LOREN 27/08] Ban goc doc MAGIC_ID" in raw2:
        print("  M2: DA CO - bo qua")
    else:
        cu = M2_CU.replace("\n", eol2)
        if raw2.count(cu) != 1:
            print("!!! LOI TO: M2 moc neo xuat hien %d lan (can 1)" % raw2.count(cu)); return 1
        raw2 = raw2.replace(cu, M2_MOI.replace("\n", eol2), 1)
        print("  M2 doc dung truong MAGIC_ID: moc neo trung 1 lan")

    if "[LOREN 27/08] Vien khoang O AN" in raw2:
        print("  M3: DA CO - bo qua")
    else:
        cu = M3_CU.replace("\n", eol2)
        if raw2.count(cu) != 1:
            print("!!! LOI TO: M3 moc neo(1) xuat hien %d lan (can 1)" % raw2.count(cu)); return 1
        raw2 = raw2.replace(cu, M3_MOI.replace("\n", eol2), 1)
        cu2 = M3_CU2.replace("\n", eol2)
        if raw2.count(cu2) != 1:
            print("!!! LOI TO: M3 moc neo(2) xuat hien %d lan (can 1)" % raw2.count(cu2)); return 1
        raw2 = raw2.replace(cu2, cu2.rstrip(eol) + eol2 +
                            "\t\t\tnfkSerial = m_CommonAttrib.nSeries;" + eol2 +
                            "\t\telse if(bKhoangAn)" + eol2, 1)
        # dong cu ngay duoi moc neo(2) la `nfkSerial = ...;` -> bo bot mot ban
        thua = ("\t\telse if(bKhoangAn)" + eol2 +
                "\t\t\tnfkSerial = m_CommonAttrib.nSeries;" + eol2)
        if raw2.count(thua) != 1:
            print("!!! LOI TO: M3 khong dung duoc khoi if/else - KHONG ghi gi"); return 1
        print("  M3 hien ngu hanh cho o AN: moc neo trung 1 lan")
    ke_hoach[M2_DICH] = raw2

    for p, noi_dung in ke_hoach.items():
        goc = io.open(p, "rb").read().decode("latin-1")
        hi0 = sum(1 for c in goc if ord(c) > 127)
        hi1 = sum(1 for c in noi_dung if ord(c) > 127)
        if hi0 != hi1:
            print("!!! LOI TO: %s byte cao %d -> %d" % (os.path.basename(p), hi0, hi1)); return 1
        print("  %-18s byte cao %d (khong doi) | CRLF %d -> %d"
              % (os.path.basename(p), hi0, goc.count("\r\n"), noi_dung.count("\r\n")))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for p, noi_dung in ke_hoach.items():
        sao = p + HAU_TO
        if not os.path.isfile(sao):
            shutil.copy2(p, sao); print("  sao luu -> %s" % sao)
        with io.open(p, "wb") as f:
            f.write(noi_dung.encode("latin-1"))
        if io.open(p, "rb").read().decode("latin-1") != noi_dung:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p); return 1
        print("  DA GHI %s" % p)
    print("\n  => build Core CA HAI cau hinh")
    return 0


if __name__ == "__main__":
    sys.exit(main())
