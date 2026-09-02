Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	SetGlbValue(1283, 0)
	local oldPlayerIndex = PlayerIndex
	local nTeamSize = GetTeamSize()

	if nTeamSize == 0 then
		if CalcItemCount(3, 6, 1, 4961, -1) >= 1  then
			return
		end

		local UTask_hs_90 = GetTask(3486)
		if UTask_hs_90 == 20 or UTask_hs_90 == 30 then
			if CalcFreeItemCellCount() < 1 then
				Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô. ")
			else
				local nItemIdx = AddItem(6,1,4961,1,0,0)
				SetItemBindState(nItemIdx,-2)
				Msg2Player("NhËn ®­îc 1 MËt lÖnh")
				SetTask(3486, 30)
				Msg2Player("§¹i hiÖp ®· ®¸nh b¹i Ng­êi ThÇn BÝ, nhËn ®­îc 1 MËt lÖnh. ")
				AddNote("§¹i hiÖp ®· ®¸nh b¹i Ng­êi ThÇn BÝ, nhËn ®­îc 1 MËt lÖnh. ")
			end
		end
	end

	for i = 1, nTeamSize do
		PlayerIndex = GetTeamMember(i)
		if CalcItemCount(3, 6, 1, 4961, -1) >= 1  then
			return
		end

		local UTask_hs_90 = GetTask(3486)
		if UTask_hs_90 == 20 or UTask_hs_90 == 30 then
			if CalcFreeItemCellCount() < 1 then
				Msg2Player("Hµnh trang ®· ®Çy, kh«ng thÓ nhËn vËt phÈm nhiÖm vô. ")
			else
				local nItemIdx = AddItem(6,1,4961,1,0,0)
				SetItemBindState(nItemIdx,-2)
				Msg2Player("NhËn ®­îc 1 MËt lÖnh")
				SetTask(3486, 30)
				Msg2Player("§¹i hiÖp ®· ®¸nh b¹i Ng­êi ThÇn BÝ, nhËn ®­îc 1 MËt lÖnh. ")
				AddNote("§¹i hiÖp ®· ®¸nh b¹i Ng­êi ThÇn BÝ, nhËn ®­îc 1 MËt lÖnh. ")
			end
		end
	end

	PlayerIndex = oldPlayerIndex
end
