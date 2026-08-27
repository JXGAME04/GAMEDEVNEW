-- item_jx1.lua - JX1 PORT 15/08/2026
-- Thay the awardtype\item.lua cua ban goc: NewItemEx (JX2, 17 tham so)
-- khong ton tai tren JX1 nen trao vat pham bang AddItem cua JX1.
-- Dang ky khoa "tbProp" giong het ban goc de tbAwardTemplet:Give hoat dong.
-- Da Tau chi dung dang: tbProp = {genre, detail, particular, level, series, luck}
-- (vd ruong moc 10 nhiem vu: {6, 1, 2374, 1, 0, 0}).

Include("\\script\\lib\\log.lua")

ItemJX1Type = {}

function ItemJX1Type:Give(tbItem, nAwardCount, tbLogTitle)
	local tbProp = tbItem.tbProp
	if not tbProp then
		return 0
	end
	local nAmount = (nAwardCount or 1) * (tbItem.nCount or 1)
	local szWhere = "award"
	if tbLogTitle and type(tbLogTitle) == "table" and tbLogTitle[1] then
		szWhere = tbLogTitle[1]
	end
	local i
	for i = 1, nAmount do
		local nItemIdx
		-- [TONGWAR 23/08] goldequip (nQuality == 1, tbProp = {0, record goldequip.txt}): AddItem2
		-- NATURE_GOLD nhu event\30thang4\EventLib.lua:176; nExpiredTime bo qua (lech co chu dich)
		if tbItem.nQuality == 1 and (tbProp[1] or 0) == 0 then
			nItemIdx = AddItem2(2, 0, tbProp[2] or 0, 0, 0, 0)
		elseif tbItem.tbParam then
			-- [VIEMDE 26/08] muc thuong co 6 O THAM SO rieng (vd Hop Mat Na cua
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
		-- [VIEMDE 26/08] han dung / khoa: chi chay khi muc thuong CO khai bao,
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
		end
		if nItemIdx and nItemIdx > 0 then
			WriteLog(format("[DaTau awardtype item] %s: %s nhan item %d,%d,%d",
				szWhere, GetName() or "", tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0))
		end
	end
	return 1
end

-- Dang ky co dieu kien: khi file nay boot doc lap (state rieng) thi
-- tbAwardTemplet chua ton tai -> bo qua, khong bao loi boot.
if tbAwardTemplet then
	tbAwardTemplet:RegType("tbProp", ItemJX1Type)
end
