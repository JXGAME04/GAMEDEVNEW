# -*- coding: utf-8 -*-
"""vS_calcprob_vonglap.py - KHU DE QUY calcProbLoop (het "stack Overflow" khi bam
the Trich lay).

GOC THAT (da do bang so, khong doan):
  1. Chu game bat log go roi cua ban goc, day gia tri BINH THUONG:
        TotalSrcSum = 134.070.000
        DesValue1..10 = 10.000 / 50.000 / 225.000 / 900.000 / 3.300.000 /
                        11.314.000 / 30.700.000 / 126.000.000 / 492.250.000 /
                        1.849.800.000
     Tinh tay theo day nay: calcProbLoop chi de quy ~8 tang roi dung - KHONG
     he vo han. Vay khong phai loi thuat toan.
  2. `Sources\\Library\\LuaLib\\src\\llimits.h:74`:
        #define DEFAULT_STACK_SIZE      128//1024
     Du an DA HA ngan xep Lua tu 1024 xuong 128 (chu thich con giu so cu).
     `luaD_checkstack` (ldo.c:44-52) nem "stack Overflow" khi het cho.
     De quy 8 tang, moi tang hang chuc bien cuc bo + tham so => vuot 128.
  3. Chot `CALCPROB_MAX_DEPTH = 64` dat o dot truoc KHONG kip chan (tran o
     tang ~8), va thuc te khong dong WriteLog nao duoc ghi - dung nhu quan sat.

VI SAO KHONG NANG DEFAULT_STACK_SIZE LEN 1024:
  No ap cho MOI Lua_State cua toan bo may chu (moi kich ban, moi NPC, moi bot).
  Du an co huong 1000 bot; nhan len la tang bo nho dang ke. Ha xuong 128 la
  quyet dinh CO Y cua du an. Sua mot ham an toan hon nhieu so voi doi hang so
  toan cuc.

MIENG VA: viet lai calcProbLoop thanh VONG LAP voi ngan xep tu quan (ba bang
Lua song song). Thuat toan va KET QUA giu nguyen tung buoc - chi doi CACH DUYET:
hai lenh de quy thanh hai lenh day vao ngan xep. Do sau bao nhieu cung khong
dung toi ngan xep C/Lua nua.
Bo luon tham so nDepth va hang CALCPROB_MAX_DEPTH cua dot truoc (khong con can).

Mac dinh DIEN TAP; --ghi moi ghi that (sao luu .truoc_vonglap lan dau).
"""
import io
import os
import re
import shutil
import sys

DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\itemvalue\itemvalue_header.lua"
HAU_TO = ".truoc_vonglap"

