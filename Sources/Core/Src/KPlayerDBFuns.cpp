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
		break;
	case STEP_ITEM_LIST:
		//*************************************************
		// Íæ¼ÒµÄ×°±¸ÁÐ±í
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
	g_SubWorldSet.GetRevivalPosFromId(m_sLoginRevivalPos.m_nSubWorldID, m_sLoginRevivalPos.m_ReviveID, &Pos);
	m_sLoginRevivalPos.m_nMpsX = Pos.x;
	m_sLoginRevivalPos.m_nMpsY = Pos.y;
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

	/*if(g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID) == -1 //neu player vao map khong co trong worldset thi load subworld dau tien
		|| (pRoleData->BaseInfo.cUseRevive && (tempPos.m_nMpsX == 0 && tempPos.m_nMpsY ==0)) //neu player vao map pos x y = 0 load ®Çu tiªn
		) 
	{
		g_SubWorldSet.GetRevivalPosFromId(SubWorld[0].m_SubWorldID,1,&Pos);
		tempPos.m_nSubWorldID = SubWorld[0].m_SubWorldID;
		tempPos.m_nMpsX = Pos.x;
		tempPos.m_nMpsY = Pos.y;
	}*/

	int nSeries = pRoleData->BaseInfo.ifiveprop;
	m_nIndex = NpcSet.AddNpcSet2(nSex, nSeries, g_SubWorldSet.SearchWorld(tempPos.m_nSubWorldID), tempPos.m_nMpsX, tempPos.m_nMpsY);
	if(m_nIndex <= 0) 
	{
		printf("[Error!]AddNpc Error DBFuns.cpp");
		if (pRoleData->BaseInfo.cUseRevive)
		{
			return -1;
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
	m_nWorldStat	= pRoleData->BaseInfo.nWorldStat;	//xÕp h¹ng thÕ giíi
	m_nSectStat		= pRoleData->BaseInfo.nSectStat;
	m_nChestPW		= pRoleData->BaseInfo.ipassrole; 
	SetChestPW(m_nChestPW);
	int nCashMoney = 0;
	int nSaveMoney = 0;
//	this->m_ItemList.Init(GetPlayerIndex());
	nCashMoney		= pRoleData->BaseInfo.imoney;
	nSaveMoney		= pRoleData->BaseInfo.isavemoney;
	m_ItemList.SetMoney(nCashMoney, nSaveMoney,0);

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
	pNpc->m_CurrentMana = pRoleData->BaseInfo.icurinner;
	pNpc->m_CurrentStamina = pRoleData->BaseInfo.icurstamina;

	m_cPK.SetNormalPKState(pRoleData->BaseInfo.cPkStatus, pRoleData->BaseInfo.ipduphong2); //#PKState
	m_cPK.SetPKValue(pRoleData->BaseInfo.ipkvalue);
	m_cRepute.SetReputeValue(pRoleData->BaseInfo.ireputevalue);
	m_cFuYuan.SetFuYuanValue(pRoleData->BaseInfo.ifuyuanvalue);
	m_cReBorn.SetReBornValue(pRoleData->BaseInfo.irebornvalue);

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
		if(strcmp(szMapTypeTmp,"City") == 0 || strcmp(szMapTypeTmp,"Capital") == 0 || strcmp(szMapTypeTmp,"Country") == 0) //thµnh thÞ // thñ ®« // th«n lµng add by tuanln
		{
			pNpc->m_FightMode = 0;
		}
		else if(strcmp(szMapTypeTmp,"Field") == 0 || strcmp(szMapTypeTmp,"Cave") == 0)
		{
			pNpc->m_FightMode = 1;
		}
	}
	return 1;
}

