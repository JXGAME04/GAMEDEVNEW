Include("\\script\\lib\\worldlibrary.lua")

function main(nItemIndex)
	
	dofile("script/item/market_pack8x.lua")
	
	if CalcFreeItemCellCount() < 10 then
		Talk(1,"","Kho¶ng trèng hµnh trang kh«ng ®ñ 10 « . §Ò nghÞ dän dÑp hµnh trang tr­íc khi më !")
		return
	end	
	
	--AddItem(6,1,2433,0,0,0,0)
	--AddItem(6,1,2434,0,0,0,0)
		i = random(1,10)
	    AddItem(0,10,5,i,0,0,10)
		RemoveItem(nItemIndex,1)
	end	
	
	