/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki襲
//	CreateTime:	2020-9-12
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#include "CoreObjGenreDef.h"
#include "CoreUseNameDef.h"

#define		ITEM_VERSION						1
#define		Def_MAX_STACK_TIENDONG	100
#define		Def_ID_XU_TIENDONG	417
#define		Def_MAXLEN_STRING_CHAT		90
#define		_CHAT_SCRIPT_OPEN
#define		OBJ_NAME_LENGHT						64
#define		MAX_TEAM_MEMBER						7		
#define		MAX_SENTENCE_LENGTH					256
#define		NUM_INFO_ITEM_CHAT					26				//26 th玭g tin 頲 slipt trong string g鰅 甶
#define		FILE_NAME_LENGTH					80
#define		PLAYER_PICKUP_CLIENT_DISTANCE		75//63
#define		defMAX_EXEC_OBJ_SCRIPT_DISTANCE		200
#define		defMAX_PLAYER_SEND_MOVE_FRAME		5
#define		PLAYER_PICKUP_SERVER_DISTANCE		40000
#define		MAX_INT								0x7fffffff
#define		STAMINA_RECOVER_SCALE	4
#define 	MAX_ITEM_MAGICATTRIB		6
#define		defNPC_GOLD_TYE		16
#define		defNPC_GOLD_SKILL_NO		5
#define		defNPC_GOLD_FIST_MAGIC_DAMGE		20
#define		POISON_DAMAGE_TIME				60
#define		POISON_DAMAGE_INTERVAL			10
#define		COLD_DAMAGE_TIME				60
#define		MAX_DEATLY_STRIKE_ENHANCEP		200
#define		MIN_FATALLY_STRIKE_ENHANCEP		30
#define		MAX_FATALLY_STRIKE_ENHANCEP		50
#define		DEF_DOWN_SKILLEXP90		50
#define		DEF_DOWN_SKILLEXP120	50
#define		LEVEL_EXPLOSIVE			120
#define		LIFE_EXPLOSIVE			25
#define		NUM_LOW_LIFETIME		2			//#thoi gian song item giam bot
#define		ROLE_NO									2
#define		PLAYER_MALE_NPCTEMPLATEID			-1
#define		PLAYER_FEMALE_NPCTEMPLATEID			-2
#define 	NORMAL_FONTSIZE						12
#define		PLAYER_SHARE_EXP_DISTANCE			768
#define		MAX_SCIRPTACTION_BUFFERNUM	512
#define		MAX_DEATH_PUNISH_PK_VALUE			10		// PK 0 n 10
#define		MAX_REPUTE_VALUE					100000	
#define		MAX_FUYUAN_VALUE					100000	
#define		MAX_REBORN_VALUE					100000
#define		MSG_NON_SETTINGS		"Kh玭g thi誸 l藀"
#define		TIMERTASK_SETTINGFILE	"\\settings\\TimerTask.txt"
#define		TASKVALUE_X2_EXP		36	//x2 甶觤 kinh nghi謒 c?nh﹏ ch誸 kh玭g m蕋
#define		TASKVALUE_STATTASK_HONOR		37	//lien dau
#define		TASKVALUE_STATTASK_ACCUM		38 //tong kim
#define		TASKVALUE_STATTASK_RESPECT		39	//diem uy danh t輈h lu?c玭g tr筺g
#define		TASKVALUE_STATTASK_REPUTE		73	//danh vong
#define		TASKVALUE_STATTASK_FUYUAN		74	//phuc duyen khong dung
#define		TASKVALUE_STATTASK_MATDOTHANBI		75//mat do than bi, son ha xa tac
#define		TASKVALUE_DB_PLAYER_TITLE_ID 71 //#PlayerTitle
#define		TASKVALUE_DB_PLAYER_TITLE_TIME	72 //#PlayerTitle
#define		TASKVALUE_DANHQUAIDATAU 89
#define		T_ANTI_HACK1 108 //add by phong ki襲 antihack	//ng ch璦 s?d鬾g
#define		T_ANTI_HACK2 109	//ng ch璦 s?d鬾g

#define		TASKVALUE_SAVEWAYPOINT_BEGIN 101
#define		TASKVALUE_MAXWAYPOINT_COUNT 3
#define		TASKVALUE_SAVESTATION_BEGIN  110 //task 110 n 143
#define		TASKVALUE_MAXSTATION_COUNT 32
#define		TASKVALUE_TIMERTASK_TAST	220
#define		TASKVALUE_TIMERTASK_RESTTIME 221
#define		TASKVALUE_STATTASK_XU			251

#define MAX_MISSION_PARAM			18 //fix by phong ki襲 m芻 nh 16
#define MISSION_PARAM_AVAILABLE		0
#define MISSION_AVAILABLE_VALUE		1
#define MAX_GLBMISSION_PARAM		3
#define	MISSION_STATNUM				10 //add by phong ki襲 using T鑞g kim battle 10 ngi top ten

struct TMissionLadderSelfInfo
{
	char		  szMissionName[64];
	unsigned char ucGroup;
	int			  nGlbParam[MAX_GLBMISSION_PARAM];
	int			  nParam[MAX_MISSION_PARAM];
	void Clear() {
		memset(szMissionName, 0, sizeof(szMissionName));
		memset(nGlbParam, 0, sizeof(nGlbParam));
		memset(nParam, 0, sizeof(nParam));
	};
};

struct TMissionLadderInfo
{
	unsigned char ucGroup;
	char		  Name[32];
	int			  nParam[MAX_MISSION_PARAM];
	void Clear() {
		ucGroup = 0;
		memset(Name, 0, sizeof(Name));
		memset(nParam, 0, sizeof(nParam));
	};
};

enum UI_TREMBLE_POSITION
{
	UIEP_BLUEITEM = 0,		
	UIEP_GEMLEVEL = 1,		
	UIEP_GEMSPIRIT = 2,		
	UIEP_GEMMETAL = 3,	
	UIEP_GEMWOOD = 4,		
	UIEP_GEMWATER = 5,		
	UIEP_GEMFIRE = 6,	
	UIEP_GEMEARTH = 7,	
};

enum UI_COMPOUND_POSITION
{
	UIEP_BOX1 = 0,		
	UIEP_BOX2 = 1,		
	UIEP_BOX3 = 2,		
};

enum UI_OUTIN_POSITION
{
	UIEP_ITEM_ = 0,		
	UIEP_CRYOLITE = 1,		
	UIEP_PROPMINE = 2,		
	UIEP_CONS1 = 3,	
	UIEP_CONS2 = 4,		
	UIEP_CONS3 = 5,		
	UIEP_CONS4 = 6,	
	UIEP_CONS5 = 7,	
	UIEP_CONS6 = 8,
	UIEP_CONS7 = 9,		
	UIEP_CONS8 = 10,	
};

