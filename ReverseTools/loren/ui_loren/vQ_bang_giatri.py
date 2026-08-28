# -*- coding: utf-8 -*-
"""vQ_bang_giatri.py - CHEP BANG GIA TRI VAT PHAM con thieu (goc lam SAP kich ban
lo ren o the Trich lay).

TRIEU CHUNG (chu game): bam nut o the "Lay" -> "Lo ren gap loi khong ro", va
ScriptError.log ghi:
    ScriptError 4:[1] Script Name: (\\script\\item\\compound\\magic_distill.lua)
                      cFuncName:(Compound)
    error: stack Overflow
    stack traceback:
       calcProbLoop  [itemvalue\\itemvalue_header.lua:136]  (de quy lap nhieu tang)
       TransItemVal  [itemvalue_header.lua:38]
       defFinalCompound [compound_header.lua:109]
       finalCompound / Compound

GOC: `calcProbLoop` de quy chia doi day gia tri de tinh xac suat. No co dieu kien
dung day du, nhung chi hoi tu khi DAY GIA TRI la so that. Gia tri do
`ITEM_CalcItemValue` tinh ra, va ham nay doc mot BO BANG ma JX1 CHUA HE CO:

    kich ban doc: makeItemFilePath(nVer, "itemvalue\\<ten>.txt")
                  = "\\settings\\item\\%03d\\itemvalue\\<ten>.txt"
                  (item_header.lua:7-11 tren may chu)
    can 8 bang trong itemvalue\\ + magicattriblevel.txt + magicattriblevel_index.txt
    o CUNG thu muc phien ban:
       ore.txt · equip_gold.txt · equip_normal_magic.txt · magicattrib_combine.txt
       equip_enchasable_{level,socket,type,magic_pos}.txt

    Kiem tren may chu: thu muc `settings\\item\\itemvalue\\` VA `settings\\item\\000\\`
    KHONG TON TAI. Thieu bang -> TabFile_GetCell tra gia tri mac dinh -> day
    gia tri suy bien -> calcProbLoop khong hoi tu -> tran ngan xep.

    Day dung la viec `PHANTICH_VIEMDE_EPDOTIM_HKMP.md` da ghi tu 26/08 la CHUA LAM:
    "Bang du lieu phai chep tu Linux (settings\\item\\004\\itemvalue\\, JX1 chua co
     thu muc nay): ... 10 tep".

NGUON DUNG: bo rut tu CLIENT VLTK - `ReverseTools\\loren\\ra\\settings\\item\\{000,001}\\`
(chu game da chot dung client VLTK lam nguon). Bo nay co san ca hai thu muc phien
ban, moi thu muc du: itemvalue\\ (10 tep) + magicattriblevel.txt +
magicattriblevel_index.txt + atlas_compound.txt.

CHEP NGUYEN VAN TUNG BYTE, khong nan gi: cac bang nay khoa theo CAP va MA THUOC
TINH chu khong theo ma vat pham, nen khong dinh chuyen lech -1 cua JX1.
KHONG dong den ban o thu muc phang `settings\\item\\` (magicattriblevel.txt o do
da khop goc, dang duoc duong C++ dung).

Mac dinh DIEN TAP; --ghi moi chep that (sao luu .truoc_banggiatri neu dich da co).
"""
import io
import os
import shutil
import sys

NGUON = r"D:\GAMEDEVNEW\ReverseTools\loren\ra\settings\item"
DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item"
HAU_TO = ".truoc_banggiatri"
PHIEN_BAN = ["000", "001"]


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vQ_bang_giatri - %s ===" % ("CHEP THAT" if ghi else "DIEN TAP"))

    ke_hoach = []
    for pb in PHIEN_BAN:
        thu_muc_nguon = os.path.join(NGUON, pb)
        if not os.path.isdir(thu_muc_nguon):
            print("  ! bo qua %s: khong co o nguon" % pb)
            continue
        print("\n--- phien ban %s" % pb)
        for goc, _, tep in os.walk(thu_muc_nguon):
            for t in tep:
                if not t.lower().endswith(".txt"):
                    continue
                p_nguon = os.path.join(goc, t)
                tuong_doi = os.path.relpath(p_nguon, NGUON)
                p_dich = os.path.join(DICH, tuong_doi)
                if os.path.isfile(p_dich):
                    a = io.open(p_nguon, "rb").read()
                    b = io.open(p_dich, "rb").read()
                    if a == b:
                        print("   da giong het, bo qua: %s" % tuong_doi)
                        continue
                    print("   GHI DE  %-52s %6d -> %6d byte" % (tuong_doi, len(b), len(a)))
                else:
                    print("   MOI     %-52s %6d byte" % (tuong_doi, os.path.getsize(p_nguon)))
                ke_hoach.append((p_nguon, p_dich, tuong_doi))

    if not ke_hoach:
        print("\nKhong co gi de chep.")
        return 0
    print("\n  tong: %d tep" % len(ke_hoach))

    if not ghi:
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de chep that.")
        return 0

    for p_nguon, p_dich, td in ke_hoach:
        os.makedirs(os.path.dirname(p_dich), exist_ok=True)
        if os.path.isfile(p_dich):
            sao = p_dich + HAU_TO
            if not os.path.isfile(sao):
                shutil.copy2(p_dich, sao)
                print("   sao luu -> %s" % os.path.basename(sao))
        shutil.copy2(p_nguon, p_dich)
        a = io.open(p_nguon, "rb").read()
        b = io.open(p_dich, "rb").read()
        if a != b:
            print("!!! LOI TO: chep xong doc lai KHONG khop: %s" % td)
            return 1
    print("\n  DA CHEP %d tep, doc lai tung byte deu khop." % len(ke_hoach))
    print("  => Kich ban Lua doc bang luc chay, KHONG phai build. Nap lai script")
    print("     hoac khoi dong lai may chu la co hieu luc.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
