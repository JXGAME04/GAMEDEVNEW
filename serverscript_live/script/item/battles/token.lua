--Author: Fong KiÒu
--Date: 2021
--Function: Item bÉy Tèng Kim lÖnh bµi t¨ng 50% tèc ®é di chuyÓn

Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	RemoveItem(nItemIdx,1)
	CastSkill(492, 1)
	Msg2Player("B¹n ®· sö dông 1 lÖnh bµi")
end