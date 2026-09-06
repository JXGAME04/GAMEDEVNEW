--Author: Fong KiÒu
--Date: 28/11/2020
--Function:Trap chÆn cæng c«ng thµnh chiÕn -- lËp d­¬ng

Include("\\script\\lib\\lib_task.lua")

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if(GetMissionV(M_CTHANHVO_1) == 0) then --cæng thµnh ch­a bÞ vì
		if(GetCurCamp() == 1) then
			SetPos(1455,3064)
		else
			SetPos(1510,3124)
		end
	end
end
