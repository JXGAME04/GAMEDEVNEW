--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add463()
	local level = random(2,10)
	AddSkillState(463, level, 1, 12 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> håi phôc néi lùc trong nöa gi©y "..(level*500).." ®iÓm")
end

function add465()
	local level = random(1,10)
	AddSkillState(465, level, 1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y håi phôc néi lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm")	
end

function add489()
	local level = random(10,20)
	AddSkillState(489, level, 1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y møc néi lùc t¨ng:: "..(level*100).." ®iÓm")	
end

Tab={add463,add465,add489}

function main( nItemIndex)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	if( IsMyItem( nItemIndex ) ) then
		local i = random(getn(Tab))
		Tab[i]()
		RemoveItem(nItemIndex,1)
	end
end

