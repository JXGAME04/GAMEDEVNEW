--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add481()
	local level = random(10,20);
	AddSkillState(481, level, 1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> B¨ng s¸t néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add486()
	local level = random(1,10)
	AddSkillState(486, level, 1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> B¨ng s¸t ngo¹i c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

Tab={add481,add486}

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

