--Author: Fong KiÒu
--Date: 28/11/2020
--Fucntion: Trap thñ cæng chÝnh

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
Include("\\script\\lib\\lib_task.lua")

RANDOM_POS={
	{1595,3272},
	{1579,3286},
	{1587,3292},
	{1588,3281},
	{1599,3278},
	{1610,3288},
	{1630,3298},
	{1622,3309},
	{1633,3314}
}

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then
		SetPos(1538,3218)
		return 
	end
	if(GetCurCamp() == 2) then
		SetPos(1595,3272)
	else
		local i = random(getn(RANDOM_POS))
		if(GetFightState() == 0) then
			SetPos(RANDOM_POS[i][1],RANDOM_POS[i][2])
			SetFightState(1)
			SetDeathScript(DEATHFILE_PTHU)
		else
			SetPos(1538,3218)
			SetFightState(0)
		end
	end
end
