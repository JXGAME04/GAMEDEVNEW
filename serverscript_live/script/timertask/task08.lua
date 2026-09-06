--Author: Fong KiÒu
--Date: 2021
--Function: Timer KiÕm M«n Quan

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\event\\kiemmonquan\\lib_kmq.lua")

function OnTimer()
	StopTimer()
end

function OnMissionTimer()
	SubWorld = SubWorldID2Idx(995)
	StopMissionTimer(MS_KIEMMONQUAN, 8)--tat hen gio so 8
	local nCount = GetMSPlayerCount(MS_KIEMMONQUAN)
	for dataindex = 1, nCount do		
		if(GetPMParam(MS_KIEMMONQUAN, dataindex, 0) == 1) then --dang online th× rêi khái map
			PlayerIndex = MSDIdx2PIdx(MS_KIEMMONQUAN, dataindex)
			NewWorld(53, 1622, 3189) --vÒ ba l¨ng
			SetDeathScript("")
			SetLogoutRV(0)
			SetFightState(0)
			LeaveTeam()
			Msg2Player("KÕt thóc ho¹t ®éng t¹i KiÕm M«n Quan tÊt c¶ rêi khái khu vùc chiÕn ®Êu")
			SetCreateTeam(1)--mo lai tinh nang to doi
			SetPKMode(0, 0) --tr¶ l¹i kiÓu pk tù do
		end
	end
	DelAllNpc(SubWorld)
	CloseMission(MS_KIEMMONQUAN)
end