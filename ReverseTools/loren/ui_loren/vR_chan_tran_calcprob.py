# -*- coding: utf-8 -*-
"""vR_chan_tran_calcprob.py - CHAN TRAN NGAN XEP trong calcProbLoop + BAT LOG GO ROI
co san cua ban goc.

TRIEU CHUNG: bam nut the "Lay" (Trich lay) -> "Lo ren gap loi khong ro";
ScriptError.log:
    Script Name: (\\script\\item\\compound\\magic_distill.lua) cFuncName:(Compound)
    error: stack Overflow
    stack traceback:  calcProbLoop [itemvalue_header.lua:136] (lap nhieu tang)
                      TransItemVal [itemvalue_header.lua:38]
                      defFinalCompound [compound_header.lua:109]

DA LOAI TRU (kiem bang so, khong doan):
  - Bang gia tri KHONG thieu: `settings\\item\\000\\` va `001\\` deu du 10 tep
    itemvalue + magicattriblevel(.._index).txt, trung tung byte voi bo VLTK.
  - ITEM_VERSION = 1 (GameDataDef.h:15) nen makeItemFilePath tim `001\\` - CO that.
  - `itemvalue_header.lua` tren may chu TRUNG BAM voi ban VLTK (aa81d0c147de),
    tuc dung nguon chu game chi dinh; `magic_distill.lua` cung khop ban Linux,
    chi lech ma vat pham da nan.

VI SAO VAN TRAN: `calcProbLoop` de quy chia doi day gia tri. No co dieu kien dung,
nhung CHI HOI TU khi day gia tri la so that va co thu tu. Neu mot phan tu la NaN
(hoac day suy bien theo kieu khac), moi phep so sanh deu tra false:
    dong  72: arydValue[a] == arydValue[b]      -> false
    dong  77: dDivVal <= arydValue[nStartIdx]   -> false
    dong  93: dDivVal >= arydValue[nEndIdx]     -> false
=> luon roi vao nhanh `else` (dong 109), tinh ra NaN moi, roi de quy voi DUNG
khoang cu -> vo han -> tran ngan xep.

HAI MIENG:

M1 (CHONG SAP - giu lau dai): them BIEN DEM DO SAU cho calcProbLoop. Day de quy
   chia doi tren mang toi da 32 phan tu nen do sau that khong bao gio qua ~32;
   dat tran 64 la rong rai gap doi, khong the cham vao duong chay dung. Qua tran
   thi chia deu xac suat cho khoang con lai roi thoat - ket qua van hop le, chi
   la khong con "chia tinh" nua. Nho vay dù du lieu co bat thuong den may,
   kich ban KHONG bao gio lam sap Lua nua.
   (Ban goc khong co chot nay vi engine Linux tinh gia tri kieu khac; day la
    chot an toan cua JX1, khong doi cach tinh xac suat cua duong chay dung.)

M2 (CHAN DOAN - tam thoi): bat lai 5 dong `Msg2Player` GO ROI **von co san trong
   ban goc** (compound_header.lua:88-90, 98, 100 - dang bi `--`). Chung in ra
   tong gia tri nguyen lieu va gia tri tung phuong an, tuc dung con so ma
   calcProbLoop nhan vao. Bam mot lan la biet ngay so nao bat thuong.
   Dung xong thi chay `--tat` de tat lai.

Mac dinh DIEN TAP; --ghi de ap; --tat de tat rieng log go roi (M2).
"""
import io
import os
import shutil
import sys

HEADER = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\compound\compound_header.lua"
ITEMVAL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\itemvalue\itemvalue_header.lua"
HAU_TO = ".truoc_chantran"

# ---- M1: chot do sau ----
M1_CU = "function calcProbLoop( arydValue, arydProb, nStartIdx, nEndIdx, dDivVal )\n"
M1_MOI = (
    "-- [LOREN 27/08] CHOT DO SAU. Day de quy chia doi tren mang <= 32 phan tu nen\n"
    "-- do sau that toi da ~32; tran 64 la gap doi, khong cham duong chay dung.\n"
    "-- Khi du lieu bat thuong (vd mot phan tu NaN) thi MOI phep so sanh o ba\n"
    "-- nhanh tren deu false, ham luon roi vao nhanh else va de quy voi DUNG\n"
    "-- khoang cu -> tran ngan xep -> sap kich ban (\"Lo ren gap loi khong ro\").\n"
    "-- Qua tran thi chia deu xac suat cho khoang con lai roi thoat.\n"
    "CALCPROB_MAX_DEPTH = 64;\n"
    "\n"
    "function calcProbLoop( arydValue, arydProb, nStartIdx, nEndIdx, dDivVal, nDepth )\n"
    "\tif( nDepth == nil ) then\n"
    "\t\tnDepth = 0;\n"
    "\tend\n"
    "\tif( nDepth >= CALCPROB_MAX_DEPTH or nStartIdx == nil or nEndIdx == nil\n"
    "\t\tor nStartIdx > nEndIdx ) then\n"
    "\t\tif( nStartIdx ~= nil and nEndIdx ~= nil and nStartIdx <= nEndIdx ) then\n"
    "\t\t\tlocal nSo = nEndIdx - nStartIdx + 1;\n"
    "\t\t\tfor i = nStartIdx, nEndIdx do\n"
    "\t\t\t\tarydProb[i] = arydProb[i] / nSo;\n"
    "\t\t\tend\n"
    "\t\t\tWriteLog( \"[LOREN] calcProbLoop cham tran do sau - du lieu gia tri bat thuong\" );\n"
    "\t\tend\n"
    "\t\treturn;\n"
    "\tend\n"
)
M1_CU2 = "\t\t\tcalcProbLoop( arydValue, arydProb, nStartIdx, nDivIdx - 1, dLessMean );\n"
M1_MOI2 = "\t\t\tcalcProbLoop( arydValue, arydProb, nStartIdx, nDivIdx - 1, dLessMean, nDepth + 1 );\n"
M1_CU3 = "\t\t\tcalcProbLoop( arydValue, arydProb, nDivIdx, nEndIdx, dMoreMean );\n"
M1_MOI3 = "\t\t\tcalcProbLoop( arydValue, arydProb, nDivIdx, nEndIdx, dMoreMean, nDepth + 1 );\n"

