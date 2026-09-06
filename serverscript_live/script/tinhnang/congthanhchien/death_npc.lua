--Author: Fong KiÒu
--Date: 2021
--Function: Cæng thµnh cò khi chÕt hiÖn t¹i ch­a sö dông

Include("\\script\\lib\\lib_map.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")

function OnDeath(nNpcIndex,nDamageIndex)
	local nPlayerIndex = NpcIdx2PIdx(nLastDamageIndex)
	if (nPlayerIndex == 0) then	-- neu la quai vat thi ngung
		return 
	end
	-- DelNpc(nNpcIndex)
	DelMSNpc(MS_CTHANHCHIEN,nNpcIndex)
end

function OnRevive(nNpcIndex)
	local nType = GetNpcValue(nNpcIndex)
	if (nType < 5) then
		--SetNpcBoss2(nNpcIndex,3)
	end
end