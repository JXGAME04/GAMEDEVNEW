//---------------------------------------------------------------------------
// Sword3 Engine (c) 2021 by KingJx
// File:	KChatItem.cpp
// Date:	2021.01.02
// Code:	Fong KiÒu
// Desc:	KChatItem Class
//---------------------------------------------------------------------------

#include	"KCore.h"

#ifndef _SERVER
#include	"CoreShell.h"
#include	"KItemSet.h"
#include	"KItem.h"
#include	"KChatItem.h"
#include	"KSubWorldSet.h"

KChatItem	g_cChatItem;

KChatItem::KChatItem()
{
	Init();
}

void	KChatItem::Init()
{
	m_cViewItemIdx = 0;
}


BOOL KChatItem::DeleteChatItemInfo()
{
	if(m_cViewItemIdx>0)
	{
		ItemSet.Remove(m_cViewItemIdx);
		m_cViewItemIdx = 0;
		return TRUE;
	}
	return FALSE;
}

#endif













