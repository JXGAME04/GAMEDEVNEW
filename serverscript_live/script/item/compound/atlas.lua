-- ¹¦ÄÜ£º×ÏÉ«×°±¸ÏµÍ³ - Ê¹ÓÃ»Æ½ðÍ¼Æ×ºÍÔ­ÁÏºÏ³É»Æ½ð×°±¸
-- Fanghao Wu 2005.1.15

IncludeLib( "FILESYS" );
Include( "\\script\\item\\item_header.lua" );
Include( "\\script\\item\\compound\\compound_header.lua" );

FILE_ATLAS_COMPOUND = "atlas_compound.txt";
loadItemTabFiles( FILE_ATLAS_COMPOUND );

-- [ºÏ³É]·ÑÓÃ
COMPOUND_COST = 100000;


function Compound( arynNecessaryItemIdx, arynAlternativeItemIdx, nExParam )
	if( isCompoundableToday() ~= 1 ) then
		Say( "Thî rÌn thÇn bÝ: H«m nay l·o phu ®· qu¸ mÖt mái, ngµy mai hiÖp sÜ h·y ®Õn ®óc trang bÞ!", 0 );
		return -1, RESULT_FAIL;
	end
	-- ³õÊ¼»¯Êý¾Ý
	if( initData ~= nil ) then
		initData();
	end
	-- Ð£ÑéÔ­ÁÏÊÇ·ñ·ûºÏ[ºÏ³É]¹æÔò£¨Í¬Ê±´æ´¢Ò»Ð©Ô­ÁÏÊý¾Ý£©
	local nResult = verifySrcItems( arynNecessaryItemIdx, arynAlternativeItemIdx );
	if( nResult ~= RESULT_SUCCEED ) then
		return -1, nResult;
	end
	-- Ö§¸¶[ºÏ³É]·ÑÓÃ
	if( COMPOUND_COST == nil or Pay( COMPOUND_COST ) ~= 1 ) then
		return -1, RESULT_NO_MONEY;
	end
	-- ¼ÆËãËùÓÐÔ­ÁÏ¼ÛÖµÁ¿×ÜºÍ
	local nSrcItemValSum = sumItemsVal( arynNecessaryItemIdx ) + sumItemsVal( arynAlternativeItemIdx );	
--	Msg2Player( "<color=yellow>-----------------------------------" );
	-- Éú³ÉÄ¿±êÎïÆ·ÐÅÏ¢
	local aryDesItemInfo = genDesItemsInfo( arynNecessaryItemIdx );
	-- ¼ÆËãÃ¿¸öÄ¿±êÎïÆ·µÄ¼ÛÖµÁ¿
	local arydDesItemVal = {};
	local nDesItemCount = getn( aryDesItemInfo );
	for i = 1, nDesItemCount do
		arydDesItemVal[i] = ITEM_CalcItemValue( aryDesItemInfo[i][1], aryDesItemInfo[i][3], aryDesItemInfo[i][4], aryDesItemInfo[i][5], aryDesItemInfo[i][6], aryDesItemInfo[i][7], aryDesItemInfo[i][8], aryDesItemInfo[i][9], aryDesItemInfo[i][10], aryDesItemInfo[i][11] );
--	Msg2Player( "<color=green>DesValue"..i..": "..arydDesItemVal[i] );
	end
--	Msg2Player( "<color=yellow>-----------------------------------" );
	
	if( nExParam == 1 ) then	-- Ô¤ÀÀ[ºÏ³É]µÄ³É¹¦ÂÊ
		previewProb( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo, arydDesItemVal );
		return -1, RESULT_SUCCEED;
	else						-- ½øÐÐ[ºÏ³É]²Ù×÷
		-- Íê³ÉÑ¡ÔñÄ¿±êÎïÆ·¡¢É¾³ýÔ­ÁÏµÈÊÕÎ²²Ù×÷
		nResultDesItemIdx, nResult = finalCompound( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo, arydDesItemVal );
		-- ·µ»ØÉú³ÉÎïÆ·Ë÷Òý£¬[ºÏ³É]½á¹û
		return nResultDesItemIdx, nResult;
	end
end

-- ³õÊ¼»¯Êý¾Ý
function initData()
	g_nAtlasVer = -1;
	g_nAtlasID = -1;
end

