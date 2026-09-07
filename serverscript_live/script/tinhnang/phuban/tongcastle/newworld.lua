-- -- [TONGCASTLE 23/08] map 984: thay EventSys EnterMap/LeaveMap (Game:RegAll khong chay duoc tren JX1)
-- khuon missions\leaguematch\combat\newworld.lua
Include("\\script\\maps\\newworldscript_default.lua")
TC_HA = "\\script\\missions\\tongcastle\\tongcastle.lua"

function OnNewWorld(szParam)
	DynamicExecuteByPlayer(PlayerIndex, TC_HA, "TongCastle:JX1_OnEnterMap")
	OnNewWorldDefault(szParam)
end

function OnLeaveWorld(szParam)
	DynamicExecuteByPlayer(PlayerIndex, TC_HA, "TongCastle:JX1_OnLeaveMap")
	OnLeaveWorldDefault(szParam)
end
