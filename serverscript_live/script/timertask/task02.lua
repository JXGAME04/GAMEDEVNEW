--Author: Fong Ki“u
--Date: 10/07/2021
--Function: MS TËng kim, c´ng thµnh v≠Ót ∂i

Include("\\script\\lib\\lib_task.lua")
Include("\\script\\lib\\lib_map.lua")
Include("\\script\\lib\\lib_server.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
-- [GOHECU 30/08] he cu da go
-- Include("\\script\\tinhnang\\congthanhchien\\lib_ctc.lua")
-- [GOHECU 30/08] he cu da go
-- Include("\\script\\tinhnang\\vuot_ai\\lib_vuotai.lua")
Include("\\script\\log_game\\save_log.lua")
Include("\\script\\header\\tongkim.lua");

function OnTimer()
	StopTimer()
	if(SubWorld == SubWorldID2Idx(MAP_TK_TC)) then	--day la gio hen tu dong day~ ra ngoai chien truong khi o trong doanh trai qua lau
		local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
		if((GetPMParam(MS_TONGKIM, nPlayerDataIdx, 0) == 1) and (GetPMParam(MS_TONGKIM, nPlayerDataIdx, 1) == 0)) then --neu co online va dang kh´ng chi’n Æ u
			if(GetMSIdxGroup(MS_TONGKIM, nPlayerDataIdx) == 1) then --tong
				
				tong_ratran()
			else	--kim
				kim_ratran()
			end
			SetPMParam(MS_TONGKIM,nPlayerDataIdx, 1, 1)		-- dang chien dau
		end
	end	
end

function OnMissionTimer()
	-- [GOHECU 30/08] da go 2 nhanh chet: ID_MAP_CTC (Cong Thanh cu) va MAP_VUOTAI
	-- (Vuot Ai cu). Chi con Tong Kim - hoat dong dang chay.
	if (SubWorld == SubWorldID2Idx(MAP_TK_TC)) then
		ontime_tongkim()
	end
end;

function ontime_tongkim()
	StopMissionTimer(MS_TONGKIM,2)
	local nTongAcc = GetMissionV(M_TICHLUYA)
	local nKimAcc  = GetMissionV(M_TICHLUYB)
	local nSoaiT = GetMissionV(M_ADDNPCA) -- value 0 ch≠a xu t hi÷n 1 Æ∑ xu t hi÷n
	local nSoaiK = GetMissionV(M_ADDNPCB) -- value 0 ch≠a xu t hi÷n 1 Æ∑ xu t hi÷n
	if(nSoaiT ==0 and nSoaiK == 0) then --so∏i ÆÓt 1 sau 30p bæt Æ«u
		if(nTongAcc < nKimAcc and nSoaiT == 0) then --Æi”m sË tËng ›t h¨n so∏i tËng xu t hi÷n tr≠Ìc
			AddSoaiPheTong()
			Msg2MSAll(MS_TONGKIM,"TËng Nguy™n So∏i Æ∑ xu t hi÷n vµ tr˘c ti’p tham chi’n.")
			StartMissionTimer(MS_TONGKIM, 2, 10*1080)--thoi gian nguyen soai ra dot 2 sau 10 phut	
		end
		if(nKimAcc < nTongAcc and nSoaiK == 0) then --Æi”m sË kim ›t h¨n so∏i kim xu t hi÷n tr≠Ìc
			AddSoaiPheKim()
			Msg2MSAll(MS_TONGKIM,"Kim Nguy™n So∏i Æ∑ xu t hi÷n vµ tr˘c ti’p tham chi’n.")
			StartMissionTimer(MS_TONGKIM, 2, 10*1080)--thoi gian nguyen soai ra dot 2 sau 10 phut	
		end
		if(nKimAcc == nTongAcc and nSoaiT ==0 and nSoaiK == 0) then -- Æi”m sË 2 b™n bªng nhau hai so∏i xu t hi÷n
			AddSoaiPheTong()
			AddSoaiPheKim()
			Msg2MSAll(MS_TONGKIM,"Nguy™n So∏i hai phe Æ∑ xu t hi÷n vµ tr˘c ti’p tham chi’n.")		
		end	
	else	--so∏i ÆÓt 2 sau 10 ÆÓt 1
		if(nSoaiT == 0) then --ÆÓt 1 so∏i tËng ch≠a xu t hi÷n
			AddSoaiPheTong()
			Msg2MSAll(MS_TONGKIM,"TËng Nguy™n So∏i Æ∑ xu t hi÷n vµ tr˘c ti’p tham chi’n.")
		end
		if(nSoaiK == 0) then --ÆÓt 1 so∏i phe kim ch≠a xu t hi÷n
			AddSoaiPheKim()
			Msg2MSAll(MS_TONGKIM,"Kim Nguy™n So∏i Æ∑ xu t hi÷n vµ tr˘c ti’p tham chi’n.")
		end
	end
end

function AddSoaiPheTong()
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	--tong soai
	local nNpcIdx
	if(nViTri == 1) then
		nNpcIdx = AddNpcEx3({636},100,{0,1,2,3,4},MAP_TK_TC,1332*32,3442*32,nil,ONDEATHNSOAITK,nil,1,0,LIFE_NGUYEN_SOAI,100,8000,1000,nil,nil,nil,90)
	else
		nNpcIdx = AddNpcEx3({636},100,{0,1,2,3,4},MAP_TK_TC,1570*32,3198*32,nil,ONDEATHNSOAITK,nil,1,0,LIFE_NGUYEN_SOAI,100,8000,1000,nil,nil,nil,90)
	end
	SetNpcValue(nNpcIdx,1)
	AddMSNpc(MS_TONGKIM,nNpcIdx,1)
	SetNpcDmgEx(nNpcIdx,0,0,0,1000*STRONGBOSS_NSTK,0,0)
	SetMission(M_ADDNPCA,1) --Æ∏nh d u xu t hi÷n so∏i phe tËng value 1
	SetMission(M_NPCIDX_SOAITONG, nNpcIdx)
end

function AddSoaiPheKim()
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	--kim soai
	local nNpcIdx
	if(nViTri == 1) then
		nNpcIdx = AddNpcEx3({642},100,{0,1,2,3,4},MAP_TK_TC,1570*32,3198*32,nil,ONDEATHNSOAITK,nil,2,0,LIFE_NGUYEN_SOAI,100,8000,1000,nil,nil,nil,90)
	else
		nNpcIdx = AddNpcEx3({642},100,{0,1,2,3,4},MAP_TK_TC,1332*32,3442*32,nil,ONDEATHNSOAITK,nil,2,0,LIFE_NGUYEN_SOAI,100,8000,1000,nil,nil,nil,90)
	end
	SetNpcValue(nNpcIdx,2)
	AddMSNpc(MS_TONGKIM,nNpcIdx,2)
	SetNpcDmgEx(nNpcIdx,0,0,0,1000*STRONGBOSS_NSTK,0,0)
	SetMission(M_ADDNPCB,1) --Æ∏nh d u xu t hi÷n so∏i phe kim value 1
	SetMission(M_NPCIDX_SOAIKIM, nNpcIdx)
end

-- [GOHECU 30/08] da go ham ontime_congthanh (64 dong) - he cu

-- [GOHECU 30/08] da go ham ontime_vuotai (32 dong) - he cu

RANDOM_POS_KIM={
	{1585,3181},
	{1570,3173},
	{1551,3175},
	{1544,3204},
	{1550,3223},
	{1569,3227},
	{1586,3216},
	{1592,3198}
}

RANDOM_POS_TONG = {
	{1308,3461},
	{1311,3442},
	{1324,3428},
	{1337,3420},
	{1352,3419},
	{1350,3439},
	{1342,3463},
	{1321,3468}
}

function kim_ratran()
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)	
	if(nViTri == 1) then
		local nRanNum = random(1, getn(RANDOM_POS_KIM))
		SetPos(RANDOM_POS_KIM[nRanNum][1], RANDOM_POS_KIM[nRanNum][2])
	else
		local nRanNum = random(1, getn(RANDOM_POS_TONG))
		SetPos(RANDOM_POS_TONG[nRanNum][1], RANDOM_POS_TONG[nRanNum][2])	
	end
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	SetDeathScript(ONDEATHQUANKIM)
	SetFightState(1)
	-- SetCurCamp(2)	
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)	
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)--set online ms tong kim param 0 value 1
	
	--CheckLimited_Join() -- limited 1 account login TONGKIM
end

function tong_ratran()
	local nViTri = GetMissionV(M_VITRI_TRENDUOI)
	if(nViTri == 1) then
		local nRanNum = random(1,getn(RANDOM_POS_TONG))
		SetPos(RANDOM_POS_TONG[nRanNum][1],RANDOM_POS_TONG[nRanNum][2])		
	else
		local nRanNum = random(1,getn(RANDOM_POS_KIM))
		SetPos(RANDOM_POS_KIM[nRanNum][1],RANDOM_POS_KIM[nRanNum][2])			
	end
	local nPlayerDataIdx = PIdx2MSDIdx(MS_TONGKIM,PlayerIndex)
	SetDeathScript(ONDEATHQUANTONG)
	SetFightState(1)	
	-- SetCurCamp(1)
	SetProtectTime(18*3)
	AddSkillState(963, 1, 0, 18*3)
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,0,1)--set online ms tong kim param 0 value 1
	-- SetPMParam(MS_TONGKIM,nPlayerDataIdx,1,1)--set online ms tong kim param 0 value 1
	--CheckLimited_Join() -- limited 1 account login TONGKIM
end