// khong chay tu day
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "KTongControl.h"
#include "S3Relay.h"


//////////////////////////////////////////////////////////////////////
// KTongControl.cpp: implementation of the CTongControl class.
//////////////////////////////////////////////////////////////////////
#include "stdio.h"
void WriteLogFile(const char* szString)
{
	
	FILE* pFile = fopen("MoneyTong.log", "a");
	fprintf(pFile, "%s\n",szString);
	fclose(pFile);
	
}

CTongControl::CTongControl(int nCamp, char *lpszPlayerName, char *lpszTongName, int nMasterSex)
{
	m_nCamp			= 0;
	m_dwMoney		= 0;
	m_nCredit		= 0;
	m_nLevel		= 0;
	m_nDirectorNum	= 0;
	m_nManagerNum	= 0;
	m_nMemberNum	= 0;

	m_nCapSize = gGetPrivateProfileIntEx("tong", "tongcap", "relay_config.ini", 0);
	m_bIsFull = false;

	m_dwNameID		= 0;
	m_szName[0]		= 0;
	m_szPassword[0]	= 0;

	m_szMasterTitle[0] = 0;
	memset(m_szDirectorTitle, 0, sizeof(m_szDirectorTitle));
	memset(m_szManagerTitle, 0, sizeof(m_szManagerTitle));
	m_szNormalTitle[0] = 0;

	m_dwMasterID = 0;
	m_szMasterName[0] = 0;

	memset(m_dwDirectorID, 0, sizeof(m_dwDirectorID));
	memset(m_szDirectorName, 0, sizeof(m_szDirectorName));

	memset(m_dwManagerID, 0, sizeof(m_dwManagerID));
	memset(m_szManagerName, 0, sizeof(m_szManagerName));

	m_psMember = (STONG_MEMBER*)new STONG_MEMBER[defTONG_INIT_MEMBER_SIZE];
	m_nMemberPointSize = defTONG_INIT_MEMBER_SIZE;
	memset(m_psMember, 0, sizeof(STONG_MEMBER) * m_nMemberPointSize);

	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return;
	if (!lpszPlayerName || !lpszPlayerName[0] || strlen(lpszPlayerName) >= defTONG_STR_LENGTH)
		return;
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) >= defTONG_STR_LENGTH)
		return;
	m_nCamp = nCamp;
	strcpy(m_szMasterName, lpszPlayerName);
	m_dwMasterID = g_String2Id(m_szMasterName);
	strcpy(m_szName, lpszTongName);
	m_dwNameID = g_String2Id(m_szName);
	m_szNormalBoyTitle[0] = 0;
	m_szNormalGirlTitle[0] = 0;
	m_nMasterSex = nMasterSex;
	memset(m_nDirectorSex, 0, sizeof(m_nDirectorSex));
	memset(m_nManagerSex, 0, sizeof(m_nManagerSex));
	//add by Fong Kieu
	m_nRecruit = 0;
	memset(m_szWayEdit, 0, sizeof(m_szWayEdit));
	memset(m_szNextTargetEdit, 0, sizeof(m_szNextTargetEdit));
	m_nExpGuide = 0;
	m_nCityGuide = 0;
	memset(m_szLeagueTName, 0, sizeof(m_szLeagueTName));
}

CTongControl::CTongControl(TTongStruct sList)
{
	m_nCamp			= sList.nCamp;
	m_dwMoney		= sList.dwMoney;
	m_nCredit		= sList.nCredit;
	m_nLevel		= sList.nLevel;
	m_nDirectorNum	= 0;
	m_nManagerNum	= 0;

	strcpy(m_szName, sList.szName);
	m_dwNameID		= g_String2Id(m_szName);
	m_szPassword[0]	= 0;

	m_szMasterTitle[0] = 0;
	memset(m_szDirectorTitle, 0, sizeof(m_szDirectorTitle));
	memset(m_szManagerTitle, 0, sizeof(m_szManagerTitle));
	m_szNormalTitle[0] = 0;

	m_dwMasterID = 0;
	m_szMasterName[0] = 0;
	memset(m_dwDirectorID, 0, sizeof(m_dwDirectorID));
	memset(m_szDirectorName, 0, sizeof(m_szDirectorName));

	memset(m_dwManagerID, 0, sizeof(m_dwManagerID));
	memset(m_szManagerName, 0, sizeof(m_szManagerName));

	m_nMemberNum = sList.MemberCount;
	m_nMemberPointSize = m_nMemberNum + defTONG_INIT_MEMBER_SIZE;
	m_nCapSize = gGetPrivateProfileIntEx("tong", "tongcap", "relay_config.ini", 0);
	m_bIsFull = false;
	m_psMember = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize];
	memset(m_psMember, 0, sizeof(STONG_MEMBER) * m_nMemberPointSize);
	m_szNormalBoyTitle[0] = 0;
	m_szNormalGirlTitle[0] = 0;
	m_nMasterSex = 0;
	memset(m_nDirectorSex, 0, sizeof(m_nDirectorSex));
	memset(m_nManagerSex, 0, sizeof(m_nManagerSex));
	//add by Fong Kieu
	m_nRecruit = 0;
	memset(m_szWayEdit, 0, sizeof(m_szWayEdit));
	memset(m_szNextTargetEdit, 0, sizeof(m_szNextTargetEdit));
	m_nExpGuide = 0;
	m_nCityGuide = 0;
	memset(m_szLeagueTName, 0, sizeof(m_szLeagueTName));
	strcpy(m_szMasterName, sList.szMasterName);

	if (strlen(sList.szMasterName) > 0)
		m_dwMasterID = g_String2Id(sList.szMasterName);
}

CTongControl::~CTongControl()
{
	if (m_psMember)
		delete []m_psMember;
}

BOOL	CTongControl::AddMember(char *lpszPlayerName, int nSex, bool fromAdd)
{
	int i = 0;
	// Construction/Destruction
	if (m_nMemberPointSize <= 0 || !m_psMember)
		return FALSE;
	if (!lpszPlayerName || strlen(lpszPlayerName) >= defTONG_STR_LENGTH)
		return FALSE;
	if (m_nMemberNum >= m_nCapSize && m_nCapSize > 0 && fromAdd)
		return FALSE;
	// lay nsex tu day de fix 
	for (i = 0; i < m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
	}
	// Search for an empty slot
	if (i >= m_nMemberPointSize)
	{
		i = m_nMemberPointSize;
		// Not found, full, open up a larger memory
		STONG_MEMBER	*pTemp;
		pTemp = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize];
		memcpy(pTemp, m_psMember, sizeof(STONG_MEMBER) * m_nMemberPointSize);

		// Back up temporarily
		delete []m_psMember;
		m_psMember = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize + defTONG_MEMBER_SIZE_ADD];
		memcpy(m_psMember, pTemp, sizeof(STONG_MEMBER) * m_nMemberPointSize);
		memset(&m_psMember[m_nMemberPointSize], 0, sizeof(STONG_MEMBER) * defTONG_MEMBER_SIZE_ADD);
		m_nMemberPointSize += defTONG_MEMBER_SIZE_ADD;

		delete []pTemp;
	}

	// Allocate a larger memory
	strcpy(m_psMember[i].m_szName, lpszPlayerName);
	m_psMember[i].m_dwNameID = g_String2Id(lpszPlayerName);
	m_psMember[i].m_nSex = nSex;
	rTRACE("gia tri nSex [CTongControl]: %d", nSex);
	m_nMemberNum++;
	if (fromAdd) {
		m_bIsFull = m_nMemberNum + m_nDirectorNum + m_nManagerNum >= m_nCapSize && m_nCapSize > 0;
	}
	return TRUE;
}

BOOL	CTongControl::ChangePassword(char *lpOld, char *lpNew)
{
	if (!lpOld || !lpNew)
		return FALSE;
	if (strlen(lpNew) >= defTONG_STR_LENGTH || strcmp(lpOld, m_szPassword) != 0)
		return FALSE;

	strcpy(m_szPassword, lpNew);

	return TRUE;
}

