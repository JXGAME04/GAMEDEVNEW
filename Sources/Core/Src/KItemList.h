#ifndef	KItemListH
#define	KItemListH

#include "KLinkArray.h"
#include "KItem.h"

#define	MAX_ITEM_ACTIVE	2
#define	REQUEST_EQUIP_ITEM		1
#define	REQUEST_EAT_MEDICINE	2

typedef struct
{
	int		nPlace;
	int		nX;
	int		nY;
} ItemPos;

class CORE_API KItemList
{
private:
	int			m_PlayerIdx;
	int			m_Hand;
	int			m_HandSkill;
	int			m_nBackHand;
	int			m_EquipItem[itempart_num];
	int			m_TrembleItem[compoundpart_num];

	PlayerItem	m_sBackItems[MAX_PLAYER_ITEM];				
	static int	ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE];	
	static int	ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE];	
	KLinkArray	m_FreeIdx;
	KLinkArray	m_UseIdx;
	int				  m_nListCurIdx;								
	BOOL		 m_bActiveSet;
	BOOL		 m_nMaskLock;	//#mat na
#ifndef _SERVER
	BOOL		m_bLockOperation;
#endif
public:
	PlayerItem		  m_Items[MAX_PLAYER_ITEM];						
	KInventory		 m_Room[room_num];
	int					   FindSame(int nGameIdx);

#ifdef _SERVER
	BOOL        CheckItemInAll(int nIdx);//add by phong kiÒu antihack
	int 			  FindNumberInAll(int nIdx);
#endif	

private:
	int			FindFree();
	int			GetEquipPlace(int nType);						
	BOOL		Fit(int nIdx, int nPlace);						
	BOOL		Fit(KItem* pItem, int nPlace);
	int			GetEquipEnhance(int nPlace);					
	int			GetActiveEquipPlace(int nPlace, int nCount);	
	void		InfectionNextEquip(int nPlace, BOOL bEquip = FALSE);
	BOOL		FindSameDetailTypeInEquipment(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY);
	BOOL		FindSameDetailTypeInImmediacy(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY);
#ifdef _SERVER
	BOOL		FindSameToRemove(int nItemNature, int nItemGenre, int nDetailType, int nItemParticular, int nLevel, int nSeries, int Place, int *pnIdx);
#endif
	friend	class KPlayer;

public:
	KItemList();
	~KItemList();
	int			Init(int nIdx);
	int			GetEquipment(int nIdx) { return m_EquipItem[nIdx]; }
	int			GetActiveAttribNum(int nIdx);			
	int			GetWeaponType();							
	int			GetWeaponParticular();					
	void		GetWeaponDamage(int* nMin, int* nMax);		
	int			AddKIL(int nIdx, int nPlace, int nX, int nY, BOOL bInit = FALSE, BOOL bBreak = FALSE);	
	BOOL		SearchPosition(int nWidth, int nHeight, ItemPos* pPos, bool bOverLookHand = false);
	BOOL		Remove(int nIdx);							
	void		RemoveAll();
	BOOL		CanEquip(int nIdx, int nPlace = -1);		
	BOOL		CanEquip(KItem* pItem, int nPlace = -1);
	BOOL		EnoughAttrib(void* pData);
	int			HaveDamageItem(int nDur);
	BOOL		Equip(int nIdx, int nPlace = -1);				
	BOOL		UnEquip(int nIdx, int nPlace = -1);				
	BOOL		EatMecidine(int nIdx);							
	PlayerItem*	GetFirstItem();
	PlayerItem*	GetNextItem();
	int			SearchID(int nID);
	void		ExchangeMoney(int nSrcRoom, int DesRoom, int nMoney);
	void		ExchangeItem(ItemPos* SrcPos,ItemPos* DesPos);
	BOOL	SetLockItem(int ItemIdx, int lock);
	BOOL	RemoveItem_YearExp(int ItemIdx);
	int			GetMoneyAmount();					
	int			GetEquipmentMoney();			
	int			GetTradeMoney();				
	BOOL		AddMoney(int nRoom, int nMoney);
	BOOL		CostMoney(int nMoney);
	BOOL		DecMoney(int nMoney);
	void		SetMoney(int nMoney1, int nMoney2, int nMoney3);
	void		SetRoomMoney(int nRoom, int nMoney);
	DWORD			GetMoney(int nRoom) { return m_Room[nRoom].GetMoney(); }	
	int			GetXu(int nRoom) { return m_Room[nRoom].GetXu(); }
	void		SetXu(int nXu);
	void		SetRoomXu(int nRoom, int nXu);
	BOOL		AddXu(int nRoom, int nXu);
	BOOL		CostXu(int nXu);
	BOOL		DecXu(int nXu);
	int			GetXuAmount();					
	int			GetEquipmentXu();
	void		SetPlayerIdx(int nIdx);				
	int			Hand() { return m_Hand; };
	void		ClearRoom(int nRoom);
	void	    ClearAll();	//míi thªm vµo sau nµy
	void		BackupTrade();
	void		RecoverTrade();
	void		StartTrade();
	int			GetItemNum(int nGenre, int nDetailType, int nParticular, int nLevel); // dem so item cung loai
	int			CountCommonItem(int nItemNature, int nItemGenre, int nDetailType = -1, int nParticularType = -1, int nLevel = -1, int nSeries = -1,  int Place = pos_equiproom); //#edit by Fong Kieu 07/06/2021
	int			CalcFreeItemCellCount(int nWidth, int nHeight, int nRoom);
	void		BackupGive();
	void		RecoverGive();
	void		StartGive();
	void		RecoverItem(int nPos);
	BOOL		CheckTrembleItem(int nIdx, int nPlace = -1);	
	void		UnTrembleItem(int nIdx, int nPlace = -1);					
	int			GetTrembleItem(int nIdx) { return m_TrembleItem[nIdx]; }
	BOOL		GetIfActive();
	BOOL		GetMaskLock() {return m_nMaskLock;};	//#mat na
	void		SetMaskLock(BOOL bFlag);// mat na
