IncludeLib("RELAYLADDER")
Include("\\script\\tong\\tong_award_head.lua")		--By Liao Zhishan
Include("\\script\\missions\\boss\\bigboss.lua") -- big boss
Include("\\script\\activitysys\\functionlib.lua")

MISSION_MATCH			= 22		-- 比赛mission
TIMER_MATCH				= 41		-- 比赛开始定时器
TIMER_BOARD				= 42		-- 通告定时器
TIMER_CLOSE				= 43		-- 比赛结束定时器
INTERVAL_BOARD			= 1			-- 通报间隔（分钟）：每分钟通报一次
INTERVAL_MATCH			= 3600		-- 每小时比赛一次（秒）
TIME_SIGNUP				= 10
LIMIT_SIGNUP			= TIME_SIGNUP * 60		-- 报名时间期限（秒）：10分钟
LIMIT_FINISH 			= 30 * 60	-- 任务时间期限（秒）：30分钟
BOAT_POSX				= 1559		-- 玩家被传送到龙舟的x坐标
BOAT_POSY				= 3226		-- 玩家被传送到龙舟的y坐标
LIMIT_PLAYER_COUNT		= 8			-- 玩家数量的最大限制
AWARD_COUNT				= 10		-- 全部闯关成功之后可以瓜分10个闯关宝箱
TSK_JOIN_DATE			= 1551
TSK_REMAIN_COUNT	= 1550

SEX_BOY					= 0			-- 男性
SEX_GIRL				= 1			-- 女性
SEX_RENYAO				= 2			-- 人妖

-- 闯关调整 2011.03.04
CHUANGGUAN30_MAP_ID		= 957
CHUANGGUAN30_TIME_LIMIT = 13*60		-- 前二十九关需要提早完成的时间
CHUANGGUAN30_START_TIME = 10
CHUANGGUAN30_END_TIME	= 22
PLAYER_MAP_TASK			= 2852
COUNT_LIMIT 		= 1



-- NPC表格列含义
-- NPC参数各列的含义：后续处理、ID、名字、等级、五行、是否BOSS(0,1)、数量、位置
NPC_ATTRIDX_PROCEED		= 1			-- NPC后续处理
NPC_ATTRIDX_ID			= 2			-- NPC的ID
NPC_ATTRIDX_NAME		= 3			-- NPC名字
NPC_ATTRIDX_LEVEL		= 4			-- NPC等级
NPC_ATTRIDX_SERIES		= 5			-- NPC五行
NPC_ATTRIDX_ISBOSS		= 6			-- 是否BOSS
NPC_ATTRIDX_COUNT		= 7			-- NPC数量
NPC_ATTRIDX_POSITION	= 8			-- NPC位置

USE_NAME_ALL_PLAYERS	= 1			-- 使用全体玩家名字
USE_NAME_THE_TOPLIST	= 2			-- 使用随机榜上名

VARS_TEAM_NAME			= 1			-- 队伍名称
-- 以下变量从(VARS_PLAYER_NAME + 1)到(VARS_PLAYER_NAME + LIMIT_PLAYER_COUNT)分别保存不同玩家的名字
VARS_PLAYER_NAME		= 2			-- 玩家名字
VARS_TEAMLEADER_FACTION = 11        -- 队长门派
VARS_TEAMLEADER_GENDER  = 12        -- 队长性别

