/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2020-9-12
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#include "CoreObjGenreDef.h"
#include "CoreUseNameDef.h"
#include <iostream>
#include <map>
#include <vector>

#define		ITEM_VERSION						1
#define		Def_MAX_STACK_TIENDONG	500
#define		Def_ID_XU_TIENDONG	417
#define		Def_MAXLEN_STRING_CHAT		90
#define		_CHAT_SCRIPT_OPEN
#define		OBJ_NAME_LENGHT						64
#define		MAX_TEAM_MEMBER						7		
#define		MAX_SENTENCE_LENGTH					256
#define		NUM_INFO_ITEM_CHAT					39				//26 th´ng tin Æ≠Óc slipt trong string gˆi Æi
#define		FILE_NAME_LENGTH					80
#define		PLAYER_PICKUP_CLIENT_DISTANCE		75//63
#define		defMAX_EXEC_OBJ_SCRIPT_DISTANCE		200
#define		defMAX_PLAYER_SEND_MOVE_FRAME		5
#define		PLAYER_PICKUP_SERVER_DISTANCE		40000
#define		MAX_INT								0x7fffffff
#define		MONEY_FLOOR							10000
#define		STAMINA_RECOVER_SCALE	4
#define 	MIN_ITEM_LEVEL				0
#define 	MAX_ITEM_LEVEL				10
#define 	MAX_ITEM_LEVELFF				20
#define 	MAX_ITEM_GENERATORLEVEL		10
#define 	MAX_ITEM_LUCK				10
#define 	MAX_ITEM_MAGICATTRIB		8
#define 	MAX_ITEM_MAGICLEVEL			MAX_ITEM_MAGICATTRIB * 2
#define 	MAX_ITEM_NORMAL_MAGICATTRIB	6
#define		defNPC_GOLD_TYE		16
#define		defNPC_GOLD_SKILL_NO		5
#define		defNPC_GOLD_FIST_MAGIC_DAMGE		20
#define		POISON_DAMAGE_TIME				60
#define		POISON_DAMAGE_INTERVAL			10
#define		COLD_DAMAGE_TIME				60
#define		MAX_DEATLY_STRIKE_ENHANCEP		100
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
#define		MAX_DEATH_PUNISH_PK_VALUE			10		// PK 0 Æ’n 10
#define		MAX_REPUTE_VALUE					100000	
#define		MAX_FUYUAN_VALUE					100000	
#define		MAX_REBORN_VALUE					100000
#define		MSG_NON_SETTINGS		"Kh´ng Thi’t LÀp"
#define		TIMERTASK_SETTINGFILE	"\\settings\\TimerTask.txt"
#define		TASKVALUE_X2_EXP		36	//x2 Æi”m kinh nghi÷m c∏ nh©n ch’t kh´ng m t
#define		TASKVALUE_STATTASK_HONOR		37	//lien dau
#define		TASKVALUE_STATTASK_ACCUM		38 //tong kim
#define		TASKVALUE_STATTASK_RESPECT		39	//diem uy danh t›ch lu¸ c´ng trπng
#define		TASKVALUE_STATTASK_REPUTE		73	//danh vong
#define		TASKVALUE_STATTASK_FUYUAN		74	//phuc duyen khong dung
#define		TASKVALUE_STATTASK_MATDOTHANBI		75//mat do than bi, son ha xa tac
#define		TASKVALUE_DB_PLAYER_TITLE_ID 71 //#PlayerTitle
#define		TASKVALUE_DB_PLAYER_TITLE_TIME	72 //#PlayerTitle
#define		TASKVALUE_DANHQUAIDATAU 89
#define		T_ANTI_HACK1 108 //add by phong ki“u antihack	//Æ„ng ch≠a s?dÙng
#define		T_ANTI_HACK2 109	//Æ„ng ch≠a s?dÙng

#define		TASKVALUE_SAVEWAYPOINT_BEGIN 101
#define		TASKVALUE_MAXWAYPOINT_COUNT 3
#define		TASKVALUE_SAVESTATION_BEGIN  110 //task 110 Æ’n 143
#define		TASKVALUE_MAXSTATION_COUNT 32
#define		TASKVALUE_TIMERTASK_TAST	220
#define		TASKVALUE_TIMERTASK_RESTTIME 221
#define		TASKVALUE_STATTASK_XU			251

#define MAX_MISSION_PARAM			18 //fix by phong ki“u m∆c Æﬁnh 16
#define MISSION_PARAM_AVAILABLE		0
#define MISSION_AVAILABLE_VALUE		1
#define MAX_GLBMISSION_PARAM		3
#define	MISSION_STATNUM				10 //add by phong ki“u using TËng kim battle 10 ng≠Íi top ten
// [KM 27/08] 8 -> 12 theo ban chuan (Linux + client VLTK).
// TRoleData KHONG doi sizeof: cap kinh mach luu trong BaseInfo.szStringduphong2[32], dung 12/32 byte.
#define MAX_MERIDIAN 12
#define MAX_MERIDIAN_LEVEL 32

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
	//ePickInFightState,//nh∆t ÆÂ trong thµnh
	eAutoRightSkill,//Æ∏nh chi™u b™n ph∂i
	eAutoPTAll, //pt t t c∂
	enumcount,
};

enum ITEM_PART
{
	itempart_head = 0,	// Õ∑ // mÚ
	itempart_body,		// …ÌÃÂ //∏o
	itempart_belt,		// —¸¥¯ // Æai l≠ng
	itempart_weapon,	// Œ‰∆˜ // vÚ kh›
	itempart_foot,	//giµy
	itempart_cuff, //bao tay
	itempart_amulet, //d©y chuy“n
	itempart_ring1,	//nh…n
	itempart_ring2,
	itempart_pendant,  //ng‰c bÈi
	itempart_horse, // ng˘a
	itempart_mask,	// mat na
	itempart_mantle,//#phi phong
	itempart_signet,
	itempart_shipin,
	itempart_hoods,
	itempart_cloak,
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
	compoundpart_box4,
	compoundpart_box5,
	compoundpart_box6,
	compoundpart_box7,
	compoundpart_box8,
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
	BOOL	bIsSkill;
	int		nIdx;
	int		nPlace;
	int		nX;
	int		nY;
	int		nPrice;
	void	Release() {
		bIsSkill = FALSE;
		nIdx = 0;
		nPlace = 0;
		nX = 0;
		nY = 0;
	};
} PlayerItem;

enum LOCK_STATE
{
	LOCK_STATE_CHARACTER = -3,
	LOCK_STATE_FOREVER = -2,
	LOCK_STATE_LOCK,
	LOCK_STATE_NORMAL,
	LOCK_STATE_UNLOCK,
};

