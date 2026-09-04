// S3Client.cpp : Defines the entry point for the application.

#include "KWin32.h"
#include "KCore.h"
#include "S3Client.h"
#include "KWin32Wnd.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "Ui/UiShell.h"
#include "Ui/PerfHud.h"
#include "CrashLog.h"
#include "NetConnect/NetConnectAgent.h"
#include "TextCtrlCmd/TextCtrlCmd.h"
#include "KPakList.h"
#include "Ui/Elem/TextPic.h"
#include "Ui/Elem/UiCursor.h"
#include "Ui/Elem/SpecialFuncs.h"
#include "Ui/FilterTextLib.h"
#include "Ui/ChatFilter.h"
#include "Ui/uibase.h"
#include "ErrorCode.h"
#include "S3Config.h"
#include "AntiHack/DumpMemory/DumpMemory.h"
#include "AntiHack/DetectWindowsTitle/DetectWindowsTitle.h"
#include "AntiHack/DetectWindowsText/DetectWindowsText.h"
#include "AntiHack/CheckSumCRC/CheckSumCRC.h"
#include "AntiHack/DetectHide/DetectHide.h"
#include "AntiHack/DetectWindowsClassName/DetectWindowsClassName.h"
#include "AntiHack/ExistMainName/ExistMainName.h"
//#include "AntiHack/ProtectBypassMainName/ProtectBypassMainName.h"
#include "AntiHack/Splash/Splash.h"
#include "Ui/UiCase/UiJxrPlayer.h"
#include "JxReplay.h"
#include "Ui/TrayMode.h"
//#include "stdafx.h"
#include "Ui/UiCase/UiInit.h"
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
//#include <tchar.h>
#include "Dbghelp.h"
//for auto
#include "Ui/UiCase/UiPlayerBar.h"
#include "Ui/UiCase/UiMsgCentrePad.h"
#include "Ui/UiCase/UiFaceSelector.h"
#include "Ui/UiCase/UiInformation.h"
#include "Ui/UiCase/UiSelServer.h"
#include "Ui/UiCase/UiLogin.h"
#include "Ui/UiCase/UiSelPlayer.h"
#include "Ui/Elem/Wnds.h"
#define ClientVersion
KMyApp		MyApp;
HINSTANCE	hInst;
KPakList	g_PakList;
CFilterTextLib g_libFilterText;
CChatFilter g_ChatFilter;



#define	QUIT_QUESTION_ID	"22"
#define	GAME_TITLE			"23"

#define REPRESENT_MODULE_2			"Represent2.dll"
#define REPRESENT_MODULE_3			"Represent3.dll"
#define CREATE_REPRESENT_SHELL_FUN	"CreateRepresentShell"
#define	GAME_FPS			18							//khung hinh game chi so khung hinh tren giay mac dinh la 18 edit by phong kieu 60FPS tieu chuan game hien nay
#define CONFIG_FILE_PATH	"Config.ini"			//duong dan file config.ini
//static int m_PaintStep = GAME_FPS / 18;
static int	g_nPaintFps = 30;		// paint frames per second, config.ini [Client] PaintFps; 0 = paint locked to logic tick (legacy)
static int	g_nPaintInterp = 1;		// config.ini [Client] PaintInterp; 1 = interpolate drawn NPC positions between logic ticks
int	g_nPaintLog = 0;		// config.ini [Client] PaintLog; 1 = write jx_paint.log frame-time probe
//int gameNumber = 0; // Game number, initialized to 0
//Represent
struct iRepresentShell* g_pRepresentShell = NULL;
struct IInlinePicEngineSink* g_pIInlinePicSink = NULL;
iCoreShell* g_pCoreShell = NULL;
KMusic* g_pMusic = NULL;

#define	DYNAMIC_LINK_REPRESENT_LIBRARY

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
static HMODULE		l_hRepresentModule = NULL;
int					g_bRepresent3 = false;
#endif

int					g_bScreen = true;
char				g_szGameName[64] = "Vo Lam Truyen Ky";//tieu de tang them 64 bit game edit by phong kieu

KClientCallback g_ClientCallback;

// Default resolution values
int SCREEN_WIDTH = 800;  // Default width
int SCREEN_HEIGHT = 600; // Default height

void LoadResolutionFromConfig() {
	char configPath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, configPath);
	strcat(configPath, "\\");
	strcat(configPath, CONFIG_FILE_PATH);

	// Read width and height from the config file
	SCREEN_WIDTH = GetPrivateProfileInt("Resolution", "Width", SCREEN_WIDTH, configPath);
	SCREEN_HEIGHT = GetPrivateProfileInt("Resolution", "Height", SCREEN_HEIGHT, configPath);
	std::cout << "Loaded Resolution: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
}

/*
 * Add this macro by liupeng on 2003.3.20
 * This macro is helper that can judge some legal character
 */
#define _private_IS_SPACE(c)   ((c) == ' ' || (c) == '\r' || (c) == '\n' || (c) == '\t' || (c) == 'x')
#define IS_SPACE(c)	_private_IS_SPACE(c)

#pragma warning (disable: 4305)
#pragma warning (disable: 4309)
//auto
const char* MMF_NAME_SERVER = "Local\\Auto_Name_MMFSV_";
const char* MMF_NAME_CLIENT = "Local\\Auto_Name_MMFCL_";
const char* EVT_CLRECV = "Local\\Auto_EventClientRecv_";
const char* EVT_SVRECV = "Local\\Auto_EventServerRecv_";
HANDLE g_hMap = NULL;
SharedState* g_pState = NULL;
HANDLE g_hEventRecv = NULL;
HANDLE g_hRepMap = NULL;
SharedState* g_pRepState = NULL;
HANDLE g_hRepEvent = NULL;
UINT g_CurNum = 0;
UINT g_CurSize = 0;
BYTE g_Buffer[SHARED_SIZE];
static int g_DrawVision = 0;
static UINT g_DrawVisionTime = 0;
static int g_ALGStep = 0;
static UINT g_AGLNextTime = 0;

int GenerateMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS lpExceptionPointer, PWCHAR pwAppName)
{
	BOOL bOwndumpFile = FALSE;
	HANDLE hDumpFile = hFile;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
	typedef BOOL(WINAPI* MiniDumpWriteDumpT) (
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);

	MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
	HMODULE hdbgHelp = LoadLibrary("DbgHelp.dll");

	if (hdbgHelp)
		pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hdbgHelp, "MiniDumpWriteDump");

	if (pfnMiniDumpWriteDump)
	{
		if (hDumpFile == NULL || hDumpFile == INVALID_HANDLE_VALUE)
		{
			TCHAR szFileName[MAX_PATH] = { 0 };
			TCHAR* szVersion = "v1.0";
			TCHAR dwBufferSize = MAX_PATH;
			SYSTEMTIME stLocalTime;
			GetLocalTime(&stLocalTime);

			CreateDirectory("DumpInfo", NULL);
			wsprintf(szFileName, "DumpInfo\\%s-%s-%04d%02d%02d-%02d%02d%02d-%ld%ld.dmp"
				, "client", szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());

			hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

			bOwndumpFile = TRUE;
			OutputDebugString(szFileName);
		}

		if (hDumpFile != INVALID_HANDLE_VALUE)
		{
			ExpParam.ThreadId = GetCurrentThreadId();
			ExpParam.ExceptionPointers = lpExceptionPointer;
			ExpParam.ClientPointers = FALSE;

			pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
				hDumpFile, MiniDumpWithDataSegs, (lpExceptionPointer ? &ExpParam : NULL), NULL, NULL);

			if (bOwndumpFile)
				CloseHandle(hDumpFile);
		}
	}

	if (hdbgHelp != NULL)
		FreeLibrary(hdbgHelp);

	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI ExeptionFillert(LPEXCEPTION_POINTERS lpExceptionInfo)
{
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return GenerateMiniDump(NULL, lpExceptionInfo, L"client");
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	// bat bo ghi log crash cang som cang tot (ghi jx_crash.log canh Game.exe)
	CrashLog_Install();

	SetUnhandledExceptionFilter(ExeptionFillert);

	//int* pbadPtr;	//add by tuanln test
	//*pbadPtr = 0;

#ifdef	TRUE

	bool bOpenTracer = false;

	while (lpCmdLine[0] == '-' || lpCmdLine[0] == '/')
	{
		lpCmdLine++;

		switch (*lpCmdLine++)
		{
		case 'c':
		case 'C':
			bOpenTracer = true;
			break;
		}

		while (IS_SPACE(*lpCmdLine))
		{
			lpCmdLine++;
		}
	}

	if (bOpenTracer)
	{
		AllocConsole();
	}

#endif // End of this function

	if (ANTI_DUMP_MEMORY)
	{
		SystemProcessesScan();
		ProtectionMainDumpMemory();
	}
	if (ANTI_DETECT_WINDOWS_TITLE)
	{
		ThreadDetectWindowsTitle();
	}
	if (ANTI_DETECT_WINDOWS_TEXT)
	{
		ThreadDetectWindowsText();
	}
	if (CHECKSUM_FILE)
	{
		ThreadCheckSumCRC();
	}
	if (ANTI_DETECT_HIDE)
	{
		ThreadDetectHide();
	}
	if (ANTI_CLASS_NAME_SCAN)
	{
		ThreadWindowsClassName();
	}
	if (ANTI_EXIST_MAIN_NAME)
	{
		ThreadExistMainName();
	}
	if (ANTI_BYPASS_MAIN_NAME)
	{
		//ThreadProtectBypassMainName();
	}
	if (PAINT_SPLASH)
	{
		CSplash splash1(TEXT(".\\GameGuard/logo.bmp"), RGB(128, 128, 128));
		splash1.ShowSplash();
		Sleep(SPLASH_TIME);
		//  Close the splash screen
		splash1.CloseSplash();
	}

	hInst = hInstance;
	LoadResolutionFromConfig();
	SetEngineResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	if (MyApp.Init(hInstance))
		MyApp.Run();

#ifdef TRUE

	if (bOpenTracer)
	{
		FreeConsole();
	}

#endif
	Error_Box();

	return 0;
}

