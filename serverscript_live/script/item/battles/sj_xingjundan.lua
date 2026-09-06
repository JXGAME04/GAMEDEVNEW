--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add462()
	local level = random(2,10);
	AddSkillState(462, level, 1, 12 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> håi phôc sinh lùc trong nöa gi©y "..(level*500).." ®iÓm")
end

function add464()
	local level = random(1,10)
	AddSkillState(464, level, 1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y håi phôc sinh lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm")
end

function add488()
	local level = random(10,20)
	AddSkillState(488, level, 1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> møc sinh lùc 30 gi©y t¨ng: "..(level*100).." ®iÓm")	
end

Tab={add462,add464,add488}

function main( nItemIndex)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return 1
	end
	if( IsMyItem( nItemIndex ) ) then
		local i = random(getn(Tab))
		Tab[i]()
		RemoveItem(nItemIndex,1)
	end
end