BOOL	CTongControl::GetTongHeadInfo(STONG_HEAD_INFO_SYNC *pInfo)
{
	if (!pInfo)
		return FALSE;

	int		i, j;

	pInfo->ProtocolFamily = pf_tong;
	pInfo->ProtocolID = enumS2C_TONG_HEAD_INFO;
	pInfo->m_dwMoney = m_dwMoney;
	pInfo->m_nCredit = m_nCredit;
	pInfo->m_btCamp = m_nCamp;
	pInfo->m_btLevel = m_nLevel;
	pInfo->m_btManagerNum = m_nManagerNum;
	pInfo->m_dwMemberNum = m_nMemberNum;
	strcpy(pInfo->m_szTongName, m_szName);
	// Add a member
	pInfo->ms_nStatusGuide = m_nRecruit; //tr筺g th竔 tuy觧 ngi bang h閕
	strcpy(pInfo->ms_szWayEdit, m_szWayEdit);
	strcpy(pInfo->ms_szNextTargetEdit, m_szNextTargetEdit);
	pInfo->ms_nExpGuide = m_nExpGuide;
	pInfo->ms_nCityGuide = m_nCityGuide;
	pInfo->ms_nTongLevel = m_nLevel; //ng c蕄 bang h閕
	strcpy(pInfo->ms_szLeagueTName, m_szLeagueTName);
	// ========= add by Fong Ki襲 =========
	pInfo->m_sMember[0].m_btFigure = enumTONG_FIGURE_MASTER;
	pInfo->m_sMember[0].m_btPos = 0;
	GetMasterTitle(pInfo->m_sMember[0].m_szTitle);
	strcpy(pInfo->m_sMember[0].m_szName, this->m_szMasterName);
	
	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pInfo->m_sMember[0].m_szName), &conndup, NULL, &nameid, &param))
	{
		tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
		if (tongconndup.IsValid())
			pInfo->m_sMember[0].m_btOnline = 1;
		else
			pInfo->m_sMember[0].m_btOnline = 0;
		tongconndup.Clearup();
	}
	else
	{
		pInfo->m_sMember[0].m_btOnline = 0;
	}
	conndup.Clearup();

	pInfo->m_btDirectorNum = 0;
	for (i = 0, j = 1; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (!m_szDirectorName[i][0])
			continue;
		pInfo->m_sMember[j].m_btFigure = enumTONG_FIGURE_DIRECTOR;
		pInfo->m_sMember[j].m_btPos = i;
		GetDirectorTitle(pInfo->m_sMember[j].m_szTitle, i);
		strcpy(pInfo->m_sMember[j].m_szName, this->m_szDirectorName[i]);

		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pInfo->m_sMember[j].m_szName), &conndup, NULL, &nameid, &param))
		{
			tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
				pInfo->m_sMember[j].m_btOnline = 1;
			else
				pInfo->m_sMember[j].m_btOnline = 0;
			tongconndup.Clearup();
		}
		else
		{
			pInfo->m_sMember[j].m_btOnline = 0;
		}
		conndup.Clearup();

		j++;
		pInfo->m_btDirectorNum++;
	}

	pInfo->m_wLength = sizeof(STONG_HEAD_INFO_SYNC) - sizeof(pInfo->m_sMember) + sizeof(STONG_ONE_LEADER_INFO) * (1 + pInfo->m_btDirectorNum);

	return TRUE;
}

BOOL	CTongControl::GetTongManagerInfo(
			STONG_GET_MANAGER_INFO_COMMAND *pApply,
			STONG_MANAGER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (pApply->m_nParam3 <= 0)
		return FALSE;

	pInfo->ProtocolFamily	= pf_tong;
	pInfo->ProtocolID		= enumS2C_TONG_MANAGER_INFO;
	pInfo->m_dwParam		= pApply->m_dwParam;
	pInfo->m_dwMoney		= m_dwMoney;
	pInfo->m_nCredit		= m_nCredit;
	pInfo->m_btCamp			= m_nCamp;
	pInfo->m_btLevel		= m_nLevel;
	pInfo->m_btDirectorNum	= m_nDirectorNum;
	pInfo->m_btManagerNum	= m_nManagerNum;
	pInfo->m_dwMemberNum	= m_nMemberNum;
	pInfo->m_btStartNo		= pApply->m_nParam2;
	strcpy(pInfo->m_szTongName, m_szName);

	int		nNeedNum, nStartNum, i, j;

	nStartNum = pApply->m_nParam2;
	nNeedNum = pApply->m_nParam3;
	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;

	pInfo->m_btCurNum = 0;
	// ========= end add by Fong Ki襲 =========
	for (i = 0, j = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (j >= nStartNum)
			break;
		if (m_szManagerName[i][0])
			j++;
	}
	if (i >= defTONG_MAX_MANAGER || j < nStartNum)
		return FALSE;

	// Find the starting position
	for (; i < defTONG_MAX_MANAGER; i++)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break;
		if (!m_szManagerName[i][0])
			continue;

		pInfo->m_sMember[pInfo->m_btCurNum].m_btFigure = enumTONG_FIGURE_MANAGER;
		pInfo->m_sMember[pInfo->m_btCurNum].m_btPos = i;
		GetManagerTitle(pInfo->m_sMember[pInfo->m_btCurNum].m_szTitle, i);
		strcpy(pInfo->m_sMember[pInfo->m_btCurNum].m_szName, m_szManagerName[i]);
		
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(pInfo->m_sMember[pInfo->m_btCurNum].m_szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
				pInfo->m_sMember[pInfo->m_btCurNum].m_btOnline = 1;
			else
				pInfo->m_sMember[pInfo->m_btCurNum].m_btOnline = 0;
			tongconndup.Clearup();
		}
		else
		{
			pInfo->m_sMember[pInfo->m_btCurNum].m_btOnline = 0;
		}
		conndup.Clearup();

		pInfo->m_btCurNum++;
	}
	if (pInfo->m_btCurNum == 0)
		return FALSE;

	pInfo->m_wLength = sizeof(STONG_MANAGER_INFO_SYNC) - sizeof(STONG_ONE_LEADER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);

	return TRUE;
}

BOOL	CTongControl::GetTongMemberInfo(
			STONG_GET_MEMBER_INFO_COMMAND *pApply,
			STONG_MEMBER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (pApply->m_nParam3 <= 0)
		return FALSE;
	if (!m_psMember || m_nMemberPointSize <= 0)
		return FALSE;

	pInfo->ProtocolFamily	= pf_tong;
	pInfo->ProtocolID		= enumS2C_TONG_MEMBER_INFO;
	pInfo->m_dwParam		= pApply->m_dwParam;
	pInfo->m_dwMoney		= m_dwMoney;
	pInfo->m_nCredit		= m_nCredit;
	pInfo->m_btCamp			= m_nCamp;
	pInfo->m_btLevel		= m_nLevel;
	pInfo->m_btDirectorNum	= m_nDirectorNum;
	pInfo->m_btManagerNum	= m_nManagerNum;
	pInfo->m_dwMemberNum	= m_nMemberNum;
	pInfo->m_btStartNo		= pApply->m_nParam2;
	if (m_szNormalTitle[0])
		strcpy(pInfo->m_szTitle, m_szNormalTitle);
	else
		strcpy(pInfo->m_szTitle, defTONG_MEMBER_TITLE);
	strcpy(pInfo->m_szTitleBoy, m_szNormalBoyTitle);
	strcpy(pInfo->m_szTitleGirl, m_szNormalGirlTitle);
	strcpy(pInfo->m_szTongName, m_szName);

	int		nNeedNum, nStartNum, i, j;

	nStartNum = pApply->m_nParam2;
	nNeedNum = pApply->m_nParam3;
	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;

	pInfo->m_btCurNum = 0;
	// Start recording data
	for (i = 0, j = 0; i < m_nMemberPointSize; i++)
	{
		if (j >= nStartNum)
			break;
		if (m_psMember[i].m_szName[0])
			j++;
	}
	if (i >= m_nMemberPointSize || j < nStartNum)
		return FALSE;

	// Find the starting position
	for (; i < m_nMemberPointSize; i++)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break;
		if (!m_psMember[i].m_szName[0])
			continue;
		pInfo->m_sMember[pInfo->m_btCurNum].m_btSex = m_psMember[i].m_nSex;

		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
				pInfo->m_sMember[pInfo->m_btCurNum].m_btOnline = 1;
			else
				pInfo->m_sMember[pInfo->m_btCurNum].m_btOnline = 0;
			tongconndup.Clearup();
		}
		else
		{
			pInfo->m_sMember[pInfo->m_btCurNum].m_btOnline = 0;
		}
		conndup.Clearup();

		strcpy(pInfo->m_sMember[pInfo->m_btCurNum].m_szName, m_psMember[i].m_szName);
		pInfo->m_btCurNum++;
	}
	if (pInfo->m_btCurNum == 0)
		return FALSE;

	pInfo->m_wLength = sizeof(STONG_MEMBER_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);

	return TRUE;
}