enum INVENTORY_ROOM
{
	room_equipment = 0,	// trang bﬁ tr™n ng≠Íi
	room_equipmentback, // backup to switch equip
	room_repository,	// r≠¨ng ch¯a ÆÂ
	room_exbox1,		// mo rong ruong 1
	room_exbox2,		// mo rong ruong 2
	room_exbox3,		// mo rong ruong 3
	room_equipmentex,	// mo rong hanh trang
	room_trade,			// giao dﬁch
	room_tradeback,		// 
	room_trade1,		// 
	room_immediacy,		// ph›m tæt
	room_give,				//10
	room_giveback,		//11
	room_affairitem,	//12
	room_partnerbag,	//13 [BDH-G3] tui ban dong hanh (Linux room_partnerbag=14)
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
	pos_gambleroom,		// OTT
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
	pos_equipback,		// equip 2
	pos_partnerbag,	// =25 [BDH-G3] tui ban dong hanh (them CUOI - giu gia tri cu)
	pos_atlas,		// [LOREN 27/08] PHONG DO PHO - phong the Do pho Hoang Kim
				// (them CUOI nen KHONG xe dich gia tri cu)
	pos_num,
};

// add combat info system
enum COMBAT_INFO_TYPE
{
	DAMAGE_MISS,
	DAMAGE_NORMAL,
	DAMAGE_DEADSTRIKE,		// CHI MANG
	DAMAGE_CURE,			// PHUC HOI
	DAMAGE_POISON,			// DOC
	COMBAT_INFO_DAMAGE_LIFE,
	COMBAT_INFO_HEAL_LIFE,
	COMBAT_INFO_DAMAGE_MANA,
	COMBAT_INFO_HEAL_MANA,
	COMBAT_INFO_DODGE,

	COMBAT_INFO_ABSORB_LIFE,	// HUT SINH LUC
	COMBAT_INFO_ABSORB_MANA,	// HUT NOI LUC
	COMBAT_INFO_ABSORB_STAMINA,	// HUT THE LUC

	COMBAT_INFO_SCORE_GET,      // 
};
// add end

// [BDH-G3] tui ban dong hanh 6x10 (partner_bag.ini Level_10=6,10; o mo theo cap tui)
#define		PARTNERBAG_ROOM_WIDTH		6
#define		PARTNERBAG_ROOM_HEIGHT		10
#define		AFFAIRITEM_ROOM_WIDTH		14	// [PF13 31/08] cu 6; panel kham 13 lo gui Region.h toi 13,
													// khoang 6xN se tu choi PlaceItem o 6..13. Chi la SUC CHUA trong
													// bo nho (nuoi MAX_PLAYER_ITEM), khong nam trong goi tin hay DB.
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
//----->Add by OTT
#define		GAMBLE_ROOM_WIDTH			10
#define		GAMBLE_ROOM_HEIGHT			4
//<-----Add End
#define		MAX_TRADE_ITEM				(TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
#define		MAX_TRADE1_ITEM				MAX_TRADE_ITEM
#define		MAX_GAMBLE_ITEM				(GAMBLE_ROOM_WIDTH * GAMBLE_ROOM_HEIGHT)
#define		IMMEDIACY_ROOM_WIDTH		9
#define		IMMEDIACY_ROOM_HEIGHT		1
#define		MAX_IMMEDIACY_ITEM			(IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
#define		MAX_PLAYER_ITEM_RESERVED	32
//#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED)
#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM*5 + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_AFFAIR_ITEM + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED)
//#define		MAX_ITEM_MAGICLEVEL			8
#define		MAX_NPC_LEVEL						120
#define		MAX_NPC_SERIES						  6 
#define		TIME_RIDE 5000 //edit by phong kieu thoi gian len xuong ngua
#define		TIME_PK	  180000	//edit by phong kieu thoi gian chuyen PK
#define		MAX_PERCENT							100
#define		REMOTE_REVIVE_TYPE			0
#define		LOCAL_REVIVE_TYPE			1
#define		MAX_MELEE_WEAPON			6
#define		MAX_MELEE_WEAPON_VHTD		9	// [VHTD 02/09] o vu khi can chien 0..8: 6 Trien Thu, 7 Dao Thuan, 8 Thuan Dao (VLTK)
#define		MAX_HS_SP					4	// [VHTD 02/09] so bo dem tang (No/Am Luat) moi NPC
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
#define		MAX_NPC_RECORDER_STATE 18 //fix by Fong Ki“u m∆c Æﬁnh lµ 8
#define		PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE	5
#define	NORMAL_NPC_PART_NO		5	
#ifndef _SERVER
#define		C_REGION_X(x)	(LOWORD(SubWorld[0].m_Region[ (x) ].m_RegionID))
#define		C_REGION_Y(y)	(HIWORD(SubWorld[0].m_Region[ (y) ].m_RegionID))
#endif

enum
{
	CHAT_S_STOP = 0,						// ∑«¡ƒÃÏ◊¥Ã¨
	CHAT_S_SCREEN,							// ”ÎÕ¨∆¡ƒªÕÊº“¡ƒÃÏ
	CHAT_S_SINGLE,							// ”ÎÕ¨∑˛ŒÒ∆˜ƒ≥ÕÊº“ÀΩ¡ƒ
	CHAT_S_TEAM,							// ”Î∂”ŒÈ»´ÃÂ≥…‘±ΩªÃ∏
	CHAT_S_NUM,								// ¡ƒÃÏ◊¥Ã¨÷–¿‡ ˝
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
	CHAT_S_ONLINE = 0,		//‘⁄œﬂ
	CHAT_S_BUSY,			//√¶¬µ
	CHAT_S_HIDE,			//“˛…Ì
	CHAT_S_LEAVE,			//¿Îø™
	CHAT_S_DISCONNECT,		//µÙœﬂ
};

// ◊¢“‚£∫¥À√∂æŸ≤ª‘ –Ì∏¸∏ƒ(by zroc)
enum OBJ_ATTRIBYTE_TYPE
{
	series_metal,			//	Ωœµ
	series_wood,			//	ƒæœµ
	series_water,			//	ÀÆœµ
	series_fire,			//	ªœµ
	series_earth,			//	Õ¡œµ
	series_nil,
	series_num = series_nil,
	// [HOASON 01/09] so mon phai TOI DA (13 = 10 goc + Hoa Son 10 + Vu Hon 11 + Tieu Dao 12);
	// truoc day MAX_FACTION = 2 phai x 5 he (KFaction.h). Dung o KProtocol.h TGAME_STAT_DATA,
	// KLadder.h, KNpc.cpp - CORE va GODDESS (DBBackup.h) phai cung con so nay.
	MAX_FACTION_NUM = 13,

	series_minus,
};

enum OBJ_GENDER
{
	OBJ_G_MALE	= 0,	//–€–‘£¨ƒ–µƒ
	OBJ_G_FEMALE,		//¥∆µƒ£¨≈Æµƒ
};

