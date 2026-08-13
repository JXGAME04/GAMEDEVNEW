#include	"KCore.h"
#include	"MyAssert.H"
#include	"KItem.h"
#include	"KItemSet.h"
#include	"KNpc.h"
#include	"KMath.h"
#include	"KPlayer.h"
#include	"KItemList.h"
#include	"KPlayerSet.h"
#include	"KItemChangeRes.h"
#include	<time.h>
#include    "KSG_StringProcess.h"
#ifdef _SERVER
//#include	"../../Headers/IServer.h"
#include	"KObjSet.h"
#endif
#ifndef _SERVER
#include	"CoreShell.h"
#include	"../../Headers/IClient.h"
#endif

#define		defEQUIP_POWER
#ifdef defEQUIP_POWER
	int		g_nEquipPower[itempart_num] =
	{2, 4, 2, 4, 1, 1, 1, 1, 1, 1, 0};
#endif

int KItemList::ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE] = 
{
	{ itempart_foot, itempart_ring1	},	//	itempart_head = 0,
	{ itempart_weapon, itempart_head },	//	itempart_body,
	{ itempart_body, itempart_amulet },	//	itempart_belt,
	{ itempart_ring1, itempart_foot },	//	itempart_weapon,
	{ itempart_cuff, itempart_pendant },//	itempart_foot,
	{ itempart_belt, itempart_ring2 },	//	itempart_cuff,
	{ itempart_head, itempart_weapon },	//	itempart_amulet,
	{ itempart_cuff, itempart_pendant },//	itempart_ring1,
	{ itempart_amulet, itempart_body },	//	itempart_ring2,
	{ itempart_belt, itempart_ring2 },	//	itempart_pendant,
	{ itempart_horse, itempart_horse },	//	itempart_horse,
	{ itempart_mask, itempart_mask },	//	itempart_mask,
	{ itempart_mantle, itempart_mantle },	//	itempart_fifong,
	{ itempart_signet, itempart_signet },	//	itempart_signet,
	{ itempart_shipin, itempart_shipin },		//	itempart_shipin,
	{ itempart_hoods, itempart_hoods },//	itempart_hoods,
	{ itempart_cloak, itempart_cloak },//	itempart_cloak,
};

int KItemList::ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE] =
{
	{ itempart_body, itempart_amulet },	//	itempart_head = 0,
	{ itempart_ring2, itempart_belt },	//	itempart_body,
	{ itempart_pendant, itempart_cuff },//	itempart_belt,
	{ itempart_amulet, itempart_body },	//	itempart_weapon,
	{ itempart_weapon, itempart_head },	//	itempart_foot,
	{ itempart_foot, itempart_ring1 },	//	itempart_cuff,
	{ itempart_belt, itempart_ring2 },	//	itempart_amulet,
	{ itempart_weapon, itempart_head },	//	itempart_ring1,
	{ itempart_cuff, itempart_pendant },//	itempart_ring2,
	{ itempart_foot, itempart_ring1 },	//	itempart_pendant,
	{ itempart_horse, itempart_horse },	//	itempart_horse,
	{ itempart_mask, itempart_mask },	//	itempart_mask,
	//{ itempart_fifong, itempart_fifong },	//	itempart_fifong,
	{ itempart_mantle, itempart_mantle },//	itempart_mantle,
	{ itempart_signet, itempart_signet },	//	itempart_signet,
	{ itempart_shipin, itempart_shipin },		//	itempart_shipin,
	{ itempart_hoods, itempart_hoods },//	itempart_hoods,
	{ itempart_cloak, itempart_cloak },//	itempart_cloak,
};

int KItemList::CountItemInAll()
{
	// Count all items in the player's item list
	return m_nItemsCount;
}

KItemList::KItemList()
{
	m_PlayerIdx = 0;
	m_nListCurIdx = 0;
	m_nItemsCount = 0;
}

KItemList::~KItemList()
{

}

int KItemList::GetWeaponType()
{
	if (m_EquipItem[itempart_weapon])
		return Item[m_EquipItem[itempart_weapon]].GetDetailType();
	else
		return -1;
}

void KItemList::GetWeaponDamage(int* nMin, int* nMax)
{
	int nWeaponIdx = m_EquipItem[itempart_weapon];
	if (nWeaponIdx)
	{
		_ASSERT(Item[nWeaponIdx].m_CommonAttrib.nItemGenre == item_equip 
			&& (Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_meleeweapon
			|| Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_rangeweapon));
		int nMinDamage, nMaxDamage, nEnhance;
		int nDamageMinBase = Item[nWeaponIdx].m_aryBaseAttrib[0].nValue[0];
		int	nDamageMaxBase = Item[nWeaponIdx].m_aryBaseAttrib[1].nValue[0];
		nMinDamage = 0;
		nMaxDamage = 0;
		nEnhance = 0;
		for (int i = 0; i < 6; i++)
		{
			switch(Item[nWeaponIdx].m_aryMagicAttrib[i].nAttribType)
			{
			case magic_weapondamagemin_v:
				nMinDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamagemax_v:
				nMaxDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamageenhance_p:
				nEnhance += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			default:
				break;
			}
		}
		*nMin = (nDamageMinBase + nMinDamage) * (100 + nEnhance) / 100;
		*nMax = (nDamageMaxBase + nMaxDamage) * (100 + nEnhance) / 100;
	}
	else	
	{
		/*
		int nDamageBase = Player[m_PlayerIdx].m_nCurStrength * Player[m_PlayerIdx].m_nCurDexterity;
		*nMin = nDamageBase >> 9;
		*nMax = nDamageBase >> 8;
		*/
		// by Spe 03/06/11
		_ASSERT(STRENGTH_SET_DAMAGE_VALUE > 0);
		*nMin = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
		*nMax = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE + 1;
	}
}

BOOL KItemList::SearchPosition(int nWidth, int nHeight, ItemPos* pPos, bool bOverLookHand)
{
	if (nWidth < 0 || nHeight < 0 || NULL == pPos)
	{
		return FALSE;
	}

	POINT	pPt;
	if (!m_Room[room_equipment].FindRoom(nWidth, nHeight, &pPt))
	{
		if((Player[m_PlayerIdx].m_dwEquipExpandTime - KSG_GetCurSec() > 0) && m_Room[room_equipmentex].FindRoom(nWidth, nHeight, &pPt))
		{
			pPos->nPlace = pos_equiproomex;
			pPos->nX = pPt.x;
			pPos->nY = pPt.y;
		}
		else if (!bOverLookHand)
		{
			if (0 != m_Hand)
			{
				return FALSE;
			}
			pPos->nPlace = pos_hand;
			pPos->nX = 0;
			pPos->nY = 0;
		}
		else
			return FALSE;
	}
	else
	{
		pPos->nPlace = pos_equiproom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
	}
	return TRUE;
}

int KItemList::AddKIL(int nIdx, int nPlace, int nX, int nY, BOOL bInit, BOOL bBreak)
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return 0;

	if (Item[nIdx].GetID() == 0)
		return 0;

#ifdef _SERVER
	if (Item[nIdx].CanStack() == TRUE && bBreak)
	{
		int pnX, pnY, pnIdx = 0;
		if (Player[m_PlayerIdx].m_ItemList.m_Room[room_equipment].FindSameItemToSort(nIdx, &pnIdx, &pnX, &pnY))
		{
			int a = Item[nIdx].GetStackNum();
			int b = Item[pnIdx].GetStackNum();
			int Total = a+b;
			if (Total>Def_MAX_STACK_TIENDONG)
			{
				Item[pnIdx].SetStackNum(Def_MAX_STACK_TIENDONG);
				this->SyncItem(pnIdx);
				Item[nIdx].SetStackNum(Total - Def_MAX_STACK_TIENDONG);
			}
			else
			{
				nX = pnX;
				nY = pnY;
				Item[nIdx].SetStackNum(Total);
				Player[m_PlayerIdx].m_ItemList.Remove(pnIdx);
				ItemSet.Remove(pnIdx);
			}

		}
	}
#endif

	int i = FindFree();
	if (!i)
		return 0;

	switch(nPlace)
	{
	case pos_hand:
		if (m_Hand)
			return 0;
		m_Items[i].nPlace = pos_hand;
		m_Items[i].nX = 0;
		m_Items[i].nY = 0;
		m_Hand = nIdx;
		break;
	case pos_equip:
		if (nX < 0 || nX >= itempart_num)
			return 0;
		if (m_EquipItem[nX])
			return 0;
		m_Items[i].nPlace = pos_equip;
		m_Items[i].nX = nX;
		m_Items[i].nY = 0;
		break;
	case pos_equipback:
		if (nX < 0 || nX >= itempart_num)
			return 0;
		if (m_AltEquipmentItem[nX])
			return 0;
		m_Items[i].nPlace = pos_equipback;
		m_Items[i].nX = nX;
		m_Items[i].nY = 0;
		break;
	case pos_equiproom://xu ly xep chong item cho nay
		if (!m_Room[room_equipment].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_equiproom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
#ifndef _SERVER
	case pos_trade1:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			return 0;
		if (!m_Room[room_trade1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_trade1;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
#endif
	case pos_repositoryroom:
		if (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox1room: // ruong mo rong 1
		if (!m_Room[room_exbox1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_exbox1room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox2room: // ruong mo rong 2
		if (!m_Room[room_exbox2].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_exbox2room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_exbox3room: // ruong mo rong 3
		if (!m_Room[room_exbox3].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_exbox3room;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_equiproomex:  // mo rong hanh trang
		if (!m_Room[room_equipmentex].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_equiproomex;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
	case pos_immediacy:
		if (!m_Room[room_immediacy].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_immediacy;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;		
	case pos_give:
		if (!m_Room[pos_give].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_give;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_affairitem:
		if (!m_Room[room_affairitem].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_affairitem;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_tremble:
		if (nX < 0 || nX >= tremblepart_num)
			return 0;
		if (m_TrembleItem[nX])
			return 0;
		m_Items[i].nPlace = pos_tremble;
		m_Items[i].nX = nX;
		m_Items[i].nY = 0;
		break;
	default:
		return 0;
	}

	m_Items[i].nIdx = nIdx;
	m_Items[i].nPrice = Item[nIdx].m_CommonAttrib.uPrice;
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);

	if (m_Items[i].nPlace == pos_equip)
	{
		Equip(m_Items[i].nIdx, nX);
	}

	if (m_Items[i].nPlace == pos_equipback)
	{
		//restore alternative equip set
		//int nItemListIdx = FindSame(m_Items[i].nIdx);
		m_AltEquipmentItem[nX] = m_Items[i].nIdx;
		//m_Items[nItemListIdx].nPlace = pos_equipback;
		//m_Items[nItemListIdx].nX = nX;
		//m_Items[nItemListIdx].nY = 0;
	}

	if (m_Items[i].nPlace == pos_tremble)		
	{
		CheckTrembleItem(m_Items[i].nIdx, nX);
	}
	//
#ifdef _SERVER
	this->SyncItem(nIdx, m_Items[i].nPlace, m_Items[i].nX, m_Items[i].nY, m_PlayerIdx, true);
//	if (!bInit)
//		Player[m_PlayerIdx].m_uMustSave = SAVE_REQUEST;
#endif

	m_nItemsCount++;
#ifndef _SERVER
	KUiObjAtContRegion	pInfo;

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,	// mat na
		UIEP_FIFONG, //#phi phong
		UIEP_SIGNET,
		UIEP_SHIPIN,
		UIEP_HOODS,
		UIEP_CLOAK,

	};

	int PartTrembleConvert[tremblepart_num] = 
	{
		UIEP_BLUEITEM,
		UIEP_GEMLEVEL,
		UIEP_GEMSPIRIT,
		UIEP_GEMMETAL,
		UIEP_GEMWOOD,
		UIEP_GEMWATER,
		UIEP_GEMFIRE,
		UIEP_GEMEARTH,
	};

	pInfo.Obj.uGenre = CGOG_ITEM;	//源装备
	pInfo.Obj.uId = nIdx;
	pInfo.Region.Width = Item[nIdx].GetWidth();
	pInfo.Region.Height = Item[nIdx].GetHeight();

	switch(nPlace)
	{
	case pos_immediacy:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	case pos_hand:
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_equiproom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_exbox1room: // ruong mo rong 1
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX1;
		break;
	case pos_exbox2room: // ruong mo rong 2
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX2;
		break;
	case pos_exbox3room: // ruong mo rong 3
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_EX_BOX3;
		break;
	case pos_equiproomex: // mo rong hanh trang
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_EX;
		break;
	case pos_trade1:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		break;
	case pos_affairitem:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_AFFAIR_ITEM;
		break;
	case pos_tremble: 
		pInfo.Region.h = 0;
		pInfo.Region.v = PartTrembleConvert[nX];
		pInfo.eContainer = UOC_TREMBLE_ITEM;
		break;
	}
	if (nPlace != pos_equipback) {
		if (nPlace != pos_trade1)
			CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 1);
		else
		{
			CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 1);
			CoreDataChanged(GDCNI_GAMBLE_DESIRE_ITEM, (DWORD)&pInfo, 1);
		}
	}

#ifdef _DEBUG
	int nLoopIdx = 0;
	nLoopIdx = m_UseIdx.GetNext(nLoopIdx);
	//g_DebugLog("[ITEM]Item Begin");
	while(nLoopIdx)
	{
	//	g_DebugLog("[ITEM]ItemListIdx:%d, Item:%d, ItemId:%d", nLoopIdx, m_Items[nLoopIdx].nIdx, Item[m_Items[nLoopIdx].nIdx].GetID());
		nLoopIdx = m_UseIdx.GetNext(nLoopIdx);
	}
#endif

#endif
	return i;
}

/*!*****************************************************************************
// Function		: KItemList::Remove
// Purpose		: 玩家失去一个装备
// Return		: int 
// Argumant		: int nGameIdx为游戏世界中道具数组的编号
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Remove(int nGameIdx)
{
	if (!nGameIdx)
		return FALSE;

	int nIdx = FindSame(nGameIdx);

	if (!nIdx)
		return FALSE;

#ifdef _SERVER
	ITEM_REMOVE_SYNC	sRemove;
	sRemove.ProtocolType = s2c_removeitem;
	sRemove.m_ID = Item[nGameIdx].m_dwID;
#endif

	switch(m_Items[nIdx].nPlace)
	{
	case pos_hand:
		m_Hand = 0;
		break;
	case pos_equip:
		UnEquip(m_Items[nIdx].nIdx);
		break;
	case pos_immediacy:
		m_Room[room_immediacy].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_equiproom:
		m_Room[room_equipment].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom:
		m_Room[room_repository].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_exbox1room: // ruong mo rong 1
		m_Room[room_exbox1].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_exbox2room: // ruong mo rong 2
		m_Room[room_exbox2].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_exbox3room: // ruong mo rong 3
		m_Room[room_exbox3].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_equiproomex: // mo rong hanh trang
		m_Room[room_equipmentex].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_traderoom:
	case pos_gambleroom:
		m_Room[room_trade].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_give:
		m_Room[room_give].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
#ifndef _SERVER
	case pos_trade1:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
		{
			_ASSERT(0);
		}
		m_Room[room_trade1].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
#endif
	case pos_affairitem:
		m_Room[room_affairitem].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_tremble:
		UnTrembleItem(m_Items[nIdx].nIdx);
		break;
	default:
		return FALSE;
	}
#ifndef _SERVER
	ItemSet.Remove(m_Items[nIdx].nIdx);

	KUiObjAtContRegion pInfo;

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,	// mat na
		UIEP_FIFONG, //#phi phong
		UIEP_SIGNET,
		UIEP_SHIPIN,
		UIEP_HOODS,
		UIEP_CLOAK,
	};

	int PartTrembleConvert[tremblepart_num] = 
	{
		UIEP_BLUEITEM,
		UIEP_GEMLEVEL,
		UIEP_GEMSPIRIT,
		UIEP_GEMMETAL,
		UIEP_GEMWOOD,
		UIEP_GEMWATER,
		UIEP_GEMFIRE,
		UIEP_GEMEARTH,
	};

	pInfo.Obj.uGenre = CGOG_ITEM;
	pInfo.Obj.uId = m_Items[nIdx].nIdx;
	pInfo.Region.Width = Item[m_Items[nIdx].nIdx].GetWidth();
	pInfo.Region.Height = Item[m_Items[nIdx].nIdx].GetHeight();

	switch(m_Items[nIdx].nPlace)
	{
	case pos_hand:
		pInfo.Obj.uGenre = CGOG_NOTHING;
		pInfo.Region.h = 0;
		pInfo.Region.v = 0;
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equiproom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_exbox1room: // ruong mo rong 1
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_EX_BOX1;
		break;
	case pos_exbox2room: // ruong mo rong 2
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_EX_BOX2;
		break;
	case pos_exbox3room: // ruong mo rong 3
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_EX_BOX3;
		break;
	case pos_equiproomex: // mo rong hanh trang
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_ITEM_EX;
		break;
	case pos_immediacy:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[m_Items[nIdx].nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_trade1:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		break;
	case pos_affairitem:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_AFFAIR_ITEM;
		break;
	case pos_tremble: 
		pInfo.Region.h = 0;
		pInfo.Region.v = PartTrembleConvert[m_Items[nIdx].nX];
		pInfo.eContainer = UOC_TREMBLE_ITEM;
		break;
	}
	if (m_Items[nIdx].nPlace != pos_trade1)
	{
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 0);
	}
	else
	{
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 0);
		CoreDataChanged(GDCNI_GAMBLE_DESIRE_ITEM, (DWORD)&pInfo, 0);
	}
#endif
	m_Items[nIdx].nIdx = 0;
	m_Items[nIdx].nPlace = 0;
	m_Items[nIdx].nX = 0;
	m_Items[nIdx].nY = 0;
	m_Items[nIdx].nPrice = 0;
	m_FreeIdx.Insert(nIdx);
	m_UseIdx.Remove(nIdx);

#ifdef _SERVER
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
	//Player[m_PlayerIdx].m_uMustSave = SAVE_REQUEST;
#endif
	m_nItemsCount--;
	return TRUE;
}

int KItemList::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

#ifdef _SERVER
BOOL  KItemList::CheckItemInAll(int nIdx)//add by phong ki襲 antihack
{
	int nNumberArray = FindNumberInAll(nIdx);
	if (!nNumberArray)
		return FALSE;

	int nWidth = Item[nIdx].GetWidth();
	int nHeight = Item[nIdx].GetHeight();

	if (nWidth <= 0 || nHeight <= 0)
		return FALSE;

	if (nNumberArray != (nWidth * nHeight))
		return FALSE;

	return TRUE;
}

int KItemList::FindNumberInAll(int nIdx)
{
	if (nIdx <= 0 && nIdx >= MAX_ITEM)
		return 0;

	int nWidth = Item[nIdx].GetWidth();
	int nHeight = Item[nIdx].GetHeight();

	if (nWidth <= 0 || nHeight <= 0)
		return 0;

	int nNumberArray = 0;

	nNumberArray += m_Room[room_equipment].FindNumberArrayItem(nIdx);
	nNumberArray += m_Room[room_repository].FindNumberArrayItem(nIdx);
	nNumberArray += m_Room[room_give].FindNumberArrayItem(nIdx);
	nNumberArray += m_Room[room_trade].FindNumberArrayItem(nIdx);
	nNumberArray += m_Room[room_immediacy].FindNumberArrayItem(nIdx) * nWidth * nHeight;
	nNumberArray += m_Room[room_affairitem].FindNumberArrayItem(nIdx);
	if (m_Hand == nIdx)
		nNumberArray += nWidth * nHeight;

	for (int i=0;i<itempart_num;i++)
	{
		if (m_EquipItem[i] > 0 && m_EquipItem[i] == nIdx)
		{
			nNumberArray += nWidth * nHeight;
		}
	}

	/*for (int j=0;j<MAX_PGBOX_ITEM;j++)//fix by phong ki襲 ch璦 ho祅 thi謓 antihack
	{
		if (m_PGBoxItem[j] > 0 && m_PGBoxItem[j] == nIdx)
		{
			nNumberArray += nWidth * nHeight;
		}
	}*/
	return nNumberArray;
}
#endif

int KItemList::FindSame(int nGameIdx)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		
		if (m_Items[nIdx].nIdx == nGameIdx)
			return nIdx;
	}
	return 0;
}

int KItemList::FindSame(DWORD dwID)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;

		if (Item[m_Items[nIdx].nIdx].GetID() == dwID)
			return nIdx;
	}
	return 0;
}


BOOL KItemList::Init(int nPlayerIdx)
{
	m_PlayerIdx = nPlayerIdx;
	m_Hand = 0;
	m_HandSkill = 0;
	m_nBackHand = 0;
	m_nMaskLock = FALSE;	//#mat na
	// TODO: Maybe we can read size from ini file.
//	int nWidth = 6;
//	int nHeight = 10;
//	m_Room[room_equipment].Init(nWidth, nHeight);
//	m_Room[room_repository].Init(nWidth, nHeight);
//	m_Room[room_trade].Init(nWidth, nHeight);
//	m_Room[room_tradeback].Init(nWidth, nHeight);
	m_Room[room_equipment].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_equipmentback].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_repository].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_trade].Init(TRADE_ROOM_WIDTH, TRADE_ROOM_HEIGHT);
	m_Room[room_exbox1].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT); // ruong mo rong 1
	m_Room[room_exbox2].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT); // ruong mo rong 2
	m_Room[room_exbox3].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT); // ruong mo rong 3
	m_Room[room_equipmentex].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT); // ruong hanh trang
	m_Room[room_tradeback].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_immediacy].Init(IMMEDIACY_ROOM_WIDTH, IMMEDIACY_ROOM_HEIGHT);
	m_Room[room_give].Init(EQUIPMENT_ROOM_WIDTH, TRADE_ROOM_HEIGHT);
	m_Room[room_giveback].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_affairitem].Init(AFFAIRITEM_ROOM_WIDTH, AFFAIRITEM_ROOM_HEIGHT);
