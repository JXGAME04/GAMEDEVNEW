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
	local level = random(1,7)
	AddSkillState(475,level,1,1080)
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng phßng b¨ng trong 1 phót "..(level*10).."%")	
end

