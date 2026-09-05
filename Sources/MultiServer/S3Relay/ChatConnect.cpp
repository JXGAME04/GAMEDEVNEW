// ChatConnect.cpp: implementation of the CChatConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "ChatConnect.h"
#include "ChatServer.h"
#include "S3Relay.h"
#include "OfflineMsgDB.h"	// [HAOHUU 04/09]

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatConnect::CChatConnect(CChatServer* pChatServer, unsigned long id)
	: CNetConnect(pChatServer, id)
{
	nServerIndex = 1;
}

CChatConnect::~CChatConnect()
{
	nServerIndex = 1;
}


void CChatConnect::OnClientConnectCreate()
{
	rTRACE("chat connect create: %s", _ip2a(GetIP()));
}

void CChatConnect::OnClientConnectClose()
{
	rTRACE("chat connect close: %s", _ip2a(GetIP()));
}

void CChatConnect::RecvPackage(const void* pData, size_t size)
{
	BYTE protocol = *(BYTE*)pData;

	if (protocol == chat_someonechat)
	{
		Proc0_SomeoneChat(pData, size);
	}
	else if (protocol == chat_channelchat)
	{
		Proc0_ChannelChat(pData, size);
	}
	else if (protocol == chat_groupman)
	{
		Proc0_GroupMan(pData, size);
	}
	else
	{
		EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
		if (pHeader->ProtocolFamily == pf_normal)
		{
			Proc0_Normal(pData, size);
		}
		else if (pHeader->ProtocolFamily == pf_udataserveridx)
		{
			Proc0_ServerCommunity(pData, size);
		}
		//else if (pHeader->ProtocolFamily == pf_playercommunity)
		//{
		//	Proc0_PlayerCommunity(pData, size);
		//}
	}
}

void CChatConnect::Proc0_Normal(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;

	if (pHeader->ProtocolID == c2s_entergame)
	{
		Proc1_Normal_EnterGame(pData, size);
	}
	else if (pHeader->ProtocolID == c2s_leavegame)
	{
		Proc1_Normal_LeaveGame(pData, size);
	}
}

void CChatConnect::Proc1_Normal_EnterGame(const void* pData, size_t size)
{
	tagEnterGame2* pEnterGame2 = (tagEnterGame2*)pData;

	std::_tstring strAcc = (char*)pEnterGame2->szAccountName;
	assert(!strAcc.empty());
	std::_tstring strRole = (char*)pEnterGame2->szCharacterName;
	assert(!strRole.empty());

	std::_tstring strHWID= (char*)pEnterGame2->szHWID;
	assert(!strHWID.empty());

	ACCINFO infoAcc;
	infoAcc.role = strRole;
	infoAcc.nameID = pEnterGame2->dwNameID;
	infoAcc.param = pEnterGame2->lnID;

	ROLEINFO infoRole;
	infoRole.acc = strAcc;
	infoRole.nameID = pEnterGame2->dwNameID;
	infoRole.param = pEnterGame2->lnID;

	PARAMINFO infoParam;
	infoParam.acc = strAcc;
	infoParam.role = strRole;
	infoParam.hwid = strHWID;
	infoParam.nameID = pEnterGame2->dwNameID;

	{{
	DUMMY_AUTOLOCKWRITE(m_lockPlayer);

	m_mapAcc[strAcc] = infoAcc;
	m_mapRole[strRole] = infoRole;
	m_mapParam[pEnterGame2->lnID] = infoParam;
	}}

	{{
	g_FriendMgr.SomeoneLogin(strRole);
	}}

	{{
	// [HAOHUU 04/09] giao loi nhan mat nhan duoc trong luc ngoai tuyen (port Linux KOfflineMsgStore)
	g_ChannelMgr.DeliverOfflineMsgs(strRole.c_str(), GetIP(), pEnterGame2->lnID, pEnterGame2->dwNameID);
	}}

	rTRACE("Chat:Player Login Acc:%s Role:%s (IP:%08X, lnID:%08X, nSelServer:%d)", strAcc.c_str(), strRole.c_str(), GetIP(), pEnterGame2->lnID, pEnterGame2->nSelServer);
}

void CChatConnect::Proc1_Normal_LeaveGame(const void* pData, size_t size)
{
	tagLeaveGame2* pLeaveGame = (tagLeaveGame2*)pData;

	std::_tstring strAcc = pLeaveGame->szAccountName;
	assert(!strAcc.empty());

	{{
	DUMMY_AUTOLOCKWRITE(m_lockPlayer);

	ACCMAP::iterator itAcc = m_mapAcc.find(strAcc);
	if (itAcc != m_mapAcc.end())
	{
		ACCINFO& rAccInfo = (*itAcc).second;

		{{
		//ensure
		g_ChannelMgr.B_ClearPlayer(GetIP(), rAccInfo.param);
		//g_FriendMgr.SomeoneLogout(rAccInfo.role);
		}}
		

		m_mapRole.erase(rAccInfo.role);
		m_mapParam.erase(rAccInfo.param);

		m_mapAcc.erase(itAcc);
	}
	}}

	rTRACE("chat player logout nSelServer:%d - Player:%s", pLeaveGame->nSelServer, strAcc.c_str());
}

