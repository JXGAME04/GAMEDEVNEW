#include "stdafx.h"
#include "application.h"
#include "resource.h"
#include <tlhelp32.h>
#include <process.h>
#include <time.h>

#include "GameServer.h"
#include "GamePlayer.h"

#include "KProtocolDef.h"
#include "AccountLoginDef.h"
#include "Buffer.h"

#include "msg_define.h"
#include "Macro.h"
#include "../Engine/Src/KWin32.h"
#include "../Engine/Src/KSG_MD5_String.h"
#include "inoutmac.h"

#include "SmartClient.h"
#include "FilterTextLib.h"
CFilterTextLib g_libFilterText;

using OnlineGameLib::Win32::CPackager;

static const char szBishopClassName[] = "StartGameSVClass";
//static const DWORD g_dwServerIdentify = 0xAEFC07B5;
static int countdowncheckGS = 0;
static HWND hWndMain;

HINSTANCE		CBishopApp::m_hInst = NULL;
CNetwork		CBishopApp::m_theNetwork;

CIntercessor	*CBishopApp::m_pIntercessor = NULL;

///////////////////////////////////////////////////////////////////////////////////////////
/*
#include <nb30.h>

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

// 输入参数：lana_num为网卡编号，一般地，从0开始，但在Windows 2000中并不一定是连续分配的 
void getmac_one(int lana_num, ASTAT& Adapter)
{
    NCB ncb;
    UCHAR uRetCode;

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = lana_num;   
    // 指定网卡号

	// 首先对选定的网卡发送一个NCBRESET命令，以便进行初始化 
    uRetCode = Netbios( &ncb );

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_num;     // 指定网卡号

    strcpy( (char *)ncb.ncb_callname, "*               ");
    ncb.ncb_buffer = (unsigned char *) &Adapter;

	// 指定返回的信息存放的变量 
    ncb.ncb_length = sizeof(Adapter);

	// 接着，可以发送NCBASTAT命令以获取网卡的信息 
    uRetCode = Netbios( &ncb );

    if ( uRetCode == 0 )
    {
		// 把网卡MAC地址格式化成常用的16进制形式，如0010-A4E4-5802 
//		lana_num
//		  Adapter.adapt.adapter_address[0],
//		  Adapter.adapt.adapter_address[1],
//		  Adapter.adapt.adapter_address[2],
//		  Adapter.adapt.adapter_address[3],
//		  Adapter.adapt.adapter_address[4],
//		  Adapter.adapt.adapter_address[5];
	}
}

int getmac_all(ASTAT** ppBuffer)
{
	NCB ncb;
    UCHAR uRetCode;
    LANA_ENUM lana_enum;

	lana_enum.length = 0;

    memset( &ncb, 0, sizeof(ncb) );
    ncb.ncb_command = NCBENUM;

    ncb.ncb_buffer = (unsigned char *) &lana_enum;
    ncb.ncb_length = sizeof(lana_enum);

	// 向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等 
	uRetCode = Netbios( &ncb );
	if ( uRetCode == 0 && lana_enum.length > 0)
	{
		*ppBuffer = new ASTAT[lana_enum.length];
		// 对每一张网卡，以其网卡编号为输入编号，获取其MAC地址 
		for ( int i = 0; i < lana_enum.length; ++i)
		{
			getmac_one(lana_enum.lana[i], (*ppBuffer)[i]);
		}
	}

	return lana_enum.length;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////

CBishopApp::CBishopApp()
		: m_theOnlyOneInstance( NULL, FALSE, TRUE, szBishopClassName )
{
}

CBishopApp::CBishopApp( HINSTANCE hInstance )
		: m_theOnlyOneInstance( NULL, FALSE, TRUE, szBishopClassName )
{
	m_hInst = hInstance;
}

CBishopApp::~CBishopApp()
{

}

static char folderServer[MAX_PATH];

struct EnumData {
    DWORD dwProcessId;
    HWND hWnd;
};

BOOL CALLBACK EnumProc( HWND hWnd, LPARAM lParam ) {
    // Retrieve storage location for communication data
    EnumData& ed = *(EnumData*)lParam;
    DWORD dwProcessId = 0x0;
    // Query process ID for hWnd
    GetWindowThreadProcessId( hWnd, &dwProcessId );
    // Apply filter - if you want to implement additional restrictions,
    // this is the place to do so.
    if ( ed.dwProcessId == dwProcessId ) {
        // Found a window matching the process ID
        ed.hWnd = hWnd;
        // Report success
        SetLastError( ERROR_SUCCESS );
        // Stop enumeration
        return FALSE;
    }
    // Continue enumeration
    return TRUE;
}

HWND FindWindowFromProcessId( DWORD dwProcessId ) {
    EnumData ed = { dwProcessId };
    if ( !EnumWindows( EnumProc, (LPARAM)&ed ) &&
         ( GetLastError() == ERROR_SUCCESS ) ) {
        return ed.hWnd;
    }
    return NULL;
}

HWND CheckWindowsRuningForName(char *processName)
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
				//exists = true;
				return FindWindowFromProcessId(entry.th32ProcessID);
			}
		}

    CloseHandle(snapshot);
    return NULL;
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


DWORD WINAPI myThread(LPVOID lpParameter)
{
	unsigned int& myCounter = *((unsigned int*)lpParameter);
	//while(myCounter < 0xFFFFFFFF) 
	//	++myCounter;
	while(1)
	{
		Sleep(3000);
		countdowncheckGS+=3000;
		if(countdowncheckGS >= 1000*60*1)
		{
			SendMessage(hWndMain,WM_CLOSE,0,0);
			break;
		}
	}
	return 0;
}

int CBishopApp::Run()
{
	/* Seed the random-number generator with current time so that
	 * the numbers will be different every time we run.
	 */
	srand( ( unsigned )time( NULL ) );

	m_theNetwork.Create();

	if (!g_libFilterText.Initialize())
	{
		//MessageBox(NULL, "text filter's initing has failed", "error", MB_OK|MB_ICONERROR);
		return 0;
	}

	//DWORD g1 = g_libFilterText.CreateTextFilter1();
	//if(g1 < 4)
	//	return 0;

	unsigned int myCounter = 0;
	DWORD myThreadID;
	HANDLE myHandle = ::CreateThread(0, 0, myThread, &myCounter, 0, &myThreadID);
	CloseHandle(myHandle);


	/*
	 * To affirm account that it is used to check the application
	 */

	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	sprintf(folderServer, "%s\\", NPath);

	//g_theSmartClient.Open( "192.168.1.128", 5002, NULL, "free", "1" );
	g_theSmartClient.Open( "103.77.167.8", 5002, NULL, m_theNetwork.GetInputUid(), m_theNetwork.GetInputPwd());

	if ( g_theSmartClient.Valid() )
	{
		m_pIntercessor = new CIntercessor( 500, m_theNetwork );

		ASSERT( m_pIntercessor );

		MainDialog();
	}

	m_theNetwork.Destroy();
	g_libFilterText.Uninitialize();

	return 0;
}