KMyApp::KMyApp()
{
	m_pInlinePicSink = NULL;
}

BOOL InitRepresentShell(BOOL bFullScreen, int nWidth, int nHeight)
{
	Error_SetErrorString(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2);
	if (g_pRepresentShell == NULL)
	{
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
		if (l_hRepresentModule == NULL && (l_hRepresentModule = LoadLibrary(g_bRepresent3 ? REPRESENT_MODULE_3 : REPRESENT_MODULE_2)) == NULL)
		{
			if (g_bRepresent3)	// [REP3 03/09] thieu Represent3.dll -> lui ve Represent2
			{
				AUTOLOG("[REP3] khong nap duoc Represent3.dll -> lui ve Represent2");
				g_bRepresent3 = false;
				return InitRepresentShell(bFullScreen, nWidth, nHeight);
			}
			Error_SetErrorCode(ERR_T_LOAD_MODULE_FAILED);
			return FALSE;
		}
		fnCreateRepresentShell pCreate = (fnCreateRepresentShell)GetProcAddress(l_hRepresentModule, CREATE_REPRESENT_SHELL_FUN);
		if (pCreate == NULL || (g_pRepresentShell = pCreate()) == NULL)
		{
			Error_SetErrorCode((pCreate == NULL) ? ERR_T_MODULE_UNCORRECT : ERR_T_MODULE_INIT_FAILED);
			return FALSE;
		}
#else
		g_pRepresentShell = CreateRepresentShell();
#endif
	}
	// [REP3 03/09] bo hack keo cua so cao them 40 px (Represent3 tu dat cua so dung nWidth x nHeight, Present 1:1)
	if (g_pRepresentShell->Create(nWidth, nHeight, bFullScreen != 0))
	{
		// He GHI / PHAT LAI ban dien .jxr. Ban tham chieu cung nap JXReplay.dll
		// ngay trong luong khoi tao Represent (KMyApp::InitRepresent 0x0056D615),
		// KHONG nap tre luc bam nut. Thieu DLL thi ham tra false va game chay
		// binh thuong nhu cu, chi la khong co tinh nang replay.
		JxReplay_Init();
		return TRUE;
	}
	else
	{
		// [REP3 03/09] Represent3 khoi tao that bai (thieu d3d9/card yeu) -> lui ve Represent2, khong thoat game
		if (g_bRepresent3)
		{
			AUTOLOG("[REP3] Represent3 Create that bai -> lui ve Represent2");
			g_pRepresentShell->Release();
			g_pRepresentShell = NULL;
#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
			if (l_hRepresentModule) { FreeLibrary(l_hRepresentModule); l_hRepresentModule = NULL; }
#endif
			g_bRepresent3 = false;
			return InitRepresentShell(bFullScreen, nWidth, nHeight);
		}
		Error_SetErrorCode(ERR_T_REPRESENT2_INIT_FAILED);
		return FALSE;
	}
}

#include <ctime>

void gen_random(char* s, size_t len) {
	for (size_t i = 0; i < len; ++i) {
		int randomChar = rand() % (26 + 26 + 10);
		if (randomChar < 26)
			s[i] = 'a' + randomChar;
		else if (randomChar < 26 + 26)
			s[i] = 'A' + randomChar - 26;
		else
			s[i] = '0' + randomChar - 26 - 26;
	}
	s[len] = 0;
}

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "Ui/UiCase/UiTeamManager2.h"
#include "Ui/UiCase/UiTargetInfo.h"
using namespace std;

void KMyApp::AddTrayIcon(HWND hWnd, LPCSTR tip)
{
    NOTIFYICONDATAA nid;
	memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAYICON;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WMAPP_TRAY;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(SWORD_ICON));
    strcpy(nid.szTip, tip);
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void KMyApp::AddTrayIconHide(HWND hWnd, LPCSTR tip)
{
	g_bTrayActive = TRUE;
	strcpy(g_szTip, tip);
	AddTrayIcon(hWnd, tip);
	ShowWindow(hWnd, SW_HIDE);
}

BOOL KMyApp::GameInit()
{
	g_bTrayActive = FALSE;
	g_uTaskbarCreated = RegisterWindowMessageA("TaskbarCreated");

	DWORD aPid = GetCurrentProcessId();
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
	{
		std::wcout << "can't get a process snapshot ";
		return 0;
	}

	for (BOOL bok = Process32First(processesSnapshot, &processInfo); bok; bok = Process32Next(processesSnapshot, &processInfo))
	{
		if (aPid == processInfo.th32ProcessID)
		{
			std::wcout << "found running process: " << processInfo.szExeFile;
			CloseHandle(processesSnapshot);

			char strCounterG[32];
			sprintf(strCounterG, "%s", processInfo.szExeFile);
			//MessageBox(g_GetMainHWnd(), strCounterG, strCounterG, MB_ICONEXCLAMATION);

			if (strcmp("Game.exe", strCounterG) != 0)
			{
				return FALSE;
			}
			//return processInfo.szExeFile;
		}

	}
	//std::wcout << "no process with given pid" << aPid;
	//CloseHandle(processesSnapshot);
	//return std::wstring();


	Error_SetErrorString("KMyApp::GameInit");
#ifdef KUI_USE_HARDWARE_MOUSE

	ShowMouse(TRUE);

#else   // KUI_USE_HARDWARE_MOUSE

	ShowMouse(FALSE);

#endif

	g_SetRootPath(NULL);
	g_SetFilePath("\\");

	g_PakList.Open("\\package.ini");//edit by phong kieu

	KIniFile* pSetting = g_UiBase.GetCommConfigFile();
	if (pSetting)
	{
		pSetting->GetString("Main", "GameName", "Vo Lam Truyen Ky", g_szGameName, sizeof(g_szGameName));
		SetWindowText(g_GetMainHWnd(), g_szGameName);
	}

#ifdef _DEBUG
	g_FindDebugWindow("#32770", "DebugWin");
#endif

	KIniFile	IniFile;
	if (!IniFile.Load("\\config.ini"))
	{
		Error_SetErrorCode(ERR_T_FILE_NO_FOUND);
		Error_SetErrorString("\\config.ini");
		return FALSE;
	}

#ifdef _DEBUG
	BOOL		bCursor = FALSE;
	if (IniFile.GetInteger("Client", "ShowCursor", 0, &bCursor))
		ShowMouse(TRUE);
#endif

	IniFile.GetInteger("Client", "FullScreen", FALSE, &g_bScreen);

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	IniFile.GetInteger("Client", "Represent", 2, &g_bRepresent3);
	g_bRepresent3 = (g_bRepresent3 == 3);									//edit by phong kieu chi cho chay 2d mac dinh g_bRepresent3 == 3
#endif

	IniFile.GetInteger("Client", "PaintFps", 30, &g_nPaintFps);
	if (g_nPaintFps < 0)
		g_nPaintFps = 0;
	if (g_nPaintFps > 60)
		g_nPaintFps = 60;
	IniFile.GetInteger("Client", "PaintInterp", 1, &g_nPaintInterp);
	IniFile.GetInteger("Client", "PaintLog", 0, &g_nPaintLog);
	int nPerfHud = 0;
	IniFile.GetInteger("Client", "PerfHud", 0, &nPerfHud);
	PerfHud_SetEnable(nPerfHud);
	if (g_nPaintFps > 30)
		timeBeginPeriod(1);	// high paint rates need 1ms Sleep/wait resolution; paired with timeEndPeriod in GameExit

	char	szPath[MAX_PATH];
	if (IniFile.GetString("Client", "CapPath", "", szPath, sizeof(szPath)))
	{
		if (szPath[0])
		{
			SetScrPicPath(szPath);
		}
	}
	// [Client] RecPath cua nhanh quay video BandiCam cu da bi bo.
	// He replay .jxr luon luu vao <thu muc Game.exe>\JxRep\ giong ban tham chieu.


	IniFile.Clear();

	if (!g_libFilterText.Initialize()
		|| !g_ChatFilter.Initialize())
		return FALSE;
	LoadResolutionFromConfig();
	if (!InitRepresentShell(g_bScreen, SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		return FALSE;
	}

	if (!UiInit())
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("UiInit");
		return FALSE;
	}

	//[wxb 2003-6-23]
	m_pInlinePicSink = new KInlinePicSink;
	if (m_pInlinePicSink)
	{
		m_pInlinePicSink->Init(g_pRepresentShell);
		_ASSERT(NULL == g_pIInlinePicSink);
		g_pIInlinePicSink = m_pInlinePicSink;
	}

	UiSetScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	UiPaint(0);

	// init dsound
	m_Sound.Init();

	SetMultiGame(TRUE);

	if ((g_pCoreShell = CoreGetShell()) == NULL)
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("CoreGetShell");
		return false;
	}
	g_pCoreShell->SetRepresentShell(g_pRepresentShell);
	g_pCoreShell->SetMusicInterface((KMusic*)&m_Music);
	g_pCoreShell->SetCallDataChangedNofify(&g_ClientCallback);
	g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	g_pMusic = &m_Music;

	if (g_NetConnectAgent.Initialize() == 0)
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("NetConnectAgent");
		return FALSE;
	}

	m_GameCounter = 0;

	m_Timer.Start();

	SetMouseHoverTime(400);

	if (UiStart())
	{
		if(!InitMapping())
			return FALSE;
		return TRUE;
	}
	else
	{
		Error_SetErrorCode(ERR_T_MODULE_INIT_FAILED);
		Error_SetErrorString("UiStart");
		return FALSE;
	}
}