enum UI_FORGE_POSITION
{
	UIEP_ITEMFOR = 0,		
	UIEP_CRYOLITEFOR = 1,		
};

struct KMapTraffic
{
	char	Content[80];
	int		Kind;
	POINT	Point;
	POINT	TPoint;
	int		MapIndex;
	void	Release() {
	memset(Content, 0, sizeof(Content));
	Kind = -1;
	Point.x = Point.y = TPoint.x = TPoint.y = MapIndex = 0;
	};
};

enum BOSS_STATE
{
	boss_none = 0,
	boss_blue,
	boss_event,
	boss_gold,
	boss_war,
	boss_muter,
	boss_num,
};

enum PAUTO_OPTION
{
	eSortItem = 48,
	eBuyItem,
	eInventoryIM,
	eRepairEquip,
	eReturnPortal,
	eAutoTuiDuocPham,
	eABanItem,
	//ePickInFightState,//nh苩  trong th祅h
	eAutoRightSkill,//nh chi猽 b猲 ph秈
	eAutoPTAll, //pt t蕋 c?
	enumcount,
};

enum ITEM_PART
{
	itempart_head = 0,	// 头 // m?
	itempart_body,		// 身体 //竜
	itempart_belt,		// 腰带 // 產i l璶g
	itempart_weapon,	// 武器 // v?kh?
	itempart_foot,	//gi祔
	itempart_cuff, //bao tay
	itempart_amulet, //d﹜ chuy襫
	itempart_ring1,	//nh蒼
	itempart_ring2,
	itempart_pendant,  //ng鋍 b閕
	itempart_horse, // ng鵤
	itempart_mask,	// mat na
	itempart_mantle,//#phi phong
	itempart_num,
};

enum TREMBLE_PART
{
	tremblepart_item = 0, 
	tremblepart_gemlevel,		
	tremblepart_gemspirit,		
	tremblepart_gemmetal,	
	tremblepart_gemwood,
	tremblepart_gemwater,
	tremblepart_gemfire,
	tremblepart_gemearth,
	tremblepart_num,
};

enum COMPOUND_PART
{
	compoundpart_box1 = 0, 
	compoundpart_box2,		
	compoundpart_box3,		
	compoundpart_num,
};

enum OUTIN_PART
{
	outinpart_item,
	outinpart_cryolite,
	outinpart_propmine,
	outinpart_consume1,
	outinpart_consume2,
	outinpart_consume3,
	outinpart_consume4,
	outinpart_consume5,
	outinpart_consume6,
	outinpart_consume7,
	outinpart_consume8,
	outinpart_num,
};

enum FORGE_PART
{
	forgepart_item = 0, 
	forgepart_cryolite,		
	forgepart_num,
};

typedef struct
{
	int		nIdx;
	int		nPlace;
	int		nX;
	int		nY;
	int		nPrice;
} PlayerItem;

enum INVENTORY_ROOM
{
	room_equipment = 0,	// trang b?tr猲 ngi
	room_repository,	// rng ch鴄 
	room_exbox1,		// mo rong ruong 1
	room_exbox2,		// mo rong ruong 2
	room_exbox3,		// mo rong ruong 3
	room_equipmentex,	// mo rong hanh trang
	room_trade,			// giao d辌h
	room_tradeback,		// 
	room_trade1,		// 
	room_immediacy,		// ph輒 t総
	room_give,				//10
	room_giveback,		//11
	room_affairitem,	//12
	room_num,			// 
};

enum ITEM_POSITION
{
	pos_hand = 1,		// tren tay
	pos_equip,			// mang tren nguoi		
	pos_equiproom,		// 	//hanh trang
	pos_repositoryroom,	//  ruong
	pos_exbox1room,		// mo rong ruong 1
	pos_exbox2room,		// mo rong ruong 2
	pos_exbox3room,		// mo rong ruong 3
	pos_equiproomex,	// mo rong hanh trang
	pos_traderoom,		// giao dich
	pos_trade1,			// 
	pos_immediacy,		// o phim tat
	pos_give,//
	pos_affairitem, //
	pos_tremble,		// Kham nam/nang cap trang bi xanh
	pos_compone,		// Luyen huyen tinh khoang thach
	pos_comptwo,		// Nang cap huyen tinh khoang thach 
	pos_compthree,		// Nang cap khoang thach thuoc tinh
	pos_distill,		// Rut Option trang bi
	pos_forge,			// Che tao trang bi tim 
	pos_enchase,		// Kham nam trang bi
	pos_skilltakewith,	//skill phim tat index 21 moi nhac len
	pos_immediacyskill, //skill phim tat index 22 khi dat xuong
	pos_num,
};

#define		AFFAIRITEM_ROOM_WIDTH		6
#define		AFFAIRITEM_ROOM_HEIGHT		4
#define		MAX_AFFAIR_ITEM				(AFFAIRITEM_ROOM_WIDTH * AFFAIRITEM_ROOM_HEIGHT)
#define		MAX_HAND_ITEM				1
#define		EQUIPMENT_ROOM_WIDTH		6
#define		EQUIPMENT_ROOM_HEIGHT		10
#define		MAX_EQUIPMENT_ITEM			(EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
#define		REPOSITORY_ROOM_WIDTH		6
#define		REPOSITORY_ROOM_HEIGHT		10
#define		MAX_REPOSITORY_ITEM			(REPOSITORY_ROOM_WIDTH * REPOSITORY_ROOM_HEIGHT)
#define		TRADE_ROOM_WIDTH			10
#define		TRADE_ROOM_HEIGHT			4
#define		MAX_TRADE_ITEM				(TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
#define		MAX_TRADE1_ITEM				MAX_TRADE_ITEM
#define		IMMEDIACY_ROOM_WIDTH		9
#define		IMMEDIACY_ROOM_HEIGHT		1
#define		MAX_IMMEDIACY_ITEM			(IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
#define		MAX_PLAYER_ITEM_RESERVED	32
//#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED)
#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM*5 + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_AFFAIR_ITEM + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED)
#define		MAX_ITEM_MAGICLEVEL			8
#define		MAX_NPC_LEVEL						120
#define		MAX_NPC_SERIES						  6 
#define		TIME_RIDE 5000 //edit by phong kieu thoi gian len xuong ngua
#define		TIME_PK	  180000	//edit by phong kieu thoi gian chuyen PK
#define		MAX_PERCENT							100
#define		REMOTE_REVIVE_TYPE			0
#define		LOCAL_REVIVE_TYPE			1
#define		MAX_MELEE_WEAPON			6
#define		HAND_PARTICULAR			6
#define		MAX_RANGE_WEAPON			3
#define		MAX_ARMOR					14
#define		MAX_HELM					14
#define		MAX_RING					1
#define		MAX_BELT					2
#define		MAX_PENDANT					2
#define		MAX_AMULET					2
#define		MAX_CUFF					2
#define		MAX_BOOT					4
#define		MAX_HORSE					6
#define		MAX_NPCSKILL			80
#define		REGION_PIXEL_WIDTH			512
#define		REGION_PIXEL_HEIGHT			1024
#define		REGION_CELL_SIZE_X			32
#define		REGION_CELL_SIZE_Y			32
#define		REGION_CELL_WIDTH			(REGION_PIXEL_WIDTH/REGION_CELL_SIZE_X)
#define		REGION_CELL_HEIGHT			(REGION_PIXEL_HEIGHT/REGION_CELL_SIZE_Y)
#define		MAX_TONG_LEVEL					10
#define		MAX_TONG_NATIONALEMBLEM			6
#define		MAX_NPCPARAM			4
#define		MAX_NPC_DIR		64
#define		MAX_WEAPON		MAX_MELEE_WEAPON + MAX_RANGE_WEAPON
#define		MAX_SKILL_STATE 18
#define		MAX_NPC_HEIGHT	128
#define		MAX_REDUCE		75
#define		MAX_RESIST		95
#define		MAX_HIT_PERCENT	95
#define		MIN_HIT_PERCENT	5
#define		MAX_NPC_RECORDER_STATE 18 //fix by Fong Ki襲 m芻 nh l?8
#define		PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE	5
#define	NORMAL_NPC_PART_NO		5	
#ifndef _SERVER
#define		C_REGION_X(x)	(LOWORD(SubWorld[0].m_Region[ (x) ].m_RegionID))
#define		C_REGION_Y(y)	(HIWORD(SubWorld[0].m_Region[ (y) ].m_RegionID))
#endif

