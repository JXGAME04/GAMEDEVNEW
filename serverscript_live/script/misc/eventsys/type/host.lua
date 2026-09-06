Include("\\script\\misc\\eventsys\\eventsys.lua")

EventSys:NewType("Host")

function OnGameServerExit(nHostID)
	EventSys:GetType("Host"):OnEvent("OnGameServerExit", nHostID)
end


function OnGameServerReady(nHostID)
	EventSys:GetType("Host"):OnEvent("OnGameServerReady", nHostID)
end
