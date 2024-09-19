// AutoupdateDlg.cpp : implementation file
//
/*
  作者：WPS万里，常齐 
  时间：2003年6 -7月
  版本：1.0
  功能：自动更新界面，通过对话框界面调用动态库UpdateDLL,并通过函数AutoUpdateDLL
  调用接口，动态库通过回调函数RefreshStatus,  来表明自己的状态，界面通过g_dlgInfo 
  来取得自动更新内核的状态。并表现在界面上

*/
#include "stdafx.h"
#include "Autoupdate.h"
#include "AutoupdateDlg.h"
#include "SiteDlg.h"
#include "StartDlg.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>

char url[MAX_PATH];
const string ConfileFileName   =  "config.ini";
int height = 500;

CAutoupdateDlg *dlg = NULL;
bool bStop = false;

#include "../UPDATEDLL/src/UpdateExport.h"

#include "../engine/Src/KWin32.h"
#include "../engine/Src/KIniFile.h"
#include "../engine/Src/KFilePath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BITMAP bmp;
BOOL bRetry = FALSE;
bool bRunning = false;
bool bReboot = false;
bool bEnterGame = false;

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
	
    if( !CreateProcess( NULL, buffer, NULL,	NULL, FALSE, 0,	NULL, NULL,	&si, &pi)) {
		bRet = FALSE;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	
	return bRet;
}


void DisplayErrorInfo(string& ErrorInfo) {
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	MessageBox( NULL, ErrorInfo.c_str(),(LPCTSTR)lpMsgBuf , MB_OK |MB_ICONWARNING );
	LocalFree( lpMsgBuf );
}

//回调函数，直接在这里显示状态
int __stdcall RefreshStatus(int nCurrentStatus, long lParam)
{
	if(bStop) return bStop;
	LPDOWNLOADFILESTATUS pFileinfo = NULL;
//根据由UpdateSet传经来的状态值nCurrentStatus 对状态信息Info给与相应的状态值
	switch(nCurrentStatus)
	{
	case defUPDATE_STATUS_INITIALIZING:
		dlg->m_DisplayVersionCtl.SetWindowText("Initializing...");
		dlg->m_progress.SetPos(20);
		break;
	case defUPDATE_STATUS_VERIFING:
		dlg->m_DisplayVersionCtl.SetWindowText("Verifing...");
		dlg->m_progress.SetPos(40);
		break;
	case defUPDATE_RESULT_VERSION_NOT_ENOUGH:
		{
			strcpy(url, (char *)lParam);
			AnnuncePanel NeedDownAnnuncePanel;
			strcpy(NeedDownAnnuncePanel.url, url);
			NeedDownAnnuncePanel.m_ResModule = dlg->m_ResModule;
			NeedDownAnnuncePanel.DoModal();
			bStop = true;
		}
//需要一次性手动升级包
		break;
	case defUPDATE_STATUS_PROCESSING_INDEX:
		dlg->m_DisplayVersionCtl.SetWindowText("Update index...");
		dlg->m_progress.SetPos(60);
		break;
	case defUPDATE_STATUS_DOWNLOADING:
		dlg->m_DisplayVersionCtl.SetWindowText("Downloading...");
		dlg->m_progress.SetPos(80);
		break;
	case defUPDATE_STATUS_DOWNLOADING_FILE:
		dlg->m_progress.SetPos(80);
		pFileinfo = (LPDOWNLOADFILESTATUS)lParam;
		if (pFileinfo) {
			CString strFile;
			strFile.Format("%s --- %d KB(%d KB)", pFileinfo->strFileName, pFileinfo->dwFileSize / 1024, pFileinfo->dwFileDownloadedSize / 1024);
			dlg->m_DisplayVersionCtl.SetWindowText(strFile);
			dlg->m_progressCurrent.SetPos((int)(pFileinfo->dwFileDownloadedSize * 100.0 / pFileinfo->dwFileSize));
		}
		break;
	case defUPDATE_STATUS_UPDATING:
		dlg->m_DisplayVersionCtl.SetWindowText("Update...");
		dlg->m_progress.SetPos(100);
		break;
	case defUPDATE_RESULT_UPDATE_SUCCESS:
		dlg->m_DisplayVersionCtl.SetWindowText("Update Success...");
		dlg->m_progress.SetPos(100);
		dlg->m_progressCurrent.SetPos(100);
		//dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
		//dlg->m_CancelButton.ShowWindow(SW_HIDE);
		//dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 120, height - 60, 90, 24);
		dlg->m_EnterGameButton.EnableButton(TRUE);
		dlg->m_EnterGameButton.EnableWindow(TRUE);
		break;
	case defUPDATE_RESULT_VERSION_MORE:
		dlg->m_DisplayVersionCtl.SetWindowText("Result version more");
		dlg->m_progress.SetPos(100);
		dlg->m_progressCurrent.SetPos(100);
		dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
		//dlg->m_CancelButton.ShowWindow(SW_HIDE);
		//dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 120, height - 60, 90, 24);
		dlg->m_EnterGameButton.EnableWindow(TRUE);
		dlg->m_EnterGameButton.EnableButton(TRUE);
		break;
	default:
		ASSERT(FALSE);
	}
	return bStop;
}

