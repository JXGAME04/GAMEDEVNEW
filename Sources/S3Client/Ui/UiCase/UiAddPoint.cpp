#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiAddPoint.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;
#define SCHEME_INI_ITEM	"UiAddPoint.ini"
KUiAddPoint* KUiAddPoint::m_pSelf = NULL;

//--------------------------------------------------------------------------
//  UiAddPoint.cpp
//	Kiem tra xem hop thoai co dang visible hay khong, neu visible thi tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiAddPoint* KUiAddPoint::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Mo hop thoai, tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiAddPoint* KUiAddPoint::OpenWindow(UI_PLAYER_ATTRIBUTE type)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiAddPoint;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
		m_pSelf->type = type;
		m_pSelf->m_Title.SetText("T¨ng sè ®iÓm");
	}
	return m_pSelf;
}

void KUiAddPoint::Show()
{
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_Password);
	Wnd_SetExclusive((KWndWindow*)this);
}

//--------------------------------------------------------------------------
// Dong hop thoai 
//--------------------------------------------------------------------------
void KUiAddPoint::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// Khoi tao hop thoai
// -------------------------------------------------------------------------
void KUiAddPoint::Initialize()
{
	AddChild(&m_Password);
	AddChild(&m_OKBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_Title);
	
	char schemePath[256];
	g_UiBase.GetCurSchemePath(schemePath, 256);
	LoadScheme(schemePath);

	Wnd_AddWindow(this);
}

// -------------------------------------------------------------------------
// Tao layout hop thoai
// -------------------------------------------------------------------------
void KUiAddPoint::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_Password.Init(&Ini, "PWfield");
		m_pSelf->m_OKBtn.Init(&Ini, "OKButton");
		m_pSelf->m_CancelBtn.Init(&Ini, "CloseButton");
		m_pSelf->m_Title.Init(&Ini, "Title");
	}
}

// -------------------------------------------------------------------------------
// Gui goi tin yeu cau mo khoa ruong len server
// -------------------------------------------------------------------------------
void KUiAddPoint::OnOK()
	{
		OnCheckInput();
		int inputedPassword = m_Password.GetIntNumber();
		g_pCoreShell->OperationRequest(GOI_TONE_UP_ATTRIBUTE, m_pSelf->type, inputedPassword);
		CloseWindow();
		return;
	}

// -------------------------------------------------------------------------
// Xu ly khi hop thoai dang mo
// -------------------------------------------------------------------------
int KUiAddPoint::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
		{
			// Click len nut cancel ~> dong hop thoai
			CloseWindow();
			nRet = 1;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OKBtn)
		{
			// Click len nut OK ~> Gui goi tin yeu cau mo khoa
			OnOK();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_RETURN && uParam == (unsigned int)(KWndWindow*)&m_Password) // Click len nut OK ~> Gui goi tin yeu cau mo khoa
		{
			OnOK();
			return 1;
		}
		else if (nParam == VK_ESCAPE && uParam == (unsigned int)(KWndWindow*)&m_Password)
		{
			CloseWindow();
			nRet = 1;
		}
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOK();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			CloseWindow();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		{
			//bool result = Check_number(m_Password.G);
			//if(result == true)
			//{
				
			//}
			m_Password.GetIntNumber();
			break;
		}
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

bool KUiAddPoint::Check_number(char* str)
{
   for (int i = 0; i < strlen(str); i++)
   if (isdigit(str[i]) == false)
      return false;
   return true;
}

// -------------------------------------------------------------------------
// Kiem tra gia tri mat khau ruong nguoi dung nhap vao (Bat buoc phai la int)
// -------------------------------------------------------------------------
void KUiAddPoint::OnCheckInput()
{
	int nPass = m_Password.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	else if (nPass > ADDPOINT_MAX_VALUE)
		nPass = ADDPOINT_MAX_VALUE;
	char	szBuff1[16], szBuff2[16];
	itoa(nPass, szBuff1, 10);
	m_Password.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_Password.SetIntText(nPass);
}