#ifdef	_SERVER
	void		Abrade(int nType, BOOL isDeathPunish); //#mai mon
	void		TradeMoveMoney(int nMoney);	
	void		SendMoneySync();		
	BOOL		IsItemExist(int nGern,int nDetailType,int nPar,int nSerise = 5,int nLevel = 0);
	BOOL		DelExistItem(int nGern,int nDetailType,int nPar,int nSerise = 5,int nLevel = 0);
	BOOL		IsTaskItemExist(int nDetailType, BYTE bType = 1);
	int			      GetTaskItemNum(int nDetailType, BYTE bType = 1 );
	BOOL		RemoveTaskItem(int nDetailType);
	BOOL		RemoveMineItem(int nDetailType);
	BOOL		RemoveGoldItem(int nDetailType);
	int				  RemoveCommonItem(int nCount, int nItemNature, int nItemGenre = -1, int nDetailType = -1, int nItemParticular = -1, int nLevel = -1, int nSeries = -1, int Place = pos_equiproom);
	BOOL		IsGoldItemExist(int nDetailType);
	void		    GetTradeRoomItemInfo();		
	BOOL		TradeCheckCanPlace();			
	BOOL		CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnX, int *pnY);
	BOOL		EatMecidine(int nPlace, int nX, int nY);		
	BOOL		AutoMoveMedicine(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY);
	void		AutoLoseItemFromEquipmentRoom(int nRate);
	BOOL		RemoveItemIdx(int nGameIdx, int nNum);
	void		AutoLoseEquip();
	void		SetLevelItem(int nIdx, int nLevel);
	void		SetSeriesItem(int nIdx, int nSeries);
	void		ChangeSpiritItem(int nIdx, int nLevelMagic);
	void		SetPointPurpleItem(int nIdx, int nPoint);
	void		SetMagic2Item(int nIdx, int nType, int nOption = 0, int nLevel = 1);
	void		SetPrice(int nIdx, int nPrice);
	void		SetPriceFromScript( int nIdx, int nPrice);//#fix bay ban trang bi khoa bao hiem
	int			GetPrice(int nGameIdx);
	void		CheckItemTime();
	void		SyncItem(int nIdx, int nPlace = 0, int nX = 0, int nY = 0, int nPlayerIndex = 0, bool m_bIsNew = false);
	void		InsertEquipment(int nIdx, bool bAutoStack = false);
#endif
	int			PositionToIndex(int P, int i);
#ifndef	_SERVER
	int				 UseItem(int nIdx);					
	int				 ChangeItemInPlayer(int nIdx);//edit by phong kieu mac trang bi vao nguoi
	BOOL		AutoMoveItem(ItemPos SrcPos,ItemPos DesPos);
	void			MenuSetMouseItem();
	void			RemoveAllInOneRoom(int nRoom);
	void			LockOperation();										
	void			UnlockOperation();
	BOOL		IsLockOperation() { return m_bLockOperation; };
	int				 GetSameDetailItemNum(int nImmediatePos);
	int				 GetGoldColor(int nSet,int nId);
	BOOL		AutoUseItem(int nGenre, int nDetailType, int nParticular, int nPlayerIndex);	
	BOOL		AutoCheckItem(int nGenre, int nDetailType, int nParticular);
	BOOL		IsTaskItemExist(int nDetailType);
#endif
};
#endif
