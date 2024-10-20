//-----------------------------------------------------------------------
//	Sword3 KNpc.cpp
//-----------------------------------------------------------------------
#include "KCore.h"
//#include <crtdbg.h>
#include "KNpcAI.h"
#include "KSkills.h"
#include "KObj.h"
#include "KObjSet.h"
#include "KMath.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KSubWorldSet.h"
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
#include "KThiefSkill.h"
#ifdef _STANDALONE
#include "KThiefSkill.cpp"
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

extern KLuaScript		*g_pNpcLevelScript;

#define	ATTACKACTION_EFFECT_PERCENT		60
#define	MIN_DOMELEE_RANGE				20
#define	MIN_BLURMOVE_SPEED				1
#define	MIN_JUMP_RANGE					0
#define	ACCELERATION_OF_GRAVITY			10
#define		SHOW_CHAT_WIDTH				24
#define		SHOW_CHAT_COLOR				0xffffffff
#define		SHOW_BLOOD_COLOR			0x00ff0000
#define		defMAX_SHOW_BLOOD_TIME		27
#define		defSHOW_BLOOD_MOVE_SPEED	2
#define		SHOW_LIFE_WIDTH				38
#define		SHOW_LIFE_HEIGHT			3
#define		SHOW_SPACE_HEIGHT			5
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

KNpc	Npc[MAX_NPC];
KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL][MAX_NPC_SERIES];

KNpc::KNpc()
{
#ifdef _SERVER
	m_AiSkillRadiusLoadFlag = 0;
#endif
	Init();
}

void KNpc::Init()
{
	memset(m_btStateInfo, 0, sizeof(m_btStateInfo));
	m_dwID = 0;
	m_Index = 0;
	m_nPlayerIdx = 0;
	m_ProcessAI = 1;
	m_Kind = kind_normal;
	m_Series = series_metal;
	m_Camp = camp_free;
	m_CurrentCamp = camp_free;
	m_Doing = do_stand;
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
	m_btRankId					= 0;
	m_btRankBattleId			= 0; //#RankBattle
	m_btPlayerTitle				= 0; //#PlayerTitle
	//ZeroMemory(MateName, sizeof(MateName)); //#MateName
	m_nMissionGroup = -1;
	m_ExItemId					= 0; // hanh trang
	m_ExBoxId					= 0; // ruong mo rong
	nRankInWorld				= 0;
	nRepute						= 0;//danh vong
	nFuYuan						= 0;//phuc duyen
	nPKValue					= 0;
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
#ifdef _SERVER
	m_pDropRate					= NULL;
	m_nTimeIdleValue		= 0;
	m_nTimeIdleCounter		 = 0;
	m_nTime_Ignorenegativestate = 0;	//3 gi©y lo¹i bá tr¹ng th¸i dÞ th­êng xuÊt ø bÊt diÔm
#endif

#ifndef _SERVER
	m_SyncSignal				= 0;
	m_sClientNpcID.m_dwRegionID	= 0;
	m_sClientNpcID.m_nNo		= -1;
	m_ResDir					= 0;
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
	POINT	POff[8] = 
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
	POINT	POff[8] = 
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
	POINT	POff[8] = 
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
	this->m_cDeathCalcExp.Active();
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

#ifdef _SERVER
	if (m_nNpcTimeout && g_SubWorldSet.GetGameTime() >= m_nNpcTimeout)
	{
		m_nNpcTimeout = 0;
		if (m_ActionScriptID)
		{
			ExecuteScript(m_ActionScriptID, "OnTimer", m_Index);//#idx cña npc hÕt thêi gian
		}
	}
	this->m_cDeathCalcExp.Active();
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
	// ³¬³öÍ¬²½¾àÀëÉ¾³ýNpc£¬9ÆÁ£¬32¸ö¸ñ×Ó


	m_DataRes.SetAction(m_ClientDoing);
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
		m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, TRUE);
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
	if (m_Command.CmdKind == do_none || m_bExchangeServer)
		return;

	if (nAI)
	{
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
			if (int nSkillIdx = m_SkillList.FindSame(m_Command.Param_X))
			{
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
			break;	
		case do_revive:
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
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
			DoStand();
			m_ProcessAI = 1;
			m_ProcessState = 1;
#ifndef _SERVER
			this->SetInstantSpr(enumINSTANT_STATE_REVIVE);
#endif
			break;
		case do_walk:
			if(m_RandMove.nTime > 0)
			Goto(m_Command.Param_X, m_Command.Param_Y);  //them moi tiktok
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
	}

#ifdef _SERVER
	if (!(g_SubWorldSet.GetGameTime() % (GAME_FPS * 3)))
	{
		UpdateNpcStateInfo();
	}
#endif		

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
			
			m_CurrentMana += m_CurrentManaReplenish;
			if (m_CurrentMana > m_CurrentManaMax)
				m_CurrentMana = m_CurrentManaMax;

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
				POINT	POff[8] = 
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
				KSkill * pOrdinSkill1 = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, nCurLevel);
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

							POINT	POff[8] = 
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
			UpdateNpcStateInfo();
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
	if (this->m_Kind == kind_normal)
		this->SetBlood(this->m_CurrentLife);
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
	POINT	POff[8] = 
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
			ExecuteScript2(DropRateScript, "DropRate", m_Index, Npc[m_nLastDamageIdx].m_nPlayerIdx); //#idx cña player tÊn c«ng cuèi cïng
		}
	}
}

