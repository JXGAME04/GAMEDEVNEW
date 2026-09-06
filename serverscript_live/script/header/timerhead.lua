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