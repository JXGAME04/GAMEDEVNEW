Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs = GetTask(3481)

	local nRandom = random(1,1000)
	if UTask_hs == 30*256+20 or UTask_hs == 30*256+30 then
		if CalcItemCount(3, 6, 1, 4951, -1) > 0 then
			return
		end

    		-- ±³°ü¿Õ¼ä²»×ã
    		if CalcFreeItemCellCount() < 1 then
			Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn Ýt nhÊt 1 « trèng, nÕu kh«ng sÏ kh«ng nhËn ®­îc vËt phÈm nhiÖm vô. ")
        		return 0
    		end

		if nRandom <= 500 then
			SetTask(3481, 30*256+30)
			local nItemIdx = AddItem(6,1,4951,1,0,0)
			SetItemBindState(nItemIdx,-2)
			Msg2Player("NhËn ®­îc B¨ng Tµm TuÕ.")
			Msg2Player("§¹i hiÖp ®o¹t l¹i B¨ng Tµm TuÕ. ")
			AddNote("§¹i hiÖp ®o¹t l¹i B¨ng Tµm TuÕ. ")
		end
	end
end