BOOL	CTongControl::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (pInstate->m_btCurFigure == pInstate->m_btNewFigure && pInstate->m_btCurPos == pInstate->m_btNewPos)
		return FALSE;

	int		i = 0, nOldPos = 0, nNewPos = 0;
	DWORD	dwNameID;
	char	szName[32];

	memcpy(szName, pInstate->m_szName, sizeof(pInstate->m_szName));
	szName[31] = 0;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_INSTATE;
	pSync->m_btOldFigure	= pInstate->m_btCurFigure;
	pSync->m_btOldPos		= 0;
	pSync->m_btNewFigure	= pInstate->m_btNewFigure;
	pSync->m_btNewPos		= 0;
	pSync->m_dwParam		= pInstate->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	pSync->m_dwTongNameID	= pInstate->m_dwTongNameID;
	pSync->m_szTitle[0]		= 0;
	strcpy(pSync->m_szName, szName);
	int nPlayerSex = 0;

	if (pInstate->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nOldPos = i;
		nPlayerSex = m_nDirectorSex[nOldPos];

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_MANAGER:
			for (i = 0; i < defTONG_MAX_MANAGER; i++)
			{
				if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
					break;
			}
			if (i >= defTONG_MAX_MANAGER)
				return FALSE;
			nNewPos = i;

			strcpy(m_szManagerName[nNewPos], szName);
			m_dwManagerID[nNewPos] = dwNameID;
			m_nManagerSex[nNewPos] = nPlayerSex;
			m_szDirectorName[nOldPos][0] = 0;
			m_dwDirectorID[nOldPos] = 0;
			m_nDirectorNum--;
			m_nManagerNum++;
			if (m_szManagerTitle[nNewPos][0])
				strcpy(m_szManagerTitle[nNewPos], "");
			strcpy(pSync->m_szTitle, m_szManagerTitle[nNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName, nPlayerSex))
				return FALSE;
			m_szDirectorName[nOldPos][0] = 0;
			m_dwDirectorID[nOldPos] = 0;
			m_nDirectorNum--;
			if (m_szNormalTitle[0])
				strcpy(m_szNormalTitle, "");
			GetMemberTitle(pSync->m_szTitle, nPlayerSex);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nOldPos = i;
		nPlayerSex = m_nManagerSex[nOldPos];

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
			{
				if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
					break;
			}
			if (i >= defTONG_MAX_DIRECTOR)
				return FALSE;
			nNewPos = i;

			strcpy(m_szDirectorName[nNewPos], szName);
			m_dwDirectorID[nNewPos] = dwNameID;
			m_nDirectorSex[nNewPos] = nPlayerSex;
			m_szManagerName[nOldPos][0] = 0;
			m_dwManagerID[nOldPos] = 0;
			m_nDirectorNum++;
			m_nManagerNum--;
			if (m_szDirectorTitle[nNewPos][0])
				strcpy(m_szDirectorTitle[nNewPos], "");
			strcpy(pSync->m_szTitle, m_szDirectorTitle[nNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName, nPlayerSex))
				return FALSE;
			m_szManagerName[nOldPos][0] = 0;
			m_dwManagerID[nOldPos] = 0;
			m_nManagerNum--;
			if (m_szNormalTitle[0])
				strcpy(m_szNormalTitle, "");
			GetMemberTitle(pSync->m_szTitle, nPlayerSex);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;
		int		nPos = -1;
		int i = 0;
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;
		nOldPos = i;
		nPlayerSex = this->m_psMember[nPos].m_nSex;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
			{
				if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
					break;
			}
			if (i >= defTONG_MAX_DIRECTOR)
				return FALSE;
			nNewPos = i;

			strcpy(m_szDirectorName[nNewPos], szName);
			m_dwDirectorID[nNewPos] = dwNameID;
			m_nDirectorSex[nNewPos] = nPlayerSex;
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			m_nDirectorNum++;
			m_nMemberNum--;
			if (m_szDirectorTitle[nNewPos][0])
				strcpy(m_szDirectorTitle[nNewPos], "");
			strcpy(pSync->m_szTitle, m_szDirectorTitle[nNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			for (i = 0; i < defTONG_MAX_MANAGER; i++)
			{
				if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
					break;
			}
			if (i >= defTONG_MAX_MANAGER)
				return FALSE;
			nNewPos = i;

			strcpy(m_szManagerName[nNewPos], szName);
			m_dwManagerID[nNewPos] = dwNameID;
			m_nManagerSex[nNewPos] = nPlayerSex;
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			m_nManagerNum++;
			m_nMemberNum--;
			if (m_szManagerTitle[nNewPos][0])
				strcpy(m_szManagerTitle[nNewPos], "");
			strcpy(pSync->m_szTitle, m_szManagerTitle[nNewPos]);
			break;
		default:
			return FALSE;
		}
	}
	pSync->m_btSuccessFlag = 1;
	pSync->m_btOldPos = nOldPos;
	pSync->m_btNewPos = nNewPos;

	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_INSTATED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_INSTATED;
				sSync.m_btFigure = pInstate->m_btNewFigure;
				sSync.m_btPos = nNewPos;
				sSync.m_dwParam = param;
				strcpy(sSync.m_szName, szName);
				switch (pInstate->m_btNewFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					strcpy(sSync.m_szTitle, m_szDirectorTitle[nNewPos]);
					break;
				case enumTONG_FIGURE_MANAGER:
					strcpy(sSync.m_szTitle, m_szManagerTitle[nNewPos]);
					break;
				case enumTONG_FIGURE_MEMBER:
					GetMemberTitle(sSync.m_szTitle, nPlayerSex);
					break;
				}

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// Start recording data
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		if (pSync->m_szTitle[0])
			//sprintf(szMsg, "%s 頲 b nhi謒 ch鴆 v %s ", szName, pSync->m_szTitle);
			sprintf(szMsg, "%s \xAE\xAD\xEE\x63 b\xE6 nhi\xD6m ch\xF8\x63 v\xf4 %s.", szName, pSync->m_szTitle);
		else
		{
			switch (pSync->m_btNewFigure)
			{
			case enumTONG_FIGURE_DIRECTOR:
				sprintf(szMsg, "%s \xAE\xAD\xEE\x63 phong ch\xF8\x63 v\xf4 %s.", szName, defTONG_DIRECTOR_TITLE);
				break;
			case enumTONG_FIGURE_MANAGER:
				sprintf(szMsg, "%s \xAE\xAD\xEE\x63 phong ch\xF8\x63 v\xf4 %s.", szName, defTONG_MANAGER_TITLE);
				break;
			case enumTONG_FIGURE_MEMBER:
				sprintf(szMsg, "%s \xAE\xAD\xEE\x63 phong ch\xF8\x63 v\xf4 M\xABn \xAE\xD6.", szName);
				break;
			}
		}
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
	}

	// Send a message to the guild channel, someone is appointed
	TMemberStruct	sMember;
	sMember.MemberClass = (TONG_MEMBER_FIGURE)pInstate->m_btNewFigure;
	sMember.nTitleIndex = nNewPos;
	strcpy(sMember.szTong, this->m_szName);
	strcpy(sMember.szName, szName);
	sMember.nSex = nPlayerSex;
	g_cTongDB.ChangeMember(sMember);
	
	return TRUE;
}

