-- ================================================================================================
-- [HE THONG] script/lib/lib_map.lua
-- Muc dich  : BAN DO: ten/id map, toa do, dich chuyen, kiem tra vung an toan.
-- Duoc nap  : Include tu 40 tep (vd chuyensinhdaisu.lua, lib.lua, lib_vt.lua, tieudau.lua); engine nap moi .lua thanh 1 lua_State luc boot
-- Include   : npcposarray.lua
-- Ham (dong): CountPlayerInMaps (61), MoveNPC (75), AddNpcEx1 (92), AddNpcEx2 (120), AddNpcEx3 (136), AddNpcEx4 (194), AddNpcEx5 (216), AddNpcNew (274), AddTrapEx1 (353), AddTrapEx2 (365), AddTrapEx3 (377), AddTrapEx4 (389), AddTrapEx5 (401), AddObstacleEx1 (407), GetMapName (688), CheckMapNoForTDP (698), CheckMapNoForTHP (707), CheckMapNoForTKCT (716), AddObjEx1 (725)
-- Sua nong  : CO - trong chuoi Include cua timerserver.lua, tu nap lai trong <= 1 phut sau khi ghi tep
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
-- Author: Fong KiÒu
-- Date: 28/11/2016
-- Chuc nang: Ham ho tro map

Include("\\script\\startgame\\npcposarray.lua")

OTHER_VOSU = ""
-- [GOHECU 30/08] hang CHET - 7 cho dung deu da comment (startgame\thanh\*.lua)
-- OTHER_SGSTHU = "\\script\\tinhnang\\vuot_ai\\sugiasatthu.lua"
-- [GOHECU 30/08] hang CHET - khong noi nao doc
-- OTHER_DATAU = "\\script\\tinhnang\\datau\\datau.lua"
OTHER_LEQUAN = "\\script\\global\\npc\\npcchucnang\\lekienquan.lua"
OTHER_RUONG = "\\script\\global\\npc\\npcchucnang\\ruongchua.lua"
OTHER_THOREN = "\\script\\global\\npc\\npcchucnang\\thoren.lua"
OTHER_BANNGUA = "\\script\\global\\npc\\npcchucnang\\banngua.lua"
OTHER_XAPHU = "\\script\\global\\npc\\npcchucnang\\xaphu.lua"
OTHER_HIEUTHUOC = "\\script\\global\\npc\\npcchucnang\\hieuthuoc.lua"
OTHER_TAPHOA = "\\script\\global\\npc\\npcchucnang\\taphoa.lua"
OTHER_VEBINH = "\\script\\global\\npc\\npcchucnang\\vebinh.lua"
OTHER_BACDAU = "\\script\\global\\npc\\npcchucnang\\bacdaulaonhan.lua"

FACTION_THIEULAM = "\\script\\global\\npc\\npcmonphai\\shaolin.lua"
FACTION_THIENVUONG = "\\script\\global\\npc\\npcmonphai\\tianwang.lua"
FACTION_NGUDOC = "\\script\\global\\npc\\npcmonphai\\wudu.lua"
FACTION_DUONGMON = "\\script\\global\\npc\\npcmonphai\\tangmen.lua"
FACTION_NGAMY = "\\script\\global\\npc\\npcmonphai\\emei.lua"
FACTION_THUYYEN = "\\script\\global\\npc\\npcmonphai\\cuiyan.lua"
FACTION_CAIBANG = "\\script\\global\\npc\\npcmonphai\\gaibang.lua"
FACTION_THIENNHAN = "\\script\\global\\npc\\npcmonphai\\tianren.lua"
FACTION_VODANG = "\\script\\global\\npc\\npcmonphai\\wudang.lua"
FACTION_CONLON = "\\script\\global\\npc\\npcmonphai\\kunlun.lua"

DEATHFILE0X = "\\script\\global\\npc\\ondeath\\death0x.lua"
DEATHFILE1X = "\\script\\global\\npc\\ondeath\\death1x.lua"
DEATHFILE2X = "\\script\\global\\npc\\ondeath\\death2x.lua"
DEATHFILE3X = "\\script\\global\\npc\\ondeath\\death3x.lua"
DEATHFILE4X = "\\script\\global\\npc\\ondeath\\death4x.lua"
DEATHFILE5X = "\\script\\global\\npc\\ondeath\\death5x.lua"
DEATHFILE6X = "\\script\\global\\npc\\ondeath\\death6x.lua"
DEATHFILE7X = "\\script\\global\\npc\\ondeath\\death7x.lua"
DEATHFILE8X = "\\script\\global\\npc\\ondeath\\death8x.lua"
DEATHFILE9X = "\\script\\global\\npc\\ondeath\\death9x.lua"
DEATHFILE10X = "\\script\\global\\npc\\ondeath\\death10x.lua"

