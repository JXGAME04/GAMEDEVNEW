//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.cpp
// Date:	2003.08.12
// Code:	Fong Ki“u
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTong.h"
#ifndef _SERVER
#include	"CoreShell.h"
#endif
#define		ID_MAP_TONGKIM								379	//cac map ko doi camp dc
#define		ID_MAP_BAODANH_TONGKIM		  324
#define		ID_MAP_LIENDAU								73
#define		ID_MAP_LIENDAU_CAOCAP			74
#define		ID_MAP_BAODANH_LIENDAU		72
#define		ID_MAP_CONGTHANH					221
#define		ID_MAP_HAUPHUONG_CONG		223
#define		ID_MAP_HAUPHUONG_THU		   222
#define		defFuncShowNormalMsg(str)		\
	{										\
		KSystemMessage	sMsg;				\
		sMsg.eType = SMT_NORMAL;			\
		sMsg.byConfirmType = SMCT_NONE;		\
		sMsg.byPriority = 0;				\
		sMsg.byParamSize = 0;				\
		sprintf(sMsg.szMessage, str);		\
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);\
	}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≥ı ºªØ
//-------------------------------------------------------------------------
void	KPlayerTong::Init(int nPlayerIdx)
{
	m_nPlayerIndex = nPlayerIdx;

	Clear();
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫«Âø’
//-------------------------------------------------------------------------
void	KPlayerTong::Clear()
{
	m_nFlag				= 0;
	m_nFigure			= enumTONG_FIGURE_MEMBER;
	m_nCamp				= 0;
	m_dwTongNameID		= 0;
	m_szName[0]			= 0;
	m_szTitle[0]		= 0;
	m_szMasterName[0]	= 0;
	m_nApplyTo			= 0;
	m_nRecruit = 0;
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î¥¥Ω®∞Ôª·
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyCreateTong(int nCamp, char *lpszTongName)
{
	defFuncShowNormalMsg(MSG_TONG_APPLY_CREATE);

	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR01);
		return FALSE;
	}

	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR02);
		return FALSE;
	}

	if (m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR03);
		return FALSE;
	}

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR04);
		return FALSE;
	}

	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR05);
		return FALSE;
	}
	if ((int)Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR06);
		return FALSE;
	}

	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}

	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoneyAmount() < PlayerSet.m_sTongParam.m_nMoney)
	{
		char	szBuf[80];
		sprintf(szBuf, MSG_TONG_CREATE_ERROR07, PlayerSet.m_sTongParam.m_nMoney);
		defFuncShowNormalMsg(szBuf);
		return FALSE;
	}

	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_CREATE_ERROR08);
		return FALSE;
	}

	int nSex = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nSex;

	TONG_APPLY_CREATE_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_CREATE_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CREATE;
	sApply.m_btCamp = (BYTE)nCamp;
	sApply.m_btSex = (BYTE)nSex;
	strcpy(sApply.m_szName, lpszTongName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Îº”»Î∞Ôª·
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyAddTong(DWORD dwNpcID)
{
	defFuncShowNormalMsg(MSG_TONG_APPLY_ADD);
	// “—æ≠ «∞Ôª·≥…‘±
	if (m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR1);
		return FALSE;
	}
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp != camp_free)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR2);
		return FALSE;
	}
	if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Level < 60)
	{
		defFuncShowNormalMsg("ßºng c p d≠Ìi 60 kh´ng th” gia nhÀp bang hÈi");
		return FALSE;
	}
	// Ωª“◊π˝≥Ã÷–
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
	{
		return FALSE;
	}
	// ◊È∂”≤ªƒ‹Ω®∞Ôª·
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
	{
		defFuncShowNormalMsg(MSG_TONG_APPLY_ADD_ERROR3);
		return FALSE;
	}

	TONG_APPLY_ADD_COMMAND	sApply;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_wLength = sizeof(TONG_APPLY_ADD_COMMAND) - 1;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_ADD;
	sApply.m_dwNpcID = dwNpcID;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sizeof(TONG_APPLY_ADD_COMMAND));

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≈–∂œ¥¥Ω®∞Ôª·Ãıº˛ «∑Ò≥…¡¢
//-------------------------------------------------------------------------
int		KPlayerTong::CheckCreateCondition(int nCamp, char *lpszTongName)
{
	if (Player[m_nPlayerIndex].m_nIndex <= 0)
		return enumTONG_CREATE_ERROR_ID1;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CREATE_ERROR_ID2;

	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH)
		return enumTONG_CREATE_ERROR_ID3;

	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return enumTONG_CREATE_ERROR_ID4;
	
	if (m_nFlag)
		return enumTONG_CREATE_ERROR_ID5;
	
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free)
		return enumTONG_CREATE_ERROR_ID6;
	
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel || 
		(int)Player[m_nPlayerIndex].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
		return enumTONG_CREATE_ERROR_ID7;
	
	if (Player[m_nPlayerIndex].m_ItemList.GetMoneyAmount() < PlayerSet.m_sTongParam.m_nMoney)
		return enumTONG_CREATE_ERROR_ID8;
	
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return enumTONG_CREATE_ERROR_ID9;
	if (!Player[m_nPlayerIndex].m_ItemList.IsTaskItemExist(PlayerSet.m_sTongParam.m_nItemReq))
	{
		return enumTONG_CREATE_ERROR_ID14;		
	}
	return 0;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫µ√µΩrelayÕ®÷™£¨∞Ôª·¥¥Ω®≥…π¶£¨¥¶¿Ìœ‡”¶ ˝æ›
