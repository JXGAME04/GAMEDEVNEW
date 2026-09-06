--Author: Fong KiÒu
--Date: 28/11/2020
--Function:Trap chÆn cæng c«ng thµnh chiÕn -- dinh xuyen

Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if(GetMissionV(M_CTHANHVO_2) == 0) then --cæng thµnh ch­a bÞ vì
		if(GetCurCamp() == 1) then
			SetPos(1516,2996)
		else
			SetPos(1573,3057)
		end
	end
end
