Include("\\script\\global\\huashan2013\\hs_shim.lua")	-- [HOASON 01/09]
function main(sel)
	OpenBox();		--弹出储物箱物品框和玩家物品框，以便于玩家进行物品和金钱的存取
	if (GetFaction() == "huashan") then
		SetRevPos(1)		--设置重生点
	end
end;
