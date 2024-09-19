#include "KWin32.h"
#include "DetectHide.h"
#include "../../S3Config.h"

void Msg_Detect_Hide()
{
	if (MESSAGE_WARNING_EN)
	{
		MessageBoxA(NULL,"Hide-Scan\nPhat hien phan mem chay an!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
	else
	{
		MessageBoxA(NULL,"Hide-Scan\nPhat hien phan mem chay an!", MESSAGE_BOX_TITLE, MB_OK | MB_ICONSTOP);
	}
}

void Detect_Hide_Scanner()
{
	HWND hWnd;
    DWORD dwProcId;
    CHAR szTitle[255];
    CHAR szClass[255];
	
	hWnd = FindWindow(0,0);
	if ( hWnd > 0)
	{
		if (GetParent(hWnd) == 0)
		{
            GetWindowThreadProcessId(hWnd, &dwProcId);
			if (!OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcId))
			{
                GetWindowTextA(hWnd, szTitle, 255);
                GetClassNameA(hWnd, szClass, 255);
				
				if (GAME_GUARD_LOG == 1)
				{
					using namespace std;
					ofstream out(TXT_GAME_GUARD_LOG, ios::app);
					out <<"\nHide-Scan: ProcessId: ", out <<dwProcId, out <<" - Class Name: ", 
						out <<szClass, out <<" - Windows Title: ", out <<szTitle, out <<" - Windows Handle: ", out <<hWnd,
						out <<" - Windows State: ", out <<(IsWindowVisible(hWnd) ? "Shown" : "Hidden");
				}
				
                if (IsWindow(hWnd))
                {
					HANDLE pr_open_close = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcId);
					TerminateProcess (pr_open_close, 0);
					CloseHandle(pr_open_close);
					
                    /*SendMessageA(hWnd, WM_CLOSE, 0, 0);
                    SendMessageA(hWnd, WM_QUIT, 0, 0);
                    SendMessageA(hWnd, WM_DESTROY, 0, 0);*/
                }
				CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Msg_Detect_Hide),NULL,0,0);
				Sleep (5000);
				ExitProcess(0);
			}
		}
	}
}

void Detect_Hide_Scan()
{
again:
	Detect_Hide_Scanner();
	Sleep(1000);
	goto again;
}

void ThreadDetectHide()
{
	if (ANTI_DETECT_HIDE)
	{
		CreateThread(NULL,NULL,LPTHREAD_START_ROUTINE(Detect_Hide_Scan),NULL,0,0);
	}
}