VARV_NPC_BATCH 			= 1			-- 怪的批次
VARV_NPC_COUNT 			= 2			-- 怪的数量
VARV_PLAYER_COUNT		= 3			-- 玩家的数量
VARV_STATE 				= 4			-- mission状态，1表示报名，2表示比赛
VARV_SIGNUP_WORLD		= 5			-- 报名地图
VARV_SIGNUP_POSX		= 6			-- 报名地点的X坐标
VARV_SIGNUP_POSY		= 7			-- 报名地点的Y坐标 
VARV_BOARD_TIMER		= 8			-- 比赛进行的时间（秒）
VARV_PLAYER_TOTAL_COUNT = 9		-- 玩家总的数量(不管是否已经死亡)
VARV_PLAYER_USE_INDEX	= 10		-- 使用到的玩家索引
VARV_NPC_USE_INDEX		= 11		-- 使用到的候选NPC索引
VARV_MISSION_RESULT		= 12		-- 任务结果，１为胜利通关，０为失败
VARV_PLAYER_SEX			= 13
-- 以下变量从(VARV_PLAYER_SEX + 1)到(VARV_PLAYER_SEX + LIMIT_PLAYER_COUNT)分别保存不同玩家的性别，０为男性，１为女性，２为人妖
VARV_PLAYER_SERIES		= VARV_PLAYER_SEX + LIMIT_PLAYER_COUNT + 1		-- 22


VARV_LEVEL = VARV_PLAYER_SERIES + LIMIT_PLAYER_COUNT + 1	--比赛等级

VARV_XIAONIESHICHEN_BATCH = VARV_LEVEL + 1		-- 小聂弑尘出现的关
VARV_BATCH_MODEL	= VARV_LEVEL + 2			-- 当前闯关的模式，0为正常模式，1为调整之后的模式


SCRIPT_NPC_DEATH 	= "\\script\\missions\\challengeoftime\\npc_death.lua"
SCRIPT_PLAYER_DEATH = "\\script\\missions\\challengeoftime\\player_death.lua"

-- 比赛等级划分
tbLevels = {
	--[MinLevel, MaxLevel)
	[1] = {50, 90;szName="Nhiệm vụ Thách thức thời gian Sơ cấp"},
	[2] = {90, 201;szName="Nhiệm vụ Thách thức thời gian cao cấp"},
	-- add new level here
}
-- 比赛地图ID
tbLevelMaps = {
	[1] = {
		464,465,466,467,468,
		469,470,471,472,473,
		474,475,476,477,478,
		479,
	}, --[1]
	[2] = {
		480, 481, 482, 483, 484,
		485, 486, 487, 488, 489,
		490, 491, 492, 493, 494,
		495
	},--[2]
}

-- 特殊NPC 的ID范围
tbRangeId = {
	[1] =  {
		{994, 1001},
		{1002, 1005}
	},
	[2] = {
		{1026, 1033},
		{1034, 1037}
	},
}

TSK_Longxuewan_Date = 2641
TSK_Longxuewan_Use	= 2642
TSK_Longxuewan_avail = 4018

function GetMatchLevel(nLevel)
	if not nLevel then
		return GetMissionV(VARV_LEVEL)
	end
	
	for i=1,getn(tbLevels) do
		if tbLevels[i][1] <= nLevel and tbLevels[i][2] > nLevel then
			return i
		end
	end
	
	return nil
end

function GetMatchMapList(nLevel)
	if (not nLevel) or (nLevel > getn(tbLevelMaps)) then
		return {}
	end
	return tbLevelMaps[nLevel]
end

-- 五行
map_series = {
	0,	-- 金
	1,	-- 木
	2,	-- ậđ
	3,	-- 火
	4	-- 土
};

-- 候选NPC的名字
map_npcname_candidates = {
	"Hậu  Khất Kiếm",
	"Điêu Dịch Đao",
	"Kiều Đỉnh Thiên",
	"Nhậm Thương Khung",
	"Bộ Hiêu Trần",
	"Mậu Tuất Nhung",
	"Hạng Phù Nhai"
};

-- 广播
function broadcast(msg)
	AddGlobalNews(msg);
end


