Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")
Include("\\script\\lib\\awardtemplet.lua")

function PetSys:TransferExp()
	if PET_IsCreate() ~= 1 then
		return 
	end
	
	local nNeedExp = 60000000
	local nReducExp = 50000000
	local nTransferCount = 25
	
	PLOG("TransferExp: exp=" .. GetExp() .. " daily=" .. (PlayerFunLib:GetTaskDaily(TSK_COUNT_TRANSFER_EXP) or -1))
	if (GetExp() < nNeedExp) then
		Talk(1,"",format(%NOT_ENOUGH_EXP, nNeedExp, nTransferCount))
		PLOG("TransferExp: da goi Talk THIEU EXP")
		return
	end
	
	if (PlayerFunLib:CheckTaskDaily(TSK_COUNT_TRANSFER_EXP,nTransferCount,format(%NOT_ENOUGH_EXP, nNeedExp, nTransferCount),"<") ~= 1) then
			PLOG("TransferExp: chan daily 25 lan/ngay")
			return
	end
	ReduceOwnExp(nReducExp)
	PLOG("TransferExp: THANH CONG +1 TamePoint (tru 50tr exp)")
	PlayerFunLib:AddTaskDaily(TSK_COUNT_TRANSFER_EXP,1)
	
	local nTamePoint = PET_GetTamePoint() + 1
	PET_SetTamePoint(nTamePoint)
	PLOG("TransferExp: sau set PET_Get=" .. PET_GetTamePoint() .. " GetTask5114=" .. GetTask(5114) .. " muon=" .. nTamePoint)
end
