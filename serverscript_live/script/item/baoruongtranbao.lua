Include("\\script\\lib\\worldlibrary.lua")
Include("\\script\\log_game\\save_log.lua")

function main(nItemIndex)
	
	-- dofile("script/item/baoruongtranbao.lua")
	
	if CheckRoom(2,3) ==0 then
		Say("Xin h·y s¾p xÕp l¹i hµnh trang! Nhí ®Ó trèng 6 « trë lªn nhÐ!", 1, "§­îc råi./no");
		return 0
	end
	
	local randomtb = random(186, 193)
	if(random(1,100) > 60) then
		randomtb = random(159, 167)
	end
	AddGoldItem(randomtb,0)
	logHoatDong("[Account:]"..GetAccount()..", ["..GetName().."] Më b¶o r­¬ng tr©n b¶o nhËn ®­îc GoldId lµ "..randomtb.." \t\n")
	
	RemoveItem(nItemIndex,1)
	return 1
	
end
