-- ==========================================================================
-- HOP MAT NA CHIEN TRUONG (Vuong Gia / Ba Gia)  - ban JX1
-- SINH TU DONG boi ReverseTools\viemde\v16_gen_maskbox.py - DUNG SUA TAY
--
-- Dich tu \script\vng_event\item\boxmask\mask_param.lua cua ban Linux.
-- Khac ban Linux DUY NHAT o cho trao vat pham: tbAwardTemplet:Give cua JX1
-- (lib\awardtype\item_jx1.lua) BO QUA nExpiredTime/nUsageTime/nBindState,
-- nen o day tu AddItem roi ap han dung/khoa bang API that cua JX1.
-- ==========================================================================

Include("\\script\\lib\\log.lua")
Include("\\script\\vng_feature\\checkinmap.lua")

-- 647 / 828 la so dong - 1 trong settings\item\mask.txt (JX1 thang hang voi ban Linux)
YDBZ_MATNA =
{
	[1] = {szName = "M∆t nπ chi’n tr≠Íng V≠¨ng Gi∂", nPart = 647},
	[2] = {szName = "M∆t nπ chi’n tr≠Íng B∏ Gi∂", nPart = 828},
}

function main(nItemIndex)
	if PlayerFunLib:VnCheckInCity("default") ~= 1 then
		return 1
	end
	local nItemParam   = GetItemParam(nItemIndex, 1) or 0
	local nExpiredTime = GetItemParam(nItemIndex, 2) or 0
	local nUsageTime   = GetItemParam(nItemIndex, 3) or 0
	local nBindState   = GetItemParam(nItemIndex, 4) or 0
	local tbMatNa = YDBZ_MATNA[nItemParam]
	if not tbMatNa then
		Msg2Player("HÈp nµy ch≠a Æ≠Óc thi’t lÀp loπi m∆t nπ.")
		return 1
	end
	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang Æ∑ Æ«y, h∑y d‰n bÌt rÂi dÔng lπi.")
		return 1
	end
	-- bay #2: AddItem cua JX1 doi TOI THIEU 7 tham so
	local nIdx = AddItem(0, 11, tbMatNa.nPart, 1, 0, 0, 0)
	if not nIdx or nIdx <= 0 then
		WriteLog(format("[ViemDe HopMatNa] %s: KHONG tao duoc mat na %d", GetName() or "", tbMatNa.nPart))
		return 1
	end
	-- ITEM_SetExpiredTime: gia tri < 20000000 = SO PHUT ke tu bay gio
	if tonumber(nExpiredTime) and tonumber(nExpiredTime) > 0 then
		ITEM_SetExpiredTime(nIdx, tonumber(nExpiredTime))
	end
	if tonumber(nUsageTime) and tonumber(nUsageTime) > 0 then
		ITEM_SetLeftUsageTime(nIdx, tonumber(nUsageTime))
	end
	if tonumber(nBindState) and tonumber(nBindState) ~= 0 then
		SetItemBindState(nIdx, -2)
	end
	Msg2Player(format("%s <color=yellow>%s<color>.", "Bn nhn c", tbMatNa.szName))
	WriteLog(format("[ViemDe HopMatNa] %s nhan %s (han %s phut, khoa %s)", GetName() or "", tbMatNa.szName, tostring(nExpiredTime), tostring(nBindState)))
	return 0
end

function GetDesc(nItemIndex)
	local nItemParam = GetItemParam(nItemIndex, 1) or 0
	local tbMatNa = YDBZ_MATNA[nItemParam]
	if not tbMatNa then
		return 1
	end
	return "<color=yellow>"..tbMatNa.szName.."<color>"
end