enum NPCCAMP
{
	camp_begin,	// 0 ch˜ træng
	camp_justice, // 1 ch∏nh ph∏i
	camp_evil, // 2 tµ ph∏i
	camp_balance, // 3 trung lÀp
	camp_free,	// 4 mµu Æ· s∏t thÒ
	camp_animal, // 5 mµu hÂng admin
	camp_event,		// 6 mµu hÂng admin
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
	IIEP_NORMAL = 0,	//“ª∞„/’˝≥£/ø…”√
	IIEP_NOT_USEABLE,	//≤ªø…”√/≤ªø…◊∞≈‰
	IIEP_SPECIAL,		//Ãÿ∂®µƒ≤ªÕ¨«Èøˆ
};

#define	GOD_MAX_OBJ_TITLE_LEN	4096	// [PF13 31/08b] cu 2048: GetDesc ghi TOAN BO tooltip noi tiep tu szTitle, 13 dong da + khoi an vuot 2048	//128¡Ÿ ±∏ƒŒ™1024Œ™¡ÀºÊ»›æ…¥˙¬Î to be modified
#define	GOD_MAX_OBJ_PROP_LEN	2560	// [PF13 31/08] cu 1024: 13 dong da + 13 dong thuoc tinh + khoi an vuot 1024 (strcat khong chan). Chi la dem UI client, khong nam trong goi tin.
#define	GOD_MAX_OBJ_DESC_LEN	1024

//==================================
//	”Œœ∑∂‘œÛµƒ√Ë ˆ
//==================================
struct KGameObjDesc
{
	char	szTitle[GOD_MAX_OBJ_TITLE_LEN];	//±ÍÃ‚£¨√˚≥∆
	char	szProp[GOD_MAX_OBJ_PROP_LEN];	// Ù–‘£¨√ø––ø…“‘tabªÆ∑÷Œ™øø◊Û”Îøø”“∂‘∆Î¡Ω≤ø∑÷
	char	szDesc[GOD_MAX_OBJ_DESC_LEN];	//√Ë ˆ
};

//==================================
//	Œ Ã‚”Îø…—°¥∞∏
//==================================

