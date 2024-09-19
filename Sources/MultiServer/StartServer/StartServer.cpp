// StartServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <time.h>
#include "FilterTextLib.h"
#include <math.h>
#include <process.h>
#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
//#include "TestUpdateDLL.h"
//#include "TestUpdateDLLDlg.h"
#include "UpdateExport.h"
//#pragma comment(lib,"ws2_32.lib")

using namespace std;

//volatile int counter = 0;

CFilterTextLib g_libFilterText;
//extern CFilterTextLib g_libFilterText;

#define MAX_LOADSTRING 100
static int countdowncheckGS = 0;
static HWND hWndMain;
static char folderServer[MAX_PATH];

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

BOOL CheckWindowsRuningForName(char *processName)
{
	bool exists = false;

	char temps[100];
	strcpy(temps, processName);
	size_t i;

	for(i = 0; temps[i] != '\0'; i++)
	  temps[i] = tolower(temps[i]);

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
		{
			char *output = NULL;
			char temps2[100];
			strcpy(temps2, entry.szExeFile);
			size_t i2;
			for(i2 = 0; temps[i2] != '\0'; i2++)
			  temps2[i2] = tolower(temps2[i2]);

			output = strstr (temps2, temps); //kiem tra pname dang running
			if(output) {
				exists = true;
			}
		}

    CloseHandle(snapshot);
    return exists;
}

BOOL CheckWindowsRuningForName2(char *processName)
{
	bool exists = false;

	char temps[100];
	strcpy(temps, processName);
	size_t i;

	for(i = 0; temps[i] != '\0'; i++)
	  temps[i] = tolower(temps[i]);

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    while (Process32Next(snapshot, &entry))
		{
			char *output = NULL;
			char temps2[100];
			strcpy(temps2, entry.szExeFile);
			size_t i2;
			for(i2 = 0; temps[i2] != '\0'; i2++)
			  temps2[i2] = tolower(temps2[i2]);

			output = strstr (temps2, temps); //kiem tra pname dang running
			if(output) {
				exists = true;
			}
		}

    CloseHandle(snapshot);
    return exists;
}

unsigned int __stdcall mythread(void*) 
{
	while (1) 
	{
		Sleep(3000);
		countdowncheckGS+=3000;
		if(countdowncheckGS >= 1000*60*10)
		{
			SendMessage(hWndMain,WM_CLOSE,0,0);
			break;
		}
	}
	return 0;
}

unsigned int __stdcall mythread2(void*) 
{
	while (1) 
	{
		Sleep(3000);
	}
	return 0;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

 	// TODO: Place code here.
	if (!g_libFilterText.Initialize())
	{
		MessageBox(NULL, "text filter's initing has failed", "error", MB_OK|MB_ICONERROR);
		return -1;
	}

	
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	sprintf(folderServer, "%s\\", NPath);

	//HANDLE myhandleA;
	//HANDLE myhandleB;

	//myhandleA = (HANDLE)::_beginthreadex(0, 0, &mythread, ( void * )0, 0, 0);
	//myhandleB = (HANDLE)::_beginthreadex(0, 0, &mythread2, ( void * )0, 0, 0);

	//WaitForSingleObject(myhandleA, 0);
	//WaitForSingleObject(myhandleB, 0);

	//CloseHandle(myhandleA);
	//CloseHandle(myhandleB);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_STARTSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_STARTSERVER);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_STARTSERVER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_STARTSERVER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
	
   hWndMain = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


struct ACCELTABLEENTRY 
{ 
  WORD fFlags; 
  WORD wAnsi; 
  WORD wId; 
  WORD padding; 
};

#define IDD_DLG_GODDESS                 101
#define IDR_ACCEL1                     202
#define IDC_EDIT_PORT                   1000
#define IDC_LIST_CLIENT                 1001
#define IDC_STATIC_SERVICE              1002
#define IDC_STATIC_DATABASE             1003
#define IDC_EDIT_MAXNUM_ROLE            1004
#define IDC_EDIT_BACKUP_SLEEP_TIME      1005
#define IDC_BTN_BACKUP_SUS_RES          1006
#define IDC_LAB_BACKUP_STATUS           1007
#define IDC_BTN_BACKUP_MANUAL           1009
#define IDC_BUTTON1                     1010
#define IDC_RESTART                     1010
#define IDC_OUTPUT                      1011

UINT FindAcceleratorCommandId(LPCTSTR pszExeName, char Key, BOOL ctrlKey, BOOL shiftKey, BOOL altKey)
{
   UINT nCommandID = 0;
 
   HMODULE hModule = ::LoadLibrary(pszExeName);
   //HMODULE hModule = ::LoadLibrary("FilterText.dll");
   if(NULL != hModule)
   {
      HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(IDD_DLG_GODDESS), RT_ACCELERATOR);
      if(NULL != hRsrc)
      {
         DWORD dwSize = ::SizeofResource(hModule, hRsrc) / 8;
         HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
         if(NULL != hGlobal)
         {
            ACCELTABLEENTRY* table = 
               (ACCELTABLEENTRY*)::LockResource(hGlobal);
            for(DWORD dwIndex = 0; dwIndex < dwSize; dwIndex++)
            {
               ACCELTABLEENTRY& entry = table[dwIndex];
               
               WORD wLo = FVIRTKEY;
               if(ctrlKey) wLo |= FCONTROL;
               if(shiftKey) wLo |= FSHIFT;
               if(altKey) wLo |= FALT;
               DWORD dwToFind = MAKELONG(wLo, (WORD)Key);
 
               wLo = entry.fFlags;
               wLo &= ~FNOINVERT;
               wLo &= ~0x0080;
               DWORD dwFound = MAKELONG(wLo, entry.wAnsi);
               if(dwToFind == dwFound)
               {
                  // accelerator has been found
                  nCommandID = entry.wId;
                  break;
               }
            }
            ::FreeResource(hGlobal);
         }
      }
      ::FreeLibrary(hModule);
   }
   return nCommandID;
}

