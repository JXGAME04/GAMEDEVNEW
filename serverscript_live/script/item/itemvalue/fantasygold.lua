-- baiyun 2012.03.16

IncludeLib( "FILESYS" );
Include( "\\script\\item\\item_header.lua" );

FANTASYGOLD_VALUE_FILE = "itemvalue\\FantasyGold.txt";

loadItemTabFiles( FANTASYGOLD_VALUE_FILE );

-- Chøc n¨ng: cöa vµo mµ ch­¬ng tr×nh gäi, tÝnh gi¸ trÞ cña mét m« t¶ vËt phÈm.
-- Tham sè: nItemVer      phiªn b¶n vËt phÈm
--          nQuality      phÈm chÊt (0 th­êng, 1 Hoµng Kim, 2 kh¶m n¹m ®­îc)
--          nGenre/nDetailType/nParticular   m· lo¹i vËt phÈm
--          nLevel        cÊp cña vËt phÈm
--          nSeries       ngò hµnh
--          nLuck         chØ sè may m¾n trong tham sè sinh
--          arynMagLvl    m¶ng MagicLevel cña vËt phÈm
--          aryMagic      m¶ng MagicID cña vËt phÈm
--          strParam      tham sè cña thao t¸c lß rÌn
-- Tr¶ vÒ : gi¸ trÞ tÝnh ®­îc (lçi th× tr¶ 0).
function CalcItemValue( nItemVer, nQuality, nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck, arynMagLvl, aryMagic, strParam )
	local nItemVal = 0;
	local strOreValPath = makeItemFilePath( nItemVer, FANTASYGOLD_VALUE_FILE );
	local nSearchRow = TabFile_Search( strOreValPath, "LEVEL", nLevel );
	if( nSearchRow >= 2 ) then
		nItemVal = tonumber( TabFile_GetCell( strOreValPath, nSearchRow, "VALUE", 0 ) );
	end
	return nItemVal;
end


function CalcFantasyGoldValueByLevel(nLevel)
	local nItemVersion = ITEM_GetLatestItemVersion();
	local nItemValue = 0;
	local strValueFilePath = makeItemFilePath(nItemVersion, FANTASYGOLD_VALUE_FILE);
	local nSearchRow = TabFile_Search(strValueFilePath, "LEVEL", nLevel);
	if( nSearchRow >= 2 ) then
		nItemValue = tonumber(TabFile_GetCell(strValueFilePath, nSearchRow, "VALUE", 0));
	end
	return nItemValue;
end

