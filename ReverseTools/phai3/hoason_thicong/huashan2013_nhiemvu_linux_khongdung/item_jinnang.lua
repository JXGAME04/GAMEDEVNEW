Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
--Èë¿Úº¯Êý
function main(nItemIndex)
    	local UTask_hs = GetTask(3481)

    	if UTask_hs < 60*256 or UTask_hs >= 60*256+80 then
	    	Msg2Player("VËt phÈm nµy ®· thÊt b¹i.")
	    	return nil
    	end

    	-- ±³°ü¿Õ¼ä²»×ã
    	if CalcFreeItemCellCount() < 1 then
		Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn Ýt nhÊt 1 « trèng råi míi më cÈm nang.")
        	return 1
    	end

	if UTask_hs == 60*256+10 then
		local nItemIdx = AddItem(6,1,4956,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc lêi nh¾n #thø nhÊt.")
		SetTask(3481, UTask_hs+10)
		Msg2Player("§i hç trî ¢n KiÕm Thu tu söa thÇn kiÕm cña bæn ph¸i.")
		AddNote("§i hç trî ¢n KiÕm Thu tu söa thÇn kiÕm cña bæn ph¸i.")
	elseif UTask_hs == 60*256+50 then
		local nItemIdx = AddItem(6,1,4957,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc lêi nh¾n #thø 2")
		SetTask(3481, UTask_hs+10)
		Msg2Player("H·y ®Õn thung lòng ®Ó tÞnh t©m.")
		AddNote("H·y ®Õn thung lòng ®Ó tÞnh t©m.")
	elseif UTask_hs == 60*256+70 then
		local nItemIdx = AddItem(6,1,4958,1,0,0)
		SetItemBindState(nItemIdx,-2)
		Msg2Player("NhËn ®­îc lêi nh¾n #thø 3")
		SetTask(3481, UTask_hs+10)
		Msg2Player("§Õn KiÕm C¸c T©y Nam tiªu diÖt Khóc V« H×nh.")
		AddNote("§Õn KiÕm C¸c T©y Nam tiªu diÖt Khóc V« H×nh.")
	else
		Msg2Player("Thêi c¬ ch­a ®Õn, kh«ng ®­îc më ra xem tr­íc.")
	end

    	return 1
end
