Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")

-- [PETSYS 29/08] Bi kip Dong Hanh: day pet 1 trong 18 ky nang BI DONG
-- (bang 1670..1687 port tu VLTK). Luu vao task 5139..5142 (4 o).
BIKIP_SKILLS = {
	[1670] = "¤n hßa",
	[1671] = "Anh Dòng",
	[1672] = "C­êng C«ng",
	[1673] = "Tinh ChuÈn",
	[1674] = "C­¬ng Ng¹nh",
	[1675] = "Xuyªn ThÝch",
	[1676] = "ChÝ MËt",
	[1677] = "Viªn NhuËn",
	[1678] = "Kiªn c­êng",
	[1679] = "§o¹n LiÖt",
	[1680] = "æn Cè",
	[1681] = "ThiÓm Quang",
	[1682] = "Phóc Quang",
	[1683] = "Kiªn NhËn",
	[1684] = "Cao N¨ng",
	[1685] = "Khinh Doanh",
	[1686] = "Phôc T«",
	[1687] = "ThuÇn TÞnh"
}

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
	tinsert(tbOpt, {%CANCEL})
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
	PLOG("HocBiKip: o=" .. nSlot .. " skill=" .. nSkillId)
	Msg2Player(format("B¹n ®ång hµnh ®· häc ®­îc kü n¨ng [%s]", BIKIP_SKILLS[nSkillId]))
end