enum
{
	CHAT_S_STOP = 0,						// 非聊天状态
	CHAT_S_SCREEN,							// 与同屏幕玩家聊天
	CHAT_S_SINGLE,							// 与同服务器某玩家私聊
	CHAT_S_TEAM,							// 与队伍全体成员交谈
	CHAT_S_NUM,								// 聊天状态中类数
};

enum PLAYER_INSTANT_STATE
{
	enumINSTANT_STATE_LEVELUP = 0,
	enumINSTANT_STATE_REVIVE,
	enumINSTANT_STATE_CREATE_TEAM,
	enumINSTANT_STATE_LOGIN,
	enumINSTANT_STATE_NUM,
};

enum CHAT_STATUS
{
	CHAT_S_ONLINE = 0,		//在线
	CHAT_S_BUSY,			//忙碌
	CHAT_S_HIDE,			//隐身
	CHAT_S_LEAVE,			//离开
	CHAT_S_DISCONNECT,		//掉线
};

// 注意：此枚举不允许更改(by zroc)
enum OBJ_ATTRIBYTE_TYPE
{
	series_metal,			//	金系
	series_wood,			//	木系
	series_water,			//	水系
	series_fire,			//	火系
	series_earth,			//	土系
	series_nil,
	series_num = series_nil,
	series_minus,
};

enum OBJ_GENDER
{
	OBJ_G_MALE	= 0,	//雄性，男的
	OBJ_G_FEMALE,		//雌的，女的
};

enum NPCCAMP
{
	camp_begin,	// 0 ch?tr緉g
	camp_justice, // 1 ch竛h ph竔
	camp_evil, // 2 t?ph竔
	camp_balance, // 3 trung l藀
	camp_free,	// 4 m祏  s竧 th?
	camp_animal, // 5 m祏 h錸g admin
	camp_event,		// 6 m祏 h錸g admin
	camp_audience, // 7
	camp_tongwar, // 8
	camp_num, // 9
};

enum NPCFIGHT // trang thai chien dau
{
	fight_none = 0,				// trang thai luyen cong
	fight_active,			// trang thai chien dau
	fight_num,				// num
};

enum
{
	enumPKNormal = 0,
	enumPKWar,
	enumPKMurder,
	enumPKTongWar,
	enumPKNum,
};


enum ITEM_IN_ENVIRO_PROP
{
	IIEP_NORMAL = 0,	//一般/正常/可用
	IIEP_NOT_USEABLE,	//不可用/不可装配
	IIEP_SPECIAL,		//特定的不同情况
};

#define	GOD_MAX_OBJ_TITLE_LEN	2048	//128临时改为1024为了兼容旧代码 to be modified
#define	GOD_MAX_OBJ_PROP_LEN	1024
#define	GOD_MAX_OBJ_DESC_LEN	1024

//==================================
//	游戏对象的描述
//==================================
struct KGameObjDesc
{
	char	szTitle[GOD_MAX_OBJ_TITLE_LEN];	//标题，名称
	char	szProp[GOD_MAX_OBJ_PROP_LEN];	//属性，每行可以tab划分为靠左与靠右对齐两部分
	char	szDesc[GOD_MAX_OBJ_DESC_LEN];	//描述
};

//==================================
//	问题与可选答案
//==================================

struct KUiAnswer
{
	char	AnswerText[256];	//可选答案文字（可以包含控制符）
	int		AnswerLen;			//可选答案存储长度（包括控制符，不包含结束符）
};

struct KTongParam
{
	int		m_nLevel;
	int		m_nLeadLevel;
	int		m_nMoney;
	int		m_nItemCount;
	int		m_nItemGenre;
	int		m_nItemDetail;
	int		m_nRepute;
	int		m_nFuYuan;
	int		m_nLeaveMoney;
	int		m_nLeaveRepute;
	int		m_nLeaveTime;
	int		m_nMoneyChangeCamp;
	int		m_nMoneySaveJiyu;
};

struct KTime
{
	int bYear;
	BYTE bMonth;
	BYTE bDay;
	BYTE bHour;
};

struct KUiQuestionAndAnswer
{
	char		Question[1024];	
	int			QuestionLen;	
	int			AnswerCount;	
	KUiAnswer	Answer[1];	
};

struct KUiNpcSpr 
{ 
    char                ImageFile[128]; 
    unsigned short         MaxFrame; 
};

typedef struct
{
	short		m_nID;				// 物品的ID
	BYTE	m_btGenre;			// 物品的类型
	short		m_btDetail;			// 物品的类别
	short		m_btParticur;		// 物品的详细类别
	BYTE	m_btSeries;			// 物品的五行
	BYTE	m_btLevel;			// 物品的等级
	BYTE	m_btLuck;			// MF
	short		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// 生成参数
	WORD	m_wVersion;			// 装备版本
	DWORD	m_dwRandomSeed;		// 随机种子
	short		m_nIdx;
	UINT	m_uPrice;
	short		m_bX;
	short		m_bY;
	BYTE	m_bPoint;
	BYTE	m_bStack;
	BYTE	m_bEnChance;
	short		m_nGoldId;
	int		m_YearExp;
	int		m_Lock;
	int		m_HLock;
	int		m_nDurability;
} ChatItem;