int	KPlayer::LoadPlayerItemList(BYTE * pRoleBuffer , BYTE* &pItemBuffer, unsigned int &nParam)
{
	KASSERT(pRoleBuffer);
	
	int nItemCount = ((TRoleData *)pRoleBuffer)->nItemCount;
	TDBItemData * pItemData = (TDBItemData *)pItemBuffer;
	
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

		//**************************************************************
		//NewItem.SetID(dwItemID);
		NewItem.m_GeneratorParam.nGeneratorLevel[0] = pItemData->iparam1;
		NewItem.m_GeneratorParam.nGeneratorLevel[1] = pItemData->iparam2;
		NewItem.m_GeneratorParam.nGeneratorLevel[2] = pItemData->iparam3;
		NewItem.m_GeneratorParam.nGeneratorLevel[3] = pItemData->iparam4;
		NewItem.m_GeneratorParam.nGeneratorLevel[4] = pItemData->iparam5;
		NewItem.m_GeneratorParam.nGeneratorLevel[5] = pItemData->iparam6;
		
		NewItem.m_GeneratorParam.nVersion			= pItemData->iequipversion;
		NewItem.m_GeneratorParam.uRandomSeed		= pItemData->irandseed;
		NewItem.m_GeneratorParam.nLuck				= pItemData->ilucky;

		NewItem.m_CommonAttrib.nGoldId				= pItemData->igoldid;

		NewItem.SetPlayerItemLock(pItemData->ilockbh);
		NewItem.SetPlayerItemHLock(pItemData->igiomokhoa);

		NewItem.SetMantle(pItemData->iiduphong1);//#phi phong
		BOOL bGetEquiptResult = 0;

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
			if (!pItemData->igoldid)
				bGetEquiptResult = ItemGen.Gen_ExistEquipment(
					NewItem.m_CommonAttrib.nDetailType, 
					NewItem.m_CommonAttrib.nParticularType,
					NewItem.m_CommonAttrib.nSeries,
					NewItem.m_CommonAttrib.nLevel, 
					NewItem.m_GeneratorParam.nGeneratorLevel, 
					NewItem.m_GeneratorParam.nLuck,
					NewItem.m_GeneratorParam.nVersion,
					&NewItem,
					NewItem.m_CommonAttrib.nEnChance,
					NewItem.m_CommonAttrib.nPoint);
			else
			bGetEquiptResult = ItemGen.GetGoldItemByIndex(NewItem.m_CommonAttrib.nGoldId,
				&NewItem,
				NewItem.m_GeneratorParam.nGeneratorLevel,
				NewItem.m_CommonAttrib.nSeries,
				NewItem.m_CommonAttrib.nEnChance);
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
		}

		//if (pItemData->idurability != 0)
		NewItem.SetDurability(pItemData->idurability); //#do ben
		NewItem.SetParam(pItemData->iiduphong2);//sè lÇn sö dông item
		NewItem.SetItemGlowLight(pItemData->iiduphong3); //ngo¹i trang item ph¸t s¸ng
		NewItem.SetExpTime(pItemData->iyear,pItemData->imonth,pItemData->iday,pItemData->ihour);

		NewItem.m_CommonAttrib.uPrice = pItemData->iiduphong9; //Load gia bay ban

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
		m_cTask.Release();
	//
	int nTaskCount = 0;
	int nTaskId = 0;
	int nTaskDegee = 0;
	char szTaskIDKey[100];
	char szTaskValueKey[100];
	nTaskCount = ((TRoleData* )pRoleBuffer)->nTaskCount;
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
	TDBTaskData * pTaskData = (TDBTaskData*) pTaskBuffer;
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
	//Íæ¼ÒÐÅÏ¢
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

	//Íæ¼ÒµÄÏÔÊ¾ÐÅÏ¢	-- remark by spe because client ui display changed 2003/07/21
