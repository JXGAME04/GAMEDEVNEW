//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
// File:	KCore.h
// Date:	2020.08.08
// Code:	Fong Ki“u
// Desc:	Header File
//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN

#ifndef KCore_H
#define KCore_H
//---------------------------------------------------------------------------
#ifdef _STANDALONE
#define CORE_API
#else
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif
#endif
//---------------------------------------------------------------------------
#define	DIR_DOWN		0
#define	DIR_LEFTDOWN	1
#define	DIR_LEFT		2
#define	DIR_LEFTUP		3
#define	DIR_UP			4
#define	DIR_RIGHTUP		5
#define	DIR_RIGHT		6
#define	DIR_RIGHTDOWN	7
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KMemBase.h"
#include "KStrBase.h"
#ifndef _SERVER
#include "KSpriteCache.h"
#include "KFont.h"
#endif

#include "KTabFile.h"
#include "KProtocol.h"
#include "KEngine.h"
#include "KScriptList.h"
#include "KScriptCache.h"
#include "KSkillManager.h"
#include "MyAssert.H"
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
class ISkill ;
#ifdef _SERVER
#ifdef _STANDALONE
#include "IServer.h"
#else
#include "../../Headers/IServer.h"
#endif
#else
#include "../../Headers/IClient.h"
#include "KMusic.h"
#include "KSoundCache.h"
#endif

#pragma warning (disable: 4512)
#pragma warning (disable: 4786)
#define TASKCONTENT
//#define		BMPMAPDEBUG
//---------------------------------------------------------------------------
#ifdef TOOLVERSION
extern CORE_API int g_ScreenX;
extern CORE_API int g_ScreenY;
#endif

#define ITOA(NUMBER)  #NUMBER

#define __TEXT_LINE__(LINE) ITOA(LINE)

//#ifdef _DEBUG
#define ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : °ÔATTENTION°Ô °˙ "#MSG
//#else
//#define ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ATTENTION!! error: "#MSG
//#endif

#define	NET_DEBUG
extern CORE_API	KTabFile		g_OrdinSkillsSetting, g_MisslesSetting;
extern CORE_API	KTabFile		g_MeridiantSetting;
extern CORE_API	KTabFile		g_SkillLevelSetting;
extern CORE_API	KTabFile		g_NpcSetting;
extern CORE_API	KTabFile		g_NpcImageSetting;
extern KTabFile					g_RankTabSetting, g_ReBornSetting;
extern KTabFile					g_MaskChangeRes;
extern KIniFile 				g_GameSetting;
extern KIniFile 				g_MapTraffic;
#ifdef _SERVER
extern KLuaScript * 			pTimeScript;
#endif

extern int				g_MaxOptMultiply;
extern int				g_xMethod;
#ifdef _SERVER
extern int		 		g_ExpRate;
extern int		 		g_MoneyRate;
extern int		 		g_Skill90ExpRate;
extern int		 		g_Skill120ExpRate;
extern int		 		g_NotAddNpcNormal;
extern KLuaScript * 	g_pStartScript;
#endif

extern int		 		g_WriteScriptNpcLog;

#ifdef TOOLVERSION
#ifndef __linux
extern CORE_API	KSpriteCache	g_SpriteCache;
#endif
#endif
#ifndef _SERVER
extern KSoundCache		g_SoundCache;
extern KMusic			*g_pMusic;

extern unsigned int	* g_pAdjustColorTab;
extern unsigned int g_ulAdjustColorCount;

//#define	defPLAY_MUSIC(lpszMusicName, nVolume, bLoopFlag)	\
//		if (g_pMusic)\
//		{\
//			g_SetFilePath("\\");\
//			g_pMusic->Open(lpszMusicName);\
//			g_pMusic->SetVolume(nVolume);\
//			g_pMusic->Play(bLoopFlag);\
//		}
#endif

//#ifdef _DEBUG
extern CORE_API BOOL			g_bDebugScript;
//#endif
extern KTabFile		g_NpcKindFile; 

#ifndef _SERVER
extern BOOL g_bUISelIntelActiveWithServer;
extern BOOL g_bUISpeakActiveWithServer;
extern int g_bUISelLastSelCount;
#endif

#ifdef _SERVER

enum DBMESSAGE
{
	DBMSG_PUSH,
	DBMSG_POP,
};

enum DBI_COMMAND
{
	DBI_PLAYERSAVE,  //1 
	DBI_PLAYERLOAD,
	DBI_PLAYERDELETE,
	DBI_GETPLAYERLISTFROMACCOUNT,
};
BOOL CORE_API g_AccessDBMsgList(DBMESSAGE Msg,  int* pnPlayerIndex, DBI_COMMAND * pnDBICommand, void ** ppParam1, void ** ppParam2);
extern KLuaScript g_WorldScript;
extern KList g_DBMsgList;

class KDBMsgNode :public KNode
{
public:
	void * pParam1;
	void * pParam2;
	int	   nPlayerIndex;
	DBI_COMMAND Command;
	KDBMsgNode(){pParam1 = pParam2 = NULL; nPlayerIndex =  0;	Command = DBI_PLAYERSAVE;};
};

#endif

#ifndef _SERVER
class KImageNode : public KNode
{
public:
	char	m_szFile[32];
	int		m_nFrame;
	int		m_nXpos;
	int		m_nYpos;
};
#endif

