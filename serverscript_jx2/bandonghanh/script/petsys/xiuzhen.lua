Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")
Include("\\script\\lib\\lib_task.lua")

-- [PETSYS 29/08] op 8 ban private VLTK: doi chan nguyen -> diem Tu Chan
ZHENYUAN_RATE = 200
XIUZHEN_MAX = 20000

function PetSys:XiuzhenPointDlg()
	if PET_IsCreate() ~= 1 then
		return
	end
	local nCo = GetTask(TASK_CHANGNGUYENDAN)
	PLOG("Xiuzhen: channguyen=" .. nCo .. " diem=" .. PET_GetXiuzhenPoint())
	if PET_GetXiuzhenPoint() >= XIUZHEN_MAX then
		Talk(1, "", "§iÓm Tu Ch©n ®· ®¹t tèi ®a")
		return
	end
	if nCo < ZHENYUAN_RATE then
		Talk(1, "", format("CÇn %d ®iÓm ch©n nguyªn ®Ó ®æi 1 ®iÓm Tu Ch©n (ng­¬i ®ang cã %d)", ZHENYUAN_RATE, nCo))
		return
	end
	local tbOpt = {}
	tinsert(tbOpt, {format("§æi %d ch©n nguyªn lÊy 1 ®iÓm Tu Ch©n", ZHENYUAN_RATE), self.XiuzhenConfirm, {self}})
	tinsert(tbOpt, {%CANCEL})
	CreateNewSayEx(format("Ng­¬i ®ang cã %d ®iÓm ch©n nguyªn", nCo), tbOpt)
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
