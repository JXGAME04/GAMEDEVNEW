#include "KCore.h"
#include <math.h>
#include "KNpc.h"
#include "KSubWorld.h"
#include "KRegion.h"
#include "GameDataDef.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KNpcTemplate.h"
#include "KDaTauCap.h"	// (r5f) DATAU_SAPMAP_ID - id dat cho, khong cap cho npc
#ifndef _SERVER
#include "CoreShell.h"
#include "Scene\KScenePlaceC.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KOption.h"
#ifndef TOOLVERSION
#include "../../Headers/IClient.h"
#endif
#endif
#include "Scene/SceneDataDef.h"
#include "KMath.h"

KNpcSet	NpcSet;

KNpcSet::KNpcSet()
{
	m_dwIDCreator = 1000;
#ifdef _SERVER
	m_nPKDamageRate = 0;
	m_nLevelDistance = 0;
	m_nNpcSpecialDamageRate = 0;
	m_nKillerPKFactionAddPKValue = 0;
	m_nFactionPKFactionAddPKValue = 0;
	m_nEnmityAddPKValue = 0;
	m_nBeKilledAddPKValue = 0;
#endif
}

void KNpcSet::Init()
{
	int i = 0;
    GenRelationTable();

	m_FreeIdx.Init(MAX_NPC);
	m_UseIdx.Init(MAX_NPC);

	for (i = MAX_NPC - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
		Npc[i].m_Node.m_nIndex = i;
#ifdef _SERVER
		Npc[i].m_cDeathCalcExp.Init(i);
		Npc[i].m_cGold.Init(i);
#endif
	}

	LoadPlayerBaseValue(PLAYER_BASE_VALUE);

#ifdef _SERVER
	KIniFile	cPKIni;
//	g_SetFilePath("\\");
	if (cPKIni.Load(PLAYER_PK_RATE_FILE))
	{
		cPKIni.GetInteger("PK", "Rate", 20, &m_nPKDamageRate);
		cPKIni.GetInteger("PK", "SpecialRate", 50, &m_nNpcSpecialDamageRate);
		cPKIni.GetInteger("PK", "FactionPKFaction", 1, &m_nFactionPKFactionAddPKValue);
		cPKIni.GetInteger("PK", "KillerPKFaction", 1, &m_nKillerPKFactionAddPKValue);
		cPKIni.GetInteger("PK", "EnmityPK", 2, &m_nEnmityAddPKValue);
		cPKIni.GetInteger("PK", "BeKilled", -1, &m_nBeKilledAddPKValue);
		cPKIni.GetInteger("PK", "LevelDistance", 25, &m_nLevelDistance);
	}
	else
	{
		m_nPKDamageRate = 20;
		m_nNpcSpecialDamageRate = 50;
		m_nFactionPKFactionAddPKValue = 1;
		m_nKillerPKFactionAddPKValue = 1;
		m_nEnmityAddPKValue = 2;					//cuu sat len pk 2
		m_nBeKilledAddPKValue = -1;
		m_nLevelDistance = 25;
	}
#endif

#ifndef _SERVER
	m_nShowPateFlag = PATE_CHAT;
	ZeroMemory(m_RequestNpc, sizeof(m_RequestNpc));
	m_RequestFreeIdx.Init(MAX_NPC_REQUEST);
	m_RequestUseIdx.Init(MAX_NPC_REQUEST);

	for (i = MAX_NPC_REQUEST - 1; i > 0; i--)
	{
		m_RequestFreeIdx.Insert(i);
	}
#endif

}

void KNpcSet::LoadPlayerBaseValue(LPSTR szFile)
{
	KIniFile	File;

	File.Load(szFile);

	File.GetInteger("Common", "HurtFrame", 12, &m_cPlayerBaseValue.nHurtFrame);
	File.GetInteger("Common", "RunSpeed", 10, &m_cPlayerBaseValue.nRunSpeed);
	File.GetInteger("Common", "WalkSpeed", 5, &m_cPlayerBaseValue.nWalkSpeed);
	File.GetInteger("Common", "AttackFrame", 20, &m_cPlayerBaseValue.nAttackFrame);
#ifndef _SERVER
	File.GetInteger("Male", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[0]);
	File.GetInteger("Female", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[1]);
	File.GetInteger("Male", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[0]);
	File.GetInteger("Female", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[1]);
	File.GetInteger("Male", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[0]);
	File.GetInteger("Female", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[1]);
#endif
	File.Clear();
}

BOOL KNpcSet::IsNpcExist(int nIdx, DWORD dwId)
{
	if (Npc[nIdx].m_dwID == dwId)
		return TRUE;
	else
		return FALSE;
}

//#moi quan he trang thai chien dau giua cac npc va player
NPC_RELATION KNpcSet::GenOneRelation(NPCKIND Kind1, NPCKIND Kind2, NPCCAMP Camp1, NPCCAMP Camp2, NPCFIGHT Fight1, NPCFIGHT Fight2)
{
	if (Kind1 == kind_dialoger || Kind2 == kind_dialoger)
		return relation_dialog;

	if (Kind1 >= kind_bird || Kind2 >= kind_bird)//#chua hoan thien
		return relation_none;

	if (Camp1 == camp_event || Camp2 == camp_event)
		return relation_none;
	
	if ((Camp1 == camp_begin && Camp2 == camp_animal)
		||(Camp1 == camp_animal && Camp2 == camp_begin))
		return relation_enemy;

	if (Camp1 == camp_begin || Camp2 == camp_begin)
		return relation_ally;
	
	// [BDH 28/08] Ban dong hanh - hanh vi ban VLTK/Linux: pet KHONG THE
	// danh/bi danh boi NGUOI CHOI hay pet khac (ke ca PK - doi thu khong
	// giet duoc pet); voi NPC thuong theo phe binh thuong (quai va pet van
	// danh nhau - pet chet thi hon me). Chan o TANG BANG de moi duong
	// (chon muc tieu, dan/AOE, AI quai, AI pet) cung mot luat.
	if (Kind1 == kind_partner || Kind2 == kind_partner)
	{
		NPCKIND KindKia = (Kind1 == kind_partner) ? Kind2 : Kind1;
		if (KindKia == kind_partner)
			return relation_none;
		if (KindKia == kind_player)
			return relation_ally;
		if (Camp1 == Camp2)
			return relation_ally;
		return relation_enemy;
	}
	
	if (Kind1 == kind_player && Kind2 == kind_player)
	{	
		if (Camp1 == camp_free || Camp2 == camp_free)
			if(Fight1 == fight_active || Fight2 == fight_active)
				return relation_enemy;
			else
				return relation_none;
	}

	if (Camp1 == Camp2)
		return relation_ally;

	if (Kind1 == kind_normal || Kind2 == kind_normal)
		return relation_enemy;

	if(Fight1 == fight_none || Fight2 == fight_none)
		return relation_none;
	else
		return relation_enemy;
}

int KNpcSet::GenRelationTable()
{
    int nKind1 = 0;
    int nKind2 = 0;
    int nCamp1 = 0;
    int nCamp2 = 0;
	int nFight1 = 0; // load them 
	int nFight2 = 0; // load them
	
    for (nKind1 = 0; nKind1 < kind_num; nKind1++)
    {
        for (nKind2 = 0; nKind2 < kind_num; nKind2++)
        {
            for (nCamp1 = 0; nCamp1 < camp_num; nCamp1++)
            {
                for (nCamp2 = 0; nCamp2 < camp_num; nCamp2++)
                {	
					for(nFight1 = 0; nFight1 < fight_num; nFight1++)
					{
						for(nFight2 = 0; nFight2 < fight_num; nFight2++)
						{
							m_RelationTable[nKind1][nKind2][nCamp1][nCamp2][nFight1][nFight2] = GenOneRelation(
										(NPCKIND)nKind1, 
										(NPCKIND)nKind2, 
										(NPCCAMP)nCamp1, 
										(NPCCAMP)nCamp2,
										(NPCFIGHT)nFight1,
										(NPCFIGHT)nFight2);
						}
					}
                }
            }
        }
    }
    return true;
}

int	KNpcSet::SearchID(DWORD dwID)
{
//	int nMaxNpc = m_BinTree.GetCount();
//	for (int i = 0; i < MAX_NPC; i++)
//	{
//		if (Npc[i].m_dwID == dwID)
//			return Npc[i].m_Index;
//	}
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_dwID == dwID)
			return nIdx;
	}

	return 0;
}

