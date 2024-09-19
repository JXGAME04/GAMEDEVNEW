//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerTeam.cpp
// Date:	2020.01.06
// Code:	Fong KiÒu
// Desc:	Team Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KNpc.h"
#ifdef _SERVER
//#include	"KNetServer.h"
//#include "../../Headers/IServer.h"
#else
//#include	"KNetClient.h"
#include "../../Headers/IClient.h"
#include	"CoreShell.h"
#endif
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KPlayerTeam.h"
//#include	"MyAssert.h"

enum
{
	Team_S_Close = 0,
	Team_S_Open,
};

KTeam		g_Team[MAX_TEAM];
#ifdef _SERVER
KTeamSet	g_TeamSet;
#endif

#ifndef _SERVER
KPlayerTeam::KPlayerTeam()
{
	Release();
}
#endif

#ifndef _SERVER
void	KPlayerTeam::Release()
{
	m_nFlag = 0;
	m_nFigure = TEAM_CAPTAIN;
	m_nApplyCaptainID = -1;
	m_nApplyCaptainID = 0;
	m_dwApplyTimer = 0;
	m_bAutoRefuseInviteFlag = FALSE;	 // TRUE tù ®éng tõ chèi lêi mêi, tõ chèi vµo ®éi
	ReleaseList();
	m_bAutoAccecpt = FALSE;      //tù ®éng nhËn lêi mêi
	m_bAutoAccecptAll = FALSE; //tù ®éng vµo tÊt c¶ c¸c nhãm
	m_bPTTongCheckBox = FALSE;
}
#endif

#ifndef _SERVER
void	KPlayerTeam::ReleaseList()
{
	for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
		m_sApplyList[i].Release();

	memset(m_AutoPT_PlayerList, 0, sizeof(m_AutoPT_PlayerList));
}
#endif


#ifndef _SERVER
BOOL	KPlayerTeam::ApplyCreate()
{
	if (m_nFlag)
		return FALSE;

	PLAYER_APPLY_CREATE_TEAM	sCreateTeam;
	sCreateTeam.ProtocolType = c2s_teamapplycreate;
	if (g_pClient)
		g_pClient->SendPackToServer(&sCreateTeam, sizeof(PLAYER_APPLY_CREATE_TEAM));

	return TRUE;
}
#endif

