Include("\\script\\tinhnang\\pubg\\pubgutils.lua")
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\timertask\\pubgend.lua")

-- player death when in pubg

function OnPlayerDeath(nPlayerIndex,nLastDamageIndex)
dofile("script/tinhnang/pubg/pubgdeath.lua")
	Msg2Region(53, "Player chet")
	
	
	local nW,nX,nY = GetWorldPos()
	PlayerIndex = nPlayerIndex
	
	if nW ~= BIENKINHPUBG then
		return;
	end
	
	--Drop het do
	DropAllPlayerItems() --drop self
	
	
	
	local playerCount = GetSubWorldPlayerCount()
	Msg2Region(53, "Sè ng­êi ch¬i cßn l¹i "..playerCount-1)
	--check khong con ai
	--truong hop nay bi race hay giet nguoi kia xong top cung bi giet
	if playerCount-1 == 0 then
		--reward
		Msg2Region(53, "Ng­êi th¾ng cuéc 3 "..GetName())
		ontime_pubgend()
		
	end
	
	
	local nDesPlayerIdx = NpcIdx2PIdx(nLastDamageIndex)
	--check con lai mot nguoi
	if playerCount-1 == 1 then
		if (nDesPlayerIdx > 0) then -- bi nguoi nay giet
			--reward dest, drop do DropAllPlayerItems(nDesPlayerIdx), kick out KickOutPlayer
			DropAllPlayerItems(nDesPlayerIdx)
			
			local szName = GetName();
			PlayerIndex = nDesPlayerIdx
			
			local szDesName = GetName();
			local nTotalKills = GetPMParam(MS_PUBG, nDesPlayerIdx, MISSION_BR_PARAM_KILLS);
			Msg2Player(format("nTotalKills %d", nTotalKills));
			Msg2Region(53, format("<color=cyan>[%s] ®· ®¸nh b¹i [%s]",szDesName, szName));
			Msg2Region(53, "Ng­êi th¾ng cuéc 2"..GetName())
			ontime_pubgend()
			KickOutSelf()
			PlayerIndex = nPlayerIndex
		else
		--van thuong cho nguoi con lai
		--get player index con lai trong map
			local nLastPlayerIndex = GetSubWorldLastPlayer(PlayerIndex)
			--reward
			PlayerIndex = nLastPlayerIndex
			Msg2Region(53, "Check point 1")
			Msg2Region(53, "Ng­êi th¾ng cuéc 1"..GetName())
			
			KickOutSelf()
			PlayerIndex = nPlayerIndex
			ontime_pubgend()
		end
	
	end
	
	if playerCount-1 > 1 then --cong exp
		Msg2MSAll(MS_PUBG, "Sè ng­êi cßn l¹i: "..playerCount-1)
		if(nDesPlayerIdx <= 0) then --bi quai giet
			-- thong bao chet nham
			local szDesName = GetName();
			Msg2Region(53, format("<color=Red>[%s] bÞ qu¸i giÕt :))", szDesName));
			 
		else
			local oriPlayerIndex = PlayerIndex;
			local szName = GetName();
			PlayerIndex = nDesPlayerIdx;
			local szDesName = GetName();
			--update kills count
			local nTotalKills = GetPMParam(MS_PUBG, nDesPlayerIdx, MISSION_BR_PARAM_KILLS);
			Msg2Player(format("nTotalKills %d", nTotalKills));
			SetPMParam(MS_PUBG, nDesPlayerIdx, MISSION_BR_PARAM_KILLS, nTotalKills+1);
			--gain exp, points for killer
			GainExp(oriPlayerIndex, nDesPlayerIdx);
			GainProp(oriPlayerIndex, nDesPlayerIdx);
			Msg2Region(53, format("<color=cyan>[%s] ®¸nh b¹i [%s]",szDesName, szName));
			PlayerIndex = oriPlayerIndex;
		end
		
	end
	--Kick out to get back to map when joining PUBG
	KickOutSelf()
end

