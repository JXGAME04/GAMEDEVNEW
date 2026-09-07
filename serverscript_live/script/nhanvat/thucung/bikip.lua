Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")

-- [PETSYS 29/08] Bi kip Dong Hanh: day pet 1 trong 18 ky nang BI DONG
-- (bang 1670..1687 port tu VLTK). Luu vao task 5139..5142 (4 o).
-- [PETKN 31/08] BIKIP_SKILLS chuyen sang common.lua (xiuzhen.lua dung chung)

function main(nItemIndex)
	if PET_IsCreate() ~= 1 then
		Talk(1, "", "Ng­¬i ch­a cã b¹n ®ång hµnh")
		return 1
	end
	if PET_GetLevel() < EXT_SKILL_OPEN_PET_LEVEL then
		Talk(1, "", format("B¹n ®ång hµnh ®¹t cÊp %d míi häc ®­îc kü n¨ng", EXT_SKILL_OPEN_PET_LEVEL))
		return 1
	end
	-- so o mo theo cap - nguyen van cong thuc VLTK
	local nChoPhep = GetExtSkillCount()
	local nSlot = 0
	local i
	for i = 0, EXT_SKILL_MAX_COUNT - 1 do
		if GetTask(5139 + i) <= 0 then
			nSlot = 5139 + i
			break
		end
	end
	if nSlot == 0 then
		Talk(1, "", format("B¹n ®ång hµnh ®· häc ®ñ %d kü n¨ng", EXT_SKILL_MAX_COUNT))
		return 1
	end
	if nSlot - 5139 + 1 > nChoPhep then
		Talk(1, "", format("CÊp hiÖn t¹i chØ më %d « kü n¨ng (mçi %d cÊp më thªm 1 «)", nChoPhep, EXT_SKILL_GET_NEW_LEVEL))
		return 1
	end
	local tbOpt = {}
	local k
	for k = 1, getn(tbPetSkillIDList) do
		local nId = tbPetSkillIDList[k]
		local bDaHoc = 0
		for i = 0, EXT_SKILL_MAX_COUNT - 1 do
			if GetTask(5139 + i) == nId then
				bDaHoc = 1
			end
		end
		if bDaHoc == 0 then
			tinsert(tbOpt, {BIKIP_SKILLS[nId], PetSys.HocBiKip, {PetSys, nId, nSlot}})
		end
	end
	tinsert(tbOpt, {CANCEL})
	CreateNewSayEx("Chän kü n¨ng muèn d¹y cho b¹n ®ång hµnh", tbOpt)
	return 1
end

function PetSys:HocBiKip(nSkillId, nSlot)
	if GetTask(nSlot) > 0 then
		return
	end
	local tbProp = PET_MIJI_ITEM.tbProp
	if CalcEquiproomItemCount(tbProp[1], tbProp[2], tbProp[3], -1) < 1 then
		Talk(1, "", "Kh«ng cßn BÝ kiÕp trong hµnh trang")
		return
	end
	ConsumeEquiproomItem(1, tbProp[1], tbProp[2], tbProp[3])
	SetTask(nSlot, nSkillId)
	SetTask(BIKIP_LEVEL_TASK0 + nSlot - 5139, 1)	-- [PETKN 31/08] cap khoi diem 1
	PET_SetSkill(1, GetTask(5124))	-- [PETKN 31/08] ap buff len PET ngay
	PLOG("HocBiKip: o=" .. nSlot .. " skill=" .. nSkillId)
	Msg2Player(format("B¹n ®ång hµnh ®· häc ®­îc kü n¨ng [%s]", BIKIP_SKILLS[nSkillId]))
end