#ifndef _SERVER
void	KPlayerTeam::InviteAdd(DWORD dwNpcID)
{
//	if ( !this->m_nFlag || this->m_nFigure != TEAM_CAPTAIN || g_Team[0].m_nState != Team_S_Open)
//		return;
	TEAM_INVITE_ADD_COMMAND	sAdd;
	sAdd.ProtocolType = c2s_teaminviteadd;
	sAdd.m_dwNpcID = dwNpcID;
	if (g_pClient)
		g_pClient->SendPackToServer(&sAdd, sizeof(TEAM_INVITE_ADD_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayerTeam::ReceiveInvite(TEAM_INVITE_ADD_SYNC *pInvite)
{
	if ( !pInvite )
		return;
	char	szName[32];
	char    m_szRTongName[32];
	int		nIdx;
	memset(szName, 0, sizeof(szName));
	memset(m_szRTongName, 0, sizeof(m_szRTongName));
	memcpy(szName, pInvite->m_szName, sizeof(pInvite->m_szName) - (sizeof(TEAM_INVITE_ADD_SYNC) - pInvite->m_wLength - 1));
	nIdx = pInvite->m_nIdx; //Idx cña player
	if (m_bAutoRefuseInviteFlag)
	{
		ReplyInvite(nIdx, 0); //tõ chèi vµo ®éi
	}
	else if (m_bAutoAccecpt)
	{
		if(m_bPTTongCheckBox)//vµo PT cïng bang
		{
			strcpy(m_szRTongName, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_szTongName);
			int nNpcIdx = NpcSet.SearchID(pInvite->m_dwID);
			if(strcmp(m_szRTongName, Npc[nNpcIdx].m_szTongName) == 0)
			{
				ReplyInvite(nIdx, 1);
				return;
			}
		}
		if(m_bAutoAccecptAll)//vµo tÊt c¶ c¸c nhãm
			ReplyInvite(nIdx, 1);
		else
		{
			for(int i = 0; i < defMAX_AUTO_MOVEMPSL; i++)
			{
				if(m_AutoPT_PlayerList[i][0] && strcmp(szName, m_AutoPT_PlayerList[i]) == 0)//chØ vµo pt víi nh÷ng ng­êi trong danh s¸ch
					ReplyInvite(nIdx, 1);
			}
		}
	}
	else
	{
		KUiPlayerItem	sPlayer;
		KSystemMessage	sMsg;

		strcpy(sPlayer.Name, szName);
		sPlayer.nIndex = pInvite->m_nIdx;
		sPlayer.uId = 0;
		sPlayer.nData = 0;

		sprintf(sMsg.szMessage, MSG_TEAM_GET_INVITE, szName);
		sMsg.eType = SMT_TEAM;
		sMsg.byConfirmType = SMCT_UI_TEAM_INVITE;
		sMsg.byPriority = 3;
		sMsg.byParamSize = sizeof(KUiPlayerItem);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
	}
}
#endif

#ifndef _SERVER
//---------------------------------------------------------------------------
//	nResult  if == 0 ¾Ü¾ø  if == 1 ½ÓÊÜ
//---------------------------------------------------------------------------
void	KPlayerTeam::ReplyInvite(int nIdx, int nResult)
{
	if (nIdx < 0 || nResult < 0 || nResult > 1)
		return;
	TEAM_REPLY_INVITE_COMMAND	sReply;
	sReply.ProtocolType = c2s_teamreplyinvite;
	sReply.m_nIndex = nIdx;
	sReply.m_btResult = nResult;
	if (g_pClient)
		g_pClient->SendPackToServer(&sReply, sizeof(TEAM_REPLY_INVITE_COMMAND));
}
#endif

#ifndef _SERVER
void	KPlayerTeam::SetAutoRefuseInvite(BOOL bFlag)//0 t¾t tù ®éng tõ chèi lêi mêi//1 bËt tù ®éng tõ chèi lêi mêi
{
	KSystemMessage	sMsg;
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;

	if (bFlag)
	{
		m_bAutoRefuseInviteFlag = TRUE;
		sprintf(sMsg.szMessage, MSG_TEAM_AUTO_REFUSE_INVITE);
	}
	else
	{
		m_bAutoRefuseInviteFlag = FALSE;
		sprintf(sMsg.szMessage, MSG_TEAM_NOT_AUTO_REFUSE_INVITE);
	}

	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
}
#endif

#ifndef _SERVER
BOOL	KPlayerTeam::GetAutoRefuseState()
{
	return m_bAutoRefuseInviteFlag;
}
#endif

#ifndef _SERVER
int	KPlayerTeam::GetTotalMemer()//add by phong kiÒu
{
	return g_Team[0].m_nMemNum;
}

void KPlayerTeam::DeleteMemerTimeAbsent(DWORD m_dwNpcID)//add by phong kiÒu ®uæi khái nhãm v¾ng mÆt qu¸ l©u
{
	g_Team[0].DeleteMember(m_dwNpcID);
}

int		KPlayerTeam::GetInfo(KUiPlayerTeam *pTeam)
{
	if (!pTeam)
		return 0;
	pTeam->nCaptainPower = g_Team[0].CalcCaptainPower();

	if (m_nFlag == 0)
		return 0;

	if (m_nFigure == TEAM_CAPTAIN)
	{
		pTeam->bTeamLeader = true;
		int nNo = 0;
		for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
		{
			if (this->m_sApplyList[i].m_dwNpcID > 0)
				nNo++;
		}
		pTeam->cNumTojoin = nNo;
	}
	else
	{
		pTeam->bTeamLeader = false;
		pTeam->cNumTojoin = 0;
	}
	pTeam->nTeamServerID = g_Team[0].m_nTeamServerID;
	pTeam->cNumMember = g_Team[0].m_nMemNum + 1;
	pTeam->bOpened = 0;
	if (g_Team[0].m_nState == Team_S_Open)
		pTeam->bOpened = 1;
	
	return 1;
}
#endif

#ifndef _SERVER
void	KPlayerTeam::UpdateInterface()
{
	KUiPlayerTeam	sTeam;
	if (GetInfo(&sTeam) == 0)
	{
		CoreDataChanged(GDCNI_TEAM, NULL, 0);
	}
	else
	{
		CoreDataChanged(GDCNI_TEAM, (unsigned int)&sTeam, 0);
	}
}
#endif

#ifndef _SERVER
void	KPlayerTeam::DeleteOneFromApplyList(DWORD dwNpcID)
{
	for (int i = 0; i < MAX_TEAM_APPLY_LIST; i++)
	{
		if (m_sApplyList[i].m_dwNpcID == dwNpcID)
		{
			m_sApplyList[i].Release();
			return;
		}
	}
}
#endif

#ifdef _SERVER
void	KPlayerTeam::Release()
{
	m_nFlag = 0;
	m_nID = -1;
	m_nFigure = 0;
	m_nApplyCaptainID = 0;
	memset(m_nInviteList, 0, sizeof(m_nInviteList));
	m_nListPos = 0;
}
#endif

#ifdef _SERVER
BOOL	KPlayerTeam::CreateTeam(int nIdx, PLAYER_APPLY_CREATE_TEAM *pCreateTeam)
{
	_ASSERT(pCreateTeam);

	if (!m_bCanTeamFlag)
	{
		PLAYER_SEND_CREATE_TEAM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_teamcreatefalse;
		sCreateFalse.m_btErrorID = Team_Create_Error_CannotCreate;
		g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_TEAM_FALSE));
		return FALSE;
	}

	if (m_nFlag)
	{
		PLAYER_SEND_CREATE_TEAM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_teamcreatefalse;
		sCreateFalse.m_btErrorID = Team_Create_Error_InTeam;
		g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_TEAM_FALSE));
		return FALSE;
	}

