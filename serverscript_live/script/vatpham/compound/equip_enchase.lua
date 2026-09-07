-- ¹¦ÄÜ£º×ÏÉ«×°±¸ÏµÍ³ - Ê¹ÓÃÄ§·¨ÊôÐÔ¿óÊ¯ÏâÇ¶µ½×ÏÉ«×°±¸
-- Fanghao Wu 2005.1.15

Include( "\\script\\item\\compound\\compound_header.lua" );
Include( "\\script\\item\\itemvalue\\magicattriblevel.lua" );
Include( "\\script\\item\\itemvalue\\itemvalue_header.lua" );

-- [ºÏ³É]·ÑÓÃ
COMPOUND_COST = 10000;


-- ³õÊ¼»¯Êý¾Ý
function initData()
	g_nEquipIdx = 0;
	g_nEquipVer = 0;
	g_nEquipRandSeed = 0;
	g_nEquipGenre = 0;
	g_nEquipDetailType = 0;
	g_nEquipParticular = 0;
	g_nEquipLevel = 0;
	g_nEquipSeries = 0;
	g_nEquipLuck = 0;
	g_arynEquipMagLvl = {}
	g_nEnchasePos = 0;
	g_nOreMagicID = 0;
end

-- »ñÈ¡[ºÏ³É]²Ù×÷²ÎÊý£¨ÓÃÓÚ´«µÝ¸øITEM_CalcItemValue¼ÆËãÎïÆ·¼ÛÖµÁ¿£©
function getCompoundParam()
	return "EQUIP_ENCHASE";
end

-- Ð£ÑéÔ­ÁÏÊÇ·ñ·ûºÏ[ºÏ³É]¹æÔò£¨Í¬Ê±´æ´¢Ò»Ð©Ô­ÁÏÊý¾Ý£©
function verifySrcItems( arynNecessaryItemIdx, arynAlternativeItemIdx )
	local nNecessaryItemCount = getn( arynNecessaryItemIdx );
	local nOreMagLvlPos = 0;
	local nOreSeries = 0;
	for i = 1, nNecessaryItemCount do
		local nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck = GetItemProp( arynNecessaryItemIdx[i] );
		if( nGenre == 0 ) then
			g_nEquipIdx = arynNecessaryItemIdx[i];
			g_nEquipVer = ITEM_GetItemVersion( arynNecessaryItemIdx[i] );
			g_nEquipRandSeed = ITEM_GetItemRandSeed( arynNecessaryItemIdx[i] );
			g_nEquipGenre = nGenre;
			g_nEquipDetailType = nDetailType;
			g_nEquipParticular = nParticular;
			g_nEquipLevel = nLevel;
			g_nEquipSeries = nSeries;
			g_nEquipLuck = nLuck;
			g_arynEquipMagLvl = GetItemAllParams( arynNecessaryItemIdx[i] );
			for j = 1, 6 do
				if( g_arynEquipMagLvl[j] == -1 ) then
					g_nEnchasePos = j;
					break;
				end
			end
		elseif( nGenre == 6 and nDetailType == 1 and nParticular >= 199 and nParticular <= 204 ) then
			nOreMagLvlPos = nParticular - 198;
			nOreSeries = nSeries;
			g_nOreMagicID = GetItemParam( arynNecessaryItemIdx[i], 1 );
		end
	end
	if( g_nEnchasePos <= 0 ) then
		return RESULT_LACK_RESOURCE;
	elseif( g_nEnchasePos ~= nOreMagLvlPos ) then
		return RESULT_RULE_ERROR;
	elseif( mod( nOreMagLvlPos, 2 ) == 0 and g_nEquipSeries ~= nOreSeries ) then
		return RESULT_SERIES_ERROR;
	elseif( g_nOreMagicID <= 0 ) then
		return RESULT_MAGIC_ERROR;
	elseif( isMagicMatchEquip( g_nEquipVer, g_nOreMagicID, g_nEquipDetailType, g_nEquipParticular ) ~= 1 ) then
		return RESULT_MAGIC_ERROR;
	elseif( isMagicMatchSeries( g_nEquipVer, g_nOreMagicID, g_nEquipSeries ) ~= 1 ) then
		return RESULT_MAGIC_ERROR;
	else
		for i = 1, g_nEnchasePos - 1  do
			if(	GetItemMagicAttrib( g_nEquipIdx, i ) == g_nOreMagicID ) then
				return RESULT_MAGIC_ERROR;
			end
		end
	end
	return RESULT_SUCCEED;
end