/*
 * Log on to system
 */
bool CBishopApp::LoginSystem()
{

	//long res = ::DialogBox( m_hInst, MAKEINTRESOURCE( IDD_DLG_LOGIN ), NULL, ( DLGPROC )LoginDlgProc );

	//if ( WM_SERVER_LOGIN_SUCCESSFUL ==  res)
	//{
		//if ( CheckUserInfo( (HWND)LoginDlgProc ) )
		//{
			//::PostMessage( (HWND)LoginDlgProc, WM_ENTER_AFFIRM, 0L, 0L );
			//return true;
		//}
	//}

	return false;
}

BOOL CALLBACK CBishopApp::LoginDlgProc( HWND hwndDlg,
		UINT message, 
		WPARAM wParam, 
		LPARAM lParam )
{
	char szBuffer[MAX_PATH];

    switch (message)
    {
	case WM_SERVER_LOGIN_SUCCESSFUL:

		::EndDialog( hwndDlg, WM_SERVER_LOGIN_SUCCESSFUL );
		
		break;

	case WM_SERVER_LOGIN_FAILED:

		switch( wParam )
		{
		case enumUsrNamePswdErr:

			::LoadString( m_hInst, IDS_NAMEPSWD_ERROR, szBuffer, MAX_PATH );

			::MessageBox( hwndDlg, szBuffer, NULL, MB_ICONEXCLAMATION );

			::PostMessage( hwndDlg, WM_LEAVE_AFFIRM, 0L, 0L );

			break;

		case enumConnectFailed:
		case enumIPPortErr:

			::LoadString( m_hInst, IDS_NETWORK_ERROR, szBuffer, MAX_PATH );

			::MessageBox( hwndDlg, szBuffer, NULL, MB_ICONEXCLAMATION );

			::PostMessage( hwndDlg, WM_LEAVE_AFFIRM, 0L, 0L );

			break;

		case enumException:
		default:

			::MessageBox( hwndDlg, "Exception!", NULL, MB_ICONEXCLAMATION );

			::EndDialog( hwndDlg, WM_SERVER_LOGIN_FAILED );

			break;
		}
		
		break;

	case WM_ENTER_AFFIRM:

		EnterToAffirm( hwndDlg );
		
		break;

	case WM_LEAVE_AFFIRM:

		LeaveToAffirm( hwndDlg );

		break;

	case WM_INITDIALOG:

		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ) );
		
		::SetDlgItemText( hwndDlg, IDC_EDIT_LOGONTO_IP, m_theNetwork.GetAccSvrIP() );
		::SetDlgItemInt( hwndDlg, IDC_EDIT_LOGONTO_PORT, m_theNetwork.GetAccSvrPort(), FALSE );


		//::SetDlgItemInt( hwndDlg, IDC_EDIT_USERNAME, m_theNetwork.GetInputUid(), FALSE );
		//::SetDlgItemInt( hwndDlg, IDC_EDIT_PASSWORD, m_theNetwork.GetInputPwd(), FALSE );

		//std::_tstring userNameL1 = gGetPrivateProfileStringEx(sec_root, key_account, file_relaycfg);
		//std::_tstring passWordL1 = gGetPrivateProfileStringEx(sec_root, key_password, file_relaycfg);
		//char* userNameL2 = const_cast<char*>(userNameL1.c_str());
		//char* passWordL2 = const_cast<char*>(passWordL1.c_str());

		::SetDlgItemText( hwndDlg, IDC_EDIT_USERNAME, m_theNetwork.GetInputUid());
		::SetDlgItemText( hwndDlg, IDC_EDIT_PASSWORD,  m_theNetwork.GetInputPwd());
		
		break;

	case WM_COMMAND:

		switch ( LOWORD( wParam ) )
		{
		case IDBS_OK_MODE:
		case IDOK:

			if ( CheckUserInfo( hwndDlg ) )
			{
				::PostMessage( hwndDlg, WM_ENTER_AFFIRM, 0L, 0L );
			}

			break;

		case IDCANCEL:
			
			::EndDialog( hwndDlg, WM_SERVER_LOGIN_FAILED );

			return TRUE; 
		} 
    }

    return FALSE;
}