BOOL KMyApp::GameExit()
{
	if (g_nPaintFps > 30)
		timeEndPeriod(1);

	if (m_pInlinePicSink)
	{
		//[wxb 2003-6-23]
		m_pInlinePicSink->UnInit();
		delete m_pInlinePicSink;
		m_pInlinePicSink = NULL;
		g_pIInlinePicSink = NULL;
	}

	UiExit();

	g_pMusic = NULL;
	if (g_pCoreShell)
	{
		g_pCoreShell->SetRepresentShell(NULL);
		g_pCoreShell->SetClient(NULL);
		g_pCoreShell->SetMusicInterface(NULL);
		g_pCoreShell->Release();
		g_pCoreShell = NULL;
	}

	// PHAI dong phien ghi truoc khi giai phong shell, neu khong luong nen nen
	// cua jxreplay.dll bi giet giua chung va tep .jxr se hong.
	JxReplay_Exit();

	if (g_pRepresentShell)
	{
		g_pRepresentShell->Release();
		g_pRepresentShell = NULL;
	}

	g_NetConnectAgent.Exit();

	m_Music.Close();
	m_Sound.Exit();

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	if (l_hRepresentModule)
	{
		FreeLibrary(l_hRepresentModule);
		l_hRepresentModule = NULL;
	}
#endif

	::ShowCursor(TRUE);

	g_ChatFilter.Uninitialize();
	g_libFilterText.Uninitialize();
//clear auto
	if (g_pState) { UnmapViewOfFile(g_pState); g_pState = NULL; }
	if (g_hMap) { CloseHandle(g_hMap); g_hMap = NULL; }
    if (g_hEventRecv) { CloseHandle(g_hEventRecv); g_hEventRecv = NULL; }
	if (g_pRepState) { UnmapViewOfFile(g_pRepState); g_pRepState = NULL; }
	if (g_hRepMap) { CloseHandle(g_hRepMap); g_hRepMap = NULL; }
    if (g_hRepEvent) { CloseHandle(g_hRepEvent); g_hRepEvent = NULL; }

	return TRUE;
}

bool KMyApp::InitMapping()
{
	char Buffer[128];
	UINT pid = GetCurrentProcessId();
	sprintf(Buffer, "%s%u", MMF_NAME_SERVER, pid);
	g_hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_SIZE, Buffer);
    if (!g_hMap)
		return false;
	g_pState = (SharedState*)MapViewOfFile(g_hMap, FILE_MAP_ALL_ACCESS, 0,0,0);
    if (!g_pState)
		return false;
    // init struct once
	sprintf(Buffer, "%s%u", EVT_SVRECV, pid);
    g_hEventRecv = CreateEvent(NULL, FALSE, FALSE, Buffer); // auto-reset
	return true;
}

void SendInfoToTool(const void * const pData, const size_t &datalength)
{
	if(!g_pRepState || datalength <= 0)
		return;
	if(g_CurSize + datalength + sizeof(UINT) > SHARED_SIZE)
		return;
	memcpy(&g_Buffer[g_CurSize], pData, datalength);
	g_CurSize += datalength;
	++g_CurNum;
}

void KMyApp::AppSendInfoToTool(const void * const pData, const size_t &datalength)
{
	SendInfoToTool(pData, datalength);
}

void KMyApp::ExtAutoLogin(const IPCAutoLogin* pALg)
{
	if(!g_pCoreShell)
		return;
	if(g_ALGStep >= 100)
		return;
	KUiInit* pInit = NULL;
	KUiSelServer* pSelsv = NULL;
	if(pInit = KUiInit::GetIfVisible())
	{
		g_ALGStep = 1;
		pInit->AutoLgNextStep();
	}
	else if(pSelsv = KUiSelServer::GetIfVisible())
	{
		g_ALGStep = 2;
		pSelsv->AutoLgNextStep(pALg->nSelSvGroup, pALg->nSelServer);
		if(!KUiSelServer::GetIfVisible())
		{
			g_AGLNextTime = timeGetTime() + 4000;
		}
	}
	else if(g_ALGStep == 2)
	{
		KUiLogin* pLogin = NULL;
		if(pLogin = KUiLogin::GetIfVisible())
		{
			g_ALGStep = 3;
			pLogin->AutoLgNextStep(pALg->szAccount, pALg->szPassword);
			g_AGLNextTime = timeGetTime() + 4000;
		}
		else if(g_AGLNextTime < timeGetTime())
		{
			g_ALGStep = 100;
			PostQuitMessage(0);
			return;
		}
	}
	else if(g_ALGStep == 3)
	{
		KUiSelPlayer* pSelP = NULL;
		if(pSelP = KUiSelPlayer::GetIfVisible())
		{
			g_ALGStep = 4;
			g_AGLNextTime = timeGetTime() + 4000;
			if(!pSelP->AutoLgNextStep(pALg->szName))
			{
				g_ALGStep = 100;
				PostQuitMessage(0);
				return;
			}
		}
		else if(g_AGLNextTime < timeGetTime())
		{
			g_ALGStep = 100;
			PostQuitMessage(0);
			return;
		}
	}
	else if(g_ALGStep == 4)
	{
		UINT uID;
		g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, (unsigned int)&uID, 0);
		if(uID == pALg->dwID)
		{
			g_ALGStep = 100;
		}
		else if(g_AGLNextTime < timeGetTime())
		{
			g_ALGStep = 100;
			PostQuitMessage(0);
			return;
		}
	}
}

void KMyApp::SendAllCommand()
{
	if(!g_pRepState || g_CurSize <= 0)
		return;
	*(UINT*)g_pRepState = g_CurNum;
	memcpy((BYTE*)g_pRepState + sizeof(UINT), g_Buffer, g_CurSize);
	g_CurSize = 0;
	g_CurNum = 0;
	SetEvent(g_hRepEvent);
}

