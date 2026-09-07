-- ================================================================================================
-- [HE THONG] script/timertask/task03.lua
-- Muc dich  : VIEC THEO LICH goi tu timerserver.lua (RunTime): taskNN.lua = nhom viec so NN (task04-07 rong tu 30/08).
-- Duoc nap  : engine nap luc boot (moi tep .lua = 1 lua_State rieng)
-- Include   : tongkim.lua, lib_task.lua, lib_tktc.lua, save_log.lua, mailmanager.lua
-- Ham (dong): OnTimer (8), OnMissionTimer (12), ontime_tongkim (18), PlayerEndTongKim (167)
-- Sua nong  : KHONG - can restart GameServer (hoac lenh GM nap lai script)
-- Quy uoc   : Lua 5.4 + lop tuong thich lua4compat (getn/format/floor/tinsert van dung); KHONG dung %x; duyet bang pairs(); ham '...' khai local arg. Kiem: ReverseTools/lua54/kiem_54.py
-- ================================================================================================
--Author: Fong Ki“u
--Date: 2021
--Function: Player Task TËng Kim
Include("\\script\\header\\tongkim.lua");
Include("\\script\\lib\\lib_task.lua")
Include("\\script\\tinhnang\\tong_kim_tcap\\lib_tktc.lua")
Include("\\script\\log_game\\save_log.lua")

function OnTimer()
	StopTimer()
end

function OnMissionTimer()
	if(SubWorld == SubWorldID2Idx(MAP_TK_TC)) then
		ontime_tongkim()
	end
end

