IncludeLib("SETTING")

function tong_check_add()
	if GetLevel() < 70 then
		Msg2Player("CÇn cÊp 70 trë lªn míi ®­îc gia nhËp bang héi")
		return
	end
	return 1
end

function tong_check_create()
	local nTransLifeCount = ST_GetTransLifeCount()	
--	if nTransLifeCount < 3 then
--		Msg2Player("CÇn trïng sinh 3 cÊp 190 trë lªn míi ®­îc thµnh lËp bang héi")
--		return 
--	end
--	if nTransLifeCount == 3 and GetLevel() < 190 then
--		Msg2Player("CÇn trïng sinh 3 cÊp 190 trë lªn míi ®­îc thµnh lËp bang héi")
--		return 
--	end

	if GetLevel() < 80 then
		Msg2Player("CÇn cÊp 80 trë lªn míi ®­îc thµnh lËp bang héi")
		return 
	end

	return 1
end

function tong_check_leave()
	return 1
end

function tong_check_changemaster()
	return 1
end

function tong_check_billboard()
	return 1
end

function tong_check_dispense()
	return 1
end

