-- Author: Fong Ki“u
--Date: 27/06/2021
--Function: TËng Kim B› B∂o

Include("\\script\\global\\vatpham.lua")

function main(nItemIdx)
	
	--dofile("script/item/tongkimbibao.lua")
	
	if CalcFreeItemCellCount() < 10 then
		Talk(1,"","Kho∂ng trËng hµnh trang kh´ng ÆÒ 10 ´ . ß“ nghﬁ d‰n d—p hµnh trang tr≠Ìc khi mÎ !")
		return
	end	
	AddItem(6,1,random(392,396),0,0,0,0)--bay tong kim
	AddRandNHKTHoatDong()
	Msg2Player("ChÛc mıng nhÀn Æ≠Óc ph«n th≠Îng khi mÎ l‘ vÀt.")
	RemoveItem(nItemIdx,1)
end