Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")

-- [PETSYS 29/08] TRANG BI PET V1 - thiet ke toi gian khop UI (khong co
-- nguon server private). O: 5143..5148 particular; 5150..5155 pct 80..120;
-- 5157/5158 tong bonus HP/MP; 5160..5165 pct tam khi duc thu.
PETEQUIP_GOC = {
	[4881] = {nSlot = 1, nHp = 500, nMp = 0},
	[4882] = {nSlot = 2, nHp = 300, nMp = 0},
	[4883] = {nSlot = 3, nHp = 400, nMp = 0},
	[4884] = {nSlot = 4, nHp = 0, nMp = 200},
	[4885] = {nSlot = 5, nHp = 200, nMp = 100},
	[4886] = {nSlot = 6, nHp = 0, nMp = 300},
}
PETEQUIP_TEN = {"Vò khÝ", "Nãn", "Y phôc", "§ai", "Giµy", "Hé Phï"}
DUC_PHI_XU = 5

function PetEquip_TinhBonus()
	local nHp = 0
	local nMp = 0
	local i
	for i = 1, 6 do
		local nP = GetTask(5143 + i - 1)
		local tb = PETEQUIP_GOC[nP]
		if tb then
			local nPct = GetTask(5150 + i - 1)
			if nPct < 80 or nPct > 120 then
				nPct = 100
			end
			nHp = nHp + floor(tb.nHp * nPct / 100)
			nMp = nMp + floor(tb.nMp * nPct / 100)
		end
	end
	SetTask(5157, nHp)
	SetTask(5158, nMp)
end

function main(nItemIndex)
	if PET_IsCreate() ~= 1 then
		Talk(1, "", "Ng­¬i ch­a cã b¹n ®ång hµnh")
		return 1
	end
	local nG, nD, nP = GetItemProp(nItemIndex)
	local tb = PETEQUIP_GOC[nP]
	if not tb then
		return 1
	end
	local nO = 5143 + tb.nSlot - 1
	local nCu = GetTask(nO)
	if nCu > 0 then
		AddItem(6, 1, nCu, 1, 0, 0)	-- tra do cu ve tui
	end
	SetTask(nO, nP)
	SetTask(5150 + tb.nSlot - 1, 100)	-- pham chat goc 100%
	PetEquip_TinhBonus()
	Msg2Player(format("§· trang bÞ [%s] cho b¹n ®ång hµnh", PETEQUIP_TEN[tb.nSlot]))
	PLOG("PetEquip: deo slot=" .. tb.nSlot .. " p=" .. nP)
	return
end

-- op 10: cua so duc lai (menu hop thoai server nhu moi he JX1)
function PetSys:EquipRebuildDlg()
	if PET_IsCreate() ~= 1 then
		return
	end
	local tbOpt = {}
	local i
	for i = 1, 6 do
		local nP = GetTask(5143 + i - 1)
		if PETEQUIP_GOC[nP] then
			tinsert(tbOpt, {format("%s - phÈm chÊt %d%%", PETEQUIP_TEN[i], GetTask(5150 + i - 1)), PetSys.EquipTry, {PetSys, i}})
		end
	end
	if getn(tbOpt) == 0 then
		Talk(1, "", "B¹n ®ång hµnh ch­a ®eo trang bÞ nµo (dïng item Trang BÞ §ång Hµnh ®Ó ®eo)")
		return
	end
	tinsert(tbOpt, {%CANCEL})
	CreateNewSayEx(format("Chän trang bÞ muèn ®óc l¹i (phÝ %d xu/lÇn)", DUC_PHI_XU), tbOpt)
end

function PetSys:EquipTry(nSlot)
	if GetTask(251) < DUC_PHI_XU then
		Talk(1, "", format("Kh«ng ®ñ %d xu", DUC_PHI_XU))
		return
	end
	SetTask(251, GetTask(251) - DUC_PHI_XU)
	local nMoi = random(80, 120)
	SetTask(5160 + nSlot - 1, nMoi)
	PLOG("PetEquip: duc thu slot=" .. nSlot .. " pct=" .. nMoi)
	local tbOpt = {}
	tinsert(tbOpt, {format("NhËn phÈm chÊt míi %d%%", nMoi), PetSys.EquipAccept, {PetSys, nSlot}})
	tinsert(tbOpt, {format("§óc tiÕp (thªm %d xu)", DUC_PHI_XU), PetSys.EquipTry, {PetSys, nSlot}})
	tinsert(tbOpt, {"Gi÷ phÈm chÊt cò"})
	CreateNewSayEx(format("KÕt qu¶ ®óc: %d%% (hiÖn t¹i %d%%)", nMoi, GetTask(5150 + nSlot - 1)), tbOpt)
end

function PetSys:EquipAccept(nSlot)
	local nMoi = GetTask(5160 + nSlot - 1)
	if nMoi < 80 or nMoi > 120 then
		return
	end
	SetTask(5150 + nSlot - 1, nMoi)
	SetTask(5160 + nSlot - 1, 0)
	PetEquip_TinhBonus()
	Msg2Player(format("§óc thµnh c«ng! %s phÈm chÊt %d%%", PETEQUIP_TEN[nSlot], nMoi))
end
