Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")
Include("\\script\\petsys\\petequip_def.lua")

-- [PETSYS 30/08] TRANG BI DONG HANH - port theo bang goc VLTK.

-- [PETKN2 01/09] ten thuoc tinh tieng Viet tu settings\petsysttribname.txt
-- (menu Duc lai truoc hien ma so kieu [233]+5000 - chu bao sua)
PETEQUIP_TENMA = nil
function PetEquip_TenMa(nMa)
	if PETEQUIP_TENMA == nil then
		PETEQUIP_TENMA = {}
		local szF = "settingspetsys\attribname.txt"
		if TabFile_Load(szF, szF) == 1 then
			local nRow = TabFile_GetRowCount(szF)
			local i
			for i = 2, nRow do
				local nM = tonumber(TabFile_GetCell(szF, i, 1))
				local szTen = TabFile_GetCell(szF, i, 2)
				if nM ~= nil and szTen ~= nil and szTen ~= "" then
					PETEQUIP_TENMA[nM] = szTen
				end
			end
		end
	end
	return PETEQUIP_TENMA[nMa] or ("thuéc tÝnh " .. nMa)
end

-- roll 3 gia tri thuoc tinh cua mon (theo khoang min..max bang goc)
function PetEquip_Roll(nId, nSlot)
	local tb = PETEQUIP_DEF[nId]
	if not tb then
		return
	end
	local i
	for i = 1, 3 do
		local a = tb.tbAttrib[i]
		local nVal = 0
		if a then
			nVal = random(a[2], a[3])
		end
		SetTask(PETEQUIP_O_ATTRIB + (nSlot - 1) * 3 + i - 1, nVal)
	end
end

function PetEquip_CapNhatBo()
	local tbDem = {}
	local i
	for i = 0, PETEQUIP_O_SO - 1 do
		local nId = GetTask(PETEQUIP_O_DAU + i)
		local tb = PETEQUIP_DEF[nId]
		if tb then
			tbDem[tb.nSuit] = (tbDem[tb.nSuit] or 0) + 1
		end
	end
	local nBoTot = -1
	local nSoTot = 0
	for k, v in pairs(tbDem) do
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

-- dung item trang bi = DEO vao dung o
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
	PetEquip_Roll(nP, tb.nSlot)
	local nBo, nSo = PetEquip_CapNhatBo()
	Msg2Player(format("B¹n ®ång hµnh ®· trang bÞ [%s] - bé %s %d mãn",
		PETEQUIP_VITRI[tb.nSlot], PETEQUIP_SUIT_TEN[tb.nSuit] or "?", nSo))
	PLOG("PetEquip: deo o=" .. tb.nSlot .. " id=" .. nP .. " bo=" .. nBo .. " so=" .. nSo)
	return
end

-- mo ta 3 thuoc tinh hien co cua mot o
function PetEquip_MoTa(nSlot)
	local nId = GetTask(PETEQUIP_O_DAU + nSlot - 1)
	local tb = PETEQUIP_DEF[nId]
	if not tb then
		return ""
	end
	local sz = ""
	local i
	for i = 1, 3 do
		local a = tb.tbAttrib[i]
		if a then
			local v = GetTask(PETEQUIP_O_ATTRIB + (nSlot - 1) * 3 + i - 1)
			sz = sz .. format("%s +%d  ", PetEquip_TenMa(a[1]), v)
		end
	end
	return sz
end

-- op 10: cua so trang bi
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
			tinsert(tbOpt, {PETEQUIP_VITRI[i],
				PetSys.EquipChon, {PetSys, i}})	-- [PETKN2] chi ten o (10 dong du mo ta se tran 512B)
		end
	end
	if getn(tbOpt) == 0 then
		Talk(1, "", "B¹n ®ång hµnh ch­a mÆc trang bÞ nµo")
		return
	end
	tinsert(tbOpt, {CANCEL})
	local nBo, nSo = PetEquip_CapNhatBo()
	CreateNewSayEx(format("Trang bÞ §ång Hµnh - bé %s %d mãn",
		(nBo >= 0 and PETEQUIP_SUIT_TEN[nBo]) or "-", nSo), tbOpt)
end

function PetSys:EquipChon(nSlot)
	local tbOpt = {}
	tinsert(tbOpt, {format("§óc l¹i (tèn 1 KÕt Tinh §ång Hµnh)"), PetSys.EquipDuc, {PetSys, nSlot}})
	tinsert(tbOpt, {"Th¸o ra", PetSys.EquipThao, {PetSys, nSlot}})
	tinsert(tbOpt, {CANCEL})
	CreateNewSayEx(format("%s: %s", PETEQUIP_VITRI[nSlot], PetEquip_MoTa(nSlot)), tbOpt)
end

function PetSys:EquipDuc(nSlot)
	local nId = GetTask(PETEQUIP_O_DAU + nSlot - 1)
	local tb = PETEQUIP_DEF[nId]
	if not tb then
		return
	end
	if CalcItemCount(-1, 6, 1, PETEQUIP_KETTINH, -1) < 1 then
		Talk(1, "", "Kh«ng ®ñ KÕt Tinh §ång Hµnh")
		return
	end
	local szCu = PetEquip_MoTa(nSlot)
	ConsumeItem(-1, 1, 6, 1, PETEQUIP_KETTINH)
	-- roll thu vao o tam 5190.. (khong ghi de gia tri dang mac)
	local tbMoi = {}
	local i
	local szMoi = ""
	for i = 1, 3 do
		local a = tb.tbAttrib[i]
		if a then
			tbMoi[i] = random(a[2], a[3])
			szMoi = szMoi .. format("%s +%d  ", PetEquip_TenMa(a[1]), tbMoi[i])
		end
	end
	PETEQUIP_TAM = tbMoi
	local tbOpt = {}
	tinsert(tbOpt, {"Gi÷ l¹i kÕt qu¶ míi", PetSys.EquipNhan, {PetSys, nSlot}})
	tinsert(tbOpt, {"Tõ bá (gi÷ thuéc tÝnh cò)"})
	CreateNewSayEx(format("Cò: %s\nMíi: %s", szCu, szMoi), tbOpt)
	PLOG("PetEquip duc: o=" .. nSlot .. " cu=" .. szCu .. " moi=" .. szMoi)
end

function PetSys:EquipNhan(nSlot)
	if PETEQUIP_TAM == nil then
		return
	end
	local i
	for i = 1, 3 do
		if PETEQUIP_TAM[i] ~= nil then
			SetTask(PETEQUIP_O_ATTRIB + (nSlot - 1) * 3 + i - 1, PETEQUIP_TAM[i])
		end
	end
	PETEQUIP_TAM = nil
	Msg2Player(format("§óc thµnh c«ng: %s", PetEquip_MoTa(nSlot)))
end

function PetSys:EquipThao(nSlot)
	local nO = PETEQUIP_O_DAU + nSlot - 1
	local nId = GetTask(nO)
	if nId <= 0 then
		return
	end
	AddItem(6, 1, nId, 1, 0, 0, 0)
	SetTask(nO, 0)
	local i
	for i = 1, 3 do
		SetTask(PETEQUIP_O_ATTRIB + (nSlot - 1) * 3 + i - 1, 0)
	end
	PetEquip_CapNhatBo()
	Msg2Player(format("§· th¸o trang bÞ ë vÞ trÝ %s", PETEQUIP_VITRI[nSlot]))
end