BOOL g_bLog = FALSE;

typedef int (__stdcall *UPDATEA_INIT)(KUPDATE_SETTING UpdateSetting);
typedef int (__stdcall *UPDATE_UNINIT)();
typedef int (__stdcall *UPDATE_START)();
typedef int (__stdcall *UPDATE_CANCEL)();

UPDATEA_INIT g_Update_Init = NULL;
UPDATE_UNINIT g_Update_UnInit = NULL;
UPDATE_START g_Update_Start = NULL;
UPDATE_START g_Update_Cancel = NULL;

HMODULE hModule;
UINT AutoUpdateDLL(char* pSite)
{
    KUPDATE_SETTING UpdateSet;
    int nRetCode = 0;
    CString sText;
	bRunning = true;

    UpdateSet.bAutoTryNextHost = false;
    UpdateSet.bUseVerify = false;
    UpdateSet.bUseFastestHost = false;
    UpdateSet.pfnCallBackProc = &RefreshStatus;
    UpdateSet.ProxySetting.bUpdateAuth = true; 
    UpdateSet.ProxySetting.nHostPort = 0;
    UpdateSet.ProxySetting.nProxyMethod = PROXY_METHOD_DIRECT;
    strcpy(UpdateSet.szVerifyInfo, "102400-010999-106075-054738");
    UpdateSet.ProxySetting.szHostAddr[0] = '\0';
    UpdateSet.ProxySetting.szPassword[0] = '\0';
    UpdateSet.ProxySetting.szUserName[0] = '\0';
	
    
	char szModulePath[MAX_PATH + 1];
	::GetModuleFileName(NULL, szModulePath, (MAX_PATH + 1));
	char *pszOffset = NULL;
	pszOffset = strrchr(szModulePath, '\\');
    ASSERT(pszOffset);
	strcpy(UpdateSet.szMainExecute, pszOffset + 1);
    pszOffset[1] = '\0';
    strcpy(UpdateSet.szDownloadPath, szModulePath);
	strcat(UpdateSet.szDownloadPath, "Update\\");
    strcpy(UpdateSet.szUpdatePath, szModulePath);
	
	UpdateSet.nUpdateMode = 0;

	
	strncpy(UpdateSet.szUpdateSite, pSite, MAX_PATH);
	
	CString strVersion = szModulePath;
	UpdateSet.nVersion = ::GetPrivateProfileInt(
		"Version",
		"Version",
		0,
		strVersion + "Version.cfg");
	
	CAutoupdateDlg* pDlg = (CAutoupdateDlg*)(AfxGetApp()->m_pMainWnd);
	UpdateSet.bLog = g_bLog;
	if (g_Update_Init == NULL || g_Update_UnInit == NULL || g_Update_Start == NULL) {
		goto Exit0;
	}

    nRetCode = g_Update_Init(UpdateSet);
	Sleep(10);
    if (nRetCode == defUPDATE_RESULT_INIT_FAILED)
        goto Exit0;
	
    nRetCode = g_Update_Start();
    g_Update_UnInit();

Exit0:
	UpdateSet.nVersion = ::GetPrivateProfileInt("Version", "Version", 0, strVersion + "Version.cfg");
	bRunning = false;
    return nRetCode;
}

