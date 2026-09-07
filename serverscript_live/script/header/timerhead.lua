-- ================================================================================================
-- [HE THONG] script/header/timerhead.lua
-- Muc dich  : Ham thoi gian dung chung cho timer.
-- Duoc nap  : Include tu 1 tep (vd laodau.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Ham (dong): GetRestSec (6), GetRestCTime (10), GetTimerTask (20)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- TimerHead.lua
-- By: Dan_Deng(2003-08-23) ¼ÆÊ±Æ÷Ïà¹Ø¹¤¾ß

FramePerSec = 18			-- Ã¿ÃëÖ¡Êıµ±×÷³£Á¿´¦Àí
CTime = 3600					-- Ã¿¸öÊ±³½°´600Ãë£¨10·ÖÖÓ£©¼ÆËã

function GetRestSec()		-- Ö±½Ó·µ»Ø¼ÆÊ±Æ÷Ê£ÓàÃëÊı
	return floor(GetRestTime() / FramePerSec)
end;

function GetRestCTime()			-- »ñÈ¡¼ÆÊ±Æ÷Ê£ÓàÊ±¼ä£¬´óÓÚÒ»¸öÊ±³½Ê±°´ÖĞ¹úÊ±¼ä×ª»»
	x = floor(GetRestTime() / FramePerSec)
	if (x < CTime) then		-- ²»×ãÒ»¸öÊ±³½
		y = x.." gi©y"
	else
		y = floor(x / CTime).." giê "
	end
	return y
end;

function GetTimerTask()			-- Í¨¹ı¼ÆÊ±Æ÷IDºÅ½âÎöÏàÓ¦ÈÎÎñ
	
end