#ifndef _SERVER
	m_Room[room_trade1].Init(TRADE_ROOM_WIDTH, TRADE_ROOM_HEIGHT);		// 这个的大小必须与 room_trade 的大小一样
#endif
	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));				// 玩家装备的道具（对应游戏世界中道具数组的索引）
	ZeroMemory(m_Items, sizeof(m_Items));						// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	ZeroMemory(m_AltEquipmentItem, sizeof(m_AltEquipmentItem));
	m_nListCurIdx = 0;											// GetFirstItem GetNextItem
	
	m_FreeIdx.Init(MAX_PLAYER_ITEM);
	m_UseIdx.Init(MAX_PLAYER_ITEM);

	for (int i = MAX_PLAYER_ITEM - 1; i > 0 ; i--)
	{
		m_FreeIdx.Insert(i);
	}
	return TRUE;
}

BOOL KItemList::CanEquip(int nIdx, int nPlace)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0 || nIdx >= MAX_ITEM || Item[nIdx].GetGenre() != item_equip)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if (nPlace != -1 && !Fit(nIdx, nPlace))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)Item[nIdx].GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData))
			return FALSE;
		nCount++;
	} while (pData != NULL);
	return TRUE;
}

BOOL KItemList::CanEquip(KItem* pItem, int nPlace /* = -1 */)
{
	if (m_PlayerIdx <= 0 || !pItem)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if (nPlace != -1 && !Fit(pItem, nPlace))
	{
		return FALSE;
	}

	if(pItem->GetDurability() == 0)//#do ben trang bi hong
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)pItem->GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData))
			return FALSE;
		nCount++;
	} while (pData != NULL);
	return TRUE;
}

BOOL KItemList::EnoughAttrib(void* pAttrib)
{
	KMagicAttrib*	pData = (KMagicAttrib *)pAttrib;
	_ASSERT(pData);
	switch(pData->nAttribType)
	{
	case magic_requirestr:
		if (Player[m_PlayerIdx].m_nCurStrength < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiredex:
		if (Player[m_PlayerIdx].m_nCurDexterity < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirevit:
		if (Player[m_PlayerIdx].m_nCurVitality < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requireeng:
		if (Player[m_PlayerIdx].m_nCurEngergy < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirelevel:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Level < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiremenpai:
		if (Player[m_PlayerIdx].m_cFaction.m_nFirstAddFaction != pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requireseries:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Series != pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiresex:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_nSex != pData->nValue[0])
		{
			return FALSE;
		}
		break;
	default:
		break;
	}
	return TRUE;
}

int KItemList::HaveDamageItem(int nDur)
{
	int nIndex = 0;
	for (int i = 0; i < itempart_num; i ++)
	{
		nIndex = GetEquipment(i);
		if (Item[nIndex].GetDurability() >= 0 && 
			Item[nIndex].GetMaxDurability() > 0 && 
			Item[nIndex].GetDurability() < nDur)
		{
			return nIndex;
		}
	}
	return 0;
}

/*!*****************************************************************************
// Function		: KItemList::Equip
// Purpose		: 
// Return		: BOOL
// Argumant		: int nIdx，游戏世界中的道具数组编号，要求一定是装备类道具
// Argumant		: int nPlace，身上装备的位置，-1自动找位置
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Equip(int nIdx, int nPlace /* = -1 */)
{
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (m_PlayerIdx <= 0 || nIdx <= 0 || nNpcIdx <= 0 || item_equip != Item[nIdx].GetGenre())
		return FALSE;

	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		_ASSERT(0);
		return FALSE;
	}

	int nEquipPlace = nPlace;
	if (-1 == nEquipPlace)
	{
		nEquipPlace = GetEquipPlace(Item[nIdx].GetDetailType());
	}
	else if (!Fit(nIdx, nEquipPlace))
	{
		return FALSE;
	}
	int nCurIdx = 0;
	int nType = 0;
	// 换装
	switch(nEquipPlace)
	{
	case itempart_head:
		{
			if (Item[nIdx].GetGoldId())
				nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
			if (nType)
				Npc[nNpcIdx].m_HelmType = nType;
			else if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD || Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
			else
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
		}
		break;
	case itempart_body:
		{
			if (Item[nIdx].GetGoldId())
			{
				nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
			}
			if (nType)
				Npc[nNpcIdx].m_ArmorType = nType;
			else if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD || Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
				Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
			else
				Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
			nCurIdx = GetEquipment(itempart_mantle); //#phi phong
			if (nCurIdx <= 0)
				Npc[nNpcIdx].m_MantleType = Item[nIdx].GetMantle();
		}
		break;
	case itempart_weapon:
		if (Item[nIdx].GetGoldId())
		{
			nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
		}
		if (Item[nIdx].GetItemGlowLight() > 0)
		{
			nType = Item[nIdx].GetItemGlowLight();
		}

		if (nType)
			Npc[nNpcIdx].m_WeaponType = nType;
		else if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetPlatinaItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow(), 1);
		else
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
		break;
	//case itempart_mask:
	//	if (!m_nMaskLock)
	//	{
	//		if (Npc[nNpcIdx].m_bMaskFeature)
	//		{
	//			Npc[nNpcIdx].m_MaskType = 0;
	//		}
	//		else
	//		{
	//			if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
	//				Npc[nNpcIdx].m_MaskType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
	//			else
	//				g_MaskChangeRes.GetInteger(Item[nIdx].GetParticular() + 2, 2, 0, &Npc[nNpcIdx].m_MaskType);
	//		}
	//	}
	//	break;
	case itempart_mantle:
		Npc[nNpcIdx].m_byMantleLevel = Item[nIdx].GetLevel();
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD || Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
			Npc[nNpcIdx].m_MantleType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_MantleType = 0;
		break;
	case itempart_hoods:
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD || Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHoodsRes(Item[nIdx].GetParticular());
		break;
	case itempart_cloak:
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD || Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetCloakRes(Item[nIdx].GetParticular());
		break;
	case itempart_horse:
		{
			if (Item[nIdx].GetGoldId())
				nType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetGoldId());
			//
			if (nType)
				Npc[nNpcIdx].m_HorseType = nType;
			else if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD || Item[nIdx].m_CommonAttrib.nItemNature == NATURE_PLATINA)
				Npc[nNpcIdx].m_HorseType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow()); \
			else
				Npc[nNpcIdx].m_HorseType = g_ItemChangeRes.GetHorseRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
			//
			if (Npc[nNpcIdx].m_HorseType >= 0)
				Npc[nNpcIdx].m_bRideHorse = TRUE;
			else
				Npc[nNpcIdx].m_bRideHorse = FALSE;
		}
		break;
	case itempart_mask://#mat na
		{
			if (!m_nMaskLock)
			{
				Npc[nNpcIdx].m_MaskType = Item[nIdx].GetBaseMagic();
			}
		}	
		break;
	default:
		break;
	}
	// 更新装备自身坐标
	m_EquipItem[nEquipPlace] = nIdx;
	m_Items[nItemListIdx].nPlace = pos_equip;
	m_Items[nItemListIdx].nX = nEquipPlace;
	m_Items[nItemListIdx].nY = 0;

//#ifdef _SERVER	because client need to show to menu
	int nActive = GetEquipEnhance(nEquipPlace);
	//Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx], nActive);
	InfectionNextEquip(nEquipPlace, TRUE);

	if (itempart_weapon == nEquipPlace)
	{		
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();
	}
	
	if (GetIfActive()) {
		bool bOverLook = FALSE;
		int nSkillId = 1556; //FullSet
		int nSkillLevel = 20;
		int nTime = -1;
		KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pSkill->CastStateSkill(Player[m_PlayerIdx].m_nIndex, 0, 0, nTime, bOverLook);
	}
	Player[m_PlayerIdx].UpdataCurData();

//#endif
	return TRUE;
}

int KItemList::CalcFreeItemCellCount(int nWidth, int nHeight, int nRoom)
{
	_ASSERT(m_Room[nRoom].m_pArray);

	return m_Room[nRoom].FindFreeCell(nWidth, nHeight);
}

/*!*****************************************************************************
// Function		: KItemList::InfectionNextEquip
// Purpose		: 装备影响其他装备
// Return		: void 
// Argumant		: int nEquipPlace
// Argumant		: BOOL bEquipUp 是装上（TRUE）还是卸下（FALSE）
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemList::InfectionNextEquip(int nEquipPlace, BOOL bEquip/* = FALSE */)
{
	if (m_PlayerIdx <= 0)
		return;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	for (int i = 0; i < MAX_ITEM_ACTIVE; i++)
	{
		int nInfectionEquip = ms_ActiveEquip[nEquipPlace][i];
		if (!m_EquipItem[nInfectionEquip])
			continue;
		if (g_IsAccrue(Item[m_EquipItem[nEquipPlace]].GetSeries(),
			Item[m_EquipItem[nInfectionEquip]].GetSeries()))
		{
			int nActive = GetEquipEnhance(nInfectionEquip);
			_ASSERT(nActive > 0);
			if (bEquip)
				Item[m_EquipItem[nInfectionEquip]].ApplyHiddenMagicAttribToNPC(&Npc[nNpcIdx], nActive);
			else
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
		}
	}
}
/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/
BOOL KItemList::UnEquip(int nIdx, int nPos/* = -1*/)
{
	int i = 0, nCurIdx = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{
		for (i = 0; i < itempart_num; i++)
		{
			if (m_EquipItem[i] == nIdx)
			{
				break;
			}
		}
		// 没有发现身上有这个装备
		if (i == itempart_num)
			return FALSE;

	}
	else
	{
		if (m_EquipItem[nPos] != nIdx)	// 东西不对
			return FALSE;
		i = nPos;
	}
	if (i == itempart_horse)
	{
		if (Npc[nNpcIdx].m_bRideHorse != FALSE)
		{
			int nActive = GetEquipEnhance(i);
			if (m_bActiveSet)
				nActive = 4;
			Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx], 0);
		}
	 }
	else
	{
		int nActive = GetEquipEnhance(i);
			if (m_bActiveSet)
				nActive = 4;
		Item[nIdx].RemoveMagicAttribFromNPC(&Npc[nNpcIdx], 0);
	}
	InfectionNextEquip(i, FALSE);
	// 这句话一定要放在上一句后，保证计算该装备激活的装备激活属性个数计算的正确性
	m_EquipItem[i] = 0;
	// 换装
	switch(i)
	{
	case itempart_head:
		Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
		break;
	case itempart_body:
		Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
		nCurIdx = GetEquipment(itempart_mantle);//#phi phong
		if (nCurIdx <= 0)
			Npc[nNpcIdx].m_MantleType = 0;
		break;
	case itempart_weapon:
		Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0);
		break;
	case itempart_horse:
		Npc[nNpcIdx].m_HorseType = -1;
		Npc[nNpcIdx].m_bRideHorse = FALSE;
		break;
	case itempart_mask:	//#mat na
		if (!m_nMaskLock)
			Npc[nNpcIdx].m_MaskType = 0;
		break;
	case itempart_mantle:
		Npc[nNpcIdx].m_byMantleLevel = 0;
		nCurIdx = GetEquipment(itempart_body);
		if (Item[nCurIdx].GetMantle() > 0)
			Npc[nNpcIdx].m_MantleType = Item[nCurIdx].GetMantle();
		else
			Npc[nNpcIdx].m_MantleType = 0;
		break;
	case itempart_cloak:
		nCurIdx = GetEquipment(itempart_body);
		if (nCurIdx)
		{
			if (Item[nCurIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
				Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nCurIdx].GetDetailType(), Item[nCurIdx].GetRow());
			else
				Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nCurIdx].GetParticular(), Item[nCurIdx].GetLevel());
		}
		else
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
		break;
	case itempart_hoods:
		nCurIdx = GetEquipment(itempart_head);
		if (nCurIdx)
		{
			if (Item[nCurIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nCurIdx].GetDetailType(), Item[nCurIdx].GetRow());
			else
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(Item[nCurIdx].GetParticular(), Item[nCurIdx].GetLevel());
		}
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
		break;
	default:
		break;
	}

	if (itempart_weapon == i)
	{
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();
	}	

	if (!GetIfActive()) {
		bool bOverLook = FALSE;
		int nSkillId = 1556; //FullSet
		int nSkillLevel = 20;
		int nTime = 0;
		KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
		memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
		Npc[Player[m_PlayerIdx].m_nIndex].SetStateSkillEffect(Player[m_PlayerIdx].m_nIndex, nSkillId, nSkillLevel, DamageMagicAttribs, 1, nTime, bOverLook);
	}
	Player[m_PlayerIdx].UpdataCurData();
	return TRUE;
}

int KItemList::GetEquipPlace(int nType)
{
	int nRet = -1;
	switch(nType)
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		nRet = itempart_weapon;
		break;
	case equip_armor:
		nRet = itempart_body;
		break;
	case equip_helm:
		nRet = itempart_head;
		break;
	case equip_boots:
		nRet = itempart_foot;
		break;
	case equip_ring:
		nRet = itempart_ring1;
		break;
	case equip_amulet:
		nRet = itempart_amulet;
		break;
	case equip_belt:
		nRet = itempart_belt;
		break;
	case equip_cuff:
		nRet = itempart_cuff;
		break;
	case equip_pendant:
		nRet = itempart_pendant;
		break;
	case equip_horse:
		nRet = itempart_horse;
		break;
	case equip_mask:	//#mat na
		nRet = itempart_mask;
		break;
	case equip_mantle://#phi phong
		nRet = itempart_mantle;
		break;
	case equip_signet:
		nRet = itempart_signet;
		break;
	case equip_shipin:
		nRet = itempart_shipin;
		break;
	case equip_hoods:
		nRet = itempart_hoods;
		break;
	case equip_cloak:
		nRet = itempart_cloak;
		break;
	default:
		break;
	}
	return nRet;
}

