-- ruong_datau_tasklink.lua - [JX1 PORT 16/08/2026, sua v2 theo chu game]
-- "Bao ruong than bi cua Da Tau" (6/1/2383; = item 2374 ban Linux).
-- Goc Linux (activity 17 + bang 71): can 6 Huyen Thien Chuy + 1 o trong.
-- [v2 - chu game chot] BO 3 lenh Van Loc / Thuong Lang / Huyen Vien
-- (2+0.05+0.008 = 2.058%%) -> don vao Khieu chien Le bao: 27.942+2.058 = 30%% tron.
-- Bang roll x1000 (1..100000):
--   25%% Dai Luc hoan | 25%% Phi Toc hoan | 30%% Khieu chien Le bao
--   10%% Que Huy Hoang cao (han 7 ngay) | 5%% Tien Thao Lo
--   3%% Cam nang doi troi dat (param 60) | 2%% Tien Thao Lo dac biet x3

function main(nItemIndex)
	if (GetItemCount(0, 6, 1, 2357) < 6) then
		Say("C«n 6 Huy“n Thi™n ChÔy, c∏c hπ Æem kh´ng ÆÒ sË l≠Óng!", 0)
		return 1
	end
	if (CalcFreeItemCellCount() < 2) then
		Say("Hµnh trang Æ∑ Æ«y, h∑y sæp x’p lπi cho ng®n næp.", 0)
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
	elseif (n <= 80000) then
		AddItem(6, 1, 2015, 1, 0, 0, 0)
	elseif (n <= 90000) then
		nIdx = AddItem(6, 1, 907, 1, 0, 0, 0)
		if (nIdx > 0) then
			AddTimeItem(nIdx, 604800)
		end
	elseif (n <= 95000) then
		AddItem(6, 1, 71, 1, 0, 0, 0)
	elseif (n <= 98000) then
		nIdx = AddItem(6, 1, 1790, 1, 0, 0, 0)
		if (nIdx > 0) then
			SetParamItem(nIdx, 60)
		end
	else
		AddItem(6, 1, 1182, 1, 0, 0, 0)
		AddItem(6, 1, 1182, 1, 0, 0, 0)
		AddItem(6, 1, 1182, 1, 0, 0, 0)
	end
	Msg2Player("Ban da mo B∂o r≠¨ng th«n b› cÒa D∑ T»u!")
	WriteLog(format("[DaTau ruong 2383] %s mo ruong, roll=%d", GetName() or "", n))
	RemoveItemByIndex(nItemIndex)
	return 0
end