-- 获取玩家集合
function GetMatchPlayerList()

	local players = {};
	-- 遍历每个队员，把玩家信息存入集合
	local index = 0;
	local player = 0;
	local player_count = 0;
	for i=1, GetMSPlayerCount(MISSION_MATCH, 1) do
		index, player = GetNextPlayer(MISSION_MATCH, index, 0);
		if (player > 0) then
			player_count = player_count + 1;
			if (player_count > LIMIT_PLAYER_COUNT) then
				print(format("Mission player count exceed the limit %d!!!", LIMIT_PLAYER_COUNT));
				break;
			else
				players[player_count] = player
			end
		end
	end

	return players
end

-- 把玩家踢回报名点
function kickout()
	
	local tbPlayers = GetMatchPlayerList()
	local world = GetMissionV(VARV_SIGNUP_WORLD)
	local pos_x = GetMissionV(VARV_SIGNUP_POSX)
	local pos_y = GetMissionV(VARV_SIGNUP_POSY)
	
	for i = 1, getn(tbPlayers) do 
		CallPlayerFunction(tbPlayers[i], DelMSPlayer ,MISSION_MATCH, 1)
		CallPlayerFunction(tbPlayers[i], SetLogoutRV, 0)
		CallPlayerFunction(tbPlayers[i], NewWorld, world, pos_x, pos_y)
	end
end

-- 启动通告定时器
function start_board_timer()
	StartMissionTimer(MISSION_MATCH, TIMER_BOARD, INTERVAL_BOARD * 60 * 18);
end

-- 关闭通告定时器
function close_board_timer()
	StopMissionTimer(MISSION_MATCH, TIMER_BOARD);
end

-- 启动比赛关闭定时器
function start_close_timer()
	-- 10分钟后启动比赛任务
	StartMissionTimer(MISSION_MATCH, TIMER_CLOSE, LIMIT_FINISH * 18);
end

-- 关闭比赛关闭定时器
function close_close_timer()
	StopMissionTimer(MISSION_MATCH, TIMER_CLOSE);
end

-- 结束比赛
function close_match()
	-- 把玩家踢出地图
	kickout();
	
	-- 清理地图
	world = SubWorldIdx2ID(SubWorld);
	ClearMapNpc(world, 1);	-- 也清理玩家
	ClearMapTrap(world); 
	ClearMapObj(world);

end

-- 把玩家姓名、性别保存在mission变量中
function save_player_info()
	local players = GetMatchPlayerList()
	for index = 1,getn(players) do
		-- 随机排序
		local szName =  CallPlayerFunction(players[index], GetName)	-- 玩家名字
		local nSex =    CallPlayerFunction(players[index], GetSex)		-- 玩家性别
		local nSeries = CallPlayerFunction(players[index], GetSeries)	-- 玩家五行
		
		SetMissionS(VARS_PLAYER_NAME + index, szName)		-- 名字
		SetMissionV(VARV_PLAYER_SEX + index, nSex)			-- 性别
		SetMissionV(VARV_PLAYER_SERIES + index, nSeries)-- 五行
	end
	
	-- 玩家总数
	SetMissionV(VARV_PLAYER_TOTAL_COUNT, getn(players));
	SetMissionV(VARV_PLAYER_USE_INDEX, 0);
end

function GetRandomNpcRange()
	local nLevel = GetMatchLevel()
	if nLevel > getn(tbRangeId) then
		return {}
	end
	return tbRangeId[nLevel]
end

-- 根据性别随机获取NPC的ID
function get_random_npc_id(sex)
	if (sex ~= 0 and sex ~= 1) then
		-- 人妖该怎么处理？
		return nil;
	end

	local range_id = GetRandomNpcRange()
	return random(range_id[sex + 1][1], range_id[sex + 1][2]);
end

-- 从文件中读取NPC位置
function get_file_pos(file, line, column)
	x = GetTabFileData(file, line, column);
	y = GetTabFileData(file, line, column + 1);
	return x, y;
end

