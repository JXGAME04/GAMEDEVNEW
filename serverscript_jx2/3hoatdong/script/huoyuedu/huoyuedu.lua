IncludeLib("FILESYS")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\misc\\eventsys\\type\\player.lua")
Include("\\script\\misc\\eventsys\\eventsys.lua")
Include("\\script\\lib\\log.lua")

tbHuoYueDu = {}
tbHuoYueDu.szFilePath = "\\settings\\huoyuedu\\huoyuedu.txt"
-- »îÔ¾¶ÈµÄ×î¸ßÖµ£¬ÐèÒªºÍ\UI\UI3\uiactivityguide\activityinfo.iniÖÐµÄMaxHuoyueduÖµ±£³ÖÒ»ÖÂ
tbHuoYueDu.nMaxHuoYueDu = 100	

tbHuoYueDu.TSK_TotalHuoYueDu = 2880 --¼ÇÂ¼Íæ¼ÒÃ¿ÌìÔÚËùÓÐ»î¶¯Àï»ñµÃ×Ü»îÔ¾¶È
tbHuoYueDu.TSK_Award = 2882 --¼ÇÂ¼»îÔ¾¶ÈÁì½±
tbHuoYueDu.TSK_Add_Week_Total = 4156 --¼ÇÂ¼ÊÇ·ñÀÛ¼ÓÖÜË¢ÐÂµÄµ½Ã¿Ìì×Ü»îÔ¾¶ÈÀïÃæ

OPENED = 1	-- »îÔ¾¶È¿ªÆô×´Ì¬
CLOSED = 2  -- »îÔ¾¶È¹Ø±Õ×´Ì¬
STATE = OPENED -- »îÔ¾¶ÈÊÇ·ñ¿ªÊ¼£¬1Îª¿ªÆô£¬ÆäËû¾ùÎª¹Ø±Õ

tbActivity2ID = {
	["shijiandetiaozhan"] = 3,
	["fenglingdu"] = 2,
	["songjin"] = 4,
	["yandibaozang"] = 5,
	["qichengdazhan"] = 6,
	["shashourenwu"] = 11,
	["huihuangzhiguo"] = 8,
	["tianziguozhan"] = 14,
	["swordburialmaze"] = 24,
	["fuguijinhe"] = 29,
	["longinfirsttime"] = 31,
	["longmenbiaoju"] = 32,
	["huangjinguo"] = 33,
	["tongcastle"] = 34,
	["wulinmengzhu"] = 35,
	["encountermap"] = 36,
	["haoyunpiao"] = 37,
	["tianlvshu"] = 38,
	["siguoya"] = 39,
	["beidouactivity"] = 40,
	["bloodybattle"] = 41,
}

function tbHuoYueDu:LoadTable(szPath)
	if (TabFile_Load(szPath, szPath) == 0) then
		print("Load TabFile Error!"..szPosPath)
		return
	end
	local nRowCount = TabFile_GetRowCount(szPath)
	self.tbActivity = self.tbActivity or {}
	local number = 1
	for i=1, (nRowCount - 1) do
		self.tbActivity[i] = self.tbActivity[i] or {}
		self.tbActivity[i]["nTaskId"] = tonumber(TabFile_GetCell(szPath, i + 1, 3))
		local nColumn = 4
		local nCount = tonumber(TabFile_GetCell(szPath, i + 1, nColumn))
		self.tbActivity[i]["nCount"] = nCount
		self.tbActivity[i]["tbValue"] = self.tbActivity[i]["tbValue"] or {}
		number = number + 1
		for j=1, nCount do
			self.tbActivity[i]["tbValue"][j] = tonumber(TabFile_GetCell(szPath, i + 1, nColumn + j))
		end
		--ÊÇ·ñÊÇÖÜË¢ÐÂ
		self.tbActivity[i]["nWeekResetFlag"] = tonumber(TabFile_GetCell(szPath, i + 1, 15)) or 0
	end
end

