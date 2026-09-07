--Author: Fong KiÒu
--Function:que hoa tuu chua tang may man cho dong doi
Include("\\script\\header\\forbidmap.lua")

function main( nItemIndex )
	
	dofile("script/item/ghj.lua")
	local W,X,Y = GetWorldPos()
	local nMapId = W
	if (checkSJMaps(nMapId) == 1) then
		Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
		return
	end	
	local size=GetTeamSize();
	if(size <= 0) then
		Msg2Player("Kh«ng cã trong ®éi ngò, kh«ng thÓ sö dông ®¹o cô nµy");
		return 
	end
	local szName = GetName();
	local nTeamId = GetTeam();
	local nCaptain = PlayerIndex;
	local nPIndex
	local count = 0;
	local PlayerTab = {};
	RemoveItem(nItemIndex,1)
	for i = 0, 7 do
		nPIndex = GetTeamMem(nTeamId, i)
		if(nPIndex > 0) then
			PlayerTab[count+1] = nPIndex;
			count = count + 1;
			PlayerIndex = nPIndex
			Msg2Player("Nh©n vËt: "..szName.." sö dông QuÕ Hoa Töu 1")
			AddSkillState(450, 1, 1, 30*60*18, -1)
		end
	end
--	PlayerIndex = nCaptain
--	AddSkillState(450, 1, 1,30*60*18, -1)
	--RemoveItem(nItemIndex,1)
end
