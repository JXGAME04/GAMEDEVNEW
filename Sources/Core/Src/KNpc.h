#ifndef KNpcH
#define KNpcH
//---------------------------------------------------------------------------
class ISkill;
#include "KCore.h"
#include "KSkillList.h"
#include "KMagicAttrib.h"
#include "GameDataDef.h"
#include "KNpcFindPath.h"
#include "KNpcDeathCalcExp.h"
#include "KIndexNode.h"
#include "KNpcGold.h"

class KSkill;
#ifndef _SERVER
#include "KNpcRes.h"
#include "KJXPathFinder.h"
#endif
//---------------------------------------------------------------------------
#ifdef	_SERVER
#define		MAX_NPC				50000 //max npc tai server 16384
#else
#define		MAX_NPC				256				//max npc tai client la 256
#endif
#define		MAX_NPCSTYLE	2022
#define		GAME_FPS						18
#define		MAX_AI_PARAM					11
#define		MAX_NPC_USE_SKILL				4
#define		STATE_FREEZE	0x0001
#define		STATE_POISON	0x0002
#define		STATE_STUN		0x0004
#define		STATE_HIDE		0x0008
#define		STATE_FROZEN	0x0010
#define		STATE_WALKRUN	0x0020

enum NPCATTRIB
{
	attrib_mana_v,
	attrib_stamina_v,
	attrib_life_v,
	attrib_mana_p,
	attrib_stamina_p,
	attrib_life_p,
};

enum NPCCMD
{
	do_none,		// Ê²Ã´Ò²²»¸É
	do_stand,		// Õ¾Á¢
	do_walk,		// ÐÐ×ß
	do_run,			// ÅÜ¶¯
	do_jump,		// ÌøÔ¾
	do_skill,		// ·¢¼¼ÄÜµÄÃüÁî
	do_magic,		// Ê©·¨
	do_attack,		// ¹¥»÷
	do_sit,			// ´ò×ø
	do_hurt,		// ÊÜÉË
	do_death,		// ËÀÍö
	do_defense,		// ¸ñµ²
	do_idle,		// ´­Æø
	do_specialskill,// ¼¼ÄÜ¿ØÖÆ¶¯×÷
	do_special1,	// ÌØÊâ1
	do_special2,	// ÌØÊâ2
	do_special3,	// ÌØÊâ3
	do_blurmove,	// ÌØÊâ4
	do_runattack,
	do_manyattack,
	do_jumpattack,
	do_revive,
	do_goattack,
};

enum CLIENTACTION
{
	cdo_fightstand,
	cdo_stand,
	cdo_stand1,
	cdo_fightwalk,
	cdo_walk,
	cdo_fightrun,
	cdo_run,
	cdo_hurt,
	cdo_death,
	cdo_attack,
	cdo_attack1,
	cdo_magic,
	cdo_sit,
	cdo_jump,
	cdo_none,   
	cdo_count,
};

enum DAMAGE_TYPE
{
	damage_physics = 0,		// ÎïÀíÉËº¦
	damage_fire,			// »ðÑæÉËº¦
	damage_cold,			// ±ù¶³ÉËº¦
	damage_light,			// ÉÁµçÉËº¦
	damage_poison,			// ¶¾ËØÉËº¦
	damage_magic,			// ÎÞÊôÐÔÉËº¦
	damage_num,				// ÉËº¦ÀàÐÍÊýÄ¿
};

enum	enumDEATH_MODE
{
	enumDEATH_MODE_NPC_KILL = 0,		// ±»npcÉ±ËÀ
	enumDEATH_MODE_PLAYER_NO_PUNISH,	// ÇÐ´èÄ£Ê½±»Íæ¼ÒÉ±ËÀ
	enumDEATH_MODE_PLAYER_PUNISH,		// ±»Íæ¼ÒPKÖÂËÀ£¬¸ù¾ÝPKÖµ½øÐÐ³Í·£
	enumDEATH_MODE_PKBATTLE_PUNISH,		// ÀàÊ½ÓÚ¹úÕ½Ê±µÄ³Í·£´¦Àí
	enumDEATH_MODE_NUM,
};

typedef struct
{
	NPCCMD		CmdKind;		// ÃüÁîC
	int			Param_X;		// ²ÎÊýX
	int			Param_Y;		// ²ÎÊýY
	int			Param_Z;		// ²ÎÊýY
} NPC_COMMAND;

typedef struct
{
	int		nTotalFrame;
	int		nCurrentFrame;
} DOING_FRAME;

struct KState
{
	int	nMagicAttrib;
	int	nValue[2];
	int	nTime;
};

struct	KSyncPos
{
	DWORD	m_dwRegionID;
	int		m_nMapX;
	int		m_nMapY;
	int		m_nOffX;
	int		m_nOffY;
	int		m_nDoing;
};

class KStateNode : public KNode
{
public:

	int				m_SkillID;					// ¼¼ÄÜID
	int				m_Level;					// ¼¼ÄÜµÈ¼¶
	int				m_LeftTime;					// Ê£ÓàÊ±¼ä
	int				m_LeftTimeH;
	BOOL			m_bOverLook;
	BOOL			m_bTempStateGraphics;
	KMagicAttrib	m_State[MAX_SKILL_STATE];	// ÐÞ¸ÄÊôÐÔÁÐ±í
	int				m_StateGraphics;			// ×´Ì¬¶¯»­Ë÷Òý
};

