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
#define		MAX_NPC				98000 //max npc tai server 16384
#else
#define		MAX_NPC				256				//max npc tai client la 256
#endif
#define		MAX_NPCSTYLE	3000
#define		GAME_FPS						18
#define		MAX_AI_PARAM					11
#define		MAX_NPC_USE_SKILL				4
#define		STATE_FREEZE	0x0001
#define		STATE_POISON	0x0002
#define		STATE_STUN		0x0004
#define		STATE_HIDE		0x0008
#define		STATE_FROZEN	0x0010
#define		STATE_WALKRUN	0x0020
#define MAX_POISON_DAMAGE 200000 //t鎛g dame c gi韎 h筺 
#define MAX_FIRE_DAMAGE 50000 //t鎛g dame hoa sat gi韎 h筺 

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
	do_none,		// 什么也不干
	do_stand,		// 站立
	do_walk,		// 行走
	do_run,			// 跑动
	do_jump,		// 跳跃
	do_skill,		// 发技能的命令
	do_magic,		// 施法
	do_attack,		// 攻击
	do_sit,			// 打坐
	do_hurt,		// 受伤
	do_death,		// 死亡
	do_defense,		// 格挡
	do_idle,		// 喘气
	do_specialskill,// 技能控制动作
	do_special1,	// 特殊1
	do_special2,	// 特殊2
	do_special3,	// 特殊3
	do_blurmove,	// 特殊4
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
	damage_physics = 0,		// 物理伤害
	damage_fire,			// 火焰伤害
	damage_cold,			// 冰冻伤害
	damage_light,			// 闪电伤害
	damage_poison,			// 毒素伤害
	damage_magic,			// 无属性伤害
	damage_num,				// 伤害类型数目
};

enum	enumDEATH_MODE
{
	enumDEATH_MODE_NPC_KILL = 0,		// 被npc杀死
	enumDEATH_MODE_PLAYER_NO_PUNISH,	// 切磋模式被玩家杀死
	enumDEATH_MODE_PLAYER_PUNISH,		// 被玩家PK致死，根据PK值进行惩罚
	enumDEATH_MODE_PKBATTLE_PUNISH,		// 类式于国战时的惩罚处理
	enumDEATH_MODE_NUM,
};

