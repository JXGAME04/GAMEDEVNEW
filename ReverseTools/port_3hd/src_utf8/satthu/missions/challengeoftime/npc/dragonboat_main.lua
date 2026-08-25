Include("\\script\\missions\\challengeoftime\\include.lua")
Include("\\script\\lib\\log.lua")
Include("\\script\\activitysys\\functionlib.lua")
function COT_CheckTeamRequirement()
	local oldplayer = PlayerIndex
	if (IsCaptain() ~= 1) then
		Say("Xin lỗi! Cần phải là Đội trưởng mới có tư cách báo danh tham gia 'Nhiệm vụ Thách thức thời gian'",0)
		return
	end
	
	if (GetTeamSize() < 4) then
		Say("Xin lỗi! Đội tham gia 'Nhiệm vụ Thách thức thời gian' cần phải có ít nhất <color=red>4<color> người!",0)
		return
	end
	
	if (GetCash() < 10000) then
		Say("Tham gia nhiệm vụ “Thách thức thời gian” cần <color=red>1vạn lượng<color>. Chuẩn bị đủ tiền rồi hãy quay lại!", 0)
		return
	end
	
	local nMatchLevel = GetMatchLevel(GetLevel())
	local nToday = tonumber(GetLocalDate("%y%m%d"));
	
	for i = 1, GetTeamSize() do 
		local nPlayerIndex = GetTeamMember(i)
		local nJoinDate = CallPlayerFunction(nPlayerIndex, GetTask, TSK_JOIN_DATE)
		local nRemainCount = CallPlayerFunction(nPlayerIndex, GetTask, TSK_REMAIN_COUNT)
		local nPlayerLevel = CallPlayerFunction(nPlayerIndex, GetLevel)
		local nTaskItem = CallPlayerFunction(nPlayerIndex, CalcItemCount, 3, 6, 1, 400, 90)
		local nFlag1 = DynamicExecuteByPlayer(nPlayerIndex,"\\script\\vng_lib\\bittask_lib.lua", "tbVNG_BitTask_Lib:getBitTask", {nTaskID = 3071,nStartBit = 25,nBitCount = 1,nMaxValue = 1})
		if nFlag1 ~= 0 then
			PlayerIndex = GetTeamMember(i)
			local szNamePlayer = GetName()
			PlayerIndex = oldplayer
			Say("Xin lỗi! Trong đội bạn có thành viên <color=yellow>".. szNamePlayer.."<color> không thỏa điều kiện để tham gia tính năng này")
			return
		end
		if (nJoinDate == nToday) and (nRemainCount <= 0) then
				Say("Nhiệm vụ Thách thức thời gian mỗi ngày chỉ được tham gia 1 lần. Số lần tha gia của tổ bạn đã đủ! Ngày mai trở lại nhé!",0);
				Msg2Team("Nhiệm vụ Thách thức thời gian mỗi ngày chỉ được tham gia 1 lần. Số lần tha gia của tổ bạn đã đủ! Ngày mai trở lại nhé!")
			return
		end
		
		if GetMatchLevel(nPlayerLevel) ~= nMatchLevel then
			if (nMatchLevel == 1) then
				Say(" Xin lỗi! Trong đội bạn có thành viên vượt <color=red>cấp 90<color>, xin kiểm tra lại rồi báo danh!",0)
				Msg2Team("Trong đội bạn có thành viên trên cấp 90, xin hãy kiểm tra lại rồi báo danh!");
				return
			elseif (nMatchLevel == 2) then
				Say("Xin lỗi! Trong đội bạn có thành viên chưa đạt <color=red>cấp 90<color>, xin kiểm tra lại rồi báo danh!",0)
				Msg2Team("Trong đội bạn có thành viên chưa đạt cấp 90, xin kiểm tra lại rồi báo danh!")
				return
			end
		end
		
		local bHaveItem = 0
		if nMatchLevel == 1 then
			for i=20,80,10 do
				if CallPlayerFunction(nPlayerIndex, CalcEquiproomItemCount, 6, 1, 400, i) > 0 then
					bHaveItem = 1
					break
				end
			end	
		elseif nMatchLevel == 2 then
			bHaveItem = CallPlayerFunction(nPlayerIndex, CalcEquiproomItemCount, 6, 1, 400, 90)
		end
		
		if (bHaveItem < 1 and nMatchLevel == 1) then
			Say("Nhiếp Thí Thần: Xin lượng thứ, tham gia nhiệm vụ khiêu chiến thời gian sơ cấp mỗi thành viên cần phải có một Sát Thủ Giản ngũ hành bất kỳ cấp 90 trở xuống, xin hãy chuẩn bị rồi hãy đến đây gặp ta",0)
			Msg2Team("Trong tổ đội của ngươi có người không có Sát Thủ Giản ngũ hành bất kỳ cấp 90 trở xuống , xin  kiểm tra lại rồi hãy báo danh tham gia !")
			return
		elseif (bHaveItem < 1 and nMatchLevel == 2) then
			Say("Nhiếp Thí Thần: Xin lượng thứ, tham gia nhiệm vụ khiêu chiến thời gian cao cấp mỗi thành viên cần phải có một Sát Thủ Giản ngũ hành bất kỳ cấp 90 , xin hãy chuẩn bị rồi hãy đến đây gặp ta",0)
			Msg2Team("Trong tổ đội của ngươi có người không có Sát Thủ Giản ngũ hành bất kỳ cấp 90, xin  kiểm tra lại rồi hãy báo danh tham gia !")
			return
		end
	end

	return 1
	
