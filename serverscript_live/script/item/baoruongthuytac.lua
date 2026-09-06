-- Author: Fong KiÒu
--Date: 27/06/2021
--Function: B¶o r­¬ng Thuû TÆc

Include("\\script\\global\\vatpham.lua")

function main(nItemIdx)
	
	--dofile("script/item/baoruongthuytac.lua")
	
	if CalcFreeItemCellCount() < 10 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 10 « . §Ò nghÞ dän dÑp hµnh trang tr­íc khi më !")
		return
	end	
	
	AddItem(6,1,11,0,0,0,0)--phao hoa
	AddRandNHKTHoatDong()
	Msg2Player("Chóc mõng nhËn ®­îc phÇn th­ëng khi më lÔ vËt.")
	RemoveItem(nItemIdx,1)
	
end