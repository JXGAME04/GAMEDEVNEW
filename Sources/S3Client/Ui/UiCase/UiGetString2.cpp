#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiGetString2.h"
#include "UiInformation.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiGetString2.ini"

KUiGetString2* KUiGetString2::m_pSelf = NULL;

KUiGetString2* KUiGetString2::OpenWindow(const char* pszTitle, const char* nScriptIndex)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiGetString2;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_nMinLen = 1;
		m_pSelf->m_nMaxLen = 512;
		strcpy(m_pSelf->m_nScriptIndex, nScriptIndex);
		strcpy(m_pSelf->m_szTitle, pszTitle);
		m_pSelf->m_Title.SetText(m_pSelf->m_szTitle);			
		m_pSelf->m_StringEdit.ClearText();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

void KUiGetString2::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
	Wnd_GameSpaceHandleInput(true);
}

KUiGetString2*	KUiGetString2::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiGetString2::Show()
{
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_StringEdit);
	Wnd_SetExclusive((KWndWindow*)this);
}

void KUiGetString2::Hide()
{
	Wnd_SetFocusWnd(NULL);
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

void KUiGetString2::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_StringEdit);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	nTextPos = 0;
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

void KUiGetString2::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Title.Init(&Ini, "Title");
			m_pSelf->m_StringEdit.Init(&Ini, "StringInput");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
		}
	}
}

int KUiGetString2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
			OnOk();
		else if (uParam == VK_ESCAPE)
			OnCancel();
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiGetString2::OnOk()
{
	KUiInPutBoxCmd	InPutCmd;
	char	szString[512];

	int nLen = m_pSelf->m_StringEdit.GetText(szString, sizeof(szString), true);	
	if (nLen >= m_nMinLen && nLen <= m_nMaxLen)
	{
		InPutCmd.nNum = 0;
		strcpy(InPutCmd.nValue, m_pSelf->m_nScriptIndex);
		strcpy(InPutCmd.szAction, szString);
		g_pCoreShell->OperationRequest(GOI_INPUT_INFO, 1, (unsigned int)(&InPutCmd));
		CloseWindow(false);
	}
	else
	{
		UIMessageBox("Vui lßng nhËp d÷ liÖu");
	}
}

void KUiGetString2::OnCancel()
{
	CloseWindow(false);
}
