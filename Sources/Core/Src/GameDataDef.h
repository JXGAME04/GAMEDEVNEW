/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
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
#define		NUM_INFO_ITEM_CHAT					26				//26 th«ng tin ®­îc slipt trong string göi ®i
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
#define		MAX_DEATH_PUNISH_PK_VALUE			10		// PK 0 ®Õn 10
#define		MAX_REPUTE_VALUE					100000	
#define		MAX_FUYUAN_VALUE					100000	
#define		MAX_REBORN_VALUE					100000
#define		MSG_NON_SETTINGS		"Kh«ng thiÕt lËp"
#define		TIMERTASK_SETTINGFILE	"\\settings\\TimerTask.txt"
#define		TASKVALUE_X2_EXP		36	//x2 ®iÓm kinh nghiÖm c¸ nh©n chÕt kh«ng mÊt
#define		TASKVALUE_STATTASK_HONOR		37	//lien dau
#define		TASKVALUE_STATTASK_ACCUM		38 //tong kim
#define		TASKVALUE_STATTASK_RESPECT		39	//diem uy danh tİch luü c«ng tr¹ng
#define		TASKVALUE_STATTASK_REPUTE		73	//danh vong
#define		TASKVALUE_STATTASK_FUYUAN		74	//phuc duyen khong dung
#define		TASKVALUE_STATTASK_MATDOTHANBI		75//mat do than bi, son ha xa tac
#define		TASKVALUE_DB_PLAYER_TITLE_ID 71 //#PlayerTitle
#define		TASKVALUE_DB_PLAYER_TITLE_TIME	72 //#PlayerTitle
#define		TASKVALUE_DANHQUAIDATAU 89
#define		T_ANTI_HACK1 108 //add by phong kiÒu antihack	//®ãng ch­a sö dông
#define		T_ANTI_HACK2 109	//®ãng ch­a sö dông

#define		TASKVALUE_SAVEWAYPOINT_BEGIN 101
#define		TASKVALUE_MAXWAYPOINT_COUNT 3
#define		TASKVALUE_SAVESTATION_BEGIN  110 //task 110 ®Õn 143
#define		TASKVALUE_MAXSTATION_COUNT 32
#define		TASKVALUE_TIMERTASK_TAST	220
#define		TASKVALUE_TIMERTASK_RESTTIME 221
#define		TASKVALUE_STATTASK_XU			251

#define MAX_MISSION_PARAM			18 //fix by phong kiÒu mÆc ®Şnh 16
#define MISSION_PARAM_AVAILABLE		0
#define MISSION_AVAILABLE_VALUE		1
#define MAX_GLBMISSION_PARAM		3
#define	MISSION_STATNUM				10 //add by phong kiÒu using Tèng kim battle 10 ng­êi top ten

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
	//ePickInFightState,//nhÆt ®å trong thµnh
	eAutoRightSkill,//®¸nh chiªu bªn ph¶i
	eAutoPTAll, //pt tÊt c¶
	enumcount,
};

