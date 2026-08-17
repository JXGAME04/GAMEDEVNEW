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
		Say("CÇn 6 HuyÒn Thiªn Chïy, c¸c h¹ ®em kh«ng ®ñ sè l­îng!", 0)
		return 1
	end
	if (CalcFreeItemCellCount() < 4) then -- [PB 17/08] nhanh 2% can 3 o + phong nguoi cam do tren chuot
		Say("Hµnh trang ®· ®Çy, h·y s¾p xÕp l¹i cho ng¨n n¾p.", 0)
		return 1
	end
	if (ConsumeItem(6, 0, 6, 1, 2357) ~= 6) then
		return 1
	end
	local n = C_Random(1, 100000)
	local nIdx = 0
	local nOk = 1
	if (n <= 25000) then
		nOk = AddItem(6, 1, 3, 1, 0, 0, 0)
	elseif (n <= 50000) then
		nOk = AddItem(6, 1, 6, 1, 0, 0, 0)
	elseif (n <= 80000) then
		nOk = AddItem(6, 1, 2015, 1, 0, 0, 0)
	elseif (n <= 90000) then
		nIdx = AddItem(6, 1, 907, 1, 0, 0, 0)
		nOk = nIdx
		if (nIdx > 0) then
			AddTimeItem(nIdx, 604800)
		end
	elseif (n <= 95000) then
		nOk = AddItem(6, 1, 71, 1, 0, 0, 0)
	elseif (n <= 98000) then
		nIdx = AddItem(6, 1, 1790, 1, 0, 0, 0)
		nOk = nIdx
		if (nIdx > 0) then
			SetParamItem(nIdx, 60)
		end
	else
		nOk = AddItem(6, 1, 1182, 1, 0, 0, 0)
		AddItem(6, 1, 1182, 1, 0, 0, 0)
		AddItem(6, 1, 1182, 1, 0, 0, 0)
	end
	if (nOk == nil or nOk <= 0) then
		-- [PB 17/08] pool item can/loi: GIU ruong lai (nguoi choi mat 6 chia, co log de GM den)
		WriteLog(format("[DaTau ruong 2383] LOI AddItem fail, roll=%d, nguoi=%s - ruong GIU lai", n, GetName() or ""))
		Msg2Player("Co loi khi mo ruong, ruong van con - hay bao GM!")
		return 1
	end
	Msg2Player("Ban da mo B¶o r­¬ng thÇn bÝ cña D· TÈu!")
	WriteLog(format("[DaTau ruong 2383] %s mo ruong, roll=%d", GetName() or "", n))
	RemoveItemByIndex(nItemIndex)
	return 0
end
