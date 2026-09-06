--Author: Fong KiÒu

SKILLLIST = {256,257,258,259,260,261,262,263,264,265}
Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)
	
	local nIndex,nItemGenre, nDetail,nParticur, nLevel, nSeries, nLuck, nStackNum = FindItemEx(nItemIdx)
	
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if ( checkSJMaps(nMapId) ~= 1 ) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end	
	
	--dofile("script/item/potion15.lua")
	
	--Msg2Player("nIndexl:"..nIndex)
	--Msg2Player("nItemGenre:"..nItemGenre)
	--Msg2Player("nDetail:"..nDetail)

	AddSkillState(SKILLLIST[nDetail], 1, 1 , 5*60*18, 0)
	Msg2Player(format("B¹n sö dông 1 viªn %s",GetNameItem(nItemIdx)))
	RemoveItem(nItemIdx,1)
end