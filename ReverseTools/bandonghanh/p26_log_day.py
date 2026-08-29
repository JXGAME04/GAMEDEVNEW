# -*- coding: utf-8 -*-
r"""[PETSYS 28/08 toi] Log day de bat tan tay vi sao dispatch im:
- ProtocolProcess: log fn + type(self[fn])
- Summon: log IsSummon / mapid / CheckMap
- ChangeName: log GetCashCoin
Ghi vao petops.log. GO SAU NGHIEM THU.
"""
import io
import os

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"


def sua(duong, cu, moi, nhan):
    p = os.path.join(SV, duong)
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if moi.splitlines()[0].strip() in s and "PLOG" in s and nhan in s:
        print("da co:", nhan)
        return
    if cu not in s:
        print("!! khong thay anchor:", nhan)
        return
    s = s.replace(cu, moi, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("VA:", nhan)


# helper log dung chung: them ham PLOG vao protocol file (sau wrapper)
p = os.path.join(SV, r"script\petsys\protocol_process_gs.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "function PLOG(" not in s:
    khoi = ('function PLOG(sz)\r\n'
            '\tappendto("petops.log")\r\n'
            '\twrite("    " .. sz .. "\\' + 'n")\r\n'
            '\twriteto()\r\n'
            'end\r\n\r\n')
    neo = "function PetSys_Protocol(nOp)"
    assert s.count(neo) == 1
    s = s.replace(neo, khoi + neo, 1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("VA: PLOG helper")

# dispatch log
sua(r"script\petsys\protocol_process_gs.lua",
    '''function PetSys:ProtocolProcess(nOperationId)
	if not self.tbProtocolFunction then
		return
	end
	local szFunction = self.tbProtocolFunction[nOperationId]
	if not szFunction then
		return
	end
	if type(self[szFunction]) == "function" then
		self[szFunction](self)
	end
end''',
    '''function PetSys:ProtocolProcess(nOperationId)
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
end''',
    "dispatch log")

# summon log
sua(r"script\petsys\summon.lua",
    '''function PetSys:Summon()
	if PET_IsSummon() == 1 then
		Talk(1, "", %ALREADY_SUMMON)
		return
	end''',
    '''function PetSys:Summon()
	PLOG("Summon: issummon=" .. PET_IsSummon() .. " iscreate=" .. PET_IsCreate())
	if PET_IsSummon() == 1 then
		Talk(1, "", %ALREADY_SUMMON)
		PLOG("Summon: da goi Talk ALREADY")
		return
	end''',
    "summon log 1")

sua(r"script\petsys\summon.lua",
    '''	if self:CheckMap(nMapId) ~= 1 then
		Talk(1, "", %FORBID_SUMMON)
		return
	end
	PET_Summon()''',
    '''	PLOG("Summon: mapid=" .. nMapId .. " chophep=" .. (self:CheckMap(nMapId) or 0))
	if self:CheckMap(nMapId) ~= 1 then
		Talk(1, "", %FORBID_SUMMON)
		PLOG("Summon: da goi Talk FORBID")
		return
	end
	local nKq = PET_Summon()
	PLOG("Summon: PET_Summon tra " .. (nKq or -1))''',
    "summon log 2")

# changename log
sua(r"script\petsys\name.lua",
    '''function PetSys:ChangeName()
	if PET_IsCreate() ~= 1 then
		return
	end''',
    '''function PetSys:ChangeName()
	PLOG("ChangeName: iscreate=" .. PET_IsCreate() .. " xu=" .. (GetCashCoin() or -1))
	if PET_IsCreate() ~= 1 then
		return
	end''',
    "name log")
print("XONG p26")
