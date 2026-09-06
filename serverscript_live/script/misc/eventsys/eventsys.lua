

EventSys = {}
EventSys.szName = "EventSys"
EventSys.MapEvnent = {}

EventSys.EventType = {}

function EventSys:NewType(szType)
	local tb = {}
	for key, value in pairs(self) do
		if type(value) == "table" then
			tb[key] = {}
		else
			tb[key] = value
		end
		tb.szName = szType
	end
	self.EventType[szType] = tb
	return tb
end

function EventSys:GetType(szType)
	return self.EventType[szType] or self
end

function EventSys:Reg(szType, varFun, ...) local arg = {n = select("#", ...), ...};
	
	local nPackNo = curpack()
	self.MapEvnent[szType] = self.MapEvnent[szType] or {}
	local nNewId = getn(self.MapEvnent[szType]) + 1
	self.MapEvnent[szType][nNewId] = {varFun, arg, nPackNo}
	return nNewId
end

function EventSys:UnReg(szType, nId)
	if not self.MapEvnent[szType] then
		return
	end
	if not self.MapEvnent[szType][nId] then
		return
	end
	self.MapEvnent[szType][nId] = nil
end

function EventSys:GetProcParam(szType, nId, tbEventParam)
	if not self.MapEvnent[szType] then
		return
	end
	local tbProcParam = self.MapEvnent[szType][nId]
	local tmpParam = {}
	for i=1, getn(tbProcParam[2]) do --有顺序要求，这个第一
		tinsert(tmpParam, tbProcParam[2][i])
	end
	for i=1, getn(tbEventParam) do--有顺序要求，这个第二
		tinsert(tmpParam, tbEventParam[i])
	end
	
	return  tbProcParam[1], tmpParam, tbProcParam[3]
end

function EventSys:OnEvent(szType, ...) local arg = {n = select("#", ...), ...};
	if not self.MapEvnent[szType] then
		return
	end
	for nId=1, getn(self.MapEvnent[szType]) do
		local varFun, tbParam, nPackNo = self:GetProcParam(szType, nId, arg)
		local nOldPack = usepack(nPackNo)
		call( varFun, tbParam)
		usepack(nOldPack)
	end
end

-- [WLLS port 20/08/2026] OnPlayerEvent: ban Linux do ENGINE bom vao
-- (kiem toan cay script goc: 0 dinh nghia Lua, ~40 call site). Ngu nghia:
-- doi PlayerIndex sang nguoi choi roi phat cho moi handler da Reg (tham so
-- luc Reg dat truoc, tham so su kien sau PlayerIndex noi duoi), moi handler
-- chay trong pack cua file dang ky (curpack/usepack).
function EventSys:OnPlayerEvent(szType, nPlayerIndex, ...) local arg = {n = select("#", ...), ...};
	local tbList = self.MapEvnent[szType]
	if (tbList == nil) then
		return
	end
	local nOldIdx = PlayerIndex
	PlayerIndex = nPlayerIndex
	for i = 1, getn(tbList) do
		local tbReg = tbList[i]
		if (tbReg) then
			local nOldPack = curpack()
			usepack(tbReg[3])
			local tbParam = {}
			local nCount = 0
			for j = 1, getn(tbReg[2]) do
				nCount = nCount + 1
				tbParam[nCount] = tbReg[2][j]
			end
			for j = 1, getn(arg) do
				nCount = nCount + 1
				tbParam[nCount] = arg[j]
			end
			call(tbReg[1], tbParam)
			usepack(nOldPack)
		end
	end
	PlayerIndex = nOldIdx
end