UINT UpdateThread(LPVOID p)
{
	bStop = false;
	Sleep(10);

	dlg->m_DisplayVersionCtl.SetWindowText("Update...");

    int ret = AutoUpdateDLL((char*)p);

	if (ret >= 0 && (ret == defUPDATE_RESULT_INIT_FAILED || ret == defUPDATE_RESULT_DOWNLOAD_INDEX_FAILED ||
		ret == defUPDATE_RESULT_PROCESS_INDEX_FAILED || ret == defUPDATE_RESULT_CONNECT_SERVER_FAILED ||
		ret == defUPDATE_RESULT_DOWNLOAD_FAILED)) {
		dlg->m_nCurrentHost++;
		if(dlg->m_nCurrentHost >= dlg->m_strHosts.GetSize()) {
			dlg->m_DisplayVersionCtl.SetWindowText("Update error retry");
			dlg->m_progress.SetPos(00);
			dlg->m_progressCurrent.SetPos(0);
			//dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
			//dlg->m_EnterGameButton.EnableWindow(false);
			dlg->m_RetryButton.EnableWindow(TRUE);
			dlg->m_RetryButton.EnableButton(TRUE);
			dlg->m_EnterGameButton.EnableWindow(FALSE);
			dlg->m_EnterGameButton.EnableButton(FALSE);
		}
		else {
			dlg->UpdateFromCurrentHost();
		}
	}
	else if (ret == defUPDATE_RESULT_CANCEL) {
		//g_Update_Cancel();
		//dlg->EndDialog(IDCANCEL); ngung va thoat update
		//dlg->m_web.Stop();
//		dlg->EndDialog(IDCANCEL);
	}
	else if(ret == 100) {
		dlg->EndDialog(IDCANCEL);
	}
	else if(ret == 101) {
//		dlg->EndDialog(IDCANCEL);
	}
	else if (ret == defUPDATE_RESULT_UPDATESELF_SUCCESS) {
//在此写入文件，避免下次还要选择升级服务器
		FILE *site_ini = fopen(dlg->m_strSiteList, "w+");
		if(site_ini) {
			char line[100];
			sprintf(line, "%d\n", dlg->m_nCurrentHost);
			fputs(line, site_ini);
			for(int index = 0; index < dlg->m_strHosts.GetSize(); index++) {
				sprintf(line, "%s\n", dlg->m_strHosts[index]);
				fputs(line, site_ini);
			}
			fclose(site_ini);
		}
///*		g_Update_Cancel();
//		dlg->m_web.Stop();
		if (!dlg->m_strUpdateSelf.IsEmpty()) {
			dlg->m_strRun.Empty();
			bReboot = true;
		}
		dlg->EndDialog(IDCANCEL);
	}
	else {
		dlg->m_DisplayVersionCtl.SetWindowText("Update Completed");
		dlg->m_progress.SetPos(100);
		dlg->m_progressCurrent.SetPos(100);
		dlg->m_EnterGameButton.ShowWindow(SW_SHOW);
		//dlg->m_CancelButton.ShowWindow(SW_HIDE);
		//dlg->m_EnterGameButton.MoveWindow(bmp.bmWidth - 120, height - 60, 90, 24);
		dlg->m_EnterGameButton.EnableWindow(TRUE);
		dlg->m_EnterGameButton.EnableButton(TRUE);
		dlg->m_RetryButton.EnableWindow(TRUE);
		dlg->m_RetryButton.EnableButton(TRUE);
	}

	//读取版本号
	char aCfgPath[MAX_PATH];
	g_GetRootPath(aCfgPath);
	strcat(aCfgPath,"\\Version.cfg");
	int aMajorVersion = ::GetPrivateProfileInt("Version", "MajorVersion", 0, aCfgPath);
	int aVersion = ::GetPrivateProfileInt("Version", "Version", 0, aCfgPath);
	aMajorVersion -= 1;
	if (aMajorVersion < 0) aMajorVersion = 1;
	CString aFormCaption = "";
	char aIntStr[8];
	aFormCaption +="Auto Update (";
	itoa(aMajorVersion, aIntStr, 10);
	aFormCaption += aIntStr;
	if(aVersion <= 14)
		aVersion = 1;
	else
		aVersion -= 14;
	if(aVersion < 10)
		aFormCaption += ".1";
	else
		aFormCaption += ".";
	itoa(aVersion, aIntStr, 10);
	aFormCaption += aIntStr;
	aFormCaption += ")";
	::SetWindowText(dlg->m_hWnd, aFormCaption);

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateDlg dialog
const CString ResFileName = "AutoUpdateRes.dll";
CAutoupdateDlg::CAutoupdateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoupdateDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CAutoupdateDlg)
	m_strVersion = _T("");
	//m_strProgress = _T("");
	//m_strProgressCurrent = _T("");
	m_nCurrentHost = 0;//**内侧三
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

