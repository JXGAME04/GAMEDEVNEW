// -------------------------------------------------------------------------
//	UiItem.cpp
//	Author: Fong Ki“u
//	24/08/2021
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiTrade.h"
#include "UiItem.h"
#include "UiGetMoney.h"
#include "UiStoreBox.h"
#include "UiTradeConfirmWnd.h"
#include "UiBreakItem.h"
#include "UiShop.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiBase.h"
#include "../ShortcutKey.h"
#include <crtdbg.h>
#include "../UiSoundSetting.h"
#include "UiGetString.h"
#include "UiPlayerShop.h"
#include "UiPlayerBar.h"
#include "../../../Headers/KProtocol.h"

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"UiItem.ini"

KUiItem* KUiItem::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UIITEM_WAIT_GETMONEY,
	UIITEM_WAIT_GETNAME,
	UIITEM_WAIT_GETPRICE,
};

KUiItem* KUiItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiItem* KUiItem::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiItem::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		KUiShop::CancelTrade();
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiItem::Initialize()
{
	AddChild(&m_Money);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_FkCoinValue);
	//AddChild(&m_FkCoinLable);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBox);
	AddChild(&m_OpenStatusPadBtn);
	AddChild(&m_MakeAdvBtn);
	AddChild(&m_MarkPriceBtn);
	AddChild(&m_MakeStallBtn);

	strcpy(m_ShopName,"Si™u Thﬁ Mini");

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_ItemBox.SetContainerId((int)UOC_ITEM_TAKE_WITH);
	m_nMoney = 0; m_nFkcoin = 0;
	m_Mark = 0;
	//m_FkCoinLable.SetText("Xu");
	Wnd_AddWindow(this);
}

void KUiItem::Breathe()
{
	if (!g_pCoreShell->GetGameData(GDI_PLAYER_IS_BAITAN, 0, 0))
	{
		m_MakeStallBtn.CheckButton(FALSE);
		if (m_Mark == 1)
		{
			KUiItem::OnNpcTradeMode(FALSE);
			g_UiBase.SetStatus(UIS_S_IDLE);
			m_Mark = 0;
		}
	}
	else if ( m_Mark == 0 && g_pCoreShell->GetGameData(GDI_PLAYER_IS_BAITAN, 0, 0))
	{
		m_MakeStallBtn.CheckButton(TRUE);
		KUiItem::OnNpcTradeMode(true);
		g_UiBase.SetStatus(UIS_S_TRADE_NPC);
		m_Mark = 1;
	}

	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	m_Money.SetMoneyText(m_nMoney);
	//
	m_nFkcoin = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_FKCOIN, 0, 0);
	m_FkCoinValue.SetCoinText(m_nFkcoin);
}

void KUiItem::OnNpcTradeMode(bool bTrue)
{
	if (m_pSelf)
		m_pSelf->m_ItemBox.EnablePickPut(!bTrue);
}

void KUiItem::UpdateData()
{
	m_ItemBox.Clear();

	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	//m_Money.SetIntText(m_nMoney);
	m_Money.SetMoneyText(m_nMoney);

	KUiObjAtRegion* pObjs = NULL;
	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount)) //c p ph∏t vÔng nhÌ kh´ng g∏n gi∏ trﬁ
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);
		for (int i = 0; i < nCount; i++)
		{
			KUiDraggedObject no;
			no.uGenre = pObjs[i].Obj.uGenre;
			no.uId = pObjs[i].Obj.uId;
			no.DataX = pObjs[i].Region.h;
			no.DataY = pObjs[i].Region.v;
			no.DataW = pObjs[i].Region.Width;
			no.DataH = pObjs[i].Region.Height;
			m_ItemBox.AddObject(&no, 1);
		}
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiItem::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		KUiDraggedObject Obj;
		Obj.uGenre = pItem->Obj.uGenre;
		Obj.uId = pItem->Obj.uId;
		Obj.DataX = pItem->Region.h;
		Obj.DataY = pItem->Region.v;
		Obj.DataW = pItem->Region.Width;
		Obj.DataH = pItem->Region.Height;
		if (bAdd)
			m_ItemBox.AddObject(&Obj, 1);
		else
			m_ItemBox.RemoveObject(&Obj);
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
	}
	else
		UpdateData();
}

void KUiItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_Money.Init(&Ini, "Money");
		m_pSelf->m_FkCoinValue.Init(&Ini, "FkCoinValue");
		//m_pSelf->m_FkCoinLable.Init(&Ini, "FkCoinLable");
		m_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_OpenStatusPadBtn.Init(&Ini, "OpenStatus");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
		m_pSelf->m_MakeAdvBtn.Init(&Ini,"MakeAdvBtn");
		m_pSelf->m_MarkPriceBtn.Init(&Ini,"MarkPriceBtn");
		m_pSelf->m_MakeStallBtn.Init(&Ini,"MakeStallBtn");
	}
}

void KUiItem::OnClickItem(KUiDraggedObject* pItem, bool bDoImmed)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	if (bDoImmed == false)
	{
		KUiItemBuySelInfo	Price = { 0 };
		if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE, (unsigned int)(&Obj), (int)(&Price)))
		{
			KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_SALE); //hi”n thﬁ hÈp thoπi x∏c nhÀn tr≠Ìc khi b∏n
		}
	}
	else
	{
		UISYS_STATUS eStatus = g_UiBase.GetStatus();
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0 && eStatus == UIS_S_TRADE_NPC)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)(&Obj), 0); //b∏n nhanh lu´n
		}
		else if (eStatus == UIS_S_TRADE_SALE)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)(&Obj), 0); //b∏n nhanh lu´n
		}
		else if (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))
		{
			if (KUiStoreBox::GetIfVisible())
				g_pCoreShell->OperationRequest(GOI_RCLICK_MOVE_ITEM, (unsigned int)(&Obj), UOC_ITEM_TAKE_WITH);
			else
				g_pCoreShell->OperationRequest(GOI_USE_ITEM, (unsigned int)(&Obj), UOC_ITEM_TAKE_WITH);
		}
		else
		{
			KSystemMessage	Msg;
			Msg.byConfirmType = SMCT_NONE;
			Msg.byParamSize = 0;
			Msg.byPriority = 0;
			Msg.eType = SMT_NORMAL;
			Msg.uReservedForUi = 0;
			sprintf(Msg.szMessage, "B m gi˜ SHIP ÆÂng thÍi chuÈt ph∂i vµo vÀt ph»m Æ” b∏n nhanh.");
			KUiSysMsgCentre::AMessageArrival(&Msg, NULL);		
		}
	}
}

void KUiItem::OnLockItem(KUiDraggedObject* pItem, int lock)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;
	g_pCoreShell->OperationRequest(GOI_LOCK_PLAYER_ITEM, (unsigned int)(&Obj), lock);
}

void KUiItem::OnRepairItem(KUiDraggedObject* pItem)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	KUiItemBuySelInfo	Price = { 0 };
	if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,
		(unsigned int)(&Obj), (int)(&Price)))
	{
		if(Price.nCurPrice > 0) //edit by phong kieu fix doan nay
		{
			KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_REPAIR);
		}
	}
}

int KUiItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
			{
				if(m_MakeStallBtn.IsButtonChecked())
				{
					g_pCoreShell->OperationRequest(GOI_LIXIAN, 0, 0);
				}
			}
			if (g_UiBase.GetStatus() == UIS_S_TRADE_SETPRICE)
			{
				g_UiBase.SetStatus(UIS_S_IDLE); //fix by phong ki“u 
				KUiItem::OnNpcTradeMode(false);
			}
		}
		break;
	case WND_N_LEFT_CLICK_ITEM:
		if (g_UiBase.GetStatus() == UIS_S_TRADE_SALE)
			OnClickItem((KUiDraggedObject*)uParam, true);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
			OnClickItem((KUiDraggedObject*)uParam, false);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR)
			OnRepairItem((KUiDraggedObject*)uParam);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_LOCKITEM)
			OnLockItem((KUiDraggedObject*)uParam, 1);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_UNLOCKITEM)
			OnLockItem((KUiDraggedObject*)uParam, 0);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_SETPRICE)
		{
			m_nItem = *(KUiDraggedObject*)uParam;
			KUiGetMoney::OpenWindow(1,1000000000,this,UIITEM_WAIT_GETPRICE,&m_Money);
		} 
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
			OnBreakItem((KUiDraggedObject*)uParam);	
		else if ((GetKeyState(VK_SHIFT) & 0x8000) != 0 && g_UiBase.GetStatus() != UIS_S_TRADE_NPC)
			OnThrowAllItem((KUiDraggedObject*)uParam);	
		else
			OnClickItem((KUiDraggedObject*)uParam, true);
		break;
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn && g_UiBase.GetStatus() != UIS_S_TRADE_SETPRICE && !m_MakeStallBtn.IsButtonChecked())
			Hide();
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenStatusPadBtn)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_STATUS);
		else if (uParam == (unsigned int)(KWndWindow*)&m_GetMoneyBtn)
		{
			if (KUiStoreBox::GetIfVisible())
				KUiGetMoney::OpenWindow(0, m_nMoney, this, UIITEM_WAIT_GETMONEY, &m_Money);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MakeAdvBtn && !KUiPlayerShop::GetIfVisible())
		{
			if (!KUiStoreBox::GetIfVisible())
				KUiGetString::OpenWindow("NhÀp lÍi rao","",(KWndWindow*)this,UIITEM_WAIT_GETNAME,1,20);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MakeStallBtn)
		{
			if (KUiPlayerShop::GetIfVisible())
			{
				m_MakeStallBtn.CheckButton(FALSE);
				return 0;
			}

			if (g_pCoreShell)
			{
				if(g_pCoreShell->OperationRequest(GDI_PLAYER_TRADE,(unsigned int)(&m_ShopName), 0) == 0)
				{
					m_MakeStallBtn.CheckButton(FALSE);
				}
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MarkPriceBtn && !KUiPlayerShop::GetIfVisible()&& !m_MakeStallBtn.IsButtonChecked() )
		{
			if (g_UiBase.GetStatus() != UIS_S_TRADE_SETPRICE && !KUiStoreBox::GetIfVisible() )
			{
				g_UiBase.SetStatus(UIS_S_TRADE_SETPRICE);
				KUiItem::OnNpcTradeMode(true);
			}
			else
			{
				g_UiBase.SetStatus(UIS_S_IDLE);
				KUiItem::OnNpcTradeMode(false);
			}
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		if (uParam == UIITEM_WAIT_GETMONEY)
			OnGetMoney(nParam);
		else if (uParam == UIITEM_WAIT_GETNAME)
		{
			if (nParam)
				strcpy(m_ShopName,(const char*)nParam);
		}
		else if (uParam == UIITEM_WAIT_GETPRICE)
		{
			if (nParam > 0)
				OnSetItem(&m_nItem,nParam);
			else
				OnSetItem(&m_nItem,0);
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiItem::OnGetMoney(int nMoney)
{	
	if (nMoney > 0 && KUiStoreBox::GetIfVisible())
	{
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX, true, nMoney);
	}
}

void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (!g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) && !g_UiBase.IsOperationEnable(UIS_O_TRADE_ITEM))
	{
		return;
	}
	//
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;
	//
	UISYS_STATUS eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);		
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_ITEM_TAKE_WITH;

		if (eStatus == UIS_S_TRADE_SALE)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)(&Pick), 0);
			return;
		}
		else if (eStatus == UIS_S_TRADE_REPAIR)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,(unsigned int)(&Pick), 0);
			return;
		}
		else if (eStatus == UIS_S_TRADE_BUY)
			return;
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_ITEM_TAKE_WITH;	
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, pPickPos ? (unsigned int)&Pick : 0, pDropPos ? (int)&Drop : 0);
}

void KUiItem::OnSetItem(KUiDraggedObject* pItem ,int nPrice)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	g_pCoreShell->OperationRequest(GDI_SET_TRADE_ITEM, (unsigned int)(&Obj), nPrice);
}

void KUiItem::OnThrowAllItem( KUiDraggedObject* pItem )
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	g_pCoreShell->OperationRequest(GDI_THROW_ALL_ITEM, (unsigned int)(&Obj), 0);
}

void KUiItem::OnBreakItem( KUiDraggedObject* pItem )
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;
	
	KUiItemBuySelInfo	Price = { 0 };
	//KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_BREAK);
	KUiBreakItem::OpenWindow(&Obj, &Price, 100);
}

void KUiItem::FkAutoSellItem(int nIdx)
{
	g_UiBase.SetStatus(UIS_S_TRADE_SALE);
	KUiObjAtContRegion	Pick;
	Pick.Obj.uGenre = CGOG_ITEM;
	Pick.Obj.uId = nIdx;
	Pick.eContainer = UOC_ITEM_TAKE_WITH;
	g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)(&Pick), 0);
	g_UiBase.SetStatus(UIS_S_IDLE);
}

void KUiItem::FkAutoRepairItem(int nIdx)
{
	g_UiBase.SetStatus(UIS_S_TRADE_REPAIR);
	KUiObjAtContRegion	Pick;
	Pick.Obj.uGenre = CGOG_ITEM;
	Pick.Obj.uId = nIdx;
	Pick.eContainer = UOC_ITEM_TAKE_WITH;
	g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,(unsigned int)(&Pick), 0);
	g_UiBase.SetStatus(UIS_S_IDLE);
}