IncludeLib("NPCINFO")

Include("\\script\\missions\\tongcastle\\game.lua")
Include("\\script\\missions\\tongcastle\\guideperson.lua")
Include("\\script\\lib\\common.lua")
Include("\\script\\lib\\objbuffer_head.lua")
Include("\\script\\lib\\awardtemplet.lua")
Include("\\script\\misc\\eventsys\\type\\npcdeath.lua")
Include("\\script\\global\\autoexec_head.lua")
Include("\\script\\lib\\log.lua")

local tbMapList = {
	[981] = {4, 2, 37,},
	[984] = {7, 2, 176,},
}

local tbTreePrice = {
	[1] = {
		[1] = 15000000,
		[2] = 35000000,
		[3] = 180000000,
	},
	[2] = {
		[1] = 22500000,
		[2] = 52500000,
		[3] = 270000000,
	},
}

function OnDeathEx(nNpcIndex, nAttackerIndex, nBelongIndex)
	local nX32,nY32, nMapIndex = GetNpcPos(nNpcIndex)
	local nMapId = SubWorldIdx2ID(nMapIndex)
	local nNpcParam = GetNpcParam(nNpcIndex, 1)
	local nDir = GetNpcParam(nNpcIndex, 2)
	if TongCastle:FindNpc(nMapId, nNpcIndex, GetNpcId(nNpcIndex)) ~= 1 then
		print("Npc Death Error!!!!!!!!!!")
		return
	end 
	
	TongCastle:UnRegANpc(nMapId, nNpcParam, nDir, nNpcIndex, GetNpcId(nNpcIndex))
	TongCastle:UpdateObstacleObj(nMapId)

	local nType = %tbMapList[nMapId][2]
	local nExp = %tbTreePrice[nType][nNpcParam]
	local szTongName = ""
	local szName = ""

	if nAttackerIndex and nAttackerIndex > 0 then
		local nJoinTime = CallPlayerFunction(nAttackerIndex, GetJoinTongTime)
		print("JoinTime:"..tostring(nJoinTime))
		if nJoinTime > %JOIN_TONG_TIME then
			szTongName = CallPlayerFunction(nAttackerIndex, GetTongName)
			szName = CallPlayerFunction(nAttackerIndex, GetName)
		end
	end
	
	if szName then
		Msg2Map(nMapId, format("<color=yellow>%s ®¸nh ng· 1 c©y %s<color>", szName, GetNpcName(nNpcIndex)))
	end
	
	if 1 <= nNpcParam and nNpcParam <= 3 then
		local handle = OB_Create()
		ObjBuffer:PushObject(handle, nMapId)
		ObjBuffer:PushObject(handle, nNpcParam)
		ObjBuffer:PushObject(handle, nX32)
		ObjBuffer:PushObject(handle, nY32)
		ObjBuffer:PushObject(handle, nDir)
		ObjBuffer:PushObject(handle, szTongName)
		ObjBuffer:PushObject(handle, szName)
		RemoteExecute("\\script\\mission\\tongcastle\\tongcastle.lua", "tbS3TongCastle:DelOneTreeData", handle)
		OB_Release(handle)
		
		TongCastle:BroadcastTreeDeath(nMapId, nNpcParam, nDir)
	end
	
	local szTreeName = GetNpcName(nNpcIndex)	
	tbLog:PlayerActionLog("LOGTINHNANGJX1_npc_BHTB","BHTBKillTree",szTreeName )
end

function CheckNearTreeNpcEx(nNpcIndex)
	local tbNpc, nCount = GetNpcAroundNpcList(nNpcIndex, 15)
	local nRes = 0 
	for i = 1, nCount do 
		local nNpcParam = GetNpcParam(tbNpc[i], 1)
		local nTmpCamp = GetTmpCamp(tbNpc[i])
		if  nNpcParam == 6 then
			nRes = nRes + 1
		end
	end
	return nRes
end

function OnTimer(nNpcIndex)
	local nTime = GetNpcParam(nNpcIndex, 3) or 0
	if nTime < 3 then
		local nNpcCount = CheckNearTreeNpcEx(nNpcIndex)
		if nTime == 0 and nNpcCount > 0 then
			local nX32, nY32, _ = GetNpcPos(nNpcIndex)
			NpcCastSkill(nNpcIndex, 93, 1, nX32, nY32)
		end
		local nBoold = floor(nNpcCount * 1000 / 3)
		local nCurLife = NPCINFO_GetNpcCurrentLife(nNpcIndex)
		NPCINFO_SetNpcCurrentLife(nNpcIndex, nCurLife+nBoold)
		nTime = nTime + 1
		SetNpcParam(nNpcIndex, 3, nTime)
		SetNpcTimer(nNpcIndex, 1*18)
	else
		SetNpcParam(nNpcIndex, 3, 0)
		SetNpcTimer(nNpcIndex, 10*18)
	end
end

function RegisterNpc()
	for szNpcName, _ in TongCastle.tbNpcTypeList do	
		local szTmpNpcName = szNpcName
		if NpcName2Replace then
			szTmpNpcName = NpcName2Replace(szNpcName)
		end 
		if szTmpNpcName then
			EventSys:GetType("NpcDeath"):Reg(szTmpNpcName, OnDeathEx)
		end
	end
end

AutoFunctions:Add(RegisterNpc)

-- [TONGCASTLE 23/08] engine JX1 goi OnDeath(nNpcIdx, nLastDamageIdx) cua ActionScript khi NPC bi nguoi
-- choi giet (KNpc.cpp) - Linux dung EventSys NpcDeath theo TEN (RegisterNpc, khong chay tren JX1).
-- Kho dem cay/ve binh song trong STATE CHU -> goi sang do (boi canh = ke giet).
function OnDeath(nNpcIndex, nDamageIdx)
	local nAtk = NpcIdx2PIdx(nDamageIdx)
	if nAtk and nAtk > 0 then
		PlayerIndex = nAtk	-- [phan bien F23] engine khong dat boi canh khi goi OnDeath cua NPC - de tbLog ghi dung ke giet
		DynamicExecuteByPlayer(nAtk, "\\script\\missions\\tongcastle\\tongcastle.lua", "TongCastle:JX1_TreeDeath", nNpcIndex)
	end
	tbLog:PlayerActionLog("LOGTINHNANGJX1_npc_BHTB","BHTBKillTree", GetNpcName(nNpcIndex))
end