void KNpc::OnDeath()
{
	if (WaitForFrame())
	{
		g_DebugLog("[DEATH] WaitForFrame TRUE");
		m_Frames.nCurrentFrame = m_Frames.nTotalFrame - 1;		
#ifndef _SERVER
		int		nTempX, nTempY;
		KObjItemInfo	sInfo;

		SubWorld[m_SubWorldIndex].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nTempX, &nTempY);
		sInfo.m_nItemID = 0;
		sInfo.m_nItemWidth = 0;
		sInfo.m_nItemHeight = 0;
		sInfo.m_nMoneyNum = 0;
		sInfo.m_nColorID = 0;
		sInfo.m_nGenre = 0;
		sInfo.m_nDetailType = 0;
		sInfo.m_nParticularType = 0;
		sInfo.m_nMovieFlag = 0;
		sInfo.m_nSoundFlag = 0;
		sInfo.m_dwNpcId1 = 0;
		sInfo.m_szName[0] = 0;
		ObjSet.ClientAdd(0, m_CorpseSettingIdx, 0, m_Dir, 0, nTempX, nTempY, sInfo);
		m_ProcessAI = 1;
#else
		if (IsPlayer())
		{
			if (Player[m_nPlayerIdx].m_dwDeathScriptId)
			{
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
								
				ExecuteScript2("\\script\\tinhnang\\datau\\danhquai.lua", "OnDeathMonsterDaTau", m_Index, m_nLastDamageIdx); //#nhiÖm vô d· tÈu ®¸nh qu¸i				
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
			//Not Finish
		}
	}
	else
	{
		g_DebugLog("[DEATH] WaitForFrame FALSE");
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
	if (m_Doing == do_hurt || m_Doing == do_death)
		return;

#ifdef _SERVER
	int giam_tho_thuong = 0;
	if (m_CurrentHitRecover <= 80)
		giam_tho_thuong = (m_CurrentHitRecover / 10) * 10;
	else
		giam_tho_thuong = 80;
	//
	if (!g_RandPercent(nHurtI / 2 + 50 - giam_tho_thuong / 2))
	{
		return;
	}
#endif
	m_Doing = do_hurt;
	m_ProcessAI	= 0;

#ifdef _SERVER
	m_Frames.nTotalFrame = m_HurtFrame * (100 - m_CurrentHitRecover) / 100;
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
	GetMpsPos(&NetCommand.nX, &NetCommand.nY);

	POINT	POff[8] = 
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
	m_Height = m_nHurtHeight * (m_Frames.nTotalFrame - m_Frames.nCurrentFrame - 1) / m_Frames.nTotalFrame;
	nX = nX + (m_nHurtDesX - nX) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;
	nY = nY + (m_nHurtDesY - nY) * m_Frames.nCurrentFrame / m_Frames.nTotalFrame;

	int nOldRegion = m_RegionIndex;
	//SetPos(nX, nY);
	CURREGION.DecRef(m_MapX, m_MapY, obj_npc);
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
#endif

	if (WaitForFrame())
	{
		g_DebugLog("[DEATH]On Hurt Finished");
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

		if (m_Frames.nTotalFrame <= 0)
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
		m_Frames.nCurrentFrame = 0;
		GetMpsPos(&m_DesX, &m_DesY);
#ifndef _SERVER
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
		if (IsPlayer())
			return;
		m_Frames.nTotalFrame = m_ReviveFrame;
		SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].DecRef(m_MapX, m_MapY, obj_npc);
		SubWorld[m_SubWorldIndex].NpcChangeRegion(m_RegionIndex, VOID_REGION, m_Index);	// spe 03/06/28
		m_Frames.nCurrentFrame = 0;
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

	POINT	POff[8] = 
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
		
	POINT	POff[8] = 
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

	if (Player[m_nPlayerIdx].CheckTrading())
		return;
	if (m_Doing == do_skill)
		return;

	ISkill * pSkill = GetActiveSkill();
	if(pSkill)
	{
		eSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();
		if (IsPlayer())
		{
			if (!m_FightMode)
				return;
			#ifdef _SERVER
			if (m_nPlayerIdx > 0)
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeAttack, FALSE);//#mµi mßn khi xuÊt skill
			#endif
		}
		if (m_SkillList.CanCast(m_ActiveSkillID, SubWorld[m_SubWorldIndex].m_dwCurrentTime))
		{
			switch (pSkill->CanCastSkill(m_Index, nX, nY))
			{
			case -1:
				#ifdef _SERVER
				if(nX == -1)
				{
					int nDesX, nDesY;
					Npc[nY].GetMpsPos(&nDesX, &nDesY);
					SendCommand(do_run, nDesX, nDesY);
					return;
				}
				#endif
				break;
			case 0:
				goto Exit;
				break;
			case 1:
				if( m_Kind != kind_player || Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this)))
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
					
					POINT	POff[8] = 
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
			int nTotalFrame = m_AttackFrame * MAX_PERCENT / (m_CurrentAttackSpeed + MAX_PERCENT);
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
			int nTotalFrame = m_CastFrame * MAX_PERCENT / (m_CurrentCastSpeed + MAX_PERCENT);
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

	POINT	POff[8] = 
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

	POINT	POff[8] = 
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
BOOL KNpc::CalcDamage(int nAttacker, int nMin, int nMax, DAMAGE_TYPE nType, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, BOOL bReturn /* = FALSE */, int nFiveElements_DamageP /*0*/, int nStolen_Life/*0*/, int nStolen_Mana/*0*/, int nStolen_Stamina/*0*/, BOOL bIsDS /*= FALSE*/, BOOL bIsFS /*= FALSE*/)
{
	if (m_Doing == do_death || m_Doing == do_revive || m_RegionIndex < 0)
		return FALSE;
	//
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
				nDamage = nDamage * MAX_DEATLY_STRIKE_ENHANCEP / MAX_PERCENT;
			if (bIsFS) //ChÝ tö
				nDamage = m_CurrentLife / MAX_PERCENT * GetRandomNumber(MIN_FATALLY_STRIKE_ENHANCEP, MAX_FATALLY_STRIKE_ENHANCEP);

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
		if (nDamage <= 0)
			return FALSE;
		//
		if(nFiveElements_DamageP > MAX_PERCENT)
			nFiveElements_DamageP = MAX_PERCENT;
		//--TÝnh Damage Min Max Ngò Hµnh T­¬ng Kh¾c Tinh khang Ngu Hanh Tuong Khac NKTK
		if ((nMissleSeries == series_metal && m_Series == series_wood) ||
			(nMissleSeries == series_water && m_Series == series_fire) ||			//Nga my ®¸nh vµo c¸i bang
			(nMissleSeries == series_wood && m_Series == series_earth) ||
			(nMissleSeries == series_fire && m_Series == series_metal) ||
			(nMissleSeries == series_earth && m_Series == series_water))
		{
			nDamage += nDamage * nFiveElements_DamageP / MAX_PERCENT;
			nDamage -= (Npc[nAttacker].m_CurrentFiveElementsEnhance - m_CurrentFiveElementsResist);		//TÝnh Damage Ngò Hµnh C­êng Ho¸ - Nh­îc Ho¸
		}
		else if ((nMissleSeries == series_metal && m_Series == series_fire) ||
				 (nMissleSeries == series_water && m_Series == series_earth) ||
				 (nMissleSeries == series_wood && m_Series == series_metal) ||
				 (nMissleSeries == series_fire && m_Series == series_water) ||			//C¸i bang ®¸nh vµo nga my
				 (nMissleSeries == series_earth && m_Series == series_wood))
		{
			nDamage -= nDamage * nFiveElements_DamageP / MAX_PERCENT;
			nDamage -= (m_CurrentFiveElementsResist - Npc[nAttacker].m_CurrentFiveElementsEnhance);		//TÝnh Damage Ngò Hµnh C­êng Ho¸ - Nh­îc Ho¸
		}
		//--End TÝnh Damage Min Max Ngò Hµnh T­¬ng Kh¾c Tinh khang Ngu Hanh Tuong Khac NKTK
		if (nRate > MAX_RESIST)
		{
			nRate = MAX_RESIST;
		}
		//
		switch(nType)
		{
			case damage_physics:
				nRate = m_CurrentPhysicsResist;
				if (nRate > m_CurrentPhysicsResistMax)
				{
					nRate = m_CurrentPhysicsResistMax;
				}
				m_PhysicsArmor.nValue[0] -= nDamage;
				if (m_PhysicsArmor.nValue[0] < 0)
				{
					nDamage = -m_PhysicsArmor.nValue[0];
					m_PhysicsArmor.nValue[0] = 0;
					m_PhysicsArmor.nTime = 0;
				}
				else
				{
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
				nRate = m_CurrentColdResist;
				if (nRate > m_CurrentColdResistMax)
				{
					nRate = m_CurrentColdResistMax;
				}			
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
				nRate = m_CurrentFireResist;
				if (nRate > m_CurrentFireResistMax)
				{
					nRate = m_CurrentFireResistMax;
				}
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
				nRate = m_CurrentLightResist;
				if (nRate > m_CurrentLightResistMax)
				{
					nRate = m_CurrentLightResistMax;
				}
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
				nRate = m_CurrentPoisonResist;
				if (nRate > m_CurrentPoisonResistMax)
				{
					nRate = m_CurrentPoisonResistMax;
				}
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
		//
		if (this->m_Kind == kind_player && nType != damage_magic) //TÝnh damage gi÷a hai player kÎ bÞ tÊn c«ng vµ kÎ tÊn c«ng
		{
			if (Npc[nAttacker].m_Kind == kind_player)
			{
				nDamage = nDamage * NpcSet.m_nPKDamageRate / MAX_PERCENT;
			}
		}
		//
		if (nDamage <= 0)
			return FALSE;
		//--TÝnh Kh¸ng Ngò Hµnh T­¬ng Kh¾c Tinh khang Ngu Hanh Tuong Khac NKTK
		if ((nMissleSeries == series_metal && m_Series == series_wood) ||
				(nMissleSeries == series_water && m_Series == series_fire) ||		//Nga my ®¸nh vµo c¸i bang
				(nMissleSeries == series_wood && m_Series == series_earth) ||
				(nMissleSeries == series_fire && m_Series == series_metal) ||
				(nMissleSeries == series_earth && m_Series == series_water))
		{
			//nRate -= nFiveElements_DamageP;
			nDamage -= (Npc[nAttacker].m_CurrentFiveElementsEnhance - m_CurrentFiveElementsResist);		//TÝnh Damage Ngò Hµnh C­êng Ho¸ - Nh­îc Ho¸
		}
		else if ((nMissleSeries == series_metal && m_Series == series_fire) ||
				 (nMissleSeries == series_water && m_Series == series_earth) ||
				 (nMissleSeries == series_wood && m_Series == series_metal) ||
				 (nMissleSeries == series_fire && m_Series == series_water) ||			//C¸i bang ®¸nh vµo nga my
				 (nMissleSeries == series_earth && m_Series == series_wood))
		{
			//nRate += nFiveElements_DamageP;
			nDamage -= (m_CurrentFiveElementsResist - Npc[nAttacker].m_CurrentFiveElementsEnhance);		//TÝnh Damage Ngò Hµnh C­êng Ho¸ - Nh­îc Ho¸
		}
		//--End TÝnh Kh¸ng Ngò Hµnh T­¬ng Kh¾c Tinh khang Ngu Hanh Tuong Khac NKTK
		if (nRate > MAX_RESIST)
		{
			nRate = MAX_RESIST;
		}
		//
		if (nType != damage_poison &&m_ManaShield.nValue[0] > 0) //Noi luc ho than vo dang
		{
			int nManaDamage = nDamage * m_ManaShield.nValue[0] / MAX_PERCENT;
			m_CurrentMana -= nManaDamage;
			nRealDamage += nManaDamage;
			if (m_CurrentMana < 0)
			{
				nDamage -= m_CurrentMana;
				nRealDamage += m_CurrentMana;
				m_CurrentMana = 0;
			}
			else
			{
				nDamage -= nManaDamage;
			}
		}
		//
		if (m_CurrentManaShield > 0)  //#giam thieu sat thuong ganh chiu HTVC con lon
		{
			nDamage -= m_CurrentManaShield / 3; //chia lµm 3 v× HTVC hiÖn t¹i ®ang m¹nh
		}
		//
		if(m_CurrentSorbDamageP)	//triet tieu sat thuong %
		{
			nDamage -= (nDamage*m_CurrentSorbDamageP) / MAX_PERCENT;
		}
		//
		nDamage -= nDamage * nRate / MAX_PERCENT;
		//
		if(nDamage <= 0) //#fix loi damge skill danh vao nguoi khong len damge khong co hieu ung edit by phong kieu //#can kiem tra
		{
			nDamage = 1 + g_Random(10);
		}		
		//
		if (nDamage <= 0)
		{
			return FALSE;
		}
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
							nMin = nDamage * m_CurrentPoisonDamageReturnPercent / MAX_PERCENT;
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
				if (nDamage > 0)
				{
					if (bIsMelee)
					{
						nMin = m_CurrentMeleeDmgRet;
						nMin += nDamage * nMax / MAX_PERCENT;
						if (nMin > 0)
						{
							if (Npc[nAttacker].m_CurrentReturnResPercent > 0)
								nMin -= nMin *  Npc[nAttacker].m_CurrentReturnResPercent  / MAX_PERCENT;
							Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, damage_magic, -1, FALSE, FALSE, TRUE);
						}
					}
					else
					{
						nMin = m_CurrentRangeDmgRet;
						nMin += nDamage * nMax / MAX_PERCENT;
						if (nMin > 0)
						{
							if (Npc[nAttacker].m_CurrentReturnResPercent)
									nMin -= nMin * - Npc[nAttacker].m_CurrentReturnResPercent   / MAX_PERCENT;
								Npc[nAttacker].CalcDamage(m_Index, nMin, nMin, damage_magic, -1, FALSE, FALSE, TRUE);
						}
					}
				}
			}
		}
		//
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

		if(g_Skill120ExpRate) // luyen skill 120
		{
			int calExpSkill = 1 * g_Skill120ExpRate / MAX_PERCENT;
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

		if(g_Skill90ExpRate) // luyen skill 90
		{
			int calExpSkill = 1 * g_Skill90ExpRate / MAX_PERCENT;
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

		Npc[nAttacker].m_CurrentLife += nDamage * nStolen_Life / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentLife > Npc[nAttacker].m_CurrentLifeMax)
			Npc[nAttacker].m_CurrentLife = Npc[nAttacker].m_CurrentLifeMax;

		Npc[nAttacker].m_CurrentMana += nDamage * nStolen_Mana / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentMana > Npc[nAttacker].m_CurrentManaMax)
			Npc[nAttacker].m_CurrentMana = Npc[nAttacker].m_CurrentManaMax;

		Npc[nAttacker].m_CurrentStamina += nDamage * nStolen_Stamina / MAX_PERCENT;
		if (Npc[nAttacker].m_CurrentStamina > Npc[nAttacker].m_CurrentStaminaMax)
			Npc[nAttacker].m_CurrentStamina = Npc[nAttacker].m_CurrentStaminaMax;

		m_CurrentMana += nDamage * m_CurrentDamage2Mana / MAX_PERCENT;
		if (m_CurrentMana > m_CurrentManaMax)
			m_CurrentMana = m_CurrentManaMax;
	}
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
	if (nRealDamage > 0 && (this->m_Kind == kind_player) && (Npc[nAttacker].m_Kind == kind_player))
	{
		if (Player[Npc[nAttacker].m_nPlayerIdx].m_dwDamageScriptId)
			Player[m_nPlayerIdx].ExecuteScript(Player[m_nPlayerIdx].m_dwDamageScriptId, "OnDamage", nRealDamage);
	}
	return TRUE;
}

