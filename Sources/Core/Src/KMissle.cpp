/*******************************************************************************
// FileName			:	KMissle.cpp
// FileAuthor		:	Fong KiÒu
// FileCreateDate	:	2020-6-10
*******************************************************************************/
#include "KCore.h"
#include "KMissle.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KMath.h"
#include <math.h>
#include "KSkillSpecial.h"
#include "KOption.h"
//#include "myassert.h"
#ifndef _SERVER
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "Scene\KScenePlaceC.h"
#include "ImgRef.h"
#endif
#include "Scene/ObstacleDef.h"
#include "KPlayer.h"
#include "KMissleSet.h"

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "../../Engine/Src/KSG_StringProcess.h"
#endif

TCollisionMatrix g_CollisionMatrix[64] =
{
	{0,	0, -1, 1, 0, 1, 1, 1}, // 0--------
	{0,	0, -1, 1, 0, 1, 1, 1}, // 1
	{0,	0, -1, 1, 0, 1, 1, 1}, // 2
	{0,	0, -1, 1, 0, 1, 1, 1}, // 3
	
	{0,	0, -1, 0, -1, 1, 0, 1}, // 4
	{0,	0, -1, 0, -1, 1, 0, 1}, // 5
	{0,	0, -1, 0, -1, 1, 0, 1}, // 6
	{0,	0, -1, 0, -1, 1, 0, 1}, // 7
	{0,	0, -1, 0, -1, 1, 0, 1}, // 8--------
	{0,	0, -1, 0, -1, 1, 0, 1}, // 9
	{0,	0, -1, 0, -1, 1, 0, 1}, // 10
	{0,	0, -1, 0, -1, 1, 0, 1}, // 11
	
	{0,	0, -1, -1, -1, 0, -1, 1}, // 12
	{0,	0, -1, -1, -1, 0, -1, 1}, // 13
	{0,	0, -1, -1, -1, 0, -1, 1}, // 14
	{0,	0, -1, -1, -1, 0, -1, 1}, // 15
	{0,	0, -1, -1, -1, 0, -1, 1}, // 16--------
	{0,	0, -1, -1, -1, 0, -1, 1}, // 17
	{0,	0, -1, -1, -1, 0, -1, 1}, // 18
	{0,	0, -1, -1, -1, 0, -1, 1}, // 19
	
	{0,	0, 0, -1, -1, -1, -1, 0}, // 20
	{0,	0, 0, -1, -1, -1, -1, 0}, // 21
	{0,	0, 0, -1, -1, -1, -1, 0}, // 22
	{0,	0, 0, -1, -1, -1, -1, 0}, // 23
	{0,	0, 0, -1, -1, -1, -1, 0}, // 24--------
	{0,	0, 0, -1, -1, -1, -1, 0}, // 25
	{0,	0, 0, -1, -1, -1, -1, 0}, // 26
	{0,	0, 0, -1, -1, -1, -1, 0}, // 27
	
	{0,	0, -1, -1, 0, -1, 1, -1}, // 28
	{0,	0, -1, -1, 0, -1, 1, -1}, // 29
	{0,	0, -1, -1, 0, -1, 1, -1}, // 30
	{0,	0, -1, -1, 0, -1, 1, -1}, // 31
	{0,	0, -1, -1, 0, -1, 1, -1}, // 32--------
	{0,	0, -1, -1, 0, -1, 1, -1}, // 33
	{0,	0, -1, -1, 0, -1, 1, -1}, // 34
	{0,	0, -1, -1, 0, -1, 1, -1}, // 35
	
	{0,	0, 0, -1, 1, -1, 1, 0}, // 36
	{0,	0, 0, -1, 1, -1, 1, 0}, // 37
	{0,	0, 0, -1, 1, -1, 1, 0}, // 38
	{0,	0, 0, -1, 1, -1, 1, 0}, // 39
	{0,	0, 0, -1, 1, -1, 1, 0}, // 40--------
	{0,	0, 0, -1, 1, -1, 1, 0}, // 41
	{0,	0, 0, -1, 1, -1, 1, 0}, // 42
	{0,	0, 0, -1, 1, -1, 1, 0}, // 43
	
	{0,	0, 1, -1, 1, 0, 1, 1}, // 44
	{0,	0, 1, -1, 1, 0, 1, 1}, // 45
	{0,	0, 1, -1, 1, 0, 1, 1}, // 46
	{0,	0, 1, -1, 1, 0, 1, 1}, // 47
	{0,	0, 1, -1, 1, 0, 1, 1}, // 48--------
	{0,	0, 1, -1, 1, 0, 1, 1}, // 49
	{0,	0, 1, -1, 1, 0, 1, 1}, // 50
	{0,	0, 1, -1, 1, 0, 1, 1}, // 51
	
	{0,	0, 1, 0, 1, 1, 0, 1}, // 52
	{0,	0, 1, 0, 1, 1, 0, 1}, // 53
	{0,	0, 1, 0, 1, 1, 0, 1}, // 54
	{0,	0, 1, 0, 1, 1, 0, 1}, // 55
	{0,	0, 1, 0, 1, 1, 0, 1}, // 56---------
	{0,	0, 1, 0, 1, 1, 0, 1}, // 57
	{0,	0, 1, 0, 1, 1, 0, 1}, // 58
	{0,	0, 1, 0, 1, 1, 0, 1}, // 59
	
	{0,	0, -1, 1, 0, 1, 1, 1}, // 60
	{0,	0, -1, 1, 0, 1, 1, 1}, // 61
	{0,	0, -1, 1, 0, 1, 1, 1}, // 62
	{0,	0, -1, 1, 0, 1, 1, 1}, // 63
};

KMissle g_MisslesLib[MAX_MISSLESTYLE];

#define CellWidth		(SubWorld[m_nSubWorldId].m_nCellWidth << 10)
#define CellHeight		(SubWorld[m_nSubWorldId].m_nCellHeight << 10)

#define RegionWidth		(SubWorld[m_nSubWorldId].m_nRegionWidth)
#define RegionHeight	(SubWorld[m_nSubWorldId].m_nRegionHeight)

#define CurRegion		SubWorld[m_nSubWorldId].m_Region[m_nRegionId]
#define CurSubWorld		SubWorld[m_nSubWorldId]

#define LeftRegion(nRegionId)	SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[2]
#define RightRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[6]
#define UpRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[4]
#define DownRegion(nRegionId)		SubWorld[m_nSubWorldId].m_Region[nRegionId].m_nConnectRegion[0]

int g_nRandMissleTab[100] = {0	};

CORE_API KMissle Missle[MAX_MISSLE];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
KMissle::KMissle()
{
	m_bIsPhysical = FALSE;
	m_bIsMelee = FALSE;
	m_nMissleSeries = -1;
	m_nMissleId = -1;
	m_nCollideOrVanishTime = 0;
	m_ulDamageInterval = 0;
	m_nTempParam1 = 0;
	m_nTempParam2 = 0;
	m_nFirstReclaimTime = 0;
	m_nEndReclaimTime = 0;
	m_nMissRate = 0;
	m_nHitCount = 0;
	
#ifdef _SERVER
	m_pMagicAttribsData = NULL;
	m_ulNextCalDamageTime = 0;
#else
	m_bFollowNpcWhenCollid = 1;
	m_bRemoving	= FALSE;
	m_btRedLum = m_btGreenLum = m_btBlueLum = 0xff;
	m_usLightRadius = 50;
#endif
	m_bMustBeHit = FALSE;
	memset(m_szMissleName, 0, sizeof(m_szMissleName));
	m_nAction = 0;
	m_bIsSlow = FALSE;
	m_bClientSend = FALSE;
	m_bRemoving = FALSE;
	nId = 0;
	m_eMoveKind = eMissleMoveKind::MISSLE_MMK_Stand;
	m_eFollowKind = eMissleFollowKind::MISSLE_MFK_None;
	m_nHeight = 0;
	m_nHeightSpeed = 0;
	m_nLifeTime = 0;
	m_nSpeed = 0;
	m_nSkillId = 0;
	m_bRangeDamage = FALSE;
	m_eRelation = 0;
	m_bAutoExplode = FALSE;
	m_bTargetSelf = FALSE;
	m_bByMissle = FALSE;
	m_nInteruptTypeWhenMove = 0;
	m_bHeelAtParent = FALSE;
	m_nLauncherSrcPX = 0;
	m_nLauncherSrcPY = 0;
	m_nCollideRange = 0;
	m_nDamageRange = 0;
	m_bCollideVanish = FALSE;
	m_bCollideFriend = FALSE;
	m_bCanSlow = FALSE;

	m_bFlyEvent = FALSE;
	m_nFlyEventTime = 0;
	m_bSubEvent = FALSE;
	m_bStartEvent = FALSE;
	m_bCollideEvent = FALSE;
	m_bVanishedEvent = FALSE;

	m_bMustBeHit = FALSE;
	m_nCurrentLife = 0;
	m_nStartLifeTime = 0;
	m_nCurrentMapX = 0;
	m_nCurrentMapY = 0;
	m_nCurrentMapZ = 0;
	m_nXOffset = 0;
	m_nYOffset = 0;
	m_nRefPX = 0;
	m_nRefPY = 0;

	m_nDesMapX = 0;
	m_nDesMapY = 0;
	m_nDesRegion = 0;
	m_bNeedReclaim = FALSE;

	m_nDoHurtP = FALSE;

	m_nXFactor = 0;
	m_nYFactor = 0;
	m_nLevel = 0;
	m_nLastDoCollisionIdx = 0;

	m_nFollowNpcIdx = 0;
	m_dwFollowNpcID = 0;

	m_nLauncher = 0;
	m_dwLauncherId = 0;
	m_nPKFlag = 0;
	m_nParentMissleIndex = 0;

	m_nCurrentSpeed = 0;
	m_nZAcceleration = 0;
	m_eMissleStatus = eMissleStatus::MS_DoWait;
	m_nSubWorldId = 0;
	m_nRegionId = 0;

	m_nMaxDamage = 0;
	m_nElementType = 0;
	m_nMaxElementDamage = 0;
	m_nElementTime = 0;
	m_nElementInterval = 0;
	m_nElementPerDamage = 0;
	m_nParam1 = 0;
	m_nParam2 = 0;
	m_nParam3 = 0;

	m_nDirIndex = 0;
	m_nDir = 0;
	m_nAngle = 0;
	m_dwBornTime = 0;
	m_bUseAttackRating = FALSE;

#ifdef _SERVER
	m_ulNextCalDamageTime = 0;
#endif

#ifndef _SERVER
	m_bMultiShow = FALSE;
	m_bFollowNpcWhenCollid = FALSE;
	m_btRedLum = 0;
	m_btGreenLum = 0;
	m_btBlueLum = 0;
	m_usLightRadius = 0;
	m_SceneID = 0;
#endif
}

void KMissle::Release()
{
///#pragma	message(ATTENTION("KMissle Release"))
#ifndef _SERVER	
	g_ScenePlace.RemoveObject(CGOG_MISSLE, m_nMissleId, m_SceneID);
	m_MissleRes.Clear();
	m_nMissleId = -1;
	m_bInterpValid = FALSE;
	m_nFollowNpcIdx = 0;
	m_nLastDoCollisionIdx = 0;
#endif
#ifdef _SERVER
	if (m_pMagicAttribsData)
		if (m_pMagicAttribsData->DelRef() <= 0) {
			m_pMagicAttribsData->Remove();
			delete m_pMagicAttribsData;
		}
		m_pMagicAttribsData = NULL;
#endif
}

