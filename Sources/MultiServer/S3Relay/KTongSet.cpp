// KTongSet.cpp: implementation of the CTongSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "TongDB.h"
#include "KTongSet.h"
#include "S3Relay.h"
#define		defTONG_MASTER_TITLE			"Bang chñ"
#define		defTONG_DIRECTOR_TITLE			"Tr­ëng l·o"
#define		defTONG_MANAGER_TITLE			"§­êng chñ"
#define		defTONG_MEMBER_TITLE			"M«n ®Ö"
#define		defTONG_NAME_SAY_ON_CHANNEL		"C«ng bè"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTongSet::CTongSet()
{
	m_pcTong = NULL;
	m_nTongPointSize = 0;
	Init();
}

CTongSet::~CTongSet()
{
	DeleteAll();
}

void	CTongSet::Init()
{
	DeleteAll();

	m_pcTong = (CTongControl**)new LPVOID[defTONG_SET_INIT_POINT_NUM];
	m_nTongPointSize = defTONG_SET_INIT_POINT_NUM;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		m_pcTong[i] = NULL;
	}
}

void	CTongSet::DeleteAll()
{
	if (m_pcTong)
	{
		for (int i = 0; i < m_nTongPointSize; i++)
		{
			if (m_pcTong[i])
			{
				delete m_pcTong[i];
				m_pcTong[i] = NULL;
			}
		}
		delete []m_pcTong;
		m_pcTong = NULL;
	}
	m_nTongPointSize = 0;
}

int		CTongSet::Create(int nCamp, char *lpszPlayerName, char *lpszTongName, int nSex)
{
	// °ï»áÄ£¿é³ö´í
	if (!m_pcTong || m_nTongPointSize <= 0)
		return enumTONG_CREATE_ERROR_ID10;
	// Ãû×Ö×Ö·û´®³ö´í
	if (!lpszPlayerName || !lpszTongName)
		return enumTONG_CREATE_ERROR_ID11;
	// Ãû×Ö×Ö·û´®¹ý³¤
	if (strlen(lpszTongName) >= defTONG_STR_LENGTH ||
		strlen(lpszPlayerName) >= defTONG_STR_LENGTH)
		return enumTONG_CREATE_ERROR_ID12;

	int		i, nPos;
	DWORD	dwTongNameID, dwPlayerNameID;

	dwTongNameID = g_String2Id(lpszTongName);
	dwPlayerNameID = g_String2Id(lpszPlayerName);

	// Ãû³Æ¼ì²â£¬ÊÇ·ñÓÐÍ¬ÃûµÄ°ïÖ÷»òÕßÍ¬ÃûµÄ°ï»á
	for (i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID || m_pcTong[i]->m_dwMasterID == dwPlayerNameID))
			break;
	}
	// °ï»áÍ¬Ãû´íÎó
	if (i < m_nTongPointSize)
		return enumTONG_CREATE_ERROR_ID13;

	// ²éÕÒ¿ÕÎ»
	nPos = -1;
	for (i = 0; i < m_nTongPointSize; i++)
	{
		if (!m_pcTong[i])
		{
			nPos = i;
			break;
		}
	}
	// ÏÖÓÐÖ¸Õë¿Õ¼äÒÑÂú£¬·ÖÅäÐÂµÄ¸ü´óµÄÖ¸Õë¿Õ¼ä
	if (nPos < 0)
	{
		// ÁÙÊ±´æ´¢¾ÉµÄÖ¸Õë
		CTongControl**	pTemp;
		pTemp = (CTongControl**)new LPVOID[m_nTongPointSize];
		for (i = 0; i < m_nTongPointSize; i++)
			pTemp[i] = m_pcTong[i];

		// ·ÖÅäÐÂµÄ¸ü´óµÄÖ¸Õë¿Õ¼ä£¬´óÐ¡ÊÇÔ­À´µÄÁ½±¶
		delete []m_pcTong;
		m_pcTong = NULL;
		m_pcTong = (CTongControl**)new LPVOID[m_nTongPointSize * 2];
		for (i = 0; i < m_nTongPointSize; i++)
			m_pcTong[i] = pTemp[i];
		delete []pTemp;
		m_nTongPointSize *= 2;
		for (i = m_nTongPointSize / 2; i < m_nTongPointSize; i++)
			m_pcTong[i] = NULL;
		nPos = m_nTongPointSize / 2;
	}

	// ²úÉúÒ»¸öÐÂµÄ°ï»á
	m_pcTong[nPos] = new CTongControl(nCamp, lpszPlayerName, lpszTongName, nSex);
	// ²úÉúÊ§°Ü
	if (m_pcTong[nPos]->m_dwNameID == 0)
	{
		delete m_pcTong[nPos];
		m_pcTong[nPos] = NULL;
		return enumTONG_CREATE_ERROR_ID14;
	}

	// ´æÅÌ£¬Êý¾Ý±£´æÖÁÊý¾Ý¿â
	TMemberStruct	sMember;
	sMember.MemberClass = enumTONG_FIGURE_MASTER;
	sMember.nTitleIndex = 0;
	strcpy(sMember.szTong, m_pcTong[nPos]->m_szName);
	strcpy(sMember.szName, m_pcTong[nPos]->m_szMasterName);
	sMember.nSex = nSex;

	try
	{
		g_cTongDB.ChangeTong(*m_pcTong[nPos]);
		g_cTongDB.ChangeMember(sMember);
	}
	catch (...)
	{
		char	szMsg[96];
		sprintf(szMsg, "WORLD");
					
		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
		if (channid != -1)
		{
			sprintf(szMsg, "%s bÞ lçi lËp bang.", m_pcTong[nPos]->m_szName);
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
		}
	}

	return 0;
}