BOOL KNpc::ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate)
{
	if (nLauncher <= 0 || nLauncher >= MAX_NPC)
		return FALSE;

	if (!m_Index || !Npc[nLauncher].m_Index)
		return FALSE;

	if (!pData)
		return FALSE;
		
	if (m_Doing == do_death || m_Doing == do_revive)
		return TRUE;

	if (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)
		return TRUE;

	KMagicAttrib *pTemp = NULL;

	pTemp = (KMagicAttrib *)pData;
	int nAr = pTemp->nValue[0]; //attackrating[0]	//§é chÝnh x¸c

	pTemp++; 
	if (bUseAR)
	{
		int nIgnoreAr = pTemp->nValue[0]; //ignoredefense[1]	//NÐ tr¸nh
		if (!CheckHitTarget(nAr, m_CurrentDefend, nIgnoreAr))	
			return FALSE;
	}

	pTemp++; 
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
	int nStolenLifeP = pTemp->nValue[0]; //steallife[6]
	
	pTemp++;
	int nStolenManaP = pTemp->nValue[0]; //stealmana[7]

	pTemp++;
	int nStolenStaminaP = pTemp->nValue[0]; //stealstamina[8]

	pTemp++; //physics damage[9]
	CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_physics, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS);

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
		if (m_PoisonState.nTime <= 0)
		{
			if (m_CurrentPoisonTimeReducePercent > MAX_REDUCE)
				m_PoisonState.nTime = pTemp->nValue[1] / 4;
			else
				m_PoisonState.nTime = pTemp->nValue[1] - pTemp->nValue[1] * m_CurrentPoisonTimeReducePercent / MAX_PERCENT;
			m_PoisonState.nValue[0] = pTemp->nValue[0];
			m_PoisonState.nValue[1] = pTemp->nValue[2];
			m_PoisonState.nMagicAttrib = 0;
		}
		else
		{
			int d1, d2, t1, t2, c1, c2;
			d1 = m_PoisonState.nValue[0];
			d2 = pTemp->nValue[0];
			t1 = m_PoisonState.nTime;
			if (m_CurrentPoisonTimeReducePercent > MAX_REDUCE)
				t2 = pTemp->nValue[1] / 4;
			else
				t2 = pTemp->nValue[1] - pTemp->nValue[1] * m_CurrentPoisonTimeReducePercent / MAX_PERCENT;
			c1 = m_PoisonState.nValue[1];
			c2 = pTemp->nValue[2];
			if (c1 > 0 && c2 > 0 && d1 > 0 && d2 > 0) 
			{
				m_PoisonState.nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
				m_PoisonState.nTime = (t1 * d1 * c2 + t2 * d2 * c1) / (d1 * c2 + d2 * c1);
				m_PoisonState.nValue[1] = (c1 + c2) / 2;
			}
		}
	}

	pTemp++; //stun[14]
	if (m_StunState.nTime <= 0)
	{
		if (g_RandPercent(pTemp->nValue[0]))
		{
			if (m_CurrentStunTimeReducePercent > MAX_REDUCE)
				m_StunState.nTime = pTemp->nValue[1] / 4;
			else
				m_StunState.nTime = pTemp->nValue[1] - pTemp->nValue[1] * m_CurrentStunTimeReducePercent / MAX_PERCENT;
			if (m_StunState.nTime > 0)
				UpdateNpcStateInfo();
		}
	}

	//ignorenegativestate_p[15] //Lo¹i bá tr¹ng th¸i dÞ th­êng xuÊt ø bÊt diÔm
	pTemp++; 
	if(g_RandPercent(nMissRate))
	{
		this->ClearNormalState();
		this->IgnoreState(TRUE);
		m_nTime_Ignorenegativestate = pTemp->nValue[1]; //3*18; // 3 gi©y
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
		ZeroMemory(&m_RandMove, sizeof(m_RandMove));	//Tr¹ng th¸i ho¶ng lo¹n skill 120 thiªn nhÉn
		this->IgnoreState(TRUE);
	}

	pTemp++; //randmove[16]
	if (pTemp->nValue[0] && pTemp->nValue[1] && g_RandPercent(nMissRate))
		return FALSE;

	if (g_RandPercent(nDoHurtP))
		DoHurt();

	int	kpRelation = NpcSet.GetRelation(m_Index, nLauncher);//Add by Phong KiÒu fix lçi Npc Set Skill5 kh«ng nhËn diÖn ®­îc m_nPeopleIdx
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
		pDes->nAttribType = magic_magicdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);
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
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);

		if (!bIsPhysical)
	    {
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentAddPhysicsMagic;
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentAddPhysicsMagic;
	    }
	}
	pTemp++; //cold damage[10]
	pDes++;
	if (pTemp->nAttribType == magic_colddamage_v)
	{
		pDes->nAttribType = magic_colddamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[1] = pTemp->nValue[1] + m_CurrentColdEnhance;
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);
	
		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[0];
			pDes->nValue[1] = max(pDes->nValue[1], m_CurrentColdMagic.nValue[1] + m_CurrentColdEnhance);
			pDes->nValue[2] += m_PhysicsMagic.nValue[0] + m_CurrentColdMagic.nValue[2];
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
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);

		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentFireMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentFireMagic.nValue[2];
		} 
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentFireDamage.nValue[0];
		pDes->nValue[2] += m_CurrentFireDamage.nValue[2];
	}
	pTemp++; //lighting damage[12]
	pDes++;
	if (pTemp->nAttribType == magic_lightingdamage_v)
	{
		pDes->nAttribType = magic_lightingdamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[2] - pTemp->nValue[0]) * m_CurrentLightEnhance / MAX_PERCENT;
		pDes->nValue[2] = pTemp->nValue[2] + (pTemp->nValue[2] * nAddDamageP / MAX_PERCENT);

		if (!bIsPhysical)
		{
			pDes->nValue[0] += m_PhysicsMagic.nValue[0] + m_CurrentLightMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2] + m_CurrentLightMagic.nValue[2];
		}
	
	}
	if (bIsPhysical)
	{
		pDes->nValue[0] += m_CurrentLightDamage.nValue[0];
		pDes->nValue[2] += m_CurrentLightDamage.nValue[2];
	}
	pTemp++; //poison damage[13]
	pDes++;
	if (pTemp->nAttribType == magic_poisondamage_v)
	{
		pDes->nAttribType = magic_poisondamage_v;
		pDes->nValue[0] = pTemp->nValue[0] + (pTemp->nValue[0] * nAddDamageP / MAX_PERCENT);
		pDes->nValue[1] = pTemp->nValue[1];
		pDes->nValue[2] = pTemp->nValue[2] * (MAX_PERCENT - m_CurrentPoisonEnhance) / MAX_PERCENT;
		if (pDes->nValue[2] <= 0)
			pDes->nValue[2] = 1;

		if (!bIsPhysical)
		{
			g_NpcAttribModify.MixPoisonDamage(pDes, &m_CurrentPoisonMagic);
			pDes->nValue[0] += m_PhysicsMagic.nValue[0];
			pDes->nValue[2] += m_PhysicsMagic.nValue[2];
		}
	}
	if (bIsPhysical)
		g_NpcAttribModify.MixPoisonDamage(pDes, &m_CurrentPoisonDamage);
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
	if (m_Doing != do_walk && m_Doing != do_run && m_Doing != do_hurt && m_Doing != do_runattack)
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