KMissle::~KMissle()
{
	
}
/*!*****************************************************************************
// Function		: KMissle::GetInfoFromTabFile
// Return		: BOOL 
// Argumant		: int nMissleId
*****************************************************************************/
BOOL KMissle::GetInfoFromTabFile(int nMissleId)
{
	if (nMissleId <= 0 ) return FALSE;
	KITabFile * pITabFile = &g_MisslesSetting;
	return GetInfoFromTabFile(pITabFile, nMissleId);
}

BOOL KMissle::GetInfoFromTabFile(KITabFile * pMisslesSetting, int nMissleId)
{
	if (nMissleId <= 0 ) return FALSE;
	m_nMissleId		= nMissleId;
	int nRow = nMissleId;
	
	pMisslesSetting->GetString(nRow, "MissleName",		   "", m_szMissleName,30, TRUE);
	
	int nHeightOld ;
	pMisslesSetting->GetInteger(nRow, "MissleHeight",		0, &nHeightOld, TRUE);
	m_nHeight = nHeightOld << 10;
	
	pMisslesSetting->GetInteger(nRow, "LifeTime",			0, &m_nLifeTime, TRUE);
	pMisslesSetting->GetInteger(nRow, "Speed",				0, &m_nSpeed, TRUE);
	pMisslesSetting->GetInteger(nRow, "ResponseSkill",		0, &m_nSkillId, TRUE);
	pMisslesSetting->GetInteger(nRow, "CollidRange",		0, &m_nCollideRange, TRUE);
	pMisslesSetting->GetInteger(nRow, "ColVanish",			0, &m_bCollideVanish, TRUE);
	pMisslesSetting->GetInteger(nRow, "CanColFriend",		0, &m_bCollideFriend, TRUE);
	pMisslesSetting->GetInteger(nRow, "CanSlow",			0, &m_bCanSlow, TRUE);
	pMisslesSetting->GetInteger(nRow, "IsRangeDmg",		0, &m_bRangeDamage, TRUE);
	pMisslesSetting->GetInteger(nRow, "DmgRange",			0, &m_nDamageRange, TRUE);
	pMisslesSetting->GetInteger(nRow, "MoveKind",			0, (int*)&m_eMoveKind, TRUE);
	pMisslesSetting->GetInteger(nRow, "FollowKind",		0, (int*)&m_eFollowKind, TRUE);
	pMisslesSetting->GetInteger(nRow, "Zacc",				0,(int*)&m_nZAcceleration, TRUE);
	pMisslesSetting->GetInteger(nRow, "Zspeed",				0,(int*)&m_nHeightSpeed, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param1",			0, &m_nParam1, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param2",			0, &m_nParam2, TRUE);
	pMisslesSetting->GetInteger(nRow, "Param3",			0, &m_nParam3, TRUE);
	
	BOOL bAutoExplode = 0;
	pMisslesSetting->GetInteger(nRow, "AutoExplode",	0, (int*)&bAutoExplode, TRUE);
	m_bAutoExplode = bAutoExplode;
	
	pMisslesSetting->GetInteger(nRow, "DmgInterval",	0, (int*)&m_ulDamageInterval, TRUE);
	
#ifndef _SERVER	
	char AnimFileCol[64];
	char SndFileCol[64];
	char AnimFileInfoCol[100];
	char szAnimFileInfo[100];

    const char *pcszTemp = NULL;
	
	pMisslesSetting->GetInteger(nRow, "RedLum",	    255, (int*)&m_btRedLum, TRUE);
	pMisslesSetting->GetInteger(nRow, "GreenLum",	255, (int*)&m_btGreenLum, TRUE);
	pMisslesSetting->GetInteger(nRow, "BlueLum",	255, (int*)&m_btBlueLum, TRUE);
	
	int nLightRadius = 0;
	pMisslesSetting->GetInteger(nRow, "LightRadius", 50, (int*)&nLightRadius, TRUE);
	m_usLightRadius = nLightRadius;
	
	pMisslesSetting->GetInteger(nRow, "MultiShow",		0, &m_bMultiShow, TRUE);
	for (int i  = 0; i < MAX_MISSLE_STATUS; i++)
	{
		sprintf(AnimFileCol, "AnimFile%d", i + 1);
		sprintf(SndFileCol,  "SndFile%d", i + 1);
		sprintf(AnimFileInfoCol, "AnimFileInfo%d", i + 1);
		
		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MissleRes.m_MissleRes[i].AnimFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MissleRes.m_MissleRes[i].SndFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, 100, TRUE);
		
		//m_MissleRes.m_MissleRes[i].nInterval = 1;
		//m_MissleRes.m_MissleRes[i].nDir = 16;
		//m_MissleRes.m_MissleRes[i].nTotalFrame = 100;

        pcszTemp = szAnimFileInfo;
        m_MissleRes.m_MissleRes[i].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MissleRes.m_MissleRes[i].nDir = KSG_StringGetInt(&pcszTemp, 16);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MissleRes.m_MissleRes[i].nInterval = KSG_StringGetInt(&pcszTemp, 1);
		//sscanf(szAnimFileInfo, "%d,%d,%d", 
		//	&m_MissleRes.m_MissleRes[i].nTotalFrame,
		//	&m_MissleRes.m_MissleRes[i].nDir,
		//	&m_MissleRes.m_MissleRes[i].nInterval
        //);

		
		sprintf(AnimFileCol, "AnimFileB%d", i + 1);
		sprintf(SndFileCol,  "SndFileB%d", i + 1);
		sprintf(AnimFileInfoCol, "AnimFileInfoB%d", i + 1);
		
		pMisslesSetting->GetString(nRow, AnimFileCol,			"", m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].AnimFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, SndFileCol,			"", m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].SndFileName, 64, TRUE);
		pMisslesSetting->GetString(nRow, AnimFileInfoCol,		"", szAnimFileInfo, 100, TRUE);
		
		//m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval = 1;
		//m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir = 16;
		//m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame = 100;
		
        pcszTemp = szAnimFileInfo;
        m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir = KSG_StringGetInt(&pcszTemp, 16);
        KSG_StringSkipSymbol(&pcszTemp, ',');
        m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval = KSG_StringGetInt(&pcszTemp, 1);

		//sscanf(szAnimFileInfo, "%d,%d,%d", 
		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nTotalFrame,
		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nDir,
		//	&m_MissleRes.m_MissleRes[i + MAX_MISSLE_STATUS].nInterval
        //);
		
	}
	pMisslesSetting->GetInteger(nRow, "LoopPlay",			0, &m_MissleRes.m_bLoopAnim, TRUE);
	pMisslesSetting->GetInteger(nRow, "SubLoop",		0, &m_MissleRes.m_bSubLoop, TRUE);
	pMisslesSetting->GetInteger(nRow, "SubStart",		0, &m_MissleRes.m_nSubStart, TRUE);
	pMisslesSetting->GetInteger(nRow, "SubStop",		0, &m_MissleRes.m_nSubStop, TRUE);
	pMisslesSetting->GetInteger(nRow, "ColFollowTarget",0, (int *)&m_bFollowNpcWhenCollid, TRUE);
#endif
	return TRUE;
}

BOOL KMissle::Init( int nLauncher, int nMissleId, int nXFactor, int nYFactor, int nLevel)
{
#ifndef _SERVER
	AUTOLOG_EVERY(1000, "[MIS-INIT] launcher=%d missleId=%d fx=%d fy=%d level=%d", nLauncher, nMissleId, nXFactor, nYFactor, nLevel);
	m_MissleRes.Init();
#endif
	return	TRUE;
}

