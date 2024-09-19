//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerFuYuan.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerFuYuan Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerFuYuan.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif

#ifdef _SERVER

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
KPlayerFuYuan::KPlayerFuYuan()
{
	m_nPlayerIndex = 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerFuYuan::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;

	m_nFuYuanValue				= 0;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void	KPlayerFuYuan::SetFuYuanValue(int nValue)
{
	this->m_nFuYuanValue = (nValue < 0 ? 0 : nValue);
	if (m_nFuYuanValue > MAX_FUYUAN_VALUE)
		m_nFuYuanValue = MAX_FUYUAN_VALUE;

	FUYUAN_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_fuyuansyncfuyuanvalue;
	sValue.m_nFuYuanValue = m_nFuYuanValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(FUYUAN_VALUE_SYNC));
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
int		KPlayerFuYuan::GetFuYuanValue()
{
	return m_nFuYuanValue;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerFuYuan::AddFuYuanValue(int nAdd)
{
	m_nFuYuanValue += nAdd;
	if (m_nFuYuanValue < 0)
		m_nFuYuanValue = 0;
	if (m_nFuYuanValue > MAX_FUYUAN_VALUE)
		m_nFuYuanValue = MAX_FUYUAN_VALUE;

	FUYUAN_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_fuyuansyncfuyuanvalue;
	sValue.m_nFuYuanValue = m_nFuYuanValue;
	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(FUYUAN_VALUE_SYNC));
}



#endif

#ifndef _SERVER

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

void	KPlayerFuYuan::SetFuYuanValue(int nValue)
{
	if (m_nFuYuanValue == nValue)
		return;

	this->m_nFuYuanValue = (nValue < 0 ? 0 : nValue);
	if (m_nFuYuanValue > MAX_FUYUAN_VALUE)
		m_nFuYuanValue = MAX_FUYUAN_VALUE;

}

void	KPlayerFuYuan::AddFuYuanValue(int nAdd)
{
	m_nFuYuanValue += nAdd;
	if (m_nFuYuanValue < 0)
		m_nFuYuanValue = 0;
	if (m_nFuYuanValue > MAX_FUYUAN_VALUE)
		m_nFuYuanValue = MAX_FUYUAN_VALUE;
}


#endif