/*!*****************************************************************************
// Function		: KItemList::Fit
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Fit(int nIdx, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(Item[nIdx].GetGenre() == item_equip);
	switch(Item[nIdx].GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:	//#mat na
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_mantle://#phi phong
		if (nPlace == itempart_mantle)
			bRet = TRUE;
		break;
	case equip_signet:
		if (nPlace == itempart_signet)
			bRet = TRUE;
		break;
	case equip_shipin:
		if (nPlace == itempart_shipin)
			bRet = TRUE;
		break;
	case equip_hoods:
		if (nPlace == itempart_hoods)
			bRet = TRUE;
		break;
	case equip_cloak:
		if (nPlace == itempart_cloak)
			bRet = TRUE;
		break;
	}
	return bRet;
}

BOOL KItemList::Fit(KItem* pItem, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(pItem->GetGenre() == item_equip);
	switch(pItem->GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:	//#mat na
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_mantle://#phi phong
		if (nPlace == itempart_mantle)
			bRet = TRUE;
		break;
	case equip_signet:
		if (nPlace == itempart_signet)
			bRet = TRUE;
		break;
	case equip_shipin:
		if (nPlace == itempart_shipin)
			bRet = TRUE;
		break;
	case equip_hoods:
		if (nPlace == itempart_hoods)
			bRet = TRUE;
		break;
	case equip_cloak:
		if (nPlace == itempart_cloak)
			bRet = TRUE;
		break;
	}
	return bRet;
}

int KItemList::GetEquipEnhance(int nPlace)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (nPlace < 0 || nPlace >= itempart_num)
	{
		_ASSERT(0);
		return 0;
	}
	int nRet = 0;

	int nSeries = Item[m_EquipItem[nPlace]].GetSeries();
	if (g_IsAccrue(Npc[nNpcIdx].m_Series, nSeries))
		nRet ++;
	for (int i = 0; i < MAX_ITEM_ACTIVE; i++)
	{
		// 看看激活这个装备的装备中是否相生这个装备
		int nActivedEquip = m_EquipItem[ms_ActivedEquip[nPlace][i]];
		if (nActivedEquip)
		{
			if (g_IsAccrue(Item[nActivedEquip].GetSeries(), nSeries))
				nRet++;
		}
	}
	return nRet;
}

int KItemList::GetGoldEquipEnhance(int nPlace)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (nPlace < 0 || nPlace >= itempart_num)
	{
		_ASSERT(0);
		return 0;
	}
	int nRet = 0, nCnt = 0, nNum = 1;

	if (nPlace >= itempart_horse || IsEnoughToActive())
	{
		nRet = MAX_ITEM_MAGICATTRIB - MAX_ITEM_NORMAL_MAGICATTRIB;
	}
	else
	{
		if (Item[m_EquipItem[nPlace]].m_CommonAttrib.nItemNature >= NATURE_GOLD)
		{
			nNum = Item[m_EquipItem[nPlace]].GetNeedToActive1();
			int nGroup = Item[m_EquipItem[nPlace]].GetGroup();
			int nSetID = Item[m_EquipItem[nPlace]].GetSetID();

			for (int i = 0; i < itempart_horse; i++)
			{
				if (m_EquipItem[i] && Item[m_EquipItem[i]].m_CommonAttrib.nItemNature >= NATURE_GOLD)
				{
					if (i == itempart_ring1)
					{
						if (m_EquipItem[itempart_ring1] &&
							m_EquipItem[itempart_ring2] &&
							Item[m_EquipItem[itempart_ring1]].GetGroup() == nGroup &&
							Item[m_EquipItem[itempart_ring1]].GetSetID() == nSetID)
							continue;
					}
					if (Item[m_EquipItem[i]].GetGroup() == nGroup)
						nCnt++;
				}
			}
		}
		nRet = (nCnt / nNum);
	}

	return nRet;
}

#ifdef _SERVER
//Edit by phong kieu khoa bao hiem trang bi
BOOL KItemList::SetLockItem(int ItemIdx, int lock)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	if(ItemIdx < 0)
		return FALSE;

	int nGameId = SearchID(ItemIdx);
	if(lock == 1)
	{
		Player[m_PlayerIdx].ExecuteScript("\\script\\player\\binditem.lua","main", nGameId,false);
	}
	else
	{
		Player[m_PlayerIdx].ExecuteScript("\\script\\player\\unbinditem.lua","main", nGameId,false);
	}
	return TRUE;
}

//Edit by phong kieu xoa vat pham het han
BOOL KItemList::RemoveItem_YearExp(int ItemIdx)
{
	if(ItemIdx < 0)
	{
		return FALSE;
	}
	
	int nGameId = SearchID(ItemIdx);
	BOOL res = Item[nGameId].GetTimeYearIsExp();
	if(res)
	{
		Remove(nGameId);// del item cua nhan vat
		ItemSet.Remove(nGameId);// del item toan sv
		return res;
	}
	return FALSE;
}
#endif

BOOL KItemList::EatMecidine(int nIdx)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	int nGenre = Item[nIdx].GetGenre();
	int	nDetailType = Item[nIdx].GetDetailType();

	_ASSERT(nGenre == item_medicine ||  nGenre == item_task || nGenre == item_mine || nGenre == item_townportal || item_magicscript);

	if (nGenre == item_medicine)
	{
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx], 3);

#ifdef _SERVER
		Remove(nIdx);
		ItemSet.Remove(nIdx);
#endif
	}

	else if (nGenre == item_task)
	{
		//Player[m_PlayerIdx].ExecuteScript(Item[nIdx].GetScript(),"main", nIdx);
	}

	else if (nGenre == item_mine)
	{
		//Player[m_PlayerIdx].ExecuteScript(Item[nIdx].GetScript(),"main", nIdx);
	}

	else if (nGenre == item_magicscript)
	{
		char* ScriptFileName = Item[nIdx].GetScript();
		bool bGlobal = true;
		//check string "thodiaphu" exist in ScriptFileName
		if (strstr(ScriptFileName, "thodiaphuvh.lua") != nullptr || 
		strstr(ScriptFileName, "tuiduocpham.lua") != nullptr)
		{
			// "thodiaphuvh.lua" exists in ScriptFileName
			bGlobal = false;
		}
		Player[m_PlayerIdx].ExecuteScript(Item[nIdx].GetScript(),"main", nIdx, bGlobal);
	}
	
	else if (nGenre == item_townportal)
	{
		if (!Npc[Player[m_PlayerIdx].m_nIndex].IsAlive() || Npc[Player[m_PlayerIdx].m_nIndex].m_CurrentLife <= 0)//fix by phong ki襲 ch誸 ho芻 m竨 nh?h琻 0 kh玭g cho ph?
		{
			return FALSE;
		}
		//fkauto
#ifndef _SERVER
		//Player[CLIENT_PLAYER_INDEX].m_cAuto.FkAutoMapSet_StepOne();
#endif
		//
		if (!Npc[Player[m_PlayerIdx].m_nIndex].m_FightMode)
		{
#ifndef _SERVER	
			KSystemMessage Msg;
			Msg.byConfirmType = SMCT_CLICK;
			Msg.byParamSize = 0;
			Msg.byPriority = 1;
			Msg.eType = SMT_PLAYER;
			strcpy(Msg.szMessage, MSG_NPC_NOT_USE_TOWNPORTAL);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, NULL);
#endif
			return FALSE;
		}
#ifdef _SERVER
		Player[m_PlayerIdx].ExecuteScript("\\script\\item\\ib\\thodiafu.lua","main", nIdx,false);
#endif
	}

	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendCommand(do_stand);
	}
	return TRUE;
}

#ifndef _SERVER
int KItemList::UseItem(int nIdx)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (0 == FindSame(nIdx))
	{
		return 0;
	}

	int		nRet = 0;
	switch(Item[nIdx].GetGenre())
	{
	case item_equip:
		//if (Equip(nNpcIdx, nIdx))
			nRet = REQUEST_EQUIP_ITEM;
		break;
	
	case item_townportal:
	case item_medicine:
	case item_magicscript:
		if (EatMecidine(nIdx))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	case item_task:
		//if (EatMecidine(nIdx))
		//	nRet = REQUEST_EAT_MEDICINE;
		break;
	case item_mine:
		//if (EatMecidine(nIdx))
		//	nRet = REQUEST_EAT_MEDICINE;
		break;
	default:
		break;
	}
	return nRet;
}
#endif

#ifndef _SERVER
int KItemList::ChangeItemInPlayer(int nIdx)
{
	if (m_PlayerIdx <= 0)
		return -1;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (0 == FindSame(nIdx))
	{
		return -1;
	}
	if(Item[nIdx].GetGenre() != item_equip)
	{
		return -1;
	}
	int		nRet = Item[nIdx].GetDetailType();
	switch(nRet)
	{
		case itempart_head:
			break;
		case itempart_body:
			break;
		case itempart_belt:
			break;
		case itempart_weapon:
			break;
		case itempart_foot:
			break;
		case itempart_cuff:
			break;
		case itempart_amulet:
			break;
		case itempart_ring1:
			break;
		case itempart_ring2:
			break;
		case itempart_pendant:
			break;
		case itempart_mask:
			break;
		case itempart_num:
			break;
		case itempart_horse:
			break;
		default:
			break;
	}
	return nRet;
}
#endif

#ifndef _SERVER 
BOOL KItemList::AutoUseItem(int nGenre, int nDetailType, int nParticular, int nPlayerIndex)
{
	int	nIdx, nXPos, nYPos, nPlace;

	nIdx = 0;
	nXPos = 0;
	nYPos = 0;
	
	if (!FindSameDetailTypeInImmediacy(nGenre, nDetailType, nParticular, &nIdx, &nXPos, &nYPos))
	{
		if(!FindSameDetailTypeInEquipment(nGenre, nDetailType, nParticular, &nIdx, &nXPos, &nYPos))
		{	
			return FALSE;
		}
		else
		{
			nPlace = pos_equiproom;
		}
	}
	else
	{
		nPlace = pos_immediacy;
	}	

	ItemPos	Pos;
	Pos.nPlace =  nPlace;
	Pos.nX = nXPos;
	Pos.nY = nYPos;
	Player[nPlayerIndex].ApplyUseItem(nIdx, Pos);

	return TRUE;
}

BOOL KItemList::AutoCheckItem(int nGenre, int nDetailType, int nParticular)
{
	int	nIdx, nXPos, nYPos;

	nIdx = 0;
	nXPos = 0;
	nYPos = 0;

	if (!FindSameDetailTypeInImmediacy(nGenre, nDetailType, nParticular, &nIdx, &nXPos, &nYPos))
	{
		if(!FindSameDetailTypeInEquipment(nGenre, nDetailType, nParticular, &nIdx, &nXPos, &nYPos))
		{	
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return TRUE;
	}	
}
#endif

int	KItemList::SearchID(int nID, int* pRetPlace, int* pRetX, int* pRetY)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[m_Items[nIdx].nIdx].GetID() == (DWORD)nID)
		{
			if(pRetPlace)
				*pRetPlace = m_Items[nIdx].nPlace;
			if(pRetX)
				*pRetX = m_Items[nIdx].nX;
			if(pRetY)
				*pRetY = m_Items[nIdx].nY;
			return m_Items[nIdx].nIdx;
		}
	}
	return 0;
}

void KItemList::ExchangeMoney(int pos1, int pos2, int nMoney)
{
	if (pos1 < 0 || pos2 < 0 || pos1 > room_trade || pos2 > room_trade)
		return;
	if (pos1 == room_repository && !Player[m_PlayerIdx].m_CUnlocked)
	{
		return;
	}
	if (m_Room[pos1].AddMoney(-nMoney))		
	{
		if (!m_Room[pos2].AddMoney(nMoney))	
		{
			m_Room[pos1].AddMoney(nMoney);	
		}
	}
	else
	{
		return;
	}

#ifndef _SERVER
	if (pos1 == room_equipment && pos2 == room_repository)
		SendClientCmdStoreMoney(0, nMoney);
	else if (pos1 == room_repository && pos2 == room_equipment)
		SendClientCmdStoreMoney(1, nMoney);
#endif
#ifdef _SERVER
	SendMoneySync();
#endif	
}

//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的总钱数
//----------------------------------------------------------------------
int KItemList::GetMoneyAmount()
{
	return (m_Room[room_equipment].GetMoney() + m_Room[room_repository].GetMoney());
}
int KItemList::GetXuAmount()
{
	return (m_Room[room_equipment].GetXu());
}
//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的钱数
//----------------------------------------------------------------------
int KItemList::GetEquipmentMoney()
{
	return m_Room[room_equipment].GetMoney();
}

int KItemList::GetTradeMoney()
{
	return m_Room[room_trade].GetMoney();
}

int KItemList::GetEquipmentXu()
{
	return m_Room[room_equipment].GetXu();
}


BOOL KItemList::AddMoney(int nRoom, int nMoney, bool Gamble)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if ( !m_Room[nRoom].AddMoney(nMoney) )
		return FALSE;

#ifdef _SERVER
	SendMoneySync(Gamble);
#endif

	return TRUE;
}

BOOL KItemList::AddXu(int nRoom, int nXu)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if ( !m_Room[nRoom].AddXu(nXu) )
		return FALSE;

	return TRUE;
}

BOOL KItemList::CostMoney(int nMoney, bool Gamble)
{
	if (nMoney > GetEquipmentMoney())
		return FALSE;

	if ( !m_Room[room_equipment].AddMoney(-nMoney) )
		return FALSE;

#ifdef _SERVER
	SendMoneySync(Gamble);
#endif

	return TRUE;
}

BOOL KItemList::CostXu(int nXu)
{
	if (nXu > GetEquipmentXu())
		return FALSE;

	if ( !m_Room[room_equipment].AddXu(-nXu) )
		return FALSE;

	return TRUE;
}

BOOL KItemList::DecMoney(int nMoney)
{
	if (nMoney < 0)
		return FALSE;

	if (nMoney > m_Room[room_equipment].GetMoney())
	{
		nMoney -= m_Room[room_equipment].GetMoney();
		SetRoomMoney(room_equipment, 0);
		if (nMoney > m_Room[room_repository].GetMoney())
			SetRoomMoney(room_repository, 0);
		else
			AddMoney(room_repository, -nMoney);
	}
	else
	{
		AddMoney(room_equipment, -nMoney);
	}

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}

BOOL KItemList::DecXu(int nXu)
{
	if (nXu < 0)
		return FALSE;

	if (nXu > m_Room[room_equipment].GetXu())
	{
		nXu -= m_Room[room_equipment].GetXu();
		SetRoomXu(room_equipment, 0);
	}
	else
	{
		AddXu(room_equipment, -nXu);
	}

	return TRUE;
}

#ifdef _SERVER

void	KItemList::TradeMoveMoney(int nMoney)
{
	m_Room[room_trade].SetMoney(nMoney);
	SendMoneySync();

	TRADE_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_trademoneysync;
	sMoney.m_nMoney = nMoney;
	g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(TRADE_MONEY_SYNC));
}
#endif

#ifdef _SERVER

void	KItemList::GambleMoveMoney(int nMoney)
{
	m_Room[room_trade].SetMoney(nMoney);
	SendMoneySync();

	GAMBLE_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_gamblemoneysync;
	sMoney.m_nMoney = nMoney;
	g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(GAMBLE_MONEY_SYNC));
}
#endif

#ifdef _SERVER

void	KItemList::SendMoneySync(bool Gamble)
{
	PLAYER_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_syncmoney;
	sMoney.m_nMoney1 = m_Room[room_equipment].GetMoney();
	sMoney.m_nMoney2 = m_Room[room_repository].GetMoney();
	sMoney.m_nMoney3 = m_Room[room_trade].GetMoney();
	sMoney.m_bGamble = Gamble;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_MONEY_SYNC));
}

#endif

void KItemList::SetMoney(int nMoney1, int nMoney2, int nMoney3)
{
	m_Room[room_equipment].SetMoney(nMoney1);
	m_Room[room_repository].SetMoney(nMoney2);
	m_Room[room_trade].SetMoney(nMoney3);
#ifndef _SERVER
	KUiObjAtContRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = nMoney2;
	sMoney.eContainer = UOC_STORE_BOX;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
#endif
}

void KItemList::SetXu(int nXu)
{
	m_Room[room_equipment].SetXu(nXu);
}

void KItemList::SetRoomMoney(int nRoom, int nMoney)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetMoney(nMoney);
}

void KItemList::SetRoomXu(int nRoom, int nXu)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetXu(nXu);
}

void KItemList::ExchangeItem(ItemPos* SrcPos, ItemPos* DesPos)
{
	if (SrcPos->nPlace != DesPos->nPlace)
		return;

	int nTempHand = m_Hand; int nTempHandSkill = m_HandSkill;
	int	nEquipIdx1 = 0; int uGenre = 0;

#ifdef _SERVER
	PLAYER_MOVE_ITEM_SYNC	sMove;
	sMove.ProtocolType = s2c_playermoveitem;
	sMove.m_btDownPos = SrcPos->nPlace;
	sMove.m_btDownX = SrcPos->nX;
	sMove.m_btDownY = SrcPos->nY;
	sMove.m_btUpPos = DesPos->nPlace;
	sMove.m_btUpX = DesPos->nX;
	sMove.m_btUpY = DesPos->nY;

	if (m_Hand && !FindSame(m_Hand))//add by phong ki襲 antihack
	{
		printf("Hack ExchangeItem m_Hand [%s] [%s]\n",Player[m_PlayerIdx].m_AccoutName,Player[m_PlayerIdx].m_PlayerName);
		return;
	}
#endif
	//load 3 storebox by kinnox;
	if(Npc[Player[m_PlayerIdx].m_nIndex].m_FightMode && DesPos->nPlace >= pos_repositoryroom && DesPos->nPlace <= pos_exbox3room)
		return;
	int Map = SubWorld[Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SubWorldIndex].m_SubWorldID;
	if (Map == 397 && DesPos->nPlace == pos_equiproomex)
		return;
	//end code
	switch(SrcPos->nPlace)
	{
	case pos_hand:	//--------------------------- Tr猲 tay ------------------------------------
		g_DebugLog("%s exchange item error", Npc[Player[m_PlayerIdx].m_nIndex].Name);
		return;
		break;

	case pos_skilltakewith:	//--------------------------- Tr猲 b秐g skill l蕐 ra ------------------------------------
		{
			uGenre = SrcPos->nX; //uGenre c馻 skill
			nEquipIdx1 = SrcPos->nY; //uId c馻 skill

			if (nEquipIdx1 < 0 || uGenre != CGOG_SKILL_FIGHT)
				return;
			//
			int nLevel = Npc[Player[m_PlayerIdx].m_nIndex].m_SkillList.GetLevel(nEquipIdx1);
			int nAddLevel = Npc[Player[m_PlayerIdx].m_nIndex].m_SkillList.GetAddLevel(nEquipIdx1);
			int nTotalLevel = nLevel + nAddLevel;
			if(nLevel <= 0) //nh鱪g skill ch璦 c?甶觤 kh玭g cho l蕐 ra
				return;

			KSkill * pSkill = (KSkill *) g_SkillManager.GetSkill(nEquipIdx1, nTotalLevel);
			if(pSkill) //ch?cho ph衟 l蕐 ra nh鱪g lo筰 skill chi課 u v?buff	//attr 1008 h?tr?b?ng 1009 h?tr?ch?ng
			{
				if(pSkill->IsAura() || pSkill->GetSkillId() == 709 || pSkill->GetSkillId() == 252 //skill kh玭g 頲 ph衟
					|| pSkill->GetSkillId() == 36 || pSkill->GetSkillId() == 630
					|| pSkill->GetAttribType() == 1008) 
					return;
				//
				if(!(pSkill->IsTargetEnemy() || pSkill->IsTargetSelf() || pSkill->GetSkillId() == 210 || pSkill->GetAttribType() == 1009)) //skill 頲 ph衟
					return;
			}

			if (m_HandSkill)
			{
				m_HandSkill = 0;
			}
			else	//a skill l猲 tay
			{
				m_HandSkill = nEquipIdx1;
			}
	#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
	#endif
		}
		break;

	case pos_immediacyskill: //--------------------------- ?ph輒 t総 1 n 9 skill ------------------------------------
		if(m_HandSkill)
		{
#ifdef _SERVER
			int nSeries = -1;
			int nLevel = 1;
			int nLuck = -1;
			int nDetailType = 13;
			int nParticularType = m_HandSkill;
			if (!m_Room[room_immediacy].CheckSameItemType(item_medicine, nDetailType, nParticularType, nLevel)) //ch璦 t錸 t筰 th?m韎 add v祇 ?
			{
				int nIndex = ItemSet.AddItemSet2(item_medicine, nSeries, nLevel, nLuck, nDetailType, nParticularType, NULL);
				if (nIndex)
				{
					nEquipIdx1 = AddKIL(nIndex, pos_immediacy, DesPos->nX, DesPos->nY);
				}
			}
			else
			{
				Player[m_PlayerIdx].ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", MSG_SKILL_SAME_IN_IMMEDIATE);
			}
#endif
			m_HandSkill = 0;
		}
		else
		{
			break;
		}
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		break;

	case pos_equip:	//--------------------------- Trang b?tr猲 ngi ------------------------------------
		if (Player[this->m_PlayerIdx].CheckTrading())	
			return;
		if (SrcPos->nX < 0 || SrcPos->nX >= itempart_num || DesPos->nX < 0 || DesPos->nX >= itempart_num)
			return;
		nEquipIdx1 = m_EquipItem[SrcPos->nX];
		if (nEquipIdx1)
		{
			UnEquip(nEquipIdx1, SrcPos->nX);
		}
		if (m_Hand)
		{
			if (CanEquip(m_Hand, DesPos->nX))
			{
				Equip(m_Hand, DesPos->nX);
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else if (nEquipIdx1)
			{
				Equip(nEquipIdx1, SrcPos->nX);
			}
		}
		else
		{
			m_Hand = nEquipIdx1;
			m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;

	case pos_immediacy:	//--------------------------- ?ph輒 t総 1 n 9 ------------------------------------
		if (Player[m_PlayerIdx].CheckTrading())	
			return;
		
		if (m_Hand)
		{
			if (m_Room[room_immediacy].CheckSameItemType(Item[m_Hand].GetGenre(), Item[m_Hand].GetDetailType(), 
				Item[m_Hand].GetParticular(), Item[m_Hand].GetLevel()))
			{	//Х c?v藅 ph萴 c飊g lo筰 trong ?
#ifdef _SERVER
				BYTE	byFinished = s2c_itemexchangefinish;
				if (g_pServer)
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &byFinished, sizeof(BYTE));
#endif

#ifndef _SERVER
				KSystemMessage	sMsg;
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				sprintf(sMsg.szMessage, MSG_ITEM_SAME_DETAIL_IN_IMMEDIATE);
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
#endif
				return;
			}
		}

		nEquipIdx1 = m_Room[room_immediacy].FindItem(SrcPos->nX, SrcPos->nY); //L蕐 ItemIdx 產ng n籱 trong ?ph輒 t総
		if (nEquipIdx1 < 0)
			return;
		
		if (nEquipIdx1)	//N誹 c?th?nh蔯 n?l猲
		{
			if(nEquipIdx1 && Item[nEquipIdx1].IsFkItemSkill()) //item skill shorcut l蕐 ra m蕋 lu玭
			{
				//EatMecidine(nEquipIdx1);
				//return;
			}
			if (!m_Room[room_immediacy].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		if (m_Hand) //N誹 tr猲 tay c?
		{
			if (m_Room[room_immediacy].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()) 
					//&& Item[m_Hand].GetStackNum() <= 1
					&& (Item[m_Hand].GetGenre() == item_medicine || Item[m_Hand].GetGenre() == item_townportal 
					|| (Item[m_Hand].GetGenre() == item_magicscript && Item[m_Hand].GetBShortKey()))) //ch?cho ph衟 b?c竎 v藅 ph萴 thu鑓 v?th?a ph?
			{	//B?item xu鑞g ?ph輒 t総
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_immediacy;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else //th玭g b竜 v藅 ph萴 kh玭g th?b?v祇 ?ph輒 t総
			{
				m_Room[room_immediacy].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				Player[m_PlayerIdx].ExecuteScript("\\script\\player\\mgs2player_from_c.lua","main", MSG_ITEM_NOT_STACK_IN_IMMEDIATE);
			}
		}
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;

	case pos_repositoryroom:	//--------------------------- Rng ch鴄  ------------------------------------
		if (Player[m_PlayerIdx].CheckTrading())	
			return;
		nEquipIdx1 = m_Room[room_repository].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_repository].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}

	//	if (m_Hand)
	//	{
			if (Item[m_Hand].CanStack(nEquipIdx1, m_Hand))
		{
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
			{
				Item[m_Hand].Remove();
				Remove(m_Hand);
			}
			else
			{
				Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - Def_MAX_STACK_TIENDONG);
				Item[nEquipIdx1].SetStackNum(Def_MAX_STACK_TIENDONG);
			}
			m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			
			return;
	//		}
		}
		
		if (Item[m_Hand].GetGenre() == item_task && Item[m_Hand].GetDetailType() == 238)//lam thuy tinh
		{		
			if (nEquipIdx1 && Item[nEquipIdx1].GetGenre() == item_equip && (Item[nEquipIdx1].GetGoldId() > 0 || Item[nEquipIdx1].IsPurple()))
			{
#ifdef _SERVER
				int nItemClass		= Item[nEquipIdx1].GetGenre();
				int nDetailType		= Item[nEquipIdx1].GetDetailType();
				int nParticularType	= Item[nEquipIdx1].GetParticular();
				int nLevel			= Item[nEquipIdx1].GetLevel();
				int nSeries			= Item[nEquipIdx1].GetSeries();
				int nLuck			= Item[nEquipIdx1].GetItemParam()->nLuck;
				int nEnChance		= Item[nEquipIdx1].GetEnChance();
				int nGoldId			= Item[nEquipIdx1].GetGoldId();	
				int nVersion		= Item[nEquipIdx1].GetItemParam()->nVersion;
				int	nGndrom			= Item[nEquipIdx1].GetItemParam()->uRandomSeed;
				int nPoint			= Item[nEquipIdx1].IsPurple();
				int nXOpt			= Item[nEquipIdx1].GetMaxOptMultiply();
				
				if (nEnChance >= 9)
				{
					return;
				}

				if (GetRandomNumber(1,10) < nEnChance + 1)
				{
					if (nEnChance >= 2)
					{
						nEnChance -= 2;
					}	
				}

				int nItemLevel[6];
				
				ZeroMemory(nItemLevel, sizeof(nItemLevel));
				
				for (int i = 0; i < 6; i ++)
				nItemLevel[i] = Item[nEquipIdx1].GetItemParam()->nGeneratorLevel[i];
				
				int	nIndex;

				if (nGoldId)
				{
					nIndex = ItemSet.AddGoldItem(nGoldId,nItemLevel,nSeries,nEnChance + 1, 0, 0, 0, 0, 0, 0, nXOpt);
				} 
				else
				{
					nIndex = ItemSet.AddItemSet2(nItemClass,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel, nVersion,nGndrom,1,nEnChance + 1,nPoint,0,0,0,0,0,0, nXOpt);
				}
				
				if (nIndex <= 0)
				{
					return;
				}
				
				Remove(m_Hand);
				ItemSet.Remove(m_Hand);
				Remove(nEquipIdx1);
				ItemSet.Remove(nEquipIdx1);
				int		x, y;
				if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
				{
					AddKIL(nIndex, pos_equiproom, x, y);
				}
				else
				{
					int		nIdx = Hand();
					if (nIdx)
					{
						Remove(nIdx);
						
						KMapPos sMapPos;
						KObjItemInfo	sInfo;
						
						Player[m_PlayerIdx].GetAboutPos(&sMapPos);
						
						sInfo.m_nItemID = nIdx;
						sInfo.m_nItemWidth = Item[nIdx].GetWidth();
						sInfo.m_nItemHeight = Item[nIdx].GetHeight();
						sInfo.m_nMoneyNum = 0;
						char szNameTemp[OBJ_NAME_LENGHT];
						if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
						{
							sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
							strcpy(sInfo.m_szName, szNameTemp);
						}
						else
						strcpy(sInfo.m_szName, Item[nIdx].GetName());
						sInfo.m_nColorID = Item[nIdx].GetColorItem();
						sInfo.m_nGenre = Item[nIdx].GetGenre();
						sInfo.m_nDetailType = Item[nIdx].GetDetailType();
						sInfo.m_nParticularType = Item[nIdx].GetParticular();
						sInfo.m_nMovieFlag = 1;
						sInfo.m_nSoundFlag = 1;
						sInfo.m_dwNpcId1 = Npc[Player[m_PlayerIdx].m_nIndex].m_dwID;
						
						int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
						if (nObj >= 0)
						{
							if (Item[nIdx].GetGenre() == item_task)
							{
								Object[nObj].SetEntireBelong(m_PlayerIdx);
							}
							else
							{
								Object[nObj].SetItemBelong(m_PlayerIdx);
							}
						}
					}
					AddKIL(nIndex, pos_hand, 0 ,0);
				}
				return;
#endif
			}				
		}

		if (m_Hand)
		{
			if (m_Room[room_repository].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else
			{
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	
	case pos_exbox1room: //--------------------------- m?r閚g rng 1 ------------------------------------
		if (Player[m_PlayerIdx].CheckTrading())	
			return;
		nEquipIdx1 = m_Room[room_exbox1].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox1].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}

		if (m_Hand)
		{
			if (m_Room[room_exbox1].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox1room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}

			else
			{
				m_Room[room_exbox1].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;

	case pos_exbox2room: //--------------------------- m?r閚g rng 2 ------------------------------------
		if (Player[m_PlayerIdx].CheckTrading())	
			return;
		nEquipIdx1 = m_Room[room_exbox2].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox2].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}

		if (m_Hand)
		{
			if (m_Room[room_exbox2].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox2room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else
			{
				m_Room[room_exbox2].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}

		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
		
	case pos_exbox3room: //--------------------------- m?r閚g rng 3 ------------------------------------
		if (Player[m_PlayerIdx].CheckTrading())	
			return;
		nEquipIdx1 = m_Room[room_exbox3].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_exbox3].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}

		if (m_Hand)
		{
			if (m_Room[room_exbox3].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_exbox3room;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}

			else
			{
				m_Room[room_exbox3].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}

		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
		
	case pos_equiproomex: //--------------------------- h祅h trang m?r閚g------------------------------------
		nEquipIdx1 = m_Room[room_equipmentex].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_equipmentex].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}

		if (m_Hand)
		{
			if (m_Room[room_equipmentex].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproomex;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}

			else
			{
				m_Room[room_equipmentex].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}

		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
		
	case pos_equiproom:	//--------------------------- h祅h trang ------------------------------------
		nEquipIdx1 = m_Room[room_equipment].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_equipment].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}
		if (Item[m_Hand].CanStack(nEquipIdx1, m_Hand))
		{
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
			{
				Item[m_Hand].Remove();
				Remove(m_Hand);
			}
			else
			{
				Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - Def_MAX_STACK_TIENDONG);
				Item[nEquipIdx1].SetStackNum(Def_MAX_STACK_TIENDONG);
			}
			m_Room[room_equipment].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			
			return;
		//	}
		}

		if (Item[m_Hand].GetGenre() == item_task && Item[m_Hand].GetDetailType() == 238)//lam thuy tinh
		{		
			if (nEquipIdx1 && Item[nEquipIdx1].GetGenre() == item_equip && (Item[nEquipIdx1].GetGoldId() > 0 || Item[nEquipIdx1].IsPurple()))
			{
#ifdef _SERVER
				int nItemClass		= Item[nEquipIdx1].GetGenre();
				int nDetailType		= Item[nEquipIdx1].GetDetailType();
				int nParticularType	= Item[nEquipIdx1].GetParticular();
				int nLevel			= Item[nEquipIdx1].GetLevel();
				int nSeries			= Item[nEquipIdx1].GetSeries();
				int nLuck			= Item[nEquipIdx1].GetItemParam()->nLuck;
				int nEnChance		= Item[nEquipIdx1].GetEnChance();
				int nGoldId			= Item[nEquipIdx1].GetGoldId();	
				int nVersion		= Item[nEquipIdx1].GetItemParam()->nVersion;
				int	nGndrom			= Item[nEquipIdx1].GetItemParam()->uRandomSeed;
				int nPoint			= Item[nEquipIdx1].IsPurple();
				int nXOpt			= Item[nEquipIdx1].GetMaxOptMultiply();
				int nItemLevel[6];
				if (nEnChance >= 9)
				{
					return;
				}

				if (GetRandomNumber(1,10) < nEnChance + 1)
				{
					if (nEnChance >= 2)
					{
						nEnChance -= 2;
					}	
				}
				ZeroMemory(nItemLevel, sizeof(nItemLevel));
				
				for (int i = 0; i < 6; i ++)
					nItemLevel[i] = Item[nEquipIdx1].GetItemParam()->nGeneratorLevel[i];
				
				int	nIndex;
				
				if (nGoldId)
				{
					nIndex = ItemSet.AddGoldItem(nGoldId,nItemLevel,nSeries,nEnChance + 1, 0, 0, 0, 0, 0, 0, nXOpt);
				} 
				else
				{
					nIndex = ItemSet.AddItemSet2(nItemClass,nSeries,nLevel,nLuck,nDetailType,nParticularType,nItemLevel,nVersion,nGndrom,1,nEnChance + 1,nPoint, 0, 0, 0, 0, 0, 0, nXOpt);
				}
				
				if (nIndex <= 0)
				{
					return;
				}
				
				Remove(m_Hand);
				ItemSet.Remove(m_Hand);
				Remove(nEquipIdx1);
				ItemSet.Remove(nEquipIdx1);
				int		x, y;
				if (CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
				{
					AddKIL(nIndex, pos_equiproom, x, y);
				}
				else
				{
					int		nIdx = Hand();
					if (nIdx)
					{
						Remove(nIdx);
						
						KMapPos sMapPos;
						KObjItemInfo	sInfo;
						
						Player[m_PlayerIdx].GetAboutPos(&sMapPos);
						
						sInfo.m_nItemID = nIdx;
						sInfo.m_nItemWidth = Item[nIdx].GetWidth();
						sInfo.m_nItemHeight = Item[nIdx].GetHeight();
						sInfo.m_nMoneyNum = 0;
						char szNameTemp[OBJ_NAME_LENGHT];
						if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
						{
							sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
							strcpy(sInfo.m_szName, szNameTemp);
						}
						else
						strcpy(sInfo.m_szName, Item[nIdx].GetName());
						sInfo.m_nColorID = Item[nIdx].GetColorItem();
						sInfo.m_nGenre = Item[nIdx].GetGenre();
						sInfo.m_nDetailType = Item[nIdx].GetDetailType();
						sInfo.m_nParticularType = Item[nIdx].GetParticular();
						sInfo.m_nMovieFlag = 1;
						sInfo.m_nSoundFlag = 1;
						sInfo.m_dwNpcId1 = Npc[Player[m_PlayerIdx].m_nIndex].m_dwID;

						int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
						if (nObj >= 0)
						{
							if (Item[nIdx].GetGenre() == item_task)
							{
								Object[nObj].SetEntireBelong(m_PlayerIdx);
							}
							else
							{
								Object[nObj].SetItemBelong(m_PlayerIdx);
							}
						}
					}
					AddKIL(nIndex, pos_hand, 0, 0);
				}
				return;
#endif
			}				
		}

		if (m_Hand)
		{
			if(Item[m_Hand].IsFkItemSkill()) //item skill shorcut l蕐 ra m蕋 lu玭
			{
				EatMecidine(m_Hand);
				return;
			}
			if (m_Room[room_equipment].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}

			else
			{
				m_Room[room_equipment].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}

		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if(nEquipIdx1 && Item[nEquipIdx1].IsFkItemSkill()) //item skill shorcut l蕐 ra m蕋 lu玭
			{
				EatMecidine(nEquipIdx1);
				return;
			}
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;

	case pos_traderoom:	//--------------------------- giao d辌h ------------------------------------
	case pos_gambleroom:	//--------------------------- OTT ------------------------------------
		if ( !Player[m_PlayerIdx].CheckTrading())	
			return;
#ifdef _SERVER
/*		if (m_Hand && Item[m_Hand].GetGenre() == item_task)
		{
			if (m_Hand )//&& Item[m_Hand].GetIsTrade() == 0) 
			{
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_TRADE_TASK_ITEM;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				return;
			}
			else
			{
				
			}
		}*/