struct KUiAnswer
{
	char	AnswerText[256];	//ø…—°¥∞∏Œƒ◊÷£®ø…“‘∞¸∫¨øÿ÷∆∑˚£©
	int		AnswerLen;			//ø…—°¥∞∏¥Ê¥¢≥§∂»£®∞¸¿®øÿ÷∆∑˚£¨≤ª∞¸∫¨Ω· ¯∑˚£©
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

#pragma pack(push, 1)  // fix loi post item sai opt

typedef struct
{
	int		m_nID;				// ŒÔ∆∑µƒID
	BYTE	m_btGenre;			// ŒÔ∆∑µƒ¿‡–Õ
	short		m_btDetail;			// ŒÔ∆∑µƒ¿‡±
	short		m_btParticur;		// ŒÔ∆∑µƒœÍœ∏¿‡±
	BYTE	m_btSeries;			// ŒÔ∆∑µƒŒÂ––
	BYTE	m_btLevel;			// ŒÔ∆∑µƒµ»º∂
	BYTE	m_btLuck;			// MF
	short		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// …˙≥…≤Œ ˝
	WORD	m_wVersion;			// ◊∞±∏∞Ê±æ
	DWORD	m_dwRandomSeed;		// ÀÊª˙÷÷◊”
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
	int		m_nNature; //goldequip
	int		m_nMaxOptMultiply;
} ChatItem;

struct KOneMsgInfo
{
	int			 nLines;				//’‚Ãı–≈œ¢’º¡À∂‡…Ÿ––
	int			 nCharWidth;			//’‚Ãı–≈œ¢’º¡À◊÷∑˚øÌ
	ChatItem	 sItem;					//thong tin cua item
	int			 nHaveItem;				//co item khong?
	int			 nPos;					// vi tri trong cau
	int			 nFace;					// so icon trong 1 dong
	int			 nLinePos;				// item nam dong thu...
	unsigned int uTextBKColor;			//’‚Ãı–≈œ¢◊÷œ‘ æ ±µƒŒƒ◊÷≥ƒµ◊µƒ—’…´,0Œ™Œﬁ
	int			 nMaxSize;				//Msgø…“‘»›ƒ…µƒ◊Ó¥Û–≈œ¢≥§∂»
	int			 nLen;					//–≈œ¢≥§∂»
	char		 Msg[1];				//–≈œ¢µƒƒ⁄»›
};
#pragma pack(pop) // fix loi post item sai opt

enum ITEMKIND
{
	normal_item = 0,	//ÆÂ træng
	green_item,				//ÆÂ xanh
	broken_item,			//ÆÂ bﬁ h·ng
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
	// [PHI PHONG 2026-08-29] noi them 3 loai cho KHOP BAN LINUX
	// (Linux: 7 brokenequip, 8 fusion, 9 starstone -- da doi chieu bang du lieu).
	// An toan: item_number chi dung 2 cho trong toan nguon, va mang
	// szColor[item_number][32] (KItem.cpp) von chi co 5 dong khoi tao tren 7.
	item_brokenequip,	// 7
	item_fusion,		// 8
	item_starstone,		// 9 -- Tinh Than Thach kham phi phong
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
	equip_damage,
	equip_violet,
	equip_gold,
	equip_platina,
	equip_number,
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
	equip_signet, //13
	equip_shipin, //14
	equip_hoods, //15
	equip_cloak, //16
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
	unsigned int uGenre;	//∂‘œÛ¿‡ Ù
	unsigned int uId;		//∂‘œÛid
//	int			 nData;		//”Î∂‘œÛ µ¿˝œ‡πÿµƒƒ≥ ˝æ›
};

//==================================
//	“‘◊¯±Í±Ì æµƒ“ª∏ˆ«¯”Ú∑∂Œß
//==================================
struct KUiRegion
{
	int		h;		//◊Û…œΩ«∆µ„∫·◊¯±Í
	int		v;		//◊Û…œΩ«∆µ„◊›◊¯±Í
	int		Width;	//«¯”Ú∫·øÌ
	int		Height;	//«¯”Ú◊›øÌ
};

enum UIOBJECT_CONTAINER
{
	UOC_IN_HAND = 1,		// ÷÷–ƒA◊≈
	UOC_GAMESPACE,			//”Œœ∑¥∞ø⁄
	UOC_IMMEDIA_ITEM,		//ø¥ΩUŒÔ∆∑
	UOC_IMMEDIA_SKILL,		//ø¥ΩUŒ‰π¶0->”?º¸Œ‰π¶£¨1,2...-> F1,F2...ø¥ΩUŒ‰π¶
	UOC_ITEM_TAKE_WITH,		//ÀÊ…Ì–Ø¥¯
	UOC_SKILL_TAKE_WITH,
	UOC_TO_BE_TRADE,		//?™±ª¬?¬Ù£¨¬?¬ÙAÊ∞Â…œ
	UOC_OTHER_TO_BE_TRADE,	//¬?¬ÙAÊ∞Â…œ£¨±d»À?™¬Ù∏¯◊‘º∫µƒ£¨
	UOC_TO_BE_GAMBLE,		//
	UOC_OTHER_TO_BE_GAMBLE,	//
	UOC_EQUIPTMENT,			//…Ì…œ◊∞±∏
	UOC_NPC_SHOP,			//npc¬?¬Ù≥°À˘
	UOC_MARKET,
	UOC_STORE_BOX,			//¥¢ŒÔœ‰
	UOC_EX_BOX1,
	UOC_EX_BOX2,
	UOC_EX_BOX3,
	UOC_ITEM_EX,
	UOC_SKILL_LIST,			//¡–≥ˆ»´≤ø”µ”–ººƒ‹µƒ¥∞ø⁄£¨ººƒ‹¥∞ø⁄
	UOC_SKILL_TREE,			//◊Û°¢”?ø…”Aººƒ‹ ˜
	UOC_ITEM_GIVE,
	UOC_AFFAIR_ITEM,		// pos tra vat pham nhiem vu
	UOC_TREMBLE_ITEM,		//Kham nam/nang cap trang bi xanh
	UOC_COMPONE_ITEM,		//Luyen huyen tinh khoang thach
	UOC_COMPTWO_ITEM,		//Nang cap huyen tinh khoang thach
	UOC_COMPTHREE_ITEM,		//Nang cap khoang thach thuoc tinh
	UOC_DISTILL_ITEM,		//Rut option trang bi
	UOC_FORGE_ITEM,			//Che tao trang bi tim
	UOC_ENCHASE_ITEM,		//Kham nam trang bi
	UOC_GAMBLE_SELF,		//On the mutual gambling panel, you want to use it for gambling
	UOC_GAMBLE_OTHER,		//On the mutual gambling panel, others use it for gambling
	UOC_PARTNER_BAG,	// [BDH-G3] tui ban dong hanh		//On the mutual gambling panel, others use it for gambling
	UOC_ATLAS_ITEM,		// [LOREN 27/08] PHONG DO PHO - o cua the Do pho Hoang Kim
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
//----->Add by OTT
enum UI_GAMBLE_OPER_DATA
{
	UGOD_IS_WILLING,
	UGOD_IS_LOCKED, //Is it locked?
	//Return: Returns the Boolean value of whether it is locked
	UGOD_IS_GAMBLING, //Can it be waiting for gambling operation (whether gambling has been confirmed)
	//Return: Returns whether it is waiting for trading operation (whether gambling has been confirmed)
	UGOD_IS_OTHER_LOCKED, //Is the other party already locked?
	//Return: Returns the Boolean value of whether the other party is already locked
};
//<-----Add End

struct PLAYERTRADE
{
	char cName[32];
	BOOL nTrade;
	int nDest;
	void Release() {
		memset(cName, 0, sizeof(cName));
		nTrade = FALSE;
		nDest = 0;
	};
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
	char			szItemName[64];	//ŒÔ∆∑√˚≥∆
	int				nOldPrice;
	int				nCurPrice;			//¬Ú¬Ùº€«Æ£¨’˝÷µŒ™¬Ùº€∏Ò£¨∏∫÷µ±Ì æ¬Ú»Îµƒº€∏ÒŒ™(-nPrice)
	int				nMoneyUnit;
	BOOL			bNewArrival;
	int				nPrice;			//¬Ú¬Ùº€«Æ£¨’˝÷µŒ™¬Ùº€∏Ò£¨∏∫÷µ±Ì æ¬Ú»Îµƒº€∏ÒŒ™(-nPrice)
	int				nPriceXu;
};

//==================================
//	±Ì æƒ≥∏ˆ”Œœ∑∂‘œÛ‘⁄◊¯±Í«¯”Ú∑∂Œßµƒ–≈œ¢Ω·ππ
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
	unsigned char	eGenre;	//»°÷µ∑∂ŒßŒ™√∂æŸ¿‡–ÕMSG_GENRE_LIST,º˚MsgGenreDef.hŒƒº˛
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
	PA_NONE = 0,	//Œﬁ∂Ø◊˜
	PA_RUN  = 0x01,	//≈‹
	PA_SIT  = 0x02,	//¥Ú◊¯
	PA_RIDE = 0x04,	//∆Ô£®¬Ì£©
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
//	œµÕ≥œ˚œ¢∑÷¿‡
//==================================
enum SYS_MESSAGE_TYPE
{
	SMT_NORMAL = 0,	//≤ª≤Œº”∑÷¿‡µƒœ˚œ¢
	SMT_SYSTEM,		//œµÕ≥£¨¡¨Ω”œ‡πÿ
	SMT_PLAYER,		//ÕÊº“œ‡πÿ
	SMT_TEAM,		//◊È∂”œ‡πÿ
	SMT_FRIEND,		//¡ƒÃÏ∫√”—œ‡πÿ
	SMT_MISSION,	//»ŒŒÒœ‡πÿ
	SMT_CLIQUE,		//∞Ô≈…œ‡πÿ
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
	SMCT_UI_GAMBLE,			//Agree or reject the gambling request,
};

struct KSystemMessage
{
	char			szMessage[128];	//œ˚œ¢Œƒ±æ
	unsigned int	uReservedForUi;	//ΩÁ√Ê π”√µƒ ˝æ›”Ú,core¿ÔÃÓ0º¥ø…
	unsigned char	eType;			//œ˚œ¢∑÷¿‡»°÷µ¿¥◊‘√∂æŸ¿‡–Õ SYS_MESSAGE_TYPE
	unsigned char	byConfirmType;	//œÏ”¶¿‡–Õ
	unsigned char	byPriority;		//”≈œ»º∂, ˝÷µ‘Ω¥Û£¨±Ì æ”≈œ»º∂‘Ω∏ﬂ
	unsigned char	byParamSize;	//∞ÈÀÊGDCNI_SYSTEM_MESSAGEœ˚œ¢µƒpParamBufÀ˘÷∏≤Œ ˝ª∫≥Â«¯ø’º‰µƒ¥Û–°°£
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
	char	szTitle[32];	//∑÷◊Èµƒ√˚≥∆
	int		nNumFriend;		//◊Èƒ⁄∫√”—µƒ ˝ƒø
};

struct KUiChatMessage
{
	unsigned int uColor;
	short	nContentLen;
	char	szContent[256];
};

struct KUiPlayerBaseInfo
{
	char	Agname[32];	//¥¬∫≈
	char	Name[32];	//√˚◊÷
	char	Title[32];	//≥∆∫≈
	char	szMateName[32]; //#MateName
	int		nCurFaction;// µ±«∞º”»Î√≈≈… id £¨»Áπ˚Œ™ -1 £¨µ±«∞√ª”–‘⁄√≈≈…÷–
	int		nRankInWorld;//Ω≠∫˛≈≈√˚÷µ,÷µŒ™0±Ì æŒ¥…œ≈≈√˚∞Â
	unsigned int nCurTong;// µ±«∞º”»Î∞Ô≈…name id £¨»Áπ˚Œ™ 0 £¨µ±«∞√ª”–‘⁄∞Ô≈…÷–
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int		nPKValue; // pk
	int		nMissionGroup;
	int		nFirstAddFaction;
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
	int		nLifeFull;			//…˙√¸¬˙÷µ
	int		nLife;				//…˙√¸
	int		nManaFull;			//ƒ⁄¡¶¬˙÷µ
	int		nMana;				//ƒ⁄¡¶
	int		nStaminaFull;		//ÃÂ¡¶¬˙÷µ
	int		nStamina;			//ÃÂ¡¶
	int		nAngryFull;			//≈≠¬˙÷µ
	int		nAngry;				//≈≠
	double		nExperienceFull;	//æ≠—È¬˙÷µ
	double		nExperience;		//µ±«∞æ≠—È÷µ
	double		nCurLevelExperience;//µ±«∞º∂±…˝º∂–Ë“™µƒæ≠—È÷µ