/*!*****************************************************************************
// Function		: KMissle::Activate
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
int KMissle::Activate()
{	
	AUTOLOG_EVERY(1000, "[MIS-ACT-BADID] id=%d region=%d skill=%d lv=%d launcher=%d life=%d/%d", m_nMissleId, m_nRegionId, m_nSkillId, m_nLevel, m_nLauncher, m_nCurrentLife, m_nLifeTime);
	if (m_nMissleId <= 0 || m_nRegionId < 0)
	{
		return  0 ;
	}
	
	_ASSERT(m_nLauncher > 0);
	AUTOLOG_EVERY(1000, "[MIS-ACT-NOLAUNCHER] id=%d skill=%d lv=%d launcher=%d launcherId=%lu life=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, m_dwLauncherId, m_nCurrentLife);
	if (m_nLauncher <= 0)
		return 0;
	
	AUTOLOG_EVERY(2000, "[MSL-OWNER-LOST] msl=%d sk=%d launcher=%d wantid=%u realid=%u sw=%d/%d region=%d life=%d/%d -> DoVanish", m_nMissleId, m_nSkillId, m_nLauncher, m_dwLauncherId, Npc[m_nLauncher].m_dwID, Npc[m_nLauncher].m_SubWorldIndex, m_nSubWorldId, Npc[m_nLauncher].m_RegionIndex, m_nCurrentLife, m_nLifeTime);
	if (!Npc[m_nLauncher].IsMatch(m_dwLauncherId) || 
		/*(Npc[m_nLauncher].m_Doing == do_death) ||*/
		(Npc[m_nLauncher].m_SubWorldIndex != m_nSubWorldId) || 
		(Npc[m_nLauncher].m_RegionIndex < 0))
	{
		DoVanish();
		return 0;	
	}

	AUTOLOG_EVERY(2000, "[MSL-PKFLAG] msl=%d sk=%d launcher=%d pkflag_npc=%d pkflag_msl=%d life=%d/%d -> DoVanish", m_nMissleId, m_nSkillId, m_nLauncher, (int)Npc[m_nLauncher].m_nPKFlag, m_nPKFlag, m_nCurrentLife, m_nLifeTime);
	if (Npc[m_nLauncher].m_nPKFlag != m_nPKFlag)
	{
		DoVanish();
		return 0;	
	}
	
	if (g_AutoLogOn())
	{
		// FIX 21/08: m_nFollowNpcIdx co the la -1 (nTargetId khong tro toi NPC, KSkills.cpp:1797)
		// nen KHONG duoc doc Npc[m_nFollowNpcIdx] truoc cua chan "> 0" ngay ben duoi.
		int nLogFollow = (m_nFollowNpcIdx > 0 && m_nFollowNpcIdx < MAX_NPC) ? m_nFollowNpcIdx : -1;
		AUTOLOG_EVERY(1000, "[MSL-TGT-LOST] msl=%d sk=%d follow=%d wantid=%u realid=%u sw=%d/%d protect=%d hide=%d -> bo bam muc tieu", m_nMissleId, m_nSkillId, m_nFollowNpcIdx, m_dwFollowNpcID, (nLogFollow >= 0) ? (unsigned int)Npc[nLogFollow].m_dwID : 0u, (nLogFollow >= 0) ? Npc[nLogFollow].m_SubWorldIndex : -1, m_nSubWorldId, (nLogFollow >= 0) ? Npc[nLogFollow].m_nProtectedTime : 0, (nLogFollow >= 0) ? Npc[nLogFollow].m_HideState.nTime : 0);
	}
	if (m_nFollowNpcIdx > 0)	// fix by Choi Huyn Woo
	{
		AUTOLOG_EVERY(1000, "[MIS-ACT-FOLLOWLOST] id=%d skill=%d follow=%d wantId=%lu haveId=%lu sw=%d/%d protect=%d hide=%d hp=%d", m_nMissleId, m_nSkillId, m_nFollowNpcIdx, m_dwFollowNpcID, Npc[m_nFollowNpcIdx].m_dwID, Npc[m_nFollowNpcIdx].m_SubWorldIndex, m_nSubWorldId, Npc[m_nFollowNpcIdx].m_nProtectedTime, Npc[m_nFollowNpcIdx].m_HideState.nTime, Npc[m_nFollowNpcIdx].m_CurrentLife);
		if (!Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID) 
			|| Npc[m_nFollowNpcIdx].m_SubWorldIndex != m_nSubWorldId
			|| Npc[m_nFollowNpcIdx].m_nProtectedTime > 0			//vong tron bat tu, vßng trßn bÊt tö
			|| Npc[m_nFollowNpcIdx].m_HideState.nTime > 0)
		{
			m_nFollowNpcIdx = 0;
		}
	}
	
	eMissleStatus eLastStatus = m_eMissleStatus;
	
	AUTOLOG_EVERY(1000, "[MIS-LIFE-END] id=%d skill=%d lv=%d life=%d/%d status=%d autoExplode=%d dmgRange=%d relation=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nCurrentLife, m_nLifeTime, (int)m_eMissleStatus, m_bAutoExplode, m_nDamageRange, m_eRelation);
	if (
		m_nCurrentLife >= m_nLifeTime 
		&& m_eMissleStatus != MS_DoVanish 
		&& m_eMissleStatus != MS_DoCollision
		)
	{
		if (m_bAutoExplode)
		{
			ProcessCollision();
			if (m_bCollideEvent)	
			{
				_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
				KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_nSkillId, m_nLevel);
				if (pOrdinSkill)
				{
					pOrdinSkill->Collidsion(this);
				}
			}
		}
		DoVanish();
#ifdef _SERVER
		m_nCurrentLife ++;
		return 1;
#endif
	}
	
	AUTOLOG_EVERY(1000, "[MSL-FLY-START] t=%u msl=%d sk=%d lv=%d launcher=%d follow=%d pos(r=%d,%d,%d off %d,%d z=%d) speed=%d fx=%d fy=%d dir=%d life=%d/%d interupt=%d", SubWorld[m_nSubWorldId].m_dwCurrentTime, m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, m_nFollowNpcIdx, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, m_nSpeed, m_nXFactor, m_nYFactor, m_nDir, m_nCurrentLife, m_nLifeTime, m_nInteruptTypeWhenMove);
	if (m_nCurrentLife == m_nStartLifeTime && m_eMissleStatus != MS_DoVanish)	
	{
		AUTOLOG_EVERY(1000, "[MIS-PREP-CALL] id=%d skill=%d interrupt=%d heelParent=%d parent=%d srcPX=%d srcPY=%d", m_nMissleId, m_nSkillId, m_nInteruptTypeWhenMove, m_bHeelAtParent, m_nParentMissleIndex, m_nLauncherSrcPX, m_nLauncherSrcPY);
		if (PrePareFly())
		{
#ifndef _SERVER
			int nSrcX2 = 0 ;
			int nSrcY2 = 0 ;
			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);
			m_MissleRes.PlaySound(MS_DoFly, nSrcX2, nSrcY2, 0);
			//CreateSpecialEffect(MS_DoFly, nSrcX2, nSrcY2, m_nCurrentMapZ);
#endif
			
			DoFly();
		}
		else
		{
			DoVanish();
		}
	}
	
	switch(m_eMissleStatus)
	{
	case MS_DoWait:
		{
			OnWait();
		}
		break;
	case MS_DoFly:
		{
			OnFly();
			if (m_bFlyEvent)
			{
				if ( (m_nCurrentLife - m_nStartLifeTime) % m_nFlyEventTime == 0 )
				{
					_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
					if (m_nLevel  <= 0 ) return 0;
					KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId , m_nLevel);
					if (pOrdinSkill)
					{
						pOrdinSkill->FlyEvent(this);
					}
				}
			}
		}
		break;
	case MS_DoCollision:
		{
			OnCollision();
		}
		break;
	case MS_DoVanish:
		{
			OnVanish();
		}
		break;
	}
	
#ifndef _SERVER
	if (m_nMissleId > 0)
	{
		int nSrcX;
		int nSrcY;
		
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
		// chup 2 moc cho noi suy khi ve (khong tinh them Map2Mps - dung luon ket qua tren)
		if (m_bInterpValid)
		{
			m_nPrevX = m_nTickX;
			m_nPrevY = m_nTickY;
			m_nPrevZ = m_nTickZ;
		}
		else
		{
			m_nPrevX = nSrcX;
			m_nPrevY = nSrcY;
			m_nPrevZ = m_nCurrentMapZ;
			m_bInterpValid = TRUE;
		}
		m_nTickX = nSrcX;
		m_nTickY = nSrcY;
		m_nTickZ = m_nCurrentMapZ;
		// mac dinh ve tai vi tri tick (dung khi tat noi suy); POSSHIFT se ghi de neu bat
		m_nDrawX = nSrcX;
		m_nDrawY = nSrcY;
		m_nDrawZ = m_nCurrentMapZ;
		if (m_usLightRadius && m_eMissleStatus != MS_DoWait)
			g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT | IPOT_RL_LIGHT_PROP );
		else
			g_ScenePlace.MoveObject(CGOG_MISSLE, m_nMissleId, nSrcX, nSrcY, m_nCurrentMapZ, m_SceneID, IPOT_RL_OBJECT);
	}
	
#endif
	m_nCurrentLife ++;
	return 1;
}

/*!*****************************************************************************
// Function		: KMissle::OnWait
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissle::OnWait()
{
	return;
}
/*!*****************************************************************************
// Function		: KMissle::OnCollision
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissle::OnCollision()
{
	return;	
}

int KMissle::CheckCollision()
{
#ifdef TOOLVERSION
	return FALSE;
#endif
	
	AUTOLOG_EVERY(1000, "[MSL-Z-LOW] msl=%d sk=%d z=%d min=%d max=%d h=%d hspeed=%d zacc=%d -> tra -1 (cham dat, se DoVanish)", m_nMissleId, m_nSkillId, m_nCurrentMapZ, MISSLE_MIN_COLLISION_ZHEIGHT, MISSLE_MAX_COLLISION_ZHEIGHT, m_nHeight, m_nHeightSpeed, m_nZAcceleration);
	if (m_nCurrentMapZ <= MISSLE_MIN_COLLISION_ZHEIGHT) 
	{
		return -1;
	}
	
	AUTOLOG_EVERY(1000, "[MSL-Z-HIGH] msl=%d sk=%d z=%d max=%d h=%d hspeed=%d -> BO QUA kiem tra va cham (bay qua dau muc tieu)", m_nMissleId, m_nSkillId, m_nCurrentMapZ, MISSLE_MAX_COLLISION_ZHEIGHT, m_nHeight, m_nHeightSpeed);
	if (m_nCurrentMapZ > MISSLE_MAX_COLLISION_ZHEIGHT) return 0;
	
	if (m_nRegionId < 0) 
	{
		return -1;
	}

	int nAbsX = 0;
	int nAbsY = 0;
	int nCellWidth = CellWidth;
	int nCellHeight = CellHeight;
	_ASSERT(nCellWidth > 0 && nCellHeight > 0);
	int nRMx = 0;
	int nRMy = 0;
	int nSearchRegion = 0;
	int nNpcIdx = 0;
	int nDX = 0;
	int nDY = 0;
	int nNpcOffsetX = 0;
	int nNpcOffsetY = 0;
	BOOL bCollision = FALSE;
	
	int nColRegion = m_nRegionId;
	int nColMapX = m_nCurrentMapX;
	int nColMapY = m_nCurrentMapY;
		
	AUTOLOG_EVERY(1000, "[MIS-COL-ENTER] id=%d skill=%d colRange=%d dmgRange=%d rel=%d region=%d map=%d,%d off=%d,%d z=%d lastHit=%d", m_nMissleId, m_nSkillId, m_nCollideRange, m_nDamageRange, m_eRelation, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, m_nLastDoCollisionIdx);
	if (m_nCollideRange == 1)
	{
		/*if(this->m_eMoveKind == MISSLE_MMK_Follow)
		{
			if (m_bNeedReclaim && m_nCurrentLife >= m_nFirstReclaimTime && m_nCurrentLife <= m_nEndReclaimTime)
			{
				if (m_nCurrentLife == m_nEndReclaimTime) 
					m_bNeedReclaim = FALSE;
				nNpcIdx = 	CheckNearestCollision();
				AUTOLOG_EVERY(1000, "[MIS-COL-RESULT] id=%d skill=%d life=%d npc=%d rel=%d map=%d,%d off=%d,%d lastHit=%d", m_nMissleId, m_nSkillId, m_nCurrentLife, nNpcIdx, m_eRelation, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nLastDoCollisionIdx);
			}
			else
			{
				_ASSERT(nColRegion >= 0);
				nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nColRegion].FindNpc(nColMapX, nColMapY, m_nLauncher, m_eRelation);
			}
		}
		else*/
			nNpcIdx = 	CheckNearestCollision();
		if (nNpcIdx > 0)
		{
			if (m_nDamageRange == 1)//ÔÚÄ¿±êNpc´¦Åö×²
				ProcessCollision(m_nLauncher, Npc[nNpcIdx].m_RegionIndex , Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, m_nDamageRange , m_eRelation);
			else
				ProcessCollision();//ÔÚ×Óµ¯Î»ÖÃ´¦ÀíÅö×²
			AUTOLOG_EVERY(500, "[MSL-HIT-CELL] t=%u msl=%d sk=%d lv=%d launcher=%d trung npc=%d(id=%u) tai(r=%d,%d,%d) dmgrange=%d colrange=%d lastidx=%d move=%d life=%d/%d", SubWorld[m_nSubWorldId].m_dwCurrentTime, m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcIdx, Npc[nNpcIdx].m_dwID, Npc[nNpcIdx].m_RegionIndex, Npc[nNpcIdx].m_MapX, Npc[nNpcIdx].m_MapY, m_nDamageRange, m_nCollideRange, m_nLastDoCollisionIdx, (int)m_eMoveKind, m_nCurrentLife, m_nLifeTime);
			if(m_nLastDoCollisionIdx == nNpcIdx)
			{
				//to do
			}
			else
			{
				if(this->m_eMoveKind == MISSLE_MMK_Line)
					m_nLastDoCollisionIdx = nNpcIdx;
				DoCollision();//×Óµ¯×÷Åö×²ºóµÄÐ§¹û
			}
			return 1;
		}
	}
	else
	{
		for (int i = -m_nCollideRange; i <= m_nCollideRange; i ++)
			for (int j = -m_nCollideRange; j <= m_nCollideRange; j ++)
			{
				if (!GetOffsetAxis(m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, i , j , nSearchRegion, nRMx, nRMy))
					continue;
				
				_ASSERT(nSearchRegion >= 0);
				nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);
				if (nNpcIdx > 0)
				{
					ProcessCollision();//´¦ÀíÅö×²
					DoCollision();//×Óµ¯×÷Åö×²ºóµÄÐ§¹û
					return 1;
				}
			}
	}
	
	return 0;
}

inline DWORD	KMissle::GetCurrentSubWorldTime()
{
	return SubWorld[m_nSubWorldId].m_dwCurrentTime;
}