CAutoupdateDlg::~CAutoupdateDlg()
{
	if(m_ResModule)
	{
		if(!FreeLibrary(m_ResModule))
		{                           
//			DisplayErrorInfo(string("FreeLibrary()"));
		}
        m_ResModule = NULL;
	}
//在此运行程序
	if(bReboot) {
		OnExecute(m_strUpdateSelf);
	}
	else if(bEnterGame) {
		OnExecute(m_strRun);
	}
}

void CAutoupdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoupdateDlg)
	DDX_Control(pDX, IDC_PROGRESSCURRENT, m_progressCurrent);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_STATIC_VERSON, m_DisplayVersionCtl);
	DDX_Control(pDX, IDCANCEL, m_CancelButton); 
	DDX_Control(pDX, IDOK, m_EnterGameButton);
	DDX_Control(pDX, IDC_RETRY, m_RetryButton);
	DDX_Control(pDX, IDC_CONFIG, m_GameOption);
	DDX_Control(pDX, IDC_BUTTONREGACC, m_RegAccount);
	DDX_Control(pDX, IDC_BUTTONPAYC, m_PayCard);
	//DDX_Text(pDX, IDC_STATIC_VERSON, m_strVersion);
	//DDX_Text(pDX, IDC_STATIC_PROGRESS, m_strProgress);
	//DDX_Text(pDX, IDC_STATIC_PROGRESSCURRENT, m_strProgressCurrent);
	DDX_Control(pDX, IDC_WindowOption, m_WindowOptionCtl);
	DDX_Control(pDX, IDC_FullScreen, m_FullScreenCtl);
	DDX_Control(pDX, IDC_3DOption, m_3DOptionCtl);
	DDX_Control(pDX, IDC_2DOption, m_2DOptionCtl);
	//DDX_Radio(pDX, IDC_2DOption, m_2DOptionValue);
	//DDX_Radio(pDX, IDC_FullScreen, m_FullScreenValue);
	//DDX_Check(pDX, IDC_DynaLightEnable, m_DynaLightEnableValue);
	DDX_Control(pDX, IDC_EXPLORER1, m_web);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAutoupdateDlg, CDialog)
