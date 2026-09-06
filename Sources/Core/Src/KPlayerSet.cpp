//#include <objbase.h>
//#include <crtdbg.h>
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "GameDataDef.h"
#include "KProtocolProcess.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#include "KSubWorldSet.h"
#include "CoreServerShell.h"
#endif
#include "KPlayerSet.h"
#include "Text.h"
#include "CoreUseNameDef.h"
#include "KBiaoChe.h"	// [LMBC 06/09] xe tieu Long Mon

#define		PLAYER_FIRST_LUCKY				0

KPlayerSet PlayerSet;

KPlayerSet::KPlayerSet()
{
#ifdef _SERVER
	m_pWelcomeMsg = NULL;
	m_ulNextSaveTime = 0;
	m_ulDelayTimePerSave = 0;
	m_ulMaxSaveTimePerPlayer = 0;
	m_bStopGameServer = FALSE;
#endif
#ifndef _SERVER
//	memset(m_szNationalEmblemPic, 0, sizeof(m_szNationalEmblemPic));
	memset(m_szFortuneRankPic, 0, sizeof(m_szFortuneRankPic));
//	memset(m_szViprankPic, 0, sizeof(m_szViprankPic));
//	memset(m_szTranlifePic, 0, sizeof(m_szTranlifePic));
#endif
}

BOOL	KPlayerSet::Init()
{
	int i;
	
#ifdef _SERVER
	m_nNumPlayer = 0;
	m_nPlayerNumMax = 0;
	m_ulNextSaveTime = 0;
	//m_ulMaxSaveTimePerPlayer = 60 * 20 * 30;	//(60 * 20 * 3) / 6; //edit by phong kieu thoi gian auto Save nhan vat
	m_ulMaxSaveTimePerPlayer = 30 * 18;// * 30
	m_ulDelayTimePerSave = m_ulMaxSaveTimePerPlayer / MAX_PLAYER;
#endif

	// ?????
	m_FreeIdx.Init(MAX_PLAYER);
	m_UseIdx.Init(MAX_PLAYER);

	// ?????????????
	for (i = MAX_PLAYER - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
	}

	if ( !m_cLevelAdd.Init() )
		return FALSE;
	if ( !m_cMagicLevelExp.Init() )
		return FALSE;
	if ( !m_cLeadExp.Init() )
		return FALSE;
	for (i = 0; i < MAX_PLAYER; i++)
	{
		Player[i].Release();
		Player[i].SetPlayerIndex(i);
#ifdef _SERVER
		Player[i].m_cPK.Init(i);
		Player[i].m_cRepute.Init(i);
		Player[i].m_cFuYuan.Init(i);
		Player[i].m_cReBorn.Init(i);
		Player[i].m_cTask.Init(i); // task
		Player[i].m_cMeridian.Init(i);
		Player[i].m_cPartner.Init(i);	// [BDH 27/08]
#endif
		Player[i].m_cTong.Init(i);
		Player[i].m_ItemList.Init(i);
		Player[i].m_Node.m_nIndex = i;
	}

	if ( !m_cNewPlayerAttribute.Init() )
		return FALSE;
	if ( !m_cPlayerStamina.Init())
		return FALSE;
#ifdef _SERVER

	KTabFile	cPKParam;

	memset(m_sPKPunishParam, 0, sizeof(m_sPKPunishParam));
//	g_SetFilePath("\\");
	if (!cPKParam.Load(defPK_PUNISH_FILE))
		return FALSE;
	for (i = 0; i < MAX_DEATH_PUNISH_PK_VALUE + 1; i++)
	{
		cPKParam.GetInteger(i + 2, 2, 1, &m_sPKPunishParam[i].m_nExpP);
		cPKParam.GetInteger(i + 2, 3, 1, &m_sPKPunishParam[i].m_nExpV);
		cPKParam.GetInteger(i + 2, 4, 1, &m_sPKPunishParam[i].m_nMoney);
		cPKParam.GetInteger(i + 2, 5, 1, &m_sPKPunishParam[i].m_nItem);
		cPKParam.GetInteger(i + 2, 6, 1, &m_sPKPunishParam[i].m_nEquip);
		cPKParam.GetInteger(i + 2, 7, 1, &m_sPKPunishParam[i].m_nAbradeP);
	}
#endif
#ifndef _SERVER
	char Buff[MAX_PATH];
	for (i=1; i<=20;i++)
	{
	//	if(i<MAX_TONG_NATIONALEMBLEM)
	//	{
	//		sprintf(Buff, "Spr_%d", i);
	//		g_GameSetting.GetString("NationalEmblem", Buff, "", m_szNationalEmblemPic[i], sizeof(m_szNationalEmblemPic[i]));	
	//	}
		if(i<(MAX_ITEM_LEVELFF+1))
		{
			sprintf(Buff, "Spr_%d", i);
			g_GameSetting.GetString("FortuneRank", Buff, "", m_szFortuneRankPic[i], sizeof(m_szFortuneRankPic[i]));		
		}
	//	if(i<(MAX_TRANSLIFE_VALUE+1))
	//	{
	//		itoa(i, Buff, 10);
	//		g_GameSetting.GetString("TransLife", Buff, "", m_szTranlifePic[i], sizeof(m_szTranlifePic[i]));
	//	}
	//	if(i<(MAX_VIPRANK_VALUE+1))
	//	{
	//		sprintf(Buff, "Spr_%d", i);
	//		g_GameSetting.GetString("VipRank", Buff, "", m_szViprankPic[i], sizeof(m_szViprankPic[i]));
	//	}
	}
#endif
	// ????
	KIniFile	cTongFile;
	if (cTongFile.Load(defPLAYER_TONG_PARAM_FILE))
	{
		cTongFile.GetInteger("TongCreate", "Level", 60, &m_sTongParam.m_nLevel);
		cTongFile.GetInteger("TongCreate", "LeadLevel", 10, &m_sTongParam.m_nLeadLevel);
		cTongFile.GetInteger("TongCreate", "Money", 500000, &m_sTongParam.m_nMoney);
		cTongFile.GetInteger("TongCreate", "ItemReq", 195, &m_sTongParam.m_nItemReq);			//item quest nhac vuong kiem
		cTongFile.GetInteger("TongLeave", "LeaveMoney", 10000, &m_sTongParam.m_nLeaveMoney);		// roi bang
		cTongFile.GetInteger("TongChange", "MoneyChangeCamp", 100000, &m_sTongParam.m_nMoneyChangeCamp);
	}
	else
	{
		m_sTongParam.m_nLevel		= 60;
		m_sTongParam.m_nLeadLevel	= 10;
		m_sTongParam.m_nMoney		= 500000;
		m_sTongParam.m_nItemReq		= 195;
		m_sTongParam.m_nLeaveMoney	= 10000;
		m_sTongParam.m_nMoneyChangeCamp		= 100000;
	}

	return TRUE;
}