void CBishopApp::EnterToAffirm( HWND hwndDlg )
{
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_PASSWORD ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_IP ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_PORT ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDOK ), FALSE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDCANCEL ), FALSE );

	/*
	 * Start up a listening thread to wait a required confirm from the account server
	 */
	//DWORD dwThreadID = 0;

	//HANDLE hThread = ::CreateThread( NULL, 0, ServerLoginRoutine, ( void * )hwndDlg, 0, &dwThreadID );
	
	//SAFE_CLOSEHANDLE( hThread );

	ServerLoginRoutine( hwndDlg );
}

void CBishopApp::LeaveToAffirm( HWND hwndDlg )
{
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_PASSWORD ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_IP ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_PORT ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDOK ), TRUE );
	::EnableWindow( ::GetDlgItem( hwndDlg, IDCANCEL ), TRUE );
}

bool CBishopApp::CheckUserInfo( HWND hwndDlg )
{
	UINT	nLen = 0;
	char	szBuffer[MAX_PATH];
	
	if ( 0 == ( nLen = ::GetDlgItemText( hwndDlg, IDC_EDIT_USERNAME, szBuffer, MAX_PATH ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_USERNAME ) );

		::MessageBox( hwndDlg, "Please input a valid username!", "Warning", MB_ICONASTERISK );

		return false;
	}

	if ( 0 == ( nLen = ::GetDlgItemText( hwndDlg, IDC_EDIT_LOGONTO_IP, szBuffer, MAX_PATH ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_IP ) );

		::MessageBox( hwndDlg, "Please input a valid IP!", "Warning", MB_ICONASTERISK );

		return false;
	}
	
	BOOL bTranslated = FALSE;

	if ( 0 == ( nLen = ::GetDlgItemInt( hwndDlg, IDC_EDIT_LOGONTO_PORT, &bTranslated, TRUE ) ) )
	{
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_LOGONTO_PORT ) );

		::MessageBox( hwndDlg, "Please input a valid port!", "Warning", MB_ICONASTERISK );

		return false;
	}

	return true;
}