void KMissle::OnFly()
{
	if (m_nInteruptTypeWhenMove)
	{
		if (m_nInteruptTypeWhenMove == Interupt_EndOldMissleLifeWhenMove)
		{
			int nPX, nPY;
			Npc[m_nLauncher].GetMpsPos(&nPX, &nPY);
			AUTOLOG_EVERY(1000, "[MIS-FLY-MOVEBREAK] id=%d skill=%d life=%d/%d interrupt=%d nowPX=%d nowPY=%d srcPX=%d srcPY=%d", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nLifeTime, m_nInteruptTypeWhenMove, nPX, nPY, m_nLauncherSrcPX, m_nLauncherSrcPY);
			if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)
			{
				
#ifndef _SERVER 
				int nSrcX2 = 0 ;
				int nSrcY2 = 0 ;
				SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX2, &nSrcY2);
				CreateSpecialEffect(MS_DoVanish, nSrcX2, nSrcY2, m_nCurrentMapZ);
#endif
				DoVanish();
				return ;
			}
		}
	}
	
	AUTOLOG_EVERY(1000, "[MSL-BARRIER] msl=%d sk=%d launcher=%d pos(r=%d,%d,%d off %d,%d z=%d) life=%d/%d -> chan dia hinh, DoVanish", m_nMissleId, m_nSkillId, m_nLauncher, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, m_nCurrentLife, m_nLifeTime);
	if (TestBarrier()) 
	{
#ifndef _SERVER 
		int nSrcX3 = 0 ;
		int nSrcY3 = 0 ;
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX3, &nSrcY3);
		CreateSpecialEffect(MS_DoVanish, nSrcX3, nSrcY3, m_nCurrentMapZ);
#endif
		DoVanish();
		return;
	}
	
	int nDOffsetX = 0;
	int nDOffsetY = 0;
	
	ZAxisMove();			
	switch(this->m_eMoveKind)
	{
	case	MISSLE_MMK_Stand:							//	Ô­µØ
		{
		}
		break;
	case	MISSLE_MMK_Parabola:						//	Å×ÎïÏß
	case	MISSLE_MMK_Line:							//	Ö±Ïß·ÉÐÐ
		{
			nDOffsetX    = (m_nSpeed * m_nXFactor);
			nDOffsetY	 = (m_nSpeed * m_nYFactor);
		}
		break;
	case MISSLE_MMK_RollBack:
		{
			if (!m_nTempParam1)	
			{
				if (m_nTempParam2 <= m_nCurrentLife)
				{
					m_nXFactor = -m_nXFactor;
					m_nYFactor = -m_nYFactor;
					m_nTempParam1 = 1;
					m_nDir = m_nDir - MaxMissleDir / 2;
					if (m_nDir < 0) m_nDir += MaxMissleDir;
				}
			}

			nDOffsetX = (m_nSpeed * m_nXFactor);
			nDOffsetY = (m_nSpeed * m_nYFactor);
		}break;
		
	case	MISSLE_MMK_Random:							//Bolt
		{
			
		}break;
		//dx = SinA * R
		//dy = Ctg(90-A/2).R = SinA*SinA / (1 + CosA) * R
	case	MISSLE_MMK_Circle:							
		{
			int nPreAngle = m_nAngle - 1;
			if (nPreAngle < 0) nPreAngle = MaxMissleDir - 1;
			m_nDir = m_nAngle + (MaxMissleDir / 4);
			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;
			int dx = (m_nSpeed + 30)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle,MaxMissleDir)) ;
			int dy = (m_nSpeed + 30)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle, MaxMissleDir)) ; 
			
			if (m_nParam2) 
			{
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			else			
			{
				int nOldRegion = m_nRegionId;
				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;
				m_nCurrentMapX	= Npc[m_nLauncher].m_MapX;
				m_nCurrentMapY	= Npc[m_nLauncher].m_MapY;
				m_nXOffset		= Npc[m_nLauncher].m_OffX + 30  * (g_DirCos(m_nAngle,MaxMissleDir) + g_DirCos(nPreAngle, MaxMissleDir));
				m_nYOffset		= Npc[m_nLauncher].m_OffY + 30  * (g_DirSin(m_nAngle,MaxMissleDir) + g_DirSin(nPreAngle,MaxMissleDir));
				CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				
				if (nOldRegion != m_nRegionId)
				{
					//SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);
					SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);
				}  
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			
			if (m_nParam1)
			{
				m_nAngle ++;
				if (m_nAngle >= MaxMissleDir)
					m_nAngle = 0;
			}
			else
			{
				m_nAngle --;
				if (m_nAngle < 0 )
					m_nAngle = MaxMissleDir - 1;
			}
			
		}
		break;	
		
	case	MISSLE_MMK_Helix:							//	 Hammer£©
		{
			int nPreAngle = m_nAngle - 1;
			if (nPreAngle < 0) 
			{
				nPreAngle = MaxMissleDir -1;
			}
			m_nDir = m_nAngle + (MaxMissleDir / 4);
			if (m_nDir >= MaxMissleDir) m_nDir = m_nDir - MaxMissleDir;
			
			int dx = (m_nSpeed + m_nCurrentLife + 50)  * (g_DirCos(m_nAngle,MaxMissleDir) - g_DirCos(nPreAngle, MaxMissleDir)) ;
			int dy = (m_nSpeed + m_nCurrentLife + 50)  * (g_DirSin(m_nAngle,MaxMissleDir) - g_DirSin(nPreAngle,MaxMissleDir)) ; 
			
			if (m_nParam2) 
			{
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			else			
			{
				int nOldRegion = m_nRegionId;
				CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				m_nRegionId		= Npc[m_nLauncher].m_RegionIndex;
				m_nCurrentMapX	= Npc[m_nLauncher].m_MapX;
				m_nCurrentMapY	= Npc[m_nLauncher].m_MapY;
				m_nXOffset		= Npc[m_nLauncher].m_OffX;
				m_nYOffset		= Npc[m_nLauncher].m_OffY;
				CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
				
				if (nOldRegion != m_nRegionId)
				{
					//SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);
					SubWorld[m_nSubWorldId].MissleChangeRegion(nOldRegion, m_nRegionId, m_nMissleId);
				}  
				nDOffsetX = dx;
				nDOffsetY = dy;
			}
			
			if (m_nParam1)
			{
				m_nAngle ++;
				if (m_nAngle >= MaxMissleDir)
					m_nAngle = 0;
			}
			else
			{
				m_nAngle --;
				if (m_nAngle < 0 )
					m_nAngle = MaxMissleDir - 1;
			}
		}
		break; 
	case	MISSLE_MMK_Follow:							
		{
			AUTOLOG_EVERY(1000, "[MIS-FLY-FOLLOWSTATE] id=%d skill=%d life=%d follow=%d fx=%d fy=%d speed=%d dir=%d", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nFollowNpcIdx, m_nXFactor, m_nYFactor, m_nSpeed, m_nDir);
			if(this->m_nFollowNpcIdx > 0)
			{
				int nDistance = 0;
				int nSrcMpsX = 0;
				int nSrcMpsY = 0;
				int nDesMpsX = 0;
				int nDesMpsY = 0;
				
				if (m_nTempParam1 ++ >= 8)
				{
					m_nTempParam1 = 0;
					if (m_nFollowNpcIdx > 0 && Npc[m_nFollowNpcIdx].IsMatch(m_dwFollowNpcID))
					{
						SubWorld[m_nSubWorldId].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, &nSrcMpsX, &nSrcMpsY);
						SubWorld[m_nSubWorldId].Map2Mps(Npc[m_nFollowNpcIdx].m_RegionIndex, Npc[m_nFollowNpcIdx].GetMapX(), Npc[m_nFollowNpcIdx].GetMapY(), Npc[m_nFollowNpcIdx].GetOffX(), Npc[m_nFollowNpcIdx].GetOffY(), &nDesMpsX, &nDesMpsY);
						nDistance = SubWorld[m_nSubWorldId].GetDistance(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
						
						AUTOLOG_EVERY(1000, "[MIS-FLY-HOMING] id=%d follow=%d dist=%d src=%d,%d des=%d,%d oldfx=%d oldfy=%d", m_nMissleId, m_nFollowNpcIdx, nDistance, nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY, m_nXFactor, m_nYFactor);
						if (nDistance != 0)
						{
							int nXFactor = ((nDesMpsX - nSrcMpsX ) << 10) / nDistance;
							int nYFactor = ((nDesMpsY - nSrcMpsY ) << 10) / nDistance;
							m_nDirIndex		= g_GetDirIndex(nSrcMpsX, nSrcMpsY, nDesMpsX, nDesMpsY);
							m_nDir			= g_DirIndex2Dir(m_nDirIndex, MaxMissleDir);
							m_nXFactor = nXFactor;
							m_nYFactor = nYFactor;
						}
					}
				}
			}
			nDOffsetX	 = m_nXFactor * m_nSpeed;
			nDOffsetY	 = m_nYFactor * m_nSpeed;
		}break;

	case	MISSLE_MMK_Motion:							
		{
			
		}break;
		
	case MISSLE_MMK_SingleLine:						
		{		
#ifdef _SERVER
			
#else
			int x = m_nXOffset;
			int y = m_nYOffset;
			int dx = (m_nSpeed * m_nXFactor);
			int dy = (m_nSpeed * m_nYFactor);
			nDOffsetX	=  dx;//* m_nCurrentLife;
			nDOffsetY	=  dy;//* m_nCurrentLife;
			
#endif
		}
		break;
	default:
		_ASSERT(0);
		
	}
	
	//
	AUTOLOG_EVERY(1000, "[MSL-FOLLOW-BLIND] msl=%d sk=%d launcher=%d movekind=%d follow=%d d(%d,%d) fx=%d fy=%d speed=%d pos(r=%d,%d,%d) life=%d/%d", m_nMissleId, m_nSkillId, m_nLauncher, (int)m_eMoveKind, m_nFollowNpcIdx, nDOffsetX, nDOffsetY, m_nXFactor, m_nYFactor, m_nSpeed, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentLife, m_nLifeTime);
	if (CheckBeyondRegion(nDOffsetX, nDOffsetY))
	{
		if (CheckCollision() == -1) 
		{
			AUTOLOG_EVERY(1000, "[MIS-FLY-COLFAIL] id=%d skill=%d life=%d/%d region=%d map=%d,%d z=%d autoExplode=%d", m_nMissleId, m_nSkillId, m_nCurrentLife, m_nLifeTime, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentMapZ, m_bAutoExplode);
			if (m_bAutoExplode)
			{
				ProcessCollision();//´¦ÀíÅö×²
			}
#ifndef _SERVER 
			int nSrcX4 = 0 ;
			int nSrcY4 = 0 ;
			SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX4, &nSrcY4);
			CreateSpecialEffect(MS_DoVanish, nSrcX4, nSrcY4, m_nCurrentMapZ);
#endif
			DoVanish();
			return;
		}
	}
	else//Èç¹û×Óµ¯·ÉÐÐ¹ý³ÌÖÐ½øÈëÁËÒ»¸öÎÞÐ§µÄRegionÔò×Óµ¯×Ô¶¯ÏûÍö
	{
		DoVanish();
	}
}
/*!*****************************************************************************
// Function		: KMissle::OnVanish
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: RomanDou
*****************************************************************************/
void KMissle::OnVanish()
{
	
}

