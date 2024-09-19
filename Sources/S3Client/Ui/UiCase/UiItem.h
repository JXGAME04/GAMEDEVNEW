/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2021
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

struct KUiObjAtRegion;

class KUiItem : public KWndShowAnimate
{
public:
	static KUiItem* OpenWindow();
	static KUiItem* GetIfVisible();				
	static void		CloseWindow(bool bDestroy);	
	static void		LoadScheme(const char* pScheme);
	static void		OnNpcTradeMode(bool bTrue);
	void			UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void			Breathe();									
	static	void	FkAutoSellItem(int nIdx);
	static	void	FkAutoRepairItem(int nIdx);
private:
	KUiItem() {}
	~KUiItem() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	void	OnClickItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	OnSetItem(KUiDraggedObject* pItem, int nPrice);
	void	OnRepairItem(KUiDraggedObject* pItem);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnGetMoney(int nMoney);
	void	OnBreakItem(KUiDraggedObject* pItem);
	void	OnThrowAllItem(KUiDraggedObject* pItem);
	void	OnLockItem(KUiDraggedObject* pItem, int lock);
private:
	static KUiItem*		m_pSelf;
private:
	int					m_nMoney;
	int					m_nFkcoin;
	int					m_Mark;
	char				m_ShopName[32];
	KUiDraggedObject	m_nItem;
	KWndText256			m_Money;
	KWndText256			m_FkCoinValue;	//add by Fong Ki“u
	//KWndText256			m_FkCoinLable;
	KWndButton			m_GetMoneyBtn;
	KWndButton			m_CloseBtn;
	KWndObjectMatrix	m_ItemBox;
	KWndButton			m_OpenStatusPadBtn;
	KWndButton			m_MakeAdvBtn;
	KWndButton			m_MarkPriceBtn;
	KWndButton			m_MakeStallBtn;
};