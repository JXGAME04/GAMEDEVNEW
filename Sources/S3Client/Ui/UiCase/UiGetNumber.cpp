// -------------------------------------------------------------------------
//	File: UiGetNumber.cpp
//	Author: Fong Kieu
//	Date: 2003-1-7
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiGetNumber.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"

#define	SCHEME_INI 	"UiGetNumber.ini"

extern iCoreShell*		g_pCoreShell;

KUiGetNumber* KUiGetNumber::m_pSelf = NULL;

KUiGetNumber::KUiGetNumber()
{

}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiGetNumber* KUiGetNumber::OpenWindow(const char* pszTitle, const char* nFunc)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiGetNumber;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		strcpy(m_pSelf->szFunc, nFunc);
		m_pSelf->m_Title.SetText(pszTitle);
		m_pSelf->m_NumIn.ClearText();
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiGetNumber::CloseWindow(bool bDestroy)
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
}

KUiGetNumber*	KUiGetNumber::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//显示窗口
void KUiGetNumber::Show()
{
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_NumIn);
	Wnd_SetExclusive((KWndWindow*)this);
}

//隐藏窗口
void KUiGetNumber::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiGetNumber::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_NumIn);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiGetNumber::LoadScheme(const char* pScheme)
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
			m_pSelf->m_NumIn.Init(&Ini, "StringInput");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
		}
	}	
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiGetNumber::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
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
		{
			OnOk();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_SPECIAL_KEY_DOWN:
		if (nParam == VK_RETURN &&
			uParam == (unsigned int)(KWndWindow*)&m_NumIn)
		{
			OnOk();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击确认按钮
//--------------------------------------------------------------------------
void KUiGetNumber::OnOk()
{
	KUiInPutBoxCmd	InPutCmd;
	OnCheckInput();
	int nMoney = m_NumIn.GetIntNumber();
	
	InPutCmd.nNum = nMoney;
	strcpy(InPutCmd.nValue, m_pSelf->szFunc);
	strcpy(InPutCmd.szAction, "");
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_INPUT_INFO, 2, (unsigned int)(&InPutCmd));
	CloseWindow(false);
}

void KUiGetNumber::OnCheckInput()
{
	int nMoney = m_NumIn.GetIntNumber();
	if (nMoney < 0)
		nMoney = 0;
	else if (nMoney > STRING_MAX_VALUE)
		nMoney = STRING_MAX_VALUE;
	char	szBuff1[16], szBuff2[16];
	itoa(nMoney, szBuff1, 10);
	m_NumIn.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_NumIn.SetIntText(nMoney);
}

void KUiGetNumber::OnCancel()
{
	CloseWindow(false);
}
