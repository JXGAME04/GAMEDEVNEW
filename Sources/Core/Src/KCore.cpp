//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2020 by Kingsoft
// File:	KCore.cpp
// Date:	2020.08.08
// Code:	Fong KiÒu
// Desc:	Core class
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KEngine.h"
#include "KMySQLDB.h"
#include "KGameKV.h"
#include "KFilePath.h"
#ifndef _SERVER
#include "KNpcResList.h"
#include "KBmpFile.h"
#include "ImgRef.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#endif
#include "KItemChangeRes.h"
#include "KNpcSet.h"
#include "KTabFile.h"
#include "KSkills.h"
#include "KPlayerSet.h"
#include "KPlayerTeam.h"
#include "KMissleSet.h"
#include "KFaction.h"
#include "KMath.h"
#include "time.h"
#include "KPlayerTask.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"
#include "KObjSet.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KNpcTemplate.h"
#include "CoreUseNameDef.h"
#include "KBuySell.h"
#include "KSortScript.h"
#include "KTaskFuns.h"
#include "TaskDef.h"
#include "LuaFuns.h"
#include "GiftCodeManager.h"

#ifndef WIN32 //add by phong kiÒu
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#else 
#include <io.h>
#include <direct.h>
#endif

#pragma warning (disable: 4512)
#pragma warning (disable: 4786)

#ifdef _SERVER
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#endif

#ifdef _SERVER
extern int g_WayPointPriceUnit;	//WayPoint WayPoint
extern int g_StationPriceUnit;	//Station Station
extern int g_DockPriceUnit;
#endif

#ifndef _SERVER
#define CLIENTWEAPONSKILL_TABFILE		"\\settings\\ClientWeaponSkill.txt"	
KTabFile g_ClientWeaponSkillTabFile;
#define ADJUSTCOLOR_TABFILE				"\\settings\\AdjustColor.txt"	
unsigned int		InitAdjustColorTab();

unsigned int	* g_pAdjustColorTab = NULL;
unsigned int g_ulAdjustColorCount = 0;

#endif

BOOL	InitTaskSetting();

#ifndef _SERVER
#include "KJXPathFinder.h"
#include "KMagicDesc.h"
#include "Scene/KScenePlaceC.h"
BOOL g_bUISelIntelActiveWithServer = FALSE;
BOOL g_bUISpeakActiveWithServer = FALSE;
int	g_bUISelLastSelCount = 0;
extern KTabFile g_StringResourseTabFile;
KTabFile g_RankTabSetting;
//extern KJXPathFinder g_JXPathFinder;
#endif

//#define DEBUGOPT_SCRIPT_MSG_FILEOUT
#ifdef _SERVER
IServer* g_pServer;
extern KTabFile g_EventItemTab;
extern KTabFile g_GoldItemTab;
extern KTabFile g_RenameNpcTabSetting;//edit by phong kieu load file rename npc
extern KTabFile g_TabFileLib;//Add by phong kiÒu
extern KTabFile g_TabFileCtrlLib;//Add by phong kiÒu
#else
IClient* g_pClient;
BOOL	 g_bPingReply;
#endif

//---------------------------------------------------------------------------
#ifdef TOOLVERSION
KSpriteCache	g_SpriteCache;
#endif
KTabFile		g_OrdinSkillsSetting, g_MisslesSetting;
KTabFile		g_MeridiantSetting;
KTabFile		g_SkillLevelSetting;
KTabFile		g_NpcSetting, g_ReBornSetting;
KTabFile		g_NpcKindFile; //Npc
int				g_nMeleeWeaponSkill[MAX_MELEEWEAPON_PARTICULARTYPE_NUM];
int				g_nRangeWeaponSkill[MAX_RANGEWEAPON_PARTICULARTYPE_NUM];
int				g_nHandSkill;

KTabFile 		g_MaskChangeRes;
KIniFile 		g_GameSetting;
KIniFile 		g_MapTraffic;

int				g_MaxOptMultiply = 1;
int				g_xMethod = 1;
#ifdef _SERVER
int				g_ExpRate = 1;
int				g_MoneyRate = 1;
int				g_Skill90ExpRate = 1;
int				g_Skill120ExpRate = 1;
int				g_NotAddNpcNormal = 1;
KLuaScript * 	g_pStartScript = NULL;
#endif

