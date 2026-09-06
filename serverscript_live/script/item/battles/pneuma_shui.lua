--Author: Fong KiÒu
--Function: Item tèng kim

Include("\\script\\header\\forbidmap.lua")

function add466()
	local level = random(1,4)
	AddSkillState(466,level,1,1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 1 phót gi¶m thä th­¬ng "..(level*10).."%")	
end

function add469()
	local level = random(1,4)
	AddSkillState(469,level,1,1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 1 phót gi¶m thêi gian bÞ cho¸ng: "..(level*10).."%")
end

Tab={add466,add469}

function main( nItemIndex)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	if(IsMyItem( nItemIndex ) ) then
		local i = random(getn(Tab))
		Tab[i]()
		RemoveItem(nItemIndex,1)
	end
end