function tbHuoYueDu:AddHuoYueDu(szActivity)
	self:TryWeeklyHuoYueAddToTotal()
	local nActivityId = tbActivity2ID[szActivity]
	if not nActivityId then
		return 
	end
	local tbActivity = self.tbActivity[nActivityId]
	local nResult = self:CheckTimes(nActivityId)
	if nResult ~= 1 then
		return
	end

	self:AddTaskCount(nActivityId)
	local nTime = self:GetTaskCount(nActivityId)
	local nAddedValue = tbActivity["tbValue"][nTime]
	
	if nAddedValue == nil or type(nAddedValue) ~= "number" or nAddedValue <= 0 then
		return
	end
	
	local nTaskValue = PlayerFunLib:GetTaskDailyCount(self.TSK_TotalHuoYueDu)
	if nAddedValue > (self.nMaxHuoYueDu - nTaskValue) then
		nAddedValue = self.nMaxHuoYueDu - nTaskValue
	end
	if nAddedValue > 0 then
		Msg2Player(format("Chóc mõng ng­¬i nhËn ®­îc %d ®iÓm n¨ng ®éng",nAddedValue))
		PlayerFunLib:AddTaskDaily(self.TSK_TotalHuoYueDu, nAddedValue)
		G_ACTIVITY:OnMessage("Huoyuedu", nTaskValue + nAddedValue)
		self:HuoYueDuSyncTask()
		local szLog = format("Player Get HuoYueDu, szKey:%s, nValue:%d", szActivity, nAddedValue)
		tbLog:PlayerActionLog(szLog)
	end
end

function tbHuoYueDu:AddTaskCount(nActivityId)
	if self.tbActivity[nActivityId] == nil then
		return
	end
	local nWeekResetFlag = self.tbActivity[nActivityId]["nWeekResetFlag"]
	if nWeekResetFlag == 0 then
		self:AddTaskDaily(nActivityId)
	else
		self:AddTaskWeekly(nActivityId)
	end
end

function tbHuoYueDu:AddTaskDaily(nActivityId)
	if self.tbActivity[nActivityId] == nil then
		return
	end
	PlayerFunLib:AddTaskDaily(self.tbActivity[nActivityId]["nTaskId"], 1)
end

function tbHuoYueDu:AddTaskWeekly(nActivityId)
	if self.tbActivity[nActivityId] == nil then
		return
	end
	PlayerFunLib:AddTaskWeekly(self.tbActivity[nActivityId]["nTaskId"], 1)	
end

function tbHuoYueDu:GetTaskDailyCount(nActivityId)
	if self.tbActivity[nActivityId] == nil then
		return
	end
	local nResult = PlayerFunLib:GetTaskDailyCount(self.tbActivity[nActivityId]["nTaskId"])
	return nResult
end

function tbHuoYueDu:GetTaskWeeklyCount(nActivityId)
	local nResult = 0
	if self.tbActivity[nActivityId] == nil then
		return nResult
	end
	nResult = PlayerFunLib:GetTaskWeekly(self.tbActivity[nActivityId]["nTaskId"])
	return nResult
end

function tbHuoYueDu:GetTaskCount(nActivityId)
	local nResult = 0
	if self.tbActivity[nActivityId] == nil then
		return nResult
	end
	local nWeekResetFlag = self.tbActivity[nActivityId]["nWeekResetFlag"]
	if nWeekResetFlag == 0 then
		nResult = self:GetTaskDailyCount(nActivityId)
	else
		nResult = self:GetTaskWeeklyCount(nActivityId)
	end
	return nResult
end

function tbHuoYueDu:AddHuoYueDuAll(tbPlayer, szActivity)
	for i=1,getn(tbPlayer) do
		CallPlayerFunction(tbPlayer[i], self.AddHuoYueDu, self, szActivity)
	end
end