#ifndef _SERVER
int		KNpcSet::SearchClientID(KClientNpcID sClientID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID == sClientID.m_dwRegionID &&
			Npc[nIdx].m_sClientNpcID.m_nNo == sClientID.m_nNo)
			return nIdx;
	}

	return 0;
}
#endif

int KNpcSet::SearchName(LPSTR szName)
{
//	for (int i = 0; i < MAX_NPC; i ++)
//	{
//		if (!strcmp(Npc[i].Name, szName))
//			return i ;
//	}
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name, szName))
			return nIdx;
	}
	return 0;
}

int KNpcSet::SearchNameNpcID(DWORD dwID)
{
	int nIdx = 0;
	DWORD	dwNameID = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind == kind_player)
			continue;
		dwNameID = g_FileName2Id(Npc[nIdx].Name);
		if (dwID == dwNameID)
			return nIdx;
	}
	return 0;
}

int KNpcSet::SearchNameID(DWORD dwID)
{
	int nIdx = 0;
	DWORD	dwNameID = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		dwNameID = g_FileName2Id(Npc[nIdx].Name);
		if (dwID == dwNameID)
			return nIdx;
	}
	return 0;
}

int KNpcSet::SearchUUID(DWORD dwID)
{
	int nIdx = 0;
	DWORD	dwNameID = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		DWORD dwNameID = 0;
#ifdef _SERVER
		dwNameID = Player[Npc[nIdx].GetPlayerIdx()].m_dwID;
#else
		dwNameID = g_FileName2Id(Npc[nIdx].Name);
#endif
		if (dwID == dwNameID)
			return nIdx;
	}
	return 0;
}

int KNpcSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

#ifndef _SERVER
int		KNpcSet::AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo)
{
	int		nNpcNo, nNpcSettingIdxInfo, nMapX, nMapY, nOffX, nOffY, nRegion;

	nNpcSettingIdxInfo = MAKELONG(1, nTemplateID);
	SubWorld[0].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;
	int nSeries = 0;

	nNpcNo = this->AddNpcSet1(nNpcSettingIdxInfo, nSeries, 0, nRegion, nMapX, nMapY, nOffX, nOffY);//#can kiem tra
	if (nNpcNo > 0)
	{
		Npc[nNpcNo].m_sClientNpcID.m_dwRegionID = MAKELONG(nRegionX, nRegionY);
		Npc[nNpcNo].m_sClientNpcID.m_nNo = nNo;
		Npc[nNpcNo].m_RegionIndex = nRegion;
		Npc[nNpcNo].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		Npc[nNpcNo].m_bClientOnly = TRUE;
		Npc[nNpcNo].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		SubWorld[0].m_Region[nRegion].DecRef(Npc[nNpcNo].m_MapX, Npc[nNpcNo].m_MapY, obj_npc);
	}

	return nNpcNo;
}
#endif

#ifdef _SERVER
int KNpcSet::AddNpcSet3(int nSubWorld, void* pNpcInfo)	//Hµm add npc Î server
{
	KSPNpc*	pKSNpcInfo = (KSPNpc *)pNpcInfo;
	int nMpsX = pKSNpcInfo->nPositionX;
	int nMpsY = pKSNpcInfo->nPositionY;
	int Map = SubWorld[nSubWorld].m_SubWorldID;
	int	nNpcSettingIdxInfo = MAKELONG(pKSNpcInfo->nLevel, pKSNpcInfo->nTemplateID);

	if (Map == 1 || Map == 2 || Map == 11 || Map == 13 || Map == 20 || Map == 25 || Map == 37 || Map == 45 || Map == 46 || Map == 47 || Map == 48 || Map == 50 || Map == 53
		|| Map == 54 || Map == 59 || Map == 65 || Map == 73 || Map == 78 || Map == 80 || Map == 81 || Map == 82 || Map == 99 || Map == 100 || Map == 101 || Map == 103
		|| Map == 111 || Map == 121 || Map == 131 || Map == 140 || Map == 153 || Map == 154 || Map == 162 || Map == 174 || Map == 176 || Map == 183 || Map == 190 || Map == 195)
		nNpcSettingIdxInfo = MAKELONG(10, pKSNpcInfo->nTemplateID);

	if (Map == 3 || Map == 7 || Map == 14 || Map == 19 || Map == 43 || Map == 44 || Map == 70 || Map == 71 || Map == 83 || Map == 115 || Map == 179)
		nNpcSettingIdxInfo = MAKELONG(20, pKSNpcInfo->nTemplateID);

	if (Map == 4 || Map == 8 || Map == 22 || Map == 74 || Map == 77 || Map == 90 || Map == 92 || Map == 136 || Map == 141 || Map == 145 || Map == 146 || Map == 170 || Map == 193)
		nNpcSettingIdxInfo = MAKELONG(30, pKSNpcInfo->nTemplateID);

	if (Map == 5 || Map == 6 || Map == 21 || Map == 23 || Map == 28 || Map == 41 || Map == 91 || Map == 113 || Map == 122 || Map == 135 || Map == 167 || Map == 168 || Map == 171 || Map == 172 || Map == 173 || Map == 180)
		nNpcSettingIdxInfo = MAKELONG(40, pKSNpcInfo->nTemplateID);

	if (Map == 12 || Map == 24 || Map == 38 || Map == 39 || Map == 40 || Map == 42 || Map == 66 || Map == 67 || Map == 116 || Map == 125 || Map == 132 || Map == 147 || Map == 163 || Map == 164 || Map == 165 || Map == 178 || Map == 182 || Map == 194)
		nNpcSettingIdxInfo = MAKELONG(50, pKSNpcInfo->nTemplateID);

	if (Map == 26 || Map == 51 || Map == 52 || Map == 56 || Map == 68 || Map == 79 || Map == 114 || Map == 117 || Map == 118 || Map == 119 || Map == 126 || Map == 127 || Map == 128 || Map == 148 || Map == 158 || Map == 166 || Map == 196)
		nNpcSettingIdxInfo = MAKELONG(60, pKSNpcInfo->nTemplateID);

	if (Map == 9 || Map == 27 || Map == 55 || Map == 69 || Map == 72 || Map == 76 || Map == 94 || Map == 120 || Map == 123 || Map == 129 || Map == 130 || Map == 142 || Map == 149 || Map == 169 || Map == 206 || Map == 319)
		nNpcSettingIdxInfo = MAKELONG(70, pKSNpcInfo->nTemplateID);

	if (Map == 10 || Map == 143 || Map == 150 || Map == 151 || Map == 181 || Map == 198 || Map == 199 || Map == 200 || Map == 201 || Map == 202 || Map == 203 || Map == 204 || Map == 205 || Map == 207 || Map == 224 || Map == 320 || Map == 919 || Map == 920)
		nNpcSettingIdxInfo = MAKELONG(80, pKSNpcInfo->nTemplateID);

	if (Map == 75 || Map == 93 || Map == 124 || Map == 144 || Map == 152 || Map == 225 || Map == 226 || Map == 227 || Map == 321 || Map == 322
		|| Map == 340 || Map == 341 || Map == 342 || Map == 875 || Map == 917 || Map == 918 || Map == 921 || Map == 922 || Map == 923 || Map == 924 || Map == 336)
		nNpcSettingIdxInfo = MAKELONG(90, pKSNpcInfo->nTemplateID);

	//	---------------------Gen Serial cho qu∏i theo config Î MapList.txt------------------------------------	
	pKSNpcInfo->cSeries = KNpcTemplate::GenNpcSeries(SubWorld[nSubWorld].m_NpcSeriesAuto, 
	SubWorld[nSubWorld].m_NpcSeriesMetal, SubWorld[nSubWorld].m_NpcSeriesWood, 
	SubWorld[nSubWorld].m_NpcSeriesWater, SubWorld[nSubWorld].m_NpcSeriesFire, 
	SubWorld[nSubWorld].m_NpcSeriesEarth);
	//	---------------------End Gen Serial cho qu∏i theo config Î MapList.txt------------------------------------	
	int nRet = AddNpcSet2(nNpcSettingIdxInfo, pKSNpcInfo->cSeries, nSubWorld, nMpsX, nMpsY);
	if (nRet)
	{
		Npc[nRet].m_TrapScriptID = 0;
		g_StrCpyLen(Npc[nRet].Name, pKSNpcInfo->szName, sizeof(Npc[nRet].Name));
		Npc[nRet].m_Kind = pKSNpcInfo->shKind;
		Npc[nRet].m_Camp = pKSNpcInfo->cCamp;
		Npc[nRet].m_CurrentCamp = pKSNpcInfo->cCamp;
		Npc[nRet].m_Series = pKSNpcInfo->cSeries;
		Npc[nRet].m_Type = pKSNpcInfo->bSpecialNpc;
		pKSNpcInfo->bSpecialNpc = 1; //boss xanh
		if (pKSNpcInfo->bSpecialNpc)
		{
			Npc[nRet].m_cGold.SetGoldTypeAndBackData();
		}
		else
		{
			Npc[nRet].m_cGold.SetGoldType(FALSE);
		}

		if (pKSNpcInfo->szScript[0])
		{
			if (pKSNpcInfo->szScript[0] == '.')
			{
				g_StrCpyLen(Npc[nRet].ActionScript, &pKSNpcInfo->szScript[1], sizeof(Npc[nRet].ActionScript));
			}
			else
			{
				g_StrCpyLen(Npc[nRet].ActionScript, pKSNpcInfo->szScript, sizeof(Npc[nRet].ActionScript));
			}

			g_StrLower(Npc[nRet].ActionScript);
			Npc[nRet].m_ActionScriptID = g_FileName2Id(Npc[nRet].ActionScript);
		
		}
		else
		{
			Npc[nRet].m_ActionScriptID = 0;
		}

		if (Npc[nRet].m_Kind != kind_player && Npc[nRet].m_SubWorldIndex >= 0 && Npc[nRet].m_RegionIndex >= 0)
		{
			char CreateScript[] = "\\script\\startgame\\npccreate.lua";
			Npc[nRet].ExecuteScript(CreateScript, "OnCreate", nRet);
		}
	}
	return nRet;
}
#endif

