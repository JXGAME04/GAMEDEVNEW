--Author: Fong KiÒu
--Date: 07/07/2021
--Function: Tiªn Th¶o Lé

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\header\\forbidmap.lua")

function main(nItemIdx)

	--local W,X,Y = GetWorldPos()
	--local nMapId = W
	--if (checkSJMaps(nMapId) == 1) then
	--	Msg2Player("B¹n kh«ng thÓ sö dông vËt phÈm nµy ë ®©y")
	--	return
	--end		
	
	local nTime = GetTask(T_X2EXP)
	--if (nTime > 0 or GetNpcExpRate() > 100) then
	if(GetNpcExpRate() > 100) then
		Talk(1,"","VÉn cßn thêi gian nh©n ®«i kinh nghiÖm.")
		return 
	end
	nTime = 60*60
	--SetTask(T_X2EXP,nTime)
	AddSkillState(440,1,1,nTime*18,-1)
	RemoveItem(nItemIdx,1)
end