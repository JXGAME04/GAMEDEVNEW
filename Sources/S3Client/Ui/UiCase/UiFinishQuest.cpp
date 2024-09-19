/*****************************************************************************************
//	Hop thoai tra nhiem vu
//	Copyright : King Soft
//	Author	:   Fong Ki“u
//	Date: 2021
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiFinishQuest.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../../ENGINE/Src/KSG_MD5_String.h"

#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiFinishQuest.ini"
#define	EXP_SCRIPT_FILE	"quest_exp.lua"
#define	POINT_SCRIPT_FILE	"quest_point.lua"
#define	LUCKY_SCRIPT_FILE	"quest_lucky.lua"
#define	RANDOM_SCRIPT_FILE	"quest_random.lua"
#define	ITEM_SCRIPT_FILE	"quest_item.lua"
#define	MONEY_SCRIPT_FILE	"quest_money.lua"

KUiFinishQuest* KUiFinishQuest::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	Kiem tra xem hop thoai co dang visible hay khong, neu visible thi tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiFinishQuest* KUiFinishQuest::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Mo hop thoai, tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiFinishQuest* KUiFinishQuest::OpenWindow(const char* pszInitString, int nType)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiFinishQuest;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if (pszInitString)
			m_pSelf->m_FinishText.SetText(pszInitString);
		else
			m_pSelf->m_FinishText.SetText("");
		m_pSelf->m_bType = nType;
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
// Dong hop thoai 
//--------------------------------------------------------------------------
void KUiFinishQuest::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}
//-----------------------------------------------------
// Khoi tao cac bien
//-----------------------------------------------------
KUiFinishQuest::KUiFinishQuest()
{
	m_bType = 0;
}
//--------------------------------------------
// Lam moi du lieu
//--------------------------------------------
void KUiFinishQuest::UpdateData()
{
	m_ExpBtn.Hide();
	m_PointBtn.Hide();
	m_LuckyBtn.Hide();
	m_RandomBtn.Hide();
	m_ItemBtn.Hide();
	m_MoneyBtn.Hide();
	switch(m_pSelf->m_bType)
	{
		case 1:
			m_ExpBtn.Show();
			m_PointBtn.Show();
			m_LuckyBtn.Show();
			break;
		case 2:
			m_ExpBtn.Show();
			m_PointBtn.Show();
			m_MoneyBtn.Show();
			break;
		case 3:
			m_ExpBtn.Show();
			m_ItemBtn.Show();
			m_LuckyBtn.Show();
			break;
		case 4:
			m_ExpBtn.Show();
			m_ItemBtn.Show();
			m_MoneyBtn.Show();
			break;
		case 5:
			m_RandomBtn.Show();
			m_PointBtn.Show();
			m_LuckyBtn.Show();
			break;
		case 6:
			m_RandomBtn.Show();
			m_PointBtn.Show();
			m_MoneyBtn.Show();
			break;
		case 7:
			m_RandomBtn.Show();
			m_ItemBtn.Show();
			m_LuckyBtn.Show();
			break;
		case 8:
			m_RandomBtn.Show();
			m_ItemBtn.Show();
			m_MoneyBtn.Show();
			break;
		default:
			break;
	}

}

void KUiFinishQuest::Show()
{
	KWndShowAnimate::Show();
	Wnd_SetExclusive((KWndWindow*)this);
}
// -------------------------------------------------------------------------
// Khoi tao hop thoai
// -------------------------------------------------------------------------
void KUiFinishQuest::Initialize()
{
	AddChild(&m_FinishText);
	AddChild(&m_ExpBtn);
	AddChild(&m_PointBtn);
	AddChild(&m_LuckyBtn);
	AddChild(&m_RandomBtn);
	AddChild(&m_ItemBtn);
	AddChild(&m_MoneyBtn);

	
	char schemePath[256];
	g_UiBase.GetCurSchemePath(schemePath, 256);
	LoadScheme(schemePath);

	Wnd_AddWindow(this);
}

// -------------------------------------------------------------------------
// Tao layout hop thoai
// -------------------------------------------------------------------------
void KUiFinishQuest::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_FinishText.Init(&Ini, "Notice");
		m_pSelf->m_ExpBtn.Init(&Ini, "ExpBtn");
		m_pSelf->m_PointBtn.Init(&Ini, "PointBtn");
		m_pSelf->m_LuckyBtn.Init(&Ini, "LuckyBtn");
		m_pSelf->m_RandomBtn.Init(&Ini, "RandomBtn");
		m_pSelf->m_ItemBtn.Init(&Ini, "ItemBtn");
		m_pSelf->m_MoneyBtn.Init(&Ini, "MoneyBtn");

	}
}

// -------------------------------------------------------------------------
// Xu ly khi hop thoai dang mo
// -------------------------------------------------------------------------
int KUiFinishQuest::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{	
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_ExpBtn)
		{	
			//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)EXP_SCRIPT_FILE, 0);
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
			}
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PointBtn)
		{	
			//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)POINT_SCRIPT_FILE, 0);
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
			}
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_LuckyBtn)
		{	
			//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)LUCKY_SCRIPT_FILE, 0);
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
			}
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_RandomBtn)
		{	
			//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)RANDOM_SCRIPT_FILE, 0);
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
			}
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ItemBtn)
		{	
			//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)ITEM_SCRIPT_FILE, 0);
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
			}
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MoneyBtn)
		{	
			//g_pCoreShell->OperationRequest(GOI_LOAD_BUTTON_SCRIPT, (unsigned int)MONEY_SCRIPT_FILE, 0);
			if (g_pCoreShell)
			{
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
			}
			CloseWindow();
		}
		break;
	default:
		nRet =  KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

