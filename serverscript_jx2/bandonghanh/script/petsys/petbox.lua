Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\petequip_def.lua")

-- [PETSYS 29/08] Ruong trang bi Dong Hanh 1/2/3 (goc 5064..5066):
-- can Chia Khoa Ruong Dong Hanh, mo ra 1 mon ngau nhien cua bo tuong ung.
PETBOX_RUONG_BO = {
	[4929] = 0,	-- Ruong 1 -> bo Bich Huyet
	[4930] = 1,	-- Ruong 2 -> bo Kim Lan
	[4931] = 1,	-- Ruong 3 -> bo Kim Lan (ban goc chua co bo 3 Dan Tam)
}

function main(nItemIndex)
	local nG, nD, nP = GetItemProp(nItemIndex)
	local nBo = PETBOX_RUONG_BO[nP]
	if nBo == nil then
		return 1
	end
	if CalcEquiproomItemCount(6, 1, PETEQUIP_CHIAKHOA, -1) < 1 then
		Talk(1, "", "CÇn Ch×a Khãa R­¬ng §ång Hµnh míi më ®­îc")
		return 1
	end
	local tbCo = {}
	local k
	for k, v in PETEQUIP_DEF do
		if v.nSuit == nBo then
			tinsert(tbCo, k)
		end
	end
	if getn(tbCo) == 0 then
		return 1
	end
	ConsumeEquiproomItem(1, 6, 1, PETEQUIP_CHIAKHOA)
	local nChon = tbCo[random(1, getn(tbCo))]
	AddItem(6, 1, nChon, 1, 0, 0)
	Msg2Player(format("Më r­¬ng nhËn ®­îc trang bÞ §ång Hµnh bé %s", PETEQUIP_SUIT_TEN[nBo]))
	PLOG("PetBox: ruong=" .. nP .. " ra=" .. nChon)
	return
end
