--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add471()
	local level = random(2,10)
	AddSkillState(471, level, 1, 12 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> håi phôc sinh lùc trong nöa gi©y "..(level*500).." ®iÓm vµ håi phôc néi lùc trong nöa gi©y "..(level*500).." ®iÓm")
end

function add472()
	local level = random(1,10)
	AddSkillState(472, level, 1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y håi phôc sinh lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm vµ 30 gi©y håi phôc néi lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm")	
end

function add490()
	local level = random(10,20)
	AddSkillState(490, level,1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> møc sinh lùc 30 gi©y t¨ng: "..(level*100).." ®iÓm vµ trong 30 gi©y møc néi lùc t¨ng: "..(level*100).." ®iÓm")
end

Tab={add471,add472,add490}

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

