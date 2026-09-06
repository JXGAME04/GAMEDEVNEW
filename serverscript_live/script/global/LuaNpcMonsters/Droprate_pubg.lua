--  <Death 9x>
Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\lib\\lib_server.lua")

function DropRate(nNpcIndex,nPlayerIndex)
PlayerIndex = nPlayerIndex;
	local nSeries			= GetNpcSeries(nNpcIndex);
	local nLevel  			= GetNpcLevel(nNpcIndex);
	local nGoldNpc 			= GetNpcBoss(nNpcIndex);
	local nPriceNormal 		= 0;
	local nDropItem			= "";
	local nPriceBossBlue 	= 0;
	local nXMoney 			= 1; -- x2 drop tien
	local nExp				= 1;
	local EXPBoss			= 1; -- x3 nhan kinh nghiem boss xanh;
	local nIndexItem = 0;
	if (nLevel <= 0) then
	return end;
	if (nSeries < 0 or nSeries > 4) then 
	return end;
	if (nLevel < 10) then
		nLevel = 1;
	elseif (nLevel > 100) then 
		nLevel = random(9,10);
	else
		nLevel = floor(nLevel/10);
	end;
	-----///DROPPRICE

	if (nLevel == 1) then
		nPriceNormal = random(30,32);
		nPriceBossBlue = random(100,132);
		NPCLIFENormal = 200;
		nDropItem = "\\settings\\droprate\\npcdroprate10.ini"	
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv10.ini"
	elseif (nLevel == 2) then
		nPriceNormal = random(50,52);
		nPriceBossBlue = random(100,232);
		NPCLIFENormal = 400;
		nDropItem = "\\settings\\droprate\\npcdroprate20.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv20.ini"
	elseif (nLevel == 3) then
		nPriceNormal = random(60,82);
		nPriceBossBlue = random(200,332);
		NPCLIFENormal = 800;
		nDropItem = "\\settings\\droprate\\npcdroprate30.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv30.ini"
	elseif (nLevel == 4) then
		nPriceNormal = random(80,102);
		nPriceBossBlue = random(200,432);
		NPCLIFENormal = 2000;
		nDropItem = "\\settings\\droprate\\npcdroprate40.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv40.ini"
	elseif (nLevel == 5) then
		nPriceNormal = random(90,202);
		nPriceBossBlue = random(300,532);
		NPCLIFENormal = 5000;
		nDropItem = "\\settings\\droprate\\npcdroprate50.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv50.ini"
	elseif (nLevel == 6) then
		nPriceNormal = random(100,252);
		nPriceBossBlue = random(300,432);
		NPCLIFENormal = 10000;
		nDropItem = "\\settings\\droprate\\npcdroprate60.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv60.ini"
	elseif (nLevel == 7) then
		nPriceNormal = random(110,272);
		nPriceBossBlue = random(300,532);
		NPCLIFENormal = 20000;
		nDropItem = "\\settings\\droprate\\npcdroprate70.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\golden_lv70.ini"
	elseif (nLevel == 8) then
		nPriceNormal = random(210,282);
		nPriceBossBlue = random(300,632);	
		NPCLIFENormal = 24000;
		nDropItem = "\\settings\\droprate\\npcdroprate80.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\80_01234droprate.ini"
	elseif (nLevel >= 9) then
		local nRanDrop = random(1,2);
		if (nRanDrop > 2) then
		nPriceNormal = random(301,302);
		nPriceBossBlue = random(301,332);
		NPCLIFENormal = 30000;
		nDropItem = "\\settings\\droprate\\npcdroprate90.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\90_01234droprate.ini"
		else
		nPriceNormal = random(411,502);
		nPriceBossBlue = random(411,532);		
		NPCLIFENormal = 30000;
		nDropItem = "\\settings\\droprate\\npcdroprate90.ini"
		nDropItemBoss = "\\settings\\droprate\\goldennpc\\90_01234droprate.ini"
		end
	end; 


if(nGoldNpc == 1) then--boss xanh

		DropRateItem(nNpcIndex,5,nDropItemBoss,1,nLevel,nSeries)
		nSel = random(0,10);
		if(nSel > 8) then
		DropNpcMoney(nNpcIndex,nPriceBossBlue*nXMoney)
end
return end
-----------------------------------------------------------------------------------------
	local nSelT = random(0,30);
		if(nSelT == 2) then
		DropNpcMoney(NpcIndex,nPriceNormal*nXMoney)
		elseif (nSelT < 4) then 
	    DropRateItem(nNpcIndex,1,nDropItem,1,nLevel,nSeries)
		end
DropItemPUBG(nNpcIndex, 0, random(0,141), 0, 10, 0, 0, 2)
dropeventPUBG(nNpcIndex);
end