struct KOneMsgInfo
{
	int			 nLines;				//这条信息占了多少行
	int			 nCharWidth;			//这条信息占了字符宽
	ChatItem	 sItem;					//thong tin cua item
	int			 nHaveItem;				//co item khong?
	int			 nPos;					// vi tri trong cau
	int			 nFace;					// so icon trong 1 dong
	int			 nLinePos;				// item nam dong thu...
	unsigned int uTextBKColor;			//这条信息字显示时的文字衬底的颜色,0为无
	int			 nMaxSize;				//Msg可以容纳的最大信息长度
	int			 nLen;					//信息长度
	char		 Msg[1];				//信息的内容
};

enum ITEMKIND
{
	normal_item = 0,	// tr緉g
	green_item,				// xanh
	broken_item,			// b?h醤g
	gold_item ,					//hoang kim
	purple_item,				//do tim
	platinum_item,			//bach kim
	kind_number,		
};

enum ITEMGENRE
{
	item_equip = 0,			
	item_medicine,			
	item_mine,				
	item_materials,			
	item_task,				
	item_townportal,
	item_magicscript,
	item_number,			
};

enum EQUIPNATURE
{
	NATURE_NORMAL = 0,
	NATURE_VIOLET,
	NATURE_GOLD,
	NATURE_PLATINA,
	NATURE_NUM,
};

enum EQUIPLEVEL
{
	equip_normal = 0,		
	equip_magic,			
	equip_rare,				
	equip_unique,			
	equip_set,				
	equip_number,	
	equip_gold,				
};

enum EQUIPDETAILTYPE
{
	equip_meleeweapon = 0,
	equip_rangeweapon,
	equip_armor,
	equip_ring,
	equip_amulet,
	equip_boots,
	equip_belt,
	equip_helm,
	equip_cuff,
	equip_pendant,
	equip_horse,
	equip_mask,	// mat na	// 11
	equip_mantle, //#phi phong // 12
	equip_detailnum, 
};

enum MEDICINEDETAILTYPE
{
	medicine_blood = 0,
	medicine_mana,
	medicine_both,
	medicine_stamina,
	medicine_antipoison,
	medicine_detailnum,
};

struct KUiGameObject
{
	unsigned int uGenre;	//对象类属
	unsigned int uId;		//对象id
//	int			 nData;		//与对象实例相关的某数据
};

//==================================
//	以坐标表示的一个区域范围
//==================================
struct KUiRegion
{
	int		h;		//左上角起点横坐标
	int		v;		//左上角起点纵坐标
	int		Width;	//区域横宽
	int		Height;	//区域纵宽
};

enum UIOBJECT_CONTAINER
{
	UOC_IN_HAND	= 1,		//手中拿着
	UOC_GAMESPACE,			//游戏窗口
	UOC_IMMEDIA_ITEM,		//快捷物品
	UOC_IMMEDIA_SKILL,		//快捷武功0->右键武功，1,2...-> F1,F2...快捷武功
	UOC_ITEM_TAKE_WITH,		//随身携带
	UOC_SKILL_TAKE_WITH,
	UOC_TO_BE_TRADE,		//要被买卖，买卖面板上
	UOC_OTHER_TO_BE_TRADE,	//买卖面板上，别人要卖给自己的，
	UOC_EQUIPTMENT,			//身上装备
	UOC_NPC_SHOP,			//npc买卖场所
	UOC_MARKET,
	UOC_STORE_BOX,			//储物箱
	UOC_EX_BOX1,
	UOC_EX_BOX2,
	UOC_EX_BOX3,
	UOC_ITEM_EX,
	UOC_SKILL_LIST,			//列出全部拥有技能的窗口，技能窗口
	UOC_SKILL_TREE,			//左、右可用技能树
	UOC_ITEM_GIVE,
	UOC_AFFAIR_ITEM,		// pos tra vat pham nhiem vu
	UOC_TREMBLE_ITEM,		//Kham nam/nang cap trang bi xanh
	UOC_COMPONE_ITEM,		//Luyen huyen tinh khoang thach
	UOC_COMPTWO_ITEM,		//Nang cap huyen tinh khoang thach
	UOC_COMPTHREE_ITEM,		//Nang cap khoang thach thuoc tinh
	UOC_DISTILL_ITEM,		//Rut option trang bi
	UOC_FORGE_ITEM,			//Che tao trang bi tim
	UOC_ENCHASE_ITEM,		//Kham nam trang bi
};

enum MONEYUNIT
{
	moneyunit_money = 0,		//tien van
	moneyunit_extpoint,				//xu
	moneyunit_fuyuan,				//diem phuc duyen
	moneyunit_repute,				//diem danh vong
	moneyunit_accum,			//diem tich luy tong kim
	moneyunit_honor,				//diem vinh du lien dau
	moneyunit_respect,				//diem uy danh
	moneyunit_num,
};

enum UI_TRADE_OPER_DATA
{
	UTOD_IS_WILLING,	

	UTOD_IS_LOCKED,		

	UTOD_IS_TRADING,		

	UTOD_IS_OTHER_LOCKED,	

};

struct KUiGiveBox           
{
	char	szTitle[32];
	char	szInitString[256];
	char	szAction1[64];
};

struct KUiTimeBoxInfo           
{
	char	szTitle[32];
	int		nTime;
	char	szAction[64];
};

struct KUiInPutBoxCmd           
{
	char	nValue[32];		// ten ham
	int		nNum;			// gia tri so
	char	szAction[64];	// gia tri chuoi
};

struct KUiItemBuySelInfo
{
	int				nItemNature;
	char			szItemName[64];	//物品名称
	int				nOldPrice;
	int				nCurPrice;			//买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
	int				nMoneyUnit;
	BOOL			bNewArrival;
	int				nPrice;			//买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
	int				nPriceXu;
};

//==================================
//	表示某个游戏对象在坐标区域范围的信息结构
//==================================
struct KUiObjAtRegion
{
	KUiGameObject	Obj;
	KUiRegion		Region;
};

struct KUiObjAtContRegion : public KUiObjAtRegion
{
	union
	{
		UIOBJECT_CONTAINER	eContainer; 
		int					nContainer;
	};
};

struct KUiBreakItemOption
{
	int		  num;
	bool	isbreakall;
};

struct KUiMsgParam
{
	unsigned char	eGenre;	//取值范围为枚举类型MSG_GENRE_LIST,见MsgGenreDef.h文件
	unsigned char	cChatPrefixLen;
	unsigned short	nMsgLength;
	char			szName[32];
#define	CHAT_MSG_PREFIX_MAX_LEN	16
	unsigned char	cChatPrefix[CHAT_MSG_PREFIX_MAX_LEN];
};