//----------------------------------------------------------------------
//	¹¦ÄÜ£ºÌí¼ÓÒ»¸ö°ï»á³ÉÔ±£¬if return == 0 ³É¹¦ else return error id
//----------------------------------------------------------------------
int		CTongSet::AddMember(char *lpszPlayerName, char *lpszTongName, int nSex)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;
	if (!lpszPlayerName || !lpszPlayerName[0] || !lpszTongName || !lpszTongName[0])
		return -1;
	if (strlen(lpszTongName) >= defTONG_STR_LENGTH ||
		strlen(lpszPlayerName) >= defTONG_STR_LENGTH)
		return -1;

	int		i;
	DWORD	dwTongNameID;

	dwTongNameID = g_String2Id(lpszTongName);

	// Ñ°ÕÒ°ï»á
	for (i = 0; i < m_nTongPointSize; i++)
	{
		// ÕÒµ½ÁË
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID))
		{
			if (!m_pcTong[i]->AddMember(lpszPlayerName, nSex))			// khong truyen sex o day
				return -1;
			else
			{
				// ´æÅÌ£¬Êý¾Ý±£´æÖÁÊý¾Ý¿â
				TMemberStruct	sMember;
				sMember.MemberClass = enumTONG_FIGURE_MEMBER;
				sMember.nTitleIndex = 0;
				strcpy(sMember.szTong, m_pcTong[i]->m_szName);
				strcpy(sMember.szName, lpszPlayerName);
//				int PlayerSex = m_pCoreServerShell->GetGameData(SGDI_CHARACTER_SEX, 0, nPlayerIdx);
				sMember.nSex = nSex;					// dau ma, tai sao no ko truyen
				g_cTongDB.ChangeMember(sMember);
				rTRACE("gia tri nSex [CTongSet]: %d", nSex);		// khong truyen sex 
				// ¸ø°ï»áÆµµÀ·¢ÏûÏ¢
				char	szMsg[96];
				sprintf(szMsg, "\\O%u", m_pcTong[i]->m_dwNameID);

				DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
				if (channid != -1)
				{
					sprintf(szMsg, "%s ®· gia nhËp bang ph¸i, xin c¸c huynh ®Ö chØ gi¸o.", lpszPlayerName);
					g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(defTONG_NAME_SAY_ON_CHANNEL), std::string(szMsg));
				}

				return i;
			}
		}
	}

	return -1;
}

//----------------------------------------------------------------------
//	¹¦ÄÜ£º»ñµÃ°ï»áÕóÓª
//----------------------------------------------------------------------
int		CTongSet::GetTongCamp(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return -1;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nCamp;

	return -1;
}

BOOL	CTongSet::GetMasterName(int nTongIdx, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;
	if (!m_pcTong[nTongIdx])
		return FALSE;
	strcpy(lpszName, m_pcTong[nTongIdx]->m_szMasterName);
	return TRUE;
}

