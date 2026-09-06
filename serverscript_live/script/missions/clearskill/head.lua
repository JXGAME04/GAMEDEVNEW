-- script viet hoa By http://tranhba.com  #´#·##Ä#(ClearSkillPoint)

-- script viet hoa By http://tranhba.com  Ì###ID#º##¢#¸ï£±##Ðó#·
CSP_TID_RevivalSWID 	= 204; -- script viet hoa By http://tranhba.com  ##Ðó##Ý#ID
CSP_TID_ReviveID 		= 205; -- script viet hoa By http://tranhba.com  ##Ðó#·ID
-- script viet hoa By http://tranhba.com  Ì###ID#º##¢#·©###´##Ä#/#±Ä##·´#£ý
CSP_TID_ClearFreeTime	= 144;
-- script viet hoa By http://tranhba.com  Ì###ID#º##¢#±#£¯#´#·Ä#£#(#´##Ä##·###´#±Ä##·##Ì##´)

CSP_TID_JewelEnterTime	= 150;	-- script viet hoa By http://tranhba.com  £#ã·±#£¯##Ì##Ä´#£ý

CSP_NEEDJEWEL_SKILL		= 3; -- script viet hoa By http://tranhba.com  #Ì##Ä######Ä##£ý
CSP_NEEDJEWEL_PROP		= 6; -- script viet hoa By http://tranhba.com  #Ì#±Ä#±#£¯#Ä##£ý

CSP_NEEDJEWEL_SKILL2		= 1;		-- script viet hoa By http://tranhba.com  1##°#ã#
CSP_NEEDJEWEL_PROP2			= 1;		-- script viet hoa By http://tranhba.com  1####ã#


CSP_NEEDLEVEL			= 50; -- script viet hoa By http://tranhba.com  ¿Ð#´#·#Ä######±ð
CSP_MAXClearTime		= 1; -- script viet hoa By http://tranhba.com  ##´ã#´#·´#£ý

CSP_RevieSWID			= 1; -- script viet hoa By http://tranhba.com  ##Ðó#·ID
CSP_MaxTestMapCount		= 10; -- script viet hoa By http://tranhba.com ·¿ß##ÐPK#´#Ä£ý¸¿

-- script viet hoa By http://tranhba.com  ##´ã³#£###Ý#
-- script viet hoa By http://tranhba.com  1-###Ì, 2-³Ð##, 3-´ãµÝ, 4-·ª##, 5-###«, 6-###ý, 7-¸ï°²
CSP_CityMapTab = {1, 11, 162, 37, 78, 80, 176}


-- script viet hoa By http://tranhba.com  #´#·##Ý#
CSP_ClearMapTab 	=  {242, 243, 244, 245, 246, 247, 248}
CSP_TestMapBeginTab =  {249, 259, 269, 279, 289, 299, 309,} -- script viet hoa By http://tranhba.com ·¿ß###³#£#ß##ÐPK#´#Ä#óß####´#ÄMapId,#####Ä£#¸####Ä



-- script viet hoa By http://tranhba.com PK#´#Ä#¤ã#MissionÌ###ID
MISSIONID = 10;
JOINSTATE = 100;


-- script viet hoa By http://tranhba.com ·##³/·##³Ð##´##Ý######º
-- script viet hoa By http://tranhba.com PK²©£¤#´#Ä##Ì#¿ó##±ª
CSP_TestHoleTab = {{1621,3236},{1533,3235},{1520,3352},{1670,3347}};

-- script viet hoa By http://tranhba.com ´ã²©£¤#´##·##³#Ä#Ä####±ª
CSP_ClearHoleTab = {{1407,3331},{1741,3264},{1737,2998},{1443,2975}};

-- script viet hoa By http://tranhba.com ·##³â#####ãß/##â#####ãß#º·##³#Ä######â####´×##Ä##±ª##·¿####¸#####±ª##ß###£#â######·##±ª##¸Ýß###£###â######·##±ª
CSP_ClearTrapTab = {{1584,3232,1592,3222 }, {1657,3243, 1648,3232}, {1653,3172,1645,3184},{1581,3166,1591,3174} }

-- script viet hoa By http://tranhba.com ##Ì#²©£¤#´Ý#£±##´ãÌ#£ý
MAX_MEMBER_COUNT = 20;