enum ITEM_PART
{
	itempart_head = 0,	// Í· // mò
	itempart_body,		// ÉíÌå //¸o
	itempart_belt,		// Ñü´ø // ®ai l­ng
	itempart_weapon,	// ÎäÆ÷ // vò khİ
	itempart_foot,	//giµy
	itempart_cuff, //bao tay
	itempart_amulet, //d©y chuyÒn
	itempart_ring1,	//nhÉn
	itempart_ring2,
	itempart_pendant,  //ngäc béi
	itempart_horse, // ngùa
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
	room_equipment = 0,	// trang bŞ trªn ng­êi
	room_repository,	// r­¬ng chøa ®å
	room_exbox1,		// mo rong ruong 1
	room_exbox2,		// mo rong ruong 2
	room_exbox3,		// mo rong ruong 3
	room_equipmentex,	// mo rong hanh trang
	room_trade,			// giao dŞch
	room_tradeback,		// 
	room_trade1,		// 
	room_immediacy,		// phİm t¾t
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
#define		MAX_NPC_RECORDER_STATE 18 //fix by Fong KiÒu mÆc ®Şnh lµ 8
#define		PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE	5
#define	NORMAL_NPC_PART_NO		5	
#ifndef _SERVER
#define		C_REGION_X(x)	(LOWORD(SubWorld[0].m_Region[ (x) ].m_RegionID))
#define		C_REGION_Y(y)	(HIWORD(SubWorld[0].m_Region[ (y) ].m_RegionID))
#endif

enum
{
	CHAT_S_STOP = 0,						// ·ÇÁÄÌì×´Ì¬
	CHAT_S_SCREEN,							// ÓëÍ¬ÆÁÄ»Íæ¼ÒÁÄÌì
	CHAT_S_SINGLE,							// ÓëÍ¬·şÎñÆ÷Ä³Íæ¼ÒË½ÁÄ
	CHAT_S_TEAM,							// Óë¶ÓÎéÈ«Ìå³ÉÔ±½»Ì¸
	CHAT_S_NUM,								// ÁÄÌì×´Ì¬ÖĞÀàÊı
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
	CHAT_S_ONLINE = 0,		//ÔÚÏß
	CHAT_S_BUSY,			//Ã¦Âµ
	CHAT_S_HIDE,			//ÒşÉí
	CHAT_S_LEAVE,			//Àë¿ª
	CHAT_S_DISCONNECT,		//µôÏß
};

// ×¢Òâ£º´ËÃ¶¾Ù²»ÔÊĞí¸ü¸Ä(by zroc)
enum OBJ_ATTRIBYTE_TYPE
{
	series_metal,			//	½ğÏµ
	series_wood,			//	Ä¾Ïµ
	series_water,			//	Ë®Ïµ
	series_fire,			//	»ğÏµ
	series_earth,			//	ÍÁÏµ
	series_nil,
	series_num = series_nil,
	series_minus,
};

enum OBJ_GENDER
{
	OBJ_G_MALE	= 0,	//ĞÛĞÔ£¬ÄĞµÄ
	OBJ_G_FEMALE,		//´ÆµÄ£¬Å®µÄ
};

enum NPCCAMP
{
	camp_begin,	// 0 ch÷ tr¾ng
	camp_justice, // 1 ch¸nh ph¸i
	camp_evil, // 2 tµ ph¸i
	camp_balance, // 3 trung lËp
	camp_free,	// 4 mµu ®á s¸t thñ
	camp_animal, // 5 mµu hång admin
	camp_event,		// 6 mµu hång admin
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
	IIEP_NORMAL = 0,	//Ò»°ã/Õı³£/¿ÉÓÃ
	IIEP_NOT_USEABLE,	//²»¿ÉÓÃ/²»¿É×°Åä
	IIEP_SPECIAL,		//ÌØ¶¨µÄ²»Í¬Çé¿ö
};

#define	GOD_MAX_OBJ_TITLE_LEN	2048	//128ÁÙÊ±¸ÄÎª1024ÎªÁË¼æÈİ¾É´úÂë to be modified
#define	GOD_MAX_OBJ_PROP_LEN	1024
#define	GOD_MAX_OBJ_DESC_LEN	1024

//==================================
//	ÓÎÏ·¶ÔÏóµÄÃèÊö
//==================================
struct KGameObjDesc
{
	char	szTitle[GOD_MAX_OBJ_TITLE_LEN];	//±êÌâ£¬Ãû³Æ
	char	szProp[GOD_MAX_OBJ_PROP_LEN];	//ÊôĞÔ£¬Ã¿ĞĞ¿ÉÒÔtab»®·ÖÎª¿¿×óÓë¿¿ÓÒ¶ÔÆëÁ½²¿·Ö
	char	szDesc[GOD_MAX_OBJ_DESC_LEN];	//ÃèÊö
};

//==================================
//	ÎÊÌâÓë¿ÉÑ¡´ğ°¸
//==================================

struct KUiAnswer
{
	char	AnswerText[256];	//¿ÉÑ¡´ğ°¸ÎÄ×Ö£¨¿ÉÒÔ°üº¬¿ØÖÆ·û£©
	int		AnswerLen;			//¿ÉÑ¡´ğ°¸´æ´¢³¤¶È£¨°üÀ¨¿ØÖÆ·û£¬²»°üº¬½áÊø·û£©
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
	short		m_nID;				// ÎïÆ·µÄID
	BYTE	m_btGenre;			// ÎïÆ·µÄÀàĞÍ
	short		m_btDetail;			// ÎïÆ·µÄÀà±ğ
	short		m_btParticur;		// ÎïÆ·µÄÏêÏ¸Àà±ğ
	BYTE	m_btSeries;			// ÎïÆ·µÄÎåĞĞ
	BYTE	m_btLevel;			// ÎïÆ·µÄµÈ¼¶
	BYTE	m_btLuck;			// MF
	short		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// Éú³É²ÎÊı
	WORD	m_wVersion;			// ×°±¸°æ±¾
	DWORD	m_dwRandomSeed;		// Ëæ»úÖÖ×Ó
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
	int			 nLines;				//ÕâÌõĞÅÏ¢Õ¼ÁË¶àÉÙĞĞ
	int			 nCharWidth;			//ÕâÌõĞÅÏ¢Õ¼ÁË×Ö·û¿í
	ChatItem	 sItem;					//thong tin cua item
	int			 nHaveItem;				//co item khong?
	int			 nPos;					// vi tri trong cau
	int			 nFace;					// so icon trong 1 dong
	int			 nLinePos;				// item nam dong thu...
	unsigned int uTextBKColor;			//ÕâÌõĞÅÏ¢×ÖÏÔÊ¾Ê±µÄÎÄ×Ö³Äµ×µÄÑÕÉ«,0ÎªÎŞ
	int			 nMaxSize;				//Msg¿ÉÒÔÈİÄÉµÄ×î´óĞÅÏ¢³¤¶È
	int			 nLen;					//ĞÅÏ¢³¤¶È
	char		 Msg[1];				//ĞÅÏ¢µÄÄÚÈİ
};

enum ITEMKIND
{
	normal_item = 0,	//®å tr¾ng
	green_item,				//®å xanh
	broken_item,			//®å bŞ háng
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
	unsigned int uGenre;	//¶ÔÏóÀàÊô
	unsigned int uId;		//¶ÔÏóid
//	int			 nData;		//Óë¶ÔÏóÊµÀıÏà¹ØµÄÄ³Êı¾İ
};

//==================================
//	ÒÔ×ø±ê±íÊ¾µÄÒ»¸öÇøÓò·¶Î§
//==================================
struct KUiRegion
{
	int		h;		//×óÉÏ½ÇÆğµãºá×ø±ê
	int		v;		//×óÉÏ½ÇÆğµã×İ×ø±ê
	int		Width;	//ÇøÓòºá¿í
	int		Height;	//ÇøÓò×İ¿í
};

enum UIOBJECT_CONTAINER
{
	UOC_IN_HAND	= 1,		//ÊÖÖĞÄÃ×Å
	UOC_GAMESPACE,			//ÓÎÏ·´°¿Ú
	UOC_IMMEDIA_ITEM,		//¿ì½İÎïÆ·
	UOC_IMMEDIA_SKILL,		//¿ì½İÎä¹¦0->ÓÒ¼üÎä¹¦£¬1,2...-> F1,F2...¿ì½İÎä¹¦
	UOC_ITEM_TAKE_WITH,		//ËæÉíĞ¯´ø
	UOC_SKILL_TAKE_WITH,
	UOC_TO_BE_TRADE,		//Òª±»ÂòÂô£¬ÂòÂôÃæ°åÉÏ
	UOC_OTHER_TO_BE_TRADE,	//ÂòÂôÃæ°åÉÏ£¬±ğÈËÒªÂô¸ø×Ô¼ºµÄ£¬
	UOC_EQUIPTMENT,			//ÉíÉÏ×°±¸
	UOC_NPC_SHOP,			//npcÂòÂô³¡Ëù
	UOC_MARKET,
	UOC_STORE_BOX,			//´¢ÎïÏä
	UOC_EX_BOX1,
	UOC_EX_BOX2,
	UOC_EX_BOX3,
	UOC_ITEM_EX,
	UOC_SKILL_LIST,			//ÁĞ³öÈ«²¿ÓµÓĞ¼¼ÄÜµÄ´°¿Ú£¬¼¼ÄÜ´°¿Ú
	UOC_SKILL_TREE,			//×ó¡¢ÓÒ¿ÉÓÃ¼¼ÄÜÊ÷
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
	char			szItemName[64];	//ÎïÆ·Ãû³Æ
	int				nOldPrice;
	int				nCurPrice;			//ÂòÂô¼ÛÇ®£¬ÕıÖµÎªÂô¼Û¸ñ£¬¸ºÖµ±íÊ¾ÂòÈëµÄ¼Û¸ñÎª(-nPrice)
	int				nMoneyUnit;
	BOOL			bNewArrival;
	int				nPrice;			//ÂòÂô¼ÛÇ®£¬ÕıÖµÎªÂô¼Û¸ñ£¬¸ºÖµ±íÊ¾ÂòÈëµÄ¼Û¸ñÎª(-nPrice)
	int				nPriceXu;
};

//==================================
//	±íÊ¾Ä³¸öÓÎÏ·¶ÔÏóÔÚ×ø±êÇøÓò·¶Î§µÄĞÅÏ¢½á¹¹
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
	unsigned char	eGenre;	//È¡Öµ·¶Î§ÎªÃ¶¾ÙÀàĞÍMSG_GENRE_LIST,¼ûMsgGenreDef.hÎÄ¼ş
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
	PA_NONE = 0,	//ÎŞ¶¯×÷
	PA_RUN  = 0x01,	//ÅÜ
	PA_SIT  = 0x02,	//´ò×ø
	PA_RIDE = 0x04,	//Æï£¨Âí£©
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
//	ÏµÍ³ÏûÏ¢·ÖÀà
//==================================
enum SYS_MESSAGE_TYPE
{
	SMT_NORMAL = 0,	//²»²Î¼Ó·ÖÀàµÄÏûÏ¢
	SMT_SYSTEM,		//ÏµÍ³£¬Á¬½ÓÏà¹Ø
	SMT_PLAYER,		//Íæ¼ÒÏà¹Ø
	SMT_TEAM,		//×é¶ÓÏà¹Ø
	SMT_FRIEND,		//ÁÄÌìºÃÓÑÏà¹Ø
	SMT_MISSION,	//ÈÎÎñÏà¹Ø
	SMT_CLIQUE,		//°ïÅÉÏà¹Ø
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
	char			szMessage[64];	//ÏûÏ¢ÎÄ±¾
	unsigned int	uReservedForUi;	//½çÃæÊ¹ÓÃµÄÊı¾İÓò,coreÀïÌî0¼´¿É
	unsigned char	eType;			//ÏûÏ¢·ÖÀàÈ¡ÖµÀ´×ÔÃ¶¾ÙÀàĞÍ SYS_MESSAGE_TYPE
	unsigned char	byConfirmType;	//ÏìÓ¦ÀàĞÍ
	unsigned char	byPriority;		//ÓÅÏÈ¼¶,ÊıÖµÔ½´ó£¬±íÊ¾ÓÅÏÈ¼¶Ô½¸ß
	unsigned char	byParamSize;	//°éËæGDCNI_SYSTEM_MESSAGEÏûÏ¢µÄpParamBufËùÖ¸²ÎÊı»º³åÇø¿Õ¼äµÄ´óĞ¡¡£
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
	char	szTitle[32];	//·Ö×éµÄÃû³Æ
	int		nNumFriend;		//×éÄÚºÃÓÑµÄÊıÄ¿
};

struct KUiChatMessage
{
	unsigned int uColor;
	short	nContentLen;
	char	szContent[256];
};

struct KUiPlayerBaseInfo
{
	char	Agname[32];	//´ÂºÅ
	char	Name[32];	//Ãû×Ö
	char	Title[32];	//³ÆºÅ
	char	szMateName[32]; //#MateName
	int		nCurFaction;// µ±Ç°¼ÓÈëÃÅÅÉ id £¬Èç¹ûÎª -1 £¬µ±Ç°Ã»ÓĞÔÚÃÅÅÉÖĞ
	int		nRankInWorld;//½­ºşÅÅÃûÖµ,ÖµÎª0±íÊ¾Î´ÉÏÅÅÃû°å
	unsigned int nCurTong;// µ±Ç°¼ÓÈë°ïÅÉname id £¬Èç¹ûÎª 0 £¬µ±Ç°Ã»ÓĞÔÚ°ïÅÉÖĞ
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
	int		nLifeFull;			//ÉúÃüÂúÖµ
	int		nLife;				//ÉúÃü
	int		nManaFull;			//ÄÚÁ¦ÂúÖµ
	int		nMana;				//ÄÚÁ¦
	int		nStaminaFull;		//ÌåÁ¦ÂúÖµ
	int		nStamina;			//ÌåÁ¦
	int		nAngryFull;			//Å­ÂúÖµ
	int		nAngry;				//Å­
	double		nExperienceFull;	//¾­ÑéÂúÖµ
	double		nExperience;		//µ±Ç°¾­ÑéÖµ
	double		nCurLevelExperience;//µ±Ç°¼¶±ğÉı¼¶ĞèÒªµÄ¾­ÑéÖµ

