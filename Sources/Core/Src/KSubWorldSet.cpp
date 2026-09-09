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
#include "KPerfTick.h"	// [PerfLog 24/08] do thoi gian tung giai doan
#include <thread>
#include <vector>
#include <chrono>

KSubWorldSet g_SubWorldSet;

KSubWorldSet::KSubWorldSet()
{
	m_nLoopRate = 0;
	m_nGameVersion = ITEM_VERSION;
	m_nMapLoaded = 0;
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
		sprintf((char*)szKeyName, "World%03d", i);
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
	m_nMapLoaded = maploaded;
	printf("====== Map load failed  : [%d] \n",mapfailed);
	printf("====== Npc loaded  : [%ld] \n", totalNpcLoaded);
	return TRUE;
}

int nActiveRegionCount;
int nActiveNpcCount;	// [PerfLog 24/08] so NPC nam trong cac region hoat dong

#ifndef _SERVER
// [WORLD 08/09 a] gom so do tu KSubWorld.cpp/KRegion.cpp roi in moi 10 giay vao jx_paint.log
double g_dWorldCanBang = 0.0;
extern double g_dWorldXoaCo, g_dWorldQuetVung, g_dWorldMaxTick;
extern unsigned g_uWorldTick, g_uWorldVung, g_uWorldNpc, g_uWorldVungTong;
extern int g_nCorePaintLog;
static void WorldInDong()
{
	if (g_nCorePaintLog <= 0 || g_uWorldTick == 0)
		return;
	static DWORD s_dwLan = 0;
	const DWORD dwNow = timeGetTime();
	if (s_dwLan == 0) { s_dwLan = dwNow; return; }
	if (dwNow - s_dwLan < 10000)
		return;
	s_dwLan = dwNow;
	FILE* pLog = fopen("jx_paint.log", "a");
	if (pLog)
	{
		fprintf(pLog, "[WORLD] t=%u tick=%u | xoa_co %.2f ms | quet_vung %.2f ms (max %.1f) | can_bang %.2f ms | vung %.1f/%.0f dang chay | npc %.0f/tick\n",
			dwNow, g_uWorldTick,
			g_dWorldXoaCo / g_uWorldTick, g_dWorldQuetVung / g_uWorldTick, g_dWorldMaxTick, g_dWorldCanBang / g_uWorldTick,
			(double)g_uWorldVung / g_uWorldTick, (double)g_uWorldVungTong / g_uWorldTick, (double)g_uWorldNpc / g_uWorldTick);
		fclose(pLog);
	}
	{	// [WORLD 09/09 b] dong trung binh theo NPC / pha
		extern double g_dWorldNhac, g_dNpcPha[4], g_dNpcTong, g_dNpcMax; extern unsigned g_uNpcLan; extern int g_nNpcMaxIdx;
		extern double g_dKhacMs[3]; extern unsigned g_uKhacSo[3];	// [WORLD 09/09 c]
		extern unsigned g_uVungSo, g_uVungLech, g_uVungXay;	// [VUNG 09/09]
		extern unsigned g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon; extern unsigned g_uAmNap; extern double g_dAmNapMs;	// [CAY 09/09 do] [AM 09/09 do]
		FILE* p2 = fopen("jx_paint.log", "a");
		if (p2)
		{
			fprintf(p2, "[WORLD b] t=%u tick=%u | nhac %.2f ms/tick | npc %.1f/tick, tong %.2f ms/tick = %.1f us/NPC"
				" | pha PS %.2f AI %.2f PC %.2f ST %.2f ms/tick | nang nhat %.2f ms idx %d | object %.1f/tick %.2f ms | dan %.1f/tick %.2f ms | nguoi choi %.2f ms/tick | vung: xay %u, hoi %u, lech %u | cay: chen %u duyet1 %u duyet2 %u khop %u con %u | am thanh %u lan %.2f ms\n",
				dwNow, g_uWorldTick, g_dWorldNhac / g_uWorldTick, (double)g_uNpcLan / g_uWorldTick, g_dNpcTong / g_uWorldTick,
				g_uNpcLan ? g_dNpcTong * 1000.0 / g_uNpcLan : 0.0,
				g_dNpcPha[0] / g_uWorldTick, g_dNpcPha[1] / g_uWorldTick, g_dNpcPha[2] / g_uWorldTick, g_dNpcPha[3] / g_uWorldTick,
				g_dNpcMax, g_nNpcMaxIdx,
				(double)g_uKhacSo[0] / g_uWorldTick, g_dKhacMs[0] / g_uWorldTick, (double)g_uKhacSo[1] / g_uWorldTick, g_dKhacMs[1] / g_uWorldTick, g_dKhacMs[2] / g_uWorldTick,
				g_uVungXay, g_uVungSo, g_uVungLech,
				g_uCayChen, g_uCayDuyet1, g_uCayDuyet2, g_uCayKhop, g_uCayCon, g_uAmNap, g_dAmNapMs);
			fclose(p2);
		}
		g_dWorldNhac = 0.0; g_dNpcPha[0] = g_dNpcPha[1] = g_dNpcPha[2] = g_dNpcPha[3] = 0.0;
		g_dNpcTong = 0.0; g_dNpcMax = 0.0; g_uNpcLan = 0; g_nNpcMaxIdx = 0;
		g_dKhacMs[0] = g_dKhacMs[1] = g_dKhacMs[2] = 0.0; g_uKhacSo[0] = g_uKhacSo[1] = g_uKhacSo[2] = 0;	// [WORLD 09/09 c]
		g_uVungSo = 0; g_uVungLech = 0; g_uVungXay = 0;	// [VUNG 09/09]
		g_uCayChen = g_uCayDuyet1 = g_uCayDuyet2 = g_uCayKhop = g_uCayCon = 0; g_uAmNap = 0; g_dAmNapMs = 0.0;	// [CAY 09/09 do] [AM 09/09 do]
	}
	g_dWorldXoaCo = g_dWorldQuetVung = g_dWorldMaxTick = g_dWorldCanBang = 0.0;
	g_uWorldTick = g_uWorldVung = g_uWorldNpc = g_uWorldVungTong = 0;
}
#endif
void KSubWorldSet::MainLoop()
{
	m_nLoopRate++;
	if (m_nLoopRate < 0)
		m_nLoopRate = 0;
#ifndef _SERVER
	//if (!(m_nLoopRate % 20))
	//	SendClientCmdPing();
	
	{	// [WORLD 09/09 b] do nhac (client)
		extern double g_dWorldNhac; extern double WorldMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);
		LARGE_INTEGER a, b; const bool bDo = (g_nCorePaintLog > 0);
		if (bDo) QueryPerformanceCounter(&a);
		this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);
		if (bDo) { QueryPerformanceCounter(&b); g_dWorldNhac += WorldMs(a, b); }
	}