function ontime_tongkim()
	StopMissionTimer(MS_TONGKIM,3)
	local nTongAcc = GetMissionV(M_TICHLUYA)
	local nKimAcc  = GetMissionV(M_TICHLUYB)
	local nCount = GetMSPlayerCount(MS_TONGKIM)
	local nPThuc  = GetMissionV(M_HINHTHUC)
	local nPlayerCamp = 0
	if(nTongAcc > nKimAcc) then
		nPlayerCamp = 1
	elseif(nTongAcc < nKimAcc) then
		nPlayerCamp = 2
	else
		nPlayerCamp = 0
	end
	
	if(nPThuc == PT_BaoVeNguyenSoai) then --b∂o v÷ nguy™n so∏i
		local nSoaiT = GetMissionV(M_ADDNPCA)
		local nSoaiK = GetMissionV(M_ADDNPCB)
		if(nSoaiT == 1 and nSoaiK == 2) then -- 1 cﬂn 2 m t -- so∏i tËng cﬂn so∏i kim m t
			nPlayerCamp = 1
			print("===>Tong Kim: Soai Tong 1 Soai Kim 2 [Tong Win]<===")
		end
		if(nSoaiT == 2 and nSoaiK == 1) then -- 1 cﬂn  2 m t -- so∏i tËng m t so∏i kim cﬂn
			nPlayerCamp = 2
			print("===>Tong Kim: Soai Tong 2 Soai Kim 1 phe [Kim Win]<===")
		end
		if(nSoaiT == 1 and nSoaiK == 1) then -- 1 cﬂn 2 m t --hai phe cﬂn so∏i gi˜ nguy™n k’t qu∂ nh≠ tr™n
			print("===>Tong Kim: hai Phe con Soai tinh diem nhu binh thuong<===")
		end
	end
	local nXutk = 1
	local nTop1 = 20
	local nTop2 = 15
	local nTop3 = 10
	local nTop10 = 5
	
	if(nPThuc == PT_CuuSat) then 
		nXutk = 3
	else
		nXutk = 1
	end
	
	local idx = 0;
	local pidx = 0;
	local players = {};
	while (1) do
		idx, pidx = GetNextPlayer(MS_TONGKIM, idx, 0);
		if (pidx > 0) then
			tinsert(players, pidx);
		end
		if (idx == 0) then
			break;
		end
	end
	local nPlayerIdx = PlayerIndex;
	Msg2SubWorld("<color=cyan>Th≠Îng trÀn TËng Kim nµy :")
	for i = 1, getn(players) do
		PlayerIndex = players[i];
		local dataindex = PIdx2MSDIdx(MS_TONGKIM, PlayerIndex)
		local szPlayerName;
		local nTotalRank = 9

		if nTotalRank > nCount then
			nTotalRank = nCount - 1
		end
		for i = 0, nTotalRank do
			szPlayerName = GetMSLadder(MS_TONGKIM, i)
	--		for j = 1, nCount do
				PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex);
				local nRank = i + 1

				-----------------------------
				if (GetName() == szPlayerName) then
				local nTongExp = 100000000 * nXutk
					AddSumExp(nTongExp)
				local nInra = nTongExp /1000000
					if (nRank == 1) then
						local nTongXu = nTop1 * nXutk
						SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + nTongXu )
						AddSkillState(1552, 50, 1, 60*60*24*18, -1)
						-----------------------------
						Msg2SubWorld("<color=green>Hπng <color>" ..
							nRank ..
							":<color=red> " ..
							GetName() .. " <color><color=yellow> nhÀn " ..nTongXu.." Xu & " ..nInra.. " tri÷u<color> Æi”m kinh nghi÷m!")
						-----------------------------
						logRutXuTK(format("***Account [%s] nh©n vÀt [%s] Rank [%s] nhÀn Æ≠Óc 20 Xu tπi hoπt ÆÈng tËng kim .***", GetAccount(), GetName(), nRank))
					else if (nRank == 2) then
						local nTongXu2 = nTop2 * nXutk
						SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + nTongXu2)
						AddSkillState(1553, 50, 1, 60*60*24*18, -1)
						-----------------------------
						Msg2SubWorld("<color=green>Hπng <color>" ..
							nRank ..
							":<color=red> " ..
							GetName() .. " <color><color=yellow> nhÀn "..nTongXu2.." Xu & " ..nInra.. " tri÷u<color> Æi”m kinh nghi÷m!")
						-----------------------------
						logRutXuTK(format("***Account [%s] nh©n vÀt [%s] Rank [%s] nhÀn Æ≠Óc 15 Xu tπi hoπt ÆÈng tËng kim.***", GetAccount(), GetName(), nRank))
					else if (nRank == 3) then
						local nTongXu3 = nTop3 * nXutk
						SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + nTongXu3)
						AddSkillState(1554, 50, 1, 60*60*24*18, -1)
						-----------------------------
						Msg2SubWorld("<color=green>Hπng <color>" ..
							nRank ..
							":<color=red> " ..
							GetName() .. " <color><color=yellow> nhÀn "..nTongXu3.." Xu & " ..nInra.. " tri÷u<color> Æi”m kinh nghi÷m!")
						-----------------------------
						logRutXuTK(format("***Account [%s] nh©n vÀt [%s] Rank [%s] nhÀn Æ≠Óc 10 Xu tπi hoπt ÆÈng tËng kim .***", GetAccount(), GetName(), nRank))
					else
						local nTongXu10 = nTop10 * nXutk
						SetTask(T_PLAYER_XU, GetTask(T_PLAYER_XU) + nTongXu10)
						-----------------------------
						Msg2SubWorld("<color=green>Hπng <color>" ..
							nRank ..
							":<color=red> " ..
							GetName() .. " <color> <color=yellow> nhÀn "..nTongXu10.." Xu & " ..nInra.. " tri÷u<color> Æi”m kinh nghi÷m!")
						-----------------------------
						logRutXuTK(format("***Account [%s] nh©n vÀt [%s] Rank [%s] nhÀn Æ≠Óc 5 Xu tπi hoπt ÆÈng tËng kim dµnh thæng lÓi.***", GetAccount(), GetName(), nRank, nNum))
						end
					end
				end
			end
		end
--	end
		PlayerEndTongKim(dataindex, nPlayerCamp)
	end
	PlayerIndex = nPlayerIdx;


	
	if(nPlayerCamp == 1) then
		AddGlobalNews(format("ßπi chi’n TËng Kim Æ∑ k’t thÛc. T›ch lÚy TËng %d:%d Kim. Phe TËng giµnh Æ≠Óc thæng lÓi!", nTongAcc, nKimAcc))
	elseif(nPlayerCamp == 2) then
		AddGlobalNews(format("ßπi chi’n TËng Kim Æ∑ k’t thÛc. T›ch lÚy TËng %d:%d Kim. Phe Kim giµnh Æ≠Óc thæng lÓi!", nTongAcc, nKimAcc))
	else
		AddGlobalNews(format("ßπi chi’n TËng Kim Æ∑ k’t thÛc. T›ch lÚy TËng %d:%d Kim. TrÀn nµy Æ∑ hﬂa!", nTongAcc, nKimAcc))
	end
	
	for i=1,20 do
		SetMission(i,0)
	end
	
	CloseMission(MS_TONGKIM)	--dong mission
	DelAllNpc(SubWorld)
	
	-- KickOutSelf()
