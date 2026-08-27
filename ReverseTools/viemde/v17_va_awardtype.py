# -*- coding: utf-8 -*-
"""VIEM DE - mo rong lib\\awardtype\\item_jx1.lua de hieu tbParam / nExpiredTime /
nUsageTime / nBindState.

VI SAO CAN:
  head.lua cua Viem De trao thuong boss cuoi bang cac muc dang
      {szName="Hop Mat Na Chien Truong", tbProp={6,1,4865,1,0,0}, nCount=1,
       nRate=11.15, nExpiredTime=10080, tbParam={1,10080,60,0,0,0}}
  * tbParam la 6 O THAM SO cua chinh cai hop; script cua hop doc lai bang
    GetItemParam(idx,1..4) de biet phat mat na Vuong Gia hay Ba Gia, han bao lau.
  * ItemJX1Type:Give cua JX1 (ban port 15/08) CHI hieu tbProp/nCount/nQuality
    -> hop roi ra se co tham so = 0 -> script tra "chua thiet lap loai mat na".
  * Cac muc Phi Phong cung dung nQuality=1 + nExpiredTime.

CACH LAM AN TOAN (tep nay DUNG CHUNG cho nhieu tinh nang - Da Tau, Tong chien...):
  chi THEM nhanh moi, chay KHI VA CHI KHI muc thuong CO cac truong do.
  Muc khong co -> di duong cu, hanh vi khong doi mot ly nao.

BAY DA DOC KY (ScriptFuns.cpp LuaAddItem):
  AddItem chi chep RIENG 6 o nItemLevel[0..5] khi truyen >= 15 doi so;
  it hon thi no NHAN BAN doi so thu 7 ra ca 6 o. Nen nhanh co tbParam phai
  goi du 15 doi so: (class, detail, part, level, series, luck,
                     p1..p6, nStackNum, nEnChance, nPoint).
  GetItemParam(idx,k) k=1..6 doc chinh 6 o do (m_GeneratorParam.nGeneratorLevel[k-1]).

  ITEM_SetExpiredTime(idx, n): n < 20000000 = SO PHUT ke tu bay gio
                               (KJx2WarInfra.cpp:1399).
  SetItemBindState(idx, -2)  : khoa vat pham (KJx2WarInfra.cpp:1504).
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde"
REL = r"\script\lib\awardtype\item_jx1.lua"
BAK = ".truoc_viemde_2608"
NHAN = "[VIEMDE 26/08]"

NEO = """		if tbItem.nQuality == 1 and (tbProp[1] or 0) == 0 then
			nItemIdx = AddItem2(2, 0, tbProp[2] or 0, 0, 0, 0)
		else
			nItemIdx = AddItem(tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0,
				tbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0, 0)
		end"""

MOI = """		if tbItem.nQuality == 1 and (tbProp[1] or 0) == 0 then
			nItemIdx = AddItem2(2, 0, tbProp[2] or 0, 0, 0, 0)
		elseif tbItem.tbParam then
			-- """ + NHAN + """ muc thuong co 6 O THAM SO rieng (vd Hop Mat Na cua
			-- Viem De: {1,10080,60,0,0,0}). BAY: LuaAddItem chi chep rieng 6 o khi
			-- truyen >= 15 doi so; it hon thi no nhan ban doi so thu 7 ra ca 6 o.
			local tp = tbItem.tbParam
			nItemIdx = AddItem(tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0,
				tbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0,
				tp[1] or 0, tp[2] or 0, tp[3] or 0, tp[4] or 0, tp[5] or 0, tp[6] or 0,
				1, 0, 0)
		else
			nItemIdx = AddItem(tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0,
				tbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0, 0)
		end
		-- """ + NHAN + """ han dung / khoa: chi chay khi muc thuong CO khai bao,
		-- nen moi muc cu (khong co truong nay) van y nguyen nhu truoc.
		if nItemIdx and nItemIdx > 0 then
			if tbItem.nExpiredTime and tbItem.nExpiredTime > 0 then
				ITEM_SetExpiredTime(nItemIdx, tbItem.nExpiredTime)
			end
			if tbItem.nUsageTime and tbItem.nUsageTime > 0 then
				ITEM_SetLeftUsageTime(nItemIdx, tbItem.nUsageTime)
			end
			if tbItem.nBindState and tbItem.nBindState ~= 0 then
				SetItemBindState(nItemIdx, -2)
			end
		end"""


def main():
    p = SRV + REL
    d = io.open(p, encoding="latin-1", newline="").read()
    if NHAN in d:
        print("da va roi - bo qua.")
        return
    n = d.count(NEO.replace("\n", "\r\n"))
    neo = NEO.replace("\n", "\r\n") if n == 1 else NEO
    if n != 1:
        n = d.count(NEO)
        if n != 1:
            print("!! NEO KHONG DUY NHAT (%d lan) - DUNG LAI, khong sua gi" % n)
            return
    moi = MOI.replace("\r\n", "\n").replace("\n", "\r\n") if "\r\n" in d else MOI
    d2 = d.replace(neo, moi)
    assert d2 != d
    if not os.path.isfile(p + BAK):
        shutil.copyfile(p, p + BAK)
    tam = p + ".dangghi"
    with open(tam, "wb") as f:
        f.write(d2.encode("latin-1"))
    os.replace(tam, p)
    # guong
    q = GUONG + REL
    os.makedirs(os.path.dirname(q), exist_ok=True)
    shutil.copyfile(p, q)
    print("da va %s" % p)

    a = open(p + BAK, "rb").read()
    b = open(p, "rb").read()
    print("KIEM BYTE: CRLF %d->%d  LFdon %d->%d  FFFD=%d  byte cao %d->%d"
          % (a.count(b"\r\n"), b.count(b"\r\n"),
             a.count(b"\n") - a.count(b"\r\n"), b.count(b"\n") - b.count(b"\r\n"),
             b.count(b"\xef\xbf\xbd"),
             sum(1 for x in a if x >= 0x80), sum(1 for x in b if x >= 0x80)))
    # Tep NAY von dung LF (khong phai CRLF nhu cac bang .txt). Luat dung la GIU
    # NGUYEN quy uoc cua tep goc, chu khong phai "phai la CRLF".
    crlf_goc = a.count(b"\r\n") > 0
    crlf_moi = b.count(b"\r\n") > 0
    assert crlf_goc == crlf_moi, "kieu xuong dong bi doi (goc CRLF=%s, moi CRLF=%s)" % (crlf_goc, crlf_moi)
    print("   kieu xuong dong: goc %s -> moi %s (giu nguyen)"
          % ("CRLF" if crlf_goc else "LF", "CRLF" if crlf_moi else "LF"))


main()
