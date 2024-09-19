#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiChangePWBox.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiChangePWBox0.ini"

KUiChangePWBox* KUiChangePWBox::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	Kiem tra xem hop thoai co dang visible hay khong, neu visible thi tra ve con tro hop thoai UiChangePWBox.cpp
//--------------------------------------------------------------------------
KUiChangePWBox* KUiChangePWBox::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Mo hop thoai, tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiChangePWBox* KUiChangePWBox::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiChangePWBox;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);

		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);

	}
	return m_pSelf;
}

void KUiChangePWBox::Show()
{
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_OldPassword);
	Wnd_SetExclusive((KWndWindow*)this);
}

//--------------------------------------------------------------------------
// Dong hop thoai 
//--------------------------------------------------------------------------
void KUiChangePWBox::CloseWindow()
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
void KUiChangePWBox::Initialize()
{
	AddChild(&m_OldPassword);
	AddChild(&m_NewPassword1);
	AddChild(&m_NewPassword2);
	AddChild(&m_OKBtn);
	AddChild(&m_CancelBtn);
	
	char schemePath[256];
	g_UiBase.GetCurSchemePath(schemePath, 256);
	LoadScheme(schemePath);

	Wnd_AddWindow(this);
}

// -------------------------------------------------------------------------
// Tao layout hop thoai
// -------------------------------------------------------------------------
void KUiChangePWBox::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_OldPassword.Init(&Ini, "Password");
		m_pSelf->m_NewPassword1.Init(&Ini, "NewPassword");
		m_pSelf->m_NewPassword2.Init(&Ini, "NewPassConfirm");
		m_pSelf->m_NoticeText.Init(&Ini, "Notice");
		m_pSelf->m_OKBtn.Init(&Ini, "Confirm");
		m_pSelf->m_CancelBtn.Init(&Ini, "Cancel");
		
		//m_NoticeText.SetText("");
	}
}

// -------------------------------------------------------------------------------
// Gui goi tin yeu cau mo khoa ruong len server
// -------------------------------------------------------------------------------
void KUiChangePWBox::OnOK()
{
	if (ValidPWs())
	{
		// Password nhap vao la hop le
		int oldPW = m_OldPassword.GetIntNumber();
		int newPW = m_NewPassword1.GetIntNumber();
		
		// Dung core shell de gui lenh doi password len server
		g_pCoreShell->OperationRequest(GOI_CP_CHANGE, oldPW, newPW);
		
		// Dong hop thoai
		CloseWindow();
	}
}

// -------------------------------------------------------------------------
// Xu ly khi hop thoai dang mo
// -------------------------------------------------------------------------
int KUiChangePWBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
		{
			// Click len nut cancel ~> dong hop thoai
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OKBtn)
		{
			// Click len nut OK ~> Gui goi tin yeu cau mo khoa
			OnOK();
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
		m_OldPassword.GetIntNumber();
		m_NewPassword1.GetIntNumber();
		m_NewPassword2.GetIntNumber();
		break;
	default:
		nRet =  KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

// --------------------------------------------------------------------------------
// Kiem tra xem 3 password nhap vao co hop le hay khong
// ------------------------------------------------------------------------------
BOOL KUiChangePWBox::ValidPWs()
{
	// De password nhap vao la hop le, chung phai dam bao do dai 6 chu so va 2 password moi nhap vao giong nhau
	int oldPW  = m_OldPassword.GetIntNumber();;
	int newPW1 = m_NewPassword1.GetIntNumber();
	int newPW2 = m_NewPassword2.GetIntNumber();
	if (oldPW < CHEST_PW_MIN_VALUE || oldPW > CHEST_PW_MAX_VALUE || 
		newPW1 < CHEST_PW_MIN_VALUE || newPW1 > CHEST_PW_MAX_VALUE || 
		newPW2 < CHEST_PW_MIN_VALUE || newPW2 > CHEST_PW_MAX_VALUE)
	{		
		
		g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PW_NOT_LONG, 0);
		return FALSE;

	}
	if ( newPW1 != newPW2)
	{
		g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PW_NOT_SAME, 0);	
		return FALSE;		
	}
	// Mat khau nhap vao vuot qua tat ca cac vong kiem tra
	//g_pCoreShell->OperationRequest(GOI_PLAYER_ACTION, PW_ACCEPTED, 0);
	return TRUE;	
}

// --------------------------------------------------------------------------------
// Xu ly khi nguoi dung nhap password
// --------------------------------------------------------------------------------
void KUiChangePWBox::OnCheckInput()
{
	int nPass = 0;
	char	szBuff1[16], szBuff2[16];
	
	// Password cu
	nPass = 0;
	nPass = m_OldPassword.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	while (nPass > CHEST_PW_MAX_VALUE)
		nPass = nPass / 10;	
	itoa(nPass, szBuff1, 10);
	m_OldPassword.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2)) 
		m_OldPassword.SetIntText(nPass);
		
	// Password moi 1
	nPass = 0;
	nPass = m_NewPassword1.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	while (nPass > CHEST_PW_MAX_VALUE)
		nPass = nPass / 10;	
	itoa(nPass, szBuff1, 10);
	m_NewPassword1.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_NewPassword1.SetIntText(nPass);
		
	// Password moi 2
	nPass = 0;
	nPass = m_NewPassword2.GetIntNumber();
	if (nPass < 0)
		nPass = 0;
	while (nPass > CHEST_PW_MAX_VALUE)
		nPass = nPass / 10;	
	itoa(nPass, szBuff1, 10);
	m_NewPassword2.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_NewPassword2.SetIntText(nPass);
}