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
#include "UiQuestDT.h"
#include "../UiBase.h"
extern iCoreShell*	g_pCoreShell;
#define Quest1							"UiDaTau.ini"

KUiDaTau* KUiDaTau::m_pSelf = NULL;

KUiDaTau* KUiDaTau::OpenWindow(const char* pszInitString, int nType)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiDaTau;
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

void KUiDaTau::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ShowCursor(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}	
}

void KUiDaTau::Initialize()
{
	AddChild(&m_FinishText);
	AddChild(&EXP);
	AddChild(&random);
	AddChild(&money);

	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}
extern int SCREEN_WIDTH;
void KUiDaTau::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, Quest1);
	if (Ini.Load(Buff))
	{
		if (SCREEN_WIDTH == 1024)
			KWndShowAnimate::Init(&Ini, "Main1024");
		else
			KWndShowAnimate::Init(&Ini, "Main");;
		EXP	.Init(&Ini, "ExpBtn");
		random.Init(&Ini, "RandomBtn");
		money.Init(&Ini, "MoneyBtn");	
		Ini.GetString("Main", "StringQuest", "", m_szLoginBg, sizeof(m_szLoginBg));
		m_pSelf->m_FinishText.Init(&Ini, "Notice");
	}
}

int KUiDaTau::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&EXP)
		{
			Hide();
			//g_pCoreShell->OperationRequest(GOI_DATAU, 1, 0);
			if (g_pCoreShell)
			{
				strcpy(m_pSelf->szFunc1, "finish_exp");
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 3, (unsigned int)m_pSelf->szFunc1);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&money)
		{
			Hide();
			//g_pCoreShell->OperationRequest(GOI_DATAU, 2, 0);
			if (g_pCoreShell)
			{
				strcpy(m_pSelf->szFunc1, "finish_money");
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 3, (unsigned int)m_pSelf->szFunc1);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&random)
		{
			Hide();
			//g_pCoreShell->OperationRequest(GOI_DATAU, 3, 0);
			if (g_pCoreShell)
			{
				strcpy(m_pSelf->szFunc1, "quest_random");
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 3, (unsigned int)m_pSelf->szFunc1);
			}
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}


// ==== auto Da Tau (WAuto 20/08) ====
// Auto bam nut cua so thuong theo DUNG DUONG CLICK that (WND_N_BUTTON_CLICK ->
// WndProc -> Hide + GOI_ADD_UI_CMD_SCRIPT) nen cua so tu dong sau khi chon,
// giong het nguoi choi tu bam.
int KUiDaTau::AutoPick(int nIdx)
{
	if (m_pSelf == NULL || !m_pSelf->IsVisible())
		return 0;
	KWndWindow* pNut = (KWndWindow*)&m_pSelf->EXP;		// 0: kinh nghiem (finish_exp)
	if (nIdx == 1)
		pNut = (KWndWindow*)&m_pSelf->money;			// 1: tien (finish_money)
	else if (nIdx == 2)
		pNut = (KWndWindow*)&m_pSelf->random;			// 2: ngau nhien (quest_random)
	m_pSelf->WndProc(WND_N_BUTTON_CLICK, (unsigned int)pNut, 0);
	return 1;
}

int KUiDaTau::AutoHide()
{
	if (m_pSelf && m_pSelf->IsVisible())
	{
		m_pSelf->Hide();
		return 1;
	}
	return 0;
}
