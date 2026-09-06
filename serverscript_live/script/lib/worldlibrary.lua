SERVER_NAME	= "Vâ L©m Thiªn §iÓu"
SERVER_MODE = 0
-- SERVER_OPEN = 2103141900 -- yy/mm/dd - hh//mm [21-nam] [03-thang] [14-ngay] [19gio] [00phut]
SERVER_OPEN = 2104011900 -- yy/mm/dd - hh//mm [21-nam] [03-thang] [14-ngay] [19gio] [00phut]
SERVER_EVENTID = 0
SERVER_NOTICE = "Sau ®ua top sÏ ho¹t ®éng trë l¹i"

function GetParam(strParam, index, key)
	local KeyName = ",";
	if(key ~= nil)then
		KeyName=key;
	end;
	nLastBegin = 1
	for i=1, index - 1 do
		nBegin = strfind(strParam, KeyName, nLastBegin)
		nLastBegin = nBegin + 1
	end;
	num = 1

	strnum = strsub(strParam, nLastBegin)
	nEnd = strfind(strnum, KeyName)
	if nEnd == nil then 
		return strnum
	end
	str1 = strsub(strnum,1,nEnd -1);
	return str1
end;

function CallPlayerFunction(pid,fun,...) local arg = {n = select("#", ...), ...};
	local oldPid = PlayerIndex
	PlayerIndex = pid
	local ret = call(fun,arg)
	PlayerIndex = oldPid
	return ret
end

function CallTeamFunction(pid,fun,...) local arg = {n = select("#", ...), ...};
	PlayerIndex = pid;
	local size = GetTeamSize();
	if size== 0 then CallPlayerFunction(pid,fun,arg) return end;
	for i=0,7 do
	local nMemId = GetTeamMem(i);
	if(nMemId > 0) then
		CallPlayerFunction(nMemId,fun,arg);
	end
	end;
	PlayerIndex = tempid;
end

function CreateTaskSay(tbTask)
	if (tbTask ~= nil) then
	local tbSay = {};
	for i = 2,getn(tbTask) do
		tbSay[i-1] = tbTask[i];
	end;
	Say(tbTask[1], getn(tbTask)-1,tbSay);
	end
end;

function CreateNewSayEx(strQuestion, tbOpt)
	if (tbOpt ~= nil) then
	local tbSay = {};
	for i = 1,getn(tbOpt) do
		tbSay[i] = tbOpt[i][1].."/"..tbOpt[i][2].."";
	end;
	Say(strQuestion, getn(tbSay),tbSay);
	end
end;