struct KUiInformationParam
{
	char	sInformation[1024];	
	char	sConfirmText[64];	
	short	nInforLen;			
	bool	bNeedConfirmNotify;	
	bool	bReserved;
};

enum PLAYER_ACTION_LIST
{
	PA_NONE = 0,	//无动作
	PA_RUN  = 0x01,	//跑
	PA_SIT  = 0x02,	//打坐
	PA_RIDE = 0x04,	//骑（马）
	EX_BOX,			// mo rong ruong 1
	EX_BOX2,		// mo rong ruong 2
	EX_BOX3,		// mo rong ruong 3
	ITEMEX,			// hanh trang
	HT_CN,			// chuc nang dang hoan thien
	OPEN_MARKET,	// ky tran cac
	PW_NOT_SAME,	// mat khau ko giong nhau
	PW_ACCEPTED,	// mat khau dc chap nhan
	PW_NOT_LONG,	// mat khau ko du do dai
	CN_GH,			// chuc nang gioi han
	KD_VP,
};

//==================================
//	系统消息分类
//==================================
enum SYS_MESSAGE_TYPE
{
	SMT_NORMAL = 0,	//不参加分类的消息
	SMT_SYSTEM,		//系统，连接相关
	SMT_PLAYER,		//玩家相关
	SMT_TEAM,		//组队相关
	SMT_FRIEND,		//聊天好友相关
	SMT_MISSION,	//任务相关
	SMT_CLIQUE,		//帮派相关
};

enum SYS_MESSAGE_CONFIRM_TYPE
{
	SMCT_NONE,				
	SMCT_CLICK,				
	SMCT_MSG_BOX,			
	SMCT_UI_RENASCENCE,		
	SMCT_UI_ATTRIBUTE,		
	SMCT_UI_SKILLS,			
	SMCT_UI_ATTRIBUTE_SKILLS,
	SMCT_UI_TEAM_INVITE,
	SMCT_UI_TEAM_APPLY,	
	SMCT_UI_TEAM,			
	SMCT_UI_INTERVIEW,		
	SMCT_UI_FRIEND_INVITE,	
	SMCT_UI_TRADE,
	SMCT_DISCONNECT,		
	SMCT_UI_TONG_JOIN_APPLY,
	SMCT_UI_ASKASSEMBLE,	// Thong bao xac nhan kham trang bi xanh
};

struct KSystemMessage
{
	char			szMessage[128];	//消息文本
	unsigned int	uReservedForUi;	//界面使用的数据域,core里填0即可
	unsigned char	eType;			//消息分类取值来自枚举类型 SYS_MESSAGE_TYPE
	unsigned char	byConfirmType;	//响应类型
	unsigned char	byPriority;		//优先级,数值越大，表示优先级越高
	unsigned char	byParamSize;	//伴随GDCNI_SYSTEM_MESSAGE消息的pParamBuf所指参数缓冲区空间的大小。
};

struct KUiChatChannel
{
	int			 nChannelNo;
	unsigned int uChannelId;
	union
	{
		int		 nChannelIndex;
		int		 nIsSubscibed;	
	};
	char		 cTitle[32];
};

struct KUiChatGroupInfo
{
	char	szTitle[32];	//分组的名称
	int		nNumFriend;		//组内好友的数目
};

struct KUiChatMessage
{
	unsigned int uColor;
	short	nContentLen;
	char	szContent[256];
};

struct KUiPlayerBaseInfo
{
	char	Agname[32];	//绰号
	char	Name[32];	//名字
	char	Title[32];	//称号
	char	szMateName[32]; //#MateName
	int		nCurFaction;// 当前加入门派 id ，如果为 -1 ，当前没有在门派中
	int		nRankInWorld;//江湖排名值,值为0表示未上排名板
	unsigned int nCurTong;// 当前加入帮派name id ，如果为 0 ，当前没有在帮派中
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int		nPKValue; // pk
	int		nMissionGroup;
};

struct KUiItemShow
{
	unsigned int nItemIdx;
	char sItemName[64];
	char sItemDesc[5024];
	char sItemIconSPR[512];
};

struct KUiPlayerRuntimeInfo
{
	int		nLifeFull;			//生命满值
	int		nLife;				//生命
	int		nManaFull;			//内力满值
	int		nMana;				//内力
	int		nStaminaFull;		//体力满值
	int		nStamina;			//体力
	int		nAngryFull;			//怒满值
	int		nAngry;				//怒
	double		nExperienceFull;	//经验满值
	double		nExperience;		//当前经验值
	double		nCurLevelExperience;//当前级别升级需要的经验值

	unsigned char	byActionDisable;//是否不可进行各种动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned char	byAction;	//正在进行的行为动作，为枚举PLAYER_ACTION_LIST取值的组合
	unsigned short	wReserved;	//保留
};

enum UI_PLAYER_ATTRIBUTE
{
	UIPA_STRENGTH = 0,			//力量
	UIPA_DEXTERITY,				//敏捷
	UIPA_VITALITY,				//活力
	UIPA_ENERGY,				//精力
};

struct KUiPlayerAttribute
{
	int		nMoney;				//银两
	int		nXu;
	int		nLevel;				//等级
	char	StatusDesc[16];		//状态描述

	int		nBARemainPoint;		//基本属性剩余点数
	int		nStrength;			//力量
	int		nDexterity;			//敏捷
	int		nVitality;			//活力
	int		nEnergy;			//精力

	int		nKillMAX;			//最大杀伤力
	int		nKillMIN;			//最小杀伤力
	int		nRightKillMax;		//右键最大杀伤力
	int		nRightKillMin;		//右键最小杀伤力

	int		nAttack;			//攻击力
	int		nDefence;			//防御力
	int		nMoveSpeed;			//移动速度
	int		nAttackSpeed;		//攻击速度
	int		nCastSpeed;		//攻击速度

	int		nPhyDef;			//物理防御
	int		nCoolDef;			//冰冻防御
	int		nLightDef;			//闪电防御
	int		nFireDef;			//火焰防御
	int		nPoisonDef;			//毒素防御

	int		nPhyDefPlus;			//物理防御
	int		nCoolDefPlus;			//冰冻防御
	int		nLightDefPlus;			//闪电防御
	int		nFireDefPlus;			//火焰防御
	int		nPoisonDefPlus;			//毒素防御

	int		nRankInWorld;	// xep hang
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int		nPKValue; // phuc duyen
};

#define		RESIST_PLUS_SCALE		15		//Kh竛g t╪g th猰 75+

#define	UPB_IMMEDIA_ITEM_COUNT	9

