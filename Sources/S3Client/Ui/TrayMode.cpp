#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KWin32App.h"
#include "TrayMode.h"
// static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
TrayMode gTrayMode;
// ----------------------------------------------------------------------------------------------

void TrayMode::HideNotify(HINSTANCE hInst)
{	
	Icon.cbSize				= sizeof(NOTIFYICONDATA);
	Icon.uID				= TRAYMODE_ICON_ID;
	Icon.uFlags				= NIF_ICON|NIF_MESSAGE|NIF_TIP;
	Icon.hWnd				= g_GetMainHWnd();
	Icon.uCallbackMessage	= TRAYMODE_ICON_MESSAGE; 
	Icon.hIcon  = LoadIcon(hInst, MAKEINTATOM(SWORD_ICON));
	strcpy(Icon.szTip,"Vo Lam Ngao The");

	ShowWindow(g_GetMainHWnd(), SW_HIDE);
	UpdateWindow(g_GetMainHWnd());	
	Shell_NotifyIcon(NIM_ADD, &Icon);	
}


void TrayMode::ShowNotify()
{	
	ShowWindow(g_GetMainHWnd(), SW_SHOW);
	UpdateWindow(g_GetMainHWnd());
	Shell_NotifyIcon(NIM_DELETE, &Icon);	
	DeleteObject(Icon.hIcon);
}