//	pRoleData->BaseInfo.ihelmres = pNpc->m_HelmType;
//	pRoleData->BaseInfo.iarmorres = pNpc->m_ArmorType;
//	pRoleData->BaseInfo.iweaponres = pNpc->m_WeaponType;
	
	//Íæ¼ÒµÈ¼¶ÐÅÏ¢
	pRoleData->BaseInfo.fightexp = m_nExp;
	pRoleData->BaseInfo.ifightlevel = pNpc->m_Level;
	
	pRoleData->BaseInfo.ileadlevel = m_dwLeadLevel;
	pRoleData->BaseInfo.ileadexp =	m_dwLeadExp;

	//ÃÅÅÉÐÅÏ¢
	pRoleData->BaseInfo.nSect =		m_cFaction.m_nCurFaction;
	pRoleData->BaseInfo.nFirstSect = 	m_cFaction.m_nFirstAddFaction;
	pRoleData->BaseInfo.ijoincount = m_cFaction.m_nAddTimes;
	pRoleData->BaseInfo.isectrole = pNpc->m_btRankId;
	pRoleData->BaseInfo.iexitemrole = pNpc->m_ExItemId; // hang trang
	pRoleData->BaseInfo.iexboxrole = pNpc->m_ExBoxId; // ruong mo rong
	pRoleData->BaseInfo.nWorldStat	= m_nWorldStat;	//xÕp h¹ng thÕ giíi
	pRoleData->BaseInfo.nSectStat	= m_nSectStat;
	pRoleData->BaseInfo.ipassrole =  m_nChestPW;
	
	pRoleData->BaseInfo.ipduphong1 = m_ImagePlayer; //#avatar player
	strcpy(pRoleData->BaseInfo.szStringduphong1,m_PlayerMateName); //#MateName

	int nCashMoney = 0;
	int nSaveMoney = 0;
	
	nCashMoney  = m_ItemList.GetMoney(room_equipment);
	nSaveMoney	= m_ItemList.GetMoney(room_repository);
	pRoleData->BaseInfo.imoney			= nCashMoney;
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

	if (m_bExchangeServer)	// chuyÓn gs
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

	//PKÏà¹Ø
	pRoleData->BaseInfo.cPkStatus = (BYTE)m_cPK.GetNormalPKState();
	pRoleData->BaseInfo.ipkvalue = m_cPK.GetPKValue();
	//
	pRoleData->BaseInfo.ipduphong2 = (BOOL)m_cPK.m_bLockPK; //#PKState
	pRoleData->BaseInfo.ireputevalue = m_cRepute.GetReputeValue();//Repute
	pRoleData->BaseInfo.ifuyuanvalue = m_cFuYuan.GetFuYuanValue();//Fuyuan
	pRoleData->BaseInfo.irebornvalue = m_cReBorn.GetReBornValue();// Reborn
	//
	pRoleData->dwFSkillOffset = (BYTE * )pRoleData->pBuffer - (BYTE *)pRoleBuffer;
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
		pItemData->iequipclasscode =  Item[nItemIndex].m_CommonAttrib.nItemGenre;
		pItemData->iequipcode =  Item[nItemIndex].GetID();
		pItemData->idetailtype =  Item[nItemIndex].m_CommonAttrib.nDetailType;
		pItemData->iparticulartype =  Item[nItemIndex].m_CommonAttrib.nParticularType;
		pItemData->ilevel =  Item[nItemIndex].m_CommonAttrib.nLevel;
		pItemData->iseries =  Item[nItemIndex].m_CommonAttrib.nSeries;
		pItemData->ilocal =  m_ItemList.m_Items[nIdx].nPlace;
		pItemData->ix =  m_ItemList.m_Items[nIdx].nX;
		pItemData->iy =  m_ItemList.m_Items[nIdx].nY;
		pItemData->iequipversion = Item[nItemIndex].GetItemParam()->nVersion;
		pItemData->irandseed = Item[nItemIndex].GetItemParam()->uRandomSeed;
		pItemData->iparam1 =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[0];
		pItemData->iparam2 =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[1];
		pItemData->iparam3 =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[2];
		pItemData->iparam4 =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[3];
		pItemData->iparam5 =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[4];
		pItemData->iparam6 =  Item[nItemIndex].GetItemParam()->nGeneratorLevel[5];
		pItemData->ilucky = 	 Item[nItemIndex].GetItemParam()->nLuck;
		pItemData->idurability = Item[nItemIndex].GetDurability();
		pItemData->istacknum = Item[nItemIndex].GetStackNum();
		pItemData->iidentify = 0;
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
		pItemData->iiduphong2 = Item[nItemIndex].GetParam(); //param sè lÇn sö dông item
		pItemData->iiduphong3 = Item[nItemIndex].GetItemGlowLight(); //thay ®æi ngo¹i trang mò, ¸o, vò khÝ ph¸t s¸ng
		pItemData->iiduphong9 = Item[nItemIndex].m_CommonAttrib.uPrice; //Luu gia bay ban cua Item
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
		if (pNode->m_nIndex == 0) continue;
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
	pRoleData->nTaskCount = nTaskCount;
	pRoleData->dwItemOffset = (BYTE *)pTaskData - pRoleBuffer;
	return 1;
}
#endif

