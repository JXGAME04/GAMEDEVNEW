Include("\\script\\missions\\tongcastle\\tongcastle.lua")
CastlePlayer = {}

local tbCity = {
	[37]	= 981,
	[176]	= 984,
}

function CastlePlayer:OnLeaveMap()
	SetFightState(0)
	DisabledUseTownP(0)
	DisabledStall(0)
	local szName = GetName()
	local nMapId, _, _ = GetWorldPos()
	TongCastle:DelPlayer2List(szName, nMapId)
	SetDeathScript("")
end

function CastlePlayer:OnEnterMap()
	SetFightState(1)
	DisabledUseTownP(1)--不能回城符
	SetLogoutRV(1)
	DisabledStall(1)	
	local szName = GetName()
	local nMapId, _, _ = GetWorldPos()
	local szTongName = GetTongName()
	SetDeathScript("\\script\\missions\\tongcastle\\castleplayer.lua")
	TongCastle:AddPlayer2List(szName, nMapId, szTongName)
end

function CastlePlayer:GetStayTime()
	return %PlayerFunLib:GetTaskDaily999(%TSK_STAY_TIME)
end

function OnDeath(nLauncherNpc)
	local nKiller = NpcIdx2PIdx(nLauncherNpc)
	if nKiller and nKiller > 0 then
		TongCastle.tbKillCount = TongCastle.tbKillCount or {}	-- [TONGCASTLE 23/08] ban sao state nay chua qua Init()
		local tbKillCount = TongCastle.tbKillCount
		local szName = CallPlayerFunction(nKiller, GetName)
		local nKillCount = tbKillCount[szName]
		if not nKillCount then
			nKillCount = 0
		end
		nKillCount = nKillCount + 1
		tbKillCount[szName] = nKillCount
		EventSys:GetType("TongCastle"):OnPlayerEvent("OnKillPlayer", nKiller, PlayerIndex, nKillCount)
	end
end