//	char	szTeamName[32];
//	memcpy(szTeamName, pCreateTeam->m_szTeamName, sizeof(szTeamName));
//	szTeamName[31] = 0;
//	if ( !szTeamName[0] )
//	{
//		PLAYER_SEND_CREATE_TEAM_FALSE	sCreateFalse;
//		sCreateFalse.ProtocolType = s2c_teamcreatefalse;
//		sCreateFalse.m_btErrorID = Team_Create_Error_Name;
//		SendToClient(Player[nIdx].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_TEAM_FALSE));
//		return FALSE;
//	}

	m_nID = g_TeamSet.CreateTeam(nIdx);//, szTeamName);
	if (m_nID >= 0)	// ¶ÓÎé´´½¨³É¹¦
	{
		m_nFlag = 1;
		m_nFigure = TEAM_CAPTAIN;
		m_nApplyCaptainID = 0;
		Npc[Player[nIdx].m_nIndex].RestoreCurrentCamp();

		if (Player[nIdx].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN)
			Player[nIdx].m_cMenuState.SetState(nIdx, PLAYER_MENU_STATE_NORMAL);

		PLAYER_SEND_CREATE_TEAM_SUCCESS	sCreateSuccess;
		sCreateSuccess.ProtocolType = s2c_teamcreatesuccess;
		sCreateSuccess.nTeamServerID = m_nID;

		g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, (BYTE*)&sCreateSuccess, sizeof(PLAYER_SEND_CREATE_TEAM_SUCCESS));
		g_Team[m_nID].SetTeamOpen();

		return TRUE;
	}
//	else if (m_nID == -1)					// ¶ÓÎé´´½¨Ê§°Ü£ºÍ¬Ãû
//	{
//		PLAYER_SEND_CREATE_TEAM_FALSE	sCreateFalse;
//		sCreateFalse.ProtocolType = s2c_teamcreatefalse;
//		sCreateFalse.m_btErrorID = Team_Create_Error_SameName;
//		SendToClient(Player[nIdx].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_TEAM_FALSE));
//		return FALSE;
//	}
	else if (m_nID == -2)
	{
		PLAYER_SEND_CREATE_TEAM_FALSE	sCreateFalse;
		sCreateFalse.ProtocolType = s2c_teamcreatefalse;
		sCreateFalse.m_btErrorID = Team_Create_Error_TeamFull;
		g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, (BYTE*)&sCreateFalse, sizeof(PLAYER_SEND_CREATE_TEAM_FALSE));
		return FALSE;
	}
	else	// ÆäËû´íÎó
	{
		return FALSE;
	}

	return TRUE;
}
#endif

