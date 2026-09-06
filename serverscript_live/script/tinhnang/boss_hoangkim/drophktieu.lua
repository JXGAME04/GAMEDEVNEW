-- Author: Fong Kieu
-- Date: 28/11/2016

Include("\\script\\lib\\lib_vatpham.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_sukien.lua")
Include("\\script\\lib\\lib_server.lua")

function DropRate(nNpcIndex, nPlayerIndex)

	PlayerIndex = nPlayerIndex
	local nSeries			= GetNpcSeries(nNpcIndex);
	local nLevel  			= GetNpcLevel(nNpcIndex);
	
	local nDropItem			= "";
	

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

	nDropItem = "\\settings\\droprate\\goldennpc\\npcdroprate90.ini"

	DropRateItem(nNpcIndex,10,nDropItem,1,nLevel,nSeries)
	
	
	local nRandom = random(1,100)
	if(nRandom == 10) then
		DropItem(nNpcIndex,6,1,26,0,0,0,0)--vlmt
	elseif(nRandom == 11) then		
		DropItem(nNpcIndex,6,1,22,0,0,0,0)--ttk
	elseif(nRandom == 12) then
		DropItem(nNpcIndex,4,353,0,0,0,0,0)--THBT
	elseif(nRandom == 13) then		
		DropItem(nNpcIndex,4,238,0,0,0,0,0)--Thuy tinh
	elseif(nRandom == 14) then				
		DropItem(nNpcIndex,4,239,0,0,0,0,0)--Thuy tinh
	elseif(nRandom == 15) then				
		DropItem(nNpcIndex,4,240,0,0,0,0,0)--Thuy tinh
	elseif(nRandom == 16) then
		DropItem(nNpcIndex,6,1,12,0,0,0,0)--ban nhuoc tam kinh
	elseif(nRandom == 17) then				
		DropItem(nNpcIndex,6,1,18,0,0,0,0)--tam tam tuong anh phu
	elseif(nRandom == 18) then				
		DropItem(nNpcIndex,6,1,23,0,0,0,0)--thiet la han
	elseif(nRandom == 19) then				
		DropItem(nNpcIndex,6,1,20,0,0,0,0)--hoa hong
	elseif(nRandom > 90) then
		DropItem(nNpcIndex,6,1,4815,0,0,0,0)--tui bi kip
	elseif(nRandom < 70) then
		DropItem(nNpcIndex,6,1,4818,0,0,0,0)--lb da tau
	end
	dropeventboss(nNpcIndex);
	dropntiendong(nNpcIndex);
	dropeventHMD(nNpcIndex);
	dropmanhhkmp(nNpcIndex);
	Msg2SubWorld("<color=green>[S˘ Ki÷n] <color=white>Boss Ti”u Hoµng Kim <color=yellow>["..GetNpcName(nNpcIndex).."]<color=white> Æ∑ bﬁ nh„m cÒa "..GetName().." ti™u di÷t")
	--Msg2SubWorld("ßπi hi÷p <color=yellow>"..GetName().."<color> Æ∑ ti™u di÷t Æ≠Óc Ti”u Hoµng Kim nhÀn nhi“u ph«n th≠Îng")
	
end
