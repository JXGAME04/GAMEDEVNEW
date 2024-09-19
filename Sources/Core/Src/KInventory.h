#ifndef KInventoryH
#define	KInventoryH

class CORE_API KInventory
{
	friend	class	KItemList;

private:
	int*	m_pArray;
	int		m_nMoney;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nXu;

public:
	KInventory();
    ~KInventory();

	BOOL		Init(int nWidth, int nHeight);
	void			Release();
	void			Clear();
	BOOL		 PickUpItem(int nIdx, int nX, int nY, int nWidth, int nHeight);
	int				  FindItem(int nX, int nY);
	BOOL		 PlaceItem(int nXpos, int nYpos, int nIdx, int nWidth, int nHeight);
	BOOL		 HoldItem(int nIdx, int nWidth, int nHeight);
	BOOL		 FindRoom(int nWidth, int nHeight, POINT* pPos);
	BOOL		 CheckRoom(int nXpos, int nYpos, int nWidth, int nHeight);
	BOOL		 AddMoney(int nMoney);
	int				  GetMoney() { return m_nMoney; }
	BOOL		 SetMoney(int nMoney) { if (nMoney < 0) return FALSE; m_nMoney = nMoney; return TRUE; }
	BOOL		 AddXu(int nXu);
	int				  GetXu() { return m_nXu; }
	BOOL		 SetXu(int nXu) { if (nXu < 0) return FALSE; m_nXu = nXu; return TRUE; }
	int				  GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY);
	BOOL		 FindFreeCell(int nX, int nY);
	BOOL		 FindEmptyPlace(int nWidth, int nHeight, POINT *pPos);
	BOOL		 FindSameToRemove(int nItemNature, int nItemGenre, int nDetail, int nItemParticular, int nLevel, int nSeries, int *pnIdx);
	BOOL		 FindSameDetailType(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY);
	int				  FindNumberArrayItem(int nIdx);//add by phong kiÒu antihack
	BOOL		 CheckSameItemType(int nGenre, int nDetail, int nParticular, int nLevel);
	int				  CalcSameItemType(int nGenre, int nDetail, int nParticular, int nLevel);
	BOOL		 FindSameItemToSort(int nIdx, int *pnIdx, int *pnX, int *pnY);
	void			 FindSameItemName(int nIdx, int pnIdx[]);
};
#endif //KInventoryH
