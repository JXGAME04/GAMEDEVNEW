--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Trap mËt ®¹o c«ng thµnh chiÕn

RANDOM_POS={
	{1585,3476},
	{1594,3480},
	{1603,3473},
	{1604,3486},
	{1590,3457}
}

function main(nPlayerIndex)
	PlayerIndex = nPlayerIndex
	local i = random(getn(RANDOM_POS))
	SetPos(RANDOM_POS[i][1],RANDOM_POS[i][2])
end
