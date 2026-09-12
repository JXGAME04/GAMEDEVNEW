/*****************************************************************************************
//	界面--login窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiInformation.h"
#include "UiLogin.h"
#include "UiSelServer.h"
#include "UiLoginBg.h"
#include "../../JxReplay.h"
#include "UiConnectInfo.h"
#include "../../Login/Login.h"
#include "../UiSoundSetting.h"
#include <crtdbg.h>
#include "../../ENGINE/Src/KSG_MD5_String.h"

#define	SCHEME_INI_LOGIN		"UiLogin.ini"
#include "../../core/src/coreshell.h"
extern iCoreShell*		g_pCoreShell;
KUiLogin* KUiLogin::m_pSelf = NULL;
bool      KUiLogin::m_bHiddenForReplay = false;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiLogin::KUiLogin()
{
	m_szLoginBg[0] = 0;
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KUiLogin::~KUiLogin()
{
}

KUiLogin* KUiLogin::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiLogin::AutoLgNextStep(const char* pszAccount, const char* pszPassword)
{
	KSG_PASSWORD Password;
	strcpy(Password.szPassword, pszPassword);
	for (int i = 0; i < (int)strlen(Password.szPassword); ++i)
	{
		if(Password.szPassword[i] != -1)
		Password.szPassword[i] = ~Password.szPassword[i];
	}
	g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SETACC, (KNPARAM)pszAccount);
	g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SETPASS, (KNPARAM)&Password.szPassword);
	g_LoginLogic.AccountLogin(pszAccount, Password);
	KUiConnectInfo::OpenWindow(CI_MI_CONNECTING, LL_S_ROLE_LIST_READY);
	CloseWindow(false);
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//  He replay .jxr: an man login trong luc phat, khoi phuc khi phat xong.
//  Giong ban tham chieu (EnterReplayMode 0x004760E0): dat co roi an UI login
//  de ban dien khong bi nen/form login ve de len.
//--------------------------------------------------------------------------
void KUiLogin::EnterReplayHide()
{
	m_bHiddenForReplay = true;
	KUiLoginBackGround::CloseWindow(false);	// an nen login (Hide)
	CloseWindow(false);						// an form login (Hide)
}

void KUiLogin::RestoreAfterReplay()
{
	if (!m_bHiddenForReplay)
		return;
	m_bHiddenForReplay = false;
	KUiLoginBackGround::Restore();	// hien lai nen login
	OpenWindow();					// hien lai form login (m_pSelf da ton tai -> Show)
}

KUiLogin* KUiLogin::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiLogin;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
        m_pSelf->Show();
	}	
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiLogin::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		bool bGhiNho = (m_pSelf->m_RememberAccount.IsButtonChecked() != 0);
		g_LoginLogic.SetRememberAccountFlag(bGhiNho);
#ifdef JX_MOBILE
		// [ANDROID 09/09 LOGIN] Tren dien thoai o "Ghi nho" = nho CA mat ma, nhu moi game mobile: bat go lai
		// mat ma bang ban phim mem moi lan vao game la cuc hinh. Cai luu xuong dia KHONG phai mat ma chu thuong
		// ma la ban bam MD5 (KUiLogin::OnLogin da bam truoc khi goi AccountLogin), lai duoc EDOneTimePad_Encipher.
		// Ban PC khong doi: o do "Ghi nho" van chi nho tai khoan (nho ca mat ma phai Alt+A + AutoLogin=6323).
		if (bGhiNho)
			g_LoginLogic.SetRememberAllFlag(true);
#endif
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->m_PassWord.ClearText(true);
			m_pSelf->Hide();
		}
	}
}

void KUiLogin::Show()
{
	KWndShowAnimate::Show();
	KUiLoginBackGround::SetConfig(m_szLoginBg);

	char	szAccount[32];
	m_RememberAccount.CheckButton(g_LoginLogic.GetLoginAccount(szAccount));
#ifdef JX_MOBILE
	// [DANGNHAP 14/09 b] Lan dang nhap DAU TIEN (may moi cai, hoac vua xoa du lieu app) chua nho gi nen o "Ghi nho" tat:
	// go tai khoan roi bam Dang nhap la KHONG luu gi ca, lan sau phai go lai. Tren dien thoai mac dinh TICK SAN;
	// ai khong muon nho thi bo tick truoc khi bam Dang nhap. Ban PC giu nguyen nhu cu.
	m_RememberAccount.CheckButton(true);
#endif
	m_Account.SetText(szAccount);
#ifdef JX_MOBILE
	// [DANGNHAP 14/09] nho ca mat ma nhung KHONG tu dang nhap (config [Login] TuDongDangNhap=0): o mat ma hien 8 dau * (kieu
	// Password chi thay cham); bam Dang nhap = dung ban bam da nho (OnLogin), go de len = mat ma moi.
	m_bDungMatMaDaLuu = false;
	if (szAccount[0] && g_LoginLogic.GetLoginPasswordSaved(NULL))
	{
		m_PassWord.SetText("********");
		m_bDungMatMaDaLuu = true;
	}
#endif
	if (szAccount[0])
		Wnd_SetFocusWnd(&m_PassWord);
	else
		Wnd_SetFocusWnd(&m_Account);
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiLogin::Initialize()
{
	AddChild(&m_Account);
	AddChild(&m_PassWord);
	AddChild(&m_Login);
	AddChild(&m_Cancel);
	AddChild(&m_RememberAccount);
	AddChild(&m_OpenRep);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}
extern int SCREEN_WIDTH;
//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiLogin::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_LOGIN);
	if (Ini.Load(Buff))
	{
		if (SCREEN_WIDTH == 1024) {
			KWndShowAnimate::Init(&Ini, "Main1024");
			Ini.GetString("Main1024", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
		}
		else {
			KWndShowAnimate::Init(&Ini, "Main");
			Ini.GetString("Main", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
		}
		m_Account .Init(&Ini, "Account");
		m_PassWord.Init(&Ini, "Password");
		m_Login   .Init(&Ini, "Login");
		m_Cancel  .Init(&Ini, "Cancel");
		m_RememberAccount.Init(&Ini, "Remember");
		m_OpenRep.Init(&Ini, "OpenRep");
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiLogin::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (KUPARAM)(KWndWindow*)&m_Login)
			OnLogin();
		else if (uParam == (KUPARAM)(KWndWindow*)&m_Cancel)
			OnCancel();
		else if (uParam == (KUPARAM)(KWndWindow*)&m_OpenRep)
		{
			// Xem lai ban dien .jxr: chon tep roi phat. Play thanh cong thi an man login.
			if (JxReplay_OpenFileAndPlay())
				EnterReplayHide();
		}
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_TAB)
		{
            if ((GetKeyState(VK_SHIFT) & 0x8000) == 0)
            {
                // 如果没有按住Shift
			    if (uParam == (KUPARAM)(KWndWindow*)&m_Account)
			    {
				    int x, y, cx;
				    m_PassWord.GetAbsolutePos(&x, &y);
				    m_PassWord.GetSize(&cx, NULL);
				    Wnd_SetCursorPos(x + cx, y);
				    Wnd_SetFocusWnd(&m_PassWord);
				    nRet = 1;
			    }
			    else if (uParam == (KUPARAM)(KWndWindow*)&m_PassWord)
			    {
				    Wnd_SetFocusWnd(NULL);
				    m_Login.SetCursorAbove();
				    nRet = 1;
			    }
            }
            else
            {   
                // 如果按住Shift
			    if (uParam == (KUPARAM)(KWndWindow*)&m_Account)
			    {
 				    Wnd_SetFocusWnd(NULL);
      			    m_Cancel.SetCursorAbove();
				    nRet = 1;
			    }
			    else if (uParam == (KUPARAM)(KWndWindow*)&m_PassWord)
			    {
			        int x, y, cx;
			        m_Account.GetAbsolutePos(&x, &y);
			        m_Account.GetSize(&cx, NULL);
			        Wnd_SetCursorPos(x + cx, y);
			        Wnd_SetFocusWnd(&m_Account);

				    nRet = 1;
			    }
            }
		}
		else if (nParam == VK_RETURN)
		{
			if (uParam == (KUPARAM)(KWndWindow*)&m_Account)
			{
				Wnd_SetFocusWnd(&m_PassWord);
				nRet = 1;
			}
			else if (uParam == (KUPARAM)(KWndWindow*)&m_PassWord)
			{
				OnLogin();
				nRet = 1;
			}
		}
		else if (nParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	case WM_KEYDOWN:
		OnKeyDown(uParam);
		nRet = 1;
		break;
	case WM_SYSKEYDOWN:
		if (uParam == 'A')
			OnEnableAutoLogin();
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiLogin::OnEnableAutoLogin()
{
	KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
	int nAutoLogin = false;
	if (pSetting)
	{
		pSetting->GetInteger("Main", "AutoLogin", 0, &nAutoLogin);
		g_UiBase.CloseCommConfigFile();
	}
	if (nAutoLogin == 6323)
	{
		g_LoginLogic.SetRememberAllFlag(true);
		m_RememberAccount.CheckButton(true);
	}
}

void KUiLogin::OnKeyDown(unsigned int uKey)
{
	if (uKey == VK_RETURN)
	{
		if (m_Cancel.IsButtonActive())
			OnCancel();
		else
			OnLogin();
	}
	else if (uKey == VK_TAB)
	{
        if ((GetKeyState(VK_SHIFT) & 0x8000) == 0)
        {
            // 如果没有按住Shift
    		if (m_Login.IsButtonActive())
			    m_RememberAccount.SetCursorAbove();
		    else if (m_RememberAccount.IsButtonActive())
			    m_Cancel.SetCursorAbove();
		    else
		    {
			    int x, y, cx;
			    m_Account.GetAbsolutePos(&x, &y);
			    m_Account.GetSize(&cx, NULL);
			    Wnd_SetCursorPos(x + cx, y);
			    Wnd_SetFocusWnd(&m_Account);
		    }
        }
        else
        {   
            // 如果按住Shift
    		if (m_Login.IsButtonActive())
            {
				int x, y, cx;
				m_PassWord.GetAbsolutePos(&x, &y);
				m_PassWord.GetSize(&cx, NULL);
				Wnd_SetCursorPos(x + cx, y);
				Wnd_SetFocusWnd(&m_PassWord);
            }
		    else if (m_RememberAccount.IsButtonActive())
            {
	    	    m_Login.SetCursorAbove();
		    }
            else   // if m_Cancel.IsButtonActive() 
		    {
                m_RememberAccount.SetCursorAbove();
		    }
            
        }
	}
	else if (uKey == VK_SPACE)
	{
		if (m_Login.IsButtonActive())
			OnLogin();
		else if (m_Cancel.IsButtonActive())
			OnCancel();
		else if (m_RememberAccount.IsButtonActive())
			m_RememberAccount.CheckButton(!m_RememberAccount.IsButtonChecked());
	}
	else if (uKey == VK_LEFT)
		m_Login.SetCursorAbove();
	else if (uKey == VK_RIGHT)
		m_Cancel.SetCursorAbove();
	else if (uKey == VK_ESCAPE)
		OnCancel();
}

//--------------------------------------------------------------------------
//	功能：登录
//--------------------------------------------------------------------------
void KUiLogin::OnLogin()
{
	char szAccount[32];
    char	     szPassword[KSG_PASSWORD_MAX_SIZE];
	KSG_PASSWORD Password;
	if (GetInputInfo(szAccount, szPassword))
	{
#ifdef JX_MOBILE
		// [DANGNHAP 14/09] o mat ma van la 8 dau * do Show() dien -> dung ban bam MD5 da nho, khong bam lai chuoi "********";
		// nguoi choi go de len thi di duong cu (bam MD5 chuoi vua go).
		if (m_bDungMatMaDaLuu && strcmp(szPassword, "********") == 0 && g_LoginLogic.GetLoginPasswordSaved(&Password))
			;
		else
		{
        #ifdef SWORDONLINE_USE_MD5_PASSWORD
        KSG_StringToMD5String(Password.szPassword, szPassword);
        #else
        strncpy(Password.szPassword, szPassword, sizeof(Password.szPassword));
        Password.szPassword[sizeof(Password.szPassword) - 1] = '\0';
        #endif
		}
#else
        #ifdef SWORDONLINE_USE_MD5_PASSWORD

        KSG_StringToMD5String(Password.szPassword, szPassword);

        #else

        #pragma message (KSG_ATTENTION("Add Password to MD5 string"))
        strncpy(Password.szPassword, szPassword, sizeof(Password.szPassword));
        Password.szPassword[sizeof(Password.szPassword) - 1] = '\0';

        #endif
#endif
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SETACC, (KNPARAM)&szAccount);
		g_pCoreShell->OperationRequest(GOI_AUTOPLAY_ACTION, ATYPE_SETPASS, (KNPARAM)&Password.szPassword);
		g_LoginLogic.AccountLogin(szAccount, Password);
		KUiConnectInfo::OpenWindow(CI_MI_CONNECTING, LL_S_ROLE_LIST_READY);
		memset(&szPassword, 0, sizeof(szPassword));	
		memset(&Password, 0, sizeof(Password));	
		CloseWindow(false);
	}
}

//--------------------------------------------------------------------------
//	功能：取消
//--------------------------------------------------------------------------
void KUiLogin::OnCancel()
{
	CloseWindow(false);
	g_LoginLogic.ReturnToIdle();
	KUiSelServer::OpenWindow();	//点击『取消』按钮返回『初始界面』；
}

//--------------------------------------------------------------------------
//	功能：获取输入的账号密码信息
//--------------------------------------------------------------------------
int KUiLogin::GetInputInfo(char* pszAccount, char* pszPassword)
{
	_ASSERT(pszAccount && pszPassword);
	if (m_Account.GetText(pszAccount, 32, false) &&
		m_PassWord.GetText(pszPassword, 32, false))
	{
#ifdef JX_MOBILE
		// [DANGNHAP 12/09] quy che game: chu dau tai khoan phai viet thuong; ban phim dien thoai hay tu viet hoa -> ha xuong
		if (pszAccount[0] >= 'A' && pszAccount[0] <= 'Z')
		{
			pszAccount[0] = (char)(pszAccount[0] - 'A' + 'a');
			m_Account.SetText(pszAccount);
		}
#endif
		return true;
	}
	CloseWindow(false);
	KUiConnectInfo::OpenWindow(CI_MI_ERROR_LOGIN_INPUT, CI_NS_LOGIN_WND);
	return false;
/*	int nLen = m_Account.GetText(m_szAccount, 32, false);
	unsigned char	cCode;
	if (nLen >= LOGIN_ACCOUNT_MIN_LEN && nLen <= LOGIN_ACCOUNT_MAX_LEN)
	{
		for (int i = 0; i < nLen;)
		{
			cCode = (unsigned char)m_szAccount[i];
			if (i + 2 <= nLen && cCode >= 0x81 && cCode <= 0xfe &&
				(cCode <= 0xa0 || cCode >= 0xaa))
			{
				i += 2;
			}
			else if ((cCode >= 0x30 && cCode <= 0x39) ||
				(cCode >= 0x41 && cCode <= 0x5a) ||
				(cCode >= 0x61 && cCode <= 0x7a))
			{
				i++;
			}
			else
				break;
		}

		if (i == nLen)
		{
			nLen = m_PassWord.GetText(m_szPassword, 32, false);
			if (IsValidPwd(m_szPassword, nLen))
				return true;
		}
	}

	KUiConnectInfo::OpenWindow(CI_MI_ACCOUNT_PWD_ERROR, CI_AE_BACK_TO_LOGIN);
	CloseWindow(false);
	return false;*/
}

bool KUiLogin::IsValidPassword(const char* pszPassword, int nLen)
{
	return true;	//to be del this line;

	if (pszPassword)
	{
		if (nLen < 0)
			nLen = strlen(pszPassword);

		if (nLen >= LOGIN_PASSWORD_MIN_LEN && nLen <= LOGIN_PASSWORD_MAX_LEN)
		{
			for (int i = 0; i < nLen; i++)
			{
				unsigned char cCode = (unsigned char)pszPassword[i];
				if ((cCode < 0x30 || cCode > 0x39) &&
					(cCode < 0x41 || cCode > 0x5a) &&
					(cCode < 0x61 || cCode > 0x7a))
				{
					break;
				}
				if (i == nLen)
					return true;
			}
		}
	}
	return false;
}