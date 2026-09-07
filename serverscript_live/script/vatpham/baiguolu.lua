--Author: Fong KiÒu Item B¸ch qu¶ lé, bach qua lo
Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)

	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) == 1) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	
	RemoveItem(nItemIdx,1)
	AddSkillState(442,1,1,64800,-1)
end