#ifdef _SERVER
void	KPlayerTeam::InviteAdd(int nIdx, TEAM_INVITE_ADD_COMMAND *pAdd)
{
	if (!pAdd)
		return;

	if ( !this->m_nFlag || this->m_nFigure != TEAM_CAPTAIN || !g_Team[this->m_nID].IsOpen() )
		return;
	int nTargetIdx = Player[nIdx].FindAroundPlayer(pAdd->m_dwNpcID);
	if (nTargetIdx == -1)
		return;
	if (!Player[nTargetIdx].m_cTeam.m_bCanTeamFlag)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TARGET_CANNOT_ADD_TEAM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	this->m_nInviteList[this->m_nListPos] = nTargetIdx;
	this->m_nListPos++;
	this->m_nListPos %= MAX_TEAM_MEMBER;
	TEAM_INVITE_ADD_SYNC	sAdd;
	sAdd.ProtocolType = s2c_teaminviteadd;
	sAdd.m_nIdx = nIdx;
	sAdd.m_dwID = Npc[Player[nIdx].m_nIndex].m_dwID;//dwID Npc göi lêi mêi PT
	memset(sAdd.m_szName, 0, sizeof(sAdd.m_szName));
	strcpy(sAdd.m_szName, Npc[Player[nIdx].m_nIndex].Name);
	sAdd.m_wLength = sizeof(TEAM_INVITE_ADD_SYNC) - 1 - sizeof(sAdd.m_szName) + strlen(sAdd.m_szName);
	g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, (BYTE*)&sAdd, sAdd.m_wLength + 1);
}
#endif

#ifdef _SERVER
void	KPlayerTeam::GetInviteReply(int nSelfIdx, int nTargetIdx, int nResult)
{
	if (!m_nFlag || m_nFigure != TEAM_CAPTAIN || !g_Team[m_nID].IsOpen())
		return;

	int i;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (m_nInviteList[i] == nTargetIdx)
			break;
	}
	if (i >= MAX_TEAM_MEMBER)
		return;
	if (Player[nTargetIdx].m_nIndex == 0)
		return;

	if (nResult == 0)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_REFUSE_INVITE; //tõ chèi vµo ®éi
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(Npc[Player[nTargetIdx].m_nIndex].Name);
		sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];
		memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
		memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[nTargetIdx].m_nIndex].Name, sizeof(Npc[Player[nTargetIdx].m_nIndex].Name));
		g_pServer->PackDataToClient(Player[nSelfIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		return;
	}

	if (g_Team[m_nID].m_nMemNum >= MAX_TEAM_MEMBER ||
		g_Team[m_nID].m_nMemNum >= g_Team[m_nID].CalcCaptainPower())
		return;

	if (Player[nTargetIdx].m_cTeam.m_nFlag)
	{
		PLAYER_APPLY_LEAVE_TEAM	sLeave;
		sLeave.ProtocolType = c2s_teamapplyleave;
		Player[nTargetIdx].LeaveTeam((BYTE*)&sLeave);
	}

	if ( !g_Team[m_nID].AddMember(nTargetIdx) )
		return;

	if (g_Team[m_nID].m_nMemNum >= MAX_TEAM_MEMBER || g_Team[m_nID].CheckFull())
	{
		g_Team[m_nID].SetTeamClose();
	}

	Player[nTargetIdx].m_cTeam.Release();
	Player[nTargetIdx].m_cTeam.m_nFlag = 1;
	Player[nTargetIdx].m_cTeam.m_nFigure = TEAM_MEMBER;
	Player[nTargetIdx].m_cTeam.m_nID = m_nID;
	Npc[Player[nTargetIdx].m_nIndex].SetCurrentCamp(Npc[Player[nSelfIdx].m_nIndex].m_Camp);

	PLAYER_TEAM_ADD_MEMBER	sAddMem;
	memset(sAddMem.m_szName, 0, sizeof(sAddMem.m_szName));
	sAddMem.ProtocolType = s2c_teamaddmember;
	sAddMem.m_dwNpcID = Npc[Player[nTargetIdx].m_nIndex].m_dwID;
	sAddMem.m_btLevel = (DWORD)Npc[Player[nTargetIdx].m_nIndex].m_Level;
	strcpy(sAddMem.m_szName, Npc[Player[nTargetIdx].m_nIndex].Name);

	g_pServer->PackDataToClient(Player[nSelfIdx].m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));

	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (g_Team[m_nID].m_nMember[i] < 0 || g_Team[m_nID].m_nMember[i] == nTargetIdx)
			continue;
		g_pServer->PackDataToClient(Player[g_Team[m_nID].m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sAddMem, sizeof(PLAYER_TEAM_ADD_MEMBER));
	}