-- »ñÈ¡[ºÏ³É]²Ù×÷²ÎÊý£¨ÓÃÓÚ´«µÝ¸øITEM_CalcItemValue¼ÆËãÎïÆ·¼ÛÖµÁ¿£©
function getCompoundParam()
	return "ATLAS";
end

-- Ð£ÑéÔ­ÁÏÊÇ·ñ·ûºÏ[ºÏ³É]¹æÔò£¨Í¬Ê±´æ´¢Ò»Ð©Ô­ÁÏÊý¾Ý£©
function verifySrcItems( arynNecessaryItemIdx, arynAlternativeItemIdx )
	local nNecessaryItemCount = getn( arynNecessaryItemIdx );
	local aryMaterialsInfo = {};
	local nMaterialCount = 0;
	local bXuanjing = 0;
	for i = 1, nNecessaryItemCount do
		local nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck = GetItemProp( arynNecessaryItemIdx[i] );
		local bIsAtlas = 0;
		if( g_nAtlasID <= 0 ) then
			local nItemVer = ITEM_GetItemVersion( arynNecessaryItemIdx[i] );
			g_nAtlasID = isAtlas( nItemVer, nGenre, nDetailType, nParticular );
			if( g_nAtlasID > 0 ) then
				g_nAtlasVer = nItemVer;
				bIsAtlas = 1;
			end
		end
		if( bIsAtlas ~= 1 ) then
			if( nGenre == 6 and nDetailType == 1 and nParticular == 146 ) then
				if( bXuanjing == 1 ) then
					return RESULT_RULE_ERROR;
				end
				bXuanjing = 1;
			else
				nMaterialCount = nMaterialCount + 1;
				aryMaterialsInfo[nMaterialCount] = {};
				aryMaterialsInfo[nMaterialCount].nGenre, aryMaterialsInfo[nMaterialCount].nDetailType, aryMaterialsInfo[nMaterialCount].nParticular, aryMaterialsInfo[nMaterialCount].nLevel, aryMaterialsInfo[nMaterialCount].nSeries, aryMaterialsInfo[nMaterialCount].nLuck = nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck;
				aryMaterialsInfo[nMaterialCount].nMagicID = GetItemParam( arynNecessaryItemIdx[i], 1 );
			end
		end
	end
	if( g_nAtlasID <= 0 or bXuanjing == 0 ) then
		return RESULT_LACK_RESOURCE;
	end
	if( verifyAtlasMaterials( g_nAtlasVer, g_nAtlasID, aryMaterialsInfo ) ~= 1 ) then
		return RESULT_LACK_RESOURCE;
	end
	return RESULT_SUCCEED;
end

-- ÅÐ¶ÏÊÇ·ñÊÇ»Æ½ðÍ¼Æ×
function isAtlas( nItemVer, nGenre, nDetailType, nParticular )
	local nAtlasID = 0;
	local strAtlasPath = makeItemFilePath( nItemVer, FILE_ATLAS_COMPOUND );
	local nRowCount = TabFile_GetRowCount( strAtlasPath );
	for i = 2, nRowCount do
		local nCurGenre = tonumber( TabFile_GetCell( strAtlasPath, i, "ATLAS_GENRE", -1 ) );
		local nCurDetailType = tonumber( TabFile_GetCell( strAtlasPath, i, "ATLAS_DETAILTYPE", -1 ) );
		local nCurParticular = tonumber( TabFile_GetCell( strAtlasPath, i, "ATLAS_PARTICULAR", -1 ) );
		if(	nParticular == nCurParticular and
			nDetailType == nCurDetailType and
			nGenre == nCurGenre ) then
			nAtlasID = i - 1;
			break;
		end
	end
	return nAtlasID;
end

