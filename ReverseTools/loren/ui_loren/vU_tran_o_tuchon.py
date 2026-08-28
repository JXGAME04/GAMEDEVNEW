# -*- coding: utf-8 -*-
"""vU_tran_o_tuchon.py - VA "bo du lieu dung van bao khong dung luat ghep" o the
KHAM NAM: tran cua vector O TU CHON dang lay nham SO O CHINH.

BANG CHUNG (nut "Soi o lo ren" cua chu game, the Kham nam):
    o 0  (chinh) genre=0 detail=0 ptc=3   cap=10 nguhanh=2   <- trang bi
    o 1  (chinh) genre=6 detail=1 ptc=146 cap=10 nguhanh=0   <- Huyen Tinh
    o 2  (chinh) genre=6 detail=1 ptc=199 cap=10 nguhanh=0   <- khoang thuoc tinh
    o 6..10 (phu) genre=6 detail=1 ptc=123 cap=10 nguhanh=1  <- 5 vien Phuc Duyen Lo
=> Ba o CHINH khop dung ba nhom bat buoc. Nam mon o o PHU cung HOP LE:
   `foundryresdemand.ini [ConsumeItem_7] Genre=6 DetailType=1 PtcType=123`
   va khoa o tu chon cua Kham nam la `Enchase_EnhanceItem = ConsumeItem`.
   Vay ma van tra RULE_ERROR.

CHO SAI - `KFoundryResDemand::CheckTuChon` (KItemCompound.cpp:534):
    // Khong vuot qua so o chinh - ban goc dung chung tran 8 muc cho ca hai vector.
    if (nCount > LaySoO(nCompoundType))
        return FOUNDRY_RESULT_RULE_ERROR;
Chinh CHU THICH da ghi "tran 8", nhung ma lai goi `LaySoO()` - ham tra SO O CHINH
(`s_anSoO[5] = 3` cho Kham nam). Nen chi can bo qua 3 mon phu la bi tu choi,
trong khi giao dien co han 8 o phu.

SO O PHU THAT (doc tu chinh giao dien va kich ban dang chay):
    Lay.ini / Khamnam co Consume1..8  -> 8 o
    compound_ui.lua: LR_UI_Lam(7, 11, 3, 3) va LR_UI_Lam(9, 11, 3, 5)
        => 11 o, o 0..2 la chinh, o 3..10 la PHU = dung 8 o
Ba thao tac co o tu chon deu la 8: Trich lay (3), Kham nam (5), Do pho HK (6)
(`s_anKhoaTuChon` chi khai ba muc nay, con lai -1).

MIENG VA: thay `LaySoO(nCompoundType)` bang hang so o phu that (8), dat ten ro
rang. Khong dong den `Check()` cua vector chinh (van dung `LaySoO` - dung).

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_tranphu lan dau).
"""
import io
import os
import shutil
import sys

DICH = r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp"
HAU_TO = ".truoc_tranphu"

CU = (
    "\t// Khong vuot qua so o chinh - ban goc dung chung tran 8 muc cho ca hai vector.\n"
    "\tif (nCount > LaySoO(nCompoundType))\n"
    "\t\treturn FOUNDRY_RESULT_RULE_ERROR;\n"
)

MOI = (
    "\t// [LOREN 27/08] TRUOC DAY LAY NHAM: dung `LaySoO()` - ham tra SO O CHINH\n"
    "\t// (Kham nam = 3) - lam tran cua vector O TU CHON. Chi can bo qua 3 mon phu\n"
    "\t// la bi tu choi, du giao dien co han 8 o phu va mon bo vao hoan toan hop le.\n"
    "\t// So o phu THAT, doc tu chinh giao dien va kich ban dang chay:\n"
    "\t//   Lay.ini / Khamnam co Consume1..8            -> 8 o\n"
    "\t//   compound_ui.lua: LR_UI_Lam(7,11,3,3) va (9,11,3,5)\n"
    "\t//       => 11 o, o 0..2 chinh, o 3..10 PHU     -> dung 8 o\n"
    "\t// Ba thao tac co o tu chon (Trich lay 3, Kham nam 5, Do pho 6) deu 8 o.\n"
    "\tconst int nSoOTuChon = 8;\n"
    "\tif (nCount > nSoOTuChon)\n"
    "\t\treturn FOUNDRY_RESULT_RULE_ERROR;\n"
)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vU_tran_o_tuchon - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH); return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "[LOREN 27/08] TRUOC DAY LAY NHAM" in raw:
        print("  DA CO - bo qua (idempotent)"); return 0

    cu = CU.replace("\n", eol)
    dem = raw.count(cu)
    if dem != 1:
        print("!!! LOI TO: moc neo xuat hien %d lan (can 1) - KHONG ghi gi" % dem); return 1
    moi = raw.replace(cu, MOI.replace("\n", eol), 1)

    hi1 = sum(1 for c in moi if ord(c) > 127)
    if hi1 != hi0:
        print("!!! LOI TO: byte cao %d -> %d" % (hi0, hi1)); return 1
    print("  moc neo trung 1 lan | byte cao %d (khong doi) | CRLF %d -> %d"
          % (hi0, raw.count("\r\n"), moi.count("\r\n")))
    # chot: Check() cua vector CHINH phai VAN dung LaySoO
    if moi.count("if (nCount <= 0 || nCount > nSoO)") != 1:
        print("!!! LOI TO: khong con chot so o chinh trong Check() - dung lai"); return 1
    print("  chot: Check() vector chinh van dung LaySoO - khong bi dong den")

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
    print("\n  => build Core CA HAI cau hinh roi dat lai CoreServer.dll")
    return 0


if __name__ == "__main__":
    sys.exit(main())