#ifndef _SERVER
	if(nRet == 1)
	{
		x = g_DirCos(m_Dir, 64) * MoveSpeed;
		y = g_DirSin(m_Dir, 64) * MoveSpeed;
	}
	else if (nRet == 0)
	{
		DoStand(); //®øng l¹i
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
//	´¦ÀíNPCµÄ×ø±ê±ä»Ã
//	CELLWIDTH¡¢CELLHEIGHT¡¢OffX¡¢OffY¾ùÊÇ·Å´óÁË1024±¶

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

	if (m_RegionIndex == -1)	// ²»¿ÉÄÜÒÆ¶¯µ½-1 Region£¬Èç¹û³öÏÖÕâÖÖÇé¿ö£¬»Ö¸´Ô­×ø±ê
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
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove, FALSE); //mµi mßn khi di chuyÓn
			}
		}
#else
		SubWorld[0].NpcChangeRegion(SubWorld[0].m_Region[nOldRegion].m_RegionID, SubWorld[0].m_Region[m_RegionIndex].m_RegionID, m_Index);
		m_dwRegionID = SubWorld[0].m_Region[m_RegionIndex].m_RegionID;
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
				Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeMove, FALSE); //#mµi mßn khi nh¶y
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

	if(cmd == do_run)
	{
		if ((m_CurrentStamina < 18 && m_nPKFlag == enumPKMurder) || (m_CurrentStamina < 8 && m_nPKFlag == enumPKTongWar)) //#chua hoan thien
			cmd = do_walk;
	}

	m_Command.CmdKind = cmd;
	m_Command.Param_X = x;
	m_Command.Param_Y = y;
	m_Command.Param_Z = z;
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
		
		POINT	POff[8] = 
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
		
	POINT	POff[8] = 
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
		g_NpcSetting.GetInteger(nNpcSettingIdx + 2, "ActiveRadius", 12, &m_ActiveRadius);		//B¸n kÝnh ho¹t ®éng
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

