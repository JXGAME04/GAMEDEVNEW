--Author: Fong KiÒu
--Date: 28/11/2020
--Fucntion: Trap thñ cæng phô sè 2

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then
		SetPos(1538,3218)
		return 
	end
	if(GetCurCamp() == 2) then
		SetPos(1679,3260)
	else
		if(GetFightState() == 0) then
			SetPos(1679,3260)
			SetFightState(1)
			SetDeathScript(DEATHFILE_PTHU)
			--SetProtectTime(18*3)
			--AddSkillState(963, 1, 0, 18*3)			
		else
			SetPos(1673,3244)
			SetFightState(0)
		end
	end
end