int	KPlayerSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

int KPlayerSet::FindSame(DWORD dwID)
{
	int nUseIdx = 0;

	nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}

int		KPlayerSet::GetFirstPlayer()
{
	m_nListCurIdx = m_UseIdx.GetNext(0);
	return m_nListCurIdx;
}

int		KPlayerSet::GetNextPlayer()
{
	if ( !m_nListCurIdx )
		return 0;
	m_nListCurIdx = m_UseIdx.GetNext(m_nListCurIdx);
	return m_nListCurIdx;
}

#ifdef _SERVER
int KPlayerSet::Add(LPSTR szPlayerID, void* pGuid)
{
	if (!pGuid || !szPlayerID || !szPlayerID[0])
		return 0;

	int i;
/*
	DWORD dwID = g_FileName2Id(szPlayerID);
	i = FindSame(dwID);
	if (i)
		return 0;
*/
	DWORD dwID = g_FileName2Id(szPlayerID);

	i = FindFree();

	if (i)
	{
		Player[i].Release();
		Player[i].m_dwID = dwID;
		Player[i].m_nNetConnectIdx = -1;
		Player[i].m_dwLoginTime = g_SubWorldSet.GetGameTime();
		memcpy(&Player[i].m_Guid, pGuid, sizeof(GUID));
		Player[i].SetPlayerIndex(i);
		m_FreeIdx.Remove(i);
		m_UseIdx.Insert(i);
		m_nNumPlayer ++;


		if (i > m_nPlayerNumMax)
		{
		m_nPlayerNumMax = i;
		}

		return i;
	}
	return 0;
}

int	KPlayerSet::Broadcasting(char* pMessage, int nLen)
{
	if ( !pMessage || nLen <= 0 || nLen >= MAX_SENTENCE_LENGTH)
		return 0;
	if (!g_pServer)
		return 0;
	g_pServer->PreparePackSink();
	KPlayerChat::SendSystemInfo(0, 0, MESSAGE_SYSTEM_ANNOUCE_HEAD, pMessage, nLen);
	g_pServer->SendPackToClient(-1);
	return 1;
}

