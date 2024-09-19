#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiShell.h"
#include "../../../core/src/coreshell.h"
#include "UiInit.h"
#include "UiLoginBg.h"
#include "../UiSoundSetting.h"
#include <crtdbg.h>
#include "UiQuestDT1.h"
#include "../UiBase.h"
extern iCoreShell*	g_pCoreShell;
#define Quest2							"UiDaTau1.ini"

KUiDaTau1* KUiDaTau1::m_pSelf = NULL;

KUiDaTau1* KUiDaTau1::OpenWindow(const char* pszInitString, int nType)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiDaTau1;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		if (pszInitString)
			m_pSelf->m_FinishText.SetText(pszInitString);
		else
			m_pSelf->m_FinishText.SetText("");
		m_pSelf->m_bType = nType;
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiDaTau1::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ShowCursor(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}	
}

void KUiDaTau1::Initialize()
{
	AddChild(&m_FinishText);
	AddChild(&diem);
	AddChild(&bonv);
	AddChild(&item);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiDaTau1::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, Quest2);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		diem.Init(&Ini, "PointBtn");
		bonv.Init(&Ini, "LuckyBtn");
		item.Init(&Ini, "ItemBtn");
		Ini.GetString("Main", "StringQuest", "", m_szLoginBg, sizeof(m_szLoginBg));
		m_pSelf->m_FinishText.Init(&Ini, "Notice");
	}
}

int KUiDaTau1::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&diem)
		{
			Hide();
			//g_pCoreShell->OperationRequest(GOI_DATAU1, 6, 0);
			if (g_pCoreShell)
			{
				strcpy(m_pSelf->szFunc1, "finish_point");
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 4, (unsigned int)m_pSelf->szFunc1);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&bonv)
		{
			Hide();
			//g_pCoreShell->OperationRequest(GOI_DATAU1, 4, 0);
			if (g_pCoreShell)
			{
				strcpy(m_pSelf->szFunc1, "finish_lucky");
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 4, (unsigned int)m_pSelf->szFunc1);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&item)
		{
			Hide();
			//g_pCoreShell->OperationRequest(GOI_DATAU1, 5, 0);
			if (g_pCoreShell)
			{
				strcpy(m_pSelf->szFunc1, "finish_item");
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 4, (unsigned int)m_pSelf->szFunc1);
			}
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}