void CChatConnect::Proc0_SomeoneChat(const void* pData, size_t size)
{
	CHAT_SOMEONECHAT_CMD* pCsc = (CHAT_SOMEONECHAT_CMD*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	g_ChannelMgr.SomeoneChat(GetIP(), *pSrcInfo, pCsc);
}

void CChatConnect::Proc0_ChannelChat(const void* pData, size_t size)
{
	CHAT_CHANNELCHAT_CMD* pCcc = (CHAT_CHANNELCHAT_CMD*)pData;
	tagPlusSrcInfo* pSrcInfo = (tagPlusSrcInfo*)((BYTE*)pData + size) - 1;

	g_ChannelMgr.ChannelChat(GetIP(), *pSrcInfo, pCcc);
}

void CChatConnect::Proc0_GroupMan(const void* pData, size_t size)
{
	CHAT_GROUPMAN* pCgc = (CHAT_GROUPMAN*)pData;
	CHAT_CHANNELCHAT_SYNC* pCccSync = (CHAT_CHANNELCHAT_SYNC*)(pCgc + 1);
	if (pCccSync->ProtocolType != chat_channelchat)
		return;	//unexpect

	void* pSent = pCccSync + 1;
	void* pPlayersData = (BYTE*)pSent + pCccSync->sentlen;


	DWORD srcIP = GetIP();

	const size_t playercount = pCgc->wPlayerCount;
	pCgc->wPlayerCount = 0;

	if (pCgc->byHasIdentify)
	{
		tagPlusSrcInfo* pPlayers = (tagPlusSrcInfo*)(pPlayersData);
		for (size_t i = 0; i < playercount; i++)
		{
			if (g_ChannelMgr.IsSubscribed(srcIP, pPlayers[i].lnID, pCccSync->channelid))
			{
				if (pCgc->wPlayerCount < i)
					pPlayers[pCgc->wPlayerCount] = pPlayers[i];
				++ pCgc->wPlayerCount;
			}
		}
	}
	else
	{
		WORD* pPlayers = (WORD*)(pPlayersData);
		for (size_t i = 0; i < playercount; i++)
		{
			if (g_ChannelMgr.IsSubscribed(srcIP, (unsigned long)pPlayers[i], pCccSync->channelid))
			{
				if (pCgc->wPlayerCount < i)
					pPlayers[pCgc->wPlayerCount] = pPlayers[i];
				++ pCgc->wPlayerCount;
			}
		}
	}

	if (pCgc->wPlayerCount <= 0)
		return;


	size_t size2 = size - (playercount - pCgc->wPlayerCount) * (pCgc->byHasIdentify ? sizeof(tagPlusSrcInfo) : sizeof(WORD));
	pCgc->wSize = size2 - 1;

	SendPackage(pCgc, size2);
}

void CChatConnect::Proc0_ServerCommunity(const void* pData, size_t size)
{
	EXTEND_HEADER* pHeader = (EXTEND_HEADER*)pData;
	if (pHeader->ProtocolID == relay_s2c_updateserver)
	{
		tagLeaveGame2* pServerData = (tagLeaveGame2*)pData;
		nServerIndex = (short)HIWORD(pServerData->nSelServer);
		nGsNumber = LOWORD(pServerData->nSelServer);
		//m_sHostServer[nServerIndex] = GetID();
		setSerRegIndex(nServerIndex);
		setGsNumber(nGsNumber);
		rTRACE("---Chat start Okay nServerIndex:%d, nServerIndex:%08X ---", nServerIndex, nGsNumber);
	}
}

BOOL CChatConnect::FindPlayerByParam(unsigned long param, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	PARAMMAP::iterator it = m_mapParam.find(param);

	if (it == m_mapParam.end())
		return FALSE;

	const PARAMINFO& infoParam = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoParam.acc;
	if (pRole != NULL)
		*pRole = infoParam.role;
	if (pNameID != NULL)
		*pNameID = infoParam.nameID;

	return TRUE;
}

BOOL CChatConnect::FindPlayerByAcc(const std::_tstring& acc, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ACCMAP::iterator it = m_mapAcc.find(acc);
	if (it == m_mapAcc.end())
		return FALSE;

	const ACCINFO& infoAcc = (*it).second;

	if (pRole != NULL)
		*pRole = infoAcc.role;
	if (pNameID != NULL)
		*pNameID = infoAcc.nameID;
	if (pParam != NULL)
		*pParam = infoAcc.param;

	if (pRealAcc != NULL)
		*pRealAcc = (*it).first;

	return TRUE;
}

BOOL CChatConnect::FindPlayerByRole(const std::_tstring& role, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	DUMMY_AUTOLOCKREAD(m_lockPlayer);

	ROLEMAP::iterator it = m_mapRole.find(role);
	if (it == m_mapRole.end())
		return FALSE;

	const ROLEINFO& infoRole = (*it).second;

	if (pAcc != NULL)
		*pAcc = infoRole.acc;
	if (pNameID != NULL)
		*pNameID = infoRole.nameID;
	if (pParam != NULL)
		*pParam = infoRole.param;

	if (pRealRole != NULL)
		*pRealRole = (*it).first;

	return TRUE;
}
