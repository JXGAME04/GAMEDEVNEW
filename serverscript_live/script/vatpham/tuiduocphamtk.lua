--Author: Fong KiÒu
--Date: 01/07/2021
--Function: Tói d­îc phÈm

Include("\\script\\header\\revivepos_head.lua")
Include("\\script\\header\\forbidmap.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")

nItemTempIdx = nil

function main(nItemIdx)
	-- dofile("script/item/tuiduocphamtk.lua")
	nItemTempIdx = nItemIdx
	OpenGetNumber("NhËp Sè L­îng", "main_callback")
end

function main_callback()
local nItemIdx = nItemTempIdx
	local nSubWorldID = GetWorldPos();
	local nFreeCell = GetNumberFromUI();
	local nNewIdx;
	local itemname = GetNameItem(nItemIdx)
	local n_param = GetParamItem(nItemIdx)
	local limitm = CalcFreeItemCellCount()
	
			if (nSubWorldID == 379 or nSubWorldID == 324 or nSubWorldID == 399 or nSubWorldID == 220 or ( nSubWorldID >= 387 and nSubWorldID <= 394))then --397
				Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng kh«ng thÓ më!!")
				return
			end
	
			if(nFreeCell <= 0) then
				Msg2Player("Xin s¾p xÕp Ýt nhÊt 1 « trèng míi cã thÓ nhËn");
			return
			elseif nFreeCell > 60 then
				nFreeCell = 60
			end
			
		
			
			
			if (n_param <= 0) then
				RemoveItem(nItemIdx,1)
				Msg2Player(""..itemname.." ®· sö dông hÕt sè lÇn, "..itemname.." tù ®éng xãa!")
				return
			end
			
			local nAddCount = nFreeCell
				if n_param < nFreeCell then
					nAddCount = n_param
				end
				if limitm < nAddCount then
					nAddCount = limitm
				end
			n_param = n_param - nAddCount
			for i = 1,nAddCount do
				nNewIdx = AddItem(1, 2, 0,5 , 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0)
				SetPlayerItemLock(nNewIdx, -2)
			end;	
			

			if n_param <= 0 then
				RemoveItem(nItemIdx, 1)
				Msg2Player(""..itemname.." ®· sö dông hÕt sè lÇn, "..itemname.." tù ®éng xãa!")
			else
					SetParamItem(nItemIdx,n_param)
					Msg2Player(""..itemname.." cßn cã thÓ sö dông "..n_param.." b×nh m¸u  ")
			end


	CheckPlayerTitle()
end