// ------------------------------------- ¸øÐÂ¶ÓÔ±·¢ÏûÏ¢ --------------------------
	int		nNpcIndex;
	PLAYER_SEND_SELF_TEAM_INFO	sSelfInfo;
	sSelfInfo.ProtocolType = s2c_teamselfinfo;
	sSelfInfo.nTeamServerID = m_nID;
	sSelfInfo.m_dwLeadExp = Player[nTargetIdx].m_dwLeadExp;
	sSelfInfo.m_btState = g_Team[m_nID].m_nState;
	// 
	nNpcIndex = Player[nSelfIdx].m_nIndex;
	sSelfInfo.m_dwNpcID[0] = Npc[nNpcIndex].m_dwID;
	sSelfInfo.m_btLevel[0] = (DWORD)Npc[nNpcIndex].m_Level;
	strcpy(sSelfInfo.m_szNpcName[0], Npc[nNpcIndex].Name);
	// 
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (g_Team[m_nID].m_nMember[i] >= 0)
		{
			nNpcIndex = Player[g_Team[m_nID].m_nMember[i]].m_nIndex;
			sSelfInfo.m_dwNpcID[i + 1] = Npc[nNpcIndex].m_dwID;
			sSelfInfo.m_btLevel[i + 1] = (DWORD)Npc[nNpcIndex].m_Level;
			strcpy(sSelfInfo.m_szNpcName[i + 1], Npc[nNpcIndex].Name);
		}
		else
		{
			sSelfInfo.m_dwNpcID[i + 1] = 0;
			sSelfInfo.m_btLevel[i + 1] = 0;
			memset(sSelfInfo.m_szNpcName[i + 1], 0, sizeof(sSelfInfo.m_szNpcName[i + 1]));
		}
	}
	// 
	g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, (BYTE*)&sSelfInfo, sizeof(PLAYER_SEND_SELF_TEAM_INFO));

	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wMsgID = enumMSG_ID_TEAM_SELF_ADD;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);

// --------------------------------- end --------------------------
}
#endif

#ifdef _SERVER
void	KPlayerTeam::SetCanTeamFlag(int nSelfIdx, BOOL bFlag)
{
	if (bFlag)
	{
		m_bCanTeamFlag = TRUE;
		return;
	}

	m_bCanTeamFlag = FALSE;
	if (nSelfIdx >= 0 && nSelfIdx < MAX_PLAYER)
	{
		PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
		sLeaveTeam.ProtocolType = c2s_teamapplyleave;
		Player[nSelfIdx].LeaveTeam((BYTE*)&sLeaveTeam);
	}
}
#endif

KTeam::KTeam()
{
	Release();
}

void	KTeam::Release()
{
	m_nCaptain = -1;
	m_nMemNum = 0;
	int		i;
	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		m_nMember[i] = -1;
	}
	m_nState = Team_S_Close;
#ifndef _SERVER
	for (i = 0; i < MAX_TEAM_MEMBER + 1; i++)
	{
		m_nMemLevel[i] = 0;
		memset(m_szMemName, 0, 32 * (MAX_TEAM_MEMBER + 1));
	}
	m_nTeamServerID = -1;
#endif
}

void	KTeam::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

int		KTeam::FindFree()
{
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (m_nMember[i] < 0)
			return i;
	}
	return -1;
}

