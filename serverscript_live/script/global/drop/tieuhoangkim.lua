----------------Boss tieu hoang kim-----------------------------------------------------------

TSK_PLAYER_BOSSKILLED = 2598

function OnDeath(nNpcIndex, nDamageIndex)

	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex);
	PlayerIndex = nPlayerIndex;

	DropItem(NpcIndex, 6, 1, 11, 0, 0, 0) --phao hoa
	
	AddOwnExp(1000000);

	local nCount = GetTask(TSK_PLAYER_BOSSKILLED);
	nCount = nCount + 1;
	SetTask(TSK_PLAYER_BOSSKILLED, nCount);
	Msg2Player("§¹i hiÖp ®· tiªu diÖt: "..nCount.." boss")
	
end;

function OnTimer(nNpcIndex)
	DelNpc(nNpcIndex);
end

function OnRevive(nNpcIndex)
	
end