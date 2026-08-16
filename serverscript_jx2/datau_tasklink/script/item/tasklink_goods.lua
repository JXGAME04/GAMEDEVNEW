
-- ¹ÖÎïµØÍ¼ÒÔ¼°É½ºÓÉçð¢Í¼²ÐÆ¬µôÂä½Å±¾
-- Edited by peres
-- 2004/12/25 Ê¥µ®½ÚÍíÉÏ

IncludeLib("BATTLE");
Include("\\script\\task\\newtask\\newtask_head.lua"); 
Include("\\script\\task\\newtask\\map_index.lua"); -- ÓÃÓÚ»ñÈ¡µØÍ¼µÄÐÅÏ¢
Include("\\script\\task\\newtask\\lib_setmembertask.lua"); -- ÓÃÓÚÑ­»·¸Ä±ä¶ÓÓÑµÄÈÎÎñ±äÁ¿

function PickUp( nItemIndex, nPlayerIndex )

local nPreservedPlayerIndex = PlayerIndex
local nMemCount = GetTeamSize()

	if (nMemCount == 0) then
	
		AddMapValues();
	
	else
	
		for i = 1, nMemCount do -- ÔÚÕâÀï¿ªÊ¼Ñ­»·±éÀúÃ¿¸öÍæ¼Ò
		
			PlayerIndex = GetTeamMember(i);
		
			AddMapValues();

		end
	
		PlayerIndex = nPreservedPlayerIndex; -- Ñ­»·½áÊøºóÔÚÕâÀï¹é»¹Ö÷Íæ¼Ò ID
	
	end
	
	return 0

end


-- ¸ù¾Ý¸÷ÖÖÌõ¼þ¸øÓèÍæ¼Ò²»Í¬ÀàÐÍµÄµØÍ¼Ö¾
function AddMapValues()

local myMapID, myMapName, myMapX, myMapY -- ÓÃÓÚ»ñÈ¡µØÍ¼Ö¾ÐÅÏ¢µÄ±äÁ¿
local myTaskType = nt_getTask(1021)
local nWorldMaps = nt_getTask(1027) -- ¿´¿´Íæ¼ÒÉíÉÏÓÐ¶àÉÙ¸öÉ½ºÓÉçð¢Í¼
local myMapNum = nt_getTask(1025) -- ÅÐ¶ÏÍæ¼ÒÉíÉÏÓÐ¶àÉÙÕÅµØÍ¼Ö¾

myMapID = SubWorldIdx2ID( SubWorld )

	if (myTaskType == 4) then
		
		myMapName, myMapX, myMapY = tl_getMapInfo(myMapID)
		
		if (myMapName == 0) or (myMapName == nil) then -- ·ÀÖ¹¿Õ×Ö·û´¦Àí
			myMapName = ""
		end
		
		-- ¸øÍæ¼ÒÔö¼ÓÒ»¾íµ±Ç°µØÍ¼µÄµØÍ¼Ö¾
		if (nt_getTask(1031) == myMapID) then
		
			if (GetByte(nt_getTask(1032),1) == 1) then
				
				myMapNum = myMapNum + 1
				nt_setTask(1025,myMapNum)
				Msg2Player("B¹n nhËn ®­îc mét tÊm"..myMapName.."§Þa §å chÝ! HiÖn t¹i b¹n cã tæng céng"..myMapNum.." tÊm.");
				
				return 0
			end
		end
		
		-- ¸øÍæ¼ÒÔö¼ÓÒ»¸öÉ½ºÓÉçð¢Í¼²ÐÆ¬
		nWorldMaps = nWorldMaps + 1
		nt_setTask(1027,nWorldMaps)
		Msg2Player("B¹n nhËn ®­îc mét m¶nh b¶n ®å S¬n Hµ X· T¾c! HiÖn t¹i b¹n cã tæng céngt"..nWorldMaps.." m¶nh b¶n ®å S¬n Hµ X· T¾c.");
		
	else
		-- ¸øÍæ¼ÒÔö¼ÓÒ»¸öÉ½ºÓÉçð¢Í¼²ÐÆ¬
		nWorldMaps = nWorldMaps + 1
		nt_setTask(1027,nWorldMaps)
		Msg2Player("B¹n nhËn ®­îc mét m¶nh b¶n ®å S¬n Hµ X· T¾c! HiÖn t¹i b¹n cã tæng céngt"..nWorldMaps.." m¶nh b¶n ®å S¬n Hµ X· T¾c.");
	end

end

-- [JX1 PORT 16/08/2026] Duong CLICK-PHAI kieu JX1 (genre 6 -> EatMecidine goi
-- main): danh cho cuon da nam trong tui (nhat truoc khi hook C++ chay, hoac
-- duong roi khac lot luoi). Cong don giong het nhat tren dat roi tu huy cuon.
function main(nItemIndex)
	local nPreservedPlayerIndex = PlayerIndex
	local nMemCount = GetTeamSize()
	if (nMemCount == 0) then
		AddMapValues()
	else
		local i
		for i = 1, nMemCount do
			PlayerIndex = GetTeamMember(i)
			AddMapValues()
		end
		PlayerIndex = nPreservedPlayerIndex
	end
	RemoveItemByIndex(nItemIndex)
	return 0
end
