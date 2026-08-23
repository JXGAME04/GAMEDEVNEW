Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\remoteexc.lua")
Include("\\script\\lib\\common.lua")

tbS3TongCastle = {}

tbS3TongCastle.tbTreeLifeTime = {
	[1] = {
		tbOpenDay = {[0]=1},
		tbStartTime = {17,00},
		tbEndTime = {19,00},
	},
	[2] = {
		tbOpenDay = {[0]=1},
		tbStartTime = {18,00},
		tbEndTime = {19,00},
	},
	[3] = {
		tbOpenDay = {[0]=1},
		tbStartTime = {18,30},
		tbEndTime = {19,00},
	},
}

tbS3TongCastle.tbMapList = {
	--[981] = {4, 2, 37,},
	[984] = {7, 2, 176,},
}

tbS3TongCastle.tbTreePoint = {
	["AttTong"] = {
		[1] = 15,
		[2] = 30,
		[3] = 150,
	},
	["DefendTong"] ={
		[1] = 10,
		[2] = 20,
		[3] = 100,
	},
	["Personal"] = {
		[1] = 20,
		[2] = 40,
		[3] = 200,
	},
}



tbS3TongCastle.SAVE_KEY = "MISSION_tongcastle"
tbS3TongCastle.KEY_TONG = 1
tbS3TongCastle.KEY_PLAYER = 2
tbS3TongCastle.KEY_TREE = 3

tbS3TongCastle.tbTongTreePoint = {}
tbS3TongCastle.tbPlayerTreePoint = {}


function tbS3TongCastle:GetTreeLifeTime(nTreeType)
	local tbOpenDay = self.tbTreeLifeTime[nTreeType].tbOpenDay
	if not tbOpenDay then
		return
	end
	
	local nWeekDay = tonumber(date("%w"))
	local nLastWeekDay = nWeekDay - 1
	if nLastWeekDay == -1 then
		nLastWeekDay = 6
	end
	
	if (not tbOpenDay[nWeekDay]) and (not tbOpenDay[nLastWeekDay]) then
		return
	end
	
	local nCurTime = tonumber(date("%H"))*60 + tonumber(date("%M"))
	local tbStartTime = self.tbTreeLifeTime[nTreeType].tbStartTime
	local tbEndTime = self.tbTreeLifeTime[nTreeType].tbEndTime
	local nStartTime = tbStartTime[1]*60 + tbStartTime[2]
	local nEndTime = tbEndTime[1]*60 + tbEndTime[2]
	
	if tbOpenDay[nLastWeekDay] and (nCurTime <= nEndTime - 1440) then
		return nStartTime-1440, nEndTime-1440
	end
	
	if tbOpenDay[nWeekDay] then
		return nStartTime, nEndTime
	end
end

function tbS3TongCastle:IsTreeInLifeTime(nTreeType)
	local nCurTime = tonumber(date("%H"))*60 + tonumber(date("%M"))
	local nStartTime,nEndTime = self:GetTreeLifeTime(nTreeType)
	if nStartTime and nEndTime then
		return (nStartTime <= nCurTime) and (nCurTime < nEndTime), nStartTime, nEndTime
	end
end

function tbS3TongCastle:Msg2Tong(ParamHandle)
	local nMsgCount = ObjBuffer:PopObject(ParamHandle)
	local szTongName = ObjBuffer:PopObject(ParamHandle)
	for i = 1, nMsgCount do
		local szMsg = ObjBuffer:PopObject(ParamHandle)
		Msg2Tong(szTongName, szMsg)
	end
end

function tbS3TongCastle:LoadPointData(nKey, tbData)
	local nCurDate = tonumber(date("%Y%m%d"))
	
	local handle = OB_Create()
	OB_LoadShareData(handle, self.SAVE_KEY, nKey, 0)
	if OB_IsEmpty(handle) == 1 then
		OB_Release(handle)
		return
	end
	
	local nRecordDate = ObjBuffer:PopObject(handle)
	local nCount = ObjBuffer:PopObject(handle)
	OB_Release(handle)
	
	if nRecordDate ~= nCurDate then
		return
	end
	
	for i = 1, nCount do
		local hData = OB_Create()
		OB_LoadShareData(hData, self.SAVE_KEY, nKey, i)
		if OB_IsEmpty(hData) ~= 1 then
			local szName = ObjBuffer:PopObject(hData)
			local nPoint = ObjBuffer:PopObject(hData)
			tbData[szName] = nPoint
		end
		OB_Release(hData)
	end
