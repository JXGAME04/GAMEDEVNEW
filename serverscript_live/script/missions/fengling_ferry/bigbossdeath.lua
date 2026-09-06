
Include("\\script\\misc\\eventsys\\type\\func.lua")

function OnDeath( nNpcIndex )
	EventSys:GetType("FengLingDu"):OnEvent("OnShuiZeiDeath", nNpcIndex, PlayerIndex)
end
