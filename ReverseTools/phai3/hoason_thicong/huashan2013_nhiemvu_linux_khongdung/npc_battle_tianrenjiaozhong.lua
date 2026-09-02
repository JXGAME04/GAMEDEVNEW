Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
-- npc death
function OnDeath(nNpcIndex)
	local UTask_hs_150 = GetTask(2885)
	local UTask_hs_150_step = GetTask(3488)

	if UTask_hs_150 == 6 and UTask_hs_150_step < 4 then
		SetTask(3488, UTask_hs_150_step+1)
	elseif UTask_hs_150 == 6 and UTask_hs_150_step == 4 then
		SetTask(2885, 7)
		SetTask(3488, 0)
		Msg2Player("§¹i hiÖp ®· trèn tho¸t khái sù tÊn c«ng cña Thiªn NhÉn Gi¸o, h·y quay l¹i xem vÕt th­¬ng cña ThÈm MÆc thÕ nµo råi.")
	end
end
