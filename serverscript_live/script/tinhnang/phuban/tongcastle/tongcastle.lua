Include("\\script\\header\\cauhinh_hoatdong.lua")	-- [HD CONFIG 24/08] lich / diem Thanh Bao chinh duoc
Include("\\script\\missions\\tongcastle\\game.lua")
Include("\\script\\lib\\common.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\global\\autoexec_head.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
local tbCity = {
	--[981] = {4, 37, "BiÖn Kinh"},
	[984] = {7, 176, " L©m An"},
}

TSK_STAY_TIME = 4056

TongCastle = Game:New()
TongCastle.szMapType = "Bang héi thµnh b¶o "
TongCastle.tbMapList = {}
TongCastle.GUARDLIMIT = HD_CFG("TC_TRAN_THUVE", 100)
TongCastle.tbForbitItemType = --ÖØÔØ Î¥½ûµÀ¾ß
{
	"TRANSFER", "MATE", "CALLNPC",
}

TongCastle.Player = {
	szPath = "\\script\\missions\\tongcastle\\castleplayer.lua",
	szClassName = "CastlePlayer",
}

TongCastle.tbPlayerStayTime = {}

TongCastle.tbTrapList = 
{
	["East"] =
	{
		szPosPath = "\\settings\\maps\\tongcastle\\easttrap.txt",--TrapµãµÄ×ø±êÎÄ¼þ
		szTrapFile = "\\script\\missions\\tongcastle\\trap.lua",--Trap½Å±¾µÄÂ·¾¶
	},
	["South"] = 
	{
		szPosPath = "\\settings\\maps\\tongcastle\\southtrap.txt",
		szTrapFile = "\\script\\missions\\tongcastle\\trap.lua",
	},
	["West"] = 
	{
		szPosPath = "\\settings\\maps\\tongcastle\\westtrap.txt",
		szTrapFile = "\\script\\missions\\tongcastle\\trap.lua",
	},
	["North"] = 
	{
		szPosPath = "\\settings\\maps\\tongcastle\\northtrap.txt",
		szTrapFile = "\\script\\missions\\tongcastle\\trap.lua",
	},
}

TongCastle.tbObstacleList = {
	[1] = "\\settings\\maps\\tongcastle\\eastobstacle.txt",
	[2] = "\\settings\\maps\\tongcastle\\southobstacle.txt",
	[3] = "\\settings\\maps\\tongcastle\\westobstacle.txt",
	[4] = "\\settings\\maps\\tongcastle\\northobstacle.txt",
}

TongCastle.tbRealTrap = {
}

TongCastle.tbTrapId = {
}

TongCastle.tbObjFlag = {
}

TongCastle.tbTrap = {
	nCount = 0,
}

TongCastle.tbCamp = {
}

TongCastle.tbFriendNpcList = {
}

TongCastle.tbTreePoint = {
	
}

TongCastle.tbPlayerList = {
}

TongCastle.tbSzDir2Id = {
	["East"] = 1,
	["South"] = 2,
	["West"] = 3,
	["North"] = 4,
}

TongCastle.tbDirName = {
	[1] = "phÝa §«ng",
	[2] = "phÝa Nam",
	[3] = "phÝa T©y",
	[4] = "phÝa B¾c",
	}

TongCastle.tbNpcParam2Type = {
	[1] = "Thanh §ång Th­îng Cæ ThÇn Thô", 
	[2] = "B¹ch §ång Th­îng Cæ ThÇn Thô", 
	[3] = "Hoµng Kim Th­îng Cæ ThÇn Thô",
	[4] = "B¨ng §«ng Thñ VÖ",
	[5] = "HuyÔn Vùng Thñ VÖ",
	[6] = "Tõ Hµng Thñ VÖ",
	[7] = "Háa T­êng Thñ VÖ",
}

TongCastle.tbNpcInfoList = {
}

TongCastle.tbRoomCondition = {
	["personroom"] 	 = {
		szCName = "Nh©n Tù Phßng",
		tbOpenDay = HD_TC_Ngay(1),
		tbStartTime = HD_CFG("TC_GIO_MO", {17,00}),
		tbEndTime = HD_CFG("TC_GIO_DONG", {19,00}),
	},
	["edgechannel"] = {
	},
	["earthchannel"] = {
		nNpcParam = 1,
	},
	["earthroom"] = {
		szCName = "§Þa Tù Phßng",
		tbOpenDay = HD_TC_Ngay(1),
		tbStartTime = HD_CFG("TC_GIO_CAY2", {18,00}),
		tbEndTime = HD_CFG("TC_GIO_DONG", {19,00}),
	},
	["skyroom"] = {
		szCName = "Thiªn Tù Phßng",
		tbOpenDay = HD_TC_Ngay(1),
		tbStartTime = HD_CFG("TC_GIO_CAY3", {18,30}),
		tbEndTime = HD_CFG("TC_GIO_DONG", {19,00}),
		nNpcParam = 2,
		nIsAllDir = 1,
	},
	["castle"] = {
		tbOpenDay = HD_TC_Ngay(176),
		tbStartTime = HD_CFG("TC_GIO_MO", {17,00}),
		tbEndTime = HD_CFG("TC_GIO_DONG", {19,00}),
	},
}

TongCastle.tbNpcTypeList = --npcµÄÀàÐÍ
{
	["Thanh §ång Th­îng Cæ ThÇn Thô"] = 
	{
		nNpcId = 1912,
		szName = "Thanh §ång Th­îng Cæ ThÇn Thô",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {1,},
		nTime = 1,
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
		szPosPath = "\\settings\\maps\\tongcastle\\bronzetree.txt",
	},
	["B¹ch §ång Th­îng Cæ ThÇn Thô"] = 
	{
		nNpcId = 1913,
		szName = "B¹ch §ång Th­îng Cæ ThÇn Thô",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {2,},
		nTime = 1,
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
		szPosPath = "\\settings\\maps\\tongcastle\\silvertree.txt",
	},
	["Hoµng Kim Th­îng Cæ ThÇn Thô"] = 
	{
		nNpcId = 1914,
		szName = "Hoµng Kim Th­îng Cæ ThÇn Thô",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {3,},
		nTime = 1,
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
		szPosPath = "\\settings\\maps\\tongcastle\\goldtree.txt",
	},
	["B¨ng §«ng Thñ VÖ"] = 
	{
		nNpcId = 2031,	-- [TONGCASTLE 23/08] id Linux 1908 bi du an dung cho boss khac - template moi chep tu Linux
		szName = "B¨ng §«ng Thñ VÖ",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {4,},
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
	},
	["HuyÔn Vùng Thñ VÖ"] = 
	{
		nNpcId = 2032,	-- [TONGCASTLE 23/08] id Linux 1909 bi du an dung cho boss khac - template moi chep tu Linux
		szName = "HuyÔn Vùng Thñ VÖ",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {5,},
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
	},
	["Tõ Hµng Thñ VÖ"] = 
	{
		nNpcId = 2033,	-- [TONGCASTLE 23/08] id Linux 1910 bi du an dung cho boss khac - template moi chep tu Linux
		szName = "Tõ Hµng Thñ VÖ",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {6,},
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
	},
	["Háa T­êng Thñ VÖ"] = 
	{
		nNpcId = 2034,	-- [TONGCASTLE 23/08] id Linux 1911 bi du an dung cho boss khac - template moi chep tu Linux
		szName = "Háa T­êng Thñ VÖ",
		nLevel = 90,
		nIsboss = 1,
		bNoRevive = 1,
		tbNpcParam = {7,},
		szScriptPath = "\\script\\missions\\tongcastle\\treedeath.lua",
	},
}

function TongCastle:Init()
	for key, value in pairs(self.tbNpcTypeList) do
			if value.szPosPath then
				self:LoadNpcList(key, value.szPosPath)
			end
	end
	for nMapId, bFlag in pairs(self.tbMapList) do
		if bFlag then
			self.tbCamp[nMapId] = {}
			self.tbCamp[nMapId].nCount = 1
			self.tbTrapId[nMapId] = {}
			self.tbTrapId[nMapId].nCount = 0
			self.tbTrap[nMapId] = {}
			self.tbTrap[nMapId].nCount = 0
			self.tbObjFlag[nMapId] = 0
			self.tbFriendNpcList[nMapId] = {}
			self.tbFriendNpcList[nMapId].tbInfo = {}
			self.tbFriendNpcList[nMapId].nGuard = 0
		end
	end
	self:AddTimer()
	self.tbKillCount = {}
end

function TongCastle:GetDir(szDir)
	return self.tbSzDir2Id[szDir]
end

function TongCastle:GetRoomOpenTime(szRoomName)
	local tbOpenDay = self.tbRoomCondition[szRoomName].tbOpenDay
	if not tbOpenDay then
		return
	end
	
	local nWeekDay = tonumber(GetLocalDate("%w"))
	local nLastWeekDay = nWeekDay - 1
	if nLastWeekDay == -1 then
		nLastWeekDay = 6
	end
	
	if (not tbOpenDay[nWeekDay]) and (not tbOpenDay[nLastWeekDay]) then
		return
	end
	
	local nCurTime = tonumber(GetLocalDate("%H"))*60 + tonumber(GetLocalDate("%M"))
	local tbStartTime = self.tbRoomCondition[szRoomName].tbStartTime
	local tbEndTime = self.tbRoomCondition[szRoomName].tbEndTime
	local nStartTime = tbStartTime[1]*60 + tbStartTime[2]
	local nEndTime = tbEndTime[1]*60 + tbEndTime[2]
	
	if tbOpenDay[nLastWeekDay] and (nCurTime < nEndTime - 1440) then
		return nStartTime-1440, nEndTime-1440
	end
	
	if tbOpenDay[nWeekDay] then
		return nStartTime, nEndTime
	end
end

function TongCastle:IsInOpenTime(szRoomName)
	local nCurTime = tonumber(GetLocalDate("%H"))*60 + tonumber(GetLocalDate("%M"))
	local nStartTime,nEndTime = self:GetRoomOpenTime(szRoomName)
	if nStartTime and nEndTime then
		return (nStartTime <= nCurTime) and (nCurTime < nEndTime), nStartTime, nEndTime
	end
end

function TongCastle:IsInOpenDay(szRoomName, nCityMapId)
	local nWeekDay = tonumber(GetLocalDate("%w"))
	local tbOpenDay = self.tbRoomCondition[szRoomName].tbOpenDay
	
	if tbOpenDay[nWeekDay] == nCityMapId then
		return 1
	end
end

function TongCastle:GetTrapIdByName(nMapId, szTrapName)
	local nOp, nEd = strfind(szTrapName, "null", 1)
	if nOp ~= nil then
		return -1
	end
	if self.tbTrapId[nMapId][szTrapName] == nil then
		self.tbTrapId[nMapId][szTrapName] = self.tbTrapId[nMapId].nCount + 1
		self.tbTrapId[nMapId].nCount = self.tbTrapId[nMapId].nCount + 1
	end
	return self.tbTrapId[nMapId][szTrapName]
end

function TongCastle:GetRoomNameByTrapName(szTrapName)
	for szRoomName, _ in pairs(self.tbRoomCondition) do
		local nPos, _ = strfind(szTrapName, szRoomName)
		if nPos then
			return szRoomName
		end
	end
	return ""
end

function TongCastle:AddATrap(szPrefix, szPosPath, szTrapFile)
	if (TabFile_Load(szPosPath, szPosPath) == 0) then
		print("Load TabFile Error!"..szPosPath)
		return
	end
	
	local nRowCount = TabFile_GetRowCount(szPosPath)
	--[LUA54] truoc la 'for nRow = 1, nRowCount do': than vong tu nhay nRow qua het mot ban ghi.
	--Lua 4 cho gan lai bien dieu khien for, Lua 5.4 KHONG -> phai viet thanh while.
	local nRow = 1
	while nRow <= nRowCount do
		local szCurTrapName = format("%s*%s", szPrefix, TabFile_GetCell(szPosPath, nRow, 2))
		local szNextTrapName = format("%s*%s", szPrefix, TabFile_GetCell(szPosPath, nRow+1, 2))
		local nIsTrans = tonumber(TabFile_GetCell(szPosPath, nRow+2, 2))
		local nNpcParam = tonumber(TabFile_GetCell(szPosPath, nRow+3, 2))
		local nTrapCount = tonumber(TabFile_GetCell(szPosPath, nRow+4, 2))
		local nTransCount = tonumber(TabFile_GetCell(szPosPath, nRow+5, 2))
		for nMapId, bFlag in pairs(self.tbMapList) do
			if bFlag and SubWorldID2Idx(nMapId) >= 0 then
				local nCurTrapId = self:GetTrapIdByName(nMapId, szCurTrapName)
				local nNextTrapId = self:GetTrapIdByName(nMapId, szNextTrapName)
				self.tbTrap[nMapId][nCurTrapId] = {}
				self.tbTrap[nMapId][nCurTrapId].szName = szCurTrapName
				self.tbTrap[nMapId][nCurTrapId].szNextName = szNextTrapName
				self.tbTrap[nMapId][nCurTrapId].nNextId = nNextTrapId
				self.tbTrap[nMapId][nCurTrapId].nIsTrans = nIsTrans
				self.tbTrap[nMapId][nCurTrapId].nCount = nTrapCount
				self.tbTrap[nMapId][nCurTrapId].nDir = self:GetDir(szPrefix)
				self.tbTrap[nMapId][nCurTrapId].tbList = {}
				self.tbTrap[nMapId][nCurTrapId].tbTransList = {}
				for i = nRow+6, nRow+6+nTrapCount-1 do
					local nX = tonumber(TabFile_GetCell(szPosPath, i, 1))
					local nY = tonumber(TabFile_GetCell(szPosPath, i, 2))
					tinsert(self.tbTrap[nMapId][nCurTrapId].tbList, {nX, nY})
					AddMapTrap(nMapId, nX, nY, szTrapFile, nCurTrapId)
				end
				for i = nRow+6+nTrapCount, nRow+6+nTrapCount+nTransCount-1 do
					local nTransX = tonumber(TabFile_GetCell(szPosPath, i, 1))
					local nTransY = tonumber(TabFile_GetCell(szPosPath, i, 2))
					tinsert(self.tbTrap[nMapId][nCurTrapId].tbTransList, {nTransX, nTransY})
				end
			end
		end
		nRow = nRow+6+nTrapCount+nTransCount-1
		nRow = nRow + 1	--[LUA54] buoc tang cua vong for cu
	end
end

function TongCastle:CheckTrapCondition(nMapId, nCurTrapId)
	local nTmpCamp = GetTmpCamp()
	if nTmpCamp == 1 then
		Msg2Player("<color=yellow>thµnh viªn bang chiÕm thµnh, h¹n chÕ kh«ng truyÒn tèng <color>")
		return 1
	end
	
	local szCurTrapName  = self.tbTrap[nMapId][nCurTrapId].szName
	local szCurRoomName  = self:GetRoomNameByTrapName(szCurTrapName)
	local szNextTrapName = self.tbTrap[nMapId][nCurTrapId].szNextName
	local szNextRooName  = self:GetRoomNameByTrapName(szNextTrapName)
	local nDir 			 = self.tbTrap[nMapId][nCurTrapId].nDir
	
	local tbNextRoom = self.tbRoomCondition[szNextRooName]
	if tbNextRoom then
		local bOpen,_,_ = self:IsInOpenTime(szNextRooName)
		if not bOpen and tbNextRoom.tbStartTime then
			local szTimeUpMsg = format("<color=yellow>%s tõ %.2d:%.2d ®Õn %.2d:%.2d më, c¸c thêi gian kh¸c kh«ng ®­îc vµo. <color>",
																	tbNextRoom.szCName, tbNextRoom.tbStartTime[1], tbNextRoom.tbStartTime[2], tbNextRoom.tbEndTime[1], tbNextRoom.tbEndTime[2])
			Msg2Player(szTimeUpMsg)
			return
		end
		
		local _,_, nMapIndex = GetPos()
		local nMapId = SubWorldIdx2ID(nMapIndex)
		local nNpcParam = tbNextRoom.nNpcParam
		local nIsAllDir = tbNextRoom.nIsAllDir
		if nNpcParam then
			local nNpcCount = 0
			if nIsAllDir == 1 then
				for i = 1, 4 do
					local szKey = format("nNpcParam%dnDir%d", nNpcParam, i)
					if self.tbFriendNpcList[nMapId][szKey] and self.tbFriendNpcList[nMapId][szKey] > 0 then
						nNpcCount = nNpcCount + self.tbFriendNpcList[nMapId][szKey]
					end
				end
			else
				local szKey = format("nNpcParam%dnDir%d", nNpcParam, nDir)
				if self.tbFriendNpcList[nMapId][szKey] and self.tbFriendNpcList[nMapId][szKey] > 0 then
						nNpcCount = nNpcCount + self.tbFriendNpcList[nMapId][szKey]
				end
			end
			if nNpcCount > 0 then
				if strfind(szNextRooName, "earthchannel") then
					Msg2Player("<color=yellow>thÇn thñ Nh©n Tù Phßng phÝa nµy ch­a ®èn xong, kh«ng thÓ tiÕn vµo §Þa Tù Phßng <color>")
				elseif strfind(szNextRooName, "skyroom") then
					Msg2Player("<color=yellow>Bèn ThÇn Thô cña §Þa Tù Phßng ch­a ®èn hÕt, kh«ng thÓ di vµo Thiªn Tù Phßng.<color>")
				end
				return
			else
--				Msg2Player("<color=yellow>npcËÀ¹â£¬¿ÉÒÔ´«ËÍ<color>")
				return 1
			end
		end
			
		return 1
	end
	return
end

function TongCastle:TestGetRoom()
	local _,_, nMapIndex = GetPos()
	local nMapId = SubWorldIdx2ID(nMapIndex)
	local szPlayerName = GetName()
	Msg2Player(self.tbPlayerList[nMapId][szPlayerName].szCurName)
end

function TongCastle:BroadcastTreeDeath(nMapId, nNpcParam, nDir)
	local nNpcCount = 0
	local szMsg = ""
	if nNpcParam == 1 then
		local szKey = format("nNpcParam%dnDir%d", nNpcParam, nDir)
		if self.tbFriendNpcList[nMapId][szKey] and self.tbFriendNpcList[nMapId][szKey] > 0 then
			nNpcCount = nNpcCount + self.tbFriendNpcList[nMapId][szKey]
		end
		if nNpcCount == 0 then
			szMsg = format("H­íng Nh©n Tù Phßng %s toµn bé ThÇn Méc bÞ ®¸nh b¹i, sau khi §Þa Tù Phßng më, h·y ®i vµo §Þa Tù Phßng.", self.tbDirName[nDir])
		else
			szMsg = format("%s h­íng Nh©n Tù Phßng toµn bé ThÇn Méc bÞ ®¸nh b¹i, tiÕp tôc ®¸nh b¹i %d ThÇn Méc h­íng %s cã thÓ sau khi §Þa Tù Phßng më h·y ®i vµo §Þa Tù Phßng.",
															self.tbDirName[nDir], nNpcCount, self.tbDirName[nDir])
		end
	elseif nNpcParam == 2 then
		for i=1, 4 do
			local szKey = format("nNpcParam%dnDir%d", nNpcParam, i)
			if self.tbFriendNpcList[nMapId][szKey] and self.tbFriendNpcList[nMapId][szKey] > 0 then
				nNpcCount = nNpcCount + self.tbFriendNpcList[nMapId][szKey]
			end
		end
		if nNpcCount == 0 then
			szMsg = "Toµn bé ThÇn Méc §Þa Tù Phßng bÞ ®¸nh b¹i, cã thÓ sau khi Thiªn Tù Phßng më h·y ®i vµo Thiªn Tù Phßng."
		else
			szMsg = format("§Þa Tù Phßng h­íng %s ThÇn Thô bÞ ®¸nh b¹i, ®¸nh b¹i ThÇn Thô cña §Þa Tù Phßng cßn d­ l¹i cã thÓ sau khi Thiªn Tù Phßng më h·y ®i vµo Thiªn Tù Phßng.", self.tbDirName[nDir])
		end
	elseif nNpcParam == 3 then
		return
	end
	Msg2Map(nMapId, szMsg)
end

function TongCastle:ExecTranAction(nMapId, nCurTrapId, nIsTrans)
	local szPlayerName = GetName()
	local szCurRooName = self:GetRoomNameByTrapName(self.tbPlayerList[nMapId][szPlayerName].szCurName)
	local szCurTrapName  = self.tbTrap[nMapId][nCurTrapId].szName
	local szRoomName  = self:GetRoomNameByTrapName(szCurTrapName)
	local szNextTrapName = self.tbTrap[nMapId][nCurTrapId].szNextName
	local szNextRoomName  = self:GetRoomNameByTrapName(szNextTrapName)
	local nNextTrapId = self.tbTrap[nMapId][nCurTrapId].nNextId
	local nDir 	= self.tbTrap[nMapId][nCurTrapId].nDir
	
	if nIsTrans == 1 then
		self.tbPlayerList[nMapId][szPlayerName].nDir = nDir
		self.tbPlayerList[nMapId][szPlayerName].szCurName = szNextTrapName	
	else
		if szCurRoomName ~= szRoomName then
			self.tbPlayerList[nMapId][szPlayerName].nDir = nDir
			self.tbPlayerList[nMapId][szPlayerName].szCurName = szCurTrapName	
		end
	end
	
	if nIsTrans == 1 then
		local nRandomCount = getn(self.tbTrap[nMapId][nCurTrapId].tbTransList)
		if nRandomCount and nRandomCount > 0 then
			local nRandomTrapId = random(1, nRandomCount)
			local nX, nY = unpack(self.tbTrap[nMapId][nCurTrapId].tbTransList[nRandomTrapId]) 
			SetPos(nX/32, nY/32)
		end
	end
end 

function TongCastle:TrapMoveTo(nMapId, nCurTrapId)
	if self.tbTrap[nMapId][nCurTrapId].nIsTrans == 1 and self:CheckTrapCondition(nMapId, nCurTrapId) == 1 then
		self:ExecTranAction(nMapId, nCurTrapId, self.tbTrap[nMapId][nCurTrapId].nIsTrans)
		return
	elseif self.tbTrap[nMapId][nCurTrapId].nIsTrans == 0 then
		self:ExecTranAction(nMapId, nCurTrapId, self.tbTrap[nMapId][nCurTrapId].nIsTrans)
	end
end

function TongCastle:NotifyPlayTime(tbPlayer)
	local nCurTime = GetCurServerTime()
	local nDiff = nCurTime - tbPlayer.nTime
	if nDiff > 60 then
		tbPlayer.nTime = nCurTime - nDiff + 60
		PlayerFunLib:AddTaskDaily999(TSK_STAY_TIME, 1)
	end
	local nTime = PlayerFunLib:GetTaskDaily999(TSK_STAY_TIME)
	local _, _, _, _, _, _, nDay = MakeDateTime(7, GetCurServerTime())
	if nTime >= 15 and nDay == 0 then --ÖÜÈÕ²Î¼Ó°ï»á³Ç±¤ÔöÍ£Áô15·ÖÖÓ£¬Ôö¼Ó»îÔ¾¶È
		-- [TONGCASTLE 23/08 phan bien F22] he huoyuedu (diem hoat bat) KHONG co tren JX1 -
		-- de nguyen se spam log 'script chua nap' moi phut moi nguoi du dieu kien Chu nhat
		--DynamicExecuteByPlayer(PlayerIndex, "\\script\\huoyuedu\\huoyuedu.lua", "tbHuoYueDu:AddHuoYueDu", "tongcastle")
	end
						
	Msg2Player(format("Thêi gian tÝch lòy trong Bang Héi Thµnh B¶o cña ng­¬I lµ <color=yellow>%d phót <color>", nTime))
end

function TongCastle:OnTime()
	for nMapId, bFlag in pairs(self.tbMapList) do
		if bFlag then
			local nMapIndex = SubWorldID2Idx(nMapId)
			if nMapIndex >= 0 and self.tbPlayerList[nMapId] then
				local szCityTongName, _ = GetCityOwner(tbCity[nMapId][1])
				for szPlayerName, tbPlayer in pairs(self.tbPlayerList[nMapId]) do
					local nPlayerIndex = SearchPlayer(szPlayerName)
					if nPlayerIndex > 0 then
						CallPlayerFunction(nPlayerIndex, self.NotifyPlayTime, self, tbPlayer)
						local szTongName, _ = CallPlayerFunction(nPlayerIndex, GetTongName)
						local szRoomName = self:GetRoomNameByTrapName(self.tbPlayerList[nMapId][szPlayerName].szCurName)
						if szCityTongName ~= szTongName and self.tbRoomCondition[szRoomName] then
							local bOpen,nStartTime,nEndTime = self:IsInOpenTime(szRoomName)
							local nCurTime = tonumber(GetLocalDate("%H"))*60 + tonumber(GetLocalDate("%M"))
							if bOpen and (nEndTime - nCurTime <= 5) then
								local szTimeUpMsg = format("phßng nµy cßn %d phót sÏ ®ãng l¹i", nEndTime - nCurTime)
								CallPlayerFunction(nPlayerIndex, Msg2Player, szTimeUpMsg)
							elseif (not bOpen) and self.tbRoomCondition[szRoomName].tbStartTime then
								CallPlayerFunction(nPlayerIndex, Msg2Player, format("%s Tù Phßng thêi gian më ngµy h«m nay ®· kÕt thóc, ng­¬i bÞ truyÒn tèng ®Õn bang héi thµnh b¶o .", self.tbRoomCondition[szRoomName].szCName))
								CallPlayerFunction(nPlayerIndex, NewWorld, 176, 1661, 3271)
							end
						end
					end
				end
				
				self:UpdateObstacleObj(nMapId)
			end
		end
	end
	
	local bCastleOpened = self:IsInOpenTime("castle")
	if not bCastleOpened then
		self:TransOutPlayer()
		self.tbKillCount = {}
	end	
	return 60*18, 0
end

function TongCastle:UpdateObstacleObj(nMapId)
	local bSkyRoomOpen = self:IsInOpenTime("skyroom")
	local nNpcCount = 0
	for i = 1, 4 do
		local szKey = format("nNpcParam%dnDir%d", 2, i)
		if self.tbFriendNpcList[nMapId][szKey] and self.tbFriendNpcList[nMapId][szKey] > 0 then
			nNpcCount = nNpcCount + self.tbFriendNpcList[nMapId][szKey]
		end
	end	
	
	if bSkyRoomOpen and nNpcCount == 0 and self.tbObjFlag[nMapId] == 1 then
		self:DelAllAObstacleObj(nMapId)
	elseif nNpcCount ~= 0 and self.tbObjFlag[nMapId] == 0 then
		self:AddAllObstacleObj(nMapId)
	end
end

function TongCastle:DelAllAObstacleObj(nMapId)
	self.tbObjFlag[nMapId] = 0
	for i = 1, 4 do
		self:ClearAObstacleObj(nMapId, i)
	end
	for i = 1, getn(self.tbTrap[nMapId]) do
		if strfind(self.tbTrap[nMapId][i].szName, "skyroom1") or strfind(self.tbTrap[nMapId][i].szName, "earthroom2") then
			self.tbTrap[nMapId][i].nIsTrans = 0
		end
	end
end

function TongCastle:AddTimer()
	AddTimer(60*18, "TongCastle:OnTime", 0)
end

function TongCastle:AddAObstacle(nMapId, szPosPath)
	if (TabFile_Load(szPosPath, szPosPath) == 0) then
		print("Load TabFile Error!"..szPosPath)
		return
	end
	local nRowCount = TabFile_GetRowCount(szPosPath)
	for nRow = 2, nRowCount do
		local nX = tonumber(TabFile_GetCell(szPosPath, nRow, 1))
		local nY = tonumber(TabFile_GetCell(szPosPath, nRow, 2))
		AddObstacleObj(469, nMapId, nX, nY)
	end
end

function TongCastle:ClearAObstacleObj(nMapId, nDir)
	local szPosPath = self.tbObstacleList[nDir]
	if (TabFile_Load(szPosPath, szPosPath) == 0) then
		print("Load TabFile Error!"..szPosPath)
		return
	end
	local nRowCount = TabFile_GetRowCount(szPosPath)
	for nRow = 2, nRowCount do
		local nX = tonumber(TabFile_GetCell(szPosPath, nRow, 1))
		local nY = tonumber(TabFile_GetCell(szPosPath, nRow, 2))
		ClearObstacleObj(nMapId, nX, nY)
	end
end

function TongCastle:AddPlayer2List(szPlayerName, nMapId, szTongName)
	if self.tbPlayerList[nMapId] == nil then
		self.tbPlayerList[nMapId] = {}
	end
	local nPlayerIndex = SearchPlayer(szPlayerName)
	if nPlayerIndex > 0 then
		self.tbPlayerList[nMapId][szPlayerName] = {nDir = 0, szCurName = "", nTime = GetCurServerTime()}
		local szCityTong, _ = GetCityOwner(tbCity[nMapId][1])
		if szCityTong == szTongName then
			self.tbCamp[nMapId][szCityTong] = 1
		elseif self.tbCamp[nMapId][szTongName] == nil or 
			self.tbCamp[nMapId][szTongName] == 1 then

			self.tbCamp[nMapId][szTongName] = self.tbCamp[nMapId].nCount + 1
			self.tbCamp[nMapId].nCount = self.tbCamp[nMapId].nCount + 1
		end 
		CallPlayerFunction(nPlayerIndex, SetTmpCamp, self.tbCamp[nMapId][szTongName])
	end
end

function TongCastle:DelPlayer2List(szPlayerName, nMapId)
	local nPlayerIndex = SearchPlayer(szPlayerName)
	if nPlayerIndex > 0 then
		self.tbPlayerList[nMapId][szPlayerName] = nil
		CallPlayerFunction(nPlayerIndex, SetTmpCamp, 0)
	end
end

function TongCastle:LoadNpcList(szName, szPosPath)
	local tb = self.tbNpcTypeList[szName]
	if (TabFile_Load(szPosPath, szPosPath) == 0) then
		print("Load TabFile Error!"..szPosPath)
		return 0
	end
	if not tb then
		return 
	end
	local nRowCount = TabFile_GetRowCount(szPosPath)
	for nRow = 2, nRowCount do
		local nX = tonumber(TabFile_GetCell(szPosPath, nRow, 1))
		local nY = tonumber(TabFile_GetCell(szPosPath, nRow, 2))
		local nDir = tonumber(TabFile_GetCell(szPosPath, nRow, 3))
		for nMapId, bFlag in pairs(self.tbMapList) do
			if bFlag then	
				local tbCurNpcInfo = {
					nParam = tb.tbNpcParam[1],
					nMapId = nMapId,
					nX = nX,
					nY = nY,
					nDir = nDir,
				}
				if self.tbNpcInfoList[nMapId] == nil then
					self.tbNpcInfoList[nMapId] = {}
				end
				tinsert(self.tbNpcInfoList[nMapId], tbCurNpcInfo)
			end
		end
	end
end

function TongCastle:ClearGuardNpc(hParam)
	local nMapId = ObjBuffer:PopObject(hParam)
	if SubWorldID2Idx(nMapId) < 0 then
		return
	end
	if self.tbFriendNpcList[nMapId] then
		local tbTmp = clone(self.tbFriendNpcList[nMapId])
		for nNpcIndex, dwNpcId in pairs(tbTmp.tbInfo) do
			if GetNpcId(nNpcIndex) == dwNpcId then
				local nNpcParam = GetNpcParam(nNpcIndex, 1)
				local nDir = GetNpcParam(nNpcIndex, 2)
				if nNpcParam >= 4 then
					self:UnRegANpc(nMapId, nNpcParam, nDir, nNpcIndex, dwNpcId)
					DelNpc(nNpcIndex)
				end
			end
		end
	end
end

function TongCastle:AddAllObstacleObj(nMapId)
	self.tbObjFlag[nMapId] = 1
	for i = 1, 4 do
		self:AddAObstacle(nMapId, self.tbObstacleList[i])
	end
	for i = 1, getn(self.tbTrap[nMapId]) do
		if strfind(self.tbTrap[nMapId][i].szName, "skyroom1") or strfind(self.tbTrap[nMapId][i].szName, "earthroom2") then
			self.tbTrap[nMapId][i].nIsTrans = 1
		end
	end
end

function TongCastle:AddTreeInMap(nMapId, nTreeType)
	self:ClearAllTreeNpc(nMapId, nTreeType)
	local tbNewNpc = {}
	local nCount = getn(self.tbNpcInfoList[nMapId])

	for i = 1, nCount do
		local tbNpcInfo = self.tbNpcInfoList[nMapId][i]
		local nNpcParam = tbNpcInfo.nParam
		if (nMapId == tbNpcInfo.nMapId) and ((not nTreeType) or nNpcParam == nTreeType) then
			local nX = tbNpcInfo.nX
			local nY = tbNpcInfo.nY
			local nDir = tbNpcInfo.nDir
			local szNpcName = self.tbNpcParam2Type[nNpcParam]
			local tbNpc = self.tbNpcTypeList[szNpcName]
			local nNewNpcIndex = self:basemission_CallNpc(tbNpc, nMapId, nX, nY)
			if nNewNpcIndex > 0 then
				SetNpcParam(nNewNpcIndex, 2, nDir)
				SetTmpCamp(1, nNewNpcIndex)
				self:RegANpc(nMapId, nNpcParam, nDir, nNewNpcIndex, GetNpcId(nNewNpcIndex))
				tinsert(tbNewNpc, {nNpcParam=nNpcParam, nX=nX, nY=nY, nDir=nDir})
			end
		end
	end

	local nNewNpcCount = getn(tbNewNpc)
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, nNewNpcCount)
	ObjBuffer:PushObject(handle, nMapId)
	for i=1,nNewNpcCount do
			ObjBuffer:PushObject(handle, tbNewNpc[i].nNpcParam)
			ObjBuffer:PushObject(handle, tbNewNpc[i].nX)
			ObjBuffer:PushObject(handle, tbNewNpc[i].nY)
			ObjBuffer:PushObject(handle, tbNewNpc[i].nDir)
	end
	RemoteExecute("\\script\\mission\\tongcastle\\tongcastle.lua", "tbS3TongCastle:SaveNpcData", handle)
	OB_Release(handle)
	
	Msg2Map(nMapId, format("<color=yellow>Lµm míi %s trong Thµnh B¶o Bang Héi. <color>", self.tbNpcParam2Type[nTreeType]))
