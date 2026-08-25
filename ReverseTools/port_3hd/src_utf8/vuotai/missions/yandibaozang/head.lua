-- 炎帝宝藏
-- by 小浪多多
-- 2007.10.24
-- ẻề..
-- 正在尝试着..
-- 寻找着属于我的天地..
IncludeLib("RELAYLADDER")
IncludeLib("FILESYS")
IncludeLib("TITLE");
IncludeLib("SETTING");

Include("\\script\\lib\\objbuffer_head.lua")
YDBZ_MISSION_MATCH			= 50		-- 比赛mission
YDBZ_TIMER_MATCH				= 87		-- 比赛开始定时器
YDBZ_TIMER_FIGHTSTATE 	= 89		-- 检测玩家战斗状态计时器
YDBZ_NPC_BOSS_COUNT			= 1			-- 争夺地boss数量计算
YDBZ_VARV_STATE 				= 4			-- mission状态，1表示报名，2准备阶段，停止报名，3开始比赛
YDBZ_SIGNUP_WORLD				= 5			-- 报名地图
YDBZ_SIGNUP_POSX				= 6			-- 报名地点的X坐标
YDBZ_SIGNUP_POSY				= 7			-- 报名地点的Y坐标 
YDBZ_NPC_COUNT					= {8,9,10}		-- 各小关小怪叠加
YDBZ_NPC_BOSS_COUNT			= 11		-- 争夺地boss
YDBZ_NPC_WAY						= {12,13,14}	-- 达到的关
YDBZ_STATE_SIGN					= 15 		--进度,YDBZ_STATE_SIGN
YDBZ_TEAM_COUNT					= 16		--参加争夺的队伍数，计算器
YDBZ_TEAM_SUM						=	17		--参加争夺的队伍总数，常量
YDBZ_NPC_TYPE						= {
	{20,21,22,23,24,25,26,27,28,29},		--A路记录怪物类型			
	{30,31,32,33,34,35,36,37,38,39},		--B路记录怪物类型	
	{40,41,42,43,44,45,46,47,48,49},		--C路记录怪物类型	
}


YDBZ_TEAM_NAME					= {1,2,3}						--A队伍名	--B队伍名	--C队伍名					

YDBZ_TEAMS_TASKID				=	1851			-- 记录玩家所在group任务变量
YDBZ_PLAY_LIMIT_COUNT   = 1852			-- 参加次数变量，1，今年的第几周，2次数 ，3日 ，4次数
YDBZ_ITEM_YANDILING			= 1853			-- 炎帝令使用变量记录,1炎帝令 2记录临时阵营
YDBZ_MISSIOM_PLAYER_KEY = 1854			-- 炎帝记录玩家身上的key
YDBZ_ITEM_YANDILING_SUM	= 1855			-- 炎帝令使用总数
YDBZ_LIMIT_SIGNUP				= 5					-- 开始刷怪之间的时间：10秒
YDBZ_LIMIT_FINISH 			= 30 * 60		-- 任务时间期限（秒）：30分钟
YDBZ_LIMIT_BOARDTIME		= 5	* 60		-- 公告时间,每5分钟公告一次时间
YDBZ_TIME_WAIT_STATE1		= 10				-- 杀完争夺地boss后等级10秒进入争夺阶段
YDBZ_TIME_WAIT_STATE3		= 30				-- 闯关成功结束后，等待30秒被传送出场地
YDBZ_LIMIT_SETFIGHTSTATE= 3					-- 争夺阶段，玩家被杀后无敌状态时间3秒
YDBZ_LIMIT_TEAMS_COUNT	= 15				-- 队伍数量的最大限制
YDBZ_LIMIT_PLAYER_LEVEL = 120				-- 玩家最低等级限制
YDBZ_LIMIT_WEEK_COUNT		= 10 				-- 一周参加最大次数
YDBZ_LIMIT_DAY_COUNT		= 4 				-- 每天参加最大次数
YDBZ_PAIHANG_ID					=	10252			--排行榜ID
YDBZ_LIMIT_ITEM					= {{6,1,1604},1,"Anh Hùng Thiếp"}	--需要信物，英雄帖，tb1，图谱ID，tb2，需要数量
YDBZ_LIMIT_DOUBEL_ITEM	= {{6,1,1617},1,"Viêm Đế Lệnh"}	--炎帝令，可获得双倍的奖励
YDBZ_AWARD_EXP 					= 600000									--每通过一关整队获得的经验
YDBZ_Faninl_AWARD_EXP		=	300000									--争夺地boss经验
YDBZ_KILLPLAYER_EXP 		= 200000									--杀死一个敌人获得经验（杀玩家得经验）
YDBZ_KILLLASTBOSS_EXP		= 1000000									--杀死最终boss获得经验
YDBZ_BOAT_POS 					=													--玩家被传送宝藏场的3个点x,y坐标。
{
	[1]={60032,104832},
	[2]={59744,123296},
	[3]={52960,121952},
}
YDBZ_FIGHTING_RELIFT = 		--玩家争夺区传送点和重生点，随机
{
	[1]=
	{
		{57408,112000},
		{57504,112160},
		{57664,112160},
	},
	[2]=
	{
		{58016,114464},
		{57888,114688},
		{58048,114784},
	},
	[3]=
	{
		{56288,112544},
		{56160,112736},
		{56320,112736},
	},
}
-- 比赛地图ID
YDBZ_MAP_MAP = {
	853,
	854,
	855,
	856,
	857,
	858,
	859,
	860,
	861,
	862,
};


