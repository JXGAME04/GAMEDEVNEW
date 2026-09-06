--Author: Fong KiÒu
--Date: 01/07/2021
--Function: Tói d­îc phÈm
FREECELL_MAUTANTHU= 20
Include("\\script\\header\\revivepos_head.lua")
Include("\\script\\header\\forbidmap.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")

function main(nItemIdx)

	 dofile("script/item/tuiduocpham.lua")
	
	local limitm = 20--CalcFreeItemCellCount()
	local sophut = 0--60*24
	local countb = 0
	local levelnguhoa = 3
	if(admincheck() == 1) then
		levelnguhoa = 5
	end
	local limitday = 24*300
	
	local nSubWorldID = GetWorldPos()
	if (GetTaskTemp(99) == 1 ) or ( nSubWorldID >= 387 and nSubWorldID <= 399)then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng kh«ng thÓ më!")
		return
	end
	
	if (nSubWorldID >= 375 and nSubWorldID <= 386) then
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng kh«ng thÓ më!!")
		return
	end
	
	--if (nSubWorldID >= 416 and nSubWorldID <= 511) then
	--	Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng kh«ng thÓ më!!!")
		--return
	--end
	
	if (nSubWorldID == 44 or nSubWorldID == 197 or nSubWorldID == 208 or nSubWorldID == 209 or nSubWorldID == 210 or nSubWorldID == 211 or nSubWorldID == 212 or (nSubWorldID >= 213 and nSubWorldID <= 223)	or nSubWorldID == 336 or nSubWorldID == 341 or nSubWorldID == 342	or nSubWorldID == 175	or nSubWorldID == 337	or nSubWorldID == 338	or nSubWorldID == 339 or ( nSubWorldID >= 387 and  nSubWorldID <= 395 ) )then 
		Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng kh«ng thÓ më!!!!")
		return
	end

	--if (CheckAllMaps(nSubWorldID) == 1) then
	--Talk(1,"","B¶n ®å hiÖn t¹i ng­¬i ®ang ®øng kh«ng thÓ më!!!!!")
		--return
	--end;
	
	if (GetLevel() < 10) then
		Talk(1,"","Ng­êi ch¬i ph¶i ®¹t ®¼ng cÊp 10 trë lªn míi cã thÓ sö dông.")
		return
	end
	
	--Msg2Player("H«m nay ®· sö dông lÇn thø: "..GetTask(T_MOTUIDUOCPHAM) + 1)
	if(GetTask(T_MOTUIDUOCPHAM) >= limitday) then
		Talk(1,"","Mçi ngµy <sex> chØ ®­îc më "..limitday.." lÇn!")
		return		
	end
	
	if limitm <= 0 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ thu xÕp hµnh trang tr­íc khi më!")
		return
	end		
	local nLfTime = GetParamItem(nItemIdx) - GetCurServerSec();
	if(nLfTime > 0) then
		Msg2Player(format("Sau <color=Red>%d gi©y<color> míi cã thÓ nhËn tiÕp",nLfTime));
	return end;
	
	for i=1, limitm do
		if CheckFreeBoxItem(1,1,2,1) == 0 then
			--Talk(1, "", 12266)
		return end
		itemidx = AddItem(1, 2, 0, levelnguhoa, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 60*sophut, 0, 0, 0)
		SetPlayerItemLock(itemidx, -2)
		
		end
	SetParamItem(nItemIdx,GetCurServerSec()+10);
	
	SetTask(T_MOTUIDUOCPHAM, GetTask(T_MOTUIDUOCPHAM) + 1)
	--Msg2Player("H«m nay ®· më "..GetTask(T_MOTUIDUOCPHAM).." lÇn cßn l¹i "..limitday - GetTask(T_MOTUIDUOCPHAM).." lÇn")
	
	if(countb > 0) then
		--Msg2Player("NhËn ®­îc "..countb.." b×nh d­îc phÈm.")
	end
	
	CheckPlayerTitle()
end
