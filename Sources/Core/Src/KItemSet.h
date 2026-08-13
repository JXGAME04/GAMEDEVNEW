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
	int		nGoldScale;
	int		nPlatinaScale;
	int		nWarningBaseline;
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
	static int genXOpt(int nLuck) {
		int xOpt = 0;

		
		if (nLuck < 0) {
			nLuck = 0; 
		}
		if (g_MaxOptMultiply < 1) {
			g_MaxOptMultiply = 1;
		}

		
		if (nLuck < 11) {
			return ::GetRandomNumber(1, 1);
		}

	
		int multiplierChance = 0;
		if (nLuck >= 41) {
			multiplierChance = 80;
		}
		else if (nLuck >= 31) {
			multiplierChance = 50;
		}
		else if (nLuck >= 21) {
			multiplierChance = 30;
		}
		else if (nLuck >= 11) {
			multiplierChance = 10;
		}

		
		int randChance = ::GetRandomNumber(1, 100);
		if (randChance <= multiplierChance) {
		
			xOpt = g_MaxOptMultiply;
		}
		else {
			
			if (g_MaxOptMultiply > 1) {
				xOpt = ::GetRandomNumber(1, g_MaxOptMultiply - 1);
			}
			else {
				xOpt = 1; 
			}
		}

		return xOpt;
	};



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
	int				Add(IN int nItemNature, IN int nItemGenre, IN int nSeries, IN int nLevel, IN int nLuck, IN int nDetail = -1, 
						IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int nVersion = 0, IN UINT nRandomSeed = 0,
						IN int nMaxOptMultiply = 1);
	//
	int				AddItemSet2(IN int nItemGenre, IN int nSeries, IN int nLevel, IN int nLuck, IN int nDetail = -1, 
		IN int nParticular = -1, IN int* pnMagicLevel = NULL, IN int nVersion = 0, IN UINT nRandomSeed = 0, 
		IN int nStackNum = 1, IN int nEnChance = 0,IN int nPoint = 0, IN int nYear = 0, 
		IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,IN int bLock=0, IN int sLock=0, IN int nMaxOptMultiply = 1);
	//
	int				AddGoldItem(IN int nId , IN int* pnMagicLevel = NULL , IN int nSeries = 5, IN int nEnChance = 0,  
								IN int nYear = 0, IN int nMonth = 0, IN int nDay = 0, IN int nHour = 0,IN int bLock=0, 
								IN int sLock=0, IN int nMaxOptMultiply = 1);
	//
	void		   Remove(IN int nIdx);
	int				GetAbradeRange(IN int nType, IN int nPart);
	int				UpgradePlatinaEquip(int Version, KItem* Item);
	//
private:
	void		   SetID(IN int nIdx);
	int				FindFree();
};

extern KItemSet	ItemSet;
#endif