end

function tbS3TongCastle:SavePointData(nKey, tbData)
	local nCurDate = tonumber(date("%Y%m%d"))
	if not tbData then
		local handle = OB_Create()
		ObjBuffer:PushObject(handle, nCurDate)
		ObjBuffer:PushObject(handle, 0)
		OB_SaveShareData(handle, self.SAVE_KEY, nKey, nCount)
		OB_Release(handle)
		return
	end
	
	local nCount = 0
	for szName, nPoint in tbData do
		nCount = nCount + 1
		local hData = OB_Create()
		ObjBuffer:PushObject(hData, szName)
		ObjBuffer:PushObject(hData, nPoint)
		OB_SaveShareData(hData, self.SAVE_KEY, nKey, nCount)
		OB_Release(hData)
	end
	
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, nCurDate)
	ObjBuffer:PushObject(handle, nCount)
	OB_SaveShareData(handle, self.SAVE_KEY, nKey, 0)
	OB_Release(handle)
end

function tbS3TongCastle:SaveTongPoint()
	self:TestShoPoint()
	self:SavePointData(self.KEY_TONG, self.tbTongTreePoint)
end

function tbS3TongCastle:SavePlayerPoint()
	self:TestShoPoint()
	self:SavePointData(self.KEY_PLAYER, self.tbPlayerTreePoint)
end

function tbS3TongCastle:LoadTongPoint()
	self.tbTongTreePoint = {}
	self:LoadPointData(self.KEY_TONG, self.tbTongTreePoint)
end

function tbS3TongCastle:LoadPlayerPoint()
	self.tbPlayerTreePoint = {}
	self:LoadPointData(self.KEY_PLAYER, self.tbPlayerTreePoint)
end

function tbS3TongCastle:AddPlayerPoint(szPlayerName, nTreeType)
	local nPoint = self.tbTreePoint["Personal"][nTreeType]
	self.tbPlayerTreePoint[szPlayerName] = nPoint + (self.tbPlayerTreePoint[szPlayerName] or 0)
end

function tbS3TongCastle:AddTongPoint(szTongName, szType, nTreeType)
	local nPoint = self.tbTreePoint[szType][nTreeType]
	self.tbTongTreePoint[szTongName] = nPoint + (self.tbTongTreePoint[szTongName] or 0)
end

function tbS3TongCastle:GetTreePoint(ParamHandle)
	local szPlayerName = ObjBuffer:PopObject(ParamHandle)
	local nMapId = ObjBuffer:PopObject(ParamHandle)
	local szTongName = ObjBuffer:PopObject(ParamHandle)
	local nPlayerPoint = self.tbPlayerTreePoint[szPlayerName] or  0
	local nTongPoint = self.tbTongTreePoint[szTongName] or 0
	
	self:SavePlayerPoint()

	local ResHandle = OB_Create()
	ObjBuffer:PushObject(ResHandle, szPlayerName)
	ObjBuffer:PushObject(ResHandle, nMapId)
	ObjBuffer:PushObject(ResHandle, nPlayerPoint)
	ObjBuffer:PushObject(ResHandle, nTongPoint)
	RemoteExecute("\\script\\missions\\tongcastle\\guideperson.lua", "ExchangeDialog", ResHandle)
	OB_Release(ResHandle)
end

function tbS3TongCastle:CalLiveTreePoint(nMapId, szCityName, nTreeType)
	local handle = OB_Create()
	OB_LoadShareData(handle, self.SAVE_KEY, self.KEY_TREE, nMapId)
	if OB_IsEmpty(handle) ~= 1 then
		local nCount = ObjBuffer:PopObject(handle)
		local nType = self.tbMapList[nMapId][2]
		for i = 1, nCount do
			local nNpcParam = ObjBuffer:PopObject(handle)
			local nX = ObjBuffer:PopObject(handle)
			local nY = ObjBuffer:PopObject(handle)
			local nDir = ObjBuffer:PopObject(handle)
			if nNpcParam >= 1 and nNpcParam <= 3 and nTreeType == nNpcParam then
				self:AddTongPoint(szCityName, "DefendTong", nTreeType)
			end
		end
	end
	self:SaveTongPoint()
	OB_Release(handle)
end

function tbS3TongCastle:CalOneCityTreePoint(nMapId, nTreeType)
	local nDate = tonumber(date("%Y%m%d"))
	local szCityName = GetCityOwner(self.tbMapList[nMapId][1])

	self:CalLiveTreePoint(nMapId, szCityName, nTreeType)
