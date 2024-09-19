// -------------------------------------------------------------------------
//	UiMarketNew.h
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndObjContainer.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList2.h"

#define	MAX_SUPERSHOP_PAGETAB 				8
#define MAX_ITEM_PER_ROW 						3
#define MAX_ITEM_PER_COLUMN 				5

class KUiShoppingCart : protected KWndShowAnimate
{
public:
	static KUiShoppingCart*	OpenWindow(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pPriceInfo);
	static KUiShoppingCart*	GetIfVisible();
	static void				CloseWindow(bool bDestroy);
	void					Show();
private:
	KUiShoppingCart();
	~KUiShoppingCart() {}
	void					Initialize();
	void					LoadScheme(const char* pScheme);
	int						WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void					UpdateData();
	void					Breathe();
	void					OnCheckInput();
	void					CartRelease(int nIndex);
	void					AddCount(int nIndex);
	void					DelCount(int nIndex);
private:
	static KUiShoppingCart*		m_pSelf;
	
public:
	KUiObjAtContRegion	m_ItemInfo;
	KUiItemBuySelInfo	m_PriceInfo;

private:
	
	KWndText80					m_Title;
	KWndText80					m_TotalCostTitleText;
	KWndText80					m_TotalSaveTitleText;
	KWndText80					m_OwnTitleText;
	KWndText80					m_TotalCostValueText;
	KWndText80					m_TotalSaveValueText;
	KWndText80					m_OwnValueText;
	KWndScrollBar				m_GH_ScrollBar;
	KWndList2					 m_GH_ListGoodsWnd;
	
	KWndImage					m_GoodsInfo[MAX_ITEM_SPC];
	//KWndObjectBox				m_GoodsInfo_ItemBox[MAX_ITEM_SPC];
	KWndImage						m_GoodsInfo_ItemImg[MAX_ITEM_SPC];
	KWndText80					m_GoodsInfo_GoodsName[MAX_ITEM_SPC];
	KWndText80					m_GoodsInfo_TotalPrice[MAX_ITEM_SPC];
	KWndLabeledButton			m_GoodsInfo_DelItem[MAX_ITEM_SPC];
	KWndButton					m_GoodsInfo_AddCount[MAX_ITEM_SPC];
	KWndButton					m_GoodsInfo_DelCount[MAX_ITEM_SPC];
	KWndEdit32					m_GoodsInfo_CountEdit[MAX_ITEM_SPC];
	
	KWndLabeledButton			m_ConfirmBuy;
	KWndLabeledButton			m_PrePaidBtn;
	KWndButton					m_CloseBtn;

	int							m_nBuyValue;	//sè l­îng
	int							m_nCostValue;	//gi¸ míi
	int							m_nOldValue;	//gi¸ gèc
	int							m_nSaveValue;	//gi¸ gi¶m
	int							m_nOwnValue; // sè xu hiÖn cã
};

class KCanGetNumFrame : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KWndSellItem  : public KWndObjectBox
{
public:
	void Initialize();
	void LoadScheme(const char* pScheme);
	virtual void	PaintWindow();
	void Clear();
	KWndImage					m_MarketGoods;
	KWndObjectBox	     	 	m_ItemBox;
	KWndImage					m_MarketGoods_DisCount;
	KCanGetNumFrame 			m_MarketGoods_imgNewArrival;
	KWndText80            		m_GoodsNameText;
	KWndText80            		m_OriginalPriceText;
	KWndText80            		m_OriginalPrice_NumberText;
	KWndText80            		m_PriceText;
	KWndText80            		m_Price_NumberText;
	KWndLabeledButton			m_MarketGoods_Buy;
private: 
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//´°¿Úº¯Êý
    
};

