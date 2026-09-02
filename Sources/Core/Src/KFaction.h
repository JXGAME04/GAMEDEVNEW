//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KFaction.h
// Date:	2020.09.26
// Code:	Fong Ki襲
// Desc:	Faction Class
//---------------------------------------------------------------------------

#ifndef KFACTION_H
#define KFACTION_H

#include "GameDataDef.h"

#define		FACTIONS_PRR_SERIES				2			
#define		MAX_FACTION						(MAX_FACTION_NUM)	// [HOASON 01/09] 13 phai; FACTIONS_PRR_SERIES chi con la 'so phai goc moi he' cho code cu (bot)

class KFaction
{
public:
	struct	SFactionAttirbute							
	{
		int		m_nIndex;								// 编号
		int		m_nSeries;								// 五行属性
		int		m_nCamp;								// 阵营
		char	m_szName[64];							// 门派名
		char	m_szValueName[64];							// ???
		char	m_szShowName[64];							// ???
	}				m_sAttribute[MAX_FACTION];			// 所有门派

public:
	BOOL			Init();								// 初始化，载入门派说明文件
	int				GetID(int nSeries, int nNo);		// 根据五行属性和本属性第几个门派得到门派编号
	int				GetID(int nSeries, char *lpszName);	// 根据五行属性和门派名得到门派编号
	int				GetIDByValueName(const char* lpszValueName);	// [HOASON 01/09] huashan/wuhun/xiaoyao -> id
	int				GetCamp(int nFactionID);			// 获得某个门派的阵营
};

extern	KFaction	g_Faction;
#endif
