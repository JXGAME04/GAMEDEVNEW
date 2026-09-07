-- ================================================================================================
-- [HE THONG] script/startgame/npccreate.lua
-- Muc dich  : OnCreate cho NPC (C++ KNpcSet.cpp goi ExecuteScript theo ten tep nay).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng) + C++ goi truc tiep theo ten tep
-- Include   : lib_server.lua
-- Ham (dong): OnCreate (3)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================

Include("\\script\\lib\\lib_server.lua")


function OnCreate(nNpcIndex)
	local nLevel  			= GetNpcLevel(nNpcIndex);
	local Series = GetNpcSeries(nNpcIndex) --a them tuy y, cong thuc y nhu the nay thoi
	local nSTVL, nDoc, nBang, nHoa, nLoi = 0,0,0,0,0
	local nGoldNpc 			= GetNpcBoss(nNpcIndex);
	local NPCLIFENormal = 0;
	local nExp				= 1;
	local EXPBoss			= 15; -- x3 nhan kinh nghiem boss xanh;
	if (nLevel <= 0) then
	return end;
	
	if (nLevel < 10) then
		nLevel = 1;
	elseif (nLevel > 100) then 
		nLevel = random(9,10);
	else
		nLevel = floor(nLevel/10);
	end;
	if (nLevel == 1) then
		NPCLIFENormal = 200;
		nExp = 1000*EXPBoss;	
	elseif (nLevel == 2) then
		NPCLIFENormal = 600;
		nExp = 1300*EXPBoss;
	elseif (nLevel == 3) then
		NPCLIFENormal = 800;
		nExp = 1400*EXPBoss;
	elseif (nLevel == 4) then
		NPCLIFENormal = 2000;
		nExp = 1500*EXPBoss;
	elseif (nLevel == 5) then
		NPCLIFENormal = 5000;
		nExp = 2000*EXPBoss;
	elseif (nLevel == 6) then
		NPCLIFENormal = 10000;
		nExp = 2000*EXPBoss;
	elseif (nLevel == 7) then
		NPCLIFENormal = 20000;
		nExp = 2000*EXPBoss;
	elseif (nLevel == 8) then
		NPCLIFENormal = 24000;
		nExp = 2000*EXPBoss;
	elseif (nLevel >= 9) then
		local nRanDrop = random(1,2);
		if (nRanDrop > 2) then
		NPCLIFENormal = 30000;
		nExp = 3000;
		else
		NPCLIFENormal = 30000;
		nExp = 5000;
		end
	end; 

	if(Series==0 and nLevel >= 5) then		--KIM
		SetNpcSkill(nNpcIndex, 419, 20, 1);
		SetNpcSkill(nNpcIndex, 419, 20, 2);
		SetNpcSkill(nNpcIndex, 419, 20, 3);
		SetNpcSkill(nNpcIndex, 424, 20, 4);
		nSTVL = 1;
	elseif(Series==1 and nLevel >= 5) then	--MOC
		SetNpcSkill(nNpcIndex, 420, 20, 1);
		SetNpcSkill(nNpcIndex, 420, 20, 2);
		SetNpcSkill(nNpcIndex, 420, 20, 3);
		SetNpcSkill(nNpcIndex, 425, 20, 4);
		nDoc = 1;--doc sat khong giong sat thuong khac, = sat thuong khac chia 5, vi no co rut' doc
	elseif(Series==2 and nLevel >= 5) then	--THUY
		SetNpcSkill(nNpcIndex, 421, 20, 1);
		SetNpcSkill(nNpcIndex, 421, 20, 2);
		SetNpcSkill(nNpcIndex, 421, 20, 3);
		SetNpcSkill(nNpcIndex, 426, 20, 4);
		nBang = 1; --neu quai he thuy thi cho bang sat
	elseif(Series==3 and nLevel >= 5) then	--HOA
		SetNpcSkill(nNpcIndex, 422, 20, 1);
		SetNpcSkill(nNpcIndex, 422, 20, 2);
		SetNpcSkill(nNpcIndex, 422, 20, 3);
		SetNpcSkill(nNpcIndex, 427, 20, 4);
		nHoa = 1;--quai he hoa thi cho hoa sat,tuy theo quai manh hay yeu ma` cho so nay, vi du 8x thi cho 75 chan han ok
	elseif(Series==4 and nLevel >= 5) then	--THO
		SetNpcSkill(nNpcIndex, 423, 20, 1);
		SetNpcSkill(nNpcIndex, 423, 20, 2);
		SetNpcSkill(nNpcIndex, 423, 20, 3);
		SetNpcSkill(nNpcIndex, 428, 20, 4);
		nLoi = 1;
	else					--KHONG CO HE, truong hop nay la add sai hay sao do
		SetNpcSkill(nNpcIndex, 418, 20, 1);
		SetNpcSkill(nNpcIndex, 418, 20, 2);
		SetNpcSkill(nNpcIndex, 418, 20, 3);
		SetNpcSkill(nNpcIndex, 418, 20, 4);
		nSTVL = 1;
	end
	--end setskill-----------------------ket thuc phan tinh skill va sat thuong---

	if nGoldNpc == 1 then			--thiet lap thuoc tinh cho boss xanh
		local nLife = floor(NPCLIFENormal/2);
		if(nLife <= 0) then
			nLife = 1
		end
		SetNpcLife(nNpcIndex, nLife,1);--set mau' cho boss xanh
		SetNpcReplenish(nNpcIndex,1,1);--phuc hoi sinh luc
		SetNpcHitRecover(nNpcIndex,100,1);--cao nhat la 100(ko giat) --tuy cam hung
		SetNpcSpeed(nNpcIndex, 20,1);--toc do di chuyen tang len
		SetNpcActiveRange(nNpcIndex, 1200)
		SetNpcResist(nNpcIndex, 8*nLevel, 8*nLevel, 8*nLevel, 8*nLevel, 8*nLevel,1);--khang' cac loai
		SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi,0);--sat thuong noi cong
		SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi,1);--sat thuong ngoai cong	
		-- SetNpcDmgEx(nNpcIndex,nSTVL*nLevel, nDoc*nLevel, nBang*nLevel, nHoa*nLevel, nLoi*nLevel,0);--sat thuong noi cong
		-- SetNpcDmgEx(nNpcIndex,nSTVL*nLevel, nDoc*nLevel, nBang*nLevel, nHoa*nLevel, nLoi*nLevel,1);--sat thuong ngoai cong		
		
	else							--thiet lap sat thuong cho npc thuong`
		SetNpcLife(nNpcIndex, NPCLIFENormal,1);
		SetNpcHitRecover(nNpcIndex,22,1);
		SetNpcRevTime(nNpcIndex,20*18,1);
		SetNpcActiveRange(nNpcIndex, 800)
		SetNpcSpeed(nNpcIndex, 10,1);--toc do di chuyen tang len
		SetNpcResist(nNpcIndex, 2*nLevel, 2*nLevel, 2*nLevel, 2*nLevel, 2*nLevel,1);--khang' cac loai
		SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi,0);--sat thuong noi cong
		SetNpcDmgEx(nNpcIndex,nSTVL, nDoc, nBang, nHoa, nLoi,1);--sat thuong ngoai cong	
	end
	NoReloadNpcAttr(nNpcIndex) ;
	
end;