DEATHFILE = "\\script\\global\\luanpcmonsters\\ondeath_normal.lua"
DROPFILE = "\\script\\global\\luanpcmonsters\\droprate_normal.lua"

DROPFILE0X = "\\script\\global\\npc\\droprate\\droprate0x.lua"
DROPFILE1X = "\\script\\global\\npc\\droprate\\droprate1x.lua"
DROPFILE2X = "\\script\\global\\npc\\droprate\\droprate2x.lua"
DROPFILE3X = "\\script\\global\\npc\\droprate\\droprate3x.lua"
DROPFILE4X = "\\script\\global\\npc\\droprate\\droprate4x.lua"
DROPFILE5X = "\\script\\global\\npc\\droprate\\droprate5x.lua"
DROPFILE6X = "\\script\\global\\npc\\droprate\\droprate6x.lua"
DROPFILE7X = "\\script\\global\\npc\\droprate\\droprate7x.lua"
DROPFILE8X = "\\script\\global\\npc\\droprate\\droprate8x.lua"
DROPFILE9X = "\\script\\global\\npc\\droprate\\droprate9x.lua"
DROPFILE10X = "\\script\\global\\npc\\droprate\\droprate10x.lua"

MAX_PLAYER = 1500

function CountPlayerInMaps(IdMaps)
	local nP = 0
	local idbt = PlayerIndex
	for i =1,MAX_PLAYER do
		PlayerIndex = i
		local W, X, Y = GetWorldPos()
		if (W == IdMaps) then
			nP = nP + 1
		end
	end
	PlayerIndex = idbt
	return nP
end

function MoveNPC(nMap, nX, nY, nOffsetX, nOffsetY, nPrice, nFightState, nTermini)
	if (nOffsetX == nil) then nOffsetX = 0 end
	if (nOffsetY == nil) then nOffsetY = 0 end	
   	local POSX = floor((nX + nOffsetX/10)*8)
    local POSY = floor((nY + nOffsetY/10)*16)
	if (GetCash() < nPrice) then
		Talk(1,"",format("Kh«ng ®ñ %d l­îng, kh«ng thÓ di chuyÓn.",nPrice))
		return 
	end
	Pay(nPrice)
	NewWorld(nMap,POSX,POSY)
	SetFightState(nFightState)
	if(nTermini ~= nil) then
		AddTermini(nTermini)
	end	
end

function AddNpcEx1(nId, nLevel, szSeries, nMap, nX, nY, szDropScript, szScript, szName, nCurCamp)
	local mapindex = SubWorldID2Idx(nMap)
    local npclvl = nLevel
	local nRandNum = getn(nId)

	local nSeries = 0
	if(szSeries~=nil) then	
		local nRandNum1 = getn(szSeries)
		nSeries = szSeries[random(1,nRandNum1)]
	end

	local nNpcId = AddNpc(nId[random(1,nRandNum)],npclvl,mapindex,nX,nY, nSeries)
	
	if(szName~=nil) then	
		SetNpcName(nNpcId, szName)
	end	
    	if(nCurCamp~=nil) then	
		SetNpcCurCamp(nNpcId, nCurCamp)
	end	
	if(szDropScript~=nil) then
    		SetNpcDropScript(nNpcId, szDropScript)
    end	
	if(szScript~=nil) then
    		SetNpcScript(nNpcId, szScript)
    end
	return nNpcId
end

function AddNpcEx2(nArray, nRegionX, nRegionY, nId, nLevel, szSeries, nMap, nX, nY, szDropScript, szScript, szName, nCurCamp)	
 	for y=1,nRegionY-nY+1 do
		for x=1,floor((nRegionX-nX+2)/2) do
			for i=1,16 do
				for k=1,16 do
					if (NPCPOSARRAY[nArray][i][k] > 0) then
						local nNpcId = AddNpcEx1(nId, nLevel, szSeries, nMap,(((nX+(x-1)*2)*8)+(k-1))*32,(((nY+(y-1))*16)+(i-1))*32, szDropScript, szScript, szName, nCurCamp)
					end
				end
			end
		end
	end
end