#ifndef _SERVER

struct	KClientNpcID
{
	DWORD	m_dwRegionID;
	int		m_nNo;
};
#endif

#ifdef _SERVER
struct KItemDropRate
{
	struct	KItemParam
	{
		int		nGenre;
		int		nDetailType;
		int		nParticulType;
		int		nRate;
	};
	int			nCount;
	int			nMagicRate;
	int			nMaxRandRate;
	int			nMoneyRate;
	int			nMoneyScale;
	int			nMinItemLevelScale;
	int			nMaxItemLevelScale;
	int			nMinItemLevel;
	int			nMaxItemLevel;
	KItemParam*	pItemParam;
};
#endif

class KNpc
{
	friend class KNpcSet;
public:

	DWORD				m_dwID;					// NpcµÄID
	int							m_Index;				// NpcµÄË÷Òý
	KIndexNode			m_Node;					// Npc's Node
	int							m_Level;				// NpcµÄµÈ¼¶
	DWORD				m_Kind;					// NpcµÄÀàÐÍ
	//char					  DescName[32];			//edit by phong kieu ngu hanh quai khai bao bien DescName
	int							m_Series;				// NpcµÄÏµ
	int							m_Type;				// NpcµÄÏµ
	int							m_Height;				// NpcµÄ¸ß¶È(ÌøÔ¾µÄÊ±ºò·ÇÁã)
	BYTE				m_btRankId;
	DWORD				m_btRankBattleId; //#RankBattle
	DWORD				m_btPlayerTitle;	//#PlayerTitle
	char				MateName[32];//#MateName
	int					nRankInWorld;			//edit by phong kieu RankWorld nguoi choi khac
	int					nRepute;//danh vong
	int					nFuYuan;//phuc duyen
	BYTE				nPKValue;
	BYTE				m_ImagePlayer;
	BYTE				nReBorn;
	BYTE				nFirstFaction;
	int					m_nStature;				//Tall 
	int					m_Recruit;
	int					m_nNpcTimeout;
	int					m_nNpcParam[MAX_NPCPARAM];
	BOOL				m_bNpcFollowFindPath;
	DWORD				m_uFindPathTime;
	DWORD				m_uFindPathMaxTime;
	DWORD				m_uLastFindPathTime;