end
function COT_GetFreeMap(nMatchLevel)
	local tbFreeMaps = {}
	local bStarted = nil
	local tbMapList = GetMatchMapList(nMatchLevel)
	
	for i = 1, getn(tbMapList) do 
		local nMapIndex = SubWorldID2Idx(tbMapList[i])
		if (nMapIndex >= 0) then
			local nState = lib:DoFunInWorld(nMapIndex, GetMissionV, VARV_STATE)
			local nMSPlayerCount = lib:DoFunInWorld(nMapIndex, GetMSPlayerCount, MISSION_MATCH, 1)
			if (nState > 0) then
			 bStarted = 1
			end
			if (nState == 1 and nMSPlayerCount == 0) then
				tinsert(tbFreeMaps, tbMapList[i])
			end
		end
	end
	
	return tbFreeMaps, bStarted
end

function want_playboat()
	local nPlayerLevel = GetLevel()
	local nMatchLevel = GetMatchLevel(nPlayerLevel)
	local tbFreeMapList, bStarted = COT_GetFreeMap(nMatchLevel)
	
	local strLevel = tbLevels[nMatchLevel].szName
	
	if bStarted then
		if (getn(tbFreeMapList) == 0) then
			Say("Khu vực này"..strLevel.."đã bắt đầu. Không còn chỗ nữa….",0)
			return
		else
			Say("Khu vực này"..strLevel.."giai đoạn báo danh đua thuyền rồng, vẫn còn <color=red>"..getn(tbFreeMapList).."<color> một bản đồ nhiệm vụ còn trống, khiêu chiến có thời gian cần phải nộp <color=red>10 vạn<color> ngân lượng, ngươi muốn nhập hội không?",2, "Phải! Ta muốn dẫn dắt đội của ta tham gia./dragon_join", "Không cần/onCancel")
			return
		end
	else
		Say("Xin lỗi! Khu vực này"..strLevel.."Nhiệm vụ Thách thức thời gian chưa đến giai đoạn báo danh. Cách <color=red>mỗi giờ<color> báo danh 1 lần. Xin chú ý hệ thống thông báo!",0)
		return
	end
	
end