VOID KNpc::ClientGotoPos(INT nX, INT nY, INT nMode /* = 0 */)
{
	// Èç¹ûÊÍ·ÅµÄÊÇÇá¹¦£¬ÔòÈ¡ÏûÏÂÒ»´ÎµÄ¶¯×÷£¬·ÀÖ¹Çá¹¦Á¬Ðø°´£¬ÖÐÍ¾²»ÄÜÈ¡ÏûÇá¹¦µÄÎÊÌâ
	//if (m_nNextActiveSkillId == 10)
	//{
	//	m_nNextActiveSkillId = 0;
	//	m_nNextActiveSkillPosX = 0;
	//	m_nNextActiveSkillPosY = 0;
	//}
	//m_cMoveQueue.Clear();

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

#ifdef _SERVER
BOOL KNpc::SendSyncData(int nClient)	//Sync npc vµ player tõ Server vÒ Client 1 lÇn
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
	POINT	POff[8] = 	//MAX_PLAYER
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
	int nMaxCount = MAX_PLAYER;//MAX_BROADCAST_COUNT;
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
		PlayerSync.MantleType		= (BYTE)m_MantleType; 
		PlayerSync.MaskType			= m_MaskType;		//#mat na
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
		PlayerSync.ReBorn			= (BYTE)Player[m_nPlayerIdx].m_cReBorn.GetReBornValue();
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
		int nMaxCount = MAX_PLAYER;	//MAX_BROADCAST_COUNT;
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

	POINT	POff[8] = 
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
    GetMpsPos(&nX, &nY);
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
    m_ImageSeries.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT; //xem ë chÕ ®é 3d
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
	KIniFile pIni;
	KTabFile pTab;
	char Buff[128], cbBuffer[32];
	int nMpsX, nMpsY, nMX, nMY, nNumFrames;
	GetMpsPos(&nMpsX, &nMpsY);
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
		strcpy(szString, Name);
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
		g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(Name) / 4, nMpsY, dwColor, 0, nHeightOff, dwBorderColor);
		nHeightOffset += nFontSize + 1;

		nX = nMpsX - nFontSize*g_StrLen(Name)/4;
		nY = nMpsY;
		//
		if (m_szTeamMem[0])//VÏ th«ng tin tõ auto lªn ®Çu nh©n vËt
		{
			sprintf(m_szTongName, "%s", m_szTeamMem);
		}
		//
		if (m_szTongName[0])
		{
			char szTong[64];
			if(m_szTongTitle[0] == 0 || NULL == m_szTongTitle)
			{
				strcpy(szTong, m_szTongName);
				if(!m_szTeamMem[0])//fix by phong kiÒu kh«ng vÏ TongFigure khi hiÓn thÞ th«ng tin auto
				{
					switch(m_nFigure)
					{
					case enumTONG_FIGURE_MEMBER:
						strcat(szTong," M«n ®Ö ");
						break;
					case enumTONG_FIGURE_MANAGER:
						strcat(szTong," §­êng chñ ");
						break;
					case enumTONG_FIGURE_DIRECTOR:
						strcat(szTong," Tr­ëng l·o ");
						break;
					case enumTONG_FIGURE_MASTER:
						strcat(szTong," Bang chñ ");
						break;
					}
				}
			}
			else
			{
				if(!m_szTeamMem[0])//fix by phong kiÒu kh«ng vÏ TongTitle khi hiÓn thÞ th«ng tin auto
					sprintf(szTong, "%s %s ", m_szTongName, m_szTongTitle);
				else
					strcpy(szTong, m_szTongName); 
			}
					
			g_pRepresent->OutputText(nFontSize, szTong, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(szTong) / 4, nMpsY - 30, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
			
		}
		//m_btHonorId = 20;
		if(m_btHonorId > 0)	 //#danh hieu
		{
			memset(Buff, 0, sizeof(Buff));
			memset(cbBuffer, 0, sizeof(cbBuffer));
			itoa(m_btHonorId, cbBuffer, 10);
			pTab.Load(HONOR_SETTING_FILE);
			pTab.GetString(cbBuffer, "HONORLINK", "",Buff,sizeof(Buff));
			pTab.GetInteger(cbBuffer, "HONORX", 0, &nMX);
			pTab.GetInteger(cbBuffer, "HONORY", 0, &nMY);
			pTab.GetInteger(cbBuffer, "TOTALFRAME", 0, &nNumFrames);
			pTab.GetInteger(cbBuffer, "WIGTH", 0, &nMW);
			pTab.Clear();

			KRUImage RURank;
			RURank.nType = ISI_T_SPR;
			RURank.Color.Color_b.a = 255;
			RURank.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RURank.uImage = 0;
			RURank.nISPosition = IMAGE_IS_POSITION_INIT;
			RURank.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RURank.szImage, Buff);
			RURank.nFrame = g_SubWorldSet.GetGameTime() / 2 % nNumFrames;

			RURank.oPosition.nX = nX - nMX;
			RURank.oPosition.nY = nY - nMY;
			RURank.oPosition.nZ = nHeightOffset;
			g_pRepresent->DrawPrimitives(1, &RURank, RU_T_IMAGE, 0);		
		}

		if (m_MaskType > 0)	//#mat na
		{
			memset(Buff, 0, sizeof(Buff));
			pIni.Load(NPC_BOBO_FILE);
			if (m_bRideHorse)
				pIni.GetString("Actions", "0", "", Buff, sizeof(Buff));
			else if (m_Doing == do_sit)
				pIni.GetString("Actions", "1", "", Buff, sizeof(Buff));
			pIni.Clear();
			KRUImage RUIconImage;
			RUIconImage.nType = ISI_T_SPR;
			RUIconImage.Color.Color_b.a = 255;
			RUIconImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			RUIconImage.uImage = 0;
			RUIconImage.nISPosition = IMAGE_IS_POSITION_INIT;
			RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(RUIconImage.szImage, Buff);
			RUIconImage.oPosition.nX = nMpsX + (nFontSize*g_StrLen(Name)/4) + 10 + (m_btHonorId > 0 ? 10 : 0);
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
			if (m_bRideHorse)
			{
				RUIconImage.oPosition.nY = nMpsY-(nHeightOff*1.48) - 22;
			}
			else
			{
				RUIconImage.oPosition.nY = nMpsY-(nHeightOff*1.48) - 12;
			}
			RUIconImage.oPosition.nZ = 0;
			RUIconImage.nFrame = m_nFrame;
			g_pRepresent->DrawPrimitives(1, &RUIconImage, RU_T_IMAGE, FALSE);			
		}

		if (m_btPlayerTitle || m_btRankBattleId) //#PlayerTitle //#RankBattle
		{
			DWORD nRankId;
			int nColor = 3;
			if(m_btRankBattleId)
			{
				pTab.Load(PLAYER_RANK_BATTLE_SETTING_TABFILE);
				nRankId = m_btRankBattleId + 1;
				pTab.GetString(nRankId, "TitleName",	"", szString, sizeof(szString));
				dwColor = pColor[nColor];
				pTab.Clear();
			}
			else if(m_btPlayerTitle)
			{
				pTab.Load(PLAYER_RANK_BATTLE_SETTING_TABFILE);
				nRankId = m_btPlayerTitle + 1;
				pTab.GetString(nRankId, "TitleName",	"", szString, sizeof(szString));
				dwColor = pColor[nColor];
				pTab.Clear();
			}

			nX = nMpsX - nFontSize*g_StrLen(szString)/4;
			nY = nMpsY - 24;
			if (m_szTongName[0] || m_szTeamMem[0])
				nY -= 30;
			g_pRepresent->OutputText(nFontSize, szString, KRF_ZERO_END, nX, nY, dwColor, 0, nHeightOff, dwBorderColor);
			nHeightOffset += nFontSize + 1;
		}

		if (m_BaiTan) //#bµy b¸n
		{
			int	nMpsX, nMpsY;
			GetMpsPos(&nMpsX, &nMpsY);
			int nWid = nFontSize * g_StrLen(ShopName) / 2 + 10;
			int nHei = nFontSize + 12;
				
			char*sOutm = "\\Spr\\Ui3\\°ÚÌ¯\\°ÚÌ¯Í·¶¥Ìõ£­ÖÐ.spr"; //Head
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

			if ((m_btPlayerTitle || m_btRankBattleId) && (m_szTongName[0] || m_szTeamMem[0]))//#PlayerTitle
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 110, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else  if (m_szTongName[0] || m_szTeamMem[0])
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 88, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else if (m_btPlayerTitle || m_btRankBattleId)
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 88, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);
			else
				g_pRepresent->OutputText(nFontSize, ShopName, KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(ShopName) / 4, nMpsY - 62, 0xffebb200, 0, nHeightOff - 11, dwBorderColor);

			char* sOutR = "\\Spr\\Ui3\\°ÚÌ¯\\°ÚÌ¯Í·¶¥Ìõ£­ÓÒ.spr";	//Head
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
			
			char* sOutL = "\\Spr\\Ui3\\°ÚÌ¯\\°ÚÌ¯Í·¶¥Ìõ£­×ó.spr";	//Head
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
		GetMpsPos(&nX, &nY);

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
			RUIconImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;	//xem ë chÕ ®é 3d
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
		GetMpsPos(&nPosX, &nPosY);
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
		GetMpsPos(&nPosX, &nPosY);
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

