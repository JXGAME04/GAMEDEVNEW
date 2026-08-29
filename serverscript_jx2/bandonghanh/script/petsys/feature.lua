Include("\\script\\lib\\log.lua")
Include("\\script\\petsys\\head.lua")

function PetSys:ChangeFeatureDlg()
	if PET_IsCreate() ~= 1 then 
		return
	end
	
	if GetCashCoin() < %CHANGE_FEATURE_COIN then
		Talk(1, "", format(%NOT_ENOUGH_COIN, %CHANGE_FEATURE_COIN))
		return
	end
	
	local szTitle = %CHANGE_FEATURE_DLG1_TITLE
	local tbOpt = {}
	tinsert(tbOpt, {"Ta muèn söa", self.ChangeFeatureChooseFeatureLevel, {self}})
	tinsert(tbOpt, {"M¾c qu¸, kh«ng söa n÷a"})
	CreateNewSayEx(%CHANGE_FEATURE_DLG1_TITLE, tbOpt)
end

function PetSys:ChangeFeatureChooseFeatureLevel()
	local szTitle = "Chän ngo¹i quan t­¬ng øng víi ®¼ng cÊp"
	local tbOpt = {}
	local nFeatureLevelLimit = ceil(PET_GetLevel() / %PET_LEVEL_STEP)
	
	for nFeatureLevel=1, nFeatureLevelLimit do
		local szFeatureRange = ((nFeatureLevel-1)*%PET_LEVEL_STEP + 1) .. " - " .. (nFeatureLevel*%PET_LEVEL_STEP)
		tinsert(tbOpt, {szFeatureRange, self.ChangeFeatureChooseFeature, {self, nFeatureLevel}})
	end
	tinsert(tbOpt, {%END_DLG})
	CreateNewSayEx(szTitle, tbOpt)
end

function PetSys:ChangeFeatureChooseFeature(nFeatureLevel)
	local szTitle = "Xin lùa chän ngo¹i quan"
	local tbOpt = {}
	local tbFeatureGroup = self.tbFeature[nFeatureLevel]
	if not tbFeatureGroup then return end

	local nFeatureLimit = ceil(PET_GetLevel() / %PET_LEVEL_STEP)
	if nFeatureLevel > nFeatureLimit then return end
	
	for i=1,getn(tbFeatureGroup) do
		tinsert(tbOpt, {tbFeatureGroup[i][1], self.ConfirmChangeFeature, {self, tbFeatureGroup[i][2]}})
	end
	tinsert(tbOpt, {%BACK, self.ChangeFeatureChooseFeatureLevel, {self}})
	tinsert(tbOpt, {%END_DLG})
	CreateNewSayEx(szTitle, tbOpt)
end

function PetSys:ConfirmChangeFeature(nFeatureId)
	if GetCashCoin() < %CHANGE_FEATURE_COIN then
		Talk(1, "", format(%NOT_ENOUGH_COIN, %CHANGE_FEATURE_COIN))
		return
	end
	if PayCoin(%CHANGE_FEATURE_COIN) == 1 then
		if self.summon == 1 then --if PET_IsSummon() == 1 then
			PET_UnSummon()
			self.summon = 0 --test
		end
		PET_SetFeatureId(nFeatureId)
		Msg2Player(%MSG_CHANGE_FEATURE)
		%tbLog:PlayerActionLog("PetSys", "ChangeFeature", PET_GetLevel())
		--Í¬ÐÔºÃÓÑ½çÃæ´¦×öÏàÓ¦ÐÞ¸Ä
	end
end