/*
BOOL	CTongControl::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (pInstate->m_btCurFigure == pInstate->m_btNewFigure && pInstate->m_btCurPos == pInstate->m_btNewPos)
		return FALSE;

	char	szName[32];

	memcpy(szName, pInstate->m_szName, sizeof(pInstate->m_szName));
	szName[31] = 0;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_INSTATE;
	pSync->m_btOldFigure	= pInstate->m_btCurFigure;
	pSync->m_btOldPos		= pInstate->m_btCurPos;
	pSync->m_btNewFigure	= pInstate->m_btNewFigure;
	pSync->m_btNewPos		= pInstate->m_btNewPos;
	pSync->m_dwParam		= pInstate->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	pSync->m_dwTongNameID	= pInstate->m_dwTongNameID;
	pSync->m_szTitle[0]		= 0;
	strcpy(pSync->m_szName, szName);

	if (pInstate->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pInstate->m_btCurPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (!m_szDirectorName[pInstate->m_btCurPos][0] ||
			strcmp(szName, m_szDirectorName[pInstate->m_btCurPos]) != 0)
			return FALSE;
		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			if (pInstate->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
			if (m_szDirectorName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szDirectorName[pInstate->m_btNewPos], szName);
			m_dwDirectorID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szDirectorName[pInstate->m_btCurPos][0] = 0;
			m_dwDirectorID[pInstate->m_btCurPos] = 0;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pInstate->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
			if (m_szManagerName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szManagerName[pInstate->m_btNewPos], szName);
			m_dwManagerID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szDirectorName[pInstate->m_btCurPos][0] = 0;
			m_dwDirectorID[pInstate->m_btCurPos] = 0;
			m_nDirectorNum--;
			m_nManagerNum++;
			strcpy(pSync->m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName))
				return FALSE;
			m_szDirectorName[pInstate->m_btCurPos][0] = 0;
			m_dwDirectorID[pInstate->m_btCurPos] = 0;
			m_nDirectorNum--;
			strcpy(pSync->m_szTitle, m_szNormalTitle);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pInstate->m_btCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (!m_szManagerName[pInstate->m_btCurPos][0] ||
			strcmp(szName, m_szManagerName[pInstate->m_btCurPos]) != 0)
			return FALSE;
		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			if (pInstate->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
			if (m_szDirectorName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szDirectorName[pInstate->m_btNewPos], szName);
			m_dwDirectorID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szManagerName[pInstate->m_btCurPos][0] = 0;
			m_dwManagerID[pInstate->m_btCurPos] = 0;
			m_nDirectorNum++;
			m_nManagerNum--;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pInstate->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
			if (m_szManagerName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szManagerName[pInstate->m_btNewPos], szName);
			m_dwManagerID[pInstate->m_btNewPos] = g_String2Id(szName);
			m_szManagerName[pInstate->m_btCurPos][0] = 0;
			m_dwManagerID[pInstate->m_btCurPos] = 0;
			strcpy(pSync->m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName))
				return FALSE;
			m_szManagerName[pInstate->m_btCurPos][0] = 0;
			m_dwManagerID[pInstate->m_btCurPos] = 0;
			m_nManagerNum--;
			strcpy(pSync->m_szTitle, m_szNormalTitle);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;
		int		nPos = -1;
		DWORD	dwNameID = g_String2Id(szName);
		if (dwNameID == 0)
			return FALSE;
		for (int i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			if (pInstate->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
			if (m_szDirectorName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szDirectorName[pInstate->m_btNewPos], szName);
			m_dwDirectorID[pInstate->m_btNewPos] = g_String2Id(szName);
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			m_nDirectorNum++;
			m_nMemberNum--;
			strcpy(pSync->m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pInstate->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
			if (m_szManagerName[pInstate->m_btNewPos][0])
				return FALSE;
			strcpy(m_szManagerName[pInstate->m_btNewPos], szName);
			m_dwManagerID[pInstate->m_btNewPos] = g_String2Id(szName);
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			m_nManagerNum++;
			m_nMemberNum--;
			strcpy(pSync->m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
			break;
		default:
			return FALSE;
		}
	}
	pSync->m_btSuccessFlag = 1;

	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_INSTATED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_INSTATED;
				sSync.m_btFigure = pInstate->m_btNewFigure;
				sSync.m_btPos = pInstate->m_btNewPos;
				sSync.m_dwParam = param;
				strcpy(sSync.m_szName, szName);
				switch (pInstate->m_btNewFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					strcpy(sSync.m_szTitle, m_szDirectorTitle[pInstate->m_btNewPos]);
					break;
				case enumTONG_FIGURE_MANAGER:
					strcpy(sSync.m_szTitle, m_szManagerTitle[pInstate->m_btNewPos]);
					break;
				case enumTONG_FIGURE_MEMBER:
					strcpy(sSync.m_szTitle, this->m_szNormalTitle);
					break;
				}

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// Save, save the data to the database
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		if (pSync->m_szTitle[0])
			sprintf(szMsg, "%s被任命为%s！", szName, pSync->m_szTitle);
		else
		{
			switch (pSync->m_btNewFigure)
			{
			case enumTONG_FIGURE_DIRECTOR:
				sprintf(szMsg, "%s被任命为长老！", szName);
				break;
			case enumTONG_FIGURE_MANAGER:
				sprintf(szMsg, "%s被任命为队长！", szName);
				break;
			case enumTONG_FIGURE_MEMBER:
				sprintf(szMsg, "%s被任命为普通帮众！", szName);
				break;
			}
		}
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string("公告"), std::string(szMsg));
	}


	// Send a message to the guild channel, someone is appointed
	TMemberStruct	sMember;
	sMember.MemberClass = (TONG_MEMBER_FIGURE)pInstate->m_btNewFigure;
	sMember.nTitleIndex = pInstate->m_btNewPos;
	strcpy(sMember.szTong, this->m_szName);
	strcpy(sMember.szName, szName);
	g_cTongDB.ChangeMember(sMember);

	return TRUE;
}
*/

BOOL	CTongControl::Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync)
{
	if (!pKick || !pSync)
		return FALSE;

	char	szName[32];
	DWORD	dwNameID;
	int		nKickPos;
	int i = 0;
	memcpy(szName, pKick->m_szName, sizeof(pKick->m_szName));
	szName[31] = 0;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_KICK;
	pSync->m_btFigure		= pKick->m_btFigure;
	pSync->m_btPos			= pKick->m_btPos;
	pSync->m_dwParam		= pKick->m_dwParam;
	pSync->m_dwTongNameID	= pKick->m_dwTongNameID;
	pSync->m_btSuccessFlag	= 0;
	strcpy(pSync->m_szName, szName);

	if (pKick->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pKick->m_btPos >= defTONG_MAX_MANAGER)
			return FALSE;
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(m_szManagerName[i], szName) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nKickPos = i;
		this->m_szManagerName[i][0] = 0;
		this->m_dwManagerID[i] = 0;
		this->m_nManagerNum--;
	}
	else if (pKick->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;

		int		nPos = -1;
		DWORD	dwNameID = g_String2Id(szName);
		if (dwNameID == 0)
			return FALSE;

		for (int i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;

		if (!m_psMember[nPos].m_szName[0] ||
			strcmp(m_psMember[nPos].m_szName, szName) != 0)
			return FALSE;
		m_psMember[nPos].m_szName[0] = 0;
		m_psMember[nPos].m_dwNameID = 0;
		m_nMemberNum--;
		nKickPos = 0;
	}
	else
	{
		return FALSE;
	}
	pSync->m_btSuccessFlag = 1;
	pSync->m_btPos = nKickPos;


	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_KICKED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_KICKED;
				sSync.m_btFigure = pKick->m_btFigure;
				sSync.m_btPos = nKickPos;
				sSync.m_dwParam = param;
				strcpy(sSync.m_szName, szName);

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// Save, save the data to the database
	g_cTongDB.DelMember(szName);

	// Save
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		//sprintf(szMsg, "%s bi tr鬰 xu蕋 kh醝 bang h閕 ", szName);
		sprintf(szMsg, "%s b\xde tr\xF4\x63 xu\xCAt kh\xE1i bang h\xE9i.", szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
	}

	return TRUE;
}

BOOL	CTongControl::Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync)
{
	if (!pLeave || !pSync)
		return FALSE;

	char	szName[32];
	memcpy(szName, pLeave->m_szName, sizeof(pLeave->m_szName));
	szName[31] = 0;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_LEAVE;
	pSync->m_dwParam		= pLeave->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	strcpy(pSync->m_szName, szName);

	switch (pLeave->m_btFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		{
			if (!m_psMember)
				break;
			int		nPos = -1;
			DWORD	dwNameID = g_String2Id(szName);
			if (dwNameID == 0)
				return FALSE;
			for (int i = 0; i < m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID != dwNameID)
					continue;
				nPos = i;
				break;
			}
			if (nPos < 0)
				return FALSE;
			if (!m_psMember[nPos].m_szName[0] ||
				strcmp(m_psMember[nPos].m_szName, szName) != 0)
				return FALSE;

			m_psMember[nPos].m_szName[0] = 0;
			m_psMember[nPos].m_dwNameID = 0;
			m_nMemberNum--;
		}
		break;
	case enumTONG_FIGURE_MANAGER:
		{
			if (pLeave->m_btPos >= 0 && pLeave->m_btPos < defTONG_MAX_MANAGER)
			{
				if (strcmp(m_szManagerName[pLeave->m_btPos], szName) == 0)
				{
					m_szManagerName[pLeave->m_btPos][0] = 0;
					m_dwManagerID[pLeave->m_btPos] = 0;
					m_nManagerNum--;
					break;
				}
			}

			int		nPos = -1;
			DWORD	dwNameID = g_String2Id(szName);
			if (dwNameID == 0)
				return FALSE;
			for (int i = 0; i < defTONG_MAX_MANAGER; i++)
			{
				if (m_dwManagerID[i] != dwNameID)
					continue;
				nPos = i;
				break;
			}
			if (nPos < 0)
				return FALSE;
			if (!m_szManagerName[nPos][0] ||
				strcmp(szName, m_szManagerName[nPos]) != 0)
				return FALSE;
			m_szManagerName[nPos][0] = 0;
			m_dwManagerID[nPos] = 0;
			m_nManagerNum--;
		}
		break;
	case enumTONG_FIGURE_DIRECTOR:
		return FALSE;
	case enumTONG_FIGURE_MASTER:
		return FALSE;
	}
	pSync->m_btSuccessFlag = 1;

	// Send a message to the guild channel, someone is kicked out of the guild
	g_cTongDB.DelMember(szName);

	// Save
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		//sprintf(szMsg, "%s r阨 kh醝 bang ph竔.", szName);
		sprintf(szMsg, "%s r\xEAi kh\xE1i bang h\xE9i.", szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
	}

	return TRUE;
}

