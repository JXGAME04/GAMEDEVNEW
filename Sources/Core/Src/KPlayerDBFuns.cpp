#include "KCore.h"

#ifdef _SERVER
#include "KEngine.h"
#include "KSubWorldSet.h"
#include "KSubWorld.h"
#include "KPlayer.h"
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#include "KNpc.h"
#include "KItem.h"
#include "KItemList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KItemChangeRes.h"
#include <time.h>
//#include "MyAssert.H"
#include "KTaskFuns.h"

//#define DEBUGOPT_DB_ROLEDATA_OUT 

KList g_DBMsgList;

int KPlayer::AddDBPlayer(char * szPlayerName, int sex, DWORD * pdwID)
{
	return FALSE;
}

int KPlayer::LoadDBPlayerInfo(BYTE *pPlayerInfo, int &nStep, unsigned int &nParam)
{
	_ASSERT(pPlayerInfo);
	int nRet = 0;
	int nRetValue = 0;
	
	switch(nStep)
	{
	case STEP_BASE_INFO:
		m_pCurStatusOffset = pPlayerInfo;
		if ((nRet = LoadPlayerBaseInfo(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++; 
			nParam = 0;
		}
		else 
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}

	//	UpdataCurData();
		break;
	case STEP_FIGHT_SKILL_LIST:
		if ((nRet = LoadPlayerFightSkillList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}

	//	UpdataCurData();
		break;
	case STEP_STATE_SKILL_LIST:
		if ((nRet = LoadPlayerStateSkillList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep ++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}

	//	UpdataCurData();
		break;
	case STEP_TASK_LIST:
		//*************************************************		
		if ((nRet = LoadPlayerTaskList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			
			int i = 0;
			for (i  = 0; i < TASKVALUE_MAXWAYPOINT_COUNT; i ++)
			{
				int nWayPoint = m_cTask.nSave[TASKVALUE_SAVEWAYPOINT_BEGIN + i];
				if (nWayPoint)
				{
					KIndexNode * pNewNode = new KIndexNode;
					pNewNode->m_nIndex = nWayPoint;
					m_PlayerWayPointList.AddTail(pNewNode);
				}
			}
			
			for (i  = 0; i < TASKVALUE_MAXSTATION_COUNT / 2; i ++)
			{
				DWORD Stations = 0;
				if (Stations = (DWORD) m_cTask.GetSaveVal(TASKVALUE_SAVESTATION_BEGIN + i))
				{
					int nStation1 = (int) HIWORD(Stations);
					int nStation2 = (int) LOWORD(Stations);
					
					if (nStation1)
					{
						KIndexNode * pNewNode = new KIndexNode;
						pNewNode->m_nIndex = nStation1;
						m_PlayerStationList.AddTail(pNewNode);
					}
					
					if (nStation2)
					{
						KIndexNode * pNewNode = new KIndexNode;
						pNewNode->m_nIndex = nStation2;
						m_PlayerStationList.AddTail(pNewNode);
					}
				}
			}
			g_TimerTask.LoadTask(this);
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep ++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}

	//	UpdataCurData();
		break;
	case STEP_ITEM_LIST:
		//*************************************************
		// 玩家的装备列表
		if ((nRet = LoadPlayerItemList(pPlayerInfo, m_pCurStatusOffset, nParam)) == 1)
		{
			nRetValue = SendSyncData(nStep, nParam);
			nStep++;
			nParam = 0;
		}
		else
		{
			if (nRet == -1)
			{
				nStep ++;
				nParam = 0;
				return 0;
			}
			else
			{
				nRetValue = SendSyncData(nStep, nParam);
			}
		}

	//	UpdataCurData();
		break;
	default:
		nStep = STEP_SYNC_END;
		break;
	}
	return nRetValue;
}

int KPlayer::UpdateDBPlayerInfo(BYTE* pPlayerInfo)
{
	if (!pPlayerInfo)
		return -1;

	SavePlayerBaseInfo(pPlayerInfo);

	SavePlayerFightSkillList(pPlayerInfo);

	SavePlayerStateSkillList(pPlayerInfo);

	SavePlayerTaskList(pPlayerInfo);

	SavePlayerItemList(pPlayerInfo);

	return 1;	
}

int	KPlayer::LoadPlayerBaseInfo(BYTE  * pRoleBuffer, BYTE * &pCurData, unsigned int &nParam)
{
	if (!pRoleBuffer)
	{
		KASSERT(pRoleBuffer);
		return -1;
	}
	
	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	
	if (nParam != 0) return -1;
	
	int		nSex;
	int		nLevel;	
#define PLAYER_MALE_NPCTEMPLATEID		-1
#define PLAYER_FEMALE_NPCTEMPLATEID		-2

	nLevel = pRoleData->BaseInfo.ifightlevel;
	nSex = pRoleData->BaseInfo.bSex;
	if (nSex)
		nSex = MAKELONG(nLevel, PLAYER_FEMALE_NPCTEMPLATEID);
	else
		nSex = MAKELONG(nLevel, PLAYER_MALE_NPCTEMPLATEID);

	m_sLoginRevivalPos.m_nSubWorldID = pRoleData->BaseInfo.irevivalid;
	m_sLoginRevivalPos.m_ReviveID = pRoleData->BaseInfo.irevivalx;
	//---------------------------------------------------------------------------------------------
label_retry:
	POINT Pos;
	if(g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_ReviveID, &Pos))
	{
		m_sLoginRevivalPos.m_nMpsX = Pos.x;
		m_sLoginRevivalPos.m_nMpsY = Pos.y;
	}
	else
	{
		m_sLoginRevivalPos.m_nSubWorldID = 53;
		m_sLoginRevivalPos.m_ReviveID = 19;
		m_sLoginRevivalPos.m_nMpsX = 52032;
		m_sLoginRevivalPos.m_nMpsY = 101696;
	}
	//---------------------------------------------------------------------------------------------
	PLAYER_REVIVAL_POS	tempPos;
	BOOL bUseRevive = FALSE;
	if (pRoleData->BaseInfo.cUseRevive)
	{
		bUseRevive = TRUE;
		tempPos.m_nSubWorldID = m_sLoginRevivalPos.m_nSubWorldID;
		tempPos.m_ReviveID = m_sLoginRevivalPos.m_ReviveID;
		tempPos.m_nMpsX = m_sLoginRevivalPos.m_nMpsX;
		tempPos.m_nMpsY = m_sLoginRevivalPos.m_nMpsY;
	}
	else
	{
		tempPos.m_nSubWorldID = pRoleData->BaseInfo.ientergameid;
		tempPos.m_nMpsX = pRoleData->BaseInfo.ientergamex;
		tempPos.m_nMpsY = pRoleData->BaseInfo.ientergamey;
	}
/*
	if(g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID) == -1 //neu player vao map khong co trong worldset thi load subworld dau tien
		|| (pRoleData->BaseInfo.cUseRevive && (tempPos.m_nMpsX == 0 && tempPos.m_nMpsY ==0)) //neu player vao map pos x y = 0 load u ti猲
		) 
	{
		g_SubWorldSet.GetRevivalPosFromId(SubWorld[0].m_SubWorldID,1,&Pos);
		tempPos.m_nSubWorldID = SubWorld[0].m_SubWorldID;
		tempPos.m_nMpsX = Pos.x;
		tempPos.m_nMpsY = Pos.y;
	}

	int nWorldIdx = g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID);

	if (nWorldIdx < 0
		|| (pRoleData->BaseInfo.cUseRevive &&
			(tempPos.m_nMpsX <= 0 || tempPos.m_nMpsY <= 0)))
	{
		printf("[Fix] Invalid Map or Pos -> Move to map 53\n");

		POINT Pos = { 0, 0 };

	
		g_SubWorldSet.GetRevivalPosFromId(53, 1, &Pos);

		
		if (Pos.x <= 0 || Pos.y <= 0)
		{
			Pos.x = 1614;   
			Pos.y = 3206;
		}

		tempPos.m_nSubWorldID = 53;
		tempPos.m_nMpsX = Pos.x;
		tempPos.m_nMpsY = Pos.y;
	}
*/
	int nSeries = pRoleData->BaseInfo.ifiveprop;
	m_nIndex = NpcSet.AddNpcSet2(nSex, nSeries, g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID), tempPos.m_nMpsX, tempPos.m_nMpsY);
	if (m_nIndex <= 0)
	{
		if (pRoleData->BaseInfo.cUseRevive)
		{
			printf("[Error!]AddNpc Error DBFuns.cpp wrongmapos\n");
			m_nIndex = NpcSet.AddNpcSet2(nSex, nSeries, g_SubWorldSet.SearchWorld(53), 51200, 102400);
			if(m_nIndex <= 0)
				return -1;
			pRoleData->BaseInfo.cIsExchange = 0;
		}
		else
		{
			pRoleData->BaseInfo.cUseRevive = 1;
			goto label_retry;
		}
	}

	m_sDeathRevivalPos = m_sLoginRevivalPos;

	KNpc* pNpc = &Npc[m_nIndex];
	pNpc->m_Kind = kind_player;
	pNpc->SetPlayerIdx(m_nPlayerIndex);
	pNpc->m_Level = nLevel;

	strcpy(pNpc->Name, pRoleData->BaseInfo.szName);
	m_nForbiddenFlag = pRoleData->BaseInfo.nForbiddenFlag;
	m_nAttributePoint = pRoleData->BaseInfo.ileftprop;
	m_nSkillPoint = pRoleData->BaseInfo.ileftfight;

	m_ImagePlayer			= pRoleData->BaseInfo.ipduphong1; //#avatar player
	strcpy(m_PlayerMateName,pRoleData->BaseInfo.szStringduphong1); //#MateName

	m_nStrength		= pRoleData->BaseInfo.ipower;
	m_nDexterity	= pRoleData->BaseInfo.iagility;
	m_nVitality		= pRoleData->BaseInfo.iouter;
	m_nEngergy		= pRoleData->BaseInfo.iinside;
	m_nLucky		= pRoleData->BaseInfo.iluck;
	m_cTong.Clear();
	m_cTong.DBSetTongNameID(pRoleData->BaseInfo.dwTongID);

	m_nCurStrength = m_nStrength;
	m_nCurDexterity = m_nDexterity;
	m_nCurVitality = m_nVitality;
	m_nCurEngergy = m_nEngergy;
	m_nCurLucky = m_nLucky;
	m_nUpExp = 0;
	this->SetFirstDamage();
	this->SetBaseAttackRating();
	this->SetBaseDefence();

	m_nExp			= pRoleData->BaseInfo.fightexp;
	m_nNextLevelExp = PlayerSet.m_cLevelAdd.GetLevelExp(pNpc->m_Level);
	m_dwLeadLevel	= pRoleData->BaseInfo.ileadlevel;
	m_dwLeadExp		= pRoleData->BaseInfo.ileadexp;

	m_cFaction.m_nCurFaction = (char)pRoleData->BaseInfo.nSect;
	m_cFaction.m_nFirstAddFaction = (char)pRoleData->BaseInfo.nFirstSect;
	m_cFaction.m_nAddTimes			= pRoleData->BaseInfo.ijoincount;
	pNpc->m_btRankId				= pRoleData->BaseInfo.isectrole;
	pNpc->m_ExItemId				= pRoleData->BaseInfo.iexitemrole; // hanh trang
	pNpc->m_ExBoxId					= pRoleData->BaseInfo.iexboxrole; // ruong mo rong
	m_nWorldStat	= pRoleData->BaseInfo.nWorldStat;	//x誴 h筺g th?gi韎
	m_nSectStat		= pRoleData->BaseInfo.nSectStat;
	m_nChestPW		= pRoleData->BaseInfo.ipassrole; 
	SetChestPW(m_nChestPW);
	int nCashMoney = 0;
	int nSaveMoney = 0;
//	this->m_ItemList.Init(GetPlayerIndex());
	nCashMoney		= pRoleData->BaseInfo.imoney;
	nSaveMoney		= pRoleData->BaseInfo.isavemoney;
	m_ItemList.SetMoney(nCashMoney, nSaveMoney,0);
	m_nActiveEquipNum = pRoleData->BaseInfo.ipduphong3; //item set num

	pNpc->m_Series	= pRoleData->BaseInfo.ifiveprop;
	pNpc->m_Camp	= pRoleData->BaseInfo.iteam;
	pNpc->m_nSex	= pRoleData->BaseInfo.bSex;
	pNpc->m_LifeMax	= pRoleData->BaseInfo.imaxlife;
	pNpc->m_StaminaMax = pRoleData->BaseInfo.imaxstamina;
	pNpc->m_ManaMax = pRoleData->BaseInfo.imaxinner;
	pNpc->m_LifeReplenish = PLAYER_LIFE_REPLENISH;
	pNpc->m_ManaReplenish = PLAYER_MANA_REPLENISH;
	pNpc->m_StaminaGain = PLAYER_STAMINA_GAIN;
	pNpc->m_StaminaLoss = PLAYER_STAMINA_LOSS;

	this->SetBaseResistData();
	SetBaseSpeedAndRadius();
	pNpc->RestoreNpcBaseInfo();
	
	pNpc->m_CurrentLife = pRoleData->BaseInfo.icurlife;
	if(pNpc->m_CurrentLife < 0)
		pNpc->m_CurrentLife = 1;
	pNpc->m_CurrentMana = pRoleData->BaseInfo.icurinner;
	pNpc->m_CurrentStamina = pRoleData->BaseInfo.icurstamina;

	m_cPK.SetNormalPKState(pRoleData->BaseInfo.cPkStatus, pRoleData->BaseInfo.ipduphong2); //#PKState
	m_cPK.SetPKValue(pRoleData->BaseInfo.ipkvalue);
	m_cRepute.SetReputeValue(pRoleData->BaseInfo.ireputevalue);
	m_cFuYuan.SetFuYuanValue(pRoleData->BaseInfo.ifuyuanvalue);
	m_cReBorn.SetReBornValue(pRoleData->BaseInfo.irebornvalue);
	BYTE tmpMeridian[MAX_MERIDIAN];
	memcpy(tmpMeridian, pRoleData->BaseInfo.szStringduphong2, sizeof(tmpMeridian));
	m_cMeridian.setMeridian(tmpMeridian);

	m_BuyInfo.Clear();
	m_cMenuState.Release();
	m_cChat.Release();
	m_cTeam.Release();
	m_cTeam.SetCanTeamFlag(m_nPlayerIndex, TRUE);
	m_nPeapleIdx = 0;
	m_nObjectIdx = 0;
	memset(m_szTaskAnswerFun, 0, sizeof(m_szTaskAnswerFun));
	m_nAvailableAnswerNum = 0;
	Npc[m_nIndex].m_ActionScriptID = 0;
	Npc[m_nIndex].m_TrapScriptID = 0;
	m_nViewEquipTime = 0;

	pNpc->m_Experience = 0;
//	memset(pNpc->m_szChatBuffer, 0, sizeof(pNpc->m_szChatBuffer));
//	pNpc->m_nCurChatTime = 0;

	pNpc->m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0);
	pNpc->m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
	pNpc->m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
	pNpc->m_HorseType = g_ItemChangeRes.GetHorseRes(0, 0);
	pNpc->m_bRideHorse = FALSE;
	pNpc->m_btHonorId = pRoleData->BaseInfo.ipduphong4; //m_btHonorId
	nParam = 1;
	pCurData = (BYTE *)&pRoleData->pBuffer;
	if (bUseRevive)
		pNpc->m_FightMode = 0;
	else
		pNpc->m_FightMode = pRoleData->BaseInfo.cFightMode;
	if (pRoleData->BaseInfo.cIsExchange)
	{
		pRoleData->BaseInfo.cIsExchange = 0; //add by tuanln
		char szMapTypeTmp[32] = {0};
		int nSubWorldIndexTemp = g_SubWorldSet.SearchWorld(pRoleData->BaseInfo.ientergameid);
		strcpy(szMapTypeTmp, SubWorld[nSubWorldIndexTemp].szMapType);
		if(strcmp(szMapTypeTmp,"City") == 0 || strcmp(szMapTypeTmp,"Capital") == 0 || strcmp(szMapTypeTmp,"Country") == 0) //th祅h th?// th? // th玭 l祅g add by tuanln
		{
			pNpc->m_FightMode = 0;
		}
		else if(strcmp(szMapTypeTmp,"Field") == 0 || strcmp(szMapTypeTmp,"Cave") == 0)
		{
			pNpc->m_FightMode = 1;
		}
	}
	UpdataCurData();
	return 1;
}
static bool g_ExcludeLine[140] = { 0 };
static bool g_ExcludeLineInit = false;

void InitExcludeLines()
{
    if (g_ExcludeLineInit)
        return;

    int arr[] = {
        0,5,10,15,20,25,
        30,38,45,50,
        60,65,70,75,
        80,
        93,95,
        100,
        110,115,
        120,125,130
    };

    int nCount = sizeof(arr) / sizeof(int);
    for (int i = 0; i < nCount; ++i)
    {
        int n = arr[i];
        if (n >= 0 && n < 140)
            g_ExcludeLine[n] = true;
    }

    g_ExcludeLineInit = true;
}


int	KPlayer::LoadPlayerItemList(BYTE * pRoleBuffer , BYTE* &pItemBuffer, unsigned int &nParam)
{
	KASSERT(pRoleBuffer);
	
	int nItemCount = ((TRoleData *)pRoleBuffer)->nItemCount;
	// [24/08] NEO LAI theo dwItemOffset o lan doc DAU TIEN (nParam == 0) thay vi
	// tin vao con tro chay - cung ly do va cung khuon nhu LoadPlayerFightSkillList.
	// Day la buoc de hong nhat: doc lech mot chut la sai toan bo tui do, roi ban
	// hong duoc ghi de len o lan luu ke tiep.
	TDBItemData * pItemData = (nParam == 0)
		? (TDBItemData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwItemOffset)
		: (TDBItemData *)pItemBuffer;
	
	if (nItemCount == 0) return 1;
	if (nParam != 0)
//		m_ItemList.Init(m_nPlayerIndex);
//	else 
	{
		if (nParam >= nItemCount )
			return -1;
	}

	char szSection[100];
	char szSubSection[100];
	char szKey[100];
	int nItemClass;
	int nItemBaseInfoCount ;
	int nItemEfficInfoCount;
	int nItemRequireInfoCount;
	int dwItemID = 0; 
	int nDetailType = 0;
	int nParticularType = 0;
	int nSeries = 0;
	int nLevel = 0;
	int nLocal = 0;
	int nItemX = 0;
	int nItemY = 0;
	int	nLucky = 0;
	int nBegin = nParam;
	int nEnd = nParam + DBLOADPERTIME_ITEM;
	
	if ( nEnd > nItemCount)
		nEnd = nItemCount;

	nParam = nEnd;	
	
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
		
	for (int i = nBegin ; i < nEnd; i ++)
	{
		
		KItem NewItem;	
		ZeroMemory(&NewItem, sizeof(KItem));
		
		//**************************************************************
		nItemClass	= pItemData->iequipclasscode;
		NewItem.m_CommonAttrib.nItemNature		= pItemData->iequipnaturecode;
		NewItem.m_CommonAttrib.nDetailType		= pItemData->idetailtype;
		NewItem.m_CommonAttrib.nParticularType	= pItemData->iparticulartype;
		NewItem.m_CommonAttrib.nLevel			= pItemData->ilevel;
		NewItem.m_CommonAttrib.nSeries			= pItemData->iseries;
		NewItem.m_CommonAttrib.nStackNum		= pItemData->istacknum;
		NewItem.m_CommonAttrib.nEnChance		= pItemData->ienchance;
		NewItem.m_CommonAttrib.nPoint			= pItemData->ipoint;
		nItemX			= pItemData->ix;
		nItemY			= pItemData->iy;
		nLocal			= pItemData->ilocal;
		NewItem.m_CommonAttrib.nRow = pItemData->irow;
		memset(NewItem.m_GeneratorParam.nGeneratorLevel, 0, sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));
		memcpy(NewItem.m_GeneratorParam.nGeneratorLevel, pItemData->iparam, sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));

		//**************************************************************
		//NewItem.SetID(dwItemID);
		
		NewItem.m_GeneratorParam.nVersion			= pItemData->iequipversion;
		NewItem.m_GeneratorParam.uRandomSeed		= pItemData->irandseed;
		NewItem.m_GeneratorParam.nLuck				= pItemData->ilucky;

		NewItem.m_CommonAttrib.nGoldId				= pItemData->igoldid;

		NewItem.SetPlayerItemLock(pItemData->ilockbh);
		NewItem.SetPlayerItemHLock(pItemData->igiomokhoa);

		NewItem.SetMantle(pItemData->iiduphong1);//#phi phong
		BOOL bGetEquiptResult = 0;
		InitExcludeLines();
		int nLine = NewItem.GetLine();

		if (NewItem.GetNature() >= NATURE_GOLD &&
	    nLine >= 0 && nLine <= 139 &&
	    !g_ExcludeLine[nLine])   // 
		{
		    bool bHasTime =
		        (pItemData->iyear  != 0 ||
		         pItemData->imonth != 0 ||
		         pItemData->iday   != 0 ||
		         pItemData->ihour  != 0);
		
		   
		    if (!bHasTime && pItemData->ilockbh == 0)
		    {
		        pItemData->ilockbh = -2;
		        NewItem.SetPlayerItemLock(pItemData->ilockbh);
		    }
		}

		if (pItemData->iyear)
		if (pItemData->iyear - 1900 < timeinfo->tm_year)
		{
			pItemData ++;
			continue;
		}
		else if (pItemData->iyear - 1900 == timeinfo->tm_year)
			if (pItemData->imonth < timeinfo->tm_mon + 1)
			{
				pItemData ++;
				continue;
			}
			else if (pItemData->imonth == timeinfo->tm_mon + 1)
				if (pItemData->iday < timeinfo->tm_mday)
				{
					pItemData ++;
					continue;
				}
				else if (pItemData->iday == timeinfo->tm_mday)
					if (pItemData->ihour <= timeinfo->tm_hour)
					{
						pItemData ++;
						continue;
					}

		switch(nItemClass)
		{
		case item_equip :			
			if (!pItemData->igoldid) {
				bGetEquiptResult = ItemGen.Gen_ExistEquipment(
					NewItem.m_CommonAttrib.nItemNature,
					NewItem.m_CommonAttrib.nItemNature >= NATURE_GOLD ? NewItem.m_CommonAttrib.nRow : NewItem.m_CommonAttrib.nDetailType,
					NewItem.m_CommonAttrib.nParticularType,
					NewItem.m_CommonAttrib.nSeries,
					NewItem.m_CommonAttrib.nLevel,
					NewItem.m_GeneratorParam.nGeneratorLevel,
					NewItem.m_GeneratorParam.nLuck,
					NewItem.m_GeneratorParam.nVersion,
					&NewItem);

				NewItem.SetPlayerItemLock(pItemData->ilockbh);
				NewItem.SetPlayerItemHLock(pItemData->igiomokhoa);
			}
			else
			bGetEquiptResult = ItemGen.GetGoldItemByIndex(NewItem.m_CommonAttrib.nGoldId,
				&NewItem,
				NewItem.m_GeneratorParam.nGeneratorLevel,
				NewItem.m_CommonAttrib.nSeries,
				NewItem.m_CommonAttrib.nEnChance);

			NewItem.SetMaxOptMultiply(pItemData->iiduphong4);
			break;
		case item_medicine:			
				bGetEquiptResult = ItemGen.Gen_Medicine(
					NewItem.m_CommonAttrib.nDetailType,
					NewItem.m_CommonAttrib.nParticularType,
					NewItem.m_CommonAttrib.nLevel,
					NewItem.m_GeneratorParam.nVersion, 
					&NewItem,
					NewItem.m_CommonAttrib.nStackNum);
			break;

		case item_mine:				
			{
			
			}
			break;
		case item_materials:		
			{
				
			}
			break;
		case item_task:				
			{
				bGetEquiptResult = ItemGen.Gen_Quest(NewItem.m_CommonAttrib.nDetailType, &NewItem,NewItem.m_CommonAttrib.nStackNum);
			}
			break;
		case item_townportal:
			{
				bGetEquiptResult = ItemGen.Gen_TownPortal(&NewItem);
			}
			break;
		case item_magicscript:
			{
				bGetEquiptResult = ItemGen.Gen_MagicScript(NewItem.m_CommonAttrib.nDetailType, NewItem.m_CommonAttrib.nParticularType, 
					&NewItem,
					NewItem.m_CommonAttrib.nLevel,
					NewItem.m_CommonAttrib.nSeries,
					NewItem.m_GeneratorParam.nLuck, NewItem.m_CommonAttrib.nStackNum);
			}
			break;
		case item_fusion:	// [DUNGLUYEN-PB 01/09] vien Van Cuong (genre 8): duong nap DB PHAI qua Gen_Fusion nhu
			{				// KItemSet::AddItemSet2 - khong thi NewItem genre 0 / kich thuoc 0 -> AddKIL that bai -> vien MAT sau relogin
				bGetEquiptResult = ItemGen.Gen_Fusion(NewItem.m_CommonAttrib.nParticularType, &NewItem, NewItem.m_CommonAttrib.nStackNum);
			}
			break;
		case item_starstone:	// [DUNGLUYEN-PB 01/09] Tinh Than Thach (genre 9): cung loi tu 29/08 (chua bao gio co case nap DB)
			{
				bGetEquiptResult = ItemGen.Gen_StarStone(NewItem.m_CommonAttrib.nParticularType, &NewItem, NewItem.m_CommonAttrib.nStackNum);
			}
			break;
		}

		//if (pItemData->idurability != 0)
		NewItem.SetDurability(pItemData->idurability); //#do ben
		NewItem.SetParam(pItemData->iiduphong2);//s?l莕 s?d鬾g item
		NewItem.SetItemGlowLight(pItemData->iiduphong3); //ngo筰 trang item ph竧 s竛g
        NewItem.SetPlayerItemLock(pItemData->ilockbh);
		NewItem.SetPlayerItemHLock(pItemData->igiomokhoa);
		NewItem.SetExpTime(pItemData->iyear,pItemData->imonth,pItemData->iday,pItemData->ihour);

		NewItem.m_CommonAttrib.uPrice = pItemData->iiduphong9; //Load gia bay ban
		// [PHI PHONG 2026-08-29] nap cap sao / ma da / cap lo / chuc phuc tu 4 o du phong
		NewItem.SetPfPack(0, pItemData->iiduphong5);
		NewItem.SetPfPack(1, pItemData->iiduphong6);
		NewItem.SetPfPack(2, pItemData->iiduphong7);
		NewItem.SetPfPack(3, pItemData->iiduphong8);
		// [PF13 31/08] don bo cuc pfpack:
		// - mon KHONG phai phi phong: 4 o nay vo nghia, ban ghi truoc 29/08 co the
		//   mang byte RAC (m_SaveBuffer chi ZeroMemory mot lan luc dang nhap) -> xoa.
		// - phi phong bo cuc v1 (chua co bit PF_CO_V2): pack[3] la time_t lan dot
		//   pha (~1,7 ty) se bi bo cuc v2 doc thanh MA DA LO 6-10 -> xoa pack[3],
		//   xoa bit cap-lo-cao cua pack[2], xoa bit 12-23 cu cua pack[0] (so lo/
		//   tran chuc phuc/canupstar - gio tra tu bang), roi bat bit v2.
		if (NewItem.GetGenre() != item_equip || NewItem.GetDetailType() != equip_mantle)
		{
			if (NewItem.GetPfPack(0) || NewItem.GetPfPack(1) || NewItem.GetPfPack(2) || NewItem.GetPfPack(3))
			{
				NewItem.SetPfPack(0, 0);
				NewItem.SetPfPack(1, 0);
				NewItem.SetPfPack(2, 0);
				NewItem.SetPfPack(3, 0);
			}
		}
		else if (!(NewItem.GetPfPack(0) & KItem::PF_CO_V2))
		{
			NewItem.SetPfPack(0, (NewItem.GetPfPack(0) & 0x00000FFF) | KItem::PF_CO_V2);
			NewItem.SetPfPack(2, NewItem.GetPfPack(2) & 0x000FFFFF);
			NewItem.SetPfPack(3, 0);
		}

		// [DUNGLUYEN 01/09] giai nen 6 o Van Cuong + seed (xem SavePlayerItemList). Ban ghi khong co dau 'FUS1'
		// (moi ban ghi truoc 01/09) -> de trong: iid/iBaiTanPrice cu co the mang byte RAC. Chi nhan P co trong
		// fusion.txt va chi tren trang bi vang/bach kim (hoac vien Van Cuong).
		NewItem.ClearFusion();
		if (pItemData->iidentify == 0x46555331)
		{
			int aP[6];
			aP[0] = pItemData->iid & 0x3FF;
			aP[1] = (pItemData->iid >> 10) & 0x3FF;
			aP[2] = (pItemData->iid >> 20) & 0x3FF;
			aP[3] = pItemData->iBaiTanPrice & 0x3FF;
			aP[4] = (pItemData->iBaiTanPrice >> 10) & 0x3FF;
			aP[5] = (pItemData->iBaiTanPrice >> 20) & 0x3FF;
			unsigned aS[6];
			aS[0] = (unsigned)pItemData->iequipcode;
			aS[1] = (unsigned)pItemData->ilocksell;
			aS[2] = (unsigned)pItemData->ilocktrade;
			aS[3] = (unsigned)pItemData->ilockdrop;
			aS[4] = (unsigned)pItemData->ifortune;
			aS[5] = (unsigned)pItemData->iowner;
			if (NewItem.GetGenre() == item_fusion)
			{
				NewItem.SetFusion(0, aP[0] ? 1 : 0, aS[0] ? aS[0] : 1);
			}
			else if (NewItem.GetGenre() == item_equip &&
				(NewItem.GetNature() == NATURE_GOLD || NewItem.GetNature() == NATURE_PLATINA))
			{
				for (int k = 0; k < 6; k++)
				{
					if (aP[k] > 0 && KItem::FUS_GetQuality(aP[k]) > 0)
						NewItem.SetFusion(k, aP[k], aS[k]);
				}
			}
		}

		pItemData ++;
		
		int nIndex = ItemSet.AddI(&NewItem);
		
		if (nIndex <= 0) 
		{
			KASSERT(0);
			continue ;
		}
		m_ItemList.AddKIL(nIndex, nLocal, nItemX, nItemY);

	}

	pItemBuffer	= (BYTE *)pItemData;

	if (nParam >= nItemCount)
		return 1;
	else 
		return 0;
}

