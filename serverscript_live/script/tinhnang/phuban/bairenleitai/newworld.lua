-- [BAIREN 23/08] map 960: thay EventSys EnterMap/LeaveMap (hundred_arena.lua:160-172 dang ky trong
-- state rieng cua no - newworldscript khong thay duoc) - khuon missions\leaguematch\combat\newworld.lua
Include("\\script\\lib\\common.lua")	-- [BAIREN 23/08 phan bien F4] split() cho PraseParam (SPEC P1)
Include("\\script\\maps\\newworldscript_default.lua")
BAIREN_HA = "\\script\\missions\\bairenleitai\\hundred_arena.lua"

function OnNewWorld(szParam)
	DynamicExecuteByPlayer(PlayerIndex, BAIREN_HA, "HundredArena:OnEnterMap")
	OnNewWorldDefault(szParam)
end

function OnLeaveWorld(szParam)
	DynamicExecuteByPlayer(PlayerIndex, BAIREN_HA, "HundredArena:OnLeaveMap")
	OnLeaveWorldDefault(szParam)
end
