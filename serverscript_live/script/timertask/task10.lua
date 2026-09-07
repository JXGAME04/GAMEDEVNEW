-- ================================================================================================
-- [HE THONG] script/timertask/task10.lua
-- Muc dich  : VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : lib_task.lua, lib_map.lua, lib_server.lua, lib_vuotai.lua, save_log.lua, lib_phonglangdo.lua, lib_tktc.lua
-- Ham (dong): OnTimer (13), OnMissionTimer (18), ontime_tongkimketthuc (26)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong Ki“u
--Date: 2021
--Function: 
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_server.lua")
-- [GOHECU 30/08] he cu da go
-- Include("\\script\\tinhnang\\vuot_ai\\lib_vuotai.lua")
Include("\\script\\log_game\\save_log.lua")
-- [GOHECU 30/08] he cu da go
-- Include("\\script\\tinhnang\\phonglangdo\\lib_phonglangdo.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")


function OnTimer()
	StopTimer();
end;

function OnMissionTimer()
	-- [GOHECU 30/08] da go 2 nhanh chet: MAP_DUATHUYEN_PLD (Phong Lang Do cu) va
	-- MAP_VUOTAI (Vuot Ai cu). Chi con Tong Kim.
	if (SubWorld == SubWorldID2Idx(MAP_TK_TC)) then
		ontime_tongkimketthuc()
	end
end;
function ontime_tongkimketthuc() --ket thuc vuot ai khi h’t 30p
	StopMissionTimer(MS_TONGKIM, 10)--tat hen gio so 2
	-- CloseMission(MS_TONGKIM) --ket thuc vuot ai khi h’t 30p
end;
-- [GOHECU 30/08] da go ham ontime_phonglangdoketthuc (4 dong) - he cu
-- [GOHECU 30/08] da go ham ontime_vuotaiketthuc (4 dong) - he cu