	BYTE				m_btStateInfo[MAX_NPC_RECORDER_STATE];	 
	BOOL				m_bHaveLoadedFromTemplate;
	KState				m_PowerState;			
	KState				m_PoisonState;			
	KState				m_FreezeState;			
	KState				m_BurnState;			
	KState				m_StunState;			
	KState				m_FrozenAction;			
	KState				m_LifeState;			
	KState				m_ManaState;			
	KState				m_MenuState;			
	KState				m_DrunkState;			
	KState				m_HideState;
	KState				m_SilentState;
	KState				m_RandMove;			
	KState				m_WalkRun;
	KState				m_LoseMana;
	KState				m_PhysicsArmor;
	KState				m_ColdArmor;
	KState				m_LightArmor;
	KState				m_PoisonArmor;
	KState				m_FireArmor;
	KState				m_ManaShield;							//#noi luc ho than
	//int					m_RideState;
	int					  m_nProtectedTime;					//vong tron bat tu, vßng trßn bÊt tö
	void				SetProtectTime(int nTime){ m_nProtectedTime = nTime; };
	int					  GetProtectTime(){ return m_nProtectedTime; }
	KList				m_StateSkillList;		//Danh sach hieu ung skill tren nguoi cua NPC
	int					m_Camp;					// Npc
	int					m_CurrentCamp;			// Npc
	NPCCMD				m_Doing;				// Npc
	CLIENTACTION		m_ClientDoing;			// Npc
	DOING_FRAME			m_Frames;				// Npc
	KSkillList			m_SkillList;			// Npc
	int					m_SubWorldIndex;		// NpcËùÔÚµÄSubWorld ID
	int					m_RegionIndex;			// NpcËùÔÚµÄRegion ID
	int					m_ActiveSkillID;		// Npc¼¤»îµÄ¼¼ÄÜID
	int					m_TimeHorse;			//edit by phong kieu len xuong ngua
	int					m_TimeSWPK;			//edit by phong kieu pk time
	int					m_ActiveAuraID;			// Npc¼¤»îµÄ¹â»·¼¼ÄÜID
	int					m_TmpAuraID[5];
	// 
	int					m_CurrentExperience;	// Npc±»É±ºóËÍ³öµÄ¾­Ñé
	int					m_CurrentLife;			// NpcµÄµ±Ç°ÉúÃü
	int					m_CurrentLifeMax;		// NpcµÄµ±Ç°ÉúÃü×î´óÖµ
	int					m_CurrentLifeReplenish;	// NpcµÄµ±Ç°ÉúÃü»Ø¸´ËÙ¶È
	int					m_CurrentLifeReplenishPercent;
	int					m_CurrentMana;			// NpcµÄµ±Ç°ÄÚÁ¦
	int					m_CurrentManaMax;		// NpcµÄµ±Ç°×î´óÄÚÁ¦
	int					m_CurrentManaReplenish;	// NpcµÄµ±Ç°ÄÚÁ¦»Ø¸´ËÙ¶È
	int					m_CurrentStamina;		// NpcµÄµ±Ç°ÌåÁ¦
	int					m_CurrentStaminaMax;	// NpcµÄµ±Ç°×î´óÌåÁ¦
	int					m_CurrentStaminaGain;	// NpcµÄµ±Ç°ÌåÁ¦»Ø¸´ËÙ¶È
	int					m_CurrentStaminaLoss;	// NpcµÄµ±Ç°ÌåÁ¦ÏÂ½µËÙ¶È
	int					m_CurrentAttackRating;	// NpcµÄµ±Ç°ÃüÖÐÂÊ
	int					m_CurrentDefend;		// NpcµÄµ±Ç°·ÀÓù
	//
	KMagicAttrib		m_PhysicsDamage;		// NpcµÄµ±Ç°ÉËº¦(ÓÉÁ¦Á¿Ãô½ÝÓëÎäÆ÷ÉËº¦¾ö¶¨£¬²»¿¼ÂÇÖ±½Ó¼ÓÉËº¦µÄÄ§·¨ÊôÐÔ)
	KMagicAttrib		m_CurrentPoisonDamage;	// NpcµÄµ±Ç°¶¾ÉËº¦
	KMagicAttrib		m_CurrentColdDamage;	// NpcµÄµ±Ç°±ùÉËº¦
	KMagicAttrib		m_CurrentFireDamage;	// NpcµÄµ±Ç°»ðÉËº¦
	KMagicAttrib		m_CurrentLightDamage;	// NpcµÄµ±Ç°µçÉËº¦
	//
	KMagicAttrib		m_PhysicsMagic;					//Noi cong
	KMagicAttrib		m_CurrentPoisonMagic;
	KMagicAttrib		m_CurrentColdMagic;
	KMagicAttrib		m_CurrentFireMagic;
	KMagicAttrib		m_CurrentLightMagic;
	//
	int					m_CurrentFireResist;	// NpcµÄµ±Ç°»ð¿¹ÐÔ
	int					m_CurrentColdResist;	// NpcµÄµ±Ç°±ù¿¹ÐÔ
	int					m_CurrentPoisonResist;	// NpcµÄµ±Ç°¶¾¿¹ÐÔ
	int					m_CurrentLightResist;	// NpcµÄµ±Ç°µç¿¹ÐÔ
	int					m_CurrentPhysicsResist;	// NpcµÄµ±Ç°ÎïÀí¿¹ÐÔ
	int					m_CurrentFireResistMax;		// NpcµÄµ±Ç°×î´ó»ð¿¹ÐÔ
	int					m_CurrentColdResistMax;		// NpcµÄµ±Ç°×î´ó±ù¿¹ÐÔ
	int					m_CurrentPoisonResistMax;	// NpcµÄµ±Ç°×î´ó¶¾¿¹ÐÔ
	int					m_CurrentLightResistMax;	// NpcµÄµ±Ç°×î´óµç¿¹ÐÔ
	int					m_CurrentPhysicsResistMax;	// NpcµÄµ±Ç°×î´óÎïÀí¿¹ÐÔ
	//
	float					m_CurrentWalkSpeed;		// NpcµÄµ±Ç°×ß¶¯ËÙ¶È
	float					m_CurrentRunSpeed;		// NpcµÄµ±Ç°ÅÜ¶¯ËÙ¶È
	int					m_CurrentJumpSpeed;		// NpcµÄµ±Ç°ÌøÔ¾ËÙ¶È
	int					m_CurrentJumpFrame;		// NpcµÄµ±Ç°ÌøÔ¾Ö¡Êý
	int					m_CurrentAttackSpeed;	// NpcµÄµ±Ç°¹¥»÷ËÙ¶È
	int					m_CurrentCastSpeed;		// NpcµÄµ±Ç°Ê©·¨ËÙ¶È
	int					m_CurrentVisionRadius;	// NpcµÄµ±Ç°ÊÓÒ°·¶Î§
	int					m_CurrentAttackRadius;	// NpcµÄµ±Ç°¹¥»÷·¶Î§
	int					m_CurrentActiveRadius;	// NpcµÄµ±Ç°»î¶¯·¶Î§	//B¸n kÝnh ho¹t ®éng
	int					m_CurrentHitRecover;	// Thêi gian phôc håi
	int					m_CurrentTreasure;		// Npc¶ªÂä×°±¸µÄÊýÁ¿
	//
	int					m_CurrentMeleeDmgRetPercent;	// Npc½ü³ÌÉËº¦·µ»ØµÄ°Ù·Ö±È
	int					m_CurrentMeleeDmgRet;			// Npc½ü³Ì±»»÷Ê±·µ»ØµÄÉËº¦µãÊý
	int					m_CurrentRangeDmgRetPercent;	// NpcÔ¶³ÌÉËº¦·µ»ØµÄ°Ù·Ö±È
	int					m_CurrentRangeDmgRet;			// NpcÔ¶³Ì±»»÷Ê±·µ»ØµÄÉËº¦µãÊý
	int					m_CurrentReturnResPercent;
	BOOL				m_CurrentSlowMissle;			// NpcÊÇ·ñÔÚÂýËÙ×Óµ¯×´Ì¬ÏÂ
	int					m_CurrentDamageReduce;			// ÎïÀíÉËº¦¼õÉÙ
	int					m_CurrentElementDamageReduce;	// ÔªËØÉËº¦¼õÉÙ
	int					m_CurrentDamage2Mana;			// ÉËº¦×ªÄÚÁ¦°Ù·Ö±È
	int					m_CurrentManaPerEnemy;			// É±Ò»¸öµÐÈË¼Ó¶àÉÙµãÄÚÁ¦
	int					m_CurrentLifeStolen;			// ÍµÉúÃü°Ù·Ö±È	//sinh luc
	int					m_CurrentManaStolen;			// ÍµÄÚÁ¦°Ù·Ö±È	//noi luc
	int					m_CurrentStaminaStolen;			// ÍµÌåÁ¦°Ù·Ö±È	//the luc
	int					m_CurrentDeadlyStrikeEnhanceP;			// ÖÂÃüÒ»»÷°Ù·Ö±È
	int					m_CurrentFatallyStrikeEnhanceP;			// ÖÂÃüÒ»»÷°Ù·Ö±È
	int					m_CurrentFatallyStrikeResP;
	//
	int					m_CurrentBlindEnemy;			// ÖÂÃ¤µÐÈË°Ù·Ö±È
	int					m_CurrentPiercePercent;			// ´©Í¸¹¥»÷°Ù·Ö±È
	int					m_CurrentFreezeTimeReducePercent;	// ±ù¶³Ê±¼ä¼õÉÙ°Ù·Ö±È //Thêi gian lµm chËm gi¶m bít
	int					m_CurrentPoisonTimeReducePercent;	// ÖÐ¶¾Ê±¼ä¼õÉÙ°Ù·Ö±È
	int					m_CurrentStunTimeReducePercent;		// Ñ£ÔÎÊ±¼ä¼õÉÙ°Ù·Ö±È
	int					m_CurrentReturnSkillPercent;			//viet them skill hoang tvt
	int					m_CurrentIgnoreSkillPercent;
	int 				m_CurrentPoisonDamageReturn;
	int 				m_CurrentPoisonDamageReturnPercent;
	KMagicAutoSkill 	m_ReplySkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_RescueSkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_AttackSkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_DeathSkill[MAX_AUTOSKILL];
	int					m_CurrentIgnoreNegativeStateP;
	int					m_CurrentFireEnhance;			// »ð¼ÓÇ¿
	int					m_CurrentColdEnhance;			// ±ù¼ÓÇ¿
	int					m_CurrentPoisonEnhance;			// ¶¾¼ÓÇ¿
	int					m_CurrentLightEnhance;			// µç¼ÓÇ¿
	int					m_CurrentAddPhysicsDamage;		// Ö±½ÓµÄÎïÀíÉËº¦¼ÓÇ¿µãÊý
	int					m_CurrentAddPhysicsMagic;		// Sat thuong vat ly noi cong
	int					m_CurrentMeleeEnhance[MAX_MELEE_WEAPON+1];	// ½ü³ÌÎïÀí¼ÓÇ¿
	int					m_CurrentRangeEnhance;			// Ô¶³ÌÎïÀí¼ÓÇ¿
	int					m_CurrentHandEnhance;			// ¿ÕÊÖÎïÀí¼ÓÇ¿
	int					m_CurrentSerisesEnhance;
	int			 		m_CurrentManaShield;								//#giam thieu sat thuong ganh chiu HTVC con lon
	int			 		m_CurrentStaticMagicShieldP;
	int					m_CurrentExpEnhance;
	int					m_CurrentSkillEnhancePercent;
	int					m_CurrentFiveElementsEnhance;
	int					m_CurrentFiveElementsResist;
	int					m_CurrentManaToSkillEnhanceP;					//#khi noi cong day tang ky nang cong kich
	int					m_CurrentSorbDamageP;								//#triet tieu sat thuong
	//
	int					m_Dir;							// NpcµÄ·½Ïò
	int					m_RedLum;						// NpcµÄÁÁ¶È
	int					m_GreenLum;
	int					m_BlueLum;
	int					m_MapX, m_MapY, m_MapZ;			// NpcµÄµØÍ¼×ø±ê
	int					m_OffX, m_OffY;					// NpcÔÚ¸ñ×ÓÖÐµÄÆ«ÒÆ×ø±ê£¨·Å´óÁË1024±¶£©
	int					m_DesX, m_DesY;					// NpcµÄÄ¿±ê×ø±ê
	int					m_SkillParam1, m_SkillParam2;
	int					m_OriginX, m_OriginY;			// NpcµÄÔ­Ê¼×ø±ê
	int					m_NextAITime;
	BYTE				m_AIMAXTime;//NpcAI

#ifndef _SERVER
	std::vector<FindPathNode> m_PathFind;
	FindPathNode				m_AutoMoveTemp;
	int AutoMoveStuckCount;
#endif


