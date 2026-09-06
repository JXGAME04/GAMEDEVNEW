-- common_sharedata.lua  (S3Relay)  [RELAYHT 06/09]
-- Port tu D:\ServerLinux\gateway\s3relay\script\lib\common_sharedata.lua
-- Chuyen Lua 4 -> 5.4: "for k,v in tb" -> pairs(tb), "%bien" (upvalue) -> local.
--
-- Mot BANG KEY -> DATA ben vung, luu trong ShareData cua relay (MySQL). Dung khi
-- can giu du lieu toan may chu ma khong muon tu quan ly so hieu ban ghi.
--
--   local tb = NewCommonShareData("EVENT_VANTIEU")
--   tb:SetData("xe_cua_bang_12", { nMapID = 173, nX = 100, nY = 200 })
--   local d = tb:GetData("xe_cua_bang_12")
--
-- Cach xep trong ShareData (giong ban Linux):
--   (key, 0, 0)  = ban ghi thong tin  {nUsed, nMaxRow}
--   (key, i, 1)  = ban ghi thu i      {data, key}

Include("\\script\\lib\\objbuffer_head.lua")

local tbKeyList = {}
local CommonShareData = {}

local INFO_KEY1 = 0
local INFO_KEY2 = 0
local RECORD_KEY2 = 1

function CommonShareData:Reset()
	self.tbInfo = { nUsed = 1, nMaxRow = 0 }
	self.tbData = {}
	self.tbMap = {}
end

function CommonShareData:_SaveRecord(row, key, data)
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, data)
	ObjBuffer:PushObject(handle, key)
	OB_SaveShareData(handle, self.szKey, row, RECORD_KEY2)
	OB_Release(handle)
end

function CommonShareData:_SaveInfo()
	local handle = OB_Create()
	ObjBuffer:PushObject(handle, self.tbInfo)
	OB_SaveShareData(handle, self.szKey, INFO_KEY1, INFO_KEY2)
	OB_Release(handle)
end

function CommonShareData:_LoadRecord(nRow)
	local h = OB_Create()
	OB_LoadShareData(h, self.szKey, nRow, RECORD_KEY2)
	if OB_IsEmpty(h) == 0 then
		local data = ObjBuffer:PopObject(h)
		local key = ObjBuffer:PopObject(h)
		OB_Release(h)
		return key, data
	end
	OB_Release(h)
	return nil, nil
end

function CommonShareData:_Load()
	local handle = OB_Create()
	OB_LoadShareData(handle, self.szKey, INFO_KEY1, INFO_KEY2)
	local tbInfo = nil
	if OB_IsEmpty(handle) == 0 then
		tbInfo = ObjBuffer:PopObject(handle)
	end
	OB_Release(handle)

	if tbInfo then
		local tbData = {}
		local tbMap = {}
		for i = 1, (tbInfo.nMaxRow or 0) do
			local key, data = self:_LoadRecord(i)
			if key and (not tbMap[key]) then
				tbData[key] = data
				tbMap[key] = i
			else
				if not key then
					OutputMsg(format("[CommonShareData] doc ban ghi hong [%s] dong %d", self.szKey, i))
				else
					OutputMsg(format("[CommonShareData] khoa trung: [%s] trong [%s]", tostring(key), self.szKey))
				end
				return tbInfo, nil, nil
			end
		end
		return tbInfo, tbData, tbMap
	end
	return nil, nil, nil
end

function CommonShareData:LoadData()
	local tbInfo, tbData, tbMap = self:_Load()
	if tbInfo and tbData and tbMap then
		self.tbInfo = tbInfo
		self.tbData = tbData
		self.tbMap = tbMap
	elseif not tbInfo then
		self:Reset()
	else
		return 0
	end
	return 1
end

-- Dat key -> data. Co roi thi ghi de. Tra ve 1 = xong, 0 = hong.
function CommonShareData:SetData(key, data)
	local t = type(key)
	if (not self.tbInfo) or ((t ~= "number") and (t ~= "string")) then
		return 0
	end

	local nRow = self.tbMap[key]
	local bSaveInfo = false
	if not nRow then
		nRow = self.tbInfo.nMaxRow + 1
		self.tbInfo.nMaxRow = nRow
		self.tbMap[key] = nRow
		bSaveInfo = true
	end

	self.tbData[key] = data
	self:_SaveRecord(nRow, key, data)
	if bSaveInfo then
		self:_SaveInfo()
	end
	return 1
end

-- Lay data cua key (nil = khong co). Sua xong PHAI goi SetData de luu lai.
function CommonShareData:GetData(key)
	if (not self.tbInfo) or (not self.tbData) then
		return nil
	end
	return self.tbData[key]
end

function CommonShareData:SaveDataInTable(tb)
	if type(tb) ~= "table" or not self.tbInfo then
		return nil
	end
	for key, data in pairs(tb) do
		self:SetData(key, data)
	end
	return 1
end

function CommonShareData:GetAllData()
	if not self.tbInfo then
		return nil
	end
	local tb = {}
	for k, v in pairs(self.tbData) do
		tb[k] = v
	end
	return tb
end

function CommonShareData:ClearData()
	self:Reset()
	self:_SaveInfo()
end

function CommonShareData:OutputAll()
	for k, v in pairs(self.tbData) do
		OutputMsg(format("[%s] = %s", tostring(k), tostring(v)))
	end
end

-- Tao mot kho du lieu ben vung tren relay. szTypeName phai DUY NHAT toan he.
function NewCommonShareData(szTypeName)
	if tbKeyList[szTypeName] then
		OutputMsg(format("[CommonShareData] ShareKey trung: [%s]", szTypeName))
		return nil
	end

	local tb = {}
	for k, v in pairs(CommonShareData) do
		tb[k] = v
	end

	tb.szKey = szTypeName
	if tb:LoadData() ~= 1 then
		return nil
	end
	tbKeyList[szTypeName] = 1
	return tb
end