int	KPlayer::LoadPlayerFightSkillList(BYTE * pRoleBuffer, BYTE * &pFightBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	int nSkillCount = 0;
	char szSkillId[100];
	char szSkillLevel[100];
	nSkillCount = ((TRoleData*)(pRoleBuffer))->nFightSkillCount;
	
	if (nSkillCount == 0)	
		return 1;
	
	if (nParam >= nSkillCount )	
		return -1;
	
	int nBegin	= nParam;
	int nEnd	= nBegin + DBLOADPERTIME_SKILL;
	if (nEnd > nSkillCount) 	nEnd = nSkillCount;
	nParam = nEnd;
	TDBSkillData * pSkillData = NULL;
	if (nBegin == 0)
		pSkillData = (TDBSkillData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwFSkillOffset);
	else
		pSkillData = (TDBSkillData*)pFightBuffer;
	for (int i = nBegin ; i < nEnd; i ++, pSkillData ++ )
	{
		int nSkillId = 0;
		int nSkillLevel = 0;
		int nSkillExp = 0;
		nSkillId = pSkillData->m_nSkillId;
		nSkillLevel = pSkillData->m_nSkillLevel;
		nSkillExp = pSkillData->m_nSkillVal;
		Npc[m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel, nSkillExp);
	}
	pFightBuffer = (BYTE*) pSkillData;

	if (nParam >= nSkillCount) return 1;
	else 
		return 0;
}

