// -------------------------------------------------------------------------
//	UiTrade.h
//	Fong Ki“u
//	2020
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"

struct KUiObjAtContRegion;

class KUiShop : protected KWndShowAnimate
{
public:
	//--------
	static KUiShop*		OpenWindow();				
	static KUiShop*		GetIfVisible();				
	static void			CloseWindow();				
	static void			LoadScheme(const char* pScheme);	
	static void			CancelTrade();
	static void			FkAutoOnBuyItem(unsigned int szItemName);
	void				UpdateData();	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateItem1(KUiObjAtContRegion* pItem, int bAdd);
private:
	KUiShop();
	~KUiShop() {}
	void	SetPage(int nIndex);
	void	Clear();
	void	OnClickButton(KWndButton* pWnd, int bCheck);
	void	OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	Initialize();						
private:
	static KUiShop*		m_pSelf;
private:
	KUiObjAtContRegion*	m_pObjsList;
	int					m_nObjCount;
	int					m_nPageCount;
	int					m_nCurrentPage;
	KWndObjectMatrix	m_ItemsBox;		
	KWndButton			m_BuyBtn;
	KWndButton			m_SellBtn;
	KWndButton			m_RepairBtn;
	KWndButton			m_PreBtn;
	KWndButton			m_NextBtn;
	KWndButton			m_CloseBtn;
};