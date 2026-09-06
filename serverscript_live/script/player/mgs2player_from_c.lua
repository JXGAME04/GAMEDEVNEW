--Author: Fong KiÒu
--Date: 2021
--Function: th«ng b¸o hÖ thèng tõ gs

Include("\\script\\tinhnang\\congthanhchien\\log_tax.lua")
Include("\\script\\log_game\\save_log.lua")

function main(str)
	--dofile("script/player/mgs2player_from_c.lua")
	if(str) then
		Msg2Player(str)
	end
end
