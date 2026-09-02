Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
function CheckCanCaiJi(UTask_step)
	if UTask_step == 4 or UTask_step == 5 or UTask_step == 6 or UTask_step == 7 then
		return 1
	else
		return 0
	end
end
-- npc death
function OnDeath(nNpcIndex)
	local UTask_jmdz = GetTask(3487)

	if UTask_jmdz >= 10*256 and UTask_jmdz < 30*256 then
		if CheckCanCaiJi(mod(UTask_jmdz, 256)) ~= 1 then
			SetTask(3487, UTask_jmdz+4)
		end
	end
end