-- ÅÐ¶ÏÔ­ÁÏÊÇ·ñ·ûºÏ»Æ½ðÍ¼Æ×µÄÒªÇó
function verifyAtlasMaterials( nAtlasVer, nAtlasID, aryMaterialsInfo )
	local strAtlasPath = makeItemFilePath( nAtlasVer, FILE_ATLAS_COMPOUND );
	local nRowCount = TabFile_GetRowCount( strAtlasPath );
	local nMaterialCount = getn( aryMaterialsInfo );
	if( nRowCount < 2 or nAtlasID >= nRowCount ) then
		return 0;
	end
	local nReqItemCount = 0;
	local nGenre, nDetailType, nParticular, nLevel, nSeries, nMagicID;
	for i = 1, 6 do
		local bExist = 0;
		nGenre = tonumber( TabFile_GetCell( strAtlasPath, nAtlasID + 1, i.."_GENRE", -1 ) );
		if( nGenre >= 0 ) then
			nReqItemCount = nReqItemCount + 1;
			nDetailType = tonumber( TabFile_GetCell( strAtlasPath, nAtlasID + 1, i.."_DETAILTYPE", -1 ) );
			nParticular = tonumber( TabFile_GetCell( strAtlasPath, nAtlasID + 1, i.."_PARTICULAR", -1 ) );
			nLevel = tonumber( TabFile_GetCell( strAtlasPath, nAtlasID + 1, i.."_LEVEL", -1 ) );
			nSeries = tonumber( TabFile_GetCell( strAtlasPath, nAtlasID + 1, i.."_SERIES" , -1 ) );
			nMagicID = tonumber( TabFile_GetCell( strAtlasPath, nAtlasID + 1, i.."_MAGIC_ID", -1 ) );
			for j = 1, nMaterialCount do
				if(	aryMaterialsInfo[j].nGenre == nGenre and
					( nDetailType < 0 or aryMaterialsInfo[j].nDetailType == nDetailType ) and
					( nParticular < 0 or aryMaterialsInfo[j].nParticular == nParticular ) and
					( nLevel < 0 or aryMaterialsInfo[j].nLevel >= nLevel ) and
					( nSeries < 0 or aryMaterialsInfo[j].nSeries == nSeries ) and
					( nMagicID < 0 or aryMaterialsInfo[j].nMagicID == nMagicID ) ) then
					bExist = 1;
					break;
				end
			end
			if( bExist ~= 1 ) then
				return 0;
			end
		end
	end
	if( nReqItemCount ~= nMaterialCount ) then
		return 0;
	end
	return 1;
end

-- Éú³ÉÄ¿±êÎïÆ·ÐÅÏ¢
function genDesItemsInfo( arynNecessaryItemIdx )
	local aryDesItemInfo = {};
	local strAtlasPath = makeItemFilePath( g_nAtlasVer, FILE_ATLAS_COMPOUND );
	nQuality = tonumber( TabFile_GetCell( strAtlasPath, g_nAtlasID + 1, "DES_QUALITY", -1 ) );
	nGenre = tonumber( TabFile_GetCell( strAtlasPath, g_nAtlasID + 1, "DES_GENRE", -1 ) );
	nDetailType = tonumber( TabFile_GetCell( strAtlasPath, g_nAtlasID + 1, "DES_DETAILTYPE", -1 ) ) - 1;
	nParticular = tonumber( TabFile_GetCell( strAtlasPath, g_nAtlasID + 1, "DES_PARTICULAR", -1 ) );
	nLevel = tonumber( TabFile_GetCell( strAtlasPath, g_nAtlasID + 1, "DES_LEVEL", -1 ) );
	nSeries = tonumber( TabFile_GetCell( strAtlasPath, g_nAtlasID + 1, "DES_SERIES", -1 ) );
	aryDesItemInfo[1] = { g_nAtlasVer, 0, nQuality, nGenre, nDetailType, nParticular, nLevel, nSeries, 0, nil, nil, getCompoundParam() };
	return aryDesItemInfo;
end

-- Íê³ÉÑ¡ÔñÄ¿±êÎïÆ·¡¢É¾³ýÔ­ÁÏµÈÊÕÎ²²Ù×÷
function finalCompound( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo, arydDesItemVal )
	return defFinalCompound( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo, arydDesItemVal );
end

-- Ô¤ÀÀ[ºÏ³É]µÄ³É¹¦ÂÊ
function previewProb( arynNecessaryItemIdx, arynAlternativeItemIdx, nSrcItemValSum, aryDesItemInfo, arydDesItemVal )
	local nSelDesItemIdx, dDesItemVal, dDesProb = TransItemVal( nSrcItemValSum, arydDesItemVal );
	if( nSelDesItemIdx <= 0 ) then
		dDesProb = 1 - dDesProb;
	end
	Talk( 1, "", format( "Tû lÖ ghÐp trang bÞ Hoµng Kim lÇn nµy lµ\n<color=yellow>%0.2f%%", dDesProb*100 ) );
end