void KPlayerSet::SendWelcomeMsg(int nPlayerIndex)
{
	//======== //edit by phong kieu tieng tau chua viet hoa
	char	szBuffer[MAX_SENTENCE_LENGTH];
	int		nLength;
	static int nMsg = 0;
	const char* Msgs[3] =
	{	"Xin ch? 1 <color=green> %s <color=restore> VLTK 1",
		"Xin ch? 3 <color=green> %s <color=restore> VLTK 2",
		"Xin ch? 2 <color=green> %s <color=restore> VLTK 3" 
	};

	memset(szBuffer, 0, sizeof(szBuffer));
	sprintf(szBuffer, Msgs[nMsg], Player[nPlayerIndex].m_PlayerName);
	nMsg++;
	nMsg %= 3;
	nLength = TEncodeText(szBuffer, strlen(szBuffer));
	KPlayerChat::SendSystemInfo(0, 0, MESSAGE_SYSTEM_ANNOUCE_HEAD, szBuffer, nLength);

	//====
	if (m_pWelcomeMsg)
	{
		int	*pAMsg = (int*)m_pWelcomeMsg;
		int nCount =  *pAMsg;
		pAMsg ++;
		for (int i = 0; i < nCount; i++)
		{
			int	nLen = *pAMsg;
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, "Ch? m?g", (((char*)pAMsg) + sizeof(int)), nLen);//tieng tau chua viet hoa
			pAMsg = (int*)(((char*)pAMsg) + nLen + sizeof(int));
		}
	}
}

void KPlayerSet::ReloadWelcomeMsg()
{
	return;
	/*    ????????????:
	????????  Msg:????	
	?????????,???????????????MAX_SENTENCE_LENGTH
	*/

	if (m_pWelcomeMsg)
	{
		free (m_pWelcomeMsg);
		m_pWelcomeMsg = NULL;
	}
	KFile		File;
	char		Head[8];
	if (File.Open("\\Msg\\WelcomeMsg.txt") == FALSE)
		return;

	int nLen = File.Size();
	m_pWelcomeMsg = malloc(nLen + sizeof(int));
	if (m_pWelcomeMsg == NULL)
		return;

	*(int*)m_pWelcomeMsg = 0;
	char* pSearchBegin = (char*)m_pWelcomeMsg + sizeof(int);
	int* pLineHeader = (int*)pSearchBegin;
	File.Read(pLineHeader, nLen);
	File.Close();

	char* pMsgHeader;
	while(pMsgHeader = (char*)memchr(pSearchBegin, ':', nLen))
	{
		pMsgHeader++;	//??????
		nLen -= pMsgHeader -pSearchBegin; //????????
		if (nLen <= 0)
			break;
		int	nSkipLen = pMsgHeader - (char*)pLineHeader - sizeof(int);
		pSearchBegin = pMsgHeader;
		if (nSkipLen < 0)
			continue;
		if (nSkipLen > 0)
			memset(&pLineHeader[1], 0, nSkipLen);
		char* pMsgTail = (char*)memchr(pSearchBegin, 0x0d, nLen);	//?????
		int	nMsgLen;
		if (pMsgTail)
		{
			*pMsgTail = 0;
			pMsgTail++;
			if ((pMsgTail < pSearchBegin + nLen) && *pMsgTail == 0x0a)
			{
				*pMsgTail = 0;
				pMsgTail++;
			}
			nMsgLen = pMsgTail - pSearchBegin;
			pSearchBegin = pMsgTail;
		}
		else
		{
			nMsgLen = nLen;
		}
		nLen -= nMsgLen;
		nMsgLen = TEncodeText((char*)&pLineHeader[1], nMsgLen + nSkipLen);		
		if (nMsgLen > 0)
		{
			*pLineHeader = nMsgLen;
			pLineHeader = (int*)(((char*)pLineHeader) + sizeof(int) + (*pLineHeader));
			(*(int*)m_pWelcomeMsg) ++;
		}
		if (pMsgTail == NULL || nLen <= 0)
			break;
	}
	if ((int*)m_pWelcomeMsg == 0)
	{
		free(m_pWelcomeMsg);
		m_pWelcomeMsg = NULL;
	}
}

