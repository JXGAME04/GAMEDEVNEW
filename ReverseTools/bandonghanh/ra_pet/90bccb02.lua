if MODEL_GAMECLIENT ~= 1 then
	return
end

Include("\\script\\petsys\\common.lua")
Include("\\script\\protocol.lua")

function OnDelete()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_DELETE)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnChangeName()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_CHANGE_NAME)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnLevelUp()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_LEVEL_UP)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnChangeFeature()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_CHANGE_FEATURE)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnSummon()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_SUMMON)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnUnSummon()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_UNSUMMON)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnTame()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_TAME)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnXiuzhen()
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, PET_OPERATION_XIUZHEN_POINT)
	ScriptProtocol:SendData(PET_PROTOCOL, handle)
	OB_Release(handle)
end

function OnCompanion()
	local handle = OB_Create()
	ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_OPEN_COMPANIONEQUIP", handle)
	OB_Release(handle)
end

function GetUpgradePointTips()
	local szTip = format("<color=yellow>%-20s%-46s%-16s\n<color>", "HÖ thèng", "§iÒu kiÖn tÝnh n¨ng","Møc ®é hoµn thµnh")
	
	for i = 1, getn(EVENT_LIST) do
		local tbData = EVENT_LIST[i]
		local bFinish = IsEventFinished(i)
		local szColor = "red"
		if bFinish == 1 then
			szColor = "green"
		end
		szTip = szTip..format("%-20s%-60s%-20s", tbData.szName, tbData.szCondition, format("<color=%s>%d/1<color>\n", szColor, bFinish))
	end
	
	return szTip
end

function OnTipsShow(szName)
	if szName == "UpgradePoint" then
		return GetUpgradePointTips()
	elseif szName == "GrownPoint" then
		return "Cã thÓ th«ng qua tÝnh n¨ng trång c©y nhËn ®­îc t¸o, mÝa, b¾p, khoai lang \n phÝm ph¶i sö dông cho b¹n ®ång hµnh ¨n nhËn \n"
	elseif szName == "TamePoint" then
		return "Cã thÓ truyÒn cho b¹n ®ång hµnh kinh nghiÖm nhËn ®­îc, mçi ngµy cã thÓ truyÒn 25 lÇn"
	elseif szName == "XiuzhenPoint" then
		return "Cã thÓ nhËn ®­îc th«ng qua truyÒn ®iÓm ch©n nguyªn cho b¹n ®ång hµnh, 150 ®iÓm ch©n nguyªn ®æi ®­îc 1 ®iÓm tu ch©n"
	end
	return ""
end

function s2c_OpenRebuildCompanionEquipUI()
	OpenRebuildCompanionEquipUI()
end

function DoRebuild(nItemId, nRet)
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nItemId)
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nRet)
	print(nItemId, nRet)
	ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_DO_REBUILD_COMPANIONEQUIP", handle)
	OB_Release(handle)
end

function s2c_DoRebuild()
    CompanionEquip_UpdateTmpAttrib(0, {})
end

function TryRebuild(nItemId)
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nItemId)
	ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_TYR_REBUILD_COMPANIONEQUIP", handle)
	OB_Release(handle)
end

function s2c_TryRebuild(nCount, tbAttrib)
	CompanionEquip_UpdateTmpAttrib(nCount, tbAttrib)
end

function EquipCousume(nItemId)
	local handle = OB_Create()
	ObjBuffer:PushByType(handle, OBJTYPE_NUMBER, nItemId)
	ScriptProtocol:SendData("emSCRIPT_PROTOCOL_C2S_ASKCONSUME_COMPANIONEQUIP", handle)
	OB_Release(handle)
end

function s2c_SetCompanionEquipCostTip(szCost)
	SetCompanionEquipCostTip(szCost)
end