--                  1     2      3         4      5  6        7        8           9    10      11      12
function AddNpcEx3(nId, nLevel, szSeries, nMap, nX, nY, szDropScript, szScript, szName, nCurCamp, nExp, nLife,
--                      13         14       15
					nReplenish, nAttackR, nDefend,			--phuc hoi sinh luc | do chinh xac | ne tranh
--                      16          17
					nMinDamage, nMaxDamage,					--sat thuong nho nhat/lon nhat
--                      18           19          
					nReviveTime, nHitRecover)				--Thoi gian phuc sinh, thoi gian phuc hoi
					
	local mapindex = SubWorldID2Idx(nMap)
	local npclvl = nLevel

	local nRandNum = getn(nId)
	local nSeries = 0
	if(szSeries~=nil) then	
		local nRandNum1 = getn(szSeries)
		nSeries = szSeries[random(1,nRandNum1)]
	end

	local nNpcId = AddNpc(nId[random(1,nRandNum)],npclvl,mapindex,nX,nY, nSeries)
	
    if(nCurCamp~=nil) then	
		SetNpcCurCamp(nNpcId, nCurCamp)
	end
	if(szName~=nil) then	
		SetNpcName(nNpcId, szName)
	end
	if(nExp~=nil) then	
		SetNpcExp(nNpcId, nExp, 1)
	end
	if(nLife~=nil) then	
		SetNpcLife(nNpcId, nLife, 1)--set luon mau' nguyen thuy~
	end
	if(nReplenish~=nil) then	
		SetNpcReplenish(nNpcId, nReplenish, 1)
	end
	if(nAttackR~=nil) then	
		SetNpcAR(nNpcId, nAttackR, 1)
	end
	if(nDefend~=nil) then	
		SetNpcDefense(nNpcId, nDefend, 1)
	end
	if(nMinDamage~=nil and nMaxDamage~=nil) then	
		SetNpcDamage(nNpcId, nMinDamage, nMaxDamage)
	end
	if(nReviveTime~=nil) then	
		SetNpcRevTime(nNpcId, nReviveTime)
	end
	if(nHitRecover~=nil) then	
		SetNpcHitRecover(nNpcId, nHitRecover, 1)
	end
	if(szDropScript~=nil) then	
		SetNpcDropScript(nNpcId, szDropScript)
	end
	if(szScript~=nil) then
    	SetNpcScript(nNpcId, szScript)
    end
	return nNpcId
end

function AddNpcEx4(nArray, nRegionX, nRegionY, nId, nLevel, szSeries, nMap, nX, nY, szDropScript, szScript, szName, nCurCamp, nExp, nLife,
--                      16         17       18
					nReplenish, nAttackR, nDefend,			--phuc hoi sinh luc | do chinh xac | ne tranh
--                      19          20
					nMinDamage, nMaxDamage,					--sat thuong nho nhat/lon nhat
--                      21           22        
					nReviveTime, nHitRecover)		
 	for y=1,nRegionY-nY+1 do
		for x=1,floor((nRegionX-nX+2)/2) do
			for i=1,16 do
				for k=1,16 do
					if (NPCPOSARRAY[nArray][i][k] > 0) then
						local nNpcId = AddNpcEx3(nId, nLevel, szSeries, nMap,(((nX+(x-1)*2)*8)+(k-1))*32,(((nY+(y-1))*16)+(i-1))*32, szDropScript, szScript, szName, nCurCamp, nExp, nLife,nReplenish, nAttackR, nDefend, nMinDamage, nMaxDamage ,nReviveTime, nHitRecover)
					end
				end
			end
		end
	end
end

