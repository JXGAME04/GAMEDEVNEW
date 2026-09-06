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