int	KPlayer::LoadPlayerStateSkillList(BYTE * pRoleBuffer, BYTE * &pStateBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	int nStateCount = ((TRoleData*)(pRoleBuffer))->nStateSkillCount;
	
	if (nStateCount == 0)
		return 1;

	if (nParam >= nStateCount )		return -1;
	int nBegin	= nParam;
	int nEnd	= nStateCount;
	if (nEnd > nStateCount) 	nEnd = nStateCount;
	nParam = nEnd;

	TDBSkillData * pStateData = (TDBSkillData*)pStateBuffer;
	for (int i = nBegin ; i < nEnd; i ++, pStateData ++ )
	{
		KSkill *pSkill = (KSkill*)g_SkillManager.GetSkill(pStateData->m_nSkillId, pStateData->m_nSkillLevel);
		// (18/08 phan bien) blob co the mang state-skill khong con trong bang (doi bang
		// skill giua hai phien) - GetSkill tra NULL, deref la SAP SERVER luc nap nhan vat.
		if (!pSkill)
			continue;
		pSkill->CastStateSkill(m_nIndex, 0, 0, pStateData->m_nSkillVal, TRUE);
	}
	pStateBuffer = (BYTE*) pStateData;

	if (nParam >= nStateCount) return 1;
	else 
		return 0;
}