# ---- M2: bat/tat log go roi co san ----
LOG_DONG = [
    "--\tMsg2Player( \"<color=yellow>-----------------------------------\" );\n",
    "--\tMsg2Player( \"<color=gold>TotalSrcSum: \"..nNecItemValSum..\"+\"..dAltItemFinalVal..\"=\"..nSrcItemValSum );\n",
    "--\t\tMsg2Player( \"<color=green>DesValue\"..i..\": \"..arydDesItemVal[i] );\n",
]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi_tep(p, noi_dung):
    sao = p + HAU_TO
    if not os.path.isfile(sao):
        shutil.copy2(p, sao)
        print("   sao luu -> %s" % os.path.basename(sao))
    with io.open(p, "wb") as f:
        f.write(noi_dung.encode("latin-1"))
    if doc(p) != noi_dung:
        print("!!! LOI TO: doc lai KHONG khop: %s" % p)
        return False
    return True


def main():
    ghi = "--ghi" in sys.argv[1:]
    tat = "--tat" in sys.argv[1:]
    print("=== vR_chan_tran_calcprob - %s ==="
          % ("TAT LOG" if tat else ("GHI THAT" if ghi else "DIEN TAP")))

    ke_hoach = []

    # ---------- M1 ----------
    raw = doc(ITEMVAL)
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)
    if "CALCPROB_MAX_DEPTH" in raw:
        print("\n  M1 chot do sau: DA CO - bo qua")
    elif not tat:
        moi = raw
        for ten, cu, thay in (("khai bao", M1_CU, M1_MOI),
                              ("de quy 1", M1_CU2, M1_MOI2),
                              ("de quy 2", M1_CU3, M1_MOI3)):
            cu_e = cu.replace("\n", eol)
            if moi.count(cu_e) != 1:
                print("!!! LOI TO: M1/%s moc neo %d lan (can 1)" % (ten, moi.count(cu_e)))
                return 1
            moi = moi.replace(cu_e, thay.replace("\n", eol), 1)
        hi1 = sum(1 for c in moi if ord(c) > 127)
        if hi1 != hi0:
            print("!!! LOI TO: M1 byte cao %d -> %d" % (hi0, hi1)); return 1
        print("\n  M1 chot do sau: 3 moc neo deu trung 1 lan | byte cao %d (khong doi)" % hi0)
        ke_hoach.append((ITEMVAL, moi))

    # ---------- M2 ----------
    raw2 = doc(HEADER)
    eol2 = "\r\n" if "\r\n" in raw2 else "\n"
    hi2 = sum(1 for c in raw2 if ord(c) > 127)
    moi2 = raw2
    dem = 0
    for d in LOG_DONG:
        cu = d.replace("\n", eol2)
        moi_d = d[2:].replace("\n", eol2)          # bo hai dau '-' dau dong
        if tat:
            cu, moi_d = moi_d, cu
        n = moi2.count(cu)
        if n >= 1:
            moi2 = moi2.replace(cu, moi_d)
            dem += n
    if dem == 0:
        print("  M2 log go roi: khong co dong nao de %s" % ("tat" if tat else "bat"))
    else:
        hi3 = sum(1 for c in moi2 if ord(c) > 127)
        if hi3 != hi2:
            print("!!! LOI TO: M2 byte cao %d -> %d" % (hi2, hi3)); return 1
        print("  M2 log go roi (co san trong ban goc): %s %d dong | byte cao %d (khong doi)"
              % ("TAT" if tat else "BAT", dem, hi2))
        ke_hoach.append((HEADER, moi2))

    if not ke_hoach:
        print("\nKhong co gi de ap."); return 0
    if not (ghi or tat):
        print("\nDIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for p, noi_dung in ke_hoach:
        if not ghi_tep(p, noi_dung):
            return 1
        print("  DA GHI %s" % os.path.basename(p))
    print("\n  => Kich ban Lua: nap lai script hoac khoi dong lai may chu la co hieu luc.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