//{{AFX_MSG_MAP(CAutoupdateDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RETRY, OnRetry)
	ON_BN_CLICKED(IDC_BUTTONREGACC, OnRegAcc)
	ON_BN_CLICKED(IDC_BUTTONPAYC, OnPayC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoupdateDlg message handlers
const WindowWidth = 643;
const WindowHeight = 530;
// Color constants.
const COLORREF rgbRed   =  0x000000FF;
const COLORREF rgbGreen =  0x0000FF00;
const COLORREF rgbBlue  =  0x00FF0000;
const COLORREF rgbBlack =  0x484848;
const COLORREF rgbWhite =  0xFF000080;

BOOL CAutoupdateDlg::OnInitDialog()
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CDialog::OnInitDialog();
	if (!m_strWebURL.IsEmpty()) 
	{
		m_web.Navigate(m_strWebURL, NULL, NULL, NULL, NULL);
	}

	m_progress.SetRange(0, 100);
	m_progressCurrent.SetRange(0, 100);
	m_progress.SetBackColor(rgbBlack);
	m_progressCurrent.SetBackColor(rgbBlack);
	m_progress.SetForegroundBmp(IDB_BITMAP_P4);
	m_progressCurrent.SetForegroundBmp(IDB_BITMAP_P4);

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// load the standard image and alternate image
	m_EnterGameButton.LoadStdImage(IDR_PNGRUNGAME, _T("PNG"));
	m_EnterGameButton.EnableToggle(TRUE);

	// just to show highlight state for article
	m_CancelButton.LoadStdImage(IDR_PNGEXITU, _T("PNG"));

	// set as disabled
	m_GameOption.ShowWindow(SW_HIDE);
	//m_GameOption.LoadStdImage(IDR_PNGCONFIG, _T("PNG"));
	//m_GameOption.EnableButton(FALSE);

	m_RegAccount.LoadStdImage(IDR_PNGREGA, _T("PNG"));
	m_PayCard.LoadStdImage(IDR_PNGPAYC, _T("PNG"));
	m_RetryButton.LoadStdImage(IDR_PNG_RETRY, _T("PNG"));

	m_ResModule = LoadLibrary(ResFileName);
	if(!m_ResModule) {
		DisplayErrorInfo(string("LoadLibrary()"));
	}

	LoadBitMapFromInstance(m_bmTitle, m_ResModule, IDB_TITLE);
	if (!((HBITMAP)m_bmTitle)) {
	    PostQuitMessage(1);
	}
	m_bmTitle.GetBitmap(&bmp);	

	//m_button1.MoveWindow(537,  496, 88, 37);
	//m_button1.SetButtonBmp(IDB_TITLE,IDB_TITLE,IDB_TITLE, m_ResModule);
	
    //CRect WindowRange(0,0,bmp.bmWidth, height);
	CRect WindowRange(0,0, 1024, 660);			//set kich thuoc form autoupdate
	MoveWindow(WindowRange);
	CenterWindow(NULL);
	
	//读取版本号
	char aCfgPath[MAX_PATH];
	g_GetRootPath(aCfgPath);
	strcat(aCfgPath,"\\Version.cfg");
	int aMajorVersion = ::GetPrivateProfileInt("Version", "MajorVersion", 0, aCfgPath);
	int aVersion = ::GetPrivateProfileInt("Version", "Version", 0, aCfgPath);
	aMajorVersion -= 1;
	if (aMajorVersion < 0) aMajorVersion = 1;
	CString aFormCaption = "";
	char aIntStr[8];
	aFormCaption +="Auto Update (";
	itoa(aMajorVersion, aIntStr, 10);
	aFormCaption += aIntStr;
	if(aVersion <= 14)
		aVersion = 1;
	else
		aVersion -= 14;
	if(aVersion < 10)
		aFormCaption += ".1";
	else
		aFormCaption += ".";
	itoa(aVersion, aIntStr, 10);
	aFormCaption += aIntStr;
	aFormCaption += ")";
	::SetWindowText(m_hWnd, aFormCaption);

	m_EnterGameButton.EnableWindow(FALSE);
	m_EnterGameButton.MoveWindow(537,  476, 88, 37);
	m_EnterGameButton.EnableButton(FALSE);

	m_RetryButton.MoveWindow(627, 476, 88, 37);
	m_RetryButton.EnableWindow(FALSE);
	m_RetryButton.EnableButton(FALSE);

	m_RegAccount.MoveWindow(717, 476, 88, 37);
	m_PayCard.MoveWindow(807, 476, 88, 37);

	//m_GameOption.MoveWindow(807,  476, 88, 37);
	m_CancelButton.MoveWindow(897, 476, 88, 37);

    m_progressCurrent.MoveWindow(573, 425, 410, 13);
	m_progress.MoveWindow(573, 448, 410, 13);

	m_DisplayVersionCtl.MoveWindow(775, 570, 200, 16);
	//m_DisplayVersionCtl.SetWindowText("http://google.com.vn");
	m_web.MoveWindow(38,421, 467, 205);

	m_2DOptionCtl.MoveWindow(635, 525, 15, 20);
	m_3DOptionCtl.MoveWindow(720, 525, 15, 20);
	m_FullScreenCtl.MoveWindow(805, 525, 15, 20);
	m_WindowOptionCtl.MoveWindow(910, 525, 15, 20);

	char aConfigPath[MAX_PATH] ={0};
	g_GetRootPath(aConfigPath);
	strcat(aConfigPath, "\\");
	strcat(aConfigPath, ConfileFileName.c_str());
	//char aCapPath[MAX_PATH] = {0};
	//GetPrivateProfileString("Client","CapPath","",aCapPath, MAX_PATH, aConfigPath);
	//if (aCapPath[0] == '\0')
	//	g_GetRootPath(aCapPath);
	//m_txtCapPath = aCapPath;
	//UpdateData(false);
	int nRepresentValue = GetPrivateProfileInt("Client","Represent", 3, aConfigPath);
	if( nRepresentValue == 2 )
	{
		m_2DOptionCtl.SetCheck(1);
		m_3DOptionCtl.SetCheck(0);
		//m_DynaLightEnableCtl.SetCheck(0);
		//m_DynaLightEnableCtl.EnableWindow(false);
	}
	else
	{
		m_2DOptionCtl.SetCheck(0);
		m_3DOptionCtl.SetCheck(1);
		//m_DynaLightEnableCtl.EnableWindow();
		//int nDynamicLight = GetPrivateProfileInt("Client","DynamicLight", 1, aConfigPath);
		//if (nDynamicLight == 0)
		//	m_DynaLightEnableCtl.SetCheck(0);
		//else
		//	m_DynaLightEnableCtl.SetCheck(1);
	}
	int nFullScreenValue = GetPrivateProfileInt("Client","FullScreen", 1, aConfigPath);
	if (nFullScreenValue == 0)
	{
		m_FullScreenCtl.SetCheck(0);
		m_WindowOptionCtl.SetCheck(1);
	}
	else
	{
		m_FullScreenCtl.SetCheck(1);
		m_WindowOptionCtl.SetCheck(0);
	}

	//UpdateFinish.SetEvent();	
	
	UpdateFromCurrentHost(); //auto update
	
	//dlg->m_RetryButton.ShowWindow(SW_HIDE);
	
//	SetTimer(200, 50, NULL);
	dlg = this;
	//m_nCurrentHost = -1;

	m_bEnableRun = FALSE;
    
	hModule = LoadLibrary("UpdateDLL.dll");
    if (!hModule)
        return FALSE;
	
    g_Update_Init = (UPDATEA_INIT)GetProcAddress(hModule, "Update_Init");
    if (!g_Update_Init)
        return FALSE;
	
    g_Update_UnInit = (UPDATE_UNINIT)GetProcAddress(hModule, "Update_UnInit");
    if (!g_Update_UnInit)
        return FALSE;
	
    g_Update_Start = (UPDATE_START)GetProcAddress(hModule, "Update_Start");
    if (!g_Update_Start)
        return FALSE; 
	
	g_Update_Cancel = (UPDATE_START)GetProcAddress(hModule, "Update_Cancel");
    if (!g_Update_Cancel)
        return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////
	//
	// Put something interesting in the background
	// This has nothing to do with the GdipButtons
	//
	
	HBITMAP		hBitmap			= NULL;
	HINSTANCE	hInstResource	= NULL;

	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_InfoTitle), RT_BITMAP);

	// Load bitmap In
	m_hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_InfoTitle), IMAGE_BITMAP, 0, 0, 0);

	///////////////////////////////////////////////////////////////////////////////////////

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutoupdateDlg::OnSaveConfig() 
{
	// TODO: Add your control notification handler code here

	try
	{
		//写入config.ini文件
		char aConfigPath[MAX_PATH] ={0};
		g_GetRootPath(aConfigPath);
		strcat(aConfigPath, "\\");
		strcat(aConfigPath, ConfileFileName.c_str());
		//char aCapPath[MAX_PATH] = {0};
		//if (m_txtCapPath == "")
		//	g_GetRootPath(aCapPath);
		//else
		//	strcpy(aCapPath, m_txtCapPath.GetBuffer(1));
		//WritePrivateProfileString("Client","CapPath",aCapPath, aConfigPath);

		
		if (m_2DOptionCtl.GetCheck())
		{
			WritePrivateProfileString("Client","Represent","2", aConfigPath);
			WritePrivateProfileString("Client","DynamicLight","0", aConfigPath); 
		}
		else
		{
			WritePrivateProfileString("Client","Represent","3", aConfigPath);
			//if (m_DynaLightEnableCtl.GetCheck())
			//	WritePrivateProfileString("Client","DynamicLight","1", aConfigPath);
			//else
			//	WritePrivateProfileString("Client","DynamicLight","0", aConfigPath);
		}
		if (m_FullScreenCtl.GetCheck())
			WritePrivateProfileString("Client","FullScreen","1", aConfigPath);
		else
			WritePrivateProfileString("Client","FullScreen","0", aConfigPath);
	
	}
	catch(exception Error)
	{
		MessageBox(Error.what(),"",MB_ICONERROR);
	}
}

