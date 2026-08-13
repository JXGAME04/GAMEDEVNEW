//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerReBorn.cpp
// Date:	2012.08.10
// Code:	SkyGold
// Desc:	PlayerReBorn Class 转生
//---------------------------------------------------------------------------


#include	"KCore.h"
#include	"KPlayer.h"
#include	"KPlayerDef.h"
#include	"KPlayerReBorn.h"

#ifndef _SERVER
#include "CoreShell.h"
#endif
#include <Text.h>

#ifdef _SERVER

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
KPlayerReBorn::KPlayerReBorn()
{
	m_nPlayerIndex = 0;
	m_nReBornValue = 0;
	m_nReBornLevel = 0;		     //当前转生的等级
	m_nReBornKeepQpiont = 0;       //保留的潜能点
	m_nReBornKeepJpiont = 0;       //保留的技能
	m_nReBornSetSkillMaxLevel = 0; //技能等级上限增加
	m_nReBornSetFanYuMaxVal = 0;   //防御上限增加
	m_nCurPlyaerLvel = 0;	     //记录玩家转生时的等级	
	m_nCurReBornNum = 0;
	m_nReBornLifeMaxVal = 0;
}

//-------------------------------------------------------------------------
//	
//-------------------------------------------------------------------------
void	KPlayerReBorn::Init(int nPlayerIdx)
{
	this->m_nPlayerIndex = nPlayerIdx;

	m_nReBornValue = 0;
	m_nReBornLevel = 0;		     //当前转生的等级
	m_nReBornKeepQpiont = 0;       //保留的潜能点
	m_nReBornKeepJpiont = 0;       //保留的技能
	m_nReBornSetSkillMaxLevel = 0; //技能等级上限增加
	m_nReBornSetFanYuMaxVal = 0;   //防御上限增加
	m_nCurPlyaerLvel = 0;	         //记录玩家转生时的等级
	m_nCurReBornNum = 0;
	m_nReBornLifeMaxVal = 0;
}