//==================================
//	主角的立即使用物品与武功
//==================================
struct KUiPlayerImmedItemSkill
{
	KUiGameObject	ImmediaItem[UPB_IMMEDIA_ITEM_COUNT];
	KUiGameObject	IMmediaSkill[2];
};

//==================================
//	主角装备安换的位置
//==================================
enum UI_EQUIPMENT_POSITION
{
	UIEP_HEAD = 0,		//头戴
	UIEP_HAND = 1,		//手持
	UIEP_NECK = 2,		//脖子
	UIEP_FINESSE = 3,	//手腕
	UIEP_BODY = 4,		//身穿
	UIEP_WAIST = 5,		//腰部
	UIEP_FINGER1 = 6,	//手指甲
	UIEP_FINGER2 = 7,	//手指乙
	UIEP_WAIST_DECOR= 8,//腰坠
	UIEP_FOOT = 9,		//脚踩
	UIEP_HORSE = 10,	//马匹
	UIEP_MASK = 11,		// mat na
	UIEP_FIFONG = 12 //#phi phong
};

//==================================
//	主角的生活技能数据
//==================================
struct KUiPlayerLiveSkillBase
{
	int		nRemainPoint;			//剩余技能点数
	int		nLiveExperience;		//当前技能经验值
	int		nLiveExperienceFull;	//升到下级需要的经验值
};

//==================================
//	单项技能数据
//==================================
struct KUiSkillData : public KUiGameObject
{
	union
	{
		int		nLevel;
		int		nData;
	};
};

//==================================
//	一个队伍中最多包含成员的数目
//==================================
#define	PLAYER_TEAM_MAX_MEMBER	500

//==================================
//	统帅能力相关的数据
//==================================
struct KUiPlayerLeaderShip
{
	int		nLeaderShipLevel;			
	int		nLeaderShipExperience;		
	int		nLeaderShipExperienceFull;	
};

struct KUiPlayerPaintTeamMNG
{
	int		nCountS;
	int		nWeightOffset;
	int		nHeightOffset_life;
	int		nHeightOffset_mana;
	int		nOffSet_member;
	int		nWid;
	int		nHei_life;
	int		nHei_mana;
};

struct FKUiObjectName //fkauto
{
	char	FkName[64];
};

struct KUiPlayerItem
{
	char			Name[32];	
	unsigned int	uId;		
	int				nIndex;		
	int				nData;		
	int				nTeamID;
	BYTE	   nFaction;
	BYTE		nPercenLife;
	BYTE		nPercenMana;
};

struct KStateTempNode
{
	int		nSkillId;
	int		nLeftTime;
};

//==================================
//	组队信息的描述
//==================================
struct KUiTeamItem
{
	KUiPlayerItem	Leader;
};

//==================================
//	队伍信息
//==================================
struct KUiPlayerTeam
{
	bool			bTeamLeader;			//玩家自己是否队长
	char			cNumMember;				//队员数目
	char			cNumTojoin;				//欲加入的人员的数目
	bool			bOpened;				//队伍是否允许其他人加入
	int				nTeamServerID;			//队伍在服务器上的id，用于标识该队伍，-1 为空
	int				nCaptainPower;
};

//==================================
//	魔法属性
//==================================
#ifndef MAGICATTRIB
#define MAGICATTRIB
struct KMagicAttrib
{
	int				nAttribType;					//属性类型
	int				nValue[3];						//属性参数
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};

struct KMagicAutoSkill
{
	int				nSkillId;
	int				nSkillLevel;
	int				nRate;						//属性参数
	DWORD			dwNextCastTime;
	int				nWaitCastTime;
	KMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = 0;};
};
#else
struct KMagicAttrib;
struct KMagicAutoSkill;
#endif

/* 这是旧的代码，新的已经放在KNpcGold里面处理了
//==================================
//	NPC加强
//==================================
struct KNpcEnchant
{
	int		nExp;					// 经验
	int		nLife;					// 生命
	int		nLifeReplenish;			// 回血
	int		nAttackRating;			// 命中
	int		nDefense;				// 防御
	int		nMinDamage;
	int		nMaxDamage;

	int		TreasureNumber;				// 装备
	int		AuraSkill;					// 光环
	int		DamageEnhance;				// 伤害
	int		SpeedEnhance;				// 速度
	int		SelfResist;					// 自身抗性
	int		ConquerResist;				// 相生抗性
#ifndef _SERVER
	char	NameModify[32];				// 改名
#endif
};

//==================================
//	NPC单项加强
//==================================
struct KNpcSpeicalEnchant
{
	int		ValueModify;
	char	NameModify[16];
};
*/

struct KMapPos
{
	int		nSubWorld;
	int		nRegion;
	int		nMapX;
	int		nMapY;
	int		nOffX;
	int		nOffY;
};

//==================================
//	选项设置项
//==================================
enum OPTIONS_LIST
{
	OPTION_PERSPECTIVE,		//透视模式  nParam = (int)(bool)bEnable 是否开启
	OPTION_DYNALIGHT,		//动态光影	nParam = (int)(bool)bEnable 是否开启
	OPTION_MUSIC_VALUE,		//音乐音量	nParam = 音量大小（取值为0到-10000）
	OPTION_SOUND_VALUE,		//音效音量	nParam = 音量大小（取值为0到-10000）
	OPTION_BRIGHTNESS,		//亮度调节	nParam = 亮度大小（取值为0到-100）
	OPTION_WEATHER,			//天气效果开关 nParam = (int)(bool)bEnable 是否开启
	OPTION_QUALITY_GIAMPLAYER,//add by phong ki襲
	OPTION_QUALITY_MATNPC,
	OPTION_QUALITY_MATPLAYER,
	OPTION_QUALITY_GIAMSKILL,
	OPTION_QUALITY_MATSKILL,
};

//==================================
//	所处的地域时间环境信息
//==================================
struct KUiSceneTimeInfo
{
	char	szSceneName[32];		//场景名
	int		nSceneId;				//场景id
	int		nScenePos0;				//场景当前坐标（东）
	int		nScenePos1;				//场景当前坐标（南）
	int		nGameSpaceTime;			//以分钟为单位
};

struct KUiSceneTimeInfoOften : public KUiSceneTimeInfo
{
};

//==================================
//	光源信息
//==================================
//整数表示的三维点坐标
struct KPosition3
{
	int nX;
	int nY;
	int nZ;
};

struct KLightInfo
{
	KPosition3 oPosition;			// 光源位置
	DWORD dwColor;					// 光源颜色及亮度
	long  nRadius;					// 作用半径
};