end

function TongCastle:TreeNpcRevive(hParam)--nMapId, nTreeType)
	local nTreeType = ObjBuffer:PopObject(hParam)
	local tbMap = self:GetMapListOnThisServer()
	
	for i=1,getn(tbMap) do
		local nMapId = tbMap[i]
		local City =tbCity[nMapId]
		if City and self:IsInOpenDay("castle", City[2]) then
			self:AddTreeInMap(nMapId, nTreeType)
		end
	end
end

function TongCastle:AddTreeFromRelay(nMapId, hParam)
	if (not hParam) or (OB_IsEmpty(hParam) == 1) then
		return
	end
	local nCount = ObjBuffer:PopObject(hParam)
	for i=1, nCount do
		local nNpcParam = ObjBuffer:PopObject(hParam)
		local nX = ObjBuffer:PopObject(hParam)
		local nY = ObjBuffer:PopObject(hParam)
		local nDir = ObjBuffer:PopObject(hParam)
		local szNpcName = self.tbNpcParam2Type[nNpcParam]
		local tbNpc = self.tbNpcTypeList[szNpcName]
		local nNewNpcIndex = self:basemission_CallNpc(tbNpc, nMapId, nX, nY)
		if nNewNpcIndex > 0 then
			SetNpcParam(nNewNpcIndex, 2, nDir)
			SetTmpCamp(1, nNewNpcIndex)
			self:RegANpc(nMapId, nNpcParam, nDir, nNewNpcIndex, GetNpcId(nNewNpcIndex))
		end
	end
	
	self:UpdateObstacleObj(nMapId)
