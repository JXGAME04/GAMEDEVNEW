// S3Client.cpp : Defines the entry point for the application.

#include "KWin32.h"
#include "KCore.h"
#include "S3Client.h"
#include "KWin32Wnd.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "Ui/UiShell.h"
#include "Ui/PerfHud.h"
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
#include "Ui/UiCase/UiCapture.h"
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
	if (g_bRepresent3 && !bFullScreen)
	{
		RECT	rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT + 40 };
		HWND	hWnd = g_GetMainHWnd();
		DWORD	dwStyle = 0;
		dwStyle = WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED |
			WS_CAPTION | WS_MINIMIZEBOX;
		SetWindowLong(hWnd, GWL_STYLE, dwStyle);
		AdjustWindowRectEx(&rc,
			dwStyle,
			GetMenu(hWnd) != NULL,
			GetWindowLong(hWnd, GWL_EXSTYLE));
		SetWindowPos(hWnd,
			HWND_NOTOPMOST,
			0,
			0,
			rc.right - rc.left,
			rc.bottom - rc.top,
			SWP_NOACTIVATE);
	}
	if (g_pRepresentShell->Create(nWidth, nHeight, bFullScreen != 0))
	{

		return TRUE;
	}
	else
	{
		Error_SetErrorCode(g_bRepresent3 ? ERR_T_REPRESENT3_INIT_FAILED : ERR_T_REPRESENT2_INIT_FAILED);
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
	char	szRecPath[MAX_PATH];
	if (IniFile.GetString("Client", "RecPath", "", szRecPath, sizeof(szRecPath)))
	{
		if (szRecPath[0])
		{
			KUiCapture::SetRecPath(szRecPath);//Set Folder save file Rec video
		}
	}


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
	if(!g_pCoreShell->GetGameData(GDI_GET_PLAYERNPC_INDEX, 0, 0))
		return;
	if(pApData->bOutWhenTP && !pApData->bOnPK)
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
	if(pApData->bCheckTPLife)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_CHECKLIFE, pApData->nTPLife))
			return;
	}
	if(pApData->bCheckTPMana)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_CHECKMANA, pApData->nTPMana))
			return;
	}
	if(pApData->bCheckTPLifeGone)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_LIFEGONE, 0))
			return;
	}
	if(pApData->bCheckTPManaGone)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_MANAGONE, 0))
			return;
	}
	if(pApData->bCheckTPIBox)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_FULLITEM, pApData->nTPiboxSel))
			return;
	}
	if(pApData->bCheckTPMoney)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_FULLMONEY, pApData->nTPMoney))
			return;
	}
	if(pApData->bCheckTPIDmg)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_TP_DMGITEM, pApData->nTPDmgItem))
			return;
	}
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
	BOOL bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PICKUP, (int)pApData);
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
		if(pApData->nSkillIdL)
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_LEFTSKILL, pApData->nSkillIdL);
		if(pApData->nSkillIdR)
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_RIGHTSKILL, pApData->nSkillIdR);
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
	if(pApData->nSkillIdA1 || pApData->nSkillIdA2)
	{
		nParam[0] = pApData->nSkillIdA1;
		nParam[1] = pApData->nSkillIdA2;
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_CHANGEAURA, (int)&nParam);
	}
	if(!Wnd_IsLButtonDown())
	{
		if(pApData->bOnPK)
		{
			if(pApData->bUseFKey && !Wnd_IsPKKeyDown())
				g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_RESETNPCID, 0);
			if(!bLaunch && (!pApData->bUseFKey || Wnd_IsPKKeyDown()))
			bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PKFIGHT, (int)pApData);
		}
		else
		{
			if(!bLaunch)
			{
				BOOL bMoving = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_MOVE, (int)pApData);
				if(!bMoving && pApData->bFight)
				{
					bLaunch = g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_FIGHT, (int)pApData);
					if(bLaunch == 2)
					{
						PostQuitMessage(0);
						return;
					}
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
	if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PTPROC, (int)pApData))
		return;
	if(pApData->nSelInvitePt)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PTINVITE, (int)pApData))
			return;
	}
	if(pApData->nSelJoinPt)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_PTJOIN, (int)pApData))
			return;
	}
	if(pApData->bFRepair)
	{
		if(g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_REPAIRF, 0))
			return;
	}
	if(pApData->bReturn && !g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_ISFIGHTMODE, 0)
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
				IPCGameLoop* pGL = (IPCGameLoop*)p;
				ExtAutoLoop(&pGL->setting);
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
	DWORD	nLogTick = 0, nLogPaint = 0;
	int	nLogCross = 0;
	DWORD	nLogCntBefore = m_GameCounter;
	g_NetConnectAgent.Breathe();
	if(g_DrawVisionTime < timeGetTime())
		g_DrawVision = 0;
	ProcIpcCommand();
	if (m_GameCounter * 1000 <= m_Timer.GetElapse() * GAME_FPS)
	{
		if (g_pCoreShell->Breathe() && UiHeartBeat())
		{
			//UiPaint(nGameFps); //Fix by kinnox cpu nhe hon nhiÒu l¾m
			KUiCapture* kct = KUiCapture::GetUiCapture();
			if (kct->m_bandiCaptureLibrary.IsCapturing())
			{
				kct->m_bandiCaptureLibrary.Work(NULL);
				TCHAR s[128];
				INT msec = kct->m_bandiCaptureLibrary.GetCaptureTime();
				//_stprintf(s, _T("%02d:%02d:%03d / %.1f MB"), msec/(60*1000), msec%(60*1000)/1000, msec%1000, kct->m_bandiCaptureLibrary.GetCaptureFileSize()/1024./1024.);
				_stprintf(s, _T("[ %02d:%02d:%03d ]"), msec / (60 * 1000), msec % (60 * 1000) / 1000, msec % 1000);
				kct->SetRecTimmer(s);
			}

			int TickCountTMG = 0;
			int TimeDelay_Update_TMG = 1000;
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
			int TickCountTI = 0;
			int TimeDelay_Update_TI = 1000;
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
			m_GameCounter++;
			int	nElapse = m_Timer.GetElapse();
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
		#define	PAINT_LEAD_MS	8	// nua chu ky vong bom
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
				// alpha 0..1000: how far the paint moment sits between the last and the next logic tick
				int	nAlpha = (int)(nPaintElapse * (DWORD)GAME_FPS - (m_GameCounter - 1) * 1000);
				if (nAlpha < 0)
					nAlpha = 0;
				if (g_pCoreShell->OperationRequest(GOI_PROCFRAME_POSSHIFT, (unsigned int)nAlpha, 1000) == 2)
					nLogCross = 1;
			}
			UiPaint(nGameFps);
			bPainted = TRUE;
		}
	}

	if (g_nPaintLog > 0 && bPainted)
		nLogPaint = timeGetTime() - nLogT0 - nLogTick;
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
				fprintf(pLog, "[SPIKE] t=%u total=%u logic=%u paint=%u tick=%d painted=%d cross=%d\n",
					nLogT0, nLogTotal, nLogTick, nLogPaint, (int)(m_GameCounter != nLogCntBefore), (int)bPainted, nLogCross);
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
		if (g_bRepresent3 && !g_bScreen)
		{
			if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST && uMsg != WM_MOUSEWHEEL) || uMsg == WM_MOUSEHOVER)
			{
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);
				y = y - y * 40 / (SCREEN_HEIGHT + 40);
				lParam = x | (y << 16);
			}
		}
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