DWORD WINAPI CBishopApp::ServerLoginRoutine( HWND hwndDlg )
{
	ASSERT( hwndDlg );

	/*
	 * Ask for log on to the account server
	 */

	UINT nPort = 0;

	char szBuffer[MAX_PATH];
	char szUsername[NAME_PWD_EX_LEN];
	char szPassword[NAME_PWD_EX_LEN];

	::GetDlgItemText( hwndDlg, IDC_EDIT_LOGONTO_IP, szBuffer, MAX_PATH );

	BOOL bTranslated = TRUE;

	nPort = ::GetDlgItemInt( hwndDlg, IDC_EDIT_LOGONTO_PORT, &bTranslated, TRUE );

	::GetDlgItemText( hwndDlg, IDC_EDIT_USERNAME, szUsername, NAME_PWD_EX_LEN );

	::GetDlgItemText( hwndDlg, IDC_EDIT_PASSWORD, szPassword, NAME_PWD_EX_LEN );

	g_theSmartClient.Open( szBuffer, nPort, hwndDlg, szUsername, szPassword );

/*	IClient *pAccSvrClient = m_theNetwork.CreateAccSvrClient( szBuffer, nPort );

	if ( !pAccSvrClient )
	{
		::PostMessage( hwndDlg, WM_SERVER_LOGIN_FAILED, enumConnectFailed, 0L );

		SAFE_RELEASE( pAccSvrClient );
		return 1L;
	}
*/
	if ( !g_theSmartClient.Valid() )
	{
		::PostMessage( hwndDlg, WM_SERVER_LOGIN_FAILED, enumConnectFailed, 0L );

		return 1L;
	}

	//SAFE_RELEASE( pAccSvrClient );

	return 0L;
}

BOOL CBishopApp::RegisterMainWndClass()
{
    WNDCLASSEX wcx; 
 
    /*
	 * Fill in the window class structure with parameters 
     * that describe the main window. 
	 */
 
    wcx.cbSize = sizeof( wcx );					// size of structure 
    wcx.style = CS_HREDRAW | CS_VREDRAW;		// redraw if size changes 
    wcx.lpfnWndProc = ( WNDPROC )MainWndProc;	// points to window procedure 
    wcx.cbClsExtra = 0;							// no extra class memory 
    wcx.cbWndExtra = 0;							// no extra window memory 
    wcx.hInstance = m_hInst;					// handle to instance 
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);						// predefined app. icon 
    wcx.hCursor = LoadCursor(NULL, 
        IDC_ARROW);								// predefined arrow 
    wcx.hbrBackground = ( HBRUSH )GetStockObject( 
        WHITE_BRUSH);							// white background brush 
    wcx.lpszMenuName =  NULL;					// name of menu resource 
    wcx.lpszClassName = _T( "IntercessorClass" );	// name of window class 
    wcx.hIconSm = ( HICON )LoadImage(m_hInst,		// small class icon 
        MAKEINTRESOURCE(IDI_ICON1),
        IMAGE_ICON, 
        GetSystemMetrics(SM_CXSMICON), 
        GetSystemMetrics(SM_CYSMICON), 
        LR_DEFAULTCOLOR); 
 
    /*
	 * Register the window class. 
	 */ 
    return ::RegisterClassEx( &wcx );
}

UINT CBishopApp::MainDialog()
{
	/*
	 * Create the main window as dialog. 
	 */
/*	if ( !RegisterMainWndClass() )
	{
		return 0L;
	}
*/
	HWND hwndMain = ::CreateDialog( m_hInst, 
		MAKEINTRESOURCE( IDD_DLG_INTERCESSOR ),
		NULL, 
		(DLGPROC)MainWndProc );

	ASSERT( hwndMain );
	
	hWndMain = hwndMain;
	::ShowWindow( hwndMain, SW_SHOWNORMAL );
	::UpdateWindow( hwndMain );
	
	BOOL bRet;
	MSG msg;

	HACCEL hAccel = LoadAccelerators(m_hInst, MAKEINTRESOURCE(IDRBS_ACCEL1));
	
	/*
	while ( ( bRet = ::GetMessage( &msg, NULL, 0, 0 ) ) != 0 )
	{
		if ( !IsWindow( hwndMain ) || !IsDialogMessage( hwndMain, &msg ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}*/
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			if (!TranslateAccelerator( 
					hwndMain,      // handle to receiving window 
					hAccel,        // handle to active accelerator table 
					&msg))         // message data 
			{
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			} 
		} 
	} 
	
	return msg.wParam;
}

/*
 *
 * MainWndProc() - Main window callback procedure.
 *  
 */

static UINT g_nViewTimer = 0x100;
static UINT g_nTimer = 0;
static BOOL checksword3paysys = FALSE;
static BOOL checks3relayserver = FALSE;
static BOOL checkgoddess = FALSE;
static BOOL checkbishop = FALSE;
static BOOL checks3relay = FALSE;
static BOOL checkgameserver = FALSE;