int	KNpc::PaintChat(int nHeightOffset)
{
	if (m_Kind != kind_player)
		return nHeightOffset;
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

	GetMpsPos(&nMpsX, &nMpsY);
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
	if(nMsgLength >= 90)//edit by phong kieu xu ly chat lon hon 90 ky tu
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
			TGetLimitLenEncodedString(szChatBuffer, nOffset, nFontSize, SHOW_CHAT_WIDTH,
				m_szChatBuffer, m_nChatContentLen, 2, true);

			m_nChatNumLine = TGetEncodedTextLineCount(m_szChatBuffer, m_nChatContentLen, SHOW_CHAT_WIDTH, m_nChatFontWidth, nFontSize, 0, 0, true);
			if (m_nChatNumLine >= 2)
				m_nChatNumLine = 2;
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
	GetMpsPos(&nMpsX, &nMpsY);
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
	GetMpsPos(&nMpsX, &nMpsY);
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

	if (Option.GetLow(LowEstPlayer))//add by phong kiÒu low npc low player
	{
		if (m_Kind == kind_player) 
		{
			//if (m_Index != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				bPaintBody = FALSE;
			}
		}
	}

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
#endif

void	KNpc::SetSeries(int nSeries)
{
	if (nSeries >= series_metal && nSeries < series_num)
		m_Series = nSeries;
	else
		m_Series = series_metal;
}

