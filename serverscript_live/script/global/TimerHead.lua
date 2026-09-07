-- ================================================================================================
-- [HE THONG] script/global/TimerHead.lua
-- Muc dich  : Ham timer (trung header/timerhead.lua).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Ham (dong): GetRestSec (6), GetRestCTime (10), GetTimerTask (20)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- TimerHead.lua
-- Author: Fong Ki“u

FramePerSec 		= 18			
CTime 				= 600					

function GetRestSec(i)		
	return floor(GetRestTime(i) / FramePerSec)
end;

function GetRestCTime(i)
	x = floor(GetRestTime(i) / FramePerSec)
	if (x < CTime) then		
		y = x.." gi©y"
	else
		y = floor(x / CTime).." giÍ"
	end
	return y
end;

function GetTimerTask(i)		
	
end