int	KPlayer::LoadPlayerTaskList(BYTE * pRoleBuffer, BYTE * &pTaskBuffer, unsigned int& nParam)
{
	KASSERT(pRoleBuffer);
	while(m_PlayerStationList.GetHead())
	{
		KIndexNode * pNode = (KIndexNode * ) m_PlayerStationList.GetHead();
		m_PlayerStationList.RemoveHead();
		delete pNode;
	}
	while(m_PlayerWayPointList.GetHead())
	{
		KIndexNode * pNode = (KIndexNode * ) m_PlayerWayPointList.GetHead();
		m_PlayerWayPointList.RemoveHead();
		delete pNode;
	}
	//
	if(nParam == 0)	
	{
		m_cTask.Release();
		SyncTaskValueToClient(-1, 0);	// [TaskGuide] bao client xoa gia tri nhan vat truoc
		{
			extern void KJx2CityWar_SyncToPlayer(int nPlayerIndex);
			KJx2CityWar_SyncToPlayer(m_nPlayerIndex);	// [CITYINFO 21/08] 7 thanh cho ban do/thue
		}
	}
	//
	int nTaskCount = 0;
	int nTaskId = 0;
	int nTaskDegee = 0;
	char szTaskIDKey[100];
	char szTaskValueKey[100];
	// [24/08] VA TRAN SO NHIEM VU. Truong 'nTaskCount' trong TRoleData la BYTE
	// (S3DBInterface.h:158) trong khi MAX_TASK = 4200: nhan vat co >= 256 nhiem vu
	// da luu bi cat con (n & 0xFF), va dung 256 thi thanh 0 -> ham nay 'return 1'
	// ngay ben duoi => MAT SACH nhiem vu, im lang.
	// KHONG the doi kieu truong do: S3DBInterface.h dung '#pragma pack(push,1)'
	// (dong 15) nen them/noi rong mot truong la DICH OFFSET moi truong sau no =>
	// hong toan bo roledb da luu; Goddess con neo thang vao kich thuoc
	// (DBTable_MySQL.cpp:69 MIN_ROLE_DATALEN, do that sizeof(TRoleData) = 746).
	// HAU QUA THAT SU NANG HON "mat nhiem vu": duong nap dung MOT con tro chay
	// (m_pCurStatusOffset) qua tung buoc, va LoadPlayerItemList lay vi tri vat pham
	// tu chinh con tro do chu KHONG seek lai theo dwItemOffset. Nen khi so task bi
	// cat, buoc vat pham doc do TU GIUA vung task => HONG CA TUI DO, roi ban hong
	// do duoc ghi de len o lan luu ke tiep.
	// Cach doc dung, khong dong vao struct, xem ngay ben duoi.
	{
		const TRoleData* pRD = (const TRoleData*)pRoleBuffer;
		// So task THAT nam san trong moi ban ghi (KE CA BAN GHI CU): SavePlayerTaskList
		// dat dwItemOffset tu con tro DA CHAY HET so task that (bien int, chua bi cat).
		// Cong thuc nay da duoc chinh du an kiem chung thuc nghiem tu 20/08 - xem
		// ToolsMySQL\jx_role.py: "(dwItemOffset-dwTaskOffset)/8 == nTaskCount" dung
		// 1003/1003 ban ghi cua roledb song; do lai 24/08 tren 3009 luot: 0 sai lech.
		const int nSpan = (int)(pRD->dwItemOffset - pRD->dwTaskOffset);
		const int nRec = (int)sizeof(TDBTaskData);
		if (nSpan >= 0 && nRec > 0 && (nSpan % nRec) == 0 && (nSpan / nRec) <= MAX_TASK)
			nTaskCount = nSpan / nRec;
		else
			nTaskCount = pRD->nTaskCount;	// ban ghi hong: quay ve truong BYTE cu
	}
	//
	if (nTaskCount == 0) 
		return 1;
	//
	if (nParam >= nTaskCount ) 
		return -1;
	//
	int nBegin	= nParam;
	int nEnd	= nBegin + DBLOADPERTIME_TASK;
	if (nEnd > nTaskCount) 
		nEnd = nTaskCount;
	nParam = nEnd;
	// [24/08] NEO LAI theo offset trong header o lan doc DAU TIEN, thay vi tin
	// vao con tro chay m_pCurStatusOffset. Day dung la khuon ma
	// LoadPlayerFightSkillList da dung san ("if (nBegin == 0) ... dwFSkillOffset").
	// Khong co no thi chi can MOT buoc nap truoc do thoat som la con tro lech, va
	// buoc nay doc du lieu tu giua vung khac - dem dung so luong cung vo ich.
	TDBTaskData * pTaskData = NULL;
	if (nBegin == 0)
		pTaskData = (TDBTaskData *)((BYTE*)pRoleBuffer + ((TRoleData*)pRoleBuffer)->dwTaskOffset);
	else
		pTaskData = (TDBTaskData*) pTaskBuffer;
	for (int i = nBegin; i < nEnd; i ++ , pTaskData ++)
	{
		nTaskId = pTaskData->m_nTaskId;
		nTaskDegee = pTaskData->m_nTaskValue;
		if (nTaskId >= MAX_TASK) 
		{
			KASSERT(0);
			continue;
		}
		m_cTask.SetSaveVal(nTaskId, nTaskDegee);
	}
	pTaskBuffer = (BYTE*) pTaskData;
	//
	if (nParam >= nTaskCount) 
		return 1;
	else 
		return 0;
}

