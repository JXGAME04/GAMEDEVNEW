--Author: Phong KiÒu
--Date 08/06/2021
--Function: khi chÕt boss vËn tiªu

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\event_vantieu\\lib_vt.lua")

function OnDeath(nNpcIndex, nLastDamageIndex)
	local nPlayerIndex = NpcIdx2PIdx(nLastDamageIndex)
	if(nPlayerIndex == 0) then --neu la quai vat thi ngung
		return
	end
	PlayerIndex = nPlayerIndex
	local szNpcName = GetNpcName(nNpcIndex)
	local nBossVTDeath = GetTask(TSK_DANH_BOSS_VT)
	if(nBossVTDeath == 0 and szNpcName == BOSS_TTHANHSON[1][3]) then
		SetTask(TSK_DANH_BOSS_VT,  nBossVTDeath + 1)
		Msg2Player("§· tiªu diÖt ®­îc boss "..szNpcName.." trªn ®­êng vËn tiªu")		
	elseif(nBossVTDeath == 1 and szNpcName == BOSS_TTHANHSON[2][3]) then
		SetTask(TSK_DANH_BOSS_VT,  nBossVTDeath + 1)
		Msg2Player("§· tiªu diÖt ®­îc boss "..szNpcName.." trªn ®­êng vËn tiªu")		
	elseif(nBossVTDeath == 2 and szNpcName == BOSS_TTHANHSON[3][3]) then
		SetTask(TSK_DANH_BOSS_VT,  nBossVTDeath + 1)
		Msg2Player("§· tiªu diÖt ®­îc boss "..szNpcName.." trªn ®­êng vËn tiªu")		
	end
end

function OnRevive(nNpcIndex)
	--SetNpcBoss2(nNpcIndex,2)
end
