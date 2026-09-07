
IncludeLib("ITEM");
Include("\\script\\item\\itemvalue\\fantasygoldessence.lua");

RESULT_UNKNOWN = -1;
RESULT_SUCCEED = 0;
RESULT_FAIL = 1;
RESULT_NO_MONEY = 2;
RESULT_LEVEL_FULL = 3;
RESULT_LACK_RESOURCE = 4;
RESULT_LEVEL_ERROR = 5;
RESULT_MAGIC_ERROR = 6;
RESULT_SERIES_ERROR = 7;
RESULT_RULE_ERROR = 8;

-- ®Æt l¹i d÷ liÖu
g_nOreGenre = 0;
g_nOreDetailType = 0;
g_nOreParticular = 0;
g_nOreLevel = 0;

-- KiÓm nguyªn liÖu cã ®óng luËt, ®ång thêi cÊt l¹i vµi sè liÖu.
function verifySrcItems(arynNecessaryItemIdx)
	local nNecessaryItemCount = getn(arynNecessaryItemIdx);
	if nNecessaryItemCount ~= 3 then
		return RESULT_LACK_RESOURCE;
	end

	local nGenre, nDetailType, nParticular, nLevel, nSeries = GetItemProp(arynNecessaryItemIdx[1]);
	-- [JX1] chot loc: chi Huyen Kim Chi Tinh 6/1/3770 moi vao duoc.
	-- Ban goc khong can vi cua so lo ren cua client da loc san o so 8.
	if( nGenre ~= 6 or nDetailType ~= 1 or nParticular ~= 3770 ) then
		return RESULT_RULE_ERROR;
	end
	if( nLevel >= 4 ) then
		return RESULT_LEVEL_FULL;
	end

	g_nOreGenre = nGenre;
	g_nOreDetailType = nDetailType;
	g_nOreParticular = nParticular;
	g_nOreLevel = nLevel;

	for i = 2, 3 do
		local nGenre, nDetailType, nParticular, nLevel, nSeries = GetItemProp(arynNecessaryItemIdx[i]);
		if g_nOreGenre ~= nGenre or g_nOreDetailType ~= nDetailType or g_nOreParticular ~= nParticular then
			return RESULT_LACK_RESOURCE;
		end
		if( g_nOreLevel ~= nLevel ) then
			return RESULT_LEVEL_ERROR;
		end
	end

	local nBind = 0;
	for i = 1, nNecessaryItemCount do
		if GetItemBindState(arynNecessaryItemIdx[i]) ~= 0 then
			nBind = 1
			break;
		end
	end

	return RESULT_SUCCEED, nLevel + 1, nBind;
end

function Compound(arynNecessaryItemIdx, arynAlternativeItemIdx, nExParam)

	-- KiÓm nguyªn liÖu cã ®óng luËt, ®ång thêi cÊt l¹i vµi sè liÖu.
	local nResult, nWantLevel, nBind = verifySrcItems(arynNecessaryItemIdx);
	if( nResult ~= RESULT_SUCCEED ) then
		return -1, nResult;
	end

	local nItemValue = CalcFantasyGoldEssenceValueByLevel(nWantLevel);
	if nItemValue == 0 then
		return RESULT_LEVEL_ERROR;
	end
	
	-- Phİ mét lÇn lß rÌn
	local nPay = nItemValue * 100000;
	if Pay(nPay) ~= 1 then
		return -1, RESULT_NO_MONEY;
	end

	-- Dùng danh s¸ch mãn ®İch.
	local nLatestItemVer = ITEM_GetLatestItemVersion();
	local aryDesItemInfo = {
		nLatestItemVer, 
		0, 
		0, 
		g_nOreGenre, 
		g_nOreDetailType, 
		g_nOreParticular, 
		g_nOreLevel + 1, 
		g_nOreSeries, 
		0,
	};

	-- Chän mãn ®İch, xo¸ nguyªn liÖu, kÕt thóc.
	local nResultDesItemIdx = AddItemEx(
		aryDesItemInfo[1], 
		aryDesItemInfo[2], 
		aryDesItemInfo[3], 
		aryDesItemInfo[4], 
		aryDesItemInfo[5], 
		aryDesItemInfo[6], 
		aryDesItemInfo[7], 
		aryDesItemInfo[8], 
		aryDesItemInfo[9], 
		0
	);

	if nBind == 1 then
		SetItemBindState(nResultDesItemIdx, -2);
	end

	if nResultDesItemIdx > 0 then
		local nItemCount = getn(arynNecessaryItemIdx);
		for i = 1, nItemCount do
			if arynNecessaryItemIdx[i] ~= nil then
				RemoveItemByIndex(arynNecessaryItemIdx[i], GetItemStackCount(arynNecessaryItemIdx[i]));
			end
		end
		WriteLog(format("[N©ng cÊp Huyªn Kim Chi Tinh]\ttµi kho¶n:%s\tnh©n vËt:%s\ttiÒn v¹n tiªu hao%d\tnhËn mét c¸i%dcÊp HuyÒn Kim Chi Tinh thµnh c«ng", GetAccount(), GetName(), nPay, nWantLevel));
		return nResultDesItemIdx, RESULT_SUCCEED;
	else
		WriteLog(format("[N©ng cÊp Huyªn Kim Chi Tinh]\ttµi kho¶n:%s\tnh©n vËt:%s\ttiÒn v¹n tiªu hao%d\tnhËn mét c¸i%dcÊp HuyÒn Kim Chi Tinh thÊt b¹i", GetAccount(), GetName(), nPay, nWantLevel));
		return -1, RESULT_FAIL;
	end
end