#endif
#ifdef _SERVER
//		g_GlobalMissionArray.Activate();
#endif
	{	// [BC 04/09 do2] nhip tick THAT cua ca the gioi (mot lan moi vong Activate)
		extern int g_nBCTick;
		g_nBCTick++;
	}
	nActiveRegionCount = 0;
	nActiveNpcCount = 0;
	

	// [PerfLog 24/08] Thay doan do chrono cu (bi comment nen khong ai doc duoc)
	// bang bo dem PerfTick: co max/p95/dem tick tre, bat tat bang config.ini [PerfLog].
	// Bo luon bien chet "activeSubworlds" + vong cho da luong bi bo do:
	// SubWorld[i].Activate() dung chung Npc[]/Player[] toan cuc KHONG co khoa
	// (KNpc.h:878, KPlayer.h:1101) nen KHONG duoc chay song song.
	{
		PERF_SCOPE(PERF_SW_ACTIVATE);
		for (int i = 0; i < MAX_SUBWORLD; i++)
		{
			if (SubWorld[i].m_SubWorldID >= 0)
			{
				SubWorld[i].Activate();
#ifndef _SERVER
				{	// [WORLD 08/09 a] do rieng can bang cache NPC
					extern double WorldMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);
					extern double g_dWorldCanBang; extern int g_nCorePaintLog;
					LARGE_INTEGER a, b;
					const bool bDo = (g_nCorePaintLog > 0);
					if (bDo) QueryPerformanceCounter(&a);
					NpcSet.CheckBalance();
					if (bDo) { QueryPerformanceCounter(&b); g_dWorldCanBang += WorldMs(a, b); }
				}
#endif
			}
		}
#ifndef _SERVER
	WorldInDong();	// [WORLD 08/09 a] in [WORLD] moi 10 giay
#endif
	}

//	if ((m_nLoopRate % 100) == 0)
//		printf("Region:%d:%d\n", m_nLoopRate, nActiveRegionCount);
#ifdef _SERVER
	{ PERF_SCOPE(PERF_AUTOSAVE); PlayerSet.AutoSave(); }
	g_PerfCount(nActiveRegionCount, nActiveNpcCount);	// [PerfLog] khoi luong tick nay
#endif
}

void KSubWorldSet::MessageLoop()
{
#ifdef _SERVER
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		if (SubWorld[i].m_SubWorldID >= 0)
			SubWorld[i].MessageLoop();
	}
#else
	if (SubWorld[0].m_SubWorldID >= 0)
		SubWorld[0].MessageLoop();
#endif
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
	//Player[CLIENT_PLAYER_INDEX].m_cAutoMove.Reset();
	Player[CLIENT_PLAYER_INDEX].m_nIndex = 0;
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
bool KSubWorldSet::GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos)
{
	if (!pPos)
		return false;

	KIniFile IniFile;
	
	g_SetFilePath(SETTING_PATH);
	if(!IniFile.Load("RevivePos.ini"))
		return false;
	char	szKeyName[32];
	char	szSection[32];
	sprintf(szSection, "%d", dwSubWorldId);
	sprintf(szKeyName, "%d", nRevivalId);
	
	int nX = 0;//51200;
	int nY = 0;//102400;
	/*if (nRevivalId < 1 || (nX == 0 && nY == 0))
	{
		int nMin = 0;
		int nMax = 0;
		IniFile.GetInteger2(szSection, "region", &nMin, &nMax);
		sprintf(szKeyName, "%d", nMin);
		IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	}*/
	char szTemp[32];
	IniFile.GetString(szSection, szKeyName, "", szTemp, sizeof(szTemp));
	if(!szTemp[0])
		return false;
	IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	pPos->x = nX;
	pPos->y = nY;
	return true;
}
#endif