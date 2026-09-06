-- Author: Fong Kieu
-- Date: 28/11/2016

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\lib\\lib_server.lua")


function OnDeath(nNpcIndex, nDamageIndex)
	DelNpc(nNpcIndex)	
	local nPlayerIndex = NpcIdx2PIdx(nDamageIndex)
	PlayerIndex = nPlayerIndex	

end

function OnRevive(nNpcIndex)
	local Series = GetNpcSeries(nNpcIndex)
	local nSTVL, nDoc, nBang, nHoa, nLoi = 0,0,0,0,0
	
		if (Series == 0) then
		SetNpcSkill(nNpcIndex,424,50,1);
		SetNpcSkill(nNpcIndex,424,50,2);
		SetNpcSkill(nNpcIndex,424,50,3);
		SetNpcSkill(nNpcIndex,424,50,4);
		nSTVL = 5 
	elseif (Series == 1) then
		SetNpcSkill(nNpcIndex,425,50,1);
		SetNpcSkill(nNpcIndex,425,50,2);
		SetNpcSkill(nNpcIndex,425,50,3);
		SetNpcSkill(nNpcIndex,425,50,4);
		nDoc = 5
	elseif (Series == 2) then
		SetNpcSkill(nNpcIndex,426,50,1);
		SetNpcSkill(nNpcIndex,426,50,2);
		SetNpcSkill(nNpcIndex,426,50,3);
		SetNpcSkill(nNpcIndex,426,50,4);
		nBang = 5 
	elseif (Series == 3) then
		SetNpcSkill(nNpcIndex,427,50,1);
		SetNpcSkill(nNpcIndex,427,50,2);
		SetNpcSkill(nNpcIndex,427,50,3);
		SetNpcSkill(nNpcIndex,427,50,4);
		nHoa = 5 
	elseif (Series == 4) then
		SetNpcSkill(nNpcIndex,428,50,1);
		SetNpcSkill(nNpcIndex,428,50,2);
		SetNpcSkill(nNpcIndex,428,50,3);
		SetNpcSkill(nNpcIndex,428,50,4);
		nLoi = 5 
	else
		SetNpcSkill(nNpcIndex,424,50,1);
		SetNpcSkill(nNpcIndex,424,50,2);
		SetNpcSkill(nNpcIndex,424,50,3);
		SetNpcSkill(nNpcIndex,424,50,4);
		nSTVL = 5 
	end;
	
	SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,0)
	SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi ,1)
	SetNpcResist(nNpcIndex, 25, 25, 25, 25, 25)--khang' cac loai
	SetNpcLife(nNpcIndex,500000);--set mau' cho boss xanh
	SetNpcExp(nNpcIndex, 9999,1);--set kinh nghiem lai nhu cu
	SetNpcReplenish(nNpcIndex,0);--phuc hoi sinh luc
	SetNpcBoss(nNpcIndex, 3)
end

function OnTimer(nNpcIndex)
	DelNpc(nNpcIndex)
end
	
	