//小地图的显示内容项
enum SCENE_PLACE_MAP_ELEM
{ 
	SCENE_PLACE_MAP_ELEM_NONE		= 0x00,		//无东西
	SCENE_PLACE_MAP_ELEM_PIC		= 0x01,		//显示缩略图
	SCENE_PLACE_MAP_ELEM_CHARACTER	= 0x02,		//显示人物
	SCENE_PLACE_MAP_ELEM_PARTNER	= 0x04,		//显示同队伍人
};

//场景的地图信息
struct KSceneMapInfo
{
	int	nScallH;		//真实场景相对于地图的横向放大比例
	int nScallV;		//真实场景相对于地图的纵向放大比例
	int	nFocusMinH;
	int nFocusMinV;
	int nFocusMaxH;
	int nFocusMaxV;
	int nOrigFocusH;
	int nOrigFocusV;
	int nFocusOffsetH;
	int nFocusOffsetV;
};

enum NPC_RELATION
{
	relation_none	= 1,
	relation_self	= 2,
	relation_ally	= 4,
	relation_enemy	= 8,
	relation_dialog	= 16,
	relation_all	= relation_none | relation_ally | relation_enemy | relation_self | relation_dialog,	
	relation_num,
};

enum NPCKIND
{
	kind_normal = 0	,		//qu竔 b譶h thng
	kind_player,				//ngi ch琲
	kind_partner,				//b筺 ng h祅h
	kind_dialoger,				//i tho筰
	kind_bird,					//bay kh玭g th蕐 s?d鬾g
	kind_mouse,				//chu閠 kh玭g th蕐 s?d鬾g
    kind_num				//t鑙 產
};

enum	// 物件类型
{
	Obj_Kind_MapObj = 0,		// 地图物件，主要用于地图动画
	Obj_Kind_Body,				// npc 的尸体
	Obj_Kind_Box,				// 宝箱
	Obj_Kind_Item,				// 掉在地上的装备
	Obj_Kind_Money,				// 掉在地上的钱
	Obj_Kind_LoopSound,			// 循环音效		//Loop
	Obj_Kind_RandSound,			// 随机音效
	Obj_Kind_Light,				// 光源（3D模式中发光的东西）
	Obj_Kind_Door,				// 门类
	Obj_Kind_Trap,				// 陷阱
	Obj_Kind_Prop,				// 小道具，可重生
	Obj_Kind_Task,				// Obj nhiem vu
	Obj_Kind_Num,				// 物件的种类数
};

//主角身份地位等一些关键属性项
enum PLAYER_BRIEF_PROP
{
	PBP_LEVEL = 1,	//登级变化	nParam表示当前等级
	PBP_FACTION,	//门派		nParam表示门派属性，如果nParam为-1表示没有门派
	PBP_CLIQUE,		//帮派		nParam为非0值表示入了帮派，0值表示脱离了帮派
};

//新闻消息的类型定义
enum NEWS_MESSAGE_TYPE
{
	NEWSMESSAGE_NORMAL,			//一般消息，显示（一次）就消息消亡了
								//无时间参数
	NEWSMESSAGE_COUNTING,		//倒计（秒）数消息，计数到0时，就消息就消亡了。
								//时间参数中的数据结构中仅秒数据有效，倒计数以秒为单位。
	NEWSMESSAGE_TIMEEND,		//定时消息，定时到时，消息就消完了，否则每半分钟显示一次。
								//时间参数表示消亡的指定时间。
	NEWSMESSAGE_NORMAL_1,		// moi them	
	
	NEWSMESSAGE_COUNTING_1,		//moi them	

	NEWSMESSAGE_TIMEEND_1,		//moi them

	NEWSMESSAGE_NORMAL_2,		// moi them	
};

#define MAX_MESSAGE_LENGTH 512

struct KNewsMessage
{
	int		nType;						//消息类型
	char	sMsg[MAX_MESSAGE_LENGTH];	//消息内容
	int		nMsgLen;					//消息内容存储长度
};

struct KNewsMessage1
{
	int		nType;						
	char	sMsg[MAX_MESSAGE_LENGTH];	
	int		nMsgLen;					
};

struct KNewsMessage2
{
	int		nType;						
	char	sMsg[MAX_MESSAGE_LENGTH];	
	int		nMsgLen;					
};

struct KRankIndex
{
	bool			bValueAppened;	//每一项是否有没有额外数据
	bool			bSortFlag;		//每一项是否有没有升降标记
	unsigned short	usIndexId;		//排名项ID数值
};

#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
	char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];	// 文字内容
	unsigned short		usMsgLen;				// 文字内容的长度
	short				cSortFlag;				// 旗标值，QOO_RANK_DATA的时候表示出升降，负值表示降，正值表示升，0值表示位置未变
	int					nValueAppend;			// 此项附带的值

};

struct KMissionRecord
{
	char			sContent[256];
	int				nContentLen;
	unsigned int	uValue;	
	//char			szContent[MAX_SCIRPTACTION_BUFFERNUM];
	//int				nRecordAct;
};

//---------------------------- 帮会相关 ------------------------

#define		defTONG_MAX_DIRECTOR				7
#define		defTONG_MAX_MANAGER					56
#define		defTONG_ONE_PAGE_MAX_NUM			100
#define		defTONG_STR_LENGTH					32
#define		defTONG_NAME_MAX_LENGTH				12

enum TONG_MEMBER_FIGURE
{
	enumTONG_FIGURE_MEMBER,				// th祅h vi猲
	enumTONG_FIGURE_MANAGER,			// 阯g ch?
	enumTONG_FIGURE_DIRECTOR,			// trng l穙
	enumTONG_FIGURE_MASTER,				// bang ch?
	enumTONG_FIGURE_LEAGUE,				//lien minh
	enumTONG_FIGURE_NUM,
};

enum
{
	enumTONG_APPLY_INFO_ID_SELF,		// 申请查询自身信息
	enumTONG_APPLY_INFO_ID_MASTER,		// 申请查询帮主信息
	enumTONG_APPLY_INFO_ID_DIRECTOR,	// 申请查询长老信息
	enumTONG_APPLY_INFO_ID_MANAGER,		// 申请查询队长信息
	enumTONG_APPLY_INFO_ID_MEMBER,		// 申请查询帮众信息(一批帮众)
	enumTONG_APPLY_INFO_ID_ONE,			// 申请查询某帮会成员信息(一个帮众)
	enumTONG_APPLY_INFO_ID_TONG_HEAD,	// 申请查询某帮会信息，用于申请加入帮会
	enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
	enumTONG_CREATE_ERROR_ID1,	// Player[m_nPlayerIndex].m_nIndex <= 0
	enumTONG_CREATE_ERROR_ID2,	// 交易过程中
	enumTONG_CREATE_ERROR_ID3,	// 帮会名问题
	enumTONG_CREATE_ERROR_ID4,	// 帮会阵营问题
	enumTONG_CREATE_ERROR_ID5,	// 已经是帮会成员
	enumTONG_CREATE_ERROR_ID6,	// 自己的阵营问题
	enumTONG_CREATE_ERROR_ID7,	// 等级问题	
	enumTONG_CREATE_ERROR_ID8,	// 钱问题
	enumTONG_CREATE_ERROR_ID9,	// 组队不能建帮会
	enumTONG_CREATE_ERROR_ID10,	// 帮会模块出错
	enumTONG_CREATE_ERROR_ID11,	// 名字字符串出错
	enumTONG_CREATE_ERROR_ID12,	// 名字字符串过长
	enumTONG_CREATE_ERROR_ID13,	// 帮会同名错误
	enumTONG_CREATE_ERROR_ID14,	// 帮会产生失败
	enumTONG_CREATE_ERROR_ID15,
};