int	KPlayer::SavePlayerBaseInfo(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) return -1;

	KNpc * pNpc = &Npc[m_nIndex];
	TRoleData * pRoleData = (TRoleData*)pRoleBuffer;
	//玩家信息
	memset(pRoleData, 0, sizeof(TRoleData));
	pRoleData->bBaseNeedUpdate = 1;
	strcpy(pRoleData->BaseInfo.szName, m_PlayerName);
	if (m_AccoutName[0])
		strcpy(pRoleData->BaseInfo.caccname, m_AccoutName);
	pRoleData->BaseInfo.nForbiddenFlag = m_nForbiddenFlag;
	pRoleData->BaseInfo.ileftprop = m_nAttributePoint;
	pRoleData->BaseInfo.ileftfight = m_nSkillPoint;
	pRoleData->BaseInfo.ipower = m_nStrength;
	pRoleData->BaseInfo.iagility = m_nDexterity;
	pRoleData->BaseInfo.iouter = m_nVitality;
	pRoleData->BaseInfo.iinside	=  m_nEngergy;
	pRoleData->BaseInfo.iluck = m_nLucky;
	pRoleData->BaseInfo.dwTongID = m_cTong.GetTongNameID();

	//玩家的显示信息	-- remark by spe because client ui display changed 2003/07/21