BOOL OnExecute(LPCSTR szFile)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	::GetStartupInfo(&si);
    ZeroMemory( &pi, sizeof(pi) );
	
	char buffer[256];
	
	strcpy(buffer, szFile);
	
	BOOL bRet = TRUE;
	
    if( !CreateProcess( NULL, buffer, NULL,	NULL, FALSE, 0,	NULL, folderServer,	&si, &pi)) {
		bRet = FALSE;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	
	return bRet;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					{
						DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
					}
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				case IDM_START_1:
					{
						char pName1[MAX_PATH] = "sword3paysys.exe";
						if(CheckWindowsRuningForName(pName1) == FALSE)
						{
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
						}
					}
				   break;
				case IDM_START_2:
					{
						char pName1[MAX_PATH] = "s3relayserver.exe";
						if(CheckWindowsRuningForName(pName1) == FALSE)
						{
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
						}
					}
				   break;
				case IDM_START_3:
					{
						char pName1[MAX_PATH] = "goddess.exe";
						if(CheckWindowsRuningForName(pName1) == FALSE)
						{
							DWORD g2 = g_libFilterText.CreateTextFilter2(3);
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
							Sleep(1000);
							//UINT nID = FindAcceleratorCommandId(_T("goddess.exe"), 'S', TRUE, FALSE, FALSE);
							//HWND hWndMain = ::FindWindow(NULL,_T("Goddess - [Disable]"));
							//if(nID > 0 && hWndMain)
							//{
							//  WPARAM wParam = MAKEWPARAM(nID, 1);
							//  ::SendMessage(hWndMain, WM_COMMAND, wParam, NULL);
							//}
							g2 = g_libFilterText.CreateTextFilter2(0);
						}
					}
				   break;
				case IDM_START_4:
					{
						char pName1[MAX_PATH] = "bishop.exe";
						if(CheckWindowsRuningForName(pName1) == FALSE)
						{
							DWORD g2 = g_libFilterText.CreateTextFilter2(4);
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
							Sleep(1000);
							g2 = g_libFilterText.CreateTextFilter2(0);
						}
					}
				   break;
				case IDM_START_5:
					{
						char pName1[MAX_PATH] = "s3relay.exe";
					    if(CheckWindowsRuningForName(pName1) == FALSE)
						{
							DWORD g2 = g_libFilterText.CreateTextFilter2(5);
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
							Sleep(1000);
							g2 = g_libFilterText.CreateTextFilter2(0);
						}
					}
				   break;
				case IDM_START_6:
					{
						char pName1[MAX_PATH] = "gameserver.exe";
					    if(CheckWindowsRuningForName(pName1) == FALSE)
						{
							//DWORD g2 = g_libFilterText.CreateTextFilter2(6);
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
						}
					}
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				g_libFilterText.CreateTextFilter2(0);
			}
			break;
		case WM_CLOSE:
			{
				PostQuitMessage(0);
				g_libFilterText.CreateTextFilter2(0);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