	unsigned char	byActionDisable;//ÊÇ·ñ²»¿É½øĞĞ¸÷ÖÖ¶¯×÷£¬ÎªÃ¶¾ÙPLAYER_ACTION_LISTÈ¡ÖµµÄ×éºÏ
	unsigned char	byAction;	//ÕıÔÚ½øĞĞµÄĞĞÎª¶¯×÷£¬ÎªÃ¶¾ÙPLAYER_ACTION_LISTÈ¡ÖµµÄ×éºÏ
	unsigned short	wReserved;	//±£Áô
};

enum UI_PLAYER_ATTRIBUTE
{
	UIPA_STRENGTH = 0,			//Á¦Á¿
	UIPA_DEXTERITY,				//Ãô½İ
	UIPA_VITALITY,				//»îÁ¦
	UIPA_ENERGY,				//¾«Á¦
};

struct KUiPlayerAttribute
{
	int		nMoney;				//ÒøÁ½
	int		nXu;
	int		nLevel;				//µÈ¼¶
	char	StatusDesc[16];		//×´Ì¬ÃèÊö

	int		nBARemainPoint;		//»ù±¾ÊôĞÔÊ£ÓàµãÊı
	int		nStrength;			//Á¦Á¿
	int		nDexterity;			//Ãô½İ
	int		nVitality;			//»îÁ¦
	int		nEnergy;			//¾«Á¦