	// Npc
	int					m_HelmType;					// NpcµÄÍ·¿øÀàÐÍ
	int					m_ArmorType;				// NpcµÄ¿ø¼×ÀàÐÍ
	int					m_WeaponType;				// NpcµÄÎäÆ÷ÀàÐÍ
	int					m_HorseType;				// NpcµÄÆïÂíÀàÐÍ
	BOOL				m_bRideHorse;				// NpcÊÇ·ñÆïÂí
	BOOL				m_RuongLock;				// Edit by phong kieu trang thai mo khoa ruong
	int					m_BaiTan;					// 
	char				ShopName[32];
	int					m_MaskType;					//#mat na
	int					m_MaskMark;					//#mat na
	BYTE				m_btHonorId;			//#danh hieu
#ifndef _SERVER
	char				m_szTeamMem[64]; //hiÓn thÞ sè ng­êi trong nhãm sö dông paint th«ng t×n tõ auto lªn nh©n vËt
	void				SetSzTeamMem(char* s)
	{
		strcpy(m_szTeamMem, s);
	};
	void				ClearSzTeamMem()
	{
		memset(m_szTeamMem, 0, sizeof(m_szTeamMem));
	};
	int					m_nTimeAbsent; //thêi gian v¾ng mÆt sö dông trong auto
#endif
	char				m_szTongName[32];
	char				m_szTongTitle[32];
	int					m_nFigure;
	char				Name[32];				// NpcµÄÃû³Æ
	char				Owner[32];				//add by phong kiÒu 19/08/2021
	int					m_nSex;					// NpcµÄÐÔ±ð0ÎªÄÐ£¬1ÎªÅ®
	int					m_NpcSettingIdx;		// NpcµÄÉè¶¨ÎÄ¼þË÷Òý
	int					m_CorpseSettingIdx;		// NpcµÄÊ¬Ìå¶¨ÒåË÷Òý
	char				ActionScript[80];		// NpcµÄÐÐÎª½Å±¾
	DWORD				m_ActionScriptID;		// NpcµÄÐÐÎª½Å±¾ID£¨Ê¹ÓÃÊ±ÓÃÕâ¸öÀ´¼ìË÷£©
	char				DropRateScript[64];
	DWORD				m_DropRateScriptID;
	BYTE				m_nPKFlag;
	int						m_nMissionGroup;
	int					m_MantleType;		//#phi phong
	DWORD				m_TrapScriptID;			// NpcµÄµ±Ç°Trap½Å±¾ID;
	int					m_nPeopleIdx;			// ¶ÔÏóÈËÎï
	int					m_nLastDamageIdx;		// ×îºóÒ»´ÎÉËº¦µÄÈËÎïË÷Òý
	int					m_nLastPoisonDamageIdx;	// ×îºóÒ»´Î¶¾ÉËº¦µÄÈËÎïË÷Òý
	int					m_nObjectIdx;			// ¶ÔÏóÎïÆ·
	//
	int					m_Experience;			
	int					m_LifeMax;				
	int					m_LifeReplenish;		// NpcµÄÉúÃü»Ø¸´ËÙ¶È
	int					m_ManaMax;				// NpcµÄ×î´óÄÚÁ¦
	int					m_ManaReplenish;		// NpcµÄÄÚÁ¦»Ø¸´ËÙ¶È
	int					m_StaminaMax;			// NpcµÄ×î´óÌåÁ¦
	int					m_StaminaGain;			// NpcµÄÌåÁ¦»Ø¸´ËÙ¶È
	int					m_StaminaLoss;			// NpcµÄÌåÁ¦ÏÂ½µËÙ¶È
	int					m_AttackRating;			// NpcµÄÃüÖÐÂÊ
	int					m_Defend;				// NpcµÄ·ÀÓù
	int					m_FireResist;			// NpcµÄ»ð¿¹ÐÔ
	int					m_ColdResist;			// NpcµÄÀä¿¹ÐÔ
	int					m_PoisonResist;			// NpcµÄ¶¾¿¹ÐÔ
	int					m_LightResist;			// NpcµÄµç¿¹ÐÔ
	int					m_PhysicsResist;		// NpcµÄÎïÀí¿¹ÐÔ
	int					m_FireResistMax;		// NpcµÄ×î´ó»ð¿¹ÐÔ
	int					m_ColdResistMax;		// NpcµÄ×î´ó±ù¿¹ÐÔ
	int					m_PoisonResistMax;		// NpcµÄ×î´ó¶¾¿¹ÐÔ
	int					m_LightResistMax;		// NpcµÄ×î´óµç¿¹ÐÔ
	int					m_PhysicsResistMax;		// NpcµÄ×î´óÎïÀí¿¹ÐÔ
	int					m_WalkSpeed;			// NpcµÄÐÐ×ßËÙ¶È
	int					m_RunSpeed;				// NpcµÄÅÜ¶¯ËÙ¶È
	int					m_JumpSpeed;			// NpcµÄÌøÔ¾ËÙ¶È
	int					m_AttackSpeed;			// NpcµÄ¹¥»÷ËÙ¶È
	int					m_CastSpeed;			// NpcµÄÊ©·¨ËÙ¶È
	int					m_VisionRadius;			// NpcµÄÊÓÒ°·¶Î§ //B¸n kÝnh tÇm nh×n
	int					m_DialogRadius;			// NpcµÄ¶Ô»°·¶Î§
	int					m_ActiveRadius;			// NpcµÄ»î¶¯·¶Î§ //B¸n kÝnh ho¹t ®éng
	int					m_HitRecover;			// NpcµÄÊÜ»÷»Ø¸´ËÙ¶È
	int					m_Treasure;				// Npc¶ªÂä×°±¸µÄÊýÁ¿
	BOOL				m_bClientOnly;			
	//
	int					m_nCurrentMeleeSkill;	// 
	int					m_nCurrentMeleeTime;	
	int					m_AiMode;				// 
	int					m_AiParam[MAX_AI_PARAM];// 
	int					m_AiAddLifeTime;
	int					m_HeadImage;
	int					m_FightMode;			// 
	int					m_OldFightMode;
	BOOL				m_bExchangeServer;
	BYTE				m_ExItemId; // hanh trang
	BYTE				m_ExBoxId; // ruong mo rong
	bool				m_bWaitingPlayerFeedBack;
	BYTE				m_btTryExecuteScriptTimes;

#ifdef _SERVER
	int					m_AiSkillRadiusLoadFlag;//
	KNpcDeathCalcExp	m_cDeathCalcExp;		// 
	KNpcGold			m_cGold;
	KItemDropRate*		m_pDropRate;
	int					m_nCurPKPunishState;	// 
	//
	int					m_nTimeIdleValue;
	int					m_nTimeIdleCounter;
	int					m_nTime_Ignorenegativestate;	//3 gi©y lo¹i bá tr¹ng th¸i dÞ th­êng xuÊt ø bÊt diÔm
#endif

#ifndef	_SERVER
	DWORD					m_SyncSignal;			// 
	KClientNpcID		m_sClientNpcID;			// 
	DWORD				m_dwRegionID;			// 
	KSyncPos			m_sSyncPos;
	char				m_szChatBuffer[MAX_SENTENCE_LENGTH];
	int					m_nChatContentLen;
	int					m_nChatNumLine;
	int					m_nChatFontWidth;
	unsigned int		m_nCurChatTime;
	int					m_nSleepFlag;
	int					m_nHurtHeight;
	int					m_nHurtDesX;
	int					m_nHurtDesY;
	int					m_nTongFlag;			// 
#endif
private:
	int					m_LoopFrames;			// 
	int					m_nPlayerIdx;
	int					m_DeathFrame;			// 
	int					m_StandFrame;
	int					m_HurtFrame;
	int					m_AttackFrame;
	int					m_CastFrame;
	int					m_WalkFrame;
	int					m_RunFrame;
	int					m_StandFrame1;
	int					m_ReviveFrame;			//  thêi gian håi sinh
	int					m_SitFrame;
	int					m_JumpFrame;
	int					m_JumpFirstSpeed;
	NPC_COMMAND			m_Command;				// 
	BOOL				m_ProcessAI;			// 
	BOOL				m_ProcessState;			// 
	int					m_XFactor;
	int					m_YFactor;
	int					m_JumpStep;
	int					m_JumpDir;
	int					m_SpecialSkillStep;		// 
	NPC_COMMAND			m_SpecialSkillCommand;	// 
	KNpcFindPath		m_PathFinder;
	BOOL				m_bActivateFlag;	

#ifndef	_SERVER
	int					m_ResDir;
	KNpcRes				m_DataRes;
	int					m_nBloodNo[5];
	int					m_nBloodAlpha[5];
	int					m_nBloodTime[5];
	char				m_szBloodNo[5][32];
public:
	void			FkAutoSetBlur(BOOL b)
	{
		m_DataRes.SetBlur(b);
	};
	//
#endif

private:
	BOOL				WaitForFrame();
	BOOL				IsReachFrame(int nPercent);
	void				DoStand();
	void				OnStand();
	void				DoRevive();
	void				OnRevive();
	void				DoWait();
	void				OnWait();
	void				DoWalk();
	void				OnWalk();
	void				DoRun();
	void				OnRun();
	void				DoSkill(int nX, int nY);
	int					DoOrdinSkill(KSkill * pSkill, int nX, int nY);
	int					DoOrdinSkill2(KSkill * pSkill, int nX, int nY);
	void				OnSkill();
	void				DoAttack();
	void				DoJump();
	BOOL				OnJump();
	void				DoSit();
	void				OnSit();
	void				DoHurt(int nHurtFrames = 0, int nX = 0, int nY =0, int nHurtI =0);
	void				OnHurt();
	// mode == 0 npc  == 1 player  == 2 player 
	//  DeathPunish  enumDEATH_MODE
	void				DoDeath(int nMode = 0, int nAttacker = 0);
	void				OnDeath();
	void				DoDefense();
	void				OnDefense();
	void				DoIdle();
	void				OnIdle();
//	------------------------------------------
	BOOL				DoManyAttack();
	void				OnManyAttack();
	BOOL				DoBlurAttack();
	BOOL				DoJumpAttack();
	BOOL				OnJumpAttack();
	BOOL				DoRunAttack();
	void				OnRunAttack();
	BOOL				CastMeleeSkill(KSkill * pSkill);
//-----------------------------------------------------------
	void				OnPlayerTalk();
	void				DoSpecial1();
	void				OnSpecial1();
	void				DoSpecial2();
	void				OnSpecial2();
	void				DoSpecial3();
	void				OnSpecial3();
	BOOL				DoBlurMove();
	void				OnBlurMove();
	void				Goto(int nMpsX, int nMpsY);
	void				RunTo(int nMpsX, int nMpsY);
	void				JumpTo(int nMpsX, int nMpsY);
	void				ServeMove(int nSpeed);
	void				ServeJump(int nSpeed);
//	void				Enchant(void);
//	void				ModifyEnchant(void* pData, int nAttrib);
	BOOL				NewPath(int nMpsX, int nMpsY);
	BOOL				NewJump(int nMpsX, int nMpsY);
	VOID				FixPos();	// ÐÞÕý×ø±êÊ¹m_OffX,m_OffY²»ÁôÐ¡Êý£¨ËõÐ¡»Ø1024±¶ºóµÄÐ¡Êý£©
	BOOL				CheckHitTarget(int nAR, int nDf, int nIngore = 0);	//§é chÝnh x¸c
	BOOL				CheckHitTarget2(int nAR, int nDf, int nIngore = 0);	//§é chÝnh x¸c
	void				AddStateSkillList(KStateNode * pNewNode);
#ifdef _SERVER
	void				LoseSingleItem(int nBelongPlayer);
	void				LoseMoney(int nBelongPlayer);
	void				PlayerDeadCreateMoneyObj(int nMoneyNum);	
	void				UpdateNpcStateInfo();
	void				AddStateInfo(int nID);
#endif

#ifndef _SERVER
	void				HurtAutoMove();
	void				AutoFixXY();
#endif

public:
	friend class KNpcAttribModify;
	friend class KThiefSkill;
	KNpc();
	inline	void		SetProcessAI(BOOL bVal)
	{
		m_ProcessAI = bVal;
	}
	int					GetMapX(void) const {return m_MapX;};
	int					GetMapY(void) const {return m_MapY;};
	int					GetMapZ(void) const {return m_MapZ;};
	int					GetOffX(void) const {return m_OffX;};
	int					GetOffY(void) const {return m_OffY;};	
	void				SetActiveFlag(BOOL bFlag) { m_bActivateFlag = bFlag; };
	void				DoPlayerTalk(char *);
	void				CheckTrap();
	void				Init();
	void				Remove();
	void				Activate();
	BOOL				IsPlayer();
	BOOL				IsAlive();
	void				SetFightMode(BOOL bFightMode);
	void				TurnTo(int nIdx);
	void				SendCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				ProcCommand(int nAI);
	ISkill* 			GetActiveSkill();
	ISkill* 			GetActiveSkillAura();
	BOOL				ProcessState();
	void				ProcStatus();
	void				ModifyAttrib(int nAttacker, void* pData);
	void				SetId(DWORD	dwID)	{ m_dwID = dwID;};
	BOOL				IsMatch(DWORD dwID)	{ return dwID == m_dwID; };	
	BOOL				Cost(NPCATTRIB nType, int nCost, BOOL bOnlyCheckCanCast = FALSE);				//OnlyCheckCanCost TRUE
	void				Load(int nNpcSettingIdx, int nLevel, int nSeries);						//fix by phong kiÒu
	void				GetMpsPos(int * pPosX, int *pPosY);
#ifndef _SERVER
	VOID				ClientGotoPos(INT nX, INT nY, INT nMode = 0);	// nMode: 0.Auto; 1.Walk; 2.Run
#endif
	BOOL				SetActiveSkill(int nSkillIdx);
	void				SetAuraSkill(int nSkillID);
	void				SetCamp(int nCamp);
	void				SetCurrentCamp(int nCamp);
	void				RestoreCurrentCamp();
	void  				SetPlayerTitle(DWORD nRankId, DWORD nTime, DWORD overLook);//#PlayerTitle
	void				SetCurPlayerTitle(DWORD nRankId,DWORD overLook);//#PlayerTitle
	void				SetRankBattle(DWORD nRankId, int nTime, int overLook);//#RankBattle
	void				SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime = -1, BOOL bOverLook = FALSE);	// Ö÷¶¯¸¨Öú¼¼ÄÜÓë±»¶¯¼¼ÄÜ
	void				ClearStateSkillEffect();
	void				IgnoreState(BOOL bNegative);
	void				ReCalcStateEffect();
	void				ClearNormalState();
	void				SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum);
	void				AppendSkillEffect(int nSkillID, BOOL bIsPhysical, BOOL bIsMelee, void *pSrcData, void *pDesData);
	int					ModifyMissleLifeTime(int nLifeTime);
	int					ModifyMissleSpeed(int nSpeed);
	BOOL				ModifyMissleCollsion(BOOL bCollsion);
	void				RestoreNpcBaseInfo(); //Set Current_Data ;
	void				RestoreState();
	void				ClearNpcState();
	BOOL				SetPlayerIdx(int nIdx);
	void				DialogNpc(int nIndex);
	void				Revive();
	void				AddBaseLifeMax(int nLife);	
	void				SetBaseLifeMax(int nLifeMax); 
	void				AddCurLifeMax(int nLife);
	void				AddBaseStaminaMax(int nStamina);
	void				SetBaseStaminaMax(int nStamina);
	void				AddCurStaminaMax(int nStamina);
	void				SetBaseManaMax(int nMana);
	void				AddBaseManaMax(int nMana);	
	void				AddCurManaMax(int nMana);
	void				CalcCurLifeReplenish();		
	void				SetSeries(int nSeries);
	void				GetNpcCopyFromTemplate(int nNpcTemplateId,int nLevel, int nSeries);
	void				SetPhysicsDamage(int nMinDamage, int nMaxDamage);	
	void				SetReviveFrame(int nReviveFrame)	//Thêi gian håi sinh
	{
		m_ReviveFrame = nReviveFrame;
	};
	void				SetBaseAttackRating(int nAttackRating);					
	void				SetBaseDefence(int nDefence);							
