-- 
-- by 
-- 2007.10.24
-- ÎÒ..
-- ..
-- ..
IncludeLib("RELAYLADDER")
IncludeLib("FILESYS")
IncludeLib("TITLE");
IncludeLib("SETTING");

Include("\\script\\lib\\objbuffer_head.lua")
YDBZ_MISSION_MATCH			= 50		-- mission
YDBZ_TIMER_MATCH				= 87		-- 
YDBZ_TIMER_FIGHTSTATE 	= 89		-- 
YDBZ_NPC_BOSS_COUNT			= 1			-- boss
YDBZ_VARV_STATE 				= 4			-- mission123
YDBZ_SIGNUP_WORLD				= 5			-- 
YDBZ_SIGNUP_POSX				= 6			-- X
YDBZ_SIGNUP_POSY				= 7			-- Y 
YDBZ_NPC_COUNT					= {8,9,10}		-- 
YDBZ_NPC_BOSS_COUNT			= 11		-- boss
YDBZ_NPC_WAY						= {12,13,14}	-- 
YDBZ_STATE_SIGN					= 15 		--,YDBZ_STATE_SIGN
YDBZ_TEAM_COUNT					= 16		--
YDBZ_TEAM_SUM						=	17		--
YDBZ_NPC_TYPE						= {
	{20,21,22,23,24,25,26,27,28,29},		--A			
	{30,31,32,33,34,35,36,37,38,39},		--B	
	{40,41,42,43,44,45,46,47,48,49},		--C	
}


YDBZ_TEAM_NAME					= {1,2,3}						--A	--B	--C					

YDBZ_TEAMS_TASKID				=	1851			-- group
YDBZ_PLAY_LIMIT_COUNT   = 1852			-- 12 3 4
YDBZ_ITEM_YANDILING			= 1853			-- ,1 2
YDBZ_MISSIOM_PLAYER_KEY = 1854			-- key
YDBZ_ITEM_YANDILING_SUM	= 1855			-- 
YDBZ_LIMIT_SIGNUP				= 5					-- 10
YDBZ_LIMIT_FINISH 			= 30 * 60		-- 30
YDBZ_LIMIT_BOARDTIME		= 5	* 60		-- ,5
YDBZ_TIME_WAIT_STATE1		= 10				-- boss10
YDBZ_TIME_WAIT_STATE3		= 30				-- 30
YDBZ_LIMIT_SETFIGHTSTATE= 3					-- 3
YDBZ_LIMIT_TEAMS_COUNT	= 15				-- 
YDBZ_LIMIT_PLAYER_LEVEL = 120				-- 
YDBZ_LIMIT_WEEK_COUNT		= 10 				-- 
YDBZ_LIMIT_DAY_COUNT		= 4 				-- 
YDBZ_PAIHANG_ID					=	10252			--ID
YDBZ_LIMIT_ITEM					= {{6, 1, 1613},1,"Anh Hïng ThiÕp"}	--tb1IDtb2
YDBZ_LIMIT_DOUBEL_ITEM	= {{6, 1, 1626},1,"Viªm §Õ LÖnh"}	--
YDBZ_AWARD_EXP 					= 600000									--
YDBZ_Faninl_AWARD_EXP		=	300000									--boss
YDBZ_KILLPLAYER_EXP 		= 200000									--
YDBZ_KILLLASTBOSS_EXP		= 1000000									--boss
YDBZ_BOAT_POS 					=													--3x,y
{
	[1]={60032,104832},
	[2]={59744,123296},
	[3]={52960,121952},
}
YDBZ_FIGHTING_RELIFT = 		--
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
-- ID
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
	local msg = format("Chóc mõng cao thñ <color=yellow>%s<color> thuéc tæ ®éi tiªu diÖt [L­¬ng Mi Nhi] ®· nhËn ®­îc phÇn th­ëng [%s] " ,GetName(),GetItemName(nItemIndex))
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

--
YDBZ_ZUANYONG_ITEM =				--
{
	[1] = {"H×nh nh©n",6,1,1605}, -- 1
	[2] = {"Viªm §Õ tr­êng mÖnh hoµn",	6,	0,	1607}, 
	[3] = {"Viªm §Õ gia bµo hoµn",	6,	0,	1608}, 
	[4] = {"Viªm §Õ ®¹i lùc hoµn",	6,	0,	1609}, 
	[5] = {"Viªm §Õ cao thiÓm hoµn",	6,	0,	1610}, 
	[6] = {"Viªm §Õ cao trung hoµn",	6,	0,	1611}, 
	[7] = {"Viªm §Õ phi tèc hoµn",	6,	0,	1612}, 
	[8] = {"Viªm §Õ b¨ng phßng hoµn",	6,	0,	1613}, 
	[9] = {"Viªm §Õ l«i phßng hoµn",	6,	0,	1614}, 
	[10] = {"Viªm §Õ hßa phßng hoµn",	6,	0,	1615}, 
	[11] = {"Viªm §Õ ®éc phßng hoµn",	6,	0,	1616}, 

}