int		KTeam::FindMemberID(DWORD dwNpcID)
{
	for (int i = 0; i <	MAX_TEAM_MEMBER; i++)
	{
#ifdef _SERVER
		if (m_nMember[i] >= 0 && Npc[Player[m_nMember[i]].m_nIndex].m_dwID == dwNpcID)
			return i;
#else
		if (m_nMember[i] > 0 && (DWORD)m_nMember[i] == dwNpcID)
			return i;
#endif
	}
	return -1;
}

#ifdef _SERVER
BOOL	KTeam::CheckFull()
{
	if (m_nMemNum < CalcCaptainPower())
		return FALSE;
	return TRUE;
}
#endif

BOOL	KTeam::SetTeamOpen()
{
#ifdef _SERVER
	if (CheckFull())
		return FALSE;
	m_nState = Team_S_Open;

	PLAYER_TEAM_OPEN_CLOSE	sTeamState;
	sTeamState.ProtocolType = (BYTE)s2c_teamopenclose;
	sTeamState.m_btOpenClose = Team_S_Open;
	g_pServer->PackDataToClient(Player[m_nCaptain].m_nNetConnectIdx, (BYTE*)&sTeamState, sizeof(PLAYER_TEAM_OPEN_CLOSE));

	Player[m_nCaptain].m_cMenuState.SetState(m_nCaptain, PLAYER_MENU_STATE_TEAMOPEN);

	NPC_SET_MENU_STATE_SYNC	sSync;
	sSync.ProtocolType = s2c_npcsetmenustate;
	sSync.m_dwID = Npc[Player[m_nCaptain].m_nIndex].m_dwID;
	sSync.m_btState = PLAYER_MENU_STATE_TEAMOPEN;
	sSync.m_wLength = sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(sSync.m_szSentence);
	Npc[Player[m_nCaptain].m_nIndex].SendDataToNearRegion((LPVOID)&sSync, sSync.m_wLength + 1);

	return TRUE;
#else
	m_nState = Team_S_Open;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return TRUE;
#endif
}

BOOL	KTeam::SetTeamClose()
{
#ifdef _SERVER
	m_nState = Team_S_Close;

	PLAYER_TEAM_OPEN_CLOSE	sTeamState;
	sTeamState.ProtocolType = (BYTE)s2c_teamopenclose;
	sTeamState.m_btOpenClose = Team_S_Close;
	g_pServer->PackDataToClient(Player[m_nCaptain].m_nNetConnectIdx, (BYTE*)&sTeamState, sizeof(PLAYER_TEAM_OPEN_CLOSE));

	Player[m_nCaptain].m_cMenuState.SetState(m_nCaptain, PLAYER_MENU_STATE_NORMAL);

	NPC_SET_MENU_STATE_SYNC	sSync;
	sSync.ProtocolType = s2c_npcsetmenustate;
	sSync.m_dwID = Npc[Player[m_nCaptain].m_nIndex].m_dwID;
	sSync.m_btState = PLAYER_MENU_STATE_NORMAL;
	sSync.m_wLength = sizeof(NPC_SET_MENU_STATE_SYNC) - 1 - sizeof(sSync.m_szSentence);
	Npc[Player[m_nCaptain].m_nIndex].SendDataToNearRegion((LPVOID)&sSync, sSync.m_wLength + 1);

	return TRUE;
#else
	m_nState = Team_S_Close;
	Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
	return TRUE;
#endif
}

#ifdef _SERVER
BOOL	KTeam::IsOpen()
{
	return m_nState;
}
#endif

int		KTeam::CalcCaptainPower()//®iÓm tµi l·nh ®¹o
{
#ifdef _SERVER
	return PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[m_nCaptain].m_dwLeadLevel);
#else
	return PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel);
#endif
}