//	void				SetBaseWalkSpeed(int nSpeed);							
//	void				SetBaseRunSpeed(int nSpeed);							
	int					GetCurActiveWeaponSkill();
	void				LoadDataFromTemplate(int nNpcTemplateId, int nLevel, int nSeries);
	void				ReSetRes(int nMark);

	BOOL				ExecuteScript(char * ScriptFileName, char * szFunName, int nParam = 0);		// string - num
	BOOL				ExecuteScript(char * ScriptFileName, char * szFunName, char * szParams);	// string - char
	BOOL				ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams);		// id - char
	BOOL				ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam);				// id - num

	BOOL				ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1 = 0, int nParam2 = 0);
	BOOL				ExecuteScript2(char * ScriptFileName, char * szFunName, char * szParams1, char * szParams2);
	BOOL				ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2);
	BOOL				ExecuteScript2(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2);

	inline int			   GetSubWorldIndex() {return m_SubWorldIndex;} //#lua drop item
#ifdef	_SERVER
	int					  DropItemFromLuaScript(int nBelongPlayer, int nGenre, int nDetail, int nParticular, int nSeries, int nLevel, int nLuck, int* pnMagicLevel, int nTimeBelong);
	int					  UpdateDBStateList(BYTE *);
	void				ExecuteRevive(){DoRevive();};
	BOOL			SendSyncData(int nClient);						
	void				NormalSync();									
	void				BroadCastRevive(int nType);
	int					  GetPlayerIdx();
	BOOL		    CalcDamage(int nAttacker, int nMin, int nMax, DAMAGE_TYPE nType, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, BOOL bReturn = FALSE , int nSeries_DamageP = 0, int nStole_Life = 0, int nStole_Mana = 0, int nStole_Stamina = 0, BOOL bIsDS = FALSE, BOOL bIsFS = FALSE);
	BOOL		    ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate);
	void			    Cast(int nSkillId, int nSkillLevel);
	void				ReplySkill();
	void				RescueSkill();
	void				AttackSkill(int nUnderAttacker);
	void				DeathSkill();
	// mode == 0 npc  == 1 player  == 2 player 	// DoDeath enumDEATH_MODE
	void				DeathPunish(int nMode, int nBelongPlayer);		//edit by phong kieu hut hit
	void				RestoreLiveData();								// ÖØÉúºó»Ö¸´NpcµÄ»ù±¾Êý¾Ý
	void                RestoreLiveData(int nDamage);					//edit by phong kieu hut hit
	void                RestoreDamage2Life(int nDamage);				//edit by phong kieu hut mau hut hit
	void                RestoreDamage2Mana(int nDamage);				//edit by phong kieu hut mana hut hit
	void				RestoreDamage2Stamina(int nDamage);			//edit by phong kieu hut noi luc
	int					 SetPos(int nX, int nY);
	int					 ChangeWorld(DWORD dwSubWorldID, int nX, int nY);	
	void				TobeExchangeServer(DWORD dwMapID, int nX, int nY);
	void				RestoreLife(){m_CurrentLife = m_CurrentLifeMax;	};
	void				RestoreMana(){m_CurrentMana = m_CurrentManaMax;};
	void				RestoreStamina(){m_CurrentStamina = m_CurrentStaminaMax; };
	void				SendDataToNearRegion(void* pBuffer, DWORD dwSize);
	int					DeathCalcPKValue(int nKiller);
	int					FindAroundPlayer(const char* Name);//add by phong kiÒu using vËn tiªu
	BOOL			CheckPlayerAround(int nPlayerIdx);