function func_npc_getid(item, index)
	-- 获取当前使用的玩家索引
	local player_use_index = GetMissionV(VARV_PLAYER_USE_INDEX) + 1;
	local player_all_count = GetMissionV(VARV_PLAYER_TOTAL_COUNT);
	local player_sex = GetMissionV(VARV_PLAYER_SEX + player_use_index);	
		
	-- 如果玩家数目不足，则随机产生男性NPC的ID
	local result = 0;
	if (player_use_index > player_all_count) then
		result = get_random_npc_id(0);
	else
		-- 根据使用的玩家索引获取当前玩家性别
		result = get_random_npc_id(player_sex);
	end

	return result;
end

function func_npc_getname(item, index)
	-- 获取当前使用的玩家索引
	local player_use_index = GetMissionV(VARV_PLAYER_USE_INDEX) + 1;
	local player_all_count = GetMissionV(VARV_PLAYER_TOTAL_COUNT);
	
	-- 如果玩家数目不足，则依次使用候选NPC名字
	local result = "";
	if (player_use_index > player_all_count) then
		local npc_use_index = GetMissionV(VARV_NPC_USE_INDEX) + 1;
		if (npc_use_index > getn(map_npcname_candidates)) then
			npc_use_index = 1;
		end
		SetMissionV(VARV_NPC_USE_INDEX, npc_use_index);
		result = map_npcname_candidates[npc_use_index];
	else
		-- 根据使用的玩家索引获取名字
		result = GetMissionS(VARS_PLAYER_NAME + player_use_index);
	end
	return result;
end

function func_ladder_getname(item, index)
	local map = item[NPC_ATTRIDX_NAME];
	if (type(map) ~= "table") then
		-- DEBUG
		--print("func_ladder_getname() fail!!!");
		return nil;
	end
	
	-- 随机获取排行榜玩家名字
	local name, data = Ladder_GetLadderInfo(map[2], random(1, 10));
	if (name ~= nil and name ~= "") then
		return name;
	end
	
	-- 随机使用候选NPC名字
	local pos = random(1, getn(map_npcname_candidates));
	return map_npcname_candidates[pos];
end

-- NPC使用全体玩家的名字
function func_npc_get_eachname(item, index)
	local player_all_count = GetMissionV(VARV_PLAYER_TOTAL_COUNT);
	if (index <= player_all_count) then
		return GetMissionS(VARS_PLAYER_NAME + index);
	else
		local count = getn(map_npcname_candidates);
		local pos = mod(index - player_all_count, count);
		if (pos == 0) then
			pos = count;
		end
		return map_npcname_candidates[pos];
	end
end

function func_npc_getseries(item, index)
	-- 获取当前使用的玩家索引
	local player_use_index = GetMissionV(VARV_PLAYER_USE_INDEX) + 1;
	local player_all_count = GetMissionV(VARV_PLAYER_TOTAL_COUNT);
	
	-- 如果玩家数目不足，则随机产生五行
	if (player_use_index > player_all_count) then
		local index = random(1, getn(map_series));
		return map_series[index];
	else
		-- 根据使用的玩家索引获取五行
		return GetMissionV(VARV_PLAYER_SERIES + player_use_index);
	end
end

function func_npc_getpos(item, index)
	local pos = item[NPC_ATTRIDX_POSITION];
	local pos_type = type(pos);
	if (pos_type == "table") then
		return pos[1], pos[2];
	elseif (pos_type ~= "function") then
		return nil;
	end
	
	local func = pos;
	local file = map_posfiles[item[NPC_ATTRIDX_COUNT]];
	local file_name = file[1];	-- 坐标文件名
	local pos_count = file[2];	-- 坐标列数量
	
	local column = 2 * (random(1, pos_count) - 1) + 1;
	return get_file_pos(file_name, index + 1, column);
end

function func_npc_proceed(item)
	local player_use_index = GetMissionV(VARV_PLAYER_USE_INDEX);
	SetMissionV(VARV_PLAYER_USE_INDEX, player_use_index + 1);
end