BOOL	CTongSet::GetMemberTitle(int nTongIdx, char *lpszTitle, int nSex)
{
	if (!lpszTitle)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;
	if (!m_pcTong[nTongIdx])
		return FALSE;
	if (nSex > 0)
		strcpy(lpszTitle, m_pcTong[nTongIdx]->m_szNormalGirlTitle);
	else
		strcpy(lpszTitle, m_pcTong[nTongIdx]->m_szNormalBoyTitle);
	if (!lpszTitle[0])
	{
		if (m_pcTong[nTongIdx]->m_szNormalTitle[0])
			strcpy(lpszTitle, m_pcTong[nTongIdx]->m_szNormalTitle);
		else
			strcpy(lpszTitle, defTONG_MEMBER_TITLE);
	}
	return TRUE;
}

BOOL	CTongSet::GetTongHeadInfo(DWORD dwTongNameID, STONG_HEAD_INFO_SYNC *pInfo)
{
	if (!m_pcTong || m_nTongPointSize <= 0 || dwTongNameID == 0)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->GetTongHeadInfo(pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pApply->m_nParam1)
		{
			return m_pcTong[i]->GetTongManagerInfo(pApply, pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pApply->m_nParam1)
		{
			return m_pcTong[i]->GetTongMemberInfo(pApply, pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pInstate->m_dwTongNameID)
		{
			BOOL bRet = m_pcTong[i]->Instate(pInstate, pSync);
			if (bRet)
			{
				g_cTongDB.ChangeTong(*m_pcTong[i]);
				return TRUE;
			}
			return FALSE;
		}
	}

	return FALSE;
}

BOOL	CTongSet::Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync)
{
	if (!pKick || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pKick->m_dwTongNameID)
		{
			return m_pcTong[i]->Kick(pKick, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync)
{
	if (!pLeave || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pLeave->m_dwTongNameID)
		{
			return m_pcTong[i]->Leave(pLeave, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			return m_pcTong[i]->AcceptMaster(pAccept);
		}
	}

	return FALSE;
}

BOOL	CTongSet::InitFromDB()
{
	int		nTongNum;

	nTongNum = g_cTongDB.GetTongCount();
	if (nTongNum < 0)
		return FALSE;
	if (nTongNum == 0)
		return TRUE;
	CTongControl**	m_tmpTong;

	m_tmpTong = (CTongControl**)new LPVOID[nTongNum];
	for (int i = 0; i < nTongNum; i++)
	{
		m_tmpTong[i] = NULL;
	}

/*	int i;*/
	TTongList	*pList = new TTongList[nTongNum];
	memset(pList, 0, sizeof(TTongList) * nTongNum);

	int nGetNum = g_cTongDB.GetTongList(pList, nTongNum);
	if (nGetNum <= 0)
		return TRUE;

	for (i = 0; i < nGetNum; i++)
	{
		m_tmpTong[i] = new CTongControl(pList[i]);
		if (!m_tmpTong[i]->m_szName[0])
		{
			delete m_tmpTong[i];
			m_tmpTong[i] = NULL;
			continue;
		}
	}
	Init();
	g_cTongDB.Close();
	g_cTongDB.OpenNew();
	for (i = 0; i < nGetNum; i++)
	{
		if (m_tmpTong[i] && m_tmpTong[i]->m_szName[0])
		{
			Create(m_tmpTong[i]->m_nCamp,m_tmpTong[i]->m_szMasterName,m_tmpTong[i]->m_szName, m_tmpTong[i]->m_psMember[0].m_nSex);
			for (int j = 0; j < m_tmpTong[i]->m_nMemberPointSize;j++)
			{
				AddMember(m_tmpTong[i]->m_psMember[j].m_szName,m_tmpTong[i]->m_szName, m_tmpTong[i]->m_psMember[j].m_nSex);
			}
		}
	}
	for (i = 0; i < nGetNum; i++)
	{
		m_pcTong[i] = new CTongControl(pList[i]);
		if (!m_pcTong[i]->m_szName[0])
		{
			delete m_pcTong[i];
			m_pcTong[i] = NULL;
		}
	}
	for (i = 0; i < nGetNum; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_szName[0])
		{
			g_cTongDB.SearchTong(m_pcTong[i]->m_szName, *(m_pcTong[i]));
			for (int j = 0;j < m_tmpTong[i]->m_nManagerNum;j++)
			{
				m_pcTong[i]->DBInstate(m_tmpTong[i]->m_szManagerName[j],1);
			}

			for (j = 0;j < m_tmpTong[i]->m_nDirectorNum;j++)
			{
				m_pcTong[i]->DBInstate(m_tmpTong[i]->m_szDirectorName[j],2);
			}
		}
	}
	if (m_tmpTong)
	{
		delete [] m_tmpTong;
	}
	if (pList)
	{
		delete [] pList;
	}

	return TRUE;
}

BOOL	CTongSet::GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync)
{
	if (!pLogin || !pSync)
		return FALSE;

	memset(pSync, 0, sizeof(STONG_LOGIN_DATA_SYNC));
	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_LOGIN_DATA;
	pSync->m_btFlag			= 0;
	pSync->m_dwParam		= pLogin->m_dwParam;
	pSync->m_btRecruit		= pLogin->m_Recruit;

	if (!m_pcTong)
		return FALSE;

	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pLogin->m_dwTongNameID)
		{
			return m_pcTong[i]->GetLoginData(pLogin, pSync);
		}
	}

	return TRUE;
}

