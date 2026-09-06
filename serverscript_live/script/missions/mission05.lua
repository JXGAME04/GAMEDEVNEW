--Author: Fong KiÒu
--Date: 2021
--Function: 

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")

function BeginMission()

end

function EndMission()

end

function OnLeave(nPlayerIndex)
	for i=1, getn(MAP_KMQ) do
		--Msg2Player(format("%d %d", i, MAP_KMQ[i]))
		if(SubWorld == SubWorldID2Idx(MAP_KMQ[i])) then	
			kiemmonquan_out(nPlayerIndex)
			return 
		end
	end
end

function kiemmonquan_out(nPlayerIndex)
	SubWorld = SubWorldID2Idx(995)
	PlayerIndex = nPlayerIndex
	local nPlayerDataIdx = PIdx2MSDIdx(MS_KIEMMONQUAN, PlayerIndex)
	SetFightState(0)
	SetDeathScript("")
	SetPunish(0)
	SetCreateTeam(1)--mo lai tinh nang to doi
	SetPKMode(0, 0) --tr¶ l¹i kiÓu pk tù do
	--SetCurCamp(GetCamp())
	local szName = GetName()
	if(nPlayerDataIdx > 0) then
		SetPMParam(MS_KIEMMONQUAN, nPlayerDataIdx, 0, 0)
		Msg2MSAll(MS_KIEMMONQUAN, format("%s ®· rêi khái KiÕm M«n Quan.", szName)) --thong bao roi khoi~
	end
end
