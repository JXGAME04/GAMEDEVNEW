Include("\\script\\petsys\\head.lua")
Include("\\script\\petsys\\lang.lua")

Include("\\script\\global\\securitylock\\security_lock.lua")

function PetSys:DeletePet()
	if PET_IsCreate() ~= 1 then
		return
	end
	
	if %tbSecurityLock:Check(%tbSecurityLock.SECURITY_LOCK_PET) ~= 1 then
		return
	end
	
	-- [JX1 28/08] "#PetSys:Fn()" khong chay (CallFunction chi nhan ham phang
	-- + tham so so - KPlayer.cpp:7106) -> dung CreateNewSayEx cua dailogsys
	local tbOpt = {}
	tinsert(tbOpt, {%CONFIRM, self.ConfirmDelete, {self}})
	tinsert(tbOpt, {%CANCEL})
	CreateNewSayEx(%DELETE_TITLE, tbOpt)
end

function PetSys:ConfirmDelete()
	PET_UnSummon()
	PET_Delete()
	--升级、增长、修炼点数会保留
end
