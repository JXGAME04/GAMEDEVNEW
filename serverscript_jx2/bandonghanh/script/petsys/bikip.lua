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
	if PET_GetLevel() < 21 then
		Talk(1, "", "B¹n ®ång hµnh ®¹t cÊp 21 míi häc ®­îc kü n¨ng")
		return 1
	end
	local nSlot = 0
	local i
	for i = 0, 3 do
		if GetTask(5139 + i) <= 0 then
			nSlot = 5139 + i
			break
		end
	end
	if nSlot == 0 then
		Talk(1, "", "B¹n ®ång hµnh ®· häc ®ñ 4 kü n¨ng")
		return 1
	end
	-- so o duoc mo theo cap: (cap-21)/5+1, toi da 4
	local nChoPhep = floor((PET_GetLevel() - 21) / 5) + 1
	if nChoPhep > 4 then
		nChoPhep = 4
	end
	if nSlot - 5139 + 1 > nChoPhep then
		Talk(1, "", format("CÊp hiÖn t¹i chØ më %d « kü n¨ng (5 cÊp më thªm 1 «)", nChoPhep))
		return 1
	end
	local tbOpt = {}
	local nId
	for nId = 1670, 1687 do
		local nCapCu = 0
		local nSlotCu = 0
		for i = 0, 3 do
			local v = GetTask(5139 + i)
			if floor(v / 100) == nId or v == nId then
				nSlotCu = 5139 + i
				nCapCu = v - nId * 100
				if nCapCu < 1 then
					nCapCu = 1
				end
			end
		end
		if nSlotCu == 0 then
			tinsert(tbOpt, {BIKIP_SKILLS[nId], PetSys.HocBiKip, {PetSys, nId, nSlot, 0}})
		elseif nCapCu < 5 then
			tinsert(tbOpt, {format("%s (cap %d)", BIKIP_SKILLS[nId], nCapCu), PetSys.HocBiKip, {PetSys, nId, nSlotCu, nCapCu}})
		end
	end
	tinsert(tbOpt, {%CANCEL})
	CreateNewSayEx("Chän kü n¨ng muèn d¹y cho b¹n ®ång hµnh", tbOpt)
	return 1
end

function PetSys:HocBiKip(nSkillId, nSlot, nCapCu)
	-- nCapCu = 0: hoc moi vao o trong; >0: nang cap skill dang o nSlot
	-- tru 1 Bi kip (dem theo ma, bo level nhu thuoc)
	if CalcEquiproomItemCount(6, 1, 4880, -1) < 1 then
		Talk(1, "", "Kh«ng cßn BÝ kÝp trong hµnh trang")
		return
	end
	local nTruoc = CalcEquiproomItemCount(6, 1, 4880, -1)
	ConsumeEquiproomItem(1, 6, 1, 4880)
	PLOG("HocBiKip: bikip " .. nTruoc .. " -> " .. CalcEquiproomItemCount(6, 1, 4880, -1))
	SetTask(nSlot, nSkillId * 100 + (nCapCu + 1))
	Msg2Player(format("B¹n ®ång hµnh ®· häc ®­îc kü n¨ng [%s]", BIKIP_SKILLS[nSkillId]))
end
