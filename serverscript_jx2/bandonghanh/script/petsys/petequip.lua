Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")
Include("\\script\\petsys\\petequip_def.lua")

-- [PETSYS 29/08] TRANG BI DONG HANH - port theo bang goc VLTK.
-- 10 o: task 5143..5152 (luu ParticularType cua mon dang deo)
-- 5163: so mon cung bo dang mac (C doc de ap thuoc tinh bo)
PETEQUIP_O_DAU = 5143
PETEQUIP_O_SO = 10
PETEQUIP_O_BOCOUNT = 5163

-- dem so mon cung bo -> ghi 5163 (bo*100 + so mon) cho C doc
function PetEquip_CapNhatBo()
	local tbDem = {}
	local i
	for i = 0, PETEQUIP_O_SO - 1 do
		local nId = GetTask(PETEQUIP_O_DAU + i)
		local tb = PETEQUIP_DEF[nId]	-- bo qua id ngoai bang
		if tb then
			tbDem[tb.nSuit] = (tbDem[tb.nSuit] or 0) + 1
		end
	end
	local nBoTot = -1
	local nSoTot = 0
	for k, v in tbDem do
		if v > nSoTot then
			nSoTot = v
			nBoTot = k
		end
	end
	if nBoTot < 0 then
		SetTask(PETEQUIP_O_BOCOUNT, 0)
	else
		SetTask(PETEQUIP_O_BOCOUNT, nBoTot * 100 + nSoTot)
	end
	return nBoTot, nSoTot
end

-- dung item trang bi = DEO vao dung o (tra mon cu ve tui)
function main(nItemIndex)
	if PET_IsCreate() ~= 1 then
		Talk(1, "", "Ng­¬i ch­a cã b¹n ®ång hµnh")
		return 1
	end
	local nG, nD, nP = GetItemProp(nItemIndex)
	local tb = PETEQUIP_DEF[nP]
	if not tb then
		return 1
	end
	local nO = PETEQUIP_O_DAU + tb.nSlot - 1
	local nCu = GetTask(nO)
	if nCu > 0 then
		AddItem(6, 1, nCu, 1, 0, 0, 0)
	end
	SetTask(nO, nP)
	local nBo, nSo = PetEquip_CapNhatBo()
	Msg2Player(format("B¹n ®ång hµnh ®· trang bÞ [%s] - bé %s %d mãn",
		PETEQUIP_VITRI[tb.nSlot], PETEQUIP_SUIT_TEN[tb.nSuit] or "?", nSo))
	PLOG("PetEquip: deo o=" .. tb.nSlot .. " id=" .. nP .. " bo=" .. nBo .. " so=" .. nSo)
	return
end

-- op 10: cua so trang bi (menu server) - xem / thao / duc lai
function PetSys:EquipRebuildDlg()
	if PET_IsCreate() ~= 1 then
		return
	end
	local tbOpt = {}
	local i
	for i = 1, PETEQUIP_O_SO do
		local nId = GetTask(PETEQUIP_O_DAU + i - 1)
		local tb = PETEQUIP_DEF[nId]
		if tb then
			tinsert(tbOpt, {format("%s: %s - th¸o ra", PETEQUIP_VITRI[i],
				PETEQUIP_SUIT_TEN[tb.nSuit] or "?"), PetSys.EquipThao, {PetSys, i}})
		end
	end
	if getn(tbOpt) == 0 then
		Talk(1, "", "B¹n ®ång hµnh ch­a mÆc trang bÞ nµo (dïng vËt phÈm trang bÞ §ång Hµnh ®Ó mÆc)")
		return
	end
	tinsert(tbOpt, {"§óc l¹i trang bÞ (®ang hoµn thiÖn)", PetSys.EquipDucLai, {PetSys}})
	tinsert(tbOpt, {%CANCEL})
	local nBo, nSo = PetEquip_CapNhatBo()
	CreateNewSayEx(format("Trang bÞ §ång Hµnh - bé %s %d mãn",
		(nBo >= 0 and PETEQUIP_SUIT_TEN[nBo]) or "-", nSo), tbOpt)
end

function PetSys:EquipThao(nSlot)
	local nO = PETEQUIP_O_DAU + nSlot - 1
	local nId = GetTask(nO)
	if nId <= 0 then
		return
	end
	AddItem(6, 1, nId, 1, 0, 0, 0)
	SetTask(nO, 0)
	PetEquip_CapNhatBo()
	Msg2Player(format("§· th¸o trang bÞ ë vÞ trÝ %s", PETEQUIP_VITRI[nSlot]))
end

function PetSys:EquipDucLai()
	Talk(1, "", "§óc l¹i trang bÞ cÇn luËt ®óc cña m¸y chñ gèc - ®ang hoµn thiÖn, ch­a më")
end
