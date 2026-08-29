Include("\\script\\petsys\\head.lua")
-- [JX1 28/08] wrapper global de C goi - dat TRUOC cac Include phu de
-- mot Include loi giua chung khong giet wrapper
function PLOG(sz)
	appendto("petops.log")
	write("    " .. sz .. "\n")
	writeto()
end

-- [PETSYS 29/08] ban lai 29 o task pet xuong client - login-sync co the
-- truot khi client con man hinh loading; goi truoc MOI op de cua so luon dung
function PetSys_ResyncClient()
	local i
	for i = 5110, 5138 do
		SetTask(i, GetTask(i))
	end
end

function PetSys_Protocol(nOp)
	appendto("petops.log")
	write(GetLocalDate("%H:%M:%S") .. " op=" .. nOp .. " nguoi=" .. GetName() .. "\n")
	writeto()
	if (PetSys == nil or type(PetSys.ProtocolProcess) ~= "function") then
		Talk(1, "", "He Ban Dong Hanh dang nap thieu module - bao admin xem ScriptError.log")
		return
	end
	PetSys_ResyncClient()
	PetSys:ProtocolProcess(nOp)
end

Include("\\script\\petsys\\dataload.lua")
Include("\\script\\petsys\\summon.lua")
Include("\\script\\petsys\\levelup.lua")
Include("\\script\\petsys\\name.lua")
Include("\\script\\petsys\\feature.lua")
Include("\\script\\petsys\\delete.lua")
Include("\\script\\petsys\\transferexp.lua")	-- [JX1 28/08] Linux nap ca thu muc luc boot; JX1 Include tuong minh
Include("\\script\\petsys\\xiuzhen.lua")
Include("\\script\\petsys\\petequip.lua")

function PetSys:ProtocolProcess(nOperationId)
	if not self.tbProtocolFunction then
		PLOG("dispatch: tbProtocolFunction NIL")
		return
	end
	local szFunction = self.tbProtocolFunction[nOperationId]
	if not szFunction then
		PLOG("dispatch: khong co fn cho op " .. nOperationId)
		return
	end
	PLOG("dispatch: op=" .. nOperationId .. " fn=" .. szFunction .. " type=" .. type(self[szFunction]))
	if type(self[szFunction]) == "function" then
		self[szFunction](self)
		PLOG("dispatch: " .. szFunction .. " xong")
	end
end
