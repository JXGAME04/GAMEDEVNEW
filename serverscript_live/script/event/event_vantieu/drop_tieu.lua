--Author: Phong KiÒu
--Date 08/06/2021
--Function: R¬i item khi mÊt tiªu

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\event_vantieu\\lib_vt.lua")



function DropRate(nNpcIndex, nPlayerIndex)

	local nPlayerNameOw = GetNpcOwner(nNpcIndex) --nhËn l¹i tiªu kú vËt phÈm khi bÞ c­íp tiªu hoµn l¹i nöa tiÒn c«ng
	local nPlayerIndexOw = FindPlayer(nPlayerNameOw)
	if(nPlayerIndexOw > 0) then
		PlayerIndex = nPlayerIndexOw
		local itemIdx = AddItem(ITEM_TIEUKY[1],ITEM_TIEUKY[2],ITEM_TIEUKY[3],ITEM_TIEUKY[4],ITEM_TIEUKY[5],ITEM_TIEUKY[6],ITEM_TIEUKY[7])
		Msg2Player("ThËt kh«ng may bÞ c­íp mÊt tiªu mang Tiªu Kú nµy vÒ nhËn tiÒn lao vô. ")
		--SetPlayerItemLock(itemIdx, -2) --kho¸ vÜnh viÔn
	end
	
	PlayerIndex = nPlayerIndex --player c­íp tiªu
	local nTask = GetTask(T_CUOPTIEU)
	if (nTask > MAX_CUOP_TIEU) then
		return
	end
	local nKind = GetNpcValue(nNpcIndex,1)		
	if(nKind == 1) then
		AddOwnExp(500000 * EXP_RATE)
		DropItem(nNpcIndex,ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],0,0,0)--r¬i hé tiªu lÖnh
	elseif(nKind == 2) then
		AddOwnExp(1000000 * EXP_RATE)
		DropItem(nNpcIndex,ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],0,0,0)--r¬i hé tiªu lÖnh
		DropItem(nNpcIndex,ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],0,0,0)--r¬i hé tiªu lÖnh
	elseif(nKind == 3) then
		AddOwnExp(2000000 * EXP_RATE)
		DropItem(nNpcIndex,ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],0,0,0)--r¬i hé tiªu lÖnh
		DropItem(nNpcIndex,ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],0,0,0)--r¬i hé tiªu lÖnh
		DropItem(nNpcIndex,ITEM_HOTIEULENH[1],ITEM_HOTIEULENH[2],ITEM_HOTIEULENH[3],0,0,0)--r¬i hé tiªu lÖnh
	end
	
	local szTieuName = GetNpcName(nNpcIndex)
	local msgLog = format("§ang trªn ®­êng ®i <color=green> %s <color> thËt kh«ng may bÞ c­íp mÊt. ", szTieuName)
	Msg2SubWorld(msgLog)
end