BOOL	CTongControl::AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;

	char	szName[32], szOldMaster[32];
	memcpy(szName, pAccept->m_szName, sizeof(pAccept->m_szName));
	szName[31] = 0;
	if (!szName[0])
		return FALSE;
	strcpy(szOldMaster, this->m_szMasterName);
	int nOldMasterSex = this->m_nMasterSex;

	// Publish a message through the guild channel: someone betrayed the guild
	if (pAccept->m_btAcceptFalg == 0)
	{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_CHANGE_MASTER_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_MASTER_FAIL;
				sFail.m_dwParam			= param;
				sFail.m_btFailID		= 1;
				sFail.m_dwTongNameID	= pAccept->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				tongconndup.SendPackage((const void *)&sFail, sizeof(sFail));
			}
			else
			{
				//tong is not connect
			}
		}
		else
		{
			//not find
		}

		return FALSE;
	}

	DWORD	dwNameID;
	int		i;

	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;
	int nNewMasterSex = 0;

	if (pAccept->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;

		nNewMasterSex = m_nDirectorSex[i];

		if (!AddMember(m_szMasterName, nNewMasterSex))
			return FALSE;
		strcpy(this->m_szMasterName, szName);
		this->m_dwMasterID = dwNameID;
		this->m_nMasterSex = nNewMasterSex;
		m_szDirectorName[i][0] = 0;
		m_dwDirectorID[i] = 0;
		this->m_nDirectorNum--;
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;

		nNewMasterSex = m_nManagerSex[i];

		if (!AddMember(m_szMasterName, nNewMasterSex))
			return FALSE;
		strcpy(this->m_szMasterName, szName);
		this->m_dwMasterID = dwNameID;
		this->m_nMasterSex = nNewMasterSex;
		m_szManagerName[i][0] = 0;
		m_dwManagerID[i] = 0;
		this->m_nManagerNum--;
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember || m_nMemberPointSize <= 0)
			return FALSE;
		for (i = 0; i < this->m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID &&
				strcmp(szName, m_psMember[i].m_szName) == 0)
				break;
		}
		if (i >= m_nMemberPointSize)
			return FALSE;
		
		nNewMasterSex = m_psMember[i].m_nSex;

		if (!AddMember(m_szMasterName, nNewMasterSex))
			return FALSE;
		strcpy(m_szMasterName, szName);
		m_dwMasterID = dwNameID;
		this->m_nMasterSex = nNewMasterSex;
		m_psMember[i].m_szName[0] = 0;
		m_psMember[i].m_dwNameID = 0;
		m_nMemberNum--;
	}
	else
	{
		return FALSE;
	}

	STONG_CHANGE_AS_SYNC	sChange;

	sChange.ProtocolFamily	= pf_tong;
	sChange.ProtocolID		= enumS2C_TONG_CHANGE_AS;
	sChange.m_dwTongNameID	= this->m_dwNameID;

	// Not capable enough
	DWORD nameid = 0;
	unsigned long param = 0;
	CNetConnectDup conndup1;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szOldMaster), &conndup1, NULL, &nameid, &param))
	{
		CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup1.GetIP());
		if (tongconndup.IsValid())
		{
			sChange.m_dwParam		= param;
			sChange.m_btFigure		= enumTONG_FIGURE_MEMBER;
			sChange.m_btPos			= 0;
			GetMemberTitle(sChange.m_szTitle, nOldMasterSex);
			strcpy(sChange.m_szName, m_szMasterName);

			tongconndup.SendPackage((const void *)&sChange, sizeof(sChange));
		}
	}

	// Send a message to the old guild leader
	CNetConnectDup conndup2;
	if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup2, NULL, &nameid, &param))
	{
		CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup2.GetIP());
		if (tongconndup.IsValid())
		{
			sChange.m_dwParam		= param;
			sChange.m_btFigure		= enumTONG_FIGURE_MASTER;
			sChange.m_btPos			= 0;
			strcpy(sChange.m_szTitle, this->m_szMasterTitle);
			strcpy(sChange.m_szName, this->m_szMasterName);

			tongconndup.SendPackage((const void *)&sChange, sizeof(sChange));
		}
	}

	STONG_CHANGE_MASTER_SYNC	sMaster;
	sMaster.ProtocolFamily	= pf_tong;
	sMaster.ProtocolID		= enumS2C_TONG_CHANGE_AS;
	sMaster.m_dwTongNameID	= m_dwNameID;
	strcpy(sMaster.m_szName, m_szMasterName);

	g_TongServer.BroadPackage((const void*)&sMaster, sizeof(sMaster));

	// Send a message to the new guild leader
	// Save
	TMemberStruct	sMember1;
	sMember1.MemberClass = enumTONG_FIGURE_MASTER;
	sMember1.nTitleIndex = 0;
	strcpy(sMember1.szTong, this->m_szName);
	strcpy(sMember1.szName, szName);
	sMember1.nSex = nNewMasterSex;
	g_cTongDB.ChangeMember(sMember1);

	// New guild leader
	TMemberStruct	sMember2;
	sMember2.MemberClass = enumTONG_FIGURE_MEMBER;
	sMember2.nTitleIndex = 0;
	strcpy(sMember2.szTong, this->m_szName);
	strcpy(sMember2.szName, szOldMaster);
	sMember2.nSex = nOldMasterSex;
	g_cTongDB.ChangeMember(sMember2);

	// Old guild leader
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		//sprintf(szMsg, "%s chuy觧 nhng v tr %s cho %s", szOldMaster, defTONG_MASTER_TITLE, szName);
		sprintf(szMsg, "%s chuy\xD3n nh\xAD\xEEng v\xDe tr\xDD %s cho %s.", szOldMaster, defTONG_MASTER_TITLE, szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
	}

	return TRUE;
}