end

function TongCastle:GetAndAddTreeFromRelay()
	local tbMap = self:GetMapListOnThisServer()
	local nCount = getn(tbMap)
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, nCount)
	OB_Release(handle)
	for i=1,getn(tbMap) do
		local handle = OB_Create()
		ObjBuffer:PushObject(handle, tbMap[i])
		RemoteExecute("\\script\\mission\\tongcastle\\tongcastle.lua",
									"tbS3TongCastle:SendTreeToGS", handle,
									"TongCastle:AddTreeFromRelay", tbMap[i])
		OB_Release(handle)
	end
end

function TongCastle:FindNpc(nMapId, nNpcIndex, dwNpcId)
	if self.tbFriendNpcList[nMapId] == nil then
		return
	end
	if self.tbFriendNpcList[nMapId].tbInfo == nil then
		return
	end
	if self.tbFriendNpcList[nMapId].tbInfo[nNpcIndex] ~= dwNpcId then
		return
	end
	return 1
end

function TongCastle:ClearAllTreeNpc(nMapId, nType)
	if self.tbFriendNpcList[nMapId] then
		local tbTmp = clone(self.tbFriendNpcList[nMapId])
		for nNpcIndex, dwNpcId in pairs(tbTmp.tbInfo) do
			if GetNpcId(nNpcIndex) == dwNpcId then
				local nNpcParam = GetNpcParam(nNpcIndex, 1)
				local nDir = GetNpcParam(nNpcIndex, 2)
				if (not nType) or (nNpcParam>=1 and nNpcParam <= 3 and nNpcParam == nType)then
					self:UnRegANpc(nMapId, nNpcParam, nDir, nNpcIndex, dwNpcId)
					DelNpc(nNpcIndex)
				end
			end
		end
	end