enum
{
	enumTONG_CHANGE_CAMP_ERROR_ID1,
	enumTONG_CHANGE_CAMP_ERROR_ID2,
	enumTONG_CHANGE_CAMP_ERROR_ID3,
	enumTONG_CHANGE_CAMP_ERROR_ID4,
	enumTONG_CHANGE_CAMP_ERROR_ID5,
	enumTONG_CHANGE_CAMP_ERROR_ID6,
	enumTONG_CHANGE_CAMP_ERROR_ID7,
	enumTONG_CHANGE_CAMP_ERROR_ID8,
};

struct KTongInfo
{
	char  szName[32];			// fix tang len hien thi o ui
	char  szMasterName[32];		//帮主名字
	int   nFaction;				//帮会阵营
	int   nMoney;				//帮会资金
	int   nReserve[2];
	int	  nMemberCount;         //帮众数量
	int   nManagerCount;        //队长数量
	int   nDirectorCount;       //长老数量
	int   nFigure;              //待删除
	//===========add by Fong Ki襲 12/12/2021==========
	int		nStatusGuide; //t譶h tr筺g bang h閕
	char  szWayEdit[32];			// ti猽 ch?bang h閕
	char  szNextTargetEdit[32];			// m鬰 ti猽 hi謓 th阨 bang h閕
	int		nExpGuide; //甶觤 kinh nghi謒 bang h閕
	int		nCityGuide; //th祅h th?chi誱 h鱱
	int		nTongLevel; //ng c蕄 bang h閕
	char  szLeagueTName[32];		//t猲 bang h閕 li猲 minh
};

enum TONG_ACTION_TYPE
{
	TONG_ACTION_DISMISS,       //踢人
	TONG_ACTION_ASSIGN,        //任命
	TONG_ACTION_DEMISE,        //传位
	TONG_ACTION_LEAVE,         //离帮
	TONG_ACTION_RECRUIT,       //招人状态
	TONG_ACTION_APPLY,         //申请加入
	TONG_ACTION_CHANGE_TITLE,
	TONG_ACTION_CHANGE_MALE_TITLE,
	TONG_ACTION_CHANGE_FEMALE_TITLE,
	TONG_ACTION_CHANGE_CAMP_JUSTIE,
	TONG_ACTION_CHANGE_CAMP_EVIL,
	TONG_ACTION_CHANGE_CAMP_BALANCE,
	TONG_ACTION_CHANGE_WAYEDIT,
	TONG_ACTION_CHANGE_NEXTTARGET,
};

enum TONG_MONEY_ACTION_TYPE
{
	TONG_ACTION_SAVE,       //踢人
	TONG_ACTION_GET,        //任命
	TONG_ACTION_SND,        //传位
};

struct KTongOperationParam
{
	TONG_ACTION_TYPE	eOper;				
	int                 nData[4];
	char				szPassword[32];
	char				lpszParam[32];
};

struct KTongMemberItem : public KUiPlayerItem
{
	char szAgname[64];	
	int btOnline;
};

struct KUiPlayerRelationWithOther : KUiPlayerItem
{
	int		nRelation;
	int		nParam;
};

struct KUiGameObjectWithName
{
	char			szName[32];
	char			szString[32];
	int				nData;
	int				nParam;
	unsigned int 	uParam;
};

#define		defMAX_ARRAY_AUTO				50
#define		defAUTO_TIME_OVERLOOK			100
#define		defAUTO_TIME_RESET_OVERLOOKL	10000
#define		defAUTO_USESKILL_MANA_LOW		30
#define		defMAX_ARRAY_STATESKILL			3
#define		defMAX_AUTO_SPACETIME			500
#define		defMAX_EAT_SPACETIME			1500
#define	    defMAX_CHECK_TSNAMETIME    3000
#define		defMAX_AUTO_SPACETIME_JB		15000
#define		defMAX_AUTO_MOVEMPSL			15
#define		defMAX_AUTO_FILTERL				40
#define		defMAX_BUY_POTION				5
#define		def_RETURN_FROM_PORTAL_STEP  5000

enum PickUpItem
{
	enumPickUpEarn,
	enumPickEventItem,
	enumPickItemColor,
	enumPickUpAll,
	enumPickNum,
};

enum Move_Mode
{
	Move_Mode_Free,
	Move_Mode_Area,
	Move_Mode_Mps,
};

#define	MAX_SUPERSHOP_SHOPTAB 10
typedef struct BuySellInfoSTRUCT
{
	int		m_nCurShop;
	int		m_nShopIdx[MAX_SUPERSHOP_SHOPTAB];
	int		m_nShopNum;
	int		m_nMoneyUnit;
	DWORD	m_SubWorldID;
	int		m_nMpsX;
	int		m_nMpsY;
	void	Clear() {
		memset(m_nShopIdx, -1, sizeof(m_nShopIdx)); 
		m_nMoneyUnit = moneyunit_money; m_SubWorldID = -1; m_nMpsX = 0; m_nMpsY = 0;
	}
} BuySellInfo;

//Begin Fkauto
enum SELECT_FIGHT_OPTION
{
	F_Auto = 0,
	F_IsRide,
	F_IsNotRide,
};
//End Fkauto

#define MAX_ITEM_SPC								5		//max item tr猲 gi?h祅g
struct FKGioHang
{
	KUiObjAtContRegion	m_ListItemInfo[MAX_ITEM_SPC];
	KUiItemBuySelInfo	m_ListPriceInfo[MAX_ITEM_SPC];
	int								m_ListCount[MAX_ITEM_SPC];
	int								m_ListShopId[MAX_ITEM_SPC];
	void Clear () {
		memset(m_ListItemInfo, 0, sizeof(m_ListItemInfo));
		memset(m_ListPriceInfo, 0, sizeof(m_ListPriceInfo));
		memset(m_ListCount, 0, sizeof(m_ListCount));
		memset(m_ListShopId, 0, sizeof(m_ListShopId));
	};
};

//--------------------------end ----------------------
#endif