function tbHuoYueDu:CheckTimes(nActivityId)
	if STATE ~= OPENED then
		return 
	end
	
	if self.tbActivity[nActivityId] == nil then
		return 
	end
	
	if PlayerFunLib:CheckTaskDaily(self.TSK_TotalHuoYueDu, self.nMaxHuoYueDu, "", ">=") == 1 then
		Msg2Player("§iÓm n¨ng ®éng cña ng­¬i h«m nay ®· ®Çy, ®iÓm n¨ng ®éng lÇn nµy kh«ng tÝnh vµo trong")
		return 
	end
	
	local tbActivity = self.tbActivity[nActivityId]
	local nTaskId = self.tbActivity[nActivityId]["nTaskId"]
	local nCount = self.tbActivity[nActivityId]["nCount"]
	local nWeekResetFlag = self.tbActivity[nActivityId]["nWeekResetFlag"]
	
	if nWeekResetFlag == 0 then
		if PlayerFunLib:CheckTaskDaily(nTaskId, nCount, "", ">=") == 1 then
			return 
		end
	else
		if PlayerFunLib:CheckTaskWeekly(nTaskId, nCount, "", ">=") == 1 then
			return 
		end
	end
	
	return 1
end

function tbHuoYueDu:ClearAll()
	for _,value in tbActivity2ID do
		local nTaskId = self.tbActivity[value]["nTaskId"]
		SetTask(nTaskId,0);
	end
	SetTask(self.TSK_Award, 0)
	SetTask(self.TSK_TotalHuoYueDu, 0)
end

function tbHuoYueDu:RegisterEvent()
	EventSys:GetType("OnLogin"):Reg(0, self.OnLogin, self)
end


function tbHuoYueDu:OnLogin()
	self:AddHuoYueDu("longinfirsttime")
	local szString = "HiÖp sÜ mçi ngµy lÇn ®Çu ®¨ng nhËp cã thÓ nhËn ®­îc n¨ng ®éng"
	Msg2Player(szString)
end

function tbHuoYueDu:HuoYueDuSyncTask()
	for _,value in tbActivity2ID do
		local nTaskId = tbHuoYueDu.tbActivity[value]["nTaskId"]
		local nTaskValue = self:GetTaskCount(value)--Ë¢ÐÂÈÎÎñ±äÁ¿
		SyncTaskValue(nTaskId)
	end
end

function tbHuoYueDu:TryWeeklyHuoYueAddToTotal()
	self:HuoYueDuSyncTask()
	local nAddFlag = PlayerFunLib:GetTaskDailyCount(self.TSK_Add_Week_Total)
	if nAddFlag == 0 then
		PlayerFunLib:AddTaskDaily(self.TSK_Add_Week_Total, 1)
		local nWeekAddValue = 0
		for _, nActivityId in tbActivity2ID do
			local nWeeklyResetFlag = self.tbActivity[nActivityId]["nWeekResetFlag"]
			if nWeeklyResetFlag == 1 then --ÖÜË¢ÐÂÐèÒªÀÛ¼Ó
				local nTaskId = self.tbActivity[nActivityId]["nTaskId"]--Ö®Ç°ÒÑ¾­¶àÉÙ´ÎÁË
				local nTaskValueCount = PlayerFunLib:GetTaskWeekly(nTaskId)
				for nIndex = 1, nTaskValueCount do
					local nAddedValue = self.tbActivity[nActivityId]["tbValue"][nIndex] or 0
					nWeekAddValue = nWeekAddValue + nAddedValue
				end
			end
		end
		
		local nTaskValue = PlayerFunLib:GetTaskDailyCount(self.TSK_TotalHuoYueDu)
		if nWeekAddValue > (self.nMaxHuoYueDu - nTaskValue) then
			nWeekAddValue = self.nMaxHuoYueDu - nTaskValue
		end
		PlayerFunLib:AddTaskDaily(self.TSK_TotalHuoYueDu, nWeekAddValue)
	end
end

tbHuoYueDu:LoadTable(tbHuoYueDu.szFilePath)
tbHuoYueDu:RegisterEvent()