typedef struct
{
	NPCCMD		CmdKind;		// 命令C
	int			Param_X;		// 参数X
	int			Param_Y;		// 参数Y
	int			Param_Z;		// 参数Y
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

	KState() : nMagicAttrib(0), nValue{ 0, 0 }, nTime(0) {}
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

	int				m_SkillID;					// 技能ID
	int				m_Level;					// 技能等级
	int				m_LeftTime;					// 剩余时间
	int				m_LeftTimeH;
	BOOL			m_bOverLook;
	BOOL			m_bTempStateGraphics;
	KMagicAttrib	m_State[MAX_SKILL_STATE];	// 修改属性列表
	int				m_StateGraphics;			// 状态动画索引
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



#define MAX_PERCENTAGE 100
#define MIN_PERCENTAGE 0
#define EXTRA_WIDTH 2

struct Color {
    int r, g, b, a;
};

const Color RED_BAR = {205, 38, 38, 0};
const Color BLUE_BAR = {30, 144, 255, 0};
const Color GRAY_BAR = {128, 128, 128, 0};



class KNpc
{
	friend class KNpcSet;
public:
#ifdef _SERVER
    int m_nLastSyncMapX;
    int m_nLastSyncMapY;
#endif
	DWORD				m_dwID;					// Npc的ID
	int							m_Index;				// Npc的索引
	KIndexNode			m_Node;					// Npc's Node
	int							m_Level;				// Npc的等级
	DWORD				m_Kind;					// Npc的类型
	//char					  DescName[32];			//edit by phong kieu ngu hanh quai khai bao bien DescName
	int							m_Series;				// Npc的系
	int							m_Type;				// Npc的系
	int							m_Height;				// Npc的高度(跳跃的时候非零)
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
	BYTE				m_byMantleLevel;
	int					m_nStature;				//Tall 
	char				m_szGameTitle[64];		// Game title (e.g., "Faction Name - Title")
	int					m_Recruit;
	int					m_nNpcTimeout;
	int					m_nNpcParam[MAX_NPCPARAM];
	BOOL				m_bNpcFollowFindPath;
	BYTE				m_btSimCityBot;		// Port SimCity: NPC la bot gia lap (server set; client luon 0)
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
	int					  m_nProtectedTime;					//vong tron bat tu, v遪g tr遪 b蕋 t?
	void				SetProtectTime(int nTime){ m_nProtectedTime = nTime; };
	int					  GetProtectTime(){ return m_nProtectedTime; }
	KList				m_StateSkillList;		//Danh sach hieu ung skill tren nguoi cua NPC
	int					m_Camp;					// Npc
	int					m_CurrentCamp;			// Npc
	NPCCMD				m_Doing;				// Npc
	CLIENTACTION		m_ClientDoing;			// Npc
	DOING_FRAME			m_Frames;				// Npc
	KSkillList			m_SkillList;			// Npc
	int					m_SubWorldIndex;		// Npc所在的SubWorld ID
	int					m_RegionIndex;			// Npc所在的Region ID
	int					m_ActiveSkillID;		// Npc激活的技能ID
	int					m_TimeHorse;			//edit by phong kieu len xuong ngua
	int					m_TimeSWPK;			//edit by phong kieu pk time
	int					m_ActiveAuraID;			// Npc激活的光环技能ID
	int					m_TmpAuraID[5];
	// 
	int					m_CurrentExperience;	// Npc被杀后送出的经验
	int					m_CurrentLife;			// Npc的当前生命
	int					m_CurrentLifeMax;		// Npc的当前生命最大值
	int					m_CurrentLifeReplenish;	// Npc的当前生命回复速度
	int					m_CurrentLifeReplenishPercent;
	int					m_CurrentMana;			// Npc的当前内力
	int					m_CurrentManaMax;		// Npc的当前最大内力
	int					m_CurrentManaReplenish;	// Npc的当前内力回复速度
	int					m_CurrentStamina;		// Npc的当前体力
	int					m_CurrentStaminaMax;	// Npc的当前最大体力
	int					m_CurrentStaminaGain;	// Npc的当前体力回复速度
	int					m_CurrentStaminaLoss;	// Npc的当前体力下降速度
	int					m_CurrentAttackRating;	// Npc的当前命中率
	int					m_CurrentDefend;		// Npc的当前防御
	//
	KMagicAttrib		m_PhysicsDamage;		// Npc的当前伤害(由力量敏捷与武器伤害决定，不考虑直接加伤害的魔法属性)
	KMagicAttrib		m_CurrentPoisonDamage;	// Npc的当前毒伤害
	KMagicAttrib		m_CurrentColdDamage;	// Npc的当前冰伤害
	KMagicAttrib		m_CurrentFireDamage;	// Npc的当前火伤害
	KMagicAttrib		m_CurrentLightDamage;	// Npc的当前电伤害
	//
	KMagicAttrib		m_PhysicsMagic;					//Noi cong
	KMagicAttrib		m_CurrentPoisonMagic;
	KMagicAttrib		m_CurrentColdMagic;
	KMagicAttrib		m_CurrentFireMagic;
	KMagicAttrib		m_CurrentLightMagic;
	//
	int					m_CurrentFireResist;	// Npc的当前火抗性
	int					m_CurrentColdResist;	// Npc的当前冰抗性
	int					m_CurrentPoisonResist;	// Npc的当前毒抗性
	int					m_CurrentLightResist;	// Npc的当前电抗性
	int					m_CurrentPhysicsResist;	// Npc的当前物理抗性
	int					m_CurrentFireResistMax;		// Npc的当前最大火抗性
	int					m_CurrentColdResistMax;		// Npc的当前最大冰抗性
	int					m_CurrentPoisonResistMax;	// Npc的当前最大毒抗性
	int					m_CurrentLightResistMax;	// Npc的当前最大电抗性
	int					m_CurrentPhysicsResistMax;	// Npc的当前最大物理抗性
	//
	float				m_CurrentWalkSpeed;		// Npc的当前走动速度
	float				m_CurrentRunSpeed;		// Npc的当前跑动速度
	int					m_CurrentJumpSpeed;		// Npc的当前跳跃速度
	int					m_CurrentJumpFrame;		// Npc的当前跳跃帧数
	int					m_CurrentAttackSpeed;	// Npc的当前攻击速度
	int					m_CurrentCastSpeed;		// Npc的当前施法速度
	int					m_CurrentVisionRadius;	// Npc的当前视野范围
	int					m_CurrentAttackRadius;	// Npc的当前攻击范围
	int					m_CurrentActiveRadius;	// Npc的当前活动范围	//B竛 k輓h ho箃 ng
	int					m_CurrentHitRecover;	// Th阨 gian ph鬰 h錳
	int					m_CurrentTreasure;		// Npc丢落装备的数量
	//
	int					m_CurrentMeleeDmgRetPercent;	// Npc近程伤害返回的百分比
	int					m_CurrentMeleeDmgRet;			// Npc近程被击时返回的伤害点数
	int					m_CurrentRangeDmgRetPercent;	// Npc远程伤害返回的百分比
	int					m_CurrentRangeDmgRet;			// Npc远程被击时返回的伤害点数
	int					m_CurrentReturnResPercent;
	BOOL				m_CurrentSlowMissle;			// Npc是否在慢速子弹状态下
	int					m_CurrentDamageReduce;			// 物理伤害减少
	int					m_CurrentElementDamageReduce;	// 元素伤害减少
	int					m_CurrentDamage2Mana;			// 伤害转内力百分比
	int					m_CurrentManaPerEnemy;			// 杀一个敌人加多少点内力
	int					m_CurrentLifeStolen;			// 偷生命百分比	//sinh luc
	int					m_CurrentManaStolen;			// 偷内力百分比	//noi luc
	int					m_CurrentStaminaStolen;			// 偷体力百分比	//the luc
	int					m_CurrentDeadlyStrikeEnhanceP;			// 致命一击百分比
	int					m_CurrentFatallyStrikeEnhanceP;			// 致命一击百分比
	int					m_CurrentFatallyStrikeResP;
	//
	int					m_CurrentBlindEnemy;			// 致盲敌人百分比
	int					m_CurrentPiercePercent;			// 穿透攻击百分比
	int					m_CurrentFreezeTimeReducePercent;	// 冰冻时间减少百分比 //Th阨 gian l祄 ch薽 gi秏 b韙
	int					m_CurrentPoisonTimeReducePercent;	// 中毒时间减少百分比
	int					m_CurrentStunTimeReducePercent;		// 眩晕时间减少百分比
	int					m_CurrentReturnSkillPercent;			//viet them skill hoang tvt
	int					m_CurrentIgnoreSkillPercent;
	int 				m_CurrentPoisonDamageReturn;
	int 				m_CurrentPoisonDamageReturnPercent;
	KMagicAutoSkill 	m_ReplySkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_RescueSkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_AttackSkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_DeathSkill[MAX_AUTOSKILL];
	KMagicAutoSkill 	m_CastSkill[MAX_AUTOSKILL];	// [VHTD 02/09] autocastskill: tu phong theo chu ky (Linux +0x182c)
	struct { int nKey; int nCount; int nMax; } m_HSSp[MAX_HS_SP];	// [VHTD 02/09] bo dem tang No (1976) / Am Luat (2116)
	int					m_nHSLockLife;			// [VHTD 02/09] lock_life gia tri
	int					m_nHSLockLifeMode;		// [VHTD 02/09] lock_life che do (1 = khong thap hon)
	BOOL				m_bHSInvincible;		// [VHTD 02/09] invincibility
	BOOL				m_bHSForbidAttack;		// [VHTD 02/09] forbit_attack
	int					m_nHSAddLightMagicP;	// [VHTD 02/09] addlightingmagic_p
	int					m_nHSMeleeReturnResP;	// [VHTD 02/09] melee_returnres_p
	int					m_nHSUnravel;			// [VHTD 02/09] unravel_effect (chua co co che)
	int					m_nHSShieldMax;		// [VHTD 02/09g] khien tinh toi da luc ap (client: thanh Player_Shield)
	// [VHTD 02/09] ham dung ca 2 ben (KNpcAttribModify goi HS_SpAdd) - khai bao ngoai #ifdef _SERVER
	void				HS_AutoCastTick();				// [VHTD 02/09] moi khung: autocastskill
	void				HS_OnStateRemoved(KStateNode* pNode);	// [VHTD 02/09] cast_when_buff_removed
	void				HS_ResetBuffTime(int nBuffSkillId);	// [VHTD 02/09] reset_bufftime
	int					HS_SpGet(int nKey);				// [VHTD 02/09] so tang hien co
	void				HS_SpAdd(int nKey, int nAdd);		// [VHTD 02/09]
	BOOL				HS_SpCost(int nKey, int nCost);		// [VHTD 02/09] tru tang, FALSE neu khong du
	void				HS_ResetVhtd();					// [VHTD 02/09] reset truong tinh lai thuoc tinh (khong reset so tang)
	void				HS_SyncSp(int nKey);				// [VHTD 02/09g] server: gui so tang toi client cua chinh minh (client: rong)
	void				HS_SyncShield();				// [VHTD 02/09g] server: gui khien tinh hien tai/toi da (client: rong)
	int					m_CurrentIgnoreNegativeStateP;
	int					m_CurrentFireEnhance;			// 火加强
	int					m_CurrentColdEnhance;			// 冰加强
	int					m_CurrentPoisonEnhance;			// 毒加强
	int					m_CurrentLightEnhance;			// 电加强
	int					m_CurrentAddPhysicsDamage;		// 直接的物理伤害加强点数
	int					m_CurrentAddPhysicsMagic;		// Sat thuong vat ly noi cong
	int					m_CurrentMeleeEnhance[MAX_MELEE_WEAPON_VHTD+1];	// [VHTD 02/09] 0..8 vu khi (7/8 thuan) + 9 du	// 近程物理加强
	int					m_CurrentRangeEnhance;			// 远程物理加强
	int					m_CurrentHandEnhance;			// 空手物理加强
	int					m_CurrentSerisesEnhance;
	int			 		m_CurrentManaShield;								//#giam thieu sat thuong ganh chiu HTVC con lon
	int			 		m_CurrentStaticMagicShieldP;
	int					m_CurrentExpEnhance;
	int					m_CurrentSkillEnhancePercent;
	int					m_CurrentExpSkillsEnchance;// ExpSkills x2
	int					m_CurrentExpSkillsVip;
	// [KM 27/08b] Khi Doanh Dan Dien / trang bi sau nay: tang % sat thuong LEN
	// muc tieu he X (me2Xdamage_p) va giam % sat thuong TU ke dich he X
	// (X2medamage_p). Chi so = series_metal..series_earth (0..4).
	int					m_nMe2SeriesDamP[series_num];
	int					m_nSeries2MeDamP[series_num];
	int					m_CurrentFiveElementsEnhance;
	int					m_CurrentFiveElementsResist;
	int					m_CurrentManaToSkillEnhanceP;					//#khi noi cong day tang ky nang cong kich
	int					m_CurrentSorbDamageP;								//#triet tieu sat thuong
	// [KM 27/08] sau thuoc tinh he kinh mach, dung chuan ban Linux + client VLTK
	int					m_CurrentBlockRate;									//#hoa giai sat thuong
	int					m_CurrentAntiBlockRate;								//#triet tieu hoa giai
	int					m_CurrentEnhanceHitRate;							//#trong kich
	int					m_CurrentAntiEnhanceHitRate;						//#khang trong kich
	int					m_CurrentAntiAllResP;								//#bo qua toan khang
	int					m_CurrentAntiSorbDamageP;							//#xuyen giam thuong
	int					m_CurrentEnhanceHitEffect;							//#hieu qua trong kich
	int					m_nKMHitPercent;									//#he so don danh luot nay
	// [HOASON 01/09b] Linux KNpc [0x137c] meleedamagereturnmana_p, [0x1380] rangedamagereturnmana_p (NAN NHAN giu, ke danh +mana),
	// [0x1388]/[0x138c] addblockrate v0/v2 -> phan tram do ngau nhien min(25, Random(256)/v0*v2) cong vao hoa giai
	int					m_CurrentMeleeDamageReturnManaP;
	int					m_CurrentRangeDamageReturnManaP;
	int					m_CurrentAddBlockRateV0;
	int					m_CurrentAddBlockRateV2;
	int					m_CurrentAntiEnhanceHitEffect;						//#khang hieu qua trong kich
	int					m_CurrentAddDamageP;								//#cong % sat thuong, goc 100
	int					m_CurrentAntiHitRecover;							//#keo dai dong tac bi thuong gay ra
	int					m_nKMAntiHitRecover;								//#tri nhan duoc o don danh nay
	int					m_CurrentSorbDamageYanP;	// [PF 31/08k] triet tieu sat thuong ban Duong, PHAN NGHIN, tran 500 (237)
	int					m_CurrentAntiStunTimeReduceP;	// [PF 31/08k] keo dai thoi gian choang TA GAY RA (220)
	int					m_CurrentDoStunP;				// [CHOANG 01/09] do_stun_p (261) - cong vao XAC SUAT gay choang (Linux +0x1334)
	int					m_CurrentAntiDoStunP;			// [CHOANG 01/09] anti_do_stun_p (219) - tru vao xac suat bi choang (Linux +0x132c)
	int					m_CurrentAntiPoisonTimeReduceP;	// [PF 31/08k] keo dai thoi gian doc TA GAY RA (204)
	int					m_CurrentDoHurtP;	// [PF 31/08k] cong xac suat gay dong tac bi thuong (205)
	int					m_CurrentAntiDoHurtP;	// [PF 31/08k] khang dong tac bi thuong (223)
	int					m_CurrentManaReplenishPercent;	// [PF 31/08k] hoi noi luc % (254, muon o reserve6)
	int					m_CurrentAntiPhysicsResYanP;	// [PF 31/08k] bo qua pho phong doi phuong (244)
	int					m_CurrentAntiFireResYanP;	// [PF 31/08k] bo qua hoa phong (217)
	int					m_CurrentAntiColdResYanP;	// [PF 31/08k] bo qua bang phong (222)
	int					m_CurrentAntiPoisonResYanP;	// [PF 31/08k] bo qua doc phong (221)
	int					m_CurrentAntiLightingResYanP;	// [PF 31/08k] bo qua loi phong (248)
	//
	int					m_Dir;							// Npc的方向
	int					m_RedLum;						// Npc的亮度
	int					m_GreenLum;
	int					m_BlueLum;
	int					m_MapX, m_MapY, m_MapZ;			// Npc的地图坐标
	int					m_OffX, m_OffY;					// Npc在格子中的偏移坐标（放大了1024倍）
	int					m_DesX, m_DesY;					// Npc的目标坐标
	int					m_SkillParam1, m_SkillParam2;
	int					m_OriginX, m_OriginY;			// Npc的原始坐标
	int					m_NextAITime;
	BYTE				m_AIMAXTime;//NpcAI
	BOOL				m_bNoReloadAttr;
	int					m_nDamageReduction;
#ifndef _SERVER
	PLAYERTRADE			m_PTrade;
	std::vector<FindPathNode> m_PathFind;
	FindPathNode				m_AutoMoveTemp;
	int AutoMoveStuckCount;
	UINT				m_StandSyncTime;
#endif


	// Npc
	int					m_HelmType;					// Npc的头盔类型
	int					m_ArmorType;				// Npc的盔甲类型
	int					m_WeaponType;				// Npc的武器类型
	int					m_HorseType;				// Npc的骑马类型
	BOOL				m_bRideHorse;				// Npc是否骑马
	BOOL				m_RuongLock;				// Edit by phong kieu trang thai mo khoa ruong
	int					m_BaiTan;					// 
	char				ShopName[32];
	int					m_MaskType;					//#mat na
	int					m_MaskMark;					//#mat na
	BOOL				m_bMaskFeature;				// Npc????
	BYTE				m_btHonorId;			//#danh hieu
#ifndef _SERVER
	char				m_szTeamMem[64]; //hi觧 th?s?ngi trong nh鉳 s?d鬾g paint th玭g t譶 t?auto l猲 nh﹏ v藅
	void				SetSzTeamMem(char* s)
	{
		strcpy(m_szTeamMem, s);
	};
	void				ClearSzTeamMem()
	{
		memset(m_szTeamMem, 0, sizeof(m_szTeamMem));
	};
	int					m_nTimeAbsent; //th阨 gian v緉g m苩 s?d鬾g trong auto

	void				ResetPathFind();
#endif
	char				m_szTongName[32];
	char				m_szTongTitle[32];
	int					m_nFigure;
	char				Name[32];				// Npc的名称
	char				Owner[32];				//add by phong ki襲 19/08/2021
	int					m_nSex;					// Npc的性别0为男，1为女
	int					m_NpcSettingIdx;		// Npc的设定文件索引
	int					m_CorpseSettingIdx;		// Npc的尸体定义索引
	char				ActionScript[80];		// Npc的行为脚本
	DWORD				m_ActionScriptID;		// Npc的行为脚本ID（使用时用这个来检索）
	char				DropRateScript[64];
	DWORD				m_DropRateScriptID;
	BYTE				m_nPKFlag;
	int					m_nMissionGroup;
	int					m_MantleType;		//#phi phong
	DWORD				m_TrapScriptID;			// Npc的当前Trap脚本ID;
#ifdef _SERVER
	// [PORT5 23/08] JX2: tham so o trap dang dung / trai tam thoi / co khong-hoi-sinh
	int				m_nLastTrapParam;		// JX2TRAP_PARAM_NONE = trap JX1
	int				m_nTmpCamp;				// Linux KNpc+0x1900; 0 = khong dung (SetTmpCamp)
	BYTE			m_bNoRevive;			// Linux +0x1824 (AddNpcEx tham so 7): chet la bien mat
	// [BDH 27/08] he ban dong hanh (KPlayerPartner.cpp): chu so huu cua NPC kind_partner
	int				m_nPartnerOwner;		// player idx cua chu (0 = khong phai partner)
	int				m_nPartnerNo;			// con so may cua chu (1..3)
#endif
	int					m_nPeopleIdx;			// 对象人物
	int					m_nLastDamageIdx;		// 最后一次伤害的人物索引
	int					m_nLastPoisonDamageIdx;	// 最后一次毒伤害的人物索引
	int					m_nObjectIdx;			// 对象物品
	//
	int					m_Experience;			
	int					m_LifeMax;				
	int					m_LifeReplenish;		// Npc的生命回复速度
	int					m_ManaMax;				// Npc的最大内力
	int					m_ManaReplenish;		// Npc的内力回复速度
	int					m_StaminaMax;			// Npc的最大体力
	int					m_StaminaGain;			// Npc的体力回复速度
	int					m_StaminaLoss;			// Npc的体力下降速度
	int					m_AttackRating;			// Npc的命中率
	int					m_Defend;				// Npc的防御
	int					m_FireResist;			// Npc的火抗性
	int					m_ColdResist;			// Npc的冷抗性
	int					m_PoisonResist;			// Npc的毒抗性
	int					m_LightResist;			// Npc的电抗性
	int					m_PhysicsResist;		// Npc的物理抗性
	int					m_FireResistMax;		// Npc的最大火抗性
	int					m_ColdResistMax;		// Npc的最大冰抗性
	int					m_PoisonResistMax;		// Npc的最大毒抗性
	int					m_LightResistMax;		// Npc的最大电抗性
	int					m_PhysicsResistMax;		// Npc的最大物理抗性
	int					m_WalkSpeed;			// Npc的行走速度
	int					m_RunSpeed;				// Npc的跑动速度
	int					m_JumpSpeed;			// Npc的跳跃速度
	int					m_AttackSpeed;			// Npc的攻击速度
	int					m_CastSpeed;			// Npc的施法速度
	int					m_VisionRadius;			// Npc的视野范围 //B竛 k輓h t莔 nh譶
	int					m_DialogRadius;			// Npc的对话范围
	int					m_ActiveRadius;			// Npc的活动范围 //B竛 k輓h ho箃 ng
	int					m_HitRecover;			// Npc的受击回复速度
	int					m_Treasure;				// Npc丢落装备的数量
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
	int					m_nTime_Ignorenegativestate;	//3 gi﹜ lo筰 b?tr筺g th竔 d?thng xu蕋 ?b蕋 di詍
	double				m_lastSyncTime;
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
	int					m_nPosShiftStep;
	// Bi chan tam thoi: dung that (tu the dung) NHUNG GIU m_DesX/m_DesY, khac
	// DoStand() von xoa dich nen phai cho lenh moi tu server.
	void				DoStandBlocked();
	// Co "toi dang sai, hay nan lai toi" - port tu ban goc JX2 (KNpc + 0x1894).
	// Khi mo phong phia client bi ket (tim duong tra 0), thay vi DoStand() cung nhu
	// truoc, ta giu nguyen trang thai chay va gio co; goi dong bo dinh ky KE TIEP
	// se ghi de toa do that tu server roi ha co.
	int					m_nNeedFixPos;
	DWORD				m_dwLastDirTick;	// tick logic cuoi da lam muot m_ResDir
	BOOL				m_bProcPosShift;
	static int			g_DrawVision;
	static int			g_DrawVisionSkill;
#endif
private:
//	int m_nLucky; // 
	void DrawLifeBar(int percentage, int baseX, int baseY, KUiPlayerPaintTeamMNG *config, int offsetMember, int halfWidth);
    void DrawManaBar(int percentage, int baseX, int baseY, KUiPlayerPaintTeamMNG *config, int offsetMember, int halfWidth);
    void DrawBar(int baseX, int baseY, int width, int heightOffset, int height, int offsetMember, int halfWidth, int percentage, const Color& color, KUiPlayerPaintTeamMNG *config);
	INT m_nTargetX; // Member variable to store the X coordinate of the target position
    INT m_nTargetY; 
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
	int					m_ReviveFrame;			//  th阨 gian h錳 sinh
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
	int					m_nBloodColor[5];
	int					m_nBloodFontSize[5];
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
	VOID				FixPos();	// 修正坐标使m_OffX,m_OffY不留小数（缩小回1024倍后的小数）
	BOOL				CheckHitTarget(int nAR, int nDf, int nIngore = 0);	//ч ch輓h x竎
	BOOL				CheckHitTarget2(int nAR, int nDf, int nIngore = 0);	//ч ch輓h x竎
	void				AddStateSkillList(KStateNode * pNewNode);
#ifdef _SERVER
	void				LoseSingleItem(int nBelongPlayer);
	void				LoseMoney(int nBelongPlayer);
	void				PlayerDeadCreateMoneyObj(int nMoneyNum);	
	void				UpdateNpcStateInfo();
	void				AddStateInfo(int nID);
	void				SyncDamageInfo(int nLauncher, int nDamage, COMBAT_INFO_TYPE damType, int skillId, bool isCrit = false, bool bBroadCast = true);
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
	DWORD				GetId() const;
	int					GetKind() const;
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
	NPC_COMMAND			GetCommand();
	void				ProcCommand(int nAI);
	ISkill* 			GetActiveSkill();
	ISkill* 			GetActiveSkillAura();
	BOOL				ProcessState();
	void				ProcStatus();
	void				ModifyAttrib(int nAttacker, void* pData);
	void				SetId(DWORD	dwID)	{ m_dwID = dwID;};
	BOOL				IsMatch(DWORD dwID)	{ return dwID == m_dwID; };	
	BOOL				Cost(NPCATTRIB nType, int nCost, BOOL bOnlyCheckCanCast = FALSE);				//OnlyCheckCanCost TRUE
	void				Load(int nNpcSettingIdx, int nLevel, int nSeries);						//fix by phong ki襲
	void				GetMpsPos(int * pPosX, int *pPosY);
	int					DetonateMissles(int nStyle, int nRadius, int nFlag);	// [HOASON 01/09b] Linux 0x08079870 (candetonate1-3)
#ifndef _SERVER
	VOID				ClientGotoPos(INT nX, INT nY, INT nMode = 0);	// nMode: 0.Auto; 1.Walk; 2.Run
	INT					  GetMapDisX(INT nIdx1, INT nIdx2);
	INT					  GetMapDisY(INT nIdx1, INT nIdx2);
	 static INT GetDistance(INT nIdx1, INT nIdx2, bool isX);
	 INT calculateMapDis(INT nIdx1, INT nIdx2, bool isX);
     void                AddSkillEffect(int nSkillID, int nLevel, int nTime);
#endif
	BOOL				SetActiveSkill(int nSkillIdx);
	void				SetAuraSkill(int nSkillID);
	void				SetCamp(int nCamp);
	void				SetCurrentCamp(int nCamp);
	void				RestoreCurrentCamp();
	void  				SetPlayerTitle(DWORD nRankId, DWORD nTime, DWORD overLook);//#PlayerTitle
	void				SetCurPlayerTitle(DWORD nRankId,DWORD overLook);//#PlayerTitle
	void				SetRankBattle(DWORD nRankId, int nTime, int overLook);//#RankBattle
	void				UpdateGameTitle();		// Update GameTitle based on faction and level
	void				SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime = -1, BOOL bOverLook = FALSE);	// 主动辅助技能与被动技能

	void				ForceClearStateSkillEffect();

	void				ForceClearStateSkillEffect(int nSkillId);
	int					GetStateSkillLevel(int nSkillId);	// [WLLS] level trang thai skill, -1 = khong co

	void				ClearStateSkillEffect();
	void				ClearStateSkillEffect(int nSkillId);
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
	void				SwitchMaskFeature();
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
	void	            SetSex(int nSex);
	void				GetNpcCopyFromTemplate(int nNpcTemplateId,int nLevel, int nSeries);
	void				SetPhysicsDamage(int nMinDamage, int nMaxDamage);	
	void				SetReviveFrame(int nReviveFrame)	//Th阨 gian h錳 sinh
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
	int					DropPUBGItemFromLuaScript(int nKind, int nGenre, int nDetail, int nParticular, int nSeries, int nLevel, int nLuck, int* pnMagicLevel, int nTimeBelong);
	int					  UpdateDBStateList(BYTE *);
	void				ExecuteRevive(){DoRevive();};
	BOOL			SendSyncData(int nClient);						
	void				NormalSync();									
	void				BroadCastRevive(int nType);
	int					  GetPlayerIdx();
	BOOL		    CalcDamage(int nAttacker, int nMin, int nMax, DAMAGE_TYPE nType, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, BOOL bReturn = FALSE , int nSeries_DamageP = 0, int nStole_Life = 0, int nStole_Mana = 0, int nStole_Stamina = 0, BOOL bIsDS = FALSE, BOOL bIsFS = FALSE, int nTotalAvg = 0);	// [HOTHAN2 01/09] nTotalAvg = tong trung binh 4 he (khien diem)
	BOOL		    ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate);
	void			    Cast(int nSkillId, int nSkillLevel);
	void				ReplySkill(int nLauncher = 0);	// [HOASON 01/09e] Linux Fire(list, chu, KE DANH): loai 1 nham ke danh
	void				RescueSkill(int nAttacker = 0);	// [HOASON 02/09] Linux Fire(+0x1898, chu, ke danh): loai 1 nham ke danh, khac nham minh
	void				AttackSkill(int nUnderAttacker);
	void				DeathSkill();
	void				CastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget);	// [HOASON 02/09] tu phong nham muc tieu: phat s2c_castskilldirectly (client ve dan) roi Cast(m_Index, -1, nTarget)
	// mode == 0 npc  == 1 player  == 2 player 	// DoDeath enumDEATH_MODE
	void				DeathPunish(int nMode, int nBelongPlayer);		//edit by phong kieu hut hit
	void				RestoreLiveData();								// 重生后恢复Npc的基本数据
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
	void				SendDataToNearRegion(void* pBuffer, DWORD dwSize, int nLimit = -1);	// [S11] nLimit>0 = tran rieng (goi chet/go)
	int					DeathCalcPKValue(int nKiller);
	int					FindAroundPlayer(const char* Name);//add by phong ki襲 using v薾 ti猽
	BOOL			CheckPlayerAround(int nPlayerIdx);
    int					GetMagicLevel(int nLuckySoDong, int mm);
	int					GetLevelMagic(int nTotalLucky, int mmopt);
    void				DropRateItem(int nCount, const char* pszFileName, int nUnknow, int nItemLevel, 
								int nItemSeries, int nBelongIdx);
