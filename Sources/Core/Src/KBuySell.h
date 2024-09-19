#ifndef	KBuySellH
#define	KBuySellH

#define	BUY_SELL_SCALE		4

class KItem;

typedef struct
{
	int		nGenre;
	int		nDetailType;
	int		nParticularType;
	int		nSeriesReq;
	int		nLevel;
	int		nPrice;
	int		nNewPrice;
	int		nExpPointSec;
	int		nQuaility;
	int		nParam;//sè lÇn sö dông item
} ItemGenerate;

class KInventory;

class KBuySell
{
public:
	KBuySell();
	~KBuySell();
	BOOL			Init();
	int				GetWidth() { return m_Width; }
	int				GetHeight() { return m_Height; }
	KItem*			GetItem(int nIndex);
	int				GetItemIndex(int nShop, int nIndex);
	BOOL			BuySellCheck(int nBuy, int nBuyIdx);
private:
	int**			m_SellItem;
	KItem*			m_Item;
	int				m_Width;
	int				m_Height;
	int				m_MaxItem;
	int				m_ItemNum;
public:
#ifdef _SERVER
	void			OpenSale(int nPlayerIdx, int nShop, int nShopMoneyUnit);
	void			OpenSale(int nPlayerIdx, int nSaleType, int nMoneyUnit, int nShopNum, int *nShopId);
	BOOL			Buy(int nPlayerIdx, int nBuy, int nBuyIdx, BYTE nBuyNumber);	// ÂòµÚ¼¸¸öÂòÂôÁÐ±íÖÐµÄµÚ¼¸ÏîµÀ¾ß
	BOOL			Sell(int nPlayerIdx, int nBuy, int nIdx, int nBuyNumber);
	BOOL			AutoSell(int nPlayerIdx, int nBuy, int nIdx, int nBuyNumber);
	BOOL	CanBuy(int nPlayerIdx, int nBuy, int nBuyIdx, int nBuyNumber);
	BOOL	AutoBuyItem(int nPlayerIdx, BYTE nItemGenre, BYTE nDetailType, BYTE nLevel, BYTE nBuyNumber);
#endif
#ifndef _SERVER
	KInventory*		m_pShopRoom;
	KInventory*		m_pSShopRoom;
	void			OpenSale(BuySellInfo *pInfo);
	void			OpenSale(int nSaleType, BuySellInfo *pInfo);
	void			PaintItem(int nIdx, int nX, int nY, BOOL bStack = FALSE);
#endif
};

extern KBuySell	BuySell;
#endif
