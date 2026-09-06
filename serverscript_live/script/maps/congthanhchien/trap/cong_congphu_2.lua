--Author: Fong KiÒu
--Date: 28/11/2020
--Fucntion: Trap c«ng cæng phô sè 2

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then
		SetPos(1897,3569)
		return 
	end
	if(GetCurCamp() == 1) then
		SetPos(1862,3401)
	else
		if(GetFightState() == 0) then
			SetPos(1862,3401)
			SetFightState(1)
			SetDeathScript(DEATHFILE_PCONG)
			--SetProtectTime(18*3)
			--AddSkillState(963, 1, 0, 18*3)			
		else
			SetPos(1869,3415)
			SetFightState(0)
		end
	end
end