end

function tbS3TongCastle:CalAllTreePoint(nTreeType)
	for nMapId, tb in self.tbMapList do
		self:CalOneCityTreePoint(nMapId, nTreeType)
	end
end

function tbS3TongCastle:DelAllTreeData(nTreeType)
	for nMapId, tb in self.tbMapList do
		if type(nMapId) == "number" then
			local nRet,tbData = self:LoadTreeData(nMapId)
			for i=1,getn(tbData) do
				if (not nTreeType) or tbData[i].nNpcParam == nTreeType then
					self:DelTreeData(nMapId, tbData[i])
				end
			end
		end
	end
end

function tbS3TongCastle:LoadTreeData(nMapId)
	local tbRes = {}
	local nRet = 0
	local handle = OB_Create()
	OB_LoadShareData(handle, self.SAVE_KEY, self.KEY_TREE, nMapId)
	if OB_IsEmpty(handle) ~= 1 then
		nRet = 1
		local nCount = ObjBuffer:PopObject(handle)
		for i = 1, nCount do
			local nNpcParam = ObjBuffer:PopObject(handle)
			local nX = ObjBuffer:PopObject(handle)
			local nY = ObjBuffer:PopObject(handle)
			local nDir = ObjBuffer:PopObject(handle)
			tinsert(tbRes, {nNpcParam = nNpcParam, nX = nX, nY = nY, nDir = nDir,})
		end
	end
	OB_Release(handle)
	return nRet, tbRes
end

function tbS3TongCastle:SaveNpcData(ParamHandle)
	local nCount = ObjBuffer:PopObject(ParamHandle)
	local nMapId = ObjBuffer:PopObject(ParamHandle)
	local nRet, tbRes = self:LoadTreeData(nMapId)
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, nCount + getn(tbRes))
	for i = 1, nCount do
		local nNpcParam = ObjBuffer:PopObject(ParamHandle)
		local nX = ObjBuffer:PopObject(ParamHandle)
		local nY = ObjBuffer:PopObject(ParamHandle)
		local nDir = ObjBuffer:PopObject(ParamHandle)
		self:PushANpcData(handle, {nNpcParam = nNpcParam, nX = nX, nY = nY, nDir = nDir,})
	end
	for i = 1, getn(tbRes) do
		self:PushANpcData(handle, tbRes[i])
	end
	OB_SaveShareData(handle, self.SAVE_KEY, self.KEY_TREE, nMapId)
	OB_Release(handle)
end

function tbS3TongCastle:Date2Time(nDate)
	local nYear = floor(nDate/10000)
	local nMD = mod(nDate, 10000)
	local nMonth = floor(nMD/100)
	local nDay = mod(nMD, 100)
	return Tm2Time(nYear, nMonth, nDay)
end

function tbS3TongCastle:PushANpcData(handle, tbRes)
	ObjBuffer:PushObject(handle, tbRes.nNpcParam)	
	ObjBuffer:PushObject(handle, tbRes.nX)	
	ObjBuffer:PushObject(handle, tbRes.nY)
	ObjBuffer:PushObject(handle, tbRes.nDir)	
end

function tbS3TongCastle:SendTreeToGS(hParam, hResult)
	local nMapId = ObjBuffer:PopObject(hParam)
	local nRet,tbData = self:LoadTreeData(nMapId)
	local tbLive = {}
	for i=1,getn(tbData) do
		if self:IsTreeInLifeTime(tbData[i].nNpcParam) then
			tinsert(tbLive, {nNpcParam = tbData[i].nNpcParam, nX = tbData[i].nX, nY = tbData[i].nY, nDir = tbData[i].nDir})
		else
			self:DelTreeData(nMapId, tbData[i])
		end
	end
	
	local nCount = getn(tbLive)
	ObjBuffer:PushObject(hResult, nCount)
	for i=1,nCount do
		self:PushANpcData(hResult, tbLive[i])
	end
end

function tbS3TongCastle:DelTreeData(nMapId, tbTreeData)
	local nRet, tbRes = self:LoadTreeData(nMapId)
	local tbLive = {}
	if nRet == 1 then
		for i = 1, getn(tbRes) do
			if (tbRes[i].nNpcParam ~= tbTreeData.nNpcParam
					or tbRes[i].nX ~= tbTreeData.nX
					or tbRes[i].nY ~= tbTreeData.nY
					or tbRes[i].nDir ~= tbTreeData.nDir) then
				tinsert(tbLive, tbRes[i])
			end
		end
		
		local nCount = getn(tbLive)
		local handle = OB_Create()
		ObjBuffer:PushObject(handle, nCount)
		for i=1, nCount do
			self:PushANpcData(handle, tbLive[i])
		end
		OB_SaveShareData(handle, self.SAVE_KEY, self.KEY_TREE, nMapId)
		OB_Release(handle)
	end