#endif
		nEquipIdx1 = m_Room[room_trade].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_trade].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}
		if (m_Hand)
		{
			if (m_Room[room_trade].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = SrcPos->nPlace;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
#ifdef _SERVER
				if (nEquipIdx1)
				{
					ITEM_REMOVE_SYNC	sRemove;
					sRemove.ProtocolType = s2c_removeitem;
					sRemove.m_ID = Item[nEquipIdx1].m_dwID;
					g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
				}
				this->SyncItem(m_Hand, pos_trade1, DesPos->nX, DesPos->nY, Player[m_PlayerIdx].m_cTrade.m_nTradeDest, true); //edit by phong kieu SYNC khi giao dich 2 nhan vat
				/*g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));*/
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				m_Hand = nEquipIdx1;
				if (FindSame(nEquipIdx1))
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			else
			{
				m_Room[room_trade].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		else
		{
#ifdef _SERVER
			if (nEquipIdx1)
			{
				ITEM_REMOVE_SYNC	sRemove;
				sRemove.ProtocolType = s2c_removeitem;
				sRemove.m_ID = Item[nEquipIdx1].m_dwID;
				g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
			}
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;

	case pos_give:	//--------------------------- b秐g giao ------------------------------------
		nEquipIdx1 = m_Room[room_give].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_give].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
			return;
		}
		//if (m_Hand)
		if (Item[m_Hand].CanStack(nEquipIdx1, m_Hand))
		{
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
			{
				Item[m_Hand].Remove();
				Remove(m_Hand);
			}
			else
			{
				Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - Def_MAX_STACK_TIENDONG);
				Item[nEquipIdx1].SetStackNum(Def_MAX_STACK_TIENDONG);
			}
			m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			
			return;
		}

		if (m_Hand)
		{
			if (m_Room[room_give].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_give;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}

			else
			{
				m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}

		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;

	case pos_affairitem:	//--------------------------- b秐g giao 2 ------------------------------------
		nEquipIdx1 = m_Room[room_affairitem].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		if (nEquipIdx1)
		{
			if (!m_Room[room_affairitem].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		if (m_Hand)
		{
			if (Item[m_Hand].CanStack(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))
				{
					Item[m_Hand].Remove();
					Remove(m_Hand);
				}
				else
				{
					Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - Def_MAX_STACK_TIENDONG);
					Item[nEquipIdx1].SetStackNum(Def_MAX_STACK_TIENDONG);
				}
				m_Room[room_affairitem].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				return;
			}
		}

		if (m_Hand)
		{
			if (m_Room[room_affairitem].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_affairitem;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}

			else
			{
				m_Room[room_affairitem].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}

		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;

	case pos_tremble:	//--------------------------- kh秏 n筸 ------------------------------------
		if (Player[this->m_PlayerIdx].CheckTrading())	
			return;
		if (SrcPos->nX < 0 || SrcPos->nX >= tremblepart_num || DesPos->nX < 0 || DesPos->nX >= tremblepart_num)
			return;
		nEquipIdx1 = m_TrembleItem[SrcPos->nX];
		if (m_Hand)
		{
			if(CheckTrembleItem(m_Hand, DesPos->nX) == TRUE)
			{	
				if (nEquipIdx1)
				{
					UnTrembleItem(nEquipIdx1, SrcPos->nX);
				}
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else if (nEquipIdx1)
			{
				CheckTrembleItem(nEquipIdx1, SrcPos->nX);
			}
		}
		else
		{	
			if (nEquipIdx1)
			{
				UnTrembleItem(nEquipIdx1, SrcPos->nX);
			}
			m_Hand = nEquipIdx1;
			m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif		
			
		}
		break;
	}

#ifndef _SERVER
	if (m_Hand != nTempHand)		// Notify to Menu
	{
		//uParam = (KUiObjAtRegion*)pInfo -> 
		//nParam = bAdd -> 0
		KUiObjAtContRegion pInfo1, pInfo2;
		if (nTempHand == 0)
		{
			pInfo2.Obj.uGenre = CGOG_NOTHING;
			pInfo2.Obj.uId = 0;
			pInfo2.Region.Width = 0;
			pInfo2.Region.Height = 0;
		}
		else
		{
			pInfo2.Obj.uGenre = CGOG_ITEM;	
			pInfo2.Obj.uId = nTempHand;
			pInfo2.Region.Width = Item[nTempHand].GetWidth();
			pInfo2.Region.Height = Item[nTempHand].GetHeight();
		}
		
		if (m_Hand == 0)
		{
			pInfo1.Obj.uGenre = CGOG_NOTHING;
			pInfo1.Obj.uId = 0;
			pInfo1.Region.Width = 0;
			pInfo1.Region.Height = 0;
		}
		else
		{
			pInfo1.Obj.uGenre = CGOG_ITEM;
			pInfo1.Obj.uId = m_Hand;
			pInfo1.Region.Width = Item[m_Hand].GetWidth();
			pInfo1.Region.Height = Item[m_Hand].GetHeight();
		}

		int PartConvert[itempart_num] = 
		{
			UIEP_HEAD,
			UIEP_BODY,
			UIEP_WAIST,
			UIEP_HAND,
			UIEP_FOOT,
			UIEP_FINESSE,
			UIEP_NECK,
			UIEP_FINGER1,
			UIEP_FINGER2,
			UIEP_WAIST_DECOR,
			UIEP_HORSE,
			UIEP_MASK,	// mat na
			UIEP_FIFONG, //#phi phong
			UIEP_SIGNET,
			UIEP_SHIPIN,
			UIEP_HOODS,
			UIEP_CLOAK,
		};

		int PartTrembleConvert[tremblepart_num] = 
		{
			UIEP_BLUEITEM,
			UIEP_GEMLEVEL,
			UIEP_GEMSPIRIT,
			UIEP_GEMMETAL,
			UIEP_GEMWOOD,
			UIEP_GEMWATER,
			UIEP_GEMFIRE,
			UIEP_GEMEARTH,
		};

		switch(SrcPos->nPlace)
		{
		case pos_immediacy:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_IMMEDIA_ITEM;
			pInfo2.eContainer = UOC_IMMEDIA_ITEM;
			break;
		case pos_equiproom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
			break;
		case pos_equip:
			pInfo1.Region.h = 0;
			pInfo1.Region.v = PartConvert[SrcPos->nX];
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartConvert[DesPos->nX];
			pInfo1.eContainer = UOC_EQUIPTMENT;
			pInfo2.eContainer = UOC_EQUIPTMENT;
			break;
		case pos_repositoryroom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX;
			pInfo2.eContainer = UOC_STORE_BOX;
			break;
		case pos_exbox1room: // ruong mo rong 1
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX1;
			pInfo2.eContainer = UOC_EX_BOX1;
			break;
		case pos_exbox2room: // ruong mo rong 2
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX2;
			pInfo2.eContainer = UOC_EX_BOX2;
			break;
		case pos_exbox3room: // ruong mo rong 3
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_EX_BOX3;
			pInfo2.eContainer = UOC_EX_BOX3;
			break;
		case pos_equiproomex: // mo rong hanh trang
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_EX;
			pInfo2.eContainer = UOC_ITEM_EX;
			break;
		case pos_traderoom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_TO_BE_TRADE;
			pInfo2.eContainer = UOC_TO_BE_TRADE;
			break;
		case pos_gambleroom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_TO_BE_GAMBLE;
			pInfo2.eContainer = UOC_TO_BE_GAMBLE;
			break;
		case pos_give:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_GIVE;
			pInfo2.eContainer = UOC_ITEM_GIVE;
			break;
		case pos_affairitem:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_AFFAIR_ITEM;
			pInfo2.eContainer = UOC_AFFAIR_ITEM;
			break;
		case pos_tremble: 
			pInfo1.Region.h = 0;
			pInfo1.Region.v = PartTrembleConvert[SrcPos->nX];
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartTrembleConvert[DesPos->nX];
			pInfo1.eContainer = UOC_TREMBLE_ITEM;
			pInfo2.eContainer = UOC_TREMBLE_ITEM;
			break;
		}
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo1, 0);
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo2, 0);
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo1, 1);
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo2, 1);
	}
	//
	if(m_HandSkill != nTempHandSkill)
	{
		KUiObjAtContRegion pInfo1, pInfo2;
		if (nTempHandSkill == 0)
		{
			pInfo2.Obj.uGenre = CGOG_NOTHING;
			pInfo2.Obj.uId = 0;
			pInfo2.Region.Width = 0;
			pInfo2.Region.Height = 0;
		}
		else
		{
			pInfo2.Obj.uGenre = CGOG_SKILL_FIGHT;	
			pInfo2.Obj.uId = nTempHandSkill;
			pInfo2.Region.Width = 1;
			pInfo2.Region.Height = 1;
		}
		
		if (m_HandSkill == 0)
		{
			pInfo1.Obj.uGenre = CGOG_NOTHING;
			pInfo1.Obj.uId = 0;
			pInfo1.Region.Width = 0;
			pInfo1.Region.Height = 0;
		}
		else
		{
			pInfo1.Obj.uGenre = CGOG_SKILL_FIGHT;
			pInfo1.Obj.uId = m_HandSkill;
			pInfo1.Region.Width = 1;
			pInfo1.Region.Height = 1;
		}
		switch(SrcPos->nPlace)
		{
		case pos_immediacy:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_IMMEDIA_ITEM;
			pInfo2.eContainer = UOC_IMMEDIA_ITEM;
			break;
		}
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo2, 0);
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo1, 1);
	}