int KNpcSet::AddNpcSet2(int nNpcSettingIdxInfo, int nSeries, int nSubWorld, int nMpsX, int nMpsY)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)
		return 0;
	SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;
	return AddNpcSet1(nNpcSettingIdxInfo, nSeries, nSubWorld, nRegion, nMapX, nMapY, nOffX, nOffY);
}

int KNpcSet::AddNpcSet1(int nNpcSettingIdxInfo, int nSeries, int nSubWorld, int nRegion, int nMapX, int nMapY, int nOffX /* = 0 */, int nOffY /* = 0 */)
{
	int i = FindFree();
	if (i == 0)
		return 0;

#ifndef _SERVER
	Npc[i].m_sClientNpcID.m_dwRegionID = 0;
	Npc[i].m_sClientNpcID.m_nNo = -1;
	Npc[i].Remove();
	for (int j = 0;j < 5;j++)
	{
		Npc[i].ClearBlood(j);
	}
#endif

	int nNpcSettingIdx = (short)HIWORD(nNpcSettingIdxInfo);// >> 7; //≥˝”⁄128
	int nLevel = LOWORD(nNpcSettingIdxInfo);// & 0x7f; 

	Npc[i].m_Index = i;
	Npc[i].m_SkillList.m_nNpcIndex = i;
	Npc[i].Load(nNpcSettingIdx, nLevel, nSeries); //Load npc viet them nSeries
	Npc[i].m_SubWorldIndex = nSubWorld;
	Npc[i].m_RegionIndex = nRegion;
	Npc[i].m_nNpcTimeout = 0;
	ZeroMemory(Npc[i].m_nNpcParam, sizeof(Npc[i].m_nNpcParam));
	Npc[i].m_bNpcFollowFindPath = FALSE;
	Npc[i].m_uFindPathTime = 0;
	Npc[i].m_uFindPathMaxTime = 0;
	Npc[i].m_uLastFindPathTime = 0;



#ifdef _SERVER

	if (SubWorld[nSubWorld].szNormalDropRate[0])	//edit by phong kieu load DropRate quai thuong
	{
		g_GameSetting.GetString("NPC", "NormalDropRate", "", Npc[i].DropRateScript, sizeof(Npc[i].DropRateScript));
		g_StrLower(Npc[i].DropRateScript);
		Npc[i].m_DropRateScriptID = g_FileName2Id(Npc[i].DropRateScript);


		//	KNpcTemplate::GenNpcDropRate(&Npc[i], SubWorld[nSubWorld].szNormalDropRate);
	}


#endif

#ifndef _SERVER
	if (nRegion >= 0 && nRegion < 9)
		Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;
#endif

	Npc[i].m_MapX = nMapX;
	Npc[i].m_MapY = nMapY;
	Npc[i].m_OffX = nOffX;
	Npc[i].m_OffY = nOffY;
	SubWorld[nSubWorld].Map2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &Npc[i].m_OriginX, &Npc[i].m_OriginY);

#ifdef _SERVER
	SetID(i);
#endif

	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	SubWorld[nSubWorld].m_Region[nRegion].AddNpc(i);//m_WorldMessage.Send(GWM_NPC_ADD, nRegion, i);
	SubWorld[nSubWorld].m_Region[nRegion].AddRef(nMapX, nMapY, obj_npc);

#ifndef _SERVER
	Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;
#endif

	return i;
}

void KNpcSet::Remove(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_NPC)
		return;

#ifdef _SERVER
	NPC_REMOVE_SYNC	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcremove;
	NetCommand.ID = Npc[nIdx].m_dwID;

	int nSubWorld = Npc[nIdx].m_SubWorldIndex;
	int nRegion = Npc[nIdx].m_RegionIndex;
	
	if (nSubWorld >= 0 && nSubWorld <= MAX_SUBWORLD && nRegion >= 0 && nRegion <= SubWorld[nSubWorld].m_nTotalRegion)
	{	
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
		// [S11 26/08] goi GO mien ngan sach (xem chu thich NPC_EVENT_BROADCAST_LIMIT).
		int nMaxCount = NPC_EVENT_BROADCAST_LIMIT;
		SubWorld[nSubWorld].m_Region[nRegion].BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY);
		
		int nConRegion;
		for (int i = 0; i < 8; i++)
		{
			nConRegion = SubWorld[nSubWorld].m_Region[nRegion].m_nConnectRegion[i];
			if (nConRegion == -1)
				continue;
			SubWorld[nSubWorld].m_Region[nConRegion].BroadCast(&NetCommand, sizeof(NetCommand), nMaxCount, Npc[nIdx].m_MapX - POff[i].x, Npc[nIdx].m_MapY - POff[i].y);
		}	
	}

#endif

	Npc[nIdx].ClearNpcState();
	Npc[nIdx].m_SkillList.Clear();
	Npc[nIdx].Remove();

	m_FreeIdx.Insert(nIdx);
	m_UseIdx.Remove(nIdx);
}

void KNpcSet::RemoveAll()
{
	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		Npc[nIdx].m_SkillList.Clear();
		Npc[nIdx].Remove();
		Npc[nIdx].ClearNpcState();
		m_FreeIdx.Insert(nIdx);
		m_UseIdx.Remove(nIdx);
		nIdx = nIdx1;
	}
}

#ifndef _SERVER
void	KNpcSet::InsertNpcToRegion(int nRegionIdx)
{
	if (nRegionIdx < 0 || nRegionIdx >= MAX_REGION)
		return;
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID > 0 && Npc[nIdx].m_dwRegionID == (DWORD)SubWorld[0].m_Region[nRegionIdx].m_RegionID)
		{
			SubWorld[0].m_Region[nRegionIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegionIdx;
			Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegionIdx].m_RegionID;
			Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
			Npc[nIdx].SendCommand(do_stand);
		}
	}
}
#endif

void KNpcSet::SetID(int m_nIndex)
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	// (r5f - phan bien) bo dem chay don dieu tu 1000 nen SOM MUON gi cung cham
	// DATAU_SAPMAP_ID; npc/sap that mang id do se bi goi xin danh ba cuop mat.
	// Bo qua dung mot gia tri = dong goi han kha nang do.
	if (m_dwIDCreator == DATAU_SAPMAP_ID || m_dwIDCreator == TK_DICH_ID)	// (04/09) + id xin vi tri dich Tong Kim
		m_dwIDCreator++;
	Npc[m_nIndex].m_dwID = m_dwIDCreator;
	m_dwIDCreator++;
}