BOOL CALLBACK CBishopApp::MainWndProc( HWND hwnd, 
                           UINT msg,
                           WPARAM wParam,
                           LPARAM lParam )
{
	static size_t nGameServerNumber = 0;
	static char szInfo[64];

	UINT uLength = 0;
	TCHAR szAnnounceText[260];

	switch ( msg )
	{
	case WM_INITDIALOG:
		
		/*
		 * Initialize
		 */
		InitMainDlg( hwnd );
		
		return TRUE;

		break;

	case WM_CLOSE:
		
		//if ( IDYES == ::MessageBox( hwnd, "Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION ) )
		//{
			if ( g_nTimer )
			{
				::KillTimer( hwnd, g_nViewTimer );

				g_nTimer = 0;
			}

			CloseMainDlg( hwnd );
			
			if ( m_pIntercessor )
			{
				m_pIntercessor->Destroy();
			}
			
			::DestroyWindow( hwnd );
			::PostQuitMessage( 0 );
		//}	

		return TRUE;

		break;

	case WM_TIMER:
		if ( g_nViewTimer == wParam )
		{
			LONG nc = CGamePlayer::GetCapability();
			LONG nw = CGamePlayer::GetWorking();
			
			sprintf( szInfo, "%d / %d", nw, nc );

			::SetDlgItemText( hwnd, IDC_STATIC_TASK, ( LPCTSTR )szInfo );
		}
		break;
	
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDBS_START_MODE:
			::EnableWindow( GetDlgItem( hwnd, IDOK ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_CLIENT_PORT ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_GAMESVR_PORT ), FALSE );

			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_ROLESVRIP ), FALSE );
			::EnableWindow( GetDlgItem( hwnd, IDC_EDIT_ROLESVRPORT ), FALSE );
			
			::CheckDlgButton( hwnd, IDC_CHECK_PLAYER, BST_CHECKED );
			::CheckDlgButton( hwnd, IDC_CHECK_GAMESVR, BST_CHECKED );
			
			::CheckDlgButton( hwnd, IDC_CHECK_ACCSVR, BST_CHECKED );
			
			UpdateVariable( hwnd );

			if ( !m_pIntercessor->Create( hwnd ) )
			{
				::EnableWindow( GetDlgItem( hwnd, IDOK ), TRUE );
				
				m_pIntercessor->Destroy();
			}
			else
			{
				g_nTimer = ::SetTimer( hwnd, g_nViewTimer, 5000, NULL );

				::SetWindowText( hwnd, "StartGameSV" );
			}
			break;
		}
		switch ( wParam )
		{
		case IDC_BUTTON1:
			{
				char pName1[MAX_PATH] = "s3relayserver.exe";
				::EnableWindow( GetDlgItem( hwnd, IDC_BUTTON6), TRUE );
				HWND h111 = CheckWindowsRuningForName(pName1);
				if(!h111)
				{
					char teamp[MAX_PATH];
					sprintf(teamp, "%s%s", folderServer, pName1);
					BOOL res1 = OnExecute (teamp);
					Sleep(1000);
					 h111 = CheckWindowsRuningForName(pName1);
					 ::SetDlgItemText( hwnd, IDC_STATIC1, "1 running ....");
					 ::ShowWindow(h111, SW_MINIMIZE);
					if(!res1)
					{
						MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
					}
				}
				else
				{
					if(checks3relayserver)
						::ShowWindow(h111, 0);
					else
						::ShowWindow(h111, 1);

					checks3relayserver = !checks3relayserver;
				}
			}
			break;
		case IDC_BUTTON6:
			{
				char pName1[MAX_PATH] = "sword3paysys.exe";
				::EnableWindow( GetDlgItem( hwnd, IDC_BUTTON2), TRUE );
				HWND h111 = CheckWindowsRuningForName(pName1);
				if(!h111)
				{
					char teamp[MAX_PATH];
					sprintf(teamp, "%s%s", folderServer, pName1);
					BOOL res1 = OnExecute (teamp);
					Sleep(1000);
					 h111 = CheckWindowsRuningForName(pName1);
					 ::ShowWindow(h111, SW_MINIMIZE);
					 ::SetDlgItemText( hwnd, IDC_STATIC2, "2 running ....");
					if(!res1)
					{
						MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
					}
				}
				else
				{
					if(checksword3paysys)
						::ShowWindow(h111, 0);
					else
						::ShowWindow(h111, 1);

					checksword3paysys = !checksword3paysys;
				}
			}
			break;
				case IDC_BUTTON2:
					{
						char pName1[MAX_PATH] = "goddess.exe";
						::EnableWindow( GetDlgItem( hwnd, IDC_BUTTON4), TRUE );
						HWND h111 = CheckWindowsRuningForName(pName1);
						if(!h111)
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
							h111 = CheckWindowsRuningForName(pName1);
							::ShowWindow(h111, SW_MINIMIZE);
							::SetDlgItemText( hwnd, IDC_STATIC3, "3 running ....");
							//UINT nID = FindAcceleratorCommandId(_T("goddess.exe"), 'S', TRUE, FALSE, FALSE);
							//HWND hWndMain = ::FindWindow(NULL,_T("Goddess - [Disable]"));
							//if(nID > 0 && hWndMain)
							//{
							//  WPARAM wParam = MAKEWPARAM(nID, 1);
							//  ::SendMessage(hWndMain, WM_COMMAND, wParam, NULL);
							//}
							g2 = g_libFilterText.CreateTextFilter2(0);
						}
						else
						{
							if(checkgoddess)
								::ShowWindow(h111, 0);
							else
								::ShowWindow(h111, 1);

							checkgoddess = !checkgoddess;
						}
					}
				   break;
				case IDC_BUTTON4:
					{
						char pName1[MAX_PATH] = "bishop.exe";
						::EnableWindow( GetDlgItem( hwnd, IDC_BUTTON3), TRUE );
						HWND h111 = CheckWindowsRuningForName(pName1);
						if(!h111)
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
							h111 = CheckWindowsRuningForName(pName1);
							::ShowWindow(h111, SW_MINIMIZE);
							::SetDlgItemText( hwnd, IDC_STATIC4, "4 running ....");
							g2 = g_libFilterText.CreateTextFilter2(0);
						}
						else
						{
							if(checkbishop)
								::ShowWindow(h111, 0);
							else
								::ShowWindow(h111, 1);

							checkbishop = !checkbishop;
						}
					}
				   break;
				case IDC_BUTTON3:
					{
						char pName1[MAX_PATH] = "s3relay.exe";
						::EnableWindow( GetDlgItem( hwnd, IDC_BUTTON5 ), TRUE );
						HWND h111 = CheckWindowsRuningForName(pName1);
						if(!h111)
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
							h111 = CheckWindowsRuningForName(pName1);
							::ShowWindow(h111, SW_MINIMIZE);
							::SetDlgItemText( hwnd, IDC_STATIC5, "5 running ....");
							g2 = g_libFilterText.CreateTextFilter2(0);
						}
						else
						{
							if(checks3relay)
								::ShowWindow(h111, 0);
							else
								::ShowWindow(h111, 1);

							checks3relay = !checks3relay;
						}
					}
				   break;
				case IDC_BUTTON5:
					{
						char pName1[MAX_PATH] = "gameserver.exe";
						HWND h111 = CheckWindowsRuningForName(pName1);
						if(!h111)
						{
							//DWORD g2 = g_libFilterText.CreateTextFilter2(6);
							char teamp[MAX_PATH];
							sprintf(teamp, "%s%s", folderServer, pName1);
							BOOL res1 = OnExecute (teamp);
							if(!res1)
							{
								MessageBox(NULL, pName1 , "error", MB_OK|MB_ICONERROR);
							}
							Sleep(1000);
							h111 = CheckWindowsRuningForName(pName1);
							::ShowWindow(h111, SW_MINIMIZE);
							::SetDlgItemText( hwnd, IDC_STATIC6, "6 running ....");
						}
						else
						{
							if(checkgameserver)
								::ShowWindow(h111, 0);
							else
								::ShowWindow(h111, 1);

							checkgameserver = !checkgameserver;
						}
					}
				   break;
		case IDCANCEL:

			::PostMessage( hwnd, WM_CLOSE, 0L, 0L );

			break;
			
		case IDC_BTN_SEND_MSG:
			{
				BOOL bToAllGS = FALSE;
				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_INDEX_SEND2GAMESERVER ) )
				{
					bToAllGS = TRUE;
				}
				
				UINT uOption = AP_NOTIFY_ALL_PLAYER;
				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_INDEX_GS2CLOSE ) )
				{
					uOption = AP_NOTIFY_GAMESERVER_SAFECLOSE;
				}

				if ( BST_CHECKED == ::IsDlgButtonChecked( hwnd, IDC_INDEX_WANINGGS2CLOSE ) )
				{
					uOption = AP_WARNING_ALL_PLAYER_QUIT;
				}

				uLength = ::GetDlgItemText( hwnd, IDC_EDIT_ANNOUNCE_MSG, szAnnounceText, 256 );
				szAnnounceText[uLength] = '\0';

				SendAnnounceText( hwnd, ( const char * )szAnnounceText, uLength, uOption, bToAllGS );
			}
			break;

		case IDC_BTN_GAMESVRINFO:
			{
				HWND hCtrl = ::GetDlgItem( hwnd, IDC_LIST_GAMESERVER );

				BOOL bRet = FALSE;

				if ( hCtrl && ::IsWindow( hCtrl ) )
				{
					int nSelIndex = LB_ERR;
					
					if ( LB_ERR != ( nSelIndex = ::SendMessage( hCtrl, LB_GETCURSEL, 0, 0 ) ) )
					{
						UINT nSearchID = 0;
				
						if ( LB_ERR != ( nSearchID = ::SendMessage( hCtrl, LB_GETITEMDATA, nSelIndex, 0 ) ) )
						{
							IGServer * pGServer = CGameServer::GetServer( nSearchID );
							
							if ( pGServer )
							{
								size_t nNum = pGServer->GetContent();
								
								bRet = ::SetDlgItemInt( hwnd, IDC_STATIC_GAMESERVER_CONTEXT, nNum, TRUE );
							}
						}
					}
				}

				if ( !bRet )
				{
					::SetDlgItemText( hwnd, IDC_STATIC_GAMESERVER_CONTEXT, "Invalid" );
				}
			}
			break;
		}

		break;
		
	case WM_SERVER_STATUS:

		switch ( wParam )
		{
		case ACCOUNTSERVER_NOTIFY:

			if ( CONNECTED == lParam )
			{
				::CheckDlgButton( hwnd, IDC_CHECK_ACCSVR, BST_CHECKED );
			}
			else // DICONNECTED
			{
				::CheckDlgButton( hwnd, IDC_CHECK_ACCSVR, BST_UNCHECKED );
			}

			break;

		case DBROLESERVER_NOTIFY:

			if ( CONNECTED == lParam )
			{
				::CheckDlgButton( hwnd, IDC_CHECK_ROLESVR, BST_CHECKED );
			}
			else // DICONNECTED
			{
				::CheckDlgButton( hwnd, IDC_CHECK_ROLESVR, BST_UNCHECKED );
			}
			
			break;
		}

		break;	

	case WM_GAMESERVER_EXCHANGE:
		{
			switch ( wParam )
			{
			case ADD_GAMESERVER_ACTION:
				{
					if ( m_pIntercessor )
					{
						const char *pInfo = m_pIntercessor->GetGameServerInfo( lParam );

						AddGameServerInfo( hwnd, lParam, pInfo );

						if ( 0 == nGameServerNumber ++ )
						{
							EnableGameSvrCtrl( hwnd, TRUE );

							HWND hCtrl = ::GetDlgItem( hwnd, IDC_LIST_GAMESERVER );
							
							if ( hCtrl && ::IsWindow( hCtrl ) )
							{
								::SendMessage( hCtrl, LB_SETCURSEL, 0, 0 );
							}
						}
					}
				}
				break;

			case DEL_GAMESERVER_ACTION:
				{
					if ( m_pIntercessor )
					{
						if ( -- nGameServerNumber <= 0 )
						{
							nGameServerNumber = 0;

							EnableGameSvrCtrl( hwnd, FALSE );

							::SetDlgItemText( hwnd, IDC_STATIC_GAMESERVER_CONTEXT, "Invalid" );
						}

						DelGameServerInfo( hwnd, lParam );
					}				
				}
				break;
			}
		}

		break;

	default:

		break;
		
	}
	
	/*
	 * Clean up any unused messages by calling DefWindowProc
	 */
	return FALSE;
	//return ::DefWindowProc( hwnd, msg, wParam, lParam );
}