#ifdef _SERVER
BOOL	KTeam::CheckAddCondition(int nPlayerIndex)
{
	if (Npc[Player[nPlayerIndex].m_nIndex].m_Camp != camp_begin && Npc[Player[m_nCaptain].m_nIndex].m_Camp == camp_begin)
		return FALSE;
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KTeam::CreateTeam(int nPlayerIndex)
{
	Release();
	m_nCaptain = nPlayerIndex;
	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KTeam::AddMember(int nPlayerIndex)
{
	// 
	if (m_nCaptain < 0)
		return FALSE;
	// 
	if ( !CheckAddCondition(nPlayerIndex) )
		return FALSE;
	// 
	if (CheckFull())
		return FALSE;

	int n;
	n = FindFree();
	if (n < 0)
		return FALSE;
	m_nMember[n] = nPlayerIndex;
	m_nMemNum++;

	return TRUE;
}
#endif

#ifdef _SERVER
BOOL	KTeam::DeleteMember(int nPlayerIndex)
{
	int		i;
	PLAYER_LEAVE_TEAM	sLeaveTeam;

	if (nPlayerIndex == m_nCaptain)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TEAM_DISMISS;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);

		Player[nPlayerIndex].m_cTeam.Release();
		Npc[Player[nPlayerIndex].m_nIndex].RestoreCurrentCamp();
		sLeaveTeam.ProtocolType = s2c_teamleave;
		sLeaveTeam.m_dwNpcID = Npc[Player[nPlayerIndex].m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));

		for (i = 0; i < MAX_TEAM_MEMBER; i++)
		{
			if (m_nMember[i] < 0)
				continue;

			g_pServer->PackDataToClient(Player[m_nMember[i]].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);

			Npc[Player[m_nMember[i]].m_nIndex].RestoreCurrentCamp();
			Player[m_nMember[i]].m_cTeam.Release();
			sLeaveTeam.m_dwNpcID = Npc[Player[m_nMember[i]].m_nIndex].m_dwID;
			g_pServer->PackDataToClient(Player[m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
		}
		Release();
		return TRUE;
	}
	else
	{
		Player[nPlayerIndex].m_cTeam.Release();
		sLeaveTeam.ProtocolType = s2c_teamleave;
		sLeaveTeam.m_dwNpcID = Npc[Player[nPlayerIndex].m_nIndex].m_dwID;
		g_pServer->PackDataToClient(Player[m_nCaptain].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
		for (i = 0; i < MAX_TEAM_MEMBER; i++)
		{
			if (m_nMember[i] == nPlayerIndex)
			{
				Npc[Player[m_nMember[i]].m_nIndex].RestoreCurrentCamp();
				m_nMember[i] = -1;
				m_nMemNum--;
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
			}
			else
			{
				if (m_nMember[i] >= 0)
					g_pServer->PackDataToClient(Player[m_nMember[i]].m_nNetConnectIdx, (BYTE*)&sLeaveTeam, sizeof(PLAYER_LEAVE_TEAM));
			}
		}
	}
	if(!IsOpen())	//add by Fong KiÒu fix Full kh«ng PT n÷a
	{
		SetTeamOpen();
	}
	return TRUE;
}
#endif

#ifndef _SERVER
void	KTeam::CreateTeam(int nCaptainNpcID, char *lpszCaptainName, int nCaptainLevel, DWORD nTeamServerID)
{
	Release();
	m_nCaptain = nCaptainNpcID;
	m_nMemLevel[0] = nCaptainLevel;
	strcpy(m_szMemName[0], lpszCaptainName);
	m_nTeamServerID = nTeamServerID;		// 
}
#endif

#ifndef _SERVER
BOOL	KTeam::AddMember(DWORD dwNpcID, int nLevel, char *lpszNpcName)
{
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (m_nMember[i] == (int)dwNpcID)
			return TRUE;
	}

	int nFreeNo;
	nFreeNo = FindFree();
	if (nFreeNo < 0)
		return FALSE;
	m_nMember[nFreeNo] = dwNpcID;
	m_nMemLevel[nFreeNo + 1] = nLevel;
	strcpy(m_szMemName[nFreeNo + 1], lpszNpcName);
	m_nMemNum++;

	return TRUE;
}
#endif

#ifndef _SERVER
void	KTeam::DeleteMember(DWORD dwNpcID)
{
	if (dwNpcID == (DWORD)m_nCaptain)
	{
		Release();
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if ((DWORD)m_nMember[i] == dwNpcID)
		{
			m_nMemNum--;
			m_nMember[i] = -1;
			m_nMemLevel[i + 1] = 0;
			m_szMemName[i + 1][0] = 0;
			return;
		}
	}
}
#endif

#ifndef _SERVER
int	KTeam::GetMemberInfo(KUiPlayerItem *pList, int nCount)
{
	if (this->m_nCaptain < 0)
		return 0;

	if (!pList || nCount <= m_nMemNum)
	{
		return m_nMemNum + 1;
	}

	int	i, nNum = 0;	
	strcpy(pList[nNum].Name, m_szMemName[0]);
	pList[nNum].uId = this->m_nCaptain;
	int nNpcIdx = NpcSet.SearchID(this->m_nCaptain);
	pList[nNum].nIndex = 0;
	if(nNpcIdx > 0)
	{
		pList[nNum].nFaction = Npc[nNpcIdx].nFirstFaction;
		pList[nNum].nPercenLife = Npc[nNpcIdx].m_CurrentLife*100 / Npc[nNpcIdx].m_CurrentLifeMax;
		pList[nNum].nPercenMana = Npc[nNpcIdx].m_CurrentMana*100 / Npc[nNpcIdx].m_CurrentManaMax;
		pList[nNum].nIndex = nNpcIdx;
	}	
	nNum++;

	for (i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (nNum >= nCount)
			break;
		if (this->m_nMember[i] <= 0)
			continue;
		strcpy(pList[nNum].Name, m_szMemName[i + 1]);
		pList[nNum].uId = m_nMember[i];
		pList[nNum].nIndex = 0;
		int nNpcIdx2 = NpcSet.SearchID(m_nMember[i]);
		if(nNpcIdx2 > 0)
		{
			pList[nNum].nFaction = Npc[nNpcIdx2].nFirstFaction;
			pList[nNum].nPercenLife = Npc[nNpcIdx2].m_CurrentLife*100 / Npc[nNpcIdx2].m_CurrentLifeMax;
			pList[nNum].nPercenMana = Npc[nNpcIdx2].m_CurrentMana*100 / Npc[nNpcIdx2].m_CurrentManaMax;
			pList[nNum].nIndex = nNpcIdx2;
		}		
		nNum++;
	}

	return nNum;
}
#endif

#ifndef _SERVER
BOOL	KTeam::CheckIn(int nPlayerIndex)
{
	if (nPlayerIndex <= 0)
		return FALSE;
	if (nPlayerIndex == m_nCaptain)
		return TRUE;
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (m_nMember[i] == nPlayerIndex)
			return TRUE;
	}

	return FALSE;
}
#endif