//-------------------------------------------------------------------------
BOOL	KPlayerTong::Create(int nCamp, char *lpszTongName)
{
//	if (!CheckCreateCondition(nCamp, lpszTongName))
//		return FALSE;

	m_nFlag			= 1;
	m_nFigure		= enumTONG_FIGURE_MASTER;
	m_nCamp			= nCamp;
	m_szTitle[0]	= 0;
	strcpy(m_szName, lpszTongName);
	strcpy(m_szMasterName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	Player[m_nPlayerIndex].m_ItemList.DecMoney(PlayerSet.m_sTongParam.m_nMoney);
	Player[m_nPlayerIndex].m_ItemList.RemoveTaskItem(PlayerSet.m_sTongParam.m_nItemReq);

	Player[m_nPlayerIndex].ExecuteScript("\\script\\log_game\\log_tong.lua", "creattong", m_nPlayerIndex);

	TONG_CREATE_SYNC	sCreate;
	sCreate.ProtocolType = s2c_tongcreate;
	sCreate.m_btCamp = nCamp;
	if (strlen(lpszTongName) < sizeof(sCreate.m_szName))
		strcpy(sCreate.m_szName, lpszTongName);
	else
	{
		memcpy(sCreate.m_szName, lpszTongName, sizeof(sCreate.m_szName) - 1);
		sCreate.m_szName[sizeof(sCreate.m_szName) - 1] = 0;
	}
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sCreate, sizeof(TONG_CREATE_SYNC));

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫µ√µΩ∑˛ŒÒ∆˜Õ®÷™¥¥Ω®∞Ôª·
//-------------------------------------------------------------------------
void	KPlayerTong::Create(TONG_CREATE_SYNC *psCreate)
{
	if (!psCreate)
		return;

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = psCreate->m_btCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = psCreate->m_btCamp;

	m_nFlag			= 1;
	m_nFigure		= enumTONG_FIGURE_MASTER;
	m_nCamp			= psCreate->m_btCamp;
	m_szTitle[0]	= 0;
	memset(m_szName, 0, sizeof(m_szName));
	memcpy(m_szName, psCreate->m_szName, sizeof(psCreate->m_szName));
	strcpy(m_szMasterName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	// Õ®÷™ΩÁ√Ê∞Ôª·Ω®¡¢≥…π¶
	defFuncShowNormalMsg(MSG_TONG_CREATE_SUCCESS);

	// Õ®÷™∞Ôª·∆µµ¿
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);

}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫Õ∑…œ «∑Ò–Ë“™∂•’“»À±Í÷æ
//-------------------------------------------------------------------------
BOOL	KPlayerTong::GetOpenFlag()
{
	return (m_nFlag && m_nFigure != enumTONG_FIGURE_MEMBER);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫◊™∑¢º”»Î∞Ôª·…Í«Î∏¯∂‘∑ΩøÕªß∂À
//-------------------------------------------------------------------------
BOOL	KPlayerTong::TransferAddApply(DWORD dwNpcID)
{
	// “—æ≠ «∞Ôª·≥…‘±
	if (m_nFlag)
		return FALSE;
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp != camp_free)
		return FALSE;
	// Ωª“◊π˝≥Ã÷–
	if (Player[m_nPlayerIndex].CheckTrading())
		return FALSE;
	// ◊È∂”≤ªƒ‹º”»Î∞Ôª·
	if (Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		return FALSE;

	int	nTarget = Player[m_nPlayerIndex].FindAroundPlayer(dwNpcID);
	if (nTarget == -1)
		return FALSE;
	if (!Player[nTarget].m_cTong.CheckAcceptAddApplyCondition())
		return FALSE;
	if (!Player[nTarget].m_cTong.CheckRecruitClose(m_nPlayerIndex,dwNpcID))
		return FALSE;
	

	m_nApplyTo = nTarget;

	// Õ®÷™øÕªß∂À
	TONG_APPLY_ADD_SYNC	sAdd;
	sAdd.ProtocolType = s2c_extendtong;
	sAdd.m_btMsgId = enumTONG_SYNC_ID_TRANSFER_ADD_APPLY;
	sAdd.m_nPlayerIdx = m_nPlayerIndex;
	strcpy(sAdd.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	sAdd.m_wLength = sizeof(TONG_APPLY_ADD_SYNC) - 1 - sizeof(sAdd.m_szName) + strlen(sAdd.m_szName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nTarget].m_nNetConnectIdx, &sAdd, sAdd.m_wLength + 1);

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≈–∂œ «∑Òø…“‘◊™∑¢±»Àµƒº”»Î∞Ôª·…Í«Î
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckAcceptAddApplyCondition()
{
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

	return TRUE;
}

BOOL KPlayerTong::CheckRecruitClose(int nPlayerIdx, DWORD dwNameID)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FALSE;

	//if (Player[nPlayerIdx].m_cTong.m_nApplyTo != m_nPlayerIndex || Player[nPlayerIdx].m_nIndex <= 0 || Player[m_nPlayerIndex].m_nIndex <= 0)
	//	return FALSE;

	//if (g_FileName2Id(Npc[Player[nPlayerIdx].m_nIndex].Name) != dwNameID)
	//	return FALSE;

	if(!m_nRecruit)
	{
		int nLength = strlen(Npc[Player[m_nPlayerIndex].m_nIndex].Name);
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_TONG_RECRUIT_CLOSE;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + nLength;
		sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];

		memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
		memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[m_nPlayerIndex].m_nIndex].Name, nLength);

		if (g_pServer)
		{
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
			g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		}

		return FALSE;
	}
	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ «∑ÒΩ” ‹≥…‘± bFlag == TRUE Ω” ‹ == FALSE ≤ªΩ” ‹
//-------------------------------------------------------------------------
void	KPlayerTong::AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag)
{
	if (nPlayerIdx <= 0)
		return;

	TONG_ACCEPT_MEMBER_COMMAND	sAccept;
	sAccept.ProtocolType	= c2s_extendtong;
	sAccept.m_wLength		= sizeof(TONG_ACCEPT_MEMBER_COMMAND) - 1;
	sAccept.m_btMsgId		= enumTONG_COMMAND_ID_ACCEPT_ADD;
	sAccept.m_nPlayerIdx	= nPlayerIdx;
	sAccept.m_dwNameID		= dwNameID;
	sAccept.m_btFlag		= (bFlag != 0);

	if (g_pClient)
		g_pClient->SendPackToServer(&sAccept, sAccept.m_wLength + 1);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫∑¢œ˚œ¢Õ®÷™æ‹æ¯ƒ≥»À…Í«Î
//-------------------------------------------------------------------------
void	KPlayerTong::SendRefuseMessage(int nPlayerIdx, DWORD dwNameID)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;
	if (Player[nPlayerIdx].m_cTong.m_nApplyTo != m_nPlayerIndex ||
		Player[nPlayerIdx].m_nIndex <= 0 ||
		Player[m_nPlayerIndex].m_nIndex <= 0)
		return;
	if (g_FileName2Id(Npc[Player[nPlayerIdx].m_nIndex].Name) != dwNameID)
		return;
	
	int nLength = strlen(Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TONG_REFUSE_ADD;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + nLength;
	sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];

	memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
	memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[m_nPlayerIndex].m_nIndex].Name, nLength);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
}
#endif

void	KPlayerTong::GetTongName(char *lpszGetName)
{
	if (!lpszGetName)
		return;
	if (!m_nFlag)
	{
		lpszGetName[0] = 0;
		return;
	}

	strcpy(lpszGetName, m_szName);
}

