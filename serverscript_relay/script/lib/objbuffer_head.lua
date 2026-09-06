-- objbuffer_head.lua  (S3Relay)  [RELAYHT 06/09]
-- Port tu D:\ServerLinux\gateway\s3relay\script\lib\objbuffer_head.lua
-- Da chuyen cu phap Lua 4 -> 5.4: "for k,v in tb" -> pairs(tb), clone -> tu viet.
--
-- ObjBuffer dong goi gia tri Lua (so / chuoi / bang / nil) thanh chuoi byte de
-- luu ShareData hoac (dot sau) chuyen qua RemoteExecute giua GameServer va relay.
-- Dinh dang PHAI giong Core\Src\KJx2SharedStore.cpp ben GameServer.

OBJTYPE_NUMBER	= 1
OBJTYPE_STRING	= 2
OBJTYPE_TABLE	= 3
OBJTYPE_NIL		= 4

ObjBuffer = {}

function ObjBuffer:New(handle)
	local tb = {}
	for k, v in pairs(ObjBuffer) do
		tb[k] = v
	end
	tb.m_Handle = handle or OB_Create()
	return tb
end

function ObjBuffer:Destroy()
	if self.m_Handle then
		OB_Release(self.m_Handle)
		self.m_Handle = nil
	end
end

function ObjBuffer:Clear()
	OB_Clear(self.m_Handle)
end

function ObjBuffer:IsEmpty()
	return OB_IsEmpty(self.m_Handle)
end

function ObjBuffer:Push(obj)
	ObjBuffer:PushObject(self.m_Handle, obj)
end

function ObjBuffer:Pop()
	return ObjBuffer:PopObject(self.m_Handle)
end

-- doc mot gia tri ra khoi handle
function ObjBuffer:PopObject(handle)
	local nDataType = OB_PopByte(handle)
	if nDataType == OBJTYPE_NUMBER then
		return ObjBuffer:PopNumber(handle)
	elseif nDataType == OBJTYPE_STRING then
		return ObjBuffer:PopString(handle)
	elseif nDataType == OBJTYPE_TABLE then
		return ObjBuffer:PopTable(handle)
	elseif nDataType == OBJTYPE_NIL then
		return nil
	else
		if nDataType == nil then
			OutputMsg("[ObjBuffer] load[NIL] khong ho tro!")
		else
			OutputMsg("[ObjBuffer] load[" .. tostring(nDataType) .. "] khong ho tro!")
		end
		return nil
	end
end

-- ghi mot gia tri vao handle
function ObjBuffer:PushObject(handle, obj)
	local tname = type(obj)
	if tname == "number" then
		OB_PushByte(handle, OBJTYPE_NUMBER)
		ObjBuffer:PushNumber(handle, obj)
	elseif tname == "string" then
		OB_PushByte(handle, OBJTYPE_STRING)
		ObjBuffer:PushString(handle, obj)
	elseif tname == "table" then
		OB_PushByte(handle, OBJTYPE_TABLE)
		ObjBuffer:PushTable(handle, obj)
	elseif tname == "nil" then
		OB_PushByte(handle, OBJTYPE_NIL)
	else
		OutputMsg("[ObjBuffer] save[" .. tname .. "] khong ho tro!")
	end
end

function ObjBuffer:PopNumber(handle)
	return OB_PopDouble(handle)
end

function ObjBuffer:PushNumber(handle, obj)
	OB_PushDouble(handle, obj)
end

function ObjBuffer:PopString(handle)
	return OB_PopString(handle)
end

function ObjBuffer:PushString(handle, obj)
	OB_PushString(handle, obj)
end

function ObjBuffer:PopTable(handle)
	local count = OB_PopInt(handle)
	local tb = {}
	for i = 1, count do
		local key = ObjBuffer:PopObject(handle)
		local val = ObjBuffer:PopObject(handle)
		if key ~= nil then
			tb[key] = val
		end
	end
	return tb
end

function ObjBuffer:PushTable(handle, tb)
	local count = 0
	for _, _ in pairs(tb) do
		count = count + 1
	end
	OB_PushInt(handle, count)
	for key, val in pairs(tb) do
		ObjBuffer:PushObject(handle, key)
		ObjBuffer:PushObject(handle, val)
	end
end