//	pRoleData->BaseInfo.ihelmres = pNpc->m_HelmType;
//	pRoleData->BaseInfo.iarmorres = pNpc->m_ArmorType;
//	pRoleData->BaseInfo.iweaponres = pNpc->m_WeaponType;
	
	//玩家等级信息
	pRoleData->BaseInfo.fightexp = m_nExp;
	pRoleData->BaseInfo.ifightlevel = pNpc->m_Level;
	
	pRoleData->BaseInfo.ileadlevel = m_dwLeadLevel;
	pRoleData->BaseInfo.ileadexp =	m_dwLeadExp;

	//门派信息
	pRoleData->BaseInfo.nSect =		m_cFaction.m_nCurFaction;
	pRoleData->BaseInfo.nFirstSect = 	m_cFaction.m_nFirstAddFaction;
	pRoleData->BaseInfo.ijoincount = m_cFaction.m_nAddTimes;
	pRoleData->BaseInfo.isectrole = pNpc->m_btRankId;
	pRoleData->BaseInfo.iexitemrole = pNpc->m_ExItemId; // hang trang
	pRoleData->BaseInfo.iexboxrole = pNpc->m_ExBoxId; // ruong mo rong
	pRoleData->BaseInfo.nWorldStat	= m_nWorldStat;	//x誴 h筺g th?gi韎
	pRoleData->BaseInfo.nSectStat	= m_nSectStat;
	pRoleData->BaseInfo.ipassrole =  m_nChestPW;
	
	pRoleData->BaseInfo.ipduphong1 = m_ImagePlayer; //#avatar player
	strcpy(pRoleData->BaseInfo.szStringduphong1,m_PlayerMateName); //#MateName

	int nCashMoney = 0;
	int nSaveMoney = 0;
	
	nCashMoney  = m_ItemList.GetMoney(room_equipment);
	nSaveMoney	= m_ItemList.GetMoney(room_repository);
	pRoleData->BaseInfo.imoney			= nCashMoney;
	pRoleData->BaseInfo.ipduphong3		= m_nActiveEquipNum; //save equip set num
	pRoleData->BaseInfo.isavemoney		= nSaveMoney;
	pRoleData->BaseInfo.ifiveprop		= pNpc->m_Series;
	pRoleData->BaseInfo.iteam			= pNpc->m_Camp;
	pRoleData->BaseInfo.bSex			= pNpc->m_nSex;
	pRoleData->BaseInfo.imaxlife		= pNpc->m_LifeMax;
	pRoleData->BaseInfo.imaxstamina		= pNpc->m_StaminaMax;
	pRoleData->BaseInfo.imaxinner		= pNpc->m_ManaMax;
	pRoleData->BaseInfo.icurlife		= pNpc->m_CurrentLife;
	pRoleData->BaseInfo.icurinner		= pNpc->m_CurrentMana;
	pRoleData->BaseInfo.icurstamina		= pNpc->m_CurrentStamina;

	pRoleData->BaseInfo.irevivalid = 	m_sLoginRevivalPos.m_nSubWorldID;
	pRoleData->BaseInfo.irevivalx = 	m_sLoginRevivalPos.m_ReviveID;
	pRoleData->BaseInfo.irevivaly = 	0;

	if (m_bExchangeServer)	// chuy觧 gs
	{
		pRoleData->BaseInfo.cUseRevive = 0;
		pRoleData->BaseInfo.ientergameid = m_sExchangePos.m_dwMapID;
		pRoleData->BaseInfo.ientergamex = m_sExchangePos.m_nX;
		pRoleData->BaseInfo.ientergamey = m_sExchangePos.m_nY;
		pRoleData->BaseInfo.cFightMode = (BYTE)pNpc->m_FightMode;
		pRoleData->BaseInfo.cIsExchange = 1;
	}
	else if (pNpc->m_SubWorldIndex >= 0 && pNpc->m_RegionIndex >= 0 && pNpc->m_Doing != do_death && pNpc->m_Doing != do_revive)
	{
		pRoleData->BaseInfo.cUseRevive = m_bUseReviveIdWhenLogin;
		pRoleData->BaseInfo.ientergameid = SubWorld[pNpc->m_SubWorldIndex].m_SubWorldID;
		pNpc->GetMpsPos(&pRoleData->BaseInfo.ientergamex, &pRoleData->BaseInfo.ientergamey);
		pRoleData->BaseInfo.cFightMode = (BYTE)pNpc->m_FightMode;
	}
	else
	{
		pRoleData->BaseInfo.cUseRevive = 1;
		pRoleData->BaseInfo.cFightMode = 0;
		if (pNpc->m_Doing == do_death || pNpc->m_Doing == do_revive)
		{
			pRoleData->BaseInfo.icurlife = pNpc->m_LifeMax;
			pRoleData->BaseInfo.icurinner = pNpc->m_ManaMax;
			pRoleData->BaseInfo.icurstamina = pNpc->m_StaminaMax;
		}
	}

	//PK相关
	pRoleData->BaseInfo.cPkStatus = (BYTE)m_cPK.GetNormalPKState();
	pRoleData->BaseInfo.ipkvalue = m_cPK.GetPKValue();
	//
	pRoleData->BaseInfo.ipduphong2 = (BOOL)m_cPK.m_bLockPK; //#PKState
	pRoleData->BaseInfo.ireputevalue = m_cRepute.GetReputeValue();//Repute
	pRoleData->BaseInfo.ifuyuanvalue = m_cFuYuan.GetFuYuanValue();//Fuyuan
	pRoleData->BaseInfo.irebornvalue = m_cReBorn.GetReBornValue();// Reborn
	pRoleData->BaseInfo.ipduphong4 = pNpc->m_btHonorId; //m_btHonorId
	//
	pRoleData->dwFSkillOffset = (BYTE * )pRoleData->pBuffer - (BYTE *)pRoleBuffer;
	memcpy(pRoleData->BaseInfo.szStringduphong2, m_cMeridian.getMeridian(), MAX_MERIDIAN);

	return 1;
}

