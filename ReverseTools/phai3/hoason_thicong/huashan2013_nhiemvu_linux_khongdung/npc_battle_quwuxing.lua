Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs = GetTask(3481)

	if CalcItemCount(-1, 6, 1, 4958, -1) >= 1  then
		ConsumeItem(-1, 1, 6, 1, 4958, -1)
	end

	if UTask_hs == 60*256+80 then
		SetTask(3481, 60*256+90)
		Msg2Player("§¹i hiÖp tiªu diÖt Khóc V« H×nh.")
		AddNote("§¹i hiÖp tiªu diÖt Khóc V« H×nh.")
	end
end
