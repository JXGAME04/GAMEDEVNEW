--Author: Fong KiÒu
--Date: 28/11/2020
--Function:Trap chÆn cæng c«ng thµnh chiÕn -- b×nh giang

Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if(GetMissionV(M_CTHANHVO_3) == 0) then --cæng thµnh ch­a bÞ vì
		if(GetCurCamp() == 1) then
			SetPos(1387,3122)
		else
			SetPos(1448,3187)
		end
	end
end
