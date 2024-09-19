#include "KCore.h"
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KNpc.h"
#include "KIniFile.h"
#include "KSubWorldSet.h"
#include "KMissleSet.h"
#include "LuaFuns.h"
#include "KNpcTemplate.h"
#include "KPlayerSet.h"
#include "KPlayer.h"

KSubWorldSet g_SubWorldSet;

KSubWorldSet::KSubWorldSet()
{
	m_nLoopRate = 0;
	m_nGameVersion = ITEM_VERSION;
#ifndef _SERVER
	m_dwPing = 0;
#endif
}

int KSubWorldSet::SearchWorld(DWORD dwID)
{
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		if ((DWORD)SubWorld[i].m_SubWorldID == dwID)
			return i;
	}
	return -1;
}

BOOL KSubWorldSet::Load(LPSTR szFileName)//edit by phong kieu Load Maps LoadMaps
{
	KIniFile	IniFile;
	char		szKeyName[32];
	int			nWorldID;
	int			nWorldCount = 0;
	
	IniFile.Load(szFileName);
	IniFile.GetInteger("Init", "Count", 1, &nWorldCount);
	if (nWorldCount > MAX_SUBWORLD)
	{
		printf("MAX_SUBWORLD= [%d] Not Load nWorldCount=[%d] \n",MAX_SUBWORLD, nWorldCount);
		return FALSE;
	}
	
	int maploaded = 0;//edit by phong kieu count map load
	int mapfailed = 0;
	long totalNpcLoaded = 0;
	
	for (int i = 0; i < nWorldCount; i++)
	{
		sprintf((char*)szKeyName, "World%02d", i);
		IniFile.GetInteger("World", szKeyName, 1000, &nWorldID);
#ifdef _SERVER
		SubWorld[i].m_nIndex = i;
		if (!SubWorld[i].LoadMap(nWorldID))
		{
			//LogError szWorldFile Not Load edit by phong kieu logerror load map
			printf("LogError szWorldFile [%d] Not Load \n",nWorldID);
			mapfailed++;
		}
		else
		{
			totalNpcLoaded = totalNpcLoaded + SubWorld[i].CountAllNpc();
			printf("====== Map [%d] Npc [%ld] Loaded !!!!!! \n", nWorldID, SubWorld[i].CountAllNpc());
			maploaded++;	
		}
#endif
	}
	printf("====== Total Map loaded : [%d] \n",maploaded);
	printf("====== Map load failed  : [%d] \n",mapfailed);
	printf("====== Npc loaded  : [%ld] \n", totalNpcLoaded);
	return TRUE;
}

int nActiveRegionCount;

void KSubWorldSet::MainLoop()
{
	m_nLoopRate++;

#ifndef _SERVER
	//if (!(m_nLoopRate % 20))
	//	SendClientCmdPing();
	
	this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);

#endif
#ifdef _SERVER
//		g_GlobalMissionArray.Activate();
#endif
	nActiveRegionCount = 0;
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].Activate();
#ifndef _SERVER
		NpcSet.CheckBalance();
#endif
	}

//	if ((m_nLoopRate % 100) == 0)
//		printf("Region:%d:%d\n", m_nLoopRate, nActiveRegionCount);
#ifdef _SERVER
	PlayerSet.AutoSave();
#endif
}

void KSubWorldSet::MessageLoop()
{
	
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].MessageLoop();
	}
}

BOOL KSubWorldSet::SendMessage(int nSubWorldID, DWORD dwMsgType, int nParam1, int nParam2, int nParam3)
{
	KWorldMsgNode *pNode = NULL;
	
	pNode = new KWorldMsgNode;
	if (!pNode)
		return FALSE;
	
	pNode->m_dwMsgType	= dwMsgType;
	pNode->m_nParam[0]	= nParam1;
	pNode->m_nParam[1]	= nParam2;
	pNode->m_nParam[2]	= nParam3;
	if (pNode->m_dwMsgType == 4001) g_DebugLog("Send !!!!");
	return SubWorld[nSubWorldID].m_WorldMessage.Send(pNode);
}

void KSubWorldSet::Close()
{
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].Close();
	}
	NpcSet.RemoveAll();
#ifndef _SERVER
	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	//Player[CLIENT_PLAYER_INDEX].m_cAuto.Release();
	Player[CLIENT_PLAYER_INDEX].m_cAutoMove.Reset();
	g_Team[0].Release();
	m_cMusic.Stop();
#endif
}

#ifndef _SERVER
void KSubWorldSet::Paint()
{
	SubWorld[0].Paint();
}
#endif

#ifdef _SERVER
void KSubWorldSet::GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos)
{
	if (!pPos)
		return;

	KIniFile IniFile;
	char	szKeyName[32];
	char	szSection[32];
	
	g_SetFilePath(SETTING_PATH);
	IniFile.Load("RevivePos.ini");
	sprintf(szSection, "%d", dwSubWorldId);
	sprintf(szKeyName, "%d", nRevivalId);
	
	int nX = 0;//51200;
	int nY = 0;//102400;
	IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	if (nRevivalId < 1 || (nX == 0 && nY == 0))
	{
		int nMin = 0;
		int nMax = 0;
		IniFile.GetInteger2(szSection, "region", &nMin, &nMax);
		sprintf(szKeyName, "%d", nMin);
		IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	}
	
	pPos->x = nX;
	pPos->y = nY;
}
#endif