BOOL CAutoupdateDlg::InitAutoUpdate()
{
	KIniFile	IniFile;
	if (!IniFile.Load("\\settings\\autoupdate.ini"))
		return FALSE;
	
	char szWebURL[MAX_PATH];
	IniFile.GetString("main", "gameurl", "http://google.com.vn/", szWebURL, MAX_PATH);
	m_strWebURL = szWebURL;
	
	char run[256];
	IniFile.GetString("main", "game", "s3client.exe", run, 256);
	
	char szModulePath[MAX_PATH + 1];
	g_GetRootPath(szModulePath);
	CString strPath = szModulePath;
	
	m_strRun = strPath + "\\" + run;
	m_strUpdateSelf = strPath + "\\Update\\" + "UpdateSelf.DAT";
	m_strSiteList = strPath + "\\Update\\" + "SiteList.ini";
	m_nCurrentHost = -1;
	m_strHosts.RemoveAll();
	
	FILE *site_ini = fopen(m_strSiteList, "r");
	if(site_ini) {
		char line[100];
		while(fgets(line, 100, site_ini) != NULL) {
			if(m_nCurrentHost == -1) {
				m_nCurrentHost = atoi(line);
				if(m_nCurrentHost == -1) m_nCurrentHost = 0;
			}
			else {
				int last = strlen(line) - 1;
				if(line[last] == '\n') {
					line[last--] = 0;
				}
				if(line[last] == '\r') {
					line[last] = 0;
				}
				m_strHosts.Add(line);
			}
		}
		fclose(site_ini);
		remove(m_strSiteList);
		IniFile.GetInteger("main", "log", 0, &g_bLog);
		if(m_nCurrentHost >= m_strHosts.GetSize()) m_nCurrentHost = 0;
		return (m_strHosts.GetSize() >= 0);
	}

	char szSite[MAX_PATH];
	int n = 1;

	CString strSite;
	CStringArray strHosts;
	do {
		szSite[0] = 0;
		strSite.Format("ftpsite%d", n);
		IniFile.GetString("main", strSite, "", szSite, MAX_PATH);
		if (szSite[0] != 0)
			strHosts.Add(szSite);
		else
			break;
		n++;
	} while(szSite[0] != 0);
	
	if (strHosts.GetSize() > 0)
	{
//在此打开一个对话框选择升级服务器
		CSiteDlg sites;
		sites.site_list = &strHosts;
		int nStart = rand() * GetTickCount() % strHosts.GetSize();

		sites.nStart = nStart;
		
		//Hien thi box select site
		//if(sites.DoModal() != IDOK) {
		//	return FALSE;
		//}
		nStart = sites.nStart;

		int i;
		for (i = nStart; i < strHosts.GetSize(); i++)
		{
			m_strHosts.Add(strHosts[i]);
		}
		for (i = 0; i < nStart; i++)
		{
			m_strHosts.Add(strHosts[i]);
		}
		ASSERT(m_strHosts.GetSize() == strHosts.GetSize());
		m_nCurrentHost = 0;

		m_nConnectionStep = ConnectionProgress/(m_strHosts.GetSize());
	}
	else
		m_nCurrentHost = -1;
	
	IniFile.GetInteger("main", "log", 0, &g_bLog);
	return (m_nCurrentHost >= 0);
}