#endif
	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendCommand(do_stand);
	}
}

#ifndef	_SERVER
BOOL	KItemList::AutoMoveItem(ItemPos SrcPos,ItemPos DesPos)
{
	if (Player[this->m_PlayerIdx].CheckTrading())	
		return FALSE;

	BOOL	bMove = FALSE;
	int		nIdx, nListIdx;

	switch (SrcPos.nPlace)
	{
	case pos_equiproom:
		{
			switch (DesPos.nPlace)
			{
			case pos_immediacy:
				{
					nIdx = m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					if (Item[nIdx].GetGenre() == item_medicine || Item[nIdx].GetGenre() == item_townportal)
					{
						//Khong lam gi het
					}
					else//Fix l鏸 s?d鬾g th?a ph?ph輒 t総
					{
						_ASSERT(0);
						return FALSE;
					}
					if (!m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					m_Room[room_immediacy].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = pos_immediacy;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			}
		}
		break;
	}

	if (!bMove)
		return bMove;

	KUiObjAtContRegion sSrcInfo, sDestInfo;

	sSrcInfo.Obj.uGenre		= CGOG_ITEM;
	sSrcInfo.Obj.uId		= nIdx;
	sSrcInfo.Region.Width	= Item[nIdx].GetWidth();
	sSrcInfo.Region.Height	= Item[nIdx].GetHeight();
	sSrcInfo.Region.h		= SrcPos.nX;
	sSrcInfo.Region.v		= SrcPos.nY;
	sSrcInfo.eContainer		= UOC_ITEM_TAKE_WITH;

	sDestInfo.Obj.uGenre	= CGOG_ITEM;
	sDestInfo.Obj.uId		= nIdx;
	sDestInfo.Region.Width	= Item[nIdx].GetWidth();
	sDestInfo.Region.Height	= Item[nIdx].GetHeight();
	sDestInfo.Region.h		= DesPos.nX;
	sDestInfo.Region.v		= DesPos.nY;
	sDestInfo.eContainer	= UOC_IMMEDIA_ITEM;

	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sSrcInfo, 0);
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sDestInfo, 1);

	return bMove;
}
#endif

#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
void	KItemList::MenuSetMouseItem()
{
	KUiObjAtContRegion	sInfo;
	if (!m_Hand)
	{
		CoreDataChanged(GDCNI_HOLD_OBJECT, 0, 0);
	}
	else
	{
		sInfo.Obj.uGenre = CGOG_ITEM;
		sInfo.Obj.uId = m_Hand;
		sInfo.Region.Width = Item[m_Hand].GetWidth();
		sInfo.Region.Height = Item[m_Hand].GetHeight();
		sInfo.Region.h = 0;
		sInfo.Region.v = 0;
		sInfo.eContainer = UOC_IN_HAND;
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&sInfo, 0);
	}
}
#endif

#ifdef _SERVER
BOOL KItemList::EatMecidine(int nPlace, int nX, int nY)
{
	int nItemIdx = 0;
	switch(nPlace)
	{
	case pos_equiproom:
		nItemIdx = m_Room[room_equipment].FindItem(nX, nY);
		if (nItemIdx > 0)
			return EatMecidine(nItemIdx);
		break;
	case pos_immediacy:
		nItemIdx = m_Room[room_immediacy].FindItem(nX, nY);
		if (nItemIdx > 0)
		{
			if (Item[nItemIdx].GetGenre() == item_medicine || Item[nItemIdx].GetGenre() == item_townportal) //Fix l鏸 s?d鬾g th?a ph?ph輒 t総
			{
				//Khong lam gi het
			}
			else
			{
				return EatMecidine(nItemIdx);
			}

			int		nGenre, nDetailType, nParticular, nIdx, nXpos, nYpos;
			BOOL	bEat;

			nGenre = Item[nItemIdx].GetGenre();					//Fix l鏸 s?d鬾g th?a ph?ph輒 t総
			nDetailType = Item[nItemIdx].GetDetailType();
			nParticular = Item[nItemIdx].GetParticular();

			bEat = EatMecidine(nItemIdx);
			if (bEat == FALSE)
				return bEat;

			if (FALSE == FindSameDetailTypeInEquipment(nGenre, nDetailType, nParticular, &nIdx, &nXpos, &nYpos))
				return bEat;

			this->AutoMoveMedicine(nIdx, nXpos, nYpos, nX, nY);

			return bEat;
		}
		break;
	default:
		break;
	}

	return FALSE;
}
#endif

PlayerItem* KItemList::GetFirstItem()
{
	m_nListCurIdx = m_UseIdx.GetNext(0);
	return &m_Items[m_nListCurIdx];
}

PlayerItem* KItemList::GetNextItem()
{
	if ( !m_nListCurIdx )
		return NULL;
	m_nListCurIdx = m_UseIdx.GetNext(m_nListCurIdx);
	return &m_Items[m_nListCurIdx];
}

void	KItemList::ClearRoom(int nRoom)
{
	if (nRoom >= 0 && nRoom < room_num)
		this->m_Room[nRoom].Clear();
}

void	KItemList::ClearAll() //m韎 th猰 v祇 sau n祔
{
	m_nItemsCount = 0;
	ClearRoom(room_equipment);
	ClearRoom(room_repository);
	ClearRoom(room_trade);
	ClearRoom(room_tradeback);
	ClearRoom(room_give);
	ClearRoom(room_giveback);
	
#ifndef _SERVER
	ClearRoom(room_trade1);
#endif

	ClearRoom(room_immediacy);
	ClearRoom(room_affairitem);

    m_Hand = 0;
	m_nBackHand = 0;
	m_HandSkill = 0;

	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));
	ZeroMemory(m_TrembleItem, sizeof(m_TrembleItem));
	ZeroMemory(m_Items, sizeof(m_Items));
	ZeroMemory(m_AltEquipmentItem, sizeof(m_AltEquipmentItem));

	m_nListCurIdx = 0;
	
	m_FreeIdx.Init(MAX_PLAYER_ITEM);
	m_UseIdx.Init(MAX_PLAYER_ITEM);

	for (int i = MAX_PLAYER_ITEM - 1; i > 0 ; i--)
	{
		m_FreeIdx.Insert(i);
	}
}

void	KItemList::BackupTrade()
{
	if ( !m_Room[room_tradeback].m_pArray )
		m_Room[room_tradeback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);
	memcpy(m_Room[room_tradeback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);

	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);

	m_nBackHand = m_Hand;
}

void	KItemList::RecoverTrade()
{
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_tradeback].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);

#ifndef _SERVER
	int nIdx = 0;
	while((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (m_Items[nIdx].nPlace == pos_trade1)
			Remove(m_Items[nIdx].nIdx);
	}
/*	for (int i = 0; i < MAX_PLAYER_ITEM; i++)
	{
		if (m_Items[i].nIdx && m_Items[i].nPlace == pos_trade1)
			Remove(m_Items[i].nIdx);
	}*/
#endif
	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}

void	KItemList::StartTrade()
{
	BackupTrade();
	ClearRoom(room_trade);
	ClearRoom(room_trade1);
}

void	KItemList::RestartTrade()
{
	BackupTrade();
}

/*!*****************************************************************************
// Function		: KItemList::RemoveAll
// Purpose		: 退出时清除所有的装备
// Return		: void
// Comments		: 会实际地从游戏世界中的道具数组中去掉
// Author		: Spe
*****************************************************************************/
void KItemList::RemoveAll()
{
	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		int nGameIdx = m_Items[nIdx].nIdx;
		Remove(m_Items[nIdx].nIdx);
#ifdef _SERVER
		// 客户端在上面KItemList::Remove()已经做了ItemSet.Remove()
		ItemSet.Remove(nGameIdx);
#endif
		nIdx = nIdx1;
	}

	ClearAll();	//m韎 th猰 v祇 sau n祔
}

int KItemList::GetWeaponParticular()
{
	if (m_EquipItem[itempart_weapon])
		return Item[m_EquipItem[itempart_weapon]].GetParticular();
	return -1;
}

