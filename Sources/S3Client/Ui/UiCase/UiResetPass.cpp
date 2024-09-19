#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiResetPass.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiResetPass.ini"

KUiResetPass* KUiResetPass::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	Kiem tra xem hop thoai co dang visible hay khong, neu visible thi tra ve con tro hop thoai UiResetPass.cpp
//--------------------------------------------------------------------------
KUiResetPass* KUiResetPass::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Mo hop thoai, tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiResetPass* KUiResetPass::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiResetPass;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

void KUiResetPass::Show()
{
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_Password);
	Wnd_SetExclusive((KWndWindow*)this);
}

//--------------------------------------------------------------------------
// Dong hop thoai 
//--------------------------------------------------------------------------
void KUiResetPass::CloseWindow()
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
void KUiResetPass::Initialize()
{
	AddChild(&m_Password);
	AddChild(&m_OKBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_Notice);
	
	char schemePath[256];
	g_UiBase.GetCurSchemePath(schemePath, 256);
	LoadScheme(schemePath);

	Wnd_AddWindow(this);
}

// -------------------------------------------------------------------------
// Tao layout hop thoai
// -------------------------------------------------------------------------
void KUiResetPass::LoadScheme(const char* pScheme)
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
		m_pSelf->m_Notice.Init(&Ini, "Notice");
	}
}

void KUiResetPass::UpdateNotice()
{
	m_Notice.SetText("Nhap pass gom 6 chu so");
}
// -------------------------------------------------------------------------------
// Gui goi tin yeu cau doi khoa ruong len server
// -------------------------------------------------------------------------------
void KUiResetPass::OnOK()
	{
		OnCheckInput();
		int inputedPassword = m_Password.GetIntNumber();
		g_pCoreShell->OperationRequest(GOI_CP_RESET, 0, inputedPassword);
		CloseWindow();
		return;
	}

// -------------------------------------------------------------------------
// Xu ly khi hop thoai dang mo
// -------------------------------------------------------------------------
int KUiResetPass::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
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
			// Click len nut OK ~> Gui goi tin yeu cau doi khoa
			OnOK();
			nRet = 1;
		}
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			// Click len nut OK ~> Gui goi tin yeu cau doi khoa
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
		OnCheckInput();
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

// -------------------------------------------------------------------------
// Kiem tra gia tri mat khau ruong nguoi dung nhap vao (Bat buoc phai la int)
// -------------------------------------------------------------------------
void KUiResetPass::OnCheckInput()
{
	int nPass = m_Password.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	else if (nPass > CHEST_PW_MAX_VALUE)
		nPass = CHEST_PW_MAX_VALUE;
	char	szBuff1[16], szBuff2[16];
	itoa(nPass, szBuff1, 10);
	m_Password.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_Password.SetIntText(nPass);
}
