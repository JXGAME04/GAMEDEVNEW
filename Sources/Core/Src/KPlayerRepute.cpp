//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerRepute.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerRepute Class
//---------------------------------------------------------------------------


#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerRepute.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif

#ifdef _SERVER

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
KPlayerRepute::KPlayerRepute()
{
	m_nPlayerIndex = 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerRepute::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;

	m_nReputeValue				= 0;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void	KPlayerRepute::SetReputeValue(int nValue)
{
	this->m_nReputeValue = (nValue < 0 ? 0 : nValue);
	if (m_nReputeValue > MAX_REPUTE_VALUE)
		m_nReputeValue = MAX_REPUTE_VALUE;

	REPUTE_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_reputesyncreputevalue;
	sValue.m_nReputeValue = m_nReputeValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(REPUTE_VALUE_SYNC));
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
int		KPlayerRepute::GetReputeValue()
{
	return m_nReputeValue;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerRepute::AddReputeValue(int nAdd)
{
	m_nReputeValue += nAdd;
	if (m_nReputeValue < 0)
		m_nReputeValue = 0;
	if (m_nReputeValue > MAX_REPUTE_VALUE)
		m_nReputeValue = MAX_REPUTE_VALUE;

	REPUTE_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_reputesyncreputevalue;
	sValue.m_nReputeValue = m_nReputeValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(REPUTE_VALUE_SYNC));
}



#endif

#ifndef _SERVER

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

void	KPlayerRepute::SetReputeValue(int nValue)
{
	if (m_nReputeValue == nValue)
		return;

	this->m_nReputeValue = (nValue < 0 ? 0 : nValue);
	if (m_nReputeValue > MAX_REPUTE_VALUE)
		m_nReputeValue = MAX_REPUTE_VALUE;

}

void	KPlayerRepute::AddReputeValue(int nAdd)
{
	m_nReputeValue += nAdd;
	if (m_nReputeValue < 0)
		m_nReputeValue = 0;
	if (m_nReputeValue > MAX_REPUTE_VALUE)
		m_nReputeValue = MAX_REPUTE_VALUE;
}


#endif













