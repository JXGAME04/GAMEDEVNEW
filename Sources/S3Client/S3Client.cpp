 // S3Client.cpp : Defines the entry point for the application.

#include "KWin32.h"
#include "KCore.h"
#include "S3Client.h"
#include "KWin32Wnd.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "Ui/UiShell.h"
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

//#include "stdafx.h"

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
//#include <tchar.h>
#include "Dbghelp.h"

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
//Represent
struct iRepresentShell*	g_pRepresentShell = NULL;
struct IInlinePicEngineSink* g_pIInlinePicSink = NULL;
iCoreShell*				g_pCoreShell = NULL;
KMusic*					g_pMusic = NULL;

#define	DYNAMIC_LINK_REPRESENT_LIBRARY

#ifdef DYNAMIC_LINK_REPRESENT_LIBRARY
	static HMODULE		l_hRepresentModule = NULL;
	int					g_bRepresent3 = false;
#endif
	
int					g_bScreen = true;
char				g_szGameName[64] = "Vo Lam Truyen Ky";//tieu de tang them 64 bit game edit by phong kieu

KClientCallback g_ClientCallback;

#define	SCREEN_WIDTH	800 //edit by phong kieu chieu rong va cao UI game.exe
#define SCREEN_HEIGHT	600

/*
 * Add this macro by liupeng on 2003.3.20
 * This macro is helper that can judge some legal character
 */
#define _private_IS_SPACE(c)   ((c) == ' ' || (c) == '\r' || (c) == '\n' || (c) == '\t' || (c) == 'x')
#define IS_SPACE(c)	_private_IS_SPACE(c)

#pragma warning (disable: 4305)
#pragma warning (disable: 4309)

int GenerateMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS lpExceptionPointer, PWCHAR pwAppName)
{
	BOOL bOwndumpFile = FALSE;
	HANDLE hDumpFile = hFile;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
	typedef BOOL(WINAPI * MiniDumpWriteDumpT) (
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION
		);

	MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;
	HMODULE hdbgHelp  = LoadLibrary("DbgHelp.dll");

	if(hdbgHelp)
		pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hdbgHelp, "MiniDumpWriteDump");

	if(pfnMiniDumpWriteDump)
	{
		if(hDumpFile == NULL || hDumpFile == INVALID_HANDLE_VALUE)
		{
			TCHAR szFileName[MAX_PATH] = {0};
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

		if(hDumpFile != INVALID_HANDLE_VALUE)
		{
			ExpParam.ThreadId = GetCurrentThreadId();
			ExpParam.ExceptionPointers = lpExceptionPointer;
			ExpParam.ClientPointers = FALSE;

			pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
				hDumpFile, MiniDumpWithDataSegs, (lpExceptionPointer ? &ExpParam : NULL), NULL, NULL);

			if(bOwndumpFile)
				CloseHandle(hDumpFile);
		}
	}

	if(hdbgHelp != NULL)
		FreeLibrary(hdbgHelp);

	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI ExeptionFillert(LPEXCEPTION_POINTERS lpExceptionInfo)
{
	if(IsDebuggerPresent())
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

    while( lpCmdLine[0] == '-' || lpCmdLine[0] == '/' )
    {
        lpCmdLine++;
		
        switch ( *lpCmdLine++ )
        {
		case 'c':
        case 'C':
            bOpenTracer = true;
            break;
        }
		
        while( IS_SPACE( *lpCmdLine ) )
        {
            lpCmdLine++;
        }
    }
	
	if ( bOpenTracer ) 
	{
		AllocConsole();
	}

#endif // End of this function

	if(ANTI_DUMP_MEMORY)
	{
		SystemProcessesScan();
		ProtectionMainDumpMemory();
	}
	if(ANTI_DETECT_WINDOWS_TITLE)
	{
		ThreadDetectWindowsTitle();
	}
	if(ANTI_DETECT_WINDOWS_TEXT)
	{
		ThreadDetectWindowsText();
	}
	if(CHECKSUM_FILE)
	{
		ThreadCheckSumCRC();
	}
	if(ANTI_DETECT_HIDE)
	{
		ThreadDetectHide();
	}
	if(ANTI_CLASS_NAME_SCAN)
	{
		ThreadWindowsClassName();
	}
	if(ANTI_EXIST_MAIN_NAME)
	{
		ThreadExistMainName();
	}
	if(ANTI_BYPASS_MAIN_NAME)
	{
		//ThreadProtectBypassMainName();
	}
	if(PAINT_SPLASH)
	{
	    CSplash splash1(TEXT(".\\GameGuard/logo.bmp"), RGB(128, 128, 128));
		splash1.ShowSplash();
		Sleep(3000);
		//  Close the splash screen
		splash1.CloseSplash();
	}

	hInst = hInstance;
	if (MyApp.Init(hInstance))
		MyApp.Run();

#ifdef TRUE

	if ( bOpenTracer )
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
	if(g_pRepresentShell->Create(nWidth, nHeight, bFullScreen != 0))
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

void gen_random(char *s, size_t len) {
     for (size_t i = 0; i < len; ++i) {
         int randomChar = rand()%(26+26+10);
         if (randomChar < 26)
             s[i] = 'a' + randomChar;
         else if (randomChar < 26+26)
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
using namespace std;

BOOL KMyApp::GameInit()
{
    

	DWORD aPid = GetCurrentProcessId();
	PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);
    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE)
    {
      std::wcout  << "can't get a process snapshot ";
      return 0;
    }

    for(BOOL bok =Process32First(processesSnapshot, &processInfo);bok;  bok = Process32Next(processesSnapshot, &processInfo))
    {
        if( aPid == processInfo.th32ProcessID)
        {
            std::wcout << "found running process: " << processInfo.szExeFile;
            CloseHandle(processesSnapshot);

			char strCounterG[32];
			sprintf(strCounterG, "%s", processInfo.szExeFile);
			//MessageBox(g_GetMainHWnd(), strCounterG, strCounterG, MB_ICONEXCLAMATION);
		
			if(strcmp("Game.exe",strCounterG) != 0)
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

	KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
	if (pSetting)
	{
		pSetting->GetString("Main", "GameName", "Vo Lam Truyen Ky", g_szGameName, sizeof(g_szGameName));
        SetWindowText(g_GetMainHWnd(), g_szGameName);
	}

#ifdef _DEBUG
	g_FindDebugWindow("#32770","DebugWin");
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

	if(UiStart())
	{
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

	return TRUE;
}

BOOL KMyApp::GameLoop()
{
	static int nGameFps = 0;
	g_NetConnectAgent.Breathe();
	
	if (m_GameCounter * 1000 <= m_Timer.GetElapse() * GAME_FPS)
	{
		if (g_pCoreShell->Breathe() && UiHeartBeat())
		{
			//UiPaint(nGameFps); //Fix by kinnox cpu nhe hon nhiÒu l¾m
			KUiCapture *kct = KUiCapture::GetUiCapture();
			if(kct->m_bandiCaptureLibrary.IsCapturing())
			{
				kct->m_bandiCaptureLibrary.Work(NULL);
				TCHAR s[128];
				INT msec = kct->m_bandiCaptureLibrary.GetCaptureTime();
				//_stprintf(s, _T("%02d:%02d:%03d / %.1f MB"), msec/(60*1000), msec%(60*1000)/1000, msec%1000, kct->m_bandiCaptureLibrary.GetCaptureFileSize()/1024./1024.);
				_stprintf(s, _T("[ %02d:%02d:%03d ]"), msec/(60*1000), msec%(60*1000)/1000, msec%1000);
				kct->SetRecTimmer(s);
			}

			m_GameCounter++;
			int	nElapse = m_Timer.GetElapse();
			if (nElapse)
				nGameFps = m_GameCounter * 1000 / nElapse;
		}
		else
		{
			return false;
		}
	}
	if (m_GameCounter * 1000 >= m_Timer.GetElapse() * GAME_FPS)
	{
		UiPaint(nGameFps);
		Sleep(1);
	}
	else if ((m_GameCounter % 8) == 0)
	{
		Sleep(1);
	}

	return true;
}

int KMyApp::HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nRet = 0;
	if (uMsg != WM_CLOSE)
	{
		UiProcessInput(uMsg, wParam, lParam);
	}
	else if (g_bScreen == false && UiIsAlreadyQuit() == false)
	{
		KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
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

