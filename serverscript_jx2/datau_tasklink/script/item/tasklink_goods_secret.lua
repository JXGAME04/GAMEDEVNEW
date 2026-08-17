
-- ¹ÖÎïµØÍ¼ÒÔ¼°É½ºÓÉçð¢Í¼²ÐÆ¬µôÂä½Å±¾
-- Edited by peres
-- 2004/12/25 Ê¥µ®½ÚÍíÉÏ

IncludeLib("BATTLE");
Include("\\script\\task\\newtask\\newtask_head.lua"); 
Include("\\script\\task\\newtask\\map_index.lua"); -- ÓÃÓÚ»ñÈ¡µØÍ¼µÄÐÅÏ¢
Include("\\script\\task\\newtask\\lib_setmembertask.lua"); -- ÓÃÓÚÑ­»·¸Ä±ä¶ÓÓÑµÄÈÎÎñ±äÁ¿

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
				
			if (GetByte(nt_getTask(1032),1) == 2) then
			
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

-- [PB 17/08/2026] Vong chia to doi viet lai theo binding JX1:
-- GetTeamSize dem CA doi truong (memnum+1) nhung GetTeamMember(i) chi doc
-- m_nMember[] (khong chua doi truong) va slot cuoi la -1 -> vong cu (i=1..size)
-- lam doi truong mat phan + loi nil o slot trong. Quy uoc moi (DLL 17/08):
-- GetTeamMember(0) = doi truong. Cong cho nguoi nhat truoc, roi cac thanh vien
-- khac (bo slot <=0 va bo trung nguoi nhat).
function TLG_ChiaToDoi()
	local nMe = PlayerIndex
	AddMapValues()
	local nMemCount = GetTeamSize()
	if (nMemCount and nMemCount > 1) then
		local i
		for i = 0, nMemCount do
			local nMem = GetTeamMember(i)
			if (nMem and nMem > 0 and nMem ~= nMe) then
				PlayerIndex = nMem
				AddMapValues()
			end
		end
		PlayerIndex = nMe
	end
end

function PickUp( nItemIndex, nPlayerIndex )
	TLG_ChiaToDoi()
	return 0
end

-- click-phai cuon trong tui: TRU CUON TRUOC roi moi chia (chong farm neu
-- co loi giua chung - PB 17/08 phat hien exploit khi remove nam sau vong chia)
function main(nItemIndex)
	RemoveItemByIndex(nItemIndex)
	TLG_ChiaToDoi()
	return 0
end
