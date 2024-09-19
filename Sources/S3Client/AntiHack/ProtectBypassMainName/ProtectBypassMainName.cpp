#include "KWin32.h"
#include "ProtectBypassMainName.h"
#include "../../S3Client.h"
#include "../../S3Config.h"
#include "psapi.h"

//Anti bypass main name
void ProtectBypassMainName()
{
	DWORD MyProcessId = GetCurrentProcessId();
	if (MyProcessId)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, MyProcessId);
		if (hProcess)
		{
			TCHAR Buffer[MAX_PATH];
			if (GetModuleBaseName(hProcess, 0, Buffer, MAX_PATH))
			{
				if (strcmp(LowCaseChanger(Buffer),LowCaseChanger(TXT_MAIN_NAME)))
				{
					TerminateProcess(hProcess,0);
					CloseHandle(hProcess);
					exit(0);
				}
			}
			CloseHandle(hProcess);
		}
	}
}

void ScanProtectBypassMainName()
{
again:
	ProtectBypassMainName();
	Sleep(30000);
	goto again;
}

void ThreadProtectBypassMainName()
{
	if (ANTI_BYPASS_MAIN_NAME)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(ScanProtectBypassMainName),NULL,0,0);
	}
}
