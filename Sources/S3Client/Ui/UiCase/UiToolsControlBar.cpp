/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Fong KiÒu
//	CreateTime:	2020-4-22
//	File:UiToolsControlBar.cpp
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiToolsControlBar.h"
#include "UiChatCentre.h"

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/KDebug.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"UiToolsControlBar.ini"
//#define	SCHEME_INI		"UiToolsControlBar_D.ini"
#define	PK_MENU_SELECT_BASE	0x001

KUiToolsControlBar* KUiToolsControlBar::m_pSelf = NULL;

char g_ArrayPK[][64] = 
{
	"LuyÖn c«ng",
	"ChiÕn ®Êu",
	"§å s¸t"
};

KUiToolsControlBar* KUiToolsControlBar::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiToolsControlBar;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

void KUiToolsControlBar::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiToolsControlBar::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiToolsControlBar::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
		}
	}
}

void KUiToolsControlBar::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left",  0, &nValue1);
			Ini.GetInteger("Main", "Top",   0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

void KUiToolsControlBar::Breathe()
{
	UpdateData();
}

int KUiToolsControlBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_PK)
			PopUpPK();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) >= PK_MENU_SELECT_BASE && (short)(LOWORD(nParam)) >= 0)
				ProcessPopUpPK(LOWORD(nParam));
		}
		break;
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiToolsControlBar::PopUpPK()
{
	int nActionDataCount = sizeof(g_ArrayPK) / 32;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == Exercises) || (i == Fighting) || (i == Murder))
		{
			strncpy(pSelUnitMenu->Items[i].szData, g_ArrayPK[i], 63);
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int Left, Top, nWidth, nHeight;
	GetSize(&nWidth, &nHeight);
	GetAbsolutePos(&Left, &Top);
	Left += ((nWidth/2) + 62);
	Top += ((nHeight/2));
	pSelUnitMenu->nX = Left;
	pSelUnitMenu->nY = Top;
	KPopupMenu::Popup(pSelUnitMenu, this, PK_MENU_SELECT_BASE);
}

void KUiToolsControlBar::ProcessPopUpPK(int nAction)
{
	switch(nAction)
	{
	case Exercises:
		g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKNormal);
		break;
	case Fighting:
		g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKWar);
		break;
	case Murder:
		g_pCoreShell->OperationRequest(GOI_PK_SETTING, 0, enumPKMurder);
		break;
	}
}