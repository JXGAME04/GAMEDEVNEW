#include "KCore.h"
#include "KItem.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
//#include "MyAssert.h"

KItemSet	ItemSet;
/*!*****************************************************************************
// Function		: KItemSet::KItemSet
// Purpose		: 
// Return		: 
// Comments		:
// Author		: Spe
*****************************************************************************/
KItemSet::KItemSet()
{
	m_dwIDCreator = 100;
	ZeroMemory(&m_sRepairParam, sizeof(REPAIR_ITEM_PARAM));
#ifdef _SERVER
	m_psItemInfo = NULL;
	m_psBackItemInfo = NULL;
#endif
}

KItemSet::~KItemSet()
{
#ifdef _SERVER
	if (m_psItemInfo)
		delete [] m_psItemInfo;
	m_psItemInfo = NULL;
	if (m_psBackItemInfo)
		delete [] m_psBackItemInfo;
	m_psBackItemInfo = NULL;
#endif
}

/*!*****************************************************************************
// Function		: KItemSet::Init
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemSet::Init()
{
	m_FreeIdx.Init(MAX_ITEM);
	m_UseIdx.Init(MAX_ITEM);

	for (int i = MAX_ITEM - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
	}
#ifdef _SERVER
	if (m_psItemInfo)
		delete [] m_psItemInfo;
	m_psItemInfo = NULL;
	m_psItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memset(this->m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
	if (m_psBackItemInfo)
		delete [] m_psBackItemInfo;
	m_psBackItemInfo = NULL;
	m_psBackItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memset(this->m_psBackItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
#endif
	KIniFile	IniFile;
	IniFile.Load(ITEM_ABRADE_FILE);
//	Repair price
	IniFile.GetInteger("Repair", "ItemPriceScale", 100, &m_sRepairParam.nPriceScale);
	IniFile.GetInteger("Repair", "MagicPriceScale", 10, &m_sRepairParam.nMagicScale);
	IniFile.GetInteger("Repair", "GoldPriceScale", 10, &m_sRepairParam.nGoldScale);
	IniFile.GetInteger("Repair", "PlatinaPriceScale", 10, &m_sRepairParam.nPlatinaScale);
	IniFile.GetInteger("Repair", "WarningBaseline", 10, &m_sRepairParam.nWarningBaseline);
//	Attack Wear
	IniFile.GetInteger("Attack", "Weapon", 256, &m_nItemAbradeRate[enumAbradeAttack][itempart_weapon]);
	IniFile.GetInteger("Attack", "Head", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_head]);
	IniFile.GetInteger("Attack", "Body", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_body]);
	IniFile.GetInteger("Attack", "Belt", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_belt]);
	IniFile.GetInteger("Attack", "Foot", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_foot]);
	IniFile.GetInteger("Attack", "Cuff", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_cuff]);
	IniFile.GetInteger("Attack", "Amulet", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_amulet]);
	IniFile.GetInteger("Attack", "Ring1", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_ring1]);
	IniFile.GetInteger("Attack", "Ring2", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_ring2]);
	IniFile.GetInteger("Attack", "Pendant", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_pendant]);
	IniFile.GetInteger("Attack", "Horse", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_horse]);
	IniFile.GetInteger("Attack", "Mask", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_mask]);
// Protection against wear and tear
	IniFile.GetInteger("Defend", "Weapon", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_weapon]);
	IniFile.GetInteger("Defend", "Head", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_head]);
	IniFile.GetInteger("Defend", "Body", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_body]);
	IniFile.GetInteger("Defend", "Belt", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_belt]);
	IniFile.GetInteger("Defend", "Foot", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_foot]);
	IniFile.GetInteger("Defend", "Cuff", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_cuff]);
	IniFile.GetInteger("Defend", "Amulet", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_amulet]);
	IniFile.GetInteger("Defend", "Ring1", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_ring1]);
	IniFile.GetInteger("Defend", "Ring2", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_ring2]);
	IniFile.GetInteger("Defend", "Pendant", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_pendant]);
	IniFile.GetInteger("Defend", "Horse", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_horse]);
// Mobile wear
	IniFile.GetInteger("Move", "Weapon", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_weapon]);
	IniFile.GetInteger("Move", "Head", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_head]);
	IniFile.GetInteger("Move", "Body", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_body]);
	IniFile.GetInteger("Move", "Belt", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_belt]);
	IniFile.GetInteger("Move", "Foot", 64, &m_nItemAbradeRate[enumAbradeMove][itempart_foot]);
	IniFile.GetInteger("Move", "Cuff", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_cuff]);
	IniFile.GetInteger("Move", "Amulet", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_amulet]);
	IniFile.GetInteger("Move", "Ring1", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_ring1]);
	IniFile.GetInteger("Move", "Ring2", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_ring2]);
	IniFile.GetInteger("Move", "Pendant", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_pendant]);
	IniFile.GetInteger("Move", "Horse", 64, &m_nItemAbradeRate[enumAbradeMove][itempart_horse]);

}

/*!*****************************************************************************
// Function		: KItemSet::SearchID
// Purpose		: 
// Return		: int 
// Argumant		: DWORD dwID
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemSet::SearchID(DWORD dwID)
{
	int nIdx = 0;

	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[nIdx].GetID() == dwID)
			break;
	}
	return nIdx;
	
}

BOOL KItemSet::Get_SizeItem(int nItemGenre,
							int nDetailType,
							int nParticularType,
							int nLevel,
							int* nWidth,
							int* nHeight
							)
{
	
	return ItemGen.Get_SizeItem(nItemGenre,
		nDetailType,
		nParticularType,
		nLevel,
		nWidth,
		nHeight
		);
}

int KItemSet::AddI(KItem* pItem)
{
	KASSERT(NULL != pItem);

	int i = FindFree();

	if (!i)
		return 0;

	Item[i] = *pItem;
#ifdef _SERVER
	SetID(i);
#endif
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	return i;
}

int KItemSet::AddGoldItem(IN int nId , IN int* pnMagicLevel , IN int nSeries,IN int nEnChance, 
						  int nYear, int nMonth, int nDay, int nHour, int bLock, int sLock, int nMaxOptMultiply)
{
	int i = FindFree();
	
	if (!i)
		return 0;
	
	KItem*	pItem = &Item[i];

	ItemGen.GetGoldItemByIndex(nId,pItem,pnMagicLevel,nSeries,nEnChance);
	/*int x = ::GetRandomNumber(1, g_MaxOptMultiply);*/
	pItem->SetMaxOptMultiply(nMaxOptMultiply);
