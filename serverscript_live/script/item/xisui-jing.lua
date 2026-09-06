--Author: Fong KiÒu Item tÈy tuû kinh
Include("\\script\\lib\\lib_task.lua")

function main(nItemIdx)

	--dofile("script/item/xisui-jing.lua")

	local str ={
		"B¹n ®· xem kü quyÓn 'TÈy Tñy Kinh' nh­ng kh«ng thÓ hiÓu",
		"B¹n ®· ®äc 'TÈy Tñy Kinh', nhËn ®­îc 5 ®iÓm tiÒm n¨ng",
	}
	local nGioihan = 15
	if(GetAccount() == "") then
		nGioihan = 50
	end
	
	local nValue = GetTask(T_TTK)
	if (nValue >= nGioihan or GetLevel() < 80) then
		Msg2Player(str[1])
	else
		SetTask(T_TTK, nValue+1)
		AddProp(5)
		RemoveItem(nItemIdx, 1)
		Msg2Player(str[2])
	end
end
