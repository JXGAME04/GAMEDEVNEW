Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs = GetTask(3481)

	if UTask_hs == 60*256+30 or UTask_hs == 60*256+40 then
    		-- ±³°ü¿Õ¼ä²»×ã
    		if CalcFreeItemCellCount() < 1 then
			Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn Ýt nhÊt 1 « trèng, nÕu kh«ng sÏ kh«ng nhËn ®­îc vËt phÈm nhiÖm vô. ")
        		return 0
    		end

		if CalcItemCount(3, 6, 1, 4959, -1) < 1 then
			local nItemIdx = AddItem(6,1,4959,1,0,0)
			SetItemBindState(nItemIdx,-2)
			Msg2Player("NhËn ®­îc B¹ch Hæ HuyÕt.")
			AddNote("NhËn ®­îc B¹ch Hæ HuyÕt.")

			SetTask(3481,60*256+40)
		end
	end
end
