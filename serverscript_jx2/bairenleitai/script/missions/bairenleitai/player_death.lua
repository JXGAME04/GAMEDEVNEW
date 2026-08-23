-- ====================== 文件信息 ======================

-- 剑侠情缘网络版大陆版 - 百人擂台角色死亡
-- 文件名　：player_death.lua
-- 创建者　：子非魚
-- 创建时间：2011-04-26 16:50:30

-- 被NPC杀死
-- 被Player杀死
-- 	.死者是擂主
-- 	.死者是挑战者
-- ======================================================
Include("\\script\\missions\\bairenleitai\\hundred_arena.lua");

-- [JX1 23/08] engine JX1 goi OnPlayerDeath(nVictimPIdx, nLauncherNpcIdx) (KNpc.cpp) - Linux goi OnDeath(Launcher)
function OnPlayerDeath(nVictimPIdx, Launcher)
	OnDeath(Launcher)
end

function OnDeath(Launcher)
	local nKilledIndex = PlayerIndex;
	-- [JX1 23/08] tbPlayerList/tbArenaList song trong state hundred_arena.lua -> goi sang state do;
	-- nType 3 = bi NPC giet, 0 = bi nguoi giet (JX1_PlayerDeath tu suy 1|2 theo nRight cua nan nhan)
	SetCurCamp(GetCamp());
	
	local nKillerIndex = NpcIdx2PIdx(Launcher);
	local szHA = "\\script\\missions\\bairenleitai\\hundred_arena.lua";
	
	if (nKillerIndex <= 0) then
		print(Launcher, GetNpcName(Launcher));
		DelNpc(Launcher);
		DynamicExecuteByPlayer(nKilledIndex, szHA, "HundredArena:JX1_PlayerDeath", 3);
	else
		DynamicExecuteByPlayer(nKilledIndex, szHA, "HundredArena:JX1_PlayerDeath", 0);
	end
	
	--Modified by DinhHQ - 20110714
	ST_StopDamageCounter();
	SetDeathScript("");
	DisabledUseTownP(0)	-- 可以回城符
	-- SetLogoutRV(0);	-- 只有在主动离开时 才设置为0
	DisabledStall(0)	-- 可以摆摊
	SetFightState(0)	-- 非战斗
	SetCurCamp(GetCamp());
	
	SetRevPos(GetPlayerRev())
	SetPKFlag(0);		
	-- [JX1 23/08] goc :49-53 `self:DelPlayer(PlayerIndex)` + NewWorld: `self` la GLOBAL CHUA GAN ->
	-- tren Linux NEM LOI runtime tai day, NewWorld khong bao gio chay; hanh vi thuc te = hoi sinh
	-- tai diem dang nhap (SetRevPos o tren), roi map 960 -> OnLeaveMap don. Giu DUNG hanh vi do.
end