int	KPlayer::SavePlayerItemList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);

	TRoleData * pRoleData = (TRoleData*) pRoleBuffer;
	TDBItemData * pItemData = (TDBItemData*) ((BYTE*)pRoleData +  pRoleData->dwItemOffset);

	int	nItemCount = 0;
	char szSection[100];
	char szKey[100];

	int nIdx = 0;
	while(1)
	{
		nIdx = m_ItemList.m_UseIdx.GetNext(nIdx);
		if (nIdx == 0 )
			break;
		int nItemIndex = m_ItemList.m_Items[nIdx].nIdx;
		
		sprintf(szSection, "%s%d", SECTION_ITEM, nItemCount + 1);
		//*****************************************************************
		pItemData->iequipnaturecode = Item[nItemIndex].m_CommonAttrib.nItemNature;
		pItemData->iequipclasscode =  Item[nItemIndex].m_CommonAttrib.nItemGenre;
		pItemData->idetailtype =  Item[nItemIndex].m_CommonAttrib.nDetailType;
		pItemData->iparticulartype =  Item[nItemIndex].m_CommonAttrib.nParticularType;
		pItemData->ilevel =  Item[nItemIndex].m_CommonAttrib.nLevel;
		pItemData->iseries =  Item[nItemIndex].m_CommonAttrib.nSeries;
		pItemData->ilocal =  m_ItemList.m_Items[nIdx].nPlace;
		pItemData->ix =  m_ItemList.m_Items[nIdx].nX;
		pItemData->iy =  m_ItemList.m_Items[nIdx].nY;
		pItemData->iequipversion = Item[nItemIndex].GetItemParam()->nVersion;
		pItemData->irandseed = Item[nItemIndex].GetItemParam()->uRandomSeed;
		memset(pItemData->iparam, 0, sizeof(pItemData->iparam));
		memcpy(pItemData->iparam, Item[nItemIndex].GetItemParam()->nGeneratorLevel, sizeof(pItemData->iparam));
		pItemData->ilucky = 	 Item[nItemIndex].GetItemParam()->nLuck;
		pItemData->idurability = Item[nItemIndex].GetDurability();
		pItemData->istacknum = Item[nItemIndex].GetStackNum();
		pItemData->igoldid = Item[nItemIndex].GetGoldId();
		pItemData->ienchance = Item[nItemIndex].GetEnChance();
		pItemData->ipoint = Item[nItemIndex].IsPurple();
		pItemData->iyear = Item[nItemIndex].GetTime()->bYear;
		pItemData->imonth = Item[nItemIndex].GetTime()->bMonth;
		pItemData->iday = Item[nItemIndex].GetTime()->bDay;
		pItemData->ihour = Item[nItemIndex].GetTime()->bHour;
		pItemData->ilockbh = Item[nItemIndex].GetPlayerItemLock();
		pItemData->igiomokhoa = Item[nItemIndex].GetPlayerItemHLock();
		pItemData->iiduphong1 = Item[nItemIndex].GetMantle(); //#phi phong
		pItemData->iiduphong2 = Item[nItemIndex].GetParam(); //param s?l莕 s?d鬾g item
		pItemData->iiduphong3 = Item[nItemIndex].GetItemGlowLight(); //thay i ngo筰 trang m? 竜, v?kh?ph竧 s竛g
		pItemData->iiduphong4 = Item[nItemIndex].GetMaxOptMultiply();
		pItemData->iiduphong9 = Item[nItemIndex].m_CommonAttrib.uPrice; //Luu gia bay ban cua Item
		// [PHI PHONG 2026-08-29] ghi cap sao / ma da / cap lo / chuc phuc xuong 4 o du phong
		pItemData->iiduphong5 = Item[nItemIndex].GetPfPack(0);
		pItemData->iiduphong6 = Item[nItemIndex].GetPfPack(1);
		pItemData->iiduphong7 = Item[nItemIndex].GetPfPack(2);
		pItemData->iiduphong8 = Item[nItemIndex].GetPfPack(3);
		pItemData->imantle = Item[nItemIndex].GetMantle();
		pItemData->irow = Item[nItemIndex].GetRow();
		// [DUNGLUYEN 01/09] 6 o Van Cuong + 6 seed dong goi vao 9 truong TDBItemData KHONG AI DOC LAI khi nap
		// (iid, iequipcode, iidentify, iBaiTanPrice, ilocksell, ilocktrade, ilockdrop, ifortune, iowner - da do
		// 49.332 mon: co khoa/fortune/owner deu sinh lai tu bang, iid/iBaiTanPrice mang rac) -> ban ghi GIU 233
		// byte, schema MySQL khong doi. Dau hieu hop le 'FUS1' o iidentify (moi ban ghi cu ghi 0).
		// PHAI ghi tuong minh ca 9 o moi lan save: m_SaveBuffer chi ZeroMemory luc dang nhap.
		{
			const KItem& sIt = Item[nItemIndex];
			BOOL bCo = (sIt.GetGenre() == item_fusion);
			int k;
			for (k = 0; k < KItem::FUS_MAX_SLOT; k++)
				if (sIt.GetFusionP(k) || sIt.GetFusionSeed(k))
					bCo = TRUE;
			if (bCo)
			{
				pItemData->iidentify = 0x46555331;	// 'FUS1'
				pItemData->iid = (sIt.GetFusionP(0) & 0x3FF) | ((sIt.GetFusionP(1) & 0x3FF) << 10) | ((sIt.GetFusionP(2) & 0x3FF) << 20);
				pItemData->iBaiTanPrice = (sIt.GetFusionP(3) & 0x3FF) | ((sIt.GetFusionP(4) & 0x3FF) << 10) | ((sIt.GetFusionP(5) & 0x3FF) << 20);
				pItemData->iequipcode = (int)sIt.GetFusionSeed(0);
				pItemData->ilocksell  = (int)sIt.GetFusionSeed(1);
				pItemData->ilocktrade = (int)sIt.GetFusionSeed(2);
				pItemData->ilockdrop  = (int)sIt.GetFusionSeed(3);
				pItemData->ifortune   = (int)sIt.GetFusionSeed(4);
				pItemData->iowner     = (DWORD)sIt.GetFusionSeed(5);
			}
			else
			{
				pItemData->iidentify = 0;
				pItemData->iid = 0;
				pItemData->iBaiTanPrice = 0;
				pItemData->iequipcode = 0;
				pItemData->ilocksell = 0;
				pItemData->ilocktrade = 0;
				pItemData->ilockdrop = 0;
				pItemData->ifortune = 0;
				pItemData->iowner = 0;
			}
		}
		//*****************************************************************************
		pItemData++;
		nItemCount ++;
	}
	
	pRoleData->nItemCount = nItemCount;
	pRoleData->dwDataLen = (BYTE*)pItemData - (BYTE*)pRoleBuffer;
	return 1;
}