#ifndef _SERVER
void KMissle::Paint()
{
	if (m_nMissleId <= 0 ) return;
	// Vi tri VE do Breathe (tick) dat va POSSHIFT noi suy moi khung ve.
	// Truoc day cho nay tu tinh Map2Mps nen am khi luon nhay nac 18 lan/giay.
	int nSrcX = m_nDrawX;
	int nSrcY = m_nDrawY;
	
	if (!m_nZAcceleration)
	{
		if(!(m_nLifeTime <= 2 && Option.GetLow(LowMissle))) //add by phong kiÒu chØ vÏ nh÷ng missless > 2 khi LowMissle
		{
			m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nDrawZ, m_nDir,m_nLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );
		}
	}
	else
	{
		int nDirIndex = g_GetDirIndex(0,0,m_nXFactor, m_nYFactor);
		int nDir = g_DirIndex2Dir(nDirIndex, 64);
		m_MissleRes.Draw(m_eMissleStatus, nSrcX, nSrcY, m_nDrawZ, nDir,m_nLifeTime - m_nStartLifeTime,  m_nCurrentLife - m_nStartLifeTime );
	}
	
	if (m_MissleRes.m_bHaveEnd && (m_MissleRes.SpecialMovieIsAllEnd()))
		SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_DEL, m_nMissleId);
}
#endif

BOOL	KMissle::CheckBeyondRegion(int nDOffsetX, int nDOffsetY)
{
	if (m_nRegionId < 0) 
		return FALSE;
	
	if (nDOffsetX == 0 && nDOffsetY == 0) return TRUE;

	AUTOLOG_EVERY(1000, "[MIS-REGION-OVERSTEP] id=%d skill=%d dOff=%d,%d speed=%d fx=%d fy=%d move=%d", m_nMissleId, m_nSkillId, nDOffsetX, nDOffsetY, m_nSpeed, m_nXFactor, m_nYFactor, (int)m_eMoveKind);
	if ( abs(nDOffsetX) >= (REGION_PIXEL_WIDTH << 10) ) 
	{
		_ASSERT(FALSE);
		return FALSE;
	}
	
	if ( abs(nDOffsetY) >= (REGION_PIXEL_HEIGHT << 10) )
	{
		_ASSERT(FALSE);
		return FALSE;
	}

	int nOldRegion		= m_nRegionId;
	int nNewXOffset		= m_nXOffset + nDOffsetX;
	int nNewYOffset		= m_nYOffset + nDOffsetY;
	int nNewMapX		= m_nCurrentMapX;
	int nNewMapY		= m_nCurrentMapY;
	int nNewRegion		= m_nRegionId;
	
	DWORD nRegionWidth = RegionWidth;
	DWORD nRegionHeight = RegionHeight;
	
	//_ASSERT(abs(nNewXOffset) <= CellWidth * 2);
	//_ASSERT(abs(nNewYOffset) <= CellHeight * 2);
	
	//	´¦ÀíNPCµÄ×ø±ê±ä»Ã
	//	CELLWIDTH¡¢CELLHEIGHT¡¢OffX¡¢OffY¾ùÊÇ·Å´óÁË1024±¶
	
	if (nNewXOffset < 0)
	{
		nNewMapX--;
		nNewXOffset += CellWidth;
	}
	else if (nNewXOffset > CellWidth)
	{
		nNewMapX++;
		nNewXOffset -= CellWidth;
	}
	
	if (nNewYOffset < 0)
	{
		nNewMapY--;
		nNewYOffset += CellHeight;
	}
	else if (nNewYOffset > CellHeight)
	{
		nNewMapY++;
		nNewYOffset -= CellHeight;
	}
	
	AUTOLOG_EVERY(1000, "[MIS-REGION-CROSS] id=%d oldRegion=%d newMap=%d,%d newOff=%d,%d rw=%lu rh=%lu dOff=%d,%d", m_nMissleId, nOldRegion, nNewMapX, nNewMapY, nNewXOffset, nNewYOffset, nRegionWidth, nRegionHeight, nDOffsetX, nDOffsetY);
	if (nNewMapX < 0)
	{
		nNewRegion = LeftRegion(m_nRegionId);
		nNewMapX += nRegionWidth;
	}
	else if ((DWORD)nNewMapX >= nRegionWidth)
	{
		nNewRegion = RightRegion(m_nRegionId);
		nNewMapX -= nRegionWidth;
	}

	if (nNewRegion < 0) 
	{
		return FALSE; 
	}
	
	if (nNewMapY < 0)
	{
		nNewRegion = UpRegion(nNewRegion);
		nNewMapY += nRegionHeight;
	}
	else if (nNewMapY >= RegionHeight)
	{
		nNewRegion = DownRegion(nNewRegion);
		nNewMapY -= nRegionHeight;
	}
	
	//ÏÂÒ»¸öÎ»ÖÃÎª²»ºÏ·¨Î»ÖÃ£¬ÔòÏûÍö
	if (nNewRegion < 0) 
	{
		return FALSE; 
	}
	else
	{
		CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		AUTOLOG_EVERY(2000, "[MIS-REGION-OK] id=%d oldRegion=%d newRegion=%d newMap=%d,%d newOff=%d,%d", m_nMissleId, nOldRegion, nNewRegion, nNewMapX, nNewMapY, nNewXOffset, nNewYOffset);
		_ASSERT(m_nCurrentMapX >= 0  &&  m_nCurrentMapY >= 0);
		
		m_nRegionId	   = nNewRegion;
		m_nCurrentMapX = nNewMapX;
		m_nCurrentMapY = nNewMapY;
		m_nXOffset	   = nNewXOffset;
		m_nYOffset	   = nNewYOffset;
		CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		
		if (nOldRegion != m_nRegionId)
		{
			SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);
		}
	}
	return TRUE;
}

KMissle&	KMissle::operator=(KMissle& Missle)
{
	Missle.m_nTempParam1	=	0;
	Missle.m_nTempParam2	=	0;
	Missle.m_nDesMapX			=	0;
	Missle.m_nDesMapY			=	0;
	Missle.m_nDesRegion		=	0;
	Missle.m_bNeedReclaim	=	FALSE;
	Missle.m_nFirstReclaimTime = 0;
	Missle.m_nEndReclaimTime = 0;

	Missle.m_bCanSlow		=	m_bCanSlow;
	Missle.m_bCollideEvent	=	m_bCollideEvent;
	Missle.m_bCollideFriend =	m_bCollideFriend;
	Missle.m_bCollideVanish	=	m_bCollideVanish;
	Missle.m_bRangeDamage	=	m_bRangeDamage;
	Missle.m_eFollowKind	=	m_eFollowKind;
	Missle.m_eMoveKind		=	m_eMoveKind;
	Missle.m_nAction		=	m_nAction;
	Missle.m_nAngle			=	m_nAngle;
	Missle.m_nCollideRange	=	m_nCollideRange;
	Missle.m_nCurrentLife	=	0;
	Missle.m_nDamageRange	=	m_nDamageRange;
	Missle.m_nMissRate		=	0;
	Missle.m_nHitCount		=	0;
	Missle.m_nDamageRange	=	m_nDamageRange;
	Missle.m_nHeight		=	m_nHeight;
	Missle.m_nLifeTime		=	m_nLifeTime;
	Missle.m_nSpeed			=   m_nSpeed;
	Missle.m_nParam1		=	m_nParam1;
	Missle.m_nParam2		=	m_nParam2;
	Missle.m_nParam3		=	m_nParam3;
	Missle.m_nCurrentMapZ	=   m_nHeight >> 10;
	Missle.m_bFlyEvent		=	m_bFlyEvent;
	Missle.m_nFlyEventTime  =	m_nFlyEventTime;
	Missle.m_nZAcceleration =	m_nZAcceleration;
	Missle.m_nHeightSpeed	=	m_nHeightSpeed;
	Missle.m_bAutoExplode	=	m_bAutoExplode;
	Missle.m_ulDamageInterval = m_ulDamageInterval;
	strcpy(Missle.m_szMissleName	,	m_szMissleName);
	
#ifndef  _SERVER
	Missle.m_bMultiShow		=  m_bMultiShow;
	Missle.m_MissleRes.m_bLoopAnim = m_MissleRes.m_bLoopAnim;
	Missle.m_MissleRes.m_bHaveEnd = FALSE;
	Missle.m_btRedLum		= m_btRedLum;
	Missle.m_btGreenLum		= m_btGreenLum;
	Missle.m_btBlueLum		= m_btBlueLum;
	Missle.m_usLightRadius	= m_usLightRadius;
	int nOffset = 0;
	
	//Èç¹ûÊÇÏàÍ¬µÄ×Óµ¯¿ÉÒÔÒÔ²»Í¬·½Ê½ÏÔÊ¾Ê±£¬ÔòËæ»ú²úÉú
	if (m_bMultiShow)		
	{
		if (g_Random(2) == 0)
		{
			nOffset = 0;
		}
		else
			nOffset = MAX_MISSLE_STATUS;
	}
	
	for (int t = 0; t < MAX_MISSLE_STATUS ; t++)
	{
		strcpy(Missle.m_MissleRes.m_MissleRes[t].AnimFileName,m_MissleRes.m_MissleRes[t + nOffset].AnimFileName);
		
		Missle.m_MissleRes.m_MissleRes[t].nTotalFrame = m_MissleRes.m_MissleRes[t + nOffset].nTotalFrame;
		Missle.m_MissleRes.m_MissleRes[t].nDir = m_MissleRes.m_MissleRes[t + nOffset].nDir;
		Missle.m_MissleRes.m_MissleRes[t].nInterval = m_MissleRes.m_MissleRes[t + nOffset].nInterval;
		
		strcpy(Missle.m_MissleRes.m_MissleRes[t].SndFileName,m_MissleRes.m_MissleRes[t + nOffset].SndFileName);
	}
	Missle.m_MissleRes.m_bSubLoop = m_MissleRes.m_bSubLoop;
	Missle.m_MissleRes.m_nSubStart = m_MissleRes.m_nSubStart;
	Missle.m_MissleRes.m_nSubStop = m_MissleRes.m_nSubStop;
#endif	
	
	return (Missle);
}

