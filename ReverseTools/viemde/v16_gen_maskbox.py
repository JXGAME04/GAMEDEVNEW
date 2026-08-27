# -*- coding: utf-8 -*-
"""VIEM DE - sinh ban JX1 cua \\script\\vng_event\\item\\boxmask\\mask_param.lua
(script cua "Hop Mat Na Chien Truong", mot trong 4 phan thuong boss cuoi).

=================== DICH NGUOC, KHONG CHEP QUA LOA =========================
Ban Linux (44 dong) lam:
  main(nItemIndex):
     - PlayerFunLib:VnCheckInCity("default") ~= 1  -> khong cho dung
     - doc 4 tham so cua chinh cai hop:
          GetItemParam(idx,1) = 1 Vuong Gia / 2 Ba Gia
          GetItemParam(idx,2) = nExpiredTime (phut)
          GetItemParam(idx,3) = nUsageTime
          GetItemParam(idx,4) = nBindState
     - tbAwardTemplet:Give({tbProp={0,11,647|828,1,0,0}, nExpiredTime=..,
                            nUsageTime=.., nBindState=..})
  GetDesc(nItemIndex): tra ten mat na tuong ung.

Da soat tung manh o JX1 truoc khi viet:
  * mat na 647/828: bang settings\\item\\mask.txt cua JX1 THANG HANG voi ban Linux
    (id = so dong - 1): 647 = "Mat na chien truong Vuong Gia", 828 = "... Ba Gia". CO SAN.
  * PlayerFunLib:VnCheckInCity: CO (script\\vng_feature\\checkinmap.lua:2), da co
    item khac cua JX1 dung (kinhmach\\honnguyenchandon.lua:32).
  * GetItemParam(idx,k) k=1..6: CO - doc m_GeneratorParam.nGeneratorLevel[k-1]
    (ScriptFuns LuaGetItemParam).
  * tbAwardTemplet:Give cua JX1 (lib\\awardtype\\item_jx1.lua) CHI hieu tbProp/nCount/
    nQuality - BO QUA nExpiredTime/nUsageTime/nBindState (chinh tep do ghi ro
    "nExpiredTime bo qua (lech co chu dich)"). Nen o day KHONG dung Give ma tu
    AddItem roi ap han dung / khoa bang API that cua JX1:
        ITEM_SetExpiredTime(idx, so_phut)   -> KJx2WarInfra.cpp:1399, <20000000 = PHUT
        ITEM_SetLeftUsageTime(idx, n)       -> KJx2WarInfra.cpp:1481
        SetItemBindState(idx, -2)           -> KJx2WarInfra.cpp:1504 (SetPlayerItemLock)
  * AddItem cua JX1 doi TOI THIEU 7 tham so (bay #2).
============================================================================
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bangtxt import uni2tcvn

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"
DICH = r"\script\item\viemde\mask_param.lua"

L = []


def s(txt):
    L.append(uni2tcvn(txt))


s("-- ==========================================================================")
s("-- HOP MAT NA CHIEN TRUONG (Vuong Gia / Ba Gia)  - ban JX1")
s("-- SINH TU DONG boi ReverseTools\\viemde\\v16_gen_maskbox.py - DUNG SUA TAY")
s("--")
s("-- Dich tu \\script\\vng_event\\item\\boxmask\\mask_param.lua cua ban Linux.")
s("-- Khac ban Linux DUY NHAT o cho trao vat pham: tbAwardTemplet:Give cua JX1")
s("-- (lib\\awardtype\\item_jx1.lua) BO QUA nExpiredTime/nUsageTime/nBindState,")
s("-- nen o day tu AddItem roi ap han dung/khoa bang API that cua JX1.")
s("-- ==========================================================================")
s("")
s('Include("\\\\script\\\\lib\\\\log.lua")')
s('Include("\\\\script\\\\vng_feature\\\\checkinmap.lua")')
s("")
s("-- 647 / 828 la so dong - 1 trong settings\\item\\mask.txt (JX1 thang hang voi ban Linux)")
s("YDBZ_MATNA =")
s("{")
s('\t[1] = {szName = "%s", nPart = 647},' % "Mặt nạ chiến trường Vương Giả")
s('\t[2] = {szName = "%s", nPart = 828},' % "Mặt nạ chiến trường Bá Giả")
s("}")
s("")
s("function main(nItemIndex)")
s('\tif PlayerFunLib:VnCheckInCity("default") ~= 1 then')
s("\t\treturn 1")
s("\tend")
s("\tlocal nItemParam   = GetItemParam(nItemIndex, 1) or 0")
s("\tlocal nExpiredTime = GetItemParam(nItemIndex, 2) or 0")
s("\tlocal nUsageTime   = GetItemParam(nItemIndex, 3) or 0")
s("\tlocal nBindState   = GetItemParam(nItemIndex, 4) or 0")
s("\tlocal tbMatNa = YDBZ_MATNA[nItemParam]")
s("\tif not tbMatNa then")
s('\t\tMsg2Player("%s")' % "Hộp này chưa được thiết lập loại mặt nạ.")
s("\t\treturn 1")
s("\tend")
s("\tif CalcFreeItemCellCount() < 1 then")
s('\t\tMsg2Player("%s")' % "Hành trang đã đầy, hãy dọn bớt rồi dùng lại.")
s("\t\treturn 1")
s("\tend")
s("\t-- bay #2: AddItem cua JX1 doi TOI THIEU 7 tham so")
s("\tlocal nIdx = AddItem(0, 11, tbMatNa.nPart, 1, 0, 0, 0)")
s("\tif not nIdx or nIdx <= 0 then")
s('\t\tWriteLog(format("[ViemDe HopMatNa] %s: KHONG tao duoc mat na %d", GetName() or "", tbMatNa.nPart))')
s("\t\treturn 1")
s("\tend")
s("\t-- ITEM_SetExpiredTime: gia tri < 20000000 = SO PHUT ke tu bay gio")
s("\tif tonumber(nExpiredTime) and tonumber(nExpiredTime) > 0 then")
s("\t\tITEM_SetExpiredTime(nIdx, tonumber(nExpiredTime))")
s("\tend")
s("\tif tonumber(nUsageTime) and tonumber(nUsageTime) > 0 then")
s("\t\tITEM_SetLeftUsageTime(nIdx, tonumber(nUsageTime))")
s("\tend")
s("\tif tonumber(nBindState) and tonumber(nBindState) ~= 0 then")
s("\t\tSetItemBindState(nIdx, -2)")
s("\tend")
s('\tMsg2Player(format("%s <color=yellow>%s<color>.", "' + uni2tcvn("Bạn nhận được") + '", tbMatNa.szName))')
s('\tWriteLog(format("[ViemDe HopMatNa] %s nhan %s (han %s phut, khoa %s)", GetName() or "", tbMatNa.szName, tostring(nExpiredTime), tostring(nBindState)))')
s("\treturn 0")
s("end")
s("")
s("function GetDesc(nItemIndex)")
s("\tlocal nItemParam = GetItemParam(nItemIndex, 1) or 0")
s("\tlocal tbMatNa = YDBZ_MATNA[nItemParam]")
s("\tif not tbMatNa then")
s("\t\treturn 1")
s("\tend")
s('\treturn "<color=yellow>"..tbMatNa.szName.."<color>"')
s("end")
s("")

noi_dung = "\r\n".join(L)
for goc in (SRV, GUONG):
    p = goc + DICH
    os.makedirs(os.path.dirname(p), exist_ok=True)
    tam = p + ".dangghi"
    with open(tam, "wb") as f:
        f.write(noi_dung.encode("latin-1"))
    os.replace(tam, p)
    print("da ghi %s (%d dong)" % (p, len(L)))