void KMyApp::ExtAutoLoop(const autoData* pApData)
{
	if(!g_pCoreShell)
		return;
	Wnd_SetPKKey(pApData->uFKey);
	AUTOLOG_EVERY(1000, "[AUTO-PASS] pass=%u t=%u pidx=%d onpk=%d fight=%d pick=%d fpick=%d datau=%d vis=%d near=%d pvis=%d skL=%d skR=%d lbtn=%d", m_GameCounter, timeGetTime(), g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0), pApData->bOnPK, pApData->bFight, pApData->bPickUp, pApData->bFollowPick, pApData->bDaTau, pApData->nVision, pApData->nNearDist, pApData->nPickVision, pApData->nSkillIdL, pApData->nSkillIdR, Wnd_IsLButtonDown());
	g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CHECKTIME, 0);
	if(pApData->bRevive)
	{
		if(PushReviveButton())
			return;
	}
	if(pApData->bOutWhenDis && !pApData->bOnPK)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_DISEXIT, 0))
		{
			PostQuitMessage(0);
			return;
		}
	}
	if(pApData->bOutTimer && !pApData->bOnPK)
	{
    	SYSTEMTIME	sTime;
		GetLocalTime(&sTime);
		if(sTime.wHour == pApData->nHour && sTime.wMinute == pApData->nMinute)
		{
			PostQuitMessage(0);
			return;
		}
	}
	AUTOLOG_EVERY(1000, "[AUTO-NOPLAYER] pass=%u t=%u pidx=%d reason=no-player-index", m_GameCounter, timeGetTime(), g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0));
	if(!g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0))
		return;
	// [DaTau] 0 = tha may; 1 = dang lam viec o thanh (bo MOVE/RETURN);
	// 2 = dang farm map nhiem vu (bo MOVE/RETURN + bo qua skip-goldboss)
	// [TongKim] uu tien cao nhat: toi khung gio thi may Tong Kim cam lai - Da Tau,
	// Hau can, di chuyen va moi dieu kien phu ve thanh deu nhuong (map bao danh
	// cam Than Hanh Phu). 0 = tha may; 1 = dang cam lai; 2 = dang trong tran.
	// [CongThanh] (03/09) may Cong Thanh Chien - goi TRUOC Tong Kim: tran cong thanh
	// moi tuan mot lan cho moi thanh, Tong Kim ngay 4 tran. Hai may loai tru nhau:
	// CT_Process tu nhuong khi Tong Kim dang cam lai (nTKHold), TK_Process nhuong khi
	// Cong Thanh dang cam lai (nCTHold). 0 = tha may; 1 = cam lai; 2 = trong tran.
	int nCT = 0;
	if(pApData->bCongThanh == 1)
		nCT = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CONGTHANH, (int)pApData);
	int nTK = 0;
	if(pApData->bTongKim == 1 && nCT == 0)
		nTK = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TONGKIM, (int)pApData);
	// [LienDau] cung mot the voi Tong Kim, nhung Tong Kim uu tien hon: chi goi may
	// Lien dau khi may Tong Kim dang THA MAY. nBS gop hai may lai (0 tha / 1 cam lai /
	// 2 dang trong san) de moi cong tac nhuong quyen ben duoi chi kiem MOT bien.
	int nLD = 0;
	if(pApData->bLienDau == 1 && nTK == 0 && nCT == 0)
		nLD = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_LIENDAU, (int)pApData);
	// [HoatDong] Bach Nhan / Bang Chien / Tin Su - chay khi Tong Kim va Lien dau tha may
	int nHD = 0;
	if((pApData->bHDBachNhan == 1 || pApData->bHDBangChien == 1 || pApData->bHDTinSu == 1)
		&& nTK == 0 && nLD == 0 && nCT == 0)
		nHD = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_HOATDONG, (int)pApData);
	// [SatThu] auto san boss Sat Thu + ghep Sat Thu Gian - chay khi Tong Kim,
	// Lien dau va Hoat dong deu tha may (uu tien thap nhat trong 4 may hoat dong).
	int nST = 0;
	if(pApData->bSatThu == 1 && nTK == 0 && nLD == 0 && nHD == 0 && nCT == 0)
		nST = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SATTHU, (int)pApData);
	const int nBS = nCT ? nCT : (nTK ? nTK : (nLD ? nLD : (nHD ? nHD : nST)));
	// [MapSuKien] (25/08) Dang dung tren MAP SU KIEN (Tong Kim, Phong Lang Do,
	// Tin Su, cac hoat dong bang hoi, Vuot ai, pho ban...)? Khi do MOI auto TU DO
	// phai nam im: khong Da Tau, khong tu di chuyen theo toa do, khong tu ve thanh
	// theo dieu kien mau/tui/tien. VAN cho danh tra + nhat do + an thuoc chay -
	// nguoi choi dang giua su kien thi van phai song duoc.
	// Bang map lay TU CHINH may chu (settings/map_type.txt -> KMapSuKien.h).
	const int nSK = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_MAPSUKIEN, 0);
	// (25/08) mot dong nhat ky cho CONG HOAT DONG: lan sau auto khong chay thi
	// nhin day la biet ngay may nao tra gi, thay vi phai mo tung ham ra doan.
	AUTOLOG_EVERY(5000, "[HD-GATE] nCT=%d nTK=%d nLD=%d nHD=%d nST=%d nBS=%d nSK=%d | bat: CT=%d TK=%d LD=%d BN=%d BC=%d TS=%d ST=%d DT=%d",
		nCT, nTK, nLD, nHD, nST, nBS, nSK, pApData->bCongThanh, pApData->bTongKim, pApData->bLienDau,
		pApData->bHDBachNhan, pApData->bHDBangChien, pApData->bHDTinSu,
		pApData->bSatThu, pApData->bDaTau);
	// [TongKim] dang cam lai (di bao danh / trong tran) thi TU bam nut hoi sinh du
	// nguoi choi khong bat 'Tu hoi sinh' - chet la chuyen thuong o Tong Kim.
	if(nBS && !pApData->bRevive)
	{
		if(PushReviveButton())
			return;
	}
	int nDT = 0;
	autoData sDTData;
	// (26/08) bo dem dung chung cho ban sao cau hinh theo nBS (cac truong hop
	// khong bao gio xay ra cung luc): nBS == 2 -> ep 'Duoi theo muc tieu' cho
	// may PK; nBS == 5 -> danh bang tab Chien dau, bo 'Bo qua boss vang';
	// bOnPK / phim tat PK -> cung ep duoi theo (chu game chot 26/08).
	autoData sBSData;
	// nSK: dang o map su kien thi KHONG chay Da Tau (vi du dang di Tong Kim /
	// Phong Lang Do / Tin Su / hoat dong bang hoi / Vuot ai)
	if(pApData->bDaTau == 1 && nBS == 0 && nSK == 0)	// so sanh ==1: WAuto.exe cu gui struct ngan, duoi buffer la rac
	{
		nDT = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_DATAU, (int)pApData);
		AUTOLOG("[DATAU-GATE] pass=%u t=%u nDT=%d bDaTau=%d skipboss=%d", m_GameCounter, timeGetTime(), nDT, pApData->bDaTau, pApData->bSkipGoldboss);
		if(nDT == 2)
		{
			memcpy(&sDTData, pApData, sizeof(autoData));
			sDTData.bSkipGoldboss = 0;	// Mat Chi chi roi tu boss
		}
	}
	if(pApData->bOutWhenTP && !pApData->bOnPK && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_EXIT, 0))
		{
			PostQuitMessage(0);
			return;
		}
	}
	int nParam[4];
	if(pApData->bCheckiLife)
	{
		nParam[0] = pApData->nIlifeCell1;
		nParam[1] = pApData->nIlifeCell2;
		nParam[2] = pApData->nIlifeCell3;
		if(nParam[2] < 200)
			nParam[2] = 200;
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PUMPLIFE, (int)&nParam);
	}
	if(pApData->bCheckiMana)
	{
		nParam[0] = pApData->nImanaCell1;
		nParam[1] = pApData->nImanaCell2;
		nParam[2] = pApData->nImanaCell3;
		if(nParam[2] < 200)
			nParam[2] = 200;
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PUMPMANA, (int)&nParam);
	}
	if(pApData->bCheckTPLife && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_CHECKLIFE, pApData->nTPLife))
			return;
	}
	if(pApData->bCheckTPMana && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_CHECKMANA, pApData->nTPMana))
			return;
	}
	if(pApData->bCheckTPLifeGone && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_LIFEGONE, 0))
			return;
	}
	if(pApData->bCheckTPManaGone && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_MANAGONE, 0))
			return;
	}
	if(pApData->bCheckTPIBox && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_FULLITEM, pApData->nTPiboxSel))
			return;
	}
	if(pApData->bCheckTPMoney && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_FULLMONEY, pApData->nTPMoney))
			return;
	}
	if(pApData->bCheckTPIDmg && nBS == 0 && nSK == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_DMGITEM, pApData->nTPDmgItem))
			return;
	}
	AUTOLOG_EVERY(1000, "[AUTO-STAGE-TP] pass=%u t=%u stage=after-tp tpl=%d tpm=%d tplg=%d tpmg=%d tpbox=%d tpmoney=%d tpdmg=%d", m_GameCounter, timeGetTime(), pApData->bCheckTPLife, pApData->bCheckTPMana, pApData->bCheckTPLifeGone, pApData->bCheckTPManaGone, pApData->bCheckTPIBox, pApData->bCheckTPMoney, pApData->bCheckTPIDmg);
	if(pApData->bChat)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CANCHAT, pApData->nChatChann))
		{
			char	Buffer[256];
			int nMsgLength = KUiFaceSelector::ConvertFaceText(Buffer, pApData->szChat, strlen(pApData->szChat));
			nMsgLength = TEncodeText(Buffer, nMsgLength);
			if(pApData->nChatChann == 0)	//chat phu can
			{
				int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
				for(int i=0;i<nChannelDataCount;++i)
				{
					if(KUiMsgCentrePad::IsChannelType(i, KUiMsgCentrePad::ch_Screen))
					{
						DWORD nChannelID = KUiMsgCentrePad::GetChannelID(i);
						KUiPlayerBar::OnSendChannelMessage(
							nChannelID, Buffer, nMsgLength);
					}
				}
			}
			else	//kenh the gioi
			{
				int i = KUiMsgCentrePad::GetChannelIndex("CH_WORLD");
				if(i >= 0)
				{
					DWORD nChannelID = KUiMsgCentrePad::GetChannelID(i);
					KUiPlayerBar::OnSendChannelMessage(
						nChannelID, Buffer, nMsgLength);
				}
			}
		}
	}
	g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PICKUPSET, Wnd_IsLButtonDown());
	// Y NGHIA GIA TRI TRA VE cua cac may hoat dong (nBS):
	//   0 = tha may cho auto tu do
	//   1 = cam lai HET: khong nhat do, khong danh (lenh duoi-nhat do_run se de len
	//       lenh di cua may nen phai bo qua)
	//   2 = may DANG GIAO muc tieu (ea.uNpcID): nhat do truoc, roi EP may PK danh
	//   4 = (25/08) chi song BEN TRONG CoreShell: may Sat Thu ve toi thanh ma tui
	//       day thi xin nhuong cho bo Hau can. case ATYPE_SATTHU tu goi ATYPE_RETURN
	//       roi doi thanh 1 truoc khi tra ra day, nen ben nay khong bao gio thay 4.
	//   3 = (25/08) giu may nhung KHONG giao muc tieu: van nhat do, con danh hay
	//       khong thi theo DUNG o "danh chu dong" cua nguoi choi. Dung cho pha nhat
	//       do / dung cho boss hoi sinh cua may Sat Thu - truoc day tra 2 nen nhan
	//       vat quay ra danh NPC xung quanh du nguoi choi da tat bOnPK.
	//   5 = (26/08) may Tin Su / Sat Thu giao muc tieu cho may DANH THUONG (tab
	//       Chien dau): chay ATYPE_FIGHT ke ca khi chua bat o 'Chien dau', luon
	//       ap sat toi tam chieu; van nhat do nhu 2.
	//   6 = (26/08) rieng Loi Chu Bach Nhan: giao muc tieu cho may PK nhung theo
	//       DUNG o 'Duoi theo muc tieu' cua nguoi choi (ep duoi theo la bi du
	//       roi dai mat chuoi).
	BOOL bLaunch = 0;
	if(nBS != 1)
		bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PICKUP, (int)pApData);
	AUTOLOG_EVERY(1000, "[PICK-RET] pass=%u t=%u pickret=%d lbtn=%d pick=%d fpick=%d pvis=%d ptype=%d city=%d nopick=%d nopcnt=%d", m_GameCounter, timeGetTime(), bLaunch, Wnd_IsLButtonDown(), pApData->bPickUp, pApData->bFollowPick, pApData->nPickVision, pApData->nPickType, pApData->bCityPick, pApData->bNoPick, pApData->nNOPCount);
	if(bLaunch != 2)
		bLaunch = 0;
	if(pApData->bUseBuff && !bLaunch)
	{
		nParam[0] = pApData->nUseBuffVal;
		nParam[1] = pApData->bPTBuff;
		bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_BASEBUFF, (int)&nParam);
	}
	if(pApData->bCLBuff && !bLaunch)
	{
		bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CLBUFF, pApData->bCLBuffCamp);
	}
	if(pApData->nSkillIdSP1 && !bLaunch)
	{
		bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SUPPORTBUFF, pApData->nSkillIdSP1);
	}
	if(pApData->nSkillIdSP2 && !bLaunch)
	{
		bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SUPPORTBUFF, pApData->nSkillIdSP2);
	}
	if(pApData->nSkillIdSP3 && !bLaunch)
	{
		bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SUPPORTBUFF, pApData->nSkillIdSP3);
	}
	if(!pApData->bOnPK)
	{
		// (02/09) ba o "Ky nang tay trai / tay phai / tu ve" DA BO - WAuto khong con
		// gan o chieu chuot trai/phai cua nhan vat nua (nguoi choi tu dat trong game).
		// Chieu may danh thuong dung nay lay tu bang CHIEU KET HOP (tab 13), xu ly
		// tron ven ben trong ATYPE_FIGHT. Hai case ATYPE_LEFTSKILL / ATYPE_RIGHTSKILL
		// van giu trong CoreShell.h de KHONG danh so lai enum, nhung khong ai goi.
		AUTOLOG_EVERY(5000, "[SKILL-SET] combo=%d khe=%d/%d/%d/%d/%d/%d skB=%d skC=%d cSec=%d skLS=%d/%d skMS=%d/%d tc=%d/%d kieu=%d ms=%d dist=%d hoi=%d nguoi=%d", pApData->bCombo, pApData->nComboSkill[0], pApData->nComboSkill[1], pApData->nComboSkill[2], pApData->nComboSkill[3], pApData->nComboSkill[4], pApData->nComboSkill[5], pApData->nSkillIdB, pApData->nSkillIdC, pApData->nSkillCSec, pApData->nSkillIdLS, pApData->nSLSPerc, pApData->nSkillIdMS, pApData->nSMSPerc, pApData->bTienChieu, pApData->nTCSkill, pApData->nTCKieu, pApData->nTCMs, pApData->nTCDist, pApData->nTCHoi, pApData->bTCChiNguoi);
	}
	else if(pApData->bDrawVision)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ISFIGHTMODE, 0))
		{
			g_DrawVision = pApData->nPKVision;
			if(g_DrawVision < 100)
				g_DrawVision = 100;
			else if(g_DrawVision > 1200)
				g_DrawVision = 1200;
			g_DrawVisionTime = timeGetTime() + 250;
		}
	}
	AUTOLOG_EVERY(500, "[BUFF-SWALLOW] pass=%u t=%u launch=%d ubuff=%d buffval=%d clbuff=%d sp1=%d sp2=%d sp3=%d", m_GameCounter, timeGetTime(), bLaunch, pApData->bUseBuff, pApData->nUseBuffVal, pApData->bCLBuff, pApData->nSkillIdSP1, pApData->nSkillIdSP2, pApData->nSkillIdSP3);
	if(pApData->nSkillIdA1 || pApData->nSkillIdA2)
	{
		nParam[0] = pApData->nSkillIdA1;
		nParam[1] = pApData->nSkillIdA2;
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CHANGEAURA, (int)&nParam);
	}
	if(!Wnd_IsLButtonDown())
	{
		if((pApData->bOnPK && nBS != 5) || nBS == 2 || nBS == 6)
		{
			// nBS == 2: muc tieu do may TK/LD/BC vua giao (ea.uNpcID) - khong duoc xoa.
			// nBS == 5: may Tin Su / Sat Thu muon may DANH THUONG - roi xuong nhanh else.
			// nBS == 6: muc tieu do may Loi Chu Bach Nhan giao - van may PK, khong ep duoi.
			// (nBS == 3 khong vao duoc day tru khi nguoi choi TU bat 'danh chu dong')
			if(pApData->bUseFKey && !Wnd_IsPKKeyDown() && nBS != 2 && nBS != 6)
				g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_RESETNPCID, 0);
			if(!bLaunch && (nBS == 2 || nBS == 6 || !pApData->bUseFKey || Wnd_IsPKKeyDown()))
			{
				// (26/08) chu game chot: "bat ke khi nao cung phai danh trong pham vi
				// skill va trong tam quet phai di chuyen den de danh" - may PK LUON
				// duoi theo / ap sat toi tam chieu, ke ca khi o 'Duoi theo muc tieu'
				// dang tat. Rieng Loi Chu Bach Nhan (nBS == 6) giu theo o nguoi choi.
				const autoData* pPKData = pApData;
				if(nBS != 6 && !pApData->bPKFollowTG)
				{
					memcpy(&sBSData, pApData, sizeof(autoData));
					sBSData.bPKFollowTG = 1;
					pPKData = &sBSData;
				}
				bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PKFIGHT, (int)pPKData);
			}
			AUTOLOG_EVERY(1000, "[PK-RET] pass=%u t=%u pkret=%d fkey=%d fkeydown=%d pkvis=%d pknear=%d pkplayer=%d pknpc=%d pkappr=%d", m_GameCounter, timeGetTime(), bLaunch, pApData->bUseFKey, Wnd_IsPKKeyDown(), pApData->nPKVision, pApData->nPKNearDist, pApData->bPKPlayer, pApData->bPKNpc, pApData->bPKAppr);
		}
		else
		{
			if(!bLaunch)
			{
				// [DaTau] khi may Da Tau cam lai thi khong cho ATYPE_MOVE gianh quyen di chuyen
				BOOL bMoving = FALSE;
				if(nDT == 0 && nBS == 0 && nSK == 0)
					bMoving = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_MOVE, (int)pApData);
					AUTOLOG_EVERY(2000, "[MOVE-RET] pass=%u t=%u moving=%d nDT=%d launch=%d killmons=%d follow=%d around=%d coord=%d uphorse=%d fdist=%d", m_GameCounter, timeGetTime(), bMoving, nDT, bLaunch, pApData->bMoveKillMons, pApData->bMoveFollow, pApData->bAroundPoint, pApData->bMoveCoord, pApData->bMoveUpHorse, pApData->nFollowDist);
				// (26/08) nBS == 5: may Tin Su / Sat Thu giao muc tieu cho may DANH
				// THUONG (tab Chien dau) - chay ke ca khi chua bat o 'Chien dau', va
				// bo 'Bo qua boss vang' (quai Tin Su / boss sat thu la boss vang).
				if(!bMoving && (pApData->bFight || nBS == 5) && nDT != 1 && (nBS == 0 || nBS == 5))
				{
					const autoData* pFData = (nDT == 2) ? &sDTData : pApData;
					if(nBS == 5)
					{
						memcpy(&sBSData, pApData, sizeof(autoData));
						sBSData.bSkipGoldboss = 0;
						pFData = &sBSData;
					}
					bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_FIGHT, (int)pFData);
					AUTOLOG_EVERY(1000, "[FIGHT-RET] pass=%u t=%u fightret=%d nDT=%d moving=%d vis=%d near=%d appr=%d fback=%d fbvis=%d selfb=%d selboss=%d skipboss=%d fmode=%d", m_GameCounter, timeGetTime(), bLaunch, nDT, bMoving, pApData->nVision, pApData->nNearDist, pApData->bApproach, pApData->bFightBack, pApData->nFBVision, pApData->nSelFBack, pApData->nSelBoss, pApData->bSkipGoldboss, g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ISFIGHTMODE, 0));
					if(bLaunch == 2)
					{
						PostQuitMessage(0);
						return;
					}
					if(!bLaunch)
						AUTOLOG_EVERY(1000, "[FIGHT-SKIP] fight tra 0 -> RESETMOVE moving=%d nDT=%d vis=%d near=%d", (int)bMoving, nDT, pApData->nVision, pApData->nNearDist);
					if(!bLaunch)
						g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_RESETMOVE, 0);
				}
			}
		}
	}
	else if(pApData->bOnPK)
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_RESETNPCID, 0);
	
	if(pApData->bEatPoison)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_EATPOISON, 0))
			return;
	}
	if(pApData->bEatLifeFull)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_EATLIFEFULL, 0))
			return;
	}
	if(pApData->bEatExp)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_EATEXPX2, 0))
			return;
	}
	if(pApData->bEatSkill)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_EATSKILLX2, 0))
			return;
	}
	if(pApData->bOpenBag)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_OPENBAG, 0))
			return;
	}
	if(pApData->bArrangeI)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ARRANGEITEM, 0))
			return;
	}
	if(pApData->bArrangeB)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ARRANGEBOX, 0))
			return;
	}
	if(!Wnd_IsLButtonDown())
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_FILTER, (int)pApData))
			return;
	}
	AUTOLOG_EVERY(1000, "[AUTO-STAGE-FILTER] pass=%u t=%u stage=after-filter launch=%d nDT=%d lbtn=%d filter=%d ftcnt=%d prize=%d level=%d", m_GameCounter, timeGetTime(), bLaunch, nDT, Wnd_IsLButtonDown(), pApData->bFilter, pApData->nFtMaCount, pApData->bPrize, pApData->bLevel);
	if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PTPROC, (int)pApData))
		return;
	if(pApData->nSelInvitePt && nBS == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PTINVITE, (int)pApData))
			return;
	}
	if(pApData->nSelJoinPt && nBS == 0)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PTJOIN, (int)pApData))
			return;
	}
	if(pApData->bFRepair)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_REPAIRF, 0))
			return;
	}
	AUTOLOG_EVERY(1000, "[AUTO-END] pass=%u t=%u stage=end launch=%d nDT=%d lbtn=%d ret=%d fmode=%d", m_GameCounter, timeGetTime(), bLaunch, nDT, Wnd_IsLButtonDown(), pApData->bReturn, g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ISFIGHTMODE, 0));
	if(pApData->bReturn && nDT == 0 && nBS == 0 && !g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ISFIGHTMODE, 0)
	&& !Wnd_IsLButtonDown())
	{
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_RETURN, (int)pApData);
	}
}

