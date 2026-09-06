--Author: Fong KiÒu
--Date: 28/11/2020
--Fucntion: Trap c«ng cæng phô sè 1

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then
		SetPos(1897,3569)
		return 
	end
	if(GetCurCamp() == 1) then
		SetPos(1732,3529)
	else
		if(GetFightState() == 0) then
			SetPos(1732,3529)
			SetFightState(1)
			SetDeathScript(DEATHFILE_PCONG)
			--SetProtectTime(18*3)
			--AddSkillState(963, 1, 0, 18*3)			
		else
			SetPos(1740,3545)
			SetFightState(0)
		end
	end
end