BOOL	CTongControl::GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync)
{
	if (!pLogin || !pSync)
		return FALSE;

	int		i;
	DWORD	dwNameID = g_String2Id(pLogin->m_szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->m_btCamp		= this->m_nCamp;
	pSync->m_btRecruit		= this->m_nRecruit;
	strcpy(pSync->m_szTongName, this->m_szName);
	strcpy(pSync->m_szMaster, this->m_szMasterName);
	strcpy(pSync->m_szName, pLogin->m_szName);
	pSync->m_nMoney		= this->m_dwMoney;
	pSync->m_nLevel = this->m_nLevel;
	pSync->m_nExp = this->m_nExpGuide;
	this->m_bIsFull = this->m_nMemberNum + this->m_nDirectorNum + this->m_nManagerNum >= this->m_nCapSize && this->m_nCapSize > 0;
	pSync->m_bIsFull = this->m_bIsFull;
	if (this->m_dwMasterID == dwNameID)
	{
		pSync->m_btFigure	= enumTONG_FIGURE_MASTER;
		pSync->m_btPos		= 0;
		pSync->m_btFlag		= 1;
		GetMasterTitle(pSync->m_szTitle);
		return TRUE;
	}

	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (this->m_dwDirectorID[i] == dwNameID)
		{
			pSync->m_btFigure	= enumTONG_FIGURE_DIRECTOR;
			pSync->m_btPos		= i;
			pSync->m_btFlag		= 1;
			GetDirectorTitle(pSync->m_szTitle, i);
			return TRUE;
		}
	}

	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (this->m_dwManagerID[i] == dwNameID)
		{
			pSync->m_btFigure	= enumTONG_FIGURE_MANAGER;
			pSync->m_btPos		= i;
			pSync->m_btFlag		= 1;
			GetManagerTitle(pSync->m_szTitle, i);
			return TRUE;
		}
	}

	if (this->m_psMember)
	{
		for (i = 0; i < this->m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				pSync->m_btFigure	= enumTONG_FIGURE_MEMBER;
				pSync->m_btPos		= 0;
				pSync->m_btFlag		= 1;
				GetMemberTitle(pSync->m_szTitle, pLogin->m_nSex);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL	CTongControl::SearchOne(char *lpszName, STONG_ONE_LEADER_INFO *pInfo)
{
	if (!lpszName || !lpszName[0] || !pInfo)
		return FALSE;

	int		i;
	DWORD	dwNameID = g_String2Id(lpszName);
	if (dwNameID == 0)
		return FALSE;

	if (m_dwMasterID == dwNameID)
	{
		pInfo->m_btFigure	= enumTONG_FIGURE_MASTER;
		pInfo->m_btPos		= 0;
		strcpy(pInfo->m_szTitle, m_szMasterTitle);
		strcpy(pInfo->m_szName, m_szMasterName);
		return TRUE;
	}

	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == dwNameID)
		{
			pInfo->m_btFigure	= enumTONG_FIGURE_DIRECTOR;
			pInfo->m_btPos		= i;
			strcpy(pInfo->m_szTitle, m_szDirectorTitle[i]);
			strcpy(pInfo->m_szName, m_szDirectorName[i]);
			return TRUE;
		}
	}

	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (m_dwManagerID[i] == dwNameID)
		{
			pInfo->m_btFigure	= enumTONG_FIGURE_MANAGER;
			pInfo->m_btPos		= i;
			strcpy(pInfo->m_szTitle, m_szManagerTitle[i]);
			strcpy(pInfo->m_szName, m_szManagerName[i]);
			return TRUE;
		}
	}

	if (m_psMember)
	{
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				pInfo->m_btFigure	= enumTONG_FIGURE_MEMBER;
				pInfo->m_btPos		= 0;
				strcpy(pInfo->m_szTitle, m_szNormalTitle);
				strcpy(pInfo->m_szName, m_psMember[i].m_szName);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CTongControl::DBInstate( char *lpszPlayerName, BYTE nSite)
{
	if (!lpszPlayerName[0] || nSite > 4)
		return FALSE;

	int		i, nOldPos, nNewPos = 0;
	DWORD	dwNameID;
	char	szName[32];

	memcpy(szName, lpszPlayerName, sizeof(lpszPlayerName));
	szName[31] = 0;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;

	if (!m_psMember)
		return FALSE;
	int		nPos = -1;

	for (i = 0; i < m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID != dwNameID)
			continue;
		nPos = i;
		break;
	}
	if (nPos < 0)
		return FALSE;
	nOldPos = i;
	int nPlayerSex = this->m_psMember[nPos].m_nSex;

	switch (nSite)
	{
	case enumTONG_FIGURE_DIRECTOR:
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nNewPos = i;

		strcpy(m_szDirectorName[nNewPos], szName);
		m_dwDirectorID[nNewPos] = dwNameID;
		m_nDirectorSex[nNewPos] = nPlayerSex;
		this->m_psMember[nPos].m_szName[0] = 0;
		this->m_psMember[nPos].m_dwNameID = 0;
		m_nDirectorNum++;
		m_nMemberNum--;
		break;
	case enumTONG_FIGURE_MANAGER:
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nNewPos = i;

		strcpy(m_szManagerName[nNewPos], szName);
		m_dwManagerID[nNewPos] = dwNameID;
		m_nManagerSex[nNewPos] = nPlayerSex;
		this->m_psMember[nPos].m_szName[0] = 0;
		this->m_psMember[nPos].m_dwNameID = 0;
		m_nManagerNum++;
		m_nMemberNum--;
		break;
	default:
		return FALSE;
	}


	// Send a message to the chat channel
	TMemberStruct	sMember;
	if (nSite == enumTONG_FIGURE_DIRECTOR)
	{
		sMember.MemberClass = enumTONG_FIGURE_DIRECTOR;
	} 
	else if (nSite == enumTONG_FIGURE_MANAGER)
	{
		sMember.MemberClass = enumTONG_FIGURE_MANAGER;
	}
	sMember.nTitleIndex = nNewPos;
	strcpy(sMember.szTong, this->m_szName);
	strcpy(sMember.szName, szName);
	sMember.nSex = nPlayerSex;
	g_cTongDB.ChangeMember(sMember);
	return TRUE;
}

BOOL CTongControl::DBChangeTitle(STONG_ACCEPT_TITLE_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;

	char	szName[32];
	memcpy(szName, pAccept->m_szName, sizeof(pAccept->m_szName));
	szName[31] = 0;
	if (!szName[0])
		return FALSE;
	
	char	szTitle[32];
	memcpy(szTitle, pAccept->m_szTitle, sizeof(pAccept->m_szTitle));
	szTitle[31] = 0;
	if (!szTitle[0])
		return FALSE;

	if (pAccept->m_btAcceptFalg == 0)
	{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_CHANGE_TITLE_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_TITLE_FAIL;
				sFail.m_dwParam			= param;
				sFail.m_btFailID		= 1;
				sFail.m_dwTongNameID	= pAccept->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				tongconndup.SendPackage((const void *)&sFail, sizeof(sFail));
			}
			else
			{
				//tong is not connect
			}
		}
		else
		{
			//not find
		}
		
		return FALSE;
	}

	DWORD	dwNameID;
	int		i;
	
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;
	rTRACE("name id da > 0");
	char	szMsg[96];			// Save, save the data to the database
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	rTRACE("Kenh chat :%d",channid); 
	if (channid == -1)
		return FALSE;
	rTRACE("da co kenh chat");
	if (pAccept->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (strcmp(szTitle, m_szDirectorTitle[i]) == 0)
			return FALSE;
		strcpy(this->m_szDirectorTitle[i], szTitle);
		sprintf(szMsg, "Thay \xAE\xE6i danh hi\xD6u %s cho %s %s th\xb5nh c\xABng.", szTitle, defTONG_DIRECTOR_TITLE, m_szDirectorName[i]);
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;

		if (strcmp(szTitle, m_szManagerTitle[i]) == 0)
			return FALSE;

		strcpy(this->m_szManagerTitle[i], szTitle);
		//sprintf(szMsg, "Thay i danh hi謚 %s cho %s %s th祅h c玭g ", szTitle, defTONG_MANAGER_TITLE, m_szManagerName[i]);
		sprintf(szMsg, "Thay \xAE\xE6i danh hi\xD6u %s cho %s %s th\xb5nh c\xABng.", szTitle, defTONG_MANAGER_TITLE, m_szManagerName[i]);

	}
	else
	{
		return FALSE;

	}
	
	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_CHANGED_TITLE_SYNC	sSync;
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_TITLE;
				sSync.m_dwParam = param;
				switch (pAccept->m_btFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					strcpy(sSync.m_szTitle, m_szDirectorTitle[i]);
					break;
				case enumTONG_FIGURE_MANAGER:
					strcpy(sSync.m_szTitle, m_szManagerTitle[i]);
					break;
				}

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
		}
	}}
	g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
	// chua co id kenh tong
	return TRUE;
}