function OnDeath(nDesNpcIndex) --old
	local nPlayerDataIdx = PIdx2MSDIdx(MISSIONID_BR,PlayerIndex);
	local oriPlayerIndex = PlayerIndex;
	Msg2Player(format("oriPlayerIndex %d", oriPlayerIndex));
	Msg2Player(format("nDesNpcIndex %d", nDesNpcIndex));
	local nDesPlayerIdx = NpcIdx2PIdx(nDesNpcIndex);
	Msg2Player(format("nDesPlayerIdx %d", nDesPlayerIdx));
	
	if(nDesPlayerIdx <= 0) then --bi quai giet
		-- thong bao chet nham
		local szDesName = GetName();
		AddNews(format("<color=Red>[%s] b? gi?t thÂ¶m bÂ¹i bÃ«i quÂ¸i thÃ³ :))", szDesName));
		checkPlayerRemain();
		return 
	end
	
	local nDesPlayerDataIdx	= PIdx2MSDIdx(MISSIONID_BR, nDesPlayerIdx);
	Msg2Player(format("nPlayerDataIdx %d nDesPlayerDataIdx %d", nDesPlayerIdx, nDesPlayerDataIdx));
	if(nPlayerDataIdx >= 0 and nDesPlayerDataIdx >= 0) then	--co ton tai.
	-- chet rot do`
	-- tinh toan va cong diem cho nguoi giet tu` do chenh lech giua 2 nguoi.
		local szName = GetName();
		PlayerIndex = nDesPlayerIdx;
		local szDesName = GetName();
		
		--update kills count
		local nTotalKills = GetPMParam(MISSIONID_BR, nDesPlayerDataIdx, MISSION_BR_PARAM_KILLS);
		Msg2Player(format("nTotalKills %d", nTotalKills));
		
		SetPMParam(MISSIONID_BR, nPlayerDataIdx, MISSION_BR_PARAM_KILLS, nTotalKills+1);
		
		--gain exp, points for killer
		GainExp(oriPlayerIndex, nDesPlayerIdx);
		GainProp(oriPlayerIndex, nDesPlayerIdx);
		
		--rot het do
		
		
		
		AddNews(format("<color=cyan>[%s] ®¸nh b¹i [%s]",szDesName, szName));
		checkPlayerRemain();
		
		
		
		--kick death player 
		PlayerIndex = oriPlayerIndex;
		KickOutSelf();
	end
end;

function OnDamage(nDamage)
	local nP1 = MSDIdx2PIdx(1, 1);
	local nP2 = MSDIdx2PIdx(1, 2);
	local nTotalDmg;
	if(PlayerIndex == nP1) then
		nTotalDmg = GetPMParam(1,1,1)+nDamage;
		SetPMParam(1,1,1,nTotalDmg)
	else
		nTotalDmg = GetPMParam(1,2,1)+nDamage;
		SetPMParam(1,2,1,nTotalDmg)
	end
	Msg2Player(format("BÂ¹n bÌƒ sÂ¸t thÂ­Â¬ng %d",nTotalDmg));
end;

function checkPlayerRemain() -- check if only 1 player left
	-- dang test check la logout luon
	-- nhung khi vo game thiet thi nen bat dau va ket thuc o mot map nao do.
	local nBRTotalPlayer = GetMSPlayerCount(MISSIONID_BR, 0);
	if nBRTotalPlayer == 2 then 
	-- con lai 2 nguoi thi nguoi con song kia la nguoi chien thang
	-- logout luon nguoi chien thang hoac move ve map nao do.
	-- tam thoi test bang cach lay nguoi con lai trong game.
		AddNews(format("<color=cyan>Game ket thuc"));
	end
	return
end;

function GainExp(killeeId, killerId)
	-- tinh toan exp 
	PlayerIndex = killeeId;
	local killeeLvl = GetLevel();
	PlayerIndex = killerId;
	local killerLvl = GetLevel();
	
	Msg2Player(format("killeeLvl [%d] killerLvl [%d]", killeeLvl, killerLvl));
	local gainPercent = 0.1;
	local nextExp = GetNextExp();
	local gainExp = gainPercent*nextExp*(killeeLvl/killerLvl);
	Msg2Player(format("nextExp [%d] ", nextExp));
	Msg2Player(format("gainExp [%d] ", gainExp));
	PlayerIndex = killerId;
	--AddPlayerExp(killerId, gainExp);
	AddOwnExp(gainExp);
	return
end;

function GainProp(killeeId, killerId)
	-- tinh toan exp 
	PlayerIndex = killeeId;
	local killeeLvl = GetLevel();
	PlayerIndex = killerId;
	local killerLvl = GetLevel();
	
	Msg2Player(format("killeeLvl [%d] killerLvl [%d]", killeeLvl, killerLvl));
	local gainPercent = 1;
	local propReward = 10;
	local gainProp = gainPercent*propReward*(killeeLvl/killerLvl);
	Msg2Player(format("propReward [%d] ", propReward));
	Msg2Player(format("gainProp [%d] ", gainProp));
	PlayerIndex = killerId;
	AddPropPoint(gainProp);
	return
end;