/*!*****************************************************************************
// Function		: KMissle::ProcessDamage
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nNpcId
// Comments		:
// Author		: RomanDou
*****************************************************************************/
BOOL KMissle::ProcessDamage(int nNpcId)
{
#ifdef _SERVER

	_ASSERT (Npc[m_nLauncher].IsMatch(m_dwLauncherId));
	AUTOLOG_EVERY(500, "[E2-PDMG-IN] missle=%d skill=%d/%d launcher=%d(id=%u kind=%u) target=%d(id=%u kind=%u doing=%d life=%d camp=%d) relFlag=%d relReal=%d melee=%d phys=%d useAR=%d missrate=%d dohurt=%d attr=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, Npc[m_nLauncher].m_dwID, Npc[m_nLauncher].m_Kind, nNpcId, Npc[nNpcId].m_dwID, Npc[nNpcId].m_Kind, (int)Npc[nNpcId].m_Doing, Npc[nNpcId].m_CurrentLife, Npc[nNpcId].m_CurrentCamp, m_eRelation, (int)NpcSet.GetRelation(m_nLauncher, nNpcId), (int)m_bIsMelee, (int)m_bIsPhysical, (int)m_bUseAttackRating, m_nMissRate, (int)m_nDoHurtP, (m_pMagicAttribsData ? m_pMagicAttribsData->m_nDamageMagicAttribsNum : -1));

	AUTOLOG_EVERY(1000, "[MIS-DMG-NOATTRIB] id=%d skill=%d lv=%d launcher=%d npc=%d attribs=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcId, (m_pMagicAttribsData ? 1 : 0));
	if (m_pMagicAttribsData) 
	{
		KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId, m_nLevel);//Add by Phong KiÒu
		AUTOLOG_EVERY(500, "[MIS-DMG-FILTER] id=%d skill=%d lv=%d pSkill=%d launcher=%d npc=%d isPlayer=%d launcherCamp=%d npcCamp=%d", m_nMissleId, m_nSkillId, m_nLevel, (pSkill ? 1 : 0), m_nLauncher, nNpcId, Npc[nNpcId].IsPlayer(), Npc[m_nLauncher].m_CurrentCamp, Npc[nNpcId].m_CurrentCamp);
		if (pSkill && m_nLauncher != nNpcId)
		{
			AUTOLOG_EVERY(500, "[DMG-SKIP-TARGETKIND] msl=%d sk=%d lv=%d launcher=%d npc=%d isplayer=%d aura=%d nonpc=%d ally=%d camp_l=%d camp_t=%d -> return TRUE khong sat thuong", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcId, (int)Npc[nNpcId].IsPlayer(), (int)pSkill->IsAura(), (int)pSkill->IsTargetNoNpc(), (int)pSkill->IsTargetAlly(), (int)Npc[m_nLauncher].m_CurrentCamp, (int)Npc[nNpcId].m_CurrentCamp);
			if (!Npc[nNpcId].IsPlayer() && pSkill->IsAura() && pSkill->IsTargetNoNpc())
				return TRUE;
			//Fix lçi ch÷ ®á cïng PT vÉn BUFF ®­îc cho nhau chu do cung pt van nhan duoc vong ho tro tu nhan vat khac
			AUTOLOG_EVERY(1000, "[E2-PDMG-DENY-ALLY] missle=%d skill=%d/%d launcher=%d(camp=%d) target=%d(camp=%d) targetally=%d -> BO QUA, tra TRUE khong sat thuong", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, Npc[m_nLauncher].m_CurrentCamp, nNpcId, Npc[nNpcId].m_CurrentCamp, (int)pSkill->IsTargetAlly());
			if(Npc[nNpcId].IsPlayer() && Npc[m_nLauncher].IsPlayer() && pSkill->IsTargetAlly() && Npc[nNpcId].m_CurrentCamp == camp_free && Npc[m_nLauncher].m_CurrentCamp == camp_free)
				return TRUE;
		}

		AUTOLOG_EVERY(500, "[DMG-TRY] t=%u msl=%d sk=%d lv=%d launcher=%d(id=%u) npc=%d(id=%u) doing=%d hp=%d series=%d phys=%d melee=%d useAR=%d hurtp=%d missrate=%d", SubWorld[m_nSubWorldId].m_dwCurrentTime, m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, Npc[m_nLauncher].m_dwID, nNpcId, Npc[nNpcId].m_dwID, (int)Npc[nNpcId].m_Doing, Npc[nNpcId].m_CurrentLife, m_nMissleSeries, (int)m_bIsPhysical, (int)m_bIsMelee, (int)m_bUseAttackRating, m_nDoHurtP, m_nMissRate);
		if (Npc[nNpcId].ReceiveDamage(m_nLauncher, m_nMissleSeries, m_bIsPhysical, m_bIsMelee, m_pMagicAttribsData->m_pDamageMagicAttribs, m_bUseAttackRating, m_nDoHurtP, m_nMissRate))
		{
			AUTOLOG_EVERY(500, "[E2-PDMG-HIT] missle=%d skill=%d/%d launcher=%d target=%d ReceiveDamage=TRUE lifeconlai=%d statenum=%d immediatenum=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcId, Npc[nNpcId].m_CurrentLife, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
			if (m_pMagicAttribsData->m_nStateMagicAttribsNum > 0)
			{
				//---ViÕt thªm xö lý skill 120
				if(Npc[nNpcId].m_CurrentIgnoreNegativeStateP || Npc[nNpcId].m_CurrentReturnSkillPercent || Npc[nNpcId].m_CurrentIgnoreSkillPercent)
				{
					if (m_pMagicAttribsData->m_pStateMagicAttribs->nValue[1] && !m_pMagicAttribsData->m_pStateMagicAttribs->nValue[2] && 
						(pSkill->GetSkillStyle() == SKILL_SS_Missles || pSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState) && pSkill->IsTargetEnemy())
					{
						if (g_RandPercent(Npc[nNpcId].m_CurrentIgnoreNegativeStateP))
							return TRUE;

						if(pSkill->IsSkillReduceResist())
						{
							if (g_RandPercent(Npc[nNpcId].m_CurrentIgnoreSkillPercent))
							{
								KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
								memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
								DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
								DamageMagicAttribs[0].nValue[0] = 0;
								Npc[nNpcId].SetStateSkillEffect(nNpcId, 724, 1, DamageMagicAttribs, 1, GAME_FPS, TRUE);
								return TRUE;
							}
						}

						if (g_RandPercent(Npc[nNpcId].m_CurrentReturnSkillPercent))
						{
							KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
							memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
							DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
							DamageMagicAttribs[0].nValue[0] = 0;
							Npc[nNpcId].SetStateSkillEffect(nNpcId, 725, 1, DamageMagicAttribs, 1, GAME_FPS, TRUE);
							//Fix phan don bua chu
							Npc[m_nLauncher].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
						}
					}
				}
				//
				Npc[nNpcId].SetStateSkillEffect(m_nLauncher, m_nSkillId, m_nLevel, m_pMagicAttribsData->m_pStateMagicAttribs, m_pMagicAttribsData->m_nStateMagicAttribsNum, m_pMagicAttribsData->m_pStateMagicAttribs[0].nValue[1]);
			}
			//
			if (m_pMagicAttribsData->m_nImmediateMagicAttribsNum > 0)
			{
				Npc[nNpcId].SetImmediatelySkillEffect(m_nLauncher, m_pMagicAttribsData->m_pImmediateAttribs, m_pMagicAttribsData->m_nImmediateMagicAttribsNum);
			}
		}
		return TRUE;
	}
AUTOLOG_EVERY(1000, "[E2-PDMG-NOATTR] missle=%d skill=%d/%d launcher=%d target=%d m_pMagicAttribsData=NULL -> tra FALSE, KHONG he gay sat thuong", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcId);
#endif //_SERVER
	return FALSE;
}

void KMissle::DoVanish()
{
	AUTOLOG_EVERY(1000, "[MIS-STATE-VANISH] id=%d skill=%d lv=%d oldStatus=%d life=%d/%d start=%d launcher=%d follow=%d lastHit=%d region=%d map=%d,%d z=%d vanishEvent=%d", m_nMissleId, m_nSkillId, m_nLevel, (int)m_eMissleStatus, m_nCurrentLife, m_nLifeTime, m_nStartLifeTime, m_nLauncher, m_nFollowNpcIdx, m_nLastDoCollisionIdx, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentMapZ, m_bVanishedEvent);
	if (m_eMissleStatus == MS_DoVanish) return ;
	AUTOLOG_EVERY(1000, "[MSL-END] t=%u msl=%d sk=%d lv=%d launcher=%d follow=%d status=%d life=%d/%d start=%d pos(r=%d,%d,%d off %d,%d z=%d) barrier=%d lasthit=%d", SubWorld[m_nSubWorldId].m_dwCurrentTime, m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, m_nFollowNpcIdx, (int)m_eMissleStatus, m_nCurrentLife, m_nLifeTime, m_nStartLifeTime, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, m_nCurrentMapZ, (int)TestBarrier(), m_nLastDoCollisionIdx);
#ifndef _SERVER
	m_MissleRes.m_bHaveEnd = TRUE;
	m_nCollideOrVanishTime = m_nCurrentLife;
#endif
	if (m_bVanishedEvent)	
	{
		_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
		KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nSkillId,m_nLevel);
		if (pOrdinSkill)
        {
			pOrdinSkill->Vanish(this);
        }
	}
#ifdef _SERVER	//·þÎñÆ÷¶ËÊ±×Óµ¯Ò»µ©½øÈëÏûÍöÆÚÔòÖ±½ÓÉ¾³ýµô
	SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_DEL, m_nMissleId);
	m_eMissleStatus = MS_DoVanish;
	return ;
#endif
	m_eMissleStatus = MS_DoVanish;
#ifndef _SERVER 
	if (m_nRegionId < 0)
	{
		_ASSERT(0);
		m_bRemoving = TRUE;
		return ;
	}
#endif
}

void KMissle::DoCollision()
{
	AUTOLOG_EVERY(1000, "[MIS-STATE-COLLIDE] id=%d skill=%d lv=%d status=%d life=%d/%d colVanish=%d colEvent=%d lastHit=%d map=%d,%d z=%d", m_nMissleId, m_nSkillId, m_nLevel, (int)m_eMissleStatus, m_nCurrentLife, m_nLifeTime, m_bCollideVanish, m_bCollideEvent, m_nLastDoCollisionIdx, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentMapZ);
	if (m_eMissleStatus == MS_DoCollision) return;
	AUTOLOG_EVERY(1000, "[MSL-COLLIDE] t=%u msl=%d sk=%d launcher=%d pos(r=%d,%d,%d) life=%d/%d colvanish=%d colevent=%d lasthit=%d", SubWorld[m_nSubWorldId].m_dwCurrentTime, m_nMissleId, m_nSkillId, m_nLauncher, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nCurrentLife, m_nLifeTime, (int)m_bCollideVanish, (int)m_bCollideEvent, m_nLastDoCollisionIdx);
	
#ifndef _SERVER
	int nSrcX = 0 ;
	int nSrcY = 0 ;
	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
#endif
	
	if (m_bCollideEvent)	
	{
		_ASSERT(m_nSkillId < MAX_SKILL && m_nLevel < MAX_SKILLLEVEL);
		KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(m_nSkillId, m_nLevel);
		if (pOrdinSkill)
        {
			pOrdinSkill->Collidsion(this);
        }
	}
	
	if (m_bCollideVanish)
	{
#ifndef _SERVER
		m_MissleRes.m_bHaveEnd = TRUE;
#endif
		
#ifndef _SERVER 
		int nSrcX5 = 0 ;
		int nSrcY5 = 0 ;
		SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX5, &nSrcY5);
		CreateSpecialEffect(MS_DoVanish, nSrcX5, nSrcY5, m_nCurrentMapZ);
#endif
		
		DoVanish();
	}
	else 
	{
#ifndef _SERVER		
		//Ôö¼Ó×²ºóµÄÐ§¹û	
		if (m_MissleRes.SpecialMovieIsAllEnd())
			CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
#endif
		m_eMissleStatus = MS_DoFly;
	}
}

void KMissle::DoFly()
{
	AUTOLOG_EVERY(1000, "[MIS-STATE-FLY] id=%d skill=%d lv=%d oldStatus=%d life=%d/%d dir=%d fx=%d fy=%d speed=%d", m_nMissleId, m_nSkillId, m_nLevel, (int)m_eMissleStatus, m_nCurrentLife, m_nLifeTime, m_nDir, m_nXFactor, m_nYFactor, m_nSpeed);
	if (m_eMissleStatus == MS_DoFly) return ;
	//³õÊ¼»¯ÌùÍ¼
	m_eMissleStatus = MS_DoFly;
}