#ifndef _SERVER
extern char* g_GetStringRes(int nStringID, char * szString, int nMaxLen);
#endif

//---------------------------------------------------------------------------
// [AutoLog 21/08] Log chan doan cho auto (danh / nhat do / di chuyen / skill /
// dong bo toa do). BAT bang Config.ini muc [Client] AutoLog = 1; mac dinh 0 =
// TAT hoan toan (g_AutoLogOn() tra 0 -> khong dinh dang chuoi, khong mo tep).
// Client: Config.ini [Client] AutoLog=1 -> ghi 'jx_auto.log'.
// Server: config.ini [AutoLog] On=1 -> ghi 'jx_auto_server.log'; [AutoLog] Name=<ten
//         nhan vat> de CHI ghi don danh lien quan nhan vat do (de trong = ghi tat ca).
// Ca hai phia CHI GHI RA TEP, khong in ra man hinh/cua so nao. Tu xoay khi tep > 64 MB.
// Dung: if (g_AutoLogOn()) g_AutoLog("[TAG] npc=%u d=%d", u, d);
//---------------------------------------------------------------------------
// CORE_API: S3Client.cpp (Game.exe) cung dat AUTOLOG_EVERY nen hai ham nay phai
// duoc XUAT ra khoi CoreClient.dll; thieu no thi Game.exe khong lien ket duoc
// (LNK2019 g_AutoLogOn / g_AutoLog) => moi diem log ben S3Client thanh ma chet.
CORE_API int  g_AutoLogOn();
CORE_API void g_AutoLog(const char* szFmt, ...);
void g_AutoLogSet(int nOn);
int  g_AutoLogWho(const char* szName);	// (server) loc theo ten nhan vat, xem KCore.cpp
int  g_AutoLogWhoIdx(int nNpcIdx);	// (server) loc theo TEN cua Npc[chi so]; TU kiem chi so, cai o KNpc.cpp
// Tien ich: AUTOLOG(...) = ghi neu dang bat; AUTOLOG_EVERY(ms, ...) = tiet che theo
// thoi gian (moi diem goi co bien static rieng nho khoi do{}while(0)).
#define AUTOLOG(...)              do { if (g_AutoLogOn()) g_AutoLog(__VA_ARGS__); } while (0)
#define AUTOLOG_EVERY(ms, ...)    do { static DWORD s_uAutoLogT = 0; if (g_AutoLogOn()) { DWORD uAutoLogNow = timeGetTime(); if ((DWORD)(uAutoLogNow - s_uAutoLogT) >= (DWORD)(ms)) { s_uAutoLogT = uAutoLogNow; g_AutoLog(__VA_ARGS__); } } } while (0)
// AUTOLOG_IDX(idx, ...) / AUTOLOG_IDX_EVERY(idx, ms, ...): chi ghi khi Npc[idx].Name TRUNG voi
// [AutoLog] Name= trong config.ini (de trong = ghi tat ca). Bat buoc o SERVER vi may chu dang chay
// ~1000 bot, bot nao cung IsPlayer() => khong loc thi log cua nhan vat that bi che lap.
// Loi the phu: doi so CHI duoc tinh khi dieu kien dung => Npc[idx] khong bi deref voi chi so xau.
#define AUTOLOG_IDX(idx, ...)             do { if (g_AutoLogWhoIdx(idx)) g_AutoLog(__VA_ARGS__); } while (0)
#define AUTOLOG_IDX_EVERY(idx, ms, ...)   do { static DWORD s_uAutoLogTI = 0; if (g_AutoLogWhoIdx(idx)) { DWORD uAutoLogNowI = timeGetTime(); if ((DWORD)(uAutoLogNowI - s_uAutoLogTI) >= (DWORD)(ms)) { s_uAutoLogTI = uAutoLogNowI; g_AutoLog(__VA_ARGS__); } } } while (0)

BOOL InitGameSetting();
BOOL InitSkillSetting();
BOOL InitMissleSetting();
BOOL InitNpcSetting();
void g_ReleaseCore();
int	 PositionToRoom(int Place);
inline int GetRandomNumber(int nMin, int nMax)
{
	return g_Random(nMax - nMin + 1) + nMin;
}
#ifdef _SERVER
void g_SetServer(LPVOID pServer);
extern IServer* g_pServer;
#else
void g_SetClient(LPVOID pClient);
extern IClient* g_pClient;
//extern BOOL	g_bPingReply;
#endif

class ThreadPool {
public:
	ThreadPool(size_t threadCount) {
		stop = false;
		for (size_t i = 0; i < threadCount; ++i) {
			workers.emplace_back([this]() {
				while (true) {
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(queueMutex);
						condition.wait(lock, [this]() { return stop || !tasks.empty(); });

						if (stop && tasks.empty())
							return;

						task = std::move(tasks.front());
						tasks.pop();
					}

					task();
				}
				});
		}
	}

	void enqueue(std::function<void()> task) {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			tasks.push(std::move(task));
		}
		condition.notify_one();
	}

	void wait() {
		while (true) {
			std::unique_lock<std::mutex> lock(queueMutex);
			if (tasks.empty())
				break;
			lock.unlock();
			std::this_thread::yield();
		}
	}

	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
			worker.join();
	}

private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop;
};

const int THREAD_COUNT = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency()/2 : 4; // Default to 4 if hardware concurrency is not available
static ThreadPool pool(THREAD_COUNT);
//---------------------------------------------------------------------------
#endif
