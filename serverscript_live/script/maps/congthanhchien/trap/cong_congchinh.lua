--Author: Fong KiÒu
--Date: 28/11/2020
--Fucntion: Trap c«ng cæng chÝnh

RANDOM_POS={
	{1795,3485},
	{1803,3478},
	{1810,3468},
	{1815,3445},
	{1805,3457},
	{1792,3467},
	{1783,3491},
	{1773,3488}
}

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if (GetMSRestTime(MS_CTHANHCHIEN,1) > 0) then
		SetPos(1897,3569)
		return
	end
	if(GetCurCamp() == 1) then
		SetPos(1795,3485)
	else
		local i = random(getn(RANDOM_POS))
		if(GetFightState() == 0) then
			SetPos(RANDOM_POS[i][1],RANDOM_POS[i][2])
			SetFightState(1)
			SetDeathScript(DEATHFILE_PCONG)
			--SetProtectTime(18*3)
			--AddSkillState(963, 1, 0, 18*3)			
		else
			SetPos(1897,3569)
			SetFightState(0)
		end
	end
end
