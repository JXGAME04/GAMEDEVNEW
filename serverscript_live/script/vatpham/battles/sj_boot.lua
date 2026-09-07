--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add493()
	local level = random(1,3)
	AddSkillState(493,level,1,1080)
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng tèc ®é di chuyÓn trong 1 phót "..(level*10).."%")
end

Tab={add493}

function main( nItemIndex)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) ~= 1) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return 
	end
	if(IsMyItem( nItemIndex )) then
		local i = random(getn(Tab))
		Tab[i]()
		RemoveItem(nItemIndex,1)
	end
end

