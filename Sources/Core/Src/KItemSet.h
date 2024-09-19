#ifndef KItemSetH
#define	KItemSetH

#include "KLinkArray.h"

#define		IN
#define		OUT
class	KItem;

enum enumAbrade
{
	enumAbradeAttack = 0,		//#tÊn c«ng
	enumAbradeDefend,			//#phßng thñ
	enumAbradeMove,				//#di chuyÓn
	enumAbradeNum,
};

typedef struct
{
	int		m_nIdx;
	int		m_nX;
	int		m_nY;
	int		m_nWidth;
	int		m_nHeight;
} TRADE_ITEM_INFO;

typedef struct
{
	int		nPriceScale;
	int		nMagicScale;
} REPAIR_ITEM_PARAM;

class KItemSet
{
private:
	DWORD			m_dwIDCreator;		
	KLinkArray		m_FreeIdx;			
	KLinkArray		m_UseIdx;			

public:
	int				m_nItemAbradeRate[enumAbradeNum][itempart_num];
	REPAIR_ITEM_PARAM	m_sRepairParam;
#ifdef _SERVER
	TRADE_ITEM_INFO		*m_psItemInfo;
	TRADE_ITEM_INFO		*m_psBackItemInfo;
	void			BackItemInfo();// copy m_psItemInfo to m_psBackItemInfo
	PlayerItem		m_sLoseItemFromEquipmentRoom[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];
	PlayerItem		m_sLoseEquipItem[itempart_num];
#endif

public:
	KItemSet();
	~KItemSet();
	void			Init();
	int				GetItemCount(IN int nItemGenre = -1);
	int				SearchID(IN DWORD dwID);
	BOOL      Get_SizeItem(IN int nItemGenre,
								  IN int nDetailType,
								   IN int nParticularType,
								   IN int nLevel,
								   IN OUT int* nWidth,
								   IN OUT int* nHeight
								   );
	int				AddI(KItem* pItem);
	//
	int				AddItemSet2(IN int nItemGenre, IN int nSeries, IN int nLevel, IN int nLuck, IN int nDetail = -1, 
		IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int nVersion = 0, IN UINT nRandomSeed = 0, 
		IN int nStackNum = 1, IN int nEnChance = 0,IN int nPoint = 0, IN int nYear = 0, 
		IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,IN int bLock=0, IN int sLock=0);
	//
	int				AddGoldItem(IN int nId , IN int* pnMagicLevel = NULL , IN int nSeries = 5, IN int nEnChance = 0,  
		IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,IN int bLock=0, IN int sLock=0);
	//
	void		   Remove(IN int nIdx);
	int				GetAbradeRange(IN int nType, IN int nPart);
	//
private:
	void		   SetID(IN int nIdx);
	int				FindFree();
};

extern KItemSet	ItemSet;
#endif