int KNpcSet::GetDistance(int nIdx1, int nIdx2)
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		int XOff = (Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellWidth;
		XOff += (Npc[nIdx1].m_OffX - Npc[nIdx2].m_OffX) >> 10;

		int YOff = (Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellHeight;
		YOff += (Npc[nIdx1].m_OffY - Npc[nIdx2].m_OffY) >> 10;

		nRet = (int)sqrt((float)(XOff * XOff + YOff * YOff));//mÌi th™m vµo float
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].Map2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].m_MapX,
			Npc[nIdx1].m_MapY,
			Npc[nIdx1].m_OffX,
			Npc[nIdx1].m_OffY,
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].Map2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].m_MapX,
			Npc[nIdx2].m_MapY,
			Npc[nIdx2].m_OffX,
			Npc[nIdx2].m_OffY,
			&X2,
			&Y2);

		nRet = (int)sqrt((float)((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1)));//mÌi th™m vµo float
	}
	return nRet;
}

int		KNpcSet::GetDistanceSquare(int nIdx1, int nIdx2)
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		int XOff = (Npc[nIdx1].m_MapX - Npc[nIdx2].m_MapX) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellWidth;
		XOff += (Npc[nIdx1].m_OffX - Npc[nIdx2].m_OffX) >> 10;

		int YOff = (Npc[nIdx1].m_MapY - Npc[nIdx2].m_MapY) * SubWorld[Npc[nIdx1].m_SubWorldIndex].m_nCellHeight;
		YOff += (Npc[nIdx1].m_OffY - Npc[nIdx2].m_OffY) >> 10;

		nRet = (int)(XOff * XOff + YOff * YOff);
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].Map2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].m_MapX,
			Npc[nIdx1].m_MapY,
			Npc[nIdx1].m_OffX,
			Npc[nIdx1].m_OffY,
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].Map2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].m_MapX,
			Npc[nIdx2].m_MapY,
			Npc[nIdx2].m_OffX,
			Npc[nIdx2].m_OffY,
			&X2,
			&Y2);

		nRet = (int)((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
	}
	return nRet;
}

int		KNpcSet::GetNextIdx(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_NPC)
		return 0;
	return m_UseIdx.GetNext(nIdx);
}

#ifdef _SERVER
BOOL KNpcSet::SyncNpc(DWORD dwID, int nClientIdx)
{
	int		nFindIndex;

	nFindIndex = SearchID(dwID);
	if (nFindIndex <= 0)
	{
		NPC_REQUEST_FAIL	RequestFail;

		RequestFail.ProtocolType = s2c_requestnpcfail;
		RequestFail.ID = dwID;
		
		if (g_pServer)
			g_pServer->PackDataToClient(nClientIdx, &RequestFail, sizeof(NPC_REQUEST_FAIL));
		return FALSE;
	}
	Npc[nFindIndex].SendSyncData(nClientIdx);
	return TRUE;
}
#endif

#ifndef _SERVER
void KNpcSet::CheckBalance()
{
	int nIdx;
	nIdx = m_UseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_UseIdx.GetNext(nIdx);
		// [S6-GANNHANH 28/08] (chu game: "BOT/NPC/nguoi choi hay nhay toa do, bien mat
		// roi hien lai") Do 20:15-21:02: 90 cu bien-mat 2-60s (p50 8,4s) = mo coi do
		// RECENTER roi cho vong quay sync server qua lau. Con mo coi ma SyncSignal con
		// TUOI (<=36 khung ~2s: server VUA sync = con song, vi tri chuan - tu loai ma cu)
		// va region cu DA NAP LAI trong cua so 3x3 -> gan lai NGAY, khoi cho 8s.
		// Gac: bo chet/hoi sinh (ke toan ref co gac death), partner (he rieng), chi gan
		// khi con trong 38 o (ngoai hon la de VANH 42 go lai = flap).
		if (Npc[nIdx].m_RegionIndex < 0
		 && nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex
		 && Npc[nIdx].m_Kind != kind_partner
		 && Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive
		 && SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal <= 36)
		{
			const int nSelf6 = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			if (nSelf6 > 0 && nSelf6 < MAX_NPC && Npc[nSelf6].m_RegionIndex >= 0)
			{
				const int nGx6 = (int)LOWORD(Npc[nIdx].m_dwRegionID) - (int)LOWORD(Npc[nSelf6].m_dwRegionID);
				const int nGy6 = (int)HIWORD(Npc[nIdx].m_dwRegionID) - (int)HIWORD(Npc[nSelf6].m_dwRegionID);
				const int nRw6 = SubWorld[0].m_nRegionWidth;
				const int nRh6 = SubWorld[0].m_nRegionHeight;
				const int nDx6 = nGx6 * nRw6 + Npc[nIdx].m_MapX - Npc[nSelf6].m_MapX;
				const int nDy6 = nGy6 * nRh6 + Npc[nIdx].m_MapY - Npc[nSelf6].m_MapY;
				const int nAx6 = (nDx6 < 0) ? -nDx6 : nDx6;
				const int nAy6 = (nDy6 < 0) ? -nDy6 : nDy6;
				if (nGx6 >= -1 && nGx6 <= 1 && nGy6 >= -1 && nGy6 <= 1
				 && nAx6 <= 38 && nAy6 <= 38)
				{
					for (int r6 = 0; r6 < MAX_REGION; r6++)
					{
						if (SubWorld[0].m_Region[r6].m_RegionID != Npc[nIdx].m_dwRegionID)
							continue;
						Npc[nIdx].m_RegionIndex = r6;
						SubWorld[0].m_Region[r6].AddNpc(nIdx);
						SubWorld[0].m_Region[r6].AddRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
						AUTOLOG("[S6-GANNHANH] npc=%u idx=%d kind=%u cell=(%d,%d) reg=%d t=%u", Npc[nIdx].m_dwID, nIdx, Npc[nIdx].m_Kind, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, r6, SubWorld[0].m_dwCurrentTime);
						break;
					}
				}
			}
		}
		if (SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal > 1000)
		{
			if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				if (Npc[nIdx].m_RegionIndex >= 0)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
                    if (Npc[nIdx].m_Doing != do_death && Npc[nIdx].m_Doing != do_revive)
    					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				}
				// [S6 26/08] Duong don KHE duy nhat theo timeout ~55 s. Neu doing=10/21 ma
				// reg >= 0 thi nhanh phia tren vua RemoveNpc KHONG DecRef -> nghi RO REF o
				// (bang dem NPC/o la BYTE, tran 255 = ket o vinh vien). Dem dong nay de biet.
				AUTOLOG("[S6-BAL] npc=%u idx=%d kind=%u doing=%d reg=%d camtick=%u t=%u", Npc[nIdx].m_dwID, nIdx, Npc[nIdx].m_Kind, (int)Npc[nIdx].m_Doing, Npc[nIdx].m_RegionIndex, SubWorld[0].m_dwCurrentTime - Npc[nIdx].m_SyncSignal, SubWorld[0].m_dwCurrentTime);
				Remove(nIdx);
			}
		}

		/*else if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex && Player[CLIENT_PLAYER_INDEX].m_nIndex > 0 && Player[CLIENT_PLAYER_INDEX].m_nIndex < MAX_NPC)
		{
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SyncSignal - Npc[nIdx].m_SyncSignal > 5*18)//Fix lag toa do
			{
				if (Npc[nIdx].m_RegionIndex >= 0)
				{
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
				}
				Remove(nIdx);
			}
		}*/

		nIdx = nTmpIdx;
	}
	nIdx = m_RequestUseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_RequestUseIdx.GetNext(nIdx);
		if (SubWorld[0].m_dwCurrentTime - m_RequestNpc[nIdx].dwRequestTime > 100)
		{
			DWORD	dwID = m_RequestNpc[nIdx].dwRequestId;
			m_RequestNpc[nIdx].dwRequestId = 0;	
			m_RequestNpc[nIdx].dwRequestTime = 0;
			
			m_RequestUseIdx.Remove(nIdx);
			m_RequestFreeIdx.Insert(nIdx);
		//	g_DebugLog("[Request]Remove %d from %d on %d timeout", dwID, nIdx, SubWorld[0].m_dwCurrentTime);
		}
		nIdx = nTmpIdx;
	}
}
#endif

