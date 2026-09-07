Include("\\script\\lib\\worldlibrary.lua")

function main(nItemIndex)
	
	dofile("script/item/tuibikip.lua")
	
	if CheckRoom(2,3) ==0 then
		Say("Xin h·y s¾p xÕp l¹i hµnh trang! Nhí ®Ó trèng 6 « trë lªn nhÐ!", 1, "§­îc råi./no");
		return 0
	end
	
	if(random(1,10) > 5) then
		AddItem(6,1,random(27,28),0,0,0,0)
	else
		AddItem(6,1,random(33,58),0,0,0,0)
	end
	
	RemoveItem(nItemIndex,1)
	return 1
	
end