int	KPlayer::SavePlayerStateSkillList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) 
		return FALSE;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	TDBSkillData * pStateData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwSSkillOffset);
	int nCount = Npc[m_nIndex].UpdateDBStateList((BYTE *)pStateData);
	if (nCount > 0)
	{
		pRoleData->nStateSkillCount = nCount;
		pRoleData->dwTaskOffset = (BYTE*)pStateData - pRoleBuffer + sizeof(TDBSkillData) * nCount;
	}
	else
	{
		pRoleData->nStateSkillCount  = 0;
		pRoleData->dwTaskOffset = (BYTE*)pStateData - pRoleBuffer;
	}
	return 1;
}

int	KPlayer::SavePlayerFightSkillList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	if (m_nIndex <= 0) 
		return FALSE;
	TRoleData * pRoleData = (TRoleData *)pRoleBuffer;
	TDBSkillData * pSkillData = (TDBSkillData *) (pRoleBuffer + pRoleData->dwFSkillOffset);
	int nCount = Npc[m_nIndex].m_SkillList.UpdateDBSkillList((BYTE *)pSkillData);
	if (nCount > 0)
	{
		pRoleData->nFightSkillCount = nCount;
		pRoleData->dwSSkillOffset = (BYTE*)pSkillData - pRoleBuffer + sizeof(TDBSkillData) * nCount;
	}
	else
	{
		pRoleData->nFightSkillCount  = 0;
		pRoleData->dwSSkillOffset = (BYTE*)pSkillData - pRoleBuffer;
	}
	return 1;
}

int	KPlayer::SavePlayerTaskList(BYTE * pRoleBuffer)
{
	_ASSERT(pRoleBuffer);
	TRoleData * pRoleData = (TRoleData *) pRoleBuffer;
	int nTaskCount = 0;
	int nTaskId = 0;
	int nTaskDegee = 0;
	char szTaskIDKey[100];
	char szTaskValueKey[100];

	KIndexNode * pNode = (KIndexNode*)m_PlayerStationList.GetHead();
	int n = 0;
	memset(&m_cTask.nSave[TASKVALUE_SAVESTATION_BEGIN], 0, (TASKVALUE_MAXSTATION_COUNT / 2) * sizeof(int));
	memset(&m_cTask.nSave[TASKVALUE_SAVEWAYPOINT_BEGIN], 0, TASKVALUE_MAXWAYPOINT_COUNT * sizeof(int));

	while(pNode)
	{
		if (n >= TASKVALUE_MAXSTATION_COUNT) break;
		DWORD dwValue = m_cTask.nSave[TASKVALUE_SAVESTATION_BEGIN + n / 2];
		DWORD OrValue = pNode->m_nIndex ;
		OrValue = OrValue << ((n % 2) * 16);
		dwValue = dwValue | OrValue;
		m_cTask.nSave[TASKVALUE_SAVESTATION_BEGIN + n / 2] = dwValue;
		n++;
		pNode = (KIndexNode*)pNode->GetNext();
	}
	
	n = 0;
	pNode = (KIndexNode*) m_PlayerWayPointList.GetHead();
	while(pNode)
	{
		if (n >= TASKVALUE_MAXWAYPOINT_COUNT) break;
		// [24/08] VA LOI TREO CUNG MAY CHU: 'continue' o day KHONG dich con tro,
		// nen chi can MOT nut co m_nIndex == 0 la vong lap chay VO HAN - ngay trong
		// duong luu nhan vat, tren luong chinh => GameServer dung im, khong sap,
		// khong ghi log gi. Vong tram xe ngay ben tren khong co nhanh continue nay
		// nen khong dinh loi.
		if (pNode->m_nIndex == 0)
		{
			pNode = (KIndexNode*)pNode->GetNext();
			continue;
		}
		m_cTask.nSave[TASKVALUE_SAVEWAYPOINT_BEGIN + n] = pNode->m_nIndex;
		n++;
		pNode = (KIndexNode*)pNode->GetNext();
	}
	//
	g_TimerTask.SaveTask(this);
	//
	TDBTaskData * pTaskData = (TDBTaskData *)(pRoleBuffer + pRoleData->dwTaskOffset);
	for (int i = 0; i < MAX_TASK; i ++)
	{
		if (!m_cTask.nSave[i]) continue;
		
		pTaskData->m_nTaskId = i;
		pTaskData->m_nTaskValue = m_cTask.GetSaveVal(i);
		nTaskCount ++;
		pTaskData ++;
	}
	//
	// [24/08] Xem chu thich day du o duong NAP (tim "VA TRAN SO NHIEM VU").
	// Van ghi truong BYTE de binary CU con doc duoc ban ghi moi, nhung BAO HOA 255
	// thay vi cat modulo: cat modulo bien dung 256 task thanh 0, ma duong nap thay
	// 0 la return ngay => mat sach nhiem vu VA bo con tro dung dau vung task, khien
	// buoc vat pham doc do tu giua du lieu task. Bao hoa 255 nghiem ngat khong te hon.
	// Nguon su that khi nap la dwItemOffset o dong ke duoi, KHONG phai truong nay.
	pRoleData->nTaskCount = (BYTE)(nTaskCount > 255 ? 255 : nTaskCount);
	pRoleData->dwItemOffset = (BYTE *)pTaskData - pRoleBuffer;
	return 1;
}
#endif