#ifndef _SERVER
int		KNpcSet::GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount)
{
	int nCamp = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp;
	int nNum = 0, i;

	if (nCount == 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			// Bot khong bao gio nhan duoc loi moi to doi: server dinh tuyen qua
			// KPlayer::FindAroundPlayer -> KRegion::FindPlayer, chi quet m_PlayerList, ma bot
			// chi nam trong m_NpcList. Moi bot = spam thuan tuy.
			if (Npc[nIdx].m_btSimCityBot)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			for (i = 0; i < MAX_TEAM_MEMBER; i++)
			{
				if ((DWORD)g_Team[0].m_nMember[i] == Npc[nIdx].m_dwID)
					break;
			}
			if (i < MAX_TEAM_MEMBER)
				continue;
			if ((DWORD)g_Team[0].m_nCaptain == Npc[nIdx].m_dwID)
				continue;
			nNum++;
		}
		
		return nNum;
	}

	if (!pList)
		return 0;

	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		for (i = 0; i < MAX_TEAM_MEMBER; i++)
		{
			if ((DWORD)g_Team[0].m_nMember[i] == Npc[nIdx].m_dwID)
				break;
		}
		if (i < MAX_TEAM_MEMBER)
			continue;
		if ((DWORD)g_Team[0].m_nCaptain == Npc[nIdx].m_dwID)
			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		strcpy(pList[nNum].Name, Npc[nIdx].Name);
		nNum++;
	}
	
	return nNum;
}
#endif

#ifndef _SERVER
int KNpcSet::GetAroundNpcMonster(KUiPlayerItem *pList, int nCount, int nRange)//Add by phong ki“u using fkauto
{
	int nNum = 0;
	//
	if (!pList)
		return 0;
	//
	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (!Npc[nIdx].IsAlive())
			continue;
		//
		int nIndexPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		int nX1 = Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt;
		int nY1 = Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt;
		int nX2 = 0;
		int nY2 = 0;
		Npc[nIdx].GetMpsPos(&nX2, &nY2);
		int nRangeNpc = GetDistanceMps(nX1, nY1, nX2, nY2);
		if (nRangeNpc > nRange)
			continue;
		//
		if (NpcSet.GetRelation(nIdx, nIndexPlayer) & relation_enemy) 
		{
			if(Npc[nIdx].m_Kind == kind_normal)
			{
				pList[nNum].nIndex = nIdx;
				pList[nNum].uId = Npc[nIdx].m_dwID;
				strcpy(pList[nNum].Name, Npc[nIdx].Name);
				pList[nNum].nData = Npc[nIdx].GetMenuState();
				nNum++;
			}
		}
	}
	
	return nNum;
}
int KNpcSet::GetAroundNpcBossMonster(KUiPlayerItem* pList, int nCount, int nRange)//Add by phong ki“u using fkauto
{
	int nNum = 0;
	//
	if (!pList)
		return 0;
	//
	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (!Npc[nIdx].IsAlive())
			continue;
		//
		int nIndexPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		int nX1 = Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt;
		int nY1 = Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt;
		int nX2 = 0;
		int nY2 = 0;
		Npc[nIdx].GetMpsPos(&nX2, &nY2);
		int nRangeNpc = GetDistanceMps(nX1, nY1, nX2, nY2);
		if (nRangeNpc > nRange)
			continue;
		//
		if (NpcSet.GetRelation(nIdx, nIndexPlayer) & relation_enemy)
		{
			if (Npc[nIdx].m_Type == boss_blue || Npc[nIdx].m_Type == boss_gold)
			{
				pList[nNum].nIndex = nIdx;
				pList[nNum].uId = Npc[nIdx].m_dwID;
				strcpy(pList[nNum].Name, Npc[nIdx].Name);
				pList[nNum].nData = Npc[nIdx].GetMenuState();
				nNum++;
			}
		}
	}

	return nNum;
}
int KNpcSet::GetAroundPlayerMonster(KUiPlayerItem* pList, int nCount, int nRange)//Add by phong ki“u using fkauto
{
	int nNum = 0;
	//
	if (!pList)
		return 0;
	//
	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (!Npc[nIdx].IsAlive())
			continue;
		//
		int nIndexPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		int nX1 = Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt;
		int nY1 = Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt;
		int nX2 = 0;
		int nY2 = 0;
		Npc[nIdx].GetMpsPos(&nX2, &nY2);
		int nRangeNpc = GetDistanceMps(nX1, nY1, nX2, nY2);
		if (nRangeNpc > nRange)
			continue;
		//
		if (NpcSet.GetRelation(nIdx, nIndexPlayer) & relation_enemy)
		{
			if (Npc[nIdx].m_Kind == kind_player)
			{
				pList[nNum].nIndex = nIdx;
				pList[nNum].uId = Npc[nIdx].m_dwID;
				strcpy(pList[nNum].Name, Npc[nIdx].Name);
				pList[nNum].nData = Npc[nIdx].GetMenuState();
				nNum++;
			}
		}
	}

	return nNum;
}
int	KNpcSet::GetAroundPlayer(KUiPlayerItem *pList, int nCount)
{
	int nNum = 0;

	if (nCount <= 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player ||
				nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
				Npc[nIdx].m_RegionIndex < 0)
			{
				continue;
			}
//			if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//				continue;
			nNum++;
		}
		
		return nNum;
	}

	if (!pList)
		return 0;

	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		// Loc bot SimCity: vong nay cat cung o nCount (16 khi KPlayerAuto.cpp:3050 goi) va
		// duyet THEO THU TU CHI SO, nen bot chi so thap chiem het khe -> dong doi THAT bi
		// tinh la "vang mat" roi bi TeamKickMember (KPlayerAuto.cpp:3073) duoi khoi to doi.
		// Cung danh sach nay quyet dinh muc tieu theo sau (KPlayerAuto.cpp:1023).
		if (Npc[nIdx].m_Kind != kind_player ||
			Npc[nIdx].m_btSimCityBot ||
			nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
			Npc[nIdx].m_RegionIndex < 0)
		{
			continue;
		}
//		if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		strcpy(pList[nNum].Name, Npc[nIdx].Name);
		pList[nNum].nData = Npc[nIdx].GetMenuState();
		nNum++;
	}
	
	return nNum;
}
#endif

#ifndef _SERVER
void	KNpcSet::SetShowNameFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_NAME;
	else
		m_nShowPateFlag &= ~PATE_NAME;
}
#endif

#ifndef _SERVER
BOOL	KNpcSet::CheckShowName()
{
	return m_nShowPateFlag & PATE_NAME;
}
#endif

#ifndef _SERVER
void	KNpcSet::SetShowLifeFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_LIFE;
	else
		m_nShowPateFlag &= ~PATE_LIFE;
}
#endif

#ifndef _SERVER
BOOL	KNpcSet::CheckShowLife()
{
	return m_nShowPateFlag & PATE_LIFE;
}
#endif

#ifndef _SERVER
void	KNpcSet::SetShowChatFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_CHAT;
	else
		m_nShowPateFlag &= ~PATE_CHAT;
}
#endif

#ifndef _SERVER
BOOL	KNpcSet::CheckShowChat()
{
	return m_nShowPateFlag & PATE_CHAT;
}
#endif

#ifndef _SERVER
void	KNpcSet::SetShowManaFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_MANA;
	else
		m_nShowPateFlag &= ~PATE_MANA;
}
#endif

