-- ================================================================================================
-- [HE THONG] script/timertask/task04.lua
-- Muc dich  : VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Ham (dong): OnTimer (6), OnMissionTimer (10)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--[GOHECU 30/08] Tep nay DA RUT VE KHUNG RONG.
--   He cu: hen gio Phong Lang Do cu
--   Ban thay: fld_landingtimer / fld_smalltimer ban Linux (TimerTask 28/29)
--   Ban goc 56 dong da chep sang _dara\script\timertask\task04.lua
-- Giu tep (khong xoa) vi settings\TimerTask.txt tro toi day theo SO HIEU.

function OnTimer()
	StopTimer()
end

function OnMissionTimer()

end
