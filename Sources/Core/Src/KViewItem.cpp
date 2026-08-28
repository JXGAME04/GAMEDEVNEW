//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.cpp
// Date:	2003.07.28
// Code:	Fong Ki襲
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#include	"KCore.h"

#ifndef _SERVER
#include	"CoreShell.h"
#include	"KItemSet.h"
#include	"KItem.h"
#include	"KViewItem.h"

KViewItem	g_cViewItem;

KViewItem::KViewItem()
{
	Init();
}

void	KViewItem::Init()
{
	m_dwNpcID	= 0;
	m_nLevel	= 1;
	m_szName[0]	= 0;
	memset(this->m_sItem, 0, sizeof(this->m_sItem));
}

void	KViewItem::ApplyViewEquip(DWORD dwNpcID)
{
	VIEW_EQUIP_COMMAND	sView;
	sView.ProtocolType = c2s_viewequip;
	sView.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sView, sizeof(sView));
}

void	KViewItem::DeleteAll()
{
	m_dwNpcID	= 0;
	m_nLevel	= 1;
	m_szName[0]	= 0;

	for (int i = 0; i < itempart_num; i ++)
	{
		if (m_sItem[i].nIdx > 0)
			ItemSet.Remove(m_sItem[i].nIdx);
		m_sItem[i].nIdx = 0;
	}
}

void	KViewItem::GetData(BYTE* pMsg)
{
	if (!pMsg)
		return;

	DeleteAll();

	int	i, j;
	int	nNpcIdx, nItemIdx, nMagicParam[MAX_ITEM_MAGICLEVEL];
	VIEW_EQUIP_SYNC		*pView = (VIEW_EQUIP_SYNC*)pMsg;
	KUiPlayerItem	sPlayer;

	m_dwNpcID = pView->m_dwNpcID;
	sPlayer.uId = m_dwNpcID;
	nNpcIdx = NpcSet.SearchID(m_dwNpcID);
	if (nNpcIdx > 0)
	{
		m_nLevel = Npc[nNpcIdx].m_Level;
		strcpy(m_szName, Npc[nNpcIdx].Name);
		sPlayer.nIndex = nNpcIdx;
		strcpy(sPlayer.Name, m_szName);
		sPlayer.nData = Npc[nNpcIdx].GetMenuState();
		sPlayer.nTeamID = pView->m_Avatar;//nay ben client nhan,sPlayer thi moi' khai bao
	}
	else
	{
		sPlayer.nIndex = 0;
		sPlayer.Name[0] = 0;
		sPlayer.nData = 0;
	}

	for (i = 0; i < itempart_num; i ++)
	{
		if (pView->m_sInfo[i].m_nID == 0)
			continue;
		for (j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
			nMagicParam[j] = pView->m_sInfo[i].m_btMagicLevel[j];

			if (pView->m_sInfo[i].m_nNature >= NATURE_GOLD) {
			nItemIdx = ItemSet.Add(pView->m_sInfo[i].m_nNature,
				pView->m_sInfo[i].m_btGenre,
				pView->m_sInfo[i].m_btSeries,
				pView->m_sInfo[i].m_btLevel,
				pView->m_sInfo[i].m_btLuck,
				pView->m_sInfo[i].m_btDetail,
				pView->m_sInfo[i].m_btParticur,
				nMagicParam,
				pView->m_sInfo[i].m_wVersion,
				pView->m_sInfo[i].m_dwRandomSeed);
			}
			else if (pView->m_sInfo[i].m_nGoldId)
			{
				nItemIdx = ItemSet.AddGoldItem(
					pView->m_sInfo[i].m_nGoldId,
					nMagicParam,
					pView->m_sInfo[i].m_btSeries,
					pView->m_sInfo[i].m_bEnChance
					);
			}
			else
			{
				nItemIdx = ItemSet.AddItemSet2(
					pView->m_sInfo[i].m_btGenre,
					pView->m_sInfo[i].m_btSeries,
					pView->m_sInfo[i].m_btLevel,
					pView->m_sInfo[i].m_btLuck,
					pView->m_sInfo[i].m_btDetail,
					pView->m_sInfo[i].m_btParticur,
					nMagicParam,
					pView->m_sInfo[i].m_wVersion,
					pView->m_sInfo[i].m_dwRandomSeed,
					pView->m_sInfo[i].m_bStack,
					pView->m_sInfo[i].m_bEnChance,
					pView->m_sInfo[i].m_bPoint);
			}
		if (nItemIdx <= 0)
			continue;
		// [LOREN 27/08] Giu PHAM CHAT de vong sang tim hien ca o day. Duong tao
		// tren khong mang nItemNature sang (do tim di nhanh AddItemSet2), nhung
		// may chu DA gui du trong m_nNature. Khong noi dieu kien >= NATURE_GOLD
		// vi truong m_btDetail duoc ma hoa khac nhau theo pham chat
		// (CoreShell.cpp:2319) - noi ra se dung sai vat pham.
		Item[nItemIdx].SetNature(pView->m_sInfo[i].m_nNature);
		Item[nItemIdx].SetID(pView->m_sInfo[i].m_nID);
		Item[nItemIdx].SetDurability(pView->m_sInfo[i].m_curDurability);
		m_sItem[i].nIdx = nItemIdx;
	}

	// 通知界面
	CoreDataChanged(GDCNI_VIEW_PLAYERITEM, (DWORD)&sPlayer, 0);
}





#endif













