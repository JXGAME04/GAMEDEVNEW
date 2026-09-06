--Author: Fong KiÒu
--Date: 2021
--Function: T©m T©m t­¬ng ¸nh phï

Include("\\script\\item\\heart_head.lua")

function main(nItemIdx)
	if (use_heart() == 1) then
		RemoveItem(nItemIdx,1)
	end
end