#ifdef _SERVER
	SetID(i);
#endif
	pItem->SetExpTime(nYear,nMonth,nDay,nHour);
	pItem->SetPlayerItemLock(bLock);
	pItem->SetPlayerItemHLock(sLock);
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	return i;
}

int KItemSet::AddItemSet2(int nItemGenre, int nSeries, int nLevel, int nLuck, int nDetailType/*=-1*/, 
						  int nParticularType/*=-1*/, int* pnMagicLevel, int nVersion/*=0*/, 
						  UINT nRandomSeed, int nStackNum, int nEnChance, int nPoint, int nYear, int nMonth, int nDay, 
						  int nHour, int bLock, int sLock, int nMaxOptMultiply)
{
	int i = FindFree();
	
	if (i == 0)
		return 0;

	KItem*	pItem = &Item[i];
	// [LOREN 27/08] FindFree() tra ve mot khe DUNG LAI: m_GeneratorParam con
	// nguyen so lieu cua mon do truoc do. Truoc day vo hai vi Gen_* deu
	// ZeroMemory khoi nay, nhung Gen_MagicScript nay GIU LAI khoi do cho
	// nguyen lieu lo ren (de khong mat ma phep khi nap tu CSDL), nen rac se
	// chay thang vao vat pham moi va lam client sap luc ve mo ta.
	// Lam sach o day: duong nap tu CSDL (KPlayerDBFuns) khong di qua ham nay
	// nen van giu duoc so lieu that cua no.
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	// [LOREN 27/08] Ba mang thuoc tinh cung phai sach: Gen_MagicScript chi lam
	// `*pItem = *pMagicScript` (gan phan co ban tu bang), KHONG dung toi chung,
	// nen chung se giu so lieu cua mon do truoc do o khe nay. Duong ve mo ta
	// khoang 199..204 doc thang m_aryBaseAttrib[0].nValue[0] (KItem.cpp:1504).
	ZeroMemory(pItem->m_aryBaseAttrib, sizeof(pItem->m_aryBaseAttrib));
	ZeroMemory(pItem->m_aryRequireAttrib, sizeof(pItem->m_aryRequireAttrib));
	ZeroMemory(pItem->m_aryMagicAttrib, sizeof(pItem->m_aryMagicAttrib));
	pItem->m_GeneratorParam.nVersion = nVersion;
	pItem->m_GeneratorParam.uRandomSeed = nRandomSeed;
	switch(nItemGenre)
	{
	case item_equip:			
		ItemGen.Gen_Equipment(nDetailType, nParticularType, nSeries, nLevel, pnMagicLevel, nLuck, nVersion, pItem, nEnChance, nPoint);
		pItem->SetMaxOptMultiply(nMaxOptMultiply);
		break;
	case item_medicine:			
		ItemGen.Gen_Medicine(nDetailType, nParticularType, nLevel, nVersion, pItem, nStackNum);
		break;
	case item_mine:
		break;
	case item_materials:		
		break;
	case item_task:				
		ItemGen.Gen_Quest(nDetailType, pItem, nStackNum);
		break;
	case item_starstone:	// [PHI PHONG] Tinh Than Thach
		ItemGen.Gen_StarStone(nParticularType, pItem, nStackNum);
		break;

	case item_townportal:
		ItemGen.Gen_TownPortal(pItem);
		break;
	case item_magicscript:
		ItemGen.Gen_MagicScript(nDetailType, nParticularType, pItem, nLevel, nSeries, nLuck, nStackNum);
		break;
	default:
		printf("KItemSet::AddItemSet2 khong tim thay nItemGenre=%d \n", nItemGenre);
		_ASSERT(0);
		break;
	}
	
#ifdef _SERVER
	SetID(i);
#endif
	pItem->SetExpTime(nYear,nMonth,nDay,nHour);
	pItem->SetPlayerItemLock(bLock);
	pItem->SetPlayerItemHLock(sLock);
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	return i;
}

int KItemSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

void KItemSet::Remove(IN int nIdx)
{
	Item[nIdx].Remove();
	
	m_UseIdx.Remove(nIdx);
	m_FreeIdx.Insert(nIdx);
}

void KItemSet::SetID(IN int nIdx)
{
	Item[nIdx].SetID(m_dwIDCreator);
	m_dwIDCreator++;
}

#ifdef _SERVER
//---------------------------------------------------------------------------
//	¹¦ÄÜ£ºcopy m_psItemInfo to m_psBackItemInfo
//---------------------------------------------------------------------------
void	KItemSet::BackItemInfo()
{
	_ASSERT(this->m_psItemInfo);
	_ASSERT(this->m_psBackItemInfo);
	if (!m_psItemInfo)
		return;
	if (!m_psBackItemInfo)
		m_psBackItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memcpy(m_psBackItemInfo, this->m_psItemInfo, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
}
#endif

int KItemSet::GetAbradeRange(IN int nType, IN int nPart)
{
	if (nType < 0 || nType >= enumAbradeNum)
		return 0;
	if (nPart < 0 || nPart >= itempart_num)
		return 0;

	return m_nItemAbradeRate[nType][nPart];
}

/*!*********************************************************************************
// Function : KItemSet::Add
// Purpose :
// Return : int array number
// Argumant : int item type (equipment? medicine? ore?...)
// Argumant : int magic level (for equipment, it is general equipment, blue equipment, bright gold, etc....)
// Argumant : int five elements attribute
// Argumant : int level
// Argumant : int luck value
// Comments :
// Author : Spe
*************************************************************************/
int KItemSet::Add(IN int nItemNature, int nItemGenre, int nSeries,
	int nLevel, int nLuck, int nDetailType/*=-1*/,
	int nParticularType/*=-1*/, int* pnMagicLevel, int nVersion/*=0*/, UINT nRandomSeed, int nMaxOptMultiply)
{

	if (nItemGenre == item_equip)	
	{		// Equipment
		int i = FindFree();

		if (i == 0)
			return 0;

		KItem* pItem = &Item[i];
		pItem->m_GeneratorParam.nVersion = nVersion;
		pItem->m_GeneratorParam.uRandomSeed = nRandomSeed;

		ItemGen.Gen_Equipment(nItemNature, nDetailType, nParticularType, nSeries, nLevel, pnMagicLevel, nLuck, nVersion, pItem);
		pItem->SetMaxOptMultiply(nMaxOptMultiply);
	#ifdef _SERVER
		SetID(i);
	#endif
		m_FreeIdx.Remove(i);
		m_UseIdx.Insert(i);
		return i;
	}
	else
		return AddItemSet2(nItemGenre, nSeries, nLevel, nLuck, nDetailType,
			nParticularType, pnMagicLevel, nVersion,
			nRandomSeed);
}

int KItemSet::UpgradePlatinaEquip(int Version, KItem* Item) {
	return ItemGen.UpgradePlatinaEquip(Version, Item);
}