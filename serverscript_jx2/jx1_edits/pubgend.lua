Include("\\script\\tinhnang\\pubg\\pubgutils.lua")

function OnTimer()
	local nW,nX,nY = GetWorldPos()
	if(nW == BIENKINHPUBG) then
		local nRestTimeBD = GetMSRestTime(MS_PUBG,14)
		if(nRestTimeBD > 18) then
			Msg2Player(format("Cßn %d gi©y sÏ b¾t ®Çu PUBG.",floor(nRestTimeBD/18)))
		end
		local nRestTimeKT = GetMSRestTime(MS_PUBG,2)
		if(nRestTimeKT > 18) then
			Msg2Player(format("Cßn %d gi©y sÏ kÕt thóc PUBG.",floor(nRestTimeKT/18)))
		end
	else
		StopTimer()
	end
end

function OnMissionTimer() 
	if (SubWorld == SubWorldID2Idx(BIENKINHPUBG)) then
		ontime_pubgend()
	end
end;

function ontime_pubgend(nGlbMission, nMap)
	StopMissionTimer(MS_PUBG, 15)--tat hen gio so 13 pubgend.lua
	if(IsMission(MS_PUBG) == 0) then
		return
	end
		local idxtemp = PlayerIndex
		for i=1,GetCountPlayerMax() do
		PlayerIndex = i
			local w, _, _ = GetWorldPos()
			if w == 996 or w == 997  then
			KickOutSelf()
			-- NewWorld(53,1619, 3185)
			SetTaskTemp(TMP_MAP_PUBG, 0)
			end
		end
		 PlayerIndex = idxtemp
	-- local nCount = GetMSPlayerCount(MS_PUBG)	
	-- for dataindex=1, nCount do	
		-- if(GetPMParam(MS_PUBG, dataindex, 0) == 1) then
			-- PlayerIndex = MSDIdx2PIdx(MS_PUBG, dataindex)
			-- print("End PUBG, kick "..PlayerIndex)
			-- KickOutSelf()
			-- SetTaskTemp(TMP_MAP_PUBG, 0)
		-- end
	-- end
	SetMission(12,0) --set lai ID Player Doi Truong = 0
	SetMission(13,0) --set lai Npc Count in Map = 0
	DelAllNpcName(SubWorld, "Wall")
	--StartMissionTimer(MS_PUBG, 10, MSTIME_VUOT_AI_XOANPC*60*18)--thoi gian ket thuc vuot ai 30p
	--KickOutSelf()
	CloseMission(MS_PUBG) --ket thuc PUBG khi het 15p
end