#ifndef _SERVER
int KNpcSet::AutoGetNpcNear(int nX1, int nY1, int nRelation, int nRange,
	int ArrayLag[], int nSize,
	BOOL m_bFightCheckBox, BOOL m_bFightSelfDef)
{
	int nIndexPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	// Track best candidates
	int bestPlayerIdx = 0;
	int bestPlayerDist = nRange + 1;   // anything beyond nRange is invalid

	int bestNormalIdx = 0;
	int bestNormalDist = nRange + 1;

	// Walk through all valid NPC indices
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)) != 0)
	{
		auto& npc = Npc[nIdx];

		// Skip out-of-region, dead, or lag-list
		if (npc.m_RegionIndex < 0 || !npc.IsAlive())
			continue;
		bool bLag = false;
		for (int i = 0; i < nSize; i++)
			if (ArrayLag[i] == nIdx) { bLag = true; break; }
		if (bLag)
			continue;

		// Check map-distance filter
		int nX2, nY2;
		npc.GetMpsPos(&nX2, &nY2);
		int distMap = GetDistanceMps(nX1, nY1, nX2, nY2);
		if (distMap > nRange)
			continue;

		// Relation check
		if ((NpcSet.GetRelation(nIdx, nIndexPlayer) & nRelation) == 0)
			continue;

		// Compute real ìgameî distance from player
		int distPlayer = GetDistance(nIdx, nIndexPlayer);

		// 1) If we're allowed to fight other players, track the *closest* one
		if (m_bFightSelfDef && npc.m_Kind == kind_player)
		{
			if (distPlayer < bestPlayerDist)
			{
				bestPlayerDist = distPlayer;
				bestPlayerIdx = nIdx;
			}
			// even if we found one ìclose,î we keep looking for someone even closer
			continue;
		}

		// 2) Otherwise (or as a fallback), if check-box is on, track the nearest normal NPC
		if (m_bFightCheckBox && npc.m_Kind == kind_normal)
		{
			if (distPlayer < bestNormalDist)
			{
				bestNormalDist = distPlayer;
				bestNormalIdx = nIdx;
			}
		}
	}

	// Prefer returning a player if we found any; else return the best normal; else 0
	if (bestPlayerIdx != 0)
		return bestPlayerIdx;
	if (bestNormalIdx != 0)
		return bestNormalIdx;
	return 0;
}
#endif

#ifndef _SERVER
BOOL	KNpcSet::CheckShowMana()
{
	return m_nShowPateFlag & PATE_MANA;
}
#endif

int KNpcSet::GetDistanceMps(int nRx1, int nRy1, int nRx2, int nRy2)
{
	return (int)sqrt((float)((nRx1 - nRx2) * (nRx1 - nRx2) + (nRy1 - nRy2) * (nRy1 - nRy2)));
}

void	KNpcSet::ClearActivateFlagOfAllNpc()
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		Npc[nIdx].m_bActivateFlag = FALSE;
	}
}

#ifndef _SERVER
void	KNpcSet::GetAroundOpenCaptain(int nCamp)
{
	int		nIdx, nNum, nNo;

	nIdx = 0;
	nNum = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
			continue;
//		if (Npc[nIdx].m_Camp != nCamp)
//			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (Npc[nIdx].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			nNum++;
	}
	
	if (nNum > 0)
	{
		KUiTeamItem* const pTeamList = new KUiTeamItem[nNum];
		nIdx = 0;
		nNo = 0;
		while (1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
				continue;
//			if (Npc[nIdx].m_Camp != nCamp)
//				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			if (Npc[nIdx].GetMenuState() == PLAYER_MENU_STATE_TEAMOPEN)
			{
				pTeamList[nNo].Leader.nIndex = nIdx;
				pTeamList[nNo].Leader.uId = Npc[nIdx].m_dwID;
				strcpy(pTeamList[nNo].Leader.Name, Npc[nIdx].Name);
				nNo++;
				if (nNo >= nNum)
					break;
			}
		}
		CoreDataChanged(GDCNI_TEAM_NEARBY_LIST, (unsigned int)pTeamList, nNo);
		delete []pTeamList;
	}
}
#endif

#ifndef _SERVER	
int	KNpcSet::SearchNpcAt(int nX, int nY, int nRelation, int nRange)
{
	int nIdx;
	int	nMin = nRange;
	int nMinIdx = 0;
	int	nLength = 0;
	int nSrcX[2];
	int	nSrcY[2];

	nSrcX[0] = nX;
	nSrcY[0] = nY;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[0], nSrcY[0], 0);

	nSrcX[1] = nX;
	nSrcY[1] = nY;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[1], nSrcY[1], 120);

	int nDx = nSrcX[0] - nSrcX[1];
	int nDy = nSrcY[0] - nSrcY[1];

	nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);

		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_RegionIndex < 0)
			continue;

		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;

		if (Npc[nIdx].m_bClientOnly)
			continue;

		if (!(GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIdx) & nRelation))
			continue;
		
		int x, y;
		SubWorld[0].Map2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY,
			Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &x, &y);

		if (nSrcY[0] > y)
			continue;

		if (nSrcY[0] < y - 120)
			continue;

		nLength = abs(nDx * (nSrcY[0] - y) / nDy + nSrcX[0] - x);
		if (nLength < nMin)
		{
			nMin = nLength;
			nMinIdx = nIdx;
		}
	}

	return nMinIdx;
}
#endif

#ifndef _SERVER
BOOL KNpcSet::IsNpcRequestExist(DWORD dwID)
{
	return (GetRequestIndex(dwID) > 0);
}

BOOL KNpcSet::InsertNpcRequest(DWORD dwID)
{
	if (IsNpcRequestExist(dwID))
	{
		return FALSE;
	}

	int nIndex = m_RequestFreeIdx.GetNext(0);
	if (!nIndex)
		return FALSE;

	m_RequestNpc[nIndex].dwRequestId = dwID;
	m_RequestNpc[nIndex].dwRequestTime = SubWorld[0].m_dwCurrentTime;
	m_RequestFreeIdx.Remove(nIndex);
	m_RequestUseIdx.Insert(nIndex);
	//g_DebugLog("[Request]Insert %d at %d on %d", dwID, nIndex, SubWorld[0].m_dwCurrentTime);

	return TRUE;
}

