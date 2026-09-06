--Author: Fong KiÒu
--Date: 2021
--Function: Qu©n Y C«ng Thµnh

Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")

NOW_END_SAY = "KÕt thóc ®èi tho¹i./no"

function main()
	dofile("script/tinhnang/congthanhchien/quany.lua")
	local TAB_QUANY = {
			"<color=green><npc><color>: ta lµ qu©n tiÕp viÖn cña c«ng thµnh quan, <sex> cã thÓ mua qu©n d­îc t¹i ®©y",
			"Mau cho ta xem/nhanQuanLuong",
			NOW_END_SAY,
		}
	SayEx({TAB_QUANY[1], TAB_QUANY[2], TAB_QUANY[3]})
end

function nhanQuanLuong()
	SetFightState(0) Sale(53,0)
end