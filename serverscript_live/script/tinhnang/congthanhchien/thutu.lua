--Author: Fong KiÒu
--Date: 28/11/2021
--Function: Player phe thñ tö trËn

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")

function OnPlayerDeath(nPlayerIndex,nLastDamageIndex) -- player, npc
	PlayerIndex = nPlayerIndex
	local nTName  = GetName()
	local nPCIdx = NpcIdx2PIdx(nLastDamageIndex)
	if (nPCIdx == 0) then	-- neu la quai vat thi ngung
		return 
	end		
	local nLTramP = 0
	local nPlayerDataCIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,nPCIdx)
	local nPlayerDataTIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,nPlayerIndex)
	local nLRankC = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,11)
	local nCRankT = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,11)
	PlayerIndex = nPCIdx
	local nCName  = GetName()
	PlayerIndex = nPlayerIndex
--	diem cho nguoi choi cong
	local nTLuyTam = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,6)
	local nLTramT  = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,7)
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,7,nLTramT+1)		-- lien tram
	if (GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,7) >= GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,5)) then		-- so sanh ltram va lien tram max
		SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,5,GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,7)) --max LT
	end
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,2,GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,2)+1)--kill player num
	nLTramT = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,7)
	nLTramP = nLTramT*5*nCRankT;
	
	local nCRankC = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,11)
	local nTLuyP = floor((TAB_QUANHAM[nCRankC][1]/nCRankT)+nLTramP)
	
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,6,nTLuyTam+nTLuyP)
	SetMission(M_TICHLUYB,GetMissionV(M_TICHLUYB)+nTLuyP)
	Msg2MSAll(MS_CTHANHCHIEN,format("Ng­êi ch¬i phe c«ng %s giÕt chÕt %s cña phe thñ. TÝch luü t¨ng thªm %d ®iÓm", nCName, nTName, nTLuyP))
	--UpdateMisionValueTopCTC()
--	nguoi choi thu chet	
	local nTVongT = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,4) -- tu vong
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,4,nTVongT+1) -- sè lÇn tö vong
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,7,0)
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,1,0)
end

function OnPlayerTimerIdle(nPlayerIndex)

end