class KUiSuperShop : protected KWndShowAnimate
{
public:
	static KUiSuperShop*	OpenWindow(BuySellInfo* pBSinfo);
	static KUiSuperShop*	OpenWindow();
	static KUiSuperShop*	GetIfVisible();
	static void				CloseWindow(bool bDestroy);
	static void				LoadScheme(const char* pScheme);
	static void				UpdateShop(BuySellInfo* pBSinfo);
	static bool				PutItem(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pPriceInfo);
	static void				BuyItem();
	static void 			GetCurGioHang(FKGioHang* pGH);
	static void				UpdateGioHang(FKGioHang* pGH);
private:
	KUiSuperShop();
	~KUiSuperShop();
	void					Initialize();
	int						WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void					UpdateData();
	void					SetSellTypeStart(int nStart);
	void					SetCurrSellSort(int nSaleId);	
	void					CancelTrade();
	void					ClearPage();
	void					SellSortChange(int i);
	void					PageChangeInfo();
	void					Clear();
	void					SetPage(int nIndex);
	void					UpdateItem(KUiObjAtContRegion* pItem, int nAdd);
	void					Breathe();
private:
	static KUiSuperShop*		m_pSelf;
private:

	KWndSellItem          		m_WndSellItem[15];
	KWndLabeledButton			m_SellType[MAX_SUPERSHOP_PAGETAB];	
	KWndLabeledButton			m_NextBtn;
	KWndLabeledButton			m_LastBtn;
	KWndLabeledButton			m_PrePaidBtn;
	KWndText256					m_SellTypeGuide;
	KWndLabeledButton			m_NextPageBtn;
	KWndLabeledButton			m_LastPageBtn;
	KWndLabeledButton			m_ShoppingCartBtn;	
	KWndButton					m_CloseBtn;
	
	BuySellInfo					m_pBSinfo;
	KWndText32        			m_CurrentPageText;
	BYTE						m_nStoreActive;
	int		 					m_nSellTypeCount;
	int		 					m_nSellTypeStart;
	int      					m_nCurrentPage;
	int     					m_nCurrentShopId;
	int      					m_nPageCount;
	unsigned int				m_uEnableTextColor;		
	unsigned int				m_uInvalidTextColor;		
	
	KUiObjAtContRegion*	m_pObjsList;
	int					m_nObjCount;
	KUiObjAtContRegion	m_pItemInfo;
	FKGioHang	m_GioHang;
};

class KUiDynamicShop : protected KWndShowAnimate
{
public:
	//----½çÃæÃæ°åÍ³Ò»µÄ½Ó¿Úº¯Êý----
	static KUiDynamicShop*		OpenWindow(BuySellInfo* pBSinfo);				//´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
	static KUiDynamicShop*		GetIfVisible();				//Èç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
	static void			CloseWindow();				//¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
	static void			LoadScheme(const char* pScheme);	//ÔØÈë½çÃæ·½°¸
	static void			CancelTrade();
	void				UpdateShop(BuySellInfo* pBSinfo);
	void				UpdateData();
	void				UpdateItem(KUiObjAtContRegion* pItem, int bAdd);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//´°¿Úº¯Êý
private:
	KUiDynamicShop();
	~KUiDynamicShop() {}
	void	SellSortChange(int i);
	void	SetPage(int nIndex);
	void	Clear();
	void	OnClickButton(KWndButton* pWnd, int bCheck);
	void	OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	Initialize();						//³õÊ¼»¯
private:
	static KUiDynamicShop* m_pSelf;
private:
	KUiObjAtContRegion* m_pObjsList;
	int							m_nObjCount;
	int							m_nPageCount;
	int							m_nCurrentPage;
	int     					m_nCurrentShopId;
	
	BuySellInfo					m_pBSinfo;
	KWndLabeledButton			m_SellType[MAX_SUPERSHOP_PAGETAB];	
	KWndObjectMatrix			m_ItemsBox;		//ÎïÆ·À¸
	KWndLabeledButton			m_BuyBtn;
	KWndLabeledButton			m_SellBtn;
	KWndLabeledButton			m_RepairBtn;
	KWndLabeledButton			m_PreBtn;
	KWndLabeledButton			m_NextBtn;
	KWndButton					m_CloseBtn;
	KWndText32					m_CurPageTxt;
	KWndImage					m_TitleImage;
};