void KNpc::SetStateSkillEffect(int nLauncher, int nSkillID, int nLevel, void *pData, int nDataNum, int nTime/* = -1*/, BOOL bOverLook/* = FALSE*/)
{
	if (nLevel <= 0 || nSkillID <= 0) return ;
	//
	_ASSERT(nSkillID < MAX_SKILL && nLevel < MAX_SKILLLEVEL);
	KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(nSkillID, nLevel);
	//
	_ASSERT(nDataNum < MAX_SKILL_STATE);
	if (nDataNum >= MAX_SKILL_STATE)
	nDataNum = MAX_SKILL_STATE;
#ifdef _SERVER
	if (pData && nDataNum >= 0)
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
				pNode->m_bOverLook = bOverLook;
				pNode->m_bTempStateGraphics = FALSE;
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
	AddStateSkillList(pNode);
	//
#ifdef _SERVER
	UpdateNpcStateInfo();
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

	if (nPercent > MAX_HIT_PERCENT)
		nPercent = MAX_HIT_PERCENT;

	if (nPercent < MIN_HIT_PERCENT)
		nPercent = MIN_HIT_PERCENT;

	BOOL bRet = g_RandPercent(nPercent);
//	g_DebugLog("[ÊýÖµ]AttackRating %d : Defense %d: RandomPercent (%d, %d)", nAR, nDf, nPercent, bRet);
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

	if (nPercent < 40)
		nPercent = 40;

	BOOL bRet = g_RandPercent(nPercent);
	return bRet;
}

void KNpc::AddStateSkillList(KStateNode * pNewNode)
{
	BOOL bFound = FALSE;
	KStateNode	*pNode;
	pNode = (KStateNode*)m_StateSkillList.GetTail();
	while (pNode)
	{	
		if (pNode->m_SkillID == pNewNode->m_SkillID)
		{
			bFound = TRUE;
			break;	
		}
		pNode = (KStateNode*)pNode->GetPrev();
	}
	// Neu nhu tim thay thi thoat
	if (bFound)
		return;
	m_StateSkillList.AddTail(pNewNode);
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
		m_HitRecover			= pNpcTemp->m_HitRecover;
		m_ReviveFrame			= pNpcTemp->m_ReviveFrame;
		m_Experience			= pNpcTemp->m_Experience;
		m_CurrentExperience		= m_Experience;
		m_LifeMax					= pNpcTemp->m_LifeMax;
		m_LifeReplenish			= pNpcTemp->m_LifeReplenish;
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
				double nSubExp;
				if (m_Level <= 90)
					nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level) / 100) * 0.9; 
				else
					nSubExp = (PlayerSet.m_cLevelAdd.GetLevelExp(m_Level) / 100) * 0.33;

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
			Player[m_nPlayerIdx].m_ItemList.Abrade(enumAbradeDefend, TRUE);//#mµi mßn  phßng thñ
			int		nPKValue;
			nPKValue = Player[this->m_nPlayerIdx].m_cPK.GetPKValue();
			if (nPKValue < 0)
				nPKValue = 0;
			if (nPKValue > MAX_DEATH_PUNISH_PK_VALUE)
				nPKValue = MAX_DEATH_PUNISH_PK_VALUE;

			double		nLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(m_Level);
			double 	nExpTru = nLevelExp / 100 * PlayerSet.m_sPKPunishParam[nPKValue].m_nExpP;
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

			Player[m_nPlayerIdx].m_ItemList.AutoLoseItemFromEquipmentRoom(PlayerSet.m_sPKPunishParam[nPKValue].m_nItem);	//#roi item khi bi chet co pk

			if (g_Random(100) < PlayerSet.m_sPKPunishParam[nPKValue].m_nEquip) //#roi item khi bi chet co pk
			{
				Player[m_nPlayerIdx].m_ItemList.AutoLoseEquip();
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
	else if (nBelongPlayer > 0 && m_pDropRate && !g_NotAddNpcNormal) //sö dông khi load npc normal trong file pak hoac folder load npc b»ng script th× kh«ng sö dông
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
    m_CurrentLife = m_CurrentLife + (nDamage*m_CurrentLifeStolen/100); 
    if (m_CurrentLife >= m_CurrentLifeMax) 
        m_CurrentLife = m_CurrentLifeMax; 
}

void KNpc::RestoreDamage2Mana(int nDamage) //edit by phong kieu hut mana
{
	m_CurrentMana = m_CurrentMana + (nDamage*m_CurrentManaStolen/100); 
	if (m_CurrentMana >= m_CurrentManaMax) 
        m_CurrentMana = m_CurrentManaMax; 
}

void KNpc::RestoreDamage2Stamina(int nDamage)//edit by phong kieu hut the luc
{	
	m_CurrentStamina = m_CurrentStamina + (nDamage*m_CurrentStaminaStolen/100);
	if(m_CurrentStamina >= m_CurrentStaminaMax)
		m_CurrentStamina = m_CurrentStaminaMax;
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
	int nIdx = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion());
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

void KNpc::LoseSingleItem(int nBelongPlayer)//danh quai roi do roi item
{
	int MAX_MAGIC_LEVEL = 6; //t¨ng lµm 4 giai ®o¹n 6->8->9->10
	if (!m_pDropRate)
		return;

	if (m_pDropRate->nMaxItemLevelScale <= 0 || m_pDropRate->nMinItemLevelScale <= 0)
		return;

	int nRand = g_Random(m_pDropRate->nMaxRandRate);
	int nCheckRand = 0;	

	int i;
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
	if(nLevel > 10) //max level item lµ 10
	{
		nLevel = 10;
	}
	//add by phong kiÒu r¬i ®å xanh theo magiclevel
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
	for (int j = 0; j < 6; j++)
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

	int nIdx = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck, nDetail, nParticular, pnMagicLevel, g_SubWorldSet.GetGameVersion());

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
			ExecuteScript(ActionScript, "OnRevive", m_Index);	// monster #idx cña npc khi håi sinh
		}
	}
#endif
}

void KNpc::RestoreLiveData()
{

}
#endif

#ifdef	_SERVER

