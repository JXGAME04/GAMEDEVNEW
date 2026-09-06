--Author: Fong KiÒu
--Date: 28/11/2020
--Function: VÖ binh mËt ®¹o trong map c«ng thµnh chiÕn

Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")

NOW_END_SAY = "KÕt thóc ®èi tho¹i./no"

function main(nNpcIndex)

	dofile("script/tinhnang/congthanhchien/vebinhmatdao.lua")

	Say("<color=fire>VÖ Binh MËt §¹o<color>: h·y nhanh chãng ®i lèi mËt ®¹o tËp kÝch qu©n c«ng thµnh",3,
	format("§i bªn ph¶i/#go_matdao(%d)",1),
	format("§i bªn tr¸i/#go_matdao(%d)",2),
	NOW_END_SAY)

end

function go_matdao(nRow)
	local TAB_POS =
	{
		{1847,3398},
		{1740,3518},		
	}
	SetPos(TAB_POS[nRow][1],TAB_POS[nRow][2])
end