--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add462()
	local level = random(2,10);
	AddSkillState(462, level,1, 12 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> håi phôc sinh lùc trong nöa gi©y "..(level*500).." ®iÓm")	
end

function add463()
	local level = random(2,10)
	AddSkillState(463, level,1, 12 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> håi phôc néi lùc trong nöa gi©y "..(level*500).." ®iÓm")	
end

function add464()
	local level = random(1,10)
	AddSkillState(464, level,1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y håi phôc sinh lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm")	
end

function add465()
	local level = random(1,10)
	AddSkillState(465, level,1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y håi phôc néi lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm")	
end

function add466()
	local level = random(1,4)
	AddSkillState(466, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 1 phót gi¶m thä th­¬ng "..(level*10).."%")	
end

function add467()
	local level = random(1,4)
	AddSkillState(467, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 1 phót gi¶m thêi gian tróng ®éc: "..(level*10).."%")	
end

function add468()
	local level = random(1,4)
	AddSkillState(468, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 1 phót gi¶m thêi gian lµm chËm: "..(level*10).."%")
end

function add469()
	local level = random(1,4)
	AddSkillState(469, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 1 phót gi¶m thêi gian bÞ cho¸ng: "..(level*10).."%")	
end

function add471()
	local level = random(2,10)
	AddSkillState(471, level,1, 12 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> håi phôc sinh lùc trong nöa gi©y "..(level*500).." ®iÓm vµ håi phôc néi lùc trong nöa gi©y "..(level*500).." ®iÓm")	
end

function add472()
	local level = random(1,10)
	AddSkillState(472, level,1, 540 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y håi phôc sinh lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm vµ 30 gi©y håi phôc néi lùc, håi phôc mçi nöa gi©y "..(level*100).." ®iÓm")	
end

function add473()
	local level = random(1,7)
	AddSkillState(473, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng phßng phæ trong 1 phót "..(level*10).."%")	
end

function add474()
	local level = random(1,7)
	AddSkillState(474, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng phßng ®éc trong 1 phót "..(level*10).."%")	
end

function add475()
	local level = random(1,7)
	AddSkillState(475, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng phßng b¨ng trong 1 phót "..(level*10).."%")	
end

function add476()
	local level = random(1,7)
		AddSkillState(476, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng phßng háa trong 1 phót "..(level*10).."%")	
end

function add477()
	local level = random(1,7)
	AddSkillState(477, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng phßng l«i trong 1 phót "..(level*10).."%")	
end

function add478()
	local level = random(1,4)
	AddSkillState(478, level,1, 1080 )
	Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng kh¸ng tÝnh trong 1 phót "..(level*10).."%")	
end

function add479()
		level = random(10,20);
		AddSkillState(479, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> s¸t th­¬ng vËt lý néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add480()
		level = random(1,5);
		AddSkillState(480, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> §éc s¸t néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add481()
		level = random(10,20);
		AddSkillState(481, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> B¨ng s¸t néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add482()
		level = random(10,20);
		AddSkillState(482, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> Ho¶ s¸t néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add483()
		level = random(10,20);
		AddSkillState(483, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> L«i s¸t néi c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add485()
		level = random(1,5);
		AddSkillState(485, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> §éc s¸t ngo¹i c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add486()
		level = random(1,4);
		AddSkillState(486, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> B¨ng s¸t ngo¹i c«ng trong 1 phót t¨ng: "..(level*10).." ®iÓm")	
end

function add487()
		level = random(5,10);
		AddSkillState(487, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> s¸t th­¬ng vËt lý ngo¹i c«ng trong 1 phót t¨ng  "..(level*10).."%")	
end

function add488()
		level = random(10,20);
		AddSkillState(488, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc <color=cyan>møc sinh lùc trong 1 phót t¨ng: "..(level*100).." ®iÓm")	
end

function add489()
		level = random(10,20);
		AddSkillState(489, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc <color=cyan>møc néi lùc trong 1 phót t¨ng: "..(level*100).." ®iÓm")	
end

function add490()
		level = random(10,20);
		AddSkillState(490, level,1, 540 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> 30 gi©y møc néi lùc t¨ng:: "..(level*100).." ®iÓm vµ trong 30 gi©y møc néi lùc t¨ng: "..(level*100).." ®iÓm")	
end

function add493()
		level = random(1,3);
		AddSkillState(493, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng tèc ®é di chuyÓn trong 1 phót "..(level*10).."%")	
end

function add495()
		level = random(10,20);
		AddSkillState(495, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng søc m¹nh tÊn c«ng trong 1 phót "..(level*10).."%")	
end

function add496()
		level = random(1,2);
		AddSkillState(496, level,1, 1080 )
		Msg2Player("B¹n nhËn ®­îc<color=cyan> t¨ng ®éng t¸c xuÊt chiªu trong 1 phót "..(level*10).."%")	
end

Tab={add462,add463,add464,add465,add466,add467,add468,add469,add471,add472,add473,add474,add475,add476,add477,add478,add479,add480,add481,add482,add483,add485,add486,add487,add488,add489,add490,add493,add495,add496}

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