end

function TongCastle:RegANpc(nMapId, nNpcParam, nDir, nNpcIndex, dwNpcId)
	local szKey = format("nNpcParam%dnDir%d", nNpcParam, nDir)
	if self.tbFriendNpcList[nMapId] == nil then
		self.tbFriendNpcList[nMapId] = {}
		self.tbFriendNpcList[nMapId].tbInfo = {}
		self.tbFriendNpcList[nMapId].nGuard = 0
	end
	if self.tbFriendNpcList[nMapId][szKey] == nil then
		self.tbFriendNpcList[nMapId][szKey] = 0
	end
	self.tbFriendNpcList[nMapId][szKey] = self.tbFriendNpcList[nMapId][szKey] + 1
	self.tbFriendNpcList[nMapId].tbInfo[nNpcIndex] = dwNpcId
	if nNpcParam >= 4 then
		self.tbFriendNpcList[nMapId].nGuard = self.tbFriendNpcList[nMapId].nGuard + 1
	end
end

function TongCastle:GuardIsLimit(nMapId)
	if self.tbFriendNpcList[nMapId].nGuard + 1 > self.GUARDLIMIT then
		return 
	end
	return 1
end

function TongCastle:UnRegANpc(nMapId, nNpcParam, nDir, nNpcIndex, dwNpcId)
	local szKey = format("nNpcParam%dnDir%d", nNpcParam, nDir)
	if self.tbFriendNpcList[nMapId] == nil then
		return
	end
	if self.tbFriendNpcList[nMapId].tbInfo == nil then
		return
	end
	local dwTmpNpcId = self.tbFriendNpcList[nMapId].tbInfo[nNpcIndex]
	if dwTmpNpcId ~= dwNpcId then
		return
	end
	if nNpcParam >= 4 then
		self.tbFriendNpcList[nMapId].nGuard = self.tbFriendNpcList[nMapId].nGuard - 1
	end
	self.tbFriendNpcList[nMapId][szKey]= self.tbFriendNpcList[nMapId][szKey] - 1
	self.tbFriendNpcList[nMapId].tbInfo[nNpcIndex] = nil
