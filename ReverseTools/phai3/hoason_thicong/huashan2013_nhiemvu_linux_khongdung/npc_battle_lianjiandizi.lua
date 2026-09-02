Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs = GetTask(3481)

	if UTask_hs == 20*256+30 then
		SetTask(3481, 20*256+40)
		Msg2Player("Tπi di‘n v‚ tr≠Íng Æ∏nh bπi Æ÷ tˆ luy÷n ki’m.")
		AddNote("Tπi di‘n v‚ tr≠Íng Æ∏nh bπi Æ÷ tˆ luy÷n ki’m.")
	end
end