BOOL KMissle::GetOffsetAxis(int nSubWorld, int nSrcRegionId, int nSrcMapX, int nSrcMapY,
							int nOffsetMapX, int nOffsetMapY, 
							int &nDesRegionId, int &nDesMapX, int &nDesMapY)
{
	nDesRegionId = -1;
	// È·¶¨Ä¿±ê¸ñ×ÓÊµ¼ÊµÄREGIONºÍ×ø±êÈ·¶¨
	nDesMapX = nSrcMapX + nOffsetMapX;
	nDesMapY = nSrcMapY + nOffsetMapY;
	
	AUTOLOG_EVERY(5000, "[MIS-OFFAXIS-BAD] sw=%d srcRegion=%d srcMap=%d,%d offset=%d,%d", nSubWorld, nSrcRegionId, nSrcMapX, nSrcMapY, nOffsetMapX, nOffsetMapY);
	if (nSrcRegionId < 0) 
		return FALSE;

	int nSearchRegion = nSrcRegionId;
	if (nDesMapX < 0)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[2];
		nDesMapX += SubWorld[nSubWorld].m_nRegionWidth;
	}
	else if (nDesMapX >= SubWorld[nSubWorld].m_nRegionWidth)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[6];
		nDesMapX -= SubWorld[nSubWorld].m_nRegionWidth;
	}
	if (nSearchRegion < 0) 
		return FALSE;
	
	if (nDesMapY < 0)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[4];
		nDesMapY += SubWorld[nSubWorld].m_nRegionHeight;
	}
	else if (nDesMapY >= SubWorld[nSubWorld].m_nRegionHeight)
	{
		nSearchRegion = SubWorld[nSubWorld].m_Region[nSearchRegion].m_nConnectRegion[0];
		nDesMapY -= SubWorld[nSubWorld].m_nRegionHeight;
	}	

	if (nSearchRegion < 0) 
		return FALSE;
	nDesRegionId = nSearchRegion;
	return TRUE;
	//_ASSERT(nSearchRegion >= 0);
	//int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nDesMapX, nDesMapY, nLauncherIdx, relation_all);
}

/*!*****************************************************************************
// Function		: KMissle::ProcessCollision
// Purpose		: 
// Return		: int 
// Argumant		: int nLauncherIdx
// Argumant		: int nRegionId
// Argumant		: int nMapX
// Argumant		: int nMapY
// Argumant		: int nRange
// Argumant		: MISSLE_RELATION eRelation
// Comments		:
// Author		: RomanDou
*****************************************************************************/
int KMissle::ProcessCollision(int nLauncherIdx, int nRegionId, int nMapX, int nMapY, int nRange , int eRelation)
{
#ifdef TOOLVERSION 
	return 0;
#endif
#ifdef _SERVER
	if (m_ulDamageInterval)
	{
		AUTOLOG_EVERY(1000, "[COLL-DMG-COOLDOWN] msl=%d sk=%d launcher=%d next=%lu now=%lu interval=%lu -> BO QUA dot sat thuong nay", m_nMissleId, m_nSkillId, nLauncherIdx, m_ulNextCalDamageTime, g_SubWorldSet.GetGameTime(), m_ulDamageInterval);
		if (m_ulNextCalDamageTime > g_SubWorldSet.GetGameTime())
		{
			return FALSE;
		}
		else
		{
			
			// 6.29 romandou missledamage interval 
			m_ulNextCalDamageTime = g_SubWorldSet.GetGameTime() + m_ulDamageInterval;
		}
	}
#endif
	AUTOLOG_EVERY(2000, "[MIS-PROC-ARG] id=%d skill=%d launcherIdx=%d region=%d map=%d,%d range=%d rel=%d hitCount=%d", m_nMissleId, m_nSkillId, nLauncherIdx, nRegionId, nMapX, nMapY, nRange, eRelation, m_nHitCount);
	if (nLauncherIdx <= 0 ) return 0;
	if (nRange <= 0) return 0;
	if (nRegionId < 0) return 0; //#can kiem tra
	AUTOLOG_EVERY(2000, "[COLL-SCAN] msl=%d sk=%d launcher=%d region=%d map(%d,%d) range=%d (nRangeX=%d) rel=%d hitcount=%d dmginterval=%d", m_nMissleId, m_nSkillId, nLauncherIdx, nRegionId, nMapX, nMapY, nRange, nRange / 2, eRelation, m_nHitCount, (int)m_ulDamageInterval);
	int nRangeX = nRange / 2;
	int	nRangeY = nRangeX;
	int	nSubWorld = Npc[nLauncherIdx].m_SubWorldIndex;
	
	_ASSERT(Npc[nLauncherIdx].m_SubWorldIndex >= 0);
	_ASSERT(nRegionId >= 0);
	
	int	nRegion = nRegionId;
	int	nRet = 0;
	int	nRMx, nRMy, nSearchRegion;

	// ¼ì²é·¶Î§ÄÚµÄ¸ñ×ÓÀïµÄNPC
	for (int i = -nRangeX; i <= nRangeX; i++)
	{
		for (int j = -nRangeY; j <= nRangeY; j++)
		{
			// È¥µô±ß½Ç¼¸¸ö¸ñ×Ó£¬±£Ö¤ÊÓÒ°ÊÇÍÖÔ²ÐÎ
			//if ((i * i + j * j ) > nRangeX * nRangeX)
				//continue;
			if (!GetOffsetAxis(nSubWorld, nRegionId, nMapX, nMapY, i , j , nSearchRegion, nRMx, nRMy))
				continue;

			_ASSERT(nSearchRegion >= 0);
			int nNpcIdx = SubWorld[nSubWorld].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, nLauncherIdx, eRelation);
			if (nNpcIdx > 0)	
			{
				if(Npc[nNpcIdx].GetProtectTime() > 0 && eRelation == relation_enemy) //®ang trong tr¹ng th¸i bÊt tö bÞ kÎ thï ®¸nh vµo return
					return 0; //vong tron bat tu, vßng trßn bÊt tö
				AUTOLOG_EVERY(2000, "[COLL-NPC-FOUND] msl=%d sk=%d launcher=%d npc=%d(id=%u) o(%d,%d) region=%d rel=%d protect=%d doing=%d nRet=%d hitcount=%d", m_nMissleId, m_nSkillId, nLauncherIdx, nNpcIdx, Npc[nNpcIdx].m_dwID, nRMx, nRMy, nSearchRegion, eRelation, Npc[nNpcIdx].GetProtectTime(), (int)Npc[nNpcIdx].m_Doing, nRet, m_nHitCount);
				nRet++;
#ifndef _SERVER
				int nSrcX = 0;
				int nSrcY = 0;
				SubWorld[0].Map2Mps(nSearchRegion, Npc[nNpcIdx].m_MapX,Npc[nNpcIdx].m_MapY, Npc[nNpcIdx].m_OffX, Npc[nNpcIdx].m_OffY,  &nSrcX, &nSrcY);
				
				if (m_bFollowNpcWhenCollid)
					CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ, nNpcIdx);
				else 
					CreateSpecialEffect(MS_DoCollision, nSrcX, nSrcY, m_nCurrentMapZ);
#else
				AUTOLOG_EVERY(1000, "[MIS-PROC-HIT] id=%d skill=%d lv=%d launcher=%d npc=%d npcId=%lu hp=%d doing=%d nRet=%d hitCount=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nLauncher, nNpcIdx, Npc[nNpcIdx].m_dwID, Npc[nNpcIdx].m_CurrentLife, (int)Npc[nNpcIdx].m_Doing, nRet, m_nHitCount);
				ProcessDamage(nNpcIdx);				
#endif
				if(m_nHitCount > 0)
				{
					if(nRet >= m_nHitCount)
						return nRet;
				}
			}
		}
	}
	return nRet;
}

int KMissle::ProcessCollision()
{
#ifdef TOOLVERSION
	return 0;
#endif
	AUTOLOG_EVERY(1000, "[COLL-CLIENTSEND] msl=%d sk=%d launcher=%d m_bClientSend=%d -> BO QUA ProcessCollision (khong quet sat thuong)", m_nMissleId, m_nSkillId, m_nLauncher, (int)m_bClientSend);
	if (m_bClientSend) return 0;
	return ProcessCollision(m_nLauncher, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nDamageRange , m_eRelation);
}

#ifndef _SERVER 

#define MISSLE_Y_OFFSET 1
BOOL KMissle::CreateSpecialEffect(eMissleStatus eStatus, int nPX, int nPY, int nPZ, int nNpcIndex)
{
	KSkillSpecialNode * pNode = NULL;
	if (nNpcIndex > 0)
	{
		pNode = (KSkillSpecialNode*)m_MissleRes.m_SkillSpecialList.GetHead();
		while(pNode)
		{
			if (pNode->m_pSkillSpecial->m_dwMatchID == Npc[nNpcIndex].m_dwID) return FALSE;
			pNode = (KSkillSpecialNode*)pNode->GetNext();
		}
	}
	m_MissleRes.PlaySound(eStatus, nPX, nPY, 0);
	if(Option.GetLow(LowMissle))
	{
		if (!g_MisslesLib[1].m_MissleRes.m_MissleRes[eStatus].AnimFileName[0])
			return FALSE; 
	}
	else
	{
		if (!m_MissleRes.m_MissleRes[eStatus].AnimFileName[0])
			return FALSE; 
	}
	pNode = new KSkillSpecialNode;
	KSkillSpecial * pSkillSpecial = new KSkillSpecial;
	pNode->m_pSkillSpecial = pSkillSpecial;
	
	int nSrcX = nPX;
	int nSrcY = nPY;
	
	pSkillSpecial->m_nPX = nSrcX;
	pSkillSpecial->m_nPY = nSrcY - 5;// MISSLE_Y_OFFSET;
	pSkillSpecial->m_nPZ = nPZ;
	pSkillSpecial->m_nNpcIndex = nNpcIndex;
	pSkillSpecial->m_dwMatchID = Npc[nNpcIndex].m_dwID;
	if(Option.GetLow(LowMissle))
	{
		pSkillSpecial->m_pMissleRes = &g_MisslesLib[1].m_MissleRes.m_MissleRes[eStatus];
	}
	else
	{
		pSkillSpecial->m_pMissleRes = &m_MissleRes.m_MissleRes[eStatus];
	}
	pSkillSpecial->m_nBeginTime = g_SubWorldSet.GetGameTime();
	pSkillSpecial->m_nEndTime = g_SubWorldSet.GetGameTime() + (pSkillSpecial->m_pMissleRes->nInterval * pSkillSpecial->m_pMissleRes->nTotalFrame / pSkillSpecial->m_pMissleRes->nDir);
	pSkillSpecial->m_nCurDir = g_DirIndex2Dir(m_nDirIndex, m_MissleRes.m_MissleRes[eStatus].nDir);
	pSkillSpecial->Init();
	m_MissleRes.m_SkillSpecialList.AddTail(pNode);
	
	return TRUE;
}

