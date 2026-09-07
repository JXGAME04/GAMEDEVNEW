--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	RemoveItem(nItemIdx,1)
	AddSkillState( 479, 10, 1, 3240)
	Msg2Player("B¹n ®­îc 1 viªn Néi Phæ hoµn")
end