end

function TongCastle:TransOutPlayer()
	local pEvent = EventSys:GetType("TongCastle")
	for nMapId, bFlag in pairs(self.tbMapList) do
		if bFlag and self.tbPlayerList[nMapId] then
			for szPlayerName, _ in pairs(self.tbPlayerList[nMapId]) do
				local nPlayerIndex = SearchPlayer(szPlayerName)
				if nPlayerIndex > 0 then
					pEvent:OnPlayerEvent("OnFinish", nPlayerIndex)
					CallPlayerFunction(nPlayerIndex, Msg2Player, "Thêi gian më Thµnh B¶o Bang Héi kÕt thóc, ng­¬i bÞ chuyÓn ra ngoµi.")
					CallPlayerFunction(nPlayerIndex, NewWorld, 176, 1661, 3271)
				end
			end
		end
	end
end

function TongCastle:GetMapListOnThisServer()
	local tbMap = {}
	for nMapId, bFlag in pairs(self.tbMapList) do
		local nMapIndex = SubWorldID2Idx(nMapId)
		if bFlag and nMapIndex >= 0 then
			tinsert(tbMap, nMapId)
		end
	end
	
	return tbMap
end

function TongCastle:TreeLifeTimeOver(hParam)
	local nTreeType = ObjBuffer:PopObject(hParam)
	local tbMap = self:GetMapListOnThisServer()
	for i=1,getn(tbMap) do
			self:ClearAllTreeNpc(tbMap[i], nTreeType)
	end
