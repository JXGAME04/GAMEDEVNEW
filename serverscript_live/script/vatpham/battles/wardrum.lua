--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim chiÕn cæ t¨ng 30% kh¸ng vµ m¸u

Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	RemoveItem(nItemIdx,1)
	CastSkill(491, 1)
	Msg2Player("B¹n ®· sö dông 1 ChiÕn cæ ")
end
