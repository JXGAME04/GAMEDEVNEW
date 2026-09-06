----------------Boss dai hoang kim-----------------------------------------------------------
--Author: Fong Kieu
--Date 07/07/2021

TSK_PLAYER_BOSSKILLED = 2598

function OnDeath(nNpcIndex, nDamageIndex)
	
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex);
	PlayerIndex = nPlayerIndex;

	DropItem(NpcIndex, 4, 353, 0, 0, 0, 0) --tinh hong bao thach
	DropItem(NpcIndex, 4, 353, 0, 0, 0, 0) --tinh hong bao thach

	AddOwnExp(6000000);

	local nCount = GetTask(TSK_PLAYER_BOSSKILLED);
	nCount = nCount + 1;
	SetTask(TSK_PLAYER_BOSSKILLED, nCount);
	Msg2Player("§¹i hiÖp ®· tiªu diÖt: "..nCount.." boss ®¹i Hoµng Kim")

end;

function OnTimer(nNpcIndex)
	DelNpc(nNpcIndex);
end

function OnRevive(nNpcIndex)
	
end