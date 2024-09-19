// -------------------------------------------------------------------------
//	Fong KiÒu
//	2021
//  ¤ nhËp sè l­îng khi giao dÞch
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiTradeConfirmWnd.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "UiPlayerShop.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiTradeConfirm.ini"

KUiTradeConfirm* KUiTradeConfirm::m_pSelf = NULL;

KUiTradeConfirm* KUiTradeConfirm::OpenWindow(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pPriceInfo, TRADE_CONFIRM_ACTION eAction, int uId)
{
	if (pObj == NULL || pPriceInfo == NULL)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTradeConfirm;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_BuyImg.Hide();
		m_pSelf->m_SaleImg.Hide();
		m_pSelf->m_RepairImg.Hide();
		m_pSelf->m_Number.Hide();
		m_pSelf->m_Increase.Enable(false);
		m_pSelf->m_Decrease.Enable(false);
		m_pSelf->m_OkBtn.Enable(true);
		m_pSelf->m_BreakAll.Hide();
		if (eAction == TCA_SALE)
		{
			m_pSelf->m_SaleImg.Show();
			m_pSelf->m_Money.SetTextColor(m_pSelf->m_uNormalPriceColor);
			if(pPriceInfo->nMoneyUnit != moneyunit_money)//#chØ hiÖn thØ gi¸ b¸n b»ng tiÒn v¹n
			{
				pPriceInfo->nMoneyUnit = moneyunit_money;
			}
		}
		else
		{
			if (eAction == TCA_BUY)
			{
				m_pSelf->m_BuyImg.Show();
				m_pSelf->m_Number.Show();
				m_pSelf->m_Increase.Enable(true);
				m_pSelf->m_Decrease.Enable(true);
				//m_pSelf->m_BuyImg.Show();
			}
			else if (eAction == TCA_BREAK)
			{
				m_pSelf->m_Number.Show();
				m_pSelf->m_Increase.Enable(true);
				m_pSelf->m_Decrease.Enable(true);
				m_pSelf->m_BreakImg.Show();
				m_pSelf->m_OkBtn.Enable(true);
				m_pSelf->m_BreakAll.Show();
			}
			else
			{
				m_pSelf->m_Number.Show();
				m_pSelf->m_Increase.Enable(false);
				m_pSelf->m_Decrease.Enable(false);
				m_pSelf->m_RepairImg.Show();
				m_pSelf->m_OkBtn.Enable(true);
				m_pSelf->m_BreakAll.Hide();
			}
			m_pSelf->m_nCurrentScore = g_pCoreShell->GetOwnValue(pPriceInfo->nMoneyUnit);	
			m_pSelf->m_OkBtn.Enable(m_pSelf->m_nCurrentScore >= pPriceInfo->nCurPrice);
			m_pSelf->m_Money.SetTextColor((m_pSelf->m_nCurrentScore >= pPriceInfo->nCurPrice) ? m_pSelf->m_uNormalPriceColor : m_pSelf->m_uNotEnoughMoneyPriceColor);
		}
		m_pSelf->m_ItemInfo = *pObj;
		m_pSelf->m_PriceInfo = *pPriceInfo;
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	m_pSelf->m_uId = uId;
	return m_pSelf;
}

KUiTradeConfirm::KUiTradeConfirm()
{
	m_PriceInfo.szItemName[0] = 0;
	m_PriceInfo.nPrice = 0;
	m_uId = 0;
	m_nCurrentScore = 0;
	m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
	m_bBreakAll = false;
}

//Èç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
KUiTradeConfirm* KUiTradeConfirm::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±ÕÏú»Ù´°¿Ú
//--------------------------------------------------------------------------
void KUiTradeConfirm::CloseWindow(bool bDestroy)
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

void KUiTradeConfirm::Show()
{
	m_ItemName.SetText(m_PriceInfo.szItemName);
	m_Money.SetMoneyUnitPrice(m_PriceInfo.nPrice,m_PriceInfo.nMoneyUnit);
	m_BuyNumber = 1;
	m_Mark = 0;
	m_Number.SetIntText(m_BuyNumber);
	int Left, Top;
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	Wnd_SetExclusive((KWndWindow*)this);
}

//Òþ²Ø´°¿Ú
void KUiTradeConfirm::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
int KUiTradeConfirm::Initialize()
{
	AddChild(&m_ItemName);
	AddChild(&m_Money);
	AddChild(&m_BuyImg);
	AddChild(&m_SaleImg);
	AddChild(&m_RepairImg);
	AddChild(&m_BreakAll);
	AddChild(&m_BreakImg);
	AddChild(&m_Increase);
	AddChild(&m_Decrease);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_Number);
	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	return true;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiTradeConfirm::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_ItemName.Init(&Ini, "ItemName");

		m_Money.Init(&Ini, "Price");			
		Ini.GetString("Price", "Color", "", Buff, sizeof(Buff));
		m_uNormalPriceColor = GetColor(Buff);
		Ini.GetString("Price", "CantBuyColor", "", Buff, sizeof(Buff));
		m_uNotEnoughMoneyPriceColor = GetColor(Buff);

		m_BuyImg.Init(&Ini, "BuyImg");
		m_SaleImg.Init(&Ini, "SaleImg");
		m_RepairImg.Init(&Ini, "RepairImg");
		m_BreakAll.Init(&Ini, "BreakAll");
		m_OkBtn.Init(&Ini, "OkBtn");
		m_CancelBtn.Init(&Ini, "CancelBtn");
		m_Increase.Init(&Ini,"Increase");
		m_Decrease.Init(&Ini,"Decrease");
		m_BreakImg.Init(&Ini,"BreakImg");
		m_Number.Init(&Ini,"BuyNumber");
		m_Money.SetMoneyUnitPrice(m_PriceInfo.nPrice,m_PriceInfo.nMoneyUnit);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿ÚÏûÏ¢º¯Êý
