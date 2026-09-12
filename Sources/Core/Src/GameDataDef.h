/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-9-12
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#ifndef JX_LP64_TYPES	// [ANDROID 08/09] long tren dia/goi: Windows 4 byte (giu nguyen long), Android LP64 phai la 32 bit
#define JX_LP64_TYPES
#if defined(JX_POSIX)
typedef int JX_LONG; typedef unsigned int JX_ULONG;
#else
typedef long JX_LONG; typedef unsigned long JX_ULONG;
#endif
#endif
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
// [PFCHAT 02/09] 39 -> 43: chuoi item-chat [..] mang them 4 o m_nPfPack (sao/chuc phuc/13 lo da phi phong) sau 16 khe magic
// [FUSCHAT 02/09] 43 -> 44: them MOT truong nen (base 62) mang 6 o Van Cuong da dung luyen
// (P + seed); truong rong = khong dung luyen -> link do thuong chi dai them 1 ky tu (dau phay).
#define		NUM_INFO_ITEM_CHAT					44				//26 th«ng tin ®­îc slipt trong string göi ®i
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
#define		MAX_DEATH_PUNISH_PK_VALUE			10		// PK 0 ®Õn 10
#define		MAX_REPUTE_VALUE					100000	
#define		MAX_FUYUAN_VALUE					100000	
#define		MAX_REBORN_VALUE					100000
#define		MSG_NON_SETTINGS		"Kh«ng ThiÕt LËp"
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
#define		T_ANTI_HACK1 108 //add by phong kiÒu antihack	//®ãng ch­a s?dông
#define		T_ANTI_HACK2 109	//®ãng ch­a s?dông

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
	room_equipment = 0,	// trang bŞ trªn ng­êi
	room_equipmentback, // backup to switch equip
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
// [LMBC 06/09] 4 -> 8: script Long Mon Tieu Cuc (ban Linux) dung toi chi so 6.
// Cay script dang chay CUNG da ghi/doc chi so 4 (tong_disciple, tong_springfestival,
// muren_death) tuc la dang ghi NGOAI MANG; noi mang bien chung thanh o that.
// KNpc.h chi duoc project Core dung (Engine/Src/LuaFuns.cpp chet vi USEOLD khong dinh nghia)
// nen chi can Rebuild ca hai cau hinh cua Core, khong phai swap dong bo nhi phan khac.
// PHAI di kem kiem bien o LuaSetNpcParam/LuaGetNpcParam (ScriptFuns.cpp).
#define		MAX_NPCPARAM			8
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
	// [HOASON 01/09] so mon phai TOI DA (13 = 10 goc + Hoa Son 10 + Vu Hon 11 + Tieu Dao 12);
	// truoc day MAX_FACTION = 2 phai x 5 he (KFaction.h). Dung o KProtocol.h TGAME_STAT_DATA,
	// KLadder.h, KNpc.cpp - CORE va GODDESS (DBBackup.h) phai cung con so nay.
	MAX_FACTION_NUM = 13,

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

#define	GOD_MAX_OBJ_TITLE_LEN	4096	// [PF13 31/08b] cu 2048: GetDesc ghi TOAN BO tooltip noi tiep tu szTitle, 13 dong da + khoi an vuot 2048	//128ÁÙÊ±¸ÄÎª1024ÎªÁË¼æÈİ¾É´úÂë to be modified
#define	GOD_MAX_OBJ_PROP_LEN	2560	// [PF13 31/08] cu 1024: 13 dong da + 13 dong thuoc tinh + khoi an vuot 1024 (strcat khong chan). Chi la dem UI client, khong nam trong goi tin.
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

#pragma pack(push, 1)  // fix loi post item sai opt

