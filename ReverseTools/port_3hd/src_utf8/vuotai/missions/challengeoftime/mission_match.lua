-- description	: 比赛任务
-- author		: wangbin
-- datetime		: 2005-06-04

Include("\\script\\missions\\challengeoftime\\include.lua")
Include("\\script\\event\\storm\\function.lua")	--Storm
Include("\\script\\misc\\eventsys\\type\\npc.lua")

-- 启动比赛开始定时器
function start_match_timer()
	-- 10分钟后启动比赛任务
	StartMissionTimer(MISSION_MATCH, TIMER_MATCH, LIMIT_SIGNUP * 18);
end

-- 开始报名
function start_signup()
	-- 设置为报名状态
	SetMissionV(VARV_STATE, 1);
	SetMissionV(VARV_BOARD_TIMER, 0);
	SetMissionS(VARS_TEAM_NAME, "");
	-- 通告龙舟比赛报名开始
	--由relay通告报名开始，2次
	--local min = floor(LIMIT_SIGNUP / 60);
	--broadcast("<#>时间的挑战任务报名开始了，请大家速来7大城市聂弑尘处组队由队长报名，报名时间为" .. min .. "分钟");
end

function InitMission()
	start_signup();
	start_match_timer();
end

function EndMission()
	close_match();
	close_board_timer();
	close_close_timer();
end

function PlayerLeave()

	BigBoss:RemoveChuangGuanBonus()
	
	SetCurCamp(GetCamp())
	SetTaskTemp(200,0)
	SetFightState(0)
	SetPunish(1)
	SetCreateTeam(1)
	SetPKFlag(0)
	ForbidChangePK(0)
	SetDeathScript("")
	
	SetTask(1505, 0)
	DisabledUseTownP(0)
	
	Msg2MSAll(MISSION_MATCH, GetName().."Rời khỏi khu vực nhiệm vụ ")
	
	--Storm 加奖励分
	if storm_valid_game(2) then	--风暴挑战没有结束（如果是玩家下线该段代码不会起作用）
		storm_add_pointex(2, 40)
		storm_end(2)
	end
	SetTask(STORM_TASKID_GAMEID_SS, 0)	--清除GameKey
end

function PlayerJoinMission(camp)
	LeaveTeam()
	AddMSPlayer(MISSION_MATCH,camp);
	SetCurCamp(camp);
	SetTaskTemp(200,1)
	SetFightState(1);
	SetLogoutRV(1);
	SetPunish(0);
	str = GetName().."Tham gia 'Nhiệm vụ Thách thức thời gian'"
	local sf_mapid = SubWorldIdx2ID(SubWorld)
	NewWorld(sf_mapid, BOAT_POSX, BOAT_POSY);
	SetCreateTeam(0);
	Msg2MSAll(MISSION_MATCH,str);
	SetPKFlag(1)
	ForbidChangePK(1);
	SetDeathScript(SCRIPT_PLAYER_DEATH);
	SetTask(1505, 1)
	DisabledUseTownP(1)

	--Storm 加入风暴积分
	SetTask(STORM_TASKID_GAMEID_SS, random(100000))	--指定随机的GameKey
end

-- 玩家退出
function OnLeave(index)
	CallPlayerFunction(index, PlayerLeave)
end

--玩家进入
function JoinMission(RoleIndex, camp)
	if (camp ~= 1) then
		return
	end
	CallPlayerFunction(RoleIndex, PlayerJoinMission, camp)
	EventSys:GetType("ChuanGuan"):OnPlayerEvent("OnEnter", RoleIndex, camp)
end

