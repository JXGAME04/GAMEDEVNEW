Include("\\script\\lib\\worldlibrary.lua")

function main(nItemIndex)
	
	-- dofile("script/item/market_pack.lua")
	
	if CalcFreeItemCellCount() < 10 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 10 « . §Ò nghÞ dän dÑp hµnh trang tr­íc khi më !")
		return
	end	
	  AddItem(0,10,8,10,0,0,10)
	RemoveItem(nItemIndex,1)
end	
	
	