#ifdef _SERVER
BOOL KItemList::IsTaskItemExist(int nDetailType,BYTE bType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (bType == 1)
		{
			int nGameIdx = m_Items[nIdx].nIdx;
			if (item_task != Item[nGameIdx].GetGenre())
				continue;
			if (nDetailType == Item[nGameIdx].GetDetailType())
			{
				return TRUE;
			}
		}
		else if (bType == 2)
		{
			int nGameIdx = m_Items[nIdx].nIdx;
			if (item_mine != Item[nGameIdx].GetGenre())
				continue;
			if (nDetailType == Item[nGameIdx].GetParticular())
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
int		KItemList::GetTaskItemNum(int nDetailType,BYTE bType)
{
	int		nNo = 0;
	int		nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (bType == 1)
		{
			if (item_task != Item[m_Items[nIdx].nIdx].GetGenre())
				continue;
			if (nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType())
			{
				nNo++;
			}
		}
		else if (bType == 2)
		{
			if (item_mine != Item[m_Items[nIdx].nIdx].GetGenre())
				continue;
			if (nDetailType == Item[m_Items[nIdx].nIdx].GetParticular())
			{
				nNo++;
			}
		}
	}
	return nNo;
}
#endif

#ifdef _SERVER
BOOL KItemList::RemoveTaskItem(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (item_task != Item[nGameIdx].GetGenre())
			continue;
		if (nDetailType == Item[nGameIdx].GetDetailType())
			{
				Remove(nGameIdx);
				return TRUE;
			}
	}
	return FALSE;
}



BOOL KItemList::RemoveMineItem(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (item_mine != Item[nGameIdx].GetGenre())
			continue;
		if (nDetailType == Item[nGameIdx].GetParticular())
		{
			Remove(nGameIdx);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
int KItemList::RemoveCommonItem(int nCount, int nItemNature, int nItemGenre, int nDetailType, int nItemParticular, int nLevel, int nSeries, int Place)
{
	int nIdx, nResult = 0;
	for (int i = 0; i < nCount; i++)
	{
		if (!FindSameToRemove(nItemNature, nItemGenre, nDetailType, nItemParticular, nLevel, nSeries, Place, &nIdx))
			break;

		if (nIdx)
		{
			if (Item[nIdx].IsStack() && 
				Item[nIdx].GetStackNum() > 1)
			{
				Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);
				this->SyncItem(nIdx);
				nResult ++;
				continue;
			}
			else
			{
				this->Remove(nIdx);
				ItemSet.Remove(nIdx);
				nResult ++;
				continue;
			}
		}
	}
	return nResult;
}
#endif

#ifdef _SERVER
void KItemList::InsertEquipment(int nIdx, bool bAutoStack)
{
	int x, y;
	if (CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y))
	{
		AddKIL(nIdx, pos_equiproom, x, y);
	}
	else
	{
		if  (Item[nIdx].GetPlayerItemLock())
		{
			POINT pPos;
			for(int nRoom = room_repository; nRoom <= room_repository+ Npc[Player[m_PlayerIdx].m_nIndex].m_ExItemId && nRoom <= room_exbox3; nRoom++)
			{
				if(m_Room[nRoom].FindRoom(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &pPos))
				{
					AddKIL(nIdx, pos_repositoryroom+nRoom-room_repository, pPos.x ,pPos.y);
					return;
				}
			}
		}

		int	nIndex = m_Hand;
		if (nIndex)
		{
			Remove(nIndex);
			
			KMapPos sMapPos;
			KObjItemInfo	sInfo;
			
			Player[m_PlayerIdx].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIndex;
			sInfo.m_nItemWidth = Item[nIndex].GetWidth();
			sInfo.m_nItemHeight = Item[nIndex].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIndex].GetGenre();
			sInfo.m_nDetailType = Item[nIndex].GetDetailType();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
			
			int nObj = ObjSet.Add(Item[nIndex].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task || Item[nIdx].GetGenre() == item_mine)
				{
					Object[nObj].SetEntireBelong(this->m_PlayerIdx);
				}
				else
				{
					Object[nObj].SetItemBelong(this->m_PlayerIdx);
				}
			}
		}
		AddKIL(nIdx, pos_hand, 0 ,0);
	}
}

void KItemList::SyncItem(int nIdx, int nPlace, int nX, int nY, int nPlayerIndex, bool m_bIsNew)
{
	ITEM_SYNC	sItem;
	sItem.ProtocolType = s2c_syncitem;
	sItem.m_bTemp = Item[nIdx].IsTemp();
	sItem.m_dwOwner = Item[nIdx].GetOwner();
	sItem.m_Nature = Item[nIdx].GetNature();
	if (Item[nIdx].GetNature() >= NATURE_GOLD)
		sItem.m_Detail = Item[nIdx].GetRow();
	else
		sItem.m_Detail = Item[nIdx].GetDetailType();
	sItem.m_Point = Item[nIdx].IsPurple();
	sItem.m_EnChance = Item[nIdx].GetEnChance();
	sItem.m_GoldId = Item[nIdx].GetGoldId();
	sItem.m_StackNum = Item[nIdx].GetStackNum();
	sItem.m_ID = Item[nIdx].GetID();
	sItem.m_Genre = Item[nIdx].GetGenre();
	sItem.m_Particur = Item[nIdx].GetParticular();
	sItem.m_Series = Item[nIdx].GetSeries();
	sItem.m_Level = Item[nIdx].GetLevel();
	sItem.m_Luck = Item[nIdx].m_GeneratorParam.nLuck;
	sItem.m_BackLocal = *Item[nIdx].GetBackLocal();
	sItem.m_btPlace = nPlace;
	sItem.m_btX = nX;
	sItem.m_btY = nY;
	sItem.m_Mantle = Item[nIdx].GetMantle(); //#phi phong
	sItem.m_Param = Item[nIdx].GetParam();	//#s?l莕 s?d鬾g
	sItem.m_GlowLight = Item[nIdx].GetItemGlowLight();	//#ngo筰 trang
	for (int j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
		sItem.m_MagicLevel[j] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[j];
	sItem.m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
	sItem.m_Version = Item[nIdx].m_GeneratorParam.nVersion;
	sItem.m_Durability = Item[nIdx].GetDurability();
	sItem.m_InsuranceCourse = Item[nIdx].GetPlayerItemLock();
	sItem.m_HInsuranceCourse = Item[nIdx].GetPlayerItemHLock();
	sItem.m_TimeE = Item[nIdx].GetTime()->bYear;
	sItem.m_bIsNew = m_bIsNew;
	sItem.m_LockItem = *Item[nIdx].GetLock();
	sItem.m_bLockSell = Item[nIdx].GetLockSell();
	sItem.m_bLockTrade = Item[nIdx].GetLockTrade();
	sItem.m_bLockDrop = Item[nIdx].GetLockDrop();
	sItem.m_Price = Item[nIdx].m_CommonAttrib.uPrice;
	sItem.m_Width = Item[nIdx].GetWidth();
	sItem.m_Height = Item[nIdx].GetHeight();
	sItem.m_Fortune = Item[nIdx].GetFortune();
	sItem.m_ExpireTime = Item[nIdx].GetExpireTime();
	sItem.m_MaxOptMultiply = Item[nIdx].GetMaxOptMultiply();
	if(nPlayerIndex)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
	else
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
}

void KItemList::SyncItemMagicAttrib(int nIdx)
{
	ITEM_SYNC_MAGIC sItem;
	sItem.ProtocolType = s2c_syncmagic;
	sItem.m_dwID = Item[nIdx].GetID();
	memcpy(sItem.m_MagicLevel, Item[nIdx].m_GeneratorParam.nGeneratorLevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
	memcpy(sItem.m_MagicAttrib, Item[nIdx].m_aryMagicAttrib, sizeof(sItem.m_MagicAttrib));
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC_MAGIC));
}

#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中把 trade room 中的 item 的 idx width height 信息写入 itemset 中的 m_psItemInfo 中去
//--------------------------------------------------------------------------
void	KItemList::GetTradeRoomItemInfo()
{
	_ASSERT(ItemSet.m_psItemInfo);
//	if (!ItemSet.m_psItemInfo)
//	{
//		ItemSet.m_psItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
//	}
	memset(ItemSet.m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[room_trade].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		_ASSERT(nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

		ItemSet.m_psItemInfo[nPos].m_nIdx = nItemIdx;
		ItemSet.m_psItemInfo[nPos].m_nWidth = Item[nItemIdx].GetWidth();
		ItemSet.m_psItemInfo[nPos].m_nHeight = Item[nItemIdx].GetHeight();
		nPos++;
	}

	// 从大到小排序
	TRADE_ITEM_INFO	sTemp;
	for (int i = nPos - 1; i >= 0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			if (ItemSet.m_psItemInfo[j].m_nWidth * ItemSet.m_psItemInfo[j].m_nHeight < 
				ItemSet.m_psItemInfo[j + 1].m_nWidth * ItemSet.m_psItemInfo[j + 1].m_nHeight)
			{
				sTemp = ItemSet.m_psItemInfo[j];
				ItemSet.m_psItemInfo[j] = ItemSet.m_psItemInfo[j + 1];
				ItemSet.m_psItemInfo[j + 1] = sTemp;
			}
		}
	}
}
#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中判断买进的物品能不能完全放进自己的物品栏
//--------------------------------------------------------------------------
BOOL	KItemList::TradeCheckCanPlace()
{
	LPINT	pnTempRoom;
	pnTempRoom = new int[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];
	memcpy(pnTempRoom, m_Room[room_equipment].m_pArray, sizeof(int) * EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT);

	int		nPos, i, j, a, b, nFind, nNext;
	for (nPos = 0; nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT; nPos++)
	{
		if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
			break;
		nFind = 0;
		for (i = 0; i < EQUIPMENT_ROOM_HEIGHT - ItemSet.m_psItemInfo[nPos].m_nHeight + 1; i++)
		{
			for (j = 0; j < EQUIPMENT_ROOM_WIDTH - ItemSet.m_psItemInfo[nPos].m_nWidth + 1; j++)
			{
				nNext = 0;
				for (a = 0; a < ItemSet.m_psItemInfo[nPos].m_nHeight; a++)
				{
					for (b = 0; b < ItemSet.m_psItemInfo[nPos].m_nWidth; b++)
					{
						if (pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b])
						{
							nNext = 1;
							break;
						}
					}
					if (nNext)
						break;
				}
				// 找到一个位置
				if (!nNext)
				{
					// 数据处理
					ItemSet.m_psItemInfo[nPos].m_nX = j;
					ItemSet.m_psItemInfo[nPos].m_nY = i;
					for (a = 0; a < ItemSet.m_psItemInfo[nPos].m_nHeight; a++)
					{
						for (b = 0; b < ItemSet.m_psItemInfo[nPos].m_nWidth; b++)
							pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b] = ItemSet.m_psItemInfo[nPos].m_nIdx;
					}

					nFind = 1;
					break;
				}
			}
			if (nFind)
				break;
		}
		if (!nFind)
		{
			delete []pnTempRoom;
			return FALSE;
		}
	}

	delete []pnTempRoom;
	return TRUE;
}
#endif
#ifdef _SERVER

BOOL	KItemList::CheckItemEquipCS()
{





for (int i = 0; i < MAX_PLAYER_ITEM; i++)
{
int nIdx = m_Items[i].nIdx;
int nPlace = m_Items[i].nPlace;
int nItemGenre = Item[i].GetGenre();
int nDetailType = Item[i].GetDetailType();

	if (nIdx > 0 && nIdx < MAX_ITEM && nPlace == pos_equip && nItemGenre == 0 && nDetailType != 10)
	{

		return FALSE;
		}
	}
	return TRUE;
}
#endif

