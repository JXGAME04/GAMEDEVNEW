
function main(sel)
if SubWorldIdx2ID(SubWorld) ~= 996 then
if ( GetFightState() == 0 ) then	-- 玩家处于非战斗状态，即在城内
	SetPos(1578,2987)		-- 设置走出Trap点，目的点在城外	
	SetFightState(1)		-- 转换为战斗状态
	SetProtectTime(18*3) --ba gi﹜ b秓 v?th阨 gian 
	AddSkillState(963, 1, 0, 18*3) 	
	else			       		-- 玩家处于战斗状态，即在城外
	SetPos(1584,2994)		-- 设置走出Trap点，目的点在城内	
	SetFightState(1)		-- 转换为非战斗状态
end;
end;
end;