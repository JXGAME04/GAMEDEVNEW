-- Author: Fong Ki“u
--Date: 06/07/2021
--Function: L‘ Bao TËng Kim

Include("\\script\\global\\vatpham.lua")

function main(nItemIdx)
	
	--dofile("script/item/lebaotongkim.lua")
	
	if CalcFreeItemCellCount() < 10 then
		Talk(1,"","Kho∂ng trËng hµnh trang kh´ng ÆÒ 10 ´ . ß“ nghﬁ d‰n d—p hµnh trang tr≠Ìc khi mÎ !")
		return
	end
	local nRan = random(1,10)
	if(nRan > 9) then
		AddOwnExp(10000000)
	else
		AddOwnExp(1000000)
	end
	AddRandNHKTHoatDong()
	Msg2Player("ChÛc mıng nhÀn Æ≠Óc ph«n th≠Îng khi mÎ l‘ vÀt.")
	RemoveItem(nItemIdx,1)
end