void KPlayerSet::PrepareRemove(int nIndex)//#khi player dang xuat
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	g_DebugLog("SERVER:Player[%s] has been removed!", Npc[Player[nIndex].m_nIndex].Name);

	Player[nIndex].m_cChat.OffLine(Player[nIndex].m_dwID);

	BC_OnPlayerLogout(nIndex);	// [LMBC 06/09] xe tieu: bat dau dem 5 phut cho chu vao lai

	Player[nIndex].ExecuteScript("\\script\\player\\playerlogout.lua", "main", 0);
	if (Player[nIndex].m_dwLogoutScriptID)
		Player[nIndex].ExecuteScript(Player[nIndex].m_dwLogoutScriptID, "OnLogout", "");

	int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
	if (nSubWorld >= 0)
	{
		// [WLLS 20/08] dang xuat = roi map: ban OnLeaveWorld truoc khi go mission
		// (schedule\newworld.lua can gui wlls_onleave de tra cho bao danh)
		KSubWorld_FireMapScript(nSubWorld, "OnLeaveWorld", nIndex);
		SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex, Player[nIndex].m_dwID);
	}

	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	Player[nIndex].LeaveTeam((BYTE*)&sLeaveTeam);

	if (Player[nIndex].m_cMenuState.m_nState != PLAYER_MENU_STATE_GAMBLING) {
		TRADE_DECISION_COMMAND	sTrade;
		sTrade.ProtocolType = c2s_tradedecision;
		sTrade.m_btDecision = 0;
		Player[nIndex].TradeDecision((BYTE*)&sTrade);
	}

	GAMBLE_DECISION_COMMAND	sGamble;
	sGamble.ProtocolType = c2s_gambledecision;
	sGamble.m_btDecision = 0;
	Player[nIndex].GambleDecision((BYTE*)&sGamble);

	Player[nIndex].m_cPK.CloseAll();

//	Player[nIndex].Save();

	Player[nIndex].WaitForRemove();
}

void KPlayerSet::PrepareLoginFailed(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	g_DebugLog("SERVER:Player[%s] has been removed!", Npc[Player[nIndex].m_nIndex].Name);

	Player[nIndex].m_cChat.OffLine(Player[nIndex].m_dwID);

	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	Player[nIndex].LeaveTeam((BYTE*)&sLeaveTeam);

	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	Player[nIndex].TradeDecision((BYTE*)&sTrade);

	Player[nIndex].m_cPK.CloseAll();

	Player[nIndex].m_cTong.Clear();

	Player[nIndex].m_nNetConnectIdx = -1;
	Player[nIndex].m_dwLoginTime = 0;
}

void KPlayerSet::PrepareExchange(int i)
{
	if (!Player[i].m_bExchangeServer || !Npc[Player[i].m_nIndex].m_bExchangeServer)
		return;

	Player[i].m_cPK.CloseAll();

	PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
	sLeaveTeam.ProtocolType = c2s_teamapplyleave;
	Player[i].LeaveTeam((BYTE*)&sLeaveTeam);

	TRADE_DECISION_COMMAND	sTrade;
	sTrade.ProtocolType = c2s_tradedecision;
	sTrade.m_btDecision = 0;
	Player[i].TradeDecision((BYTE*)&sTrade);

	Player[i].Save();
//	Player[i].UpdateEnterGamePos(Player[i].m_sExchangePos.m_dwMapID,
//		Player[i].m_sExchangePos.m_nX,
//		Player[i].m_sExchangePos.m_nY,
//		Npc[Player[i].m_nIndex].m_FightMode);
}

void KPlayerSet::RemoveQuiting(int nIndex)
{
	if (!Player[nIndex].m_bForeQuit && (Player[nIndex].m_nNetConnectIdx == -1 || Player[nIndex].m_dwID == 0))
		return;

	Player[nIndex].m_bForeQuit = FALSE;
	if (Player[nIndex].IsWaitingRemove())
	{
		if (Player[nIndex].m_nIndex > 0)	// have npc
		{
			int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
			
			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
				//SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex, Player[nIndex].m_dwID);
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, obj_npc);
			}	
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}

		Player[nIndex].m_cTong.Clear();

		Player[nIndex].m_ItemList.RemoveAll();
		Npc[Player[nIndex].m_nIndex].ClearNpcState();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		Player[nIndex].Release();
		
		m_FreeIdx.Insert(nIndex);
		m_UseIdx.Remove(nIndex);
		m_nNumPlayer --;
	}
}	