	int		nKillMAX;			//×î´óÉ±ÉËÁ¦
	int		nKillMIN;			//×îĞ¡É±ÉËÁ¦
	int		nRightKillMax;		//ÓÒ¼ü×î´óÉ±ÉËÁ¦
	int		nRightKillMin;		//ÓÒ¼ü×îĞ¡É±ÉËÁ¦

	int		nAttack;			//¹¥»÷Á¦
	int		nDefence;			//·ÀÓùÁ¦
	int		nMoveSpeed;			//ÒÆ¶¯ËÙ¶È
	int		nAttackSpeed;		//¹¥»÷ËÙ¶È
	int		nCastSpeed;		//¹¥»÷ËÙ¶È

	int		nPhyDef;			//ÎïÀí·ÀÓù
	int		nCoolDef;			//±ù¶³·ÀÓù
	int		nLightDef;			//ÉÁµç·ÀÓù
	int		nFireDef;			//»ğÑæ·ÀÓù
	int		nPoisonDef;			//¶¾ËØ·ÀÓù

	int		nPhyDefPlus;			//ÎïÀí·ÀÓù
	int		nCoolDefPlus;			//±ù¶³·ÀÓù
	int		nLightDefPlus;			//ÉÁµç·ÀÓù
	int		nFireDefPlus;			//»ğÑæ·ÀÓù
	int		nPoisonDefPlus;			//¶¾ËØ·ÀÓù

