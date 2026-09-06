
function main(sel)
local w,x,y = GetWorldPos()

if ( GetFightState() == 0 ) then	-- 玩家处于非战斗状态，即在城内
	SetPos(floor(x/32)+3,floor(y/32)+3)		-- 设置走出Trap点，目的点在城外	
	SetFightState(1)		-- 转换为战斗状态
	else			       		-- 玩家处于战斗状态，即在城外
	SetPos(floor(x/32)-2,floor(y/32)-2)		-- 设置走出Trap点，目的点在城外	
	SetFightState(0)		-- 转换为非战斗状态
end;
end;