local  _Message =  function (nItemIndex)
	local handle = OB_Create()
	local msg = format("Chúc mừng cao thủ <color=yellow>%s<color> thuộc tổ đội tiêu diệt [Lương Mi Nhi] đã nhận được phần thưởng [%s] " ,GetName(),GetItemName(nItemIndex))
	local _, nTongId = GetTongName()
	if (nTongId ~= 0) then
		Msg2Tong(nTongId, msg)
	end
	ObjBuffer:PushObject(handle, msg)
	RemoteExecute("\\script\\event\\msg2allworld.lua", "broadcast", handle)
	OB_Release(handle)
	Msg2Team(msg)
	Msg2SubWorld(msg)
	AddGlobalNews(msg)
end

--奖励
YDBZ_ZUANYONG_ITEM =				--专用物品
{
	[1] = {"Hình nhân",6,1,1605}, -- 1固定为傀儡
	[2] = {"Viêm Đế trường mệnh hoàn",	6,	0,	1607}, 
	[3] = {"Viêm Đế gia bào hoàn",	6,	0,	1608}, 
	[4] = {"Viêm Đế đại lực hoàn",	6,	0,	1609}, 
	[5] = {"Viêm Đế cao thiểm hoàn",	6,	0,	1610}, 
	[6] = {"Viêm Đế cao trung hoàn",	6,	0,	1611}, 
	[7] = {"Viêm Đế phi tốc hoàn",	6,	0,	1612}, 
	[8] = {"Viêm Đế băng phòng hoàn",	6,	0,	1613}, 
	[9] = {"Viêm Đế lôi phòng hoàn",	6,	0,	1614}, 
	[10] = {"Viêm Đế hòa phòng hoàn",	6,	0,	1615}, 
	[11] = {"Viêm Đế độc phòng hoàn",	6,	0,	1616}, 

}