//--------------------------------------------------------------------------
int KUiTradeConfirm::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
		{
			if (m_BuyImg.IsVisible())
				OnBuy();
			else if (m_SaleImg.IsVisible())
				OnSale();
			else if (m_RepairImg.IsVisible())
				OnRepair();
			else
				OnBreak();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BreakAll)
			m_bBreakAll = !m_bBreakAll;
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Increase && (m_BuyImg.IsVisible() || m_BreakImg.IsVisible()) && !KUiPlayerShop::GetIfVisible())
		{
			m_BuyNumber++;
			if (m_BuyNumber > 60)
				m_BuyNumber = 60;
			else if (m_BuyNumber < 1)
				m_BuyNumber = 1;
			m_Money.SetIntText(m_PriceInfo.nPrice * m_BuyNumber);
			int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
			m_Money.SetTextColor((nHoldMoney >= m_PriceInfo.nPrice * m_BuyNumber) ?
				m_uNormalPriceColor : m_uNotEnoughMoneyPriceColor);
			m_Number.SetIntText(m_BuyNumber);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Decrease && (m_BuyImg.IsVisible() || m_BreakImg.IsVisible()) && !KUiPlayerShop::GetIfVisible())
		{
			m_BuyNumber--;
			if (m_BuyNumber > 60)
				m_BuyNumber = 60;
			else if (m_BuyNumber < 1)
				m_BuyNumber = 1;
			m_Money.SetIntText(m_PriceInfo.nPrice * m_BuyNumber);
			int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
			m_Money.SetTextColor((nHoldMoney >= m_PriceInfo.nPrice * m_BuyNumber) ?
				m_uNormalPriceColor : m_uNotEnoughMoneyPriceColor);
			m_Number.SetIntText(m_BuyNumber);
		}
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			if (m_SaleImg.IsVisible())
			{
				OnSale();
			}
			else if (m_BuyImg.IsVisible())
			{
				if (m_BuyImg.IsDisable())
					OnCancel();
				else
					OnBuy();
			}
			else if (m_RepairImg.IsDisable())
			{
				if (m_RepairImg.IsDisable())
					OnCancel();
				else
					OnRepair();
			}
			else if (m_BreakImg.IsDisable())
			{
				OnCancel();
			}
			else
			{
				OnBreak();
			}
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
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

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦µã»÷Âò°´Å¥
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnBuy()
{
	int nBuyNumber = m_Number.GetIntNumber();
	if (m_ItemInfo.Obj.uGenre == CGOG_NPCSELLITEM)
	{
		m_Mark = m_Number.GetIntNumber() * 3;
		if (g_pCoreShell && nBuyNumber)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)(&m_ItemInfo), nBuyNumber);
		}
		CloseWindow(false);
	}
	else if (m_ItemInfo.Obj.uGenre == CGOG_PLAYERSELLITEM)
	{
		if (g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_PLAYER_BUY, (unsigned int)(&m_ItemInfo), m_uId);
		}
		CloseWindow(false);
	}
}

void KUiTradeConfirm::Breathe()
{
	m_BuyNumber = m_Number.GetIntNumber(); 
	if(m_BuyNumber == 0) m_BuyNumber = 1;
	//
	m_Money.SetMoneyUnitPrice(m_PriceInfo.nCurPrice * m_BuyNumber,m_PriceInfo.nMoneyUnit);
	//
	if (m_ItemInfo.Obj.uGenre == CGOG_NPCSELLITEM)
	{
		if (m_Mark > 0)
		{
			if (g_pCoreShell && (m_Mark % 3 == 0))
			{
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY, (unsigned int)(&m_ItemInfo), 0);
			}
			m_Mark--;
			if (m_Mark == 0)
			{
				CloseWindow(false);
			}
		}
		else if (m_BuyNumber <= 1)
		{
			m_Mark = 0;
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦µã»÷Âô°´Å¥
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnSale()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL, (unsigned int)(&m_ItemInfo), 0);
	}
	CloseWindow(false);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦µãÐÞÀí°´Å¥
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnRepair()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR, (unsigned int)(&m_ItemInfo), 0);
	}
	CloseWindow(false);
}

void KUiTradeConfirm::OnCheckInput()
{	
	int nBuyNumber = m_Number.GetIntNumber();

	if (m_BuyImg.IsVisible())
	{	
		if (m_PriceInfo.nCurPrice)
		{
			if (nBuyNumber > (m_nCurrentScore / m_PriceInfo.nCurPrice))
				nBuyNumber = (m_nCurrentScore / m_PriceInfo.nCurPrice);
		}
	}

	if(nBuyNumber < 0)
		nBuyNumber = 0;
	if(nBuyNumber > 100)//fix by phong kiÒu 24/08/2021
		nBuyNumber = 100;

	char	szBuff1[16], szBuff2[16];
	itoa(nBuyNumber, szBuff1, 10);
	m_Number.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_Number.SetIntText(nBuyNumber);
}

void KUiTradeConfirm::OnCancel()
{
	CloseWindow(false);
}

void KUiTradeConfirm::OnBreak()
{
	if (g_pCoreShell)
	{
		KUiBreakItemOption breakOption;
		breakOption.num = m_BuyNumber;
		breakOption.isbreakall = m_bBreakAll;
		//
		g_pCoreShell->OperationRequest(GOI_NPC_ITEM_BREAK, (unsigned int)(&m_ItemInfo), (unsigned int)(&breakOption));
	}
	CloseWindow(false);
}
