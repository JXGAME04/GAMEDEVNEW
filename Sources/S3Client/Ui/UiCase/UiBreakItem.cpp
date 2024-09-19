// -------------------------------------------------------------------------
//	File: UiBreakItem.cpp
//	Author: Fong Kieu
//	Date: 2021
// -------------------------------------------------------------------------

#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiBreakItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiBreakItem.ini"

KUiBreakItem* KUiBreakItem::m_pSelf = NULL;

KUiBreakItem* KUiBreakItem::OpenWindow(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pInfo, int nMaxItem)
{
	if (pObj == NULL)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiBreakItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (nMaxItem > 0)
			m_pSelf->m_nMaxItem = nMaxItem - 1;
		else
			m_pSelf->m_nMaxItem = 0;
		m_pSelf->m_ItemInfo = *pObj;
		m_pSelf->m_BreakInfo = *pInfo;
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiBreakItem::KUiBreakItem()
{
	m_BreakInfo.szItemName[0] = 0;
	m_nMaxItem = 0;
	m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
}

//如果窗口正被显示，则返回实例指针
KUiBreakItem* KUiBreakItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiBreakItem::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}


//显示窗口
void KUiBreakItem::Show()
{
	m_ItemName.SetText(m_BreakInfo.szItemName);
	m_Number.SetIntText(1);
	int Left, Top;
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_Number);
	Wnd_SetExclusive((KWndWindow*)this);
}

//隐藏窗口
void KUiBreakItem::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiBreakItem::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_ItemName);
	AddChild(&m_Number);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_Increase);
	AddChild(&m_Decrease);
	AddChild(&m_CheckBtn);
	IsCheck = false;	
	Wnd_AddWindow(this);
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiBreakItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_ItemName.Init(&Ini, "ItemName");
		m_OkBtn.Init(&Ini, "OkBtn");
		m_CancelBtn.Init(&Ini, "CancelBtn");
		m_Number.Init(&Ini,"StringInput");
		m_Increase.Init(&Ini,"Increase");
		m_Decrease.Init(&Ini,"Decrease");
		m_CheckBtn.Init(&Ini,"CheckBtn");
		m_Number.SetIntText(1);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiBreakItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Increase)
			OnIncrease();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Decrease)
			OnDecrease();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CheckBtn)
			OnCheck();			
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
			uParam == (unsigned int)(KWndWindow*)&m_Number)
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

void KUiBreakItem::OnCancel()
{
	CloseWindow(false);
}

void KUiBreakItem::OnCheck()
{	
	if(IsCheck == false)
	{
		IsCheck = true;
		m_CheckBtn.CheckButton(true);
	}
	else
	{
		IsCheck = false;
		m_CheckBtn.CheckButton(false);
	}	
}

void KUiBreakItem::OnCheckInput()
{
	int nMoney = m_Number.GetIntNumber();
	if (nMoney <= 0)
		nMoney = 0;
	else if (nMoney >= m_nMaxItem)
		nMoney = m_nMaxItem;
	char	szBuff1[16], szBuff2[16];
	itoa(nMoney, szBuff1, 10);
	m_Number.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_Number.SetIntText(nMoney);
}

void KUiBreakItem::OnOk()
{
	KUiBreakItemOption	breakOption;
	if (g_pCoreShell)
	{
		OnCheckInput();
		int nNumber = m_Number.GetIntNumber();
		breakOption.num = nNumber;
		breakOption.isbreakall = IsCheck;
		g_pCoreShell->OperationRequest(GOI_NPC_ITEM_BREAK, (unsigned int)(&m_ItemInfo), (unsigned int)(&breakOption));
	}
	CloseWindow(false);
}

void KUiBreakItem::OnIncrease()
{	
	int nNumber = m_Number.GetIntNumber();
	nNumber++;
	int m_nMaxNumber = 50;
	if (nNumber > m_nMaxNumber)
		nNumber = m_nMaxNumber;

	m_Number.SetIntText(nNumber);
}

//-----------------------
void KUiBreakItem::OnDecrease()
{	
	int nNumber = m_Number.GetIntNumber();
	nNumber--;
	if(nNumber < 1)
		nNumber = 1;
	m_Number.SetIntText(nNumber);
}