--                  1     2      3         4      5  6        7        8           9    10      11      12
function AddNpcEx5(nId, nLevel, szSeries, nMap, nX, nY, szDropScript, szScript, szName, nCurCamp, nExp, nLife,
--                      13         14       15
					nReplenish, nAttackR, nDefend,			--phuc hoi sinh luc | do chinh xac | ne tranh
--                      16          17
					nMinDamage, nMaxDamage,					--sat thuong nho nhat/lon nhat
--                      18           19          20
					nReviveTime, nHitRecover, nBossType)				--Thoi gian phuc sinh, thoi gian phuc hoi, loai boss gold
					
	local mapindex = SubWorldID2Idx(nMap)
	local npclvl = nLevel
	local nRandNum = getn(nId)
	local nSeries = 0
	if(szSeries~=nil) then	
		local nRandNum1 = getn(szSeries)
		nSeries = szSeries[random(1,nRandNum1)]
	end
	local nNpcId = AddNpc(nId[random(1,nRandNum)],npclvl,mapindex,nX,nY, nSeries)
    if(nCurCamp~=nil) then	
		SetNpcCurCamp(nNpcId, nCurCamp)
	end
	if(szName~=nil) then	
		SetNpcName(nNpcId, szName)
	end
	if(nExp~=nil) then	
		SetNpcExp(nNpcId, nExp, 1)
	end
	if(nLife~=nil) then	
		SetNpcLife(nNpcId, nLife, 1)--set luon mau' nguyen thuy~
	end
	if(nReplenish~=nil) then	
		SetNpcReplenish(nNpcId, nReplenish, 1)
	end
	if(nAttackR~=nil) then	
		SetNpcAR(nNpcId, nAttackR, 1)
	end
	if(nDefend~=nil) then	
		SetNpcDefense(nNpcId, nDefend, 1)
	end
	if(nMinDamage~=nil and nMaxDamage~=nil) then	
		SetNpcDamage(nNpcId, nMinDamage, nMaxDamage)
	end
	if(nReviveTime~=nil) then	
		SetNpcRevTime(nNpcId, nReviveTime)
	end
	if(nHitRecover~=nil) then	
		SetNpcHitRecover(nNpcId, nHitRecover, 1)
	end
	if(nBossType~=nil) then
    	--SetNpcBoss2(nNpcId, nBossType)
    end	
	if(szDropScript~=nil) then	
		SetNpcDropScript(nNpcId, szDropScript)
	end
	if(szScript~=nil) then
    	SetNpcScript(nNpcId, szScript)
    end
	return nNpcId
end

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
	mapindex = SubWorldID2Idx(nMap)
	local nX,nY = nBX,nBY
	for i=0,nDistance do
		AddTrap(mapindex, (nX + i)*32, (nY - i)*32, szScript)
	end
	nX = nX - 1
	for i=0,nDistance do
		AddTrap(mapindex, (nX + i)*32, (nY - i)*32, szScript)
	end
end

function AddTrapEx2(nMap, nBX, nBY, nDistance, szScript)
	mapindex = SubWorldID2Idx(nMap)
	local nX,nY = nBX,nBY
	for i=0,nDistance do
		AddTrap(mapindex, (nX + i)*32, (nY + i)*32, szScript)
	end
	nY = nY - 1
	for i=0,nDistance do
		AddTrap(mapindex, (nX + i)*32, (nY + i)*32, szScript)
	end
end

function AddTrapEx3(nMap, nBX, nBY, nDistance, szScript)
	mapindex = SubWorldID2Idx(nMap)
	local nX,nY = nBX,nBY;
	for i=0,nDistance do
		AddTrap(mapindex, nX*32, (nY + i)*32, szScript)
	end
	nX = nX - 1;
	for i=0,nDistance do
		AddTrap(mapindex, nX*32, (nY + i)*32, szScript)
	end
end

function AddTrapEx4(nMap, nBX, nBY, nDistance, szScript)
	mapindex = SubWorldID2Idx(nMap)
	local nX,nY = nBX,nBY
	for i=0,nDistance do
		AddTrap(mapindex, (nX + i)*32, nY*32, szScript)
	end
	nY = nY - 1
	for i=0,nDistance do
		AddTrap(mapindex, (nX + i)*32, nY*32, szScript)
	end
end

function AddTrapEx5(nMap, nBX, nBY, szScript)
	mapindex = SubWorldID2Idx(nMap)
	local nX,nY = nBX,nBY
	AddTrap(mapindex, nX, nY, szScript)
end

function AddObstacleEx1(nMap, nBX, nBY, nDistance, value)
	mapindex = SubWorldID2Idx(nMap)
	local nX,nY = nBX,nBY
	for i=0,nDistance do
		AddObstacle(mapindex, (nX + i)*32, (nY - i)*32, value)
	end
	nX = nX - 1
	for i=0,nDistance do
		AddObstacle(mapindex, (nX + i)*32, (nY - i)*32, value)
	end
end

