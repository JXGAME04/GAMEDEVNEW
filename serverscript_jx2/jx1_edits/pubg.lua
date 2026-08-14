Include("\\script\\tinhnang\\pubg\\pubgutils.lua")
Include("\\script\\lib\\lib_task.lua")

function OnTimer()
	local nW,nX,nY = GetWorldPos()
	if(nW == BIENKINHPUBGWAIT) then
		local nRestTimeBD = GetMSRestTime(MS_PUBG,12)
		if(nRestTimeBD > 18) then
			Msg2Player(format("Con %d giay bat dau PUBG.",floor(nRestTimeBD/18)))
		end
		local nRestTimeKT = GetMSRestTime(MS_PUBG,2)
		if(nRestTimeKT > 18) then
			Msg2Player(format("Con %d giay ket thuc PUBG.",floor(nRestTimeKT/18)))
		end
	else
		StopTimer()
	end
end

function OnMissionTimer() 
	ontime_pubg(12, SubWorld)
end;

function ontime_pubg(nGlbMission, nMap)
	StopMissionTimer(MS_PUBG,12)--tat hen gio so 12
	SetGlbMission(nGlbMission, 0)--set lai chua dang ky cho lan sau
	SubWorld = SubWorldID2Idx(BIENKINHPUBG)
	print(SubWorld.. " "..GetMSPlayerCount(MS_PUBG).." "..MS_PUBG)
	if(GetMSPlayerCount(MS_PUBG) <=PUBG_MIN_PLAYER) then--map nay ko ai tham gia->close
		--DelAllNpc(SubWorld)
		--CloseMission(MS_PUBG) --ket thuc vuot ai khong co nguoi tham gia
		print(format("===> Close PUBG[%d] Player Join <=  so nguoi can thiet <===", nMap))	
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
					-- KickOutSelf()
					-- SetTaskTemp(TMP_MAP_PUBG, 0)
				-- end
			-- end
		SetMission(12,0) --set lai ID Player Doi Truong = 0
		SetMission(13,0) --set lai Npc Count in Map = 0
		DelAllNpc(SubWorld)
		--KickOutSelf()
		CloseMission(MS_PUBG) --ket thuc PUBG khi het 15p
		return 
	else
		print("PUBG bat dau voi "..GetMSPlayerCount(MS_PUBG).." nguoi choi.")
	end
	StartMissionTimer(MS_PUBG, 15, MSTIME_PUBG_KT*60*18)--thoi gian ket thuc PUBG 15p
	start_pubg()
	-- logHoatDong(format("Bat dau PUBG [%d]", nMap))
end