//--------------------------------------------------------------------------
//	功能：判断一定长宽的物品能否放进物品栏 (为了服务器效率，本函数里面没有调用其他函数)
//--------------------------------------------------------------------------
BOOL	KItemList::CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnX, int *pnY, int nRoom)
{
	if (nWidth <= 0 || nHeight <= 0 || !pnX || !pnY)
		return FALSE;

	_ASSERT(m_Room[nRoom].m_pArray);

	LPINT	pnTempRoom;
	int		i, j, a, b, nNext;

	pnTempRoom = m_Room[nRoom].m_pArray;

	for (i = 0; i < EQUIPMENT_ROOM_HEIGHT - nHeight + 1; i++)
	{
		for (j = 0; j < EQUIPMENT_ROOM_WIDTH - nWidth + 1; j++)
		{
			nNext = 0;
			for (a = 0; a < nHeight; a++)
			{
				for (b = 0; b < nWidth; b++)
				{
					if (pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b])
					{
						nNext = 1;
						break;
					}
				}
				if (nNext)
					break;
			}
			if (!nNext)
			{
				*pnX = j;
				*pnY = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

#ifdef _SERVER
BOOL	KItemList::FindSameToRemove(int nItemNature, int nItemGenre, int nDetailType, int nItemParticular, int nLevel, int nSeries, int Place, int *pnIdx)
{
	return m_Room[PositionToRoom(Place)].FindSameToRemove(nItemNature, nItemGenre, nDetailType, nItemParticular, nLevel, nSeries, pnIdx);
}
#endif

BOOL	KItemList::FindSameDetailTypeInEquipment(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY)
{
	return m_Room[room_equipment].FindSameDetailType(nGenre, nDetail, nParticular, pnIdx, pnX, pnY);
}

BOOL	KItemList::FindSameDetailTypeInImmediacy(int nGenre, int nDetail, int nParticular, int *pnIdx, int *pnX, int *pnY)
{
	return m_Room[room_immediacy].FindSameDetailType(nGenre, nDetail, nParticular, pnIdx, pnX, pnY);
}

#ifndef _SERVER
BOOL KItemList::IsTaskItemExist(int nDetailType)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (item_task != Item[nGameIdx].GetGenre())
			continue;
		if (nDetailType == Item[nGameIdx].GetDetailType())
		{
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
//------------------------------------------------------------------------------
//	功能：自动把一个药品从room_equipment移动到room_immediacy
//------------------------------------------------------------------------------
BOOL	KItemList::AutoMoveMedicine(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY)
{
	if (!m_Room[room_equipment].m_pArray || !m_Room[room_immediacy].m_pArray)
		return FALSE;
	if (nSrcX < 0 || nSrcX >= m_Room[room_equipment].m_nWidth || nSrcY < 0 || nSrcY >= m_Room[room_equipment].m_nHeight)
		return FALSE;
	if (nDestX < 0 || nDestX >= m_Room[room_immediacy].m_nWidth || nDestY < 0 || nDestY >= m_Room[room_immediacy].m_nHeight)
		return FALSE;
	if (nItemIdx != m_Room[room_equipment].m_pArray[nSrcY * m_Room[room_equipment].m_nWidth + nSrcX] ||
		0 != m_Room[room_immediacy].m_pArray[nDestY * m_Room[room_immediacy].m_nWidth + nDestX])
		return FALSE;

	_ASSERT(Item[nItemIdx].GetWidth() == 1 && Item[nItemIdx].GetHeight() == 1);
	if (!m_Room[room_equipment].PickUpItem(nItemIdx, nSrcX, nSrcY, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
		return FALSE;
	if (!m_Room[room_immediacy].PlaceItem(nDestX, nDestY, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
	{
		m_Room[room_equipment].PlaceItem(nSrcX, nSrcY, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight());
		return FALSE;
	}

	int nListIdx = FindSame(nItemIdx);
	_ASSERT(nListIdx > 0);
	m_Items[nListIdx].nPlace = pos_immediacy;
	m_Items[nListIdx].nX = nDestX;
	m_Items[nListIdx].nY = nDestY;

	ITEM_AUTO_MOVE_SYNC	sMove;
	sMove.ProtocolType = s2c_ItemAutoMove;
	sMove.m_btSrcPos = pos_equiproom;
	sMove.m_btSrcX = nSrcX;
	sMove.m_btSrcY = nSrcY;
	sMove.m_btDestPos = pos_immediacy;
	sMove.m_btDestX = nDestX;
	sMove.m_btDestY = nDestY;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(ITEM_AUTO_MOVE_SYNC));

	return TRUE;
}
#endif

#ifndef	_SERVER
void	KItemList::RemoveAllInOneRoom(int nRoom)
{
	if (nRoom < 0 || nRoom >= room_num)
		return;

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[nRoom].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		_ASSERT(nPos < m_Room[nRoom].m_nWidth * m_Room[nRoom].m_nHeight);

		Remove(nItemIdx);

		nPos++;
	}
}
#endif

#ifndef _SERVER
void KItemList::LockOperation()
{
	if (IsLockOperation())
	{
		_ASSERT(0);
		return;
	}
	m_nLockTimeout = GetTickCount();
	m_bLockOperation = TRUE;
}
#endif

#ifndef _SERVER
void KItemList::UnlockOperation()
{
	if (!IsLockOperation())
	{
		return;
	}
	m_bLockOperation = FALSE;
}
#endif

int KItemList::GetActiveAttribNum(int nIdx)
{
	for (int i = 0; i < itempart_num; i++)
	{
		if (nIdx == m_EquipItem[i])
		{
			return GetEquipEnhance(i);
		}
	}
	return 0;
}

int KItemList::GetGoldActiveAttribNum(int nIdx)
{
	for (int i = 0; i < itempart_num; i++)
	{
		if (i >= itempart_horse && nIdx == m_EquipItem[i])
		{
			return MAX_ITEM_MAGICATTRIB / 2; //do tu` ngua. tro di kich 4 dong
		}
		if (nIdx == m_EquipItem[i])
		{
			return GetGoldEquipEnhance(i);
		}
	}
	return 0;
}

#ifdef _SERVER
BOOL KItemList::RemoveItemIdx(int nGameIdx,int nNum)
{
	if (nGameIdx <= 0)
		return FALSE;

	int nListIndex = FindSame(nGameIdx);
	if (!nListIndex)
	{
		_ASSERT(0);
		return FALSE;
	}

	int nStackNum = Item[nGameIdx].GetStackNum();

	if (nNum > nStackNum)
		nNum = nStackNum;

	if (nStackNum == nNum)
	{
		Remove(nGameIdx);
		ItemSet.Remove(nGameIdx);
	}
	else
	{
		Item[nGameIdx].SetStackNum(nStackNum - nNum);
		this->SyncItem(nGameIdx);
	}
	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------------
//	功能：丢失随身物品
//-------------------------------------------------------------------------------
void	KItemList::AutoLoseItemFromEquipmentRoom(int nRate)
{
	if (Player[m_PlayerIdx].CheckTrading())
		return;
	if (nRate <= 0 || nRate > 100)
		return;

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	// 先统计所有物品，排除任务物品
	while (1)
	{
		nItemIdx = m_Room[room_equipment].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		if (item_task == Item[nItemIdx].GetGenre())
			continue;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nIdx = nItemIdx;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nPlace = pos_equiproom;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nX = nXpos;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nY = nYpos;
		nPos++;
	}
	if (nPos == 0)
		return;

	KMapPos			sMapPos;
	int				nSelect;
	int				nObj;
	KObjItemInfo	sInfo;

	for (int i = 0; i < nPos; i++)
	{
		if (g_Random(100) >= nRate)
			continue;
		nItemIdx = ItemSet.m_sLoseItemFromEquipmentRoom[i].nIdx;
		Player[m_PlayerIdx].GetAboutPos(&sMapPos);
		// 丢弃
		if (Remove(nItemIdx))
		{
			sInfo.m_nItemID = nItemIdx;
			sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nItemIdx].GetGenre() != item_equip && Item[nItemIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nItemIdx].GetName(), Item[nItemIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
			sInfo.m_nColorID = Item[nItemIdx].GetColorItem();
			sInfo.m_nGenre = Item[nItemIdx].GetGenre();
			sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nItemIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;

			nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				Object[nObj].SetItemBelong(-1);
			}

			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_DEATH_LOSE_ITEM;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(sInfo.m_szName);
			sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
			memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
			memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), sInfo.m_szName, sizeof(sInfo.m_szName));
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		}
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------------
void	KItemList::AutoLoseEquip()
{
#ifndef defEQUIP_POWER
	int		i, nPos = 0;

	for (i = 0; i < itempart_num; i++)
	{
		if (m_EquipItem[i] <= 0)
			continue;
		if (i == itempart_horse)		//edit by phong kieu pk khi bi chet khong cho rot ngua ra khoi nguoi
			continue;
		ItemSet.m_sLoseEquipItem[nPos].nIdx = m_EquipItem[i];
		ItemSet.m_sLoseEquipItem[nPos].nPlace = nPos;
		nPos++;
	}
	if (nPos == 0)
		return;

	int		nSelect = g_Random(nPos);
#endif

#ifdef defEQUIP_POWER
	int		i, nPos = 0, nTotalPower = 0;

	for (i = 0; i < itempart_num; i++)
	{
		if (m_EquipItem[i] <= 0)
			continue;
		nTotalPower += g_nEquipPower[i];
		ItemSet.m_sLoseEquipItem[nPos].nIdx = m_EquipItem[i];
		ItemSet.m_sLoseEquipItem[nPos].nPlace = nPos;
		ItemSet.m_sLoseEquipItem[nPos].nX = nTotalPower;	// nX 借用一下
		nPos++;
	}
	if (nTotalPower == 0)
		return;
	int		nSelect = g_Random(nTotalPower);
	for (i = 0; i < nPos; i++)
	{
		if (ItemSet.m_sLoseEquipItem[i].nX > nSelect)
		{
			nSelect = i;
			break;
		}
	}
	if (i >= nPos)
		return;
#endif

	int			 nItemIdx;
	KMapPos		sMapPos;

	nItemIdx = ItemSet.m_sLoseEquipItem[nSelect].nIdx;
	Player[m_PlayerIdx].GetAboutPos(&sMapPos);
	if (Remove(nItemIdx))
	{
		int		nObj;
		KObjItemInfo	sInfo;
		sInfo.m_nItemID = nItemIdx;
		sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
		sInfo.m_nMoneyNum = 0;
		char szNameTemp[OBJ_NAME_LENGHT];
		if (Item[nItemIdx].GetGenre() != item_equip && Item[nItemIdx].GetStackNum() > 1)
		{
			sprintf(szNameTemp, "%s x %d", Item[nItemIdx].GetName(), Item[nItemIdx].GetStackNum());
			strcpy(sInfo.m_szName, szNameTemp);
		}
		else
			strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
		sInfo.m_nColorID = Item[nItemIdx].GetColorItem();
		sInfo.m_nGenre = Item[nItemIdx].GetGenre();
		sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
		sInfo.m_nParticularType = Item[nItemIdx].GetParticular();
		sInfo.m_nMovieFlag = 1;
		sInfo.m_nSoundFlag = 1;
		sInfo.m_dwNpcId1 = 0;

		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
		if (nObj >= 0)
		{
			Object[nObj].SetItemBelong(-1);
		}

		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_DEATH_LOSE_ITEM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(sInfo.m_szName);
		sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
		memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
		memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), sInfo.m_szName, sizeof(sInfo.m_szName));
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	}
}
#endif

#ifndef _SERVER
int	KItemList::GetSameDetailItemNum(int nImmediatePos)
{
	if (nImmediatePos < 0 || nImmediatePos >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return 0;

	int	nIdx = m_Room[room_immediacy].FindItem(nImmediatePos, 0);
	if (nIdx <= 0)
		return 0;

	return m_Room[room_equipment].CalcSameItemType(Item[nIdx].GetGenre(), Item[nIdx].GetDetailType(), 
		Item[nIdx].GetParticular(), Item[nIdx].GetLevel()) + Item[nIdx].GetStackNum();
}
#endif

#ifdef _SERVER
void KItemList::Abrade(int nType, BOOL isDeathPunish) //#m礽 m遪 item
{
	int nItemIdx = 0;
	for (int i = 0; i < itempart_num; i++)
	{
		nItemIdx = m_EquipItem[i];
		if (nItemIdx)
		{
			int nOldDur = Item[nItemIdx].GetDurability();
			if(nOldDur ==0)
				continue;
			int m_sAbradeP = PlayerSet.m_sPKPunishParam[Player[m_PlayerIdx].m_cPK.GetPKValue()].m_nAbradeP;
			int m_sAbradeRange = ItemSet.GetAbradeRange(nType, i);
			if(!isDeathPunish)					//#khi bi pk chet moi tinh them mai mon trang bi
				m_sAbradeP = 0;
			int nDur = Item[nItemIdx].Abrade(m_sAbradeP, m_sAbradeRange); //pk bi hong item
			if(nDur == -1)
				continue;
			if (nOldDur != nDur)
			{
				ITEM_DURABILITY_CHANGE sIDC;
				sIDC.ProtocolType = s2c_itemdurabilitychange;
				sIDC.dwItemID = Item[nItemIdx].GetID();
				sIDC.nChange = nDur - nOldDur;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sIDC, sizeof(ITEM_DURABILITY_CHANGE));
			}
			if (nDur == 0) //#trang bi hong thanh cuc sat trang b?h醤g th祅h c鬰 s総
			{
				//
				//Item[nItemIdx].m_CommonAttrib.nWidth = 1; //i l筰 w = 1 h = 1
				//Item[nItemIdx].m_CommonAttrib.nHeight = 1;
				//
				Remove(nItemIdx);
				InsertEquipment(nItemIdx);
				//
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_ITEM_DAMAGED;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
				sMsg.m_lpBuf = (void *)Item[nItemIdx].m_dwID;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
				sMsg.m_lpBuf = 0;
			}
		}
	}
}
#endif

BOOL KItemList::GetIfActive()
{
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	int nIdx1 = 0;
	int nIdx2 = 0;
	int i = 0;
	int j = 0;
	int nCountItemInGroup = 0;
	BOOL bActive = FALSE;
	for (i = 0; i < itempart_num; i++)
	{
		nIdx1 = m_EquipItem[i];
		if (!nIdx1 || Item[nIdx1].GetKind() != gold_item)
			continue;
		if (!nIdx1 || Item[nIdx1].GetLine() < 0)
			continue;
		
		if (i == itempart_ring1)
		{
			if (Item[m_EquipItem[itempart_ring1]].GetLine() == Item[m_EquipItem[itempart_ring2]].GetLine())
				continue;
		}

		nCountItemInGroup = 1;
		if (Item[nIdx1].GetSet() > 0 && Item[nIdx1].GetSetNum() > 0)
		{
			for (j = i+1; j < itempart_num; j++)
			{
				nIdx2 = m_EquipItem[j];
				if (!nIdx2 || Item[nIdx1].GetKind() != gold_item)
					continue;

				if (Item[nIdx2].GetLine() < 0)
					continue;
				
				if (j == itempart_ring1)
				{
					if (Item[m_EquipItem[itempart_ring1]].GetLine() == Item[m_EquipItem[itempart_ring2]].GetLine())
						continue;
				}

				if (Item[nIdx1].GetSet() == Item[nIdx2].GetSet())
				{
					if (Item[nIdx1].GetLine() != Item[nIdx2].GetLine())
					{
						nCountItemInGroup++;
					}
				}
			}
			if (nCountItemInGroup >= Item[nIdx1].GetSetNum()) //mac du do thi kich
				bActive = 1;
			if (nCountItemInGroup >= Item[nIdx1].GetSetNum()) //mac du do thi kich new goldequip TODO
				bActive = 1;
		}
	}
	if (m_bActiveSet != bActive)
	{
		m_bActiveSet = bActive;
	}
	return m_bActiveSet;
}

BOOL KItemList::GetIfActive(int nItemIdx)
{
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	int nIdx1 = 0;
	int nIdx2 = 0;
	int i = 0;
	int j = 0;
	int nCountItemInGroup = 0;
	BOOL bActive = FALSE;
	nCountItemInGroup = 1;
	for (i = 0; i < itempart_num; i++)
	{
		nIdx1 = m_EquipItem[i];
		if (!nIdx1 || Item[nIdx1].GetKind() != gold_item) //GOLD or PLATINA
			continue;
		if (!nIdx1 || Item[nIdx1].GetLine() < 0)
			continue;

		if (i == itempart_ring1)
		{
			if (Item[m_EquipItem[itempart_ring1]].GetLine() == Item[m_EquipItem[itempart_ring2]].GetLine())
				continue;
		}

		if (Item[nIdx1].GetSet() == Item[nItemIdx].GetSet())
		{
			if (Item[nIdx1].GetLine() != Item[nIdx2].GetLine())
			{
				nCountItemInGroup++;
			}
			if (nCountItemInGroup >= Item[nIdx1].GetSetNum()) //mac du do thi kich
				bActive = TRUE;
		}
	}
	return bActive;
}

#ifndef _SERVER
int KItemList::GetGoldColor(int nSet, int nId)
{
	int i = 0;
	for (i = 0;i < itempart_num;i++)
	{
		if (Item[m_EquipItem[i]].GetSet() == nSet && Item[m_EquipItem[i]].GetSetId() == nId)
		{
			return 2;
		}
	}

	for (i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		if (Item[m_Items[i].nIdx].GetSet() == nSet && Item[m_Items[i].nIdx].GetSetId() == nId)
		{
			return 1;
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
void KItemList::SetLevelItem(int nIdx, int nLevel)
{	
	if(!nIdx)
		return;

	if(nLevel > 10)
		nLevel = 10;

	int nX = 0;
	int nY = 0;
	if(!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &nX, &nY))
	{	
		nX = 0;
		nY = 0;
		return;
	}

	Item[nIdx].SetLevel(nLevel);

	int nIndex = ItemSet.AddI(&Item[nIdx]);
	if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
	{
		AddKIL(nIndex, pos_equiproom, nX, nY);
		Remove(nIdx);
		ItemSet.Remove(nIdx);
	}

	nX = 0;
	nY = 0;
	nIndex = 0;
}
//---------------------------------------------------
// Thay doi he vat pham
//---------------------------------------------------
void KItemList::SetSeriesItem(int nIdx, int nSeries)
{	
	if(!nIdx)
		return;

	if(nSeries > 4)
		nSeries = 4;

	int nX = 0;
	int nY = 0;
	if(!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &nX, &nY))
	{	
		nX = 0;
		nY = 0;
		return;
	}

	Item[nIdx].SetSeries(nSeries);

	int nIndex = ItemSet.AddI(&Item[nIdx]);
	if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
	{
		AddKIL(nIndex, pos_equiproom, nX, nY);
		Remove(nIdx);
		ItemSet.Remove(nIdx);
	}

	nX = 0;
	nY = 0;
	nIndex = 0;
}
//----------------------------------------------------
// Thay doi option vat pham
//----------------------------------------------------
void KItemList::ChangeSpiritItem(int nIdx, int nLevelMagic)
{	
	if(!nIdx)
		return;

	int nItemLevel[6];
	int nX = 0;
	int nY = 0;
	memset(&nItemLevel, 0, sizeof(nItemLevel));
	if(!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &nX, &nY))
	{	
		nX = 0;
		nY = 0;
		return;
	}
	for (int i = 0; i < 6; i ++)
		nItemLevel[i] = nLevelMagic;

	int nIndex = ItemSet.AddItemSet2(Item[nIdx].GetGenre(), Item[nIdx].GetSeries(), Item[nIdx].GetLevel(), 10, 
		Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), nItemLevel, g_SubWorldSet.GetGameVersion(), 0,1,0,0,0,0,0,0,0,0, Item[nIdx].GetMaxOptMultiply());
	if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
	{
		AddKIL(nIndex, pos_equiproom, nX, nY);
		Remove(nIdx);
		ItemSet.Remove(nIdx);
	}

	memset(&nItemLevel, 0, sizeof(nItemLevel));
	nX = 0;
	nY = 0;
	nIndex = 0;
}

void KItemList::SetPointPurpleItem(int nIdx, int nPoint)// Tao ra do tim
{	
	if(!nIdx)
		return;
	if(Item[nIdx].GetGenre() != item_equip)
		return;
	if(nPoint < 1 || nPoint > 6)
		return;

	int i = 0;
	int nX;
	int nY;
	if(!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &nX, &nY))
	{	
		nX = 0;
		nY = 0;
		return;
	}

	//Item[nIdx].SetPointPurple(nPoint);
	
	for(i = 0; i < 6; i++)
	{	
		if(i < nPoint)
		{
			//Item[nIdx].m_aryMagicAttrib[i].nAttribType = magic_item_reserve10;
			Item[nIdx].m_aryMagicAttrib[i].nValue[0] = 0;
			Item[nIdx].m_aryMagicAttrib[i].nValue[1] = 0;
			Item[nIdx].m_aryMagicAttrib[i].nValue[2] = 0;
		}
		else
		{
			Item[nIdx].m_aryMagicAttrib[i].nAttribType = 0;
			Item[nIdx].m_aryMagicAttrib[i].nValue[0] = 0;
			Item[nIdx].m_aryMagicAttrib[i].nValue[1] = 0;
			Item[nIdx].m_aryMagicAttrib[i].nValue[2] = 0; 
		}
	}
	//int nIndex = ItemSet.Add(Item[nIdx].GetGenre(), Item[nIdx].GetSeries(), Item[nIdx].GetLevel(), Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), Item[nIdx].GetPointPurple(), Item[nIdx].m_aryMagicAttrib, Item[nIdx].GetStackNum());
	//if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
	//{
	//	Add(nIndex, pos_equiproom, nX, nY);
	//	Remove(nIdx);
	//	ItemSet.Remove(nIdx);
	//}

	i = 0;
	nX = 0;
	nY = 0;
	//nIndex = 0;
}

void KItemList::SetMagic2Item(int nIdx, int nType, int nOption /*= 0*/, int nLevel /*= 1*/) // Gan option vao item
{
	if(!nIdx)
		return;
	int nGenre = Item[nIdx].GetGenre();
	
	if(nGenre != item_mine && nGenre != item_equip)
		return;
	
	int nX = 0;
	int nY = 0;
	if(!CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &nX, &nY))
	{	
		nX = 0;
		nY = 0;
		return;
	}
	switch(nGenre)
	{
		case item_equip:
			{
				//if(nOption > Item[nIdx].GetPointPurple() - 1)
				//	return;

				//if(!Item[nIdx].CanSetMagic2Item(nType))
				//	return;
				
				int nPos = 0;
				if(nOption == 0 || nOption == 2 || nOption == 4)
				{
					nPos = 1;
				}
				else
				{
					nPos = 0;
				}
				const KMAGICATTRIB_TABFILE* pData;
				//pData = ItemGen.Gen_OptionMagic(nPos, nType, nLevel);
				
				/*if (pData == NULL)
				{
					for (int i = nLevel + 1; i < 11; i++)
					{
						pData = ItemGen.Gen_OptionMagic(nPos, nType, i);
						if (pData != NULL)
							break;
					}
				}
 
				if (pData != NULL)
				{	
					Item[nIdx].m_aryMagicAttrib[nOption].nAttribType = pData->m_MagicAttrib.nPropKind;
					Item[nIdx].m_aryMagicAttrib[nOption].nValue[0] = ::GetRandomNumber(pData->m_MagicAttrib.aryRange[0].nMin,pData->m_MagicAttrib.aryRange[0].nMax);;
					Item[nIdx].m_aryMagicAttrib[nOption].nValue[1] = 0;
					Item[nIdx].m_aryMagicAttrib[nOption].nValue[2] = 0;
				}*/
			} 
			break;
		case item_mine:
			{
				//Item[nIdx].SetPointPurple(1);
				if(nOption < 0 || nOption > 10)
				{
					nOption = Item[nIdx].GetLevel();
				}
				Item[nIdx].SetLevel(nOption); 

				Item[nIdx].m_aryMagicAttrib[0].nAttribType = nType;
				Item[nIdx].m_aryMagicAttrib[0].nValue[0] = 0;
				Item[nIdx].m_aryMagicAttrib[0].nValue[1] = 0;
				Item[nIdx].m_aryMagicAttrib[0].nValue[2] = 0;
			}
			break;
	}
	
	//int nIndex = ItemSet.Add(Item[nIdx].GetGenre(), Item[nIdx].GetSeries(), Item[nIdx].GetLevel(), Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), Item[nIdx].GetPointPurple(), Item[nIdx].m_aryMagicAttrib, Item[nIdx].GetStackNum());
	//if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
	//{
	//	Add(nIndex, pos_equiproom, nX, nY);
	//	Remove(nIdx);
	//	ItemSet.Remove(nIdx);
	//}

	nX = 0;
	nY = 0;
	//nIndex = 0;
}
#endif

#ifdef _SERVER
void KItemList::SetPriceFromScript( int nIdx, int nPrice)//#fix bay ban trang bi khoa bao hiem
{
	int nId = FindSame(nIdx);
	if (nPrice)
	{
		m_Items[nId].nPrice = nPrice;
	} 
	else
	{
		m_Items[nId].nPrice = 0;
	}
}

void KItemList::SetPrice( int nIdx, int nPrice)
{
	int nGameId = SearchID(nIdx);
	int nId = FindSame(nGameId);
	if (nPrice)
	{
		m_Items[nId].nPrice = nPrice;
		Item[nGameId].m_CommonAttrib.uPrice = nPrice;
	} 
	else
	{
		m_Items[nId].nPrice = 0;
	}
}

int KItemList::GetPrice( int nGameIdx )
{
	int nId = FindSame(nGameIdx);
	if (nId)
		return m_Items[nId].nPrice;
	else
		return 0;
}

void KItemList::CheckItemTime()
{

	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{

		if (Item[m_Items[i].nIdx].GetTime()->bYear)
		if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 < timeinfo->tm_year)
		{
			Remove(m_Items[i].nIdx);
			ItemSet.Remove(m_Items[i].nIdx);
		}
		else if (Item[m_Items[i].nIdx].GetTime()->bYear - 1900 == timeinfo->tm_year)
			if (Item[m_Items[i].nIdx].GetTime()->bMonth < timeinfo->tm_mon + 1)
			{
				Remove(m_Items[i].nIdx);
				ItemSet.Remove(m_Items[i].nIdx);
			}
			else if (Item[m_Items[i].nIdx].GetTime()->bMonth == timeinfo->tm_mon + 1)
				if (Item[m_Items[i].nIdx].GetTime()->bDay < timeinfo->tm_mday)
				{
					Remove(m_Items[i].nIdx);
					ItemSet.Remove(m_Items[i].nIdx);
				}
				else if (Item[m_Items[i].nIdx].GetTime()->bDay == timeinfo->tm_mday)
					if (Item[m_Items[i].nIdx].GetTime()->bHour <= timeinfo->tm_hour)
					{
						Remove(m_Items[i].nIdx);
						ItemSet.Remove(m_Items[i].nIdx);
					}
	}
}