void KPlayerSet::RemoveLoginTimeOut(int nIndex)
{
	if (Player[nIndex].IsLoginTimeOut())
	{
		if (Player[nIndex].m_nIndex > 0)
		{
			int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
			int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		
/*		if (nSubWorld >= 0)
			SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);*/
		
			if (nSubWorld >= 0 && nRegion >= 0)
			{
				SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
				SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, obj_npc);
			}
			NpcSet.Remove(Player[nIndex].m_nIndex);
		}		
		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		Player[nIndex].LoginTimeOut();
		
		m_FreeIdx.Insert(nIndex);
		m_UseIdx.Remove(nIndex);
		m_nNumPlayer --;
	}
}

void KPlayerSet::RemoveExchanging(int nIndex)
{
	if (Player[nIndex].m_nNetConnectIdx == -1)
		return;

	if (Player[nIndex].IsExchangingServer())
	{
		int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
		int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		
		if (nSubWorld >= 0 && nRegion >= 0)
		{
			SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
			//SubWorld[nSubWorld].m_MissionArray.RemovePlayer(nIndex, Player[nIndex].m_dwID);
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
			SubWorld[nSubWorld].m_Region[nRegion].DecRef(Npc[Player[nIndex].m_nIndex].m_MapX, Npc[Player[nIndex].m_nIndex].m_MapY, obj_npc);
		}
		
		if (Player[nIndex].m_nIndex > 0)
			NpcSet.Remove(Player[nIndex].m_nIndex);
		
		Player[nIndex].m_ItemList.RemoveAll();
		Player[nIndex].m_dwID = 0;
		Player[nIndex].m_nIndex = 0;
		Player[nIndex].m_nNetConnectIdx = -1;
		Player[nIndex].Release();
		
		m_FreeIdx.Insert(nIndex);
		m_UseIdx.Remove(nIndex);
		m_nNumPlayer --;
	}
}
#endif

/*
int KPlayerSet::FindClient(int nClient)
{
	int i;

	i = m_UseIdx.GetNext(0);
	while(i)
	{
		if (Player[i].m_nNetConnectIdx == nClient)
			return i;
		i = m_UseIdx.GetNext(i);
	}
	return 0;
}
*/

#ifdef _SERVER
void KPlayerSet::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	if (nIndex <= 0 && nIndex >= MAX_PLAYER)
		return;

	int i = Player[nIndex].m_nNetConnectIdx;

	if (i >= 0)
	{
//		_ASSERT(g_CoreServerShell.CheckProtocolSize(pChar, nSize));
		if (Player[nIndex].m_dwID && !Player[nIndex].m_bExchangeServer)
			g_ProtocolProcess.ProcessNetMsg(nIndex, (BYTE*)pChar);
	}
}
#endif

//---------------------------------------------------------------------------
//	??:????
//---------------------------------------------------------------------------
KLevelAdd::KLevelAdd()
{
	memset(m_nLevelExp, 0, sizeof(double) * MAX_LEVEL);
	memset(m_nLifePerLevel, 0, sizeof(int) * series_num);
	memset(m_nManaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerLevel, 0, sizeof(int) * series_num);
	memset(m_nLifePerVitality, 0, sizeof(int) * series_num);
	memset(m_nStaminaPerVitality, 0, sizeof(int) * series_num);
	memset(m_nManaPerEnergy, 0, sizeof(int) * series_num);
}