BOOL CTongControl::DBChangeSexTitle(STONG_ACCEPT_SEX_TITLE_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	
	char	szTitle[32];
	int i = 0;
	memcpy(szTitle, pAccept->m_szTitle, sizeof(pAccept->m_szTitle));
	szTitle[31] = 0;
	if (!szTitle[0])
		return FALSE;
//	rTRACE("title sex o s3relay -1");
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
//		rTRACE("title sex o s3relay 0");	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	
	if (channid == -1)				// thong bao len kenh chat
		return FALSE;
	rTRACE("gioi tinh : %d",pAccept->m_btSex);
	rTRACE("Danh hieu: %s - %s",m_szNormalGirlTitle, szTitle);
	if (pAccept->m_btSex)
	{
		if (strcmp(m_szNormalGirlTitle, szTitle) == 0)// chua co id kenh chat bang
			return FALSE;
//		rTRACE("title sex o s3relay 1");			// no chay cai cho nay thoi chu chua doi dc, tiep theo thi e ko biet lam tiep
		strcpy(this->m_szNormalGirlTitle, szTitle);
		sprintf(szMsg, "Thay \xAE\xE6i danh hi\xD6u n\xf7 trong bang th\xB5nh %s.", szTitle);
	}
	else
	{
		if (strcmp(m_szNormalBoyTitle, szTitle) == 0)
			return FALSE;
		rTRACE("title sex o s3relay 2");
		strcpy(this->m_szNormalBoyTitle, szTitle);
		//sprintf(szMsg, "Thay i danh hi謚 nam trong bang th祅h %s", szTitle);
		sprintf(szMsg, "Thay \xAE\xE6i danh hi\xD6u nam trong bang th\xB5nh %s.", szTitle);
	}

	if (!m_psMember || m_nMemberPointSize <= 0)
		return FALSE;
		rTRACE("title sex o s3relay 3");
	for (i = 0; i < this->m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
		rTRACE("gt db %d - gt input %d",m_psMember[i].m_nSex, pAccept->m_btSex);
		if (m_psMember[i].m_nSex == pAccept->m_btSex)
		{
			{{
				CNetConnectDup conndup;
				DWORD nameid = 0;
				unsigned long param = 0;
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
				{
					CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_TITLE_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_TITLE;
						sSync.m_dwParam = param;
						strcpy(sSync.m_szTitle, szTitle);
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
						rTRACE("title sex o s3relay: %s", sSync.m_szTitle);
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
		}
	}
	if (i >= m_nMemberPointSize)
		return FALSE;
	rTRACE("title sex o s3relay 4");
	g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
	return TRUE;
}

BOOL	CTongControl::GetMasterTitle(char *lpszTitle)
{
	if (!lpszTitle)
		return FALSE;
	if (m_szMasterTitle[0])
		strcpy(lpszTitle, m_szMasterTitle);
	else {
		strcpy(lpszTitle, defTONG_MASTER_TITLE);
	}
	return TRUE;
}

BOOL	CTongControl::GetDirectorTitle(char *lpszTitle, int nPos)
{
	if (!lpszTitle)
		return FALSE;
	if (m_szDirectorTitle[nPos][0])
		strcpy(lpszTitle, m_szDirectorTitle[nPos]);
	else
		strcpy(lpszTitle, defTONG_DIRECTOR_TITLE);
	return TRUE;
}

BOOL	CTongControl::GetManagerTitle(char *lpszTitle, int nPos)
{
	if (!lpszTitle)
		return FALSE;
	if (m_szManagerTitle[nPos][0])
		strcpy(lpszTitle, m_szManagerTitle[nPos]);
	else
		strcpy(lpszTitle, defTONG_MANAGER_TITLE);
	return TRUE;
}

BOOL	CTongControl::GetMemberTitle(char *lpszTitle, int nSex)
{
	if (!lpszTitle)
		return FALSE;
	if (nSex)
		strcpy(lpszTitle, m_szNormalGirlTitle);
	else
		strcpy(lpszTitle, m_szNormalBoyTitle);
	if (!lpszTitle[0])
	{
		if (m_szNormalTitle[0])
			strcpy(lpszTitle, m_szNormalTitle);
		else
			strcpy(lpszTitle, defTONG_MEMBER_TITLE);
	}
	return TRUE;
}

BOOL CTongControl::ChangeMoney( STONG_MONEY_COMMAND* pMoney, STONG_MONEY_SYNC *Sync)
{
	if (!pMoney)
		return FALSE;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	
	Sync->ProtocolFamily = pf_tong;
	Sync->m_dwTongNameID = m_dwNameID;
	Sync->m_dwParam = pMoney->m_dwParam;
	strcpy(Sync->m_szName,pMoney->m_szName);
	switch(pMoney->ProtocolID)
	{
	case enumC2S_TONG_MONEY_SAVE:
		m_dwMoney += pMoney->m_dwMoney;
		
		Sync->ProtocolID = enumS2C_TONG_MONEY_SAVE;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;
		if(channid > 0)
		{
			//sprintf(szMsg, "%s  c鑞g hi課 cho bang %d lng ", pMoney->m_szName, pMoney->m_dwMoney);
			sprintf(szMsg, "%s \xAE\xB7 c\xe8ng hi\xCFn cho bang %d l\xAD\xEEng.", pMoney->m_szName, pMoney->m_dwMoney);
			WriteLogFile(szMsg);
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		}
		break;
	case enumC2S_TONG_MONEY_GET:
		if (m_dwMoney < pMoney->m_dwMoney)
			return FALSE;
		m_dwMoney -= pMoney->m_dwMoney;
		
		Sync->ProtocolID = enumS2C_TONG_MONEY_GET;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;
		if (channid != -1)
		{
			//sprintf(szMsg, "%s  r髏 %d lng!!!", pMoney->m_szName, pMoney->m_dwMoney);
			sprintf(szMsg, "%s \xae\xB7 r\xF3t %d l\xAD\xEEng!!!", pMoney->m_szName, pMoney->m_dwMoney);
			WriteLogFile(szMsg);
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		}
		break;
	case enumC2S_TONG_MONEY_SND:
		if (m_dwMoney < pMoney->m_dwMoney)
			return FALSE;
		m_dwMoney -= pMoney->m_dwMoney;
		
		Sync->ProtocolID = enumS2C_TONG_MONEY_SND;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;
		
		if (channid != -1)
		{
			//sprintf(szMsg, "%s ph竧 %d lng cho t蕋 c bang.", pMoney->m_szName, pMoney->m_dwMoney);
			sprintf(szMsg, "%s ph\xB8t %d l\xAD\xEEng cho t\xCAt c bang.", pMoney->m_szName, pMoney->m_dwMoney);
			WriteLogFile(szMsg);
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		}
		break;
	}
	TMoneyStruct tMoney;
	strcpy(tMoney.szTong, m_szName);
	tMoney.m_dwMoney = m_dwMoney;
	g_cTongDB.DelMoney(m_szName);
	g_cTongDB.ChangeMoney(tMoney);
	
	return TRUE;
}

BOOL CTongControl::DBChangeTongLevel(STONG_CHANGE_LEVEL_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;

	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	//if (pChange->m_btFigure == enumTONG_FIGURE_MASTER || pChange->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (m_nLevel == pChange->m_nTongLevel)
			return FALSE;

		m_nLevel = pChange->m_nTongLevel;

		//sprintf(szMsg, "Bang h閕 thay i ng c蕄 th祅h %d.", pChange->m_nTongLevel);
		sprintf(szMsg, "Bang h\xE9i thay \xAE\xE6i \xae\xBCng c\xCAp th\xB5nh %d.", pChange->m_nTongLevel);

		int i = 0;
		
		{{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
			{
				tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_BE_CHANGED_LEVEL_SYNC	sSync;
					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_LEVEL;
					sSync.m_dwParam = param;
					sSync.m_nTongLevel = m_nLevel;
					tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
				}
				tongconndup.Clearup();
			}
			conndup.Clearup();
		}}
		
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_LEVEL_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_LEVEL;
						sSync.m_dwParam = param;
						sSync.m_nTongLevel = m_nLevel;
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}
		
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_LEVEL_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_LEVEL;
						sSync.m_dwParam = param;
						sSync.m_nTongLevel = m_nLevel;
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}

		if (m_psMember && m_nMemberPointSize > 0)
		{
			for (i = 0; i < this->m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID == 0)
					break;
				
				{{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_LEVEL_SYNC	sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_LEVEL;
							sSync.m_dwParam = param;
							sSync.m_nTongLevel = m_nLevel;
							tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}}
				Sleep(5);
			}
		}
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeTongExp(STONG_CHANGE_EXP_COMMAND* pChange)
{
	if (!pChange)
		return FALSE;

	DWORD dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;

	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;

	char szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

//	if (pChange->m_btFigure == enumTONG_FIGURE_MASTER || pChange->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (m_nExpGuide == pChange->m_nTongExp)
			return FALSE;

		m_nExpGuide = pChange->m_nTongExp;

		sprintf(szMsg, "Bang h閕 thay i kinh nghi謒 th祅h %d.", pChange->m_nTongExp);

		int i = 0;

		// Notify the master
		{
			{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_EXP_SYNC sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_EXP;
						sSync.m_dwParam = param;
						sSync.m_nTongExp = m_nExpGuide;
						tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}
		}

		// Notify directors
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
				break;
			{
				{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_EXP_SYNC sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_EXP;
							sSync.m_dwParam = param;
							sSync.m_nTongExp = m_nExpGuide;
							tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}
			}
			Sleep(5);
		}

		// Notify managers
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
				break;
			{
				{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_EXP_SYNC sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_EXP;
							sSync.m_dwParam = param;
							sSync.m_nTongExp = m_nExpGuide;
							tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}
			}
			Sleep(5);
		}

		// Notify members
		if (m_psMember && m_nMemberPointSize > 0)
		{
			for (i = 0; i < this->m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID == 0)
					break;

				{
					{
						if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
						{
							tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
							if (tongconndup.IsValid())
							{
								STONG_BE_CHANGED_EXP_SYNC sSync;
								sSync.ProtocolFamily = pf_tong;
								sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_EXP;
								sSync.m_dwParam = param;
								sSync.m_nTongExp = m_nExpGuide;
								tongconndup.SendPackage((const void*)&sSync, sizeof(sSync));
							}
							tongconndup.Clearup();
						}
						conndup.Clearup();
					}
				}
				Sleep(5);
			}
		}

		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeTongWayEdit(STONG_CHANGE_WAYEDIT_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;

	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	if (pChange->m_btFigure == enumTONG_FIGURE_MASTER || pChange->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (strcmp(m_szWayEdit, pChange->m_szWayEdit) == 0)
			return FALSE;

		strcpy(m_szWayEdit, pChange->m_szWayEdit);

		//sprintf(szMsg, "%s thay i ti猽 ch th祅h %s.", pChange->m_szName, pChange->m_szWayEdit);
		sprintf(szMsg, "%s thay \xae\xE6i ti\xAAu ch\xdd th\xB5nh %s.", pChange->m_szName, pChange->m_szWayEdit);

		int i = 0;
		
		{{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
			{
				tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_BE_CHANGED_WAYEDIT_SYNC	sSync;
					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_WAYEDIT;
					sSync.m_dwParam = param;
					strcpy(sSync.m_szWayEdit, m_szWayEdit);
					tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
				}
				tongconndup.Clearup();
			}
			conndup.Clearup();
		}}
		
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_WAYEDIT_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_WAYEDIT;
						sSync.m_dwParam = param;
						strcpy(sSync.m_szWayEdit, m_szWayEdit);
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}
		
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_WAYEDIT_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_WAYEDIT;
						sSync.m_dwParam = param;
						strcpy(sSync.m_szWayEdit, m_szWayEdit);
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}

		if (m_psMember && m_nMemberPointSize > 0)
		{
			for (i = 0; i < this->m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID == 0)
					break;
				
				{{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_WAYEDIT_SYNC	sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_WAYEDIT;
							sSync.m_dwParam = param;
							strcpy(sSync.m_szWayEdit, m_szWayEdit);
							tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}}
				Sleep(5);
			}
		}
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeTongNextTarget(STONG_CHANGE_NEXTTARGET_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;

	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	if (pChange->m_btFigure == enumTONG_FIGURE_MASTER || pChange->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (strcmp(m_szNextTargetEdit, pChange->m_szNextTarget) == 0)
			return FALSE;

		strcpy(m_szNextTargetEdit, pChange->m_szNextTarget);

		//sprintf(szMsg, "%s thay i m鬰 ti猽 th祅h %s.", pChange->m_szName, pChange->m_szNextTarget);
		sprintf(szMsg, "%s thay \xae\xE6i m\xF4\x63 ti\xAAu th\xb5nh %s.", pChange->m_szName, pChange->m_szNextTarget);
		int i = 0;
		
		{{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
			{
				tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_BE_CHANGED_NEXTTARGET_SYNC	sSync;
					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_NEXTTARGET;
					sSync.m_dwParam = param;
					strcpy(sSync.m_szNextTarget, m_szNextTargetEdit);
					tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
				}
				tongconndup.Clearup();
			}
			conndup.Clearup();
		}}
		
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_NEXTTARGET_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_NEXTTARGET;
						sSync.m_dwParam = param;
						strcpy(sSync.m_szNextTarget, m_szNextTargetEdit);
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}
		
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_NEXTTARGET_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_NEXTTARGET;
						sSync.m_dwParam = param;
						strcpy(sSync.m_szNextTarget, m_szNextTargetEdit);
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}

		if (m_psMember && m_nMemberPointSize > 0)
		{
			for (i = 0; i < this->m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID == 0)
					break;
				
				{{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_NEXTTARGET_SYNC	sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_NEXTTARGET;
							sSync.m_dwParam = param;
							strcpy(sSync.m_szNextTarget, m_szNextTargetEdit);
							tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}}
				Sleep(5);
			}
		}
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeRecruit(STONG_CHANGE_RECRUIT_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;

	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	if (pChange->m_btFigure == enumTONG_FIGURE_MASTER || pChange->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (m_nRecruit == pChange->m_btRecruit)
			return FALSE;

		m_nRecruit = pChange->m_btRecruit;

		if (pChange->m_btRecruit == 0)
			//sprintf(szMsg, "%s ng ch  tuy觧 th祅h vi猲 bang.", pChange->m_szName);
			sprintf(szMsg, "%s x\xe3\x61 ch\xd5 \xae\xE9 tuy\xD3n th\xb5nh vi\xAAn bang.", pChange->m_szName);
		else
			//sprintf(szMsg, "%s m ch  tuy觧 th祅h vi猲 bang.", pChange->m_szName);
			sprintf(szMsg, "%s \xae\x61ng ch\xd5 \xae\xE9 tuy\xD3n th\xb5nh vi\xAAn bang.", pChange->m_szName);

		int i = 0;
		
		{{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
			{
				tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_BE_CHANGED_RECRUIT_SYNC	sSync;
					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_RECRUIT;
					sSync.m_dwParam = param;
					sSync.m_btRecruit = m_nRecruit;
					tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
				}
				tongconndup.Clearup();
			}
			conndup.Clearup();
		}}
		
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_RECRUIT_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_RECRUIT;
						sSync.m_dwParam = param;
						sSync.m_btRecruit = m_nRecruit;
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}
		
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_RECRUIT_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_RECRUIT;
						sSync.m_dwParam = param;
						sSync.m_btRecruit = m_nRecruit;
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}

		if (m_psMember && m_nMemberPointSize > 0)
		{
			for (i = 0; i < this->m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID == 0)
					break;
				
				{{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_RECRUIT_SYNC	sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_RECRUIT;
							sSync.m_dwParam = param;
							sSync.m_btRecruit = m_nRecruit;
							tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}}
				Sleep(5);
			}
		}
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeCamp(STONG_CHANGE_CAMP_COMMAND *pChange) //t輓h n╪g n祔  l祄 頲 tr猲 t蕋 c c竎 gs
{
	if (!pChange)
		return FALSE;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;

	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;

	if (m_dwMoney < pChange->m_nMoney)
	{
		if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
		{
			tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_CHANGE_CAMP_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_CAMP_FAIL;
				sFail.m_dwParam			= param;
				sFail.m_btFailID		= 8;
				sFail.m_dwTongNameID	= pChange->m_dwTongNameID;
				memcpy(sFail.m_szName, m_szMasterName, sizeof(m_szMasterName));
				tongconndup.SendPackage((const void *)&sFail, sizeof(sFail));
			}
		}
		return FALSE;
	}
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	if (pChange->m_btFigure == enumTONG_FIGURE_MASTER || pChange->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (m_nCamp == pChange->m_btCamp)
			return FALSE;
		m_nCamp = pChange->m_btCamp;

		m_dwMoney -= pChange->m_nMoney;
		
		TMoneyStruct tMoney;
		strcpy(tMoney.szTong,m_szName);
		tMoney.m_dwMoney = m_dwMoney;
		g_cTongDB.DelMoney(m_szName);
		g_cTongDB.ChangeMoney(tMoney);

		if (pChange->m_btCamp == camp_justice)
			//sprintf(szMsg, "%s thay i phe bang th祅h l藀 ch ch輓h ph竔.", pChange->m_szName);
			sprintf(szMsg, "%s thay \xae\xE6i phe bang th\xb5nh ch\xDdnh ph\xB8i.", pChange->m_szName);
		else if (pChange->m_btCamp == camp_evil)
			sprintf(szMsg, "%s thay \xae\xE6i phe bang th\xb5nh t\xb5 ph\xB8i.", pChange->m_szName);
		else if (pChange->m_btCamp == camp_balance)
			sprintf(szMsg, "%s thay \xae\xE6i phe bang th\xb5nh trung l\xcbp.", pChange->m_szName);

		int i = 0;
		
		{{
			if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
			{
				tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
				if (tongconndup.IsValid())
				{
					STONG_BE_CHANGED_CAMP_SYNC	sSync;
					sSync.ProtocolFamily = pf_tong;
					sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_CAMP;
					sSync.m_dwParam = param;
					sSync.m_btCamp = m_nCamp;
					tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
				}
				tongconndup.Clearup();
			}
			conndup.Clearup();
		}}
		
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szDirectorName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_CAMP_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_CAMP;
						sSync.m_dwParam = param;
						sSync.m_btCamp = m_nCamp;
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}
		
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0)
				break;
			{{
				if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_szManagerName[i]), &conndup, NULL, &nameid, &param))
				{
					tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_CAMP_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_CAMP;
						sSync.m_dwParam = param;
						sSync.m_btCamp = m_nCamp;
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
			Sleep(5);
		}

		if (m_psMember && m_nMemberPointSize > 0)
		{
			for (i = 0; i < this->m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID == 0)
					break;
				
				{{
					if (g_TongServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
					{
						tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
						if (tongconndup.IsValid())
						{
							STONG_BE_CHANGED_CAMP_SYNC	sSync;
							sSync.ProtocolFamily = pf_tong;
							sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_CAMP;
							sSync.m_dwParam = param;
							sSync.m_btCamp = m_nCamp;
							tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
						}
						tongconndup.Clearup();
					}
					conndup.Clearup();
				}}
				Sleep(5);
			}
		}
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}