void CBishopApp::AddGameServerInfo( HWND hDlg, UINT nID, const char *pInfo )
{
	if ( pInfo && pInfo[0] )
	{
		HWND hCtrl = ::GetDlgItem( hDlg, IDC_LIST_GAMESERVER );
		
		if ( hCtrl && ::IsWindow( hCtrl ) )
		{
			int nIndex = ::SendMessage( hCtrl, LB_ADDSTRING, 0, ( LPARAM )pInfo );

			if ( LB_ERR != nIndex )
			{
				::SendMessage( hCtrl, LB_SETITEMDATA, nIndex, nID );
			}
		}
	}
}

void CBishopApp::DelGameServerInfo( HWND hDlg, UINT nID )
{
	HWND hCtrl = ::GetDlgItem( hDlg, IDC_LIST_GAMESERVER );
	
	if ( hCtrl && ::IsWindow( hCtrl ) )
	{
		int nCount = ::SendMessage( hCtrl, LB_GETCOUNT, 0, 0 );

		if ( LB_ERR != nCount )
		{			
			for ( int i=0; i<nCount; i++ )
			{
				UINT nSearchID = 0;
				
				if ( nID == ( nSearchID = ::SendMessage( hCtrl, LB_GETITEMDATA, i, 0 ) ) )
				{
					::SendMessage( hCtrl, LB_DELETESTRING, i, 0 );

					return;
				}
			}
		}
	}
}