	unsigned char	byActionDisable;// «∑Ò≤ªø…Ω¯––∏˜÷÷∂Ø◊˜£¨Œ™√∂æŸPLAYER_ACTION_LIST»°÷µµƒ◊È∫œ
	unsigned char	byAction;	//’˝‘⁄Ω¯––µƒ––Œ™∂Ø◊˜£¨Œ™√∂æŸPLAYER_ACTION_LIST»°÷µµƒ◊È∫œ
	unsigned short	wReserved;	//±£¡Ù
};

enum UI_PLAYER_ATTRIBUTE
{
	UIPA_STRENGTH = 0,			//¡¶¡ø
	UIPA_DEXTERITY,				//√ÙΩ›
	UIPA_VITALITY,				//ªÓ¡¶
	UIPA_ENERGY,				//æ´¡¶
};

struct KUiPlayerAttribute
{
	int		nMoney;				//“¯¡Ω
	int		nXu;
	int		nLevel;				//µ»º∂
	char	StatusDesc[16];		//◊¥Ã¨√Ë ˆ

	int		nBARemainPoint;		//ª˘±æ Ù–‘ £”‡µ„ ˝
	int		nStrength;			//¡¶¡ø
	int		nDexterity;			//√ÙΩ›
	int		nVitality;			//ªÓ¡¶
	int		nEnergy;			//æ´¡¶

	int		nKillMAX;			//◊Ó¥Û…±…À¡¶
	int		nKillMIN;			//◊Ó–°…±…À¡¶
	int		nRightKillMax;		//”“º¸◊Ó¥Û…±…À¡¶
	int		nRightKillMin;		//”“º¸◊Ó–°…±…À¡¶

	int		nAttack;			//π•ª˜¡¶
	int		nDefence;			//∑¿”˘¡¶
	int		nMoveSpeed;			//“∆∂ØÀŸ∂»
	int		nAttackSpeed;		//π•ª˜ÀŸ∂»
	int		nCastSpeed;		//π•ª˜ÀŸ∂»

	int		nPhyDef;			//ŒÔ¿Ì∑¿”˘
	int		nCoolDef;			//±˘∂≥∑¿”˘
	int		nLightDef;			//…¡µÁ∑¿”˘
	int		nFireDef;			//ª—Ê∑¿”˘
	int		nPoisonDef;			//∂æÀÿ∑¿”˘

	int		nPhyDefPlus;			//ŒÔ¿Ì∑¿”˘
	int		nCoolDefPlus;			//±˘∂≥∑¿”˘
	int		nLightDefPlus;			//…¡µÁ∑¿”˘
	int		nFireDefPlus;			//ª—Ê∑¿”˘
	int		nPoisonDefPlus;			//∂æÀÿ∑¿”˘

