Include("\\script\\petsys\\lang.lua")
Include("\\script\\lib\\log.lua")
Include("\\script\\petsys\\head.lua")


local tbLevelUpNeedItem = {tbProp = {6,1,4875,-1,0,0}, nCount = 1}	-- [29/08] level -1: item AddItem ra mang level khac 1 (do: lv1=0 lvAny=10)

local tbLevelUpAttr = 
{
	[INDEX_STR] = {1,2,3,4,5},
	[INDEX_DEX] = {1,2,3,4,5},
	[INDEX_VIT] = {1,2,3,4,5},
	[INDEX_ENG] = {1,2,3,4,5},
	[INDEX_HP] = {10,20,30,40,50},
	[INDEX_MP] = {10,20,30,40,50},
}

local attrInitValue = 
{
	[INDEX_STR] = {5,6,7,8,9,10},
	[INDEX_DEX] = {5,6,7,8,9,10},
	[INDEX_VIT] = {5,6,7,8,9,10},
	[INDEX_ENG] = {5,6,7,8,9,10},
	[INDEX_HP] = {200,210,220,230,240,250},
	[INDEX_MP] = {200,210,220,230,240,250},
}

function PetSys:LevelUpDlg()
	PLOG("LevelUpDlg: lv=" .. PET_GetLevel() .. " diem=" .. PET_GetUpgradePoint() .. "/" .. PET_GetGrownPoint() .. "/" .. PET_GetTamePoint())
	if PET_GetLevel() + 1 > %MAX_LEVEL then
		Talk(1, "", %LEVEL_LIMIT)
		return
	end
	
	local tbNextLeveldata = self.tbLevelUp[PET_GetLevel() + 1]
	if not tbNextLeveldata then 
		PLOG("LevelUpDlg: tbLevelUp[" .. (PET_GetLevel() + 1) .. "] NIL - dataload chua nap?")
		return 
	end
	PLOG("LevelUpDlg: can " .. tbNextLeveldata[1] .. "/" .. tbNextLeveldata[2] .. "/" .. tbNextLeveldata[3] .. " thuoc=" .. tbNextLeveldata[4])
	PLOG("LevelUpDlg: dem thuoc lv1=" .. (CalcEquiproomItemCount(6,1,4875,1) or -1) .. " lv0=" .. (CalcEquiproomItemCount(6,1,4875,0) or -1) .. " lvAny=" .. (CalcEquiproomItemCount(6,1,4875,-1) or -1))
	
	--如果升级点数不满足
	if PET_GetUpgradePoint() < tbNextLeveldata[1] or PET_GetGrownPoint() < tbNextLeveldata[2] or PET_GetTamePoint() < tbNextLeveldata[3] then
		Talk(1, "", format(%NOT_ENOUGH_POINT,tbNextLeveldata[1], tbNextLeveldata[2], tbNextLeveldata[3]))
		return
	end
	
	--增长药不够
	if not PlayerFunLib:CheckItemInBag(%tbLevelUpNeedItem ,tbNextLeveldata[4], format(%NOT_ENOUGH_LEVEL_UP_ITEM, tbNextLeveldata[4])) then
		return
	end
	
	--二次确认提示升级成功率 
	local szTitle = format(%LEVEL_UP_RATE_TITLE, tbNextLeveldata[4], tbNextLeveldata[5])
	local tbOpt = {}
	tinsert(tbOpt, {%CONFIRM, self.ConfirmLevelUp, {self, tbNextLeveldata[4], tbNextLeveldata[5]}})
	tinsert(tbOpt, {%CANCEL})
	CreateNewSayEx(szTitle, tbOpt)
end

function PetSys:ConfirmLevelUp(nRequireItemCount, nRate)
	if not PlayerFunLib:CheckItemInBag(%tbLevelUpNeedItem ,nRequireItemCount, format(%NOT_ENOUGH_LEVEL_UP_ITEM, nRequireItemCount)) then
		return
	end
	PlayerFunLib:ConsumeEquiproomItem(%tbLevelUpNeedItem, nRequireItemCount)
	if self:GetRandom(nRate) == 1 then
		self:LevelUp()
	else
		Msg2Player(%LEVEL_UP_FAIL)
		%tbLog:PlayerActionLog("PetSys", "LevelUpFail", PET_GetLevel())
	end
end

function PetSys:GetRandom(nRate)
	local rtotal = 10000000
	local rcur=random(1,rtotal);
	local rstep=0;
	rstep=rstep+floor(nRate*rtotal/100);
	if(rcur <= rstep) then
		return 1
	else
		return 0
	end
end

function PetSys:LevelUp()
	PET_SetLevel(PET_GetLevel() + 1)
	self:AddAttrib()
	self:AddSkill()
	Msg2Player(format(%LEVEL_UP_SUCCESS,PET_GetLevel()))
	%tbLog:PlayerActionLog("PetSys", "LevelUpSuccess", PET_GetLevel())
end

function PetSys:AddAttrib()
	local nPetLevel = PET_GetLevel()
	
	if nPetLevel == 1 then
		PET_ClearAttrib()
	end
	
	local tbRandomTable = %tbLevelUpAttr
	if nPetLevel == 2 then
		tbRandomTable = %attrInitValue
	end

	for i=1, ATTRIB_COUNT do 
		local nRandCount = getn(tbRandomTable[i])
		local nValueIndex = random(1, nRandCount)
		local nValue = tbRandomTable[i][nValueIndex]
		PET_AddAttrib(i, nValue)
	end
end

function PetSys:AddSkill()
	local nPetLevel = PET_GetLevel()
	--local nSkill = random(1,4)
	--PET_SetSkill(1, nSkill)
	
	--фi kh玭g cho ng蓇 nhi猲 1/4 skill - Modified By NgaVN - 20130606
	PET_SetSkill(1, GetTask(5124))	-- [JX1] PET_TV_SKILL0; 3061 la o Linux
end
-- [JX1 nan id] item id da nan theo bang JX1: {'CARD': 4874, 'MED': 4875, 'APPLE': 4876, 'SUGAR': 4877, 'MAIZE': 4878, 'POTATO': 4879}
