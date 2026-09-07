-- [29/08] PLOG phai co o MOI state petsys (item script chay state rieng;
-- thieu -> 'attempt to call global PLOG' lam dut ca ham dung item)
if (PLOG == nil) then
	function PLOG(sz)
		appendto("petops.log")
		write("    " .. sz .. "\n")
		writeto()
	end
end
Include("\\script\\petsys\\jx1_compat.lua")
Include("\\script\\misc\\eventsys\\type\\player.lua")
Include("\\script\\petsys\\lang.lua")
Include("\\script\\activitysys\\playerfunlib.lua")
Include("\\script\\dailogsys\\dailogsay.lua")
Include("\\script\\petsys\\common.lua")
IncludeLib("PET")

-- [JX1 28/08] Include khong guard -> moi module Include head lai la
-- PetSys={} RESET, xoa sach ham cac module nap truoc (goc 'moi nut im')
if PetSys == nil then
	PetSys = {}
	PetSys.tbFeature = {}
	PetSys.tbLevelUp = {}
end


PetSys.MapList = {1, 11, 37, 176, 162, 78, 80, 174, 121, 153, 101, 99, 100, 20, 53, 54, 175, 55} --Æß´ó³ÇÊÐ

PetSys.tbProtocolFunction = 
{
	[PET_OPERATION_DELETE] = "DeletePet",
	[PET_OPERATION_SUMMON] = "Summon",
	[PET_OPERATION_UNSUMMON] = "UnSummon",
	[PET_OPERATION_LEVEL_UP] = "LevelUpDlg",
	[PET_OPERATION_CHANGE_NAME] = "ChangeName",
	[PET_OPERATION_CHANGE_FEATURE] = "ChangeFeatureDlg",
	[PET_OPERATION_TAME] = "TransferExp",
	[PET_OPERATION_XIUZHEN_POINT] = "XiuzhenPointDlg",
	[10] = "EquipRebuildDlg",	-- trang bi Dong Hanh
}

PetSys.tbPetSkill = {
	-- [PETKN 31/08] 4 loai skill mac dinh gio la DON DANH (ten khop
	-- skills.txt 1600..1603); loai 2 hiem (10%) dame cao hon 20%.
	[1] ={szSkillName="Tr¶m KÝch", nRate = 30},
	[2] ={szSkillName="Cuång KÝch",nRate = 10},
	[3] = {szSkillName="LiÖt KÝch",nRate = 30},
	[4] = {szSkillName="Ph¸ KÝch", nRate = 30},
}

--Function nhËn ngÉu nhiªn 1 trong 4 loai pet
function PetSys:GivRandomPet(tbItem)
	if tbItem == nil then
		return 0
	end
	local rtotal = 10000000
	local rcur=random(1,rtotal)
	local rstep=0
	for i=1,getn(tbItem) do
		rstep=rstep+floor(tbItem[i].nRate*rtotal/100)
		if(rcur <= rstep) then
			return i
		end
	end
end

--Functin khi login se tù ®éng kiÓm tra  vµ reset task
function PetSys:Login_Restart()	
	local nToday = tonumber(GetLocalDate("%Y%m%d")) or 0
	local nRecordDate = GetTask(3008)
	if nToday == nRecordDate then
		return
	end
	SetTask(3008, nToday)
	SetTask(3007, 0)
end

