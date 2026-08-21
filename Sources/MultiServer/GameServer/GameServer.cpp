// GameServer.cpp : Defines the entry point for the console application.
//

/*#ifdef _STANDALONE
#ifndef __linux
//#include <winsock2.h>
#endif
#endif*/

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include "Dbghelp.h"
#include "StdAfx.h"
#include "KSOServer.h"
#include "FilterTextLib.h"
CFilterTextLib g_libFilterText;

extern KSwordOnLineSever g_SOServer;
/*#ifdef __linux
#include <unistd.h>
#include <signal.h> 
#include <sys/param.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
void init_daemon(void) { 
	int pid; 
	int i; 
	if(pid=fork()) exit(0);			//是父进程，结束父进程 
	else if(pid< 0) exit(1);		//fork失败，退出 
//是第一子进程，后台继续执行 
	setsid();//第一子进程成为新的会话组长和进程组长 
//并与控制终端分离 
	if(pid = fork()) exit(0);//是第一子进程，结束第一子进程 
	else if(pid< 0) 
	exit(1);//fork失败，退出 
//是第二子进程，继续 
//第二子进程不再是会话组长 

	for(i=0; i< NOFILE; ++i) close(i); 
	umask(0);//重设文件创建掩模 
	return; 
} 

#endif*/

#pragma warning (disable: 4305)
#pragma warning (disable: 4309)

void ClearConsoleToColors(int ForgC, int BackC);        //function declaration

enum Colors{
	black,          //  0 text color - multiply by 16, for background colors
	dark_blue,      //  1
	dark_green,     //  2
	dark_cyan,      //  3
	dark_red,       //  4
	dark_magenta,   //  5
	dark_yellow,    //  6
	light_gray,     //  7
	dark_gray,      //  8
	light_blue,     //  9
	light_green,    // 10
	light_cyan,     // 11
	light_red,      // 12
	light_magenta,  // 13
	light_yellow,   // 14
	white           // 15
};

int g_nPort = 0;
static int countdownCheckLic = 0;

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
			SYSTEMTIME stLocalTime;
			GetLocalTime(&stLocalTime);

			CreateDirectory("DumpInfo", NULL);
			wsprintf(szFileName, "DumpInfo\\%s-%s-%04d%02d%02d-%02d%02d%02d-%ld%ld.dmp",
				pwAppName, szVersion,
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
				hDumpFile, MiniDumpWithFullMemory, // Full dump type
				(lpExceptionPointer ? &ExpParam : NULL), NULL, NULL);

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
	if(IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return GenerateMiniDump(NULL, lpExceptionInfo, L"gameserver");
}

int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter(ExeptionFillert);

	// (21/08 - chu game: "bo toan bo log in ra trong gameserver chi de in ra file
	// log thoi") NAN DONG ca stdout lan stderr sang tep ngay dong dau tien cua
	// main: moi printf co san trong EXE van chay, chi la chay vao tep thay vi ra
	// console. Lam the nay thay vi di sua tung loi goi printf (~450 cho) vi vua
	// de sot vua mat thong tin khi can chan doan su co.
	// CoreServer.dll PHAI tu nan dong RIENG cua no (CoreServerShell.cpp): ca hai
	// deu dung CRT TINH (RuntimeLibrary = MultiThreaded) nen moi ben mot ban CRT,
	// tuc HAI stdout khac nhau - va vi the cung phai ghi ra HAI TEP khac nhau,
	// khong thi hai bo dem se ghi chen vao nhau lam nat dong.
	{
		FILE* fOut = freopen("GameServer.log", "a", stdout);
		FILE* fErr = freopen("GameServer.log", "a", stderr);
		if (fOut) setvbuf(fOut, NULL, _IOLBF, 4096);   // xa theo DONG: tat dot ngot
		if (fErr) setvbuf(fErr, NULL, _IONBF, 0);      // van con dong cuoi de doc
	}

	ClearConsoleToColors(7,1);         //function call to change console background color

	BOOL bRunning = TRUE;
	if (argc == 2)
	{
		g_nPort = atoi(argv[1]);
	}
/*#ifdef __linux
	init_daemon();
#endif*/

	printf("==Main thread server is starting ......==\n");

	TCHAR moduleFileName[MAX_PATH+1];
	DWORD moduleFileNameLen = MAX_PATH;
	char szBuffer[MAX_PATH+1]={0};
	unsigned uRetCode = 0;
	uRetCode = ::GetModuleFileName(NULL,moduleFileName,moduleFileNameLen);
	sprintf(szBuffer,"%s",moduleFileName);

	char szExe[MAX_PATH]="";
	char* szLine = strtok(moduleFileName,"\\");
	while(NULL!=szLine)
	{
		strcpy(szExe,szLine);
		szLine = strtok(NULL,"\\");
	}
	int lencopy = strlen(szExe)-4;
	char moduleFileNamenew[MAX_PATH]={0};
	strncpy(moduleFileNamenew,szExe,lencopy);

	if (!g_SOServer.InitServer(moduleFileNamenew))
	{
		bRunning = FALSE;
		Sleep(2000);
		ExitProcess(0);
		return 0;
	}

	while(bRunning)
	{
		bRunning = g_SOServer.Breathe();
	}

	g_SOServer.Release();
	return 1;
}

void ClearConsoleToColors(int ForgC, int BackC) //edit by phong kieu change GS console corlor
{
     WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
     //This is used to get the handle to current output buffer.
     
     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     //This is used to reset the carat/cursor to the top left.
     
     COORD coord = {0, 0};
                      //This is a return value indicating how many characterss were written
                        //   It is not used but we need to capture this since it will be
                          //   written anyway (passing NULL causes an access violation).
                          
     DWORD count;
 
                                   //This is a structure containing all the console info
                          // It is used here to find the size of the console.
                          
     CONSOLE_SCREEN_BUFFER_INFO csbi;
                     //Now the current color will be set by this handle
                     
     SetConsoleTextAttribute(hStdOut, wColor);
     
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
                              //This fills the buffer with a given character (in this case 32=space).
          FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
 
          FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
                              //This will set our cursor position for the next print statement
          SetConsoleCursorPosition(hStdOut, coord);
     }
     return;
}

#ifdef _STANDALONE
extern "C"
{
void lua_outerrmsg(const char * szerrmsg)
{
	fprintf(stderr, szerrmsg);
}
};
#endif