TAB_MAPNAME = { --IdMap--MapName--nX--nY
	{1,  "Ph­îng T­êng",  1528,  2733},
	{2,  "Hoa S¬n",  2287,  4092},
	{3,  "T©y B¾c KiÕm C¸c Thôc §¹o",  932,  4076},
	{4,  "KiÕm C¸c Thôc §¹o Kim Quang ®éng",  1797,  3058},
	{5,  "Kinh Hoµng ®éng KiÕm C¸c Thôc §¹o",  1822,  3473},
	{6,  "KiÕm C¸c Thôc §¹o Táa V©n ®éng",  1384,  3286},
	{7,  "§Þa BiÓu TÇn L¨ng",  2417,  2631},
	{8,  "TÇng 1 TÇn L¨ng",  1603,  3497},
	{9,  "Tr­êng Giang Nguyªn §Çu",  2478,  5691},
	{10,  "Nh¹n Th¹ch ®éng",  1603,  3209},
	{11,  "Thµnh §«",  3100,  4822},
	{12,  "§Þa ®¹o hËu viÖn TÝn T­íng tù",  1589,  3188},
	{13,  "Nga My ph¸i",  1867,  5021},
	{14,  "M·nh Hæ ®éng",  1589,  3217},
	{15,  "Gi¶ng Kinh §­êng",  1588,  3200},
	{16,  "Nga Mi TiÒn ®iÖn",  1595,  3180},
	{17,  "Nga Mi ChÝnh ®iÖn",  1592,  3211},
	{18,  "Nga Mi Ph¸i T¶ Thiªn §iÖn",  1608,  3191},
	{19,  "T©y Nam KiÕm C¸c Thôc §¹o",  3486,  4070},
	{20,  "Giang T©n Th«n",  3701,  6232},
	{21,  "Thanh Thµnh S¬n",  2247,  3923},
	{22,  "B¹ch V©n ®éng",  1752,  3372},
	{23,  "ThÇn Tiªn ®éng",  1710,  3077},
	{24,  "H­ëng Thñy ®éng",  1692,  3189},
	{25,  "§­êng M«n",  3532,  5770},
	{26,  "Tróc T¬ ®éng tÇng 1",  1603,  3210},
	{27,  "Tróc T¬ ®éng tÇng 2",  1607,  3208},
	{28,  "Tróc T¬ ®éng tÇng 3",  1551,  3192},
	{29,  "§­êng M«n ¸m KhÝ Phßng",  1604,  3201},
	{30,  "§­êng M«n Háa KhÝ Phßng",  1597,  3205},
	{31,  "§­êng M«n Vâ C«ng Phßng",  1610,  3209},
	{32,  "§­êng M«n Y D­îc Phßng",  1595,  3205},
	{33,  "§­êng M«n §¹i S¶nh",  1596,  3213},
	{34,  "§­êng M«n NghÞ Sù phßng",  1599,  3211},
	{35,  "§­êng M«n ChÝnh Phßng",  1598,  3208},
	{36,  "§­êng M«n TÕ Tæ ®­êng",  1592,  3193},
	{37,  "BiÖn Kinh",  2081,  2477},
	{38,  "TÇng 1 Mª cung ThiÕt th¸p ",  1702,  3134},
	{39,  "TÇng 2 Mª cung ThiÕt th¸p",  1690,  3032},
	{40,  "TÇng 3 Mª cung ThiÕt th¸p",  1668,  3133},
	{41,  "Phôc Ng­u S¬n T©y",  2078,  2804},
	{42,  "Thiªn T©m ®éng",  1648,  3119},
	{43,  "KiÕm C¸c Trung Nguyªn",  1673,  2713},
	{45,  "Thiªn NhÉn Gi¸o",  1559,  3246},
	{46,  "Thiªn NhÉn Gi¸o Néi ThÊt",  1611,  3201},
	{47,  "Thiªn NhÉn Gi¸o Néi ThÊt T¶ ",  1606,  3054},
	{48,  "Thiªn NhÉn Gi¸o Néi ThÊt H÷u",  1672,  3138},
	{49,  "Thiªn NhÉn Gi¸o Néi ThÊt",  1608,  3192},
	{50,  "ThÊt S¸t ®éng",  1425,  3217},
	{51,  "Thiªn NhÉn gi¸o Th¸nh ®éng",  1666,  3291},
	{52,  "Thiªn NhÉn gi¸o Th¸nh ®éng",  1617,  3261},
	{53,  "Ba L¨ng huyÖn",  1342,  3346},
	{54,  "Nam Nh¹c trÊn",  1618,  2969},
	{55,  "§µo Hoa Nguyªn",  1729,  3282},
	{56,  "Hoµnh S¬n",  1594,  3210},
	{57,  "Hoµnh S¬n ®¹i ®×nh",  1586,  3202},
	{58,  "§éc C« KiÕm Ngäa thÊt",  1608,  3192},
	{59,  "Thiªn V­¬ng Bang",  1425,  3472},
	{60,  "Thiªn V­¬ng Bang ",  1594,  3193},
	{61,  "Thiªn V­¬ng §¹i ®iÖn",  1591,  3204},
	{62,  "Thiªn V­¬ng §¹i ®iÖn",  1611,  3202},
	{65,  "Thiªn V­¬ng ®¶o s¬n ®éng",  1596,  3250},
	{66,  "D­íi ®¸y §éng §×nh Hå ",  1596,  3199},
	{67,  "D­íi ®¸y §éng §×nh Hå ",  1591,  3193},
	{68,  "Thanh Loa ®¶o",  1545,  2972},
	{69,  "Thanh Loa ®¶o s¬n ®éng",  1600,  3207},
	{70,  "Vò L¨ng s¬n",  1649,  3050},
	{71,  "B¹ch Thñy ®éng",  1739,  3208},
	{72,  "§¹i Tï ®éng",  1611,  2969},
	{73,  "Phôc L­u ®éng",  1599,  3210},
	{74,  "ThÇn N«ng gi¸ ",  1947,  3120},
	{75,  "Kho¶ Lang ®éng",  1872,  3071},
	{76,  "S¬n B¶o ®éng",  1796,  2959},
	{77,  "YÕn Tö ®éng",  1549,  3169},
	{78,  "T­¬ng D­¬ng",  1299,  3483},
	{79,  "T­¬ng D­¬ng mËt ®¹o",  1599,  3206},
	{80,  "D­¬ng Ch©u",  1345,  3359},
	{81,  "Vâ §ang ph¸i",  1620,  3595},
	{82,  "Mª cung ®¸y giÕng Vâ §ang ph¸i",  1593,  3193},
	{83,  "Vâ §ang ph¸i Háa Lang ®éng",  1584,  3248},
	{84,  "Tö Tiªu §¹i §iÖn Vâ §ang ph¸i",  1590,  3193},
	{85,  "ThËp Ph­¬ng §iÖn Vâ §ang ph¸i",  1597,  3192},
	{86,  "Phô MÉu §iÖn Vâ §ang ph¸i ",  1606,  3191},
	{87,  "Long Hæ §iÖn Vâ §ang ph¸i",  1596,  3190},
	{90,  "Phôc Ng­u S¬n §«ng",  1649,  3567},
	{91,  "Kª Qu¸n ®éng",  1806,  2858},
	{92,  "Thôc C­¬ng s¬n",  2025,  3040},
	{93,  "TiÕn Cóc ®éng",  1526,  3172},
	{94,  "Linh Cèc ®éng",  1751,  3107},
	{96,  "T©n Thñ kh¸ch c¬",  1583,  3217},
	{98,  "Thanh khª ®éng",  1526,  2958},
	{99,  "VÜnh L¹c trÊn",  1849,  2835},
	{100,  "Chu Tiªn trÊn",  1852,  3356},
	{101,  "§¹o H­¬ng th«n",  1767,  2816},
	{102,  "Th¸nh ®éng tÇng 2",  1608,  3199},
	{103,  "ThiÕu L©m ph¸i",  1622,  3565},
	{104,  "Tµng Kinh C¸c",  1592,  3190},
	{105,  "§¹t Ma §­êng",  1601,  3190},
	{106,  "§¹i Hïng b¶o ®iÖn",  1591,  3204},
	{107,  "phßng §Ö tö 1",  1593,  3178},
	{108,  "phßng §Ö tö 2",  1596,  3182},
	{109,  "Ph­¬ng tr­îng ThiÒn phßng",  1596,  3185},
	{110,  "La H¸n §­êng",  1606,  3199},
	{111,  "Méc Nh©n H¹ng",  1580,  3417},
	{112,  "Thiªn PhËt ®iÖn",  1601,  3193},
	{113,  "MËt thÊt ThiÕu L©m",  1675,  3361},
	{114,  "108 La H¸n trËn",  1499,  3312},
	{115,  "C¸i Bang",  1538,  4021},
	{116,  "TÇng 1 lßng ®Êt",  1787,  2980},
	{117,  "TÇng 2 lßng ®Êt",  1660,  2947},
	{118,  "TÇng 3 lßng ®Êt",  1525,  2962},
	{119,  "TÇng 4 lßng ®Êt",  1569,  3099},
	{120,  "TÇng 5 lßng ®Êt",  1815,  3195},
	{121,  "Long M«n trÊn",  1715,  4394},
	{122,  "Hoµng Hµ Nguyªn §Çu",  1612,  3328},
	{123,  "L·o Hæ ®éng",  1698,  3363},
	{124,  "C¸n Viªn ®éng",  1672,  3420},
	{125,  "L­u Tiªn ®éng tÇng 1",  1811,  3219},
	{126,  "L­u Tiªn ®éng tÇng 2",  1496,  2929},
	{127,  "L­u Tiªn ®éng tÇng 3",  1769,  3216},
	{128,  "L­u Tiªn ®éng tÇng 4",  1515,  3123},
	{129,  "L­u Tiªn ®éng tÇng 5",  1597,  3212},
	{130,  "L­u Tiªn ®éng tÇng 6",  1547,  3143},
	{131,  "C«n L«n ph¸i",  1872,  3659},
	{132,  "B¨ng HuyÖt ®éng ",  1701,  3407},
	{135,  "KiÕn TÝnh Phong s¬n ®éng",  1748,  3198},
	{136,  "Kho¸i Ho¹t L©m",  1716,  3144},
	{137,  "Thiªn Vi ®iÖn",  1603,  3202},
	{138,  "C«n L«n ph¸i ChÝnh ®iÖn",  1604,  3203},
	{139,  "Tö Vi ®iÖn",  1612,  3207},
	{140,  "D­îc V­¬ng Cèc",  2429,  3731},
	{141,  "D­îc V­¬ng ®éng tÇng 1",  1603,  3372},
	{142,  "D­îc V­¬ng ®éng tÇng 2",  1467,  3288},
	{143,  "D­îc V­¬ng ®éng tÇng 3",  1602,  3215},
	{144,  "D­îc V­¬ng ®éng tÇng 4",  1690,  3023},
	{145,  "TuyÕt B¸o ®éng tÇng 1",  1563,  3245},
	{146,  "TuyÕt B¸o ®éng tÇng 2",  1630,  3325},
	{147,  "TuyÕt B¸o ®éng tÇng 3",  1630,  3323},
	{148,  "TuyÕt B¸o ®éng tÇng 4",  1527,  3273},
	{149,  "TuyÕt B¸o ®éng tÇng 5",  1611,  3220},
	{150,  "TuyÕt B¸o ®éng tÇng 6",  1613,  3184},
	{151,  "TuyÕt B¸o ®éng tÇng 7",  1612,  3218},
	{152,  "TuyÕt B¸o ®éng tÇng 8",  1675,  3365},
	{153,  "Th¹ch Cæ trÊn",  1439,  3042},
	{154,  "Thóy Yªn m«n",  628,  1261},
	{155,  "Thóy Yªn M«n B¸ch Hoa ®×nh",  1585,  3207},
	{156,  "Thóy Yªn M«n phßng §Ö tö 1",  1599,  3203},
	{157,  "Thóy Yªn M«n phßng §Ö tö 2",  1587,  3221},
	{158,  "Thóy Yªn M«n CÊm §Þa mª cung",  1584,  3191},
	{160,  "Thóy Yªn M«n Thñ Ngäc s¶nh",  1596,  3208},
	{161,  "Thóy Yªn M«n Chñ phßng",  1600,  3202},
	{162,  "§¹i Lý",  1642,  2935},
	{163,  "§Þa ®¹o ¸c b¸",  1559,  3201},
	{164,  "Thiªn TÇm th¸p tÇng 1",  1606,  3194},
	{165,  "Thiªn TÇm th¸p tÇng 2",  1501,  3295},
	{166,  "Thiªn TÇm th¸p tÇng 3",  1662,  3245},
	{167,  "§iÓm Th­¬ng s¬n",  1214,  2731},
	{168,  "Phông Nh·n ®éng",  1508,  3005},
	{169,  "Long Nh·n ®éng",  1596,  3212},
	{170,  "Thæ PhØ ®éng",  1605,  3191},
	{171,  "§iÓm Th­¬ng ®éng tÇng 1",  1532,  3130},
	{172,  "§iÓm Th­¬ng ®éng tÇng 2",  1586,  3223},
	{173,  "§iÓm Th­¬ng ®éng tÇng 3",  1353,  3102},
	{174,  "Long TuyÒn th«n",  1818,  3017},
	{175,  "T©y S¬n th«n",  1595,  3240},
	{176,  "L©m An",  1614,  2561},
	{177,  "Ngù Hoa viªn",  1619,  3222},
	{178,  "S¬n ®éng Ngù Hoa viªn",  1609,  3200},
	{179,  "La Tiªu s¬n",  1618,  3292},
	{180,  "Long Cung ®éng",  1509,  3106},
	{181,  "L­ìng Thñy ®éng",  1590,  3199},
	{182,  "NghiÖt Long ®éng",  1997,  3067},
	{183,  "Ngò §éc Gi¸o",  1371,  3677},
	{184,  "XÝch YÕt tr¹i",  1590,  3208},
	{185,  "Kim Xµ tr¹i",  1612,  3204},
	{186,  "MÆc Thï tr¹i",  1613,  3210},
	{187,  "Thanh Ng« tr¹i",  1605,  3200},
	{188,  "Ng©n ThiÒm tr¹i",  1587,  3205},
	{189,  "Tæng tr¹i",  1581,  3194},
	{190,  "Ngò §éc Gi¸o Mª cung nhËp m«n",  1380,  3003},
	{193,  "Vò Di s¬n",  1937,  2851},
	{194,  "Ngäc Hoa ®éng",  1599,  3199},
	{195,  "Nh¹n §·ng s¬n",  682,  2863},
	{196,  "D­¬ng Gi¸c ®éng",  1691,  3068},
	{198,  "Thanh khª ®éng",  1522,  2955},
	{199,  "Vò L¨ng ®éng",  1644,  2906},
	{200,  "Cæ D­¬ng ®éng",  1612,  3171},
	{201,  "B¨ng Hµ ®éng",  1599,  3197},
	{202,  "Phï Dung ®éng",  1786,  2823},
	{203,  "V« Danh ®éng",  1551,  2992},
	{204,  "Phi Thiªn ®éng",  1679,  3391},
	{205,  "D­¬ng Trung ®éng",  1698,  3408},
	{206,  "TÇn L¨ng tÇng 2",  1790,  3265},
	{207,  "TÇn L¨ng tÇng 3",  1595,  3205},
	{224,  "Sa m¹c §Þa biÓu",  1591,  3013},
	{225,  "S¬n ®éng Sa m¹c tÇng 1",  1476,  3274},
	{226,  "S¬n ®éng Sa m¹c tÇng 2",  1553,  3173},
	{227,  "S¬n ®éng Sa m¹c tÇng 3",  1530,  3270},
	{231,  "N¬i ë Th­¬ng L­¬ng kh¸ch",  1611,  3193},
	{319,  "L©m Du Quan",  2031,  3286},
	{320,  "Ch©n nói Tr­êng B¹ch",  1386,  2253},
	{321,  "Tr­êng B¹ch s¬n Nam ",  966,  2296},
	{322,  "Tr­êng B¹ch s¬n B¾c",  2048,  4120},
	{336,  "Phong L¨ng §é",  1853,  3446},
	{340,  "M¹c Cao QuËt",  1853,  3446},
	{995,  "KiÕm M«n Quan",  1652,  3364},
}