function COT_TeamMemberJoin(nMatchLevel)
	local w,x,y = GetWorldPos();
	local nPrisonId = 208
	if w == nPrisonId then	-- 在这里判断是否有越狱企图
		return
	end
	
	local nConsumed = 0
	if nMatchLevel == 1 then
		for i=20,80,10 do
			if(CalcEquiproomItemCount( 6, 1, 400, i ) > 0) then
				nConsumed = ConsumeEquiproomItem( 1, 6, 1, 400, i)
				if nConsumed > 0 then break end
			end
		end
	elseif nMatchLevel == 2 then
		nConsumed = ConsumeItem(3, 1, 6,1,400,90)
	end
	
	if (nConsumed > 0) then
		local nToday = tonumber(GetLocalDate("%y%m%d"));
		if(GetTask(TSK_JOIN_DATE) ~= nToday) then
			SetTask(TSK_REMAIN_COUNT, COUNT_LIMIT);
			SetTask(TSK_JOIN_DATE, nToday);
		end
		SetTask(TSK_REMAIN_COUNT, GetTask(TSK_REMAIN_COUNT) - 1);
		
		-- 判断是否使用了龙血丸
		if GetTask(%TSK_Longxuewan_Date) == nToday then
			local nCurCount = GetTask(%TSK_Longxuewan_avail)
			if nCurCount >= 0 then
				SetTask(%TSK_Longxuewan_avail, nCurCount - 1)
			end
		else
			SetTask(%TSK_Longxuewan_avail, -1)
		end

		JoinMission(MISSION_MATCH, 1)
		G_ACTIVITY:OnMessage("SignUpChuangguan", PlayerIndex, nMatchLevel)
		
		PlayerFunLib:AddTaskDaily(3079, 1)
		if PlayerFunLib:GetTaskDailyCount(3079) > 1 then
			tbLog:PlayerActionLog("TinhNangKey","BaoDanhVuotAiThuPhi")
		else
			tbLog:PlayerActionLog("TinhNangKey","BaoDanhVuotAiMienPhi")
		end
		
	end
end

function COT_TeamJoin(nMatchLevel)

	local szLeaderName = GetName()
	
	local tbPlayers = {}
	for i=1, GetTeamSize() do
		tbPlayers[i] = GetTeamMember(i)
	end		
	
	local w,x,y = GetWorldPos()
	SetMissionV(VARV_SIGNUP_WORLD, w)
	SetMissionV(VARV_SIGNUP_POSX, x)
	SetMissionV(VARV_SIGNUP_POSY, y)
	SetMissionV(VARV_LEVEL, nMatchLevel)
	SetMissionS(VARS_TEAM_NAME, szLeaderName)
	SetMissionS(VARS_TEAMLEADER_FACTION, GetLastFactionNumber())
	SetMissionS(VARS_TEAMLEADER_GENDER, GetSex())
	Pay(10000)
	
	local ndate = tonumber(GetLocalDate("%H"))
	if ndate <= 22 and ndate >= 10 and nMatchLevel == 2 then
		SetMissionV(VARV_BATCH_MODEL,1)
	else
		SetMissionV(VARV_BATCH_MODEL,0)
	end
	
	for i = 1 , getn(tbPlayers) do 
		CallPlayerFunction(tbPlayers[i], COT_TeamMemberJoin, nMatchLevel)
	end
	
	tbLog:PlayerActionLog("EventChienThang042011","BaoDanhVuotAi")-- 报名日志
end

function dragon_join()
	
	local nLeaderLevel = GetLevel()
	local nMatchLevel = GetMatchLevel(nLeaderLevel)
	
	if not COT_CheckTeamRequirement(nMatchLevel) then
		return
	end
	
	local tbFreeMaps = COT_GetFreeMap(nMatchLevel)
	for i = 1, getn(tbFreeMaps) do 
		local nMapIndex = SubWorldID2Idx(tbFreeMaps[i])
		if nMapIndex >= 0 then
			lib:DoFunInWorld(nMapIndex, COT_TeamJoin, nMatchLevel)
			return
		end
	end
	
	local strLevel = tbLevels[nMatchLevel].szName
	Say("Xin lỗi! Hiện tại khu vực <color=red>"..strLevel.."<color>đã không còn chỗ. Xin đợi vòng sau!",0)
end
