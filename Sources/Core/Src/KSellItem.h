//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KViewItem.h
// Date:	2003.07.28
// Code:	Fong Ki襲
// Desc:	KViewItem Class
//---------------------------------------------------------------------------


#ifndef _SERVER

class KSellItem
{
private:
	DWORD		m_dwNpcID;				// 对方玩家的 npc 的 id
	char		m_szName[32];			// 对方玩家的名字
	int			m_nProcess;
public:
	PlayerItem	m_sItem[60];	// 对方玩家穿在身上的装备在客户端 Item 数组中的位置信息
	int			m_nIdx[60];
	int			m_nId;
	int			m_nDorp[60];

public:
	KSellItem();						// 构造
	void		Init();					// 初始化
	void		ApplyViewItem(DWORD dwNpcID);
	void		UpdateItem(DWORD dwNpcID);
	int			GetCount();
	void		Check(int nCount, DWORD dwId);
	void		DeleteAll(int nUpdate = 0);
	void		GetData(BYTE* pMsg ,int nUpdate = 0);
	int			FindIdx(int uId);
};

extern	KSellItem	g_cSellItem;

#endif


