//=====================================
// File name    : UiCapture.cpp
// Develop      : Dzung Dolby
//=====================================

#include "KWin32.h"
#include "KIniFile.h"
#include "KWin32Wnd.h"
#include "UiCapture.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiLoginBg.h"
#include "UiSysMsgCentre.h"

#define SCHEME_INI_CAPTURE "UiCapture.ini"
static char m_zRecPath[MAX_PATH] = ".\\Jxrelay";
static char m_zPathName[MAX_PATH] = "";

KUiCapture *KUiCapture::m_pSelf = NULL;

KUiCapture::KUiCapture()
{
    m_szLoginBg[0] = 0;
	doPause = FALSE;
}

KUiCapture::~KUiCapture()
{}

KUiCapture* KUiCapture::OpenWindow()
{
    if (m_pSelf == NULL)
    {
        m_pSelf = new KUiCapture;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
    {
    //    KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
        m_pSelf->Show();
    }
    return m_pSelf;
}

void KUiCapture::CloseWindow()
{
    if (m_pSelf)
    {
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

void KUiCapture::SetRecPath(const char *path)
{
	if(path[0] && strlen(path) < 128)
		memcpy(m_zRecPath, path, strlen(path));
	else
		strcpy(m_zRecPath, ".\\Jxrelay");

	 if(!CreateDirectory(path ,NULL)) // tao folder save video neu chua co
    {
        return;
    }
}

void KUiCapture::SetRecTimmer(const char *timmer)
{
	m_pSelf->m_TimRecTxt.SetText(timmer);
}

KUiCapture* KUiCapture::GetUiCapture()
{
    if (m_pSelf == NULL)
    {
        m_pSelf = new KUiCapture;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
    {
    //    KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
        //m_pSelf->Show();
    }
    return m_pSelf;
}



void KUiCapture::Initialize()
{
    AddChild(&m_RecButton);
    AddChild(&m_PauseButton);
    //AddChild(&m_StopButton);
	//AddChild(&m_CloseButton);
	AddChild(&m_TimRecTxt);

    char Scheme[128];
    g_UiBase.GetCurSchemePath(Scheme, 128);
    m_pSelf->LoadScheme(Scheme);
    Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiCapture::LoadScheme(const char *pScheme)
{
    char Buff[128];
    KIniFile    Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_CAPTURE);
    if (Ini.Load(Buff))
    {
        KWndShowAnimate::Init(&Ini, "Main");
        m_RecButton.        Init(&Ini, "Rec_btn");
        m_PauseButton.      Init(&Ini, "Pause_btn");
        //m_StopButton.       Init(&Ini, "Stop_btn");
		//m_CloseButton.      Init(&Ini, "Close_btn");
		m_TimRecTxt.       Init(&Ini, "TimeRec_txt");
		m_TimRecTxt.SetText("[ 00:00:000 ]");
    }

}

void KUiCapture::StartCapture()
{
	//https://issfilestore.beesocialagency.com/record-the-game-with-bandicap-sdk-by-onouro/
	
	if(m_bandiCaptureLibrary.IsCapturing()==TRUE)
	{
		m_bandiCaptureLibrary.Stop();
		m_bandiCaptureLibrary.Destroy();
		CloseWindow();

		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.byParamSize = 0;
		Msg.byPriority = 0;
		Msg.eType = SMT_NORMAL;
		Msg.uReservedForUi = 0;
		sprintf(Msg.szMessage, "Video ®· l­u %s", m_zPathName);
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);

	}
	else
	{
		if(m_bandiCaptureLibrary.IsCreated()==FALSE)
		{
		  // If failed, check mismatching the version of BCL DLL and BandiCap.h . 
		  if(FAILED(m_bandiCaptureLibrary.Create(BANDICAP_RELEASE_DLL_FILE_NAME)))
			ASSERT(0);

		  // If don't call this function, BCL shows an watermark in upper corner of captured video.
		  // To remove logo, you may fill this function with provided license information. 
		  //if(FAILED(m_bandiCaptureLibrary.Verify("BCL-SDK-TRIAL", "f5b0b287")))
		  if(FAILED(m_bandiCaptureLibrary.Verify("MAIET-GUNZ-20080916", "d25f910a")))
			ASSERT(0);
		 }

		if(m_bandiCaptureLibrary.IsCreated())
		{
			  BCAP_CONFIG cfg;

			  // Use presets to easy configuration. As you want, you can set this configure directly.
			  BCapConfigPreset(&cfg, BCAP_PRESET_DEFAULT); 
			  m_bandiCaptureLibrary.CheckConfig(&cfg);       // Correct wrong settings
			  m_bandiCaptureLibrary.SetConfig(&cfg);         // Apply presets

			  m_bandiCaptureLibrary.SetMinMaxFPS(30, 60);    // Set minimal, maximal frame rate

			  // Create filename by using current time. 
			  TCHAR pathName[MAX_PATH];
			  m_bandiCaptureLibrary.MakePathnameByDate(_T(m_zRecPath), _T("Capture"), _T("mp4"), pathName, MAX_PATH);

			HWND hWnd = g_GetMainHWnd();
			HRESULT hr = m_bandiCaptureLibrary.Start(pathName, NULL, BCAP_MODE_GDI , (LONG_PTR)hWnd);
			strcpy(m_zPathName, pathName);
			//HRESULT hr = m_bandiCaptureLibrary.Start(pathName, NULL, BCAP_MODE_GDI, NULL);
			if(FAILED(hr))
			{
				ASSERT(0);
				m_bandiCaptureLibrary.Stop();
			}
		}
	}
}

KUiCapture* KUiCapture::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

BOOL KUiCapture::CheckWinXP()
{
    DWORD version = GetVersion();
    DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
    DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));
    return ((major == 5) && (minor == 1)); // 5.1 is WIN Xp  5.2 is XP x64
}

int KUiCapture::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    int nRet = 0;
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_RecButton)
			{
				//if(CheckWinXP()==FALSE)
				{
					StartCapture();
				}
				/*else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.byParamSize = 0;
					Msg.byPriority = 0;
					Msg.eType = SMT_NORMAL;
					Msg.uReservedForUi = 0;
					sprintf(Msg.szMessage, "TÝnh n¨ng nµy kh«ng hç trî cho Windows XP");
					KUiSysMsgCentre::AMessageArrival(&Msg, NULL);			
				}*/
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_PauseButton)
			{
				if(m_bandiCaptureLibrary.IsCreated())
				{
					m_bandiCaptureLibrary.Pause(!doPause);
					doPause = !doPause;
				}
			}
		}
        break;
    
    default:
        break;
    }
    return nRet;
}