TAB_MAPNOFORTDP = 
{
	17,
	24,	
	25,	
	26,		
	30,
	31,
	32,
	33,
	34,
	38,
	51,	
	52,
	69,
	72,
	73,
	74,
}

TAB_MAPNOFORTHP = 
{
	17,
	24,	
	25,	
	26,	
	30,
	31,
	32,
	33,
	34,
	38,
--	51,	
	52,
	69,
	72,
	73,
	74,
}

TAB_MAPNOFORTKCT = 
{
	17,
	24,	
	25,	
	26,	
	30,
	31,
	32,
	33,
	34,
	38,
--	51,	
	52,
	69,
	72,
	73,
	74,
}

function GetMapName(nMap)
	local mapid = nMap
	for i = 1, getn(TAB_MAPNAME) do
		if (mapid == TAB_MAPNAME[i][1]) then
			return TAB_MAPNAME[i][2]
		end
	end
	return format("%d",nMap)
end

function CheckMapNoForTDP(nMap)
	for i = 1, getn(TAB_MAPNOFORTDP) do
		if (nMap == TAB_MAPNOFORTDP[i]) then
		return 1
		end
	end
	return 0
end

function CheckMapNoForTHP(nMap)
	for i = 1, getn(TAB_MAPNOFORTHP) do
		if (nMap == TAB_MAPNOFORTHP[i]) then
		return 1
		end
	end	
	return 0
end

function CheckMapNoForTKCT(nMap)		-- tong kim chieu thu
	for i = 1, getn(TAB_MAPNOFORTKCT) do
		if (nMap == TAB_MAPNOFORTKCT[i]) then
		return 1
		end
	end	
	return 0
end

function AddObjEx1(nIdObj, nMap, nBX, nBY, szScript)
	local mapindex = SubWorldID2Idx(nMap)
	local idobj = nIdObj
	local nX,nY = nBX,nBY;
	AddObj(idobj, mapindex, nX, nY, szScript)
end