int				g_WriteScriptNpcLog=1;

#ifndef	_SERVER
KSoundCache		g_SoundCache;
KMusic			*g_pMusic = NULL;
#endif

//int				g_HPMonter = 1;
#ifdef _SERVER
//int				g_ExpServer = 1;
KLuaScript * 	pTimeScript =(KLuaScript*) g_GetScript("//script//timerserver.lua");
#endif

//#ifdef _DEBUG
CORE_API BOOL			g_bDebugScript;//When True , Testing Debug, Script Will be ReLoaded EveryTime When to be Executed, Not Using ScriptCach.
//#endif

KLuaScript	*	g_pNpcLevelScript = NULL;
KLuaScript g_WorldScript;

void g_InitProtocol();

//---------------------------------------------------------------------------
CORE_API void g_InitCore(char * nParmName)
{
	g_InitProtocol();

#ifdef _SERVER
	// (20/08) Mo ket noi MySQL cho toan bo du lieu tinh nang + nhat ky.
	// THAT BAI THI VAN CHAY TIEP: cac tinh nang tu quay ve duong ghi tep cu.
	// Tuyet doi khong duoc lam sap may chu chi vi MySQL chua san sang --
	// dung nguyen tac "them mot phu thuoc thi phai them mot duong lui".
	if (!g_MySQLDB.Init())
		KDBLog("CANH BAO: khong ket noi duoc MySQL -- cac tinh nang se dung tep nhu cu");
	else
	{
		// (20/08) Neu tep trang thai bi MAT (doi may, hong dia, xoa nham) thi
		// dung lai tu MySQL TRUOC khi cac tinh nang doc. Chi lam khi tep THIEU --
		// tep con do thi khong dung toi, tranh de len ban moi hon tren dia.
		char szRoot[MAX_PATH], szP[MAX_PATH];
		g_GetRootPath(szRoot);
		sprintf(szP, "%s\\settings\\jx2citywar.txt", szRoot);
		KGameKV::GetFileToDisk("jx2.citywar", "file", szP, true);
		sprintf(szP, "%s\\settings\\jx2league.txt", szRoot);
		KGameKV::GetFileToDisk("jx2.league", "file", szP, true);
		sprintf(szP, "%s\\settings\\jx2ladder.txt", szRoot);
		KGameKV::GetFileToDisk("jx2.ladder", "file", szP, true);
	}
#endif
#ifdef _DEBUG
	g_bDebugScript = 0;
	g_FindDebugWindow("#32770", "DebugWin");
	
	{
		srand((unsigned int) time(NULL));
#ifdef DEBUGOPT_SCRIPT_MSG_FILEOUT
		struct tm *newtime;
		time_t long_time;
		time( &long_time );   	 /* Get time as long integer. */
		newtime = localtime( &long_time ); /* Convert to local time. */
		char szFileName[MAX_PATH];
		char szFileName1[MAX_PATH];
		char szPathName[MAX_PATH];
		g_CreatePath("\\DebugData");
		g_CreatePath("\\DebugData\\Script");
			
		GetCurrentDirectory(MAX_PATH, szPathName);

		sprintf(szFileName, "%s\\DebugData\\Script\\ScriptOut_%d%d%d%d%d.txt", szPathName, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
		sprintf(szFileName1, "%s\\DebugData\\Script\\ScriptErr_%d%d%d%d%d.txt", szPathName, newtime->tm_mon, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
		lua_setdebugout(szFileName, szFileName1);
#endif
	}

#endif
	
#ifndef __linux
	g_RandomSeed(GetTickCount());
#else
	g_RandomSeed(clock());
#endif
	srand( (unsigned)time( NULL ) );
	printf("=====================================\n");
	printf("---[Memory] KPlayer init:%ld * %ld = %ld mb!\n", sizeof(KPlayer), MAX_PLAYER, MAX_PLAYER * sizeof(KPlayer) / (1024 * 1024));
	printf("---[Memory] KNpc init:%ld * %ld = %ld mb!\n", sizeof(KNpc), MAX_NPC, MAX_NPC * sizeof(KNpc) / (1024 * 1024));
	printf("---[Memory] KItem init:%ld * %ld = %ld mb!\n", sizeof(KItem), MAX_ITEM, MAX_ITEM * sizeof(KItem) / (1024 * 1024));
	printf("---[Memory] KObj init:%ld * %ld = %ld mb!\n", sizeof(KObj), MAX_OBJECT, MAX_OBJECT * sizeof(KObj) / (1024 * 1024));
	printf("---[Memory] KMissle init:%ld * %ld = %ld mb!\n", sizeof(KMissle), MAX_MISSLE, MAX_MISSLE * sizeof(KMissle) / (1024 * 1024));
	printf("---[Memory] KSubWorld init:%ld * %ld = %ld mb!\n", sizeof(KSubWorld), MAX_SUBWORLD, MAX_SUBWORLD * sizeof(KSubWorld) / (1024 * 1024));
	printf("---[Memory] KPlayerTask init:%ld * %ld = %ld mb!\n", sizeof(KPlayerTask), MAX_TASK, MAX_TASK * sizeof(KPlayerTask) / (1024 * 1024));
	printf("---[Memory] g_ScriptSet init:%ld * %ld = %ld mb!\n", sizeof(g_ScriptSet), MAX_SCRIPT_IN_SET, MAX_SCRIPT_IN_SET * sizeof(g_ScriptSet) / (1024 * 1024));
	printf("=====================================\n");
	printf("MaxSubWordCount = %d \tMaxPlayerCount = %d \tMaxNpcCount = %d\n", MAX_SUBWORLD, MAX_PLAYER, MAX_NPC);
	printf("MaxItemCount = %d \tMaxObjectCount = %d\n", MAX_ITEM, MAX_OBJECT);
	printf("MaxPlayerLevel = %d \tMaxNpcLevel = %d\n", MAX_LEVEL, MAX_NPC_LEVEL);
	printf("MaxSkillCount = %d \tMaxSkillLevel = %d \tMaxMissileCount = %d\n", MAX_NPCSKILL, MAX_SKILLLEVEL, MAX_MISSLE);
	time_t ltime;
	time( &ltime );
	printf("Starting Core......%s", ctime( &ltime ));
	printf("=====================================\n");
	
#ifdef TOOLVERSION	
	g_SpriteCache.Init(256);
#endif

#ifndef _SERVER
	g_bPingReply = TRUE;
	g_SoundCache.Init(256);
	g_SubWorldSet.m_cMusic.Init();
#endif

#ifndef TOOLVERSION
	g_InitSeries();
    if (!g_InitMath())
    {
    	g_DebugLog("[Math] Init!");
    }
	ItemSet.Init();
	ItemGen.Init();
	MeridianManager.Init();
#ifndef _SERVER
	InitAdjustColorTab();
	g_MagicDesc.Init();
#endif
	g_ItemChangeRes.Init();
#endif
	NpcSet.Init();
	ObjSet.Init();
	MissleSet.Init();
	g_IniScriptEngine();
	g_OrdinSkillsSetting.Load(SKILL_SETTING_FILE);
	g_MeridiantSetting.Load(MERIDIAN_SETTING_FILE);
	g_MisslesSetting.Load(MISSLES_SETTING_FILE);
	g_NpcSetting.Load(NPC_SETTING_FILE);
	InitGameSetting();
	g_DebugLog("[Script] %d", sizeof(g_ScriptSet));
	InitSkillSetting();
	InitMissleSetting();
	InitNpcSetting();
	InitTaskSetting();
	NpcSet.m_cGoldTemplate.Init();
	if (!PlayerSet.Init())
	{
		printf("Init PlayerSet Error!!!!\n");
	}
	
#ifdef _SERVER
	// (20/08) Ten lo phai khop cot `batch` trong bang jx1_game.giftcode.
	g_GiftCodeFanCungManager.SetBatch("tuan");
	g_GiftCodeNewManager.SetBatch("new");
	g_GiftCodeFanCungManager.LoadGiftCodes("dulieu\\giftcode\\giftcode_tuan_list.txt");
	g_GiftCodeFanCungManager.LoadUsedCodes("dulieu\\giftcode\\giftcode_tuan_used.txt");
	g_GiftCodeNewManager.LoadGiftCodes("dulieu\\giftcode\\giftcode_new_list.txt");
	g_GiftCodeNewManager.LoadUsedCodes("dulieu\\giftcode\\giftcode_new_used.txt");
	
	if (!g_ReBornSetting.Load(GAME_REBORN_FILE))
	{
		printf(" g_ReBornSetting load error\n");
	}
	memset(g_TaskGlobalValue, 0, sizeof(g_TaskGlobalValue));
	g_TeamSet.Init();

	char tempMapSetPath[128];
	sprintf(tempMapSetPath, "\\maps\\WorldSet_%s.ini", nParmName);

	printf("====== Maps (%s) loading ...... \n", tempMapSetPath);
	if(g_SubWorldSet.Load(tempMapSetPath))
		printf("====== Maps Load(%s) okay!!!!\n", tempMapSetPath);
	else
		printf("====== Maps Load(%s) error!!!!\n", tempMapSetPath);

	g_WorldScript.Init();
	g_WorldScript.RegisterFunctions(WorldScriptFuns, g_GetWorldScriptFunNum());
#endif
	
#ifndef _SERVER
	g_ScenePlace.Initialize();
#endif
	time(&ltime);
	g_DebugLog("Initting g_Faction %s",  ctime(&ltime));
	g_Faction.Init();
	time(&ltime);
#ifdef _SERVER
#ifdef _DEBUG
	KIniFile ServerSettingIni;
	if (ServerSettingIni.Load("\\Setting.ini"))
	{
		ServerSettingIni.GetInteger("Setting", "DebugScript", 0, &g_bDebugScript);
	}
#endif
#endif
	
	memset(g_nMeleeWeaponSkill, 0, sizeof(g_nMeleeWeaponSkill));
	memset(g_nRangeWeaponSkill, 0, sizeof(g_nRangeWeaponSkill));
	KTabFile Weapon_PhysicsSkillIdFile;
	if (Weapon_PhysicsSkillIdFile.Load(WEAPON_PHYSICSSKILLFILE))
	{
		int nHeight = Weapon_PhysicsSkillIdFile.GetHeight() - 1;
		int nDetailCol		= Weapon_PhysicsSkillIdFile.FindColumn(WEAPON_DETAILTYPE);
		int	nParticularCol	= Weapon_PhysicsSkillIdFile.FindColumn(WEAPON_PARTICULARTYPE);
		int nPhysicsSkillCol = Weapon_PhysicsSkillIdFile.FindColumn(WEAPON_SKILLID);
		
		for (int i = 0; i < nHeight; i ++)
		{
			int nDetail = 0;
			int nParticular  = 0;
			int nPhysicsSkill = 0;
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2, nDetailCol, -1, &nDetail);
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2, nParticularCol, -1, &nParticular);
			Weapon_PhysicsSkillIdFile.GetInteger(i + 2, nPhysicsSkillCol, -1, &nPhysicsSkill);
			
			if (nDetail == 0 )
			{
				if (nParticular >= 0 && nParticular < MAX_MELEEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nMeleeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == 1)
			{
				if (nParticular >= 0 && nParticular < MAX_RANGEWEAPON_PARTICULARTYPE_NUM && nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nRangeWeaponSkill[nParticular] = nPhysicsSkill;
			}
			else if (nDetail == -1) 
			{
				if (nPhysicsSkill > 0 && nPhysicsSkill < MAX_SKILL)
					g_nHandSkill = nPhysicsSkill;
			}
		}
	}

#ifndef TOOLVERSION
	if (!BuySell.Init())
	{
		g_DebugLog("Buysell init failed!");
	}
#endif

	printf("End of Init ,Game Started! %s" , ctime(&ltime));
	printf("====================================\n");
}

BOOL	InitTaskSetting()
{
#ifdef _SERVER
	if (!g_RenameNpcTabSetting.Load(RENAME_NPC_SETTING_TABFILE))//edit by phong kieu load file Rename NpcName.txt
	{
		g_DebugLog("[TASK]CAN NOT LOAD %s", RENAME_NPC_SETTING_TABFILE);
	}

	if (!g_EventItemTab.Load(QUESTITEM_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD EventItem Setting File %s", QUESTITEM_TABFILE);
	}

	if (!g_GoldItemTab.Load(TABFILE_GOLDITEM_FULL_O))
	{
		g_DebugLog("[TASK]CAN NOT LOAD GoldItem Setting File %s", TABFILE_GOLDITEM_FULL_O);
	}

	if (!g_WayPointTabFile.Load(WORLD_WAYPOINT_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD WayPoint Setting File %s", WORLD_WAYPOINT_TABFILE);
	}

	if (!g_StationTabFile.Load(WORLD_STATION_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD WayPoint Setting File %s", WORLD_STATION_TABFILE);
	}

	if (!g_StationPriceTabFile.Load(WORLD_STATIONPRICE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD STATION PRICE TabFile %s", WORLD_STATIONPRICE_TABFILE);
	}

	if (!g_WayPointPriceTabFile.Load(WORLD_WAYPOINTPRICE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD WAYPOINT PRICE TabFile %s", WORLD_WAYPOINTPRICE_TABFILE);
	}
	
	if (!g_DockTabFile.Load(WORLD_DOCK_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD DOCK TabFile %s", WORLD_DOCK_TABFILE);
	}

	if (!g_DockPriceTabFile.Load(WORLD_DOCKPRICE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD DOCK PRICE TabFile %s", WORLD_DOCKPRICE_TABFILE);
	}

	int nWidth1 = g_StationPriceTabFile.GetWidth();
	int nHeight1 = g_StationPriceTabFile.GetHeight();
	int nWidth2 = g_WayPointPriceTabFile.GetWidth();
	int nHeight2 = g_WayPointPriceTabFile.GetHeight();
	int nWidth3 = g_DockPriceTabFile.GetWidth();
	int nHeight3 = g_DockPriceTabFile.GetHeight();
	
	KASSERT(nHeight1 * nWidth1);
	KASSERT(nHeight2 * nWidth2);
	KASSERT(nHeight3 * nWidth3);
	
	if (nWidth1 * nHeight1)
	{
		g_StationPriceTabFile.GetInteger(1,1, 1, &g_StationPriceUnit);
		g_pStationPriceTab = new int[nWidth1 * nHeight1];
		memset(g_pStationPriceTab, -1, nWidth1 * nHeight1 * sizeof(int));
	}
	
	if (nWidth2 * nHeight2)
	{
		g_WayPointPriceTabFile.GetInteger(1,1,1, &g_WayPointPriceUnit);
		g_pWayPointPriceTab = new int[nWidth2 * nHeight2];
		memset(g_pWayPointPriceTab, -1, nWidth2 * nHeight2 * sizeof(int));
	}
	
	if (nWidth3 * nHeight3)
	{
		g_DockPriceTabFile.GetInteger(1,1,1, &g_DockPriceUnit);
		g_pDockPriceTab = new int[nWidth3 * nHeight3];
		memset(g_pDockPriceTab, -1, nWidth3 * nHeight3 * sizeof(int));
	}
	if (!g_TimerTask.Init())
	{
		g_DebugLog("Timer Task Init Error!");
	}

	if (!g_MissionTabFile.Load(TASK_MISSION_SETTING_TABFILE))
	{
		g_DebugLog("[error]Can Not Open %s", TASK_MISSION_SETTING_TABFILE);
	}
	
#endif
#ifndef _SERVER
	if (!g_StringResourseTabFile.Load(STRINGRESOURSE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD %s", STRINGRESOURSE_TABFILE);
	}

	if (!g_RankTabSetting.Load(PLAYER_RANK_SETTING_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD %s", PLAYER_RANK_SETTING_TABFILE);
	}

#endif
	
	return TRUE;
}

BOOL	InitNpcSetting()
{
	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	g_DebugLog("nNpcTemplateNum %d", nNpcTemplateNum);
	memset(g_pNpcTemplate, 0, sizeof(void*) * MAX_NPCSTYLE*MAX_NPC_LEVEL*MAX_NPC_SERIES);
	if ( !g_NpcKindFile.Load(NPC_RES_KIND_FILE_NAME) )
	{
		g_DebugLog("NPC_RES_KIND_FILE_NAME %s !!!", NPC_RES_KIND_FILE_NAME);
	}

#ifdef _SERVER
	g_pNpcLevelScript = (KLuaScript*)g_GetScript(NPC_LEVELSCRIPT_FILENAME);
#else
	g_pNpcLevelScript = new KLuaScript;
	g_pNpcLevelScript->Init();
	if (!g_pNpcLevelScript->Load(NPC_LEVELSCRIPT_FILENAME))
	{
		printf("[error] %s", NPC_LEVELSCRIPT_FILENAME);
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}
#endif
	
	if (!g_pNpcLevelScript) 
	{
		printf("g_pNpcLevelScript NULL %s \n", NPC_LEVELSCRIPT_FILENAME);
	}

#ifndef _SERVER
	g_NpcResList.Init();
#endif
	
	return TRUE;
}

BOOL	InitMissleSetting()
{
	int nMissleNum = g_MisslesSetting.GetHeight() - 1;
	
	for (int i = 0; i < nMissleNum; i++)
	{
		int nMissleId = 0;
		g_MisslesSetting.GetInteger(i + 2, "MissleId", -1, &nMissleId);
		
		if (nMissleId > 0)
		{
			g_MisslesLib[nMissleId].GetInfoFromTabFile(i + 2);
			g_MisslesLib[nMissleId].m_nMissleId = nMissleId;
		}
	}
	g_DebugLog("g_MisslesLib size %d", sizeof(g_MisslesLib));
	return TRUE;
}

extern int g_LoadSkillInfo();
BOOL	InitSkillSetting()
{
	
	if (!g_SkillManager.Init())
	{
		_ASSERT(0);
	}

#ifndef _SERVER
	if (!g_ClientWeaponSkillTabFile.Load(CLIENTWEAPONSKILL_TABFILE))
	{
		g_DebugLog("Can Not Load %s", CLIENTWEAPONSKILL_TABFILE);
	}
#endif

	return TRUE;
}

#ifdef _SERVER
BOOL	LoadNpcSettingFromBinFile(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}

BOOL	SaveAsBinFileFromNpcSetting(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}
#endif

//---------------------------------------------------------------------------

void g_ReleaseCore()
{
#ifdef _SERVER
	// Dong truoc tien: Close() cho luong ghi nen XA HET hang doi roi moi tat,
	// nen khong mat nhat ky cua nhung giay cuoi.
	g_MySQLDB.Close();
#endif
	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned long k = 0;
	
	for (i = 0; i < nNpcTemplateNum; i++)
	{
		for (j = 0; j < MAX_NPC_LEVEL; j++)
		{
			for (k = 0; k < MAX_NPC_SERIES; k++)
			{
				if (g_pNpcTemplate[i][j][k])
				{
					delete ((KNpcTemplate *)g_pNpcTemplate[i][j][k]);
					g_pNpcTemplate[i][j][k] = NULL;
				}
			}
		}
	}
#ifdef _SERVER
	if (g_pServer)
	{
		g_pServer->Release();
		g_pServer = NULL;
	}
	
	if (g_pStationPriceTab)
	{
		delete [] g_pStationPriceTab;
		g_pStationPriceTab = NULL;
	}

	if (g_pWayPointPriceTab)
	{
		delete [] g_pWayPointPriceTab;
		g_pWayPointPriceTab = NULL;
	}
	
	if (g_pDockPriceTab)
	{
		delete [] g_pDockPriceTab;
		g_pDockPriceTab = NULL;
	}
#else
	g_SubWorldSet.Close();
	g_ScenePlace.ClosePlace();
	if (g_pNpcLevelScript)
	{
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}
	if (g_pAdjustColorTab)
	{
		delete []g_pAdjustColorTab;
		g_pAdjustColorTab = NULL;
		g_ulAdjustColorCount = 0;
	}
#endif

    g_UnInitMath();
}

#ifdef _SERVER
void g_SetServer(LPVOID pServer)
{
	g_pServer = reinterpret_cast< IServer * >(pServer);
}

#endif

#ifndef _SERVER
void g_SetClient(LPVOID pClient)
{
	g_pClient = reinterpret_cast< IClient * >(pClient);
}

unsigned int	InitAdjustColorTab()
{
	g_pAdjustColorTab = NULL;
	g_ulAdjustColorCount = 0;
	KTabFile TabFile;
	if (!TabFile.Load(ADJUSTCOLOR_TABFILE))
	{
		_ASSERT(0);
		g_DebugLog("ÎÞ·¨´ò¿ª%s", ADJUSTCOLOR_TABFILE);
		return 0;
	}

	int nHeight = TabFile.GetHeight() - 1;
	
	if (nHeight <= 0)
		return 0;

	g_pAdjustColorTab = (unsigned int *)new unsigned long [nHeight];
	g_ulAdjustColorCount = nHeight;

	for (int i = 0; i < nHeight; i ++)
	{
		BYTE bAlpha = 0;
		BYTE bRed	= 0;
		BYTE bGreen	= 0;
		BYTE bBlue	= 0;
		int nAlpha;
		int nRed;
		int nGreen;
		int nBlue;
		TabFile.GetInteger(i + 2, "ALPHA", 0x000000ff, &nAlpha);
		nAlpha	&= 0xff;
		TabFile.GetInteger(i + 2,"RED", 0, &nRed);
		nRed	&= 0xff;
		TabFile.GetInteger(i + 2,"GREEN",  0, &nGreen);
		nGreen	&= 0xff;
		TabFile.GetInteger(i + 2,"BLUE",  0, &nBlue);
		nBlue	&= 0xff;
		unsigned long ulAdjustColor = nAlpha << 24 | nRed << 16 | nGreen << 8 | nBlue;
		g_pAdjustColorTab[i] = ulAdjustColor;
	}
	return g_ulAdjustColorCount;
}
#endif

BOOL InitGameSetting()
{
	if (!g_MapTraffic.Load(MINIMAP_SETTING_FILE_INI))
	{
		g_DebugLog("Can not load %s", MINIMAP_SETTING_FILE_INI);
	}
	if (g_GameSetting.Load(GAME_SETTING_FILE_INI))
	{
		g_GameSetting.GetInteger("ServerConfig", "MaxOptMultiply", 1, &g_MaxOptMultiply);
		g_GameSetting.GetInteger("ServerConfig", "xMethod", 1, &g_xMethod);
#ifdef _SERVER
		g_GameSetting.GetInteger("ServerConfig", "ExpRate", 1, &g_ExpRate);
		g_GameSetting.GetInteger("ServerConfig", "MoneyRate", 1, &g_MoneyRate);
		g_GameSetting.GetInteger("ServerConfig", "Skill90Rate", 0, &g_Skill90ExpRate);					//#trong file config khong duoc duoi 100
		g_GameSetting.GetInteger("ServerConfig", "Skill120Rate", 0, &g_Skill120ExpRate);			//#trong file config khong duoc duoi 100
		g_GameSetting.GetInteger("ServerConfig", "NotAddNpcNormal", 1, &g_NotAddNpcNormal); //#cÊu h×nh kh«ng add npc normal 1 lµ kh«ng add 0 lµ cho phÐp add
#endif
		g_GameSetting.GetInteger("ServerConfig", "WriteScriptNpcLog", 0, &g_WriteScriptNpcLog);	//cÊu h×nh ghi log ®­êng dÉn script khi ®èi tho¹i víi npc
	}
	else
	{
		g_DebugLog("Can not load %s", GAME_SETTING_FILE_INI);
	}
	return TRUE;
}

//---------------------------------------------------------------------------
// [AutoLog 21/08] Xem mo ta o KCore.h.
// - Doc cong tac MOT LAN tu Config.ini [Client] AutoLog (thu muc lam viec cua
//   tien trinh: Game.exe -> bin\client, GameServer -> bin\server).
// - Moi dong tu them 't=<ms> pid=<pid> ' de so khop giua cac dong/tien trinh.
// - Chong phinh: toi da 600 dong/giay (dong bi bo duoc dem va bao lai o dong ke
//   tiep), va xoay tep khi > 64 MB (doi ten thanh jx_auto.log.1).
//---------------------------------------------------------------------------
static int   s_nAutoLog = -1;		// -1 = chua doc cau hinh
static DWORD s_uAutoLogSec = 0;	// giay dang dem
static int   s_nAutoLogCnt = 0;	// so dong da ghi trong giay do
static int   s_nAutoLogDrop = 0;	// so dong bi bo vi vuot tran
static FILE* s_pAutoLogFile = NULL;	// tep log giu mo san (tranh fopen moi dong)
static int   s_nAutoLogSince = 0;	// so dong ke tu lan flush truoc
static UINT  s_uAutoLogFlush = 0;	// moc flush gan nhat

void g_AutoLogSet(int nOn)
{
	s_nAutoLog = nOn ? 1 : 0;
}

int g_AutoLogOn()
{
#ifdef _SERVER
	return 0;	// (21/08 - yeu cau chu game) log CHI cho client, ban server KHONG ghi/in gi
#else
	if (s_nAutoLog < 0)
	{
#ifdef WIN32
		s_nAutoLog = (int)GetPrivateProfileIntA("Client", "AutoLog", 0, ".\\Config.ini");
#else
		s_nAutoLog = 0;
#endif
	}
	return s_nAutoLog;
#endif
}

void g_AutoLog(const char* szFmt, ...)
{
	if (!g_AutoLogOn())
		return;
	DWORD uNow = timeGetTime();
	DWORD uSec = uNow / 1000;
	if (uSec != s_uAutoLogSec)
	{
		s_uAutoLogSec = uSec;
		s_nAutoLogCnt = 0;
	}
	if (s_nAutoLogCnt >= 600)
	{
		++s_nAutoLogDrop;
		return;
	}
	++s_nAutoLogCnt;
	char szLine[1024];
	va_list va;
	va_start(va, szFmt);
	int n = _vsnprintf(szLine, sizeof(szLine) - 2, szFmt, va);
	va_end(va);
	szLine[sizeof(szLine) - 1] = 0;	// _vsnprintf khong bao dam ket NUL khi day buffer
	if (n < 0)
		szLine[sizeof(szLine) - 2] = 0;
	// giu tep MO SAN: fopen/fclose moi dong ton 30-200us, 600 dong/giay se lam lech
	// chinh cai dang do (bot khung / danh miss). Chi flush dinh ky.
	if (!s_pAutoLogFile)
	{
		s_pAutoLogFile = fopen("jx_auto.log", "a");
		if (!s_pAutoLogFile)
		{
			s_nAutoLog = 0;	// khong mo duoc tep -> tat han, khoi thu lai moi dong
			return;
		}
	}
	FILE* pLog = s_pAutoLogFile;
	if (s_nAutoLogDrop > 0)
	{
		fprintf(pLog, "t=%u pid=%u [AUTOLOG] bo qua %d dong (tran 600 dong/giay)\n",
			uNow, (unsigned int)GetCurrentProcessId(), s_nAutoLogDrop);
		s_nAutoLogDrop = 0;
	}
	fprintf(pLog, "t=%u pid=%u %s\n", uNow, (unsigned int)GetCurrentProcessId(), szLine);
	long nSize = ftell(pLog);
	++s_nAutoLogSince;
	if (s_nAutoLogSince >= 50 || (DWORD)(uNow - s_uAutoLogFlush) >= 500)
	{
		fflush(pLog);
		s_nAutoLogSince = 0;
		s_uAutoLogFlush = uNow;
	}
	if (nSize > 64 * 1024 * 1024)
	{
		fclose(pLog);
		s_pAutoLogFile = NULL;
		remove("jx_auto.log.1");
		rename("jx_auto.log", "jx_auto.log.1");
	}
}

int PositionToRoom(int Place)
{
	switch (Place)
	{
		case pos_hand:
		case pos_equip:
		case pos_equiproom:
			return room_equipment;
		case pos_equiproomex:
			return room_equipmentex;
		case pos_repositoryroom:
			return room_repository;
		case pos_traderoom:
		case pos_gambleroom:
			return room_equipment;
		case pos_trade1:
			return room_equipment;
		case pos_immediacy:
			return room_immediacy;
		case pos_give:
			return room_give;
		case pos_affairitem:
			return room_affairitem;
		default:
			return room_equipment;
	}
	return room_equipment;
}
