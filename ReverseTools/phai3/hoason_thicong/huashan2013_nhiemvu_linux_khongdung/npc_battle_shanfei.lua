Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
Include("\\script\\activitysys\\playerfunlib.lua")

-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs = GetTask(3481)

	if UTask_hs == 50*256+20 or UTask_hs == 50*256+30 then
		if CalcItemCount(3, 6, 1, 4954, -1) < 3 then
    			-- ±³°ü¿Õ¼ä²»×ã
    			if CalcFreeItemCellCount() < 1 then
				Msg2Player("Hµnh trang kh«ng ®ñ chç, h·y s¾p xÕp l¹i cßn Ýt nhÊt 1 « trèng, nÕu kh«ng sÏ kh«ng nhËn ®­îc vËt phÈm nhiÖm vô. ")
        			return 0
    			end

			if UTask_hs == 50*256+20 then
				SetTask(3482, GetTask(3482)+1)

				if GetTask(3482) >= 3 then
					SetTask(3481, 50*256+30)
					SetTask(3482, 0)
				end
			end

			PlayerFunLib:GetItem({tbProp={6,1,4954,1,0,0},nBindState=-2,},1,"")
			AddNote("NhËn ®­îc vËt phÈm tõ th«n d©n.")
		end
	end
end