void CAutoupdateDlg::UpdateFromCurrentHost() {
	if (m_nCurrentHost >= 0 && m_nCurrentHost < m_strHosts.GetSize() )
	{
		AfxBeginThread(UpdateThread, (void*)(LPCTSTR)m_strHosts[m_nCurrentHost]);
	}
}

void CAutoupdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CAutoupdateDlg::OnPaint() 
{
	CPaintDC PaintDC(this); 
	if (IsIconic())
	{
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		PaintDC.DrawIcon(x, y, m_hIcon);
	}
	else {
		//画标题
        CDC dc;
        CBitmap *pOldBitmap = NULL;
		dc.CreateCompatibleDC(&PaintDC);
        
        pOldBitmap = dc.SelectObject(&m_bmTitle);

		PaintDC.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, SRCCOPY);

        dc.SelectObject(pOldBitmap);
       
	}
}


HCURSOR CAutoupdateDlg::OnQueryDragIcon() {
	return (HCURSOR) m_hIcon;
}

void CAutoupdateDlg::OnCancel() {
	m_web.Stop();
	CWnd* p = GetDlgItem(IDCANCEL);
	CString str;
	p->GetWindowText(str);
	m_DisplayVersionCtl.SetWindowText("OnCancel...");
	m_progress.SetPos(0);
	m_progressCurrent.SetPos(0);
	bStop = true;
	if (g_Update_Cancel) {
		g_Update_Cancel();
	}
	EndDialog(IDCANCEL);
	//m_CancelButton.ShowWindow(SW_SHOW);
	//m_ExitButton.ShowWindow(SW_SHOW);
}

