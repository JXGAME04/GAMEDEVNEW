Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs_90 = GetTask(3486)

	if UTask_hs_90 >= 80 and UTask_hs_90 < 84 then
		SetTask(3486, UTask_hs_90+1)
	elseif UTask_hs_90 == 84 then
		SetTask(3486, 90)
   		Msg2Player("§¹i hiÖp ®· tiªu diÖt toµn bé S¸t Thñ Thiªn NhÉn Gi¸o. ")
   		AddNote("§¹i hiÖp ®· tiªu diÖt toµn bé S¸t Thñ Thiªn NhÉn Gi¸o. ")
	end
end
