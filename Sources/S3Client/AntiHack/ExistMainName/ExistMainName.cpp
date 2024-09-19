#include "KWin32.h"
#include "ExistMainName.h"
#include "../../S3Config.h"

//Exist Main Name 
void Msg_Exist_Main_Name()
{
    if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Warning!\nInvalid file name detected!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Canh bao!\nPhat hien ten tap tin khong hop le!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
}

void ExistMainName()
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	if(GetFileAttributesEx(TXT_MAIN_NAME,GetFileExInfoStandard,&wfad) == 0)
	{	
		if (GAME_GUARD_LOG)
		{
			using namespace std;
			ofstream out(TXT_GAME_GUARD_LOG, ios::app);
			out << "\nFile-Scan:  ", out <<   "Invalid file name detected!";
		}
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Exist_Main_Name),NULL,0,0);
		Sleep (3000);
		DWORD MyProcessId = GetCurrentProcessId();
		if (MyProcessId)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, MyProcessId);
			if (hProcess)
			{
				TerminateProcess(hProcess,0);
				CloseHandle(hProcess);
				exit(0);
			}
		}
	}
}

void ScanExistMainName()
{
again:
	ExistMainName();
	Sleep(30000);
    goto again;
}

void ThreadExistMainName()
{
	if (ANTI_EXIST_MAIN_NAME)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(ScanExistMainName),NULL,0,0);
	}
}
