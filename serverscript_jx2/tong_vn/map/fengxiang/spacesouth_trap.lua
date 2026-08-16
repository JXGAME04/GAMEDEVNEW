--Include( "\\script\\tong\\map\\spacesouth_trap.lua" )
function main(sel)
	if ( GetTaskTemp(2777) ~= 1 ) then	-- 15/08: co RIENG thay GetFightState (o JX1 do la co RUT VU KHI)	-- 玩家处于非战斗状态，即在门内
		SetPos(1574, 3242);		-- 设置走出Trap点，目的点在门外
		SetTaskTemp(2777, 1);	-- 15/08: danh dau DANG TRONG khu chien dau		-- 转换为战斗状态
		SetTempRevPos(SubWorldIdx2ID(SubWorld),1660 * 32,3257 * 32)
	else
		SetPos(1581, 3233);		-- 设置走出Trap点，目的点在门内	
		SetTaskTemp(2777, 0);	-- 15/08: da ra khoi khu		-- 转换为非战斗状态
		SetRevPos(GetPlayerRev())
		SetCurCamp(GetCamp())
	end;
end;