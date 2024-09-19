// Bishop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "Application.h"

#include "Macro.h"

#include "Dbghelp.h"

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
				, "bishop" , szVersion, 
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

	return GenerateMiniDump(NULL, lpExceptionInfo, L"bishop");
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	SetUnhandledExceptionFilter(ExeptionFillert);

	//int* pbadPtr;	//add by tuanln test
	//*pbadPtr = 0;

	/*
	 * Add this funtion by liupeng on 2003
	 * We can find some error when start a console tracer
	 *
	 * Please check 'project-setting'
	 */
#ifdef	CONSOLE_DEBUG

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

	cprintf( "Startup application!\n" );

#endif

	CBishopApp app( hInstance );

	int nRet = app.Run();

#ifdef CONSOLE_DEBUG

	cprintf( "End of application!\n" );

	if ( bOpenTracer )
	{
		FreeConsole();
	}

#endif
	
	return nRet;
}