	int		nRankInWorld;	// xep hang
	int		nRepute; // danh vong
	int		nFuYuan; // phuc duyen
	int		nReBorn; // trung sinh
	int		nPKValue; // phuc duyen
};

#define		RESIST_PLUS_SCALE		15		//Kh¸ng t¨ng thªm 75+

#define	UPB_IMMEDIA_ITEM_COUNT	9

//==================================
//	Ö÷½ÇµÄÁ¢¼´Ê¹ÓÃÎïÆ·ÓëÎä¹¦
//==================================
struct KUiPlayerImmedItemSkill
{
	KUiGameObject	ImmediaItem[UPB_IMMEDIA_ITEM_COUNT];
	KUiGameObject	IMmediaSkill[2];
};

//==================================
//	Ö÷½Ç×°±¸°²»»µÄÎ»ÖÃ
//==================================
enum UI_EQUIPMENT_POSITION
{
	UIEP_HEAD = 0,		//Í·´÷
	UIEP_HAND = 1,		//ÊÖ³Ö
	UIEP_NECK = 2,		//²±×Ó
	UIEP_FINESSE = 3,	//ÊÖÍó
	UIEP_BODY = 4,		//Éí´©
	UIEP_WAIST = 5,		//Ñü²¿
	UIEP_FINGER1 = 6,	//ÊÖÖ¸¼×
	UIEP_FINGER2 = 7,	//ÊÖÖ¸ÒÒ
	UIEP_WAIST_DECOR= 8,//Ñü×¹
	UIEP_FOOT = 9,		//½Å²È
	UIEP_HORSE = 10,	//ÂíÆ¥
	UIEP_MASK = 11,		// mat na
	UIEP_FIFONG = 12 //#phi phong
};

//==================================
//	Ö÷½ÇµÄÉú»î¼¼ÄÜÊı¾İ
//==================================
struct KUiPlayerLiveSkillBase
{
	int		nRemainPoint;			//Ê£Óà¼¼ÄÜµãÊı
	int		nLiveExperience;		//µ±Ç°¼¼ÄÜ¾­ÑéÖµ
	int		nLiveExperienceFull;	//Éıµ½ÏÂ¼¶ĞèÒªµÄ¾­ÑéÖµ
};

//==================================
//	µ¥Ïî¼¼ÄÜÊı¾İ
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
//	Ò»¸ö¶ÓÎéÖĞ×î¶à°üº¬³ÉÔ±µÄÊıÄ¿
//==================================
#define	PLAYER_TEAM_MAX_MEMBER	500

//==================================
//	Í³Ë§ÄÜÁ¦Ïà¹ØµÄÊı¾İ
//==================================
struct KUiPlayerLeaderShip
{
	int		nLeaderShipLevel;			
	int		nLeaderShipExperience;		
	int		nLeaderShipExperienceFull;	
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
//	×é¶ÓĞÅÏ¢µÄÃèÊö
//==================================
struct KUiTeamItem
{
	KUiPlayerItem	Leader;
};

//==================================
//	¶ÓÎéĞÅÏ¢
//==================================
struct KUiPlayerTeam
{
	bool			bTeamLeader;			//Íæ¼Ò×Ô¼ºÊÇ·ñ¶Ó³¤
	char			cNumMember;				//¶ÓÔ±ÊıÄ¿
	char			cNumTojoin;				//Óû¼ÓÈëµÄÈËÔ±µÄÊıÄ¿
	bool			bOpened;				//¶ÓÎéÊÇ·ñÔÊĞíÆäËûÈË¼ÓÈë
	int				nTeamServerID;			//¶ÓÎéÔÚ·şÎñÆ÷ÉÏµÄid£¬ÓÃÓÚ±êÊ¶¸Ã¶ÓÎé£¬-1 Îª¿Õ
	int				nCaptainPower;
};

//==================================
//	Ä§·¨ÊôĞÔ
//==================================
#ifndef MAGICATTRIB
#define MAGICATTRIB
struct KMagicAttrib
{
	int				nAttribType;					//ÊôĞÔÀàĞÍ
	int				nValue[3];						//ÊôĞÔ²ÎÊı
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};

struct KMagicAutoSkill
{
	int				nSkillId;
	int				nSkillLevel;
	int				nRate;						//ÊôĞÔ²ÎÊı
	DWORD			dwNextCastTime;
	int				nWaitCastTime;
	KMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = 0;};
};
#else
struct KMagicAttrib;
struct KMagicAutoSkill;
#endif

/* ÕâÊÇ¾ÉµÄ´úÂë£¬ĞÂµÄÒÑ¾­·ÅÔÚKNpcGoldÀïÃæ´¦ÀíÁË
//==================================
//	NPC¼ÓÇ¿
//==================================
struct KNpcEnchant
{
	int		nExp;					// ¾­Ñé
	int		nLife;					// ÉúÃü
	int		nLifeReplenish;			// »ØÑª
	int		nAttackRating;			// ÃüÖĞ
	int		nDefense;				// ·ÀÓù
	int		nMinDamage;
	int		nMaxDamage;

	int		TreasureNumber;				// ×°±¸
	int		AuraSkill;					// ¹â»·
	int		DamageEnhance;				// ÉËº¦
	int		SpeedEnhance;				// ËÙ¶È
	int		SelfResist;					// ×ÔÉí¿¹ĞÔ
	int		ConquerResist;				// ÏàÉú¿¹ĞÔ
#ifndef _SERVER
	char	NameModify[32];				// ¸ÄÃû
#endif
};

//==================================
//	NPCµ¥Ïî¼ÓÇ¿
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
//	Ñ¡ÏîÉèÖÃÏî
//==================================
enum OPTIONS_LIST
{
	OPTION_PERSPECTIVE,		//Í¸ÊÓÄ£Ê½  nParam = (int)(bool)bEnable ÊÇ·ñ¿ªÆô
	OPTION_DYNALIGHT,		//¶¯Ì¬¹âÓ°	nParam = (int)(bool)bEnable ÊÇ·ñ¿ªÆô
	OPTION_MUSIC_VALUE,		//ÒôÀÖÒôÁ¿	nParam = ÒôÁ¿´óĞ¡£¨È¡ÖµÎª0µ½-10000£©
	OPTION_SOUND_VALUE,		//ÒôĞ§ÒôÁ¿	nParam = ÒôÁ¿´óĞ¡£¨È¡ÖµÎª0µ½-10000£©
	OPTION_BRIGHTNESS,		//ÁÁ¶Èµ÷½Ú	nParam = ÁÁ¶È´óĞ¡£¨È¡ÖµÎª0µ½-100£©
	OPTION_WEATHER,			//ÌìÆøĞ§¹û¿ª¹Ø nParam = (int)(bool)bEnable ÊÇ·ñ¿ªÆô
	OPTION_QUALITY_GIAMPLAYER,//add by phong kiÒu
	OPTION_QUALITY_MATNPC,
	OPTION_QUALITY_MATPLAYER,
	OPTION_QUALITY_GIAMSKILL,
	OPTION_QUALITY_MATSKILL,
};

//==================================
//	Ëù´¦µÄµØÓòÊ±¼ä»·¾³ĞÅÏ¢
//==================================
struct KUiSceneTimeInfo
{
	char	szSceneName[32];		//³¡¾°Ãû
	int		nSceneId;				//³¡¾°id
	int		nScenePos0;				//³¡¾°µ±Ç°×ø±ê£¨¶«£©
	int		nScenePos1;				//³¡¾°µ±Ç°×ø±ê£¨ÄÏ£©
	int		nGameSpaceTime;			//ÒÔ·ÖÖÓÎªµ¥Î»
};

struct KUiSceneTimeInfoOften : public KUiSceneTimeInfo
{
};

//==================================
//	¹âÔ´ĞÅÏ¢
//==================================
//ÕûÊı±íÊ¾µÄÈıÎ¬µã×ø±ê
struct KPosition3
{
	int nX;
	int nY;
	int nZ;
};

struct KLightInfo
{
	KPosition3 oPosition;			// ¹âÔ´Î»ÖÃ
	DWORD dwColor;					// ¹âÔ´ÑÕÉ«¼°ÁÁ¶È
	long  nRadius;					// ×÷ÓÃ°ë¾¶
};


//Ğ¡µØÍ¼µÄÏÔÊ¾ÄÚÈİÏî
enum SCENE_PLACE_MAP_ELEM
{ 
	SCENE_PLACE_MAP_ELEM_NONE		= 0x00,		//ÎŞ¶«Î÷
	SCENE_PLACE_MAP_ELEM_PIC		= 0x01,		//ÏÔÊ¾ËõÂÔÍ¼
	SCENE_PLACE_MAP_ELEM_CHARACTER	= 0x02,		//ÏÔÊ¾ÈËÎï
	SCENE_PLACE_MAP_ELEM_PARTNER	= 0x04,		//ÏÔÊ¾Í¬¶ÓÎéÈË
};

//³¡¾°µÄµØÍ¼ĞÅÏ¢
struct KSceneMapInfo
{
	int	nScallH;		//ÕæÊµ³¡¾°Ïà¶ÔÓÚµØÍ¼µÄºáÏò·Å´ó±ÈÀı
	int nScallV;		//ÕæÊµ³¡¾°Ïà¶ÔÓÚµØÍ¼µÄ×İÏò·Å´ó±ÈÀı
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
	kind_normal = 0	,		//qu¸i b×nh th­êng
	kind_player,				//ng­êi ch¬i
	kind_partner,				//b¹n ®ång hµnh
	kind_dialoger,				//®èi tho¹i
	kind_bird,					//bay kh«ng thÊy sö dông
	kind_mouse,				//chuét kh«ng thÊy sö dông
    kind_num				//tèi ®a
};

enum	// Îï¼şÀàĞÍ
{
	Obj_Kind_MapObj = 0,		// µØÍ¼Îï¼ş£¬Ö÷ÒªÓÃÓÚµØÍ¼¶¯»­
	Obj_Kind_Body,				// npc µÄÊ¬Ìå
	Obj_Kind_Box,				// ±¦Ïä
	Obj_Kind_Item,				// µôÔÚµØÉÏµÄ×°±¸
	Obj_Kind_Money,				// µôÔÚµØÉÏµÄÇ®
	Obj_Kind_LoopSound,			// Ñ­»·ÒôĞ§		//Loop
	Obj_Kind_RandSound,			// Ëæ»úÒôĞ§
	Obj_Kind_Light,				// ¹âÔ´£¨3DÄ£Ê½ÖĞ·¢¹âµÄ¶«Î÷£©
	Obj_Kind_Door,				// ÃÅÀà
	Obj_Kind_Trap,				// ÏİÚå
	Obj_Kind_Prop,				// Ğ¡µÀ¾ß£¬¿ÉÖØÉú
	Obj_Kind_Task,				// Obj nhiem vu
	Obj_Kind_Num,				// Îï¼şµÄÖÖÀàÊı
};

//Ö÷½ÇÉí·İµØÎ»µÈÒ»Ğ©¹Ø¼üÊôĞÔÏî
enum PLAYER_BRIEF_PROP
{
	PBP_LEVEL = 1,	//µÇ¼¶±ä»¯	nParam±íÊ¾µ±Ç°µÈ¼¶
	PBP_FACTION,	//ÃÅÅÉ		nParam±íÊ¾ÃÅÅÉÊôĞÔ£¬Èç¹ûnParamÎª-1±íÊ¾Ã»ÓĞÃÅÅÉ
	PBP_CLIQUE,		//°ïÅÉ		nParamÎª·Ç0Öµ±íÊ¾ÈëÁË°ïÅÉ£¬0Öµ±íÊ¾ÍÑÀëÁË°ïÅÉ
};

//ĞÂÎÅÏûÏ¢µÄÀàĞÍ¶¨Òå
enum NEWS_MESSAGE_TYPE
{
	NEWSMESSAGE_NORMAL,			//Ò»°ãÏûÏ¢£¬ÏÔÊ¾£¨Ò»´Î£©¾ÍÏûÏ¢ÏûÍöÁË
								//ÎŞÊ±¼ä²ÎÊı
	NEWSMESSAGE_COUNTING,		//µ¹¼Æ£¨Ãë£©ÊıÏûÏ¢£¬¼ÆÊıµ½0Ê±£¬¾ÍÏûÏ¢¾ÍÏûÍöÁË¡£
								//Ê±¼ä²ÎÊıÖĞµÄÊı¾İ½á¹¹ÖĞ½öÃëÊı¾İÓĞĞ§£¬µ¹¼ÆÊıÒÔÃëÎªµ¥Î»¡£
	NEWSMESSAGE_TIMEEND,		//¶¨Ê±ÏûÏ¢£¬¶¨Ê±µ½Ê±£¬ÏûÏ¢¾ÍÏûÍêÁË£¬·ñÔòÃ¿°ë·ÖÖÓÏÔÊ¾Ò»´Î¡£
								//Ê±¼ä²ÎÊı±íÊ¾ÏûÍöµÄÖ¸¶¨Ê±¼ä¡£
	NEWSMESSAGE_NORMAL_1,		// moi them	
	
	NEWSMESSAGE_COUNTING_1,		//moi them	

	NEWSMESSAGE_TIMEEND_1,		//moi them

	NEWSMESSAGE_NORMAL_2,		// moi them	
};

#define MAX_MESSAGE_LENGTH 512

struct KNewsMessage
{
	int		nType;						//ÏûÏ¢ÀàĞÍ
	char	sMsg[MAX_MESSAGE_LENGTH];	//ÏûÏ¢ÄÚÈİ
	int		nMsgLen;					//ÏûÏ¢ÄÚÈİ´æ´¢³¤¶È
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
	bool			bValueAppened;	//Ã¿Ò»ÏîÊÇ·ñÓĞÃ»ÓĞ¶îÍâÊı¾İ
	bool			bSortFlag;		//Ã¿Ò»ÏîÊÇ·ñÓĞÃ»ÓĞÉı½µ±ê¼Ç
	unsigned short	usIndexId;		//ÅÅÃûÏîIDÊıÖµ
};

#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
	char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];	// ÎÄ×ÖÄÚÈİ
	unsigned short		usMsgLen;				// ÎÄ×ÖÄÚÈİµÄ³¤¶È
	short				cSortFlag;				// Æì±êÖµ£¬QOO_RANK_DATAµÄÊ±ºò±íÊ¾³öÉı½µ£¬¸ºÖµ±íÊ¾½µ£¬ÕıÖµ±íÊ¾Éı£¬0Öµ±íÊ¾Î»ÖÃÎ´±ä
	int					nValueAppend;			// ´ËÏî¸½´øµÄÖµ

};

struct KMissionRecord
{
	char			sContent[256];
	int				nContentLen;
	unsigned int	uValue;	
	//char			szContent[MAX_SCIRPTACTION_BUFFERNUM];
	//int				nRecordAct;
};

//---------------------------- °ï»áÏà¹Ø ------------------------

#define		defTONG_MAX_DIRECTOR				7
#define		defTONG_MAX_MANAGER					56
#define		defTONG_ONE_PAGE_MAX_NUM			7
#define		defTONG_STR_LENGTH					32
#define		defTONG_NAME_MAX_LENGTH				16

enum TONG_MEMBER_FIGURE
{
	enumTONG_FIGURE_MEMBER,				// thµnh viªn
	enumTONG_FIGURE_MANAGER,			// ®­êng chñ
	enumTONG_FIGURE_DIRECTOR,			// tr­ëng l·o
	enumTONG_FIGURE_MASTER,				// bang chñ
	enumTONG_FIGURE_LEAGUE,				//lien minh
	enumTONG_FIGURE_NUM,
};

enum
{
	enumTONG_APPLY_INFO_ID_SELF,		// ÉêÇë²éÑ¯×ÔÉíĞÅÏ¢
	enumTONG_APPLY_INFO_ID_MASTER,		// ÉêÇë²éÑ¯°ïÖ÷ĞÅÏ¢
	enumTONG_APPLY_INFO_ID_DIRECTOR,	// ÉêÇë²éÑ¯³¤ÀÏĞÅÏ¢
	enumTONG_APPLY_INFO_ID_MANAGER,		// ÉêÇë²éÑ¯¶Ó³¤ĞÅÏ¢
	enumTONG_APPLY_INFO_ID_MEMBER,		// ÉêÇë²éÑ¯°ïÖÚĞÅÏ¢(Ò»Åú°ïÖÚ)
	enumTONG_APPLY_INFO_ID_ONE,			// ÉêÇë²éÑ¯Ä³°ï»á³ÉÔ±ĞÅÏ¢(Ò»¸ö°ïÖÚ)
	enumTONG_APPLY_INFO_ID_TONG_HEAD,	// ÉêÇë²éÑ¯Ä³°ï»áĞÅÏ¢£¬ÓÃÓÚÉêÇë¼ÓÈë°ï»á
	enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
	enumTONG_CREATE_ERROR_ID1,	// Player[m_nPlayerIndex].m_nIndex <= 0
	enumTONG_CREATE_ERROR_ID2,	// ½»Ò×¹ı³ÌÖĞ
	enumTONG_CREATE_ERROR_ID3,	// °ï»áÃûÎÊÌâ
	enumTONG_CREATE_ERROR_ID4,	// °ï»áÕóÓªÎÊÌâ
	enumTONG_CREATE_ERROR_ID5,	// ÒÑ¾­ÊÇ°ï»á³ÉÔ±
	enumTONG_CREATE_ERROR_ID6,	// ×Ô¼ºµÄÕóÓªÎÊÌâ
	enumTONG_CREATE_ERROR_ID7,	// µÈ¼¶ÎÊÌâ	
	enumTONG_CREATE_ERROR_ID8,	// Ç®ÎÊÌâ
	enumTONG_CREATE_ERROR_ID9,	// ×é¶Ó²»ÄÜ½¨°ï»á
	enumTONG_CREATE_ERROR_ID10,	// °ï»áÄ£¿é³ö´í
	enumTONG_CREATE_ERROR_ID11,	// Ãû×Ö×Ö·û´®³ö´í
	enumTONG_CREATE_ERROR_ID12,	// Ãû×Ö×Ö·û´®¹ı³¤
	enumTONG_CREATE_ERROR_ID13,	// °ï»áÍ¬Ãû´íÎó
	enumTONG_CREATE_ERROR_ID14,	// °ï»á²úÉúÊ§°Ü
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
	char  szMasterName[32];		//°ïÖ÷Ãû×Ö
	int   nFaction;				//°ï»áÕóÓª
	int   nMoney;				//°ï»á×Ê½ğ
	int   nReserve[2];
	int	  nMemberCount;         //°ïÖÚÊıÁ¿
	int   nManagerCount;        //¶Ó³¤ÊıÁ¿
	int   nDirectorCount;       //³¤ÀÏÊıÁ¿
	int   nFigure;              //´ıÉ¾³ı
	//===========add by Fong KiÒu 12/12/2021==========
	int		nStatusGuide; //t×nh tr¹ng bang héi
	char  szWayEdit[32];			// tiªu chİ bang héi
	char  szNextTargetEdit[32];			// môc tiªu hiÖn thêi bang héi
	int		nExpGuide; //®iÓm kinh nghiÖm bang héi
	int		nCityGuide; //thµnh thŞ chiÕm h÷u
	int		nTongLevel; //®¼ng cÊp bang héi
	char  szLeagueTName[32];		//tªn bang héi liªn minh
};

enum TONG_ACTION_TYPE
{
	TONG_ACTION_DISMISS,       //ÌßÈË
	TONG_ACTION_ASSIGN,        //ÈÎÃü
	TONG_ACTION_DEMISE,        //´«Î»
	TONG_ACTION_LEAVE,         //Àë°ï
	TONG_ACTION_RECRUIT,       //ÕĞÈË×´Ì¬
	TONG_ACTION_APPLY,         //ÉêÇë¼ÓÈë
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
	TONG_ACTION_SAVE,       //ÌßÈË
	TONG_ACTION_GET,        //ÈÎÃü
	TONG_ACTION_SND,        //´«Î»
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
typedef struct
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

#define MAX_ITEM_SPC								5		//max item trªn giá hµng
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
