// ChatServer.cpp: implementation of the CChatServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChatServer.h"
#include "ChatConnect.h"
#include "S3Relay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatServer::CChatServer()
{

}

CChatServer::~CChatServer()
{

}


CNetConnect* CChatServer::CreateConnect(CNetServer* pNetServer, unsigned long id)
{
	return new CChatConnect((CChatServer*)pNetServer, id);
}

void CChatServer::DestroyConnect(CNetConnect* pConn)
{
	delete pConn;
}


void CChatServer::OnBuildup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	rTRACE("chat server startup");
}

void CChatServer::OnClearup()
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.clear();

	rTRACE("chat server shutdown");
}

void CChatServer::OnClientConnectCreate(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect[pConn->GetIP()] = (CChatConnect*)pConn;
}

void CChatServer::OnClientConnectClose(CNetConnect* pConn)
{
	AUTOLOCKWRITE(m_lockIpMap);

	m_mapIp2Connect.erase(pConn->GetIP());
}

CNetConnectDup CChatServer::FindChatConnectByIP(DWORD IP)
{
	AUTOLOCKREAD(m_lockIpMap);

	IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(IP);
	if (it == m_mapIp2Connect.end())
		return CNetConnectDup();

	CChatConnect* pChatConn = (*it).second;
	if (!pChatConn)
		return CNetConnectDup();

	return CNetConnectDup(*pChatConn);
}

BOOL CChatServer::TraceInfo()
{
	AUTOLOCKREAD(m_lockIpMap);

	std::_tstring info("message: [ChatServer] ");
	char buffer[_MAX_PATH];

	sprintf(buffer, "<total: %d> : ", m_mapIp2Connect.size());
	info.append(buffer);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		if (it != m_mapIp2Connect.begin())
			info.append(", ");

		sprintf(buffer, "%08X", (*it).first);
		info.append(buffer);
	}

	rTRACE(info.c_str());

	return TRUE;
}

BOOL CChatServer::FindPlayerBySerNoAndGsNumber(unsigned int _nServerIndex, unsigned int _nGsNumber, CNetConnectDup* pConnDup)
{
	AUTOLOCKREAD(m_lockIpMap);
	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); ++it)
	{
		CChatConnect* pConnMe = (*it).second;
		if (pConnMe != NULL)
		{
			if(pConnMe->getSerRegIndex() == _nServerIndex && pConnMe->getGsNumber() == _nGsNumber)
			{
				if(pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CChatServer::FindPlayerByAcc(CChatConnect* pConn, const std::_tstring& acc, CNetConnectDup* pConnDup, std::_tstring* pRole, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealAcc)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CChatConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByAcc(acc, pRole, pNameID, pParam, pRealAcc))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CChatServer::FindPlayerByRole(CChatConnect* pConn, const std::_tstring& role, CNetConnectDup* pConnDup, std::_tstring* pAcc, DWORD* pNameID, unsigned long* pParam, std::_tstring* pRealRole)
{
	AUTOLOCKREAD(m_lockIpMap);

	for (IP2CONNECTMAP::iterator it = m_mapIp2Connect.begin(); it != m_mapIp2Connect.end(); it++)
	{
		CChatConnect* pConnMe = (*it).second;

		if (pConnMe != pConn)
		{
			if (pConnMe->FindPlayerByRole(role, pAcc, pNameID, pParam, pRealRole))
			{
				if (pConnDup != NULL)
					*pConnDup = *pConnMe;

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CChatServer::FindPlayerByIpParam(CChatConnect* pConn, DWORD ip, unsigned long param, CNetConnectDup* pConnDup, std::_tstring* pAcc, std::_tstring* pRole, DWORD* pNameID, bool isReturn)
{
	AUTOLOCKREAD(m_lockIpMap);
	IP2CONNECTMAP::iterator it = m_mapIp2Connect.find(ip);
	if (it == m_mapIp2Connect.end())
		return FALSE;

	if(isReturn)
		return TRUE;

	CChatConnect* pConnMe = (*it).second;
	if(pConnMe != pConn)
	{
		if(pConnMe->FindPlayerByParam(param, pAcc, pRole, pNameID))
		{
			if(pConnDup != NULL)
				*pConnDup = *pConnMe;

			return TRUE;
		}
	}

	return FALSE;
}