#endif

	BOOL			IsCanInput() { return m_ProcessAI; };
#ifndef _SERVER
	void				SetSleepMode(BOOL bSleep) { m_nSleepFlag = bSleep; m_DataRes.SetSleepState(bSleep);};
	void				SetNpcState(BYTE* pNpcState);
	void				RemoveRes();
	void				ProcNetCommand(NPCCMD cmd, int x = 0, int y = 0, int z = 0);
	void				Paint();
	int					PaintInfo(int nHeightOffset, bool bSelect, int nFontSize = 12, DWORD	dwBorderColor = 0);
	void				PaintSeriesNpc(char* szName, int nFontSize, int nHeightOff);
	int					PaintChat(int nHeightOffset);
	int					PaintMantle(int nHeightOff, int nFontSize, int nMpsX, int nMpsY);
	int					SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength);
	int					PaintLife(int nHeightOffset, bool bSelect);
	int					PaintMana(int nHeightOffset);
	int					PaintTeamMNG(KUiPlayerItem *m_pPlayersList, KUiPlayerPaintTeamMNG *nPainTMG); 
	int					PaintTargetInfo(KUiPlayerItem *m_pPlayersList, KUiPlayerPaintTeamMNG *nPainTMG);
	static int clamp(int val, int minVal, int maxVal);
	void				DrawBorder();
	int					DrawMenuState(int n);
	void				DrawBlood();	
	void				SetMenuState(int nState, char *lpszSentence = NULL, int nLength = 0);	
	int					GetMenuState();				
	DWORD		SearchAroundID(DWORD dwID);	
	void				SetSpecialSpr(char *lpszSprName);
	void				SetInstantSpr(int nNo);
	int					GetNormalNpcStandDir(int nFrame);
	KNpcRes*			GetNpcRes(){return &m_DataRes;};
	int						GetNpcPate();
	int						GetNpcPatePeopleInfo();
	void				GetDrawPos(int * pPosX, int *pPosY);	// drawn (interpolated) position - Paint* overlay code only
	void				ClearBlood(int i);
	void				SetBlood(int nNo);
	int					PaintBlood(int nHeightOffset);	
	void				SetBlood2(DAMAGESHOW* Damage);
	BOOL				FindStateSkill(int nID);
	void				HideRes(int nType, bool bFlag);
	void				ChangeRecruit(int m_Recruit);
	int				GetRecruit(){return m_Recruit;};
	void			OnRunByFPS(int nStep);
	void			OnWalkByFPS(int nStep);

#endif

	int			m_nTime;
	int			m_nTotalFrame;
	int			m_nFrame;
	
};
#ifdef _SERVER
extern KNpc* Npc;
#elif TOOLVERSION
extern CORE_API KNpc Npc[MAX_NPC];
#else
extern KNpc Npc[MAX_NPC];
#endif

#endif

