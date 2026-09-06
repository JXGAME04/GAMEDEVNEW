-- Author:	Kinnox;
-- Date:	25-08-2021
-- Functions: Script Ondeath _ Droprate  Normal / Blue Boss / Gold Boss;
Include("\\script\\lib\\lib_server.lua")

function OnDeath(nNpcIndex,nDamageIndex)
local nPlayerIndex = NpcIdx2PIdx(nDamageIndex);	
PlayerIndex = nPlayerIndex;
end;

function OnRevive(nNpcIndex)
	
end;