BOOL KItemList::RemoveGoldItem( int nDetailType )
{
	if (nDetailType <= 0)
	{
		return FALSE;
	}
	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		if (m_Items[i].nPlace != room_equipment)
		{
			continue;
		}
		if (Item[m_Items[i].nIdx].GetGoldId() == nDetailType)
		{
			Remove(m_Items[i].nIdx);
			ItemSet.Remove(m_Items[i].nIdx);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KItemList::IsGoldItemExist( int nDetailType )
{
	if (nDetailType <= 0)
	{
		return FALSE;
	}
	for (int i = 0;i < MAX_PLAYER_ITEM;i++)
	{
		if (m_Items[i].nPlace != room_equipment)
		{
			continue;
		}
		if (Item[m_Items[i].nIdx].GetGoldId() == nDetailType)
		{
			return TRUE;
		}
	}
	return FALSE;
}
#endif

void KItemList::SetMaskLock( BOOL bFlag )//#mat na
{
	m_nMaskLock = bFlag;
#ifdef _SERVER
	NPC_SIT_SYNC Sync;
	Sync.ProtocolType = s2c_syncmasklock;
	Sync.ID = m_nMaskLock;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&Sync, sizeof(NPC_SIT_SYNC));
#endif
}

BOOL KItemList::IsEnoughToActive()
{
	int nArray[itempart_horse][3], nCnt = 0, nFlg = FALSE;
	ZeroMemory(nArray, sizeof(nArray));

	for (int i = 0; i < itempart_horse; i++)
	{
		nFlg = FALSE;
		if (m_EquipItem[i] && Item[m_EquipItem[i]].m_CommonAttrib.nItemNature >= NATURE_GOLD)
		{
			if (i == itempart_ring1)
			{
				if (m_EquipItem[itempart_ring1] &&
					m_EquipItem[itempart_ring2] &&
					/*Item[m_EquipItem[itempart_ring1]].GetNature() == Item[m_EquipItem[itempart_ring2]].GetNature() && */
					Item[m_EquipItem[itempart_ring1]].GetGroup() == Item[m_EquipItem[itempart_ring2]].GetGroup() &&
					Item[m_EquipItem[itempart_ring1]].GetSetID() == Item[m_EquipItem[itempart_ring2]].GetSetID())
					continue;
			}
			for (int j = 0; j < itempart_horse; j++)
			{
				if (/*nArray[j][0] == Item[m_EquipItem[i]].GetNature() && */
					nArray[j][1] == Item[m_EquipItem[i]].GetGroup())
				{
					nArray[j][2]++;
					nFlg = TRUE;
					if (nArray[j][2] >= Item[m_EquipItem[i]].GetNeedToActive2())
						return TRUE;
				}
			}
			if (nFlg == FALSE)
			{
				nArray[nCnt][0] = Item[m_EquipItem[i]].GetNature();
				nArray[nCnt][1] = Item[m_EquipItem[i]].GetGroup();
				nArray[nCnt][2] = 1;

				if (nArray[nCnt][2] >= Item[m_EquipItem[i]].GetNeedToActive2())
					return TRUE;
				nCnt++;
			}
		}
	}
	return FALSE;
}

#ifdef _SERVER
BOOL KItemList::IsItemExist( int nGern,int nDetailType,int nPar,int nSerise,int nLevel )
{
	int nIdx = 0;
	while((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (Item[m_Items[nIdx].nIdx].GetGenre() != nGern ||
			Item[m_Items[nIdx].nIdx].GetDetailType() != nDetailType ||
			Item[m_Items[nIdx].nIdx].GetParticular() != nPar
			|| Item[m_Items[nIdx].nIdx].GetGoldId() || Item[m_Items[nIdx].nIdx].IsPurple())
			continue;		

		if (nSerise < 5)
			if (Item[m_Items[nIdx].nIdx].GetSeries() != nSerise)
				continue;

		if (nLevel != 0)
			if (Item[m_Items[nIdx].nIdx].GetLevel() != nLevel)
				continue;

		return TRUE;
	}
	return FALSE;
}

BOOL KItemList::DelExistItem( int nGern,int nDetailType,int nPar,int nSerise /*= 5*/,int nLevel /*= 0*/ )
{
	int nIdx = 0;
	while((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (Item[m_Items[nIdx].nIdx].GetGenre() != nGern ||
			Item[m_Items[nIdx].nIdx].GetDetailType() != nDetailType ||
			Item[m_Items[nIdx].nIdx].GetParticular() != nPar
			|| Item[m_Items[nIdx].nIdx].GetGoldId() || Item[m_Items[nIdx].nIdx].IsPurple())
			continue;		
		
		if (nSerise < 5)
			if (Item[m_Items[nIdx].nIdx].GetSeries() != nSerise)
				continue;
			
			if (nLevel != 0)
				if (Item[m_Items[nIdx].nIdx].GetLevel() != nLevel)
					continue;
				
		Remove(m_Items[nIdx].nIdx);
		ItemSet.Remove(m_Items[nIdx].nIdx);
		return TRUE;
	}
	return FALSE;
}
#endif

void KItemList::StartGive()
{
	BackupGive();
	ClearRoom(room_give);
}

void KItemList::BackupGive()
{
	if ( !m_Room[room_giveback].m_pArray )
		m_Room[room_giveback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);
	memcpy(m_Room[room_giveback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_giveback].m_nWidth * m_Room[room_giveback].m_nHeight);
	
	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	
	m_nBackHand = m_Hand;
}

void KItemList::RecoverGive()
{
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_giveback].m_pArray, sizeof(int) * m_Room[room_giveback].m_nWidth * m_Room[room_giveback].m_nHeight);

	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}

void	KItemList::RecoverItem(int nPos)
{	
	int i = 0;
	switch(nPos)
	{	
		/*case pos_give:
#ifdef _SERVER
			if(m_GiveItem > 0)
			{
				int nIndex = ItemSet.Add(&Item[m_GiveItem]);
				int nX, nY;
				if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
				{
					Add(nIndex, pos_equiproom, nX, nY);
					Remove(m_GiveItem);
					ItemSet.Remove(m_GiveItem);
				}
			}
#else
			m_GiveItem = 0;
#endif
			break;*/
		case pos_tremble:
#ifdef _SERVER	
			for(i = 0; i < tremblepart_num; i++) 
			{
				if(m_TrembleItem[i] > 0)
				{
					int nIndex = ItemSet.AddI(&Item[m_TrembleItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						AddKIL(nIndex, pos_equiproom, nX, nY);
						Remove(m_TrembleItem[i]);
						ItemSet.Remove(m_TrembleItem[i]);
					}
				}
			}
#else
			memset(&m_TrembleItem, 0, sizeof(m_TrembleItem));
#endif
			break;
		/*case pos_compone:
#ifdef _SERVER	
			for(i = 0; i < compoundpart_num; i++)
			{
				if(m_CompOneItem[i] > 0)
				{
					int nIndex = ItemSet.Add(&Item[m_CompOneItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						Add(nIndex, pos_equiproom, nX, nY);
						Remove(m_CompOneItem[i]);
						ItemSet.Remove(m_CompOneItem[i]);
					}
				}
			}
#else
			memset(&m_CompOneItem, 0, sizeof(m_CompOneItem));
#endif
			break;
		case pos_comptwo:
#ifdef _SERVER	
			for(i = 0; i < compoundpart_num; i++)
			{
				if(m_CompTwoItem[i] > 0)
				{
					int nIndex = ItemSet.Add(&Item[m_CompTwoItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						Add(nIndex, pos_equiproom, nX, nY);
						Remove(m_CompTwoItem[i]);
						ItemSet.Remove(m_CompTwoItem[i]);
					}
				}
			}
#else
			memset(&m_CompTwoItem, 0, sizeof(m_CompTwoItem));
#endif
			break;
		case pos_compthree:
#ifdef _SERVER	
			for(i = 0; i < compoundpart_num; i++)
			{
				if(m_CompThreeItem[i] > 0)
				{
					int nIndex = ItemSet.Add(&Item[m_CompThreeItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						Add(nIndex, pos_equiproom, nX, nY);
						Remove(m_CompThreeItem[i]);
						ItemSet.Remove(m_CompThreeItem[i]);
					}
				}
			}
#else
			memset(&m_CompThreeItem, 0, sizeof(m_CompThreeItem));
#endif
			break;
		case pos_distill:
#ifdef _SERVER	
			for(i = 0; i < outinpart_num; i++)
			{
				if(m_DistillItem[i] > 0)
				{
					int nIndex = ItemSet.Add(&Item[m_DistillItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						Add(nIndex, pos_equiproom, nX, nY);
						Remove(m_DistillItem[i]);
						ItemSet.Remove(m_DistillItem[i]);
					}
				}
			}
#else
			memset(&m_DistillItem, 0, sizeof(m_DistillItem));
#endif
			break;
		case pos_forge:
#ifdef _SERVER	
			for(i = 0; i < forgepart_num; i++)
			{
				if(m_ForgeItem[i] > 0)
				{
					int nIndex = ItemSet.Add(&Item[m_ForgeItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						Add(nIndex, pos_equiproom, nX, nY);
						Remove(m_ForgeItem[i]);
						ItemSet.Remove(m_ForgeItem[i]);
					}
				}
			}
#else
			memset(&m_ForgeItem, 0, sizeof(m_ForgeItem));
#endif
			break;
		case pos_enchase:
#ifdef _SERVER	
			for(i = 0; i < outinpart_num; i++)
			{
				if(m_EnchaseItem[i] > 0)
				{
					int nIndex = ItemSet.Add(&Item[m_EnchaseItem[i]]);
					int nX, nY;
					if(CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &nX, &nY))
					{
						Add(nIndex, pos_equiproom, nX, nY);
						Remove(m_EnchaseItem[i]);
						ItemSet.Remove(m_EnchaseItem[i]);
					}
				}
			}
#else
			memset(&m_EnchaseItem, 0, sizeof(m_EnchaseItem));
#endif
			break;*/
		default:
			break;
	}
}

BOOL KItemList::CheckTrembleItem(int nIdx, int nPlace /* = -1 */) 
{
	if (m_PlayerIdx <= 0 || nIdx <= 0)
		return FALSE;
	
	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		_ASSERT(0);
		return FALSE;
	}
	int nGenre = 0; 
	int nDetail = 0;
	nGenre = Item[nIdx].GetGenre();
	nDetail = Item[nIdx].GetDetailType();
	
	switch(nPlace)
	{	
		case 0:
			if(nGenre != item_equip)
			{
				return FALSE;
			}
			if(Item[nIdx].GetGoldId() > 0 
				//|| Item[nIdx].GetPointPurple() 
				|| !Item[nIdx].GetAttribType())
			{
				return FALSE;
			}
			break;
		case 1:
			if(m_TrembleItem[1] || m_TrembleItem[2] || m_TrembleItem[3] || m_TrembleItem[4] || m_TrembleItem[5] || m_TrembleItem[6] || m_TrembleItem[7])
			{
				return FALSE;
			}
			if(nGenre != item_task)
			{
				return FALSE;
			}
			if(nDetail != 239)//t?thu?tinh
			{
				return FALSE;
			}
			break;
		case 2:
			if(m_TrembleItem[1] || m_TrembleItem[2] || m_TrembleItem[3] || m_TrembleItem[4] || m_TrembleItem[5] || m_TrembleItem[6] || m_TrembleItem[7])
			{
				return FALSE;
			}
			if(nGenre != item_task)
			{
				return FALSE;
			}
			if(nDetail != 240)//l鬰 thu?tinh
			{
				return FALSE;
			}
			break;
		default:
			if(m_TrembleItem[1] || m_TrembleItem[2] || m_TrembleItem[3] || m_TrembleItem[4] || m_TrembleItem[5] || m_TrembleItem[6] || m_TrembleItem[7])
			{
				return FALSE;
			}
			if(nGenre != item_task)
			{
				return FALSE;
			}
			if(nDetail != 238) //lam thuy tinh			
			{
				return FALSE;
			}
			break;
	}

	m_TrembleItem[nPlace] = nIdx;
	m_Items[nItemListIdx].nPlace = pos_tremble;
	m_Items[nItemListIdx].nX = nPlace;
	m_Items[nItemListIdx].nY = 0;

	nGenre = 0; 
	nDetail = 0;
	return TRUE;
}

void KItemList::UnTrembleItem(int nIdx, int nPos/* = -1*/)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return;

	if (nIdx <= 0)
		return;

	if (nPos <= 0)
	{
		for (i = 0; i < tremblepart_num; i++)
		{
			if (m_TrembleItem[i] == nIdx)
			{
				break;
			}
		}
		if (i == tremblepart_num)
			return;

	}
	else
	{
		if (m_TrembleItem[nPos] != nIdx)	
			return;
		i = nPos;
	}
	
	switch(i)
	{	
		case 0:
			if(Item[nIdx].GetGenre() != item_equip)
				return;
			break;
		default:
			if(Item[nIdx].GetGenre() != item_task)
				return;
			break;
	}
	m_TrembleItem[i] = 0;

	i = 0;
	return;
}

int		KItemList::GetItemNum(int nGenre, int nDetailType, int nParticular, int nLevel)
{
	int		nNo = 0;
	int		nIdx = 0, nX = 0, nY = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		{	
				if (nGenre == Item[m_Items[nIdx].nIdx].GetGenre() && 
					nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() && 
					nParticular == Item[m_Items[nIdx].nIdx].GetParticular() &&
					nLevel == Item[m_Items[nIdx].nIdx].GetLevel())
				{
						if (Item[m_Items[nIdx].nIdx].IsStack())
							nNo += Item[m_Items[nIdx].nIdx].GetStackNum();
						else
							nNo++;
				}
		}
	}
	return nNo;
}

int		KItemList::CountCommonItem(int nItemNature, int nItemGenre, int nDetailType, int nParticularType, int nLevel, int nSeries, int Place)
{
	int		nIdx = 0;
	int nResult = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;

		if (nItemNature != Item[nGameIdx].GetNature())
			continue;

		if (nItemGenre != Item[nGameIdx].GetGenre())
			continue;

		if (nDetailType > -1 && (nDetailType != Item[nGameIdx].GetDetailType()))
			continue;

		if (nParticularType > -1 && (nParticularType != Item[nGameIdx].GetParticular()))
			continue;

		if (nLevel > -1 && (nLevel != Item[nGameIdx].GetLevel()))
			continue;

		if (nSeries > -1 && (nSeries != Item[nGameIdx].GetSeries()))
			continue;

		if (m_Items[nIdx].nPlace != Place)
			continue;

		if (Item[nGameIdx].IsStack())
			nResult += Item[nGameIdx].GetStackNum();
		else
			nResult++;
	}
	return nResult;
}

int KItemList::PositionToIndex(int P, int i)
{
	if (P >= pos_hand && P < pos_num)
	{
		switch (P)
		{
		case pos_equip:
		{
			if(i >= 0 && i < itempart_num)
				return m_EquipItem[i];
		}
		}
	}
	return 0;
}