end

function TongCastle:Start()
	for nMapId, _ in pairs(tbCity) do
		self:AssociateMap(nMapId)
	end
	
	self:SetForbitItem()
	self:Init()
	-- [TONGCASTLE 23/08] RegAll (EventSys EnterMap/LeaveMap + DynamicExecute tra table) KHONG chay duoc
	-- tren JX1 (1 state/tep, DynamicExecute khong tra gia tri) -> thay bang newworld.lua cua map 984
	--self:RegAll()
	self:AddAllTrap()
	self:GetAndAddTreeFromRelay()
	
end

function TongCastle:basemission_CallNpc(tbNpc, nMapId, nPosX, nPosY)
	nMapId	= nMapId or tbNpc.nMapId
	nPosX	= nPosX or tbNpc.nPosX 
	nPosY	= nPosY or tbNpc.nPosY
	local nMapIndex = SubWorldID2Idx(nMapId)
	if nMapIndex >= 0 then		
		local nNpcIndex = AddNpcEx(
			tbNpc.nNpcId,  --npc Id
			tbNpc.nLevel or 1,		-- µÈ¼¶
			tbNpc.nSeries or random(0,4),		-- ÎåÐÐ
			nMapIndex,	-- µØÍ¼
			nPosX,	-- X×ø±ê
			nPosY,	-- Y×ø±ê
			tbNpc.bNoRevive or 1,			-- ²»ÖØÉú
			tbNpc.szName,		-- Ãû×Ö
			tbNpc.nIsboss or 0
		);	-- ÊÇ·ñBOSS
		if nNpcIndex <= 0 then
			return 0
		end
		if tbNpc.szDeathScript then
			SetNpcDeathScript(nNpcIndex, tbNpc.szDeathScript);
		end
			
		if tbNpc.szScriptPath then 
			SetNpcScript(nNpcIndex, tbNpc.szScriptPath)
		end
		if tbNpc.nTime then
			SetNpcTimer(nNpcIndex, 18*tbNpc.nTime);
		end
		
		if tbNpc.nCurCamp then
			SetNpcCurCamp(nNpcIndex, tbNpc.nCurCamp)
		end
		if tbNpc.tbNpcParam then
			for i =1, getn(tbNpc.tbNpcParam) do 
				if tbNpc.tbNpcParam[i] then
					SetNpcParam(nNpcIndex, i, tbNpc.tbNpcParam[i])
				end
			end
		end
		if tbNpc.pCallBack then
			tbNpc:pCallBack(nNpcIndex)
		end
		return nNpcIndex
	else
		return 0
	end
