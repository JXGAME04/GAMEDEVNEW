//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.cpp
// Date:	2003.07.28
// Code:	Fong KiÒu
// Desc:	KViewItem Class
//---------------------------------------------------------------------------

#include	"KCore.h"

#ifndef _SERVER
#include	"CoreShell.h"
#include	"KItemSet.h"
#include	"KItem.h"
#include	"KSellItem.h"

KSellItem	g_cSellItem;

KSellItem::KSellItem()
{
	Init();
}

void	KSellItem::Init()
{
	m_dwNpcID	= 0;
	m_szName[0]	= 0;
	m_nProcess = 0;
	memset(this->m_sItem, 0, sizeof(this->m_sItem));
}

void	KSellItem::ApplyViewItem(DWORD dwNpcID)
{
	VIEW_EQUIP_COMMAND	sView;
	sView.ProtocolType = c2s_viewitem;
	sView.m_dwNpcID = dwNpcID;
	sView.m_bPrcess = 0;
	if (g_pClient)
		g_pClient->SendPackToServer(&sView, sizeof(sView));
	m_nProcess = 0;
}

void	KSellItem::DeleteAll(int nUpdate)
{
	m_dwNpcID	= 0;
	m_szName[0]	= 0;
	m_nProcess = 0;
	m_nId = 0;
	ZeroMemory(m_nIdx,sizeof(m_nIdx));

	for (int i = 0; i < 60; i ++)
	{
		if (!nUpdate)
		{
			if (m_sItem[i].nIdx > 0)
				ItemSet.Remove(m_sItem[i].nIdx);
			m_nDorp[i] = 0;
		}
		else
		{
			if (m_nDorp[i])
				ItemSet.Remove(m_nDorp[i]);
			m_nDorp[i] = m_sItem[i].nIdx;
		}
		m_sItem[i].nIdx = 0;
		m_sItem[i].nPrice = 0;
		m_sItem[i].nX = 0;
		m_sItem[i].nY = 0;
	}
}

int		KSellItem::FindIdx(int uId)
{
	for (int i = 0; i < 60; i++)
	{
		if (m_sItem[i].nIdx == uId)
		{
			return m_nIdx[i];
		}
	}
	return 0;
}

void	KSellItem::GetData(BYTE* pMsg ,int nUpdate)
{
	if (!pMsg)
		return;

	if (!m_nProcess && !nUpdate)
		DeleteAll();
	else if (!m_nProcess && nUpdate)
		DeleteAll(1);


	int					i, j;
	int					nNpcIdx, nItemIdx, nMagicParam[6];
	VIEW_ITEM_SYNC		*pView = (VIEW_ITEM_SYNC*)pMsg;
	KUiPlayerItem	sPlayer;

	m_dwNpcID = pView->m_dwNpcID;
	sPlayer.uId = m_dwNpcID;
	nNpcIdx = NpcSet.SearchID(m_dwNpcID);
	if (nNpcIdx > 0)
	{
		strcpy(m_szName, Npc[nNpcIdx].Name);
		sPlayer.nIndex = nNpcIdx;
		strcpy(sPlayer.Name, m_szName);
		sPlayer.nData = Npc[nNpcIdx].GetMenuState();
	}
	else
	{
		sPlayer.nIndex = 0;
		sPlayer.Name[0] = 0;
		sPlayer.nData = 0;
	}

	for (i = 0; i < 6; i ++)
	{
		if (m_nProcess != m_nId)
		{
			m_nProcess = 60;
			break;
		}
		m_nProcess++;
		if (pView->m_sInfo[i].m_nID == 0)
			continue;

			for (j = 0; j < 6; j++)
				nMagicParam[j] = pView->m_sInfo[i].m_btMagicLevel[j];
			if (pView->m_sInfo[i].m_nGoldId)
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
			Item[nItemIdx].SetID(pView->m_sInfo[i].m_nID);
			Item[nItemIdx].SetPrice(pView->m_sInfo[i].m_uPrice);
			Item[nItemIdx].SetExpTime(pView->m_sInfo[i].m_YearExp,0,0,0);
			Item[nItemIdx].SetDurability(pView->m_sInfo[i].m_curDurability);
			Item[nItemIdx].SetParam(pView->m_sInfo[i].m_nParam);
			Item[nItemIdx].SetStackNum(pView->m_sInfo[i].m_bStack);

			m_sItem[m_nId].nIdx = /*pView->m_sInfo[i].m_nIdx*/nItemIdx;
			m_sItem[m_nId].nPrice = pView->m_sInfo[i].m_uPrice;
			m_sItem[m_nId].nX = pView->m_sInfo[i].m_bX;
			m_sItem[m_nId].nY = pView->m_sInfo[i].m_bY;
			m_nIdx[m_nId] = pView->m_sInfo[i].m_nIdx;

			m_nId++;
	}

	if (m_nProcess < 60 && !nUpdate)
	{
		VIEW_EQUIP_COMMAND	sView;
		sView.ProtocolType = c2s_viewitem;
		sView.m_dwNpcID = pView->m_dwNpcID;
		sView.m_bPrcess = (BYTE)m_nProcess;
		if (g_pClient)
			g_pClient->SendPackToServer(&sView, sizeof(sView));
		return;
	}
	else if (m_nProcess < 60 && nUpdate)
	{
		VIEW_EQUIP_COMMAND	sView;
		sView.ProtocolType = c2s_updateviewitem;
		sView.m_dwNpcID = pView->m_dwNpcID;
		sView.m_bPrcess = (BYTE)m_nProcess;
		if (g_pClient)
			g_pClient->SendPackToServer(&sView, sizeof(sView));
		return;
	}

	if (!nUpdate)
		CoreDataChanged(GDCNI_VIEW_PLAYERSELLITEM, (unsigned int)&sPlayer, 0);
	else
		CoreDataChanged(GDCNI_VIEW_PLAYERUPDATEITEM, 0 ,0);
}

void KSellItem::UpdateItem( DWORD dwNpcID )
{
	if (m_nProcess >= 60)
	{
		VIEW_EQUIP_COMMAND	sView;
		sView.ProtocolType = c2s_updateviewitem;
		sView.m_dwNpcID = dwNpcID;
		sView.m_bPrcess = 0;
		if (g_pClient)
			g_pClient->SendPackToServer(&sView, sizeof(sView));
		m_nProcess = 0;
	}
}

int KSellItem::GetCount()
{
	if (m_nProcess < 60)
	{
		return -1;
	}
	else
	{
		return m_nId;
	}
}

void KSellItem::Check( int nCount, DWORD dwId )
{
	if (nCount == 0)
		CoreDataChanged(GDCNI_CLOSE_BAITAN, 0 ,0);

	if (nCount != m_nId)
	{
		UpdateItem(dwId);
	}
}


#endif