void	KPlayerTong::GetTongTitle(char *lpszGetTitle)
{
	if (!lpszGetTitle)
		return;
	if (!m_nFlag)
	{
		lpszGetTitle[0] = 0;
		return;
	}

	strcpy(lpszGetTitle, m_szTitle);
}

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫≈–∂œ±»Àº”»Î◊‘º∫∞Ôª·Ãıº˛ «∑Ò≥…¡¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckAddCondition(int nPlayerIdx)
{
//--------------------- ◊‘º∫µƒÃıº˛ -----------------------
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;

//--------------------- ∂‘∑ΩµƒÃıº˛ -----------------------
	//  «∑Ò…Í«Î¡À
	if (Player[nPlayerIdx].m_cTong.m_nApplyTo != this->m_nPlayerIndex)
		return FALSE;
	// “—æ≠ «∞Ôª·≥…‘±
	if (Player[nPlayerIdx].m_cTong.m_nFlag)
		return FALSE;
	// ◊‘º∫µƒ’Û”™Œ Ã‚
	if (Npc[Player[nPlayerIdx].m_nIndex].m_CurrentCamp != camp_free ||
		Npc[Player[nPlayerIdx].m_nIndex].m_Camp != camp_free)
		return FALSE;
	// Ωª“◊π˝≥Ã÷–
	if (Player[nPlayerIdx].CheckTrading())
		return FALSE;
	// ◊È∂”≤ªƒ‹º”»Î∞Ôª·
	if (Player[nPlayerIdx].m_cTeam.m_nFlag)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫º”»Î∞Ôª·£¨≥…Œ™∆’Õ®∞Ô÷⁄
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, char *lpszTongName, char *lpszMasterName, char *lpszTitleName)
{
	if (!lpszTongName || !lpszMasterName || !lpszTitleName)
		return FALSE;
	// ∞Ôª·’Û”™Œ Ã‚
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag		= 1;
	m_nFigure	= enumTONG_FIGURE_MEMBER;
	m_nCamp		= nCamp;
	strcpy(this->m_szName, lpszTongName);
	strcpy(this->m_szMasterName, lpszMasterName);
	strcpy(this->m_szTitle, lpszTitleName);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;

	// Õ®÷™øÕªß∂À
	TONG_Add_SYNC	sAdd;
	sAdd.ProtocolType = s2c_extendtong;
	sAdd.m_wLength = sizeof(sAdd) - 1;
	sAdd.m_btMsgId = enumTONG_SYNC_ID_ADD;
	sAdd.m_btCamp = this->m_nCamp;
	strcpy(sAdd.m_szTongName, m_szName);
	strcpy(sAdd.m_szTitle, m_szTitle);
	strcpy(sAdd.m_szMaster, m_szMasterName);

	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sAdd, sAdd.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫º”»Î∞Ôª·£¨≥…Œ™∆’Õ®∞Ô÷⁄
//-------------------------------------------------------------------------
BOOL	KPlayerTong::AddTong(int nCamp, char *lpszTongName, char *lpszTitle, char *lpszMaster)
{
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) > defTONG_NAME_MAX_LENGTH)
		return FALSE;
	// ∞Ôª·’Û”™Œ Ã‚
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;

	m_nFlag		= 1;
	m_nFigure	= enumTONG_FIGURE_MEMBER;
	m_nCamp		= nCamp;
	strcpy(m_szName, lpszTongName);
	if (!lpszTitle)
		this->m_szTitle[0] = 0;
	else
		strcpy(m_szTitle, lpszTitle);
	if (!lpszMaster)
		this->m_szMasterName[0] = 0;
	else
		strcpy(m_szMasterName, lpszMaster);
	m_dwTongNameID	= g_FileName2Id(m_szName);

	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;

	defFuncShowNormalMsg(MSG_TONG_ADD_SUCCESS);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î»Œ√¸
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (nCurFigure < 0 || nCurFigure >= enumTONG_FIGURE_NUM ||
		nNewFigure < 0 || nNewFigure >= enumTONG_FIGURE_NUM)
		return FALSE;
	if (nCurPos < 0 || nNewPos < 0)
		return FALSE;
	if (nCurFigure == nNewFigure && nCurPos == nNewPos)
		return FALSE;

	switch (m_nFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		// ∞Ô÷⁄√ª”–»Œ√¸»®¡¶
		return FALSE;
	case enumTONG_FIGURE_MANAGER:
		// ∂”≥§√ª”–»Œ√¸»®¡¶
		return FALSE;
	case enumTONG_FIGURE_DIRECTOR:
		// ≥§¿œ÷ª”–∂‘∂”≥§°¢∞Ô÷⁄µƒ»Œ√¸»®¡¶
		if ((nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER) ||
			(nNewFigure != enumTONG_FIGURE_MANAGER && nNewFigure != enumTONG_FIGURE_MEMBER))
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		break;
	case enumTONG_FIGURE_MASTER:
		// ∞Ô÷˜”–∂‘≥§¿œ°¢∂”≥§°¢∞Ô÷⁄µƒ»Œ√¸»®¡¶
		if (nCurFigure == enumTONG_FIGURE_MASTER || nNewFigure == enumTONG_FIGURE_MASTER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_MANAGER && nCurPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nCurFigure == enumTONG_FIGURE_DIRECTOR && nCurPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_MANAGER && nNewPos >= defTONG_MAX_MANAGER)
			return FALSE;
		if (nNewFigure == enumTONG_FIGURE_DIRECTOR && nNewPos >= defTONG_MAX_DIRECTOR)
			return FALSE;
		break;
	default:
		return FALSE;
	}

	TONG_APPLY_INSTATE_COMMAND	sApply;

	if (strlen(lpszName) >= sizeof(sApply.m_szName))
		return FALSE;
	sApply.ProtocolType = c2s_extendtong;
	sApply.m_btMsgId = enumTONG_COMMAND_ID_APPLY_INSTATE;
	sApply.m_dwTongNameID = g_FileName2Id(this->m_szName);
	sApply.m_btCurFigure = nCurFigure;
	sApply.m_btCurPos = nCurPos;
	sApply.m_btNewFigure = nNewFigure;
	sApply.m_btNewPos = nNewPos;
	memset(sApply.m_szName, 0, sizeof(sApply.m_szName));
	strcpy(sApply.m_szName, lpszName);
	sApply.m_wLength = sizeof(sApply) - 1;

	if (g_pClient)
		g_pClient->SendPackToServer(&sApply, sApply.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«ÎÃﬂ»À
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyKick(int nCurFigure, int nCurPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER)
		return FALSE;
	if (nCurFigure == enumTONG_FIGURE_MANAGER && (nCurPos < 0 || nCurPos >= defTONG_MAX_MANAGER))
		return FALSE;

	TONG_APPLY_KICK_COMMAND	sKick;
	sKick.ProtocolType		= c2s_extendtong;
	sKick.m_wLength			= sizeof(sKick) - 1;
	sKick.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_KICK;
	sKick.m_btFigure		= nCurFigure;
	sKick.m_btPos			= nCurPos;
	sKick.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sKick.m_szName, lpszName);

	if (g_pClient)
		g_pClient->SendPackToServer(&sKick, sKick.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î¥´Œª
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;

	TONG_APPLY_CHANGE_MASTER_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_MASTER;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure		= nCurFigure;
	sChange.m_btPos			= nPos;
	strcpy(sChange.m_szName, lpszName);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«Î¿Îø™∞Ôª·
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyLeave()
{
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE1);
		return FALSE;
	}
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		defFuncShowNormalMsg(MSG_TONG_CANNOT_LEAVE2);
		return FALSE;
	}

	TONG_APPLY_LEAVE_COMMAND	sLeave;
	sLeave.ProtocolType		= c2s_extendtong;
	sLeave.m_wLength		= sizeof(sLeave) - 1;
	sLeave.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_LEAVE;
	sLeave.m_btFigure		= m_nFigure;
	sLeave.m_btPos			= 0;
	sLeave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	strcpy(sLeave.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

	if (g_pClient)
		g_pClient->SendPackToServer(&sLeave, sLeave.m_wLength + 1);

	return TRUE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Í«ÎªÒµ√∞Ôª·–≈œ¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName/*=NULL*/)
{
	if (nInfoID < 0 || nInfoID >= enumTONG_APPLY_INFO_ID_NUM)
		return FALSE;

	TONG_APPLY_INFO_COMMAND	sInfo;
	sInfo.ProtocolType = c2s_extendtong;
	sInfo.m_btMsgId = enumTONG_COMMAND_ID_APPLY_INFO;
	sInfo.m_btInfoID = nInfoID;
	sInfo.m_nParam1 = nParam1;
	sInfo.m_nParam2 = nParam2;
	sInfo.m_nParam3 = nParam3;

	switch (nInfoID)
	{
	case enumTONG_APPLY_INFO_ID_SELF:
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;

	case enumTONG_APPLY_INFO_ID_MASTER:
		break;

	case enumTONG_APPLY_INFO_ID_DIRECTOR:
		break;

	case enumTONG_APPLY_INFO_ID_MANAGER:
		if (!m_nFlag)
			break;
	/*	if (this->m_nFigure != enumTONG_FIGURE_MASTER &&
			this->m_nFigure != enumTONG_FIGURE_DIRECTOR && 
			this->m_nFigure != enumTONG_FIGURE_MANAGER &&
			this->m_nFigure != enumTONG_FIGURE_MEMBER)
			break;*/

		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;

	case enumTONG_APPLY_INFO_ID_MEMBER:
		if (!m_nFlag)
			break;
		/*	if (this->m_nFigure != enumTONG_FIGURE_MASTER &&
			this->m_nFigure != enumTONG_FIGURE_DIRECTOR && 
			this->m_nFigure != enumTONG_FIGURE_MANAGER &&
			this->m_nFigure != enumTONG_FIGURE_MEMBER)
			break;*/

		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);

		break;

	case enumTONG_APPLY_INFO_ID_ONE:
		if (!lpszName || !lpszName[0])
			break;
		if (strlen(lpszName) >= 32)
			break;
		strcpy(sInfo.m_szBuf, lpszName);
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf) + strlen(lpszName);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	case enumTONG_APPLY_INFO_ID_TONG_HEAD:
		if (nParam1 <= 0 || nParam1 >= MAX_NPC)
			break;
		if (nParam1 != Player[CLIENT_PLAYER_INDEX].m_nIndex && Npc[nParam1].m_nTongFlag == 0)
			break;
		sInfo.m_nParam1 = Npc[nParam1].m_dwID;
		sInfo.m_wLength = sizeof(sInfo) - 1 - sizeof(sInfo.m_szBuf);
		if (g_pClient)
			g_pClient->SendPackToServer(&sInfo, sInfo.m_wLength + 1);
		break;
	}
	return TRUE;
}
#endif

