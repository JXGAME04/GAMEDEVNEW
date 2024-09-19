#include "KWin32.h"
#include "CRC32.h"
#include "CheckSumCRC.h"
#include "../../S3Config.h"

void Msg_Error_CRC_CoreClient()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "CoreClient.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "CoreClient.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_Engine()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Engine.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Engine.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_Heaven()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Heaven.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Heaven.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_Rainbow()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Rainbow.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Rainbow.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_Represent2()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Represent2.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Represent2.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_Represent3()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Represent3.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Represent3.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_LuaLibDll()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "LuaLibDll.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "LuaLibDll.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_FilterText()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "FilterText.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "FilterText.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_AntihackDll()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "Antihack.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "Antihack.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void Msg_Error_CRC_AntiHideMain()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL, "AntiHideMain.dll Corrupted or missing file!", "Error", MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL, "AntiHideMain.dll da bi hong hoac da bi thay doi!", "Error", MB_OK | MB_ICONSTOP);
	}
}

void CheckSumCRC()
{
	BOOL bExit = FALSE;
	CRC32 MyCRC32;
	MyCRC32.Initialize();
	unsigned long ulCRC1 = MyCRC32.FileCRC("CoreClient.dll");
	unsigned long ulCRC2 = MyCRC32.FileCRC("Heaven.dll");
	unsigned long ulCRC3 = MyCRC32.FileCRC("Rainbow.dll");
	unsigned long ulCRC4 = MyCRC32.FileCRC("Engine.dll");
	unsigned long ulCRC5 = MyCRC32.FileCRC("Represent2.dll");
	unsigned long ulCRC6 = MyCRC32.FileCRC("Represent3.dll");
	unsigned long ulCRC7 = MyCRC32.FileCRC("LuaLibDll.dll");
	unsigned long ulCRC8 = MyCRC32.FileCRC("FilterText.dll");
	unsigned long ulCRC9 = MyCRC32.FileCRC("AntiHack.dll");
	unsigned long ulCRC10 = MyCRC32.FileCRC("AntiHideMain.dll");
	if (ulCRC1 != 2645134642)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_CoreClient),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC2 != 2048231004)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_Heaven),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC3 != 355585020)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_Rainbow),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC4 != 2026648496)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_Engine),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC5 != 340351167)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_Represent2),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC6 != 3272586251)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_Represent3),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC7 != 1525568251)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_LuaLibDll),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC8 != 905106875)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_FilterText),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC9 != 1655987112)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_AntihackDll),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}
	if (ulCRC10 != 3232310157)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Error_CRC_AntiHideMain),NULL,0,0);
		Sleep(3000);
		bExit = TRUE;
		goto ExitGame;
	}

ExitGame:
	if (bExit)
	{
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

void ScanCheckSumCRC()
{
again:
	CheckSumCRC();
	Sleep (30000);
	goto again;
}

void ThreadCheckSumCRC()
{
	if (CHECKSUM_FILE)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(ScanCheckSumCRC),NULL,0,0);
	}
}
