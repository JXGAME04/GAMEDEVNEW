-- ================================================================================================
-- [HE THONG] script/timertask/task08.lua
-- Muc dich  : VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : lib_task.lua, lib_kmq.lua
-- Ham (dong): OnTimer (7), OnMissionTimer (11)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
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