#endif

#ifndef _SERVER
	void				SetSleepMode(BOOL bSleep) { m_nSleepFlag = bSleep; m_DataRes.SetSleepState(bSleep);};
	void				SetNpcState(BYTE* pNpcState);
	void				RemoveRes();
	void				ProcNetCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				Paint();
	int					PaintInfo(int nHeightOffset, bool bSelect, int nFontSize = 12, DWORD	dwBorderColor = 0);
	void				PaintSeriesNpc(char* szName, int nFontSize, int nHeightOff);
	int					PaintChat(int nHeightOffset);
	int					SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength);
	int					PaintLife(int nHeightOffset, bool bSelect);
	int					PaintMana(int nHeightOffset);

	void				DrawBorder();
	int					DrawMenuState(int n);
	void				DrawBlood();	
	BOOL			IsCanInput() { return m_ProcessAI; };
	void				SetMenuState(int nState, char *lpszSentence = NULL, int nLength = 0);	
	int					GetMenuState();				
	DWORD		SearchAroundID(DWORD dwID);	
	void				SetSpecialSpr(char *lpszSprName);
	void				SetInstantSpr(int nNo);
	int					GetNormalNpcStandDir(int nFrame);
	KNpcRes*			GetNpcRes(){return &m_DataRes;};
	int						GetNpcPate();
	int						GetNpcPatePeopleInfo();
	void				ClearBlood(int i);
	void				SetBlood(int nNo);
	int					PaintBlood(int nHeightOffset);	
	BOOL				FindStateSkill(int nID);
	void				HideRes(int nType, bool bFlag);
	void				ChangeRecruit(int m_Recruit);
	int				GetRecruit(){return m_Recruit;};
#endif

	int			m_nTime;
	int			m_nTotalFrame;
	int			m_nFrame;
	
};
#ifndef TOOLVERSION
extern KNpc Npc[MAX_NPC];
#else
extern CORE_API KNpc Npc[MAX_NPC];
#endif

#endif

