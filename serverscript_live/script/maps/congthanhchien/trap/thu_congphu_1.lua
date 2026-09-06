--Author: Fong KiÒu
--Date: 28/11/2020
--Fucntion: Trap thñ cæng phô sè 1

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then
		SetPos(1538,3218)
		return 
	end
	if(GetCurCamp() == 2) then
		SetPos(1549,3372)
	else
		if(GetFightState() == 0) then
			SetPos(1549,3372)
			SetFightState(1)
			SetDeathScript(DEATHFILE_PTHU)
			--SetProtectTime(18*3)
			--AddSkillState(963, 1, 0, 18*3)			
		else
			SetPos(1543,3359)
			SetFightState(0)
		end
	end
end