typedef struct
{
	int		m_nID;				// ÎïÆ·µÄID
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
	int		m_nNature; //goldequip
	int		m_nMaxOptMultiply;
	int		m_nPfPack[4];		// [PFCHAT 02/09] = KItem::m_nPfPack (sao/chuc phuc/13 lo da phi phong). Cuoi struct. LUU Y: ChatItem nam trong goi s2c_diceitem (Headers\KDiceProtocol.h) -> CoreServer + CoreClient swap CUNG LUC.
	// [FUSCHAT 02/09] = KItem::m_nFusionP / m_uFusionSeed - 6 o Van Cuong da dung luyen.
	// Phai bang KItem::FUS_MAX_SLOT (6). Cuoi struct nhu pfpack, va cung nam trong goi
	// s2c_diceitem -> CoreServer + CoreClient + Game.exe swap CUNG LUC.
	int		m_nFusionP[6];
	unsigned	m_uFusionSeed[6];
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
#pragma pack(pop) // fix loi post item sai opt

// ======== [FUSCHAT 02/09] nen 6 o Van Cuong vao MOT truong cua chuoi item-chat ========
// Vi sao khong ghi thang 12 so thap phan: GameServer vut goi chat >= 255 byte
// (KSOServer.cpp:2778), ma 12 so (6 seed toi 10 chu so) an ~90 ky tu cua MOI link,
// khong con cho cho chu nguoi choi go kem. Cach nen: moi o DA dung luyen = 9 ky tu
// base 62 theo thu tu [chi so o (1) | P (2) | seed (6)]; o trong khong ghi gi, nen
// mon khong dung luyen chi ton dung 1 dau phay ngan cach.
#define	FUSCHAT_MAX_STR		64		// 6 o x 9 ky tu + '\0'

// tra -1 neu khong phai chu so base 62 (dung lam ca cong tac kiem chuoi hong)
inline int FUSCHAT_TriSo(char c)
{
	if (c >= '0' && c <= '9')	return c - '0';
	if (c >= 'A' && c <= 'Z')	return c - 'A' + 10;
	if (c >= 'a' && c <= 'z')	return c - 'a' + 36;
	return -1;
}

inline void FUSCHAT_GhiSo(char* pszOut, unsigned uVal, int nKyTu)
{
	static const char szB62[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for (int i = nKyTu - 1; i >= 0; i--)
	{
		pszOut[i] = szB62[uVal % 62];
		uVal /= 62;
	}
}

// 6 o cua ChatItem -> chuoi (co the rong). pszOut can FUSCHAT_MAX_STR byte.
inline void FUSCHAT_Ma(const ChatItem* pItem, char* pszOut)
{
	if (!pszOut)
		return;
	pszOut[0] = 0;
	if (!pItem)
		return;
	int nOff = 0;
	for (int i = 0; i < 6; i++)
	{
		if (pItem->m_nFusionP[i] <= 0)
			continue;
		pszOut[nOff] = "0123456789"[i];		// chi so o 0..5
		FUSCHAT_GhiSo(pszOut + nOff + 1, (unsigned)pItem->m_nFusionP[i], 2);
		FUSCHAT_GhiSo(pszOut + nOff + 3, pItem->m_uFusionSeed[i], 6);
		nOff += 9;
	}
	pszOut[nOff] = 0;
}

// Chuoi -> 6 o. Chuoi rong/hong/thieu ky tu: bo qua tu cho hong tro di (mon hien
// nhu chua dung luyen o cac o do) - khong bao gio doc qua dau '\0'.
inline void FUSCHAT_Giai(const char* pszIn, ChatItem* pItem)
{
	if (!pItem)
		return;
	for (int i = 0; i < 6; i++)
	{
		pItem->m_nFusionP[i] = 0;
		pItem->m_uFusionSeed[i] = 0;
	}
	if (!pszIn)
		return;
	for (int nOff = 0; pszIn[nOff]; nOff += 9)
	{
		int j;
		for (j = 0; j < 9; j++)
		{
			if (FUSCHAT_TriSo(pszIn[nOff + j]) < 0)
				return;		// gap '\0' hoac ky tu la -> dung han
		}
		int nSlot = FUSCHAT_TriSo(pszIn[nOff]);
		if (nSlot < 0 || nSlot >= 6)
			return;
		int nP = FUSCHAT_TriSo(pszIn[nOff + 1]) * 62 + FUSCHAT_TriSo(pszIn[nOff + 2]);
		unsigned uSeed = 0;
		for (j = 0; j < 6; j++)
			uSeed = uSeed * 62 + (unsigned)FUSCHAT_TriSo(pszIn[nOff + 3 + j]);
		pItem->m_nFusionP[nSlot] = nP;
		pItem->m_uFusionSeed[nSlot] = uSeed;
	}
}
// ======== het khoi FUSCHAT ========

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
	UOC_IN_HAND = 1,		//ÊÖÖĞÄA×Å
	UOC_GAMESPACE,			//ÓÎÏ·´°¿Ú
	UOC_IMMEDIA_ITEM,		//¿´½UÎïÆ·
	UOC_IMMEDIA_SKILL,		//¿´½UÎä¹¦0->Ó?¼üÎä¹¦£¬1,2...-> F1,F2...¿´½UÎä¹¦
	UOC_ITEM_TAKE_WITH,		//ËæÉíĞ¯´ø
	UOC_SKILL_TAKE_WITH,
	UOC_TO_BE_TRADE,		//?ª±»Â?Âô£¬Â?ÂôAæ°åÉÏ
	UOC_OTHER_TO_BE_TRADE,	//Â?ÂôAæ°åÉÏ£¬±dÈË?ªÂô¸ø×Ô¼ºµÄ£¬
	UOC_TO_BE_GAMBLE,		//
	UOC_OTHER_TO_BE_GAMBLE,	//
	UOC_EQUIPTMENT,			//ÉíÉÏ×°±¸
	UOC_NPC_SHOP,			//npcÂ?Âô³¡Ëù
	UOC_MARKET,
	UOC_STORE_BOX,			//´¢ÎïÏä
	UOC_EX_BOX1,
	UOC_EX_BOX2,
	UOC_EX_BOX3,
	UOC_ITEM_EX,
	UOC_SKILL_LIST,			//ÁĞ³öÈ«²¿ÓµÓĞ¼¼ÄÜµÄ´°¿Ú£¬¼¼ÄÜ´°¿Ú
	UOC_SKILL_TREE,			//×ó¡¢Ó?¿ÉÓA¼¼ÄÜÊ÷
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
	SMCT_UI_GAMBLE,			//Agree or reject the gambling request,
};

struct KSystemMessage
{
	char			szMessage[128];	//ÏûÏ¢ÎÄ±¾
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
	int		nShield;		// [VHTD 02/09g] khien tinh con lai (0 = khong co)
	int		nShieldFull;	// [VHTD 02/09g] khien tinh toi da luc ap
};

enum UI_PLAYER_ATTRIBUTE : int	// [ANDROID 08/09] khop khai bao truoc 'enum UI_PLAYER_ATTRIBUTE : int' (UiAddPoint.h/UiStatus.h)
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
	BYTE	bMeridianLevel[MAX_MERIDIAN];
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
	UIEP_FIFONG = 12, //#phi phong
	UIEP_SIGNET,
	UIEP_SHIPIN,
	UIEP_HOODS,
	UIEP_CLOAK,
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

// [VHTD 02/09g] o 'tang' No/Am Luat (GDI_HS_SP)
struct KHsSpView
{
	int		nKey;
	int		nCount;
	int		nMax;
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

struct KUiTargetDetailInfo
{
	char			sTargetName[32];
	int				nLifePercent;
	BYTE			Series;
#ifdef JX_MOBILE
	// [ANDROID 09/09 VONG] vi tri VE cua muc tieu (toa do THE GIOI) de client ve vong chon duoi chan
	int				nViTriVeX;
	int				nViTriVeY;
	int				nDangKhoa;	// 1 = muc tieu dang KHOA (m_nPeopleIdx); 0 = chi dang tro chuot vao
	// [ANDROID 09/09 DANH] chi so NPC - can de goi LockSomeoneUseSkill(nChiSoNpc, ...)
	int				nChiSoNpc;
#endif
};

//==================================
//	[ANDROID 09/09 NGUA] Hoi Core truoc khi bam nut ky nang tren dien thoai.
//	GDI_KYNANG_MOBILE: uParam = KJxKyNangHoi*, nParam = 0.
//==================================
struct KJxKyNangHoi
{
	int	nSkillId;	// VAO : ma ky nang
	int	nNgua;		// RA  : 0 = danh duoc luon
						//       1 = ky nang chi dung duoc DUOI ngua, da gui lenh xuong -> lan nay thoi
						//       2 = ky nang chi dung duoc TREN ngua, da gui lenh len   -> lan nay thoi
						//       3 = khong dung duoc (con han len/xuong ngua, hoac gioi han khac)
	int	nTamDanh;	// RA  : tam danh cua ky nang (dung de biet diem ngam nam dau khi keo)
	int	nLaAura;	// RA  : 1 = ky nang TRO (bat/tat) chu khong phai danh
	// [ANDROID 10/09 BUFF] them o CUOI de khong doi bo cuc phan tren
	int	nTuDung;	// RA  : 1 = ky nang TU DUNG (buff / hoi phuc len minh hoac dong doi, khong nham ke dich)
	char szTen[64];	// RA  : ten ky nang (ghi tren bang chon)
	int	nCanDiem;	// RA  : 1 = ky nang can mot DIEM (khinh cong...): khong nham ai ca (khong dich/minh/dong doi)
	int	nHuong;		// RA  : huong nhan vat dang quay (Npc.m_Dir 0..63: 0 xuong, 16 trai, 32 len, 48 phai)
	int	nTamCap;	// RA  : tam danh theo CAP DA HOC (ban tham khao R171: radius theo cap)
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
//	int nMin; //luu gia tri min cua opt
//	int nMax;  //luu gia tri max cua opt
	//KMagicAttrib() { nValue[0] = nValue[1] = nValue[2] = nAttribType = nMin = nMax = 0; };
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};

struct KMagicAutoSkill
{
	int				nSkillId;
	int				nSkillLevel;
	int				nRate;						//ÊôĞÔ²ÎÊı
	DWORD			dwNextCastTime;
	int				nWaitCastTime;
	int				nType;						// [HOASON 01/09e] byte cao nValue[0] (Linux): 1 = nham ke danh/muc tieu, khac = nham minh
	KMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = nType = 0;};
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
	JX_LONG  nRadius;					// ×÷ÓÃ°ë¾¶
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
	Obj_Kind_Obstacle,
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
#define		defTONG_ONE_PAGE_MAX_NUM			100
#define		defTONG_STR_LENGTH					32
#define		defTONG_NAME_MAX_LENGTH				12

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
