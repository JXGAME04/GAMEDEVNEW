function OnDeath(index)
	Msg2Team(GetName().." Không may tại trận tử vong.")
	SetDeathScript("")
	SetLogoutRV(0)
	NewWorld(11,3207,4978)
end