--Author: Fong KiÒu
--Date: 28/11/2020
--Function: Xa phu trong map c«ng thµnh chiÕn

NOW_END_SAY = "KÕt thóc ®èi tho¹i./no"

function main(nNpcIndex)
	
	Say("Ng­¬i muèn ®i ®©u?", 6,
		"Ph­îng T­êng Phñ./go_move",
		"T­¬ng D­¬ng Phñ./go_move",
		"D­¬ng Ch©u Phñ./go_move",
		"Thµnh §« Phñ./go_move",
		"BiÖn Kinh Phñ./go_move",
		NOW_END_SAY)

end

function go_move(nSel)
	local nRow = nSel + 1
	local TAB_MOVE_MAP =
	{
		{1, 1594, 3197, 6 },
		{78, 1569, 3229, 13},		
		{80, 1764, 3026, 24},
		{11, 3140, 5064, 30},
		{37, 1723, 3097, 18},
	}
	NewWorld(TAB_MOVE_MAP[nRow][1], TAB_MOVE_MAP[nRow][2], TAB_MOVE_MAP[nRow][3])
	SetRevPos(TAB_MOVE_MAP[nRow][4])
end

function no()

end