void CBishopApp::InitMainDlg( HWND hDlg )
{
	::SetDlgItemText( hDlg, IDC_EDIT_ACCSVRIP, m_theNetwork.GetAccSvrIP() );
	::SetDlgItemInt( hDlg, IDC_EDIT_ACCSVRPORT, m_theNetwork.GetAccSvrPort(), FALSE );

	::SetDlgItemText( hDlg, IDC_EDIT_ROLESVRIP, m_theNetwork.GetRoleSvrIP() );
	::SetDlgItemInt( hDlg, IDC_EDIT_ROLESVRPORT, m_theNetwork.GetRoleSvrPort(), FALSE );

	::SetDlgItemInt( hDlg, IDC_EDIT_CLIENT_PORT, m_theNetwork.GetClientOpenPort(), FALSE );

	::SetDlgItemInt( hDlg, IDC_EDIT_GAMESVR_PORT, m_theNetwork.GetGameSvrOpenPort(), FALSE );

	::CheckDlgButton( hDlg, IDC_INDEX_SEND2GAMESERVER, BST_CHECKED );
	::CheckDlgButton( hDlg, IDC_RADIO_ANNOUNCE_OPTION, BST_CHECKED );
	g_theSmartClient.m_hwndContain = hDlg;
}

void CBishopApp::CloseMainDlg( HWND hDlg )
{
	UpdateVariable( hDlg );

	g_theSmartClient.Close();
}

