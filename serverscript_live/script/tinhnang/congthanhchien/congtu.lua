--Author: Fong KiÒu
--Date: 28/11/2021
--Function: Player Phe C«ng Tö TrËn

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")

function OnPlayerDeath(nPlayerIndex,nLastDamageIndex) -- player, npc
	PlayerIndex = nPlayerIndex
	local nCName  = GetName()
	local nPTIdx = NpcIdx2PIdx(nLastDamageIndex)
	if (nPTIdx == 0) then	-- neu la quai vat thi ngung
		return 
	end
	local nLTramP = 0
	local nPlayerDataTIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,nPTIdx)
	local nPlayerDataCIdx = PIdx2MSDIdx(MS_CTHANHCHIEN,nPlayerIndex)
	local nLRankT = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,11)
	local nCRankC = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,11)
	PlayerIndex = nPTIdx
	local nTName  = GetName()
	PlayerIndex = nPlayerIndex
	
--	diem cho nguoi choi thu
	local nTLuyTam = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,6)
	local nLTramT  = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,7)
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,7,nLTramT+1)		-- lien tram
	if (GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,7) >= GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,5)) then		-- so sanh ltram va lien tram max
		SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,5,GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,7)) --max LT
	end
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,2,GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,2)+1) --kill player num
	nLTramT = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,7)
	nLTramP = nLTramT*5*nCRankC
	
	local nCRankT = GetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,11)
	local nTLuyP = floor((TAB_QUANHAM[nCRankT][1]/nCRankC)+nLTramP)
	
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataTIdx,6,nTLuyTam+nTLuyP)
	SetMission(M_TICHLUYA,GetMissionV(M_TICHLUYA)+nTLuyP)
	Msg2MSAll(MS_CTHANHCHIEN,format("Ng­êi ch¬i phe thñ %s giÕt chÕt %s cña phe c«ng. TÝch luü t¨ng thªm %d ®iÓm", nTName, nCName, nTLuyP))
	--UpdateMisionValueTopCTC()
--	nguoi choi cong chet	
	local nTVongC = GetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,4) -- tu vong
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,4,nTVongC+1) -- sè lÇn tö vong
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,7,0)
	SetPMParam(MS_CTHANHCHIEN,nPlayerDataCIdx,1,0)
end

function OnPlayerTimerIdle(nPlayerIndex)

end