//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void	KPlayerReBorn::SetReBornValue(int nValue)
{
	this->m_nReBornValue = (nValue < 0 ? 0 : nValue);
	/*

	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE;

	  BYTE    m_nReBornLevel;		       //当前转生的等级
	  BYTE    m_nReBornKeepQpiont;       //保留的潜能点
	  BYTE	m_nReBornKeepJpiont;       //保留的技能
	  BYTE    m_nReBornSetSkillMaxLevel; //技能等级上限增加
	  BYTE    m_nReBornSetFanYuMaxVal;   //防御上限增加

	  */
	if (this->m_nReBornValue > 0)
	{
		this->m_nCurReBornNum = EGetByte(this->m_nReBornValue, 1); //第一个 偏移记录 行号 次数	   
		this->m_nCurPlyaerLvel = EGetByte(this->m_nReBornValue, 2); //第二个偏移记录NPC转生时的等级

		int nCurRow = GetReBornLevelRow(this->m_nCurPlyaerLvel);

		if (nCurRow > 0)
		{
			g_ReBornSetting.GetInteger(nCurRow, 2, 0, &m_nReBornLevel);
			g_ReBornSetting.GetInteger(nCurRow, 3, 0, &m_nReBornKeepQpiont);
			g_ReBornSetting.GetInteger(nCurRow, 4, 0, &m_nReBornKeepJpiont);
			g_ReBornSetting.GetInteger(nCurRow, 5, 0, &m_nReBornSetSkillMaxLevel);
			g_ReBornSetting.GetInteger(nCurRow, 6, 0, &m_nReBornSetFanYuMaxVal);
			g_ReBornSetting.GetInteger(nCurRow, 7, 0, &m_nReBornLifeMaxVal);
		}
		else
		{
			this->m_nReBornLevel = 0;
			this->m_nReBornKeepQpiont = 0;
			this->m_nReBornKeepJpiont = 0;
			this->m_nReBornSetSkillMaxLevel = 0;
			this->m_nReBornSetFanYuMaxVal = 0;
			this->m_nReBornLifeMaxVal = 0;
		}
	}

	if (this->m_nReBornValue <= 0)
	{
		this->m_nCurReBornNum = 0;
		this->m_nCurPlyaerLvel = 0;
		this->m_nReBornLevel = 0;
		this->m_nReBornKeepQpiont = 0;
		this->m_nReBornKeepJpiont = 0;
		this->m_nReBornSetSkillMaxLevel = 0;
		this->m_nReBornSetFanYuMaxVal = 0;
		this->m_nReBornLifeMaxVal = 0;
	}

	REBORN_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_rebornsyncrebornvalue;
	sValue.m_nReBornValue = m_nReBornValue;
	sValue.m_nCurPlyaerLvel = m_nCurPlyaerLvel;
	sValue.m_nReBornLevel = m_nReBornLevel;		      //当前转生的等级
	sValue.m_nReBornKeepQpiont = m_nReBornKeepQpiont;       //保留的潜能点
	sValue.m_nReBornKeepJpiont = m_nReBornKeepJpiont;       //保留的技能
	sValue.m_nReBornSetSkillMaxLevel = m_nReBornSetSkillMaxLevel; //技能等级上限增加
	sValue.m_nReBornSetFanYuMaxVal = m_nReBornSetFanYuMaxVal;   //防御上限增加
	sValue.m_nReBornLifeMaxVal = m_nReBornLifeMaxVal;
	sValue.m_nCurReBornNum = m_nCurReBornNum;

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
void	KPlayerReBorn::AddReBornValue(int nAdd, int nLevel)
{
	//m_nReBornValue += nAdd;

	m_nReBornValue = ESetByte(m_nReBornValue, 1, m_nCurReBornNum + nAdd);
	m_nReBornValue = ESetByte(m_nReBornValue, 2, nLevel);

	if (m_nReBornValue < 0)
		m_nReBornValue = 0;

	/*if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE; */

	if (this->m_nReBornValue > 0)
	{
		this->m_nCurReBornNum = EGetByte(this->m_nReBornValue, 1); //第一个 偏移记录 行号  次数
		this->m_nCurPlyaerLvel = EGetByte(this->m_nReBornValue, 2); //第二个偏移记录NPC转生时的等级

		int nCurRow = GetReBornLevelRow(this->m_nCurPlyaerLvel);

		if (nCurRow > 0)
		{
			g_ReBornSetting.GetInteger(nCurRow, 2, 0, &m_nReBornLevel);
			g_ReBornSetting.GetInteger(nCurRow, 3, 0, &m_nReBornKeepQpiont);
			g_ReBornSetting.GetInteger(nCurRow, 4, 0, &m_nReBornKeepJpiont);
			g_ReBornSetting.GetInteger(nCurRow, 5, 0, &m_nReBornSetSkillMaxLevel);
			g_ReBornSetting.GetInteger(nCurRow, 6, 0, &m_nReBornSetFanYuMaxVal);
			g_ReBornSetting.GetInteger(nCurRow, 7, 0, &m_nReBornLifeMaxVal);

		}
		else
		{
			this->m_nReBornLevel = 0;
			this->m_nReBornKeepQpiont = 0;
			this->m_nReBornKeepJpiont = 0;
			this->m_nReBornSetSkillMaxLevel = 0;
			this->m_nReBornSetFanYuMaxVal = 0;
			this->m_nReBornLifeMaxVal = 0;
		}
	}

	if (this->m_nReBornValue <= 0)
	{
		this->m_nCurReBornNum = 0;
		this->m_nCurPlyaerLvel = 0;
		this->m_nReBornLevel = 0;
		this->m_nReBornKeepQpiont = 0;
		this->m_nReBornKeepJpiont = 0;
		this->m_nReBornSetSkillMaxLevel = 0;
		this->m_nReBornSetFanYuMaxVal = 0;
		this->m_nReBornLifeMaxVal = 0;
	}


	REBORN_VALUE_SYNC	sValue;
	sValue.ProtocolType = s2c_rebornsyncrebornvalue;
	sValue.m_nReBornValue = m_nReBornValue;
	sValue.m_nCurReBornNum = m_nCurReBornNum;
	sValue.m_nCurPlyaerLvel = m_nCurPlyaerLvel;
	sValue.m_nReBornLevel = m_nReBornLevel;		      //当前转生的等级
	sValue.m_nReBornKeepQpiont = m_nReBornKeepQpiont;       //保留的潜能点
	sValue.m_nReBornKeepJpiont = m_nReBornKeepJpiont;       //保留的技能
	sValue.m_nReBornSetSkillMaxLevel = m_nReBornSetSkillMaxLevel; //技能等级上限增加
	sValue.m_nReBornSetFanYuMaxVal = m_nReBornSetFanYuMaxVal;   //防御上限增加
	sValue.m_nReBornLifeMaxVal = m_nReBornLifeMaxVal;

	g_pServer->PackDataToClient(Player[m_nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(REBORN_VALUE_SYNC));
}


int  KPlayerReBorn::GetReBornLevelRow(int nInLevel)
{
	int nRows = g_ReBornSetting.GetHeight() + 1;
	int nReg = 0;

	for (int i = 2; i < nRows; ++i)
	{
		int nCurLve = 0;

		g_ReBornSetting.GetInteger(i, 2, 0, &nCurLve);

		if (nCurLve == nInLevel)
		{
			nReg = i;
			break;
		}
	}

	return	 nReg;
}

/*
int  KPlayerReBorn::SetReBornByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;

	nByteValue = (nByteValue & 0xff);

	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;

	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayerReBorn::GetReBornByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;

	if (nByteNumber > 4 || nByteNumber <= 0)
		return nByteValue;

	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
 */

#endif

#ifndef _SERVER

 //-------------------------------------------------------------------------
 //
 //-------------------------------------------------------------------------




void	KPlayerReBorn::SetReBornValue(BYTE* pMsg)
{

	REBORN_VALUE_SYNC* pValue = (REBORN_VALUE_SYNC*)pMsg;

	if (m_nReBornValue == pValue->m_nReBornValue)
	{
		return;
	}

	this->m_nReBornValue = (pValue->m_nReBornValue < 0 ? 0 : pValue->m_nReBornValue);
	/*
	  if (m_nReBornValue > MAX_REBORN_VALUE)
		  m_nReBornValue = MAX_REBORN_VALUE;
	  */
	this->m_nReBornLevel = pValue->m_nReBornLevel;
	this->m_nReBornKeepQpiont = pValue->m_nReBornKeepQpiont;
	this->m_nReBornKeepJpiont = pValue->m_nReBornKeepJpiont;
	this->m_nReBornSetSkillMaxLevel = pValue->m_nReBornSetSkillMaxLevel;
	this->m_nReBornSetFanYuMaxVal = pValue->m_nReBornSetFanYuMaxVal;
	this->m_nCurReBornNum = pValue->m_nCurReBornNum;
	this->m_nCurPlyaerLvel = pValue->m_nCurPlyaerLvel;
	this->m_nReBornLifeMaxVal = pValue->m_nReBornLifeMaxVal;
	if (this->m_nReBornValue <= 0)
	{
		this->m_nCurPlyaerLvel = 0;
		this->m_nCurReBornNum = 0;
		this->m_nReBornLevel = 0;
		this->m_nReBornKeepQpiont = 0;
		this->m_nReBornKeepJpiont = 0;
		this->m_nReBornSetSkillMaxLevel = 0;
		this->m_nReBornSetFanYuMaxVal = 0;
		this->m_nReBornLifeMaxVal = 0;
	}

	/*
	 BYTE    m_nReBornLevel;		       //当前转生的等级
	 BYTE    m_nReBornKeepQpiont;       //保留的潜能点
	 BYTE	m_nReBornKeepJpiont;       //保留的技能
	 BYTE    m_nReBornSetSkillMaxLevel; //技能等级上限增加
	 BYTE    m_nReBornSetFanYuMaxVal;   //防御上限增加

	 */
}

/*
int  KPlayerReBorn::SetReBornByte(int nIntValue,int nByteNumber,int nByteValue)
{
	BYTE * pByte =	NULL;

	nByteValue = (nByteValue & 0xff);

	if (nByteNumber > 4 || nByteNumber <= 0) ///4
		return nIntValue;

	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
	//Lua_PushNumber(L, nIntValue);
	return nIntValue;
}

int  KPlayerReBorn::GetReBornByte(int nIntValue,int nByteNumber)
{
	int nByteValue = 0;

	if (nByteNumber > 4 || nByteNumber <= 0)
		return nByteValue;

	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	//Lua_PushNumber(L, nByteValue);
	return nByteValue;
}
*/

void	KPlayerReBorn::AddReBornValue(int nAdd)
{
	m_nReBornValue += nAdd;
	if (m_nReBornValue < 0)
		m_nReBornValue = 0;
	/*
	if (m_nReBornValue > MAX_REBORN_VALUE)
		m_nReBornValue = MAX_REBORN_VALUE; */
}

#endif