//---------------------------------------------------------------------------
//	??:???
//---------------------------------------------------------------------------
BOOL	KLevelAdd::Init()
{
	int			i;
	KTabFile	LevelExp;
	if ( !LevelExp.Load(PLAYER_LEVEL_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEVEL; i++)
	{
		LevelExp.GetDouble(i + 2, 2, 0, &m_nLevelExp[i]);
		double m_LevelExp1 = 0;											//add by phong ki? code exp ki? m? theo VNG c?Tr?g sinh
		LevelExp.GetDouble(i + 2, 3, 0, &m_LevelExp1);
		m_LevelExp1 *= 10000;
		m_nLevelExp[i] += m_LevelExp1;
	}

	KTabFile	LevelAdd;
	if ( !LevelAdd.Load(PLAYER_LEVEL_ADD_FILE) )
		return FALSE;
	for (i = 0; i < series_num; i++)
	{
		LevelAdd.GetInteger(i + 2, 2, 0, &m_nLifePerLevel[i]);
		LevelAdd.GetInteger(i + 2, 3, 0, &m_nStaminaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 4, 0, &m_nManaPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 5, 0, &m_nLifePerVitality[i]);
		LevelAdd.GetInteger(i + 2, 6, 0, &m_nStaminaPerVitality[i]);
		LevelAdd.GetInteger(i + 2, 7, 0, &m_nManaPerEnergy[i]);
		LevelAdd.GetInteger(i + 2, 8, 0, &m_nLeadExpShare[i]);
		LevelAdd.GetInteger(i + 2, 9, 0, &m_nFireResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 10, 0, &m_nColdResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 11, 0, &m_nPoisonResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 12, 0, &m_nLightResistPerLevel[i]);
		LevelAdd.GetInteger(i + 2, 13, 0, &m_nPhysicsResistPerLevel[i]);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
//	??:??????????
//	??:nLevel  ????
//---------------------------------------------------------------------------
double		KLevelAdd::GetLevelExp(int nLevel)
{
	if (nLevel < 1 || nLevel > MAX_LEVEL)
		return 0;
	return m_nLevelExp[nLevel - 1];
}

//---------------------------------------------------------------------------
//	??:???????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	??:???????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	??:???????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerLevel(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerLevel[nSeries];
}

//---------------------------------------------------------------------------
//	??:????????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetLifePerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLifePerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	??:????????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetStaminaPerVitality(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nStaminaPerVitality[nSeries];
}

//---------------------------------------------------------------------------
//	??:????????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetManaPerEnergy(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nManaPerEnergy[nSeries];
}

//---------------------------------------------------------------------------
//	??:????????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetLeadExpShare(int nSeries)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	return m_nLeadExpShare[nSeries];
}

//---------------------------------------------------------------------------
//	??:?????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetFireResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nFireResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	??:?????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetColdResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nColdResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	??:?????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetPoisonResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nPoisonResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	??:?????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetLightResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nLightResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	??:??????????????
//---------------------------------------------------------------------------
int		KLevelAdd::GetPhysicsResist(int nSeries, int nLevel)
{
	if (nSeries < 0 || nSeries >= series_num)
		return 0;
	if (nLevel <= 0 || nLevel >= MAX_LEVEL)
		return 0;
	return (m_nPhysicsResistPerLevel[nSeries] * nLevel / 100);
}

//---------------------------------------------------------------------------
//	??:????
//---------------------------------------------------------------------------
KMagicLevelExp::KMagicLevelExp()
{
}

//---------------------------------------------------------------------------
//	??:???
//---------------------------------------------------------------------------
BOOL	KMagicLevelExp::Init()
{
	if ( !m_MagicLevelExpFile.Load(PLAYER_MAGIC_LEVEL_EXP_FILE) )
		return FALSE;
		
	return TRUE;
}

int		KMagicLevelExp::GetNextExp(int nSkillId, int nLevel)
{
	if (nSkillId <= 0 && nSkillId > MAX_SKILL)
		return 0;
	if (nLevel <= 0 && nLevel > MAX_TRAIN_SKILLEXPLEVEL)
		return 0;	
		
	int nExp = 0;
	char nRow[16];
	char nCol[8];
	itoa(nSkillId, nRow, 10);
	sprintf(nCol, "LEVEL%d", nLevel);
	m_MagicLevelExpFile.GetInteger(nRow, nCol, 0, &nExp);
	
	return nExp;
}

//---------------------------------------------------------------------------
//	??:????
//---------------------------------------------------------------------------
KTeamLeadExp::KTeamLeadExp()
{
	for (int i = 0; i < MAX_LEAD_LEVEL; i++)
	{
		m_sPerLevel[i].m_dwExp = 0;
		m_sPerLevel[i].m_dwMemNum = 1;
	}
}

//---------------------------------------------------------------------------
//	??:???,???????
//---------------------------------------------------------------------------
BOOL	KTeamLeadExp::Init()
{
	int			i;
	KTabFile	LevelExp;

	if ( !LevelExp.Load(PLAYER_LEVEL_LEAD_EXP_FILE) )
		return FALSE;
	for (i = 0; i < MAX_LEAD_LEVEL; i++)
	{
		LevelExp.GetInteger(i + 2, 2, 0, (int*)&m_sPerLevel[i].m_dwExp);
		LevelExp.GetInteger(i + 2, 3, 1, (int*)&m_sPerLevel[i].m_dwMemNum);
	}

	return TRUE;
}