DWORD	KPlayerTong::GetTongNameID()
{
	return (m_nFlag ? m_dwTongNameID : 0);
//	if (!m_nFlag)
//		return 0;
//	if (m_szName[0])
//		return 0;
//	return g_FileName2Id(m_szName);
}

void	KPlayerTong::GetTongMasterName(char *lpszGetMasterName)
{
	if (!lpszGetMasterName)
		return;
	if (!m_nFlag)
	{
		lpszGetMasterName[0] = 0;
		return;
	}
	
	strcpy(lpszGetMasterName, m_szMasterName);
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ «∑Ò”–»®¡¶≤È—Ø∂”≥§–≈œ¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetManagerInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	//if (m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	//	return TRUE;
		return TRUE;
}

//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ «∑Ò”–»®¡¶≤È—Ø∞Ô÷⁄–≈œ¢
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CanGetMemberInfo(DWORD dwTongNameID)
{
	if (!m_nFlag)
		return FALSE;
	if (dwTongNameID != g_FileName2Id(this->m_szName))
		return FALSE;
	//if (m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
	//	return TRUE;
		return TRUE;
}

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫∏¯øÕªß∂À∑¢ÀÕ◊‘º∫‘⁄∞Ôª·÷–µƒ–≈œ¢
//-------------------------------------------------------------------------
void	KPlayerTong::SendSelfInfo()
{
	TONG_SELF_INFO_SYNC	sInfo;
	sInfo.ProtocolType = s2c_extendtong;
	sInfo.m_wLength = sizeof(sInfo) - 1;
	sInfo.m_btMsgId = enumTONG_SYNC_ID_SELF_INFO;
	sInfo.m_btJoinFlag = this->m_nFlag;
	sInfo.m_btFigure = this->m_nFigure;
	sInfo.m_btCamp = this->m_nCamp;
	sInfo.m_dwMoney = this->m_dwMoney;
	sInfo.m_btRecruit = this->m_nRecruit;
	strcpy(sInfo.m_szMaster, this->m_szMasterName);
	strcpy(sInfo.m_szTitle, this->m_szTitle);
	strcpy(sInfo.m_szTongName, this->m_szName);
	if (g_pServer)
		g_pServer->PackDataToClient(Player[this->m_nPlayerIndex].m_nNetConnectIdx, &sInfo, sInfo.m_wLength + 1);
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫…Ë∂®◊‘…Ì∞Ôª·–≈œ¢
//-------------------------------------------------------------------------
void	KPlayerTong::SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo)
{
	if (pInfo->m_btJoinFlag == 0)
	{
		if (m_nFlag)
		{
			KUiGameObjectWithName	sUi;
			strcpy(sUi.szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
			sUi.nData = TONG_ACTION_LEAVE;
			sUi.nParam = m_nFigure;
			sUi.uParam = 0;
			sUi.szString[0] = 0;
			CoreDataChanged(GDCNI_TONG_ACTION_RESULT, (unsigned int)&sUi, 1);

			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = camp_free;
			if (!Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = camp_free;
		}
		Clear();
		CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
		return;
	}

	if (m_nFlag == 1 && m_nFigure != pInfo->m_btFigure)
	{
		//defFuncShowNormalMsg("Vﬁ tr› cÒa bπn trong bang hÈi Æ∑ Æ≠Óc thay ÆÊi");
	}

	this->m_nFlag = 1;
	this->m_nFigure = pInfo->m_btFigure;
	this->m_nCamp = pInfo->m_btCamp;
	this->m_dwMoney = pInfo->m_dwMoney;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;
	if (Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag == 0)
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;
	memcpy(this->m_szMasterName, pInfo->m_szMaster, sizeof(pInfo->m_szMaster));
	memcpy(this->m_szName, pInfo->m_szTongName, sizeof(pInfo->m_szTongName));
	memcpy(this->m_szTitle, pInfo->m_szTitle, sizeof(pInfo->m_szTitle));
	m_dwTongNameID	= g_FileName2Id(m_szName);
	this->m_nRecruit = pInfo->m_btRecruit;
	CoreDataChanged(GDCNI_PLAYER_BASE_INFO, 0, 0);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–»Œ√¸»®¿˚
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckInstatePower(TONG_APPLY_INSTATE_COMMAND *pApply)
{
	if (!pApply)
		return FALSE;
	if (pApply->m_wLength <= sizeof(TONG_APPLY_INSTATE_COMMAND) - 1 - sizeof(pApply->m_szName))
		return FALSE;
	if (!m_nFlag || (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR))
		return FALSE;
	if (pApply->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	if (pApply->m_btCurFigure == pApply->m_btNewFigure && pApply->m_btCurPos == pApply->m_btNewPos)
		return FALSE;

	char	szName[32];
	memcpy(szName, pApply->m_szName, sizeof(pApply->m_szName));
	szName[31] = 0;
	if (strlen(szName) >= 31)
		return FALSE;

	if (m_nFigure == enumTONG_FIGURE_MASTER)
	{
		if (pApply->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}

		if (pApply->m_btNewFigure == enumTONG_FIGURE_DIRECTOR)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_DIRECTOR)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}
	}
	else // if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pApply->m_btCurFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btCurPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btCurFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}

		if (pApply->m_btNewFigure == enumTONG_FIGURE_MANAGER)
		{
			if (pApply->m_btNewPos >= defTONG_MAX_MANAGER)
				return FALSE;
		}
		else if (pApply->m_btNewFigure == enumTONG_FIGURE_MEMBER)
		{
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–Ãﬂ»À»®¿˚
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckKickPower(TONG_APPLY_KICK_COMMAND *pKick)
{
	if (!pKick)
		return FALSE;
	if (pKick->m_wLength + 1 != sizeof(TONG_APPLY_KICK_COMMAND))
		return FALSE;
	if (!m_nFlag || (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR))
		return FALSE;
	if (pKick->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (pKick->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pKick->m_btPos >= defTONG_MAX_MANAGER)
			return FALSE;
	}
	else if (pKick->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
	}
	else
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫±ª»Œ√¸
//-------------------------------------------------------------------------
void	KPlayerTong::BeInstated(STONG_SERVER_TO_CORE_BE_INSTATED *pSync)
{
	this->m_nFlag = 1;
	this->m_nFigure = pSync->m_btFigure;
	strcpy(m_szTitle, pSync->m_szTitle);

	SendSelfInfo();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫±ªÃﬂ≥ˆ∞Ôª·
//-------------------------------------------------------------------------
void	KPlayerTong::BeKicked(STONG_SERVER_TO_CORE_BE_KICKED *pSync)
{
	Clear();

	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;
	if (!Player[this->m_nPlayerIndex].m_cTeam.m_nFlag)
		Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = camp_free;

	SendSelfInfo();

	SHOW_MSG_SYNC	sMsg;
	// ±ªÃﬂ≥ˆ∞Ôª·
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TONG_BE_KICK;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–¿Îø™»®¿˚
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckLeavePower(TONG_APPLY_LEAVE_COMMAND *pLeave)
{
	if (!pLeave)
		return FALSE;
	if (pLeave->m_wLength + 1 != sizeof(TONG_APPLY_LEAVE_COMMAND))
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (pLeave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫¿Îø™∞Ôª·
//-------------------------------------------------------------------------
void	KPlayerTong::Leave(STONG_SERVER_TO_CORE_LEAVE *pLeave)
{
	if (strcmp(pLeave->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return;

	if (pLeave->m_bSuccessFlag)
	{
		Clear();
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;
		if (!Player[this->m_nPlayerIndex].m_cTeam.m_nFlag)
			Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = camp_free;
		SendSelfInfo();

		SHOW_MSG_SYNC	sMsg;
		// ¿Îø™∞Ôª·≥…π¶
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_LEAVE_SUCCESS;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	}
	else
	{
		SendSelfInfo();

		SHOW_MSG_SYNC	sMsg;
		// ¿Îø™∞Ôª· ß∞‹
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TONG_LEAVE_FAIL;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–»®¿˚ªª∞Ô÷˜
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckChangeMasterPower(TONG_APPLY_CHANGE_MASTER_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_MASTER_COMMAND))
		return FALSE;
	if (!m_nFlag || m_nFigure != enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	char	szName[32];
	memcpy(szName, pChange->m_szName, sizeof(pChange->m_szName));
	szName[31] = 0;
	if (strlen(szName) >= 31)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ºÏ≤‚ «∑Ò”–ƒ‹¡¶Ω” ‹¥´Œª
//-------------------------------------------------------------------------
BOOL	KPlayerTong::CheckGetMasterPower(STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER *pCheck)
{
	if (!pCheck)
		return FALSE;
	if (!m_nFlag || m_nFigure == enumTONG_FIGURE_MASTER)
		return FALSE;
	if (pCheck->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
//	if (m_nFigure != pCheck->m_btFigure)
//		return FALSE;
	if (strcmp(pCheck->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return FALSE;
	// µ»º∂Œ Ã‚
	if (Npc[Player[m_nPlayerIndex].m_nIndex].m_Level < PlayerSet.m_sTongParam.m_nLevel || 
		(int)Player[m_nPlayerIndex].m_dwLeadLevel < PlayerSet.m_sTongParam.m_nLeadLevel)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫¥´Œªµº÷¬…Ì∑›∏ƒ±‰
//-------------------------------------------------------------------------
void	KPlayerTong::ChangeAs(STONG_SERVER_TO_CORE_CHANGE_AS *pAs)
{
	if (!pAs)
		return;
	if (!m_nFlag)
		return;
	if (pAs->m_dwTongNameID != g_FileName2Id(m_szName))
		return;

	m_nFigure = pAs->m_btFigure;
	strcpy(m_szTitle, pAs->m_szTitle);
	strcpy(this->m_szMasterName, pAs->m_szName);

	this->SendSelfInfo();

	SHOW_MSG_SYNC	sMsg;

	sMsg.ProtocolType = s2c_msgshow;
	if (m_nFigure == enumTONG_FIGURE_MASTER)
		sMsg.m_wMsgID = enumMSG_ID_TONG_CHANGE_AS_MASTER;
	else
		sMsg.m_wMsgID = enumMSG_ID_TONG_CHANGE_AS_MEMBER;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);

	return;
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫ªª∞Ô÷˜
//-------------------------------------------------------------------------
void	KPlayerTong::ChangeMaster(char *lpszMaster)
{
	if (!lpszMaster || !lpszMaster[0])
		return;
	if (strlen(lpszMaster) >= sizeof(this->m_szMasterName))
		return;
	strcpy(m_szMasterName, lpszMaster);

	this->SendSelfInfo();
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------
//	π¶ƒ‹£∫µ«¬Ω ±∫ÚªÒµ√∞Ôª·–≈œ¢
//-------------------------------------------------------------------------
void	KPlayerTong::Login(STONG_SERVER_TO_CORE_LOGIN *pLogin)
{
	if (strcmp(Npc[Player[this->m_nPlayerIndex].m_nIndex].Name, pLogin->m_szName) != 0)//#can kiem tra
	{
		Clear();
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;
		if (!Player[this->m_nPlayerIndex].m_cTeam.m_nFlag)
			Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
		SendSelfInfo();
		return;
	}

	if (pLogin->m_nFlag == 0)
	{
		Clear();
		Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = camp_free;
		if (!Player[m_nPlayerIndex].m_cTeam.m_nFlag)
			Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
		this->SendSelfInfo();

		return;
	}

	m_nFlag			= 1;
	m_nFigure		= pLogin->m_nFigure;
	m_nCamp			= pLogin->m_nCamp;
	m_dwTongNameID	= g_FileName2Id(pLogin->m_szTongName);
	m_nApplyTo		= 0;
	strcpy(m_szName, pLogin->m_szTongName);
	strcpy(m_szTitle, pLogin->m_szTitle);
	strcpy(m_szMasterName, pLogin->m_szMaster);
	m_nRecruit = pLogin->m_nRecruit;
	m_dwMoney = pLogin->m_nMoney;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
	if (!Player[m_nPlayerIndex].m_cTeam.m_nFlag)
		Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	//strcpy(Npc[Player[m_nPlayerIndex].m_nIndex].m_szTongName, pLogin->m_szTongName);//edit by phong kieu send tong name vao Npc
	this->SendSelfInfo();
}
#endif

#ifndef _SERVER
void    KPlayerTong::OpenCreateInterface()
{
	CoreDataChanged(GDCNI_OPEN_TONG_CREATE_SHEET, 1, 0);
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyChangeTitle(int nCurFigure, int nCurPos, char *lpszName, char *lpszTitle)
{
	if (!lpszName || !lpszName[0] || strlen(lpszName) >= 32)
		return FALSE;
	if (!lpszTitle || !lpszTitle[0] || strlen(lpszTitle) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (nCurFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (nCurFigure == enumTONG_FIGURE_MANAGER && (nCurPos < 0 || nCurPos >= defTONG_MAX_MANAGER))
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (nCurFigure != enumTONG_FIGURE_MANAGER && nCurFigure != enumTONG_FIGURE_MEMBER)
			return FALSE;
	}
	
	TONG_APPLY_CHANGE_TITLE_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_TITLE;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure		= nCurFigure;
	sChange.m_btPos			= nCurPos;
	strcpy(sChange.m_szName, lpszName);
	strcpy(sChange.m_szTitle, lpszTitle);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTong::CheckChangeTitlePower(TONG_APPLY_CHANGE_TITLE_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_TITLE_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (pChange->m_btFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;
	if (pChange->m_btFigure == enumTONG_FIGURE_MANAGER && (pChange->m_btPos < 0 || pChange->m_btPos >= defTONG_MAX_MANAGER))
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_DIRECTOR)
	{
		if (pChange->m_btFigure != enumTONG_FIGURE_MANAGER && pChange->m_btFigure != enumTONG_FIGURE_MEMBER)
			return FALSE;
	}
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	char	szName[32];
	memcpy(szName, pChange->m_szName, sizeof(pChange->m_szName));
	szName[31] = 0;
	if (strlen(szName) >= 31)
		return FALSE;

	char	szTitle[32];
	memcpy(szTitle, pChange->m_szTitle, sizeof(pChange->m_szTitle));
	szTitle[31] = 0;
	if (strlen(szTitle) >= 31)
		return FALSE;

	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTong::CheckGetTitlePower(STONG_SERVER_TO_CORE_CHECK_GET_TITLE_POWER *pCheck)
{
	if (!pCheck)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure == enumTONG_FIGURE_MEMBER)
		return FALSE;
	if (pCheck->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	if (strcmp(pCheck->m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name) != 0)
		return FALSE;
	if (strcmp(pCheck->m_szTitle, m_szTitle) == 0)
		return FALSE;

	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyChangeSexTitle(char *lpszTitle, int nSex)
{
	if (!lpszTitle || !lpszTitle[0] || strlen(lpszTitle) >= 32)
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	
	TONG_APPLY_CHANGE_SEX_TITLE_COMMAND	sChange;
	sChange.ProtocolType	= c2s_extendtong;
	sChange.m_wLength		= sizeof(sChange) - 1;
	sChange.m_btMsgId		= enumTONG_COMMAND_ID_APPLY_CHANGE_SEX_TITLE;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btSex			= nSex;
	strcpy(sChange.m_szTitle, lpszTitle);
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTong::CheckChangeSexTitlePower(TONG_APPLY_CHANGE_SEX_TITLE_COMMAND *pChange)
{
	if (!pChange)
		return FALSE;
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_SEX_TITLE_COMMAND))
		return FALSE;
	if (!m_nFlag)
		return FALSE;
	if (m_nFigure != enumTONG_FIGURE_MASTER && m_nFigure != enumTONG_FIGURE_DIRECTOR)
		return FALSE;
	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	char	szTitle[32];
	memcpy(szTitle, pChange->m_szTitle, sizeof(pChange->m_szTitle));
	szTitle[31] = 0;
	if (strlen(szTitle) >= 31)
		return FALSE;
	
	return TRUE;
}
#endif

#ifdef _SERVER
void	KPlayerTong::BeChangedTitle(STONG_SERVER_TO_CORE_BE_CHANGED_TITLE *pSync)
{
	strcpy(m_szTitle, pSync->m_szTitle);
	
	SendSelfInfo();
}
#endif

#ifndef _SERVER
BOOL KPlayerTong::ApplySaveMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	//if (m_nFigure != enumTONG_FIGURE_MASTER )
	//{
	//	KSystemMessage	sMsg;
	//	sprintf(sMsg.szMessage, "T›nh n®ng chÿ sˆ dÙng cho bang chÒ.");
	//	sMsg.eType = SMT_NORMAL;
	//	sMsg.byConfirmType = SMCT_NONE;
	//	sMsg.byPriority = 0;
	//	sMsg.byParamSize = 0;
	//	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
	//	return FALSE;
	//}
	if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment) < nMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_MONEY_SAVE;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL KPlayerTong::ApplyGetMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "T›nh n®ng chÿ sˆ dÙng cho bang chÒ.");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	
	if (this->m_dwMoney < nMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Ng©n khË kh´ng ÆÒ!!!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	
	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_MONEY_GET;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL KPlayerTong::FkTongSndMoneyCheckWorld()
{
	int m_nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	if(Npc[m_nIndex].m_FightMode)
		return FALSE;
	int m_SubWorldID = SubWorld[Npc[m_nIndex].m_SubWorldIndex].m_SubWorldID;
	if(!m_SubWorldID)
		return FALSE;
	if(m_SubWorldID == 78)//thµnh t≠¨ng d≠¨ng
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL KPlayerTong::ApplySndMoney(DWORD nMoney)
{
	if (!m_nFlag)
		return FALSE;
	if (nMoney <= 0)
		return FALSE;
	int nIdx = -1, nCount = 0, nCountManager = 0;
	
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "T›nh n®ng chÿ sˆ dÙng cho bang chÒ.");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}

	if (this->m_dwMoney < nMoney)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Ng©n khË kh´ng ÆÒ!!!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	
	if(!FkTongSndMoneyCheckWorld())
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Thµnh vi™n bang ph∂i Æ’n Nha M´n T≠¨ng D≠¨ng mÌi Æ≠Óc mÎ ng©n khË");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;
	}
	
/*	nIdx = PlayerSet.GetFirstPlayer();
	while (nIdx)
	{
		if (strcmp(Npc[Player[nIdx].m_nIndex].TongName,m_szName) == 0)
		{
			nCount++;
			if (Npc[Player[nIdx].m_nIndex].m_nFigure == enumTONG_FIGURE_MANAGER)
			{
				nCountManager++;			
			}
		}
		nIdx = PlayerSet.GetNextPlayer();
	}
	
	if (nCount == 0)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Bang hÈi ch≠a c„ thµnh vi™n!!!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;	
	}
	
	if (nCountManager == 0)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, "Bang hÈi ch≠a c„ thµnh vi™n lµ Æ≠Íng chÒ!!!");
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		return FALSE;	
	}	*/
	
	TONG_APPLY_SAVE_COMMAND	sSave;
	sSave.ProtocolType		= c2s_extendtong;
	sSave.m_wLength			= sizeof(sSave) - 1;
	sSave.m_btMsgId			= enumTONG_COMMAND_ID_APPLY_MONEY_SND;
	sSave.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sSave.m_dwMoney			= nMoney;
	strcpy(sSave.m_szName, Npc[Player[m_nPlayerIndex].m_nIndex].Name);
	if (g_pClient)
		g_pClient->SendPackToServer(&sSave, sSave.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL KPlayerTong::CheckMoneyPower( TONG_APPLY_SAVE_COMMAND *pSave )
{
	if (!pSave)
		return FALSE;
	if (pSave->m_wLength + 1 != sizeof(TONG_APPLY_SAVE_COMMAND))
		return FALSE;
	if ((!m_nFlag || m_nFigure == enumTONG_FIGURE_MANAGER || m_nFigure == enumTONG_FIGURE_MEMBER) && pSave->m_btMsgId != enumTONG_COMMAND_ID_APPLY_MONEY_SAVE)
		return FALSE;
	if (!m_nFlag && pSave->m_btMsgId == enumTONG_COMMAND_ID_APPLY_MONEY_SAVE)
		return FALSE;
	if (pSave->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;
	
	return TRUE;
}

void KPlayerTong::ChangeMoney(DWORD dwMoney)
{
	m_dwMoney = dwMoney;
	this->SendSelfInfo();
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyTongChangeRecruit(int nRecruit)
{
	if (!m_nFlag)
		return FALSE;
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR1);
		return FALSE;
	}
	TONG_APPLY_CHANGE_RECRUIT_COMMAND	sChange;
	sChange.ProtocolType = c2s_extendtong;
	sChange.m_wLength = sizeof(sChange) - 1;
	sChange.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CHANGE_RECRUIT;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure = m_nFigure;
	sChange.m_btRecruit = (BYTE)nRecruit;
	strcpy(sChange.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}

BOOL	KPlayerTong::ApplyTongChangeLevel(int nLevel)
{
	if (!m_nFlag)
		return FALSE;
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR1);
		return FALSE;
	}
	TONG_APPLY_CHANGE_LEVEL_COMMAND	sChange;
	sChange.ProtocolType = c2s_extendtong;
	sChange.m_wLength = sizeof(sChange) - 1;
	sChange.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CHANGE_LEVEL;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure = m_nFigure;
	sChange.m_nLevel = nLevel;
	strcpy(sChange.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}

BOOL	KPlayerTong::ApplyTongChangeWayEdit(char *lpszStr)
{
	if (!m_nFlag)
		return FALSE;
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR1);
		return FALSE;
	}
	TONG_APPLY_CHANGE_WAYEDIT_COMMAND	sChange;
	sChange.ProtocolType = c2s_extendtong;
	sChange.m_wLength = sizeof(sChange) - 1;
	sChange.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CHANGE_WAYEDIT;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure = m_nFigure;
	strcpy(sChange.m_szWayEdit, lpszStr);
	strcpy(sChange.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}

BOOL	KPlayerTong::ApplyTongChangeNextTarget(char *lpszStr)
{
	if (!m_nFlag)
		return FALSE;
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR1);
		return FALSE;
	}
	TONG_APPLY_CHANGE_NEXTTARGET_COMMAND	sChange;
	sChange.ProtocolType = c2s_extendtong;
	sChange.m_wLength = sizeof(sChange) - 1;
	sChange.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CHANGE_NEXTTARGET;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure = m_nFigure;
	strcpy(sChange.m_szNextTarget, lpszStr);
	strcpy(sChange.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}

#endif

#ifndef _SERVER
BOOL	KPlayerTong::ApplyTongChangeCamp(int nCamp)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return FALSE;

	if (!m_nFlag)
		return FALSE;

	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR1);
		return FALSE;
	}

	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
	{
		defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP_ERROR2);
		return FALSE;
	}
	
	if (nCamp == m_nCamp)
		return FALSE;

	if (m_dwMoney < (DWORD)PlayerSet.m_sTongParam.m_nMoneyChangeCamp)
	{
		char	szBuf[80];
		sprintf(szBuf, MSG_TONG_CHANGE_CAMP_ERROR3, PlayerSet.m_sTongParam.m_nMoneyChangeCamp);
		defFuncShowNormalMsg(szBuf);
		return FALSE;
	}

	defFuncShowNormalMsg(MSG_TONG_CHANGE_CAMP);

	TONG_APPLY_CHANGE_CAMP_COMMAND	sChange;
	sChange.ProtocolType = c2s_extendtong;
	sChange.m_wLength = sizeof(sChange) - 1;
	sChange.m_btMsgId = enumTONG_COMMAND_ID_APPLY_CHANGE_CAMP;
	sChange.m_dwTongNameID	= g_FileName2Id(this->m_szName);
	sChange.m_btFigure = m_nFigure;
	sChange.m_btCamp = (BYTE)nCamp;
	sChange.m_nMoney = PlayerSet.m_sTongParam.m_nMoneyChangeCamp;
	strcpy(sChange.m_szName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
	
	if (g_pClient)
		g_pClient->SendPackToServer(&sChange, sChange.m_wLength + 1);
	
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ChangeRecruit(int nRecruit)
{
	if (m_nRecruit == nRecruit)
		return FALSE;
	
	m_nRecruit		= nRecruit;
	return TRUE;
}
#endif

#ifndef _SERVER
BOOL	KPlayerTong::ChangeCamp(int nCamp)
{
	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return FALSE;
	
	if (nCamp == m_nCamp)
		return FALSE;
	
	m_nCamp		= nCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp = m_nCamp;
	
	return TRUE;
}
#endif

#ifdef _SERVER
int		KPlayerTong::CheckChangeCampCondition(TONG_APPLY_CHANGE_CAMP_COMMAND *pChange)
{
	if (!pChange)
		return enumTONG_CHANGE_CAMP_ERROR_ID1;
	
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_CAMP_COMMAND))
		return enumTONG_CHANGE_CAMP_ERROR_ID2;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CHANGE_CAMP_ERROR_ID3;

	if (!m_nFlag)
		return enumTONG_CHANGE_CAMP_ERROR_ID4;
	
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
		return enumTONG_CHANGE_CAMP_ERROR_ID5;

	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	if (pChange->m_btCamp != camp_justice && pChange->m_btCamp != camp_evil && pChange->m_btCamp != camp_balance)
		return enumTONG_CHANGE_CAMP_ERROR_ID6;

	if (m_nCamp == pChange->m_btCamp)
		return enumTONG_CHANGE_CAMP_ERROR_ID7;

	if (m_dwMoney < PlayerSet.m_sTongParam.m_nMoneyChangeCamp)
		return enumTONG_CHANGE_CAMP_ERROR_ID8;

	g_DebugLog("Du dieu kien");
	return 0;
}

int		KPlayerTong::CheckChangeRecutCondition(TONG_APPLY_CHANGE_RECRUIT_COMMAND *pChange)
{
	if (!pChange)
		return enumTONG_CHANGE_CAMP_ERROR_ID1;
	
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_RECRUIT_COMMAND))
		return enumTONG_CHANGE_CAMP_ERROR_ID2;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CHANGE_CAMP_ERROR_ID3;

	if (!m_nFlag)
		return enumTONG_CHANGE_CAMP_ERROR_ID4;
	
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
		return enumTONG_CHANGE_CAMP_ERROR_ID5;

	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return 0;
}

int		KPlayerTong::CheckChangeLevelCondition(TONG_APPLY_CHANGE_LEVEL_COMMAND *pChange)
{
	if (!pChange)
		return enumTONG_CHANGE_CAMP_ERROR_ID1;
	
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_LEVEL_COMMAND))
		return enumTONG_CHANGE_CAMP_ERROR_ID2;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CHANGE_CAMP_ERROR_ID3;

	if (!m_nFlag)
		return enumTONG_CHANGE_CAMP_ERROR_ID4;
	
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
		return enumTONG_CHANGE_CAMP_ERROR_ID5;

	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return 0;
}

int		KPlayerTong::CheckChangeWayEditCondition(TONG_APPLY_CHANGE_WAYEDIT_COMMAND *pChange)
{
	if (!pChange)
		return enumTONG_CHANGE_CAMP_ERROR_ID1;
	
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_WAYEDIT_COMMAND))
		return enumTONG_CHANGE_CAMP_ERROR_ID2;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CHANGE_CAMP_ERROR_ID3;

	if (!m_nFlag)
		return enumTONG_CHANGE_CAMP_ERROR_ID4;
	
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
		return enumTONG_CHANGE_CAMP_ERROR_ID5;

	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return 0;
}

int		KPlayerTong::CheckChangeNextTargetCondition(TONG_APPLY_CHANGE_NEXTTARGET_COMMAND *pChange)
{
	if (!pChange)
		return enumTONG_CHANGE_CAMP_ERROR_ID1;
	
	if (pChange->m_wLength + 1 != sizeof(TONG_APPLY_CHANGE_NEXTTARGET_COMMAND))
		return enumTONG_CHANGE_CAMP_ERROR_ID2;

	if (Player[m_nPlayerIndex].CheckTrading())
		return enumTONG_CHANGE_CAMP_ERROR_ID3;

	if (!m_nFlag)
		return enumTONG_CHANGE_CAMP_ERROR_ID4;
	
	if (!(m_nFigure == enumTONG_FIGURE_MASTER || m_nFigure == enumTONG_FIGURE_DIRECTOR))
		return enumTONG_CHANGE_CAMP_ERROR_ID5;

	if (pChange->m_dwTongNameID != g_FileName2Id(m_szName))
		return FALSE;

	return 0;
}
#endif

#ifdef _SERVER
void	KPlayerTong::BeChangedCamp(STONG_SERVER_TO_CORE_BE_CHANGED_CAMP *pSync)
{
	int nSubWorldIndex = Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
	if (nSubWorldID == ID_MAP_TONGKIM || nSubWorldID == ID_MAP_BAODANH_TONGKIM || 
		nSubWorldID == ID_MAP_LIENDAU || nSubWorldID == ID_MAP_LIENDAU_CAOCAP || nSubWorldID == ID_MAP_BAODANH_LIENDAU ||
		nSubWorldID == ID_MAP_CONGTHANH || nSubWorldID == ID_MAP_HAUPHUONG_CONG || nSubWorldID == ID_MAP_HAUPHUONG_THU)
		return;
	if (pSync->m_nCamp != camp_justice && pSync->m_nCamp != camp_evil && pSync->m_nCamp != camp_balance)
		return;

	if (pSync->m_nCamp == m_nCamp)
		return;
	
	m_nCamp		= pSync->m_nCamp;
	this->SendSelfInfo();//#can kiem tra
	Npc[Player[m_nPlayerIndex].m_nIndex].m_Camp = m_nCamp;
	Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	
	TONG_CHANGE_CAMP_SYNC	sSync;
	sSync.ProtocolType = s2c_extendtong;
	sSync.m_wLength = sizeof(sSync) - 1;
	sSync.m_btMsgId = enumTONG_SYNC_ID_CHANGE_CAMP;
	sSync.m_btCamp = this->m_nCamp;
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sSync, sSync.m_wLength + 1);
}
#endif

#ifdef _SERVER
void	KPlayerTong::BeChangedRecruit(STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT *pSync)
{
	int nSubWorldIndex = Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex;
	int nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
	if (nSubWorldID == ID_MAP_TONGKIM || nSubWorldID == ID_MAP_BAODANH_TONGKIM || 
		nSubWorldID == ID_MAP_LIENDAU || nSubWorldID == ID_MAP_LIENDAU_CAOCAP || nSubWorldID == ID_MAP_BAODANH_LIENDAU ||
		nSubWorldID == ID_MAP_CONGTHANH || nSubWorldID == ID_MAP_HAUPHUONG_CONG || nSubWorldID == ID_MAP_HAUPHUONG_THU)
		return;

	if (pSync->m_nRecruit == m_nRecruit)
		return;
	
	m_nRecruit		= pSync->m_nRecruit;
	this->SendSelfInfo();//#can kiem tra
	//Npc[Player[m_nPlayerIndex].m_nIndex].m_Recruit = pSync->m_nRecruit;
	//Npc[Player[m_nPlayerIndex].m_nIndex].m_CurrentCamp = m_nCamp;
	
	TONG_CHANGE_RECRUIT_SYNC	sSync;
	sSync.ProtocolType = s2c_extendtong;
	sSync.m_wLength = sizeof(sSync) - 1;
	sSync.m_btMsgId = enumTONG_SYNC_ID_CHANGE_RECRUIT;
	sSync.m_btRecruit = this->m_nRecruit;
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, &sSync, sSync.m_wLength + 1);
}
#endif





