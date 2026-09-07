--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add483()
	local level = random(10,20)
	AddSkillState(483, level, 1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> L«i s¸t néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add499()
	local level = random(10,30)
	AddSkillState(499, level, 1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> L«i s¸t ngo¹i c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

Tab={add483,add499}

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