BOOL	CTongSet::SearchOne(DWORD dwTongNameID, char *lpszName, STONG_ONE_LEADER_INFO *pInfo)
{
	if (!m_pcTong)
		return FALSE;
	if (dwTongNameID == 0 || !lpszName || !lpszName[0] || !pInfo)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->SearchOne(lpszName, pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::AcceptTitle(STONG_ACCEPT_TITLE_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			BOOL bRet = m_pcTong[i]->DBChangeTitle(pAccept);
			if (bRet)
			{
				g_cTongDB.ChangeTong(*m_pcTong[i]);
				return TRUE;
			}
			return FALSE;
		}
	}
	
	return FALSE;
}

BOOL CTongSet::AcceptSexTitle(STONG_ACCEPT_SEX_TITLE_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			BOOL bRet = m_pcTong[i]->DBChangeSexTitle(pAccept);
			if (bRet)
			{
				g_cTongDB.ChangeTong(*m_pcTong[i]);
				return TRUE;
			}
			return FALSE;
		}
	}
	
	return FALSE;
}

BOOL CTongSet::ChangeMoney( STONG_MONEY_COMMAND *pMoney, STONG_MONEY_SYNC *Sync)
{
	if (!pMoney)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// Ñ°ÕÒ°ï»á
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pMoney->m_dwTongNameID)
		{
			return m_pcTong[i]->ChangeMoney(pMoney, Sync);
		}
	}
	
	return FALSE;
}

BOOL CTongSet::ChangeCamp(STONG_CHANGE_CAMP_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == pChange->m_dwTongNameID) && (m_pcTong[i]->m_dwMoney >= (DWORD)pChange->m_nMoney))
		{
			if (m_pcTong[i]->DBChangeCamp(pChange))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeRecruit(STONG_CHANGE_RECRUIT_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == pChange->m_dwTongNameID))
		{
			if (m_pcTong[i]->DBChangeRecruit(pChange))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongLevel(STONG_CHANGE_LEVEL_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == pChange->m_dwTongNameID))
		{
			if (m_pcTong[i]->DBChangeTongLevel(pChange))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongWayEdit(STONG_CHANGE_WAYEDIT_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == pChange->m_dwTongNameID))
		{
			if (m_pcTong[i]->DBChangeTongWayEdit(pChange))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongNextTarger(STONG_CHANGE_NEXTTARGET_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == pChange->m_dwTongNameID))
		{
			if (m_pcTong[i]->DBChangeTongNextTarget(pChange))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

int	CTongSet::GetExtPoint(STONG_GET_EXTPOINT_COMMAND *pExt)
{
	if (!pExt)
		return FALSE;
	return g_cTongDB.GetExtPoint(pExt->m_szAccountName);
}

BOOL	CTongSet::UpdateExtPoint(STONG_UPDATE_EXTPOINT_COMMAND *pExt)
{
	if (!pExt)
		return FALSE;
	return g_cTongDB.UpdateExtPoint(pExt->m_szAccountName, pExt->m_nExtPoint);
}