void KNpcSet::RemoveNpcRequest(DWORD dwID)
{
	if(!IsNpcRequestExist(dwID))
	{
		return;
	}

	int nIndex = GetRequestIndex(dwID);

	// because _ASSERT(IsNpcRequestExist()); so nIndex > 0;
	m_RequestNpc[nIndex].dwRequestId = 0;	
	m_RequestNpc[nIndex].dwRequestTime = 0;

	m_RequestUseIdx.Remove(nIndex);
	m_RequestFreeIdx.Insert(nIndex);
	//g_DebugLog("[Request]Remove %d from %d on %d", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

int KNpcSet::GetRequestIndex(DWORD dwID)
{
	int nIndex = m_RequestUseIdx.GetNext(0);

	while(nIndex)
	{
		if (m_RequestNpc[nIndex].dwRequestId == dwID)
		{
			return nIndex;
		}
		nIndex = m_RequestUseIdx.GetNext(nIndex);
	}
	return 0;
}
#endif

NPC_RELATION KNpcSet::GetRelation(int nId1, int nId2)
{
	if (nId1 == nId2)
		return relation_self;

#ifndef _SERVER
	if (Npc[nId1].m_bClientOnly || Npc[nId2].m_bClientOnly)
		return relation_none;
#endif

    _ASSERT(
        ((Npc[nId1].m_Kind >= 0) && (Npc[nId1].m_Kind < kind_num)) &&
        ((Npc[nId2].m_Kind >= 0) && (Npc[nId2].m_Kind < kind_num)) &&
        ((Npc[nId1].m_CurrentCamp >= 0) && (Npc[nId1].m_CurrentCamp < camp_num)) &&
        ((Npc[nId2].m_CurrentCamp >= 0) && (Npc[nId2].m_CurrentCamp < camp_num))
    );

#ifndef _SERVER
	if (Player[CLIENT_PLAYER_INDEX].m_nIndex != nId1 && Player[CLIENT_PLAYER_INDEX].m_nIndex != nId2)
	{	
		if (!Npc[nId1].IsAlive() || !Npc[nId2].IsAlive())
			return relation_none;
	//	if (Npc[nId1].m_Kind == kind_player && Npc[nId2].m_Kind == kind_player && Npc[nId1].m_nPKFlag == enumPKMurder)
		//	return relation_none;

		NPC_RELATION nRelation =  (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
			[Npc[nId2].m_Kind]
			[Npc[nId1].m_CurrentCamp]
			[Npc[nId2].m_CurrentCamp]
			[Npc[nId1].m_FightMode]
			[Npc[nId2].m_FightMode];

		if (nRelation == relation_enemy)
		{
			if (Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_player && 
				(!Npc[nId1].m_nPKFlag || 
				!Npc[nId2].m_nPKFlag || 
				Npc[nId1].m_FightMode == fight_none || 
				Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
			if ((Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_normal && Npc[nId1].m_FightMode == fight_none) || 
				(Npc[nId1].m_Kind == kind_normal && 
				Npc[nId2].m_Kind == kind_player && Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
		}
		return nRelation;	
	}
	else if (Player[CLIENT_PLAYER_INDEX].m_nIndex == nId1)
	{
		if (/*!Npc[nId1].IsAlive() || */!Npc[nId2].IsAlive())
			return relation_none;

		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetExercisePKAim() == Npc[nId2].m_dwID)
			return relation_enemy;

		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING && 
			Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKAimNpcID() == Npc[nId2].m_dwID
			)
			return relation_enemy;

		if (Npc[nId2].m_Kind == kind_player  && Npc[nId2].m_nPKFlag == enumPKMurder &&
				Npc[nId1].m_FightMode && Npc[nId2].m_FightMode && 
				Npc[nId2].m_CurrentCamp != camp_begin && 
				Npc[nId1].m_CurrentCamp != camp_begin &&
				Npc[nId2].m_CurrentCamp != Npc[nId1].m_CurrentCamp)
				return relation_enemy;
		if ((Npc[nId2].m_Kind == kind_player && Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState() == enumPKMurder) || 
				(Npc[nId2].m_Kind == kind_player && 
				Npc[nId2].m_nPKFlag == enumPKMurder && 
				Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState())
			)
		{
			if(Npc[nId1].m_FightMode && Npc[nId2].m_FightMode && 
				Npc[nId2].m_CurrentCamp != camp_begin && 
				Npc[nId1].m_CurrentCamp != camp_begin &&
				Npc[nId2].m_CurrentCamp != Npc[nId1].m_CurrentCamp
				)
			{
				return relation_enemy;
			}
		}
		
		//#chua hoan thien
		/*if (Npc[nId1].m_nTeamServerID >= 0 && 
			Npc[nId2].m_nTeamServerID >= 0 && 
			Npc[nId1].m_nTeamServerID == Npc[nId2].m_nTeamServerID
			)
			return relation_ally;

		if (Npc[nId2].m_Kind == kind_player && 
			Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState() == enumPKTongWar &&  
			Npc[nId2].m_nPKFlag == enumPKTongWar)
		{
			if (Npc[nId1].m_FightMode && Npc[nId2].m_FightMode)
			{
				if (Npc[nId1].m_dwTongNameID == Npc[nId2].m_dwTongNameID)
				{
					if (Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp)
						return relation_none;
					else
						return relation_ally;
				}
				else
					return relation_enemy;

			}
			else
			{
				if (Npc[nId1].m_dwTongNameID == Npc[nId2].m_dwTongNameID)
					return relation_ally;
				else
					return relation_none;
			}
		}*/

		NPC_RELATION nRelation =  (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
			[Npc[nId2].m_Kind]
			[Npc[nId1].m_CurrentCamp]
			[Npc[nId2].m_CurrentCamp]
			[Npc[nId1].m_FightMode]
			[Npc[nId2].m_FightMode];
		
		if (nRelation == relation_enemy)
		{
			if (Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_player && 
				(!Npc[nId1].m_nPKFlag || 
				!Npc[nId2].m_nPKFlag || 
				Npc[nId1].m_FightMode == fight_none || 
				Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
			if ((Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_normal && Npc[nId1].m_FightMode == fight_none) || 
				(Npc[nId1].m_Kind == kind_normal && 
				Npc[nId2].m_Kind == kind_player && Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
		}
		/*else if(nRelation == relation_ally)
		{
			if (Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_player && 
				(Npc[nId1].m_FightMode != Npc[nId2].m_FightMode)
				)
				return relation_none;
		}*/
		return nRelation;
	}
	else
	{
		if (!Npc[nId1].IsAlive()/* || !Npc[nId2].IsAlive()*/)
			return relation_none;

		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetExercisePKAim() == Npc[nId1].m_dwID)
			return relation_enemy;

		if (Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING && 
			Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKAimNpcID() == Npc[nId1].m_dwID)
			return relation_enemy;
		if (Npc[nId1].m_Kind == kind_player  && Npc[nId1].m_nPKFlag == enumPKMurder &&
				Npc[nId1].m_FightMode && Npc[nId2].m_FightMode && 
				Npc[nId2].m_CurrentCamp != camp_begin && 
				Npc[nId1].m_CurrentCamp != camp_begin &&
				Npc[nId2].m_CurrentCamp != Npc[nId1].m_CurrentCamp)
				return relation_enemy;
		if ((Npc[nId1].m_Kind == kind_player && Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState() == enumPKMurder) || 
				(Npc[nId1].m_Kind == kind_player && 
				Npc[nId1].m_nPKFlag == enumPKMurder && 
				!Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState())
			)
		{
			if(Npc[nId1].m_FightMode && Npc[nId2].m_FightMode && 
				(Npc[nId1].m_CurrentCamp != camp_begin && 
				Npc[nId2].m_CurrentCamp != camp_begin && 
				Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp)
				)
			{
				return relation_enemy;
			}
		}
		/*//#chua hoan thien
		if (Npc[nId1].m_nTeamServerID >= 0 && 
			Npc[nId2].m_nTeamServerID >= 0 && 
			Npc[nId1].m_nTeamServerID == Npc[nId2].m_nTeamServerID
			)
			return relation_ally;

		if (Npc[nId1].m_Kind == kind_player && 
			Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState() == enumPKTongWar &&  
			Npc[nId1].m_nPKFlag == enumPKTongWar
			)
		{
			if (Npc[nId1].m_FightMode && Npc[nId2].m_FightMode)
			{
				if (Npc[nId2].m_dwTongNameID == Npc[nId1].m_dwTongNameID)
				{
					if (Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp)
						return relation_none;
					else
						return relation_ally;
				}
				else
					return relation_enemy;
			}
			else
			{
				if (Npc[nId2].m_dwTongNameID == Npc[nId1].m_dwTongNameID)
					return relation_ally;
				else
					return relation_none;
			}
		}*/


		NPC_RELATION nRelation =  (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
			[Npc[nId2].m_Kind]
			[Npc[nId1].m_CurrentCamp]
			[Npc[nId2].m_CurrentCamp]
			[Npc[nId1].m_FightMode]
			[Npc[nId2].m_FightMode];			

		if (nRelation == relation_enemy)
		{
			if (Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_player && 
				(!Npc[nId1].m_nPKFlag || 
				!Npc[nId2].m_nPKFlag || 
				Npc[nId1].m_FightMode == fight_none || 
				Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
			if ((Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_normal && Npc[nId1].m_FightMode == fight_none) || 
				(Npc[nId1].m_Kind == kind_normal && 
				Npc[nId2].m_Kind == kind_player && Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
		}
		/*else if(nRelation == relation_ally)
		{
			if (Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_player && 
				(Npc[nId1].m_FightMode != Npc[nId2].m_FightMode)
				)
				return relation_none;
		}*/
		return nRelation;
	}
#endif

#ifdef _SERVER
	// Port SimCity: bot gia lap KHONG bao gio la ke dich cua ai.
	// LY DO PHAI CHAN O DAY (doc tu ma - day la loi dang song, khong phai phong xa):
	//   Nhanh else ben duoi mo dau bang
	//     if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nId2].m_nPlayerIdx)
	//         return relation_enemy;
	//   Bot co m_nPlayerIdx = 0 (KNpc.cpp:139) va nguoi choi that khong dau vo dai thi
	//   GetExercisePKAim() tra 0 (KPlayerPK.cpp:43) => 0 == 0 => relation_enemy, TRUOC moi
	//   kiem tra camp / co PK / FightMode. Tuc la MOI nguoi choi that la ke dich cua MOI bot
	//   ngay tu khung hinh dau tien.
	//   Hau qua da kiem: KPlayer::DialogNpc (KPlayer.cpp:8033) doi relation_none nen moi duong
	//   hoi thoai voi bot bi chan; KMissle.cpp:638/670 FindNpc theo m_eRelation nen dan/am khi
	//   DUNG LAI o bot thay vi bay toi quai.
	//   6 cong m_btSimCityBot san co trong KNpc.cpp KHONG cai nao chan duoc cho nay.
	if (Npc[nId1].m_btSimCityBot || Npc[nId2].m_btSimCityBot)
	{
		return relation_none;
	}

	// [PORT5 23/08] trai tam thoi JX2 (Linux GetRelation 0x0809EF72-0x0809EFB3): ca hai co
	// TmpCamp != 0 va (khong phai nguoi choi HOAC dang bat chien dau) -> DE LEN moi luat
	// phe/PK: khac trai = dich, cung trai = dong minh (bairenleitai / tongcastle).
	if (Npc[nId1].m_nTmpCamp && Npc[nId2].m_nTmpCamp &&
		Npc[nId1].m_Kind != kind_dialoger && Npc[nId2].m_Kind != kind_dialoger &&
		(Npc[nId1].m_Kind != kind_player || Npc[nId1].m_FightMode) &&
		(Npc[nId2].m_Kind != kind_player || Npc[nId2].m_FightMode))
	{
		return (Npc[nId1].m_nTmpCamp != Npc[nId2].m_nTmpCamp) ? relation_enemy : relation_ally;
	}

	if (Npc[nId1].m_Kind != kind_player || Npc[nId2].m_Kind != kind_player)
	{
		NPC_RELATION result = (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
			[Npc[nId2].m_Kind]
			[Npc[nId1].m_CurrentCamp]
			[Npc[nId2].m_CurrentCamp]
			[Npc[nId1].m_FightMode]
			[Npc[nId2].m_FightMode];

		return result;
	}
	else
	{
		//if (!Npc[nId1].IsAlive() || !Npc[nId2].IsAlive())
		//	return relation_none;

		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetExercisePKAim() == Npc[nId2].m_nPlayerIdx)
			return relation_enemy;

		if (Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING && 
			Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetEnmityPKAim() == Npc[nId2].m_nPlayerIdx
			)
			return relation_enemy;
		if ((Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder || Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder) &&
				Npc[nId1].m_FightMode && Npc[nId2].m_FightMode && 
				Npc[nId2].m_CurrentCamp != camp_begin && 
				Npc[nId1].m_CurrentCamp != camp_begin &&
				Npc[nId2].m_CurrentCamp != Npc[nId1].m_CurrentCamp)
				return relation_enemy;
		if ((Npc[nId2].m_Kind == kind_player && Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder) ||
				(Npc[nId2].m_Kind == kind_player && 
				Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKMurder && 
				!Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState())
			)
		{
			if (Npc[nId1].m_FightMode && Npc[nId2].m_FightMode && 
				Npc[nId1].m_CurrentCamp != camp_begin && 
				Npc[nId2].m_CurrentCamp != camp_begin && 
				Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp
				)
			{
				return relation_enemy;
			}
		}

		if (Player[Npc[nId1].m_nPlayerIdx].m_cTeam.m_nID >= 0 && 
			Player[Npc[nId2].m_nPlayerIdx].m_cTeam.m_nID >= 0 && 
			Player[Npc[nId1].m_nPlayerIdx].m_cTeam.m_nID == Player[Npc[nId2].m_nPlayerIdx].m_cTeam.m_nID
			)
			return relation_ally;
	
		if (Npc[nId2].m_Kind == kind_player && 
			Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() == enumPKTongWar &&
			Npc[nId2].m_nPKFlag == enumPKTongWar
			)
		{
			if (Npc[nId1].m_FightMode && Npc[nId2].m_FightMode)
			{
				if (Player[nId1].m_cTong.m_dwTongNameID == Player[nId2].m_cTong.m_dwTongNameID)
				{
					if (Npc[nId1].m_CurrentCamp != Npc[nId2].m_CurrentCamp)
						return relation_none;
					else
						return relation_ally;
				}
				else
					return relation_enemy;
			}
			else
			{
				if (Player[nId1].m_cTong.m_dwTongNameID == Player[nId2].m_cTong.m_dwTongNameID)
					return relation_ally;
				else
					return relation_none;
			}
		}

		NPC_RELATION nRelation =  (NPC_RELATION)m_RelationTable
			[Npc[nId1].m_Kind]
			[Npc[nId2].m_Kind]
			[Npc[nId1].m_CurrentCamp]
			[Npc[nId2].m_CurrentCamp]
			[Npc[nId1].m_FightMode]
			[Npc[nId2].m_FightMode];
			
		if (nRelation == relation_enemy)
		{
			if (!Player[Npc[nId1].m_nPlayerIdx].m_cPK.GetNormalPKState() || 
				!Player[Npc[nId2].m_nPlayerIdx].m_cPK.GetNormalPKState() || 
				Npc[nId1].m_FightMode == fight_none || 
				Npc[nId2].m_FightMode == fight_none
				)
				return relation_none;
			if ((Npc[nId1].m_Kind == kind_player && 
				Npc[nId2].m_Kind == kind_normal && Npc[nId1].m_FightMode == fight_none) || 
				(Npc[nId1].m_Kind == kind_normal && 
				Npc[nId2].m_Kind == kind_player && Npc[nId2].m_FightMode == fight_none)
				)
				return relation_none;
		}
		/*else if(nRelation == relation_ally)
		{
			if (Npc[nId1].m_FightMode != Npc[nId2].m_FightMode)
				return relation_none;
		}*/
		return nRelation;
	}
#endif
}

//------------------------ class KInstantSpecial start -------------------------
#ifndef _SERVER
KInstantSpecial::KInstantSpecial()
{
	int		i;
	this->m_nLoadFlag = FALSE;
	for (i = 0; i < MAX_INSTANT_STATE; i++)
		this->m_szSprName[i][0] = 0;
	for (i = 0; i < MAX_INSTANT_SOUND; i++)
		this->m_szSoundName[i][0] = 0;

	m_pSoundNode = NULL;
	m_pWave = NULL;
}

void	KInstantSpecial::LoadSprName()
{
	int		i;
	for (i = 0; i < MAX_INSTANT_STATE; i++)
		m_szSprName[i][0] = 0;

	KTabFile	cSprName;
//	g_SetFilePath("\\");
	if (!cSprName.Load(PLAYER_INSTANT_SPECIAL_FILE))
		return;
	for (i = 0; i < MAX_INSTANT_STATE; i++)
		cSprName.GetString(i + 2, 3, "", m_szSprName[i], sizeof(m_szSprName[i]));
}

void	KInstantSpecial::LoadSoundName()
{
	int		i;
	for (i = 0; i < MAX_INSTANT_SOUND; i++)
		m_szSoundName[i][0] = 0;

	KIniFile	cSoundName;
	char		szTemp[32];
//	g_SetFilePath("\\");
	if (!cSoundName.Load(defINSTANT_SOUND_FILE))
		return;
	for (i = 0; i < MAX_INSTANT_SOUND; i++)
	{
		sprintf(szTemp, "%d", i);
		cSoundName.GetString("Game", szTemp, "", this->m_szSoundName[i], sizeof(m_szSoundName[i]));
	}
}

void	KInstantSpecial::GetSprName(int nNo, char *lpszName, int nLength)
{
	if (!lpszName || nLength <= 0)
		return;
	if (nNo < 0 || nNo >= MAX_INSTANT_STATE)
	{
		lpszName[0] = 0;
		return;
	}
	if (this->m_nLoadFlag == FALSE)
	{
		this->LoadSprName();
		this->LoadSoundName();
		m_nLoadFlag = TRUE;
	}

	if (strlen(this->m_szSprName[nNo]) < (DWORD)nLength)
		strcpy(lpszName, m_szSprName[nNo]);
	else
		lpszName[0] = 0;
}

void	KInstantSpecial::PlaySound(int nNo)
{
	if (this->m_nLoadFlag == FALSE)
	{
		this->LoadSprName();
		this->LoadSoundName();
		m_nLoadFlag = TRUE;
	}
	if (nNo < 0 || nNo >= MAX_INSTANT_SOUND)
		return;
	if ( !m_szSoundName[nNo][0] )
		return;

	m_pSoundNode = (KCacheNode*)g_SoundCache.GetNode(m_szSoundName[nNo], (KCacheNode*)m_pSoundNode);
	m_pWave = (KWavSound*)m_pSoundNode->m_lpData;
	if (m_pWave)
	{
		if (m_pWave->IsPlaying())
			return;
		m_pWave->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
	}
}

#endif
//------------------------- class KInstantSpecial end --------------------------
