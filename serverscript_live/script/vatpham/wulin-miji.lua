--hoangnhk
Include("\\script\\lib\\lib_task.lua")

function main(nItemIdx)

	--dofile("script/item/wulin-miji.lua")

	local str={
		"B¹n cÇm quyÓn Vâ L©m MËt TÞch nghiªn cøu c¶ nöa ngµy, kÕt qu¶ còng kh«ng lÜnh ngé ®­îc g×. ",
		"B¹n cÇm quyÓn Vâ L©m MËt TÞch nghiªn cøu c¶ nöa ngµy, kÕt qu¶ lÜnh ngé ®­îc chót Ýt. ",
		"B¹n cÇm quyÓn Vâ L©m MËt TÞch nghiªn cøu c¶ nöa ngµy, kÕt qu¶ còng thu ®­îc mét vµi ®iÒu t©m ®¾c liªn quan ®Õn vâ c«ng ",
		"B¹n ®· nghiÒn ngÉm kü quyÓn Vâ L©m MËt TÞch, nh­ng kh«ng thu ®­îc ®iÒu g× t©m ®¾c "
		}
	local nGioihan = 15
	if(GetAccount() == "") then
		nGioihan = 50
	end	
	local nValue = GetTask(T_VLMT)
	if (nValue >= nGioihan) then
		Msg2Player(str[4])
	elseif(GetLevel() < 80) then
		Msg2Player(str[2])
	else
		SetTask(T_VLMT, nValue+1)
		AddMagicPoint(1)
		RemoveItem(nItemIdx, 1)
		Msg2Player(str[3])
	end
end