void CAutoupdateDlg::OnDestroy()
{
//	WaitForSingleObject(UpdateFinish.m_hObject, INFINITE);
	while(bRunning) {
		Sleep(100);
	}
	if (hModule) {
      FreeLibrary(hModule);
	  hModule = NULL;
	}

	OnSaveConfig();
	CDialog::OnDestroy();

}

void CAutoupdateDlg::OnOK()
{
	if (!m_strRun.IsEmpty())
	{
		bEnterGame = true;
	}
	OnSaveConfig();
	EndDialog(IDOK);
}
 
BEGIN_EVENTSINK_MAP(CAutoupdateDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CAutoupdateDlg)
	ON_EVENT(CAutoupdateDlg, IDC_EXPLORER1, 271 /* NavigateError */, OnNavigateErrorExplorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

//	ShellExecute(NULL,"open","\\\\s\\自动更新\\剑侠情缘3网上交易平台.htm",NULL,NULL,SW_SHOW);
//	ShellExecute(NULL,"open","\\\\s\\自动更新\\剑侠情缘3网上交易平台.htm",NULL,NULL,SW_SHOW);

void CAutoupdateDlg::LoadBitMapFromInstance(CBitmap &Dest, HINSTANCE Hinstance, WORD wResourceID) {
    HBITMAP TempBitMap = LoadBitmap(Hinstance,MAKEINTRESOURCE(wResourceID));
	if(!TempBitMap)	{
		DisplayErrorInfo(string("LoadBitmap()"));
	}
		
	if(!Dest.Attach(TempBitMap)) {
		DisplayErrorInfo(string("FromHandle()"));
	}
}


HBRUSH CAutoupdateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(nCtlColor == CTLCOLOR_STATIC   )
	{
		//pDC->SetTextColor(RGB(229,223,0)); 
		pDC->SetTextColor(RGB(102,101,98)); //set mau trong suot cho lable
		// Set the background mode for text to transparent 
		// so background will show thru.
		pDC->SetBkMode(TRANSPARENT);
		//pDC->SetBkColor(RGB(24,4,0));
		//hbr = (HBRUSH) GetStockObject(NULL_BRUSH);
		//hbr = (HBRUSH)(m_DisplayVersionCtl.GetSafeHandle());
	}
	hbr = (HBRUSH)CreateSolidBrush(RGB(31, 5, 0));
	return hbr;
}

void CAutoupdateDlg::OnConfig() 
{
	OnSaveConfig();
	//GameOptionPanel OptionInterface;
	//OptionInterface.DoModal();
}

const char *HOME_PAGE = "http://google.com.vn";
void CAutoupdateDlg::OnNavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Frame, VARIANT FAR* StatusCode, BOOL FAR* Cancel) 
{
	if(!bRetry) {
		bRetry = TRUE;
		m_web.Navigate(HOME_PAGE, NULL, NULL, NULL, NULL);
	}
}

void CAutoupdateDlg::OnRegAcc() 
{
	ShellExecute(NULL,"open","http://cucthongkedaknong.gov.vn/index.htm",NULL,NULL,SW_SHOW);
}

void CAutoupdateDlg::OnPayC() 
{
	ShellExecute(NULL,"open","http://cucthongkedaknong.gov.vn/index.htm",NULL,NULL,SW_SHOW);
}

void CAutoupdateDlg::OnRetry() 
{
	//重试连接
	dlg->m_RetryButton.EnableWindow(FALSE);
	dlg->m_RetryButton.EnableButton(FALSE);
	//m_RetryButton.ShowWindow(SW_HIDE);
	//m_CancelButton.ShowWindow(SW_SHOW);

	m_nCurrentHost = 0;
	UpdateFromCurrentHost();

	dlg = this;
	//m_nCurrentHost = -1;

	m_bEnableRun = FALSE;

}
