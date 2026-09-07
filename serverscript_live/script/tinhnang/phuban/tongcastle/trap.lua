Include("\\script\\missions\\tongcastle\\tongcastle.lua")

function main(nTrapId)
	-- [TONGCASTLE 23/08] tbTrap song trong state chu (engine ta 1 state/tep)
	DynamicExecuteByPlayer(PlayerIndex, "\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:JX1_Trap", nTrapId)
end