void KMyApp::ProcIpcCommand()
{
	if(!g_pState)
		return;
	if (WaitForSingleObject(g_hEventRecv, 0) == WAIT_OBJECT_0)
	{
		HWND hWnd = g_GetMainHWnd();
		UINT uCount = *(UINT*)g_pState;
		SharedState* p = (SharedState*)((BYTE*)g_pState + sizeof(UINT));
		for(UINT c = 0; c<uCount; ++c)
		{
			switch(p->CmdID)
			{
			case PRT_CONNECT:
			{
				char Buffer[128];
				UINT pid = GetCurrentProcessId();
				sprintf(Buffer, "%s%u", MMF_NAME_CLIENT, pid);
				g_hRepMap = OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, Buffer);
				g_pRepState = (SharedState*)MapViewOfFile(g_hRepMap, FILE_MAP_READ | FILE_MAP_WRITE, 0,0,0);
				sprintf(Buffer, "%s%u", EVT_CLRECV, pid);
				g_hRepEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, Buffer);
				SharedState s;
				s.CmdID = PRG_REPLYRECVED;
				s.Size = sizeof(SharedState);
				SendInfoToTool(&s, sizeof(SharedState));
			}
			break;
			case PRT_GAMELOOP:
			{
				// (24/08) WAuto.exe CU gui goi NGAN hon struct hien tai -> phan duoi la RAC
				// (cac truong moi cua Tong Kim / Da Tau doc phai rac se tu bat auto lung tung).
				// Chep sang ban sao DA XOA TRANG, khong ghi de len goi ke tiep trong bo dem.
				IPCGameLoop* pGL = (IPCGameLoop*)p;
				if(pGL->Size >= sizeof(IPCGameLoop))
				{
					ExtAutoLoop(&pGL->setting);
				}
				else
				{
					IPCGameLoop sGL;
					memset(&sGL, 0, sizeof(sGL));
					memcpy(&sGL, pGL, pGL->Size);
					ExtAutoLoop(&sGL.setting);
				}
			}
			break;
			case PRT_HIENTHI:	// [REP3 03/09] tuy chon hien thi tu WAuto (NpcTheSame / MissleIndex), khong can tick auto
			{
				IPCHienThi* pHT = (IPCHienThi*)p;
				if(pHT->Size >= sizeof(IPCHienThi) && g_pCoreShell)
					g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_HIENTHI, (int)pHT);
			}
			break;
			case PRT_HIDEGAME:
			{
				IPCHideGame* pCmd = (IPCHideGame*)p;
				if(pCmd->bHide)
				{
					if(g_bTrayActive)
						break;
					KUiPlayerBaseInfo	Info;
					memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
					g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
					if(Info.Name[0])
						AddTrayIconHide(hWnd, Info.Name);
					else
						AddTrayIconHide(hWnd, "< >");
				}
				else
				{
					if(!g_bTrayActive)
						break;
					g_bTrayActive = FALSE;
					NOTIFYICONDATAA nid;
					memset(&nid, 0, sizeof(nid));
					nid.cbSize = sizeof(nid);
					nid.hWnd = hWnd;
					nid.uID = ID_TRAYICON;
					Shell_NotifyIcon(NIM_DELETE, &nid);
					ShowWindow(hWnd, SW_RESTORE);
					SetForegroundWindow(hWnd);
				}
			}
			break;
			case PRT_ISHIDE:
			{
				IPCHideGame s;
				s.CmdID = PRG_REPISHIDE;
				s.Size = sizeof(IPCHideGame);
				s.bHide = g_bTrayActive;
				SendInfoToTool(&s, sizeof(IPCHideGame));
			}
			break;
			case PRT_TICKSTART:
			{
				IPCHideGame* pCmd = (IPCHideGame*)p;
				g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CLEAR, pCmd->bHide);
			}
			break;
			case PRT_RETONOFPK:
			{
				if(g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0))
				{
					IPCHideGame* pCmd = (IPCHideGame*)p;
					char szInfo[64];
					if(pCmd->bHide)
						strcpy(szInfo, "BËt chÕ ®é AutoPK");
					else
						strcpy(szInfo, "ChuyÓn vÒ luyÖn c«ng, t¾t AutoPK");
					KUiMsgCentrePad::SystemMessageArrival(szInfo, strlen(szInfo));
				}
			}
			break;
			case PRT_RETAUTOONOF:
			{
				if(g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0))
				{
					IPCHideGame* pCmd = (IPCHideGame*)p;
					char szInfo[64];
					if(pCmd->bHide)
						strcpy(szInfo, "KÝch ho¹t Auto bªn ngoµi");
					else
						strcpy(szInfo, "Ng­ng ho¹t ®éng Auto bªn ngoµi");
					KUiMsgCentrePad::SystemMessageArrival(szInfo, strlen(szInfo));
				}
			}
			break;
			case PRT_GETITEMNAME:
			{
				if(g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0))
				{
					char szBuffer[4812];
					SharedState* pN = (SharedState*)&szBuffer[0];
					pN->CmdID = PRG_OPENNOPICK;
					int* pCount = (int*)(pN+1);
					char* pName = (char*)(pCount+1);
					*pCount = g_pCoreShell->OperationRequest(
						GOI_AUTOPLAY_ACTION, ATYPE_GETITEMNAME, (int)pName);
					pN->Size = sizeof(SharedState) + sizeof(int) + (*pCount)*80;
					SendInfoToTool(pN, pN->Size);
				}
			}
			break;
			case PRT_GETTEAMAROUND:
			{
				if(g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0))
				{
					char szBuffer[3232];
					IPCHideGame* pCmd = (IPCHideGame*)p;
					SharedState* pN = (SharedState*)&szBuffer[0];
					pN->CmdID = PRG_TEAMNAMELIST;
					int* pCount = (int*)(pN+1);
					*pCount++ = pCmd->bHide;
					char* pName = (char*)(pCount+1);
					*pCount = g_pCoreShell->OperationRequest(
						GOI_AUTOPLAY_ACTION, ATYPE_GETAROUNDNAME, (int)pName);
					pN->Size = sizeof(SharedState) + sizeof(int)*2 + (*pCount)*32;
					SendInfoToTool(pN, pN->Size);
				}
			}
			break;
			case PRT_ACTAUTOLG:
			{
				IPCAutoLogin* pCmd = (IPCAutoLogin*)p;
				ExtAutoLogin(pCmd);
			}
			break;
			case PRT_QUITGAME:
			{
				PostQuitMessage(0);
				return;
			}
			break;
			}
			p = (SharedState*)((BYTE*)p + p->Size);
		}
	}
}

