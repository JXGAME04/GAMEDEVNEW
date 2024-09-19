//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerReBorn.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerReBorn Class
//---------------------------------------------------------------------------


#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerReBorn.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif

#ifdef _SERVER

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
KPlayerReBorn::KPlayerReBorn()
{
	m_nPlayerIndex = 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerReBorn::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;

	m_nReBornValue				= 0;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void	KPlayerReBorn::SetReBornValue(int nValue)
{
	this->m_nReBornValue = (nValue < 0 ? 0 : nValue);
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE;

	REBORN_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_rebornsyncrebornvalue;
	sValue.m_nReBornValue = m_nReBornValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(REBORN_VALUE_SYNC));
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
int		KPlayerReBorn::GetReBornValue()
{
	return m_nReBornValue;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerReBorn::AddReBornValue(int nAdd)
{
	m_nReBornValue += nAdd;
	if (m_nReBornValue < 0)
		m_nReBornValue = 0;
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE;

	REBORN_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_rebornsyncrebornvalue;
	sValue.m_nReBornValue = m_nReBornValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(REBORN_VALUE_SYNC));
}



#endif

#ifndef _SERVER

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

void	KPlayerReBorn::SetReBornValue(int nValue)
{
	if (m_nReBornValue == nValue)
		return;

	this->m_nReBornValue = (nValue < 0 ? 0 : nValue);
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE;

}

void	KPlayerReBorn::AddReBornValue(int nAdd)
{
	m_nReBornValue += nAdd;
	if (m_nReBornValue < 0)
		m_nReBornValue = 0;
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE;
}


#endif