	int		nRankInWorld;	// xep hang
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int		nPKValue; // phuc duyen
	BYTE	bMeridianLevel[MAX_MERIDIAN];
};

#define		RESIST_PLUS_SCALE		15		//Kh∏ng t®ng th™m 75+

#define	UPB_IMMEDIA_ITEM_COUNT	9

//==================================
//	÷˜Ω«µƒ¡¢º¥ π”√ŒÔ∆∑”ÎŒ‰π¶
//==================================
struct KUiPlayerImmedItemSkill
{
	KUiGameObject	ImmediaItem[UPB_IMMEDIA_ITEM_COUNT];
	KUiGameObject	IMmediaSkill[2];
};

//==================================
//	÷˜Ω«◊∞±∏∞≤ªªµƒŒª÷√
//==================================
enum UI_EQUIPMENT_POSITION
{
	UIEP_HEAD = 0,		//Õ∑¥˜
	UIEP_HAND = 1,		// ÷≥÷
	UIEP_NECK = 2,		//≤±◊”
	UIEP_FINESSE = 3,	// ÷ÕÛ
	UIEP_BODY = 4,		//…Ì¥©
	UIEP_WAIST = 5,		//—¸≤ø
	UIEP_FINGER1 = 6,	// ÷÷∏º◊
	UIEP_FINGER2 = 7,	// ÷÷∏““
	UIEP_WAIST_DECOR= 8,//—¸◊π
	UIEP_FOOT = 9,		//Ω≈≤»
	UIEP_HORSE = 10,	//¬Ì∆•
	UIEP_MASK = 11,		// mat na
	UIEP_FIFONG = 12, //#phi phong
	UIEP_SIGNET,
	UIEP_SHIPIN,
	UIEP_HOODS,
	UIEP_CLOAK,
};

//==================================
//	÷˜Ω«µƒ…˙ªÓººƒ‹ ˝æ›
//==================================
struct KUiPlayerLiveSkillBase
{
	int		nRemainPoint;			// £”‡ººƒ‹µ„ ˝
	int		nLiveExperience;		//µ±«∞ººƒ‹æ≠—È÷µ
	int		nLiveExperienceFull;	//…˝µΩœ¬º∂–Ë“™µƒæ≠—È÷µ
};

//==================================
//	µ•œÓººƒ‹ ˝æ›
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
//	“ª∏ˆ∂”ŒÈ÷–◊Ó∂‡∞¸∫¨≥…‘±µƒ ˝ƒø
//==================================
#define	PLAYER_TEAM_MAX_MEMBER	500

//==================================
//	Õ≥Àßƒ‹¡¶œ‡πÿµƒ ˝æ›
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
//	◊È∂”–≈œ¢µƒ√Ë ˆ
//==================================
struct KUiTeamItem
{
	KUiPlayerItem	Leader;
};

//==================================
//	∂”ŒÈ–≈œ¢
//==================================
struct KUiPlayerTeam
{
	bool			bTeamLeader;			//ÕÊº“◊‘º∫ «∑Ò∂”≥§
	char			cNumMember;				//∂”‘± ˝ƒø
	char			cNumTojoin;				//”˚º”»Îµƒ»À‘±µƒ ˝ƒø
	bool			bOpened;				//∂”ŒÈ «∑Ò‘ –Ì∆‰À˚»Àº”»Î
	int				nTeamServerID;			//∂”ŒÈ‘⁄∑˛ŒÒ∆˜…œµƒid£¨”√”⁄±Í ∂∏√∂”ŒÈ£¨-1 Œ™ø’
	int				nCaptainPower;
};

struct KUiTargetDetailInfo
{
	char			sTargetName[32];
	int				nLifePercent;
	BYTE			Series;
};

//==================================
//	ƒß∑® Ù–‘
//==================================
#ifndef MAGICATTRIB
#define MAGICATTRIB
struct KMagicAttrib
{
	int				nAttribType;					// Ù–‘¿‡–Õ
	int				nValue[3];						// Ù–‘≤Œ ˝
//	int nMin; //luu gia tri min cua opt
//	int nMax;  //luu gia tri max cua opt
	//KMagicAttrib() { nValue[0] = nValue[1] = nValue[2] = nAttribType = nMin = nMax = 0; };
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};

struct KMagicAutoSkill
{
	int				nSkillId;
	int				nSkillLevel;
	int				nRate;						// Ù–‘≤Œ ˝
	DWORD			dwNextCastTime;
	int				nWaitCastTime;
	int				nType;						// [HOASON 01/09e] byte cao nValue[0] (Linux): 1 = nham ke danh/muc tieu, khac = nham minh
	KMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = nType = 0;};
};
#else
struct KMagicAttrib;
struct KMagicAutoSkill;
#endif

/* ’‚ «æ…µƒ¥˙¬Î£¨–¬µƒ“—æ≠∑≈‘⁄KNpcGold¿Ô√Ê¥¶¿Ì¡À
//==================================
//	NPCº”«ø
//==================================
struct KNpcEnchant
{
	int		nExp;					// æ≠—È
	int		nLife;					// …˙√¸
	int		nLifeReplenish;			// ªÿ—™
	int		nAttackRating;			// √¸÷–
	int		nDefense;				// ∑¿”˘
	int		nMinDamage;
	int		nMaxDamage;

	int		TreasureNumber;				// ◊∞±∏
	int		AuraSkill;					// π‚ª∑
	int		DamageEnhance;				// …À∫¶
	int		SpeedEnhance;				// ÀŸ∂»
	int		SelfResist;					// ◊‘…Ìøπ–‘
	int		ConquerResist;				// œ‡…˙øπ–‘
#ifndef _SERVER
	char	NameModify[32];				// ∏ƒ√˚
#endif
};

//==================================
//	NPCµ•œÓº”«ø
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
//	—°œÓ…Ë÷√œÓ
//==================================
enum OPTIONS_LIST
{
	OPTION_PERSPECTIVE,		//Õ∏ ”ƒ£ Ω  nParam = (int)(bool)bEnable  «∑Òø™∆Ù
	OPTION_DYNALIGHT,		//∂ØÃ¨π‚”∞	nParam = (int)(bool)bEnable  «∑Òø™∆Ù
	OPTION_MUSIC_VALUE,		//“Ù¿÷“Ù¡ø	nParam = “Ù¡ø¥Û–°£®»°÷µŒ™0µΩ-10000£©
	OPTION_SOUND_VALUE,		//“Ù–ß“Ù¡ø	nParam = “Ù¡ø¥Û–°£®»°÷µŒ™0µΩ-10000£©
	OPTION_BRIGHTNESS,		//¡¡∂»µ˜Ω⁄	nParam = ¡¡∂»¥Û–°£®»°÷µŒ™0µΩ-100£©
	OPTION_WEATHER,			//ÃÏ∆¯–ßπ˚ø™πÿ nParam = (int)(bool)bEnable  «∑Òø™∆Ù
	OPTION_QUALITY_GIAMPLAYER,//add by phong ki“u
	OPTION_QUALITY_MATNPC,
	OPTION_QUALITY_MATPLAYER,
	OPTION_QUALITY_GIAMSKILL,
	OPTION_QUALITY_MATSKILL,
};

//==================================
//	À˘¥¶µƒµÿ”Ú ±º‰ª∑æ≥–≈œ¢
//==================================
struct KUiSceneTimeInfo
{
	char	szSceneName[32];		//≥°æ∞√˚
	int		nSceneId;				//≥°æ∞id
	int		nScenePos0;				//≥°æ∞µ±«∞◊¯±Í£®∂´£©
	int		nScenePos1;				//≥°æ∞µ±«∞◊¯±Í£®ƒœ£©
	int		nGameSpaceTime;			//“‘∑÷÷”Œ™µ•Œª
};

struct KUiSceneTimeInfoOften : public KUiSceneTimeInfo
{
};

//==================================
//	π‚‘¥–≈œ¢
//==================================
//’˚ ˝±Ì æµƒ»˝Œ¨µ„◊¯±Í
struct KPosition3
{
	int nX;
	int nY;
	int nZ;
};

struct KLightInfo
{
	KPosition3 oPosition;			// π‚‘¥Œª÷√
	DWORD dwColor;					// π‚‘¥—’…´º∞¡¡∂»
	long  nRadius;					// ◊˜”√∞Îæ∂
};


//–°µÿÕºµƒœ‘ æƒ⁄»›œÓ
enum SCENE_PLACE_MAP_ELEM
{ 
	SCENE_PLACE_MAP_ELEM_NONE		= 0x00,		//Œﬁ∂´Œ˜
	SCENE_PLACE_MAP_ELEM_PIC		= 0x01,		//œ‘ æÀı¬‘Õº
	SCENE_PLACE_MAP_ELEM_CHARACTER	= 0x02,		//œ‘ æ»ÀŒÔ
	SCENE_PLACE_MAP_ELEM_PARTNER	= 0x04,		//œ‘ æÕ¨∂”ŒÈ»À
};

//≥°æ∞µƒµÿÕº–≈œ¢
struct KSceneMapInfo
{
	int	nScallH;		//’Ê µ≥°æ∞œ‡∂‘”⁄µÿÕºµƒ∫·œÚ∑≈¥Û±»¿˝
	int nScallV;		//’Ê µ≥°æ∞œ‡∂‘”⁄µÿÕºµƒ◊›œÚ∑≈¥Û±»¿˝
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
	kind_normal = 0	,		//qu∏i b◊nh th≠Íng
	kind_player,				//ng≠Íi ch¨i
	kind_partner,				//bπn ÆÂng hµnh
	kind_dialoger,				//ÆËi thoπi
	kind_bird,					//bay kh´ng th y sˆ dÙng
	kind_mouse,				//chuÈt kh´ng th y sˆ dÙng
    kind_num				//tËi Æa
};

enum	// ŒÔº˛¿‡–Õ
{
	Obj_Kind_MapObj = 0,		// µÿÕºŒÔº˛£¨÷˜“™”√”⁄µÿÕº∂Øª≠
	Obj_Kind_Body,				// npc µƒ ¨ÃÂ
	Obj_Kind_Box,				// ±¶œ‰
	Obj_Kind_Item,				// µÙ‘⁄µÿ…œµƒ◊∞±∏
	Obj_Kind_Money,				// µÙ‘⁄µÿ…œµƒ«Æ
	Obj_Kind_LoopSound,			// —≠ª∑“Ù–ß		//Loop
	Obj_Kind_RandSound,			// ÀÊª˙“Ù–ß
	Obj_Kind_Light,				// π‚‘¥£®3Dƒ£ Ω÷–∑¢π‚µƒ∂´Œ˜£©
	Obj_Kind_Door,				// √≈¿‡
	Obj_Kind_Trap,				// œ›⁄Â
	Obj_Kind_Prop,				// –°µ¿æﬂ£¨ø…÷ÿ…˙
	Obj_Kind_Task,				// Obj nhiem vu
	Obj_Kind_Obstacle,
	Obj_Kind_Num,				// ŒÔº˛µƒ÷÷¿‡ ˝
};

//÷˜Ω«…Ì∑›µÿŒªµ»“ª–©πÿº¸ Ù–‘œÓ
enum PLAYER_BRIEF_PROP
{
	PBP_LEVEL = 1,	//µ«º∂±‰ªØ	nParam±Ì æµ±«∞µ»º∂
	PBP_FACTION,	//√≈≈…		nParam±Ì æ√≈≈… Ù–‘£¨»Áπ˚nParamŒ™-1±Ì æ√ª”–√≈≈…
	PBP_CLIQUE,		//∞Ô≈…		nParamŒ™∑«0÷µ±Ì æ»Î¡À∞Ô≈…£¨0÷µ±Ì æÕ—¿Î¡À∞Ô≈…
};

//–¬Œ≈œ˚œ¢µƒ¿‡–Õ∂®“Â
enum NEWS_MESSAGE_TYPE
{
	NEWSMESSAGE_NORMAL,			//“ª∞„œ˚œ¢£¨œ‘ æ£®“ª¥Œ£©æÕœ˚œ¢œ˚Õˆ¡À
								//Œﬁ ±º‰≤Œ ˝
	NEWSMESSAGE_COUNTING,		//µπº∆£®√Î£© ˝œ˚œ¢£¨º∆ ˝µΩ0 ±£¨æÕœ˚œ¢æÕœ˚Õˆ¡À°£
								// ±º‰≤Œ ˝÷–µƒ ˝æ›Ω·ππ÷–Ωˆ√Î ˝æ›”––ß£¨µπº∆ ˝“‘√ÎŒ™µ•Œª°£
	NEWSMESSAGE_TIMEEND,		//∂® ±œ˚œ¢£¨∂® ±µΩ ±£¨œ˚œ¢æÕœ˚ÕÍ¡À£¨∑Ò‘Ú√ø∞Î∑÷÷”œ‘ æ“ª¥Œ°£
								// ±º‰≤Œ ˝±Ì æœ˚Õˆµƒ÷∏∂® ±º‰°£
	NEWSMESSAGE_NORMAL_1,		// moi them	
	
	NEWSMESSAGE_COUNTING_1,		//moi them	

	NEWSMESSAGE_TIMEEND_1,		//moi them

	NEWSMESSAGE_NORMAL_2,		// moi them	
};

#define MAX_MESSAGE_LENGTH 512

struct KNewsMessage
{
	int		nType;						//œ˚œ¢¿‡–Õ
	char	sMsg[MAX_MESSAGE_LENGTH];	//œ˚œ¢ƒ⁄»›
	int		nMsgLen;					//œ˚œ¢ƒ⁄»›¥Ê¥¢≥§∂»
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
	bool			bValueAppened;	//√ø“ªœÓ «∑Ò”–√ª”–∂ÓÕ‚ ˝æ›
	bool			bSortFlag;		//√ø“ªœÓ «∑Ò”–√ª”–…˝Ωµ±Íº«
	unsigned short	usIndexId;		//≈≈√˚œÓID ˝÷µ
};

#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
	char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];	// Œƒ◊÷ƒ⁄»›
	unsigned short		usMsgLen;				// Œƒ◊÷ƒ⁄»›µƒ≥§∂»
	short				cSortFlag;				// ∆Ï±Í÷µ£¨QOO_RANK_DATAµƒ ±∫Ú±Ì æ≥ˆ…˝Ωµ£¨∏∫÷µ±Ì æΩµ£¨’˝÷µ±Ì æ…˝£¨0÷µ±Ì æŒª÷√Œ¥±‰
	int					nValueAppend;			// ¥ÀœÓ∏Ω¥¯µƒ÷µ

};

struct KMissionRecord
{
	char			sContent[256];
	int				nContentLen;
	unsigned int	uValue;	
	//char			szContent[MAX_SCIRPTACTION_BUFFERNUM];
	//int				nRecordAct;
};

//---------------------------- ∞Ôª·œ‡πÿ ------------------------

#define		defTONG_MAX_DIRECTOR				7
#define		defTONG_MAX_MANAGER					56
#define		defTONG_ONE_PAGE_MAX_NUM			100
#define		defTONG_STR_LENGTH					32
#define		defTONG_NAME_MAX_LENGTH				12

enum TONG_MEMBER_FIGURE
{
	enumTONG_FIGURE_MEMBER,				// thµnh vi™n
	enumTONG_FIGURE_MANAGER,			// Æ≠Íng chÒ
	enumTONG_FIGURE_DIRECTOR,			// tr≠Îng l∑o
	enumTONG_FIGURE_MASTER,				// bang chÒ
	enumTONG_FIGURE_LEAGUE,				//lien minh
	enumTONG_FIGURE_NUM,
};

enum
{
	enumTONG_APPLY_INFO_ID_SELF,		// …Í«Î≤È—Ø◊‘…Ì–≈œ¢
	enumTONG_APPLY_INFO_ID_MASTER,		// …Í«Î≤È—Ø∞Ô÷˜–≈œ¢
	enumTONG_APPLY_INFO_ID_DIRECTOR,	// …Í«Î≤È—Ø≥§¿œ–≈œ¢
	enumTONG_APPLY_INFO_ID_MANAGER,		// …Í«Î≤È—Ø∂”≥§–≈œ¢
	enumTONG_APPLY_INFO_ID_MEMBER,		// …Í«Î≤È—Ø∞Ô÷⁄–≈œ¢(“ª≈˙∞Ô÷⁄)
	enumTONG_APPLY_INFO_ID_ONE,			// …Í«Î≤È—Øƒ≥∞Ôª·≥…‘±–≈œ¢(“ª∏ˆ∞Ô÷⁄)
	enumTONG_APPLY_INFO_ID_TONG_HEAD,	// …Í«Î≤È—Øƒ≥∞Ôª·–≈œ¢£¨”√”⁄…Í«Îº”»Î∞Ôª·
	enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
	enumTONG_CREATE_ERROR_ID1,	// Player[m_nPlayerIndex].m_nIndex <= 0
	enumTONG_CREATE_ERROR_ID2,	// Ωª“◊π˝≥Ã÷–
	enumTONG_CREATE_ERROR_ID3,	// ∞Ôª·√˚Œ Ã‚
	enumTONG_CREATE_ERROR_ID4,	// ∞Ôª·’Û”™Œ Ã‚
	enumTONG_CREATE_ERROR_ID5,	// “—æ≠ «∞Ôª·≥…‘±
	enumTONG_CREATE_ERROR_ID6,	// ◊‘º∫µƒ’Û”™Œ Ã‚
	enumTONG_CREATE_ERROR_ID7,	// µ»º∂Œ Ã‚	
	enumTONG_CREATE_ERROR_ID8,	// «ÆŒ Ã‚
	enumTONG_CREATE_ERROR_ID9,	// ◊È∂”≤ªƒ‹Ω®∞Ôª·
	enumTONG_CREATE_ERROR_ID10,	// ∞Ôª·ƒ£øÈ≥ˆ¥Ì
	enumTONG_CREATE_ERROR_ID11,	// √˚◊÷◊÷∑˚¥Æ≥ˆ¥Ì
	enumTONG_CREATE_ERROR_ID12,	// √˚◊÷◊÷∑˚¥Æπ˝≥§
	enumTONG_CREATE_ERROR_ID13,	// ∞Ôª·Õ¨√˚¥ÌŒÛ
	enumTONG_CREATE_ERROR_ID14,	// ∞Ôª·≤˙…˙ ß∞‹
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
	char  szMasterName[32];		//∞Ô÷˜√˚◊÷
	int   nFaction;				//∞Ôª·’Û”™
	int   nMoney;				//∞Ôª·◊ Ω
	int   nReserve[2];
	int	  nMemberCount;         //∞Ô÷⁄ ˝¡ø
	int   nManagerCount;        //∂”≥§ ˝¡ø
	int   nDirectorCount;       //≥§¿œ ˝¡ø
	int   nFigure;              //¥˝…æ≥˝
	//===========add by Fong Ki“u 12/12/2021==========
	int		nStatusGuide; //t◊nh trπng bang hÈi
	char  szWayEdit[32];			// ti™u ch› bang hÈi
	char  szNextTargetEdit[32];			// mÙc ti™u hi÷n thÍi bang hÈi
	int		nExpGuide; //Æi”m kinh nghi÷m bang hÈi
	int		nCityGuide; //thµnh thﬁ chi’m h˜u
	int		nTongLevel; //Æºng c p bang hÈi
	char  szLeagueTName[32];		//t™n bang hÈi li™n minh
};

enum TONG_ACTION_TYPE
{
	TONG_ACTION_DISMISS,       //Ãﬂ»À
	TONG_ACTION_ASSIGN,        //»Œ√¸
	TONG_ACTION_DEMISE,        //¥´Œª
	TONG_ACTION_LEAVE,         //¿Î∞Ô
	TONG_ACTION_RECRUIT,       //’–»À◊¥Ã¨
	TONG_ACTION_APPLY,         //…Í«Îº”»Î
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
	TONG_ACTION_SAVE,       //Ãﬂ»À
	TONG_ACTION_GET,        //»Œ√¸
	TONG_ACTION_SND,        //¥´Œª
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
#define		def_TASK_TIMEOUT  15000
#define		def_MAX_IDLETIME  180000

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

#define MAX_ITEM_SPC								5		//max item tr™n gi· hµng
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



struct KLockItem
{
	KLockItem() { Clear(); };
	~KLockItem() { Clear(); };
	short nState;
	DWORD dwLockTime;
	void Clear() {
		nState = LOCK_STATE_NORMAL;
		dwLockTime = 0;
	};
	BOOL IsLock() {
		return (nState != LOCK_STATE_NORMAL);
	};
};

struct SetMeridianData
{
	int WayProtected;
	int WayEnhanced;
	int Type;
	int Level;
};

struct BauCuaData
{
	int nActionType;
	int nBetType;		// 0: bau, 1: cua, 2: ca, 3: ga, 4: nai, 5: tom
	int nMoney;		// so tien
};


enum class DiceFace {
	DEER = 0,
	GOURD,
	ROOSTER,
	FISH,
	CRAB,
	SHRIMP,
	COUNT
};

enum baucua_actiontype
{
	BAUCUA_MAKE_HOST = 0,
	BAUCUA_NO_HOST,
	BAUCUA_DEPOSIT,
	BAUCUA_WITHDRAW,
	BAUCUA_BET,
	BAUCUA_CANCEL_BET,
	BAUCUA_GET_RESULT,
	BAUCUA_GET_INFO,
};

enum baucua_resulttype
{
	BAUCUA_RESULT_DEPOSIT = 0,	// deposit result	
	BAUCUA_RESULT_WITHDRAW,		// withdraw result
	BAUCUA_RESULT_BET,			// bet result
	BAUCUA_RESULT_CANCEL_BET,	// cancel bet result
	BAUCUA_RESULT_ROUND_RESULT,	// get result
	BAUCUA_RESULT_INFO,			// get info result
};



struct BauCuaStatus {
	int playerDeposit;
	std::string hostId;
	int hostDeposit;
	std::map<DiceFace, int> lastRoundBets; // last round total bets each dice face
	std::map<DiceFace, int> currentBets; // this round total bets each dice face
	std::map<DiceFace, int> playerCurrentBet; // this round player bets each dice face
	int roundId;
	std::string commitmentHash;
	int remainingSeconds;
	std::vector<DiceFace> lastDiceResult;
};

struct BauCuaStatusSend {
	int playerDeposit;
	char hostId[32];
	int hostDeposit;
	int lastRoundBets[6];
	int currentBets[6];
	int playerCurrentBet[6]; // this round player bets
	int lastDiceResult[3];
	int roundId;
	char commitmentHash[32];
	int remainingSeconds;
};

#endif
