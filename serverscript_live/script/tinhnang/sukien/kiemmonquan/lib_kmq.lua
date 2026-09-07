-- Author: Fong Ki“u
-- Date: 28/11/2020
-- Function: Lib ki’m m´n quan

Include("\\script\\lib\\lib_server.lua")
Include("\\script\\lib\\lib_ham.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_vatpham.lua")

SCRIPTNPCKIEMMONQUAN = "\\script\\event\\kiemmonquan\\congtonthu.lua"
PLAYERDEATHKMQ = "\\script\\event\\kiemmonquan\\playerdeathkmq.lua"
TIME_KMQKT = 60 -- di‘n ra trong thÍi gian 60 phÛt
MAP_KMQ = {
	995,
}
TAB_TIME_KMQ = {
	{18,39},
	--{02,51},
	{19,00},
}

function addnpckiemmonquan()
	local nNpcIdx
	nNpcIdx = AddNpcEx1({203},1,nil,995,206 * 8 * 32, 210 * 16 * 32,nil,SCRIPTNPCKIEMMONQUAN,"C´ng T´n Th≠",6)
	nNpcIdx = AddNpcEx1({203},1,nil,995,222 * 8 * 32, 209 * 16 * 32,nil,SCRIPTNPCKIEMMONQUAN,"C´ng T´n Th≠",6)
	nNpcIdx = AddNpcEx1({203},1,nil,995,225 * 8 * 32, 224 * 16 * 32,nil,SCRIPTNPCKIEMMONQUAN,"C´ng T´n Th≠",6)
	nNpcIdx = AddNpcEx1({203},1,nil,995,209 * 8 * 32, 221 * 16 * 32,nil,SCRIPTNPCKIEMMONQUAN,"C´ng T´n Th≠",6)
end

function GoMap995VT(nVT)
	local nx = 206 * 8
	local ny = 210 * 16
	if(nVT == 2) then
		nx = 222 * 8
		ny = 209 * 16		
	end
	if(nVT == 3) then	
		nx = 225 * 8
		ny = 224 * 16		
	end
	if(nVT == 4) then	
		nx = 209 * 8
		ny = 221 * 16		
	end	
		
		
			
	if(NewWorld(995, nx + random(0, 8), ny + random(0, 8)) > 0) then
		
		local nSubWorldId = SubWorldID2Idx(MAP_KMQ[1])
		SubWorld = nSubWorldId
		local nPlayerDataIdx = PIdx2MSDIdx(MS_KIEMMONQUAN, PlayerIndex)
		if(nPlayerDataIdx <= 0) then
			nPlayerDataIdx = AddMSPlayer(MS_KIEMMONQUAN, 1)
		end
		SetPMParam(MS_KIEMMONQUAN, nPlayerDataIdx, 0, 1)
		SetFightState(1)
		SetPKMode(1, 1) --ep kieu chien dau
		SetPunish(1) --bat tinh nang chet khong mat gi
		SetDeathScript(PLAYERDEATHKMQ)
		--SetRevPos(78, 29)
		SetLogoutRV(1)--dung diem phuc sinh dang nhap
		LeaveTeam()--roi to doi hien tai
		--SetCurCamp(4)
		SetProtectTime(18*3)
		AddSkillState(963, 1, 0, 18*3)			
		Msg2MSAll(MS_KIEMMONQUAN, format("%s Æ∑ Æi vµo Ki’m M´n Quan.", GetName())) --thong bao vao KMQ
	end	
end
TAB_MAPUTNAMEKMQ = { 
	--{1, "Ph≠Óng T≠Íng"},
	--{11, "Thµnh ß´"},
--	{37, "Bi÷n Kinh"},
	{78, "T≠¨ng D≠¨ng"},
	{80, "D≠¨ng Ch©u"},
	{162, "ßπi L˝"},	
--	{176, "L©m an"},
--	{53, "Ba Lang Huyen"},
}

function CheckMapNoFor_KMQ(nMap)
	for i = 1, getn(TAB_MAPUTNAMEKMQ) do
		if (nMap == TAB_MAPUTNAMEKMQ[i][1]) then
			return 1
		end
	end
	return 0
end

function GoMap995()
	local nMap,x,y = GetWorldPos()
	local nSubWorldId = SubWorldID2Idx(MAP_KMQ[1])
	SubWorld = nSubWorldId
	if(CheckMapNoFor_KMQ(nMap) == 0) then
		Talk(1,"","Kh´ng th” tham gia hoπt ÆÈng tπi Æ©y, di chuy”n Æ’n T≠¨ng D≠¨ng, ßπi L˝, D≠¨ng Ch©u.!!!") 
		return
		end
	if(IsMission(MS_KIEMMONQUAN) == 0) then
	Talk(1,"","Hoπt ÆÈng tπi Ki’m M´n Quan 12h vµo 20h mÌi giÍ bæt Æ«u")
		return
	end
	if(GetCamp() == 0) then
		Talk(1,"","T©n thÒ kh´ng th” di chuy”n vµo khu v˘c nµy")
		return
	end
	SayEx({"<color=green><npc>: Ch‰n vﬁ tr› ",
		"Vﬁ tr› 1/#GoMap995VT(1)",
		"Vﬁ tr› 2/#GoMap995VT(2)",
		"Vﬁ tr› 3/#GoMap995VT(3)",
		"Vﬁ tr› 4/#GoMap995VT(4)",
		"Kh´ng Æi/no"})
end