BOOL	KMissle::CreateMissleForShow(char * szMovie, char * szFormat, char * szSound, TMissleForShow * pShowParam)
{
	if (!pShowParam || !szMovie || !szMovie[0])
		return FALSE;
	int nPX = 0;
	int nPY = 0;
	int nPZ = 0;
	
	if (pShowParam->nNpcIndex > 0)
	{
		Npc[pShowParam->nNpcIndex].GetMpsPos(&nPX, &nPY);
	}
	else
	{
		nPX = pShowParam->nPX;
		nPY = pShowParam->nPY;
	}

	int nSubWorldId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex;
	int nMissleIndex = MissleSet.Add(nSubWorldId , nPX , nPY);
	if (nMissleIndex < 0)	
		return FALSE;
	
	Missle[nMissleIndex].m_nDir				= Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Dir;
	Missle[nMissleIndex].m_nDirIndex		= g_Dir2DirIndex(Missle[nMissleIndex].m_nDir, MaxMissleDir);
	Missle[nMissleIndex].m_nFollowNpcIdx	= 0;
	Missle[nMissleIndex].m_dwBornTime		= SubWorld[nSubWorldId].m_dwCurrentTime;
	Missle[nMissleIndex].m_nSubWorldId		= nSubWorldId;
	Missle[nMissleIndex].m_nLauncher		= pShowParam->nLauncherIndex;
	Missle[nMissleIndex].m_dwLauncherId		= Npc[pShowParam->nLauncherIndex].m_dwID;
	Missle[nMissleIndex].m_nPKFlag			= Npc[pShowParam->nLauncherIndex].m_nPKFlag;
	Missle[nMissleIndex].m_nMissleSeries	= -1;
	KSkill * pSkill =(KSkill*) Npc[pShowParam->nLauncherIndex].GetActiveSkill();
	if(pSkill)
		Missle[nMissleIndex].m_nMissleSeries	= pSkill->GetSkillSeries();

	Missle[nMissleIndex].m_nParentMissleIndex = 0;
	
	Missle[nMissleIndex].m_nSkillId			= 0;
	Missle[nMissleIndex].m_nStartLifeTime	= 0;
	Missle[nMissleIndex].m_nLifeTime		= 1;
	Missle[nMissleIndex].m_nRefPX			= 0;
	Missle[nMissleIndex].m_nRefPY			= 0;
	Missle[nMissleIndex].m_MissleRes.Clear();

	Missle[nMissleIndex].m_MissleRes.LoadResource(MS_DoWait, szMovie, szSound);
	char * pcszTemp = szFormat;
	Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nTotalFrame = KSG_StringGetInt(&pcszTemp, 100);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nDir = KSG_StringGetInt(&pcszTemp, 16);
	KSG_StringSkipSymbol(&pcszTemp, ',');
    Missle[nMissleIndex].m_MissleRes.m_MissleRes[MS_DoWait].nInterval = KSG_StringGetInt(&pcszTemp, 1);

	Missle[nMissleIndex].CreateSpecialEffect(MS_DoWait, nPX, nPY, nPZ, pShowParam->nNpcIndex);
	return TRUE;
}

void	KMissle::GetLightInfo(KLightInfo * pLightInfo)
{
	if (!pLightInfo) 
	{
		return ;
	}
	
	int nPX, nPY, nPZ;
	GetMpsPos(&nPX, &nPY);
	nPZ = m_nCurrentMapZ;
	
	pLightInfo->oPosition.nX = nPX;
	pLightInfo->oPosition.nY = nPY;
	pLightInfo->oPosition.nZ = nPZ;
	pLightInfo->dwColor = 0xff000000 | m_btRedLum << 16 | m_btGreenLum << 8 | m_btBlueLum;
	pLightInfo->nRadius = m_usLightRadius;
}
#endif

void KMissle::DoWait()
{
	//	if (m_eMissleStatus == MS_DoWait) return;
	m_eMissleStatus = MS_DoWait;
	
#ifndef _SERVER 
	int nSrcX = 0 ;
	int nSrcY = 0 ;
	SubWorld[0].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY,m_nXOffset, m_nYOffset, &nSrcX, &nSrcY);
	CreateSpecialEffect(MS_DoWait, nSrcX, nSrcY, m_nCurrentMapZ);
#endif
	
}
//µ±×Óµú½øÈëfly×´Ì¬Ê±£¬ÐèÒª¸ù¾ÝÇé¿ö±ä¶¯
BOOL	KMissle::PrePareFly()
{
	if (m_eMoveKind == MISSLE_MMK_RollBack)
		m_nTempParam2 =  m_nStartLifeTime + (m_nLifeTime - m_nStartLifeTime ) / 2;

	//ÊÇ·ñ»áËæ·¢ËÍÕßµÄÒÆ¶¯¶øÖÐ¶Ï£¬ÀàÊ½Ä§ÊÞ3ÖÐ´óÐÍ·¨Êõ
	if (m_nInteruptTypeWhenMove)
	{
		int nPX, nPY;
		Npc[m_nLauncher].GetMpsPos(&nPX, &nPY);
		AUTOLOG_EVERY(1000, "[MIS-PREP-MOVED] id=%d skill=%d lv=%d interrupt=%d nowPX=%d nowPY=%d srcPX=%d srcPY=%d", m_nMissleId, m_nSkillId, m_nLevel, m_nInteruptTypeWhenMove, nPX, nPY, m_nLauncherSrcPX, m_nLauncherSrcPY);
		if (nPX != m_nLauncherSrcPX || nPY != m_nLauncherSrcPY)
		{
			return false;
		}
	}
	
	//×ÓµúÎ»ÖÃÐèÒª¸üÕýÎªµ½ÊÊµ±µÄÎ»ÖÃ£¨×Óµ¯µÄ³öÏÖ×ÜÊÇÒÔÄ³¸ö¿ÉÄÜÎ»ÖÃÔÚ²»¶Ï±ä»¯µÄÎïÌåÎª²ÎÕÕÎï£©
	if (m_bHeelAtParent)
	{
		int nNewPX = 0;
		int nNewPY = 0;
		
		if (m_nParentMissleIndex) // ²Î¿¼µãÎªÄ¸×Óµ¯
		{
			AUTOLOG_EVERY(1000, "[MIS-PREP-PARENT] id=%d skill=%d parent=%d parentLauncherId=%lu myLauncherId=%lu refPX=%d refPY=%d", m_nMissleId, m_nSkillId, m_nParentMissleIndex, Missle[m_nParentMissleIndex].m_dwLauncherId, m_dwLauncherId, m_nRefPX, m_nRefPY);
			if (Missle[m_nParentMissleIndex].m_dwLauncherId != m_dwLauncherId)
			{
				return false;
			}
			else
			{
				int nParentPX, nParentPY;
				int nSrcPX, nSrcPY;
				Missle[m_nParentMissleIndex].GetMpsPos(&nParentPX, &nParentPY);
				GetMpsPos(&nSrcPX, &nSrcPY);
				nNewPX = nSrcPX + (nParentPX - m_nRefPX);
				nNewPY = nSrcPY + (nParentPY - m_nRefPY);
			}
		}
		else
			//²Î¿¼µãÎª·¢ËÍÕß
		{
			_ASSERT(m_nLauncher > 0);
			int nParentPX, nParentPY;
			int nSrcPX, nSrcPY;
			
			Npc[m_nLauncher].GetMpsPos(&nParentPX, &nParentPY);
			GetMpsPos(&nSrcPX, &nSrcPY);
			
			nNewPX = nSrcPX + (nParentPX - m_nRefPX);
			nNewPY = nSrcPY + (nParentPY - m_nRefPY);
		}
		
		int nOldRegion = m_nRegionId;
		CurRegion.DecRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		SubWorld[m_nSubWorldId].Mps2Map(nNewPX, nNewPY, &m_nRegionId, &m_nCurrentMapX, &m_nCurrentMapY, &m_nXOffset, &m_nYOffset);
		CurRegion.AddRef(m_nCurrentMapX, m_nCurrentMapY, obj_missle);
		
		if (nOldRegion != m_nRegionId)
		{
			SubWorld[m_nSubWorldId].m_WorldMessage.Send(GWM_MISSLE_CHANGE_REGION, nOldRegion, m_nRegionId, m_nMissleId);
		} 
		
	}
	
	return true;
	
}

int KMissle::CheckNearestCollision()
{
	int nSearchRegion = 0;
	int nRMx = 0;
	int nRMy = 0;
	BOOL bCollision = TRUE;
	int nNpcIdx = 0;
	int nDX = 0;
	int nDY = 0;
	int nNpcOffsetX = 0;
	int nNpcOffsetY = 0;
	int nAbsX = 0;
	int nAbsY = 0;
	int nCellWidth = CellWidth;
	int nCellHeight = CellHeight;
	_ASSERT(nCellWidth > 0 && nCellHeight > 0);
	
	for (int i = -1; i <= 1; i ++)
		for (int j = -1; j <= 1; j ++)
		{
			if (!KMissle::GetOffsetAxis(
				m_nSubWorldId,
				m_nRegionId, 
				m_nCurrentMapX, 
				m_nCurrentMapY, 
				i , 
				j , 
				nSearchRegion, 
				nRMx, 
				nRMy
				))
				continue;
			
			_ASSERT(nSearchRegion >= 0);
			nNpcIdx = SubWorld[m_nSubWorldId].m_Region[nSearchRegion].FindNpc(nRMx, nRMy, m_nLauncher, m_eRelation);
			
			if (nNpcIdx > 0)
			{
				bCollision = TRUE;
				nDX = m_nCurrentMapX - Npc[nNpcIdx].m_MapX;
				nDY = m_nCurrentMapY - Npc[nNpcIdx].m_MapY;
				nNpcOffsetX = Npc[nNpcIdx].m_OffX;
				nNpcOffsetY = Npc[nNpcIdx].m_OffY;
				nAbsX = abs(nDX);
				nAbsY = abs(nDY);
				
				if (nAbsX)
				{
					if (nDX < 0)
					{
						if (nCellWidth - m_nXOffset + nNpcOffsetX > nCellWidth)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
					else if (nDX > 0)
					{
						if (nCellWidth - nNpcOffsetX + m_nXOffset > nCellWidth)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
				}
				
				if (nAbsY)
				{
					if (nDY <0)
					{
						if (nCellHeight - m_nYOffset + nNpcOffsetY > nCellHeight)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
					else if (nDY >0)
					{
						if (nCellHeight - nNpcOffsetY + m_nYOffset > nCellHeight)
						{
							bCollision = FALSE;
							goto CheckCollision;
						}
					}
				}
				
				
CheckCollision:
				AUTOLOG_EVERY(2000, "[MSL-NEARMISS] msl=%d sk=%d npc=%d(id=%u) d_cell(%d,%d) msloff(%d,%d) npcoff(%d,%d) cell(%d,%d) bCollision=%d", m_nMissleId, m_nSkillId, nNpcIdx, Npc[nNpcIdx].m_dwID, nDX, nDY, m_nXOffset, m_nYOffset, nNpcOffsetX, nNpcOffsetY, nCellWidth, nCellHeight, (int)bCollision);
				if (bCollision)
					return nNpcIdx;
			}
		}
		
		return 0;
}

void	KMissle::GetMpsPos(int *pPosX, int *pPosY)
{
	AUTOLOG_EVERY(5000, "[MIS-MPS-BADREGION] id=%d skill=%d sw=%d region=%d map=%d,%d off=%d,%d", m_nMissleId, m_nSkillId, m_nSubWorldId, m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset);
	SubWorld[m_nSubWorldId].Map2Mps(m_nRegionId, m_nCurrentMapX, m_nCurrentMapY, m_nXOffset, m_nYOffset, pPosX, pPosY);
};