--                  1     2      3    4   5      6        7        8           9            10      11    12
function AddNpcNew(nId,nLevel, nMap, nX, nY, szScript, nCurCamp, szName,  bBarrierCheck, nSeries, nExp, nLife,
--                      13         14       15
					nReplenish, nAttackR, nDefend,			--phuc hoi sinh luc | do chinh xac | ne tranh
--                      16          17
					nMinDamage, nMaxDamage,					--sat thuong nho nhat/lon nhat
--                      18
					nNoAppend,								--Tinh damage tren skill
--                      19           20           21
					nReviveTime, nHitRecover, nBossType,	--Thoi gian phuc sinh, thoi gian phuc hoi, kieu boss
--                      22
					DropRateFile,							--File ty le rot do`
--						23		24		25
					bRemoveDeath, nKind, nMissionAdd)			--di chuyen tu do | phan loai | Mission
	   	mapindex = SubWorldID2Idx(nMap)
		npcid = nId
    	npclvl = nLevel
    	bBarrier = 0
		if(bBarrierCheck~=nil) then
    		bBarrier = bBarrierCheck;
    	end
	local nNpcId = AddNpc(npcid,npclvl,mapindex,nX,nY,bBarrier);
    if(nCurCamp~=nil) then	
		SetNpcCurCamp(nNpcId, nCurCamp);
	end
	if(szName~=nil) then	
		SetNpcName(nNpcId, szName);
		end
	if(nSeries~=nil) then	
		SetNpcSeries(nNpcId, nSeries);
	end
	if(nExp~=nil) then	
		SetNpcExp(nNpcId, nExp, 1);
		end
	if(nLife~=nil) then	
		SetNpcLife(nNpcId, nLife, 1);
		end
	if(nReplenish~=nil) then	
		SetNpcReplenish(nNpcId, nReplenish, 1);
		end
	if(nAttackR~=nil) then	
		SetNpcAR(nNpcId, nAttackR, 1);
		end
	if(nDefend~=nil) then	
		SetNpcDefense(nNpcId, nDefend, 1);
		end
	if(nMinDamage~=nil and nMaxDamage~=nil) then	
		SetNpcDamage(nNpcId, nMinDamage, nMaxDamage);
		end
	if(nNoAppend~=nil) then	
		--SetNpcCancelDmg(nNpcId, nNoAppend);
		end
	if(nReviveTime~=nil) then	
		SetNpcRevTime(nNpcId,nReviveTime);
		end
	if(nHitRecover~=nil) then	
		SetNpcHitRecover(nNpcId, nHitRecover, 1);
		end
	if(nBossType~=nil) then	
		--SetNpcBoss2(nNpcId, nBossType);
		end
	if(DropRateFile~=nil) then	
		SetNpcDropScript(nNpcId, DropRateFile);
		end
	if(bRemoveDeath~=nil) then
		SetNpcRemoveDeath(bRemoveDeath);
		end
	if(nKind~=nil) then
		SetNpcKind(nKind);
		end
	if(nMissionAdd ~= nil) then
		AddMSNpc(nMissionAdd,nNpcIdx);
	end
	------------------
	if(szScript~=nil) then
    	SetNpcScript(nNpcId, szScript);
    	end	
	return nNpcId
end;

function AddTrapEx1(nMap, nBX, nBY, nDistance, szScript)
	local nX,nY = nBX,nBY;
	for i=0,nDistance do
		AddTrap(nMap, (nX + i)*32, (nY - i)*32, szScript)
	end
	nX = nX - 1;
	for i=0,nDistance do
		AddTrap(nMap, (nX + i)*32, (nY - i)*32, szScript)
	end
end;

function AddTrapEx2(nMap, nBX, nBY, nDistance, szScript)
	local nX,nY = nBX,nBY;
	for i=0,nDistance do
		AddTrap(nMap, (nX + i)*32, (nY + i)*32, szScript)
	end
	nY = nY - 1;
	for i=0,nDistance do
		AddTrap(nMap, (nX + i)*32, (nY + i)*32, szScript)
	end
end;

function AddTrapEx3(nMap, nBX, nBY, nDistance, szScript)
	local nX,nY = nBX,nBY;
	for i=0,nDistance do
		AddTrap(nMap, nX*32, (nY + i)*32, szScript)
	end
	nX = nX - 1;
	for i=0,nDistance do
		AddTrap(nMap, nX*32, (nY + i)*32, szScript)
	end
end;

function AddTrapEx4(nMap, nBX, nBY, nDistance, szScript)
	local nX,nY = nBX,nBY;
	for i=0,nDistance do
		AddTrap(nMap, (nX + i)*32, nY*32, szScript)
	end
	nY = nY - 1;
	for i=0,nDistance do
		AddTrap(nMap, (nX + i)*32, nY*32, szScript)
	end
end;

function AddTrapEx5(nNum, nMap, nSX, nSY, nDX, nDY, szScript)
	local nRange = 1
	local nDir = GetDir(nSX,nSY,nDX,nDY);
	local nDis = GetDistance(nSX,nSY,nDX,nDY);
	local nSin = DirSin(nDir);
	local nCos = DirCos(nDir);
	local nTrapNum = floor(nDis/nRange/50);
	for i=1,nTrapNum do
		for j=1,nNum do
		AddTrap(nMap,nSX+floor(nCos*i*nRange/20)+nNum*25,floor(nSY+nSin*i*nRange/20),szScript)
		end;
	end;
end;

function AddQuestKey(nID)
	AddItemID(AddItem(0,4,nID,0,0,5,0,0));
end;

function AddMagicScript(nID)
	AddItemID(AddItem(0,6,nID,0,0,5,0,0));
end;

function DropQuestKey(NpcIndex,nID)
	local nIndex;
	nIndex = AddItem(0,4,nID,0,0,5,0,0);
	DropItem(NpcIndex,nIndex);
end;

function DropMagicScript(NpcIndex,nID)
	local nIndex;
	nIndex = AddItem(6,1,nID,0,0,0,0);
	DropItem(NpcIndex,nIndex);
end;

-- function TaoBang(tTable, sTableName, sTab)
    -- sTab = sTab or "";  
    -- sTmp = ""  
    -- sTmp = sTmp..sTab..sTableName.."={"  
 
    -- local tStart = 0  
    -- for key, value in tTable do  
  
        -- if tStart == 1 then  
            -- sTmp = sTmp..",\r\n"  
 
        -- else  
            -- sTmp = sTmp.."\r\n"  
            -- tStart = 1  
        -- end  
        -- local sKey = (type(key) == "string") and format("[%q]",key) or format("[%d]",key);  
        -- if(type(value) == "table") then  
            -- sTmp = sTmp..TaoBang(value, sKey, sTab.."\t");  
        -- else  
            -- local sValue = (type(value) == "string") and format("%q",value) or tostring(value);  
            -- sTmp = sTmp..sTab.."\t"..sKey.." = "..sValue 
        -- end  
 
    -- end 
    -- sTmp = sTmp.."\r\n"..sTab.."}"  
    -- return sTmp
  
-- end  

-- function SaveData(file, string)   ---Tuy ngan nhung wan trong nhat >''<
-- file_op = openfile( file, "w+" )  --- a+ la vit them hem xoa file dau -- w+ -- la che do doc ghi xoa file dau  
    -- write(file_op,string) 
    -- closefile(file_op)  
-- end 

-- function LuuBang(file, string)
-- local f,e = openfile( file, "w+" )
-- if f then
    -- write(f,string)
    -- closefile(f)
    -- return 1
-- else
    -- local _,_,path = strfind(file, "(.+[/_\\]).+$") 
    -- if path ~= nil then execute("mkdir ".."\""..gsub(path, "/", "\\").."\"") end
    -- f,e = openfile( file, "w+" )
    -- if f then
        -- write(f,string) 
        -- closefile(f)
        -- return 2
    -- else
        -- return 0
        -- end
    -- end
-- end

function no()
end;