-- 
YDBZ_tbaward_item ={
	[1]=--
	{
	},
	[2]=--boss
	{--%,,ID,(0,1),,
		{50,1,{6, 1, 1614,1,0,0},1,"H×nh nh©n",1},		--
		{100,15,{1, 2, 0, 5, 0, 0},0,"Ngò Hoa Ngäc Lé Hoµn",0}, --
		--{10,1,{6, 1, 1615,1,0,0},1,"Viªm §Õ §å §»ng",1},			--
	},
	[3]=--boss
	{
		{100,1,{6, 1, 1614,1,0,0},1,"H×nh nh©n",1},		--
		{100,30,{1, 2, 0, 5, 0, 0},0,"Ngò Hoa Ngäc Lé Hoµn",0},--
	},
--Modifiled by:ThanhLD - 20140226 - change award from Boss cuèi tÝnh n¨ng Viªm §Õ
	[4]=--boss
	{	
		[1] = {--PhÇn 1
			{szName="Tinh Tinh Kho¸ng",tbProp={6, 1, 4429,1,0,0},nCount=1,nRate=6.430},
			{szName="Tinh ThiÕt Kho¸ng",tbProp={6, 1, 4428,1,0,0},nCount=1,nRate=3.210},
		},
		[2] = {--PhÇn 2
			{szName="Hép MÆt N¹ ChiÕn Tr­êng",tbProp={6, 1, 4865,1,0,0},nCount=1,nRate=11.15,nExpiredTime=10080,tbParam={1,10080,60,0,0,0}},
			{szName="Hép MÆt N¹ ChiÕn Tr­êng",tbProp={6, 1, 4865,1,0,0},nCount=1,nRate=2.57,nExpiredTime=10080,tbParam={2,10080,60,0,0,0}},
			{szName="Phi phong Ngù Phong (Träng kÝch)",tbProp={0,3475},nCount=1,nRate=3,nQuality = 1,nExpiredTime=10080,},
			{szName="Phi phong Ngù Phong (X¸c suÊt hãa gi¶i s¸t th­¬ng)",tbProp={0,3474},nCount=1,nRate=2.1,nQuality = 1,nExpiredTime=10080,},
			{szName="Phi Phong PhÖ Quang ( träng kÝch)",tbProp={0,3478},nCount=1,nRate=1.5,nQuality = 1,nExpiredTime=10080,},
			{szName="Phi Phong CÊp PhÖ Quang (hãa gi¶i s¸t th­¬ng)",tbProp={0,3477},nCount=1,nRate=1,nQuality = 1,nExpiredTime=10080,},
		},
		[3] = {--PhÇn 3
			{szName="Tói D­îc PhÈm",tbProp={6, 1, 4813,1,0,0},nCount=1,nRate=40},
			{szName="Cµn Kh«n T¹o Hãa §an (®¹i) ",tbProp={6, 1, 214,1,0,0},nCount=50,nRate=30},
			{szName="§¹i Lùc hoµn",tbProp={6, 1, 3,1,0,0},nCount=1,nRate=15},
			{szName="Phi Tèc hoµn",tbProp={6, 1, 6,1,0,0},nCount=1,nRate=15},
		},
		[4] = {--PhÇn 4
			{szName = "§å Phæ §»ng Long Béi", tbProp = {6, 1, 4869,1,0,0}, nRate = 0.1,},
			{szName="§»ng Long Th¹ch - H¹",tbProp={6, 1, 4866,1,0,0},nCount=5,nRate=2},
			{szName = "§å Phæ Tinh S­¬ng Yªu §¸i", tbProp = {6, 1, 4867,1,0,0}, nRate = 0.11,},
			{szName = "§å Phæ Tinh S­¬ng Béi", tbProp = {6, 1, 4868,1,0,0}, nRate = 0.11,},
		},
	},
}

-- 2011.03.23
YDBZ_tbaward_item_ex = 
{
	[1] = {szName="Viªm §Õ BÝ B¶o",tbProp={6, 1, 3423,1,0,0}},		-- p
}

--
-- NPC
-- NPCIDBOSS(0,1)
YDBZ_NPC_ATTRIDX_PROCEED		= 1			-- NPC
YDBZ_NPC_ATTRIDX_ID			= 2			-- NPCID
YDBZ_NPC_ATTRIDX_NAME		= 3			-- NPC
YDBZ_NPC_ATTRIDX_LEVEL		= 4			-- NPC
YDBZ_NPC_ATTRIDX_SERIES		= 5			-- NPC
YDBZ_NPC_ATTRIDX_ISBOSS		= 6			-- BOSS
YDBZ_NPC_ATTRIDX_COUNT		= 7			-- NPC
YDBZ_NPC_ATTRIDX_POSITION	= 8			-- NPC
--

YDBZ_SCRIPT_NPC_DEATH 	= "\\script\\missions\\yandibaozang\\npc_death.lua"
YDBZ_SCRIPT_PLAYER_DEATH = "\\script\\missions\\yandibaozang\\player_death.lua"
--
---- 
YDBZ_map_series = {
	0,	-- 
	1,	-- 
	2,	-- Ë®
	3,	-- 
	4,	-- 
};


YDBZ_mapfile_trap =
{
	{"\\settings\\maps\\yandibaozang\\trap\\a","\\script\\missions\\yandibaozang\\trap\\a",10,"\\settings\\maps\\yandibaozang\\trap\\clear\\a"},
	{"\\settings\\maps\\yandibaozang\\trap\\b","\\script\\missions\\yandibaozang\\trap\\b",10,"\\settings\\maps\\yandibaozang\\trap\\clear\\b"},
	{"\\settings\\maps\\yandibaozang\\trap\\c","\\script\\missions\\yandibaozang\\trap\\c",10,"\\settings\\maps\\yandibaozang\\trap\\clear\\c"},
}