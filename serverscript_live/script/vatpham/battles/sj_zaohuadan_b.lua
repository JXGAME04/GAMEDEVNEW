--Author: Fong KiÒu
--Date: 2021
--Function: Item Tèng Kim cµn kh«n t¹o ho¸ ®an tiÓu

Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end
	if(RemoveItem(nItemIdx,1)==1) then
		ModifyAttrib(153,2000,100,0)
		ModifyAttrib(154,2000,100,0)
	end
end