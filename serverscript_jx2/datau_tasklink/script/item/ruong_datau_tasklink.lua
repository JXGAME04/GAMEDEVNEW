-- ruong_datau_tasklink.lua - [JX1 PORT 16/08/2026]
-- "Bao ruong than bi cua Da Tau" (6/1/2383, ten trung voi item 2374 ban Linux).
-- Port dung co che goc (activity 17 config 1 + bang thuong 71 cua Linux):
--   - Can 6 Huyen Thien Chuy (JX1 id 6/1/2357; Linux 2348) + 1 o trong
--   - Roll DUNG ti le bang 71 (tong 100%%): x1000 de giu chinh xac 0.008%%
--   - Item remap theo TEN sang id JX1: 6/0/3->6/1/3, 6/0/6->6/1/6, 2006->2015,
--     906->907 (han 7 ngay), 1781->1790 (param 60), 1181->1182 x3,
--     2351->2360, 2352->2361, 2353->2362, 71 giu nguyen.

function main(nItemIndex)
	if (GetItemCount(0, 6, 1, 2357) < 6) then
		Say("CÇn 6 HuyÒn Thiªn Chïy, c¸c h¹ ®em kh«ng ®ñ sè l­îng!", 0)
		return 1
	end
	if (CalcFreeItemCellCount() < 2) then
		Say("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cho ng¨n n¾p.", 0)
		return 1
	end
	if (ConsumeItem(6, 0, 6, 1, 2357) ~= 6) then
		return 1
	end
	local n = C_Random(1, 100000)
	local nIdx = 0
	if (n <= 25000) then
		AddItem(6, 1, 3, 1, 0, 0, 0)
	elseif (n <= 50000) then
		AddItem(6, 1, 6, 1, 0, 0, 0)
	elseif (n <= 77942) then
		AddItem(6, 1, 2015, 1, 0, 0, 0)
	elseif (n <= 87942) then
		nIdx = AddItem(6, 1, 907, 1, 0, 0, 0)
		if (nIdx > 0) then
			AddTimeItem(nIdx, 604800)
		end
	elseif (n <= 92942) then
		AddItem(6, 1, 71, 1, 0, 0, 0)
	elseif (n <= 95942) then
		nIdx = AddItem(6, 1, 1790, 1, 0, 0, 0)
		if (nIdx > 0) then
			SetParamItem(nIdx, 60)
		end
	elseif (n <= 97942) then
		AddItem(6, 1, 1182, 1, 0, 0, 0)
		AddItem(6, 1, 1182, 1, 0, 0, 0)
		AddItem(6, 1, 1182, 1, 0, 0, 0)
	elseif (n <= 99942) then
		AddItem(6, 1, 2362, 1, 0, 0, 0)
	elseif (n <= 99992) then
		AddItem(6, 1, 2361, 1, 0, 0, 0)
	else
		AddItem(6, 1, 2360, 1, 0, 0, 0)
	end
	Msg2Player("Ban da mo B¶o r­¬ng thÇn bÝ cña D· TÈu!")
	WriteLog(format("[DaTau ruong 2383] %s mo ruong, roll=%d", GetName() or "", n))
	RemoveItemByIndex(nItemIndex)
	return 0
end
