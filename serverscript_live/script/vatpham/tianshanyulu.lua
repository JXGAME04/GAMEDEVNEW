--Author: Fong KiÒu
--Function: Item Thiªn s¬n b¶o lé
Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) == 1) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end	
		
	AddSkillState(441, 1, 1, 64800,-1)
	RemoveItem(nItemIdx,1)
end