HAM_MOI = """-- [LOREN 27/08] VONG LAP thay cho DE QUY.
-- Ngan xep Lua cua du an chi 128 o (LuaLib\\src\\llimits.h:74 -
-- "#define DEFAULT_STACK_SIZE 128//1024", da ha tu 1024). Ban de quy chi can
-- ~8 tang la vuot => luaD_checkstack nem "stack Overflow" => kich ban chet,
-- nguoi choi thay "Lo ren gap loi khong ro".
-- Thuat toan va ket qua GIU NGUYEN tung buoc; chi thay hai lenh goi de quy
-- bang hai lenh day vao ngan xep tu quan (ba bang chay song song).
function calcProbLoop( arydValue, arydProb, nStartIdx, nEndIdx, dDivVal )
\tlocal aryS = {};
\tlocal aryE = {};
\tlocal aryD = {};
\tlocal nTop = 1;
\taryS[1] = nStartIdx;
\taryE[1] = nEndIdx;
\taryD[1] = dDivVal;

\twhile( nTop > 0 ) do
\t\tlocal nS = aryS[nTop];
\t\tlocal nE = aryE[nTop];
\t\tlocal dD = aryD[nTop];
\t\tnTop = nTop - 1;

\t\tif( nS == nil or nE == nil or dD == nil or nS > nE ) then
\t\t\t-- muc hong: bo qua, khong lam sap kich ban
\t\telseif( nS < nE and arydValue[nS] == arydValue[nE] and dD == arydValue[nS] ) then
\t\t\tfor i = nS, nE do
\t\t\t\tarydProb[i] = arydProb[i] / ( nE - nS + 1 );
\t\t\tend
\t\telseif( dD <= arydValue[nS] ) then
\t\t\tlocal nEqualCount = 0;
\t\t\tfor i = nS, nE do
\t\t\t\tif( arydValue[nS] == arydValue[i] ) then
\t\t\t\t\tnEqualCount = nEqualCount + 1;
\t\t\t\telse
\t\t\t\t\tbreak;
\t\t\t\tend
\t\t\tend
\t\t\tfor i = nS, nS + nEqualCount - 1 do
\t\t\t\tarydProb[i] = arydProb[i] * ( dD / arydValue[i] ) / nEqualCount;
\t\t\tend
\t\t\tfor i = nS + nEqualCount, nE do
\t\t\t\tarydProb[i] = 0;
\t\t\tend
\t\telseif( dD >= arydValue[nE] ) then
\t\t\tlocal nEqualCount = 0;
\t\t\tfor i = nE, nS, -1 do
\t\t\t\tif( arydValue[nE] == arydValue[i] ) then
\t\t\t\t\tnEqualCount = nEqualCount + 1;
\t\t\t\telse
\t\t\t\t\tbreak;
\t\t\t\tend
\t\t\tend
\t\t\tfor i = nS, nE - nEqualCount do
\t\t\t\tarydProb[i] = 0;
\t\t\tend
\t\t\tfor i = nE - nEqualCount + 1, nE do
\t\t\t\tarydProb[i] = arydProb[i] / nEqualCount;
\t\t\tend
\t\telse
\t\t\tlocal nDivIdx = nS;
\t\t\tlocal dLessSum = 0;
\t\t\tlocal dMoreSum = 0;

\t\t\tfor i = nS, nE do
\t\t\t\tif( arydValue[i] <= dD ) then
\t\t\t\t\tdLessSum = dLessSum + arydValue[i];
\t\t\t\t\tnDivIdx = nDivIdx + 1;
\t\t\t\telse
\t\t\t\t\tdMoreSum = dMoreSum + arydValue[i];
\t\t\t\tend
\t\t\tend

\t\t\tlocal dLessMean = ( dLessSum / (nDivIdx - nS) + arydValue[nDivIdx-1] ) / 2;
\t\t\tlocal dMoreMean = ( dMoreSum / (nE - nDivIdx + 1) + arydValue[nDivIdx] ) / 2;
\t\t\tlocal dLessProb = ( dMoreMean - dD ) / ( dMoreMean - dLessMean );
\t\t\tlocal dMoreProb = 1 - dLessProb;

\t\t\tfor i = nS, nDivIdx - 1 do
\t\t\t\tarydProb[i] = arydProb[i] * dLessProb;
\t\t\tend
\t\t\tfor i = nDivIdx, nE do
\t\t\t\tarydProb[i] = arydProb[i] * dMoreProb;
\t\t\tend

\t\t\tif( nS < nDivIdx - 1 ) then
\t\t\t\tnTop = nTop + 1;
\t\t\t\taryS[nTop] = nS;
\t\t\t\taryE[nTop] = nDivIdx - 1;
\t\t\t\taryD[nTop] = dLessMean;
\t\t\tend
\t\t\tif( nDivIdx < nE ) then
\t\t\t\tnTop = nTop + 1;
\t\t\t\taryS[nTop] = nDivIdx;
\t\t\t\taryE[nTop] = nE;
\t\t\t\taryD[nTop] = dMoreMean;
\t\t\tend
\t\tend
\tend
end
"""


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== vS_calcprob_vonglap - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    if not os.path.isfile(DICH):
        print("!!! LOI TO: khong thay %s" % DICH); return 1
    raw = io.open(DICH, "rb").read().decode("latin-1")
    eol = "\r\n" if "\r\n" in raw else "\n"
    hi0 = sum(1 for c in raw if ord(c) > 127)

    if "VONG LAP thay cho DE QUY" in raw:
        print("  DA CO - bo qua (idempotent)"); return 0

    dong = raw.split(eol)
    # ranh gioi: tu dong dau tien cua khoi chot/ham calcProbLoop
    # den dong ngay TRUOC chu thich cua inssort
    i_dau = None
    for i, l in enumerate(dong):
        if l.startswith("-- [LOREN 27/08] CHOT DO SAU") or l.startswith("CALCPROB_MAX_DEPTH"):
            i_dau = i
            break
        if l.startswith("function calcProbLoop"):
            i_dau = i
            break
    if i_dau is None:
        print("!!! LOI TO: khong tim thay dau ham calcProbLoop"); return 1

    i_cuoi = None
    for j in range(i_dau + 1, len(dong)):
        if dong[j].startswith("function inssort"):
            i_cuoi = j
            break
    if i_cuoi is None:
        print("!!! LOI TO: khong tim thay function inssort de chot ranh gioi"); return 1
    # lui ve truoc cac dong chu thich cua inssort
    k = i_cuoi - 1
    while k > i_dau and (dong[k].strip() == "" or dong[k].strip().startswith("--")):
        k -= 1
    i_cuoi = k + 1

    cu = eol.join(dong[i_dau:i_cuoi])
    if "calcProbLoop" not in cu:
        print("!!! LOI TO: vung cat khong chua calcProbLoop"); return 1
    n_de_quy = cu.count("calcProbLoop( arydValue")
    print("  vung thay: dong %d..%d (%d dong) | %d lenh de quy ben trong"
          % (i_dau + 1, i_cuoi, i_cuoi - i_dau, n_de_quy))

    moi = eol.join(dong[:i_dau]) + eol + HAM_MOI.replace("\n", eol) + eol.join(dong[i_cuoi:])

    hi1 = sum(1 for c in moi if ord(c) > 127)
    # Vung thay chua chu thich tieng Trung goc (byte cao GBK). Ban vong lap dung
    # chu thich ASCII nen byte cao GIAM - chap nhan duoc, ban goc con nguyen o
    # tep sao luu .truoc_vonglap. Chi CAM byte cao TANG (dau hieu hong ma hoa).
    if hi1 > hi0:
        print("!!! LOI TO: byte cao TANG %d -> %d (nghi hong ma hoa)" % (hi0, hi1)); return 1
    if "�" in moi:
        print("!!! LOI TO: co ky tu FFFD"); return 1
    print("  byte cao %d -> %d (giam do bo chu thich GBK cua vung thay - co backup)"
          % (hi0, hi1))
    # can bang tu khoa
    code = eol.join(l.split("--")[0] if not l.strip().startswith("--") else ""
                    for l in moi.split(eol))
    mo = len(re.findall(r"\b(function|if|for|while)\b", code))
    het = len(re.findall(r"\bend\b", code))
    print("  mo=%d end=%d %s" % (mo, het, "CAN BANG" if mo == het else "!!! LECH"))
    if mo != het:
        print("!!! LOI TO: cu phap lech - KHONG ghi gi"); return 1
    # chi duoc xuat hien DUNG 1 lan = dong khai bao ham; nhieu hon = con goi de quy
    n_xh = HAM_MOI.count("calcProbLoop( arydValue")
    if n_xh != 1:
        print("!!! LOI TO: ban moi con %d loi goi de quy" % (n_xh - 1)); return 1
    print("  ban moi: 0 lenh de quy (dung ngan xep tu quan)")

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
