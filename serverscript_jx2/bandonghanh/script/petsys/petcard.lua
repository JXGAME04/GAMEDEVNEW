Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\petsys\\summon.lua")	-- [JX1] main() goi PetSys:Summon()



function main()
	--如果使用等级不够150就提示 返回1
	local nTransLife = ST_GetTransLifeCount()
	if nTransLife < 2  or (nTransLife == 2 and GetLevel() < 150) then 
		Talk(1, "", format(%MSG_NOT_ENOUGH_LEVEL, %MIN_LEVEL))
		return 1
	end

	--如果已有同行好友就提示不能创建
	if PET_IsCreate() == 1 then 
		Talk(1, "", %ALREADY_CREATE_PET)
		return 1
	end
	
	local nFeatureLevel = ceil(1 / %PET_LEVEL_STEP)
	local tbFeature = PetSys.tbFeature[nFeatureLevel]
	local nFeatureIdIndex = random(1, getn(tbFeature))
	
	--Nh薾 ng蓇 nhi猲 pet - Modified By NgaVN - 20130607
	local nRet = PetSys:GivRandomPet(PetSys.tbPetSkill )
	SetTask(5124, nRet)	-- [JX1] PET_TV_SKILL0; 3061 la o Linux
	
	PET_Create(tbFeature[nFeatureIdIndex][2], tbFeature[nFeatureIdIndex][1])
	PetSys:Summon()
	Talk(1, "", format(%ALREADY_SKILL_PET,PetSys.tbPetSkill [nRet].szSkillName))
end