end

function tbS3TongCastle:DelOneTreeData(ParamHandle)
	local nMapId = ObjBuffer:PopObject(ParamHandle)
	local tbTreeData = {}
	tbTreeData.nNpcParam = ObjBuffer:PopObject(ParamHandle)
	tbTreeData.nX = ObjBuffer:PopObject(ParamHandle)
	tbTreeData.nY = ObjBuffer:PopObject(ParamHandle)
	tbTreeData.nDir = ObjBuffer:PopObject(ParamHandle)
	
	local szTongName = ObjBuffer:PopObject(ParamHandle)
	local szName = ObjBuffer:PopObject(ParamHandle)
	
	self:DelTreeData(nMapId, tbTreeData)
	
	if szTongName and szTongName ~= "" and 1 <= tbTreeData.nNpcParam and tbTreeData.nNpcParam <= 3 then
		self:AddTongPoint(szTongName, "AttTong", tbTreeData.nNpcParam)
		self:SaveTongPoint()
		if szName and szName ~= "" then
			self:AddPlayerPoint(szName, tbTreeData.nNpcParam)
			self:SavePlayerPoint()
		end
	end
end

function tbS3TongCastle:CheckAndReviveTree()
	local nCurTime = tonumber(date("%H"))*60 + tonumber(date("%M"))
	local nWeekday = tonumber(date("%w"))
	
	for i=1,getn(self.tbTreeLifeTime) do
		local tbWeekAndTime = self.tbTreeLifeTime[i]
		local nStartTime = tbWeekAndTime.tbStartTime[1]*60 + tbWeekAndTime.tbStartTime[2]
		if tbWeekAndTime.tbOpenDay[nWeekday] and nStartTime == nCurTime then
			local handle = OB_Create()
			local nTreeType = i
			ObjBuffer:PushObject(handle, nTreeType)
			RemoteExecute("\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:TreeNpcRevive", handle)
			OB_Release(handle)
		end
	end
end

function tbS3TongCastle:CheckAndDeleteTree()
	local nCurTime = tonumber(date("%H"))*60 + tonumber(date("%M"))
	local nWeekday = tonumber(date("%w"))
	for i=1,getn(self.tbTreeLifeTime) do
		local nStartTime, nEndTime = self:GetTreeLifeTime(i)
		if nEndTime and nCurTime == nEndTime then
			self:CalAllTreePoint(i)
			self:DelAllTreeData(i)
			local handle = OB_Create()
			ObjBuffer:PushObject(handle, i)
			RemoteExecute("\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:TreeLifeTimeOver", handle)
			OB_Release(handle)
			if i == 3 then
				self:DeleteGuard()
			end
		end
	end
	if nCurTime == 0 then
		self.tbTongTreePoint = {}
		self.tbPlayerTreePoint = {}
		self:SaveTongPoint()
		self:SavePlayerPoint()
	end
end

function tbS3TongCastle:DeleteGuard()
	for nMapId,_ in self.tbMapList do
		if type(nMapId) == "number" then
			local handle = OB_Create()
			ObjBuffer:PushObject(handle, nMapId)
			RemoteExecute("\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:ClearGuardNpc", handle)
			OB_Release(handle)
		end
	end
end

function tbS3TongCastle:TestShowTreeLive()
	for nMapId,_ in self.tbMapList do
		if type(nMapId) == "number" then
			local nRet,tbTreeData = self:LoadTreeData(nMapId)
			OutputMsg(format("Map[%d] has %d tree", nMapId, getn(tbTreeData)))
		end
	end
end

function tbS3TongCastle:TestShoPoint()
	OutputMsg("[Player]")
	for k, v in self.tbPlayerTreePoint do
		OutputMsg(format("%s : %d", k, v))
	end
	
	OutputMsg("[Tong]")
	for k, v in self.tbTongTreePoint do
		OutputMsg(format("%s : %d", k, v))
	end
end

function tbS3TongCastle:TestShowDebug()
	self:TestShowTreeLive()
	self:TestShoPoint()
end


tbS3TongCastle:LoadTongPoint()
tbS3TongCastle:LoadPlayerPoint()