CSP_PKTIME = 18 * 60 * 5;
CSP_CAMPMANX = 1582 * 32;
CSP_CAMPMANY = 3303 * 32;

-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com 
-- script viet hoa By http://tranhba.com  #ï#ý³#£##ßºÅ##Ì##µã##´#·##Ý#ID
-- script viet hoa By http://tranhba.com  nCityIndex: ³#£##ßºÅ
function CSP_GetClearMapID(nCityIndex)
	nCount = getn(CSP_ClearMapTab);
	if (nCityIndex < 1) or (nCityIndex > nCount) then
		return -1;
	end
	
	return CSP_ClearMapTab[nCityIndex];
end;

-- script viet hoa By http://tranhba.com  #ï#ý³#£##ßºÅ #####·#±#°¿â###Ä#´#·²©£¤##Ý#MapId
function CSP_GetFreeTestMapID(nCityIndex)
	nCount = getn(CSP_TestMapBeginTab);
	if (nCityIndex < 1) or (nCityIndex > nCount) then
		return -1;
	end
	
	nRet = -1;
	OldSubWorld = SubWorld
	
	for i = 1, CSP_MaxTestMapCount do 
		nMapId = CSP_TestMapBeginTab[nCityIndex] + i - 1;
		idx = SubWorldID2Idx(nMapId);

		if (idx == -1) then 
			print("GetFreeTestMapID Error!!!");
		else
			SubWorld = idx
			if (GetMissionV(1) == 0) then
				nRet = nMapId
				break
			end;
		end;
	end;
	
	SubWorld = OldSubWorld
	return nRet
end;

-- script viet hoa By http://tranhba.com  #ï#ý³#£##ßºÅ##Ì##µã#³#£###Ý#ID
function CSP_GetCityMapID(nCityIndex)
	nCount = getn(CSP_CityMapTab);
	if (nCityIndex < 1) or (nCityIndex > nCount) then
		return -1;
	end
	
	return CSP_CityMapTab[nCityIndex];
end;

-- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com -- script viet hoa By http://tranhba.com 
-- script viet hoa By http://tranhba.com  #ï#ý#´#·##Ý#ID##Ì#³#£##ßºÅ(´ã1¿#£##·)
function CSP_GetCityIndexByClearMap(nMapID)
	nCount = getn(CSP_ClearMapTab);
	for i = 1, nCount do
		if (nMapID == CSP_ClearMapTab[i]) then 
			return i;
		end;
	end;
	return -1
end;

-- script viet hoa By http://tranhba.com  #ï#ý#´#·²©£¤##Ý#ID##Ì#³#£##ßºÅ(´ã1¿#£##·)
function CSP_GetCityIndexByTestMap(nMapID)
	nCount = getn(CSP_TestMapBeginTab);
	for i = 1, nCount do
		if (nMapID >= CSP_TestMapBeginTab[i] and nMapID < (CSP_TestMapBeginTab[i] + CSP_MaxTestMapCount)) then 
			return i;
		end;
	end;
	return -1
end;

-- script viet hoa By http://tranhba.com  #ï#ý³#£###Ý#ID##Ì#³#£##ßºÅ(´ã1¿#£##·)
function CSP_GetCityIndexByCityMap(nMapID)
	nCount = getn(CSP_CityMapTab);
	for i = 1, nCount do
		if (nMapID == CSP_CityMapTab[i]) then 
			return i;
		end;
	end;
	return -1	
end;

function CSP_CheckValid()
	if (IsDisabledUseTownP() ~= 1) then
		return 0;
	end;	
	return 1;
end

function CSP_SwitchTrap(trapIndex)
	if (GetFightState() == 0) then 
		SetFightState(1)
		--SetPKFlag(0)
		--ForbidChangePK(1);
		--SetPunish(0)
		-- script viet hoa By http://tranhba.com Ý##ßÝ#³#£±##±#´#RV##²#¤ó#¢´##ÌÌ#£±ã·RV(³#£###Ðó#·####Ý#³##·)
		--SetLogoutRV(1);
		
		SetPos(CSP_ClearTrapTab[trapIndex][1],CSP_ClearTrapTab[trapIndex][2])
	else
		SetFightState(0)	
		--SetPKFlag(1)
		--ForbidChangePK(0);
		SetPos(CSP_ClearTrapTab[trapIndex][3],CSP_ClearTrapTab[trapIndex][4])
	end;
end