end

AutoFunctions:Add(TongCastle.Start, TongCastle)

function TestShowDebug()
	local handle = OB_Create()
	RemoteExecute("\\script\\mission\\tongcastle\\tongcastle.lua",
								"tbS3TongCastle:TestShowDebug", handle)
	OB_Release(handle)
end

-- ===== -- [TONGCASTLE 23/08] cau noi 1 state/tep: trap/guard/treedeath/newworld goi sang STATE CHU nay =====
TC_JX1_GUARDTYPE = {[1] = 4, [2] = 5, [3] = 6, [4] = 7}	-- guard.lua tbGuardType -> nNpcParam
TC_JX1_JOINTONGTIME = HD_CFG("TC_VAO_BANG_PHUT", 1440)	-- guideperson JOIN_TONG_TIME = 60*24 phut

function TongCastle:JX1_OnEnterMap()	-- = castleplayer CastlePlayer:OnEnterMap
	SetFightState(1)
	DisabledUseTownP(1)
	SetLogoutRV(1)
	DisabledStall(1)
	SetDeathScript("\\script\\missions\\tongcastle\\castleplayer.lua")
	local nMapId = GetWorldPos()
	self:AddPlayer2List(GetName(), nMapId, GetTongName())
end

function TongCastle:JX1_OnLeaveMap()	-- = castleplayer CastlePlayer:OnLeaveMap (quet moi map cho chac)
	SetFightState(0)
	DisabledUseTownP(0)
	DisabledStall(0)
	SetDeathScript("")
	local szName = GetName()
	for nMapId, bFlag in pairs(self.tbMapList) do
		if bFlag and self.tbPlayerList[nMapId] and self.tbPlayerList[nMapId][szName] then
			self:DelPlayer2List(szName, nMapId)
		end
	end
