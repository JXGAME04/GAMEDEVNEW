#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiShell.h"
#include "../../../core/src/coreshell.h"
#include "../Elem/PopupMenu.h"
#include "../UiSoundSetting.h"
#include <crtdbg.h>
#include "UiPK.h"
#include "../UiBase.h"
extern iCoreShell*	g_pCoreShell;
#define PK_UI							"UiPK.ini"

KUiPK* KUiPK::m_pSelf = NULL;

KUiPK::KUiPK()
{
m_pSkinMenu = NULL;
}

KUiPK* KUiPK::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiPK;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiPK* KUiPK::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiPK::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->CancelMenu();
		Wnd_ShowCursor(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}	
}

void KUiPK::CancelMenu()
{
	if (m_pSkinMenu)
	{
		KPopupMenu::Cancel();
		free(m_pSkinMenu);
		m_pSkinMenu = NULL;
	}
}

void KUiPK::Initialize()
{
	AddChild(&LCBT);
	AddChild(&CDBT);
	AddChild(&DSBT);
	AddChild(&LuyenCong);
	AddChild(&ChienDau);
	AddChild(&DoSat);
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiPK::LoadScheme(const char* pScheme)
{
	char Buff[32];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, PK_UI);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		LCBT		.Init(&Ini, "LuyenCongBtn");
		CDBT		.Init(&Ini, "ChienDauBtn");
		DSBT		.Init(&Ini, "DoSatBtn");
		LuyenCong	.Init(&Ini, "LuyenCong");
		LuyenCong	.SetText("LuyÖn c«ng");
		ChienDau	.Init(&Ini, "ChienDau");
		ChienDau	.SetText("ChiÕn ®Êu");
		DoSat		.Init(&Ini, "DoSat");
		DoSat		.SetText("§å s¸t");
	}
}

int KUiPK::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&LuyenCong)
		{

		}
		else if (uParam == (unsigned int)(KWndWindow*)&ChienDau)
		{

		}
		else if (uParam == (unsigned int)(KWndWindow*)&DoSat)
		{

		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		Hide();
		break;		
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}