end

function PlayerEndTongKim(dataindex, nPlayerCamp)
	
	PlayerIndex = MSDIdx2PIdx(MS_TONGKIM, dataindex)
	TK_GuiDiemChoToi(TKDIEM_KIND_AN)	-- [TKDIEM 04/09] an bang diem truoc khi dua nguoi choi roi tran
	local nPhe = GetMSIdxGroup(MS_TONGKIM, dataindex)
	SetPKMode(0, 0)--phuc hoi pk tu do
	SetFightState(0)--phi chien dau
	SetPunish(0)
	SetCreateTeam(1)
	ReSetMask()
	RestoreRunSpeed()
--	RemoveRankBattle()	
	NewWorld(TAB_PHE_TONGKIM[nPhe][6][1], TAB_PHE_TONGKIM[nPhe][6][2], TAB_PHE_TONGKIM[nPhe][6][3])	
	
	local nTotalAccum = 0
	nTotalAccum = GetPMParam(MS_TONGKIM, dataindex , 6)
	local nBattleRank = GetPMParam(MS_TONGKIM, dataindex, 11)
	-- [KM 28/08] chuan Linux 1001\config.lua "TongKim3000NhanNguyenLieu": dat 3000
	-- diem tich luy trong tran -> 2 Chan Nguyen Don (trung). Dung diem GOC cua tran
	-- (nTotalAccum truoc khi cong them thuong phe).
	if nTotalAccum >= 3000 then
		-- [MAIL 03/09 D9] 3000 diem tich luy tran -> 2 Chan Nguyen Don (trung) gui qua thu
		Include("\\script\\mail\\mailmanager.lua")
		MailManager_SendRewardTemplet("tongkim", nil, "Th≠Îng 3000 Æi”m t›ch lÚy TËng Kim", "ßπt tr™n 3000 Æi”m t›ch lÚy trÀn: 2 Ch©n Nguy™n ß¨n (trung) Æ›nh kÃm trong th≠.", {{tbProp = {6, 1, 4846, 0, 0, 0}, nCount = 2, nStack = 1}}, 30)
		Msg2Player("ßπt tr™n 3000 Æi”m t›ch lÚy trÀn: 2 Ch©n Nguy™n ß¨n (trung) Æ∑ gˆi vµo hÈp th≠.")
	end
	if(nPlayerCamp == nPhe) then --nam trong phe chien thang, thuong them tich luy
		local nAccum = TICH_LUY_THUONG_TK_W * nBattleRank
		if(nTotalAccum >= MIN_POINT_AWARD) then
			ThuongPheThangTongKim(nBattleRank)
		end
		nTotalAccum = nTotalAccum + nAccum
	else 
		if(nPlayerCamp ~= 0) then --thua
			local nAccum = TICH_LUY_THUONG_TK_L * nBattleRank
			if(nTotalAccum >= MIN_POINT_AWARD) then
				ThuongPheThuaTongKim(nBattleRank)
			end					
			nTotalAccum = nTotalAccum + nAccum
		else --hoµ
			local nAccum = TICH_LUY_THUONG_TK_H * nBattleRank
			if(nTotalAccum >= MIN_POINT_AWARD) then
				ThuongHaiPheHoaTongKim(nBattleRank)
			end					
			nTotalAccum = nTotalAccum + nAccum
		end
	end
	
	SetTask(T_TONGKIM,GetTask(T_TONGKIM)+nTotalAccum)
	SetTask(T_CHECKPHETK, 0) --h’t trÀn set phe tham gia 0
--	SetTask(T_CHECKDATETK, 0)--h’t trÀn set thÍi gian tham gia 0
	Talk(1, "", "Xin chÛc mıng! <sex> nhÀn Æ≠Óc ph«n th≠Îng <color=yellow>"..nTotalAccum.."<color> Æi”m t›ch lÚy TËng Kim.")
	SetCurCamp(GetCamp())--phuc hoi phe ban dau	
	SetDeathScript("") --set script khi chet	
	RemoveRankBattle()
	IgnoreState()	--xoa het trang thai skill tren nguoi
	SetLogoutRV(0)
	SetTempRevPos(53, 51904, 102048)--ket thuc thiet lap diem hoi sinh o Ba Lang Huyen
	SetNpcTimeIdle(0)
	-- reset_savetask_tongkim_tk()
end