//---------------------------------------------------------------------------
//	??:?????????
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevel(DWORD dwExp, int nCurLeadLevel)
{
	if (dwExp <= 0)
		return 1;
	if (nCurLeadLevel > 0)
	{
		if (nCurLeadLevel >= MAX_LEAD_LEVEL)
			return MAX_LEAD_LEVEL;
		if (dwExp < m_sPerLevel[nCurLeadLevel - 1].m_dwExp)
			return nCurLeadLevel;
		if (dwExp < m_sPerLevel[nCurLeadLevel].m_dwExp)
			return nCurLeadLevel + 1;
	}
	for (int i = 0; i < MAX_LEAD_LEVEL; i++)
	{
		if (dwExp < m_sPerLevel[i].m_dwExp)
		{
			return i + 1;
		}
	}
	return MAX_LEAD_LEVEL;
}

//---------------------------------------------------------------------------
//	??:???????????
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromExp(DWORD dwExp)
{
	int nGetLevel;
	nGetLevel = GetLevel(dwExp);
	return m_sPerLevel[nGetLevel - 1].m_dwMemNum;
}

//---------------------------------------------------------------------------
//	??:???????????
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetMemNumFromLevel(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return m_sPerLevel[nLevel - 1].m_dwMemNum;
	return 1;
}

//---------------------------------------------------------------------------
//	??:?????????????
//---------------------------------------------------------------------------
int		KTeamLeadExp::GetLevelExp(int nLevel)
{
	if (1 <= nLevel && nLevel <= MAX_LEAD_LEVEL)
		return this->m_sPerLevel[nLevel - 1].m_dwExp;
	return 0;
}

#ifdef _SERVER
void	KPlayerSet::Activate()
{
}
#endif

KNewPlayerAttribute::KNewPlayerAttribute()
{
	memset(m_nStrength, 0, sizeof(m_nStrength));
	memset(m_nDexterity, 0, sizeof(m_nDexterity));
	memset(m_nVitality, 0, sizeof(m_nVitality));
	memset(m_nEngergy, 0, sizeof(m_nEngergy));
	memset(m_nLucky, 0, sizeof(m_nLucky));
}

BOOL	KNewPlayerAttribute::Init()
{
	char		szSeries[5][16] = {"metal", "wood", "water", "fire", "earth"};
	KIniFile	AttributeFile;

	if ( !AttributeFile.Load(BASE_ATTRIBUTE_FILE_NAME) )
		return FALSE;

	for (int i = 0; i < series_num; i++)
	{
		AttributeFile.GetInteger(szSeries[i], "Strength", 0, &m_nStrength[i]);
		AttributeFile.GetInteger(szSeries[i], "Dexterity", 0, &m_nDexterity[i]);
		AttributeFile.GetInteger(szSeries[i], "Vitality", 0, &m_nVitality[i]);
		AttributeFile.GetInteger(szSeries[i], "Engergy", 0, &m_nEngergy[i]);
		AttributeFile.GetInteger(szSeries[i], "Lucky", PLAYER_FIRST_LUCKY, &m_nLucky[i]);
	}

	return TRUE;
}

KPlayerStamina::KPlayerStamina()
{
	m_nNormalAdd = 0;
	m_nExerciseRunSub = 0;
	m_nFightRunSub = 0;
	m_nKillRunSub = 0;
	m_nTongWarRunSub = 0;
	m_nSitAdd = 0;
}

BOOL	KPlayerStamina::Init()
{
	KIniFile	StaminaFile;

	if ( !StaminaFile.Load(PLAYER_STAMINA_FILE_NAME) )
		return FALSE;

	StaminaFile.GetInteger("stamina", "NormalAdd", 1, &m_nNormalAdd);
	StaminaFile.GetInteger("stamina", "ExerciseRunSub", 1, &m_nExerciseRunSub);
	StaminaFile.GetInteger("stamina", "FightRunSub", 1, &m_nFightRunSub);
	StaminaFile.GetInteger("stamina", "KillRunSub", 18, &m_nKillRunSub);
	StaminaFile.GetInteger("stamina", "TongWarRunSub", 8, &m_nTongWarRunSub);
	StaminaFile.GetInteger("stamina", "SitAdd", 10, &m_nSitAdd);

	return TRUE;
}

