
if 1 ~= MODEL_GAMECLIENT then
	return
end
Include("\\script\\ui\\uicontrols\\controls.lua")
local nPak = usepack(0)
IncludeLib("FILESYS")
Include("\\script\\lib\\common.lua")
Include("\\script\\ui\\uidefine.lua")
Include("\\script\\ui\\msgdefine.lua")



UiManage.UIGROUP_MAIN_WND = "Main"

UiManage.tbClass = {}

UiManage.tbDropType = {
	DROP_FORBID = 0,
	DROP_TEMP = 1,
	DROP_SAVEITEM = 2,
}

UiManage.tbPickType = {
	PICK_FORBID = 0,
	PICK_TEMP = 1,
	PICK_SAVEITEM = 2,
}

function UiManage:GetConfigByUiGroup(szUiGroup)
	return self.UI_DEFINE[szUiGroup]
end

function UiManage:GetUiGroupByConfig(szTgtConfig)
	for szUiGroup, szConfig in self.UI_DEFINE do
		if szConfig == szTgtConfig then
			return szUiGroup
		end
	end
end

function UiManage:GetAllUiGroup()
	local tbUiGroupList = {}

	for szUiGroup, _ in self.UI_DEFINE do
		tbUiGroupList[getn(tbUiGroupList) + 1] = szUiGroup
	end

	return tbUiGroupList
end

function UiManage:LoadAllUiGroup()
	for szUiGroup, szConfig in self.UI_DEFINE do
		if self.UI_LAYER[szUiGroup] then
			local szLayer = self.UI_LAYER[szUiGroup];
			LoadUiGroup(szUiGroup, szConfig, GetLayerNumber(szLayer));
		else
			LoadUiGroup(szUiGroup, szConfig)
		end

	end

	for _, tbClass in self.tbClass do
		if tbClass.OnCreate then
			local nCurPack = nil;
			if tbClass.nPak then
				nCurPack = usepack(tbClass.nPak);
			end
			call(tbClass.OnCreate, {tbClass}, "x")
			if nCurPack then
				usepack(nCurPack)
			end
		end
	end
end

function UiManage:ReleaseAllUiGroup()
	for szUiGroup, _ in self.tbClass do
		self:CallGroupFunction(szUiGroup, "OnDestroy")
	end
end

function UiManage:ReleaseUiGroup(szGroupName)

	self:CallGroupFunction(szGroupName, "OnDestroy")

end

function UiManage:RegisterClass(tbClass)

	local szUiGroup = tbClass.UIGROUP

	if MODEL_EXPERIENCE and self.UI_BlackList_Experience[szUiGroup] then
		return nil
	end

	if not szUiGroup then
		-- WriteLog("UiManage:RegisterClass Err No UiGroup")
		return nil
	end

	if self.tbClass[szUiGroup] then
		-- WriteLog(format("UiManage:RegisterClass Err Duplicate UiGroup %s", szUiGroup))
		return nil
	end

	self.tbClass[szUiGroup] = tbClass
end

function UiManage:GetClass(szUiGroup)
	local tb = self.tbClass[szUiGroup]
	if not tb then 
		tb = {UIGROUP=szUiGroup}
		self:RegisterClass(tb)
	end
	return tb
end

function UiManage:OpenWindow(szUiGroup, ...)

	if MODEL_EXPERIENCE and self.UI_BlackList_Experience[szUiGroup] then
		return nil
	end

	if not szUiGroup then
		return 0
	end

	local tbClass = self:GetClass(szUiGroup)

	if not tbClass then
		print("not tbClass")
		return 0
	end

	local nTmpPak = nil
	if tbClass._nPak then
		nTmpPak = usepack(tbClass._nPak)
	end

	if 1 == self:IsWindowVisible(szUiGroup) then
		print("is visible")
		return 0
	end

	local bResult = 1
	local nCurPack = nil;
	if tbClass.nPak then
		nCurPack = usepack(tbClass.nPak);
	end

	-- {tbClass, unpack(arg)}这种写法似乎有问题
	local tbArgs = {tbClass};
	for i = 1, getn(arg) do
		tinsert(tbArgs, arg[i])
	end


	if 1 == bResult and tbClass.PreOpen then
		bResult = call(tbClass.PreOpen, tbArgs, "x") or 0
	end

	if 1 == bResult and tbClass.OnOpen then
		bResult = call(tbClass.OnOpen, tbArgs, "x") or 0
	end

	if 1 == bResult then
		Wnd_Show(szUiGroup, self.UIGROUP_MAIN_WND)
		Wnd_BringToTop(szUiGroup, self.UIGROUP_MAIN_WND)

		UiSoundPlay(self.SOUND_DEFINE.WND_OPENCLOSE)

		if tbClass.PostOpen then
			call(tbClass.PostOpen, tbArgs, "x")
		end
	end

	if nCurPack then
		usepack(nCurPack)
	end

	if 1 ~= bResult then
		self:CloseWindow(szUiGroup)
	end

	if nTmpPak then
		usepack(nTmpPak)
	end
	return bResult
end

