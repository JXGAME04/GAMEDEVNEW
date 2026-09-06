-- Author: Fong Ki“u
-- Date: 28/11/2020
-- Function: Ch∆n cÊng 3 trap khi ch≠a bæt Æ«u

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")

function main(nPlayerIndex)
	local nTime = GetMSRestTime(MS_TONGKIM,1)
	local nSecond = floor(nTime/18)	
	if (nTime > 0) then
		SetPos(1332, 3443)
		SetFightState(1)
		Talk(1, "", "ßπi chi’n TËng Kim cﬂn <color=red>"..nSecond.."<color> gi©y sœ bæt Æ«u")
	end	
end
