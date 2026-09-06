--Author: Fong KiÒu
--Function: Item tèng kim

Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	RemoveItem(nItemIdx,1)
	CastSkill(510,1)
	Msg2Player("B¹n sö dông 1 Kh¸ng ®¹n chi gi¸c")
end