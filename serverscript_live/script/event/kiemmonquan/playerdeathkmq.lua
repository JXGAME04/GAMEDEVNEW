-- Author: Fong KiÒu
--Date: 2021
--Function: Player KiÕm M«n Quan BÞ chÕt

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")

function OnPlayerDeath(nPlayerIndex, nLastDamageIndex)
	PlayerIndex = nPlayerIndex
	local nSubWorldId = SubWorldID2Idx(995)
	SubWorld = nSubWorldId
	local nPlayerDataIdx = PIdx2MSDIdx(MS_KIEMMONQUAN, PlayerIndex)
	SetDeathScript("")
	SetPunish(0)					--t¾t tÝnh n¨ng chÕt kh«ng mÊt g×
	SetFightState(0)
	SetCreateTeam(1)--më l¹i tÝnh n¨ng tæ ®éi
	SetPKMode(0, 0) --tr¶ l¹i kiÓu pk tù do
	--SetCurCamp(GetCamp())
	local szName = GetName()
	if(nPlayerDataIdx > 0) then	
		SetPMParam(MS_KIEMMONQUAN, nPlayerDataIdx, 0, 0)
		Msg2MSAll(MS_KIEMMONQUAN, format("%s ®· tö n¹n t¹i KiÕm M«n Quan quay vÒ thµnh.", szName)) --thong bao roi khoi~
	end		
end

function OnPlayerTimerIdle(nPlayerIndex)

end