#ifdef _SERVER
BOOL	KTeam::CheckIn(int nPlayerIndex)
{
	if (nPlayerIndex <= 0)
		return FALSE;
	if (nPlayerIndex == m_nCaptain)
		return TRUE;
	for (int i = 0; i < MAX_TEAM_MEMBER; i++)
	{
		if (m_nMember[i] == nPlayerIndex)
			return TRUE;
	}

	return FALSE;
}
#endif

#ifdef _SERVER
void	KTeamSet::Init()
{
	for (int i = 0; i < MAX_TEAM; i++)
		g_Team[i].SetIndex(i);
}
#endif

#ifdef _SERVER
int		KTeamSet::FindFree()
{
	for (int i = 0; i < MAX_TEAM; i++)
	{
		if (g_Team[i].m_nCaptain < 0)
			return i;
	}
	return -1;
}
#endif

/*
#ifdef _SERVER
BOOL	KTeamSet::CheckName(char *lpszName)
{
	if (!lpszName || !lpszName[0])
		return FALSE;

	for (int i = 0; i < MAX_TEAM; i++)
	{
		if (g_Team[i].m_nCaptain < 0)
			continue;
		if (strcmp(lpszName, g_Team[i].m_szName) == 0)
			return FALSE;
	}

	return TRUE;
}
#endif
*/

#ifdef _SERVER
int		KTeamSet::CreateTeam(int nPlayerID)
{
	int nTeamID;
	nTeamID = FindFree();
	if (nTeamID < 0)
		return -2;
	if (g_Team[nTeamID].CreateTeam(nPlayerID));
		return nTeamID;
	return -1;
}
#endif






