void CBishopApp::UpdateVariable( HWND hDlg )
{
	char szBuffer[MAX_PATH];

	::GetDlgItemText( hDlg, IDC_EDIT_ROLESVRIP, szBuffer, MAX_PATH );
	m_theNetwork.SetRoleSvrIP( szBuffer );

	UINT nValue = 0;
	BOOL bTranslated = TRUE;

	nValue = ::GetDlgItemInt( hDlg, IDC_EDIT_ROLESVRPORT, &bTranslated, FALSE );
	m_theNetwork.SetRoleSvrPort( nValue );

	nValue = ::GetDlgItemInt( hDlg, IDC_EDIT_CLIENT_PORT, &bTranslated, FALSE );
	m_theNetwork.SetClientOpenPort( nValue );

	nValue = ::GetDlgItemInt( hDlg, IDC_EDIT_GAMESVR_PORT, &bTranslated, FALSE );
	m_theNetwork.SetGameSvrOpenPort( nValue );

}

void CBishopApp::EnableGameSvrCtrl( HWND hwndDlg, BOOL nEnable )
{
	EnableCtrl( hwndDlg, IDC_BTN_GAMESVRINFO, nEnable );
	EnableCtrl( hwndDlg, IDC_LIST_GAMESERVER, nEnable );

	EnableCtrl( hwndDlg, IDC_RADIO_ANNOUNCE_OPTION, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_GS2CLOSE, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_WANINGGS2CLOSE, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_SEND2GAMESERVER, nEnable );
	EnableCtrl( hwndDlg, IDC_INDEX_SEND2GS_ISSEL, nEnable );
	EnableCtrl( hwndDlg, IDC_EDIT_ANNOUNCE_MSG, nEnable );
	EnableCtrl( hwndDlg, IDC_BTN_SEND_MSG, nEnable );
}

void CBishopApp::EnableCtrl( HWND hwndDlg, UINT nCtrlID, BOOL nEnable )
{
	HWND hCtrl = ::GetDlgItem( hwndDlg, nCtrlID );
	
	if ( hCtrl && ::IsWindow( hCtrl ) )
	{
		::EnableWindow( hCtrl, nEnable );
	}	
}

void CBishopApp::SendAnnounceText( HWND hwndDlg, const char *pText, UINT uLength, UINT uOption, BOOL bAllGS )
{
	if ( !pText || 0 == uLength )
	{
		::MessageBox( hwndDlg, "Please input a valid message!", "Warning", MB_OK | MB_ICONINFORMATION );
		
		::SetFocus( ::GetDlgItem( hwndDlg, IDC_EDIT_ANNOUNCE_MSG ) );
		
		return;
	}

	if ( bAllGS )
	{
		CGameServer::SendToAll( pText, uLength, uOption );
	}
	else
	{
		HWND hCtrl = ::GetDlgItem( hwndDlg, IDC_LIST_GAMESERVER );
		
		if ( hCtrl && ::IsWindow( hCtrl ) )
		{
			int nSelIndex = LB_ERR;
			
			if ( LB_ERR != ( nSelIndex = ::SendMessage( hCtrl, LB_GETCURSEL, 0, 0 ) ) )
			{
				UINT nSearchID = 0;
				
				if ( LB_ERR != ( nSearchID = ::SendMessage( hCtrl, LB_GETITEMDATA, nSelIndex, 0 ) ) )
				{
					IGServer * pGServer = CGameServer::GetServer( nSearchID );
					
					if ( pGServer )
					{
						pGServer->SendText( pText, uLength, uOption );
					}
				}
			}
		}
	}
}