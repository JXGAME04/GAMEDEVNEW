Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\log_game\\save_log.lua")

function main(nItemIndex)
	
	-- dofile("script/item/brnhutinhhiepcot.lua")
	
	if CheckRoom(2,3) ==0 then
		Say("Xin h·y s¾p xÕp l¹i hµnh trang! Nhí ®Ó trèng 6 « trë lªn nhÐ!", 1, "§­îc råi./no");
		return 0
	end
	local randomtb = random(186, 193)
	AddGoldItem(randomtb,0)
	logHoatDong("[Account:]"..GetAccount()..", ["..GetName().."] Më b¶o r­¬ng nhu t×nh hiÖp cèt nhËn ®­îc GoldId lµ "..randomtb.." \t\n")
	
	RemoveItem(nItemIndex,1)
	return 1
	
end
