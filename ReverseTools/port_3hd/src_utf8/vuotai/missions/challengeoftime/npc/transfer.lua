Include("\\script\\misc\\eventsys\\type\\npc.lua")
Include("\\script\\missions\\challengeoftime\\include.lua")
Include("\\script\\missions\\challengeoftime\\chuangguang30.lua")
Include("\\script\\dailogsys\\g_dialog.lua")


function main()
	local nNpcIndex = GetLastDiagNpc();
	local szNpcName = GetNpcName(nNpcIndex);
	if NpcName2Replace then szNpcName = NpcName2Replace(szNpcName) end
	local tbDailog = DailogClass:new(szNpcName);
	G_ACTIVITY:OnMessage("ClickNpc", tbDailog, nNpcIndex);
	
	EventSys:GetType("AddNpcOption"):OnEvent(szNpcName, tbDailog, nNpcIndex)
	
	
	tbDailog.szTitleMsg = "<#><npc>Xin chúc mừng các chư vị đã vượt qua được 29 cửa ải trong thời gian 13 phút, Muốn tiếp tục Vượt ải chứ!"
	
	tbDailog:AddOptEntry("Tiếp tục Vượt ải", gotonewworld); 
	
	tbDailog:AddOptEntry("Rời khỏi", leave); 	
	tbDailog:Show()
end


function leave()
	local world = GetMissionV(VARV_SIGNUP_WORLD);
	local pos_x = GetMissionV(VARV_SIGNUP_POSX);
	local pos_y = GetMissionV(VARV_SIGNUP_POSY); 
	DelMSPlayer(MISSION_MATCH,1);
	SetLogoutRV(0);
	NewWorld(world, pos_x, pos_y);
end


function gotonewworld()
	local ndate = tonumber(GetLocalDate("%H"))
	if ndate > 22 and ndate < 10 then
		Say("Trong thời gian này không thể tiến hành tiếp tục Vượt ải", 0)
		leave()
		return 
	end
	local nMinute = tonumber(GetLocalDate("%M"))
	if nMinute >= 24 then
		Talk(1, "", "Xin lượng thứ, Ngươi đã vượt quá thời gian truyền tống") 		return
	end
	if GetMissionV(VARV_BOARD_TIMER) >= (CHUANGGUAN30_TIME_LIMIT + 60) then
		Say("Xin lượng thứ, Ngươi đã vượt quá thời gian truyền tống!",0)
		return 
	end
	-- wangjingjun
	local nX,nY, nMapIndex = GetPos()
	SetTask(PLAYER_MAP_TASK, SubWorldIdx2ID(nMapIndex)) -- 将玩家的当前地图的ID保存下来
	-- 从\\settings\\maps\\liandandong\\npc_3.txt中选择的一个位置
	NewWorld(CHUANGGUAN30_MAP_ID, floor(51072/32), floor(102272/32))	-- 传送到新的地图
end
