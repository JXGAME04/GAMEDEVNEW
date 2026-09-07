-- [LOCAL54 06/09 toi] 4 ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc
local add505, add506, add507, add508
--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim

Include("\\script\\header\\forbidmap.lua")

function add505()
	CastSkill(505, 1 )
	Msg2Player("B¹n sö dông 1<color=cyan> Tam Thanh Khİ, khiÕn kÎ ®Şch quanh b¹n bŞ cho¸ng.")	
end

function add506()
	CastSkill(506, 1 )
	Msg2Player("B¹n sö dông 1<color=cyan> Tam Thanh Khİ, khiÕn kÎ ®Şch quanh b¹n bŞ ®ãng b¨ng.")	
end

function add507()
	CastSkill(507, 1 )
	Msg2Player("B¹n sö dông 1<color=cyan> Tam Thanh Khİ, khiÕn kÎ ®Şch quanh b¹n bŞ tróng ®éc.")	
end

function add508()
	CastSkill(508, 1 )
	Msg2Player("B¹n sö dông 1<color=cyan> Tam Thanh Khİ, khiÕn kÎ ®Şch quanh b¹n bŞ gi¶m tèc.")	
end

Tab={add505,add506,add507,add508}

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