-- 掉落物品
YDBZ_tbaward_item ={
	[1]=--小怪
	{
	},
	[2]=--小关boss
	{--几率%,个数,物品ID,是否掉地上(0,掉地上，1直接随机掉一个队员身上),名称,双倍丸是否有效
		{50,1,{6,1,1605,1,0,0},1,"Hình nhân",1},		--傀儡
		{100,15,{1, 2, 0, 5, 0, 0},0,"Ngũ Hoa Ngọc Lộ Hoàn",0}, --五花
		--{10,1,{6,1,1606,1,0,0},1,"Viêm Đế Đồ Đằng",1},			--碎片
	},
	[3]=--争夺地小boss
	{
		{100,1,{6,1,1605,1,0,0},1,"Hình nhân",1},		--傀儡
		{100,30,{1, 2, 0, 5, 0, 0},0,"Ngũ Hoa Ngọc Lộ Hoàn",0},--五花
	},
--Modifiled by:ThanhLD - 20140226 - change award from Boss cuối tính năng Viêm Đế
	[4]=--最终boss
	{	
		[1] = {--Phần 1
			{szName="Tinh Tinh Khoáng",tbProp={6,1,3811,1,0,0},nCount=1,nRate=6.430},
			{szName="Tinh Thiết Khoáng",tbProp={6,1,3810,1,0,0},nCount=1,nRate=3.210},
		},
		[2] = {--Phần 2
			{szName="Hộp Mặt Nạ Chiến Trường",tbProp={6,1,30350,1,0,0},nCount=1,nRate=11.15,nExpiredTime=10080,tbParam={1,10080,60,0,0,0}},
			{szName="Hộp Mặt Nạ Chiến Trường",tbProp={6,1,30350,1,0,0},nCount=1,nRate=2.57,nExpiredTime=10080,tbParam={2,10080,60,0,0,0}},
			{szName="Phi phong Ngự Phong (Trọng kích)",tbProp={0,3475},nCount=1,nRate=3,nQuality = 1,nExpiredTime=10080,},
			{szName="Phi phong Ngự Phong (Xác suất hóa giải sát thương)",tbProp={0,3474},nCount=1,nRate=2.1,nQuality = 1,nExpiredTime=10080,},
			{szName="Phi Phong Phệ Quang ( trọng kích)",tbProp={0,3478},nCount=1,nRate=1.5,nQuality = 1,nExpiredTime=10080,},
			{szName="Phi Phong Cấp Phệ Quang (hóa giải sát thương)",tbProp={0,3477},nCount=1,nRate=1,nQuality = 1,nExpiredTime=10080,},
		},
		[3] = {--Phần 3
			{szName="Túi Dược Phẩm",tbProp={6,1,30557,1,0,0},nCount=1,nRate=40},
			{szName="Càn Khôn Tạo Hóa Đan (đại) ",tbProp={6,1,215,1,0,0},nCount=50,nRate=30},
			{szName="Đại Lực hoàn",tbProp={6,0,3,1,0,0},nCount=1,nRate=15},
			{szName="Phi Tốc hoàn",tbProp={6,0,6,1,0,0},nCount=1,nRate=15},
		},
		[4] = {--Phần 4
			{szName = "Đồ Phổ Đằng Long Bội", tbProp = {6, 1, 30534,1,0,0}, nRate = 0.1,},
			{szName="Đằng Long Thạch - Hạ",tbProp={6,1,30538,1,0,0},nCount=5,nRate=2},
			{szName = "Đồ Phổ Tinh Sương Yêu Đái", tbProp = {6, 1, 30008,1,0,0}, nRate = 0.11,},
			{szName = "Đồ Phổ Tinh Sương Bội", tbProp = {6, 1, 30011,1,0,0}, nRate = 0.11,},
		},
	},
}

-- 2011.03.23
YDBZ_tbaward_item_ex = 
{
	[1] = {szName="Viêm Đế Bí Bảo",tbProp={6,1,2805,1,0,0}},		-- 炎帝秘宝p有待修改
}

--
-- NPC表格列含义
-- NPC参数各列的含义：后续处理、ID、名字、等级、五行、是否BOSS(0,1)、数量、位置
YDBZ_NPC_ATTRIDX_PROCEED		= 1			-- NPC后续处理
YDBZ_NPC_ATTRIDX_ID			= 2			-- NPC的ID
YDBZ_NPC_ATTRIDX_NAME		= 3			-- NPC名字
YDBZ_NPC_ATTRIDX_LEVEL		= 4			-- NPC等级
YDBZ_NPC_ATTRIDX_SERIES		= 5			-- NPC五行
YDBZ_NPC_ATTRIDX_ISBOSS		= 6			-- 是否BOSS
YDBZ_NPC_ATTRIDX_COUNT		= 7			-- NPC数量
YDBZ_NPC_ATTRIDX_POSITION	= 8			-- NPC位置
--

YDBZ_SCRIPT_NPC_DEATH 	= "\\script\\missions\\yandibaozang\\npc_death.lua"
YDBZ_SCRIPT_PLAYER_DEATH = "\\script\\missions\\yandibaozang\\player_death.lua"
--
---- 五行
YDBZ_map_series = {
	0,	-- 金
	1,	-- 木
	2,	-- ậđ
	3,	-- 火
	4,	-- 土
};


YDBZ_mapfile_trap =
{
	{"\\settings\\maps\\yandibaozang\\trap\\a","\\script\\missions\\yandibaozang\\trap\\a",10,"\\settings\\maps\\yandibaozang\\trap\\clear\\a"},
	{"\\settings\\maps\\yandibaozang\\trap\\b","\\script\\missions\\yandibaozang\\trap\\b",10,"\\settings\\maps\\yandibaozang\\trap\\clear\\b"},
	{"\\settings\\maps\\yandibaozang\\trap\\c","\\script\\missions\\yandibaozang\\trap\\c",10,"\\settings\\maps\\yandibaozang\\trap\\clear\\c"},
}