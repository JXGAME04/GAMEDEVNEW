--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Trap mËt ®¹o c«ng thµnh chiÕn

RANDOM_POS={
	{1750,3505},
	{1836,3411}
}

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	if(GetCurCamp() == 1) then
		local i = random(getn(RANDOM_POS))
		SetPos(RANDOM_POS[i][1],RANDOM_POS[i][2])
	end
end