end

function TongCastle:JX1_Trap(nTrapId)	-- = trap.lua main
	local _, _, nMapIndex = GetPos()
	local nMapId = SubWorldIdx2ID(nMapIndex)
	self:TrapMoveTo(nMapId, nTrapId)
end

function TongCastle:JX1_CallGuard(nType, nItemIndex)	-- = guard.lua CallGuard (chay trong state chu de RegANpc/GuardIsLimit dung kho that)
	local nNpcParam = TC_JX1_GUARDTYPE[nType]
	if not nNpcParam then
		return
	end
	local nX32, nY32, nMapIndex = GetPos()
	local nMapId = SubWorldIdx2ID(nMapIndex)
	if tbCity[nMapId] == nil then
		Talk(1, "", "§Þa ®é hiÖn t¹i kh«ng thÓ triÖu håi Thñ VÖ")
		return
	end
	local szCityTongName = GetCityOwner(tbCity[nMapId][1])
	if szCityTongName ~= GetTongName() then
		Talk(1, "", "Ng­¬i chØ cã thÓ triÖu håi Thñ VÖ t¹i bang chiÕm thµnh cña m×nh")
		return
	end
	if self:GuardIsLimit(nMapId) ~= 1 then
		Talk(1, "", "Thñ VÖ ®· v­ît giíi h¹n, kh«ng thÓ triÖu håi")
		return
	end
	local tbNpc, nCount = GetAroundNpcList(HD_CFG("TC_BANKINH_BUA", 15))
	local bNear = nil
	for i = 1, nCount do
		local nP = GetNpcParam(tbNpc[i], 1)
		if nP >= 1 and nP <= 3 and GetTmpCamp(tbNpc[i]) == 1 then
			bNear = 1
		end
	end
	if bNear ~= 1 then
		Talk(1, "", "ChØ cã thÓ triÖu håi Thñ VÖ gÇn chç ThÇn Méc")
		return
	end
	local szNpcName = self.tbNpcParam2Type[nNpcParam]
	local tbNpcT = self.tbNpcTypeList[szNpcName]
	local nNewNpcIndex = self:basemission_CallNpc(tbNpcT, nMapId, nX32, nY32)
	local nStackCount = GetItemStackCount(nItemIndex)
	if nStackCount > 1 then
		SetItemStackCount(nItemIndex, nStackCount - 1)
	else
		RemoveItemByIndex(nItemIndex)
	end
	if nNewNpcIndex > 0 then
		SetTmpCamp(1, nNewNpcIndex)
		self:RegANpc(nMapId, nNpcParam, 0, nNewNpcIndex, GetNpcId(nNewNpcIndex))
	end
end

function TongCastle:JX1_TreeDeath(nNpcIndex)	-- = treedeath.lua OnDeathEx (boi canh = ke giet, qua DynamicExecuteByPlayer)
	local nX32, nY32, nMapIndex = GetNpcPos(nNpcIndex)
	local nMapId = SubWorldIdx2ID(nMapIndex)
	local nNpcParam = GetNpcParam(nNpcIndex, 1)
	local nDir = GetNpcParam(nNpcIndex, 2)
	if self:FindNpc(nMapId, nNpcIndex, GetNpcId(nNpcIndex)) ~= 1 then
		print("Npc Death Error!!!!!!!!!!")
		return
	end
	self:UnRegANpc(nMapId, nNpcParam, nDir, nNpcIndex, GetNpcId(nNpcIndex))
	self:UpdateObstacleObj(nMapId)
	local szTongName = ""
	local szName = ""
	if (GetJoinTongTime() > TC_JX1_JOINTONGTIME) then
		szTongName = GetTongName()
		szName = GetName()
	end
	Msg2Map(nMapId, format("<color=yellow>%s ®¸nh ng· 1 c©y %s<color>", szName, GetNpcName(nNpcIndex)))
	if 1 <= nNpcParam and nNpcParam <= 3 then
		local handle = OB_Create()
		ObjBuffer:PushObject(handle, nMapId)
		ObjBuffer:PushObject(handle, nNpcParam)
		ObjBuffer:PushObject(handle, nX32)
		ObjBuffer:PushObject(handle, nY32)
		ObjBuffer:PushObject(handle, nDir)
		ObjBuffer:PushObject(handle, szTongName)
		ObjBuffer:PushObject(handle, szName)
		RemoteExecute("\\script\\mission\\tongcastle\\tongcastle.lua", "tbS3TongCastle:DelOneTreeData", handle)
		OB_Release(handle)
		self:BroadcastTreeDeath(nMapId, nNpcParam, nDir)
	end
end
