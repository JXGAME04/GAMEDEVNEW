//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
// File:	KFaction.cpp
// Date:	2002.09.26
// Code:	Fong Ki襲
// Desc:	Faction Class
//---------------------------------------------------------------------------

#include	"KCore.h"
//#include	"MyAssert.h"
#include	"KIniFile.h"
#include	"KSkills.h"
#include	"KFaction.h"
#include	"CoreUseNameDef.h"

KFaction	g_Faction;

BOOL	KFaction::Init()
{
	KIniFile	Ini;
	char		szSection[80], szBuffer[32];
	char		szSeries[series_num][16] = {"金", "木", "水", "火", "土"};
	char		szCamp[camp_num][16] = {"新手", "正派", "邪派", "中立", "杀手", "野兽", "路人"};
	int			i = 0, j = 0, k = 0, nArrayPos = 0;

	for (i = 0; i < MAX_FACTION; i++)
	{
		m_sAttribute[i].m_nIndex = i;
		m_sAttribute[i].m_nSeries = series_metal;
		m_sAttribute[i].m_nCamp = camp_justice;
		m_sAttribute[i].m_szName[0] = 0;
	}

	if ( !Ini.Load(FACTION_FILE) )
		return FALSE;

	for (i = 0; i < MAX_FACTION; i++)
	{
		sprintf(szSection, "%d", i);
		// [HOASON 01/09] muc [11]/[12] chua co trong FactionInfo.ini: doc Name KHONG mac dinh, rong -> bo qua
		// (neu khong, GetString tra mac dinh 'Thieu Lam/kim' -> 2 phai ma 'Thieu Lam' gia o id 11/12)
		Ini.GetString(szSection, "Name", "", m_sAttribute[i].m_szName, sizeof(m_sAttribute[i].m_szName));
		if (m_sAttribute[i].m_szName[0] == 0)
			continue;
		Ini.GetString(szSection, "Series", "金", szBuffer, sizeof(szBuffer));

		// [HOASON 01/09] id phai = SO MUC [i] trong FactionInfo.ini (khong con 'o trong dau tien cua he'
		// nua - cong thuc he*2+k sup do khi 13 phai chia 5 he khong deu). 10 muc goc giu nguyen id 0..9.
		nArrayPos = i;
		for (j = 0; j < series_num; j++)
		{
			if (strcmp(szBuffer, szSeries[j]) == 0)
			{
				m_sAttribute[nArrayPos].m_nSeries = j;
				break;
			}
		}
		if (j >= series_num)
			continue;						// muc khong co / he la -> bo qua, ten de trong
		Ini.GetString(szSection, "Name", "少林派", m_sAttribute[nArrayPos].m_szName, sizeof(m_sAttribute[nArrayPos].m_szName));
		Ini.GetString(szSection, "ValueName", "shaolin", m_sAttribute[nArrayPos].m_szValueName, sizeof(m_sAttribute[nArrayPos].m_szValueName));
		Ini.GetString(szSection, "ShowName", "Thi誹 L﹎", m_sAttribute[nArrayPos].m_szShowName, sizeof(m_sAttribute[nArrayPos].m_szShowName));
		Ini.GetString(szSection, "Camp", "正义", szBuffer, sizeof(szBuffer));
		for (j = 0; j < camp_num; j++)
		{
			if (strcmp(szBuffer, szCamp[j]) == 0)
			{
				m_sAttribute[nArrayPos].m_nCamp = j;
				break;
			}
		}
		_ASSERT(j < camp_num);
	}

	return TRUE;
}

int		KFaction::GetID(int nSeries, int nNo)
{
	// [HOASON 01/09] phai thu nNo (0..) trong so cac phai cung he, duyet bang thay vi he*2+nNo
	if (nSeries < series_metal || nSeries >= series_num || nNo < 0)
		return -1;
	int nDem = 0;
	for (int i = 0; i < MAX_FACTION; i++)
	{
		if (m_sAttribute[i].m_szName[0] == 0 || m_sAttribute[i].m_nSeries != nSeries)
			continue;
		if (nDem == nNo)
			return i;
		nDem++;
	}
	return -1;
}

int		KFaction::GetIDByValueName(const char* lpszValueName)
{
	if (!lpszValueName || !lpszValueName[0])
		return -1;
	for (int i = 0; i < MAX_FACTION; i++)
	{
		if (m_sAttribute[i].m_szName[0] && strcmp(lpszValueName, m_sAttribute[i].m_szValueName) == 0)
			return i;
	}
	return -1;
}

int		KFaction::GetID(int nSeries, char *lpszName)
{
	if (nSeries < series_metal || nSeries >= series_num)
		return -1;
	if ( !lpszName || !lpszName[0])
		return -1;
	// [HOASON 01/09] duyet toan bang, chi lay phai cung he; nhan CA ten GBK (Name=) LAN ValueName (huashan...)
	for (int i = 0; i < MAX_FACTION; i++)
	{
		if (m_sAttribute[i].m_szName[0] == 0 || m_sAttribute[i].m_nSeries != nSeries)
			continue;
		if (strcmp(lpszName, m_sAttribute[i].m_szName) == 0 || strcmp(lpszName, m_sAttribute[i].m_szValueName) == 0)
			return i;
	}
	return -1;
}

int		KFaction::GetCamp(int nFactionID)
{
	if (nFactionID < 0 || nFactionID >= MAX_FACTION)
		return -1;
	return m_sAttribute[nFactionID].m_nCamp;
}
