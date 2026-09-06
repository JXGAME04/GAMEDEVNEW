Include("\\script\\global\\maplist.lua")
Include("\\script\\activitysys\\config\\129\\variables.lua")

function OnBiaoCheFarAwayPlayerDisapper(nNpcIndex, szPlayerName)--ïÚ³µÀëÍæ¼ÒÌ«Ô¶£¬³¬Ê±Ö®ºó×Ô¶¯ÏûÊ§
    --´Ëº¯ÊýÖÐµÄPlayerIndexÎÞÐ§£¬²»ÄÜÊ¹ÓÃ
    local nX32, nY32, nMapIdx = GetNpcPos(nNpcIndex);
    local nMapID = SubWorldIdx2ID(nMapIdx);
    local szBiaoCheName = GetNpcName(nNpcIndex);
    WriteYunBiaoLog(format("[Long M«n Tiªu Côc] %s t¹i [%s] c¸ch ng­êi ch¬i 750m qu¸ giê biÕn mÊt", szBiaoCheName, tbGlobalMapId2Name[nMapID]));
    AddStatData("lmbj_car_faraway");
end

function OnBiaoCheDisapper(nNpcIndex, szPlayerName)--ïÚ³µ´æÔÚ³¬¹ý30·ÖÖÓÏûÊ§
    --´Ëº¯ÊýÖÐµÄPlayerIndexÎÞÐ§£¬²»ÄÜÊ¹ÓÃ
    local nX32, nY32, nMapIdx = GetNpcPos(nNpcIndex);
    local nMapID = SubWorldIdx2ID(nMapIdx);
    local szBiaoCheName = GetNpcName(nNpcIndex);
    WriteYunBiaoLog(format("[Long M«n Tiªu Côc] %s t¹i [%s] ¸p tiªu qu¸ giê tù ®éng biÕn mÊt", szBiaoCheName, tbGlobalMapId2Name[nMapID]));
    AddStatData("lmbj_car_timeover");
    
    local nLevel = GetNpcParam(nNpcIndex, 1);
    if (nLevel >= 10) then
    	local nTongId = GetNpcParam(nNpcIndex, 5) + GetNpcParam(nNpcIndex, 6);
	    if (nTongId ~= 0) then
	    	local szMsg = format("[¸p tiªu bang héi] %s t¹i [%s]qu¸ giê ¸p tiªu sÏ tù ®éng mÊt", szBiaoCheName, tbGlobalMapId2Name[nMapID]);
	    	Msg2Tong(nTongId, szMsg);
	    end
    end
end

function OnExchangeServerStart()
    --´Ëº¯ÊýÖÐµÄPlayerIndex¼´Îª¿ç·þÕßµÄË÷Òý
    DynamicExecuteByPlayer(PlayerIndex, "\\script\\activitysys\\config\\129\\extend.lua", "pActivity:PlayerExchageServerLoginOut_BeforeSaveData")
end

function OnBiaoCheChangeMapNotice()
    local nTaskFlagValue = GetTask(TSK_LMBJTaskFlag); -- ÈÎÎñ±ê¼Ç
    if (nTaskFlagValue == 0) then
        return nil;
    end
    local nCurTime = GetCurrentTime();
    if (nCurTime-GetTask(3511) < 15) then
        return nil;
    end
--    local tbBiaoCheLevel = { 1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 
--                10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,};
    local getLevel = function(nId)
        local nLv = nil;
        if nId >= 19 and nId < 23 then -- 7ÐÇïÚ³µ
            nLv = 7;
        elseif nId >= 23 and nId < 26 then -- 8ÐÇïÚ³µ
            nLv = 8;
        elseif nId >= 26 and nId < 27 then -- 9ÐÇïÚ³µ
            nLv = 9;
        elseif nId >= 27 and nId < 45 then -- 10ÐÇïÚ³µ
            nLv = 10;
        end
        return nLv;
    end
    local nId = floor(nTaskFlagValue/10);
    local nLevel = getLevel(nId);
    if not nLevel then
        return nil;
    end
    SetTask(3511, nCurTime);
    local szName = GetName();
    local nMapID = GetWorldPos();
    local sGlobalNews = format("%s-Tiªu Xa %d Sao xuÊt hiÖn ë %s.", szName, nLevel, tbGlobalMapId2Name[nMapID]);
    if nLevel >= 10 then
        local szTongName = GetTongName();
        sGlobalNews = format("Bang %s %s hé tèng Tiªu Xa Bang %d Sao xuÊt hiÖn ë %s.", szTongName, szName, nLevel, tbGlobalMapId2Name[nMapID]);
    end
    AddGlobalNews(sGlobalNews);
end