void	KNpc::SendDataToNearRegion(void* pBuffer, DWORD dwSize)
{
	_ASSERT(m_RegionIndex >= 0);
	if (m_RegionIndex < 0)
		return;

	POINT	POff[8] = 
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

int	KNpc::FindAroundPlayer(const char* Name)//add by phong kiÒu using vËn tiªu
{
	int nNpc = 0;
	if (Name[0] <= 0 || m_RegionIndex < 0)
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

	if (this->m_Kind != kind_player || Npc[nKiller].m_Kind != kind_player || !m_FightMode)
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
		if (m_SkillList.GetLevel(m_ActiveAuraID) > 0)
		{
			KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_ActiveAuraID, 1);
			if (pOrdinSkill)
			{
				if (pOrdinSkill->GetStateSpecailId())
					AddStateInfo(pOrdinSkill->GetStateSpecailId());
				//
				if(pOrdinSkill->GetAppendSkillNum())
				{
					for( j = 0; j < pOrdinSkill->GetAppendSkillNum(); j++)
					{
						if(m_SkillList.GetLevel(pOrdinSkill->GetAppendSkillId(j)) <= 0)
							continue;
						pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(pOrdinSkill->GetAppendSkillId(j), 1);
						if (pOrdinSkill)
						{
							if (pOrdinSkill->GetStateSpecailId())
								AddStateInfo(pOrdinSkill->GetStateSpecailId());
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
	//
	for (i; i < MAX_SKILL_STATE; i++)
	{
		m_btStateInfo[i] = 0;
	}
}

void KNpc::AddStateInfo(int nID)
{	
	// Tim xem da ton tai chua
	BOOL bEC = FALSE;
	int i = 0;
	for (i = 0; i < MAX_NPC_RECORDER_STATE; i++)
	{
		if (m_btStateInfo[i] == nID)
		{
			bEC = TRUE;
			break;
		}
	}
	if (bEC == TRUE)//Ton tai roi khong add nua
		return;
	// Tim vi tri Emty de add vao
	for (i = 0; i < MAX_NPC_RECORDER_STATE; i++)
	{
		if (m_btStateInfo[i] == 0)
			break;
	}
	if (i == MAX_NPC_RECORDER_STATE)
		i = 0;
	m_btStateInfo[i] = nID;
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
		pTempNode->Remove();
		delete pTempNode;
	}
	return;
}

void	KNpc::RestoreNpcBaseInfo()
{
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

	memset(&m_CurrentFireDamage, 0, sizeof(m_CurrentFireDamage));
	memset(&m_CurrentColdDamage, 0, sizeof(m_CurrentColdDamage));
	memset(&m_CurrentLightDamage, 0, sizeof(m_CurrentLightDamage));
	memset(&m_CurrentPoisonDamage, 0, sizeof(m_CurrentPoisonDamage));

	memset(&m_CurrentFireMagic, 0, sizeof(m_CurrentFireMagic));
	memset(&m_CurrentColdMagic, 0, sizeof(m_CurrentColdMagic));
	memset(&m_CurrentLightMagic, 0, sizeof(m_CurrentLightMagic));
	memset(&m_CurrentPoisonMagic, 0, sizeof(m_CurrentPoisonMagic));

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
	m_CurrentFiveElementsEnhance = 0;
	m_CurrentFiveElementsResist = 0;
	m_CurrentManaToSkillEnhanceP = 0;					//#khi noi cong day tang ky nang cong kich
	m_CurrentSorbDamageP = 0;								//#triet tieu sat thuong
	Player[m_nPlayerIdx].m_nCurLucky = Player[m_nPlayerIdx].m_nLucky;
	//
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
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
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
	return 1;
}
#endif

#ifdef _SERVER
int KNpc::ChangeWorld(DWORD dwSubWorldID, int nX, int nY)
{
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(dwSubWorldID);

	//if (!IsPlayer()) return 0; //Bá ®i vËn tiªu ®ang cÇn
	
	if (-1 == nTargetSubWorld)
	{
		//if (m_SubWorldIndex >= 0)
		//	SubWorld[m_SubWorldIndex].m_MissionArray.RemovePlayer(m_nPlayerIdx, Player[m_nPlayerIdx].m_dwID);
		TobeExchangeServer(dwSubWorldID, nX, nY);
		g_DebugLog("MapID %d haven't been loaded!", dwSubWorldID);
		return 0;
	}

	Player[m_nPlayerIdx].m_nPrePayMoney = 0;
	
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
		SendDataToNearRegion(&RemoveSync, sizeof(NPC_REMOVE_SYNC));
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

void KNpc::ClearStateSkillEffect()
{
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
			UpdateNpcStateInfo();
#endif
			pTempNode = NULL;
			continue;
		}
	}
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
	if (m_Kind != kind_player)
		return;
	
	if (m_Index <= 0)
		return;

	if (m_SubWorldIndex < 0 || m_RegionIndex < 0)
		return;

	DWORD	dwTrap = SubWorld[m_SubWorldIndex].m_Region[m_RegionIndex].GetTrap(m_MapX, m_MapY);
	if (m_TrapScriptID == dwTrap)
	{
		return;
	}
	else
	{
		m_TrapScriptID = dwTrap;
	}

	if (!m_TrapScriptID)
	{
		return;
	}

	Player[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, "main", m_nPlayerIdx);
}

void KNpc::SetFightMode(BOOL bFightMode)
{
	g_DebugLog("[DEATH]SetMode:%d", bFightMode);
	m_FightMode = bFightMode;

#ifdef _SERVER
	if (this->m_Kind == kind_player)
		Player[this->m_nPlayerIdx].m_cPK.CloseAll();
#endif
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
			UpdateNpcStateInfo();
#endif
			continue;
		}
	}
	//
#ifdef _SERVER
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
		if (m_RegionIndex >= 0)
			DoDeath();
		break;
	case do_hurt:
		if (m_RegionIndex >= 0)
			DoHurt(x, y, z);
		break;
	case do_revive:
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
		SetInstantSpr(enumINSTANT_STATE_REVIVE);
		break;
	case do_stand:
		DoStand();
		m_ProcessAI = 1;
		m_ProcessState = 1;
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
}
#endif

#ifndef _SERVER
void	KNpc::SetBlood(int nNo) //nh¶y Damage khi ®¸nh vµo NPC
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
int	KNpc::PaintBlood(int nHeightOffset)
{
	for (int i = 0;i < 5;i++)
	{
		if (!m_szBloodNo[i][0])
			continue;
		
		int	nHeightOff = (int)(nHeightOffset + (defMAX_SHOW_BLOOD_TIME - m_nBloodTime[i]) * defSHOW_BLOOD_MOVE_SPEED);
		int nFontSize = 16;
		DWORD	dwColor = SHOW_BLOOD_COLOR | (m_nBloodAlpha[i] << 24);
		int		nMpsX, nMpsY;
		GetMpsPos(&nMpsX, &nMpsY);
		g_pRepresent->OutputText(nFontSize, m_szBloodNo[i], KRF_ZERO_END, nMpsX - nFontSize * g_StrLen(m_szBloodNo[i]) / 4, nMpsY, dwColor, 0, nHeightOff);
		
		m_nBloodTime[i]--;
		if (m_nBloodTime[i] <= 0)
		{
			ClearBlood(i);
			continue;
		}
		m_nBloodAlpha[i]++;
		if (m_nBloodAlpha[i] > 31)
		m_nBloodAlpha[i] = 31;
	}

	return nHeightOffset;
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
void KNpc::ResetPathFind() {
	m_PathFind.clear();
}
#endif