#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerName(int nIndex, char* szName)
{
//	int i = FindClient(nClient);

	int i = nIndex;

	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}
	strcpy(szName, Player[i].m_PlayerName);
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerAccount(int i, char* szName)
{
//	int i = FindClient(nClient);
	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}

	strcpy(szName, Player[i].m_AccoutName);
	return TRUE;
}
BOOL	KPlayerSet::GetPlayerHWID(int i, char* szName)
{
//	int i = FindClient(nClient);
	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}

	strcpy(szName, Player[i].m_nPlayerHWID);
	return TRUE;
}

#endif

#ifdef _SERVER

int		KPlayerSet::AttachPlayer(const unsigned long lnID, GUID* pGuid, char* sHWID)
{
	if (lnID >= MAX_PLAYER || NULL == pGuid)
		return 0;

	int nUseIdx = m_UseIdx.GetPrev(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx == -1)
		{
			if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
			{
				Player[nUseIdx].m_nNetConnectIdx = lnID;
				Player[nUseIdx].m_ulLastSaveTime = g_SubWorldSet.m_nLoopRate;
				strncpy(Player[nUseIdx].m_nPlayerHWID, sHWID, sizeof(Player[nUseIdx].m_nPlayerHWID) - 1);
				Player[nUseIdx].m_nPlayerHWID[sizeof(Player[nUseIdx].m_nPlayerHWID) - 1] = '\0';
//				Player[nUseIdx].m_dwLoginTime = -1;
				return nUseIdx;
			}
			else
			{
				g_DebugLog("[error]Player[nUseIdx].m_nNetConnectIdx == -1 but Attach to a connected player named %s", Player[nUseIdx].m_PlayerName);
				//return 0;
			}
		}
		nUseIdx = m_UseIdx.GetPrev(nUseIdx);
	}
	return 0;
}
void		KPlayerSet::RemoveAllPlayerLixianByAccount(char* szAccName)
{
	int nUseIdx = 1;
	while (nUseIdx)
	{
		//	if (0 == memcmp(&Player[nUseIdx].m_AccoutName, szAccName, sizeof(char*)))
		if (strcmpi(Player[nUseIdx].m_AccoutName, szAccName) == 0)
		{
			if (Player[nUseIdx].m_nLixian)//neu dang uy thac
			{
				Player[nUseIdx].m_nLixian = 0;
				Player[nUseIdx].m_bIsQuiting = TRUE;
				RemoveQuiting(nUseIdx);//edit by phong kieu Remove account dang uy thac
			}
		}
		nUseIdx++;

		if (nUseIdx >= MAX_PLAYER)
			break;
	}
}

int		KPlayerSet::GetPlayerIndexByGuid(GUID* pGuid)
{
	int nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
		{
			if (Player[nUseIdx].m_nNetConnectIdx != -1)
			{
				return nUseIdx;
			}
			else
			{
				g_DebugLog("[error]Find Guid to a disconnect player");
				return 0;
			}
		}
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}
#endif

#ifdef _SERVER
void KPlayerSet::AutoSave()
{
	unsigned long uTime = g_SubWorldSet.GetGameTime();

	if (uTime >= m_ulNextSaveTime)
	{
		int nUseIdx = m_UseIdx.GetNext(0);
		// (20/08) Truoc day `break` nam NGOAI nhanh if(Save()), nen mot nguoi choi
		// co Save() luon that bai (vd Lua tat luu qua SetEnablePlayerSave, hoac
		// UpdateDBPlayerInfo tra -1) se duoc chon lai moi tick roi break => CA MAY
		// CHU khong con ai duoc luu. Nay: that bai thi di tiep nguoi ke, chi break
		// khi da luu duoc. Van giu dung tran 1 luot luu THANH CONG moi tick.
		int nThu = 0;
		const int nThuToiDa = 8;	// tran so lan thu/tick, khong quet het 1500 khe
		while(nUseIdx)
		{
			if (Player[nUseIdx].CanSave() && uTime - Player[nUseIdx].m_ulLastSaveTime >= m_ulMaxSaveTimePerPlayer)
			{
				if (Player[nUseIdx].Save())
				{
					Player[nUseIdx].m_uMustSave = SAVE_REQUEST;
//					Player[nUseIdx].m_ulLastSaveTime = uTime;
					m_ulNextSaveTime += m_ulDelayTimePerSave;
					break;
				}
				if (++nThu >= nThuToiDa)
					break;
			}
			nUseIdx = m_UseIdx.GetNext(nUseIdx);
		}
	}
}
#endif
