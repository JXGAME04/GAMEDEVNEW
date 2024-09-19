#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiShell.h"
#include "UiInit.h"
#include "UiLoginBg.h"
#include "UiConnectInfo.h"
#include "UiNotice.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../../Login/Login.h"
#include <crtdbg.h>

/*
Author: Fong KiÒu
Function: Giíi thiÖu NPH
*/

#define	SCHEME_INI_NOTICE				"UiNotice.ini"
#define	NOTICELIST_INI					"\\Ui\\Notice.ini"

KUiNotice* KUiNotice::m_pSelf = NULL;

KUiNotice::KUiNotice()
{

}

KUiNotice::~KUiNotice()
{
}

KUiNotice* KUiNotice::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiNotice;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->Show();
		m_pSelf->UpdateNotice();
	}
	return m_pSelf;
}

void KUiNotice::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

void KUiNotice::Initialize()
{
	AddChild(&m_Notice);
	AddChild(&m_Info);
	AddChild(&m_Close);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

KUiNotice*	KUiNotice::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiNotice::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_NOTICE);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_Notice		.Init(&Ini, "Notice");
		m_Info    		.Init(&Ini, "Info");	
		m_Close   		.Init(&Ini, "Close");
	}
}

void KUiNotice::UpdateNotice()
{
	KIniFile	File;
	if (!File.Load(NOTICELIST_INI))
		return;
		
    MsgListBox_LoadContent(m_Info.GetMessageListBox(), &File, "InfoList");
    MsgListBox_LoadContent(m_Notice.GetMessageListBox(), &File, "RuleList");
}

int KUiNotice::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam);
		break;
	case WM_KEYDOWN:
		nRet = OnKeyDown(uParam);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiNotice::OnClickButton(KWndButton* pBtn)
{
	if (pBtn == &m_Close)
	{
		if (KUiInit::OpenWindow(false, false))
			CloseWindow(true);
	}
}

int KUiNotice::OnKeyDown(unsigned int uKey)
{
	int	nRet = 1;
	KWndWindow* pToActive = NULL;
	switch (uKey)
	{
	case VK_ESCAPE:
		if (m_Close.IsButtonActive())
			if (KUiInit::OpenWindow(false, false))
				CloseWindow(true);
		break;
	case VK_LEFT:
		if (m_Close.IsButtonActive() == 0)
			pToActive = &m_Close;
		break;
	case VK_RIGHT:
		if (m_Close.IsButtonActive() == 0)
			pToActive = &m_Close;
		break;
	default:
		nRet = 0;
	break;
	}
	if (pToActive)
		pToActive->SetCursorAbove();
	return nRet;
}

void KUiNotice::ShowCompleted()
{
    m_Close.SetCursorAbove();
}