Include("\\script\\petsys\\head.lua")
-- [CFGBDH 30/08] hai tep duoi day la LA (khong Include gi).
Include("\\script\\cauhinh\\ch_lib.lua")
Include("\\script\\cauhinh\\ch_chung.lua")
-- [CFGBDH 30/08] Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)
-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.
function BDH_CFG(szKhoa, macdinh)
	if (G_CFG ~= nil) then
		return G_CFG(szKhoa, macdinh)
	end
	return macdinh
end

Include("\\script\\petsys\\lang.lua")
Include("\\script\\lib\\lib_task.lua")

-- [PETSYS 29/08] op 8 ban private VLTK: doi chan nguyen -> diem Tu Chan
ZHENYUAN_RATE = BDH_CFG("BDH_CHANNGUYEN_DOI_1_TUCHAN", 200)
XIUZHEN_MAX = BDH_CFG("BDH_TUCHAN_TOI_DA", 20000)

function PetSys:XiuzhenPointDlg()
	if PET_IsCreate() ~= 1 then
		return
	end
	local nCo = GetTask(TASK_CHANGNGUYENDAN)
	PLOG("Xiuzhen: channguyen=" .. nCo .. " diem=" .. PET_GetXiuzhenPoint())
	local tbOpt = {}
	if PET_GetXiuzhenPoint() < XIUZHEN_MAX and nCo >= ZHENYUAN_RATE then
		tinsert(tbOpt, {format("§æi %d ch©n nguyªn lÊy 1 ®iÓm Tu Ch©n", ZHENYUAN_RATE), self.XiuzhenConfirm, {self}})
	end
	-- [PETKN 31/08] nang cap 4 ky nang bi kip bang diem Tu Chan:
	-- cap N -> N+1 ton N x BIKIP_TUCHAN_MOI_CAP diem, tran BIKIP_MAX_LEVEL
	local i
	for i = 0, 3 do
		local nId = GetTask(5139 + i)
		if nId > 0 and BIKIP_SKILLS[nId] ~= nil then
			local nLv = GetTask(BIKIP_LEVEL_TASK0 + i)
			if nLv < 1 then
				nLv = 1
			end
			if nLv < BIKIP_MAX_LEVEL then
				tinsert(tbOpt, {format("N©ng [%s] cÊp %d lªn %d (tèn %d ®iÓm Tu Ch©n)", BIKIP_SKILLS[nId], nLv, nLv + 1, nLv * BIKIP_TUCHAN_MOI_CAP), self.BiKipNangCap, {self, i}})
			else
				tinsert(tbOpt, {format("[%s] ®· ®¹t cÊp tèi ®a %d", BIKIP_SKILLS[nId], nLv)})
			end
		end
	end
	tinsert(tbOpt, {CANCEL})
	CreateNewSayEx(format("§iÓm Tu Ch©n: %d - ch©n nguyªn: %d", PET_GetXiuzhenPoint(), nCo), tbOpt)
end

-- [PETKN 31/08] nO = 0..3 (o bi kip)
function PetSys:BiKipNangCap(nO)
	local nId = GetTask(5139 + nO)
	if nId <= 0 or BIKIP_SKILLS[nId] == nil then
		return
	end
	local nLv = GetTask(BIKIP_LEVEL_TASK0 + nO)
	if nLv < 1 then
		nLv = 1
	end
	if nLv >= BIKIP_MAX_LEVEL then
		return
	end
	local nGia = nLv * BIKIP_TUCHAN_MOI_CAP
	if PET_GetXiuzhenPoint() < nGia then
		Talk(1, "", format("CÇn %d ®iÓm Tu Ch©n ®Ó n©ng cÊp (ng­¬i ®ang cã %d)", nGia, PET_GetXiuzhenPoint()))
		return
	end
	PET_SetXiuzhenPoint(PET_GetXiuzhenPoint() - nGia)
	SetTask(BIKIP_LEVEL_TASK0 + nO, nLv + 1)
	PET_SetSkill(1, GetTask(5124))	-- ap lai buff tren PET ngay
	PLOG("BiKipNangCap: o=" .. nO .. " skill=" .. nId .. " cap=" .. (nLv + 1) .. " tru=" .. nGia)
	Msg2Player(format("§· n©ng [%s] lªn cÊp %d, ®iÓm Tu Ch©n cßn %d", BIKIP_SKILLS[nId], nLv + 1, PET_GetXiuzhenPoint()))
end

function PetSys:XiuzhenConfirm()
	local nCo = GetTask(TASK_CHANGNGUYENDAN)
	if nCo < ZHENYUAN_RATE then
		return
	end
	if PET_GetXiuzhenPoint() >= XIUZHEN_MAX then
		return
	end
	SetTask(TASK_CHANGNGUYENDAN, nCo - ZHENYUAN_RATE)
	PET_SetXiuzhenPoint(PET_GetXiuzhenPoint() + 1)
	PLOG("Xiuzhen: DOI XONG - con " .. GetTask(TASK_CHANGNGUYENDAN) .. " CN, diem=" .. PET_GetXiuzhenPoint())
	Msg2Player(format("§æi thµnh c«ng! §iÓm Tu Ch©n hiÖn t¹i: %d", PET_GetXiuzhenPoint()))
end
