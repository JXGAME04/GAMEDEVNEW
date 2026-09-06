--Author: Fong KiÒu
--Date: 07/07/2021
--Function: ThÈm Cöu Ba L¨ng HuyÖn

function main(sel)
	
	Say("<color=green>ThÈm cöu: <color>Chèn th«n lµng yªn tØnh nµy, kh«ng muèn cã sù quÊy ph¸. <sex> ®Õn ®©y ®Ó lµm g× ?",5,
	"Ta muèn më réng r­¬ng/morongruongchua",
	"Ta muèn më réng r­¬ng 2/morongruongchua2",
	"Ta muèn më réng r­¬ng 3/morongruongchua3",
	"Ta muèn më réng hµnh trang/moronghanhtrang",
	"Ta chØ ®Õn th¨m «ng./no")
end

function no()
end

function morongruongchua3()
	local tienmmr = 10000000
	if GetExBox() == 2 then
		if GetCash() >= tienmmr then
			Pay(tienmmr)
			SetExBox(3)
			Msg2Player("Ng­¬i ®· më ru¬ng 3 thµnh c«ng !")
		else
			Talk(1,"","Ng­¬i kh«ng cã mang theo ®ñ tiÒn "..tienmmr.." l­îng mµ muèn ta gióp sao ?")
		end
		else if GetExBox() == 0 then
			Talk(1,"","Ng­¬i ch­a më ru¬ng 1 !")
		else if GetExBox() == 1 then
			Talk(1,"","Ng­¬i ch­a më ru¬ng 2 !")
		else if GetExBox() == 3 then
			Talk(1,"","Ng­¬i ®· më ru¬ng 3 råi cßn muèn më c¸i g×?")
		end
		end
		end
	end
end

function morongruongchua2()
	local tienmmr = 10000000
	if GetExBox() == 1 then
	if GetCash() >= tienmmr then
		Pay(tienmmr)
		SetExBox(2)
		Msg2Player("Ng­¬i ®· më ru¬ng 2 thµnh c«ng !")
	else
		Talk(1,"","Ng­¬i kh«ng cã mang theo ®ñ tiÒn "..tienmmr.." luîng mµ muèn ta gióp sao ?")
	end
	else if GetExBox() == 0 then
		Talk(1,"","Ng­¬i chu¨ më ru¬ng 1 !")
	else if GetExBox() >= 2 then
		Talk(1,"","Ng­¬i ®· më ru¬ng 2 råi cßn muèn më g× n÷a !")
	end
	end
	end
end

function morongruongchua()
	local tienmmr = 10000000
	if GetExBox() == 0 then
		if GetCash() >= tienmmr then
			Pay(tienmmr)
			SetExBox(1)
			Msg2Player("Ng­¬i ®· më ru¬ng thµnh c«ng !")
		else
		Talk(1,"","Ng­¬i kh«ng cã mang theo ®ñ tiÒn "..tienmmr.." luîng mµ muèn ta gióp sao ?")
		end
	else
		Talk(1,"","Ng­¬i ®· më ru¬ng 1 råi cßn muèn më c¸i g× n÷a?")
	end
end

function moronghanhtrang()
	local tienmmr = 20000000
	if GetExItem() == 1 then
		Talk(1,"","Ng­¬i ®· më hµnh trang råi cßn muèn më c¸i g× n÷a?")
	else
		if GetCash() >= tienmmr then
			Pay(tienmmr)
			SetExItem(1)
			Msg2Player("Ng­¬i ®· më hµnh trang thµnh c«ng !")
		else
			Talk(1,"","Ng­¬i kh«ng cã mang theo ®ñ tiÒn "..tienmmr.." l­îng mµ muèn ta gióp sao ?")
		end
	end
end