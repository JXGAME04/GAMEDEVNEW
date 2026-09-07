-- ================================================================================================
-- [HE THONG] script/timertask/task09.lua
-- Muc dich  : VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : worldlibrary.lua, taskid.lua
-- Ham (dong): OnTimer (7), OnMissionTimer (26)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong KiÒu
--Date: 2021
--Function: 

Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\header\\taskid.lua")

function OnTimer() --2h chay ham OnTimer
	local PK_value = GetPK()
	local nMapId = GetWorldPos()
	StopTimer()

	if (nMapId == 208) then --dai lao
		if (PK_value > 1) then
			Msg2Player("Sau khi xem xÐt kü hµnh ®éng ph¹m téi, téi cña ng­¬i gi¶m bít.") 
			SetPK(PK_value - 1) 
			SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100+(PK_value-1)))
			SetTimer(2*60*60*18,9) --2h chay ham OnTimer
		else
			Msg2Player("Sau khi xem xÐt kü hµnh ®éng ph¹m téi, cuèi cïng b¹n còng röa s¹ch téi cña m×nh.")
			SetPK(0)
			SetTask(TASK_DUNGCHUNG3, SetNumber(3,GetTask(TASK_DUNGCHUNG3),3,100))
		end
	end
end

function OnMissionTimer(nIndex)
	StopMissionTimer(nIndex,9)
end