BOOL KMyApp::GameLoop()
{
	static int nGameFps = 0;
	// frame-time probe (PaintLog=1): where does a slow pass spend its time
	DWORD	nLogT0 = g_nPaintLog > 0 ? timeGetTime() : 0;
	DWORD	nLogTick = 0, nLogPaint = 0, nLogShift = 0;
	// Moc tick logic THAT (khong phai moc ly tuong n*55,56ms): dung lam goc cho
	// alpha noi suy. Xem chu thich tai cho tinh nAlpha ben duoi.
	static DWORD	s_dwLastTickAt = 0;
	static DWORD	s_dwTickSpan = 0;
	int	nLogCross = 0;
	DWORD	nLogCntBefore = m_GameCounter;
	g_NetConnectAgent.Breathe();

	// He replay .jxr: hoi lai trang thai tu DLL MOI VONG BOM (ban tham chieu
	// lam y het tai 0x0056E3BD). Khi dang phat lai thi bo qua toan bo logic
	// game va chi chay may bom khung cua trinh phat.
	JxReplay_Breathe();
	if (JxReplay_IsPlaying())
	{
		if (KUiJxrPlayer::GetIfVisible() == NULL)
			KUiJxrPlayer::OpenWindow();
		KUiJxrPlayer::PlayPump();
		return true;
	}
	// Phat .jxr vua ket thuc: khoi phuc man dang nhap neu no da bi an de xem.
	// (no-op neu khong o trang thai xem tu man login)
	KUiLogin::RestoreAfterReplay();

	if(g_DrawVisionTime < timeGetTime())
		g_DrawVision = 0;
	ProcIpcCommand();
	if (m_GameCounter * 1000 <= m_Timer.GetElapse() * GAME_FPS)
	{
		// Probe tach khoi logic: [SPIKE] do duoc nhung cu logic=59-108ms trong mot vong
		// lap, trong khi ticksum (ben trong Breathe) chi 1-27ms MOI GIAY => cu do nam o
		// phan khac. Tach ra de biet la Breathe hay UiHeartBeat hay phan sau do.
		DWORD	dwLgT0 = g_nPaintLog > 0 ? timeGetTime() : 0;
		BOOL	bLgBre = g_pCoreShell->Breathe();
		DWORD	dwLgT1 = g_nPaintLog > 0 ? timeGetTime() : 0;
		BOOL	bLgUi  = bLgBre ? UiHeartBeat() : FALSE;
		DWORD	dwLgT2 = g_nPaintLog > 0 ? timeGetTime() : 0;
		if (bLgBre && bLgUi)
		{
			//UiPaint(nGameFps); //Fix by kinnox cpu nhe hon nhiÒu l¾m
			// He replay .jxr: bom bo dem khung 30Hz roi day xuong shell.
			// KHONG goi cho nay thi se KHONG co gi duoc ghi va cung khong bao loi.
			JxReplay_OnGameFrame();

			// PHAI la static: bien cuc bo bi dat lai = 0 MOI VONG nen dieu kien
			// (GetTickCount() - 0 > 1000) LUON dung => UpdateData chay MOI TICK
			// (18 lan/giay) thay vi moi giay nhu y dinh. Ma UpdateData khong he re:
			// Hide/Show 24 phan tu + Clear() + TeamOperation vao Core + DOC FILE
			// cau hinh. Do that: [SPIKE] logic=59-108ms trong mot vong lap.
			static int TickCountTMG = 0;
			const int TimeDelay_Update_TMG = 1000;
			if (KUiTeamManager2::GetIfVisible() == NULL)
			{
				KUiTeamManager2::OpenWindow();
			}
			else
			{
				if (GetTickCount() - TickCountTMG > TimeDelay_Update_TMG)
				{
					TickCountTMG = GetTickCount();
					KUiTeamManager2* kt2 = KUiTeamManager2::GetUiTeamManager();
					kt2->UpdateData(NULL);
				}
			}
			///

			///UiTargetInfo
			// Cung loi voi TickCountTMG o tren - phai la static.
			static int TickCountTI = 0;
			const int TimeDelay_Update_TI = 1000;
			if (KUiTargetInfo::GetIfVisible() == NULL)
			{
				KUiTargetInfo::OpenWindow();
			}
			else
			{
				if (GetTickCount() - TickCountTI > TimeDelay_Update_TI)
				{
					TickCountTI = GetTickCount();
					KUiTargetInfo* ti = KUiTargetInfo::GetUiTargetInfo();
					ti->UpdateData(NULL);
				}
			}
			if (g_nPaintLog > 0)
			{
				DWORD	dwLgT3 = timeGetTime();
				if (dwLgT3 - dwLgT0 >= 30)
				{
					FILE* pLgLog = fopen("jx_paint.log", "a");
					if (pLgLog)
					{
						fprintf(pLgLog, "[LOGIC] t=%u tong=%u bre=%u uihb=%u sau=%u\n",
							dwLgT0, dwLgT3 - dwLgT0, dwLgT1 - dwLgT0, dwLgT2 - dwLgT1, dwLgT3 - dwLgT2);
						fclose(pLgLog);
					}
				}
			}
			m_GameCounter++;
			int	nElapse = m_Timer.GetElapse();
			// Chup moc tick THAT + khoang giua hai tick that, de lop noi suy neo dung
			// vao no thay vi neo vao moc ly tuong (m_GameCounter-1)*55,56ms.
			if (s_dwLastTickAt && (DWORD)nElapse > s_dwLastTickAt)
				s_dwTickSpan = (DWORD)nElapse - s_dwLastTickAt;
			s_dwLastTickAt = (DWORD)nElapse;
			if (nElapse)
				nGameFps = m_GameCounter * 1000 / nElapse;
			g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_DRAWVISION, g_DrawVision);
			if (g_nPaintFps > 0)
				g_pCoreShell->OperationRequest(GOI_PROCFRAME_BREATHE, (unsigned int)(g_nPaintInterp > 0 ? 1 : 0), g_nPaintLog);	// snapshot tick positions for paint interpolation
			else
				UiPaint(nGameFps);//nhe hon
		}
		else
		{
			return false;
		}
	}
	SendAllCommand();
	if (MyApp.NotifiIconState())
	{	
		gTrayMode.ShowNotify();
		MyApp.m_bNotifiIconState = FALSE;
	}	
	
	if (g_nPaintLog > 0)
		nLogTick = timeGetTime() - nLogT0;
	BOOL	bPainted = FALSE;
	if (g_nPaintFps > 0)
	{
		// Paint clock separated from the 18-fps logic clock (JX2 client PaintFps mechanism).
		// Nhip duoc dat tu THOI DIEM VE THUC TE chu khong tu moc ly thuyet: vong bom ngoai
		// chi moi 16ms mot co hoi (KWin32App::Run, nInterval = 1000/60) trong khi 60fps can
		// 16,67ms - doi dung moc se cu ~25 khung lai truot 1 co hoi, thanh mot khung 32ms.
		// FPS trung binh van ~60 nhung mat nhin thay giut. Cho phep ve som PAINT_LEAD_MS
		// va tinh moc ke tu luc ve that => khoang cach cac khung deu nhau.
		// Nua chu ky vong bom. PHAI di theo nInterval trong KWin32App::Run:
		// luoi 16ms -> lead 8; luoi 8ms -> lead 4. Dat lead >= ca chu ky luoi thi
		// dieu kien ve luon thoa va game ve moi luot bom (do that: 85-102 khung/giay
		// thay vi 60, ngon thua 40-60% CPU ve ma man 60Hz khong hien duoc).
		#define	PAINT_LEAD_MS	4	// nua chu ky vong bom (nInterval = 8)
		static DWORD s_dwNextPaint = 0;
		DWORD	nPaintElapse = m_Timer.GetElapse();
		DWORD	nPaintStep = 1000 / (DWORD)g_nPaintFps;
		if (nPaintStep < 1)
			nPaintStep = 1;
		if ((int)(nPaintElapse + PAINT_LEAD_MS - s_dwNextPaint) >= 0)
		{
			s_dwNextPaint = nPaintElapse + nPaintStep;
			if (g_nPaintInterp > 0 && m_GameCounter > 0)
			{
				// alpha 0..1000: khung ve nay nam o dau giua hai tick logic.
				// Truoc day neo vao moc LY TUONG (m_GameCounter-1)*1000/18: tick that luon
				// xay ra TRE hon moc do (phai doi luot bom ke tiep), nen trong khoang tre
				// alpha vuot 1000 va bi kep tran o CoreShell.cpp:11799 => vi tri DONG BANG
				// vai khung moi chu ky tick, du FPS van du. Nay neo vao moc tick THAT va
				// chia cho khoang tick THAT do duoc => alpha trai deu 0..1000 dung mot lan
				// giua hai tick, khong con doan ket tran.
				int	nAlpha;
				if (s_dwTickSpan >= 20 && s_dwTickSpan <= 200 && nPaintElapse >= s_dwLastTickAt)
					nAlpha = (int)((nPaintElapse - s_dwLastTickAt) * 1000 / s_dwTickSpan);
				else
					nAlpha = (int)(nPaintElapse * (DWORD)GAME_FPS - (m_GameCounter - 1) * 1000);
				if (nAlpha < 0)
					nAlpha = 0;
				// Do rieng POSSHIFT: truoc day chi phi nay bi tinh vao "paint=" cua
				// [SPIKE] du no chay TRUOC UiPaint (muc 12.5 DIEUTRA_KHUNG_DONG_NGUOI).
				DWORD	nLogShiftT0 = g_nPaintLog > 0 ? timeGetTime() : 0;
				if (g_pCoreShell->OperationRequest(GOI_PROCFRAME_POSSHIFT, (unsigned int)nAlpha, 1000) == 2)
					nLogCross = 1;
				if (g_nPaintLog > 0)
					nLogShift = timeGetTime() - nLogShiftT0;
			}
			UiPaint(nGameFps);
			bPainted = TRUE;
		}
	}

	if (g_nPaintLog > 0 && bPainted)
		nLogPaint = timeGetTime() - nLogT0 - nLogTick - nLogShift;
	if (m_GameCounter * 1000 >= m_Timer.GetElapse() * GAME_FPS)
	{
		//UiPaint(nGameFps);
		if (!bPainted)
			Sleep(1);
	}
	else if ((m_GameCounter % 8) == 0)
	{
		Sleep(1);
	}

	if (g_nPaintLog > 0)
	{
		// pass >= 25ms is a visible hitch at 60fps; aggregate every 10s
		static DWORD s_LogSum = 0, s_LogCnt = 0, s_LogMax = 0, s_LogSpk = 0, s_LogCross = 0, s_LogLast = 0;
		DWORD	nLogTotal = timeGetTime() - nLogT0;
		s_LogSum += nLogTotal;
		s_LogCnt++;
		if (nLogTotal > s_LogMax)
			s_LogMax = nLogTotal;
		if (nLogCross)
			s_LogCross++;
		if (nLogTotal >= 25)
		{
			s_LogSpk++;
			FILE* pLog = fopen("jx_paint.log", "a");
			if (pLog)
			{
				fprintf(pLog, "[SPIKE] t=%u total=%u logic=%u shift=%u paint=%u tick=%d painted=%d cross=%d\n",
					nLogT0, nLogTotal, nLogTick, nLogShift, nLogPaint, (int)(m_GameCounter != nLogCntBefore), (int)bPainted, nLogCross);
				fclose(pLog);
			}
		}
		if (s_LogLast == 0)
			s_LogLast = nLogT0;
		if (nLogT0 - s_LogLast >= 10000)
		{
			FILE* pLog = fopen("jx_paint.log", "a");
			if (pLog)
			{
				fprintf(pLog, "[SUM] t=%u passes=%u avg=%u max=%u spikes=%u cross=%u\n",
					nLogT0, s_LogCnt, s_LogCnt ? s_LogSum / s_LogCnt : 0, s_LogMax, s_LogSpk, s_LogCross);
				fclose(pLog);
			}
			s_LogSum = s_LogCnt = s_LogMax = s_LogSpk = s_LogCross = 0;
			s_LogLast = nLogT0;
		}
	}

	return true;
}

int KMyApp::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nRet = 0;
	if (uMsg != WM_CLOSE)
	{
		// [REP3 03/09] bo co toa do chuot y*40/808 (di kem hack cua so +40 da bo)
		UiProcessInput(uMsg, wParam, lParam);
	}
	else if (g_bScreen == false && UiIsAlreadyQuit() == false)
	{
		KIniFile* pSetting = g_UiBase.GetCommConfigFile();
		if (pSetting)
		{
			char	szMsg[128], szTitle[64];
			pSetting->GetString("InfoString", QUIT_QUESTION_ID, "", szMsg, sizeof(szMsg));
			pSetting->GetString("InfoString", GAME_TITLE, "", szTitle, sizeof(szTitle));
			if (szMsg[0] && szTitle[0])
			{
				nRet = (MessageBox(g_GetMainHWnd(), szMsg, szTitle,
					MB_YESNO | MB_ICONQUESTION) != IDYES);
			}
		}
	}
	return nRet;
}

