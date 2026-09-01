//-----------------------------------------------------------------------
//	Sword3 KNpc.cpp
//-----------------------------------------------------------------------
#include "KCore.h"
#ifdef _SERVER
extern void Partner_OnNpcDeath(int nNpcIdx);	// [BDH 27/08] KPlayerPartner.cpp
#endif
//#include <crtdbg.h>
#include "KNpcAI.h"
#include "KSkills.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"
#include "KPlayerBot.h"	// PB_IsBot / PB_TrapLog - bot mien trap
#include "KRegion.h"
#include "KNpcTemplate.h"
#include "KItemSet.h"
#include "KSortScript.h"
#include "KOption.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "../../../lib/S3DBInterface.h"
#include "KPlayerSet.h"
#include "KSkillManager.h"
#else
#include "../../Headers/IClient.h"
#include "CoreShell.h"
#include "Scene/KScenePlaceC.h"
#include "KIme.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "ImgRef.h"
#include "Text.h"
#endif
#include "KNpcAttribModify.h"
#include "CoreUseNameDef.h"
#include "KSubWorld.h"
#include "Scene/ObstacleDef.h"
#include "Scene/SceneDataDef.h"
#include "KThiefSkill.h"
#ifdef _STANDALONE
#include "KThiefSkill.cpp"
#endif
#include <KItemChangeRes.h>

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#include <MapHandler.h>
extern KLuaScript		*g_pNpcLevelScript;
#ifndef _SERVER
extern BOOL			g_bPaintInterpFocus;	// CoreShell.cpp: PaintFps interpolation drives the camera
#endif

#define	ATTACKACTION_EFFECT_PERCENT		60
#define	MIN_DOMELEE_RANGE				20
#define	MIN_BLURMOVE_SPEED				1
#define	MIN_JUMP_RANGE					0
#define	ACCELERATION_OF_GRAVITY			10
#define		SHOW_CHAT_WIDTH				24
#define		SHOW_CHAT_COLOR				0xffffffff
#define		SHOW_BLOOD_COLOR			0x00ff0000
#define		SHOW_BLOOD_COLOR_MISS			0x00ff00ff
#define		SHOW_BLOOD_COLOR_CRIT		0x00ffd24d
#define		SHOW_BLOOD_COLOR_ABSORB_LIFE		0x0000ff00
#define		SHOW_BLOOD_COLOR_ABSORB_MANA		0x000000ff
#define		SHOW_BLOOD_COLOR_ABSORB_STAMINA		0x00d2ebdb
#define		defMAX_SHOW_BLOOD_TIME		40
#define		defSHOW_BLOOD_MOVE_SPEED	7
#define		SHOW_LIFE_WIDTH				40
#define		SHOW_LIFE_HEIGHT			3
#define		SHOW_SPACE_HEIGHT			5
#define		FIND_PATH_DISTANCE		1
#define FRAME2TIME							18
#define	GAME_UPDATE_TIME		10
#define	GAME_SYNC_LOSS			100
#define	STAMINA_RECOVER_SCALE	4

#define	REGIONWIDTH			SubWorld[m_SubWorldIndex].m_nRegionWidth
#define	REGIONHEIGHT		SubWorld[m_SubWorldIndex].m_nRegionHeight
//1024
#define	CELLWIDTH			(SubWorld[m_SubWorldIndex].m_nCellWidth << 10)
#define	CELLHEIGHT			(SubWorld[m_SubWorldIndex].m_nCellHeight << 10)
// 
#define	CURREGION			SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex]
// 
#define	LEFTREGIONIDX		CURREGION.m_nConnectRegion[2]
#define	RIGHTREGIONIDX		CURREGION.m_nConnectRegion[6]
#define	UPREGIONIDX			CURREGION.m_nConnectRegion[4]
#define	DOWNREGIONIDX		CURREGION.m_nConnectRegion[0]
#define	LEFTUPREGIONIDX		CURREGION.m_nConnectRegion[3]
#define	LEFTDOWNREGIONIDX	CURREGION.m_nConnectRegion[1]
#define	RIGHTUPREGIONIDX	CURREGION.m_nConnectRegion[5]
#define	RIGHTDOWNREGIONIDX	CURREGION.m_nConnectRegion[7]

#define	LEFTREGION			SubWorld[m_SubWorldIndex].m_Region[LEFTREGIONIDX]
#define	RIGHTREGION			SubWorld[m_SubWorldIndex].m_Region[RIGHTREGIONIDX]
#define	UPREGION			SubWorld[m_SubWorldIndex].m_Region[UPREGIONIDX]
#define	DOWNREGION			SubWorld[m_SubWorldIndex].m_Region[DOWNREGIONIDX]
#define	LEFTUPREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTUPREGIONIDX]
#define	LEFTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[LEFTDOWNREGIONIDX]
#define	RIGHTUPREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTUPREGIONIDX]
#define	RIGHTDOWNREGION		SubWorld[m_SubWorldIndex].m_Region[RIGHTDOWNREGIONIDX]

#define	CONREGION(x)		SubWorld[m_SubWorldIndex].m_Region[CURREGION.m_nConnectRegion[x]]
#define	CONREGIONIDX(x)		CURREGION.m_nConnectRegion[x]
#ifndef _SERVER
KNpc	Npc[MAX_NPC];
#else
KNpc* Npc = new KNpc[MAX_NPC];
#endif
KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL][MAX_NPC_SERIES];

KNpc::KNpc()
{
#ifdef _SERVER
	m_AiSkillRadiusLoadFlag = 0;
#endif
	Init();

}

inline DWORD KNpc::GetId() const
{
	return m_dwID;
}

inline int KNpc::GetKind() const
{
	return m_Kind;
}

void KNpc::Init()
{
	memset(m_btStateInfo, 0, sizeof(m_btStateInfo));
	m_dwID = 0;
	m_Index = 0;
	m_nPlayerIdx = 0;
	m_ProcessAI = 1;
	m_Kind = kind_normal;
	m_btSimCityBot   = 0;		// Port SimCity: bot gia lap
	m_Series = series_metal;
	m_Camp = camp_free;
	m_CurrentCamp = camp_free;
	m_Doing = do_stand;
	// FIX 24/08: Init() (duoc KNpc::Remove goi) KHONG xoa m_Command. Sau ban va giu lenh do_skill o
	// khung cuoi hoat anh, khe NPC co the duoc thu hoi khi con mot lenh treo => nhan vat MOI nap vao
	// khe do se thi hanh lenh cua nguoi truoc (nang nhat: DoSkill -> SendCommand(do_run) toi toa do cu).
	m_Command.CmdKind = do_none;
	m_Height = 0;
	m_Frames.nCurrentFrame = 0;
	m_Frames.nTotalFrame = 0;
	m_SubWorldIndex = 0;
	m_RegionIndex = -1;
	m_Experience = 0;
	m_ActiveSkillID = 0;
	m_TimeSWPK = 0;
	m_TimeHorse = 0;
	m_SkillParam1 = 0;
	m_SkillParam2 = 0;
	m_nNpcTimeout = 0;
	ZeroMemory(m_nNpcParam, sizeof(m_nNpcParam));
	m_bNpcFollowFindPath = FALSE;
	m_uFindPathTime = 0;
	m_uFindPathMaxTime = 0;
	m_uLastFindPathTime = 0;
#ifndef _SERVER
	m_nChatContentLen = 0;
	m_nCurChatTime = 0;
	m_nChatNumLine = 0;
	m_nChatFontWidth = 0;
	m_nStature = 0;
	ZeroMemory(m_szTongName, sizeof(m_szTongName));
	ZeroMemory(m_szTongTitle, sizeof(m_szTongTitle));
	ZeroMemory(m_szTeamMem, sizeof(m_szTeamMem));
	m_nFigure = -1;
	m_nTimeAbsent = 0; //thêi gian v¾ng mÆt sö dông trong auto
	m_StandSyncTime = 0;
#endif
	m_CurrentLife = 100;			
	m_CurrentLifeMax = 100;		
	m_CurrentLifeReplenish = 0;	
	m_CurrentLifeReplenishPercent = 0;
	m_CurrentMana = 100;			
	m_CurrentManaMax = 100;		
	m_CurrentManaReplenish = 0;	
	m_CurrentStamina = 100;		
	m_CurrentStaminaMax = 100;	
	m_CurrentStaminaGain = 0;	
	m_CurrentStaminaLoss = 0;	
	m_CurrentAttackRating = 100;	
	m_CurrentDefend = 10;		
	m_CurrentWalkSpeed = 5;		
	m_CurrentRunSpeed = 10;		
	m_CurrentJumpSpeed = 12;	
	m_CurrentJumpFrame = 40;	
	m_CurrentAttackSpeed = 0;	
	m_CurrentCastSpeed = 0;		
	m_CurrentVisionRadius = 40;	
	m_CurrentAttackRadius = 30;	
	m_CurrentHitRecover = 0;		//thêi gian phôc håi
	m_CurrentAddPhysicsDamage = 0;	
	m_CurrentAddPhysicsMagic = 0;
	m_Dir = 0;					
	m_JumpStep = 0;
	m_JumpDir = 0;			
	m_MapZ = 0;					
	m_HelmType = 1;				
	m_ArmorType = 1;			
	m_WeaponType = 1;			
	m_HorseType = -1;			
	m_bRideHorse = FALSE;		
	m_RuongLock = FALSE;		// edit by phong kieu trang thai mo khoa ruong
	m_BaiTan = 0;
	m_MaskType = 0;						//#mat na
	m_MaskMark = 0;
	m_bMaskFeature = TRUE;
	m_btHonorId = 0;					//#danh hieu
	ZeroMemory(ShopName,32);
	//m_RideState = 0;
	m_nProtectedTime = 0;				//vong tron bat tu, vßng trßn bÊt tö
	ZeroMemory(Name, 32);		
	m_NpcSettingIdx = 0;		
	m_CorpseSettingIdx = 0;		// Body
	ZeroMemory(ActionScript,80);
	m_ActionScriptID = 0;
	ZeroMemory(DropRateScript,64);
	m_DropRateScriptID = 0;
	m_MantleType	= 0;			//#phi phong
	m_TrapScriptID = 0;
#ifdef _SERVER
	m_nLastTrapParam = JX2TRAP_PARAM_NONE;	// [PORT5 23/08]
	m_nTmpCamp = 0;
	m_bNoRevive = 0;
#endif
	m_btRankId					= 0;
	m_btRankBattleId			= 0; //#RankBattle
	m_btPlayerTitle				= 0; //#PlayerTitle
	ZeroMemory(m_szGameTitle, sizeof(m_szGameTitle));
	//ZeroMemory(MateName, sizeof(MateName)); //#MateName
	m_nMissionGroup = -1;
	m_ExItemId					= 0; // hanh trang
	m_ExBoxId					= 0; // ruong mo rong
	nRankInWorld				= 0;
	nRepute						= 0;//danh vong
	nFuYuan						= 0;//phuc duyen
	nPKValue					= 0;
	m_byMantleLevel				= 0;
	m_ImagePlayer				= 0;
	nReBorn						= 0;
	nFirstFaction				= -1;
	m_LifeMax					= 100;		
	m_LifeReplenish				= 0;		
	m_ManaMax					= 100;		
	m_ManaReplenish				= 0;		
	m_StaminaMax				= 100;		
	m_StaminaGain				= 0;		
	m_StaminaLoss				= 0;		
	m_AttackRating				= 100;		
	m_Defend					= 10;		
	m_WalkSpeed					= 6;		
	m_RunSpeed					= 10;		
	m_JumpSpeed					= 12;		
	m_AttackSpeed				= 0;		
	m_CastSpeed					= 0;		
	m_VisionRadius				= 40;		
	m_DialogRadius				= 124;		
	m_HitRecover				= 12;		
	m_nPeopleIdx				= 0;
	m_LoopFrames				= 0;
	m_WalkFrame					= 12;
	m_RunFrame					= 15;
	m_StandFrame				= 15;
	m_DeathFrame				= 15;
	m_HurtFrame					= 10;
	m_AttackFrame				= 20;
	m_CastFrame					= 20;
	m_SitFrame					= 15;
	m_JumpFrame					= 40;
	m_AIMAXTime					= 25;
	m_NextAITime				= 0;
	m_ProcessState				= 1;
	m_ReviveFrame				= 100;					//Thêi gian håi sinh
	m_bExchangeServer			= FALSE;
	m_bActivateFlag				= FALSE;
	m_FightMode					= 0;
	m_OldFightMode				= 0;
	m_bNoReloadAttr				= FALSE;

#ifdef _SERVER
	m_pDropRate					= NULL;
	m_nTimeIdleValue		= 0;
	m_nTimeIdleCounter		 = 0;
	m_nTime_Ignorenegativestate = 0;	//3 gi©y lo¹i bá tr¹ng th¸i dÞ th­êng xuÊt ø bÊt diÔm
	m_nCurPKPunishState = 0;
	m_lastSyncTime = 0;
#endif

#ifndef _SERVER
	m_SyncSignal				= 0;
	m_sClientNpcID.m_dwRegionID	= 0;
	m_sClientNpcID.m_nNo		= -1;
	m_ResDir					= 0;
	m_dwLastDirTick				= 0;
	m_nNeedFixPos				= 0;
	m_nPKFlag					= enumPKNormal;
	m_nSleepFlag				= 0;
	memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
	memset(m_nBloodNo, 0, sizeof(m_nBloodNo));
	memset(m_nBloodAlpha, 0, sizeof(m_nBloodAlpha));
	memset(m_nBloodTime, 0, sizeof(m_nBloodTime));
	memset(m_szBloodNo, 0, sizeof(m_szBloodNo));
	m_nTongFlag					= 0;
#endif

	m_nLastPoisonDamageIdx = 0;
	m_nLastDamageIdx = 0;
	m_bHaveLoadedFromTemplate = FALSE;
	m_bClientOnly = FALSE;
	m_bWaitingPlayerFeedBack	= false;
	m_btTryExecuteScriptTimes	= 0;
	memset(Owner, 0, sizeof(Owner));
	m_CurrentRangeDmgRetPercent = 0;
	m_CurrentMeleeDmgRetPercent = 0;
	m_CurrentMeleeDmgRet = 0;
	m_CurrentRangeDmgRet = 0;
	m_CurrentReturnResPercent = 0;
	m_CurrentActiveRadius = 0;
	m_CurrentTreasure = 0;
	m_CurrentPoisonDamageReturn = 0;
	m_nFigure = 0;
	m_ActiveRadius = 0;
	m_Treasure = 0;
	m_nCurrentMeleeSkill = 0;
	m_nCurrentMeleeTime = 0;
	m_AiMode = 0;
	m_AiAddLifeTime = 0;
	m_HeadImage = 0;
	m_nTargetX = 0;
	m_nTargetY = 0;
	m_StandFrame1 = 0;
	m_JumpFirstSpeed = 0;
	m_XFactor = 0;
	m_YFactor = 0;
	m_SpecialSkillStep = 0;
	m_nTime = 0;
	m_nTotalFrame = 0;
	m_nFrame = 0;
	m_CurrentExperience = 0;
	m_Type = 0;
	m_ClientDoing = cdo_stand;
	m_nStature = 0;
	m_Recruit = 0;
	m_ActiveAuraID = 0;
	m_CurrentFireResist = 0;
	m_CurrentColdResist = 0;
	m_CurrentPoisonResist = 0;
	m_CurrentLightResist = 0;
	m_CurrentPhysicsResist = 0;
	m_CurrentFireResistMax = 0;
	m_CurrentColdResistMax = 0;
	m_CurrentPoisonResistMax = 0;
	m_CurrentLightResistMax = 0;
	m_CurrentPhysicsResistMax = 0;
	m_CurrentSlowMissle = 0;
	m_CurrentDamageReduce = 0;
	m_CurrentElementDamageReduce = 0;
	m_CurrentDamage2Mana = 0;
	m_CurrentManaPerEnemy = 0;
	m_CurrentLifeStolen = 0;
	m_CurrentManaStolen = 0;
	m_CurrentStaminaStolen = 0;
	m_CurrentDeadlyStrikeEnhanceP = 0;
	m_CurrentFatallyStrikeEnhanceP = 0;
	m_CurrentFatallyStrikeResP = 0;
	m_CurrentBlindEnemy = 0;
	m_CurrentPiercePercent = 0;
	m_CurrentFreezeTimeReducePercent = 0;
	m_CurrentPoisonTimeReducePercent = 0;
	m_CurrentStunTimeReducePercent = 0;
	m_CurrentReturnSkillPercent = 0;
	m_CurrentIgnoreSkillPercent = 0;
	m_CurrentPoisonDamageReturnPercent = 0;
	m_CurrentIgnoreNegativeStateP = 0;
	m_CurrentFireEnhance = 0;
	m_CurrentColdEnhance = 0;
	m_CurrentPoisonEnhance = 0;
	m_CurrentLightEnhance = 0;
	m_CurrentRangeEnhance = 0;
	m_CurrentHandEnhance = 0;
	m_CurrentSerisesEnhance = 0;
	m_CurrentManaShield = 0;
	m_CurrentStaticMagicShieldP = 0;
	m_CurrentExpEnhance = 0;
	m_CurrentSkillEnhancePercent = 0;
	m_CurrentExpSkillsEnchance = 0;
	m_CurrentExpSkillsVip = 0;
	memset(m_nMe2SeriesDamP, 0, sizeof(m_nMe2SeriesDamP));	// [KM 27/08b]
	memset(m_nSeries2MeDamP, 0, sizeof(m_nSeries2MeDamP));
	m_CurrentFiveElementsEnhance = 0;
	m_CurrentFiveElementsResist = 0;
	m_CurrentManaToSkillEnhanceP = 0;
	m_CurrentSorbDamageP = 0;
	m_CurrentBlockRate = 0; m_CurrentAntiBlockRate = 0;					// [KM 27/08]
	m_CurrentEnhanceHitRate = 0; m_CurrentAntiEnhanceHitRate = 0;		// [KM 27/08]
	m_CurrentAntiAllResP = 0; m_CurrentAntiSorbDamageP = 0;				// [KM 27/08]
	m_CurrentEnhanceHitEffect = 0; m_nKMHitPercent = 100;				// [KM 27/08]
	m_CurrentAntiEnhanceHitEffect = 0; m_CurrentAntiHitRecover = 0;		// [KM 27/08]
	m_CurrentAddDamageP = 100; m_nKMAntiHitRecover = 0;					// [KM 27/08] goc 100 theo Linux
	m_CurrentSorbDamageYanP = 0;	// [PF 31/08k]
	m_CurrentAntiStunTimeReduceP = 0;	// [PF 31/08k]
	m_CurrentAntiPoisonTimeReduceP = 0;	// [PF 31/08k]
	m_CurrentDoHurtP = 0;	// [PF 31/08k]
	m_CurrentAntiDoHurtP = 0;	// [PF 31/08k]
	m_CurrentManaReplenishPercent = 0;	// [PF 31/08k]
	m_CurrentAntiPhysicsResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiFireResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiColdResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiPoisonResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiLightingResYanP = 0;	// [PF 31/08k]
	m_RedLum = 0;
	m_GreenLum = 0;
	m_BlueLum = 0;
	m_MapX = 0;
	m_MapY = 0;
	m_OffX = 0;
	m_OffY = 0;
	m_DesX = 0;
	m_DesY = 0;
	m_OriginX = 0;
	m_OriginY = 0;
	m_nSex = 0;
	m_nObjectIdx = 0;
	m_FireResist = 0;
	m_ColdResist = 0;
	m_PoisonResist = 0;
	m_LightResist = 0;
	m_PhysicsResist = 0;
	m_FireResistMax = 0;
	m_ColdResistMax = 0;
	m_PoisonResistMax = 0;
	m_LightResistMax = 0;
	m_PhysicsResistMax = 0;
	m_Level = 0;
	m_nDamageReduction = 0;
}

ISkill* KNpc::GetActiveSkill()
{
	_ASSERT(m_ActiveSkillID < MAX_SKILL);
	int nCurLevel = m_SkillList.GetCurrentLevel(m_ActiveSkillID);
	if (nCurLevel > 0)
		return g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	else 
		return NULL;
}

ISkill* KNpc::GetActiveSkillAura()
{
	_ASSERT(m_ActiveSkillID < MAX_SKILL);
	int nCurLevel = m_SkillList.GetCurrentLevel(m_ActiveSkillID);
	if (nCurLevel > 0)
		return g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	else 
		return NULL;
}

void KNpc::SetCurrentCamp(int nCamp)
{
	m_CurrentCamp = nCamp;

#ifdef _SERVER
	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;

	int	nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::SetCamp(int nCamp)
{
	m_Camp = nCamp;
#ifdef _SERVER
	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	if (m_RegionIndex < 0)
		return;

	NPC_CHGCAMP_SYNC	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcchgcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_Camp;

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::RestoreCurrentCamp()
{
	m_CurrentCamp = m_Camp;
#ifdef _SERVER
	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	if (m_RegionIndex < 0)
		return;

	NPC_CHGCURCAMP_SYNC	NetCommand;
	
	NetCommand.ProtocolType = (BYTE)s2c_npcchgcurcamp;
	NetCommand.ID = m_dwID;
	NetCommand.Camp = (BYTE)m_CurrentCamp;
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

#define		NPC_SHOW_CHAT_TIME		15000
int		IR_IsTimePassed(unsigned int uInterval, unsigned int& uLastTimer);

void KNpc::Activate()
{
	if (!m_Index)
	{
		//g_DebugLog("[DEATH] No Index: %d", m_Index);
		return;
	}

	if (m_bExchangeServer)
	{
		//g_DebugLog("[DEATH] Change Server: %d", m_bExchangeServer);
		return;
	}

	// Check here
	if (m_bActivateFlag)
	{
		m_bActivateFlag = FALSE;	// restore flag
		return;
	}
	//m_bActivateFlag = TRUE;

	m_LoopFrames++;

	//g_DebugLog("[DEATH] m_ProcessState: %d", m_ProcessState);
	if (m_ProcessState)
	{
		if (ProcessState())
			return;
	}

	if (m_ProcessAI)
	{
		NpcAI.Activate(m_Index);
	}
	else
	{
		NpcAI.NotActivate(m_Index);
	}

	ProcCommand(m_ProcessAI);
	ProcStatus();

#ifdef _SERVER
	// [24/08] Truoc day ham nay bi goi HAI LAN trong cung mot lan KNpc::Activate
	// (o day va mot lan nua o cuoi ham, canh khoi m_nNpcTimeout). No GIAM bo dem
	// m_nTime-- nen cua so quy cong sat thuong bi rut con MOT NUA:
	// defMAX_CALC_EXP_TIME = 1200 tick (66,7 giay o 18 khung/giay) ma thuc te chi
	// con 600 tick (33,3 giay). Da kiem: giua hai loi goi khong co 'return' nao nen
	// ca hai luon chay cung nhau. Giu DUNG MOT loi goi o day.
	// LUU Y CAN BANG: cua so tro ve dung 66,7 giay - nguoi danh quai roi quay lai
	// trong 66 giay van duoc tinh cong (truoc kia qua 33 giay la mat), tuc anh huong
	// ai duoc tinh ha quai => chia kinh nghiem va quyen roi do.
	// Muon giu Y NGUYEN cam giac cu: doi defMAX_CALC_EXP_TIME 1200 -> 600 trong
	// KNpcDeathCalcExp.h (mot dong), ma van dung.
	this->m_cDeathCalcExp.Active();
#endif

/*	if (Map == 220 && Player[CLIENT_PLAYER_INDEX].m_nIndex != m_Index && Npc[m_Index].m_Kind == kind_player)
	{
		m_MaskType = 1330;
	}
	else
	{
		m_MaskType = 0;
	}*/

if (m_Kind == kind_player)  // míi thªm tõ src mobile
 {
#ifdef _SERVER //check PUBG out range
	// [24/08] Doc SubWorld[] NGAY TAI DAY thay vi o dau ham: truoc kia MOI NPC
	// (ke ca quai, chiem da so) deu doc mot o trong mang KSubWorld rat lon roi
	// vut di, vi 'Map' chi duoc dung trong dung nhanh nay.
	const int Map = SubWorld[m_SubWorldIndex].m_SubWorldID;
	if (Map == PUBG_MAP) {
		if (g_MapHandler.getCurrentRadius() > 0 && g_MapHandler.getCurrentRadius() < g_MapHandler.getDistanceToCenter(m_DesX, m_DesY)) {
			//buff skill tru mau
			ExecuteScript("\\script\\tinhnang\\pubg\\pubgutils.lua", "NgoaiBo", this->GetPlayerIdx());
		}
	}
#endif
	if (m_MaskType > 0 && m_MaskMark != 0 && m_MaskMark != m_MaskType)//#mat na
	{
		ReSetRes(1);
		m_MaskMark = 0;
	}
	else if (m_MaskType > 0 && !m_MaskMark)
	{
		ReSetRes(0);
		m_MaskMark = m_MaskType;
	}
	else if (m_MaskType == 0 && m_MaskMark)
	{
		ReSetRes(1);
		m_MaskMark = 0;
	}
} 
 

#ifdef _SERVER
	if (m_nNpcTimeout && g_SubWorldSet.GetGameTime() >= m_nNpcTimeout)
	{
		m_nNpcTimeout = 0;
		if (m_ActionScriptID)
		{
			ExecuteScript(m_ActionScriptID, "OnTimer", m_Index);//#idx cña npc hÕt thêi gian
		}
	}
	// [24/08] Loi goi m_cDeathCalcExp.Active() thu hai o day DA BO - xem chu thich
	// day du o dau ham KNpc::Activate (ngay sau ProcStatus).
#endif

#ifndef _SERVER

	if (m_RegionIndex == -1)
		return;
	//
	//HurtAutoMove(); //bÞ ®¸nh ®au bÞ giùt l¹i
	//
	if (m_MaskMark > 0)//#mat na
	{
		GetNpcCopyFromTemplate(m_MaskType, m_Level, m_Series);
	}
	//
	//AutoFixXY();
	//
	// Â³Â¬Â³Ã¶ÃÂ¬Â²Â½Â¾Ã Ã€Ã«Ã‰Â¾Â³?NpcÂ£Â¬9Ã†ÃÂ£Â¬32Â¸Ã¶Â¸Ã±Ã—Ã“
	// 32 o = 1024 MPS. Voi do phan giai 1024x768 dang chay thi NUA vung nhin doc da
	// ~31,5 o ((768 + WINDAGE_T 90 + WINDAGE_B 150) * 2 / 32 = 63 o) => NPC o MEP
	// TREN/DUOI nam DUNG NGAY nguong nay, bi go khoi region khi VAN CON nhin thay.
	// Nang len 40 de bao het vung nhin + bien an toan cho do phan giai cao hon.
	// NPC ngoai man hinh van bi cat canh khi VE (KIpotLeaf.cpp:122-123) nen chi ton
	// them phan Activate; do that: logic chi chiem 1-27ms MOI GIAY, con nhieu du dia.
	#define	MAX_SYNC_RANGE	40
	// [S12b 28/08] (a) GetMapDis tra VOID_DIS (0x7FFFFFFF) khi CHINH MINH chua duoc dat vao
	// region (vua LoadMap/teleport) - truoc day VOID_DIS >= 40 nen go SACH moi NPC moi frame
	// trong khi S6_XaQuaTam cung dieu kien lai "nhan het" -> flap go-gan ~9Hz (do 27+28/08:
	// 179-208 chu ky <500ms/phien). Self chua dat -> DUNG go, dong quy uoc voi S6_XaQuaTam.
	// (b) Nguong go +2 o so nguong nhan-lai 40 cua S6_XaQuaTam: hai thuoc do (o-nguyen vs
	// mps, per-truc) lech nhau <1 o tai bien -> NPC nam dung vanh 40 bi go-gan moi tick.
	int nS6VDisX = GetMapDisX(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex);
	int nS6VDisY = GetMapDisY(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex);
	if (!IsPlayer() && nS6VDisX != 0x7FFFFFFF && nS6VDisY != 0x7FFFFFFF
		&& (nS6VDisX >= MAX_SYNC_RANGE + 2 || nS6VDisY >= MAX_SYNC_RANGE + 2))
	{
		// [S6 26/08] Duong MO COI thu 2: NPC cach nguoi choi >= 40 o bi go khoi region
		// (khong xoa khoi NpcSet). Day la mot lan duong "bot bien mat".
		AUTOLOG("[S6-VANH] npc=%u idx=%d kind=%u doing=%d cell=(%d,%d) reg=%d t=%u", m_dwID, m_Index, m_Kind, (int)m_Doing, m_MapX, m_MapY, m_RegionIndex, SubWorld[0].m_dwCurrentTime);
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		m_RegionIndex = -1;
		// Go luon LA CAY khoi scene. Truoc day chi go khoi region roi return NGAY -
		// tuc return TRUOC khi goi SetPos ben duoi - nhung la cay van con nen NPC do
		// VAN DUOC VE voi vi tri DONG BANG trong khi hoat anh van chay
		// => "dung yen ma chan di chuyen" (thay ro nhat o MEP khung hinh).
		m_DataRes.Remove(m_Index);
		return;
	}

	if (!IsPlayer() && SubWorld[0].m_dwCurrentTime - m_SyncSignal > 120)
	{
		// [S6 26/08] Duong MO COI thu 3: NPC cam goi sync > 120 tick (~6,7 s) bi go khoi
		// region. XAC (doing=10) chac chan roi vao day vi SyncNpcMin bo qua goi cua NPC chet.
		AUTOLOG("[S6-CAM] npc=%u idx=%d kind=%u doing=%d camtick=%u cell=(%d,%d) reg=%d t=%u", m_dwID, m_Index, m_Kind, (int)m_Doing, SubWorld[0].m_dwCurrentTime - m_SyncSignal, m_MapX, m_MapY, m_RegionIndex, SubWorld[0].m_dwCurrentTime);
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		m_RegionIndex = -1;
		// Go luon LA CAY khoi scene. Truoc day chi go khoi region roi return NGAY -
		// tuc return TRUOC khi goi SetPos ben duoi - nhung la cay van con nen NPC do
		// VAN DUOC VE voi vi tri DONG BANG trong khi hoat anh van chay
		// => "dung yen ma chan di chuyen" (thay ro nhat o MEP khung hinh).
		m_DataRes.Remove(m_Index);
		// [FIX-C 26/08] XAC (do_death/do_revive) cam sync: SyncNpcMin bo qua goi cua NPC
		// chet nen xac khong bao gio duoc cap nhat - truoc day thanh mo coi treo den 55 s.
		// Thoi diem xac BIEN MAT y het cu (cung moc 6,7 s nay - go khoi region la het ve),
		// chi khac: tra khe ngay. Hoi sinh la teleport di cho khac nen khong mat gi.
		if ((m_Doing == do_death || m_Doing == do_revive) && m_Kind != kind_partner)
		{
			AUTOLOG("[S6-XOAXAC] npc=%u idx=%d kind=%u doing=%d t=%u", m_dwID, m_Index, m_Kind, (int)m_Doing, SubWorld[0].m_dwCurrentTime);
			NpcSet.Remove(m_Index);
		}
		return;
	}

#ifndef _SERVER
	// [S9-VE 26/08] Nhan DUY NHAT nhin thay LOP VE. Toan bo he log truoc day chi chup trang
	// thai logic (m_Doing/m_ClientDoing) nen khi nguoi choi bao 'nam bep duoi dat' thi khong
	// the phan biet loi o KNpc, o KNpcRes hay o Represent. Chi ghi cho CHINH NHAN VAT, va chi
	// khi tu the DOI (hoac 2 giay mot lan) nen rat nhe.
	// Doc: resdoing=8 => loi o lop ve; cdoing=8 => loi o KNpc; ca ba deu 1 ma van nam => Represent.
	if (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		static DWORD s_uS9VeT = 0;
		static int s_nS9VeCu = -999;
		DWORD uS9Now = timeGetTime();
		if (s_nS9VeCu != (int)m_ClientDoing || (DWORD)(uS9Now - s_uS9VeT) >= 2000)
		{
			s_uS9VeT = uS9Now;
			s_nS9VeCu = (int)m_ClientDoing;
			AUTOLOG("[S9-VE] doing=%d cdoing=%d resdoing=%d resaction=%d frame=%d/%d reg=%d t=%u", (int)m_Doing, (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(), m_Frames.nCurrentFrame, m_Frames.nTotalFrame, m_RegionIndex, SubWorld[0].m_dwCurrentTime);
		}
	}
#endif
	m_DataRes.SetAction(m_ClientDoing);
#ifndef _SERVER
	// [S9-KET r2 26/08] Ban tren ghi TRUOC khi doi hoat anh nen luon tre mot nhip (do that
	// 7/7 ca deu tu hoi ngay sau do => bao dong gia). Cho nay ghi SAU khi da goi SetAction:
	// neu lop ve VAN khong khop voi trang thai logic thi la KET THAT (SetAction that bai -
	// vd m_pcResNode = NULL sau khi la cay bi go). Chi ghi cho chinh nhan vat, va chi khi
	// KET LIEN TIEP 3 nhip tro len de khoi bao dong vi mot khung le.
	if (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		static int s_nS9KetDem = 0;
		if (m_DataRes.GetResDoing() != (int)m_ClientDoing)
		{
			s_nS9KetDem++;
			if (s_nS9KetDem == 3 || (s_nS9KetDem > 3 && (s_nS9KetDem % 18) == 0))
				AUTOLOG("[S9-KET] LOP VE KHONG DOI DUOC: cdoing=%d resdoing=%d resaction=%d doing=%d lien tiep %d nhip t=%u", (int)m_ClientDoing, m_DataRes.GetResDoing(), m_DataRes.GetAction(), (int)m_Doing, s_nS9KetDem, SubWorld[0].m_dwCurrentTime);
		}
		else if (s_nS9KetDem)
		{
			if (s_nS9KetDem >= 3)
				AUTOLOG("[S9-KET-HET] lop ve da khop lai sau %d nhip (cdoing=%d) t=%u", s_nS9KetDem, (int)m_ClientDoing, SubWorld[0].m_dwCurrentTime);
			s_nS9KetDem = 0;
		}
	}
#endif
	m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType, m_MantleType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);	
	m_DataRes.SetState(m_btStateInfo, &g_NpcResList); //fix by phong kiÒu

	int		nMpsX, nMpsY;
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index)
	{
		SubWorld[0].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, g_bPaintInterpFocus ? FALSE : TRUE);
	}
	else
	{
		SubWorld[0].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, FALSE);
	}

	if (m_Kind == kind_bird || m_Kind == kind_mouse)
		m_SyncSignal = SubWorld[0].m_dwCurrentTime;

	if (m_nChatContentLen > 0)
	{
		if (IR_GetCurrentTime() - m_nCurChatTime > NPC_SHOW_CHAT_TIME)
		{
			m_nChatContentLen = 0;
			m_nChatNumLine = 0;
			m_nChatFontWidth = 0;
			m_nCurChatTime = 0;
		}
	}
#endif
}

void	KNpc::ReSetRes(int nMark)
{
#ifndef _SERVER
	char	szNpcTypeName[32];
#endif
	if (nMark == 1)
	{
#ifndef _SERVER
		if (m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "MainMan");
			//strcpy(szNpcTypeName, "ÄÐÖ÷½Ç");		kiÓu npc kieu npc bao gåm nam vµ n÷ man and lady
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "MainLady");
			//strcpy(szNpcTypeName, "Å®Ö÷½Ç");		kiÓu npc kieu npc bao gåm nam vµ n÷ man and lady
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(FALSE);
		}
#endif
		//		TODO: Load Player Data;
		m_WalkSpeed = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	= NpcSet.GetPlayerHurtFrame();
	}
	else
	{
		GetNpcCopyFromTemplate(m_MaskType, m_Level, m_Series);
#ifndef _SERVER	
		g_NpcSetting.GetString(m_MaskType + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])
		{
			g_NpcKindFile.GetString(2, "ÈËÎïÃû³Æ", "", szNpcTypeName, sizeof(szNpcTypeName));
		}
#endif
	}
#ifndef _SERVER
	m_DataRes.Remove(m_Index);
	m_DataRes.Init(szNpcTypeName, &g_NpcResList);
	m_DataRes.SetAction(m_ClientDoing);
	m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType, m_MantleType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);
#endif
}

void KNpc::ProcStatus()
{
	//g_DebugLog("[DEATH] m_bExchangeServer: %d", m_bExchangeServer);
	if (m_bExchangeServer)
		return;

	switch(m_Doing)
	{
	case do_stand:
		OnStand();
		break;
	case do_run:
		OnRun();
		break;
	case do_walk:
		OnWalk();
		break;
	case do_attack:
	case do_magic:
		OnSkill();
		break;
	case do_sit:
		OnSit();
		break;
	case do_jump:
		OnJump();
		break;
	case do_hurt:
		OnHurt();
		break;
	case do_revive:
		OnRevive();
		break;
	case do_death:
		OnDeath();
		break;
	case do_defense:
		OnDefense();
		break;
	case do_special1:
		OnSpecial1();
		break;
	case do_special2:
		OnSpecial2();
		break;
	case do_special3:
		OnSpecial3();
		break;
	case do_blurmove:
		OnBlurMove();
		break;
	case do_manyattack:
		OnManyAttack();
		break;
	case do_runattack:
		OnRunAttack();
		break;
	case do_jumpattack:
		OnJumpAttack();
		break;
	case do_idle:
		OnIdle();
	default:
		break;
	}
}

void KNpc::ProcCommand(int nAI)
{
	AUTOLOG_EVERY(2000, "[E4_PROCCMD_IN] npc=%d id=%u ai=%d cmd=%d p=(%d,%d,%d) exch=%d rgn=%d doing=%d", m_Index, m_dwID, nAI, (int)m_Command.CmdKind, m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z, m_bExchangeServer, m_RegionIndex, (int)m_Doing);
	if (m_Command.CmdKind == do_none || m_bExchangeServer)
		return;

	if (nAI)
	{
		AUTOLOG_EVERY(1000, "[E4_PROCCMD_NOREGION] npc=%d id=%u cmd=%d rgn=%d sw=%d map=(%d,%d)", m_Index, m_dwID, (int)m_Command.CmdKind, m_RegionIndex, m_SubWorldIndex, m_MapX, m_MapY);
		if (m_RegionIndex < 0)
			return;
		switch (m_Command.CmdKind)
		{
		case do_stand:
			DoStand();
			break;
		case do_walk:
			Goto(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_run:
			RunTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_jump:
			JumpTo(m_Command.Param_X, m_Command.Param_Y);
			break;
		case do_skill:
			if (IsPlayer())
				AUTOLOG_IDX_EVERY(m_Index, 1000, "[S2-SKILL-NOTLEARNED] npc=%d id=%u skill_req=%d found_idx=%d p2=%d p3=%d doing=%d fight=%d", m_Index, m_dwID, m_Command.Param_X, m_SkillList.FindSame(m_Command.Param_X), m_Command.Param_Y, m_Command.Param_Z, (int)m_Doing, (int)m_FightMode);
			if (IsPlayer())
				AUTOLOG_IDX(m_Index, "[S3-PROC-FINDSAME] npc=%d plr=%d reqskill=%d slot=%d active=%d doing=%d p=(%d,%d)", m_Index, m_nPlayerIdx, m_Command.Param_X, m_SkillList.FindSame(m_Command.Param_X), m_ActiveSkillID, (int)m_Doing, m_Command.Param_Y, m_Command.Param_Z);
			if (int nSkillIdx = m_SkillList.FindSame(m_Command.Param_X))
			{
				if (IsPlayer())
					AUTOLOG_IDX_EVERY(m_Index, 500, "[S3-SETACTIVE] npc=%d plr=%d slot=%d slotskill=%d lv=%d addlv=%d curlv=%d actbefore=%d radbefore=%d", m_Index, m_nPlayerIdx, nSkillIdx, m_SkillList.m_Skills[nSkillIdx].SkillId, m_SkillList.m_Skills[nSkillIdx].SkillLevel, m_SkillList.m_Skills[nSkillIdx].AddLevel, m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel, m_ActiveSkillID, m_CurrentAttackRadius);
				SetActiveSkill(nSkillIdx);
				DoSkill(m_Command.Param_Y, m_Command.Param_Z);
			}
			else
			{
				DoStand();
			}
			break;
		case do_sit:
			DoSit();
			break;
		case do_defense:
			DoDefense();
			break;
		case do_idle:
			DoIdle();
			break;
		case do_hurt:
			if (m_RegionIndex >= 0)
				DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
#ifndef _SERVER
			else // ERROR
				DoStand();
#endif
			break;
		case do_revive:
			{
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
        	}
			break;
		case  do_blurmove:
				DoBlurMove();
			break;
		}
	}
	else
	{
		switch(m_Command.CmdKind)
		{
		case do_hurt:
			if (m_RegionIndex >= 0)
				DoHurt(m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z);
			break;
		case do_revive:
			{
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
			}
			break;
		case do_walk:
			if (m_RandMove.nTime > 0)
				Goto(m_Command.Param_X, m_Command.Param_Y);
			else if (0 == m_ProcessAI && IsPlayer() && m_Doing == do_stand /* && Player[m_nPlayerIdx].m_cFaction.GetCurFactionNo() <= 2 */)
				m_ProcessAI = 1;
			break;

		case do_skill: // Linq Sau khi luot bi do, ket cac skill tao du anh
		case do_run:
			if (IsPlayer() && m_Command.CmdKind == do_skill)
				AUTOLOG_IDX(m_Index, "[S3-CMD-SWALLOW] npc=%d plr=%d cmd=%d skill=%d p=(%d,%d) doing=%d procai=%d rgn=%d", m_Index, m_nPlayerIdx, (int)m_Command.CmdKind, m_Command.Param_X, m_Command.Param_Y, m_Command.Param_Z, (int)m_Doing, m_ProcessAI, m_RegionIndex);
			if (0 == m_ProcessAI && IsPlayer() && m_Doing == do_stand /* && Player[m_nPlayerIdx].m_cFaction.GetCurFactionNo() <= 2 */ )
				m_ProcessAI = 1;
			// FIX 24/08: ProcCommand chay TRUOC ProcStatus (KNpc.cpp:598-599). Dung khung ma OnSkill
			// ket thuc hoat anh (WaitForFrame tra TRUE roi DoStand + m_ProcessAI=1) thi lenh vua toi
			// bi dong cuoi ham xoa mat, phai cho client gui lai ~1 khung. Giu lai DUNG khung do.
			// Bat buoc co nTotalFrame > 0: neu = 0 thi OnSkill khong bao gio DoStand => lenh ket vinh vien.
			if (IsPlayer() && m_Command.CmdKind == do_skill && 0 == m_ProcessAI &&
				(m_Doing == do_attack || m_Doing == do_magic) &&
				m_Frames.nTotalFrame > 0 &&
				m_Frames.nCurrentFrame + 1 >= m_Frames.nTotalFrame)
				return;
			break;
		default:
			break;
		}
	}
	m_Command.CmdKind = do_none;
}

BOOL KNpc::ProcessState()
{
	int nRet = FALSE;
	if (m_RegionIndex < 0)
		return FALSE;

	if (!(m_LoopFrames % GAME_FPS))
	{
		Player[m_nPlayerIdx].ProcessDouble(); //#time x2 Exp chÕt kh«ng mÊt
#ifdef _SERVER
		// [KM 28/08] Khi Doanh Dan Dien: het han GIUA PHIEN thi go hieu ung ngay,
		// truoc day phai thoat vao lai moi mat. Ham co co chan nen khong doi
		// trang thai la return luon, gan nhu khong ton gi.
		if (m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER && GetKind() == kind_player)
			Player[m_nPlayerIdx].CapNhatKhiDoanh();
#endif
	}
/*
#ifdef _SERVER
	if (!(g_SubWorldSet.GetGameTime() % (GAME_FPS * 3)))
	{
		UpdateNpcStateInfo();
	}
#endif		*/

	if (!(m_LoopFrames % GAME_UPDATE_TIME))
	{
#ifdef _SERVER
		if (m_Doing == do_sit)
		{
			int nLifeAdd = m_CurrentLifeMax * 3 / 1000;
			if (nLifeAdd <= 0)
				nLifeAdd = 1;
			m_CurrentLife += nLifeAdd;
			if (m_CurrentLife > m_CurrentLifeMax)
				m_CurrentLife = m_CurrentLifeMax;

			int nManaAdd = m_CurrentManaMax * 3 / 1000;
			if (nManaAdd <= 0)
				nManaAdd = 1;
			m_CurrentMana += nManaAdd;
			if (m_CurrentMana > m_CurrentManaMax)
				m_CurrentMana = m_CurrentManaMax;

			m_CurrentStamina += PlayerSet.m_cPlayerStamina.m_nSitAdd;
			if (m_CurrentStamina > m_CurrentStaminaMax)
				m_CurrentStamina = m_CurrentStaminaMax;
		}
		
		if (m_StunState.nTime <= 0)
		{
			m_CurrentLife += m_CurrentLifeReplenish + (m_CurrentLifeReplenish * m_CurrentLifeReplenishPercent / MAX_PERCENT);
			if (m_CurrentLife > m_CurrentLifeMax)
				m_CurrentLife = m_CurrentLifeMax;
			else if(m_CurrentLife < 0)
				m_CurrentLife = 0;
		
			// [PF 31/08k] manareplenish_p (254): hoi noi luc theo %, doi xung voi ve % cua sinh luc o tren
			m_CurrentMana += m_CurrentManaReplenish + (m_CurrentManaReplenish * m_CurrentManaReplenishPercent / MAX_PERCENT);
			if (m_CurrentMana > m_CurrentManaMax)
				m_CurrentMana = m_CurrentManaMax;
			else if(m_CurrentMana < 0)
				m_CurrentMana = 0;
		
			if (m_Doing == do_stand)
			{
				m_CurrentStamina += m_CurrentStaminaGain;
			}
			else
			{
				if (m_nPKFlag < enumPKMurder)// kh«ng ph¶i tr¹ng th¸i giÕt ng­êi thÓ lùc håi phôc
					m_CurrentStamina += m_CurrentStaminaGain / STAMINA_RECOVER_SCALE;				
			}
			if (m_CurrentStamina > m_CurrentStaminaMax)
				m_CurrentStamina = m_CurrentStaminaMax;
			else if(m_CurrentStamina < 0)
				m_CurrentStamina = 0;
		}
#endif
	
		if (m_ActiveAuraID)
		{
			if (m_SkillList.GetLevel(m_ActiveAuraID) > 0)
			{
				int nCurLevel = m_SkillList.GetCurrentLevel(m_ActiveAuraID);

				int nMpsX, nMpsY;
				SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);
				//if (m_ActiveAuraID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL) //#chua hoan thien
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nCurLevel);
#ifdef _SERVER
				NPC_SKILL_SYNC SkillCmd;
				SkillCmd.ID = this->m_dwID;
				if (pOrdinSkill)
				{
					//if (pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles)
					SkillCmd.nSkillID = pOrdinSkill->GetChildSkillId();
				}
				else
				{
					SkillCmd.nSkillID = 0;
				}

				SkillCmd.nSkillLevel = nCurLevel;
				SkillCmd.nMpsX = -1;
				SkillCmd.nMpsY = m_dwID;
				SkillCmd.ProtocolType = s2c_castskilldirectly;
				static const POINT	POff[8] = 
				{
					{0, 32},
					{-16, 32},
					{-16, 0},
					{-16, -32},
					{0, -32},
					{16, -32},
					{16, 0},
					{16, 32},
				};
				
				int nMaxCount = MAX_BROADCAST_COUNT;
				CURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);
				int i;
				for (i = 0; i < 8; i++)
				{
					if (CONREGIONIDX(i) == -1)
						continue;
					CONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
				}
#endif				
				KSkill * pOrdinSkill1 = pOrdinSkill;	// [24/08] tra dung con tro da lay o tren
								// (cung m_ActiveAuraID + nCurLevel, GetSkill chi la tra bang)
				int nChildSkillId = 0;
				if (pOrdinSkill1)
				{
					nChildSkillId = pOrdinSkill1->GetChildSkillId();
					
					KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nCurLevel);
					if (pOrdinSkill2)
					{
						pOrdinSkill2->Cast(m_Index, nMpsX, nMpsY);
					}
				}
				if(pOrdinSkill && pOrdinSkill->GetAppendSkillNum())
				{
					for(int j = 0; j < pOrdinSkill->GetAppendSkillNum(); j++)
					{
						int nAppendId = pOrdinSkill->GetAppendSkillId(j);
						int nAppendLv = m_SkillList.GetCurrentLevel(nAppendId);
						if(nAppendLv > nCurLevel)
							nAppendLv = nCurLevel;
						if (nAppendId < MAX_SKILL && nAppendLv > 0 && nAppendLv < MAX_SKILLLEVEL) //#chua hoan thien
						{
#ifdef _SERVER
							KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(nAppendId, nAppendLv);
							if (pOrdinSkill1)
							{
								SkillCmd.nSkillID = pOrdinSkill1->GetChildSkillId();
							}
							else
							{
								SkillCmd.nSkillID = 0;
							}
							SkillCmd.nSkillLevel = nAppendLv;
							SkillCmd.nMpsX = -1;
							SkillCmd.nMpsY = m_dwID;
							SkillCmd.ProtocolType = s2c_castskilldirectly;

							static const POINT	POff[8] = 
							{
								{0, 32},
								{-16, 32},
								{-16, 0},
								{-16, -32},
								{0, -32},
								{16, -32},
								{16, 0},
								{16, 32},
							};
							
							int nMaxCount = MAX_BROADCAST_COUNT;
							CURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);
							int i;
							for (i = 0; i < 8; i++)
							{
								if (CONREGIONIDX(i) == -1)
									continue;
								CONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
							}
#endif				
							KSkill * pOrdinSkill2 = (KSkill *) g_SkillManager.GetSkill(nAppendId, nAppendLv);
							int nChildSkillId = 0;
							if (pOrdinSkill2)
							{
								nChildSkillId = pOrdinSkill2->GetChildSkillId();
				
								KSkill * pOrdinSkill3 = (KSkill *) g_SkillManager.GetSkill(nChildSkillId, nAppendLv);
								if (pOrdinSkill3)
								{
									pOrdinSkill3->Cast(m_Index, nMpsX, nMpsY);
								}
							}
						}
					}
				}
			}
		}
	}	

#ifdef _SERVER
	if (m_PoisonState.nTime > 0)
	{
		m_PoisonState.nTime--;
		m_PoisonState.nMagicAttrib++;
		if (m_PoisonState.nValue[1] == 0)
		{
			m_PoisonState.nValue[1] = 1;
		}
		if (!(m_PoisonState.nMagicAttrib % m_PoisonState.nValue[1]))
		{
			CalcDamage(m_nLastPoisonDamageIdx, m_PoisonState.nValue[0], m_PoisonState.nValue[0], damage_poison, -1, FALSE, FALSE, TRUE);
		}
		if(m_PoisonState.nTime == 0)
		{
			m_PoisonState.nValue[0] = 0;
			m_PoisonState.nValue[1] = 0;
			m_PoisonState.nMagicAttrib = 0;
		}
	}

	if (m_FreezeState.nTime > 0)
	{
		m_FreezeState.nTime--;
		if (m_FreezeState.nTime & 1)
		{
			nRet = TRUE;
		}
	}

	if (m_BurnState.nTime > 0)
	{
		m_BurnState.nTime--;
		if (m_BurnState.nValue[1] == 0)
		{
			m_BurnState.nValue[1] = 1;
		}
		if (!(m_BurnState.nTime % m_BurnState.nValue[1]))
		{
			CalcDamage(m_Index, m_BurnState.nValue[0], m_BurnState.nValue[0], damage_fire, -1, FALSE, FALSE, TRUE);
		}
	}

	if (m_FrozenAction.nTime > 0)
	{
		m_FrozenAction.nTime--;
	}

	if (m_RandMove.nTime > 0)
	{
		m_ProcessAI	= 0;
		if (!(g_SubWorldSet.GetGameTime() % GAME_UPDATE_TIME))
		{
			int nDesX, nDesY;
			GetMpsPos(&nDesX, &nDesY);
			int nRan = ::GetRandomNumber(0,1);
			if (nRan)
				nDesX -= g_Random(100);
			else
				nDesX += g_Random(100);
				
			nRan = ::GetRandomNumber(0,1);
			if (nRan)
				nDesY -= g_Random(100);
			else
				nDesY += g_Random(100);	
			SendCommand(do_walk, nDesX, nDesY);
		}
		m_RandMove.nTime--;
		if(m_RandMove.nTime == 0)   // them moi tiktok
			m_ProcessAI	= 1;
	}
	
	if (m_StunState.nTime > 0)
	{
		m_StunState.nTime--;
		nRet = TRUE;
	}

	if (m_LifeState.nTime > 0)
	{
		m_LifeState.nTime--;
		if (!(m_LifeState.nTime % GAME_UPDATE_TIME))
		{
			m_CurrentLife += m_LifeState.nValue[0];
			if (m_CurrentLife > m_CurrentLifeMax)
			{
				m_CurrentLife = m_CurrentLifeMax;
			}
			else if(m_CurrentLife < 0)
				m_CurrentLife = 0;
		}
	}

	if (m_ManaState.nTime > 0)
	{
		m_ManaState.nTime--;
		if (!(m_ManaState.nTime % GAME_UPDATE_TIME))
		{
			m_CurrentMana += m_ManaState.nValue[0];
			if (m_CurrentMana > m_CurrentManaMax)
			{
				m_CurrentMana = m_CurrentManaMax;
			}
			else if(m_CurrentMana < 0)
				m_CurrentMana = 0;
		}
	}

	if (m_LoseMana.nTime > 0)
	{
		m_LoseMana.nTime--;
		if (!(m_LoseMana.nTime % GAME_FPS))
		{
			m_CurrentMana -= m_LoseMana.nValue[0];
			if (m_CurrentMana < 0)
				m_CurrentMana = 0;
		}
	}

	if (m_HideState.nTime > 0)
	{
		m_HideState.nTime --;
	}

	if (m_SilentState.nTime > 0)
	{
		m_SilentState.nTime --;
	}

	if (m_WalkRun.nTime > 0)
	{
		m_WalkRun.nTime --;
	}
	//
	if(m_nProtectedTime > 0)			//vong tron bat tu, vßng trßn bÊt tö
	{
		m_nProtectedTime --;
	}
	//
	if(m_nTime_Ignorenegativestate > 0) //3 gi©y lo¹i bá tr¹ng th¸i dÞ th­êng xuÊt ø bÊt diÔm
	{
		m_nTime_Ignorenegativestate --;
	}
#endif

#ifndef _SERVER
	bool bAdjustColorId = false;
	//
	if (m_FreezeState.nTime > 0)
	{
		if (SubWorld[0].m_dwCurrentTime & 1)
			nRet = TRUE;
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_freeze);
		bAdjustColorId = true;
	}
	//
	if(m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		if (m_RandMove.nTime > 0)
		{
			m_ProcessAI	= 0;
			if (!(g_SubWorldSet.GetGameTime() % GAME_UPDATE_TIME))
			{
				int nDesX, nDesY;
				GetMpsPos(&nDesX, &nDesY);
				int nRan = ::GetRandomNumber(0,1);
				if (nRan)
					nDesX -= g_Random(100);   //them moi tiktok
				else
					nDesX += g_Random(100);
				
				nRan = ::GetRandomNumber(0,1);
				if (nRan)
					nDesY -= g_Random(100);
				else
					nDesY += g_Random(100);
				SendCommand(do_walk, nDesX, nDesY);
			}
			m_RandMove.nTime--;
			if(m_RandMove.nTime == 0)
			m_ProcessAI	= 1;
		}
	}
	//
	if (m_StunState.nTime > 0)
	{
		m_DataRes.SetSpecialSpr("\\spr\\skill\\²¹³ä\\mag_spe_Ñ£ÔÎ.spr");
		nRet = TRUE;
	}
	//
	if (m_PoisonState.nTime > 0)
	{
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_poison);
		bAdjustColorId = true;
	}
	//
	if (m_BurnState.nTime > 0)
	{
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_burn);
		bAdjustColorId = true;
	}
	//
	if (!bAdjustColorId)
		m_DataRes.SetAdjustColorId(KNpcRes::adjustcolor_physics);
#endif
#ifndef _SERVER
	if(IsPlayer())
	{
#endif
#ifdef _SERVER
	bool bStateRemove = false;
#endif
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetPrev();
		//
		if (pTempNode->m_LeftTime == -1)	
			continue;
		//
		if (pTempNode->m_LeftTime == 0)
		{
			int i;
			for (i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
				{
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
				}
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;
			pTempNode = NULL;
			//
#ifdef _SERVER
			bStateRemove = true;
#endif
			continue;
		}
		else
		{
			pTempNode->m_LeftTime --;
			if(pTempNode->m_LeftTimeH >= 0)
				pTempNode->m_LeftTimeH --;
		}
	}
#ifdef _SERVER
	if(bStateRemove)
	{
		UpdateNpcStateInfo();
		//BroadCastState();
	}
#endif
#ifndef _SERVER
	}
#endif
	return nRet;
}

void KNpc::DoDeath(int nMode/* = 0*/, int nAttacker)
{
	//_ASSERT(m_Doing != do_death);
	// do_death == 10
	//g_DebugLog("[DEATH] m_Doing: %d", m_Doing);
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;
	//
	if (m_Doing == do_death)
		return;
	//
	if (IsPlayer() && !m_FightMode)	
	{
		m_CurrentLife = 1;
		return;
	}
	//
	if (IsPlayer())//#mat na
	{
		Player[m_nPlayerIdx].m_ItemList.SetMaskLock(FALSE);
		int nIdx = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_mask);
		m_MaskType = Item[nIdx].GetBaseMagic();
	}
	//
#ifndef _SERVER
	//if (this->m_Kind == kind_normal)
		//this->SetBlood(this->m_CurrentLife);
#endif
	//
	m_Doing = do_death;
	m_ProcessAI	= 0;
	m_ProcessState = 0;
	//
	m_Frames.nTotalFrame = m_DeathFrame;
	m_Frames.nCurrentFrame = 0;
	m_Height = 0;
	//
#ifdef _SERVER
	// [S7 26/08] luong chet NGUOI THAT (net>=0, bo ~1000 bot). Chu game: "danh TK lau
	// chet 0 mau roi vai giay sau moi ve duoc thanh" - do timeline tu day.
	if (IsPlayer() && m_nPlayerIdx > 0 && Player[m_nPlayerIdx].m_nNetConnectIdx >= 0)
		AUTOLOG("[S7-CHET] id=%u deathframe=%d mode=%d attacker=%d t=%u", m_dwID, m_DeathFrame, nMode, nAttacker, SubWorld[0].m_dwCurrentTime);
	int nPlayer = 0;
	//
	if (this->m_Kind != kind_player && nAttacker > 0 && nAttacker < MAX_NPC)
	{
		nPlayer = m_cDeathCalcExp.CalcExp(nAttacker);
	}
	//
	DeathPunish(nMode, nPlayer);
	//
	NPC_DEATH_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcdeath;
	NetCommand.ID = m_dwID;
	//
	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	//
	// [S11 26/08] goi CHET mien ngan sach (xem chu thich NPC_EVENT_BROADCAST_LIMIT).
	int nMaxCount = NPC_EVENT_BROADCAST_LIMIT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
	m_ClientDoing = cdo_death;
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx == m_Index)
	{
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = 0;
	}
#endif
	if (IsPlayer())
	{
		//Not Finish
	}
	else
	{
		if (DropRateScript[0])
		{
			ExecuteScript2(DropRateScript, "DropRate", m_Index, Npc[m_nLastDamageIdx].m_nPlayerIdx); //#idx cÃ±a player tÃŠn cÂ«ng cuÃ¨i cÃ¯ng
		}
	}
}

void KNpc::OnDeath()
{
	if (WaitForFrame())
	{
		//g_DebugLog("[DEATH] WaitForFrame TRUE");
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;		
#ifndef _SERVER
		if (!IsPlayer())
		{
            m_ProcessAI = 1;
        }
#else
		if (IsPlayer())
		{
			// [S7 26/08] het hoat anh chet -> chay death-script (TK dua ve trai o day).
			if (Player[m_nPlayerIdx].m_nNetConnectIdx >= 0)
				AUTOLOG("[S7-CHET2] id=%u het hoat anh chet -> chay death-script t=%u", m_dwID, SubWorld[0].m_dwCurrentTime);
			if (Player[m_nPlayerIdx].m_dwDeathScriptId)
			{
				// [JX2COMPAT 22/08] script chet cua Linux dat OnDeath(Launcher) (citywar_arena, tongwar,
				// bw, messenger playerdead_tollgate); JX1 dat OnPlayerDeath(pidx, killer). Do ham co that.
				char szDeathFun[16];
				strcpy(szDeathFun, "OnPlayerDeath");
				{
					KLuaScript* pDs = (KLuaScript*)g_GetScript(Player[m_nPlayerIdx].m_dwDeathScriptId);
					if (pDs && pDs->m_LuaState)
					{
						int nTopD = Lua_GetTopIndex(pDs->m_LuaState);
						Lua_GetGlobal(pDs->m_LuaState, "OnPlayerDeath");
						if (!lua_isfunction(pDs->m_LuaState, Lua_GetTopIndex(pDs->m_LuaState)))
							strcpy(szDeathFun, "OnDeath");
						lua_settop(pDs->m_LuaState, nTopD);
					}
				}
				if (strcmp(szDeathFun, "OnDeath") == 0)
					Player[m_nPlayerIdx].ExecuteScript(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnDeath", m_nLastDamageIdx);
				else
				Player[m_nPlayerIdx].ExecuteScript2(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnPlayerDeath", m_nPlayerIdx, m_nLastDamageIdx); //#khi player bi chet
			}
		}
		else  if (Npc[m_nLastDamageIdx].IsPlayer())
		{
			int nIdx = Npc[m_nLastDamageIdx].m_nPlayerIdx;
			if (nIdx)
			{
				if (ActionScript[0])
				{
					ExecuteScript2(ActionScript, "OnDeath", m_Index, m_nLastDamageIdx); //#khi player giet chet npc hoac player
				}
								
				// Truoc day moc nay goi thang vao \script\tinhnang\datau\danhquai.lua cua he Da Tau CU.
				// He do da go (ban thay = script\task\newtask\tasklink, port 15-16/08); phan con
				// phai chay moi lan giet quai chi la moc dem cua Ban Dong Hanh, nay o onkillnpc.lua.
				// CHU Y: ham nay chay MOI LAN co NPC chet vi tay nguoi choi - dung them viec nang.
				ExecuteScript2("\\script\\global\\onkillnpc.lua", "OnPlayerKillNpc", m_Index, m_nLastDamageIdx);	// [DONDATAU 30/08] moc "nguoi choi giet NPC".
			}
		}

		if (!IsPlayer())
		{
			this->m_cGold.RecoverBackData();
		}
#endif

		if (m_Kind != kind_partner)
		{
			DoRevive();
#ifdef _SERVER
			// [PORT5 23/08] AddNpcEx bNoRevive: xac chi nan ~1s roi OnRevive go NPC (Linux xoa
			// ngay sau hoat anh chet qua hang doi 0x3E9 - 0x08083520)
			if (!IsPlayer() && m_bNoRevive && m_Doing == do_revive)
				m_Frames.nTotalFrame = 18;
			int nPIdx = GetPlayerIdx();
			if(nPIdx > 0)
			{
				// [REV 26/08] -1 = nguoi choi DA bam ve thanh trong luc hoat anh chet
				// -> hoi sinh lien o Active(); con lai giu auto 5 s nhu cu.
				Player[nPIdx].m_nAutoRevTime = (Player[nPIdx].m_nAutoRevTime == -1) ? (g_SubWorldSet.GetGameTime() - 1) : (g_SubWorldSet.GetGameTime() + 90);//5s
			}
#endif		
#ifndef _SERVER
			if (m_Kind != kind_player)
			{
				SubWorld[0].m_WorldMessage.Send(GWM_NPC_DEL, m_Index); //xoa npc khi chet edit by phong kieu
				return;
			}
#endif		
		}
		else	
		{
			// [BDH 27/08] dong hanh HON ME: khong hoi sinh, khong go theo duong thuong.
			// Partner_OnNpcDeath dat phat PUNISH_TIME + goi partner_action.lua OnDeath.
#ifdef _SERVER
			Partner_OnNpcDeath(m_Index);
#endif
		}
	}
	else
	{
		//g_DebugLog("[DEATH] WaitForFrame FALSE");
	}
}

void KNpc::DoDefense()
{
	m_ProcessAI = 0;
}

void KNpc::OnDefense()
{
}

void KNpc::DoIdle()
{
	if (m_Doing == do_idle)
		return;
	m_Doing = do_idle;
}

void KNpc::OnIdle()
{
}

void KNpc::DoHurt(int nHurtFrames, int nX, int nY,int nHurtI)
{
	_ASSERT(m_RegionIndex >= 0);
#ifndef _SERVER
	m_DataRes.SetBlur(FALSE);
#endif
	if (m_RegionIndex < 0)
		return;
		
	if ((m_Doing == do_hurt && nHurtI <= 100) || m_Doing == do_death || m_Doing == do_runattack || m_Doing == do_goattack)
		return;
		

#ifdef _SERVER
	int giam_tho_thuong = 0;
	if (m_CurrentHitRecover <= 80)
    {
		// [PHUCHOI 01/09] A2 - bo lam tron xuong boi 10: moi diem le duoi 10 bi vut
		// trang (nen nguoi choi 6 + full mach 28 = 34 -> chi an 30), va mon 'thoi gian
		// phuc hoi' cap thap 1-9 diem KHONG doi gi het cho toi khi vuot boi 10 ke tiep.
		// Day la trieu chung chu game bao "thoi gian phuc hoi chua dung tac dung".
		// Giu nguyen nhanh tran 80 ben duoi de khong doi can bang dau tren.
		giam_tho_thuong = m_CurrentHitRecover;
    }
	else
    {
		giam_tho_thuong = 80;
    }
	// [KM 27/08] anti_hitrecover cua NGUOI DANH triet tieu bot suc hoi phuc cua nan nhan.
	// Linux 0x0807FA30 lay tri nay lam THOI LUONG dong tac bi thuong dat len nan nhan.
	if (m_nKMAntiHitRecover > 0)
	{
		giam_tho_thuong -= m_nKMAntiHitRecover;
		if (giam_tho_thuong < 0)
			giam_tho_thuong = 0;
		m_nKMAntiHitRecover = 0;
	}
	//
	if (!g_RandPercent(nHurtI * 3 / 4 + 60 - giam_tho_thuong / 4))
	//if (!g_RandPercent(nHurtI / 2 + 50 - giam_tho_thuong / 2))
	{
		return;
	}
#endif
	m_Doing = do_hurt;
	m_ProcessAI	= 0;

#ifdef _SERVER
	m_Frames.nTotalFrame = m_HurtFrame  * (100 - giam_tho_thuong) / 100;
//	m_Frames.nTotalFrame = 18 * m_HurtFrame * nHurtI *(100 - giam_tho_thuong)/ 100000;
#else
	m_ClientDoing = cdo_hurt;
	m_Frames.nTotalFrame = nHurtFrames;
	m_nHurtDesX = nX;
	m_nHurtDesY = nY;
	if (m_Height > 0)
	{
		m_nHurtHeight = m_Height;
	}
	else
	{
		m_nHurtHeight = 0;
	}
#endif
	if (m_Frames.nTotalFrame == 0)
		m_Frames.nTotalFrame = 1;
	m_Frames.nCurrentFrame = 0;

#ifdef _SERVER
	NPC_HURT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npchurt;
	NetCommand.ID = m_dwID;
	NetCommand.nFrames = m_Frames.nTotalFrame;

	GetMpsPos(&NetCommand.nX, &NetCommand.nY);  // thªm míi tõ src mobile 

	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif
}

void KNpc::OnHurt()
{
	if (m_RegionIndex < 0)
	{
		g_DebugLog("[error]%s Region Index < 0 when hurt", Name);
		return;
	}
	int nX, nY;
	GetMpsPos(&nX, &nY);
#ifdef _SERVER
	m_Height = 0;
#endif
#ifndef _SERVER
	if(m_Frames.nTotalFrame > 0)
	{
	m_Height = m_nHurtHeight * (m_Frames.nTotalFrame - m_Frames.nCurrentFrame - 1) / m_Frames.nTotalFrame;
	nX = nX + (m_nHurtDesX - nX) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;
	nY = nY + (m_nHurtDesY - nY) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;

	int nOldRegion = m_RegionIndex;
	//SetPos(nX, nY);
	//CURREGION.DecRef(m_MapX, m_MapY, obj_npc);
	SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	nRegion = -1;
	nMapX = nMapY = nOffX = nOffY = 0;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion == -1)
	{
		SubWorld[0].m_Region[nOldRegion].RemoveNpc(m_Index);
		m_dwRegionID = 0;
	}
	else if (nOldRegion != nRegion)
	{
		m_RegionIndex = nRegion;
		m_MapX = nMapX;
		m_MapY = nMapY;
		m_OffX = nOffX;
		m_OffY = nOffY;
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
	}
	if (nRegion >= 0)
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
}
#endif

	if (WaitForFrame())
	{
		//g_DebugLog("[DEATH]On Hurt Finished");
		DoStand();
		m_ProcessAI = 1;
	}
}

void KNpc::DoSpecial1()
{
	DoBlurAttack();
}

void KNpc::OnSpecial1()
{	
	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (pSkill)
		{
			int nChildSkill = pSkill->GetChildSkillId();
			int nChildSkillLevel = pSkill->m_ulLevel;
			
			if (nChildSkill > 0)
			{
				KSkill * pChildSkill = (KSkill*)g_SkillManager.GetSkill(nChildSkill, nChildSkillLevel);
				if (pChildSkill)
				{
					pChildSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
				}
			}
		}

		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
}

void KNpc::DoSpecial2()
{
}

void KNpc::OnSpecial2()
{

	if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		ISkill * pSkill = GetActiveSkill();
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		switch(eStyle)
		{
		case SKILL_SS_Thief:
			{
				( (KThiefSkill*)pSkill )->OnSkill(this);
			}
			break;
		} 
		
		if (m_Frames.nTotalFrame == 0)
		{
			m_ProcessAI = 1;
		}
	}
	
}

void KNpc::DoSpecial3()
{
}

void KNpc::OnSpecial3()
{
}

BOOL KNpc::DoBlurMove()
{
	if(m_Doing == do_hurt || m_Doing == do_death)
		return FALSE;

	if (m_SkillParam1 <= 0)
		return FALSE;

#ifndef _SERVER
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
		_ASSERT(0);
		DoStand();
		return FALSE;
	}
#else
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return FALSE;
#endif

	KSkill * pSkill =(KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;

	m_ProcessAI = 0;

	switch(m_SpecialSkillStep)
	{
	case 0:
		{
		int nX, nY;
		GetMpsPos(&nX, &nY);

		if (nX == m_DesX && nY == m_DesY)
		{
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
			return FALSE;
		}

		int nDir = g_GetDirIndex(nX, nY, m_DesX, m_DesY);
		int	nMaxLength = m_SkillParam1;
		int	nWantLength = SubWorld[m_SubWorldIndex].GetDistance(nX, nY, m_DesX, m_DesY);
		int	nSin = g_DirSin(nDir, 64);
		int	nCos = g_DirCos(nDir, 64);

		if (nWantLength > nMaxLength)
		{
			m_DesX = nX + ((nMaxLength * nCos) >> 10);
			m_DesY = nY + ((nMaxLength * nSin) >> 10);
			nWantLength = nMaxLength;
		}
		else if (nWantLength <= MIN_DOMELEE_RANGE)
		{
			return FALSE;
		}

		int nStep = nWantLength / MIN_BLURMOVE_SPEED;

		int nTestX = 0;
		int nTestY = 0;
		int nSuccessStep = 0;

		for (int i = 1; i < nStep + 1; i++)
		{
			nTestX = nX + ((MIN_BLURMOVE_SPEED * nCos * i) >> 10);
			nTestY = nY + ((MIN_BLURMOVE_SPEED * nSin * i) >> 10);
			int nBarrier = SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY);
			DWORD dwTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);
			if (Obstacle_NULL == nBarrier && dwTrap == 0)
			{
				nSuccessStep = i;
			}
			if (Obstacle_Normal == nBarrier || Obstacle_Fly == nBarrier || dwTrap)
			{
				if (nSuccessStep <= MIN_DOMELEE_RANGE / MIN_BLURMOVE_SPEED)
				{
					DoStand();
					m_ProcessAI = 1;
					m_SpecialSkillStep = 0;
					return FALSE;
				}
				m_DesX = nX + ((MIN_BLURMOVE_SPEED * nCos * nSuccessStep) >> 10);
				m_DesY = nY + ((MIN_BLURMOVE_SPEED * nSin * nSuccessStep) >> 10);
				nStep = nSuccessStep;
				break;
			}
		}
		}
		m_Doing = do_blurmove;
		break;
	case 1:
		m_ProcessAI = 0;
		m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(0);
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_blurmove;
		break;
	case 2:
		{
#ifndef _SERVER
		int nX, nY;
		GetMpsPos(&nX, &nY);
#endif
		int nOldRegion = m_RegionIndex;
		int nOldMapX = m_MapX;
		int nOldMapY = m_MapY;
		int nOldOffX = m_OffX;
		int nOldOffY = m_OffY;
		
		if (!m_bClientOnly)
			CURREGION.DecRef(m_MapX, m_MapY, obj_npc);

		SubWorld[m_SubWorldIndex].Mps2Map(m_DesX, m_DesY, &m_RegionIndex,&m_MapX, &m_MapY, &m_OffX, &m_OffY);

		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		
		if (m_RegionIndex == -1)
		{
			m_RegionIndex = nOldRegion;
			m_MapX = nOldMapX;
			m_MapY = nOldMapY;
			m_OffX = nOldOffX;
			m_OffY = nOldOffY;
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
			return FALSE;
		}
		
		if (nOldRegion != m_RegionIndex)
		{
#ifdef _SERVER
			SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
			if (IsPlayer())
			{
				SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
				if (m_nPlayerIdx > 0)
				{
					Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove, FALSE); //#mµi mßn khi l­ít
				}
			}
#else
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
#endif
		}
#ifndef _SERVER
		m_DataRes.CreateBlur(m_Index, g_GetDistance(nX, nY, m_DesX, m_DesY), m_Dir);
#endif
		}
		break;
	}
	return TRUE;
}

void KNpc::OnBlurMove()
{
	if (m_SpecialSkillStep == 0)
	{
		m_SpecialSkillStep ++;
		DoBlurMove();
	}
	else if (m_SpecialSkillStep == 1)
	{
		if (WaitForFrame())
		{
			m_SpecialSkillStep ++;
			DoBlurMove();
		}
	}
	else
	{
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

void KNpc::DoStand()
{
	m_Frames.nTotalFrame = m_StandFrame;
	if (m_Doing == do_stand)
	{
		return;
	}
	else
	{
		FixPos(); //add by Fong KiÒu from KT
		m_Doing = do_stand;
#ifndef _SERVER
		// [FIX-1 26/08] Dung han thi khong con 'dang bi chan' nua - xoa bo dem, neu khong no
		// giu quyen ghi de toa do cua goi dong bo mai mai (do that: co leo toi 304 qua 53 giay).
		// (m_nNeedFixPos chi ton tai o ban client - KNpc.h khai bao trong #ifndef _SERVER)
		m_nNeedFixPos = 0;
#endif
		m_Frames.nCurrentFrame = 0;
		GetMpsPos(&m_DesX, &m_DesY);
#ifndef _SERVER
//		m_StandSyncTime = timeGetTime()+2*1000; //giay*1000
		m_DataRes.SetBlur(FALSE); //add by Fong KiÒu from KT
		if (m_FightMode)
			m_ClientDoing = cdo_fightstand;
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
		m_DataRes.StopSound();
#endif
	}
}

#ifndef _SERVER
// Bi chan tam thoi (GetDir tra 0): chi doi HOAT ANH sang tu the dung, GIU NGUYEN
// m_Doing va m_DesX/m_DesY.
// CANH BAO: ban dau ham nay dat m_Doing = do_stand va toi ghi chu thich "tick sau GetDir
// tu thu lai" - SAI: ServeMove thoat ngay o dau ham (KNpc.cpp:4409) khi m_Doing
// khong phai do_walk/do_run/... nen KHONG con duong nao goi lai GetDir; NPC phai
// nam cho goi mang moi (5 tick = 278ms) moi tinh lai => dong nguoi thanh dung lau.
// Giu m_Doing thi ServeMove van chay moi tick va tu thu lai duong ngay khi thong,
// con mat nguoi choi van thay tu the DUNG (khong con "chan chay tai cho").
void KNpc::DoStandBlocked()
{
	if (m_ClientDoing == cdo_stand || m_ClientDoing == cdo_fightstand)
		return;	// da o tu the dung roi
	m_DataRes.SetBlur(FALSE);
	if (m_FightMode)
		m_ClientDoing = cdo_fightstand;
	else
		m_ClientDoing = cdo_stand;
	m_DataRes.StopSound();
}
#endif

void KNpc::OnStand()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightstand;
		}
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}
#else
		if(IsPlayer() && m_nTimeIdleValue > 0)
		{
			m_nTimeIdleCounter++;
			if(m_nTimeIdleCounter >= m_nTimeIdleValue)
			{
				if (Player[m_nPlayerIdx].m_dwDeathScriptId)
				{
					Player[m_nPlayerIdx].ExecuteScript2(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnPlayerTimerIdle", m_nPlayerIdx, 0); //#khi player het thoi gian idle
				}
				m_nTimeIdleCounter = 0;
				m_nTimeIdleValue = 0;
			}
		}
#endif
	}
}

void KNpc::DoRevive()
{
	if (m_RegionIndex < 0)
	{
		g_DebugLog("[error]%s Region Index < 0 when dorevive", Name);
		return;
	}
	//
	if (m_Doing == do_revive)
	{
        g_DebugLog("[error]%s Doing when dorevive 111111111", Name);
		return;
	}
	else
	{
		m_Doing = do_revive;
		m_ProcessAI = 0;
		m_ProcessState = 0;
		//
		ClearStateSkillEffect();
		ClearNormalState();
		//
#ifdef _SERVER
		if (!IsPlayer()) // ²»ÊÇÍæ¼Ò ¼ÌÐøÖ´ÐÐ
		{
			m_Frames.nTotalFrame = m_ReviveFrame;  // ¸³ÖµÖØÉúÊ±¼ä

			if (m_RegionIndex>=0)
			{
			   SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			   SubWorld[m_SubWorldIndex].NpcChangeRegion(m_RegionIndex,VOID_REGION,m_Index);	//¼ÓÈëÖØÉúÁÐ±í
			}

			m_Frames.nCurrentFrame = 0;
		}
#else
		if (IsPlayer())
		{
			Player[m_nPlayerIdx].LeaveTeam();//chet mat to doi 
			//
			KSystemMessage Msg;
			Msg.byConfirmType = SMCT_UI_RENASCENCE;
			Msg.byParamSize = 0;
			Msg.byPriority = 255;
			Msg.eType = SMT_PLAYER;
			sprintf(Msg.szMessage, MSG_NPC_DEATH, Name);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL); //Th«ng b¸o vÒ thµnh d­ìng søc
		}
		m_Frames.nTotalFrame = m_DeathFrame;
		m_ClientDoing = cdo_death;
#endif
	}
}

void KNpc::OnRevive()
{
#ifdef _SERVER
	if (!IsPlayer() && WaitForFrame())
	{
		// [PORT5 23/08] AddNpcEx bNoRevive: go NPC thay vi hoi sinh (khuon LuaDelNpc; cung thoi
		// diem voi DelNpc trong script OnRevive cua global\drop\daihoangkim.lua - da chay that)
		if (m_bNoRevive)
		{
			if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
			{
				SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
				// [VA 31/08] BO DecRef o day: DoRevive (KNpc.cpp:2306) DA tra phan dem cua o
				// nay 18 khung truoc. m_RegionIndex con >= 0 chi vi NpcChangeRegion
				// (KSubWorld.cpp:2368) CO Y khong reset khi dich la VOID_REGION - do la gia
				// tri CU, khong phai bang chung NPC con chiem o. Tru lan hai se AN MAT phan
				// dem cua NPC KHAC dung chung o (g_nPbNpcChan=0 cho chong o) => bo dem ve 0
				// trong khi van con quai song => KRegion::FindNpc (KRegion.h:191) thoat ngay
				// => con do TANG HINH truoc moi phep va cham du van song va van danh tra.
				// Do that: o (vung 37, 7,2) 440 lan quet 0 cham; cung con quai do o ba o khac
				// cham 25-50%. Khuon chot nay da co san o KRegion.cpp:659.
				NpcSet.Remove(m_Index);
			}
			return;
		}
		Revive();
	}
#else	
	m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
#endif
}

void KNpc::DoRun()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_CurrentRunSpeed)
		m_Frames.nTotalFrame = (m_RunFrame * m_RunSpeed) / m_CurrentRunSpeed;
	else
		m_Frames.nTotalFrame = m_RunFrame;

#ifndef _SERVER
	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightrun;
	}
	else
	{
		m_ClientDoing = cdo_run;
	}
#endif

	if (IsPlayer())
	{
/*		if (!Cost(attrib_stamina, m_CurrentStaminaLoss))
		{
			DoWalk();
			return;
		}*/
	}

#ifdef _SERVER

	NPC_RUN_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcrun;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;

	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

	if (m_Doing == do_run)
	{
		return;
	}
	m_Doing = do_run;

	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnRun()
{	
	WaitForFrame();
	if (!(m_LoopFrames % GAME_UPDATE_TIME) && Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder) // giet nguoi thÓ lùc gi¶m
	{
#ifdef _SERVER
		// [WLLS] ForbitStamina(1): dong bang tieu hao the luc trong dau truong
		if (m_Kind == kind_player && m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER &&
			Player[m_nPlayerIdx].m_bWllsForbidStamina)
			;
		else
#endif
		m_CurrentStamina -= 18;
		if (m_CurrentStamina <= 0)
		{
			m_CurrentStamina = 0;
		}
	}

	if (m_CurrentStamina == 0)
	{	
		if(m_Doing == do_runattack)
		{	
			m_CurrentRunSpeed += 50;
			CheckTrap(); 
			ServeMove(m_CurrentRunSpeed);
			m_CurrentRunSpeed -= 50;
		}
		else
		{
			DoWalk();
		}
	}
	else
	{
		if(m_Doing == do_runattack)
		{	
			m_CurrentRunSpeed += 50;
			CheckTrap(); 
			ServeMove(m_CurrentRunSpeed);
			m_CurrentRunSpeed -= 50;
		}
		else
		{
			ServeMove(m_CurrentRunSpeed);
		}
	}
//
#ifdef _SERVER
	if(IsPlayer() && m_nTimeIdleCounter > 0)
	{
		m_nTimeIdleCounter = 0;
	}
#endif
//
}

void KNpc::DoSit()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_Doing == do_sit)
	{
//		DoStand();
		return;
	}
	
	m_Doing = do_sit;
#ifdef _SERVER	
	NPC_SIT_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcsit;
	NetCommand.ID = m_dwID;
		
	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};

	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
		m_ClientDoing = cdo_sit;
#endif

	m_Frames.nTotalFrame = m_SitFrame;
	m_Frames.nCurrentFrame = 0;
}

void KNpc::OnSit()
{
	if (WaitForFrame())
	{
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;
//
#ifdef _SERVER
		if(IsPlayer() && m_nTimeIdleValue > 0)
		{
			m_nTimeIdleCounter++;
			if(m_nTimeIdleCounter >= m_nTimeIdleValue*18)
			{
				if (Player[m_nPlayerIdx].m_dwDeathScriptId)
				{
					Player[m_nPlayerIdx].ExecuteScript2(Player[m_nPlayerIdx].m_dwDeathScriptId, "OnPlayerTimerIdle", m_nPlayerIdx, 0); //#khi player het thoi gian idle
				}
				m_nTimeIdleCounter = 0;
				m_nTimeIdleValue = 0;
			}
		}
#endif
//
	}
	FixPos(); //add by Fong KiÒu from KT
}

void KNpc::DoSkill(int nX, int nY)
{
	_ASSERT(m_RegionIndex >= 0);

	AUTOLOG_EVERY(300, "[E4_SKILL_IN] npc=%d id=%u plr=%d skill=%d tgt=(%d,%d) doing=%d fight=%d rgn=%d", m_Index, m_dwID, m_nPlayerIdx, m_ActiveSkillID, nX, nY, (int)m_Doing, m_FightMode, m_RegionIndex);
	if (Player[m_nPlayerIdx].CheckTrading())
		return;
	AUTOLOG_EVERY(1000, "[E4_SKILL_REJ_BUSY] npc=%d id=%u skill=%d doing=%d frame=%d/%d", m_Index, m_dwID, m_ActiveSkillID, (int)m_Doing, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);
	if (m_Doing == do_skill || m_Doing == do_hurt)
		return;

	ISkill * pSkill = GetActiveSkill();
	if(pSkill)
	{
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		if (IsPlayer())
		{
			AUTOLOG_EVERY(1000, "[E4_SKILL_REJ_NOFIGHT] npc=%d id=%u plr=%d skill=%d fight=%d", m_Index, m_dwID, m_nPlayerIdx, m_ActiveSkillID, m_FightMode);
			if (!m_FightMode)
				return;
			#ifdef _SERVER
			if (m_nPlayerIdx > 0)
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeAttack, FALSE);//#mÂµi mÃŸn khi xuÃŠt skill
			#endif
		}
		AUTOLOG_EVERY(500, "[E4_SKILL_COOLDOWN] npc=%d id=%u skill=%d now=%u cancast=%d", m_Index, m_dwID, m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime, (int)m_SkillList.CanCast(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime));
		if (m_SkillList.CanCast(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{
			AUTOLOG_EVERY(300, "[E4_SKILL_CANCAST] npc=%d id=%u skill=%d style=%d tgt=(%d,%d) map=(%d,%d) off=(%d,%d) dir=%d radius=%d", m_Index, m_dwID, m_ActiveSkillID, (int)eStyle, nX, nY, m_MapX, m_MapY, m_OffX, m_OffY, m_Dir, m_CurrentAttackRadius);
			switch (pSkill->CanCastSkill(m_Index, nX, nY))
			{
			case -1:
				#ifdef _SERVER
				if(nX == -1)
				{
					int nDesX, nDesY;
					Npc[nY].GetMpsPos(&nDesX, &nDesY);
					if (IsPlayer())
						AUTOLOG_IDX(m_Index, "[S2-MELEE-TOOFAR-RUN] npc=%d id=%u skill=%d tgt_idx=%d dist=%d radius=%d cong=%d curradius=%d chay_toi=(%d,%d) map=(%d,%d) doing=%d ngua=%d", m_Index, m_dwID, m_ActiveSkillID, nY, NpcSet.GetDistance(m_Index, nY), pSkill->GetAttackRadius(), pSkill->GetAttackRadius() + 20, m_CurrentAttackRadius, nDesX, nDesY, m_MapX, m_MapY, (int)m_Doing, (int)m_bRideHorse);
					SendCommand(do_run, nDesX, nDesY);
					return;
				}
				#endif
				break;
			case 0:
				if (IsPlayer())
					AUTOLOG_IDX_EVERY(m_Index, 500, "[S2-CANCAST-DENY] npc=%d id=%u skill=%d style=%d p1=%d p2=%d dist=%d radius=%d fight=%d silent=%d ngua=%d mana=%d life=%d", m_Index, m_dwID, m_ActiveSkillID, (int)eStyle, nX, nY, ((nX == -1 && nY > 0 && nY < MAX_NPC) ? NpcSet.GetDistance(m_Index, nY) : -1), pSkill->GetAttackRadius(), (int)m_FightMode, m_SilentState.nTime, (int)m_bRideHorse, m_CurrentMana, m_CurrentLife);
				goto Exit;
				break;
			case 1: //!IsPlayer() -> chuan cho ca server va client
				AUTOLOG_EVERY(500, "[E4_SKILL_COST] npc=%d id=%u skill=%d costtype=%d cost=%d mana=%d/%d stam=%d life=%d/%d", m_Index, m_dwID, m_ActiveSkillID, (int)pSkill->GetSkillCostType(), pSkill->GetSkillCost(this), m_CurrentMana, m_CurrentManaMax, m_CurrentStamina, m_CurrentLife, m_CurrentLifeMax);
				if(!IsPlayer() || Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this)))
				{
					#ifdef _SERVER
					NPC_SKILL_SYNC	NetCommand;
					
					NetCommand.ProtocolType = (BYTE)s2c_skillcast;
					NetCommand.ID = m_dwID;
					NetCommand.nSkillID = m_ActiveSkillID;
					NetCommand.nSkillLevel = m_SkillList.GetCurrentLevel(m_ActiveSkillID);
					
					if (nY <= 0 ) 
					{
						DoStand();
						return;
					}
					
					NetCommand.nMpsX = nX;
					if (nX == -1) //m_nDesX == -1 means attack someone whose id is DesY , and if m_nDesX == -2 means attack at somedir
					{
						NetCommand.nMpsY = Npc[nY].m_dwID;
						if (0 == NetCommand.nMpsY || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
							return;
					}
					else
					{
						NetCommand.nMpsY = nY;
					}

					m_SkillParam1 = nX;
					m_SkillParam2 = nY;
					m_DesX = nX;
					m_DesY = nY;
					
					static const POINT	POff[8] = 
					{
						{0, 32},
						{-16, 32},
						{-16, 0},
						{-16, -32},
						{0, -32},
						{16, -32},
						{16, 0},
						{16, 32},
					};
					int nMaxCount = MAX_BROADCAST_COUNT;
					CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
					int i;
					for (i = 0; i < 8; i++)
					{
						if (CONREGIONIDX(i) == -1)
							continue;
						CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
					}
					#endif		
#ifdef _SERVER
					// (21/08 toi) S4-CAST: may chu THUC SU thi hanh chieu (da qua CanCastSkill + Cost). Doi chieu voi
					// S4-MSL-HIT/S4-MSL-END de biet dan co cham muc tieu khong = "danh hut" that. Loc ten, khong tiet che.
					if (IsPlayer() && g_AutoLogWhoIdx(m_Index))
					{
						int nLogT = (nX == -1 && nY > 0 && nY < MAX_NPC) ? nY : -1;
						int nLogMX = 0, nLogMY = 0, nLogTX = 0, nLogTY = 0;
						GetMpsPos(&nLogMX, &nLogMY);
						if (nLogT >= 0) Npc[nLogT].GetMpsPos(&nLogTX, &nLogTY);
						g_AutoLog("[S4-CAST] npc=%d skill=%d lv=%d style=%d p=(%d,%d) tgt=%d(id=%u kind=%u doing=%d life=%d) dist=%d radius=%d me=(%d,%d) tgtmps=(%d,%d) medoing=%d",
							m_Index, m_ActiveSkillID, (int)m_SkillList.GetCurrentLevel(m_ActiveSkillID), (int)eStyle, nX, nY,
							nLogT, (nLogT >= 0) ? (unsigned int)Npc[nLogT].m_dwID : 0u, (nLogT >= 0) ? (unsigned int)Npc[nLogT].m_Kind : 0u,
							(nLogT >= 0) ? (int)Npc[nLogT].m_Doing : -1, (nLogT >= 0) ? Npc[nLogT].m_CurrentLife : -1,
							(nLogT >= 0) ? g_GetDistance(nLogMX, nLogMY, nLogTX, nLogTY) : -1, pSkill->GetAttackRadius(),
							nLogMX, nLogMY, nLogTX, nLogTY, (int)m_Doing);
					}
#endif
					if (eStyle == SKILL_SS_Missles 
						|| eStyle == SKILL_SS_Melee 
						|| eStyle == SKILL_SS_InitiativeNpcState 
						|| eStyle == SKILL_SS_PassivityNpcState)
					{
						DoOrdinSkill((KSkill *) pSkill, nX, nY);
					}
					else
					{
						switch(eStyle)
						{
						case SKILL_SS_Thief:
							{
								((KThiefSkill*)pSkill)->DoSkill(this, nX, nY);

							}break;
						default:
							return;
						}
					}
					//
					if (m_HideState.nTime) //sö dông skill hiÖn h×nh
					{
						m_HideState.nTime = 0;
					}
					return;
				}
			}
		}
	}
	else
	{
		//_ASSERT(pSkill);
		return;
	}
Exit:
AUTOLOG_EVERY(300, "[E4_SKILL_ABORT] npc=%d id=%u skill=%d tgt=(%d,%d) people=%d object=%d doing=%d", m_Index, m_dwID, m_ActiveSkillID, nX, nY, m_nPeopleIdx, m_nObjectIdx, (int)m_Doing);
	m_nPeopleIdx = 0;
	m_nObjectIdx = 0;
	DoStand();
}  

int KNpc::DoOrdinSkill(KSkill * pSkill, int nX, int nY)
{
	_ASSERT(pSkill);
	if(!pSkill) return 0;

#ifndef _SERVER		
	m_DataRes.StopSound();
	int x, y, tx, ty;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
	
	if (nY < 0)
		return 0;
	
	if (nX < 0)
	{
		if (nX != -1) 
			return 0;
		
		if (nY >= MAX_NPC || Npc[nY].m_dwID == 0 || Npc[nY].m_SubWorldIndex != m_SubWorldIndex)
			return 0;
		Npc[nY].GetMpsPos(&tx, &ty);
	}
	else
	{
		tx = nX;
		ty = nY;
	}
	
	m_SkillParam1 = nX;
	m_SkillParam2 = nY;
	m_DesX = nX;
	m_DesY = nY;
	
	m_Dir = g_GetDirIndex(x, y, tx, ty);
	if (pSkill->GetPreCastEffectFile()[0])
		m_DataRes.SetSpecialSpr((char *)pSkill->GetPreCastEffectFile());
	
	if (IsPlayer())
		pSkill->PlayPreCastSound(m_nSex,x, y);
	
	if (pSkill->IsNeedShadow())		
		m_DataRes.SetBlur(TRUE);
	else
		m_DataRes.SetBlur(FALSE);
#endif
	
	CLIENTACTION ClientDoing = pSkill->GetActionType();
	
#ifndef _SERVER
	if (ClientDoing >= cdo_count) 
		m_ClientDoing = cdo_magic;
	else if (ClientDoing != cdo_none)
		m_ClientDoing = ClientDoing;
#endif
	if (pSkill->GetSkillStyle() == SKILL_SS_Melee)
	{
		AUTOLOG_EVERY(300, "[E4_MELEE_TRY] npc=%d id=%u skill=%d melee=%d des=(%d,%d) map=(%d,%d) jspeed=%d", m_Index, m_dwID, m_ActiveSkillID, (int)pSkill->GetMeleeType(), m_DesX, m_DesY, m_MapX, m_MapY, m_CurrentJumpSpeed);
		if (CastMeleeSkill(pSkill) == FALSE)
		{
			m_nPeopleIdx = 0;
			m_nObjectIdx = 0;
			m_ProcessAI = 1;
			DoStand();
			
			return 1 ;
		}
		if(!pSkill->IsAura())
		{
			DWORD dwCastTime = 0;
			eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
			if (eStyle == SKILL_SS_Missles 
				|| eStyle == SKILL_SS_Melee 
				|| eStyle == SKILL_SS_InitiativeNpcState 
				|| eStyle == SKILL_SS_PassivityNpcState)
			{
				dwCastTime = pSkill->GetDelayPerCast(m_bRideHorse);
			}
			else
			{
				switch(eStyle)
				{
				case SKILL_SS_Thief:
					{
						dwCastTime = ((KThiefSkill*)pSkill)->GetDelayPerCast();
					}break;
				}
			}
			m_SkillList.SetNextCastTime(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);
		}
	}

	else if (pSkill->IsPhysical())
	{
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			int attackSpeedDivisor = m_CurrentAttackSpeed + MAX_PERCENT;
			if (attackSpeedDivisor <= 0) attackSpeedDivisor = 1;
			int nTotalFrame = m_AttackFrame * MAX_PERCENT / attackSpeedDivisor;
			m_Frames.nTotalFrame = nTotalFrame - nTotalFrame % 2;
			if (m_Frames.nTotalFrame <= 0)
				m_Frames.nTotalFrame = 1;
		}
		
#ifndef _SERVER
		if (g_Random(3))
			m_ClientDoing = cdo_attack;
		else 
			m_ClientDoing = cdo_attack1;
#endif
		m_Doing = do_attack;
	}
	else
	{
		if (ClientDoing == cdo_none) 
			m_Frames.nTotalFrame = 0;
		else
		{
			int castSpeedDivisor = m_CurrentCastSpeed + MAX_PERCENT;
			if (castSpeedDivisor <= 0) castSpeedDivisor = 1;
			int nTotalFrame = m_CastFrame * MAX_PERCENT / castSpeedDivisor;
			m_Frames.nTotalFrame = nTotalFrame - nTotalFrame % 2;
			if (m_Frames.nTotalFrame <= 0)
				m_Frames.nTotalFrame = 1;
		}
		m_Doing  = do_magic;
	}
	m_ProcessAI = 0;
	m_Frames.nCurrentFrame = 0;	
	return 1;
}

void KNpc::DoAttack()
{
	AUTOLOG_EVERY(500, "[E4_DOATTACK_IN] npc=%d id=%u doing=%d aframe=%d aspeed=%d people=%d", m_Index, m_dwID, (int)m_Doing, m_AttackFrame, m_CurrentAttackSpeed, m_nPeopleIdx);
	if (m_Doing == do_attack)
		return;

#ifndef _SERVER
	if (g_Random(2) == 1)
	{
		m_ClientDoing = cdo_attack;
	}
	else
	{
		m_ClientDoing = cdo_attack1;
	}
#endif

	m_ProcessAI = 0;
	m_Frames.nTotalFrame = m_AttackFrame * 100 / (100 + m_CurrentAttackSpeed);
	m_Frames.nCurrentFrame = 0;
	m_Doing = do_attack;
}

BOOL	KNpc::CastMeleeSkill(KSkill * pSkill)
{
	BOOL bSuceess = FALSE;
	_ASSERT(pSkill);
	if(!pSkill) return FALSE;
	
	switch(pSkill->GetMeleeType())
	{
	case Melee_AttackWithBlur:
		{
			bSuceess = DoBlurAttack();
		}break;
	case Melee_Jump:
		{
			if (NewJump(m_DesX, m_DesY))
			{
				DoJump();
				bSuceess = TRUE;
			}
		}break;
	case Melee_JumpAndAttack:
		{
			if (m_DesX < 0 && m_DesY > 0) 
			{
				int x, y;
				SubWorld[m_SubWorldIndex].Map2Mps
					(
					Npc[m_DesY].m_RegionIndex,
					Npc[m_DesY].m_MapX, 
					Npc[m_DesY].m_MapY, 
					Npc[m_DesY].m_OffX, 
					Npc[m_DesY].m_OffY, 
					&x,
					&y
					);
				
				m_DesX = x + 1;
				m_DesY = y;
			}
			if (NewJump(m_DesX, m_DesY))
			{
				DoJumpAttack();
				bSuceess = TRUE;
			}
		}break;
	case Melee_RunAndAttack:
		{
			bSuceess = DoRunAttack();

		}break;
	case Melee_ManyAttack:
		{
			bSuceess = DoManyAttack();
		}break;
	case Melee_MoveWithBlur:
		{
			m_SkillParam1 = pSkill->GetParam1();
			bSuceess = DoBlurMove();
		}break;
	default:
		m_ProcessAI = 1;
		break;
	}
	return bSuceess;

}

BOOL KNpc::DoBlurAttack()// DoSpecail1
{
	if (m_Doing == do_special1)
		return FALSE;
	
	KSkill * pSkill = (KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	_ASSERT(pSkill->GetSkillStyle() == SKILL_SS_Melee);

#ifndef _SERVER
		m_ClientDoing = pSkill->GetActionType();

		if (pSkill->IsNeedShadow())	
			m_DataRes.SetBlur(TRUE);	//nguyen nhan gay ra nhan su dung skill ao anh

		if (m_ClientDoing == cdo_attack1 || m_ClientDoing == cdo_attack)
		{
			if(g_Random(3))
				m_ClientDoing = cdo_attack;
			else 
				m_ClientDoing = cdo_attack1;
		}
#endif
	m_Frames.nTotalFrame = m_AttackFrame * 100 / (100 + m_CurrentAttackSpeed);
	m_Frames.nCurrentFrame = 0;
	m_Doing = do_special1;
	return TRUE;
}

void KNpc::OnSkill()
{
	if (WaitForFrame() && m_Frames.nTotalFrame != 0)
	{
		DoStand();
		m_ProcessAI = 1;	
	}
	else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
	{
		KSkill * pSkill = NULL;
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif

		if (m_DesX == -1) 
		{
			if (m_DesY <= 0) 
				goto Label_ProcessAI;
			
			if (Npc[m_DesY].m_RegionIndex < 0) 
				goto Label_ProcessAI;
		}
			
		pSkill =(KSkill*) GetActiveSkill();

		if (pSkill)
		{
			pSkill->Cast(m_Index, m_DesX, m_DesY);

			DWORD dwCastTime = 0;
			eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
			if (eStyle == SKILL_SS_Missles 
				|| eStyle == SKILL_SS_Melee 
				|| eStyle == SKILL_SS_InitiativeNpcState 
				|| eStyle == SKILL_SS_PassivityNpcState)
			{
				dwCastTime = pSkill->GetDelayPerCast(m_bRideHorse);
			}
			else if (eStyle == SKILL_SS_Thief)
				dwCastTime = ((KThiefSkill*)pSkill)->GetDelayPerCast();

			m_SkillList.SetNextCastTime(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);
		}

Label_ProcessAI:
		if (m_Frames.nTotalFrame <= 0)
		{

			m_ProcessAI = 1;
		}
	}
//
//#ifdef _SERVER
//	if(IsPlayer() && m_nTimeIdleCounter > 0)
//	{
//		m_nTimeIdleCounter = 0;
//	}
//#endif
//
}

void KNpc::JumpTo(int nMpsX, int nMpsY)
{
	if (NewJump(nMpsX, nMpsY))
		DoJump();
	else
	{
		RunTo(nMpsX, nMpsY);
	}
}

void KNpc::RunTo(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoRun();
}

void KNpc::Goto(int nMpsX, int nMpsY)
{
	if (NewPath(nMpsX, nMpsY))
		DoWalk();
}

void KNpc::DoWalk()
{
	_ASSERT(m_RegionIndex >= 0);

	if (m_CurrentWalkSpeed)
		m_Frames.nTotalFrame = (m_WalkFrame * m_WalkSpeed) / m_CurrentWalkSpeed + 1;
	else
		m_Frames.nTotalFrame = m_WalkFrame;
	
#ifdef _SERVER	
	NPC_WALK_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcwalk;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;

	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}	
#endif

	if (m_Doing == do_walk)
	{
		return;
	}
	m_Doing = do_walk;
	m_Frames.nCurrentFrame = 0;

#ifndef _SERVER
	if (m_FightMode)
	{
		m_ClientDoing = cdo_fightwalk;
		Goto(m_DesX, m_DesY);
	}
	else
	{
		m_ClientDoing = cdo_walk;
	}
#endif
	
}

void KNpc::DoPlayerTalk(char * szTalk)
{
#ifdef _SERVER
	_ASSERT(m_RegionIndex >= 0);
	int nTalkLen = strlen(szTalk);
	if (!nTalkLen) return;
	BYTE * pNetCommand = new  BYTE[nTalkLen + 6 + 1];
	pNetCommand[0] = (BYTE)s2c_playertalk;
	*(DWORD *)(pNetCommand + 1) = m_dwID;
	pNetCommand[5] = nTalkLen;
	strcpy((char*)(pNetCommand + 6), szTalk);
	pNetCommand[nTalkLen + 6 ] = '\0';

	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(pNetCommand, nTalkLen + 6 + 1, nMaxCount, m_MapX, m_MapY);
	int i;
	for (i = 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(pNetCommand, nTalkLen + 6 + 1, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
	if (pNetCommand)
	{
		delete [] pNetCommand;
	}
#endif

}

void KNpc::OnPlayerTalk()
{

}

void KNpc::OnWalk()
{
	WaitForFrame();
	ServeMove(m_CurrentWalkSpeed);
//
#ifdef _SERVER
	if(IsPlayer() && m_nTimeIdleCounter > 0)
	{
		m_nTimeIdleCounter = 0;
	}
#endif
//
}

void KNpc::ModifyAttrib(int nAttacker, void* pData)
{
	if (pData != NULL)
		g_NpcAttribModify.ModifyAttrib(this, pData);
}

#ifdef _SERVER
void KNpc::ReplySkill()
{
	if (!m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_ReplySkill[i].nSkillId > 0 && m_ReplySkill[i].nSkillId < MAX_SKILL && 
			m_ReplySkill[i].nSkillLevel > 0 && m_ReplySkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_ReplySkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_ReplySkill[i].nRate))
				{
					this->Cast(m_ReplySkill[i].nSkillId, m_ReplySkill[i].nSkillLevel);
					m_ReplySkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_ReplySkill[i].nWaitCastTime;
				}
			}
		}
	}
}

void KNpc::RescueSkill()
{
	if (!m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_RescueSkill[i].nSkillId > 0 && m_RescueSkill[i].nSkillId < MAX_SKILL && 
			m_RescueSkill[i].nSkillLevel > 0 && m_RescueSkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_RescueSkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_RescueSkill[i].nRate))
				{
					this->Cast(m_RescueSkill[i].nSkillId, m_RescueSkill[i].nSkillLevel);
					m_RescueSkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_RescueSkill[i].nWaitCastTime;
				}
			}
		}
	}
}

void KNpc::AttackSkill(int nLauncher)
{
	if (!m_Index || !Npc[nLauncher].m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_AttackSkill[i].nSkillId > 0 && m_AttackSkill[i].nSkillId < MAX_SKILL && 
			m_AttackSkill[i].nSkillLevel > 0 && m_AttackSkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_AttackSkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_AttackSkill[i].nRate))
				{
					KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(m_AttackSkill[i].nSkillId, m_AttackSkill[i].nSkillLevel);
					if(pSkill)
						pSkill->Cast(m_Index, -1, nLauncher);
					m_AttackSkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_AttackSkill[i].nWaitCastTime;
				}
			}
		}
	}
}

void KNpc::DeathSkill()
{
	if (!m_Index)
		return;

	if (m_Doing == do_death || m_Doing == do_revive)
		return;

	for (int i = 0; i < MAX_AUTOSKILL; i ++)
	{
		if (m_DeathSkill[i].nSkillId > 0 && m_ReplySkill[i].nSkillId < MAX_SKILL && 
			m_DeathSkill[i].nSkillLevel > 0 && m_DeathSkill[i].nSkillLevel < MAX_SKILLLEVEL)
		{
			if (m_DeathSkill[i].dwNextCastTime < SubWorld[m_SubWorldIndex].m_dwCurrentTime)
			{
				if (g_RandPercent(m_DeathSkill[i].nRate))
				{
					this->Cast(m_DeathSkill[i].nSkillId, m_DeathSkill[i].nSkillLevel);
					m_DeathSkill[i].dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + m_DeathSkill[i].nWaitCastTime;
				}
			}
		}
	}
}

int	KNpc::UpdateDBStateList(BYTE * pStateBuffer)
{
	if (!pStateBuffer)
		return -1;
	
	int nCount = 0;

	TDBSkillData * pStateData = (TDBSkillData *) pStateBuffer;
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetPrev();

		if (pTempNode->m_SkillID > 0 && pTempNode->m_SkillID < MAX_SKILL && 
			pTempNode->m_Level > 0 && pTempNode->m_Level < MAX_SKILLLEVEL && 
			pTempNode->m_bOverLook)
		{
			pStateData->m_nSkillId = pTempNode->m_SkillID;
			pStateData->m_nSkillLevel = pTempNode->m_Level;
			pStateData->m_nSkillVal = pTempNode->m_LeftTime;
			pStateData++;
			nCount++;
		}
	}
	return nCount;
}
#endif

#ifdef _SERVER
void KNpc::SyncDamageInfo(int nLauncher, int nDamage, COMBAT_INFO_TYPE damType, int skillId, bool isCrit, bool bBroadCast /* = false */)
{
	DAMAGESHOW	damInfo;

	damInfo.ProtocolType = s2c_show_damage;
	damInfo.nDamage = nDamage;
	damInfo.enType = (BYTE)damType;
	damInfo.SkillId = (WORD)skillId;
	damInfo.IsCrit = isCrit;
	damInfo.dwReceiver = GetId();
	damInfo.dwLauncher = Npc[nLauncher].GetId();
	bBroadCast = true;
	if (bBroadCast)
	{
		int nMaxCount = MAX_BROADCAST_COUNT;
		if (m_SubWorldIndex >= 0 && m_SubWorldIndex < MAX_SUBWORLD) {
			if (m_RegionIndex >= 0 && m_RegionIndex < SubWorld[m_SubWorldIndex].m_nTotalRegion) {
				CURREGION.BroadCast(&damInfo, sizeof(damInfo), nMaxCount, m_MapX, m_MapY);
				static const POINT	POff[8] =
				{
					{0, 32},
					{-16, 32},
					{-16, 0},
					{-16, -32},
					{0, -32},
					{16, -32},
					{16, 0},
					{16, 32},
				};

				int i;
				for (i = 0; i < 8; i++)
				{
					if (CONREGIONIDX(i) == -1)
						continue;
					CONREGION(i).BroadCast(&damInfo, sizeof(damInfo), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
				}
			}
		}
	}
	{
		if (g_pServer)
		{
			//There is no need to send repeated messages about the harm you have caused to yourself.
			if (nLauncher != m_Index)
			{
				int launcherPlayerIndex = Npc[nLauncher].m_nPlayerIdx;
				if (launcherPlayerIndex >= 0)
				{
					g_pServer->PackDataToClient(Player[launcherPlayerIndex].m_nNetConnectIdx, &damInfo, sizeof(damInfo));
				}
			}

			int selfPlayerIndex = 0;
			if (GetKind() == kind_player)
			{
				selfPlayerIndex = GetPlayerIdx();
			}
			//else if (GetKind() == kind_creature)
			//{
			//	//The information about the damage caused by your own master to you does not need to be sent repeatedly
			//	if (Player[GetSummonerIdx()].GetNpcIndex() != nLauncher)
			//	{
			//		selfPlayerIndex = Npc[m_Index].GetSummonerIdx();
			//	}
			//}
			//else if (GetKind() == kind_employee)
			//{
			//	if (Player[GetEmployerIdx()].GetNpcIndex() != nLauncher)
			//	{
			//		selfPlayerIndex = Npc[m_Index].GetEmployerIdx();
			//	}
			//}

			if (selfPlayerIndex >= 0)
			{
				g_pServer->PackDataToClient(Player[selfPlayerIndex].m_nNetConnectIdx, &damInfo, sizeof(damInfo));
			}
		}
	}
}
#endif

#ifdef _SERVER
BOOL KNpc::CalcDamage(int nAttacker, int nMin, int nMax, DAMAGE_TYPE nType, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, BOOL bReturn /* = FALSE */, int nFiveElements_DamageP /*0*/, int nStolen_Life/*0*/, int nStolen_Mana/*0*/, int nStolen_Stamina/*0*/, BOOL bIsDS /*= FALSE*/, BOOL bIsFS /*= FALSE*/)
{
	AUTOLOG_EVERY(500, "[E2-CALC-IN] target=%d(doing=%d region=%d life=%d kind=%u) attacker=%d type=%d min=%d max=%d series=%d fivep=%d phys=%d melee=%d return=%d DS=%d FS=%d", m_Index, (int)m_Doing, m_RegionIndex, m_CurrentLife, m_Kind, nAttacker, (int)nType, nMin, nMax, nMissleSeries, nFiveElements_DamageP, (int)bIsPhysical, (int)bIsMelee, (int)bReturn, (int)bIsDS, (int)bIsFS);
	if (m_Doing == do_death || m_Doing == do_revive || m_RegionIndex < 0)
		return FALSE;

	// ===== [KM 27/08] HE SO TRONG KICH =====
	// Ban chuan Linux 0x0808A5F8..0x0808A687: nMin/nMax duoc nhan he so
	//   factor = add_damage_p(mac dinh 100) * nHitPercent / 10000, lam tron VE 0.
	// nHitPercent do xuc xac Trong Kich o ReceiveDamage dat (100 thuong, 200+E khi trong kich).
	// Chi ap cho don danh that, KHONG ap cho sat thuong phan don (bReturn).
	if (!bReturn && nAttacker > 0 && nAttacker < MAX_NPC)
	{
		int nKMPct = Npc[nAttacker].m_nKMHitPercent;
		int nKMAdd = Npc[nAttacker].m_CurrentAddDamageP;
		if (nKMAdd <= 0)					// khe Npc dung lai chua kip dat goc 100
			nKMAdd = MAX_PERCENT;
		if (nKMPct != MAX_PERCENT || nKMAdd != MAX_PERCENT)
		{
			nMin = (int)((__int64)nMin * nKMAdd * nKMPct / (MAX_PERCENT * MAX_PERCENT));
			nMax = (int)((__int64)nMax * nKMAdd * nKMPct / (MAX_PERCENT * MAX_PERCENT));
			AUTOLOG_EVERY(1000, "[KM-CRIT] tgt=%d lch=%d pct=%d add=%d min=%d max=%d", m_Index, nAttacker, nKMPct, nKMAdd, nMin, nMax);
		}
	}
	//
	AUTOLOG_EVERY(1000, "[E2-CALC-NORANGE] target=%d attacker=%d type=%d min=%d max=%d -> tra FALSE, khong co sat thuong de tinh", m_Index, nAttacker, (int)nType, nMin, nMax);
	if (nMin + nMax <= 0)
		return FALSE;
	//
	int nDamage = 0, nRealDamage = 0;
	int	nRate = 0;
	//
	if(Owner[0]) //add by phong kiÒu npc vËn tiªu
	{
		if(strcmp(Player[Npc[nAttacker].m_nPlayerIdx].m_PlayerName,Owner) == 0)//#Tù ®¸nh vµo Tiªu Xa kh«ng lªn Damage
		{
			return FALSE;
		}

		if(Player[Npc[nAttacker].m_nPlayerIdx].m_cPK.GetNormalPKState() == 0)//#PK luyÖn c«ng kh«ng lªn Damage
		{
			return FALSE;
		}
	}
	//
	if (g_AutoLogOn())
	{
		// FIX 21/08: ban goc chi doc bieu thuc nay BEN TRONG if(Owner[0]); nAttacker co the = 0
		// (nhip doc goi CalcDamage(m_nLastPoisonDamageIdx,...)) => Player[0] la khe chua khoi tao.
		int nLogA = (nAttacker > 0 && nAttacker < MAX_NPC) ? nAttacker : -1;
		int nLogP = (nLogA >= 0 && Npc[nLogA].m_nPlayerIdx > 0 && Npc[nLogA].m_nPlayerIdx < MAX_PLAYER) ? Npc[nLogA].m_nPlayerIdx : -1;
		AUTOLOG_EVERY(1000, "[E2-CALC-OWNER] target=%d attacker=%d owner=%d attackerplayeridx=%d pkstate=%d -> da qua cua chu tieu xa / PK luyen cong", m_Index, nAttacker, (int)(Owner[0] != 0), (nLogA >= 0) ? Npc[nLogA].m_nPlayerIdx : -1, (nLogP >= 0) ? (int)Player[nLogP].m_cPK.GetNormalPKState() : -1);
	}
	if (m_Series == series_minus)
	{
		nDamage = 1;
	}
	else
	{
		int nDamageRange = nMax - nMin;
		if (nDamageRange < 0) 
		{
			nDamage = nMax + g_Random(-nDamageRange);
		}
		else
		{
			nDamage = nMin + g_Random(nMax - nMin);
		}
		//
		//if(bReturn)
		{
			if (bIsDS) //ChÝ m¹ng
			{
				nDamage = (int)((int64_t)nDamage * MAX_DEATLY_STRIKE_ENHANCEP / MAX_PERCENT);
			  	if (this->m_Kind == kind_player && 
			    Npc[nAttacker].m_Kind == kind_player)
			    {
			        nDamage = (int)((int64_t)nDamage * MAX_DEATLY_STRIKE_ENHANCEP / MAX_PERCENT);
			    }
			}
			if (bIsFS)
				{
				    float fReduction = 1.0f - m_nDamageReduction / 100.0f;
				
				    if (fReduction < 0.0f)
				        fReduction = 0.0f;
				
				    int nRand = GetRandomNumber(MIN_FATALLY_STRIKE_ENHANCEP, MAX_FATALLY_STRIKE_ENHANCEP);
				
				    nDamage = (int)(m_CurrentLife * fReduction * nRand / MAX_PERCENT);
				}

			if(this->m_Kind == kind_normal && this->m_cGold.GetGoldType() > 0)
			{
				nDamage = nDamage * NpcSet.m_nNpcSpecialDamageRate / MAX_PERCENT;
			}
		}
		//
		if(!bReturn)
		{
			if(m_Level >= LEVEL_EXPLOSIVE)
			{
				ReplySkill();
				if (m_CurrentLife < (m_CurrentLifeMax * LIFE_EXPLOSIVE / MAX_PERCENT))
					RescueSkill();
			}
			
			if(Npc[nAttacker].m_Level >= LEVEL_EXPLOSIVE)
			{
				Npc[nAttacker].AttackSkill(m_Index);
			}
		}
		//
		if (nDamage <= 0 && Npc[nAttacker].GetKind() == kind_player)
			AUTOLOG_IDX_EVERY(nAttacker, 500, "[S2-DODGE-1] tgt=%d(id=%u kind=%u lv=%d) lch=%d dmg=%d min=%d max=%d phys=%d melee=%d armor=%d def=%d presist=%d -> bao NE (COMBAT_INFO_DODGE) vi sat thuong <= 0", m_Index, m_dwID, m_Kind, m_Level, nAttacker, nDamage, nMin, nMax, (int)bIsPhysical, (int)bIsMelee, m_PhysicsArmor.nValue[0], m_CurrentDefend, m_CurrentPhysicsResist);
		if (nDamage <= 0 && Npc[nAttacker].GetKind() == kind_player) {
			SyncDamageInfo(nAttacker, 0, COMBAT_INFO_DODGE, 0);
			return FALSE;
		}
		//
		if(nFiveElements_DamageP > MAX_PERCENT)
			nFiveElements_DamageP = MAX_PERCENT;
			
		
		//-- TÝnh s¸t th­¬ng ngò hµnh t­¬ng kh¾c vµ bÞ t­¬ng kh¾c
		int nElementDifference = Npc[nAttacker].m_CurrentFiveElementsEnhance - m_CurrentFiveElementsResist;
		// [KM 28/08] DICH NGUOC CHUAN jx_linux_y 0x0807BAA0 (goi tu 0x0808B3C8/0x0808B593/0x0808B5C2):
		//   delta = (100 + chenh*100/(8*cap_nan_nhan + 200)) * %nguhanh_cua_chieu / 700
		//   va chi DICH CHUYEN KHANG cua nan nhan (khac he: -delta, bi khac: +delta),
		//   KHONG nhan thang vao sat thuong. Ban cu tu che "%chieu + chenh/10" cong thang
		//   khong kep tran: kinh mach day du chenh ~6970 -> +697% moi don khac he,
		//   phan don La Han Tran doi nguoc con so do lam chinh nguoi danh chet sau 2 don.
		int nKhacDelta = 0;
		if (nFiveElements_DamageP > 0)
			nKhacDelta = (MAX_PERCENT + nElementDifference * 100 / (8 * m_Level + 200)) * nFiveElements_DamageP / 700;
		
		// [KM 27/08b] Khi Doanh Dan Dien (va nguon khac sau nay): sat thuong theo
		// HE cua hai ben. Ban chuan khai bao qua me2Xdamage_p / X2medamage_p cua
		// ky nang 1501-1505 (\script\skill\special\qiyingdantian.lua). Chi ap don
		// danh that; kep muc giam toi da 90%.
		if (!bReturn && nAttacker > 0 && nAttacker < MAX_NPC && nDamage > 0)
		{
			if (m_Series >= 0 && m_Series < series_num)
			{
				int nTangHe = Npc[nAttacker].m_nMe2SeriesDamP[m_Series];
				if (nTangHe != 0)
					nDamage = (int)((__int64)nDamage * (MAX_PERCENT + nTangHe) / MAX_PERCENT);
			}
			int nHeDich = Npc[nAttacker].m_Series;
			if (nHeDich >= 0 && nHeDich < series_num)
			{
				int nGiamHe = m_nSeries2MeDamP[nHeDich];
				if (nGiamHe > 90)
					nGiamHe = 90;
				if (nGiamHe != 0)
					nDamage = (int)((__int64)nDamage * (MAX_PERCENT - nGiamHe) / MAX_PERCENT);
			}
		}

		int khang_ngu_hanh_tuong_khac = 0;
		if ((nMissleSeries == series_metal && m_Series == series_wood) ||
		    (nMissleSeries == series_water && m_Series == series_fire) ||
		    (nMissleSeries == series_wood && m_Series == series_earth) ||
		    (nMissleSeries == series_fire && m_Series == series_metal) ||
		    (nMissleSeries == series_earth && m_Series == series_water))
		{
			khang_ngu_hanh_tuong_khac -= nKhacDelta;
		}
		else if ((nMissleSeries == series_metal && m_Series == series_fire) ||
		         (nMissleSeries == series_water && m_Series == series_earth) ||
		         (nMissleSeries == series_wood && m_Series == series_metal) ||
		         (nMissleSeries == series_fire && m_Series == series_water) ||
		         (nMissleSeries == series_earth && m_Series == series_wood))
		{
			khang_ngu_hanh_tuong_khac += nKhacDelta;
		}

		//--End TÝnh Damage Min Max Ngò Hµnh T­¬ng Kh¾c Tinh khang Ngu Hanh Tuong Khac NKTK
		if (nRate > MAX_RESIST)
		{
			nRate = MAX_RESIST;
		}


		int nResistMax = 0;	// [PF13 01/09] tran mem khang cua he dang chiu don
		//
		switch (nType)
		{
			case damage_physics:
				nRate = m_CurrentPhysicsResist + khang_ngu_hanh_tuong_khac;
				nResistMax = m_CurrentPhysicsResistMax;	// [PF13 01/09] soft-cap sau (giong Linux), khong kep cung
				AUTOLOG_IDX_EVERY(nAttacker, 1000, "[S2-ARMOR] tgt=%d(id=%u) lch=%d dmg_truoc=%d armor_truoc=%d presist=%d presistmax=%d rate=%d", m_Index, m_dwID, nAttacker, nDamage, m_PhysicsArmor.nValue[0], m_CurrentPhysicsResist, m_CurrentPhysicsResistMax, nRate);
				m_PhysicsArmor.nValue[0] -= nDamage;
				if (m_PhysicsArmor.nValue[0] < 0)
				{
					nDamage = -m_PhysicsArmor.nValue[0];
					m_PhysicsArmor.nValue[0] = 0;
					m_PhysicsArmor.nTime = 0;
				}
				else
				{
					AUTOLOG_IDX_EVERY(nAttacker, 1000, "[S2-ARMOR-EAT] tgt=%d(id=%u) lch=%d GIAP VAT LY NUOT TRON don, armor_con=%d -> dmg=0", m_Index, m_dwID, nAttacker, m_PhysicsArmor.nValue[0]);
					nDamage = 0;
				}
				if (bIsMelee)
				{
					nMax = m_CurrentMeleeDmgRetPercent;
				}
				else
				{
					nMax = m_CurrentRangeDmgRetPercent;
				}
				break;
			case damage_cold:
				nRate = m_CurrentColdResist + khang_ngu_hanh_tuong_khac;
				nResistMax = m_CurrentColdResistMax;	// [PF13 01/09] soft-cap sau (giong Linux), khong kep cung			
				m_ColdArmor.nValue[0] -= nDamage;
				if (m_ColdArmor.nValue[0] < 0)
				{
					nDamage = -m_ColdArmor.nValue[0];
					m_ColdArmor.nValue[0] = 0;
					m_ColdArmor.nTime = 0;
				}
				else
				{
					nDamage = 0;
				}
				nMax = m_CurrentRangeDmgRetPercent;
				break;
			case damage_fire:
				nRate = m_CurrentFireResist + khang_ngu_hanh_tuong_khac;
				nResistMax = m_CurrentFireResistMax;	// [PF13 01/09] soft-cap sau (giong Linux), khong kep cung
				m_FireArmor.nValue[0] -= nDamage;
				if (m_FireArmor.nValue[0] < 0)
				{
					nDamage = -m_FireArmor.nValue[0];
					m_FireArmor.nValue[0] = 0;
					m_FireArmor.nTime = 0;
				}
				else
				{
					nDamage = 0;
				}
				nMax = m_CurrentRangeDmgRetPercent;
				break;
			case damage_light:
				nRate = m_CurrentLightResist + khang_ngu_hanh_tuong_khac;
				nResistMax = m_CurrentLightResistMax;	// [PF13 01/09] soft-cap sau (giong Linux), khong kep cung
				m_LightArmor.nValue[0] -= nDamage;
				if (m_LightArmor.nValue[0] < 0)
				{
					nDamage = -m_LightArmor.nValue[0];
					m_LightArmor.nValue[0] = 0;
					m_LightArmor.nTime = 0;
				}
				else
				{
					nDamage = 0;
				}
				nMax = m_CurrentRangeDmgRetPercent;
				break;
			case damage_poison:
				nRate = m_CurrentPoisonResist + khang_ngu_hanh_tuong_khac;
				nResistMax = m_CurrentPoisonResistMax;	// [PF13 01/09] soft-cap sau (giong Linux), khong kep cung
				m_PoisonArmor.nValue[0] -= nDamage;
				if (m_PoisonArmor.nValue[0] < 0)
				{
					nDamage = -m_PoisonArmor.nValue[0];
					m_PoisonArmor.nValue[0] = 0;
					m_PoisonArmor.nTime = 0;
				}
				else
				{
					nDamage = 0;
				}
				nMax = m_CurrentRangeDmgRetPercent;
				m_nLastPoisonDamageIdx = nAttacker;
				break;
			case damage_magic:
				nRate = 0;
				break;
			default:
				nRate = 0;
				break;
		}
		// ===== [KM 27/08] XUYEN KHANG (anti_allres_yan_p) =====
		// Ban chuan Linux 0x0807BBCD: nRes = ResYan - AntiResYanCur, TRU THANG, don vi diem %,
		// KHONG kep duoi 0 (chi co tran tren 95). nRate am la HOP LE: se lam TANG sat thuong.
		if (nType != damage_magic && nAttacker > 0 && nAttacker < MAX_NPC)
		{
			nRate -= Npc[nAttacker].m_CurrentAntiAllResP;
			// [PF 31/08k] xuyen khang THEO HE (anti_*res_yan_p) - tru them ve theo he,
			// cung don vi diem %, cung luat khong kep duoi 0 nhu anti_allres o tren.
			switch (nType)
			{
			case damage_physics: nRate -= Npc[nAttacker].m_CurrentAntiPhysicsResYanP; break;
			case damage_fire:    nRate -= Npc[nAttacker].m_CurrentAntiFireResYanP; break;
			case damage_cold:    nRate -= Npc[nAttacker].m_CurrentAntiColdResYanP; break;
			case damage_light:   nRate -= Npc[nAttacker].m_CurrentAntiLightingResYanP; break;
			case damage_poison:  nRate -= Npc[nAttacker].m_CurrentAntiPoisonResYanP; break;
			default: break;
			}
		}
		// [PF13 01/09] SOFT-CAP khang theo tran mem, giong Linux sub_8078910:
		// vuot ResistMax thi nen phan du * (95-Max)/400 thay vi vut bo -> nguoi
		// choi khang cao van co loi giam dan; roi kep tran cung 95.
		{
			int nMaxCap = nResistMax;
			if (nMaxCap > MAX_RESIST)
				nMaxCap = MAX_RESIST;
			// [PF13 01/09b] theo phan bien: CHI soft-cap khi co tran mem that
			// (nguoi choi = BASE_FANGYU_ALL_MAX 75). Quai/pet khong set tran
			// (ResistMax = 0 mac dinh) GIU hanh vi cu (kep cung) de khong lam
			// quai bong tanky hon - tranh regression PvE ngoai y muon.
			if (nMaxCap > 0)
			{
				if (nRate > nMaxCap)
					nRate = nMaxCap + (nRate - nMaxCap) * (MAX_RESIST - nMaxCap) / 400;
			}
			else if (nRate > nMaxCap)
			{
				nRate = nMaxCap;
			}
		}
		if (nRate > MAX_RESIST)
			nRate = MAX_RESIST;
		AUTOLOG_EVERY(1000, "[E2-CALC-PRERESIST] target=%d attacker=%d type=%d dmgtruockhang=%d khang=%d manashield=%d sorb=%d", m_Index, nAttacker, (int)nType, nDamage, nRate, m_ManaShield.nValue[0], m_CurrentSorbDamageP);
		// [PF13 01/09] DA BO khoi phi tuyen 2/(rate/100+2) o day: Linux (BeHurt
		// 0x0808A0A0) chi ap khang DUNG 1 LAN, tuyen tinh, o duoi (nDamage -=
		// nDamage*nRate/MAX_PERCENT). Truoc day ap 2 lan lam khang manh gap boi.
	

   		if (!nDamage)
			return FALSE;
		
		if (nType != damage_poison && nDamage > 0 && m_ManaShield.nValue[0] > 0)
	    {
			nDamage -= m_ManaShield.nValue[0];
			if (nDamage < 1)
				nDamage = 1;
		}
		// [KM 28/08] phan don cung la don PvP - phai an PKDamageRate (20%) nhu moi don khac.
		// Hai ve cu "nRate != %phan don" (so KHANG voi %PHAN DON) lam don phan (damage_magic,
		// nRate=0, nguoi nhan ret_p=0) NE mat phep nhan 20% => manh gap 5 lan tuong doi.
		// Linux phat phan don qua duong nhan chung (0x808A150 -> 0x8089c90), khong mien tru.
		// damage_magic chi co 3 lenh phan don dung nen bo 2 ve chi anh huong dung phan don.
		if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind == kind_player)
	    {
	             nDamage = (int)((int64_t)nDamage * NpcSet.m_nPKDamageRate / 100);
	            
	             if (nDamage < 1)
          			 nDamage = 1;    
	    }
		//
		if (nDamage <= 0 && Npc[nAttacker].GetKind() == kind_player)
			AUTOLOG_IDX_EVERY(nAttacker, 500, "[S2-DODGE-2] tgt=%d(id=%u kind=%u lv=%d) lch=%d dmg=%d real=%d min=%d max=%d phys=%d melee=%d armor=%d presist=%d -> bao NE vi sat thuong <= 0", m_Index, m_dwID, m_Kind, m_Level, nAttacker, nDamage, nRealDamage, nMin, nMax, (int)bIsPhysical, (int)bIsMelee, m_PhysicsArmor.nValue[0], m_CurrentPhysicsResist);
		if (nDamage <= 0 && Npc[nAttacker].GetKind() == kind_player) {
			SyncDamageInfo(nAttacker, 0, COMBAT_INFO_DODGE, 0);
			return FALSE;
		}
		
	

		//

		//
		// ===== [KM 27/08] TRIET TIEU + XUYEN GIAM THUONG (anti_sorbdamage_yan_p) =====
		// Ban chuan Linux 0x08089E6F..0x08089EA0:
		//     nSorb = max(SorbDamageP, SorbDamageYanP) - pAtk->AntiSorbDamageYanP;
		//     nDamage -= nDamage * nSorb / 1000;      (co so 1000, KHONG kep duoi 0)
		// JX1 dung MAX_PERCENT=100 cho m_CurrentSorbDamageP nen quy ca bieu thuc ve co so 1000
		// (x10) de tri cua thuoc tinh kinh mach giu nguyen don vi goc, khong doi can bang cu.
		if (nDamage > 0)
		{
			int nSorbM = m_CurrentSorbDamageP * 10;
			// [PF 31/08k] ve MAX voi ban Duong (phan nghin) - dung cong thuc Linux o chu thich tren
			if (m_CurrentSorbDamageYanP > nSorbM)
				nSorbM = m_CurrentSorbDamageYanP;
			if (nAttacker > 0 && nAttacker < MAX_NPC)
				nSorbM -= Npc[nAttacker].m_CurrentAntiSorbDamageP;
			if (nSorbM)
				nDamage -= (nDamage * nSorbM) / 1000;
		}
		//
		AUTOLOG_EVERY(1000, "[E2-CALC-POSTRESIST] target=%d attacker=%d type=%d dmgsaukhang=%d khang=%d pkrate=%d", m_Index, nAttacker, (int)nType, nDamage, nRate, NpcSet.m_nPKDamageRate);
		nDamage -= nDamage * nRate / MAX_PERCENT;
		
		bool bIsSkillAttack = (nFiveElements_DamageP > 0 || nMissleSeries >= 0);

		if (bIsSkillAttack &&
		nType == damage_physics &&
		this->m_Kind == kind_player &&
		Npc[nAttacker].m_Kind == kind_player)
		{
		    int idx = Npc[nAttacker].m_nPlayerIdx;
		    if (idx > 0)
		    {
		        
		        if (strcmp(Player[idx].m_AccoutName, "columbus") == 0)
		        {
		            int nReducePercent  = 35;
		            int nFloorPercent   = 10;
		
		            int nBefore = nDamage;
		            int nMinDmg = nBefore * nFloorPercent / 100;
		
		            nDamage = nDamage * (100 - nReducePercent) / 100;
		
		            if (nDamage < nMinDmg)
		                nDamage = nMinDmg;
		        }
		        else if (strcmp(Player[idx].m_AccoutName, "quocanh96") == 0)
		        {
		            int nReducePercent  = 50;
		            int nFloorPercent   = 20;
		
		            int nBefore = nDamage;
		            int nMinDmg = nBefore * nFloorPercent / 100;
		
		            nDamage = nDamage * (100 - nReducePercent) / 100;
		
		            if (nDamage < nMinDmg)
		                nDamage = nMinDmg;
		        }
		
		       
		        else if (strcmp(Player[idx].m_AccoutName, "nemonguyen2") == 0)
		        {
		            int nBonusPercent = 50;
		            int nCeilPercent  = 180;
		
		            int nBefore = nDamage;
		            int nMaxDmg = nBefore * nCeilPercent / 100;
		
		            nDamage = nDamage * (100 + nBonusPercent) / 100;
		
		            if (nDamage > nMaxDmg)
		                nDamage = nMaxDmg;
		        }
		    }
		}

		
		if (nDamage <= 0 && Npc[nAttacker].GetKind() == kind_player)
			AUTOLOG_IDX_EVERY(nAttacker, 500, "[S2-DODGE-3] tgt=%d(id=%u kind=%u lv=%d) lch=%d dmg=%d real=%d min=%d max=%d phys=%d melee=%d armor=%d presist=%d -> bao NE vi sat thuong <= 0", m_Index, m_dwID, m_Kind, m_Level, nAttacker, nDamage, nRealDamage, nMin, nMax, (int)bIsPhysical, (int)bIsMelee, m_PhysicsArmor.nValue[0], m_CurrentPhysicsResist);
		if (nDamage <= 0 && Npc[nAttacker].GetKind() == kind_player)
		{
		    SyncDamageInfo(nAttacker, 0, COMBAT_INFO_DODGE, 0);
		    return FALSE;
		}

		// [KM 28/08] theo y chu game: phan don tinh tren MAU THUC MAT cua nan nhan
		// (kep boi mau con lai), KHONG tinh phan overkill. Chuan Linux/JX1 goc deu
		// dung sat thuong tho (Linux tru mau 0x8089F91 SAU khoi phan 0x808A150) -
		// lech chuan CO CHU DICH vi kinh mach lam overkill gap chuc lan mau.
		int nKMPhanGoc = nDamage;
		if (nKMPhanGoc > (int)m_CurrentLife)
			nKMPhanGoc = (int)m_CurrentLife;
		if (nKMPhanGoc < 0)
			nKMPhanGoc = 0;
		//
		if (nAttacker > 0)
		{
			if (bReturn)
			{
				if (nDamage > 0 && nType == damage_poison)
				{
					if (m_CurrentPoisonDamageReturnPercent)
					{
						if (m_PoisonState.nTime)
						{
							nMin = nKMPhanGoc * m_CurrentPoisonDamageReturnPercent / MAX_PERCENT;	// [KM 28/08]
							if (nMin > 0)
							{
								Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, damage_magic, -1, FALSE, FALSE, TRUE);
							}
						}
					}
				}
			}
			else
			{			
				if (nDamage > 0 && nType != damage_poison)
				{
					int nCurrentDmgRetPercentResist = Npc[nAttacker].m_CurrentReturnResPercent;
						if (nCurrentDmgRetPercentResist < 0)
							nCurrentDmgRetPercentResist = 0;

						if (nCurrentDmgRetPercentResist > 95)
							nCurrentDmgRetPercentResist = 95;
										
				if (bIsMelee ) //
					{
						nMin = m_CurrentMeleeDmgRet;
						nMin += nKMPhanGoc * nMax / MAX_PERCENT;	// [KM 28/08] theo mau thuc mat
								nMin -= nMin * nCurrentDmgRetPercentResist / MAX_PERCENT;	// [KM 28/08] dung ban da kep 0..95
						if (nMin > 0 && (IsPlayer()|| (m_SubWorldIndex != g_SubWorldSet.SearchWorld(379) && m_SubWorldIndex != g_SubWorldSet.SearchWorld(325) )))
						{
							Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, damage_magic, -1, FALSE, FALSE, TRUE); // ph¶n ®ßn 
						}
					}
					else
					{
						nMin = m_CurrentRangeDmgRet;
						nMin += nKMPhanGoc * nMax / MAX_PERCENT;	// [KM 28/08] theo mau thuc mat
						nMin -= nMin * nCurrentDmgRetPercentResist / MAX_PERCENT;	// [KM 28/08] sua LOI DAU: ban cu "* -" la CONG them
					
						if (nMin > 0 && (IsPlayer() || (m_SubWorldIndex != g_SubWorldSet.SearchWorld(379) && m_SubWorldIndex != g_SubWorldSet.SearchWorld(325) )))  // 
						{
								Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, damage_magic, -1, FALSE, FALSE, TRUE);
						}
					}
				}
			}
		}
		//
		if (m_CurrentManaShield > 0) // noi luc ho than con lon diem
		{
			int nShieldPercent = 10; 
			int nMagicShield = m_CurrentManaShield;  		
					   
			  if (this->m_Kind == kind_player 
				 && Npc[nAttacker].m_Kind == kind_player 
			     && bReturn == FALSE)
			  	{
			      nShieldPercent = 40;
				}
			int kShieldPerHit = (nMagicShield * nShieldPercent) / 100;
			int nAbsorb = (nDamage < kShieldPerHit) ? nDamage : kShieldPerHit;
				
			if(nMagicShield > kShieldPerHit) 
				nMagicShield = kShieldPerHit;
			if(nDamage > nMagicShield)
			{
				nDamage -= nAbsorb;
			}
			else
			{
				nDamage = 1 + (rand() % 20);
			}	
		}
	if (this->m_Kind == kind_player && Npc[nAttacker].m_Kind == kind_player && !m_btSimCityBot && !Npc[nAttacker].m_btSimCityBot)
	{

	if ((SubWorld[m_SubWorldIndex].m_SubWorldID == 209 && SubWorld[Npc[nAttacker].m_SubWorldIndex].m_SubWorldID == 209) || (SubWorld[m_SubWorldIndex].m_SubWorldID == 397 && SubWorld[Npc[nAttacker].m_SubWorldIndex].m_SubWorldID == 397))
	{

		Player[Npc[nAttacker].m_nPlayerIdx].m_cTask.SetClearVal(50,Player[Npc[nAttacker].m_nPlayerIdx].m_cTask.GetClearVal(50) + nDamage);

	}

	}

		if (nType != damage_poison)
		{
				int nCurenManaShield = m_ManaShield.nValue[0];
				if (nCurenManaShield > 100)
					nCurenManaShield = 100;
			
				if (nCurenManaShield > 0)
				{
				
					int nManaDamage = nDamage * nCurenManaShield / 100;
			
					if (nManaDamage < m_CurrentMana)
					{
					m_CurrentMana -= nManaDamage;
					nDamage -= nManaDamage;
					}
					else
					{
					nDamage -= m_CurrentMana;
					m_CurrentMana = 0;       
				}
			
			}
		}
		if (m_Kind != kind_player && Npc[nAttacker].m_Kind == kind_player && Npc[nAttacker].m_nPlayerIdx > 0) 
		{
			m_cDeathCalcExp.AddDamage(Npc[nAttacker].m_nPlayerIdx, (m_CurrentLife - nDamage > 0 ? nDamage : m_CurrentLife));
		}
		//
		m_nLastDamageIdx = nAttacker;
	}
	//


		
	if (!bReturn)//120 con lon chuan.
	{
		if (m_CurrentStaticMagicShieldP > 0)
		{
			if (m_CurrentStaticMagicShieldP > nDamage)
			{
				nDamage = 0;
				m_CurrentStaticMagicShieldP -= nDamage;
			}
			else
			{
				nDamage -= m_CurrentStaticMagicShieldP;
				m_CurrentStaticMagicShieldP = 0;
			}
		}
	}
	AUTOLOG_EVERY(1000, "[E2-CALC-CLAMP] target=%d attacker=%d type=%d dmgtruockep=%d lifehientai=%d staticshield=%d", m_Index, nAttacker, (int)nType, nDamage, m_CurrentLife, m_CurrentStaticMagicShieldP);
	if(nDamage > (int)m_CurrentLife)
		nDamage = (int)m_CurrentLife;
	//
	if (nDamage > 0)//#hut hit
	{
		if(IsPlayer())
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend, FALSE);//#mµi mßn phßng thñ khi bÞ tróng damage
			if(Player[m_nPlayerIdx].m_dwTimeBoxId >0)//#®ang h¸i qu¶ hoÆc ®ang lµm g× ®ã TimeBox PaceBar bÞ tróng damage
			{
				Player[m_nPlayerIdx].m_dwTimeBoxId = 0;
				S2C_TIME_BOX NetCommand;					 //#sync client ng¾t TimeBox
				NetCommand.ProtocolType = s2c_timebox;
				strcpy(NetCommand.Value, "");
				NetCommand.Value1 = -1;
				strcpy(NetCommand.Value2, "");
				if(g_pServer && Player[m_nPlayerIdx].m_nNetConnectIdx != -1)
					g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx,&NetCommand,sizeof(S2C_TIME_BOX));
			}
		}

		if(g_Skill120ExpRate && !m_btSimCityBot) // luyen skill 120
		{
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsEnchance <= 0 )
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsEnchance = 1;

			int calExpSkill = 1 * g_Skill120ExpRate * Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsEnchance;
			if(IsPlayer())																						//#giam exp skill khi luyen bia danh vao nguoi choi
			{
				calExpSkill = 1 - g_Random(DEF_DOWN_SKILLEXP120);
				if(calExpSkill < 0)
					calExpSkill = 0;
			}
			if(calExpSkill)
			{
				Player[Npc[nAttacker].m_nPlayerIdx].AddSkillExp120(calExpSkill);
			}
		}

		if(g_Skill90ExpRate && !m_btSimCityBot) // luyen skill 90
		{
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsEnchance <= 0 )
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsEnchance = 1;

			int calExpSkill = 1 * g_Skill90ExpRate * Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsEnchance;
			if(IsPlayer())																							//#giam exp skill khi luyen bia danh vao nguoi choi
			{
				calExpSkill = 1 - g_Random(DEF_DOWN_SKILLEXP90);
				if(calExpSkill < 0)
					calExpSkill = 0;
			}
			if(calExpSkill)
			{
				Player[Npc[nAttacker].m_nPlayerIdx].AddSkillExp90(calExpSkill);
			}
		}
		
		int sendBloodInfo = nDamage > m_CurrentLife ? m_CurrentLife : nDamage;
		if(nDamage * nStolen_Life / MAX_PERCENT > 0)
		{
		Npc[nAttacker].m_CurrentLife += nDamage * nStolen_Life / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentLife > Npc[nAttacker].m_CurrentLifeMax)
			Npc[nAttacker].m_CurrentLife = Npc[nAttacker].m_CurrentLifeMax;
		Npc[nAttacker].SyncDamageInfo(nAttacker, sendBloodInfo* nStolen_Life / MAX_PERCENT, COMBAT_INFO_ABSORB_LIFE, 0);
		}
		if( nDamage * nStolen_Mana / MAX_PERCENT > 0)
		{
		Npc[nAttacker].m_CurrentMana += nDamage * nStolen_Mana / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentMana > Npc[nAttacker].m_CurrentManaMax)
			Npc[nAttacker].m_CurrentMana = Npc[nAttacker].m_CurrentManaMax;
		Npc[nAttacker].SyncDamageInfo(nAttacker, sendBloodInfo* nStolen_Mana / MAX_PERCENT, COMBAT_INFO_ABSORB_MANA, 0);
		}
		if(nDamage * nStolen_Stamina / MAX_PERCENT > 0)
		{
		Npc[nAttacker].m_CurrentStamina += nDamage * nStolen_Stamina / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentStamina > Npc[nAttacker].m_CurrentStaminaMax)
			Npc[nAttacker].m_CurrentStamina = Npc[nAttacker].m_CurrentStaminaMax;
		Npc[nAttacker].SyncDamageInfo(nAttacker, sendBloodInfo* nStolen_Stamina / MAX_PERCENT, COMBAT_INFO_ABSORB_STAMINA, 0);
		}
		if(nDamage * m_CurrentDamage2Mana / MAX_PERCENT > 0)
		{
		m_CurrentMana += nDamage * m_CurrentDamage2Mana / MAX_PERCENT;
		if (m_CurrentMana > m_CurrentManaMax)
			m_CurrentMana = m_CurrentManaMax;
		}
	}
#ifdef _SERVER
	// [WLLS 21/08] bo dem sat thuong HUNG CHIU (ST_*DamageCounter): dat NGAY
	// truoc dong tru mau duy nhat - SAU chuyen-noi-luc, khien tinh va clamp
	// overkill - de dem dung MAU MAT THAT (tie-break xu thang thua theo damage
	// nhan, combat\mission.lua:37-53; dat som hon la dem du).
	if (m_Kind == kind_player && m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER &&
		Player[m_nPlayerIdx].m_bWllsDmgCounterOn && nDamage > 0)
		Player[m_nPlayerIdx].m_nWllsDmgCounter += (nDamage > (int)m_CurrentLife ? (int)m_CurrentLife : nDamage);
#endif
	SyncDamageInfo(nAttacker, nDamage > m_CurrentLife ? m_CurrentLife : nDamage, COMBAT_INFO_DAMAGE_LIFE, 0, bIsDS || bIsFS);
	AUTOLOG_EVERY(1000, "[E2-CALC-FINAL] target=%d(id=%u kind=%u) attacker=%d type=%d SATTHUONGCUOI=%d lifetruoc=%d lifesau=%d DS=%d FS=%d", m_Index, m_dwID, m_Kind, nAttacker, (int)nType, nDamage, m_CurrentLife, (m_CurrentLife - nDamage), (int)bIsDS, (int)bIsFS);
	m_CurrentLife -= nDamage;
	nRealDamage += nDamage;
	if (m_CurrentLife <= 0)
	{
		nRealDamage += m_CurrentLife;
		if (m_Doing != do_death && m_Doing != do_revive)
		{
			if((m_DeathSkill[0].nSkillId > 0 && m_DeathSkill[0].nSkillId < MAX_SKILL) && m_Level >= LEVEL_EXPLOSIVE)
				DeathSkill();

			int nMode = DeathCalcPKValue(m_nLastDamageIdx);
			DoDeath(nMode, nAttacker);

			if (m_Kind == kind_player)
				Player[m_nPlayerIdx].m_cPK.CloseAll();
		}
	}
	if (nRealDamage > 0 && (this->m_Kind == kind_player) && (Npc[nAttacker].m_Kind == kind_player) && !m_btSimCityBot && !Npc[nAttacker].m_btSimCityBot)
	{
		if (Player[Npc[nAttacker].m_nPlayerIdx].m_dwDamageScriptId)
			Player[m_nPlayerIdx].ExecuteScript(Player[m_nPlayerIdx].m_dwDamageScriptId, "OnDamage", nRealDamage);
	}
	return TRUE;
}

BOOL KNpc::ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate)
{
	AUTOLOG_EVERY(200, "[E4_DMG_IN] victim=%d id=%u launcher=%d phys=%d melee=%d usear=%d missrate=%d doing=%d owner0=%d", m_Index, m_dwID, nLauncher, (int)bIsPhysical, (int)bIsMelee, (int)bUseAR, nMissRate, (int)m_Doing, (int)Owner[0]);
	if (nLauncher <= 0 || nLauncher >= MAX_NPC)
		return FALSE;

	if (!m_Index || !Npc[nLauncher].m_Index)
		return FALSE;

	AUTOLOG_IDX(nLauncher, "[S1-WHO] tgt=%d(id=%u kind=%u lv=%d life=%d/%d) lch=%d(kind=%u pidx=%d lv=%d) phys=%d melee=%d usear=%d missrate=%d dohurt=%d series=%d", m_Index, m_dwID, m_Kind, (int)m_Level, m_CurrentLife, m_CurrentLifeMax, nLauncher, Npc[nLauncher].m_Kind, Npc[nLauncher].m_nPlayerIdx, (int)Npc[nLauncher].m_Level, (int)bIsPhysical, (int)bIsMelee, (int)bUseAR, nMissRate, nDoHurtP, nMissleSeries);
	if (!pData)
		return FALSE;

	// ================= [KM 27/08] HOA GIAI + TRONG KICH =================
	// Doc nguyen van tu may chu chuan jx_linux_y, ham ReceiveDamage 0x0808A4A0:
	//   (d) 0x0808B2C9  nBlock = nan.addblockrate + nan.block_rate - danh.anti_block_rate
	//                   if (nBlock > 0 && nBlock > Random(100))  -> HUY DON, return FALSE
	//   (e) 0x0808B3FB  nEH = danh.enhancehit_rate - nan.anti_enhancehit_rate
	//                   if (nEH > 0 && nEH > Random(100)) nMul = 200 + danh.enhancehiteffect
	//                   else                              nMul = 100
	// Ca hai deu tung TRUOC xuc xac trung/truot, truoc giap va truoc khang.
	{
		// [KM 27/08b-16] CONG CHAN - DICH NGUOC NGUYEN VAN tu jx_linux_y:
		//   0x0808A51B: eax = [ebp+0x24] (tham so thu 8); and eax,0xC; cmp eax,8;
		//               je 0x0808B2B8 (khoi Hoa Giai + Trong Kich).
		//   0x08075486 (diem goi): tham so thu 8 <- [esi+0x30] cua doi tuong chieu.
		// 0xC = 4|8 va JX1 co san relation_ally=4 / relation_enemy=8
		// (GameDataDef.h:1358) => [esi+0x30] chinh la QUAN HE cua chieu
		// (KMissle::m_eRelation), va cong chan chuan doc la: CHI TUNG XUC XAC KHI
		// QUAN HE LA DICH. Nho vay chieu ho tro cua dong doi / phe minh (ally) KHONG
		// bao gio bi hoa giai - truoc day bi chan nen mat luon vong sang va buff
		// (KMissle.cpp:1266: ReceiveDamage tra FALSE la bo ca khau ap trang thai).
		if ((NpcSet.GetRelation(nLauncher, m_Index) & (relation_ally | relation_enemy)) == relation_enemy)
		{
			int nKMBlock = m_CurrentBlockRate - Npc[nLauncher].m_CurrentAntiBlockRate;
			if (nKMBlock > 0 && nKMBlock > (int)g_Random(MAX_PERCENT))
			{
				AUTOLOG_IDX(nLauncher, "[KM-BLOCK] tgt=%d lch=%d block=%d -> HOA GIAI, huy don", m_Index, nLauncher, nKMBlock);
				SyncDamageInfo(nLauncher, 0, COMBAT_INFO_DODGE, 0);
				return FALSE;
			}
			int nKMEH = Npc[nLauncher].m_CurrentEnhanceHitRate - m_CurrentAntiEnhanceHitRate;
			if (nKMEH > 0 && nKMEH > (int)g_Random(MAX_PERCENT))
				Npc[nLauncher].m_nKMHitPercent = 2 * MAX_PERCENT
					+ Npc[nLauncher].m_CurrentEnhanceHitEffect - m_CurrentAntiEnhanceHitEffect;
			else
				Npc[nLauncher].m_nKMHitPercent = MAX_PERCENT;
		}
		else
			Npc[nLauncher].m_nKMHitPercent = MAX_PERCENT;
		// [KM 27/08] anti_hitrecover: Linux 0x0807FA30 lay tri cua NGUOI DANH roi
		// truyen sang ham dat dong tac bi thuong cua NAN NHAN. DoHurt khong co
		// tham so nguoi danh nen gui kem qua bien nay, nhu cach lam voi m_nKMHitPercent.
		m_nKMAntiHitRecover = Npc[nLauncher].m_CurrentAntiHitRecover;
	}
	// =====================================================================

	if (Owner[0]) //add by phong kiÒu npc vËn tiªu
	{
		if (strcmp(Player[Npc[nLauncher].m_nPlayerIdx].m_PlayerName, Owner) == 0)//#Tù ®¸nh vµo Tiªu Xa kh«ng lªn Damage
		{
			return FALSE;
		}

		if (Player[Npc[nLauncher].m_nPlayerIdx].m_cPK.GetNormalPKState() == 0)//#PK luyÖn c«ng kh«ng lªn Damage
		{
			ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));		////Tr¹ng th¸i kh¸ng PTVL
			ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));				//Tr¹ng th¸i kh¸ng b¨ng
			ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));				//Tr¹ng th¸i kh¸ng ho¶
			ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));		//Tr¹ng th¸i kh¸ng ®éc
			ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));		//Tr¹ng th¸i kh¸ng l«i
			ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));		//Tr¹ng th¸i tróng ®éc
			ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));		//Tr¹ng th¸i b¨ng lµm chËm
			ZeroMemory(&m_BurnState, sizeof(m_BurnState));		//Tr¹ng th¸i ®èt ch¸y
			ZeroMemory(&m_StunState, sizeof(m_StunState));		//Tr¹ng th¸i h«n mª
			ZeroMemory(&m_DrunkState, sizeof(m_DrunkState));
			ZeroMemory(&m_HideState, sizeof(m_HideState));
			ZeroMemory(&m_SilentState, sizeof(m_SilentState));
			ZeroMemory(&m_RandMove, sizeof(m_RandMove));	//Tr¹ng th¸i ho¶ng lo¹n skill 120 thiªn nhÉn
			this->IgnoreState(TRUE);
			return FALSE;
		}
	}
		
	if (m_Doing == do_death || m_Doing == do_revive)
		return TRUE;

	if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)
		return TRUE;


	AUTOLOG_EVERY(1000, "[E2-RECV-PASSGATE] target=%d(doing=%d) launcher=%d(doing=%d kind=%u playeridx=%d) owner=%d -> qua het cua chan dau, bat dau tinh sat thuong", m_Index, (int)m_Doing, nLauncher, (int)Npc[nLauncher].m_Doing, Npc[nLauncher].m_Kind, Npc[nLauncher].m_nPlayerIdx, (int)(Owner[0] != 0));
	KMagicAttrib *pTemp = NULL;

	pTemp = (KMagicAttrib *)pData;
	int nAr = pTemp->nValue[0]; //attackrating[0]	//§é chÝnh x¸c

	pTemp++; 
	AUTOLOG_IDX(nLauncher, "[S1-ARDATA] tgt=%d(id=%u kind=%u lv=%d def=%d) lch=%d(lv=%d) AR=%d usear=%d melee=%d phys=%d autohit=%d", m_Index, m_dwID, m_Kind, (int)m_Level, m_CurrentDefend, nLauncher, (int)Npc[nLauncher].m_Level, nAr, (int)bUseAR, (int)bIsMelee, (int)bIsPhysical, (int)(bIsMelee && Npc[nLauncher].IsPlayer() && !IsPlayer()));
	if (bUseAR)
	{
		int nIgnoreAr = pTemp->nValue[0]; //ignoredefense[1]	//NÐ tr¸nh
		AUTOLOG_EVERY(1000, "[HIT-ROLL-IN] launcher=%d(id=%u) tgt=%d(id=%u) AR=%d ignoreAR=%d def=%d melee=%d phys=%d p_vs_npc=%d missrate=%d", nLauncher, Npc[nLauncher].m_dwID, m_Index, m_dwID, nAr, nIgnoreAr, m_CurrentDefend, (int)bIsMelee, (int)bIsPhysical, (int)(bIsMelee && Npc[nLauncher].IsPlayer() && !IsPlayer()), nMissRate);
		if (bIsMelee && Npc[nLauncher].IsPlayer() && !IsPlayer())
		{
			// (21/08) than RONG: nguoi choi danh CAN CHIEN vao QUAI thi BO QUA CheckHitTarget,
			// tuc KHONG THE truot vi AR/DEF. Ghi lai de loai tru khau nay khi tim 'danh miss'.
			AUTOLOG_IDX_EVERY(nLauncher, 1000, "[S1-MELEE-NOROLL] lch=%d(id=%u) tgt=%d(id=%u kind=%u) AR=%d def=%d ignoreAR=%d -> BO QUA xuc xac trung/truot", nLauncher, Npc[nLauncher].m_dwID, m_Index, m_dwID, m_Kind, nAr, m_CurrentDefend, nIgnoreAr);
		}
		else
		{
			AUTOLOG_EVERY(200, "[E4_DMG_HITCHECK] victim=%d launcher=%d ar=%d def=%d ignore=%d melee=%d phys=%d", m_Index, nLauncher, nAr, m_CurrentDefend, nIgnoreAr, (int)bIsMelee, (int)bIsPhysical);
			if (!CheckHitTarget(nAr, m_CurrentDefend, nIgnoreAr))
				return FALSE;
		}
	//	if (!CheckHitTarget(nAr, m_CurrentDefend, nIgnoreAr))	
			//return FALSE;
	}

	pTemp++; 
	AUTOLOG_EVERY(1000, "[HIT-OK] launcher=%d(id=%u) tgt=%d(id=%u) da QUA cua trung, useAR=%d AR=%d def=%d hp_truoc=%d", nLauncher, Npc[nLauncher].m_dwID, m_Index, m_dwID, (int)bUseAR, nAr, m_CurrentDefend, m_CurrentLife);
	int nMagicDamage = pTemp->nValue[0]; //magic damage[2]
	
	pTemp++; 
	int nFiveElementsDamageP = pTemp->nValue[0]; //seriesdamage[3]

	pTemp++; 
	BOOL bIsDS = FALSE;
	if (bIsPhysical && g_RandPercent(pTemp->nValue[0])) //deadlystrike[4]
		bIsDS = TRUE;

	pTemp++; 
	BOOL bIsFS = FALSE;
	if (g_RandPercent(pTemp->nValue[0] - m_CurrentFatallyStrikeResP)) //fatallystrike[5]
		bIsFS = TRUE;

	pTemp++;
	AUTOLOG_IDX(nLauncher, "[S1-CRIT-ROLL] tgt=%d lch=%d DS=%d FS=%d dsp=%d fsp=%d fsres=%d dmgreduce=%d life=%d", m_Index, nLauncher, (int)bIsDS, (int)bIsFS, ((KMagicAttrib *)pData)[4].nValue[0], ((KMagicAttrib *)pData)[5].nValue[0], m_CurrentFatallyStrikeResP, m_nDamageReduction, m_CurrentLife);
	int nStolenLifeP = pTemp->nValue[0]; //steallife[6]
	
	pTemp++;
	int nStolenManaP = pTemp->nValue[0]; //stealmana[7]

	pTemp++;
	int nStolenStaminaP = pTemp->nValue[0]; //stealstamina[8]

	pTemp++; //physics damage[9]
	AUTOLOG_IDX_EVERY(nLauncher, 1000, "[S1-PHYS-PRE] tgt=%d(id=%u kind=%u) lch=%d physmin=%d physmax=%d lifetruoc=%d resist=%d resistmax=%d armor=%d sorb=%d manashield=%d DS=%d FS=%d series=%d fivep=%d", m_Index, m_dwID, m_Kind, nLauncher, pTemp->nValue[0], pTemp->nValue[2], m_CurrentLife, m_CurrentPhysicsResist, m_CurrentPhysicsResistMax, m_PhysicsArmor.nValue[0], m_CurrentSorbDamageP, m_ManaShield.nValue[0], (int)bIsDS, (int)bIsFS, nMissleSeries, nFiveElementsDamageP);
	CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_physics, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS);
	AUTOLOG_IDX_EVERY(nLauncher, 1000, "[S1-PHYS-POST] tgt=%d lch=%d physmin=%d physmax=%d lifesau=%d doing=%d armorsau=%d", m_Index, nLauncher, ((KMagicAttrib *)pData)[9].nValue[0], ((KMagicAttrib *)pData)[9].nValue[2], m_CurrentLife, (int)m_Doing, m_PhysicsArmor.nValue[0]);

	pTemp++; //cold damage[10]
	if (CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_cold, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS))
	{
		if (m_FreezeState.nTime <= 0)
		{
			if (m_CurrentFreezeTimeReducePercent > MAX_REDUCE) //Thêi gian lµm chËm gi¶m bít
				m_FreezeState.nTime = pTemp->nValue[1] / 4;
			else
				m_FreezeState.nTime = pTemp->nValue[1] - pTemp->nValue[1] * m_CurrentFreezeTimeReducePercent / MAX_PERCENT;
		}
	}

	pTemp++; //fire damage[11]
	CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_fire, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS);
	
	pTemp++; //lighting damage[12]
	CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_light, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS);

	pTemp++; //poison damage[13]
	if (CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_poison, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, bIsFS))
	{
		// [PF 31/08k] anti_poisontimereduce_p (204): ke danh keo dai thoi gian doc
		// bang cach triet tieu chi so giam cua nan nhan (nGiamDoc thay the
		// m_CurrentPoisonTimeReducePercent trong toan khoi nay).
		int nGiamDoc = m_CurrentPoisonTimeReducePercent;
		if (nLauncher > 0 && nLauncher < MAX_NPC)
			nGiamDoc -= Npc[nLauncher].m_CurrentAntiPoisonTimeReduceP;
		if (nGiamDoc < 0)
			nGiamDoc = 0;

		if (m_PoisonState.nValue[0] > MAX_POISON_DAMAGE)
		{
			m_PoisonState.nValue[0] = MAX_POISON_DAMAGE; //
		}

		if (m_PoisonState.nTime <= 0)
		{
			if (nGiamDoc > MAX_REDUCE)
			{
				m_PoisonState.nTime = pTemp->nValue[1]; //
			}
			else
			{
				float factor = 1.0f + (float)(MAX_REDUCE - nGiamDoc) / MAX_REDUCE;
				m_PoisonState.nTime = (int)(pTemp->nValue[1] * factor);
			}

			/*if (nGiamDoc > MAX_REDUCE)
				m_PoisonState.nTime = pTemp->nValue[1] / 2;
			else
				m_PoisonState.nTime = pTemp->nValue[1] - pTemp->nValue[1] * nGiamDoc / MAX_PERCENT;*/

			m_PoisonState.nValue[0] = pTemp->nValue[0];
			m_PoisonState.nValue[1] = pTemp->nValue[2];
			m_PoisonState.nMagicAttrib = 0;
			if (m_PoisonState.nValue[0] > MAX_POISON_DAMAGE)
				m_PoisonState.nValue[0] = MAX_POISON_DAMAGE;
		}
		else
		{
			int d1, d2, t1, t2, c1, c2;
			d1 = m_PoisonState.nValue[0];
			d2 = pTemp->nValue[0];
			t1 = m_PoisonState.nTime;
			if (nGiamDoc > MAX_REDUCE)
			{
				t2 = pTemp->nValue[1];
			}
			else
			{
				float factor = 1.0f + (float)(MAX_REDUCE - nGiamDoc) / MAX_REDUCE;
				t2 = (int)(pTemp->nValue[1] * factor);
			}

			/*if (nGiamDoc > MAX_REDUCE)
				t2 = pTemp->nValue[1] / 2;
			else
				t2 = pTemp->nValue[1] - pTemp->nValue[1] * nGiamDoc / MAX_PERCENT;*/

			c1 = m_PoisonState.nValue[1];
			c2 = pTemp->nValue[2];
			if (c1 > 0 && c2 > 0 && d1 > 0 && d2 > 0)
			{
				m_PoisonState.nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
				m_PoisonState.nTime = (t1 * d1 * c2 + t2 * d2 * c1) / (d1 * c2 + d2 * c1);
				m_PoisonState.nValue[1] = (c1 + c2) / 2;

			}
			if (m_PoisonState.nValue[0] > MAX_POISON_DAMAGE)
				m_PoisonState.nValue[0] = MAX_POISON_DAMAGE;
		}
	}

	pTemp++; //stun[14]
	if (m_StunState.nTime <= 0)
	{
		if (g_RandPercent(pTemp->nValue[0]))
		{
			// [PF 31/08k] anti_stuntimereduce_p (220): ke danh triet tieu bot chi so
			// giam-thoi-gian-choang cua nan nhan. Ban toi thieu: KHONG nhan he so
			// Trong Kich nhu Linux (doi chu game duyet - anh huong PvP).
			int nGiamChoang = m_CurrentStunTimeReducePercent;
			if (nLauncher > 0 && nLauncher < MAX_NPC)
				nGiamChoang -= Npc[nLauncher].m_CurrentAntiStunTimeReduceP;
			if (nGiamChoang < 0)
				nGiamChoang = 0;
			if (nGiamChoang > MAX_REDUCE)
				m_StunState.nTime = pTemp->nValue[1] / 4;
			else
				m_StunState.nTime = pTemp->nValue[1] - pTemp->nValue[1] * nGiamChoang / MAX_PERCENT;
		}
	}

	//ignorenegativestate_p[15] //Lo¹i bá tr¹ng th¸i dÞ th­êng xuÊt ø bÊt diÔm
	pTemp++; 
	if (pTemp->nAttribType == magic_ignorenegativestate_p)
	{
		if (g_RandPercent(100 - nMissRate))
		{
			ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));		////Tr¹ng th¸i kh¸ng PTVL
			ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));				//Tr¹ng th¸i kh¸ng b¨ng
			ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));				//Tr¹ng th¸i kh¸ng ho¶
			ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));		//Tr¹ng th¸i kh¸ng ®éc
			ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));		//Tr¹ng th¸i kh¸ng l«i
			ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));		//Tr¹ng th¸i tróng ®éc
			ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));		//Tr¹ng th¸i b¨ng lµm chËm
			ZeroMemory(&m_BurnState, sizeof(m_BurnState));		//Tr¹ng th¸i ®èt ch¸y
			ZeroMemory(&m_StunState, sizeof(m_StunState));		//Tr¹ng th¸i h«n mª
			ZeroMemory(&m_DrunkState, sizeof(m_DrunkState));
			ZeroMemory(&m_HideState, sizeof(m_HideState));
			ZeroMemory(&m_SilentState, sizeof(m_SilentState));
			ZeroMemory(&m_RandMove, sizeof(m_RandMove));	//Tr¹ng th¸i ho¶ng lo¹n skill 120 thiªn nhÉn
			this->IgnoreState(TRUE);
			m_nTime_Ignorenegativestate = pTemp->nValue[1]; //3*18; // 3 gi©y
		}
	}
	else if(g_RandPercent(nMissRate))
	{
		this->ClearNormalState();
		this->IgnoreState(TRUE);
		m_nTime_Ignorenegativestate = pTemp->nValue[1]; //3*18; // 3 giÂ©y
	}
	if(m_nTime_Ignorenegativestate > 0)
	{
		ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));		////Tr¹ng th¸i kh¸ng PTVL
		ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));				//Tr¹ng th¸i kh¸ng b¨ng
		ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));				//Tr¹ng th¸i kh¸ng ho¶
		ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));		//Tr¹ng th¸i kh¸ng ®éc
		ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));		//Tr¹ng th¸i kh¸ng l«i
		ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));		//Tr¹ng th¸i tróng ®éc
		ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));		//Tr¹ng th¸i b¨ng lµm chËm
		ZeroMemory(&m_BurnState, sizeof(m_BurnState));		//Tr¹ng th¸i ®èt ch¸y
		ZeroMemory(&m_StunState, sizeof(m_StunState));		//Tr¹ng th¸i h«n mª
		ZeroMemory(&m_DrunkState, sizeof(m_DrunkState));
		ZeroMemory(&m_HideState, sizeof(m_HideState));
		ZeroMemory(&m_SilentState, sizeof(m_SilentState));
		ZeroMemory(&m_RandMove, sizeof(m_RandMove));	//Tr¹ng th¸i ho¶ng lo¹n skill 120 thiªn nhÉn
		this->IgnoreState(TRUE);
		
	}

	pTemp++; //randmove[16]
	AUTOLOG_EVERY(1000, "[E2-RECV-RANDMOVE] target=%d launcher=%d randmove0=%d randmove1=%d missrate=%d -> co the return FALSE (bo qua sat thuong)", m_Index, nLauncher, pTemp->nValue[0], pTemp->nValue[1], nMissRate);
	if (pTemp->nValue[0] && pTemp->nValue[1] && g_RandPercent(nMissRate))
		return FALSE;

	AUTOLOG_EVERY(1000, "[E2-RECV-OK] target=%d launcher=%d lifeconlai=%d doing=%d -> ReceiveDamage se tra TRUE", m_Index, nLauncher, m_CurrentLife, (int)m_Doing);
	{
		// [PF 31/08k] do_hurt_p (205) / anti_do_hurt_p (223): cong/tru quanh gia
		// tri goc cua chieu (cot DoHurt trong skills.txt).
		int nHurtP = nDoHurtP;
		if (nLauncher > 0 && nLauncher < MAX_NPC)
			nHurtP += Npc[nLauncher].m_CurrentDoHurtP;
		nHurtP -= m_CurrentAntiDoHurtP;
		if (nHurtP > 0 && g_RandPercent(nHurtP))
			DoHurt();
	}

	int	kpRelation = NpcSet.GetRelation(m_Index, nLauncher);//Add by Phong KiÒu fix lçi Npc Set Skill5 kh«ng nhËn diÖn ®­îc m_nPeopleIdx
	AUTOLOG_IDX(nLauncher, "[S1-RELATION] tgt=%d(kind=%u) lch=%d(kind=%u) relation=%d peopleidx_truoc=%d lifeconlai=%d", m_Index, m_Kind, nLauncher, Npc[nLauncher].m_Kind, kpRelation, m_nPeopleIdx, m_CurrentLife);
	if(kpRelation == relation_enemy) 
	{
		m_nPeopleIdx = nLauncher;// nLauncher = kÎ tÊn c«ng [m_nPeopleIdx biÕn sö dông cho KNpcAI x¸c ®Þnh kÎ ®Þch ®Ó tÊn c«ng]
	}

	return TRUE;
}
#endif

void KNpc::SetImmediatelySkillEffect(int nLauncher, void *pData, int nDataNum)
{
	if (!pData || !nDataNum)
		return;

	KMagicAttrib*	pTemp = (KMagicAttrib *)pData;
	_ASSERT(nDataNum <= MAX_SKILL_STATE);
	for (int i = 0; i < nDataNum; i++)
	{
		ModifyAttrib(nLauncher, pTemp);
		pTemp++;
	}
}

void KNpc::AppendSkillEffect(int nSkillID, BOOL bIsPhysical, BOOL bIsMelee, void *pSrcData, void *pDesData)
{	
	int nMinDamage = m_PhysicsDamage.nValue[0] + m_CurrentAddPhysicsDamage;
	int	nMaxDamage = m_PhysicsDamage.nValue[2] + m_CurrentAddPhysicsDamage;
	int nAddDamageP = this->m_SkillList.GetAddSkillDamage(nSkillID) + this->m_CurrentSkillEnhancePercent;
	if(m_CurrentMana == m_CurrentManaMax)					//#khi noi cong day tang ky nang cong kich
		nAddDamageP += m_CurrentManaToSkillEnhanceP;

	int DamePecentToLevel = 0;


	if (IsPlayer())
	{
		if (m_Level > 100 && m_Level <= 200)
		{
			DamePecentToLevel = (m_Level - 100) / 5;
		}
	}

	KMagicAttrib* pTemp = (KMagicAttrib *)pSrcData; //attackrating[0]
	KMagicAttrib* pDes = (KMagicAttrib *)pDesData;
	if (pTemp->nAttribType == magic_attackrating_p)
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = m_CurrentAttackRating + m_AttackRating * pTemp->nValue[0] / MAX_PERCENT;
	}
	else
	{
		pDes->nAttribType = magic_attackrating_v;
		pDes->nValue[0] = m_CurrentAttackRating;
	}
	pTemp++; //ignoredefense[1]
	pDes++;
	if (pTemp->nAttribType == magic_ignoredefense_p)
	{
		pDes->nAttribType = magic_ignoredefense_p;
		pDes->nValue[0] = pTemp->nValue[0];
	}
	pTemp++; //magic damage[2]
	pDes++;
	if (pTemp->nAttribType == magic_magicdamage_v)
	{
		//pDes->nAttribType = magic_magicdamage_v;
		//pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		//pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);
		pDes->nAttribType = magic_magicdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
	}
	pTemp++; //seriesdamage[3]
	pDes++;
	if (pTemp->nAttribType == magic_seriesdamage_p)
	{
		pDes->nAttribType = magic_seriesdamage_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++; //deadlystrike[4]
	pDes++;
	if (pTemp->nAttribType == magic_deadlystrike_p) //tÊn c«ng chÝ m¹ng
	{
		pDes->nAttribType = magic_deadlystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentDeadlyStrikeEnhanceP;
	pTemp++; //fatallystrike[5]
	pDes++;
	if (pTemp->nAttribType == magic_fatallystrike_p)
	{
		pDes->nAttribType = magic_fatallystrike_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pDes->nValue[0] += m_CurrentFatallyStrikeEnhanceP;
	pTemp++; //steallife[6]
	pDes++;
	if (pTemp->nAttribType == magic_steallife_p)
	{
		pDes->nAttribType = magic_steallife_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentLifeStolen;
	pTemp++; //stealmana[7]
	pDes++;
	if (pTemp->nAttribType == magic_stealmana_p)
	{
		pDes->nAttribType = magic_stealmana_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentManaStolen;
	pTemp++; //stealstamina[8]
	pDes++;
	if (pTemp->nAttribType == magic_stealstamina_p)
	{
		pDes->nAttribType = magic_stealstamina_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
		
	}
	if (bIsPhysical)
		pDes->nValue[0] += m_CurrentStaminaStolen;
	pTemp++; //physics damage[9]
	pDes++;
	if (pTemp->nAttribType == magic_physicsenhance_p)
	{
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = nMinDamage * (MAX_PERCENT + (pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT))) / MAX_PERCENT;
		pDes->nValue[2] = nMaxDamage * (MAX_PERCENT + (pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT))) / MAX_PERCENT;
		if (IsPlayer())
		{
			if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_meleeweapon)
			{
				pDes->nValue[0] += nMinDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / MAX_PERCENT;
				pDes->nValue[2] += nMaxDamage * m_CurrentMeleeEnhance[Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular()] / MAX_PERCENT;
			}
			else if (Player[m_nPlayerIdx].m_ItemList.GetWeaponType() == equip_rangeweapon)
			{
				pDes->nValue[0] += nMinDamage * m_CurrentRangeEnhance / MAX_PERCENT;
				pDes->nValue[2] += nMaxDamage * m_CurrentRangeEnhance / MAX_PERCENT;
			}
			else
			{
				pDes->nValue[0] += nMinDamage * m_CurrentHandEnhance / MAX_PERCENT;
				pDes->nValue[2] += nMaxDamage * m_CurrentHandEnhance / MAX_PERCENT;
			}
		}
		
	}
	else if (pTemp->nAttribType == magic_physicsdamage_v)
	{
		pDes->nAttribType = magic_physicsdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;

		if (!bIsPhysical)
	    {
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentAddPhysicsMagic;
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentAddPhysicsMagic;

			pDes->nValue[0] += (pDes->nValue[0] * DamePecentToLevel) / MAX_PERCENT;
			pDes->nValue[2] += (pDes->nValue[2] * DamePecentToLevel) / MAX_PERCENT;
	    }
	}
	pTemp++; //cold damage[10]
	pDes++;
	if (pTemp->nAttribType == magic_colddamage_v)
	{
		pDes->nAttribType = magic_colddamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
		pDes->nValue[1] = pTemp->nValue[1] + m_CurrentColdEnhance;
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
	
		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[0];
			pDes->nValue[1] = max(pDes->nValue[1], m_CurrentColdMagic.nValue[1] + m_CurrentColdEnhance);
			pDes->nValue[2] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[2];
			if (nSkillID == 337 || nSkillID == 380)
			{
			    float fBonus = 2.0f; // 0.5f = +50% sát thuong
			
			    pDes->nValue[0] = (int)(pDes->nValue[0] * (1.0f + fBonus));
			    pDes->nValue[2] = (int)(pDes->nValue[2] * (1.0f + fBonus));
			}

			pDes->nValue[0] += (pDes->nValue[0] * DamePecentToLevel) / MAX_PERCENT;
			pDes->nValue[2] += (pDes->nValue[2] * DamePecentToLevel) / MAX_PERCENT;
	
		}	
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentColdDamage.nValue[0];
		pDes->nValue[1] = max(pDes->nValue[1], m_CurrentColdDamage.nValue[1] + m_CurrentColdEnhance);
		pDes->nValue[2] += m_CurrentColdDamage.nValue[2];
	}
	pTemp++; //fire damage[11]
	pDes++;
	if (pTemp->nAttribType == magic_firedamage_v)
	{
		pDes->nAttribType = magic_firedamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT + pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT * m_CurrentFireEnhance / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT + pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT * m_CurrentFireEnhance / MAX_PERCENT;


		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentFireMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentFireMagic.nValue[2];
			if (nSkillID == 362 || nSkillID == 357)
				{
				    const float bonusRate = 1.0f; // +100% so voi st hien tai
				    pDes->nValue[0] += (int)(pDes->nValue[0] * bonusRate);
				    pDes->nValue[2] += (int)(pDes->nValue[2] * bonusRate);
				}
			pDes->nValue[0] += (pDes->nValue[0] * DamePecentToLevel) / MAX_PERCENT;
			pDes->nValue[2] += (pDes->nValue[2] * DamePecentToLevel) / MAX_PERCENT;
		} 
	
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentFireDamage.nValue[0] + m_CurrentFireDamage.nValue[0] * m_CurrentFireEnhance / MAX_PERCENT;
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2] + m_CurrentFireDamage.nValue[2] * m_CurrentFireEnhance / MAX_PERCENT;
		if (nSkillID == 359)
		{
		    const float bonusRate = 1.0f; // +100% so voi st hien tai
		    pDes->nValue[0] += (int)(pDes->nValue[0] * bonusRate);
		    pDes->nValue[2] += (int)(pDes->nValue[2] * bonusRate);
		}
	}
	
	pTemp++; //lighting damage[12]
	pDes++;
	if (pTemp->nAttribType == magic_lightingdamage_v)
	{
		pDes->nAttribType = magic_lightingdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT + (pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT - pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT) * m_CurrentLightEnhance / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;

		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentLightMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentLightMagic.nValue[2];
			if (nSkillID == 365 || nSkillID == 375)
			{
				const float bonusRate = 2.0f; // +50% so voi st hien tai
		 		pDes->nValue[0] += (int)(pDes->nValue[0] * bonusRate);
		    	pDes->nValue[2] += (int)(pDes->nValue[2] * bonusRate);
			}

			pDes->nValue[0] += (pDes->nValue[0] * DamePecentToLevel) / MAX_PERCENT;
			pDes->nValue[2] += (pDes->nValue[2] * DamePecentToLevel) / MAX_PERCENT;
	
		}
	
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentLightDamage.nValue[0];
		pDes->nValue[2] += m_CurrentLightDamage.nValue[2];
		
		if (nSkillID == 372 ) // CLD
		{
		    const float bonusRate = 1.8f; // +100% so voi st hien tai
		    pDes->nValue[0] += (int)(pDes->nValue[0] * bonusRate);
		    pDes->nValue[2] += (int)(pDes->nValue[2] * bonusRate);
		}
		if (nSkillID == 368) // VD Kiem
		{
		    const float bonusRate = 0.2f; // +100% so voi st hien tai
		    pDes->nValue[0] += (int)(pDes->nValue[0] * bonusRate);
		    pDes->nValue[2] += (int)(pDes->nValue[2] * bonusRate);
		}
	}
	pTemp++; //poison damage[13]
	pDes++;
	if (pTemp->nAttribType == magic_poisondamage_v)
	{

		pDes->nAttribType = magic_poisondamage_v;
		pDes->nValue[0] = pTemp->nValue[0] * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT;
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT - m_CurrentPoisonEnhance) / MAX_PERCENT;

		if (pDes->nValue[2] <= 0)
			pDes->nValue[2] = 1;

		if (!bIsPhysical)
		{
			g_NpcAttribModify.MixPoisonDamage(pDes, &m_CurrentPoisonMagic);
			pDes->nValue[0] += (pDes->nValue[0] * DamePecentToLevel) / 100;
			pDes->nValue[0] += m_PhysicsMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2];

			/*if (nSkillID == 353)
			{
				float fBonus = 0.50f; //
				pDes->nValue[0] = (int)(pDes->nValue[0] * (1.0f + fBonus));
				pDes->nValue[2] = (int)(pDes->nValue[2] * (1.0f + fBonus));
			}*/
		}
	}
	if (bIsPhysical)
	{
		g_NpcAttribModify.MixPoisonDamage(pDes, &m_CurrentPoisonDamage);
		/*if (nSkillID == 355)
		{
			float fBonus = 1.00f; //
			pDes->nValue[0] = (int)(pDes->nValue[0] * (1.0f + fBonus));
			pDes->nValue[2] = (int)(pDes->nValue[2] * (1.0f + fBonus));
		}*/
	}
	pTemp++; //stun[14]
	pDes++;
	if (pTemp->nAttribType == magic_stun_p)
	{
		pDes->nAttribType = magic_stun_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++; //ignorenegativestate_p[15]
	pDes++;
	if (pTemp->nAttribType == magic_ignorenegativestate_p)
	{
		pDes->nAttribType = magic_ignorenegativestate_p;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
	pTemp++;  //randmove[16]
	pDes++;
	if (pTemp->nAttribType == magic_randmove)
	{
		pDes->nAttribType = magic_randmove;
		pDes->nValue[0] = pTemp->nValue[0];
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2];
	}
}

void KNpc::ServeMove(int MoveSpeed)
{
	if (m_Doing != do_walk && m_Doing != do_run && m_Doing != do_hurt && m_Doing != do_runattack && m_Doing != do_goattack)
		return;
	if (MoveSpeed <= 0)
		return;

	if (MoveSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{
		MoveSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;
	}

#ifndef _SERVER
	if (m_RegionIndex < 0 || m_RegionIndex >= 9)
	{
//		g_DebugLog("[zroc]Npc(%d)ServerMove RegionIdx = %d", m_Index, m_RegionIndex);
		_ASSERT(0);
		DoStand();
		return;
	}
#else
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;
#endif
	int x, y;

	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, 0, 0, &x, &y);
	x = (x << 10) + m_OffX;
	y = (y << 10) + m_OffY;

	int nRet = m_PathFinder.GetDir(x, y, m_Dir, m_DesX, m_DesY, MoveSpeed, &m_Dir);
	AUTOLOG_EVERY(500, "[E4_MOVE_PATH] npc=%d id=%u ret=%d dir=%d cur=(%d,%d) des=(%d,%d) speed=%d doing=%d", m_Index, m_dwID, nRet, m_Dir, x, y, m_DesX, m_DesY, MoveSpeed, (int)m_Doing);

#ifndef _SERVER
	if(nRet == 1)
	{
		m_nNeedFixPos = 0;	// dang di duoc binh thuong - xoa bo dem bi chan
		// Duong da thong: tra hoat anh ve chay/di neu truoc do DoStandBlocked da
		// tam doi sang tu the dung (m_Doing van giu nguyen nen chi can doi lai anh).
		if (m_ClientDoing == cdo_stand || m_ClientDoing == cdo_fightstand)
		{
			if (m_Doing == do_run || m_Doing == do_runattack)
				m_ClientDoing = m_FightMode ? cdo_fightrun : cdo_run;
			else if (m_Doing == do_walk)
				m_ClientDoing = m_FightMode ? cdo_fightwalk : cdo_walk;
		}
		x = g_DirCos(m_Dir, 64) * MoveSpeed;
		y = g_DirSin(m_Dir, 64) * MoveSpeed;
	}
	else if (nRet == 2)
	{
		// [FIX-1 26/08] DA TOI DICH (khong phai bi chan): dung that - dat m_Doing =
		// do_stand va xoa bo dem 'toi dang sai'. Truoc day roi vao nhanh duoi va bi coi
		// la bi chan nen NPC ket trang thai chay + bo dem tang mai. Ban _SERVER (:4674)
		// von da lam dung dieu nay.
		DoStand();
		return;
	}
	else if (nRet == 0)
	{
		// Bi chan. Ban goc JX2 (0x00598080) KHONG dung nhan vat lai o day: no gio co
		// "toi dang sai" roi return, giu nguyen trang thai chay, tick sau thu tiep;
		// goi dong bo ke tiep se nan lai toa do. JX1 truoc day goi DoStand() => dong
		// nguoi la ai cung dung khung roi nhay - dung trieu chung dang gap.
		// RIENG nhan vat cua minh van phai dung that, neu khong se di xuyen nguoi roi
		// bi server keo lui lien tuc.
		if (m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			DoStand(); //Â®Ã¸ng lÂ¹i
			return;
		}
		// Dung THAT (het canh chan chay tai cho) nhung GIU DICH: tick sau tu thu
		// lai duong, thong la di tiep ngay - khong phai cho lenh moi nhu DoStand().
		// Van giu co de SyncNpcMin nan vi tri khi client/server lech nhau.
		m_nNeedFixPos++;
		DoStandBlocked();
		return;
	}
	else if (nRet == -1)
	{
		SubWorld[0].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		m_RegionIndex = -1;
		return;
	}
	else
	{
		return;
	}
#endif
//
#ifdef _SERVER
	if(nRet == 1)
	{
		x = g_DirCos(m_Dir, 64) * MoveSpeed;
		y = g_DirSin(m_Dir, 64) * MoveSpeed;
	}
	else
	{
		DoStand();
		return;
	}
#endif

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;

	m_OffX += x;
	m_OffY += y;
//	Â´Â¦Ã€Ã­NPCÂµÃ„Ã—Ã¸Â±ÃªÂ±Ã¤Â»Ãƒ
//	CELLWIDTHÂ¡Â¢CELLHEIGHTÂ¡Â¢OffXÂ¡Â¢OffYÂ¾Ã¹ÃŠÃ‡Â·Ã…Â´Ã³ÃÃ‹1024Â±Â¶

	if (!m_bClientOnly)
		CURREGION.DecRef(m_MapX, m_MapY, obj_npc);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}

	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}

	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (!m_bClientOnly && m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	}

	if (m_RegionIndex == -1)	// Â²Â»Â¿Ã‰Ã„ÃœÃ’Ã†Â¶Â¯ÂµÂ½-1 RegionÂ£Â¬ÃˆÃ§Â¹Ã»Â³Ã¶ÃÃ–Ã•Ã¢Ã–Ã–Ã‡Ã©Â¿Ã¶Â£Â¬Â»Ã–Â¸Â´Ã”Â­Ã—Ã¸Â±Ãª
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			if (m_nPlayerIdx > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove, FALSE); //mÂµi mÃŸn khi di chuyÃ“n
			}
		}
#else
		if (m_RegionIndex >= 0)
		{
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
		}
#endif
	}
}

void KNpc::ServeJump(int nSpeed)
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (!(m_Doing == do_jump || m_Doing == do_jumpattack))
		return;
	
	if (nSpeed <= 0)
		return;

	if (nSpeed >= SubWorld[m_SubWorldIndex].m_nCellWidth)
	{
		nSpeed = SubWorld[m_SubWorldIndex].m_nCellWidth - 1;
	}

	m_OffX += g_DirCos(m_JumpDir, 64) * nSpeed;
	m_OffY += g_DirSin(m_JumpDir, 64) * nSpeed;

	// s = vt - a * t * t / 2
	m_Height = (m_JumpFirstSpeed * m_Frames.nCurrentFrame - ACCELERATION_OF_GRAVITY * m_Frames.nCurrentFrame * m_Frames.nCurrentFrame / 2) / 8;
	if (m_Height < 0)
		m_Height = 0;

	int nOldRegion = m_RegionIndex;
	int nOldMapX = m_MapX;
	int nOldMapY = m_MapY;
	int nOldOffX = m_OffX;
	int nOldOffY = m_OffY;
	CURREGION.DecRef(m_MapX, m_MapY, obj_npc);
	
	if (m_OffX < 0)
	{
		m_MapX--;
		m_OffX += CELLWIDTH;
	}
	else if (m_OffX > CELLWIDTH)
	{
		m_MapX++;
		m_OffX -= CELLWIDTH;
	}
	
	if (m_OffY < 0)
	{
		m_MapY--;
		m_OffY += CELLHEIGHT;
	}
	else if (m_OffY > CELLHEIGHT)
	{
		m_MapY++;
		m_OffY -= CELLHEIGHT;
	}
	
	if (m_MapX < 0)
	{
		m_RegionIndex = LEFTREGIONIDX;
		m_MapX += REGIONWIDTH;
	}
	else if (m_MapX >= REGIONWIDTH)
	{
		m_RegionIndex = RIGHTREGIONIDX;
		m_MapX -= REGIONWIDTH;
	}
	
	if (m_RegionIndex >= 0)
	{
		if (m_MapY < 0)
		{
			m_RegionIndex = UPREGIONIDX;
			m_MapY += REGIONHEIGHT;
		}
		else if (m_MapY >= REGIONHEIGHT)
		{
			m_RegionIndex = DOWNREGIONIDX;
			m_MapY -= REGIONHEIGHT;
		}
		if (m_RegionIndex >= 0)
			CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
	}

	if (m_RegionIndex == -1)
	{
		m_RegionIndex = nOldRegion;
		m_MapX = nOldMapX;
		m_MapY = nOldMapY;
		m_OffX = nOldOffX;
		m_OffY = nOldOffY;
		CURREGION.AddRef(m_MapX, m_MapY, obj_npc);
		return;
	}

	if (nOldRegion != m_RegionIndex)
	{
#ifdef _SERVER
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, m_RegionIndex, m_Index);
		if (IsPlayer())
		{
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, m_RegionIndex, m_nPlayerIdx);
			if (m_nPlayerIdx > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove, FALSE); //#mÂµi mÃŸn khi nhÂ¶y
			}
		}
#else
		if (m_RegionIndex >= 0)
		{
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
			m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
		}
#endif
	}		
}

void KNpc::SendCommand(NPCCMD cmd,int x,int y, int z)
{
	if (m_FrozenAction.nTime > 0)
	{
		AUTOLOG_EVERY(1000, "[E4_CMD_FROZEN] npc=%d id=%u cmd=%d p=(%d,%d,%d) doing=%d frozen=%d", m_Index, m_dwID, (int)cmd, x, y, z, (int)m_Doing, m_FrozenAction.nTime);
		if (cmd == do_walk || 
			cmd == do_run || 
			cmd == do_runattack || 
			cmd == do_jump || 
			cmd == do_jumpattack || 
			cmd == do_skill || 
			cmd == do_magic || 
			cmd == do_attack || 
			cmd == do_blurmove)
		{
			return;
		}
	}

	if (m_RandMove.nTime > 0)
	{
		AUTOLOG_EVERY(1000, "[E4_CMD_RANDMOVE] npc=%d id=%u cmd=%d p=(%d,%d,%d) doing=%d randmove=%d", m_Index, m_dwID, (int)cmd, x, y, z, (int)m_Doing, m_RandMove.nTime);
		if (cmd == do_run || 
			cmd == do_runattack || 
			cmd == do_jump || 
			cmd == do_jumpattack || 
			cmd == do_skill || 
			cmd == do_magic || 
			cmd == do_attack || 
			cmd == do_blurmove)
		{
			return;
		}
	}

	AUTOLOG_EVERY(2000, "[E4_CMD_STAMINA] npc=%d id=%u cmd=%d stam=%d pkflag=%d", m_Index, m_dwID, (int)cmd, m_CurrentStamina, m_nPKFlag);
	if(cmd == do_run)
	{
		if ((m_CurrentStamina < 18 && m_nPKFlag == enumPKMurder) || (m_CurrentStamina < 8 && m_nPKFlag == enumPKTongWar)) //#chua hoan thien
			cmd = do_walk;
	}
#ifdef _SERVER
	if (cmd == do_revive)
	{
	    m_ProcessAI = 1;
	    m_ProcessState = 1;
		DoStand();
		return;
	}
#endif
	AUTOLOG_EVERY(2000, "[E4_CMD_ACCEPT] npc=%d id=%u cmd=%d p=(%d,%d,%d) doing=%d skill=%d map=(%d,%d) rgn=%d", m_Index, m_dwID, (int)cmd, x, y, z, (int)m_Doing, m_ActiveSkillID, m_MapX, m_MapY, m_RegionIndex);
	m_Command.CmdKind = cmd;
	m_Command.Param_X = x;
	m_Command.Param_Y = y;
	m_Command.Param_Z = z;
}

NPC_COMMAND KNpc::GetCommand()
{
	return m_Command;
}

BOOL KNpc::NewPath(int nMpsX, int nMpsY)
{

	m_DesX = nMpsX;
	m_DesY = nMpsY;
	return TRUE;
}

BOOL KNpc::NewJump(int nMpsX, int nMpsY)
{
	_ASSERT(m_CurrentJumpSpeed > 0);
	if (m_CurrentJumpSpeed <= 0)
		return FALSE;
	
	int nX, nY;
	GetMpsPos(&nX, &nY);

	if (nX == nMpsX && nY == nMpsY)
		return FALSE;

	int nRangeCheckL = (nX - nMpsX) * (nX - nMpsX) + (nY - nMpsY) * (nY - nMpsY);
	AUTOLOG_EVERY(500, "[E4_JUMP_RANGE] npc=%d id=%u cur=(%d,%d) des=(%d,%d) dist2=%d jspeed=%d jframe=%d", m_Index, m_dwID, nX, nY, nMpsX, nMpsY, nRangeCheckL, m_CurrentJumpSpeed, m_CurrentJumpFrame);
	if (nRangeCheckL <= 32 * 32)
		return FALSE;

	int nDir = g_GetDirIndex(nX, nY, nMpsX, nMpsY);
	int	nMaxLength = m_CurrentJumpSpeed * m_CurrentJumpFrame;
	int	nWantLength = g_GetDistance(nX, nY, nMpsX, nMpsY);
	int	nSin = g_DirSin(nDir, 64);
	int	nCos = g_DirCos(nDir, 64);

	if (nWantLength > nMaxLength)
	{
		m_DesX = nX + ((nMaxLength * nCos) >> 10);
		m_DesY = nY + ((nMaxLength * nSin) >> 10);
		nWantLength = nMaxLength;
	}
	else if (nWantLength <= MIN_JUMP_RANGE)
	{
		m_DesX = nMpsX;
		m_DesY = nMpsY;
		return FALSE;
	}

	m_JumpStep = nWantLength / m_CurrentJumpSpeed;

	int nTestX = 0;
	int nTestY = 0;
	int nSuccessStep = 0;

	for (int i = 1; i < m_JumpStep + 1; i++)
	{
		nTestX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
		nTestY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
		int nBarrier = SubWorld[m_SubWorldIndex].GetBarrier(nTestX, nTestY);
		if (Obstacle_NULL == nBarrier)
		{
			nSuccessStep = i;
		}
		if (Obstacle_Normal == nBarrier || Obstacle_Fly == nBarrier)
		{
			if (nSuccessStep <= MIN_JUMP_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * nSuccessStep) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * nSuccessStep) >> 10);
			m_JumpStep = nSuccessStep;
			break;
		}
		int	nTrap = SubWorld[m_SubWorldIndex].GetTrap(nTestX, nTestY);
		if (nTrap)
		{
			if (i <= MIN_JUMP_RANGE / m_CurrentJumpSpeed)
			{
				return FALSE;
			}
			m_DesX = nX + ((m_CurrentJumpSpeed * nCos * i) >> 10);
			m_DesY = nY + ((m_CurrentJumpSpeed * nSin * i) >> 10);
			m_JumpStep = i;
			break;
		}
	}
	m_JumpDir = nDir;
	return TRUE;
}

VOID KNpc::FixPos()
{
	// ËÄÉáÎåÈëÂÔÈ¥ºóÊ®Î»
	m_OffX = (m_OffX + 512) & (~1023);
	m_OffY = (m_OffY + 512) & (~1023);
}

BOOL KNpc::Cost(NPCATTRIB nType, int nCost, BOOL bOnlyCheckCanCast)
{
	if (!IsPlayer())
		return TRUE;

	int *pSource = NULL;
	int nCurCost = nCost;
	switch(nType)
	{
	case attrib_mana_v:
		pSource = &m_CurrentMana;
		break;
	case attrib_mana_p:
		pSource = &m_CurrentMana;
		nCurCost = m_CurrentManaMax * nCost / MAX_PERCENT;
		break;
	case attrib_life_v:
		pSource = &m_CurrentLife;
		break;
	case attrib_life_p:
		pSource = &m_CurrentLife;
		nCurCost = m_CurrentLifeMax * nCost / MAX_PERCENT;
		break;
	case attrib_stamina_v:
		pSource = &m_CurrentStamina;
		break;
	case attrib_stamina_p:
		pSource = &m_CurrentStamina;
		nCurCost = m_CurrentStaminaMax * nCost / MAX_PERCENT;
		break;
	default:
		break;
	}

#ifdef _SERVER
	// [WLLS 21/08] ForbitStamina (trong tran lien dau): khong kiem / khong tru
	// THE LUC khi ra chieu - dong bo voi nhanh tru-18 khi chay do PK da gate o
	// phia tren (KNpc.cpp ~2200). pSource = NULL -> bo qua ca kiem lan tru.
	if ((nType == attrib_stamina_v || nType == attrib_stamina_p) &&
		m_Kind == kind_player && m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER &&
		Player[m_nPlayerIdx].m_bWllsForbidStamina)
		pSource = NULL;
#endif
	if (pSource)
	{
		if ((nType== attrib_life_v || nType== attrib_life_p)? ((*pSource-1) < nCurCost):(*pSource < nCurCost))
		{
#ifndef _SERVER
			KSystemMessage Msg;

			Msg.byConfirmType = SMCT_NONE;
			Msg.byParamSize = 0;
			Msg.byPriority = 1;
			Msg.eType = SMT_NORMAL;
			switch(nType)
			{
				case attrib_mana_v:
				case attrib_mana_p:
					g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_MANA, sizeof(Msg.szMessage));
					break;
				case attrib_life_v:
				case attrib_life_p:
					g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_LIFE, sizeof(Msg.szMessage));
					break;
				case attrib_stamina_v:
				case attrib_stamina_p:
					g_StrCpyLen(Msg.szMessage, MSG_NPC_NO_STAMINA, sizeof(Msg.szMessage));
					break;
				default:
					break;
			}
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
			return FALSE;
		}
		else
		{
#ifndef _SERVER	//client khong can` tru`
			return TRUE;
#endif
			if (!bOnlyCheckCanCast)
				*pSource -= nCurCost;
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef _SERVER
void KNpc::Cast(int nSkillId, int nSkillLevel)
{
	if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)
	{
		int nMpsX, nMpsY;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);		
		_ASSERT(nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL);

		NPC_SKILL_SYNC SkillCmd;
		SkillCmd.ID = this->m_dwID;
		SkillCmd.nSkillID = nSkillId;	
		SkillCmd.nSkillLevel = nSkillLevel;
		SkillCmd.nMpsX = -1;
		SkillCmd.nMpsY = m_dwID;
		SkillCmd.ProtocolType = s2c_castskilldirectly;
		
		static const POINT	POff[8] = 
		{
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};
		
		int nMaxCount = MAX_BROADCAST_COUNT;
		CURREGION.BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX, m_MapY);
		int i;
		for (i = 0; i < 8; i++)
		{
			if (CONREGIONIDX(i) == -1)
				continue;
			CONREGION(i).BroadCast(&SkillCmd, sizeof(SkillCmd), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
		}
			
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pOrdinSkill->Cast(m_Index, nMpsX, nMpsY);

		if(!pOrdinSkill->IsAura())
		{
			DWORD dwCastTime = 0;
			eSkillStyle eStyle = (eSkillStyle)pOrdinSkill->GetSkillStyle();
			if (eStyle == SKILL_SS_Missles 
				|| eStyle == SKILL_SS_Melee 
				|| eStyle == SKILL_SS_InitiativeNpcState 
				|| eStyle == SKILL_SS_PassivityNpcState)
			{
				dwCastTime = pOrdinSkill->GetDelayPerCast(m_bRideHorse);
			}
			else
			{
				switch(eStyle)
				{
				case SKILL_SS_Thief:
					{
						dwCastTime = ((KThiefSkill*)pOrdinSkill)->GetDelayPerCast();
					}break;
				}
			}
			m_SkillList.SetNextCastTime(nSkillId, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);
		}
	}
}
#endif

void KNpc::DoJump()
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	if (m_Doing == do_jump)
		return;
	
	m_Doing = do_jump;
	m_Dir = m_JumpDir;
	m_ProcessAI	= 0;
	m_JumpFirstSpeed = ACCELERATION_OF_GRAVITY * (m_JumpStep - 1) / 2 ;
#ifdef _SERVER
	NPC_JUMP_SYNC	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_npcjump;
	NetCommand.ID = m_dwID;
	NetCommand.nMpsX = m_DesX;
	NetCommand.nMpsY = m_DesY;
		
	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX, m_MapY);
	for (int i= 0; i < 8; i++)
	{
		if (CONREGIONIDX(i) == -1)
			continue;
		CONREGION(i).BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
#endif

#ifndef _SERVER
		m_ClientDoing = cdo_jump;
#endif

	m_Frames.nTotalFrame = m_JumpStep;
	m_Frames.nCurrentFrame = 0;	
}

BOOL KNpc::OnJump()
{
//
#ifdef _SERVER
	if(IsPlayer() && m_nTimeIdleCounter > 0)
	{
		m_nTimeIdleCounter = 0;
	}
#endif
//
	CheckTrap();
	ServeJump(m_CurrentJumpSpeed);
	if (WaitForFrame())
	{
		FixPos();
		DoStand();
		m_ProcessAI	= 1;
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::WaitForFrame()
{
	m_Frames.nCurrentFrame++;
	if (m_Frames.nCurrentFrame < m_Frames.nTotalFrame)
	{
		return FALSE;
	}
	m_Frames.nCurrentFrame = 0;
	return TRUE;
}

BOOL KNpc::IsReachFrame(int nPercent)
{
	if (m_Frames.nCurrentFrame == m_Frames.nTotalFrame * nPercent / 100)
	{
		return TRUE;
	}
	return FALSE;
}

void KNpc::Load(int nNpcSettingIdx, int nLevel, int nSeries)
{
	m_PathFinder.Init(m_Index);
	if (nLevel <= 0) 
	{
		nLevel = 1;
	}

#ifndef _SERVER
	char	szNpcTypeName[32];
#endif
	if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID || nNpcSettingIdx == PLAYER_FEMALE_NPCTEMPLATEID)
	{
		m_NpcSettingIdx = nNpcSettingIdx;
		m_Level = nLevel;
#ifndef _SERVER
		if (nNpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID)
		{
			strcpy(szNpcTypeName, "MainMan");
			//strcpy(szNpcTypeName, "ÄÐÖ÷½Ç"); //kiÓu npc kieu npc bao gåm nam vµ n÷ man and lady
			m_StandFrame = NpcSet.GetPlayerStandFrame(TRUE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(TRUE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(TRUE);
		}
		else
		{
			strcpy(szNpcTypeName, "MainLady");
			//strcpy(szNpcTypeName, "Å®Ö÷½Ç"); kiÓu npc kieu npc bao gåm nam vµ n÷ man and lady
			m_StandFrame = NpcSet.GetPlayerStandFrame(FALSE);
			m_WalkFrame = NpcSet.GetPlayerWalkFrame(FALSE);
			m_RunFrame = NpcSet.GetPlayerRunFrame(FALSE);
		}
#endif
		//		TODO: Load Player Data;
		m_WalkSpeed = NpcSet.GetPlayerWalkSpeed();
		m_RunSpeed = NpcSet.GetPlayerRunSpeed();
		m_AttackFrame = NpcSet.GetPlayerAttackFrame();
		m_HurtFrame	= NpcSet.GetPlayerHurtFrame();
	}
	else
	{
		GetNpcCopyFromTemplate(nNpcSettingIdx, nLevel, nSeries);

#ifndef _SERVER	
		g_NpcSetting.GetString(nNpcSettingIdx + 2, "NpcResType", "", szNpcTypeName, sizeof(szNpcTypeName));
		if (!szNpcTypeName[0])//khong tim thay npc gan mac dinh ten npc dau tien
		{
			g_NpcKindFile.GetString(2, "ÈËÎïÃû³Æ", "", szNpcTypeName, sizeof(szNpcTypeName));
		}
		//g_NpcSetting.GetString(nNpcSettingIdx + 2, "DescName", "", DescName, sizeof(DescName));
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIMode", 12, &m_AiMode);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam1", 12, &m_AiParam[0]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam2", 12, &m_AiParam[1]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam3", 12, &m_AiParam[2]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam4", 12, &m_AiParam[3]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam5", 12, &m_AiParam[4]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam6", 12, &m_AiParam[5]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "AIParam7", 12, &m_AiParam[6]);
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ActiveRadius", 12, &m_ActiveRadius);		//BÂ¸n kÃnh hoÂ¹t Â®Ã©ng
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ClientOnly", 0, &m_bClientOnly);
		if (m_AiMode == 11 || m_AiMode == 12 || m_AiMode == 17)
			m_AiParam[6] = m_AiMode;
#endif
	}
#ifndef _SERVER
	m_DataRes.Init(szNpcTypeName, &g_NpcResList);
	m_DataRes.SetAction(m_ClientDoing);
	m_DataRes.SetRideHorse(m_bRideHorse);
	m_DataRes.SetArmor(m_ArmorType, m_MantleType);
	m_DataRes.SetHelm(m_HelmType);
	m_DataRes.SetHorse(m_HorseType);
	m_DataRes.SetWeapon(m_WeaponType);
#endif

#ifdef _SERVER
	if(!IsPlayer() && m_SkillList.m_Skills[defNPC_GOLD_SKILL_NO].SkillId)
	{
		SetAuraSkill(m_SkillList.m_Skills[defNPC_GOLD_SKILL_NO].SkillId);
	}
#endif

	m_CurrentCamp = m_Camp;
}

void KNpc::GetMpsPos(int *pPosX, int *pPosY)
{
#ifdef _SERVER
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
#else
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
//	KSubWorld::Map2Mps(C_REGION_X(m_RegionIndex), C_REGION_Y(m_RegionIndex), m_MapX, m_MapY, m_OffX, m_OffY, pPosX, pPosY);
#endif
}

#ifndef _SERVER
// Drawn (interpolated) position - use ONLY in Paint* overlay code.
// Identical to GetMpsPos when interpolation is off, because the logic tick
// refreshes KNpcRes with the plain tick position every tick.
void KNpc::GetDrawPos(int *pPosX, int *pPosY)
{
	m_DataRes.GetPos(pPosX, pPosY);
}
#endif

#ifndef _SERVER
#define VOID_DIS 0x7FFFFFFF
INT	KNpc::GetMapDisX(INT nIdx1, INT nIdx2)
{
	if (Npc[nIdx1].m_RegionIndex < 0 || Npc[nIdx2].m_RegionIndex < 0)
		return VOID_DIS;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return VOID_DIS;
	INT nSubWorldIdx = Npc[nIdx1].m_SubWorldIndex;
	INT region1 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx1].m_RegionIndex].m_RegionID,
		region2 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx2].m_RegionIndex].m_RegionID;
	return abs((LOWORD(region1) - LOWORD(region2)) * 
		SubWorld[nSubWorldIdx].m_nRegionWidth + Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX);
}

INT	KNpc::GetMapDisY(INT nIdx1, INT nIdx2)
{
	if (Npc[nIdx1].m_RegionIndex == -1 || Npc[nIdx2].m_RegionIndex == -1)
		return VOID_DIS;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return VOID_DIS;
	INT nSubWorldIdx = Npc[nIdx1].m_SubWorldIndex;
	INT region1 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx1].m_RegionIndex].m_RegionID,
		region2 = SubWorld[nSubWorldIdx].m_Region[Npc[nIdx2].m_RegionIndex].m_RegionID;
	return abs((HIWORD(region1) - HIWORD(region2)) * 
		SubWorld[nSubWorldIdx].m_nRegionHeight + Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY);
}


VOID KNpc::ClientGotoPos(INT nX, INT nY, INT nMode /* = 0 */)
{


	INT nOrgX, nOrgY;
	GetMpsPos(&nOrgX, &nOrgY);

	//KG_PROCESS_ERROR(!Player[CLIENT_PLAYER_INDEX].IsLocked());

	//KG_PROCESS_ERROR(!HaveState(emSTATE_CONFUSE));

	if ((nMode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) || nMode == 2)
	{
		if (m_Doing == do_run)
		{
			m_DesX = nX;
			m_DesY = nY;
			SendClientCmdRun(nX, nY);
		}
		else
		{
			SendClientCmdRun(nX, nY);
		}
		SendCommand(do_run, nX, nY);
	}
	else
	{
		if (m_Doing == do_walk)
		{
			m_DesX = nX;
			m_DesY = nY;
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			SendClientCmdWalk(nX, nY);
		}
		SendCommand(do_walk, nX, nY);
	}

//Exit0:
	return;
}
#endif

BOOL	KNpc::SetActiveSkill(int nSkillIdx)
{
	if (nSkillIdx <= 0 || nSkillIdx >= MAX_NPCSKILL)
		return FALSE;

	if (m_SkillList.m_Skills[nSkillIdx].SkillId <= 0 || m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel <= 0)
		return FALSE;

	m_ActiveSkillID = m_SkillList.m_Skills[nSkillIdx].SkillId;
	//int nCurLevel = m_SkillList.m_Skills[nSkillIdx].CurrentSkillLevel;//#can kiem tra
	int nCurLevel = m_SkillList.m_Skills[nSkillIdx].SkillLevel + m_SkillList.m_Skills[nSkillIdx].AddLevel;
	_ASSERT(m_ActiveSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL && nCurLevel > 0);
	
	ISkill * pISkill =  g_SkillManager.GetSkill(m_ActiveSkillID, nCurLevel);
	if (pISkill)
    {
		m_CurrentAttackRadius = pISkill->GetAttackRadius();
    }
	return TRUE;
}

void KNpc::SetAuraSkill(int nSkillID)
{
	int nCurLevel = 0;
	int nAddLevel = 0;
	if (nSkillID <= 0 || nSkillID >= MAX_SKILL) 
    {
        nSkillID = 0;
    }
	else
	{
		nCurLevel = m_SkillList.GetCurrentLevel(nSkillID);
		if (nCurLevel <= 0) 
        {
            nSkillID = 0;
        }
		else
		{
			_ASSERT(nSkillID < MAX_SKILL && nCurLevel < MAX_SKILLLEVEL);
			KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nCurLevel + nAddLevel);
            if (!pOrdinSkill || !pOrdinSkill->IsAura())
			{
				nSkillID  = 0;
			}
		}
	}
	m_ActiveAuraID = nSkillID;

#ifndef _SERVER
	SKILL_CHANGEAURASKILL_COMMAND ChangeAuraMsg;
	ChangeAuraMsg.ProtocolType = c2s_changeauraskill;
	ChangeAuraMsg.m_nAuraSkill = m_ActiveAuraID;
	if (g_pClient)
		g_pClient->SendPackToServer(&ChangeAuraMsg, sizeof(SKILL_CHANGEAURASKILL_COMMAND));
#else
	UpdateNpcStateInfo();
#endif

}

BOOL KNpc::SetPlayerIdx(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return FALSE;

	if (m_Kind != kind_player)
		return FALSE;

	m_nPlayerIdx = nIdx;
	return TRUE;
}

void KNpc::SwitchMaskFeature()
{
	m_bMaskFeature = !m_bMaskFeature;
#ifdef _SERVER
	int nIdx = Player[m_nPlayerIdx].m_ItemList.GetEquipment(itempart_mask);
	if (m_bMaskFeature)
	{
		if (nIdx && !Player[m_nPlayerIdx].m_ItemList.GetMaskLock())
			m_MaskType = 0;
	}
	else
	{
		if (nIdx && !Player[m_nPlayerIdx].m_ItemList.GetMaskLock())
		{
			switch (Item[nIdx].m_CommonAttrib.nItemNature)
			{
			case NATURE_GOLD:
				m_MaskType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
				break;
			case NATURE_PLATINA:
				break;
			default:
				g_MaskChangeRes.GetInteger(Item[nIdx].GetParticular() + 2, 2, 0, &m_MaskType);
				break;
			}
		}
	}
#endif

#ifdef _SERVER
	S2C_PLAYER_SYNC	sMsg;
	sMsg.ProtocolType = s2c_playersync;
	sMsg.m_wLength = sizeof(S2C_PLAYER_SYNC) - 1;
	sMsg.m_lpBuf = 0;
	sMsg.m_wMsgID = enumS2C_PLAYERSYNC_ID_MASKFEATURE;
	g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
#endif
}


#ifdef _SERVER
BOOL KNpc::SendSyncData(int nClient)	//Sync npc vµ player to Server v? Client 1 lÇn
{
	BOOL	bRet = FALSE;
	NPC_SYNC	NpcSync;
	NpcSync.ProtocolType		= (BYTE)s2c_syncnpc;
	NpcSync.Camp				= (BYTE)m_Camp;
	NpcSync.CurrentCamp			= (BYTE)m_CurrentCamp;
	NpcSync.m_bySeries			= (BYTE)m_Series;
	NpcSync.m_byType			= (BYTE)m_Type;
	if (m_CurrentLifeMax > 0)
		NpcSync.LifePerCent		= (BYTE)((m_CurrentLife << 7) / m_CurrentLifeMax);
	else
		NpcSync.LifePerCent		= 0;
	if (this->IsPlayer())
		NpcSync.m_btMenuState	= (BYTE)Player[this->m_nPlayerIdx].m_cMenuState.m_nState;
	else
		NpcSync.m_btMenuState	= 0;
	NpcSync.m_Doing				= (BYTE)m_Doing;
	NpcSync.m_btKind			= (BYTE)m_Kind;
	GetMpsPos((int *)&NpcSync.MapX, (int *)&NpcSync.MapY);
	NpcSync.ID					= m_dwID;
	NpcSync.NpcSettingIdx		= MAKELONG(m_Level, m_NpcSettingIdx);
	NpcSync.NpcEnchant			= (WORD)this->m_cGold.GetGoldType();
	// [3HD 25/08] boss san boss sat thu / tin su (HD3_AddNpc flag 1): server
	// danh dau m_Type = boss_gold; client to mau ten theo NpcEnchant (client
	// dat m_Type = NpcEnchant o ca 2 duong sync) nen day qua truong nay.
	// He quai vang (GetGoldType != 0) giu uu tien - khong doi hanh vi cu.
	if (NpcSync.NpcEnchant == 0 && m_Type == boss_gold)
		NpcSync.NpcEnchant		= (WORD)boss_gold;
	NpcSync.m_CurrentLife		= m_CurrentLife;
	NpcSync.m_CurrentLifeMax	= m_CurrentLifeMax;
	NpcSync.m_LifeMax			= m_LifeMax;
	NpcSync.m_CurrentLifeReplenish = m_CurrentLifeReplenish;
	NpcSync.m_LifeReplenish		= m_LifeReplenish;
	NpcSync.m_CurrentAttackRating = m_CurrentAttackRating;
	NpcSync.m_AttackRating		= m_AttackRating;
	NpcSync.m_CurrentDefend		= m_CurrentDefend;
	NpcSync.m_Defend			= m_Defend;
	NpcSync.m_CurrentExperience = m_CurrentExperience;
	NpcSync.m_Experience		= m_Experience;
	NpcSync.m_CurrentWalkSpeed  = m_CurrentWalkSpeed;
	NpcSync.m_WalkSpeed			= m_WalkSpeed;
	NpcSync.m_CurrentRunSpeed	= m_CurrentRunSpeed;
	NpcSync.m_RunSpeed			= m_RunSpeed;
	NpcSync.m_CurrentHitRecover = m_CurrentHitRecover;		//thêi gian phôc håi
	NpcSync.m_HitRecover		= m_HitRecover;		//thêi gian phôc håi
	NpcSync.MissionGroup		= (int)m_nMissionGroup;//#NpcMissionGroup
	strcpy(NpcSync.m_szName, Name);
	NpcSync.m_wLength			= sizeof(NPC_SYNC) - 1 - sizeof(NpcSync.m_szName) + strlen(NpcSync.m_szName);
	if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&NpcSync, NpcSync.m_wLength + 1)))
	{
//		printf("Packing sync data ok...\n");//edit by phong kieu paking sync data ok
		bRet = TRUE;
	}
	else
	{
		printf("Packing sync data failed...\n");
		return FALSE;
	}
	g_DebugLog("[Sync]%d:%s<%d> request to %d. size:%d", SubWorld[m_SubWorldIndex].m_dwCurrentTime, Name, m_Kind, nClient, NpcSync.m_wLength + 1);

	if (IsPlayer())
	{
		PLAYER_SYNC	PlayerSync;
		PlayerSync.ProtocolType		= (BYTE)s2c_syncplayer;
		PlayerSync.ID				= m_dwID;
		PlayerSync.ArmorType		= (BYTE)m_ArmorType;
		PlayerSync.AttackSpeed		= (BYTE)m_CurrentAttackSpeed;
		PlayerSync.CastSpeed		= (BYTE)m_CurrentCastSpeed;
		PlayerSync.HelmType			= (BYTE)m_HelmType;
		PlayerSync.MaskType			= m_MaskType;			//#mat na
		PlayerSync.MantleType		= (BYTE)m_MantleType; 
		if(m_bRideHorse)				//edit by phong kieu len xuong ngua
		{
			PlayerSync.HorseType    = (BYTE)m_HorseType;
		}
		else
		{
			PlayerSync.HorseType    = -1;
		}
		PlayerSync.m_bBaiTan = m_BaiTan;
		PlayerSync.RunSpeed			= (BYTE)m_CurrentRunSpeed;
		PlayerSync.WalkSpeed		= (BYTE)m_CurrentWalkSpeed;
		PlayerSync.WeaponType		= (BYTE)m_WeaponType;
		PlayerSync.RankID			= (BYTE)m_btRankId;
		PlayerSync.RankBattleID			= (DWORD)m_btRankBattleId;//#RankBattle
		PlayerSync.PlayerTitle			= (DWORD)m_btPlayerTitle;//#PlayerTitle
		strcpy(PlayerSync.MateName, Player[m_nPlayerIdx].m_PlayerMateName); //#MateName
		PlayerSync.CUnlocked		= (BYTE)Player[m_nPlayerIdx].m_CUnlocked;//sync trang thai dong mo khoa ruong
		PlayerSync.ExItemID			= (BYTE)m_ExItemId; // hanh trang
		PlayerSync.ExBoxID			= (BYTE)m_ExBoxId; // ruong mo rong
		PlayerSync.RankInWorld		= (int)Player[m_nPlayerIdx].m_nWorldStat; //xÕp h¹ng thÕ giíi
		PlayerSync.Repute			= (int)Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
		PlayerSync.FuYuan			= (int)Player[m_nPlayerIdx].m_cFuYuan.GetFuYuanValue();
		PlayerSync.PKValue			= (BYTE)Player[m_nPlayerIdx].m_cPK.GetPKValue();
		PlayerSync.MantleLevel		= (BYTE)m_byMantleLevel;
		PlayerSync.ReBorn			= (BYTE)Player[m_nPlayerIdx].m_cReBorn.GetReBornValue();
		PlayerSync.nFirstFaction			= (BYTE)Player[m_nPlayerIdx].GetFirstAddFaction();
		PlayerSync.m_btSomeFlag		= 0;
		if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState())
			PlayerSync.m_btSomeFlag |= 0x01;
		if (Npc[Player[m_nPlayerIdx].m_nIndex].m_FightMode)
			PlayerSync.m_btSomeFlag |= 0x02;
		if (Player[m_nPlayerIdx].m_bSleepMode)
			PlayerSync.m_btSomeFlag |= 0x04;
		if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKWar)
			PlayerSync.m_btSomeFlag |= 0x10;
		Player[m_nPlayerIdx].m_cTong.GetTongName(PlayerSync.TongName);
		Player[m_nPlayerIdx].m_cTong.GetTongTitle(PlayerSync.TongTitle);
		PlayerSync.TongFigure = Player[m_nPlayerIdx].m_cTong.GetFigure();
		PlayerSync.TongRecruit = Player[m_nPlayerIdx].m_cTong.GetRecruit();
		PlayerSync.ImagePlayer		= (BYTE)Player[m_nPlayerIdx].m_ImagePlayer;
		UpdateGameTitle(); // ensure m_szGameTitle is updated
		strcpy(PlayerSync.GameTitle, m_szGameTitle);
		memcpy(PlayerSync.bMeridianLevel, Player[m_nPlayerIdx].m_cMeridian.getMeridian(), sizeof(PlayerSync.bMeridianLevel));

		// ---- Port SimCity: nguon du lieu cho BOT ----
		// Bot la KNpc kind_player nhung KHONG co KPlayer: m_nPlayerIdx = 0 (KNpc.cpp:139)
		// nen moi dong Player[m_nPlayerIdx] o tren doc tu Player[0] (o dat truoc, luon rong).
		// Hau qua neu khong ghi de: MOI bot deu hien "khong bang hoi, PK 0, danh vong 0,
		// trung sinh 0, khong phoi ngau, hang the gioi 0" - lo ngay la bot.
		// Cac truong duoi day deu la truong CO SAN cua KNpc, khong them truong moi,
		// khong doi kich thuoc goi tin (PLAYER_SYNC = 222 byte).
		if (m_btSimCityBot)
		{
			// strncpy chu KHONG strcpy: xem SC_AddBot - cac truong nay tung la rac heap
			// vi ZeroMemory cua chung nam trong khoi #ifndef _SERVER (KNpc.cpp:164-176)
			// va ZeroMemory(MateName) o KNpc.cpp:232 bi chu thich.
			strncpy(PlayerSync.MateName, MateName, sizeof(PlayerSync.MateName) - 1);
			PlayerSync.MateName[sizeof(PlayerSync.MateName) - 1] = 0;
			strncpy(PlayerSync.TongName, m_szTongName, sizeof(PlayerSync.TongName) - 1);
			PlayerSync.TongName[sizeof(PlayerSync.TongName) - 1] = 0;
			strncpy(PlayerSync.TongTitle, m_szTongTitle, sizeof(PlayerSync.TongTitle) - 1);
			PlayerSync.TongTitle[sizeof(PlayerSync.TongTitle) - 1] = 0;
			PlayerSync.TongFigure   = (BYTE)m_nFigure;
			PlayerSync.TongRecruit  = m_Recruit;
			PlayerSync.CUnlocked    = 0;
			PlayerSync.RankInWorld  = (int)nRankInWorld;
			PlayerSync.Repute       = (int)nRepute;
			PlayerSync.FuYuan       = (int)nFuYuan;
			PlayerSync.PKValue      = (BYTE)nPKValue;
			PlayerSync.ReBorn       = (BYTE)nReBorn;
			PlayerSync.nFirstFaction = (BYTE)nFirstFaction;
			PlayerSync.ImagePlayer  = (BYTE)m_ImagePlayer;
			memset(PlayerSync.bMeridianLevel, 0, sizeof(PlayerSync.bMeridianLevel));
			// co trang thai: bot luon hoa binh (0x01), khong ngu (0x04), khong chien (0x10).
			PlayerSync.m_btSomeFlag = 0x01;
			if (m_FightMode)
				PlayerSync.m_btSomeFlag |= 0x02;
			if (m_szTongName[0])
				PlayerSync.m_btSomeFlag |= 0x08;
			// 0x20 = CO NHAN DIEN BOT cho client. Bit nay dang trong: client hien tai chi
			// test 0x01/0x02/0x04/0x10 (KProtocolProcess.cpp:2282-2297) nen bit la bi bo qua
			// => client cu tuong thich nguoc 100%, kich thuoc goi khong doi.
			// Client moi dung bit nay de loc bot khoi GetAroundPlayer / moi to doi (WAuto).
			PlayerSync.m_btSomeFlag |= 0x20;
		}
		if (SUCCEEDED(g_pServer->PackDataToClient(nClient, (BYTE*)&PlayerSync, sizeof(PLAYER_SYNC))))
		{
			//printf("Packing player sync data ok...\n");//edit by phong kieu sync data ok
			bRet = TRUE;
		}
		else
		{
			printf("Packing player sync data failed...\n");
			return FALSE;
		}
	}
	// [S10-M1 26/08] TRAM LO HONG GOC (co tu ban 2003, comment 'need check later -- spe
	// 03/05/27' o KProtocolProcess.cpp:1987): goi NPC_SYNC (ADD) khong mang dich den, nen
	// client thay NPC dang chay giua chang la tu che lenh 'chay toi cho dang dung' - ban
	// sao mu dich cho toi chang ke tiep (do that: 10% khoang cho > 2,2s; ban sao tu quay
	// dau gap 6,1 lan server). Gui kem MOT goi lenh CO SAN (s2c_npcrun/s2c_npcwalk, 13
	// byte, client co handler tu truoc - khong doi protocol) mang DICH THAT m_DesX/m_DesY
	// cho rieng client nay. Cung hang doi FIFO per-client (ServerStage.cpp:396+) nen luon
	// toi SAU goi ADD va de len lenh tu che (khe lenh mot cho - ke ghi sau thang).
	// GAC SONG COT m_DesX>0: DoSkill muon m_DesX=-1/m_DesY=CHI-SO-KHE lam tham so ma khong
	// doi m_Doing (KNpc.cpp:2592+2649) - thieu gac la ban sao chay ve goc map.
	// Khong gui cho chinh chu nhan vat (client cung se vut qua ConformIdx) do sach log.
	// Ket qua goi phu KHONG duoc dong vao bRet: chuoi login theo buoc (KPlayerDBFuns.cpp:46+)
	// doc gia tri tra ve cua ham nay de quyet dinh buoc nap nhan vat.
	if (bRet && (m_Doing == do_run || m_Doing == do_walk) && m_DesX > 0 && m_DesY > 0 &&
		!(IsPlayer() && Player[m_nPlayerIdx].m_nNetConnectIdx == nClient))
	{
		NPC_RUN_SYNC RunCmd;
		RunCmd.ProtocolType = (BYTE)((m_Doing == do_walk) ? s2c_npcwalk : s2c_npcrun);
		RunCmd.ID = m_dwID;
		RunCmd.nMpsX = m_DesX;
		RunCmd.nMpsY = m_DesY;
		g_pServer->PackDataToClient(nClient, (BYTE*)&RunCmd, sizeof(RunCmd));
		g_DebugLog("[S10-M1]%d:%s goi dich that (%d,%d) doing=%d cho client %d", SubWorld[m_SubWorldIndex].m_dwCurrentTime, Name, m_DesX, m_DesY, (int)m_Doing, nClient);
	}
	return bRet;
}

void KNpc::NormalSync() //Sync npc min liªn tôc tõ server vÒ client
{
	if (m_Doing == do_revive || m_Doing == do_death || !m_Index || m_RegionIndex < 0)
		return;

	NPC_NORMAL_SYNC NpcSync;
	int	nMpsX, nMpsY;

	GetMpsPos(&nMpsX, &nMpsY);

	NpcSync.ProtocolType = (BYTE)s2c_syncnpcmin;
	NpcSync.ID = m_dwID;
	NpcSync.MapX = nMpsX;
	NpcSync.MapY = nMpsY;
	NpcSync.m_fkRegionID = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_RegionID;
	NpcSync.m_fkOffX = m_OffX;
	NpcSync.m_fkOffY = m_OffY;
	NpcSync.Camp = (BYTE)m_CurrentCamp;
	NpcSync.Doing = (BYTE)m_Doing;
	NpcSync.State = 0;
	if (m_FreezeState.nTime > 0)
		NpcSync.State |= STATE_FREEZE;
	if (m_PoisonState.nTime > 0)
		NpcSync.State |= STATE_POISON;
	if (m_StunState.nTime > 0)
		NpcSync.State |= STATE_STUN;
	if (m_HideState.nTime > 0)
		NpcSync.State |= STATE_HIDE;
	//if (m_MoveShadow.nTime > 0)
	//	NpcSync.State |= STATE_MOVE;
	if (m_FrozenAction.nTime > 0)
		NpcSync.State |= STATE_FROZEN;
	if (m_WalkRun.nTime > 0)
		NpcSync.State |= STATE_WALKRUN;
	//if (m_ForbidAttack.nTime > 0)
	//	NpcSync.State |= STATE_FBDATK;
	NpcSync.m_bySeries			= (BYTE)m_Series;
	NpcSync.m_nProtectedTime = m_nProtectedTime;			//vong tron bat tu, vßng trßn bÊt tö
	NpcSync.m_CurrentLife		= m_CurrentLife;
	NpcSync.m_CurrentLifeMax	= m_CurrentLifeMax;
	NpcSync.m_LifeMax			= m_LifeMax;
	NpcSync.m_WalkSpeed			= m_WalkSpeed;
	NpcSync.m_RunSpeed			= m_RunSpeed;
	NpcSync.m_ASpeed			= m_CurrentAttackSpeed;
	NpcSync.m_CSpeed			= m_CurrentCastSpeed;
	NpcSync.m_CurrentMana		= m_CurrentMana;	//viet them sync mana teamMNG
	NpcSync.m_CurrentManaMax	= m_CurrentManaMax;
	NpcSync.m_ManaMax			= m_ManaMax;
	NpcSync.MissionGroup		= m_nMissionGroup;//#NpcMissionGroup
	memcpy(NpcSync.StateInfo, m_btStateInfo, sizeof(BYTE) * MAX_SKILL_STATE);
	NpcSync.NpcEnchant			= m_cGold.GetGoldType();
	// [3HD 25/08] nhu SendSyncData: giu mau VANG qua duong sync lien tuc.
	if (NpcSync.NpcEnchant == 0 && m_Type == boss_gold)
		NpcSync.NpcEnchant		= boss_gold;
	static const POINT	POff[8] = 	//MAX_PLAYER
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	// Tran nguoi nhan moi lan phat dong bo. Ban goc JX2 dung 100 (MAX_BROADCAST_COUNT);
	// cay nay tung nang len MAX_PLAYER (1500) = gan nhu bo tran. Chu game chon thu 500.
	// Luu y: day la ngan sach DUYET dung chung cho ca 9 vung, nen phai di kem con tro xoay
	// diem bat dau trong KRegion::BroadCast, neu khong thi nguoi xep sau vi tri 500 se khong
	// bao gio nhan duoc dong bo (nguoi vo hinh).
	int nMaxCount = NPC_SYNC_BROADCAST_LIMIT;
	CURREGION.BroadCast(&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);
	int j;
	for (j = 0; j < 8; j++)
	{
		int nConRegion = CURREGION.m_nConnectRegion[j];
		if (nConRegion == -1)
			continue;
		_ASSERT(m_SubWorldIndex >= 0 && nConRegion >= 0);
		SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcSync, sizeof(NPC_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
	}
	//------------------------------------------------------End SYNC 1-------------------------------
	if (IsPlayer())
	{
		PLAYER_NORMAL_SYNC PlayerSync;
		PlayerSync.ProtocolType	= (BYTE)s2c_syncplayermin;
		PlayerSync.ID			= m_dwID;
		PlayerSync.WalkSpeed	= (BYTE)m_CurrentWalkSpeed;
		PlayerSync.RunSpeed		= (BYTE)m_CurrentRunSpeed;
		PlayerSync.AttackSpeed	= m_CurrentAttackSpeed;
		PlayerSync.CastSpeed	= m_CurrentCastSpeed;
		PlayerSync.HelmType		= (BYTE)m_HelmType;
		PlayerSync.ArmorType	= (BYTE)m_ArmorType;
		PlayerSync.WeaponType	= (BYTE)m_WeaponType;
		PlayerSync.MantleType	= (BYTE)m_MantleType; 
		PlayerSync.MaskType		= m_MaskType;		//#mat na
		PlayerSync.HonorID		= (BYTE)m_btHonorId;	//#HonorID
		if(m_bRideHorse)						//edit by phong kieu len xuong ngua
        {
            PlayerSync.HorseType        = (BYTE)m_HorseType;
        }
        else
        {
            PlayerSync.HorseType        = -1;
        }
		PlayerSync.RankID		= (BYTE)m_btRankId;
		PlayerSync.RankBattleID			= (DWORD)m_btRankBattleId;//#RankBattle
		PlayerSync.PlayerTitle			= (DWORD)m_btPlayerTitle;//#PlayerTitle
		PlayerSync.m_bBaiTan	=	(BYTE)m_BaiTan;
		strcpy(PlayerSync.MateName, Player[m_nPlayerIdx].m_PlayerMateName); //#MateName
		PlayerSync.CUnlocked	= (BYTE)Player[m_nPlayerIdx].m_CUnlocked;//sync trang thai dong mo khoa ruong
		PlayerSync.ExItemID		= (BYTE)m_ExItemId; // hanh trang
		PlayerSync.ExBoxID		= (BYTE)m_ExBoxId; // ruong mo rong
		PlayerSync.RankInWorld		= (int)Player[m_nPlayerIdx].m_nWorldStat; //xÕp h¹ng thÕ giíi
		PlayerSync.Repute			= (int)Player[m_nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
		PlayerSync.FuYuan			= (int)Player[m_nPlayerIdx].m_cFuYuan.GetFuYuanValue();
		PlayerSync.PKValue			= (BYTE)Player[m_nPlayerIdx].m_cPK.GetPKValue();
		PlayerSync.MantleLevel		= (BYTE)m_byMantleLevel;
		PlayerSync.ReBorn			= (BYTE)Player[m_nPlayerIdx].m_cReBorn.GetReBornNum();
		PlayerSync.nFirstFaction			= (BYTE)Player[m_nPlayerIdx].GetFirstAddFaction();
		PlayerSync.m_btSomeFlag		= 0;
		if (!Player[m_nPlayerIdx].m_cPK.GetNormalPKState())
			PlayerSync.m_btSomeFlag |= 0x01;
		if (m_FightMode)
			PlayerSync.m_btSomeFlag |= 0x02;
		if (Player[m_nPlayerIdx].m_bSleepMode)
			PlayerSync.m_btSomeFlag |= 0x04;
		if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKWar)
			PlayerSync.m_btSomeFlag |= 0x10;
		if (Player[m_nPlayerIdx].m_cTong.GetOpenFlag())
			PlayerSync.m_btSomeFlag |= 0x08;
		Player[m_nPlayerIdx].m_cTong.GetTongName(PlayerSync.TongName);
		Player[m_nPlayerIdx].m_cTong.GetTongTitle(PlayerSync.TongTitle);
		PlayerSync.TongFigure = Player[m_nPlayerIdx].m_cTong.GetFigure();
		PlayerSync.TongRecruit = Player[m_nPlayerIdx].m_cTong.GetRecruit();
		PlayerSync.ImagePlayer		= (BYTE)Player[m_nPlayerIdx].m_ImagePlayer;
		UpdateGameTitle(); // ensure m_szGameTitle is updated
		strcpy(PlayerSync.GameTitle, m_szGameTitle);
		memcpy(PlayerSync.bMeridianLevel, Player[m_nPlayerIdx].m_cMeridian.getMeridian(), sizeof(PlayerSync.bMeridianLevel));

		// ---- Port SimCity: nguon du lieu cho BOT ----
		// Bot la KNpc kind_player nhung KHONG co KPlayer: m_nPlayerIdx = 0 (KNpc.cpp:139)
		// nen moi dong Player[m_nPlayerIdx] o tren doc tu Player[0] (o dat truoc, luon rong).
		// Hau qua neu khong ghi de: MOI bot deu hien "khong bang hoi, PK 0, danh vong 0,
		// trung sinh 0, khong phoi ngau, hang the gioi 0" - lo ngay la bot.
		// Cac truong duoi day deu la truong CO SAN cua KNpc, khong them truong moi,
		// khong doi kich thuoc goi tin (PLAYER_NORMAL_SYNC = 229 byte).
		if (m_btSimCityBot)
		{
			// strncpy chu KHONG strcpy: xem SC_AddBot - cac truong nay tung la rac heap
			// vi ZeroMemory cua chung nam trong khoi #ifndef _SERVER (KNpc.cpp:164-176)
			// va ZeroMemory(MateName) o KNpc.cpp:232 bi chu thich.
			strncpy(PlayerSync.MateName, MateName, sizeof(PlayerSync.MateName) - 1);
			PlayerSync.MateName[sizeof(PlayerSync.MateName) - 1] = 0;
			strncpy(PlayerSync.TongName, m_szTongName, sizeof(PlayerSync.TongName) - 1);
			PlayerSync.TongName[sizeof(PlayerSync.TongName) - 1] = 0;
			strncpy(PlayerSync.TongTitle, m_szTongTitle, sizeof(PlayerSync.TongTitle) - 1);
			PlayerSync.TongTitle[sizeof(PlayerSync.TongTitle) - 1] = 0;
			PlayerSync.TongFigure   = (BYTE)m_nFigure;
			PlayerSync.TongRecruit  = m_Recruit;
			PlayerSync.CUnlocked    = 0;
			PlayerSync.RankInWorld  = (int)nRankInWorld;
			PlayerSync.Repute       = (int)nRepute;
			PlayerSync.FuYuan       = (int)nFuYuan;
			PlayerSync.PKValue      = (BYTE)nPKValue;
			PlayerSync.ReBorn       = (BYTE)nReBorn;
			PlayerSync.nFirstFaction = (BYTE)nFirstFaction;
			PlayerSync.ImagePlayer  = (BYTE)m_ImagePlayer;
			memset(PlayerSync.bMeridianLevel, 0, sizeof(PlayerSync.bMeridianLevel));
			// co trang thai: bot luon hoa binh (0x01), khong ngu (0x04), khong chien (0x10).
			PlayerSync.m_btSomeFlag = 0x01;
			if (m_FightMode)
				PlayerSync.m_btSomeFlag |= 0x02;
			if (m_szTongName[0])
				PlayerSync.m_btSomeFlag |= 0x08;
			// 0x20 = CO NHAN DIEN BOT cho client. Bit nay dang trong: client hien tai chi
			// test 0x01/0x02/0x04/0x10 (KProtocolProcess.cpp:2282-2297) nen bit la bi bo qua
			// => client cu tuong thich nguoc 100%, kich thuoc goi khong doi.
			// Client moi dung bit nay de loc bot khoi GetAroundPlayer / moi to doi (WAuto).
			PlayerSync.m_btSomeFlag |= 0x20;
		}
		int nMaxCount = NPC_SYNC_BROADCAST_LIMIT;
		CURREGION.BroadCast(&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX, m_MapY);
		for (j = 0; j < 8; j++)
		{
			int nConRegion = CURREGION.m_nConnectRegion[j];
			if (nConRegion == -1)
				continue;
			SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&PlayerSync, sizeof(PLAYER_NORMAL_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
		}
		//------------------------------------------------------End SYNC 2-------------------------------
		NPC_PLAYER_TYPE_NORMAL_SYNC	sSync;
		sSync.ProtocolType = s2c_syncnpcminplayer;
		sSync.m_dwNpcID = m_dwID;
		sSync.m_dwMapX = nMpsX;
		sSync.m_dwMapY = nMpsY;
		sSync.m_wOffX = m_OffX;
		sSync.m_wOffY = m_OffY;
	//	sSync.m_byDoing = m_Doing;
	//	sSync.MapID = SubWorld[m_SubWorldIndex].m_SubWorldID;
   	//	sSync.m_nEquipCount = Player[m_nPlayerIdx].m_ItemList.CountItemInAll();

		// Bot khong co ket noi mang: Player[0].m_nNetConnectIdx = -1. Goi nay chi phi tien
		// mang vo ich moi tick moi bot (khong sap - CIOCPServer::PackDataToClient co kiem bien,
		// MultiServer/Heaven/ServerStage.cpp:390-396).
		if (!m_btSimCityBot && m_nPlayerIdx > 0)
			g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(sSync));
		//------------------------------------------------------End SYNC 3-------------------------------
	}
}

void KNpc::BroadCastRevive(int nType)
{
	if (!IsPlayer())
		return;

	if (m_RegionIndex < 0)
		return;

	NPC_REVIVE_SYNC	NpcReviveSync;
	NpcReviveSync.ProtocolType = s2c_playerrevive;
	NpcReviveSync.ID = m_dwID;
	NpcReviveSync.Type = (BYTE)nType;

	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	int nMaxCount = MAX_BROADCAST_COUNT;
	CURREGION.BroadCast((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount, m_MapX, m_MapY);
	int j;
	for (j = 0; j < 8; j++)
	{
		int nConRegion = CURREGION.m_nConnectRegion[j];
		if (nConRegion == -1)
			continue;
		SubWorld[m_SubWorldIndex].m_Region[nConRegion].BroadCast((BYTE*)&NpcReviveSync, sizeof(NPC_REVIVE_SYNC), nMaxCount, m_MapX - POff[j].x, m_MapY - POff[j].y);
	}
}

int	KNpc::GetPlayerIdx()
{
	if (m_Kind != kind_player)
		return 0;
	return m_nPlayerIdx;
}

#endif

#ifndef _SERVER
void	KNpc::PaintSeriesNpc(char* szName, int nFontSize, int nHeightOff) 
{
	int nX, nY;
	KRUImage	m_ImageSeries;
	char*	m_nImageSeries;
    GetDrawPos(&nX, &nY);
	switch(m_Series)
	{
		case 0:
        {
            m_nImageSeries = "\\Spr\\Ui3\\Npc\\series0.spr";
			break;
        }
        case 1:
        {
            m_nImageSeries = "\\Spr\\Ui3\\Npc\\series1.spr";
			break;
        }
        case 2:
        {
            m_nImageSeries = "\\Spr\\Ui3\\Npc\\series2.spr";
			break;
        }
        case 3:
        {
            m_nImageSeries = "\\Spr\\Ui3\\Npc\\series3.spr";
			break;
        }
        case 4:
        {
            m_nImageSeries = "\\Spr\\Ui3\\Npc\\series4.spr";
			break;
        }
		default:
			break;
	}
    m_ImageSeries.nType = ISI_T_SPR;
    m_ImageSeries.Color.Color_b.a = 255;
    m_ImageSeries.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
    m_ImageSeries.uImage = 0;
    m_ImageSeries.nISPosition = IMAGE_IS_POSITION_INIT;
    m_ImageSeries.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT; //xem Ã« chÃ• Â®Ã© 3d
    strcpy(m_ImageSeries.szImage, m_nImageSeries);
    m_ImageSeries.oPosition.nX = nX + nFontSize * g_StrLen(szName) / 4 + 5;
    m_ImageSeries.oPosition.nY = nY; 
    m_ImageSeries.oPosition.nZ = nHeightOff;
    m_ImageSeries.nFrame = 0;
    g_pRepresent->DrawPrimitives(1, &m_ImageSeries, RU_T_IMAGE, FALSE);  
}

#include "scene/KScenePlaceC.h"
int KNpc::PaintInfo(int nHeightOffset, bool bSelect, int nFontSize, DWORD dwBorderColor)
{
	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && m_HideState.nTime > 0)
	{
		return 0;
	}
	// PaintInfo runs for EVERY npc EVERY paint frame. These config tables never
	// change at runtime, so load them ONCE and reuse. Before this fix a local
	// KIniFile/KTabFile was Load()-ed (read + parse + AES decrypt) per npc per
	// frame => 5-20 ms/frame in crowds with many titled players.
	static KTabFile s_HonorTab;
	static KIniFile s_BoboIni;
	static KTabFile s_RankTab;
	static BOOL s_bInfoTabsLoaded = FALSE;
	if (!s_bInfoTabsLoaded)
	{
		s_HonorTab.Load(HONOR_SETTING_FILE);
		s_BoboIni.Load(NPC_BOBO_FILE);
		s_RankTab.Load(PLAYER_RANK_BATTLE_SETTING_TABFILE);
		s_bInfoTabsLoaded = TRUE;
	}
	nFontSize = 13;
	char Buff[128], cbBuffer[32];
	int nMpsX, nMpsY, nMX, nMY, nNumFrames,  nXX, nYY;
	GetDrawPos(&nMpsX, &nMpsY);
	if(KNpc::g_DrawVision && m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex)
	{
		KRULine		Line;
		Line.oPosition.nZ = Line.oEndPos.nZ = 0;
		int nVision = KNpc::g_DrawVisionSkill;
		int i;
		Line.Color.Color_dw = 0xfff08000;
		for( i=0;i<64;++i)
		{
			if(i < 63)
			{
				Line.oPosition.nX = nMpsX + ((nVision * g_DirCos(i, 64)) >> 10);
				Line.oPosition.nY = nMpsY + ((nVision * g_DirSin(i, 64)) >> 10);
				Line.oEndPos.nX = nMpsX + ((nVision * g_DirCos(i+1, 64)) >> 10);
				Line.oEndPos.nY = nMpsY + ((nVision * g_DirSin(i+1, 64)) >> 10);
			}
			else
			{
				Line.oPosition.nX = nMpsX + ((nVision * g_DirCos(i, 64)) >> 10);
				Line.oPosition.nY = nMpsY + ((nVision * g_DirSin(i, 64)) >> 10);
				Line.oEndPos.nX = nMpsX + ((nVision * g_DirCos(0, 64)) >> 10);
				Line.oEndPos.nY = nMpsY + ((nVision * g_DirSin(0, 64)) >> 10);
			}
			g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 0);
		}
		nVision = KNpc::g_DrawVision;
		Line.Color.Color_dw = 0xff00f000;
		for( i=0;i<64;++i)
		{
			if(i < 63)
			{
				Line.oPosition.nX = nMpsX + ((nVision * g_DirCos(i, 64)) >> 10);
				Line.oPosition.nY = nMpsY + ((nVision * g_DirSin(i, 64)) >> 10);
				Line.oEndPos.nX = nMpsX + ((nVision * g_DirCos(i+1, 64)) >> 10);
				Line.oEndPos.nY = nMpsY + ((nVision * g_DirSin(i+1, 64)) >> 10);
			}
			else
			{
				Line.oPosition.nX = nMpsX + ((nVision * g_DirCos(i, 64)) >> 10);
				Line.oPosition.nY = nMpsY + ((nVision * g_DirSin(i, 64)) >> 10);
				Line.oEndPos.nX = nMpsX + ((nVision * g_DirCos(0, 64)) >> 10);
				Line.oEndPos.nY = nMpsY + ((nVision * g_DirSin(0, 64)) >> 10);
			}
			g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 0);
		}
	}
	
	DWORD	dwColor;
	int nX, nY;
	int nMW = 0;
	int nHeightOff = nHeightOffset + nFontSize + 1;

	int pColor[6] = 
	{
		0xff000000,
		0xff009f00, //mµu xanh l¸
		0xff6495ED, //mµu xanh d­¬ng
		0xffc829ef, //mµu tÝm
		0xffffd100, //mµu vµng
		0xffff6b00, //mµu cam
	};
	
	if (m_Kind == kind_player)
	{
		
		switch(m_CurrentCamp)
		{
		case camp_begin: // 0 mµu tr¾ng
			dwColor = 0xffffffff;
			break;
		case camp_justice:	//1 chinh phai
			dwColor = 0xff000000 | (255 << 16) | (168 << 8) | 94;
			break;
		case camp_evil:		//2 ta phai
			dwColor = 0xff000000 | (255 << 16) | (146 << 8) | 255;
			break;
		case camp_balance: //3 trung lap
			dwColor = 0xff000000 | (85 << 16) | (255 << 8) | 145;
			break;
		case camp_free: //4 sat thu
			dwColor = 0xff000000 | (255 << 16);
			break;
		case camp_animal: // 5 mµu tr¾ng
			dwColor = 0xffffffff; 
			break;
		case camp_event: // 6 mµu hång
			dwColor = 0xff000000 | (238 << 16) | (18 << 8) | 137;
			break;
		default:
			dwColor = 0x00ce14c4;
			break;
		}
		

		char	szString[128];
		if (SubWorld[Npc[CLIENT_PLAYER_INDEX].m_SubWorldIndex].m_SubWorldID == 209 && m_nPlayerIdx != CLIENT_PLAYER_INDEX)
		{
			strcpy(szString, "Nh©n Sü Vâ L©m");
		}
		else
		{
			strcpy(szString, Name);
		}
		if (m_FreezeState.nTime || m_PoisonState.nTime || m_FrozenAction.nTime || m_StunState.nTime)
		{
			strcat(szString, "(");
			if (m_FreezeState.nTime)
				strcat(szString, "B¨ng");
			if (m_PoisonState.nTime)
				strcat(szString, "§éc");
			if (m_FrozenAction.nTime)
				strcat(szString, "Mª");
			if (m_StunState.nTime)
				strcat(szString, "Cho¸ng");
			if(m_HideState.nTime > 0)
				strcat(szString, "Èn");

			strcat(szString, ")");
		}
	
		nXX = nMpsX - nFontSize * g_StrLen(szString) / 4 + ((m_byMantleLevel > 0 && m_byMantleLevel <= MAX_ITEM_LEVELFF) ? 40 : 0);
		nYY = nMpsY;
		g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nXX, nYY, dwColor, 0, nHeightOff, dwBorderColor);
		if (m_byMantleLevel>0 && (m_byMantleLevel <= MAX_ITEM_LEVELFF))
			nXX = PaintMantle(nHeightOff, nFontSize, nXX, nYY);
		nHeightOffset += nFontSize + 1;

		nX = nMpsX - nFontSize*g_StrLen(Name)/4;
		nY = nMpsY;
		//
	/*	if (m_szTeamMem[0])//VÏ th«ng tin tõ auto lªn ®Çu nh©n vËt
		{
			sprintf(m_szTongName, "%s", m_szTeamMem);
		}*/

		char szUpperLine[64];  // 
		int yOffset = nMpsY - 60;  // 
		int smallerFontSize = 12;
		if (m_szGameTitle[0] &&   m_szTongName[0] && (m_btPlayerTitle || m_btRankBattleId))
		{
		    yOffset = nMpsY - 120;
		}
		else if (m_szGameTitle[0] &&
		        (m_szTongName[0] || m_btPlayerTitle || m_btRankBattleId))
		{
		    yOffset = nMpsY - 90;
		}
		else if (m_szGameTitle[0] &&
		        (m_szTongTitle == NULL || m_szTongTitle[0] == 0))
		{
		    yOffset = nMpsY - 50;
		}

				
		

		if (m_szTeamMem[0])
		{	
			//		DWORD dwWhiteColor = 0xFFFFFFFF;
			strcpy(szUpperLine, m_szTeamMem); //
			g_pRepresent->OutputText(smallerFontSize, szUpperLine, KRF_ZERO_END, nMpsX - smallerFontSize * g_StrLen(szUpperLine) / 4, yOffset, dwColor, 0, nHeightOff, dwBorderColor);
			yOffset += smallerFontSize + 1;
		}
		//
		if (m_szTongName[0])
		{
			char szTong[64];
			if(m_szTongTitle[0] == 0 || NULL == m_szTongTitle)
			{
				strcpy(szTong, m_szTongName);
			//	if(!m_szTeamMem[0])//fix by phong kiÒu kh«ng vÏ TongFigure khi hiÓn thÞ th«ng tin auto
			//	{
					switch(m_nFigure)
					{
					case enumTONG_FIGURE_MEMBER:
						strcat(szTong, "[M«n §Ö]");
						break;
					case enumTONG_FIGURE_MANAGER:
						strcat(szTong, "[§­êng Chñ]");
						break;
					case enumTONG_FIGURE_DIRECTOR:
						strcat(szTong, "[Tr­ëng L·o]");
						break;
					case enumTONG_FIGURE_MASTER:
						strcat(szTong, "[Bang Chñ]");
						break;
					}
				}
			else
			{
				if (SubWorld[Npc[CLIENT_PLAYER_INDEX].m_SubWorldIndex].m_SubWorldID == 209)
				{
					sprintf(szTong,"");
				}
				else
				{
					sprintf(szTong, "[%s] %s", m_szTongName, m_szTongTitle);
				}
				//	else
				//		strcpy(szTong, m_szTongName); 
			}
			//	g_pRepresent->OutputText(nFontSize, szTong, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szTong) / 4, yOffset, dwColor, 0, nHeightOff, dwBorderColor);
			g_pRepresent->OutputText(nFontSize, szTong, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szTong) / 4, nMpsY -24, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}
		if (m_szGameTitle[0])
		{
			int Map = SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].m_SubWorldID;
			
			
			if (Map != 209)
    		{
				int nLineStep = (int)((nFontSize + 2) * 1.6f);
				nYY -= (int)((nFontSize + 2) * 2.0f);
				DWORD dwGreenColor = 0xFF00FF00; // M?u xanh l? (ARGB: alpha=255, red=0, green=255, blue=0) - RGB(0, 255, 0)
	   			if (m_btPlayerTitle || m_btRankBattleId)
				{
	        	nYY -= nLineStep;
	   			}
				if (m_szTongName[0])
				{
	       		 nYY -= nLineStep;
	   			}

				g_pRepresent->OutputText(nFontSize, m_szGameTitle, KRF_ZERO_END,
					nMpsX - nFontSize * g_StrLen(m_szGameTitle) / 4, nYY,
					dwGreenColor, 0, nHeightOff, dwBorderColor);
				nHeightOffset += nFontSize + 1;
			}
		}
		//m_btHonorId = 20;
		if(m_btHonorId > 0)	 //#danh hieu
		{
			memset(Buff, 0, sizeof(Buff));
			memset(cbBuffer, 0, sizeof(cbBuffer));
			itoa(m_btHonorId, cbBuffer, 10);
			s_HonorTab.GetString(cbBuffer, "HONORLINK", "",Buff,sizeof(Buff));
			s_HonorTab.GetInteger(cbBuffer, "HONORX", 0, &nMX);
			s_HonorTab.GetInteger(cbBuffer, "HONORY", 0, &nMY);
			s_HonorTab.GetInteger(cbBuffer, "TOTALFRAME", 1, &nNumFrames);
			s_HonorTab.GetInteger(cbBuffer, "WIGTH", 0, &nMW);

			KRUImage RURank;
			RURank.nType = ISI_T_SPR;
			RURank.Color.Color_b.a = 255;
			RURank.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RURank.uImage = 0;
			RURank.nISPosition = IMAGE_IS_POSITION_INIT;
			RURank.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RURank.szImage, Buff);
			RURank.nFrame = g_SubWorldSet.GetGameTime() / 2 % nNumFrames;

			RURank.oPosition.nX = nMpsX - nMX;
			RURank.oPosition.nY = nY - nMY;
			RURank.oPosition.nZ = nHeightOffset;
			g_pRepresent->DrawPrimitives(1, &RURank, RU_T_IMAGE, 0);		
		}

		if (m_MaskType > 0)	//#mat na
		{
			memset(Buff, 0, sizeof(Buff));
			if (m_bRideHorse)
				s_BoboIni.GetString("Actions", "0", "", Buff, sizeof(Buff));
			else if (m_Doing == do_sit)
				s_BoboIni.GetString("Actions", "1", "", Buff, sizeof(Buff));
			KRUImage RUIconImage;
			RUIconImage.nType = ISI_T_SPR;
			RUIconImage.Color.Color_b.a = 255;
			RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImage.uImage = 0;
			RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImage.szImage, Buff);
			RUIconImage.oPosition.nX = nMpsX + (nFontSize*g_StrLen(Name)/4) + 10 + (m_btHonorId > 0 ? 10 : 0) + (nReBorn > 0 ? 10 : 0);
			RUIconImage.oPosition.nY = nY + 20;
			RUIconImage.oPosition.nZ = nHeightOff;
			RUIconImage.nFrame = 0;
			g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);				
		}

		if (nReBorn) //#trung sinh
		{
			char* sOut1 = "\\spr\\ranktrungsinh\\saotrungsinh1.spr";
			char* sOut2 = "\\spr\\ranktrungsinh\\saotrungsinh2.spr";
			char* sOut3 = "\\spr\\ranktrungsinh\\saotrungsinh3.spr";
			char* sOut4 = "\\spr\\ranktrungsinh\\saotrungsinh4.spr";
			char* sOut5 = "\\spr\\ranktrungsinh\\saotrungsinh5.spr";
			char* sOut6 = "\\spr\\ranktrungsinh\\saotrungsinh6.spr";
			char* sOut7 = "\\spr\\ranktrungsinh\\saotrungsinh7.spr";
			char* sOut8 = "\\spr\\ranktrungsinh\\saotrungsinh8.spr";
			char* sOut9 = "\\spr\\ranktrungsinh\\saotrungsinh9.spr";
			char* sOut10 = "\\spr\\ranktrungsinh\\saotrungsinh10.spr";
			m_nTotalFrame = 10;
			if (GetTickCount() - m_nTime >= 350)
			{ 
				m_nFrame++; 
				m_nTime = GetTickCount(); 
				if (m_nFrame >= m_nTotalFrame) 
					m_nFrame = 0; 
			}
			KRUImage RUIconImage;
			RUIconImage.nType = ISI_T_SPR;
			RUIconImage.Color.Color_b.a = 255;
			RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImage.uImage = 0;
			RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
			if (nReBorn == 1)
			{
				strcpy(RUIconImage.szImage, sOut1);
			}
			else if (nReBorn == 2)
			{
				strcpy(RUIconImage.szImage, sOut2);
			}
			else if (nReBorn == 3)
			{
				strcpy(RUIconImage.szImage, sOut3);
			}
			else if (nReBorn == 4)
			{
				strcpy(RUIconImage.szImage, sOut4);
			}
			else if (nReBorn == 5)
			{
				strcpy(RUIconImage.szImage, sOut5);
			}
			else if (nReBorn == 6)
			{
				strcpy(RUIconImage.szImage, sOut6);
			}
			else if (nReBorn == 7)
			{
				strcpy(RUIconImage.szImage, sOut7);
			}
			else if (nReBorn == 8)
			{
				strcpy(RUIconImage.szImage, sOut8);
			}
			else if (nReBorn == 9)
			{
				strcpy(RUIconImage.szImage, sOut9);
			}
			else if (nReBorn == 10)	// tai doan nay cho ket thuc luon
			{
				strcpy(RUIconImage.szImage, sOut10);
			}
			else if (nReBorn > 10)	// neu hon 10 thi hien thi 10
			{
				strcpy(RUIconImage.szImage, sOut10);
			}
			if (m_bRideHorse)
			{
				RUIconImage.oPosition.nY = nMpsY-(nHeightOff*1.48) - 22;
			}
			else
			{
				RUIconImage.oPosition.nY = nMpsY-(nHeightOff*1.48) - 12;
			}
	
			RUIconImage.oPosition.nX = nMpsX
			    - (nFontSize * g_StrLen(Name) / 4)  
			    - 10                                
			    - (m_btHonorId > 0 ? 10 : 0)
				- ((m_byMantleLevel > 0 && m_byMantleLevel <= MAX_ITEM_LEVELFF) ? 40 : 0);
			RUIconImage.oPosition.nY = nMpsY - 8;
			RUIconImage.oPosition.nZ = nHeightOff - 12;
			RUIconImage.nFrame = (SubWorld[0].m_dwCurrentTime * 10 / 18) % 10;
			g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);	
			//RUIconImage.oPosition.nZ = 0;
				//RUIconImage.nFrame = m_nFrame;
			//	g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);			
		}

		if (m_btPlayerTitle || m_btRankBattleId) //#PlayerTitle //#RankBattle
		{
			DWORD nRankId;
			int nColor = 3;
			if(m_btRankBattleId)
			{
				nRankId = m_btRankBattleId + 1;
				s_RankTab.GetString(nRankId, "TitleName",	"", szString, sizeof(szString));
				dwColor = pColor[nColor];
			}
			else if(m_btPlayerTitle)
			{
				nRankId = m_btPlayerTitle + 1;
				s_RankTab.GetString(nRankId, "TitleName",	"", szString, sizeof(szString));
				dwColor = pColor[nColor];
			}

			nX = nMpsX - nFontSize*g_StrLen(szString)/4;
			nY = nMpsY - 24;
			if (m_szTongName[0])
				nY -= 30;
			g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nX, nY, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		if (m_BaiTan) //#bµy b¸n
		{
			int	nMpsX, nMpsY;
			GetDrawPos(&nMpsX, &nMpsY);
			int nWid = nFontSize * g_StrLen(ShopName) / 2 + 10;
			int nHei = nFontSize + 12;
				
			char*sOutm = "\\Spr\\Ui3\\°Ú̀¯\\°Ú̀¯Í·¶¥̀ơ£­ÖĐ.spr"; //Head
			KRUImage RUIconImageR;
			RUIconImageR.nType = ISI_T_SPR;
			RUIconImageR.Color.Color_b.a = 255;
			RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImageR.uImage = 0;
			RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImageR.szImage, sOutm);
			RUIconImageR.oPosition.nX = nMpsX - nWid / 2;
			RUIconImageR.oPosition.nY = nMpsY - 52;
			RUIconImageR.oPosition.nZ = nHeightOffset;
			RUIconImageR.nFrame = 0;
			for (int i = 0; i < nWid;i++)
			{
				RUIconImageR.oPosition.nX = nMpsX - nWid / 2 + i;
				g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
			}

			if (m_szGameTitle[0] && (m_btPlayerTitle || m_btRankBattleId) && (m_szTongName[0] || m_szTeamMem[0]))//#PlayerTitle
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 134, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else if ((m_btPlayerTitle || m_btRankBattleId) && (m_szTongName[0] || m_szTeamMem[0]))
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 110, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else if (m_szGameTitle[0] && ((m_szTongName[0] || m_szTeamMem[0])|| (m_btPlayerTitle || m_btRankBattleId)))
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 110, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else  if (m_szTongName[0] || m_szTeamMem[0])
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 88, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else if (m_btPlayerTitle || m_btRankBattleId)
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 88, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else if (m_szGameTitle[0])
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 88, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 62, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);

			char* sOutR = "\\Spr\\Ui3\\°Ú̀¯\\°Ú̀¯Í·¶¥̀ơ£­Ó̉.spr";	//Head
			RUIconImageR.nType = ISI_T_SPR;
			RUIconImageR.Color.Color_b.a = 255;
			RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImageR.uImage = 0;
			RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImageR.szImage, sOutR);
			RUIconImageR.oPosition.nX = nMpsX + nWid / 2;
			RUIconImageR.oPosition.nY = nMpsY;
			RUIconImageR.oPosition.nZ = nHeightOffset + 30;
			RUIconImageR.nFrame = 0;
			g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
			
			char* sOutL = "\\Spr\\Ui3\\°Ú̀¯\\°Ú̀¯Í·¶¥̀ơ£­×ó.spr";	//Head
			RUIconImageR.nType = ISI_T_SPR;
			RUIconImageR.Color.Color_b.a = 255;
			RUIconImageR.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImageR.uImage = 0;
			RUIconImageR.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImageR.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImageR.szImage, sOutL);
			RUIconImageR.oPosition.nX = nMpsX - nWid / 2 - 18;
			RUIconImageR.oPosition.nY = nMpsY;
			RUIconImageR.oPosition.nZ = nHeightOffset + 30;
			RUIconImageR.nFrame = 0;
			g_pRepresent->DrawPrimitives(1, &RUIconImageR, RU_T_IMAGE, FALSE);
			nHeightOffset += nFontSize + 1;
		}
	}
	else if (m_Kind == kind_dialoger)
	{
		//dwColor = 0xffffffff;
		//g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
		//nHeightOffset += nFontSize + 1;
		//int nNpcIdx = 0;
		
		dwColor = 0xffffffff;
		g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
		nHeightOffset += nFontSize + 1;
		
		//strcpy(DescName,"Npc DescName");
		//dwColor = 0x0027fe04 ;
		//g_pRepresent->OutputText(nFontSize, DescName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(DescName) / 4, nMpsY - 30, dwColor, 0, nHeightOff, dwBorderColor);
		//nHeightOffset += nFontSize + 1;
	}
//	else if (bSelect)
//	{
//		if (this->m_cGold.GetGoldType() == 0)
//			dwColor = 0xffffffff;
//		else
//			dwColor = 0xffebb200;	// ??
//		g_pRepresent->OutputText(nFontSize, Name, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
//		nHeightOffset += nFontSize + 1;
//	}

	else if (bSelect)//ngu hanh quai
	{	
		int nX, nY;
		GetDrawPos(&nX, &nY);

		switch(m_Type)
		{
		case boss_none:
			dwColor = 0xffffffff;// mau trang 
			break;
		case boss_blue:
			dwColor = 0x006569d7;//ten quai mau xanh duong
			break;
		case boss_gold:
			dwColor = 255 << 16 | 217 << 8 | 78;			//dwColor = 0xFFCC00;// mau hoang kim
			break;
		case boss_event:
			dwColor = 0x00ce14c4;//ten quai mau tim 
			break;
		case boss_war:
			dwColor = 0xFF0000;// mau do
			break;
		default:
			dwColor = 0x00CC00;// mau xanh la
			break;
		}

		char	szString[128];
		strcpy(szString, Name);
		/*if (m_FreezeState.nTime || m_PoisonState.nTime || m_FrozenAction.nTime || m_StunState.nTime)//ch÷ hiÖu øng qu¸i khi bÞ tróng ®ßn kh«ng vÏ n÷a
		{
			strcat(szString, "("); 
			if (m_FreezeState.nTime)
				strcat(szString, "B¨ng");
			if (m_PoisonState.nTime)
				strcat(szString, "§éc");
			if (m_FrozenAction.nTime)
				strcat(szString, "Mª");
			if (m_StunState.nTime)
				strcat(szString, "Cho¸ng");
			if(m_HideState.nTime > 0)
				strcat(szString, "Èn");
			strcat(szString, ")");
		}*/
	
		g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);	
		nHeightOffset += nFontSize + 1;
		PaintSeriesNpc(Name, nFontSize, nHeightOff);
        //char szDebuger[80];
		//sprintf(szDebuger, "%s[%d][%d][%d]", Name,  m_CurrentLife, m_Experience * 2,m_Level );
		//g_pRepresent->OutputText(nFontSize, szDebuger, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szDebuger) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
		//PaintSeriesNpc(Name, nFontSize, nHeightOff);
		/*if (m_Series > -1)
		{
			memset(Buff, 0, sizeof(Buff));
			memset(cbBuffer, 0, sizeof(cbBuffer));
			itoa(this->m_Series, cbBuffer, 10);
			pIni.Load(NPC_BOBO_FILE);
			pIni.GetString("Series", cbBuffer, "", Buff, sizeof(Buff));
			pIni.Clear();
			KRUImage RUIconImage;
			RUIconImage.nType = ISI_T_SPR;
			RUIconImage.Color.Color_b.a = 255;
			RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImage.uImage = 0;
			RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;	//xem Ã« chÃ• Â®Ã© 3d
			strcpy(RUIconImage.szImage, Buff);
			RUIconImage.oPosition.nX = nMpsX + nFontSize * g_StrLen(Name) / 4 + 5;
			RUIconImage.oPosition.nY = nMpsY; 
			RUIconImage.oPosition.nZ = nHeightOff;
			RUIconImage.nFrame = 0;
			g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);
		}*/
	}
	
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex != m_Index && Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char	szMsg[256];
		int nPosX, nPosY;
		GetDrawPos(&nPosX, &nPosY);
		sprintf(szMsg,
			"NpcID:%d NpcIndex:%d Life:%d\n"
			"RegionIndex:%d Pos1:%d,%d Offset:%d, %d\n"
			"RegionID:%d Pos2:%d,%d\n",
			m_dwID, m_Index, m_CurrentLife,
			m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY,
			SubWorld[0].m_Region[m_RegionIndex].m_RegionID, nPosX, nPosY
		);
		g_pRepresent->OutputText(12, szMsg, KRF_ZERO_END, nMpsX, nMpsY + 20, 0xfff0fff0, 0, m_Height);
	}
	
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex == m_Index && Player[CLIENT_PLAYER_INDEX].m_DebugMode)
	{
		char	szMsg[256];
		int nCount[9];
		for (int i = 0; i < 9; i++)
			nCount[i] = 0;
		if (LEFTUPREGIONIDX >= 0)
			nCount[0] = LEFTUPREGION.m_NpcList.GetNodeCount();
		if (UPREGIONIDX >= 0)
			nCount[1] = UPREGION.m_NpcList.GetNodeCount();
		if (RIGHTUPREGIONIDX >= 0)
			nCount[2] = RIGHTUPREGION.m_NpcList.GetNodeCount();
		if (LEFTREGIONIDX >= 0)
			nCount[3] = LEFTREGION.m_NpcList.GetNodeCount();
		if (m_RegionIndex >= 0)
			nCount[4] = CURREGION.m_NpcList.GetNodeCount();
		if (RIGHTREGIONIDX >= 0)
			nCount[5] = RIGHTREGION.m_NpcList.GetNodeCount();
		if (LEFTDOWNREGIONIDX >= 0)
			nCount[6] = LEFTDOWNREGION.m_NpcList.GetNodeCount();
		if (DOWNREGIONIDX >= 0)
			nCount[7] = DOWNREGION.m_NpcList.GetNodeCount();
		if (RIGHTDOWNREGIONIDX >= 0)
			nCount[8] = RIGHTDOWNREGION.m_NpcList.GetNodeCount();
		
		int nPosX, nPosY;
		GetDrawPos(&nPosX, &nPosY);
		sprintf(szMsg,
			"NpcID:%d Life:%d\n"
			"RegionIndex:%d Pos1:%d,%d Offset:%d, %d\n"
			"RegionID:%d Pos2:%d,%d\n"
			"CR_NpcNumber:%d\n"		//Npc trong cïng region
			"AR_NpcNumber:\n"			//Npc ë c¸c 8 region l©n cËn vµ ë gi÷a ®ang ®øng
			"%02d,%02d,%02d\n"
			"%02d,%02d,%02d\n"
			"%02d,%02d,%02d",
			m_dwID, m_CurrentLife,
			m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY,
			SubWorld[0].m_Region[m_RegionIndex].m_RegionID, nPosX, nPosY,
			CURREGION.m_NpcList.GetNodeCount(),
			nCount[0], nCount[1], nCount[2],
			nCount[3], nCount[4], nCount[5],
			nCount[6], nCount[7], nCount[8]			
			);
		
		g_pRepresent->OutputText(12, szMsg, -1, 320, 40, 0xffffffff);

	}
#endif

	return nHeightOffset;
}

int KNpc::PaintMantle(int nHeightOff, int nFontSize, int nMpsX, int nMpsY)
{
	KRUImage RUIconImage;
	RUIconImage.nType = ISI_T_SPR;
	RUIconImage.Color.Color_b.a = 255;
	RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	RUIconImage.uImage = 0;
	RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
	RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	strcpy(RUIconImage.szImage, PlayerSet.m_szFortuneRankPic[m_byMantleLevel]);
	KImageParam	Param;
	g_pRepresent->GetImageParam(RUIconImage.szImage, &Param, ISI_T_SPR);
	RUIconImage.oPosition.nX = nMpsX - Param.nWidth;
	RUIconImage.oPosition.nY = nMpsY - Param.nHeight/2 - 4;
	RUIconImage.oPosition.nZ = nHeightOff;
	RUIconImage.nFrame = g_SubWorldSet.GetGameTime() % Param.nNumFrames;
	g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, 0);

	return RUIconImage.oPosition.nX;
}
int	KNpc::PaintChat(int nHeightOffset)
{
	//if (m_Kind != kind_player)
	//	return nHeightOffset;
	if (m_nChatContentLen <= 0)
		return nHeightOffset;
	if (m_nChatNumLine <= 0)
		return nHeightOffset;
	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && m_HideState.nTime > 0)
		return 0;

	int nFontSize = 12;
	int	nWidth, nHeight;
	int	nMpsX, nMpsY;
	KRUShadow sShadow;
	KOutputTextParam sParam;
	sParam.BorderColor = 0;
	sParam.nNumLine = m_nChatNumLine;
	nWidth = m_nChatFontWidth * nFontSize / 2;
	nHeight = sParam.nNumLine * (nFontSize + 1);

	nWidth += 6;	
	nHeight += 5;

	GetDrawPos(&nMpsX, &nMpsY);
	sParam.nX = nMpsX - nWidth / 2;
	sParam.nY = nMpsY;
	sParam.nZ = nHeightOffset + nHeight;
	sParam.Color = SHOW_CHAT_COLOR;
	sParam.nSkipLine = 0;
	sParam.nVertAlign = 0;

	sShadow.oPosition.nX = sParam.nX;
	sShadow.oPosition.nX -= 3;	
	sShadow.oPosition.nY = sParam.nY;
	sShadow.oPosition.nZ = sParam.nZ;
	sShadow.oEndPos.nX = sParam.nX + nWidth;
	sShadow.oEndPos.nX += 2;	
	sShadow.oEndPos.nY = sParam.nY;
	sShadow.oEndPos.nZ = sParam.nZ - nHeight;
	//sShadow.Color.Color_dw = 0x00FFFF00;
	//g_pRepresent->DrawPrimitives(1, &sShadow, RU_T_RECT, false);

	sShadow.Color.Color_dw = 0x14000000;
	//g_pRepresent->DrawPrimitives(1, &sShadow, RU_T_SHADOW, false);
	
	sParam.bPicPackInSingleLine = true;
	g_pRepresent->OutputRichText(nFontSize, &sParam, m_szChatBuffer, m_nChatContentLen, nWidth);

	return sParam.nZ;
}

#include "../../Engine/Src/Text.h"
int	KNpc::SetChatInfo(const char* Name, const char* pMsgBuff, unsigned short nMsgLength)
{
	if(nMsgLength >= 128)//edit by phong kieu xu ly chat lon hon 90 ky tu
	{
		nMsgLength = 15;
	}

	int nFontSize = 12;

	char szChatBuffer[MAX_SENTENCE_LENGTH];

	memset(szChatBuffer, 0, sizeof(szChatBuffer));

	if (nMsgLength)
	{
		int nOffset = 0;
		if (pMsgBuff[0] != KTC_TAB)
		{
			szChatBuffer[nOffset] = (char)KTC_COLOR;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0xFF;
			nOffset++;
			szChatBuffer[nOffset] = (char)0x00;
			nOffset++;
			strncpy(szChatBuffer + nOffset, Name, 32);
			nOffset += strlen(Name);
			szChatBuffer[nOffset] = ':';
			nOffset++;
			szChatBuffer[nOffset] = (char)KTC_COLOR_RESTORE;
			nOffset++;
		}
		else
		{
			pMsgBuff ++;
			nMsgLength --;
		}

		if (nMsgLength)
		{
			memcpy(szChatBuffer + nOffset, pMsgBuff, nMsgLength);
			nOffset += nMsgLength;

			memset(m_szChatBuffer, 0, sizeof(m_szChatBuffer));
			m_nChatContentLen = MAX_SENTENCE_LENGTH;
			int maxLine = 3;
			TGetLimitLenEncodedString(szChatBuffer, nOffset, nFontSize, SHOW_CHAT_WIDTH,
				m_szChatBuffer, m_nChatContentLen, maxLine, true);

			m_nChatNumLine = TGetEncodedTextLineCount(m_szChatBuffer, m_nChatContentLen, SHOW_CHAT_WIDTH, m_nChatFontWidth, nFontSize, 0, 0, true);
			if (m_nChatNumLine >= maxLine)
				m_nChatNumLine = maxLine;
			m_nCurChatTime = IR_GetCurrentTime();
			return true;
		}
	}
	return false;
}

int	KNpc::PaintLife(int nHeightOffset, bool bSelect)
{
	if (!bSelect && (m_Kind != kind_player && m_Kind != kind_partner))
		return nHeightOffset;

	if (m_CurrentLifeMax <= 0)
		return nHeightOffset;

	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && m_HideState.nTime > 0)
		return nHeightOffset;

	//#khong su dung
	/*if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player ||
		 m_Kind == kind_partner)
		 && m_nPKFlag != enumPKMurder
		)
		return nHeightOffset;*/

	int	nMpsX, nMpsY;
	GetDrawPos(&nMpsX, &nMpsY);
	int nWid = SHOW_LIFE_WIDTH;
	int nHei = SHOW_LIFE_HEIGHT;
	KRUShadow	Blood;
	int nX = m_CurrentLife * 100 / m_CurrentLifeMax;
	if(nX > 100) //#fix thanh mau, mana
		nX = 100;
	if(nX < 0)
		nX = 0;
	if (nX >= 50)
	{
		Blood.Color.Color_b.r = 0;
		Blood.Color.Color_b.g = 255;
		Blood.Color.Color_b.b = 0;
	}
	else if (nX >= 25)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 255;
		Blood.Color.Color_b.b = 0;
	}
	else
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 0;
	}
	/*//#khong su dung
	if (m_nPKFlag == enumPKMurder)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 204;
	}
	if(nPKValue >= 10)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 0;
	}*/

	if (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex) &&
		(m_Kind == kind_player ||
		 m_Kind == kind_partner)
		)
	{
		Blood.Color.Color_b.r = 231;
		Blood.Color.Color_b.g = 194;
		Blood.Color.Color_b.b = 0;
	}

	if (Npc[m_Index].m_nPKFlag == enumPKMurder || 
		Player[Npc[m_Index].m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING)
	{
		Blood.Color.Color_b.r = 238;
		Blood.Color.Color_b.g = 18;
		Blood.Color.Color_b.b = 137;
	}
	if (Npc[m_Index].m_nPKFlag == enumPKTongWar)
	{
		Blood.Color.Color_b.r = 170;
		Blood.Color.Color_b.g = 30;
		Blood.Color.Color_b.b = 255;
	}
	if (Npc[m_Index].nPKValue >= 10)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 66;
	}

	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
	//edit by phong kieu khong ve~ mau npc doi thoai
	if (m_Kind == kind_dialoger)
	{
		//khong ve mau
	}
	else
	{
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);
	}

	Blood.Color.Color_b.r = 128;
	Blood.Color.Color_b.g = 128;
	Blood.Color.Color_b.b = 128;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);
	
	return nHeightOffset + nHei;
}

int	KNpc::PaintMana(int nHeightOffset)
{
	if (m_Kind != kind_player &&
		m_Kind != kind_partner)
		return nHeightOffset;

	if (m_CurrentManaMax <= 0)
		return nHeightOffset;

	return nHeightOffset;

	int	nMpsX, nMpsY;
	GetDrawPos(&nMpsX, &nMpsY);
	int nWid = 38;
	int nHei = 5;
	KRUShadow	Blood;
	int nX = m_CurrentMana * 100 / m_CurrentManaMax;
	if(nX > 100) //#fix thanh mau, mana
		nX = 100;
	if(nX < 0)
		nX = 0;
	if (nX >= 50)
	{
		Blood.Color.Color_b.r = 0;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 255;
	}
	else if (nX >= 25)
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 255;
		Blood.Color.Color_b.b = 0;
	}
	else
	{
		Blood.Color.Color_b.r = 255;
		Blood.Color.Color_b.g = 0;
		Blood.Color.Color_b.b = 0;
	}
	Blood.Color.Color_b.a = 0;
	Blood.oPosition.nX = nMpsX - nWid / 2;
	Blood.oPosition.nY = nMpsY;
	Blood.oPosition.nZ = nHeightOffset + nHei;
	Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
	Blood.oEndPos.nY = nMpsY;
	Blood.oEndPos.nZ = nHeightOffset;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);

	Blood.Color.Color_b.r = 255;
	Blood.Color.Color_b.g = 255;
	Blood.Color.Color_b.b = 255;
	Blood.oPosition.nX = Blood.oEndPos.nX;
	Blood.oEndPos.nX = nMpsX + nWid / 2;
	g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, FALSE);
	
	return nHeightOffset + nHei;
}

int KNpc::PaintTargetInfo(KUiPlayerItem* m_pPlayersList, KUiPlayerPaintTeamMNG* nPainTMG)
{
	for (int i = 0; i < nPainTMG->nCountS; i++)
	{
		int verDistance = 33;
		int nWid = 120;// nPainTMG->nWid;//58;
		int nHei_life = nPainTMG->nHei_life;//7;
		int nHei_mana = nPainTMG->nHei_mana;//4;

		int nWeightOffset = nPainTMG->nWeightOffset; //253;
		int nHeightOffset_life = nPainTMG->nHeightOffset_life; //179;
		int nHeightOffset_mana = nPainTMG->nHeightOffset_mana; //175;
		int nOffSet_member = i * nPainTMG->nOffSet_member; //38; // offset tinh bang px

		KRUShadow	Blood;
		int nX = m_pPlayersList[i].nPercenLife;
		if (nX > 100) //#fix thanh mau, mana
			nX = 100;
		if (nX < 0)
			nX = 0;
		//real life
		Blood.Color.Color_b.r = 205;
		Blood.Color.Color_b.g = 38;
		Blood.Color.Color_b.b = 38;
		Blood.Color.Color_b.a = 0;
		Blood.oPosition.nX = 420;
		Blood.oPosition.nY = 65 + i * verDistance - 10;
		if (i <= 2)
			nHei_life = 6;
		Blood.oPosition.nZ = nHeightOffset_life + nHei_life - nOffSet_member;
		Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
		Blood.oEndPos.nY = 75 + i * verDistance - 10; 
		Blood.oEndPos.nZ = nHeightOffset_life - nOffSet_member;
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);

		//background
		Blood.Color.Color_b.r = 128;
		Blood.Color.Color_b.g = 128;
		Blood.Color.Color_b.b = 128;
		Blood.oPosition.nX = Blood.oEndPos.nX;
		Blood.oEndPos.nX = Blood.oPosition.nX + nWid * (100 - nX) / 100;
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);

		int nX_mana = 0;
		if (nX_mana > 100)
			nX_mana = 100;

		////real mana
		//Blood.Color.Color_b.r = 30;
		//Blood.Color.Color_b.g = 144;
		//Blood.Color.Color_b.b = 255;
		//Blood.Color.Color_b.a = 0;
		//Blood.oPosition.nX = 46 + 530; 
		//Blood.oPosition.nY = 68 + i * verDistance - 10; 
		//if (i <= 2)
		//	nHei_mana = 3;
		//Blood.oPosition.nZ = nHeightOffset_mana + nHei_mana - nOffSet_member;
		//Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX_mana / 100;
		//Blood.oEndPos.nY = 71 + i * verDistance - 10; 
		//Blood.oEndPos.nZ = nHeightOffset_mana - nOffSet_member;
		//g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);

		////background
		//Blood.Color.Color_b.r = 128;
		//Blood.Color.Color_b.g = 128;
		//Blood.Color.Color_b.b = 128;
		//Blood.oPosition.nX = Blood.oEndPos.nX;
		//Blood.oEndPos.nX = Blood.oPosition.nX + 2 + nWid * (100 - nX_mana) / 100;
		//g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);
	}

	return 1;
}

int KNpc::PaintTeamMNG(KUiPlayerItem *m_pPlayersList, KUiPlayerPaintTeamMNG *nPainTMG)
{
	for(int i=0; i < nPainTMG->nCountS; i++)
	{
		int verDistance = 33;
		int	nMpsX = 53457;
		int nMpsY = 104208;
		GetDrawPos(&nMpsX, &nMpsY);
		int nWid = nPainTMG->nWid;//58;
		int nHei_life = nPainTMG->nHei_life;//7;
		int nHei_mana = nPainTMG->nHei_mana;//4;

		int nWeightOffset = nPainTMG->nWeightOffset; //253;
		int nHeightOffset_life = nPainTMG->nHeightOffset_life; //179;
		int nHeightOffset_mana = nPainTMG->nHeightOffset_mana; //175;
		int nOffSet_member = i*nPainTMG->nOffSet_member; //38; // offset tinh bang px

		KRUShadow	Blood;
		int nX = m_pPlayersList[i].nPercenLife;
		if(nX > 100) //#fix thanh mau, mana
			nX = 100;
		if(nX < 0)
			nX = 0;
		//real life
		Blood.Color.Color_b.r = 205;
		Blood.Color.Color_b.g = 38;
		Blood.Color.Color_b.b = 38;
		Blood.Color.Color_b.a = 0;
		Blood.oPosition.nX = 46;// nMpsX - nWeightOffset - nWid / 2;
		Blood.oPosition.nY = 62 + i * verDistance;// nMpsY;
		if(i <= 2)
			nHei_life = 6;
		Blood.oPosition.nZ = nHeightOffset_life + nHei_life - nOffSet_member;
		Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX / 100;
		Blood.oEndPos.nY = 67 + i * verDistance; // nMpsY; 
		Blood.oEndPos.nZ = nHeightOffset_life - nOffSet_member;
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);

		//background
		Blood.Color.Color_b.r = 128;
		Blood.Color.Color_b.g = 128;
		Blood.Color.Color_b.b = 128;
		Blood.oPosition.nX = Blood.oEndPos.nX;
		Blood.oEndPos.nX = Blood.oPosition.nX+ 2 + nWid * (100-nX) / 100;
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);

		int nX_mana = m_pPlayersList[i].nPercenMana;
		if(nX_mana > 100)
			nX_mana = 100;

		//real mana
		Blood.Color.Color_b.r = 30;
		Blood.Color.Color_b.g = 144;
		Blood.Color.Color_b.b = 255;
		Blood.Color.Color_b.a = 0;
		Blood.oPosition.nX = 46; // nMpsX - nWeightOffset - nWid / 2;
		Blood.oPosition.nY = 68 + i * verDistance; // nMpsY;
		if(i <= 2)
			nHei_mana = 3;
		Blood.oPosition.nZ = nHeightOffset_mana + nHei_mana - nOffSet_member;
		Blood.oEndPos.nX = Blood.oPosition.nX + nWid * nX_mana / 100;
		Blood.oEndPos.nY = 71 + i * verDistance; // nMpsY;
		Blood.oEndPos.nZ = nHeightOffset_mana - nOffSet_member;
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE);

		//background
		Blood.Color.Color_b.r = 128;
		Blood.Color.Color_b.g = 128;
		Blood.Color.Color_b.b = 128;
		Blood.oPosition.nX = Blood.oEndPos.nX;
		Blood.oEndPos.nX = Blood.oPosition.nX+ 2 + nWid * (100-nX_mana) / 100;
		g_pRepresent->DrawPrimitives(1, &Blood, RU_T_SHADOW, TRUE );
	}

	return 1;
}

void KNpc::Paint()
{
	if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex && (m_CurrentCamp == camp_audience || m_HideState.nTime > 0))
	{
		return;
	}

	BOOL bPaintBody = TRUE;

	if (Option.GetLow(LowNpc))
	{
		if (m_Kind == kind_normal || m_Kind == kind_dialoger)
		{
			if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				bPaintBody = FALSE;
			}
		}
	}

	if (Option.GetLow(LowEstPlayer))//add by phong kiÃ’u low npc low player
	{
		if (m_Kind == kind_player) 
		{
			//if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				bPaintBody = FALSE;
			}
		}
	}

	// Lam muot huong ve. Ban goc JX2 chay doan nay MOI LAN VE, nen khi PaintFps
	// cao hon 18 thi nhan vat quay mat nhanh hon nguyen ban (60fps = nhanh 3,3 lan).
	// Chot theo tick logic de toc do quay giong het ban goc du PaintFps bang bao nhieu.
	// Van dat trong ham ve (khong phai Activate) vi Activate bi bo qua khi
	// ProcessState() tra TRUE - luc choang/dong bang - se lam mat dung hinh giua luc quay.
	if (m_dwLastDirTick != SubWorld[0].m_dwCurrentTime)
	{
		m_dwLastDirTick = SubWorld[0].m_dwCurrentTime;
		if (m_ResDir != m_Dir)
		{
				int nDirOff = m_Dir - m_ResDir;
				if (nDirOff > 32)
						nDirOff -= 64;
				else if (nDirOff < - 32)
						nDirOff += 64;
				m_ResDir += nDirOff / 2;
				if (m_ResDir >= 64)
						m_ResDir -= 64;
				if (m_ResDir < 0)
						m_ResDir += 64;
		}
	}

	m_DataRes.Draw(m_Index, m_ResDir, m_Frames.nTotalFrame, m_Frames.nCurrentFrame, FALSE, bPaintBody);
	int nHeight = GetNpcPate() + GetNpcPatePeopleInfo();
	//add by Fong Kieu
	if (m_szTongName[0] || m_btPlayerTitle || m_btRankBattleId)
		nHeight += 10;
	if (m_szTongName[0] && (m_btPlayerTitle || m_btRankBattleId))
		nHeight += 20;

	//End add by Fong Kieu
	DrawMenuState(nHeight);
}
#endif

void	KNpc::AddBaseLifeMax(int nLife)
{
	m_LifeMax += nLife;
	m_CurrentLifeMax = m_LifeMax;
}

void	KNpc::SetBaseLifeMax(int nLifeMax) 
{
	m_LifeMax = nLifeMax;
	m_CurrentLifeMax = m_LifeMax;
}

void	KNpc::AddCurLifeMax(int nLife)
{
	m_CurrentLifeMax += nLife;
}

void	KNpc::AddBaseStaminaMax(int nStamina)
{
	m_StaminaMax += nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}

void	KNpc::SetBaseStaminaMax(int nStamina)
{
	m_StaminaMax = nStamina;
	m_CurrentStaminaMax = m_StaminaMax;
}

void	KNpc::AddCurStaminaMax(int nStamina)
{
	m_CurrentStaminaMax += nStamina;
}

void	KNpc::AddBaseManaMax(int nMana)
{
	m_ManaMax += nMana;
	m_CurrentManaMax = m_ManaMax;
}

void	KNpc::SetBaseManaMax(int nMana)
{
	m_ManaMax = nMana;
	m_CurrentManaMax = m_ManaMax;
}

void	KNpc::AddCurManaMax(int nMana)
{
	m_CurrentManaMax += nMana;
}

/*
void	KNpc::ResetLifeReplenish()
{
	m_LifeReplenish = (m_Level + 5) / 6;
	m_CurrentLifeReplenish = m_LifeReplenish;
}
*/

/*
void	KNpc::CalcCurLifeMax()
{
}
*/

/*
void	KNpc::CalcCurStaminaMax()
{
	m_CurrentStaminaMax = m_StaminaMax;		
}
*/

/*
void	KNpc::CalcCurManaMax()
{
	m_CurrentManaMax = m_ManaMax;			
}
*/

void	KNpc::CalcCurLifeReplenish()
{
	m_CurrentLifeReplenish = m_LifeReplenish;	
}

void	KNpc::Remove()
{
/*	m_LoopFrames = 0;
	m_Index = 0;
	m_PlayerIdx = -1;
	m_Kind = 0;
	m_dwID = 0;
	Name[0] = 0;*/
	Init();
#ifndef _SERVER
	m_DataRes.Remove(m_Index);
#endif
}

#ifndef _SERVER
void	KNpc::RemoveRes()
{
	m_DataRes.Remove(m_Index);
}


void KNpc::AddSkillEffect(int nSkillID, int nLevel, int nTime)
{
	KSkill * pSkillActive = (KSkill *) g_SkillManager.GetSkill(nSkillID,nLevel);
   if (pSkillActive)
	{
	KMagicAttrib* pMagicAttrib = pSkillActive->GetStateAttribs();
	int pMagicAttribNum = pSkillActive->GetAppendSkillNum();
	SetStateSkillEffect(m_Index, nSkillID, nLevel, pMagicAttrib, pMagicAttribNum, nTime, TRUE);
	}
				
}
#endif
void	KNpc::SetSeries(int nSeries)
{
	if (nSeries >= series_metal && nSeries < series_num)
		m_Series = nSeries;
	else
		m_Series = series_metal;
}
void	KNpc::SetSex(int nSex)
{
 if (nSex != 0 && nSex != 1)
	 return;
    m_nSex = nSex;
}


void KNpc::SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime/* = -1*/, BOOL bOverLook/* = FALSE*/)
{
	if (nLevel <= 0 || nSkillID <= 0) return ;
	//

		int Map = SubWorld[m_SubWorldIndex].m_SubWorldID;

	if ((Map == 209) && nLauncher != m_Index)
	{
		if (nSkillID == 206 || nSkillID == 208 || nSkillID == 93 || nSkillID == 207 || nSkillID == 281 || nSkillID == 292 || nSkillID == 476
		|| nSkillID == 171 || nSkillID == 173 || nSkillID == 178  || nSkillID == 202 || nSkillID == 332)
		{
			return;
		}
	}


	_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);
	//
	_ASSERT(nDataNum < MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
	nDataNum = MAX_SKILL_STATE;
#ifdef _SERVER
	if (IsPlayer() && pData && nDataNum >= 0)
	{
		STATE_EFFECT_SYNC	Sync;
		Sync.ProtocolType = s2c_syncstateeffect;
		Sync.m_dwSkillID = nSkillID;
		Sync.m_nLevel = nLevel;
		Sync.m_nTime = nTime;
		Sync.m_bOverLook = bOverLook;
		memcpy(Sync.m_MagicAttrib, pData, sizeof(KMagicAttrib) * nDataNum);
		Sync.m_wLength = sizeof(Sync) - sizeof(KMagicAttrib) * (MAX_SKILL_STATE - nDataNum) - 1;
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync, Sync.m_wLength + 1);
	}
#endif
	//
	KStateNode* pNode;
	KMagicAttrib* pTemp = NULL;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		if (pNode->m_SkillID == nSkillID) //NÕu ®· cã skill trªn ng­êi
		{
			if (pNode->m_Level == nLevel)
			{
				if(pNode->m_LeftTimeH >= 0)
					pNode->m_LeftTime = pNode->m_LeftTimeH;
				else
					pNode->m_LeftTime = nTime;
				if(nTime == -1)
					pNode->m_LeftTime = nTime;
				pNode->m_bOverLook = bOverLook;
				pNode->m_bTempStateGraphics = FALSE;
				if (nTime == 0) {
					pNode->m_LeftTime = nTime;
					memset(pNode->m_State, 0, sizeof(pNode->m_State));
					pNode->m_Level = 0;
					pNode->m_SkillID = 0;
				}
			}
			else if (pNode->m_Level < nLevel) //xö lý lÊy level skill cao h¬n
			{
				pNode->m_LeftTimeH = nTime;
				pTemp = (KMagicAttrib *)pData;
				for (int i = 0; i < nDataNum; i++)
				{
					ModifyAttrib(nLauncher, &pNode->m_State[i]);
					ModifyAttrib(nLauncher, pTemp);
					pNode->m_State[i].nAttribType = pTemp->nAttribType;
					pNode->m_State[i].nValue[0] = -pTemp->nValue[0];
					pNode->m_State[i].nValue[1] = -pTemp->nValue[1];
					pNode->m_State[i].nValue[2] = -pTemp->nValue[2];
					pTemp++;
				}
			}
			return; //kÕt thóc kh«ng lµm bªn d­íi
		}
		pNode = (KStateNode *)pNode->GetPrev();
	}
	//
	pNode = new KStateNode;	//Thªm míi skill vµo ng­êi
	pNode->m_SkillID = nSkillID;
	pNode->m_Level = nLevel;
	pNode->m_LeftTime = nTime;
	pNode->m_LeftTimeH = 0;
	pNode->m_bOverLook = bOverLook;
	pNode->m_bTempStateGraphics = FALSE;
	//
	if (pOrdinSkill)
		pNode->m_StateGraphics = pOrdinSkill->GetStateSpecailId();
	else
		pNode->m_StateGraphics = 0;
	//
	pTemp = (KMagicAttrib *)pData;
	for (int i = 0; i < nDataNum; i++)
	{
		ModifyAttrib(nLauncher, pTemp);
		pNode->m_State[i].nAttribType = pTemp->nAttribType;
		pNode->m_State[i].nValue[0] = -pTemp->nValue[0];
		pNode->m_State[i].nValue[1] = -pTemp->nValue[1];
		pNode->m_State[i].nValue[2] = -pTemp->nValue[2];
		pTemp++;
	}
	m_StateSkillList.AddTail(pNode);
#ifdef _SERVER	
	if (pNode->m_StateGraphics) 
	{
		UpdateNpcStateInfo();
		//BroadCastState();
	}
#endif
}

BOOL KNpc::ModifyMissleCollsion(BOOL bCollsion)
{
	if (bCollsion)
		return TRUE;

	if (g_RandPercent(m_CurrentPiercePercent))
		return TRUE;
	else
		return FALSE;
}

int KNpc::ModifyMissleLifeTime(int nLifeTime)
{
	if (IsPlayer())
	{
		//return Player[m_PlayerIdx].GetWeapon().GetRange();
		return nLifeTime;
	}
	else
	{
		return nLifeTime;
	}
}

int	KNpc::ModifyMissleSpeed(int nSpeed)
{
	if (m_CurrentSlowMissle)
	{
		return nSpeed / 2;
	}
	return nSpeed;
}

BOOL KNpc::DoManyAttack()
{
	m_ProcessAI = 0;
	
	KSkill * pSkill = (KSkill*) GetActiveSkill();
	if (!pSkill) 
        return FALSE;
	
	if (pSkill->GetChildSkillNum() <= m_SpecialSkillStep) 		
        goto ExitManyAttack;
#ifndef _SERVER
        m_DataRes.SetBlur(TRUE);
#endif
	
	m_Frames.nTotalFrame = pSkill->GetMissleGenerateTime(m_SpecialSkillStep);
	
	int x, y;
	SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
//	m_DesX = x;
//	m_DesY = y;

#ifndef _SERVER
	if (m_nPlayerIdx > 0)
		pSkill->PlayPreCastSound(m_nSex, x ,y);
	if (g_Random(2))
		m_ClientDoing = cdo_attack;
	else 
		m_ClientDoing = cdo_attack1;
#endif

	m_Doing = do_manyattack;
	
	m_Frames.nCurrentFrame = 0;

	return TRUE;

ExitManyAttack:

#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	DoStand();
	m_ProcessAI = 1;
	m_SpecialSkillStep = 0;

	return TRUE;
}

void KNpc::OnManyAttack()
{
	if (WaitForFrame())
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;

		int nPhySkillId =  pSkill->GetChildSkillId();//GetCurActiveWeaponSkill(); Changed 

		if (nPhySkillId > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
			if (pOrdinSkill)
            {
				pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
            }
		}
		m_SpecialSkillStep ++;
		DoManyAttack();

	}	
}

BOOL	KNpc::DoRunAttack()
{
	m_ProcessAI = 0;

	switch(m_SpecialSkillStep)
	{
	case 0:
		m_Frames.nTotalFrame = 20;
		m_ProcessAI = 0;
		
#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);

		if (m_FightMode)
		{
			m_ClientDoing = cdo_fightrun;
		}
		else
		{
			m_ClientDoing = cdo_run;
		}
#endif
		
		if (m_DesX < 0 && m_DesY > 0) 
		{
			int x, y;
			SubWorld[m_SubWorldIndex].Map2Mps
				(
				Npc[m_DesY].m_RegionIndex,
				Npc[m_DesY].m_MapX, 
				Npc[m_DesY].m_MapY, 
				Npc[m_DesY].m_OffX, 
				Npc[m_DesY].m_OffY, 
				&x,
				&y
				);

		m_DesX = x;
		m_DesY = y;
		}

		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 1:
#ifndef _SERVER
		if (m_FightMode)
			m_ClientDoing = cdo_fightstand;
		else if (g_Random(6) != 1)
		{
			m_ClientDoing = cdo_stand;
		}
		else
		{
			m_ClientDoing = cdo_stand1;
		}

		int x, y, tx, ty;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		m_Frames.nTotalFrame = 0;
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_runattack;
		break;

	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}

	m_Frames.nCurrentFrame = 0;
		
	return TRUE;

}

void	KNpc::OnRunAttack()
{
	if (m_SpecialSkillStep == 0)
	{
		OnRun();
		KSkill * pSkill = (KSkill*)GetActiveSkill();
		if (!pSkill) 
            return ;
		
        if (m_Doing == do_stand || (DWORD)m_nCurrentMeleeTime > pSkill->GetMissleGenerateTime(0)) 
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;
			DoRunAttack();
		}
		else
		{
			m_nCurrentMeleeTime ++;
		}

		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
		m_ProcessAI = 0;

			KSkill * pSkill = (KSkill*)GetActiveSkill();
			if (!pSkill) 
                return ;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
				    pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
	}
}

BOOL KNpc::DoJumpAttack()
{
	m_ProcessAI = 0;
	
	switch(m_SpecialSkillStep)
	{
	case 0:
		DoJump();

#ifndef _SERVER
		m_DataRes.SetBlur(TRUE);
		m_ClientDoing = cdo_jump;
#endif
		m_Doing = do_jumpattack;
		break;
		
	case 1:
#ifndef _SERVER
		if (g_Random(2))	
			m_ClientDoing = cdo_attack;
		else
			m_ClientDoing = cdo_attack1;
		int x, y, tx, ty;
		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &x, &y);
		if (m_SkillParam1 == -1)
		{
			Npc[m_SkillParam2].GetMpsPos(&tx, &ty);
		}
		else
		{
			tx = m_SkillParam1;
			ty = m_SkillParam2;
		}
		m_Dir = g_GetDirIndex(x, y, tx, ty);
#endif
		m_Frames.nTotalFrame = m_AttackFrame * 100 / (100 + m_CurrentAttackSpeed);
		m_Frames.nCurrentFrame = 0;
		m_Doing = do_jumpattack;
		break;
		
	case 2:
	case 3:
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
		break;
	}
	
	m_Frames.nCurrentFrame = 0;
	
	return TRUE;
	
}

BOOL KNpc::OnJumpAttack()
{
	if (m_SpecialSkillStep == 0)
	{
		if (!OnJump())
		{
			m_SpecialSkillStep ++;
			m_nCurrentMeleeTime = 0;
			DoJumpAttack();
		}
		m_ProcessAI = 0;
	}
	else if (m_SpecialSkillStep == 1)
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		if (WaitForFrame() &&m_Frames.nTotalFrame != 0)
		{
			DoStand();
			m_ProcessAI = 1;	
		}
		else if (IsReachFrame(ATTACKACTION_EFFECT_PERCENT))
		{
			KSkill * pSkill =(KSkill*) GetActiveSkill();
			if (!pSkill) 
                return FALSE;
			
            int nCurPhySkillId = pSkill->GetChildSkillId();//GetCurActiveWeaponSkill();
			if (nCurPhySkillId > 0)
			{
				KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nCurPhySkillId, pSkill->m_ulLevel, SKILL_SS_Missles);
				if (pOrdinSkill)
                {
					pOrdinSkill->Cast(m_Index, m_SkillParam1, m_SkillParam2);
                }
			}
			FixPos();
			DoStand();
			m_ProcessAI = 1;
			m_SpecialSkillStep = 0;
		}
	}
	else
	{
#ifndef _SERVER
		m_DataRes.SetBlur(FALSE);
#endif
		DoStand();
		m_ProcessAI = 1;
		m_SpecialSkillStep = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::CheckHitTarget(int nAR, int nDf, int nIngore/* = 0*/)
{
	int nDefense = nDf * (100 - nIngore) / 100;
	int nPercent = 0;
	//Question nAr+ nDefense  == 0 ,error!so.....,must modify
	if ((nAR + nDefense) == 0) 
		nPercent = 100;
	else
		nPercent = nAR * 100 / (nAR + nDefense);

	AUTOLOG_EVERY(200, "[E4_HIT_PERCENT] ar=%d df=%d ignore=%d def=%d percent=%d", nAR, nDf, nIngore, nDefense, nPercent);
	if (nPercent > MAX_HIT_PERCENT)
		nPercent = MAX_HIT_PERCENT;

	AUTOLOG_EVERY(1000, "[HIT-PERCENT] AR=%d Df=%d ignore=%d def_hieuluc=%d percent=%d (kep trong %d..%d)", nAR, nDf, nIngore, nDefense, nPercent, MIN_HIT_PERCENT, MAX_HIT_PERCENT);
	if (nPercent < MIN_HIT_PERCENT)
		nPercent = MIN_HIT_PERCENT;
		AUTOLOG_EVERY(1000, "[E2-HITRATE] AR=%d Df=%d ignore=%d defsauignore=%d tile=%d%%", nAR, nDf, nIngore, (nDf * (100 - nIngore) / 100), nPercent);

	BOOL bRet = g_RandPercent(nPercent);
//	g_DebugLog("[ÃŠÃ½Ã–Âµ]AttackRating %d : Defense %d: RandomPercent (%d, %d)", nAR, nDf, nPercent, bRet);
	return bRet;
}

BOOL KNpc::CheckHitTarget2(int nAR, int nDf, int nIngore/* = 0*/) //§é chÝnh x¸c
{
	int nDefense = 0;
	if (nIngore < MAX_PERCENT)
		nDefense = nDf * (MAX_PERCENT - nIngore) / MAX_PERCENT;
	int nPercent = 0;
	if (nAR < 0)
		return FALSE;

	if (nDf < 0)
		nPercent = MAX_HIT_PERCENT;
	else if ((nAR + nDefense) == 0) 
		nPercent = 50;
	else
		nPercent = nAR * MAX_PERCENT / (nAR + nDefense);

	if (nPercent > MAX_HIT_PERCENT + 4)
		nPercent = MAX_HIT_PERCENT;

	AUTOLOG_EVERY(200, "[E4_HIT_PERCENT2] ar=%d df=%d ignore=%d def=%d percent=%d", nAR, nDf, nIngore, nDefense, nPercent);
	if (nPercent < 40)
		nPercent = 40;

	BOOL bRet = g_RandPercent(nPercent);
	return bRet;
}

void KNpc::GetNpcCopyFromTemplate(int nNpcTemplateId, int nLevel, int nSeries)
{
	if (nNpcTemplateId < 0)
		return ;

	

	if (g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries])
		LoadDataFromTemplate(nNpcTemplateId, nLevel, nSeries);
	else
	{
		if (!g_pNpcTemplate[nNpcTemplateId][0][0])
		{
			g_pNpcTemplate[nNpcTemplateId][0][0] = new KNpcTemplate;
			g_pNpcTemplate[nNpcTemplateId][0][0]->InitNpcBaseData(nNpcTemplateId);
			g_pNpcTemplate[nNpcTemplateId][0][0]->m_NpcSettingIdx = nNpcTemplateId;
			g_pNpcTemplate[nNpcTemplateId][0][0]->m_bHaveLoadedFromTemplate = TRUE;
		}

		KLuaScript * pLevelScript = NULL;

#ifdef _SERVER
		pLevelScript = (KLuaScript*)g_GetScript(g_pNpcTemplate[nNpcTemplateId][0][0]->m_dwLevelSettingScript);
		if (pLevelScript == NULL)
			pLevelScript = g_pNpcLevelScript;
#else
		
		KLuaScript LevelScript;
		if (!g_pNpcTemplate[nNpcTemplateId][0][0]->m_szLevelSettingScript[0])//kh«ng cã script lÊy script mÆc ®Þnh
			pLevelScript = g_pNpcLevelScript;
		else
		{
			LevelScript.Init();
			if (!LevelScript.Load(g_pNpcTemplate[nNpcTemplateId][0][0]->m_szLevelSettingScript))
			{
				//_ASSERT(0);
				printf("[error] LevelScript.Load(%s)", g_pNpcTemplate[nNpcTemplateId][0][0]->m_szLevelSettingScript);
				pLevelScript = g_pNpcLevelScript;
			}
			else
				pLevelScript = &LevelScript;
		}
#endif

	
		g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries] = new KNpcTemplate;
		*g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries] = *g_pNpcTemplate[nNpcTemplateId][0][0];
		g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries]->m_nLevel = nLevel;
		g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries]->m_Series = nSeries;
		g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries]->InitNpcLevelData(&g_NpcKindFile, nNpcTemplateId, pLevelScript, nLevel, nSeries);
		g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries]->m_bHaveLoadedFromTemplate = TRUE;
		LoadDataFromTemplate(nNpcTemplateId, nLevel, nSeries);
	}
}

void	KNpc::LoadDataFromTemplate(int nNpcTemplateId, int nLevel, int nSeries)
{
	if (nNpcTemplateId < 0 )
	{
		g_DebugLog("NpcTemplateId < 0");
		return ;
	}
	KNpcTemplate * pNpcTemp = g_pNpcTemplate[nNpcTemplateId][nLevel][nSeries];

	m_HeadImage =	pNpcTemp->m_HeadImage;
	m_CorpseSettingIdx =	pNpcTemp->m_CorpseSettingIdx;
	m_DeathFrame =	pNpcTemp->m_DeathFrame;
	m_WalkFrame =	pNpcTemp->m_WalkFrame;
	m_RunFrame =	pNpcTemp->m_RunFrame;
	m_HurtFrame =	pNpcTemp->m_HurtFrame;
	if(!m_bNoReloadAttr)
		m_WalkSpeed =	pNpcTemp->m_WalkSpeed;
	m_StandFrame =  pNpcTemp->m_StandFrame;
	m_StandFrame1 = pNpcTemp->m_StandFrame1;

#ifndef _SERVER
	m_ArmorType				= pNpcTemp->m_ArmorType;
	m_HelmType				= pNpcTemp->m_HelmType;
	m_WeaponType			= pNpcTemp->m_WeaponType;
#endif

	if(m_Kind != kind_player)
	{
		m_AttackFrame =	pNpcTemp->m_AttackFrame;
		m_CastFrame =	pNpcTemp->m_CastFrame;
		if (!m_bNoReloadAttr)
			m_RunSpeed =	pNpcTemp->m_RunSpeed;
#ifndef _SERVER
		m_HorseType				= pNpcTemp->m_HorseType;
		m_bRideHorse			= pNpcTemp->m_bRideHorse;
		//strcpy(DescName,		  pNpcTemp->DescName);
#endif
		strcpy(Name, pNpcTemp->Name);
		m_Kind = pNpcTemp->m_Kind;
		m_Camp = pNpcTemp->m_Camp;
		m_Series = pNpcTemp->m_Series;
		m_bClientOnly = pNpcTemp->m_bClientOnly;
		m_NpcSettingIdx = pNpcTemp->m_NpcSettingIdx;
		m_nStature		= pNpcTemp->m_nStature;

#ifdef _SERVER	
		m_SkillList		= pNpcTemp->m_SkillList;
		m_AiMode		= pNpcTemp->m_AiMode;
		m_AiAddLifeTime	= 0;

		if (!m_AiSkillRadiusLoadFlag)
		{
			m_AiSkillRadiusLoadFlag = 1;
			int i;
			for (i = 0; i < MAX_AI_PARAM - 1; i ++)
				m_AiParam[i] =	pNpcTemp->m_AiParam[i];

			int		nMaxRadius = 0, nTempRadius;
			KSkill	*pSkill;
			for (i = 1; i < MAX_NPC_USE_SKILL + 1; i++)
			{
				int tSkillId = 53; int tSkillLevel = 1;	//add by phong kiÒu fix bug skill id b»ng 0 vµ skilllevel = 0
				if(m_SkillList.m_Skills[i].SkillId > 0)
				{
					tSkillId = m_SkillList.m_Skills[i].SkillId;
				}
				if(m_SkillList.m_Skills[i].CurrentSkillLevel > 0) 
				{
					tSkillLevel = m_SkillList.m_Skills[i].CurrentSkillLevel;
				}
				pSkill = (KSkill*)g_SkillManager.GetSkill(tSkillId, tSkillLevel); //end add

				//pSkill = (KSkill*)g_SkillManager.GetSkill(m_SkillList.m_Skills[i].SkillId, m_SkillList.m_Skills[i].CurrentSkillLevel); //mÆc ®Þnh src
				if (!pSkill)
					continue;
				nTempRadius = pSkill->GetAttackRadius();
				if (nTempRadius > nMaxRadius)
					nMaxRadius = nTempRadius;
			}
			m_AiParam[MAX_AI_PARAM - 1] = nMaxRadius * nMaxRadius;
		}
		//
		m_FireResistMax			= pNpcTemp->m_FireResistMax;
		m_ColdResistMax			= pNpcTemp->m_ColdResistMax;
		m_LightResistMax		= pNpcTemp->m_LightResistMax;
		m_PoisonResistMax		= pNpcTemp->m_PoisonResistMax;
		m_PhysicsResistMax		= pNpcTemp->m_PhysicsResistMax;
		m_ActiveRadius			= pNpcTemp->m_ActiveRadius;		//B¸n kÝnh ho¹t ®éng
		m_VisionRadius			= pNpcTemp->m_VisionRadius;
		m_AIMAXTime				= pNpcTemp->m_AIMAXTime;
		m_ReviveFrame			= pNpcTemp->m_ReviveFrame;
		m_Experience			= pNpcTemp->m_Experience;
		m_CurrentExperience		= m_Experience;
		if (!m_bNoReloadAttr) {
			m_HitRecover = pNpcTemp->m_HitRecover;
			m_LifeMax = pNpcTemp->m_LifeMax;
			m_LifeReplenish = pNpcTemp->m_LifeReplenish;
		}
		m_AttackRating			= pNpcTemp->m_AttackRating;
		m_Defend				= pNpcTemp->m_Defend;
		//
		m_PhysicsDamage	= pNpcTemp->m_PhysicsDamage;
		m_PhysicsMagic = pNpcTemp->m_PhysicsDamage;
		//
		m_RedLum				= pNpcTemp->m_RedLum;
		m_GreenLum				= pNpcTemp->m_GreenLum;
		m_BlueLum				= pNpcTemp->m_BlueLum;
		m_FireResist			= pNpcTemp->m_FireResist;
		m_ColdResist			= pNpcTemp->m_ColdResist;
		m_LightResist			= pNpcTemp->m_LightResist;
		m_PoisonResist			= pNpcTemp->m_PoisonResist;
		m_PhysicsResist			= pNpcTemp->m_PhysicsResist;
		m_Treasure					= pNpcTemp->m_Treasure;
#endif
		m_Level = pNpcTemp->m_nLevel;
		RestoreNpcBaseInfo();
	}
}

void	KNpc::SetPhysicsDamage(int nMinDamage, int nMaxDamage)
{
	m_PhysicsDamage.nValue[0] = nMinDamage;
	m_PhysicsDamage.nValue[2] = nMaxDamage;
}

void	KNpc::SetBaseAttackRating(int nAttackRating)
{
	m_AttackRating = nAttackRating;
	m_CurrentAttackRating = m_AttackRating;
}

void	KNpc::SetBaseDefence(int nDefence)
{
	m_Defend = nDefence;
	m_CurrentDefend = m_Defend;
}

/*
void	KNpc::SetBaseWalkSpeed(int nSpeed)
{
	m_WalkSpeed = nSpeed;
	m_CurrentWalkSpeed = m_WalkSpeed;
}
*/

/*
void	KNpc::SetBaseRunSpeed(int nSpeed)
{
	m_RunSpeed = nSpeed;
	m_CurrentRunSpeed = m_RunSpeed;
}
*/

#ifdef _SERVER
#define	LOSE_EXP_SCALE		10
void KNpc::DeathPunish(int nMode, int nBelongPlayer)
{
	if (IsPlayer())
	{
		if (nMode == enumDEATH_MODE_NPC_KILL)
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend, TRUE);//#mµi mßn  phßng thñ
   
			if (Player[m_nPlayerIdx].m_nExp >= 0) //fix by phong kiÒu nh©n vËt chÕt mµ céng thªm exp v× v­ît miÒn kiÓu int
			{
				double nSubExp = 0;
				if (m_Level <= 10)
					nSubExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)/5;
				else if (m_Level <= 50)
					nSubExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)/10;
				else if (m_Level < 80)
					nSubExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level)/15;
				else if (m_Level <= 90)
					nSubExp =50000;
				else if (m_Level > 90)
					nSubExp = 100000;	
				Player[m_nPlayerIdx].DirectAddExp( -nSubExp );
			}

			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney() / 2;
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);

				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf = 0;

				if (nMoney / 2 > 0)
					PlayerDeadCreateMoneyObj(nMoney / 2);
			}
		}

		else if (nMode == enumDEATH_MODE_PLAYER_NO_PUNISH)
		{
			return;
		}
		else if (nMode == enumDEATH_MODE_PKBATTLE_PUNISH)
		{
			return;
		}

		else //if (nMode == enumDEATH_MODE_PLAYER_PUNISH)
		{
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend, TRUE);//#mÂµi mÃŸn  phÃŸng thÃ±
			int		nPKValue;
			nPKValue = Player[this->m_nPlayerIdx].m_cPK.GetPKValue();
			if (nPKValue < 0)
				nPKValue = 0;
			if (nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
				nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

			double		nLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level);
			double 	nExpTru = 0;
				if (m_Level <= 10)
					nExpTru = (nLevelExp/5) * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP/100;
				else if (m_Level <= 50)
					nExpTru = (nLevelExp/10) * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP/100;
				else if (m_Level < 80)
					nExpTru = (nLevelExp/15) * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP/100;
				else if (m_Level <= 90)
					nExpTru =(nLevelExp/20) * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP/100;
				else if (m_Level > 90)
					nExpTru = (nLevelExp/25) * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP/100;

			if (Player[m_nPlayerIdx].m_nExp	< -1600000000)
			{
				nExpTru = 0;
			}
			Player[m_nPlayerIdx].DirectAddExp( -nExpTru ); //#pk mat diem kn
//xxx		
			int nMoney = Player[m_nPlayerIdx].m_ItemList.GetEquipmentMoney() / 100 * PlayerSet.m_sPKPunishParam[nPKValue].m_nMoney;
			if (nMoney > 0)
			{
				Player[m_nPlayerIdx].m_ItemList.CostMoney(nMoney);

				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_DEC_MONEY;
				sMsg.m_lpBuf = (void *)(nMoney);
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf = 0;
				
				if (nMoney / 2 > 0)
					PlayerDeadCreateMoneyObj(nMoney / 2);
			}

		/*	Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(PlayerSet.m_sPKPunishParam[nPKValue].m_nItem);	//#roi item khi bi chet co pk

			if (g_Random(100) < PlayerSet.m_sPKPunishParam[nPKValue].m_nEquip) //#roi item khi bi chet co pk
			{
				Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
			}*/
       

				if (nPKValue == 10)
				{	
					if (Player[m_nPlayerIdx].m_nExp < (-Player[m_nPlayerIdx].m_nNextLevelExp/4))
					{
						ChangeWorld(208, 1787 * 32, 3058 * 32);
						SetFightMode(fight_none);
					}
				}
			Player[m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nBeKilledAddPKValue);
			if (m_nLastDamageIdx)
			{
				if (Npc[m_nLastDamageIdx].IsPlayer())
				{
					KPlayerChat::MakeEnemy(Name, Npc[m_nLastDamageIdx].Name);
				}
			}
		}
	}
	else if (nBelongPlayer > 0 && m_pDropRate && !g_NotAddNpcNormal) //sÃ¶ dÃ´ng khi load npc normal trong file pak hoac folder load npc bÂ»ng script thÃ— khÂ«ng sÃ¶ dÃ´ng
	{
		for (int i = 0; i < m_CurrentTreasure; i++)
		{
			if (g_RandPercent(m_pDropRate->nMoneyRate))
			{
				LoseMoney(nBelongPlayer);
			}
			else
			{
				LoseSingleItem(nBelongPlayer);
			}
		}
	}
}

void KNpc::RestoreLiveData(int nDamage) 
{             

}     

void KNpc::RestoreDamage2Life(int nDamage) //edit by phong kieu hut mau
{ 
	if((nDamage*m_CurrentLifeStolen/100) > 0)
	{
    m_CurrentLife = m_CurrentLife + (nDamage*m_CurrentLifeStolen/100); 
    if (m_CurrentLife >= m_CurrentLifeMax) 
        m_CurrentLife = m_CurrentLifeMax; 
	}
}

void KNpc::RestoreDamage2Mana(int nDamage) //edit by phong kieu hut mana
{
	if( (nDamage*m_CurrentManaStolen/100) > 0)
	{
	m_CurrentMana = m_CurrentMana + (nDamage*m_CurrentManaStolen/100); 
	if (m_CurrentMana >= m_CurrentManaMax) 
        m_CurrentMana = m_CurrentManaMax; 
	}
}

void KNpc::RestoreDamage2Stamina(int nDamage)//edit by phong kieu hut the luc
{	
	if( (nDamage*m_CurrentStaminaStolen/100) > 0)
	{
	m_CurrentStamina = m_CurrentStamina + (nDamage*m_CurrentStaminaStolen/100);
	if(m_CurrentStamina >= m_CurrentStaminaMax)
		m_CurrentStamina = m_CurrentStaminaMax;
	}
}

void	KNpc::PlayerDeadCreateMoneyObj(int nMoneyNum)
{
	int		nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoneyNum);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(-1);
	}
}

void KNpc::LoseMoney(int nBelongPlayer)//danh quai roi tien
{
	int nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	int nMoney = m_CurrentExperience * m_pDropRate->nMoneyScale / 100; //tien quai roi mac dinh diem kinh nghiem quai nhan  // g_MoneyRate trong setting
	
	if(g_MoneyRate)
		nMoney = nMoney * g_MoneyRate; //x2 money trong file config

	if (nMoney <= 0)
		return;

	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoney);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		if (nBelongPlayer > 0)
			Object[nObjIdx].SetItemBelong(nBelongPlayer);
		else
			Object[nObjIdx].SetItemBelong(-1);
	}
}

int KNpc::DropItemFromLuaScript(int nBelongPlayer, int nGenre, int nDetail, int nParticular, int nSeries, int nLevel, int nLuck, int* pnMagicLevel, int nTimeBelong)
{
	int nXOpt = ItemSet.genXOpt(nLuck);
	int nIdx = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(), 0, 1,0,0,0,0,0,0,0,0,nXOpt);
	if (nIdx <= 0)
		return -1;

	int		nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	KObjItemInfo sInfo;
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	char szNameTemp[OBJ_NAME_LENGHT];
	if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
	{
		sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
		strcpy(sInfo.m_szName, szNameTemp);
	}
	else
		strcpy(sInfo.m_szName, Item[nIdx].GetName());
	sInfo.m_nColorID = Item[nIdx].GetColorItem();
	sInfo.m_nGenre = Item[nIdx].GetGenre();
	sInfo.m_nDetailType = Item[nIdx].GetDetailType();
	sInfo.m_nParticularType = Item[nIdx].GetParticular();
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	sInfo.m_dwNpcId1 = 0;
	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
		{
			if(nTimeBelong > 0)
				Object[nObj].SetItemBelong2(nBelongPlayer, nTimeBelong);
			else
				Object[nObj].SetItemBelong(nBelongPlayer);
		}
		else
			Object[nObj].SetItemBelong(-1);
	}
	return nIdx;
}

//DropPUBGItemFromLuaScript
int KNpc::DropPUBGItemFromLuaScript(int nKind, int nGenre, int nDetail, int nParticular, int nSeries, int nLevel, int nLuck, int* pnMagicLevel, int nTimeBelong)
{
	if (nKind != NATURE_GOLD && nKind != NATURE_PLATINA) //only drop gold or platina
		return -1;
	int nXOpt = ItemSet.genXOpt(nLuck);
	int nIdx = ItemSet.Add(nKind, nGenre, nSeries, nLevel, nLuck, nDetail, nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(), 0, nXOpt);
	/*Add(IN int nItemNature, int nItemGenre, int nSeries,
	int nLevel, int nLuck, int nDetailType,
		int nParticularType, int* pnMagicLevel, int nVersion, UINT nRandomSeed, int nMaxOptMultiply)*/
	if (nIdx <= 0)
		return -1;

	int		nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);

	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY,
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	KObjItemInfo sInfo;
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	char szNameTemp[OBJ_NAME_LENGHT];
	if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
	{
		sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
		strcpy(sInfo.m_szName, szNameTemp);
	}
	else
		strcpy(sInfo.m_szName, Item[nIdx].GetName());
	sInfo.m_nColorID = Item[nIdx].GetColorItem();
	sInfo.m_nGenre = Item[nIdx].GetGenre();
	sInfo.m_nDetailType = Item[nIdx].GetDetailType();
	sInfo.m_nParticularType = Item[nIdx].GetParticular();
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	sInfo.m_dwNpcId1 = 0;
	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		Object[nObj].SetItemBelong(-1);
	}
	return nIdx;
}
void KNpc::LoseSingleItem(int nBelongPlayer)//danh quai roi do roi item
{
	int MAX_MAGIC_LEVEL = 6; //tÂ¨ng lÂµm 4 giai Â®oÂ¹n 6->8->9->10
	if (!m_pDropRate)
		return;

	if (m_pDropRate->nMaxItemLevelScale <= 0 || m_pDropRate->nMinItemLevelScale <= 0)
		return;

	int nRand = g_Random(m_pDropRate->nMaxRandRate);
	int nCheckRand = 0;	

	int i, j;
	for (i = 0; i < m_pDropRate->nCount; i++)
	{
		if (nRand >= nCheckRand && nRand < nCheckRand + m_pDropRate->pItemParam[i].nRate)
		{
			break;
		}
		nCheckRand += m_pDropRate->pItemParam[i].nRate;
	}

	if (i == m_pDropRate->nCount)
		return;

	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel, pnMagicLevel[6];
	nGenre = m_pDropRate->pItemParam[i].nGenre;
	nDetail = m_pDropRate->pItemParam[i].nDetailType;
	nParticular = m_pDropRate->pItemParam[i].nParticulType;
	nSeries = m_Series;
	nLuck = Player[nBelongPlayer].m_nCurLucky;

	int nMaxLevel = m_Level / m_pDropRate->nMaxItemLevelScale;
	int nMinLevel = m_Level / m_pDropRate->nMinItemLevelScale;

	if (nMaxLevel > m_pDropRate->nMaxItemLevel)
		nMaxLevel = m_pDropRate->nMaxItemLevel;

	if (nMinLevel < m_pDropRate->nMinItemLevel)
		nMinLevel = m_pDropRate->nMinItemLevel;

	if (nMaxLevel < m_pDropRate->nMinItemLevel)
		nMaxLevel = m_pDropRate->nMinItemLevel;
	
	if (nMinLevel > m_pDropRate->nMaxItemLevel)
		nMinLevel = m_pDropRate->nMaxItemLevel;

	if (nMaxLevel < nMinLevel)
	{
		int nTemp = nMinLevel;
		nMinLevel = nMaxLevel;
		nMaxLevel = nTemp;
	}
	
	nLevel = g_Random(nMaxLevel - nMinLevel) + nMinLevel;
	if(nLevel > 10) //max level item lÂµ 10
	{
		nLevel = 10;
	}
	//add by phong kiÃ’u rÂ¬i Â®Ã¥ xanh theo magiclevel
	int nMagicLevel = 0;
	if(nLuck >= 0 && nLuck <= 10)
	{
		nMagicLevel = g_Random(3);
	}
	else if (nLuck > 10 && nLuck <= 20)
	{
		nMagicLevel = g_Random(4);
		if(nMagicLevel < 3) nMagicLevel = 3;
	}
	else if (nLuck > 20 && nLuck <= 30)
	{
		nMagicLevel = g_Random(5);
		if(nMagicLevel < 4) nMagicLevel = 4;
	}
	else if (nLuck > 30 && nLuck <= 40)
	{
		nMagicLevel = g_Random(6);
		if(nMagicLevel < 5) nMagicLevel = 5;
	}
	else if (nLuck > 40 && nLuck <= 50)
	{
		nMagicLevel = g_Random(7);
		if(nMagicLevel < 6) nMagicLevel = 6;
	}
	else if (nLuck > 50 && nLuck <= 60)
	{
		nMagicLevel = g_Random(8);
		if(nMagicLevel < 7) nMagicLevel = 7;
	}
	else if (nLuck > 60)
	{
		nMagicLevel = g_Random(9);
		if(nMagicLevel < 8) nMagicLevel = 8;
	}
	//
	if(nMagicLevel > MAX_MAGIC_LEVEL)
	{
		nMagicLevel = MAX_MAGIC_LEVEL;
	}
	//
	BOOL bSkip = FALSE;
	for (j = 0; j < 6; j++)
	{
		if (!bSkip)
		{
			 if (g_Random(2 + nLuck))
			 {
				 pnMagicLevel[j] = nMagicLevel;
			 }
			 else
			 {
				 pnMagicLevel[j] = 0;
				 bSkip = TRUE;
			 }
		}
		else
		{
			pnMagicLevel[j] = 0;
		}
	}
	int nXOpt = ItemSet.genXOpt(nLuck);
	int nIdx = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(), 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, nXOpt);

	if (nIdx <= 0)
		return;

	int	nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = m_SubWorldIndex;
	SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);

	int nObj;
	KObjItemInfo sInfo;
	sInfo.m_nItemID = nIdx;
	sInfo.m_nItemWidth = Item[nIdx].GetWidth();
	sInfo.m_nItemHeight = Item[nIdx].GetHeight();
	sInfo.m_nMoneyNum = 0;
	char szNameTemp[OBJ_NAME_LENGHT];
	if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
	{
		sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
		strcpy(sInfo.m_szName, szNameTemp);
	}
	else
		strcpy(sInfo.m_szName, Item[nIdx].GetName());
	sInfo.m_nColorID = Item[nIdx].GetColorItem();
	sInfo.m_nGenre = Item[nIdx].GetGenre();
	sInfo.m_nDetailType = Item[nIdx].GetDetailType();
	sInfo.m_nParticularType = Item[nIdx].GetParticular();
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	sInfo.m_dwNpcId1 = 0;
	nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
	if (nObj == -1)
	{
		ItemSet.Remove(nIdx);
	}
	else
	{
		if (nBelongPlayer > 0)
			Object[nObj].SetItemBelong(nBelongPlayer);
		else
			Object[nObj].SetItemBelong(-1);
	}
}

void KNpc::Revive()
{	
	//RestoreLiveData();
	RestoreNpcBaseInfo();
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(m_OriginX, m_OriginY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	if (m_RegionIndex < 0)
		return;
	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
#ifdef _SERVER
	//SubWorld[m_SubWorldIndex].m_WorldMessage.Send(GWM_NPC_CHANGE_REGION, VOID_REGION, nRegion, m_Index);
	SubWorld[m_SubWorldIndex].NpcChangeRegion(VOID_REGION, nRegion, m_Index);	// spe 03/06/28
#else
	SubWorld[0].NpcChangeRegion(VOID_REGION, SubWorld[0].m_Region[nRegion].m_RegionID, m_Index);
#endif
	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;
	m_AiAddLifeTime = 0;
#ifdef _SERVER
	this->m_cGold.RandChangeGold(0, 0);
	if (!IsPlayer())
	{
		if(m_SkillList.m_Skills[5].SkillId)
		{
			SetAuraSkill(m_SkillList.m_Skills[5].SkillId); 
		}
		if (ActionScript[0])
		{
			ExecuteScript(ActionScript, "OnRevive", m_Index);	// monster #idx cÃ±a npc khi hÃ¥i sinh
		}
	}
#endif
}

void KNpc::RestoreLiveData()
{

}
#endif

#ifdef	_SERVER

void	KNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize, int nLimit)
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	static const POINT	POff[8] = 
	{
		{0, 32},
		{-16, 32},
		{-16, 0},
		{-16, -32},
		{0, -32},
		{16, -32},
		{16, 0},
		{16, 32},
	};
	// [S11] nLimit>0 = goi su kien mot-lan (chet/go) mien ngan sach; mac dinh giu 100.
	int nMaxCount = (nLimit > 0) ? nLimit : MAX_BROADCAST_COUNT;
	SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX, m_MapY);
	for (int i= 0; i < 8; i++)
	{
		if (SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i] < 0)
			continue;
		SubWorld[m_SubWorldIndex].m_Region[SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i]].BroadCast(pBuffer, dwSize, nMaxCount, m_MapX - POff[i].x, m_MapY - POff[i].y);
	}
}
#endif

#ifdef	_SERVER

int	KNpc::FindAroundPlayer(const char* Name)//add by phong kiÃ’u using vÃ‹n tiÂªu
{
	int nNpc = 0;
	if (Name[0] == 0 || m_RegionIndex < 0)
		return nNpc;
	nNpc = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].SearchNpcName(Name);
	if (nNpc)
		return nNpc;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nNpc = SubWorld[m_SubWorldIndex].m_Region[nRegionNo].SearchNpcName(Name);
		if (nNpc)
			return nNpc;
	}
	return nNpc;
}

int		KNpc::DeathCalcPKValue(int nKiller)
{
	if (nKiller <= 0 || nKiller >= MAX_NPC)
		return enumDEATH_MODE_NPC_KILL;
	
	if (m_nCurPKPunishState == enumDEATH_MODE_PKBATTLE_PUNISH)
		return enumDEATH_MODE_PKBATTLE_PUNISH;

	if (this->m_Kind != kind_player || Npc[nKiller].m_Kind != kind_player || !m_FightMode || m_btSimCityBot || Npc[nKiller].m_btSimCityBot)
		return enumDEATH_MODE_NPC_KILL;

	if (Player[m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nKiller].m_nPlayerIdx)
		return enumDEATH_MODE_PLAYER_NO_PUNISH;

	if (Player[m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING && Player[m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nKiller].m_nPlayerIdx)
	{
		if (Player[Npc[nKiller].m_nPlayerIdx].m_cPK.IsEnmityPKLauncher())
			Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nEnmityAddPKValue);//cuu sat len pk 2
		return enumDEATH_MODE_PLAYER_PUNISH;
	}
	if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKNormal && Player[Npc[nKiller].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder) //do sat giet nguoi
	{
		Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(1);//do sat len pk 1
		return enumDEATH_MODE_PLAYER_PUNISH;		
	}
	
	if (m_Level <= 50 && Npc[nKiller].m_Level * 2 >= m_Level * 3)//(HLv:LLv >= 3:2)
	{
		if (Player[m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKNormal)
		{
			if (Npc[nKiller].m_CurrentCamp == camp_free)
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nKillerPKFactionAddPKValue);
			else
				Player[Npc[nKiller].m_nPlayerIdx].m_cPK.AddPKValue(NpcSet.m_nFactionPKFactionAddPKValue);
		}
		return enumDEATH_MODE_PLAYER_PUNISH;
	}

	return enumDEATH_MODE_PLAYER_PUNISH;
}
#endif

#ifdef	_SERVER

BOOL	KNpc::CheckPlayerAround(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || m_RegionIndex < 0)
		return FALSE;
	if (SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].CheckPlayerIn(nPlayerIdx))
		return TRUE;
	int		nRegionNo;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		if (SubWorld[m_SubWorldIndex].m_Region[nRegionNo].CheckPlayerIn(nPlayerIdx))
			return TRUE;
	}
	return FALSE;
}
int KNpc::GetMagicLevel(int nLuckySoDong, int mm) 
{

    if (nLuckySoDong < 2) 
	{
        if (mm < 15) return 3;
        if (mm < 45) return 2;
        if (mm < 75) return 1;
        return 0;
    } 
	else if (nLuckySoDong < 6)
	{
        if (mm < 5) return 4;
        if (mm < 20) return 3;
        if (mm < 53) return 2;
        if (mm < 78) return 1;
        return 0;
    } 
	else if (nLuckySoDong < 10) 
	{
        if (mm < 10) return 4;
        if (mm < 25) return 3;
        if (mm < 62) return 2;
        if (mm < 87) return 2;
        return 1;
    } 
	else if (nLuckySoDong < 14) 
	{
        if (mm < 5) return 5;
        if (mm < 20) return 4;
        if (mm < 35) return 3;
        if (mm < 75) return 2;
        return 2;
    } else if (nLuckySoDong < 18) {
        if (mm < 10) return 5;
        if (mm < 25) return 4;
        if (mm < 50) return 3;
        if (mm < 90) return 2;
        return 2;
    } else if (nLuckySoDong < 22) {
        if (mm < 10) return 5;
        if (mm < 30) return 4;
        if (mm < 55) return 3;
        if (mm < 90) return 2;
        return 1;
    } else if (nLuckySoDong < 26) {
        if (mm < 5) return 6;
        if (mm < 15) return 5;
        if (mm < 35) return 4;
        if (mm < 60) return 3;
        if (mm < 90) return 2;
        return 1;
    } else if (nLuckySoDong < 30) {
        if (mm < 5) return 6;
        if (mm < 20) return 5;
        if (mm < 45) return 4;
        if (mm < 80) return 3;
        return 2;
    } else if (nLuckySoDong < 34) {
        if (mm < 7) return 6;
        if (mm < 27) return 5;
        if (mm < 52) return 4;
        if (mm < 87) return 3;
        return 2;
    } else {
        if (mm < 15) return 6;
        if (mm < 30) return 5;
        if (mm < 65) return 4;
        return 4;
    }
}

// Helper function to get the level magic based on total luck
int KNpc::GetLevelMagic(int nTotalLucky, int mmopt) 
{
    if (nTotalLucky < 4) 
	{
        if (mmopt < 10) return 1;
        if (mmopt < 30) return 2;
        if (mmopt < 70) return 3;
        if (mmopt < 90) return 4;
        return 5;
    } 
	else if (nTotalLucky < 8) 
	{
        if (mmopt < 5) return 1;
        if (mmopt < 25) return 2;
        if (mmopt < 65) return 3;
        if (mmopt < 85) return 4;
        if (mmopt < 95) return 5;
        return 6;
    } else if (nTotalLucky < 12) {
        if (mmopt < 10) return 2;
        if (mmopt < 40) return 3;
        if (mmopt < 75) return 4;
        if (mmopt < 90) return 5;
        return 6;
    } else if (nTotalLucky < 16) {
        if (mmopt < 5) return 2;
        if (mmopt < 25) return 3;
        if (mmopt < 60) return 4;
        if (mmopt < 80) return 5;
        if (mmopt < 95) return 6;
        return 6;
    } else if (nTotalLucky < 20) {
        if (mmopt < 10) return 3;
        if (mmopt < 30) return 4;
        if (mmopt < 50) return 5;
        if (mmopt < 75) return 6;
        return 6;
    } else if (nTotalLucky < 24) {
        if (mmopt < 5) return 3;
        if (mmopt < 20) return 4;
        if (mmopt < 40) return 5;
        if (mmopt < 60) return 6;
        if (mmopt < 80) return 7;
        return 8;
    } else if (nTotalLucky < 28) {
        if (mmopt < 15) return 4;
        if (mmopt < 40) return 5;
        if (mmopt < 70) return 6;
        if (mmopt < 85) return 7;
        if (mmopt < 95) return 8;
        return 9;
    } else if (nTotalLucky < 32) {
        if (mmopt < 5) return 4;
        if (mmopt < 20) return 5;
        if (mmopt < 40) return 6;
        if (mmopt < 60) return 7;
        if (mmopt < 80) return 8;
        if (mmopt < 97) return 9;
        return 9;
    } else if (nTotalLucky < 36) {
        if (mmopt < 5) return 5;
        if (mmopt < 25) return 6;
        if (mmopt < 55) return 7;
        if (mmopt < 75) return 8;
        if (mmopt < 96) return 9;
        return 9;
    } else if (nTotalLucky < 40) {
        if (mmopt < 5) return 5;
        if (mmopt < 30) return 6;
        if (mmopt < 50) return 7;
        if (mmopt < 70) return 8;
        if (mmopt < 95) return 9;
        return 10;
    } else if (nTotalLucky < 44) {
        if (mmopt < 15) return 6;
        if (mmopt < 35) return 7;
        if (mmopt < 55) return 8;
        if (mmopt < 94) return 9;
        return 10;
    } else if (nTotalLucky < 48) {
        if (mmopt < 10) return 6;
        if (mmopt < 30) return 7;
        if (mmopt < 50) return 8;
        if (mmopt < 90) return 9;
        return 10;
    } else if (nTotalLucky < 52) {
        if (mmopt < 5) return 6;
        if (mmopt < 25) return 7;
        if (mmopt < 45) return 8;
        if (mmopt < 88) return 9;
        return 10;
    } else if (nTotalLucky < 56) {
        if (mmopt < 20) return 7;
        if (mmopt < 40) return 8;
        if (mmopt < 85) return 9;
        return 10;
    } else if (nTotalLucky < 60) {
        if (mmopt < 15) return 7;
        if (mmopt < 35) return 8;
        if (mmopt < 80) return 9;
        return 10;
    } else if (nTotalLucky == 100) {
        if (mmopt < 40) return 9;
        return 10;
    } else {
        if (mmopt < 20) return 6;
        if (mmopt < 40) return 7;
        if (mmopt < 60) return 8;
        if (mmopt < 90) return 9;
        return 10;
    }
}

void KNpc::DropRateItem(int nCount, const char* pszFileName, int nUnknown, int nItemLevel, int nItemSeries, int nBelongIdx)
{
	if (nCount <= 0 || nBelongIdx <= 0 || nBelongIdx >= MAX_PLAYER || !pszFileName || !pszFileName[0])
		return;
	// KNpcTemplate::GenNpcDropRate(&Npc[m_nNpcIdx],SubWorld[.m_SubWorldIndex].szNormalDropRate);
	KItemDropRate* m_pDropRate = KNpcTemplate::UpdateDropRate((char*)pszFileName);
	if (!m_pDropRate || m_pDropRate->nMaxItemLevelScale <= 0 || m_pDropRate->nMinItemLevelScale <= 0)
		return;

	int nRand, nCheckRand = 0;
	int nGenre, nSeries, nLuck, nDetail, nParticular, nLevel, nLevelMagic, pnMagicLevel[MAX_ITEM_MAGICLEVEL];
	int nX, nY;

	POINT ptLocal;
	KMapPos Pos;
	int nObj;
	KObjItemInfo sInfo;
	int j = 0;
	GetMpsPos(&nX, &nY);

	// [VA5LOI 29/08] CHONG TREO MAY CHU. `j++` chi nam trong nhanh trung
	// (cuoi ham), nen neu TONG RandRate = 0 thi khong muc nao trung duoc
	// va vong while duoi day KHONG BAO GIO THOAT. Co tep .ini nhu the
	// that: npcdroprate_fenglindubei.ini khai Count=86 ma khong co muc
	// [n] nao (KNpcTemplate.cpp:41-44 cho moi truong mac dinh 0).
	int nTongRate = 0;
	for (int nT = 0; nT < m_pDropRate->nCount; nT++)
		nTongRate += m_pDropRate->pItemParam[nT].nRate;
	// [NOICHOT 29/08] chi chan dung cai gay TREO. g_Random(0) tra ve 0 chu
	// khong chia cho 0 (Engine\Src\KRandom.cpp), nen tep .ini co
	// RandRange = 0 ma tong rate > 0 van rot do binh thuong (luon trung
	// muc dau). Chan ca truong hop do la DOI HANH VI, khong phai chong treo.
	if (nTongRate <= 0 || m_pDropRate->nMaxRandRate < 0)
		return;
	// [PBLUA 29/08] Tran so lan roll, dat theo SO DO THAT (da quet ca 49
	// tep settings\droprate\**\*.ini): tep te nhat la
	// npcdroprate110.ini voi RandRange 400.000 / tong rate 4.300 = 93
	// lan roll ky vong cho MOI mon. Voi so mon nhieu nhat ma script yeu
	// cau, ky vong xau nhat van duoi 2.000 lan. Tran 2.000.000 gap hon
	// mot nghin lan con so do, nen du lieu dung KHONG BAO GIO cham toi
	// va ti le rot do giu nguyen y het truoc khi va.
	int nRollConLai = 2000000;

	while (j < nCount)
	{
		if (--nRollConLai < 0)
			break;
		nRand = g_Random(m_pDropRate->nMaxRandRate);
		nCheckRand = 0;

		for (int i = 0; i < m_pDropRate->nCount; i++)
		{
			if (nRand >= nCheckRand && nRand < nCheckRand + m_pDropRate->pItemParam[i].nRate)
			{
				nGenre = m_pDropRate->pItemParam[i].nGenre;
				nDetail = m_pDropRate->pItemParam[i].nDetailType;
				nParticular = m_pDropRate->pItemParam[i].nParticulType;
				nSeries = nItemSeries;
				nLuck = Player[nBelongIdx].m_nCurLucky;

				int nMaxLevel = max(m_pDropRate->nMaxItemLevel, m_pDropRate->nMinItemLevel);
				int nMinLevel = min(m_pDropRate->nMaxItemLevel, m_pDropRate->nMinItemLevel);
				int nMaxLevelMagic = max(m_pDropRate->nMaxItemLevelScale, m_pDropRate->nMinItemLevelScale);
				int nMinLevelMagic = min(m_pDropRate->nMaxItemLevelScale, m_pDropRate->nMinItemLevelScale);

				if (g_RandPercent(nLuck / 10))
					nLevel = nMaxLevel;
				else if (g_RandPercent(nLuck / 2 + 50))
					nLevel = g_Random(nMaxLevel + 1 - nMinLevel) + nMinLevel;
				else
					nLevel = nMinLevel;

				int nLuckySoDong = 0;
				int nTotalLucky = 0;
				int nSlMagicTT = 0;
				if (this->m_cGold.GetGoldType() == 1) // boss xanh
				{
					nLuckySoDong = nLuck + 5;
					nTotalLucky = nLuck + 10;
				}
				else if (this->m_cGold.m_nGoldType >= defNPC_GOLD_TYE) // boss vÃ ng
				{
					nLuckySoDong = nLuck + 10;
					nTotalLucky = nLuck + 20;
				}
				else
					nLuckySoDong = nTotalLucky = nLuck;
					
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentExpSkillsVip == 2)
					{
					nLuckySoDong = nLuck + 150;
					nTotalLucky = nLuck + 200;
					}
				else
					nLuckySoDong = nTotalLucky = nLuck;

				printf("Debug: Lucky: %d - Dong: %d\n", nTotalLucky, nLuckySoDong);

				int mm = g_Random(100);  // Ngau nhien 0 - 99
				if (nLuckySoDong < 2) // Neu may man >= 0, < 2
				{
					if (mm < 15)  // 15%
						nSlMagicTT = 3;   // 3 dong
					else if (mm < 45)  // 30%
						nSlMagicTT = 2; // 2 dong
					else if (mm < 75)  // 30%
						nSlMagicTT = 1; // 1 dong
					else  // 25%
						nSlMagicTT = 0; // 0 dong
				}
				else if (nLuckySoDong < 6)
				{
					if (mm < 5)
						nSlMagicTT = 4;
					else if (mm < 20)
						nSlMagicTT = 3;
					else if (mm < 53)
						nSlMagicTT = 2;
					else if (mm < 78)
						nSlMagicTT = 1;
					else
						nSlMagicTT = 0;
				}
				else if (nLuckySoDong < 10)
				{
					if (mm < 10)
						nSlMagicTT = 4;
					else if (mm < 25)
						nSlMagicTT = 3;
					else if (mm < 62)
						nSlMagicTT = 2;
					else if (mm < 87)
						nSlMagicTT = 2;
					else
						nSlMagicTT = 1;
				}
				else if (nLuckySoDong < 14)
				{
					if (mm < 5)
						nSlMagicTT = 5;
					else if (mm < 20)
						nSlMagicTT = 4;
					else if (mm < 35)
						nSlMagicTT = 3;
					else if (mm < 75)
						nSlMagicTT = 2;
					else
						nSlMagicTT = 1;
				}
				else if (nLuckySoDong < 18)
				{
					if (mm < 10)
						nSlMagicTT = 5;
					else if (mm < 25)
						nSlMagicTT = 4;
					else if (mm < 50)
						nSlMagicTT = 3;
					else if (mm < 90)
						nSlMagicTT = 2;
					else
						nSlMagicTT = 1;
				}
				else if (nLuckySoDong < 22)
				{
					if (mm < 10)
						nSlMagicTT = 5;
					else if (mm < 30)
						nSlMagicTT = 4;
					else if (mm < 55)
						nSlMagicTT = 3;
					else if (mm < 90)
						nSlMagicTT = 2;
					else
						nSlMagicTT = 1;
				}
				else if (nLuckySoDong < 26)
				{
					if (mm < 5)
						nSlMagicTT = 6;
					else if (mm < 15)
						nSlMagicTT = 5;
					else if (mm < 35)
						nSlMagicTT = 4;
					else if (mm < 60)
						nSlMagicTT = 3;
					else if (mm < 90)
						nSlMagicTT = 2;
					else
						nSlMagicTT = 1;
				}
				else if (nLuckySoDong < 30)
				{
					if (mm < 5)
						nSlMagicTT = 6;
					else if (mm < 20)
						nSlMagicTT = 5;
					else if (mm < 45)
						nSlMagicTT = 4;
					else if (mm < 80)
						nSlMagicTT = 3;
					else
						nSlMagicTT = 2;
				}
				else if (nLuckySoDong < 34)
				{
					if (mm < 7)
						nSlMagicTT = 6;
					else if (mm < 27)
						nSlMagicTT = 5;
					else if (mm < 52)
						nSlMagicTT = 4;
					else if (mm < 87)
						nSlMagicTT = 3;
					else
						nSlMagicTT = 2;
				}
				else if (nLuckySoDong >= 100)
				{
					if (mm < 30)
						nSlMagicTT = 5;
					else
						nSlMagicTT = 6;
				}
				else
				{
					if (mm < 15)
						nSlMagicTT = 6;
					else if (mm < 30)
						nSlMagicTT = 5;
					else if (mm < 65)
						nSlMagicTT = 4;
					else
						nSlMagicTT = 4;
				}


				BOOL	bSkip = FALSE;
				for (int j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
				{
					if (!bSkip)
					{

						if (j < nSlMagicTT)
						{



							int mmopt = g_Random(100);
							if (nTotalLucky < 4) // Neu may man < 4
							{
								if (mmopt < 10) // 10%
									nLevelMagic = 1;  // Level opt = 1
								else if (mmopt < 30) // 20%
									nLevelMagic = 2;
								else if (mmopt < 70) // 40%
									nLevelMagic = 3;
								else if (mmopt < 90) // 20%
									nLevelMagic = 4;
								else	                        // 10%
									nLevelMagic = 5;
							}
							else if (nTotalLucky < 8)
							{
								if (mmopt < 5)
									nLevelMagic = 1;
								else if (mmopt < 25)
									nLevelMagic = 2;
								else if (mmopt < 65)
									nLevelMagic = 3;
								else if (mmopt < 85)
									nLevelMagic = 4;
								else if (mmopt < 95)
									nLevelMagic = 5;
								else
									nLevelMagic = 6;
							}
							else if (nTotalLucky < 12)
							{
								if (mmopt < 10)
									nLevelMagic = 2;
								else if (mmopt < 40)
									nLevelMagic = 3;
								else if (mmopt < 75)
									nLevelMagic = 4;
								else if (mmopt < 90)
									nLevelMagic = 5;
								else
									nLevelMagic = 6;
							}
							else if (nTotalLucky < 16)
							{
								if (mmopt < 5)
									nLevelMagic = 2;
								else if (mmopt < 25)
									nLevelMagic = 3;
								else if (mmopt < 60)
									nLevelMagic = 4;
								else if (mmopt < 80)
									nLevelMagic = 5;
								else if (mmopt < 95)
									nLevelMagic = 6;
								else
									nLevelMagic = 6;
							}
							else if (nTotalLucky < 20)
							{
								if (mmopt < 10)
									nLevelMagic = 3;
								else if (mmopt < 30)
									nLevelMagic = 4;
								else if (mmopt < 50)
									nLevelMagic = 5;
								else if (mmopt < 75)
									nLevelMagic = 6;
								else
									nLevelMagic = 6;
							}
							else if (nTotalLucky < 24)
							{
								if (mmopt < 5)
									nLevelMagic = 3;
								else if (mmopt < 20)
									nLevelMagic = 4;
								else if (mmopt < 40)
									nLevelMagic = 5;
								else if (mmopt < 60)
									nLevelMagic = 6;
								else if (mmopt < 80)
									nLevelMagic = 7;
								else
									nLevelMagic = 8;
							}
							else if (nTotalLucky < 28)
							{
								if (mmopt < 15)
									nLevelMagic = 4;
								else if (mmopt < 40)
									nLevelMagic = 5;
								else if (mmopt < 70)
									nLevelMagic = 6;
								else if (mmopt < 85)
									nLevelMagic = 7;
								else	if (mmopt < 95)
									nLevelMagic = 8;
								else
									nLevelMagic = 9;
							}
							else if (nTotalLucky < 32)
							{
								if (mmopt < 5)
									nLevelMagic = 4;
								else if (mmopt < 20)
									nLevelMagic = 5;
								else if (mmopt < 40)
									nLevelMagic = 6;
								else if (mmopt < 60)
									nLevelMagic = 7;
								else if (mmopt < 80)
									nLevelMagic = 8;
								else if (mmopt < 97)
									nLevelMagic = 9;
								else
									nLevelMagic = 9;
							}
							else if (nTotalLucky < 36)
							{
								if (mmopt < 5)
									nLevelMagic = 5;
								else if (mmopt < 25)
									nLevelMagic = 6;
								else if (mmopt < 55)
									nLevelMagic = 7;
								else if (mmopt < 75)
									nLevelMagic = 8;
								else if (mmopt < 96)
									nLevelMagic = 9;
								else
									nLevelMagic = 9;
							}
							else if (nTotalLucky < 40)
							{
								if (mmopt < 5)
									nLevelMagic = 5;
								else if (mmopt < 30)
									nLevelMagic = 6;
								else if (mmopt < 50)
									nLevelMagic = 7;
								else if (mmopt < 70)
									nLevelMagic = 8;
								else if (mmopt < 95)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky < 44)
							{
								if (mmopt < 15)
									nLevelMagic = 6;
								else if (mmopt < 35)
									nLevelMagic = 7;
								else if (mmopt < 55)
									nLevelMagic = 8;
								else if (mmopt < 94)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky < 48)
							{
								if (mmopt < 10)
									nLevelMagic = 6;
								else if (mmopt < 30)
									nLevelMagic = 7;
								else if (mmopt < 50)
									nLevelMagic = 8;
								else if (mmopt < 90)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky < 52)
							{
								if (mmopt < 5)
									nLevelMagic = 6;
								else if (mmopt < 25)
									nLevelMagic = 7;
								else if (mmopt < 45)
									nLevelMagic = 8;
								else if (mmopt < 88)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky < 56)
							{
								if (mmopt < 20)
									nLevelMagic = 7;
								else if (mmopt < 40)
									nLevelMagic = 8;
								else if (mmopt < 85)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky < 60)
							{
								if (mmopt < 15)
									nLevelMagic = 7;
								else if (mmopt < 35)
									nLevelMagic = 8;
								else if (mmopt < 80)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky < 120)
							{
								if (mmopt < 40)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else if (nTotalLucky >= 150)
							{
								if (mmopt < 20)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}
							else
							{
								if (mmopt < 10)
									nLevelMagic = 7;
								else if (mmopt < 30)
									nLevelMagic = 8;
								else if (mmopt < 70)
									nLevelMagic = 9;
								else
									nLevelMagic = 10;
							}


							if (nLevelMagic > nLevel && nLevel < 6)
								pnMagicLevel[j] = nLevel;
							else
								pnMagicLevel[j] = nLevelMagic;

						}
						else
						{
							pnMagicLevel[j] = 0;
							bSkip = TRUE;
						}
					}
					else
					{
						pnMagicLevel[j] = 0;
					}
				}
				int nXOpt = ItemSet.genXOpt(nLuck);
				int nIdx = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion(), 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, nXOpt);
				if (nIdx <= 0 || nIdx >= MAX_ITEM)
					continue;

				GetMpsPos(&nX, &nY);
				ptLocal.x = nX;
				ptLocal.y = nY;
				SubWorld[m_SubWorldIndex].GetFreeObjPos(ptLocal);
				Pos.nSubWorld = m_SubWorldIndex;
				SubWorld[m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,
					&Pos.nRegion, &Pos.nMapX, &Pos.nMapY,
					&Pos.nOffX, &Pos.nOffY);


				sInfo.m_nItemID = nIdx;
				sInfo.m_nItemWidth = Item[nIdx].GetWidth();
				sInfo.m_nItemHeight = Item[nIdx].GetHeight();
				sInfo.m_nMoneyNum = 0;
				if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
				{
					sprintf(sInfo.m_szName, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				}
				else
				{
					strcpy(sInfo.m_szName, Item[nIdx].GetName());
				}
				sInfo.m_nColorID = Item[nIdx].GetColorItem();
				sInfo.m_nGenre = Item[nIdx].GetGenre();
				sInfo.m_nDetailType = Item[nIdx].GetDetailType();
				sInfo.m_nParticularType = Item[nIdx].GetParticular();
				sInfo.m_nMovieFlag = 1;
				sInfo.m_nSoundFlag = 1;
				sInfo.m_dwNpcId1 = 0;

				nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), Pos, sInfo);
				if (nObj == -1)
				{
					ItemSet.Remove(nIdx);
				}
				else
				{
					Object[nObj].SetItemBelong(nBelongIdx > 0 ? nBelongIdx : -1);
				}
				j++;
				break;
			}
			nCheckRand += m_pDropRate->pItemParam[i].nRate;
		}
	}
}

#endif

#ifndef _SERVER

void	KNpc::SetMenuState(int nState, char *lpszSentence, int nLength)
{
	this->m_DataRes.SetMenuState(nState, lpszSentence, nLength);
}
#endif

#ifndef _SERVER

int		KNpc::GetMenuState()
{
	return this->m_DataRes.GetMenuState();
}
#endif

#ifndef _SERVER

DWORD	KNpc::SearchAroundID(DWORD dwID)
{
	int		nIdx, nRegionNo;
	nIdx = SubWorld[0].m_Region[m_RegionIndex].SearchNpc(dwID);
	if (nIdx)
		return nIdx;
	for (int i = 0; i < 8; i++)
	{
		nRegionNo = SubWorld[0].m_Region[m_RegionIndex].m_nConnectRegion[i];
		if ( nRegionNo < 0)
			continue;
		nIdx = SubWorld[0].m_Region[nRegionNo].SearchNpc(dwID);
		if (nIdx)
			return nIdx;
	}
	return 0;
}
#endif

#ifndef _SERVER
void	KNpc::SetSpecialSpr(char *lpszSprName)
{
	m_DataRes.SetSpecialSpr(lpszSprName);
}
#endif

#ifndef _SERVER
void	KNpc::SetInstantSpr(int nNo)
{
	char	szName[FILE_NAME_LENGTH];
	szName[0] = 0;
	NpcSet.m_cInstantSpecial.GetSprName(nNo, szName, sizeof(szName));
	if (szName[0])
		this->SetSpecialSpr(szName);
}
#endif

#ifndef _SERVER
int		KNpc::GetNormalNpcStandDir(int nFrame)
{
	return m_DataRes.GetNormalNpcStandDir(nFrame);
}
#endif

#ifdef _SERVER
void KNpc::UpdateNpcStateInfo()
{
	int i = 0, j = 0;
	memset(m_btStateInfo, 0, sizeof(BYTE) * MAX_SKILL_STATE);
	KStateNode *pNode = (KStateNode*)m_StateSkillList.GetTail();
	//
	if (m_ActiveAuraID)
	{
		int nLevel = m_SkillList.GetCurrentLevel(m_ActiveAuraID);
		if (nLevel > 0)
		{
			int nSpecialID = 0;
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nLevel);
			if (pOrdinSkill)
            {
				if (nSpecialID = pOrdinSkill->GetStateSpecailId())
					m_btStateInfo[i++] = nSpecialID;
				if(pOrdinSkill->GetAppendSkillNum())
				{
					for( j = 0; j < pOrdinSkill->GetAppendSkillNum(); j++)
					{
						int nChildId = pOrdinSkill->GetAppendSkillId(j);
						nLevel = m_SkillList.GetCurrentLevel(nChildId);
						if(nLevel > 0)
						{
							KSkill * pChildSkill = (KSkill *) g_SkillManager.GetSkill(nChildId, nLevel);
							if(pChildSkill)
							{
								if (nSpecialID = pChildSkill->GetStateSpecailId())
									m_btStateInfo[i++] = nSpecialID;
							}
						}
					}
				}
            }
		}
	}
	//
	while ( pNode && i < MAX_SKILL_STATE)
	{
		if (pNode->m_StateGraphics > 0)
		{
			m_btStateInfo[i++] = pNode->m_StateGraphics;
		}
		pNode = (KStateNode*)pNode->GetPrev();
	}
}


#endif

#ifndef _SERVER
void	KNpc::SetNpcState(BYTE* pNpcState)
{
	if (!pNpcState)
		return;

	memcpy(m_btStateInfo, pNpcState, sizeof(BYTE) * MAX_SKILL_STATE);
}
#endif

void	KNpc::ClearNpcState()
{
	KStateNode * pNode = (KStateNode*)m_StateSkillList.GetTail();
	KStateNode * pTempNode = NULL;
	
	while(pNode)
	{
		pTempNode = pNode;
		pNode = (KStateNode*) pNode->GetPrev();
		pTempNode->m_bOverLook = 0;
		pTempNode->m_LeftTime = 0;
		pTempNode->m_SkillID = 0;
		memset(pTempNode->m_State, 0, sizeof(pTempNode->m_State));
		pTempNode->Remove();
		delete pTempNode;
	}
	return;
}

void	KNpc::RestoreNpcBaseInfo()
{
	int nmPlayerIdx = 0;

	if (m_Kind == kind_player)
	{//?????
#ifdef _SERVER
		nmPlayerIdx = GetPlayerIdx();
#else
		nmPlayerIdx = CLIENT_PLAYER_INDEX;
#endif

	}
	m_CurrentCamp = m_Camp;
	m_ActiveSkillID = 0;
	m_ActiveAuraID = 0;
	m_TimeHorse = 0;
	m_TimeSWPK = 0;

	m_nPeopleIdx = 0;
	m_nLastDamageIdx = 0;
	m_nLastPoisonDamageIdx = 0;
	m_nObjectIdx = 0;

	m_CurrentLife			= m_LifeMax;
	m_CurrentLifeMax		= m_LifeMax;
	m_CurrentLifeReplenish	= m_LifeReplenish;
	m_CurrentMana			= m_ManaMax;
	m_CurrentManaMax		= m_ManaMax;
	m_CurrentManaReplenish	= m_ManaReplenish;
	m_CurrentStamina		= m_StaminaMax;
	m_CurrentStaminaMax		= m_StaminaMax;
	m_CurrentStaminaGain	= m_StaminaGain;
	m_CurrentStaminaLoss	= m_StaminaLoss;

	if (!m_bNoReloadAttr) {
		memset(&m_CurrentFireDamage, 0, sizeof(m_CurrentFireDamage));
		memset(&m_CurrentColdDamage, 0, sizeof(m_CurrentColdDamage));
		memset(&m_CurrentLightDamage, 0, sizeof(m_CurrentLightDamage));
		memset(&m_CurrentPoisonDamage, 0, sizeof(m_CurrentPoisonDamage));

		memset(&m_CurrentFireMagic, 0, sizeof(m_CurrentFireMagic));
		memset(&m_CurrentColdMagic, 0, sizeof(m_CurrentColdMagic));
		memset(&m_CurrentLightMagic, 0, sizeof(m_CurrentLightMagic));
		memset(&m_CurrentPoisonMagic, 0, sizeof(m_CurrentPoisonMagic));
	}

	m_CurrentAttackRating	= m_AttackRating;
	m_CurrentDefend			= m_Defend;

	m_CurrentFireResist		= m_FireResist;
	m_CurrentColdResist		= m_ColdResist;
	m_CurrentPoisonResist	= m_PoisonResist;
	m_CurrentLightResist	= m_LightResist;
	m_CurrentPhysicsResist	= m_PhysicsResist;
	m_CurrentFireResistMax	= m_FireResistMax;
	m_CurrentColdResistMax	= m_ColdResistMax;
	m_CurrentPoisonResistMax = m_PoisonResistMax;
	m_CurrentLightResistMax	= m_LightResistMax;
	m_CurrentPhysicsResistMax  = m_PhysicsResistMax;

	m_CurrentWalkSpeed		= m_WalkSpeed;
	m_CurrentRunSpeed		= m_RunSpeed;
	m_CurrentAttackSpeed	= m_AttackSpeed;
	m_CurrentCastSpeed		= m_CastSpeed;
	m_CurrentVisionRadius	= m_VisionRadius;
	m_CurrentActiveRadius	= m_ActiveRadius;	//B¸n kÝnh ho¹t ®éng
	m_CurrentHitRecover		= m_HitRecover;		//thêi gian phôc håi
	m_CurrentTreasure		= m_Treasure;

	if (m_Kind == kind_player)
	{//?????
		int nCurbei = 0;

		/*int nFomoidx=GetCurFuMoIdx();

		if 	(nFomoidx>-1)
		{
			if (GetCurFoMoSkllLevel(nFomoidx)>=5) //?????????
			{
				nCurbei=GetCurFoMoSkllLevel(nFomoidx)/5;
			}
		}
		*/
		int nTempVal_life = 0;
		int nTempVal_fangyu = 0;
		m_CurrentLifeMax = m_LifeMax * (100 + nTempVal_life + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornLifeMaxVal() + nCurbei * 50) / 100;
		//TakeTrader(m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
		//m_LifeMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;
		m_CurrentManaMax = m_ManaMax * (100 + nTempVal_life + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornLifeMaxVal() + nCurbei * 50) / 100;
		//TakeTrader(m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50),100);
		//m_ManaMax*(100+Player[nmPlayerIdx].m_cReBorn.GetReBornNum()*50+nCurbei*50)/100;//????????????
		m_CurrentFireResistMax = m_FireResistMax + nTempVal_fangyu + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal(); //200
		m_CurrentColdResistMax = m_ColdResistMax + nTempVal_fangyu + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
		m_CurrentPoisonResistMax = m_PoisonResistMax + nTempVal_fangyu + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
		m_CurrentLightResistMax = m_LightResistMax + nTempVal_fangyu + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
		m_CurrentPhysicsResistMax = m_PhysicsResistMax + nTempVal_fangyu + Player[nmPlayerIdx].m_cReBorn.GetReBornNum() * Player[nmPlayerIdx].m_cReBorn.GetReBornFanYuMaxVal();
	}
	else
	{
		m_CurrentLifeMax = m_LifeMax;
		m_CurrentManaMax = m_ManaMax;//????????????
		m_CurrentFireResistMax = m_FireResistMax; //200
		m_CurrentColdResistMax = m_ColdResistMax;
		m_CurrentPoisonResistMax = m_PoisonResistMax;
		m_CurrentLightResistMax = m_LightResistMax;
		m_CurrentPhysicsResistMax = m_PhysicsResistMax;
	}

	m_CurrentDamage2Mana	= 0;
	m_CurrentManaPerEnemy	= 0;
	m_CurrentLifeStolen		= 0;
	m_CurrentManaStolen		= 0;
	m_CurrentStaminaStolen	= 0;
	
	m_CurrentDeadlyStrikeEnhanceP	= 0;
	m_CurrentFatallyStrikeEnhanceP	= 0;
	m_CurrentFatallyStrikeResP	= 0;

	m_CurrentBlindEnemy		= 0;
	m_CurrentPiercePercent	= 0;
	m_CurrentFreezeTimeReducePercent	= 0;	//Thêi gian lµm chËm gi¶m bít
	m_CurrentPoisonTimeReducePercent	= 0;
	m_CurrentStunTimeReducePercent		= 0;
	m_CurrentIgnoreNegativeStateP = 0;
	m_CurrentFireEnhance	= 0;
	m_CurrentColdEnhance	= 0;
	m_CurrentPoisonEnhance	= 0;
	m_CurrentLightEnhance	= 0;
	m_CurrentRangeEnhance	= 0;
	m_CurrentHandEnhance	= 0;
	m_CurrentExpSkillsVip = 0;
    m_CurrentExpSkillsEnchance = 1; // ExpSkills x2
	m_CurrentReturnSkillPercent = 0;
	m_CurrentIgnoreSkillPercent = 0;
	ZeroMemory(m_CurrentMeleeEnhance, sizeof(m_CurrentMeleeEnhance));
	memset(&m_ReplySkill, 0, sizeof(m_ReplySkill));
	memset(&m_RescueSkill, 0, sizeof(m_RescueSkill));
	memset(&m_AttackSkill, 0, sizeof(m_AttackSkill));
	memset(&m_DeathSkill, 0, sizeof(m_DeathSkill));
	m_CurrentManaShield = 0;		//#giam thieu sat thuong ganh chiu HTVC con lon
	m_CurrentStaticMagicShieldP = 0;
	m_CurrentPoisonDamageReturnPercent = 0;
	m_CurrentExpEnhance = 0;
	m_CurrentSkillEnhancePercent = 0;
	memset(m_nMe2SeriesDamP, 0, sizeof(m_nMe2SeriesDamP));	// [KM 27/08b]
	memset(m_nSeries2MeDamP, 0, sizeof(m_nSeries2MeDamP));
	m_CurrentFiveElementsEnhance = 0;
	m_CurrentFiveElementsResist = 0;
	m_CurrentManaToSkillEnhanceP = 0;					//#khi noi cong day tang ky nang cong kich
	m_CurrentSorbDamageP = 0;								//#triet tieu sat thuong
	m_CurrentBlockRate = 0; m_CurrentAntiBlockRate = 0;					// [KM 27/08]
	m_CurrentEnhanceHitRate = 0; m_CurrentAntiEnhanceHitRate = 0;		// [KM 27/08]
	m_CurrentAntiAllResP = 0; m_CurrentAntiSorbDamageP = 0;				// [KM 27/08]
	m_CurrentEnhanceHitEffect = 0; m_nKMHitPercent = 100;				// [KM 27/08]
	m_CurrentAntiEnhanceHitEffect = 0; m_CurrentAntiHitRecover = 0;		// [KM 27/08]
	m_CurrentAddDamageP = 100; m_nKMAntiHitRecover = 0;					// [KM 27/08] goc 100 theo Linux
	m_CurrentSorbDamageYanP = 0;	// [PF 31/08k]
	m_CurrentAntiStunTimeReduceP = 0;	// [PF 31/08k]
	m_CurrentAntiPoisonTimeReduceP = 0;	// [PF 31/08k]
	m_CurrentDoHurtP = 0;	// [PF 31/08k]
	m_CurrentAntiDoHurtP = 0;	// [PF 31/08k]
	m_CurrentManaReplenishPercent = 0;	// [PF 31/08k]
	m_CurrentAntiPhysicsResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiFireResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiColdResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiPoisonResYanP = 0;	// [PF 31/08k]
	m_CurrentAntiLightingResYanP = 0;	// [PF 31/08k]
	Player[m_nPlayerIdx].m_nCurLucky = Player[m_nPlayerIdx].m_nLucky;
	//
	memset(&m_ManaShield, 0, sizeof(m_ManaShield));
	memset(&m_LightArmor, 0, sizeof(m_LightArmor));
	memset(&m_PhysicsArmor, 0, sizeof(m_PhysicsArmor));
	memset(&m_ColdArmor, 0, sizeof(m_ColdArmor));
	memset(&m_FireArmor, 0, sizeof(m_FireArmor));
	memset(&m_PoisonArmor, 0, sizeof(m_PoisonArmor));

	ClearStateSkillEffect();
	ClearNormalState();
}

#ifndef _SERVER
void KNpc::DrawBorder()
{
	if (m_Index <= 0)
		return;

	m_DataRes.DrawBorder();
}

int KNpc::DrawMenuState(int n)
{
	if (m_Index <= 0)
		return n;

	return m_DataRes.DrawMenuState(n);
}

void KNpc::DrawBlood()
{
	if (m_Kind != kind_normal)
		return;

	int nFontSize = 12;
	int nHeightOff = GetNpcPate();
	//if (NpcSet.CheckShowLife())
	{
		nHeightOff = PaintLife(nHeightOff, true);
		nHeightOff += SHOW_SPACE_HEIGHT;
	}
	//if (NpcSet.CheckShowName())
	{
		nHeightOff = PaintInfo(nHeightOff, true);
	}
}
#endif

#ifdef _SERVER
int KNpc::SetPos(int nX, int nY)
{
	AUTOLOG_EVERY(1000, "[E4_POS_SETPOS] npc=%d id=%u sw=%d rgn=%d old=(%d,%d,%d,%d) new=(%d,%d) doing=%d", m_Index, m_dwID, m_SubWorldIndex, m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, nX, nY, (int)m_Doing);
	if (m_SubWorldIndex < 0)
	{
		_ASSERT(0);
		return 0;
	}

	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[m_SubWorldIndex].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);

	if (nRegion < 0)
	{
		g_DebugLog("[Script]SetPos error:SubWorld:%d, Pos(%d, %d)", SubWorld[m_SubWorldIndex].m_SubWorldID, nX, nY);
		return 0;
	}

	int nOldRegion = m_RegionIndex;
	if (m_RegionIndex >= 0)
	{
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);

		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC), NPC_EVENT_BROADCAST_LIMIT);	// [S11] go khong cat ai
	}

	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;

	if (nOldRegion != nRegion || nRegion < 0)
	{
		SubWorld[m_SubWorldIndex].NpcChangeRegion(nOldRegion, nRegion, m_Index);
		if (IsPlayer())
			SubWorld[m_SubWorldIndex].PlayerChangeRegion(nOldRegion, nRegion, m_nPlayerIdx);
	}

	SubWorld[m_SubWorldIndex].m_Region[nRegion].AddRef(m_MapX, m_MapY, obj_npc);

	DoStand();
	m_ProcessAI = 1;
	m_ProcessState = 1;
	// [S12-TELE 27/08] SetPos CUNG MAP von KHONG bao cho chinh nguoi choi (chi phat
	// s2c_npcremove cho nguoi xung quanh) - client mu, chi phat hien qua vong xoay
	// tu-sync 5 NPC/tick/region (p50 564ms) => chuoi [S8-NAN] bung 4 cu lien tiep sau
	// moi lan script teleport + dat di (do that 26/08: 8/9 cu bung la loai nay).
	// Gui NGAY mot goi tu-sync CO SAN (s2c_syncnpcminplayer, 27 byte) cho rieng chu
	// nhan vat: client snap MOT lan dung luc dich chuyen that (hop mat nguoi choi).
	// memset bat buoc: khuon goc de m_byDoing/MapID/m_nEquipCount la rac stack.
	// Gac m_nPlayerIdx > 0: bot SimCity co m_nPlayerIdx = 0; bot thuong co
	// m_nNetConnectIdx = -1 (PackDataToClient tu chan nhung khoi ton call).
	if (IsPlayer() && !m_btSimCityBot && m_nPlayerIdx > 0 && m_nPlayerIdx < MAX_PLAYER &&
		Player[m_nPlayerIdx].m_nNetConnectIdx >= 0)
	{
		NPC_PLAYER_TYPE_NORMAL_SYNC sTeleSync;
		memset(&sTeleSync, 0, sizeof(sTeleSync));
		sTeleSync.ProtocolType = s2c_syncnpcminplayer;
		sTeleSync.m_dwNpcID = m_dwID;
		int nTeleX = 0, nTeleY = 0;
		GetMpsPos(&nTeleX, &nTeleY);
		sTeleSync.m_dwMapX = nTeleX;
		sTeleSync.m_dwMapY = nTeleY;
		sTeleSync.m_wOffX = m_OffX;
		sTeleSync.m_wOffY = m_OffY;
		g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sTeleSync, sizeof(sTeleSync));
		// [S12b 28/08] g_DebugLog chi ban WM_COPYDATA sang DebugWin (khong ghi file) ->
		// khong the nghiem thu. Doi sang AUTOLOG de dem duoc trong jx_auto_server.log.
		AUTOLOG("[S12-TELE] %s setpos cung map -> bao chinh chu (%d,%d) t=%u", Name, nTeleX, nTeleY, SubWorld[m_SubWorldIndex].m_dwCurrentTime);
	}
	return 1;
}
#endif

#ifdef _SERVER
int KNpc::ChangeWorld(DWORD dwSubWorldID, int nX, int nY)
{
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);

	//if (!IsPlayer()) return 0; //BÃ¡ Â®i vÃ‹n tiÂªu Â®ang cÃ‡n
	
	if (-1 == nTargetSubWorld)
	{
		//if (m_SubWorldIndex >= 0)
		//	SubWorld[m_SubWorldIndex].m_MissionArray.RemovePlayer(m_nPlayerIdx, Player[m_nPlayerIdx].m_dwID);
		//TobeExchangeServer(dwSubWorldID, nX, nY);
		g_DebugLog("MapID %d haven't been loaded!", dwSubWorldID);
		return 0;
	}

	if (IsPlayer())	// [BDH 27/08] NPC thuong co m_nPlayerIdx=0 -> truoc day ghi de Player[0]
		Player[m_nPlayerIdx].m_nPrePayMoney = 0;
	
	AUTOLOG_EVERY(1000, "[E4_POS_CHANGEWORLD] npc=%d id=%u swid=%u cursw=%d target=%d to=(%d,%d) doing=%d", m_Index, m_dwID, dwSubWorldID, m_SubWorldIndex, nTargetSubWorld, nX, nY, (int)m_Doing);
	if (nTargetSubWorld == m_SubWorldIndex)
	{
		return SetPos(nX, nY);
	}
	
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	SubWorld[nTargetSubWorld].Mps2Map(nX, nY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	
	if (nRegion < 0)
	{
		g_DebugLog("[Map]Change Pos(%d,%d) Invalid!", nX, nY);
		return 0;
	}
	
	//if (m_SubWorldIndex >= 0)
	//	SubWorld[m_SubWorldIndex].m_MissionArray.RemovePlayer(m_nPlayerIdx, Player[m_nPlayerIdx].m_dwID);
    
	if (m_SubWorldIndex >= 0 && m_RegionIndex >= 0)
	{
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].RemoveNpc(m_Index);
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);

		NPC_REMOVE_SYNC	RemoveSync;
		RemoveSync.ProtocolType = s2c_npcremove;
		RemoveSync.ID = m_dwID;
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC), NPC_EVENT_BROADCAST_LIMIT);	// [S11] go khong cat ai
	}

	int nSourceSubWorld = m_SubWorldIndex;
	int nSourceRegion = m_RegionIndex;

	m_SubWorldIndex = nTargetSubWorld;
	m_RegionIndex = nRegion;
	m_MapX = nMapX;
	m_MapY = nMapY;
	m_MapZ = 0;
	m_OffX = nOffX;
	m_OffY = nOffY;
	SubWorld[nTargetSubWorld].Map2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &m_OriginX, &m_OriginY);
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddNpc(m_Index);
	SubWorld[nTargetSubWorld].m_Region[nRegion].AddRef(m_MapX, m_MapY, obj_npc);
	DoStand();
	m_ProcessAI = 1;
    
	if (IsPlayer())
	{
        
		SubWorld[nTargetSubWorld].SendSyncData(m_Index, Player[m_nPlayerIdx].m_nNetConnectIdx);
		SubWorld[nSourceSubWorld].RemovePlayer(nSourceRegion, m_nPlayerIdx);
		SubWorld[nTargetSubWorld].AddPlayer(nRegion, m_nPlayerIdx);
		// [WLLS 20/08] NewWorldScript cua MapList.ini: roi map -> OnLeaveWorld,
		// vao map -> OnNewWorld (Linux GS co san co che nay, Windows bo quen).
		KSubWorld_FireMapScript(nSourceSubWorld, "OnLeaveWorld", m_nPlayerIdx);
		KSubWorld_FireMapScript(nTargetSubWorld, "OnNewWorld", m_nPlayerIdx);
	}
	return 1;
}
#endif

#ifdef _SERVER
void KNpc::TobeExchangeServer(DWORD dwMapID, int nX, int nY)
{
	if (!IsPlayer())
	{
		return;
	}

	m_OldFightMode = m_FightMode;
	m_bExchangeServer = TRUE;
	if (m_nPlayerIdx > 0 && m_nPlayerIdx <= MAX_PLAYER)
	{
		Player[m_nPlayerIdx].TobeExchangeServer(dwMapID, nX, nY);
	}
}
#endif

BOOL KNpc::IsPlayer()
{
#ifdef _SERVER
	return m_Kind == kind_player;
#else
	return m_Index == Player[CLIENT_PLAYER_INDEX].m_nIndex;
#endif
}

BOOL KNpc::IsAlive()
{
	if(m_Doing == do_death)
	{
		return FALSE;
	}

	if(m_Doing == do_revive)
	{
		return FALSE;
	}
	
	return TRUE;
}

void KNpc::ForceClearStateSkillEffect() {
	KStateNode* pNode;
	pNode = (KStateNode*)m_StateSkillList.GetTail();
#ifdef _SERVER
	bool bStateRemove = false;
#endif
	while (pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode*)pNode->GetPrev();
		if (pTempNode)
		{
			for (int i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;
#ifdef _SERVER
			bStateRemove = true;
#endif
			pTempNode = NULL;
			continue;
		}
	}
#ifdef _SERVER
	if(bStateRemove)
		UpdateNpcStateInfo();
#endif
}

// [WLLS 20/08] tra level trang thai skill dang treo (GetSkillState):
// khong co -> -1 (officer.lua:184 so sanh ~= -1 truoc khi RemoveSkillState).
int KNpc::GetStateSkillLevel(int nSkillId)
{
	KStateNode* pNode = (KStateNode*)m_StateSkillList.GetTail();
	while (pNode)
	{
		if (pNode->m_SkillID == nSkillId)
			return pNode->m_Level;
		pNode = (KStateNode*)pNode->GetPrev();
	}
	return -1;
}

void KNpc::ForceClearStateSkillEffect(int nSkillId)
{
#ifdef _SERVER
	bool bStateRemove = false;
#endif
	KStateNode* pNode;
	pNode = (KStateNode*)m_StateSkillList.GetTail();
	while (pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode*)pNode->GetPrev();
		if (nSkillId != pTempNode->m_SkillID)
			continue;
		//
		if (pTempNode)
		{
			for (int i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;
#ifdef _SERVER
			bStateRemove = true;
#endif
			pTempNode = NULL;
			continue;
		}
	}
#ifdef _SERVER
	if(bStateRemove)
		UpdateNpcStateInfo();
#endif
}

void KNpc::ClearStateSkillEffect()
{
#ifdef _SERVER
	bool bStateRemove = false;
#endif
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetPrev();
		//
		//pNode->m_LeftTime = nTime;
		if (pTempNode->m_bOverLook)	//KhÂ«ng xoÂ¸ nhÃ·ng skill c? OverLook
			continue;

		if (pTempNode->m_LeftTime == -1)	
			continue;
		//
		if (pTempNode->m_LeftTime > 0)
		{
			for (int i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;
#ifdef _SERVER
			bStateRemove = true;
#endif
			pTempNode = NULL;
			continue;
		}
	}
#ifdef _SERVER
	if(bStateRemove)
		UpdateNpcStateInfo();
#endif
}


void KNpc::ClearStateSkillEffect(int nSkillId)
{
#ifdef _SERVER
	bool bStateRemove = false;
#endif
	KStateNode* pNode;
	pNode = (KStateNode*)m_StateSkillList.GetTail();
	while (pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode*)pNode->GetPrev();
		if (nSkillId != pTempNode->m_SkillID)
			continue;
		//
			for (int i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
			}
			_ASSERT(pTempNode != NULL);
			
			pTempNode->Remove();
			delete pTempNode;
			pTempNode = NULL;
#ifdef _SERVER
			bStateRemove = true;
#endif
			continue;
	}
#ifdef _SERVER
	if(bStateRemove)
		UpdateNpcStateInfo();
#endif
}


void KNpc::ClearNormalState()
{
	ZeroMemory(&m_PhysicsArmor, sizeof(m_PhysicsArmor));
	ZeroMemory(&m_ColdArmor, sizeof(m_ColdArmor));
	ZeroMemory(&m_FireArmor, sizeof(m_FireArmor));
	ZeroMemory(&m_PoisonArmor, sizeof(m_PoisonArmor));
	ZeroMemory(&m_LightArmor, sizeof(m_LightArmor));
	//ZeroMemory(&m_ManaShield, sizeof(m_ManaShield));
	ZeroMemory(&m_PoisonState, sizeof(m_PoisonState));
	ZeroMemory(&m_FreezeState, sizeof(m_FreezeState));
	ZeroMemory(&m_BurnState, sizeof(m_BurnState));
	//ZeroMemory(&m_FrozenAction, sizeof(m_FrozenAction));
	ZeroMemory(&m_StunState, sizeof(m_StunState));
	ZeroMemory(&m_LifeState, sizeof(m_LifeState));
	ZeroMemory(&m_ManaState, sizeof(m_ManaState));
	ZeroMemory(&m_DrunkState, sizeof(m_DrunkState));
	ZeroMemory(&m_HideState, sizeof(m_HideState));
	ZeroMemory(&m_SilentState, sizeof(m_SilentState));
	ZeroMemory(&m_RandMove, sizeof(m_RandMove));
}

void KNpc::CheckTrap()
{
	if (m_btSimCityBot)		// Port SimCity: bot gia lap khong kich bay
		return;
	if (m_Kind != kind_player)
		return;
	
	if (m_Index <= 0)
		return;

	if (m_SubWorldIndex < 0 || m_RegionIndex < 0)
		return;

	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(m_MapX, m_MapY);
	// [PORT5 23/08] trap JX2 (AddMapTrap tham so 5): so sanh CA (scriptId, param) de hai vung
	// trap ke nhau CUNG script khac param van kich (tongcastle); trap JX1 param NONE - nhu cu.
	// [FIX 24/08] GetTrapParam CHI ton tai o ban _SERVER (KRegion.cpp:1341 nam trong
	// #ifdef _SERVER) - khong guard thi CoreClient.dll loi LNK2019.
	int nCellTrapParam = 0;
#ifdef _SERVER
	nCellTrapParam = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrapParam(m_MapX, m_MapY);
#endif
	if (m_TrapScriptID == dwTrap
#ifdef _SERVER
		&& m_nLastTrapParam == nCellTrapParam
#endif
		)
	{
		return;
	}
	else
	{
		m_TrapScriptID = dwTrap;
#ifdef _SERVER
		m_nLastTrapParam = nCellTrapParam;
#endif
	}

	if (!m_TrapScriptID)
	{
		return;
	}

#ifdef _SERVER
	// BOT KPLAYER MIEN TRAP (nhu bot SimCity m_btSimCityBot o dau ham).
	// Trap o cua map luyen (vd Hoa Son) chay script day nguoi khong du dieu kien
	// NGUOC VE DIEM VAO - bot cham trap la "xuat hien lai tai diem cu" (chu game
	// quan sat 18/08). Bot doi ban do bang ChangeWorld rieng, khong can trap;
	// van ghi bot.log de biet bot DA dap trap nao, o dau.
	if (PB_IsBot(m_nPlayerIdx))
	{
		PB_TrapLog(m_nPlayerIdx, m_TrapScriptID, m_MapX, m_MapY);
		return;
	}
#endif

#ifdef _SERVER
	if (nCellTrapParam != JX2TRAP_PARAM_NONE)
	{
		// [PORT5 23/08] trap dat qua AddMapTrap: main(nParam) nhu Linux (KNpc::CheckTrap 0x0807DA78;
		// 4 tham so -> param 0 -> main(0) cung nhu Linux)
		Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", nCellTrapParam);
		return;
	}
#endif
	Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", m_nPlayerIdx);
}

void KNpc::SetFightMode(BOOL bFightMode)
{
#ifdef _SERVER
	if (this->m_Kind == kind_player)
		Player[this->m_nPlayerIdx].m_cPK.CloseAll();
#endif

	m_FightMode = bFightMode;
}

void KNpc::TurnTo(int nIdx)
{
	if (!Npc[nIdx].m_Index || !m_Index)
		return;

	int nX1, nY1, nX2, nY2;

	GetMpsPos(&nX1, &nY1);
	Npc[nIdx].GetMpsPos(&nX2, &nY2);

	m_Dir = g_GetDirIndex(nX1, nY1, nX2, nY2);
}

void KNpc::ReCalcStateEffect()
{
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		if (pNode->m_LeftTime != 0)
		{
			int i;
			for (i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pNode->m_State[i].nAttribType)
				{
					KMagicAttrib	MagicAttrib;
					MagicAttrib.nAttribType = pNode->m_State[i].nAttribType;
					MagicAttrib.nValue[0] = -pNode->m_State[i].nValue[0];
					MagicAttrib.nValue[1] = -pNode->m_State[i].nValue[1];
					MagicAttrib.nValue[2] = -pNode->m_State[i].nValue[2];
					ModifyAttrib(m_Index, &MagicAttrib);
				}
			}
		}
		pNode = (KStateNode *)pNode->GetPrev();
	}
}

void KNpc::IgnoreState(BOOL bNegative) //Bá qua tr¹ng th¸i
{
#ifdef _SERVER
	bool bStateRemove = false;
#endif
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		KStateNode* pTempNode = pNode;
		pNode = (KStateNode *)pNode->GetPrev();
		//
		if (pTempNode->m_bOverLook)	//Kh«ng xo¸ nh÷ng skill cã OverLook
			continue;
		//
		if (pTempNode->m_LeftTime >= 0)
		{
			if(bNegative)
			{
				KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(pTempNode->m_SkillID, pTempNode->m_Level);
				if (!pSkill)
					continue;

				if (!pSkill->IsTargetOnly() && !pSkill->IsTargetEnemy())
					continue;
			}
			//
			int i;
			for (i = 0; i < MAX_SKILL_STATE; i++)
			{
				if (pTempNode->m_State[i].nAttribType)
				{
					ModifyAttrib(m_Index, &pTempNode->m_State[i]);
				}
			}
			_ASSERT(pTempNode != NULL);
			pTempNode->Remove();
			delete pTempNode;
			pTempNode = NULL;
			//
#ifdef _SERVER
			bStateRemove = true;
#endif
			continue;
		}
	}
	//
#ifdef _SERVER
	if(bStateRemove)
		UpdateNpcStateInfo();
	IGNORE_STATE_SYNC	Sync; 
	Sync.ProtocolType = s2c_ignorestate;
	Sync.bNegative = bNegative;
	g_pServer->PackDataToClient(Player[m_nPlayerIdx].m_nNetConnectIdx, &Sync, sizeof(IGNORE_STATE_SYNC));
#endif
}

#ifndef _SERVER
extern KTabFile g_ClientWeaponSkillTabFile;
#endif

int		KNpc::GetCurActiveWeaponSkill()
{
	int nSkillId = 0;
	if (IsPlayer())
	{
		
		int nDetailType = Player[m_nPlayerIdx].m_ItemList.GetWeaponType();
		int nParticularType = Player[m_nPlayerIdx].m_ItemList.GetWeaponParticular();
		
		if (nDetailType == 0)
		{
			nSkillId = g_nMeleeWeaponSkill[nParticularType];
		}
		else if (nDetailType == 1)
		{
			nSkillId = g_nRangeWeaponSkill[nParticularType];
		}
		else if (nDetailType == -1)
		{
			nSkillId = g_nHandSkill;
		}
	}
	else
	{
#ifdef _SERVER
		//Real Npc
		return 0;
#else
		if (m_Kind == kind_player) // No Local Player
		{
			g_ClientWeaponSkillTabFile.GetInteger(m_WeaponType + 1, "SkillId", 0, &nSkillId);
		}
		else						//Real Npc
		{
			return 0;
		}
#endif
	}
	return nSkillId;
}

#ifndef _SERVER
void	KNpc::AutoFixXY()
{
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex != m_Index)
	{
		if (m_sSyncPos.m_nDoing == do_runattack
					|| (m_sSyncPos.m_nDoing == do_stand
					|| m_sSyncPos.m_nDoing == do_magic
					|| m_sSyncPos.m_nDoing == do_attack
					|| m_sSyncPos.m_nDoing == do_manyattack
					|| m_sSyncPos.m_nDoing == do_jumpattack
					|| m_sSyncPos.m_nDoing == do_goattack
					) 
				&& (m_Doing == do_run || m_Doing == do_walk))
		{	
			int	nRegionIdx;

			if ((DWORD)SubWorld[0].m_Region[m_RegionIndex].m_RegionID == m_sSyncPos.m_dwRegionID)
			{
				SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
				m_MapX = m_sSyncPos.m_nMapX;
				m_MapY = m_sSyncPos.m_nMapY;
				m_OffX = m_sSyncPos.m_nOffX;
				m_OffY = m_sSyncPos.m_nOffY;
				memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
				SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
			}
			else
			{
				nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
				if (nRegionIdx < 0)
					return;
				SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
				SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
				m_RegionIndex = nRegionIdx;
				m_dwRegionID = m_sSyncPos.m_dwRegionID;
				m_MapX = m_sSyncPos.m_nMapX;
				m_MapY = m_sSyncPos.m_nMapY;
				m_OffX = m_sSyncPos.m_nOffX;
				m_OffY = m_sSyncPos.m_nOffY;
				memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
			}
		}
	}
	else if (m_Doing == do_sit)
	{
		int	nRegionIdx;

		if ((DWORD)SubWorld[0].m_Region[m_RegionIndex].m_RegionID == m_sSyncPos.m_dwRegionID)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
			SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
		}
		else
		{
			nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
			if (nRegionIdx < 0)
				return;
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
		}
	}
}

void	KNpc::HurtAutoMove()
{
	if (this->m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		return;

	if (this->m_Doing != do_hurt)
		return;

	if (m_sSyncPos.m_nDoing != do_hurt && m_sSyncPos.m_nDoing != do_stand)
		return;

	int	nFrames, nRegionIdx;

	nFrames = m_Frames.nTotalFrame - m_Frames.nCurrentFrame;
	if (nFrames <= 1)
	{
		if ((DWORD)SubWorld[0].m_Region[m_RegionIndex].m_RegionID == m_sSyncPos.m_dwRegionID)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
			SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
		}
		else
		{
			nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
			if (nRegionIdx < 0)
				return;
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = m_sSyncPos.m_nMapX;
			m_MapY = m_sSyncPos.m_nMapY;
			m_OffX = m_sSyncPos.m_nOffX;
			m_OffY = m_sSyncPos.m_nOffY;
			memset(&m_sSyncPos, 0, sizeof(m_sSyncPos));
		}
	}
	else
	{
		nRegionIdx = SubWorld[0].FindRegion(m_sSyncPos.m_dwRegionID);
		if (nRegionIdx < 0)
			return;
		int		nNpcX, nNpcY, nSyncX, nSyncY;
		int		nNewX, nNewY, nMapX, nMapY, nOffX, nOffY;
		SubWorld[0].Map2Mps(m_RegionIndex, 
			m_MapX, m_MapY,
			m_OffX, m_OffY,
			&nNpcX, &nNpcY);
		SubWorld[0].Map2Mps(nRegionIdx, 
			m_sSyncPos.m_nMapX, m_sSyncPos.m_nMapY,
			m_sSyncPos.m_nOffX, m_sSyncPos.m_nOffY,
			&nSyncX, &nSyncY);
		nNewX = nNpcX + (nSyncX - nNpcX) / nFrames;
		nNewY = nNpcY + (nSyncY - nNpcY) / nFrames;
		SubWorld[0].Mps2Map(nNewX, nNewY, &nRegionIdx, &nMapX, &nMapY, &nOffX, &nOffY);
		//_ASSERT(nRegionIdx >= 0);
		if (nRegionIdx < 0)
			return;
		if (nRegionIdx != m_RegionIndex)
		{
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[m_RegionIndex].m_RegionID, SubWorld[0].m_Region[nRegionIdx].m_RegionID, m_Index);
			m_RegionIndex = nRegionIdx;
			m_dwRegionID = m_sSyncPos.m_dwRegionID;
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
		}
		else
		{
			SubWorld[0].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
			m_MapX = nMapX;
			m_MapY = nMapY;
			m_OffX = nOffX;
			m_OffY = nOffY;
			SubWorld[0].m_Region[m_RegionIndex].AddRef(m_MapX, m_MapY, obj_npc);
		}
	}
}

#endif

#ifndef _SERVER
void KNpc::ProcNetCommand(NPCCMD cmd, int x /* = 0 */, int y /* = 0 */, int z /* = 0 */)
{
	switch (cmd)
	{
	case do_death:
		{
		if (m_RegionIndex >= 0)
			DoDeath();
		}
		break;
	case do_hurt:
		if (m_RegionIndex >= 0)
			DoHurt(x, y, z);
		break;
	case do_revive:  //?Â¾Ã—Ã…Â£Â¿
		{
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		SetInstantSpr(enumINSTANT_STATE_REVIVE); //ÃŠÃÂ·Ã…?Â»Â¸Ã¶Ã–Ã˜Ã‰ÃºÂµÃ„Ã‹Â²Â¼Ã¤?Ã˜?Â§	
		}
		break;
	case do_stand:
		{
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		}
	    break;
	default:
		break;
	}
}
#endif

#ifndef _SERVER
void	KNpc::ClearBlood(int i)
{
	m_nBloodNo[i]		= 0;
	m_nBloodAlpha[i]	= 0;
	m_nBloodTime[i]		= 0;
	m_szBloodNo[i][0]	= 0;
	m_nBloodColor[i]	= 0;
	m_nBloodFontSize[i]	= 0;
}
#endif

#ifndef _SERVER
void	KNpc::SetBlood(int nNo) //nhÂ¶y Damage khi Â®Â¸nh vÂµo NPC
{
	if (nNo <= 0)
		return;
	int i;
	for (i = 0;i < 5;i++)
	{
		if (!m_szBloodNo[i][0])
			break;
	}
	if (i == 5) 
		i = 0;
	m_nBloodNo[i]		= nNo;
	m_nBloodAlpha[i]	= 0;
	m_nBloodTime[i]		= defMAX_SHOW_BLOOD_TIME;
	sprintf(m_szBloodNo[i], "%d", nNo);
}
#endif

#ifndef _SERVER
#define DAMAGE_INFO_TOTAL_FRAME 100
#define MAX_DAMAGE_INFO_TEXT_WIDTH 50
#define DAMAGE_INFO_TEXT_CHAR_WIDTH 10
#define DAMAGE_INFO_TEXT_HEIGHT 30
#define DAMAGE_INFO_TEXT_RANDOM_X 150
#define DAMAGE_INFO_TEXT_RANDOM_Y 100
#define DAMAGE_INFO_TEXT_BASIC_OFFSET_Y 200

int ComputeFrameFromTime(int tick) {
	if (tick < 0) tick = 0;
	if (tick > defMAX_SHOW_BLOOD_TIME) tick = defMAX_SHOW_BLOOD_TIME;

	int half = defMAX_SHOW_BLOOD_TIME / 2; // 20

	if (tick <= half) {
		// 0 ? 4
		return (tick * 4) / half;
	}
	else {
		// 4 ? 0
		return ((defMAX_SHOW_BLOOD_TIME - tick) * 4) / half;
	}
}

int	KNpc::PaintBlood(int nHeightOffset)
{
	int addbloodspace = 50;
	auto RenderBlood = [&](int i) {
		if (!m_szBloodNo[i][0])
			return;

		int nHeightOff = (int)(nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodTime[i]) * defSHOW_BLOOD_MOVE_SPEED / 3);
		DWORD dwColor = m_nBloodColor[i] | (m_nBloodAlpha[i] << 24); // Apply alpha to color
		int nMpsX, nMpsY;
		GetDrawPos(&nMpsX, &nMpsY);

		KRUImage pImage;
		int damage = atoi(m_szBloodNo[i]);
		if (damage == 0 && strcmp(m_szBloodNo[i], (const char*)"MISS"))
			goto NOTHING;
		if (m_nBloodColor[i] == SHOW_BLOOD_COLOR) {
			pImage.oPosition.nX = nMpsX;// +info.PosX;
			pImage.oPosition.nY = nMpsY - nHeightOffset + addbloodspace;// -DAMAGE_INFO_TEXT_BASIC_OFFSET_Y;// -info.PosY;
			pImage.oPosition.nZ = nHeightOff;
			pImage.Color.Color_b.a = 255;
			pImage.uImage = 0;
			pImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			pImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			pImage.nType = 1;
			sprintf(pImage.szImage, "%s", "\\spr\\blood\\damage.spr");
			do {
				pImage.nFrame = (unsigned short)(damage % 10);
				damage /= 10;
				pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
				g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
			} while (damage > 0);
			pImage.nFrame = 10;
			pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
			g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
		}
		else if (m_nBloodColor[i] == SHOW_BLOOD_COLOR_CRIT) {
			int frameSize = ComputeFrameFromTime(m_nBloodTime[i]);
			KRUImage minusImage;
			// Prepare minus sign image
			sprintf(minusImage.szImage, "\\spr\\blood\\-.spr");
			minusImage.uImage = 0;
			minusImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			minusImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			minusImage.nType = 1;
			minusImage.nFrame = frameSize;
			minusImage.Color.Color_b.a = 255;
			minusImage.oPosition.nY = nMpsY - nHeightOffset + 20 + addbloodspace;// -DAMAGE_INFO_TEXT_BASIC_OFFSET_Y;
			minusImage.oPosition.nZ = nHeightOff;
			// Step 1: Prepare digit images before the damage loop
			KRUImage digitImages[10];
			for (int d = 0; d < 10; ++d) {
				sprintf(digitImages[d].szImage, "\\spr\\blood\\%d.spr", d);
				digitImages[d].uImage = 0;
				digitImages[d].bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
				digitImages[d].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				digitImages[d].nType = 1;
				digitImages[d].nFrame = frameSize;
				digitImages[d].Color.Color_b.a = 255;
				digitImages[d].oPosition.nY = nMpsY - nHeightOffset + addbloodspace;// -DAMAGE_INFO_TEXT_BASIC_OFFSET_Y;
				digitImages[d].oPosition.nZ = nHeightOff;
			}

			// Step 2: Extract digits (right-to-left)
			std::vector<int> digits;
			int temp = damage;
			if (temp == 0) digits.push_back(0);
			while (temp > 0) {
				digits.push_back(temp % 10);
				temp /= 10;
			}

			// Step 3: Draw from right to left
			int currentX = nMpsX-15;
			for (int digit : digits) {
				KRUImage pImage = digitImages[digit]; // copy template
				pImage.oPosition.nX = currentX;
				currentX -= DAMAGE_INFO_TEXT_CHAR_WIDTH + 5;
				g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
			}
			minusImage.oPosition.nX = currentX;
			g_pRepresent->DrawPrimitives(1, &minusImage, RU_T_IMAGE, FALSE);
		}
		else if (m_nBloodColor[i] == SHOW_BLOOD_COLOR_ABSORB_LIFE) {
			pImage.oPosition.nX = nMpsX;// +info.PosX;
			pImage.oPosition.nY = nMpsY - nHeightOffset + addbloodspace;// -DAMAGE_INFO_TEXT_BASIC_OFFSET_Y;// -info.PosY;
			pImage.oPosition.nZ = nHeightOff;
			pImage.Color.Color_b.a = 255;
			pImage.uImage = 0;
			pImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			pImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			pImage.nType = 1;
			sprintf(pImage.szImage, "%s", "\\spr\\blood\\heal.spr"); //green
			do {
				pImage.nFrame = (unsigned short)(damage % 10);
				damage /= 10;
				pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
				g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
			} while (damage > 0);
			pImage.nFrame = 10;
			pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
			g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
		}
		else if (m_nBloodColor[i] == SHOW_BLOOD_COLOR_ABSORB_MANA) {
			pImage.oPosition.nX = nMpsX;// +info.PosX;
			pImage.oPosition.nY = nMpsY - nHeightOffset + addbloodspace;// -DAMAGE_INFO_TEXT_BASIC_OFFSET_Y;// -info.PosY;
			pImage.oPosition.nZ = nHeightOff;
			pImage.Color.Color_b.a = 255;
			pImage.uImage = 0;
			pImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			pImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			pImage.nType = 1;
			sprintf(pImage.szImage, "%s", "\\spr\\blood\\healmana.spr"); //blue
			do {
				pImage.nFrame = (unsigned short)(damage % 10);
				damage /= 10;
				pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
				g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
			} while (damage > 0);
			pImage.nFrame = 10;
			pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
			g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
		}
		else if (m_nBloodColor[i] == SHOW_BLOOD_COLOR_MISS) {
			pImage.oPosition.nX = nMpsX;// +info.PosX;
			pImage.oPosition.nY = nMpsY - nHeightOffset + addbloodspace;// -DAMAGE_INFO_TEXT_BASIC_OFFSET_Y;// -info.PosY;
			pImage.oPosition.nZ = nHeightOff;
			pImage.Color.Color_b.a = 255;
			pImage.uImage = 0;
			pImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			pImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			pImage.nType = 1;
			sprintf(pImage.szImage, "%s", "\\spr\\blood\\miss.spr");
			pImage.nFrame = 0;
			pImage.oPosition.nX -= DAMAGE_INFO_TEXT_CHAR_WIDTH;
			g_pRepresent->DrawPrimitives(1, &pImage, RU_T_IMAGE, FALSE);
		}
NOTHING :
		m_nBloodTime[i]--;
		if (m_nBloodTime[i] <= 0)
		{
			ClearBlood(i);
			return;
		}
	};

	// First pass: Render non-critical hits
	for (int i = 0; i < 5; i++)
	{
		if (m_nBloodColor[i] != SHOW_BLOOD_COLOR_CRIT)
		{
			RenderBlood(i);
		}
	}

	// Second pass: Render critical hits
	for (int i = 0; i < 5; i++)
	{
		if (m_nBloodColor[i] == SHOW_BLOOD_COLOR_CRIT)
		{
			RenderBlood(i);
		}
	}

	return nHeightOffset;
}

void	KNpc::SetBlood2(DAMAGESHOW* Damage)
{
	if (Damage->nDamage <= 0 && Damage->enType != COMBAT_INFO_DODGE)
		return;
	int i;
	for (i = 0; i < 5; i++)
	{
		if (!m_szBloodNo[i][0])
			break;
	}
	if (i == 5)
		i = 0;
	m_nBloodNo[i] = Damage->nDamage;
	if (Damage->enType == COMBAT_INFO_DAMAGE_LIFE) {
		if (Damage->IsCrit) {
			m_nBloodColor[i] = SHOW_BLOOD_COLOR_CRIT;
			m_nBloodFontSize[i] = 16;
		}
		else {
			m_nBloodColor[i] = SHOW_BLOOD_COLOR;
			m_nBloodFontSize[i] = 12;
		}
	}
	else if (Damage->enType == COMBAT_INFO_ABSORB_LIFE) {
		m_nBloodColor[i] = SHOW_BLOOD_COLOR_ABSORB_LIFE;
		m_nBloodFontSize[i] = 14;
	}
	else if (Damage->enType == COMBAT_INFO_ABSORB_MANA) {
		m_nBloodColor[i] = SHOW_BLOOD_COLOR_ABSORB_MANA;
		m_nBloodFontSize[i] = 14;
	}
	else if (Damage->enType == COMBAT_INFO_ABSORB_STAMINA) {
		m_nBloodColor[i] = SHOW_BLOOD_COLOR_ABSORB_STAMINA;
		m_nBloodFontSize[i] = 14;
	}
	else if (Damage->enType == COMBAT_INFO_DODGE) {
		m_nBloodColor[i] = SHOW_BLOOD_COLOR_MISS;
		m_nBloodFontSize[i] = 14;
		m_nBloodAlpha[i] = 0;
		m_nBloodTime[i] = defMAX_SHOW_BLOOD_TIME;
		sprintf(m_szBloodNo[i], "MISS");
		return;
	}

	m_nBloodAlpha[i] = 0;
	m_nBloodTime[i] = defMAX_SHOW_BLOOD_TIME;
	if (Damage->IsCrit)
		sprintf(m_szBloodNo[i], "%d", Damage->nDamage);
	else
		sprintf(m_szBloodNo[i], "%d", Damage->nDamage);
}
#endif

#ifndef _SERVER
int	KNpc::GetNpcPate()
{
	int nHeight = m_Height + m_nStature;
	if (m_Kind == kind_player)
	{
		if (m_nSex)
			nHeight += 84;	
		else
			nHeight += 84;	
		if (m_MaskType == 0)
		{
			if (m_Doing == do_sit && MulDiv(10, m_Frames.nCurrentFrame, m_Frames.nTotalFrame) >= 8)
				nHeight -= MulDiv(30, m_Frames.nCurrentFrame, m_Frames.nTotalFrame);
			
			if (m_bRideHorse)
				nHeight += 38;	
		}
		else
		{
			nHeight += 20;
		}
	}
	return nHeight;
}
#endif

#ifndef _SERVER
int	KNpc::GetNpcPatePeopleInfo()
{
	int nFontSize = 12;
	if (m_nChatContentLen > 0 && m_nChatNumLine > 0)
		return m_nChatNumLine * (nFontSize + 1);

	int nHeight = 0;
	if (NpcSet.CheckShowLife())
	{
		if (m_Kind == kind_player ||
			m_Kind == kind_partner)
		{
			if (m_CurrentLifeMax > 0 && (relation_enemy == NpcSet.GetRelation(m_Index, Player[CLIENT_PLAYER_INDEX].m_nIndex))
				&& Npc[m_Index].m_nPKFlag != enumPKMurder
				)
				nHeight += SHOW_LIFE_HEIGHT;
		}
	}
	if (NpcSet.CheckShowName())
	{
		if (nHeight != 0)
			nHeight += SHOW_SPACE_HEIGHT;

		if (m_Kind == kind_player || m_Kind == kind_dialoger)
			nHeight += nFontSize + 1;
	}
	return nHeight;
}
#endif

#ifndef _SERVER
BOOL KNpc::FindStateSkill(int nID)
{
	if (nID == 0)
		return FALSE;
	KStateNode* pNode;
	pNode = (KStateNode *)m_StateSkillList.GetTail();
	while(pNode)
	{
		if (pNode->m_SkillID == nID)
		{
			return TRUE;
		}
		pNode = (KStateNode *)pNode->GetPrev();
	}
	return FALSE;
}
#endif

#ifndef _SERVER
void KNpc::HideRes(int nType, bool bFlag)
{
	switch(nType)
	{
	case 1:
		m_DataRes.m_bHideNpc = bFlag;
		break;
	case 2:
		m_DataRes.m_bHidePlayer = bFlag;
		break;
	}
}

void KNpc::ChangeRecruit(int m_btRecruit)
{
	m_Recruit = m_btRecruit;
}
#endif

void KNpc::SetCurPlayerTitle(DWORD nRankId, DWORD overLook)//#PlayerTitle
{
	KTabFile pTab;
	pTab.Load(PLAYER_RANK_BATTLE_SETTING_TABFILE);
	DWORD nExpandRankId = 0;
	int nExtSkill, nExtSkillLevel;	
	if (nRankId && nRankId > 0)
	{
		m_btPlayerTitle = nRankId;
		nExpandRankId = nRankId + 1;
		pTab.GetInteger(nExpandRankId, "ExtSkill1", 0, &nExtSkill);
		pTab.GetInteger(nExpandRankId, "ExtSkillLevel1", 0, &nExtSkillLevel);
		pTab.Clear();
		if (nExtSkill && nExtSkillLevel)
		{
			KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nExtSkill, nExtSkillLevel);
			pSkill->CastStateSkill(Player[m_nPlayerIdx].m_nIndex, 0, 0, -1, overLook);
		}
		if(overLook == 1)
		{
			m_btPlayerTitle = 0; //remove
		}
	}
}

void KNpc::SetPlayerTitle(DWORD nRankId, DWORD nTime, DWORD overLook)//#PlayerTitle
{
	KTabFile pTab;
	pTab.Load(PLAYER_RANK_BATTLE_SETTING_TABFILE);
	if (nRankId && nRankId > 0)
	{
		m_btPlayerTitle = nRankId;
		Player[m_nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_DB_PLAYER_TITLE_ID, m_btPlayerTitle);
		DWORD nSaveTime = time(0) + nTime/18;
		Player[m_nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_DB_PLAYER_TITLE_TIME, nSaveTime);
		SetCurPlayerTitle(nRankId, overLook);
	}
}

void KNpc::SetRankBattle(DWORD nRankId, int nTime, int overLook)//#RankBattle
{
	KTabFile pTab;
	pTab.Load(PLAYER_RANK_BATTLE_SETTING_TABFILE);
	DWORD nExpandRankId = 0;
	int nExtSkill, nExtSkillLevel;	
	if (nRankId && nRankId > 0)
	{
		nExpandRankId = nRankId + 1;
		m_btRankBattleId = nRankId ;
		pTab.GetInteger(nExpandRankId, "ExtSkill1", 0, &nExtSkill);	
		pTab.GetInteger(nExpandRankId, "ExtSkillLevel1", 0, &nExtSkillLevel);
		pTab.Clear();
		if (nExtSkill && nExtSkillLevel)
		{
			KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(nExtSkill, nExtSkillLevel);
			pSkill->CastStateSkill(Player[m_nPlayerIdx].m_nIndex, 0, 0, nTime, overLook);
		}
		if(overLook == 1)
		{
			m_btRankBattleId = 0; //remove
		}
	}
}

BOOL KNpc::ExecuteScript(char * ScriptFileName, char * szFunName, int nParam)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript(g_FileName2Id(ScriptFileName), szFunName, nParam);	
}

BOOL KNpc::ExecuteScript(DWORD dwScriptId,  char * szFunName, int nParam)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_Index);
			pScript->SetGlobalName(SCRIPT_NPCINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_NPCID);
			
			Lua_PushNumber(pScript->m_LuaState, m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			int nTopIndex = 0;
			
			pScript->SafeCallBegin(&nTopIndex);
			if (pScript->CallFunction(szFunName,0, "d", nParam)) 
			{
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%d]!!!!!", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL KNpc::ExecuteScript(DWORD dwScriptId, char * szFunName, char *  szParams)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_Index);
			pScript->SetGlobalName(SCRIPT_NPCINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_NPCID);
			
			Lua_PushNumber(pScript->m_LuaState, m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			
			pScript->SafeCallBegin(&nTopIndex);
			
			if ( (!szParams) || !szParams[0]) 
			{
				if (pScript->CallFunction(szFunName, 0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName,0, "sd", szParams,0)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%d]!!!!!", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}


BOOL KNpc::ExecuteScript(char * ScriptFileName, char * szFunName, char *  szParams)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	return ExecuteScript(dwScriptId, szFunName, szParams);
}

BOOL	KNpc::ExecuteScript2(DWORD dwScriptId, char * szFunName, char *  szParams1, char *  szParams2)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		int nTopIndex = 0;
		
		if (pScript)
		{
			m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_Index);
			pScript->SetGlobalName(SCRIPT_NPCINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_NPCID);
			
			Lua_PushNumber(pScript->m_LuaState, m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			pScript->SafeCallBegin(&nTopIndex);
			
			if ( ((!szParams1) || !szParams1[0]) && ((!szParams2) || !szParams2[0])) 
			{
				if (pScript->CallFunction(szFunName, 0,""))
				{
					bExecuteScriptMistake = false;
				}
			}
			else
			{
				if (pScript->CallFunction(szFunName, 0, "ssd", szParams1, szParams2, 0)) 
				{
					bExecuteScriptMistake = false;
				}
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			m_ActionScriptID = 0;
			return FALSE;
		}
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%u]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KNpc::ExecuteScript2(DWORD dwScriptId,  char * szFunName, int nParam1, int nParam2)
{
	try
	{
		m_btTryExecuteScriptTimes = 0;
		bool bExecuteScriptMistake = true;
		int nTopIndex = 0;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(dwScriptId);
		if (pScript)
		{
			m_ActionScriptID = dwScriptId;
			Lua_PushNumber(pScript->m_LuaState, m_Index);
			pScript->SetGlobalName(SCRIPT_NPCINDEX);
			
			Lua_PushNumber(pScript->m_LuaState, m_dwID);
			pScript->SetGlobalName(SCRIPT_NPCID);

			Lua_PushNumber(pScript->m_LuaState, Npc[m_nLastDamageIdx].m_nPlayerIdx);//#can kiem tra
			pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

			Lua_PushNumber(pScript->m_LuaState, Npc[m_nLastDamageIdx].m_dwID);
			pScript->SetGlobalName(SCRIPT_PLAYERID);
			
			Lua_PushNumber(pScript->m_LuaState, m_SubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

			pScript->SafeCallBegin(&nTopIndex);
			if (pScript->CallFunction(szFunName, 0, "dd", nParam1, nParam2)) 
			{
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			m_bWaitingPlayerFeedBack = false;
			m_btTryExecuteScriptTimes = 0;
			m_ActionScriptID = 0;
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		printf("Exception Have Caught When Execute Script[%u]!!!!!\n", dwScriptId);
		m_bWaitingPlayerFeedBack = false;
		m_btTryExecuteScriptTimes = 0;
		m_ActionScriptID = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL	KNpc::ExecuteScript2(char * ScriptFileName, char * szFunName, int nParam1, int nParam2)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0]) return FALSE;
	return ExecuteScript2(g_FileName2Id(ScriptFileName), szFunName, nParam1, nParam2);	
}

BOOL	KNpc::ExecuteScript2(char * ScriptFileName, char * szFunName, char *  szParams1, char *  szParams2)
{
	if (!ScriptFileName || !ScriptFileName[0] || !szFunName  || !szFunName[0])
		return FALSE;
	DWORD dwScriptId = g_FileName2Id(ScriptFileName);
	return ExecuteScript2(dwScriptId, szFunName, szParams1, szParams2);
}

#ifndef _SERVER
void KNpc::ResetPathFind()
{
	g_ScenePlace.RemoveFlag();       
}
/*
void KNpc::OnRunByFPS(int nStep)
{
	if(m_CurrentRunSpeed <= 0 || nStep <= 0)
		return;
	int nSpeed = m_CurrentRunSpeed;
	if(nSpeed > 32)
		nSpeed = 32;
	int nTrueSpeed = (int)nSpeed/nStep;
	int nAddPerStep = (int)nSpeed % nStep;
	if(m_nPosShiftStep < nAddPerStep)
		nTrueSpeed++;
	m_nPosShiftStep++;
	if(m_nPosShiftStep >= nStep)
		m_nPosShiftStep = 0;
	ServeMove(nTrueSpeed);
}

void KNpc::OnWalkByFPS(int nStep)
{
	if(m_CurrentWalkSpeed <= 0 || nStep <= 0)
		return;
	int nSpeed = m_CurrentWalkSpeed;
	if(nSpeed > 32)
		nSpeed = 32;
	int nTrueSpeed = (int)nSpeed/nStep;
	int nAddPerStep = (int)nSpeed % nStep;
	if(m_nPosShiftStep < nAddPerStep)
		nTrueSpeed++;
	m_nPosShiftStep++;
	if(m_nPosShiftStep >= nStep)
		m_nPosShiftStep = 0;
	ServeMove(nTrueSpeed);
}
*/
#endif

void KNpc::UpdateGameTitle() // Added by TinKer for Faction and Level display
{
	// Port SimCity: bot co m_nPlayerIdx = 0 nen cong cu chan het. Cho bot di qua -
	// day la kenh DUY NHAT hien duoc mon phai cua bot (bang "Tin tuc" phia client gan
	// cung nCurFaction = -1 khi xem NGUOI KHAC, CoreShell.cpp:1141-1144, nguoi that cung vay).
	if (!IsPlayer() || (m_nPlayerIdx <= 0 && !m_btSimCityBot))
	{
		m_szGameTitle[0] = 0;
		return;
	}
	
		
	static const char* const FactionName[] = {
		"ThiÕu L©m",
		"Thiªn V­¬ng Bang",
		"§­êng M«n",
		"Ngñ §éc",
		"Nga My",
		"Thóy Yªn",
		"C¸i Bang",
		"Thiªn NhÉn",
		"Vâ §ang",
		"C«n L«n"
	};

	// Bot lay so mon phai tu KNpc::nFirstFaction (truong san co, VON DA duoc truyen xuong
	// client trong ca hai goi sync). Luu y nFirstFaction la BYTE va Init dat -1 => 255,
	// nen phep kiem <= 9 o duoi vua lam cong chan chi so mang vua loc gia tri "chua vao phai".
	int nFaction;
	if (m_btSimCityBot)
		nFaction = (nFirstFaction <= 9) ? (int)nFirstFaction : -1;
	else
		nFaction = Player[m_nPlayerIdx].m_cFaction.GetCurFactionNo();
	int nLevel = m_Level;

	if (nLevel > 0)
	{
		const char* szFactionName;
		if (nFaction >= 0 && nFaction <= 9)
		{
			szFactionName = FactionName[nFaction];
			sprintf(m_szGameTitle, "%s/Lv:%d", szFactionName, nLevel);
		}
		else
		{
			sprintf(m_szGameTitle, "Lv:%d", nLevel);
		}
	}
	else
	{
		m_szGameTitle[0] = 0;
	}
}

//---------------------------------------------------------------------------
// g_AutoLogWhoIdx: loc log chan doan theo TEN nhan vat, tra qua CHI SO NPC.
// Dat o day (khong o KCore.cpp) vi can mang Npc[]. Tu kiem chi so nen moi noi
// goi deu an toan, ke ca khi chi so am / vuot tran.
//---------------------------------------------------------------------------
int g_AutoLogWhoIdx(int nNpcIdx)
{
	if (!g_AutoLogOn())
		return 0;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	return g_AutoLogWho(Npc[nNpcIdx].Name);
}
