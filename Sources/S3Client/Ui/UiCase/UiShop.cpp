// -------------------------------------------------------------------------
//	UiTrade.h
//	Fong Ki“u
//	2020
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiShop.h"
#include "UiItem.h"
#include "UiStatus.h"				//edit by phong kieu open F3 khi mo shop
#include "UiTradeConfirmWnd.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/coreshell.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "UiInformation.h"
#include "UiSysMsgCentre.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI		"UiShop.ini"

KUiShop* KUiShop::m_pSelf = NULL;

KUiShop::KUiShop()
{
	m_pObjsList = NULL;
	m_nObjCount = 0;
	m_nPageCount = 0;
}

KUiShop* KUiShop::GetIfVisible()
{
	return m_pSelf;
}

KUiShop* KUiShop::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiShop;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		g_UiBase.SetStatus(UIS_S_TRADE_NPC);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();

		if (KUiItem::GetIfVisible() == NULL)
		{
			//KUiStatus::OpenWindow(); 	//open F3 khi mo shop edit by phong kieu
			KUiItem::OpenWindow();
		}
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);
		KUiItem::OnNpcTradeMode(true);
	}
	return m_pSelf;
}

void KUiShop::CloseWindow()
{
	if (m_pSelf)
	{
		KUiItem::OnNpcTradeMode(false);
		KUiTradeConfirm::CloseWindow(true);
		g_UiBase.SetStatus(UIS_S_IDLE);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiShop::Initialize()
{
	AddChild(&m_ItemsBox);
	AddChild(&m_BuyBtn);
	AddChild(&m_SellBtn);
	AddChild(&m_RepairBtn);
	AddChild(&m_PreBtn);
	AddChild(&m_NextBtn);
	AddChild(&m_CloseBtn);
	
	m_ItemsBox.SetContainerId((int)UOC_NPC_SHOP);
	Wnd_AddWindow(this);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

void KUiShop::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{		
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
			m_pSelf->m_ItemsBox.Init(&Ini, "ItemBox");
			m_pSelf->m_BuyBtn.Init(&Ini, "BuyBtn");
			m_pSelf->m_SellBtn.Init(&Ini, "SellBtn");
			m_pSelf->m_RepairBtn.Init(&Ini, "RepairBtn");
			m_pSelf->m_PreBtn.Init(&Ini, "LeftBtn");
			m_pSelf->m_NextBtn.Init(&Ini,"RightBtn");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");

			m_pSelf->m_ItemsBox.EnablePickPut(false);
		}
	}
}

void KUiShop::CancelTrade()
{
	CloseWindow();
}

int	KUiShop::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam, nParam);
		break;
	case WND_N_LEFT_CLICK_ITEM:
		OnBuyItem((KUiDraggedObject*)uParam, g_UiBase.GetStatus() == UIS_S_TRADE_BUY);
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		if (nParam == (int)(KWndWindow*)&m_ItemsBox)
			OnBuyItem((KUiDraggedObject*)uParam, true);
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
			{
				CloseWindow();
			}
			else
			{
				m_BuyBtn.CheckButton(false);
				m_SellBtn.CheckButton(false);
				m_RepairBtn.CheckButton(false);
				g_UiBase.SetStatus(UIS_S_TRADE_NPC);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiShop::FkAutoOnBuyItem(unsigned int szItemName)
{
	if (szItemName == 0 || g_pCoreShell == NULL)
		return;

	int nIdx = g_pCoreShell->GetItemIdxNpcShop((char*) szItemName);
	if(nIdx >= 0)
	{
		g_UiBase.SetStatus(UIS_S_TRADE_BUY);
		KUiObjAtContRegion	Obj;
		Obj.Obj.uGenre = CGOG_NPCSELLITEM;
		Obj.Obj.uId = nIdx;
		Obj.eContainer = UOC_NPC_SHOP;
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)(&Obj), 1);
		g_UiBase.SetStatus(UIS_S_IDLE);
	}
}

void KUiShop::OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_NPC_SHOP;

	UISYS_STATUS eStatus = g_UiBase.GetStatus();
		if (bDoImmed == false)
		{	
			KUiItemBuySelInfo	Price = { 0 };
			if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
				(unsigned int)(&Obj), (int)(&Price)) && eStatus != UIS_S_TRADE_SALE && eStatus != UIS_S_TRADE_REPAIR)
			{
				KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_BUY);
			}			
		}
		else
		{
			if (eStatus == UIS_S_TRADE_BUY)
			{
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)(&Obj), 1);	
				return;
			}		
			else
			{
				if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
				{
					g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)(&Obj), 1);
					return;
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.byParamSize = 0;
					Msg.byPriority = 0;
					Msg.eType = SMT_NORMAL;
					Msg.uReservedForUi = 0;
					strcpy(Msg.szMessage, "Nh n gi˜ ph›m Shift ÆÂng thÍi nh n chuÈt ph∂i lÀp t¯c mua Æ≠Óc vÀt ph»m!");
					KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
					return;
				}	
			}	
		}
}

void KUiShop::OnClickButton(KWndButton* pWnd, int bCheck)
{
	if (Wnd_GetDragObj(NULL))
		return;
	if (pWnd == &m_BuyBtn)
	{
		if (bCheck)
		{
			m_SellBtn.CheckButton(false);
			m_RepairBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_BUY);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
	else if (pWnd == &m_SellBtn)
	{
		if (bCheck)
		{
			m_BuyBtn.CheckButton(false);
			m_RepairBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_SALE);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
	else if (pWnd == &m_RepairBtn)
	{
		if (bCheck)
		{
			m_BuyBtn.CheckButton(false);
			m_SellBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_REPAIR);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
	else if (pWnd == &m_PreBtn)
		SetPage(m_nCurrentPage - 1);
	else if(pWnd == &m_NextBtn)
		SetPage(m_nCurrentPage + 1);
	else if (pWnd == &m_CloseBtn)
		CloseWindow();
}

void KUiShop::SetPage(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nPageCount && m_pObjsList)
	{
		m_ItemsBox.Clear();
		for (int i = 0; i < m_nObjCount; i++)
		{
			if (m_pObjsList[i].nContainer == nIndex)
			{
				//m_ItemsBox.AddObject((KUiDraggedObject*)&m_pObjsList[i], 1);
				UpdateItem1(&m_pObjsList[i], true);
			}
		}
		m_nCurrentPage = nIndex;
	}
}

void KUiShop::UpdateItem1(KUiObjAtContRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemsBox.AddObject(&Obj, 1);
			else
				m_ItemsBox.RemoveObject(&Obj);
		}
	}
}

void KUiShop::UpdateData()
{
	Clear();
	m_nObjCount = g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, 0, 0);
	if (m_nObjCount == 0)
		return;

	if (m_pObjsList = (KUiObjAtContRegion*)malloc(sizeof(KUiObjAtContRegion) * m_nObjCount))
	{
		g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, (unsigned int)m_pObjsList, m_nObjCount);
		m_nPageCount = m_pObjsList[m_nObjCount - 1].nContainer + 1;
		SetPage(0);
		m_PreBtn.Enable(m_nPageCount > 1);
		m_NextBtn.Enable(m_nPageCount > 1);
	}
	else
		m_nObjCount = 0;
}

void KUiShop::Clear()
{
	m_nObjCount  = 0;
	m_nPageCount = 0;
	if (m_pObjsList)
	{
		free(m_pObjsList);
		m_pObjsList = NULL;
	}
}