function UiManage:CloseWindow(szUiGroup, ...)
	if not szUiGroup then
		return 0
	end

	local tbClass = self:GetClass(szUiGroup)

	if not tbClass then
		return 0
	end

	local nTmpPak = nil
	if tbClass._nPak then
		nTmpPak = usepack(tbClass._nPak)
	end
	if 1 ~= self:IsWindowVisible(szUiGroup) then
		return 0
	end

	local bResult = 1

	local nCurPack = nil;
	if tbClass.nPak then
		nCurPack = usepack(tbClass.nPak);
	end

	local tbArgs = {tbClass};
	for i = 1, getn(arg) do
		tinsert(tbArgs, arg[i])
	end

	if 1 == bResult and tbClass.PreClose then
		bResult = call(tbClass.PreClose, tbArgs, "x")
	end

	if 1 == bResult and tbClass.OnClose then
		bResult = call(tbClass.OnClose, tbArgs, "x")
	end

	if 1 == bResult then
		Wnd_Hide(szUiGroup, self.UIGROUP_MAIN_WND)

		if tbClass.PostClose then
			tbClass:PostClose(unpack(arg))
		end
	end

	if nCurPack then
		usepack(nCurPack)
	end
	
	if nTmpPak then
		usepack(nTmpPak)
	end
	return bResult
end

function UiManage:SwitchWindow(szUiGroup, ...)
	if 1 == self:IsWindowVisible(szUiGroup) then
		return self:CloseWindow(szUiGroup, unpack(arg))
	else
		return self:OpenWindow(szUiGroup, unpack(arg))
	end
end

function UiManage:IsWindowVisible(szUiGroup)
	return Wnd_IsVisible(szUiGroup, self.UIGROUP_MAIN_WND)
end

function UiManage:OnMessageWindow(szUiGroup, nMsg, szWnd, ...)
	--print(format("Msg:szUiGroup = %s, nMsg = %d, szWnd = %s", szUiGroup, nMsg, szWnd))

	local tbClass = self:GetClass(szUiGroup)

	if not tbClass then
		return 0
	end

	local szHandler = self.MSG_DEFINE[nMsg]

	if not szHandler then
		return 0
	end

	local fnHandler = tbClass[szHandler]

	if not fnHandler or type(fnHandler) ~= "function" then
		return 0
	end
	
	return fnHandler(tbClass, szWnd, unpack(arg))
end

function UiManage:CheckDropState(szUiGroup, szWnd, nDropItemId)
	local tbClass = self:GetClass(szUiGroup)

	if not tbClass then
		return 0
	end

	local fnHandler = tbClass["CheckDropState"]
	if not fnHandler or type(fnHandler) ~= "function" then
		return 0
	end

	return fnHandler(tbClass, szWnd, nDropItemId)
end

function UiManage:CheckPickState(szUiGroup, szWnd, nPickItemId)
	local tbClass = self:GetClass(szUiGroup)

	if not tbClass then
		return 0, 0
	end

	local fnHandler = tbClass["CheckPickState"]
	if not fnHandler or type(fnHandler) ~= "function" then
		return 0, 0
	end

	return fnHandler(tbClass, szWnd, nPickItemId)
end

function UiManage:GetIfFloatWndsVisible()
	for szUiGroup,_ in UiManage.UI_DEFINE do
		local bContinue = nil
		for i=1,getn(UiManage.UI_WhiteList) do
			if szUiGroup == UiManage.UI_WhiteList[i] then
				bContinue = 1
				break
			end
		end
		if not bContinue and self:IsWindowVisible(szUiGroup) == 1 then
			return 1
		end
	end
	return 0
end

function UiManage:CloseFloatWnds()
	for szUiGroup,_ in UiManage.UI_DEFINE do
		local bContinue = nil
		for i=1,getn(UiManage.UI_WhiteList) do
			if szUiGroup == UiManage.UI_WhiteList[i] then
				bContinue = 1
				break
			end
		end
		if not bContinue and self:IsWindowVisible(szUiGroup) == 1 then
			self:CloseWindow(szUiGroup)
		end
	end
	return
end

function UiManage:OnCommand(szUiGroup, szCommand)
	
	if not (szUiGroup and szCommand) then
		return
	end
	
	print(format("Command:szUiGroup = %s, szCommand = %s", szUiGroup, szCommand))

	if szUiGroup ~= "" then
		local pClass = self.tbClass[szUiGroup]

		if not (pClass and pClass.OnCommand) then
			return
		end

		local nTmpPak = nil
		if pClass._nPak then
			nTmpPak = usepack(pClass._nPak)
		end
		pClass:OnCommand(szCommand);
		usepack(nTmpPak)
	else
		for key, pClass in self.tbClass do
			if pClass and pClass.OnCommand then
				local nTmpPak = nil
				if pClass._nPak then
					nTmpPak = usepack(pClass._nPak)
				end
				pClass:OnCommand(szCommand);
				usepack(nTmpPak)
			end
		end
	end
end


function UiManage:CallGroupFunction(szUiGroup, szFunction, ...)
	if not (szUiGroup and szFunction) then
		return
	end

	local pClass = self.tbClass[szUiGroup]

	local tbArgs = {pClass};
	for i = 1, getn(arg) do
		tinsert(tbArgs, arg[i])
	end

	local ret = nil;

	if pClass and pClass[szFunction] and type(pClass[szFunction]) == "function" then
		local nOldPak = curpack();
		if pClass.nPak then
			nOldPak = usepack(pClass.nPak);
		end
		ret = call(pClass[szFunction], tbArgs, "x")
		usepack(nOldPak)
	end

	return ret
end

usepack(nPak)