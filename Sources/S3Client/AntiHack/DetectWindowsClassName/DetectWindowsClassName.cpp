#include "KWin32.h"
#include "DetectWindowsClassName.h"
#include "../../S3Config.h"

//Anti Detect ClassName
void Msg_WindowsClassName()
{
	if (MESSAGE_WARNING_EN == 1)
	{
		MessageBoxA(NULL,"ClassName-Scan\nAn illegal choice has been detected!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);	
	}
	else
	{
		MessageBoxA(NULL,"ClassName-Scan\nPhat hien co dau hieu hack!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);	
	}
} 

void ClassWindowsScanner(LPCSTR WindowClass)
{
	HWND WinClass = FindWindowExA(NULL,NULL,WindowClass,NULL);
	if( WinClass > 0)
	{
		if (GAME_GUARD_LOG == 1)
		{
			using namespace std;
			ofstream out(TXT_GAME_GUARD_LOG, ios::app);
			out << "\nClassName-Scan: ", out <<   WindowClass;
		}
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_WindowsClassName),NULL,0,0);
		Sleep(5000); 
		ExitProcess(0);	 
	}
}

void WindowsClassNameScanner()
{    
	//ClassWindowsScanner("ConsoleWindowClass");   // Prompt de comando 
	//ClassWindowsScanner("ThunderRT6FormDC");    //autoclic Klic0r
	ClassWindowsScanner("PROCEXPL");             // Process explorer
	ClassWindowsScanner("TreeListWindowClass");  // Process explorer (Process windows)
	ClassWindowsScanner("ProcessHacker");        // Process Hacker	
	ClassWindowsScanner("PhTreeNew");            // Process Hakcer (Process windows)
	ClassWindowsScanner("RegEdit_RegEdit");      // Regedit
	ClassWindowsScanner("0x150114 (1376532)");   // Win 7 - System configuration
	//ClassWindowsScanner("SysListView32");        // Lista de processos do process explorer
	ClassWindowsScanner("Tmb");
	ClassWindowsScanner("TformSettings");
	ClassWindowsScanner("Afx:400000:8:10011:0:20575");
	ClassWindowsScanner("TWildProxyMain");
	ClassWindowsScanner("TUserdefinedform");
	ClassWindowsScanner("TformAddressChange");
	ClassWindowsScanner("TMemoryBrowser");
	ClassWindowsScanner("TFoundCodeDialog");
}

void WindowsClassNameScan()
{
again:
	WindowsClassNameScanner();
	Sleep(30000);
	goto again;
}

void ThreadWindowsClassName()
{
	if (ANTI_CLASS_NAME_SCAN)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(WindowsClassNameScan),NULL,0,0);
	}
}
