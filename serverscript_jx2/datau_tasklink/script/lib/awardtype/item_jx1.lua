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
		local nItemIdx = AddItem(tbProp[1] or 0, tbProp[2] or 0, tbProp[3] or 0,
			tbProp[4] or 1, tbProp[5] or 0, tbProp[6] or 0, 0)
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