-- Éú³ÉÄ¿±êÎïÆ·ÐÅÏ¢
function genDesItemsInfo( arynNecessaryItemIdx )
	local aryDesItemInfo = {};
	local aryMagAttrLvlRange = getMagAttrLvlRange( g_nEquipVer, g_nOreMagicID, 1 );
	local nRangeCount = getn( aryMagAttrLvlRange );
	local nDesCount = 0;
	for i = 1, nRangeCount do
		for j = aryMagAttrLvlRange[i][1], aryMagAttrLvlRange[i][2] do
			if( j > 0 ) then
				local arynMagLvl = {};
				for i = 1, 6 do
					arynMagLvl[i] = g_arynEquipMagLvl[i];
				end
				arynMagLvl[g_nEnchasePos] = j;
				nDesCount = nDesCount + 1;
				aryDesItemInfo[nDesCount] = { g_nEquipVer, g_nEquipRandSeed, 2, g_nEquipGenre, g_nEquipDetailType, g_nEquipParticular, g_nEquipLevel, g_nEquipSeries, g_nEquipLuck, arynMagLvl, nil, getCompoundParam() };
			end
		end
	end
	return aryDesItemInfo;
end

-- Íê³ÉÑ¡ÔñÄ¿±êÎïÆ·¡¢É¾³ýÔ­ÁÏµÈÊÕÎ²²Ù×÷
function finalCompound( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo, arydDesItemVal )
	local nEquipVal = ITEM_CalcItemValue( g_nEquipIdx, getCompoundParam() );
	local nDesCount = getn( arydDesItemVal );
	for i = 1, nDesCount do
		if( nEquipVal < arydDesItemVal[i] ) then
			arydDesItemVal[nDesCount+1] = nEquipVal;			
			break;
		end
	end
	local nSelDesItemIdx, dDesItemVal, dDesProb = TransItemVal( nSrcItemValSum, arydDesItemVal );
	if( nSelDesItemIdx > nDesCount ) then
		nSelDesItemIdx = 0;
	end
	local nMaxDur = GetMaxDurability( g_nEquipIdx );
	local nCurDur = GetCurDurability( g_nEquipIdx );
	if( nSelDesItemIdx > 0 ) then
		local nDesItemValCut = floor( dDesItemVal / SUM_UNIT );
		if( nDesItemValCut >= COMMON_SUM_BASE_VAL ) then
			local nCommonItemVal = GetGlbValue( GLBID_COMMON_VAL_SUM );
			SetGlbValue( GLBID_COMMON_VAL_SUM, nCommonItemVal + nDesItemValCut );
			if( nCommonItemVal + nDesItemValCut >= COMMON_SUM_MAX_VAL ) then
				local strMsg = format( " [C¶nh b¸o]%s Tæng gi¸ trÞ vËt phÈm t­¬ng quan cña Trang bÞ HuyÒn Tinh [ghÐp]h«m nay (%0.2fE) , ®· v­ît qu¸ møc c¶nh b¸o tæng gi¸ trÞ (%0.2fE) , cã thÓ xuÊt hiÖn hiÖn t­îng quÐt vËt phÈm, h·y liªn hÖ khÈn víi bé phËn nghiªn cøu!!!  ", date( "%Y-%m-%d %H:%M:%S" ), ( nCommonItemVal + nDesItemValCut ) * SUM_UNIT / 100000000, COMMON_SUM_MAX_VAL * SUM_UNIT / 100000000 );
				print( strMsg );
				WriteLog( strMsg );
			end
		end
		writeCompoundLog( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo[nSelDesItemIdx], arydDesItemVal[nSelDesItemIdx], dDesProb );
		removeItems( arynNecessaryItemIdx );
		removeItems( arynAlternativeItemIdx );
		local nItemIdx = addItemByInfo( aryDesItemInfo[nSelDesItemIdx] );
		if( nItemIdx > 0 ) then
			SetMaxDurability( nItemIdx, nMaxDur );
			SetCurDurability( nItemIdx, nCurDur );
			return nItemIdx, RESULT_SUCCEED;
		end
	else
		writeCompoundLog( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, nil, dDesItemVal, dDesProb );
		for i = 1, getn( arynNecessaryItemIdx ) do
			if( arynNecessaryItemIdx[i] == g_nEquipIdx ) then
				arynNecessaryItemIdx[i] = nil;
				break;
			end
		end
		removeItems( arynNecessaryItemIdx );
		removeItems( arynAlternativeItemIdx );
		return -1, RESULT_FAIL;
	end
end

-- ¼ÆËã±ØÐë²ÄÁÏÖÐµÄ[ºÏ³É]¹Ø¼üÎïÆ·µÄ¼ÛÖµ×ÜÁ¿£¬ÏÞÖÆ¿ÉÑ¡²ÄÁÏ¼ÛÖµÁ¿¼ÓÈ¨
function sumMainItemVal( arynNecessaryItemIdx )
	local nMainItemValSum = 0;
	for i = 1, getn( arynNecessaryItemIdx ) do
		local nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck = GetItemProp( arynNecessaryItemIdx[i] );
		if( nGenre == 6 and nDetailType == 1 and ( nParticular == 146 or ( nParticular >= 199 and nParticular <= 204 ) ) ) then
			nMainItemValSum = nMainItemValSum + ITEM_CalcItemValue( arynNecessaryItemIdx[i], getCompoundParam() );
		end
	end
	return nMainItemValSum;
end