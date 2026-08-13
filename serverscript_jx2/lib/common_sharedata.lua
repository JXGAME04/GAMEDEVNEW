Include("\\script\\lib\\objbuffer_head.lua")
local tbKeyList = {}
local CommonShareData = {}

local INFO_KEY1 = 0
local INFO_KEY2 = 0
local RECORD_KEY2 = 1

---------------------
--db里的储存结构,空的格子表示没有使用
--| - | 0  | 1 | 2 |
--| 0 |info|   |   |
--| 1 |    |d-k|   |
--| 2 |    |d-k|   |
--| 3 |    |d-k|   |
--...
--d=data k=key
---------------------

--Format of info record = {nUsed = 1, nMaxRow = 10}
--Format of echo data record = {data, key}

function CommonShareData:Reset()
	self.tbInfo = {nUsed=1, nMaxRow=0}
	self.tbData = {}
	self.tbMap = {}
end

function CommonShareData:_SaveRecord(row, key, data)
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, data)
	ObjBuffer:PushObject(handle, key)
	OB_SaveShareData(handle, self.szKey, row, %RECORD_KEY2)
	OB_Release(handle)
end

function CommonShareData:_SaveInfo()
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, self.tbInfo)
	OB_SaveShareData(handle, self.szKey, %INFO_KEY1, %INFO_KEY2)
	OB_Release(handle)
end

function CommonShareData:_LoadRecord(nRow)
	local h = OB_Create()
	OB_LoadShareData(h, self.szKey, nRow, %RECORD_KEY2)
	if OB_IsEmpty(h) == 0 then
		local data = %ObjBuffer:PopObject(h)
		local key = %ObjBuffer:PopObject(h)
		OB_Release(h)
		return key, data
	end
end

function CommonShareData:OutputAll()
	for k, v in self.tbData do
		OutputMsg(format("[%s]=%s, [%s]=%s",type(k), type(v), tostring(k), tostring(v)))
	end
end

function CommonShareData:_Load()
	local handle = OB_Create()
	OB_LoadShareData(handle, self.szKey, %INFO_KEY1, %INFO_KEY2)
	local tbInfo
	if OB_IsEmpty(handle) == 0 then
		tbInfo = ObjBuffer:PopObject(handle)
	end
	OB_Release(handle)
	
	if tbInfo then
		local tbData = {}
		local tbMap = {}
		local nFailed = 0
		--OutputMsg(format("nMaxRow=%d", tbInfo.nMaxRow))
		for i = 1, tbInfo.nMaxRow do
			local key, data = self:_LoadRecord(i) --{key, data}
			--OutputMsg(format("[%s]=%s", tostring(key), tostring(data)))
			if key and (not tbMap[key]) then
				tbData[key] = data
				tbMap[key] = i
			else
				if not key then
					OutputMsg(format("[CommonShareData] load record failed [%s]", self.szKey))
				else
					OutputMsg(format("[CommonShareData] duplicate UniqueKey: [%s] in [%s]", tostring(key), self.szKey))
				end
				return tbInfo, nil
			end
		end
		return tbInfo, tbData, tbMap
	end
	return
end

function CommonShareData:LoadData()
	local tbInfo, tbData, tbMap = self:_Load(self.nVersion)
	if tbInfo and tbData and tbMap then	--成功
		self.tbInfo = tbInfo
		self.tbData = tbData
		self.tbMap = tbMap
	elseif not tbInfo then	--没有数据
		self:Reset()
	else	--失败
		return 0
	end
	return 1
end

--保存key和对应的data
--若key已存在, 则更新对应的data
--若key不存在，则插入。
--返回值: 1=成功，2=失败
function CommonShareData:SetData(key, data)
	local t = type(key)
	if (not self.tbInfo) or ((t ~= "number") and (t ~= "string")) then
		return 0
	end
	
	local nRow = self.tbMap[key]
	local nSaveInfo = 0
	if not nRow then
		nRow = self.tbInfo.nMaxRow + 1
		self.tbInfo.nMaxRow = nRow
		self.tbMap[key] = nRow
		nSaveInfo = 1
	end
	
	self.tbData[key] = data
	self:_SaveRecord(nRow, key, data)
	if nSaveInfo == 1 then
		self:_SaveInfo()
	end
	return 1
end

--获取key所关联的数据
--返回值: data(=nil表示不存在)
--对返回值修改之后如果要保存，必须调用SetData
function CommonShareData:GetData(key)
	if (not self.tbInfo) or (not self.tbData) then
		return 
	end
	return self.tbData[key]
end

--保存tb里所有key为字符串或数字的元素
--返回值： 1=成功， 0=失败
function CommonShareData:SaveDataInTable(tb)
	if type(tb) ~= "table" then
		return
	end
	
	if not self.tbInfo then
		return
	end
	
	for key, data in tb do
		self:SetData(key, data)
	end
	return 1
end

--获取所有数据，把数据保存到一个table，并返回
--返回值: nil=失败， table=成功, table={[key1]=data1, [key2]=data2, ...}
--对返回值修改之后如果要保存，必须调用SetData
function CommonShareData:GetAllData()
	if not self.tbInfo then
		return
	end
	
	local tb = {}
	local tbData = self.tbData
	for k, v in tbData do
		tb[k] = v
	end
	return tb
end

--删除所有数据
function CommonShareData:ClearData()
	self:Reset()
	self:_SaveInfo()
end

--创建一个s3relay共享数据,并从数据库里加载(如果存在)
--参数szTypeName: 名字，(字符串类型, 唯一) 
--返回值：一个实例 或者 nil=失败
function NewCommonShareData(szTypeName)
	if %tbKeyList[szTypeName] then
		OutputMsg(format("[CommonShareData] duplicate ShareKey: [%s]", szTypeName))
		return
	end
	
	local tb = {}
	for k, v in %CommonShareData do
		tb[k] = v
	end
	
	tb.szKey = szTypeName
	if tb:LoadData() ~= 1 then
		return
	end
	return tb
end
