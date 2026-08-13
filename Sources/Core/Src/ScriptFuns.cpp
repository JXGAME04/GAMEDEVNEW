/*******************************************************************************
// FileName			:	ScriptFuns.cpp
// FileAuthor		:	Fong Ki襲
// FileCreateDate	:	2002-11-19 15:58:20
*******************************************************************************/
#ifndef WIN32
#include <string>
#endif

#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "KStepLuaScript.h"
#include "LuaLib.h"
#include "KScriptList.h"
#include <string.h>
#include "LuaFuns.h"
#include "KCore.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KItemSet.h"
//#include "KNetClient.h"
#include "KScriptValueSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KBuySell.h"
#include "KTaskFuns.h"
#include "KPlayerDef.h"
#include "TaskDef.h"
#ifdef _SERVER
//#include "KNetServer.h"
//#include "../MultiServer/Heaven/interface/iServer.h"
#include "KNewProtocolProcess.h"
#endif
#include "KSortScript.h"
#ifndef __linux
#include "Shlwapi.h"
#include "windows.h"
#include "winbase.h"
#include <direct.h>
#else
#include "unistd.h"
#endif

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "../../Engine/Src/KSG_StringProcess.h"
#include "../../Engine/Src/Text.h"
#endif

#include <ctime>
#include <KTongProtocol.h>
#include <MapHandler.h>
#include <GiftCodeManager.h>
#ifndef WIN32
typedef struct  _SYSTEMTIME
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}	SYSTEMTIME;
typedef struct  _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
}	FILETIME;
#endif

inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }

KScriptList		g_StoryScriptList;
KStepLuaScript* LuaGetScript(Lua_State* L);
int	GetPlayerIndex(Lua_State* L);
extern int g_GetPriceToStation(int, int);
extern int g_GetPriceToWayPoint(int, int);
extern int g_GetPriceToDock(int, int);

const DWORD h_186 = 669600;			//edit by phong kieu thoi gian cho mo khoa 186 gio

int LuaGetBit(Lua_State* L)
{
	int nBitValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);

	if (nBitNumber >= 32 || nBitNumber <= 0)
		goto lab_getbit;
	nBitValue = (nIntValue & (1 << (nBitNumber - 1))) != 0;
lab_getbit:
	Lua_PushNumber(L, nBitValue);
	return 1;
}

int LuaSetBit(Lua_State* L)
{
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	int nBitValue = (int)Lua_ValueToNumber(L, 3);
	nBitValue = (nBitValue == 1);

	if (nBitNumber > 32 || nBitNumber <= 0)
		goto lab_setbit;

	nIntValue = (nIntValue | (1 << (nBitNumber - 1)));
lab_setbit:
	Lua_PushNumber(L, nIntValue);
	return 1;
}

int LuaGetByte(Lua_State* L)
{
	int nByteValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);

	if (nByteNumber > 4 || nByteNumber <= 0)
		goto lab_getByte;
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8))) >> ((nByteNumber - 1) * 8);

lab_getByte:
	Lua_PushNumber(L, nByteValue);
	return 1;
}

int LuaSetByte(Lua_State* L)
{
	BYTE* pByte = NULL;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	int nByteValue = (int)Lua_ValueToNumber(L, 3);
	nByteValue = (nByteValue & 0xff);

	if (nByteNumber > 4 || nByteNumber <= 0)
		goto lab_setByte;

	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber - 1)) = (BYTE)nByteValue;
	//nIntValue = (nIntValue | (0xff << ((nByteNumber - 1) * 8) )) ;
lab_setByte:
	Lua_PushNumber(L, nIntValue);
	return 1;
}

#ifdef _SERVER
int LuaSetPlayerReputeValue(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nReputeValue = (int)Lua_ValueToNumber(L, 1);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cRepute.SetReputeValue(nReputeValue);
	return 0;
}

int LuaGetPlayerReputeValue(Lua_State* L)
{
	int nReputeValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerreputevalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerreputevalue;
	nReputeValue = Player[nPlayerIndex].m_cRepute.GetReputeValue();

lab_getplayerreputevalue:
	Lua_PushNumber(L, nReputeValue);
	return 1;
}

int LuaAddPlayerFuYuanValue(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nFuYuanValue = (int)Lua_ValueToNumber(L, 1);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cFuYuan.SetFuYuanValue(Player[nPlayerIndex].m_cFuYuan.GetFuYuanValue() + nFuYuanValue);
	return 0;
}

int LuaSetPlayerFuYuanValue(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nFuYuanValue = (int)Lua_ValueToNumber(L, 1);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cFuYuan.SetFuYuanValue(nFuYuanValue);
	return 0;
}

int LuaGetExtPoint(Lua_State* L)
{
	int nResult = 0;
	int nPlayerIndex = 0;
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getextpoint;

	nResult = Player[nPlayerIndex].GetExtPoint();

lab_getextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaSetExtPoint(Lua_State* L)
{
	int nResult = 0;
	int nExtPoint = 0;
	int nChange = 1;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_setextpoint;

	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_setextpoint;
	nExtPoint = Lua_ValueToNumber(L, 1);
	if (nExtPoint < 0)
		goto lab_setextpoint;
	Player[nPlayerIndex].SetExtPoint(nExtPoint, nChange);

lab_setextpoint:
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaAddExtPoint(Lua_State* L)
{
	int nResult = 0;
	int nExtPoint = 0;
	int nChange = 1;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_setextpoint;

	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_setextpoint;
	nExtPoint = Lua_ValueToNumber(L, 1);
	if (nExtPoint < 0)
		goto lab_setextpoint;
	Player[nPlayerIndex].AddExtPoint(nExtPoint, nChange);

lab_setextpoint:
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetGameTime(Lua_State* L)
{
	Lua_PushNumber(L, g_SubWorldSet.GetGameTime());
	return 1;
}

int LuaGetPlayerFuYuanValue(Lua_State* L)
{
	int nFuYuanValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerfuyuanvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerfuyuanvalue;
	nFuYuanValue = Player[nPlayerIndex].m_cFuYuan.GetFuYuanValue();

lab_getplayerfuyuanvalue:
	Lua_PushNumber(L, nFuYuanValue);
	return 1;
}


int LuaSetPlayerMeridianValue(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	int nMeridianType = (int)Lua_ValueToNumber(L, 1);
	int nMeridianLevel = (int)Lua_ValueToNumber(L, 2);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	if (nMeridianLevel < 0 || nMeridianLevel > MAX_MERIDIAN_LEVEL)
		return 0;

	int levelDiff = Player[nPlayerIndex].m_cMeridian.setMeridian(nMeridianType, nMeridianLevel);
	if (levelDiff == 0) //Same level, do nothing
		return 0;
	if (levelDiff > 0) {
		//[xxxC____] => current level 4
		//[xxxDDDC_] => DDD levelDiff = 3, new current level 7
		MeridianManager.ApplyMaridianToNPC(&Npc[Player[nPlayerIndex].m_nIndex], nMeridianType, nMeridianLevel, levelDiff); //Add all new higher levels
	}
	else {
		MeridianManager.RemoveMaridianFromNPC(&Npc[Player[nPlayerIndex].m_nIndex], nMeridianType, nMeridianLevel, levelDiff); //Remove diff current meridian effects levels
	}
	Player[nPlayerIndex].UpdataCurData();
	//Sync
	MERIDIAN_SYNC	sValue;
	sValue.ProtocolType = s2c_syncmeridian;
	memcpy(sValue.m_nMeridian, Player[nPlayerIndex].m_cMeridian.getMeridian(), sizeof(sValue.m_nMeridian));
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sValue, sizeof(MERIDIAN_SYNC));
	return 0;
}

//?????????
int LuaGetReBornJPoint(Lua_State* L)
{

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	Lua_PushNumber(L, Player[nPlayerIndex].m_cReBorn.GetReBornKeepJpiont());
	return 1;

}
//??????????
int LuaGetReBornQPoint(Lua_State* L)
{
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	Lua_PushNumber(L, Player[nPlayerIndex].m_cReBorn.GetReBornKeepQpiont());
	return 1;

}

#endif

#ifdef _SERVER
int LuaModifyRepute(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	int nValue = (int)Lua_ValueToNumber(L, 1);

	Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_REPUTE, Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE) + nValue);
	if (nValue < 0)
	{
		char szMsg[100];
		sprintf(szMsg, "LuaModifyRepute %d!", -nValue);
	}
	else
	{
		char szMsg[100];
		sprintf(szMsg, "LuaModifyRepute %d!", nValue);
	}
	return 0;
}

int LuaModifyAttrib(Lua_State* L)//add by phong ki襲 v藅 ph萴 m竨 l韓 t鑞g kim
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
		return 0;
	KNpc* pNPC = &Npc[Player[nPlayerIndex].m_nIndex];
	KMagicAttrib pBaseAttrib;
	pBaseAttrib.nAttribType = (int)Lua_ValueToNumber(L, 1);
	pBaseAttrib.nValue[0] = (int)Lua_ValueToNumber(L, 2);
	pBaseAttrib.nValue[1] = (int)Lua_ValueToNumber(L, 3);
	pBaseAttrib.nValue[2] = (int)Lua_ValueToNumber(L, 4);
	KMagicAttrib* pAttrib = &pBaseAttrib;
	if (-1 != pAttrib->nAttribType)
	{
		pNPC->ModifyAttrib(pNPC->m_Index, (void*)pAttrib);
	}
	return 0;
}

int LuaST_IsRideHorse(Lua_State* L)//add by phong ki襲 check player tr猲 ng鵤
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	if (Npc[Player[nPlayerIndex].m_nIndex].m_bRideHorse)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaCountEquipPlayer(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nCount = 0;
	int nItem = 0;
	if (nPlayerIndex > 0)
	{
		for (int i = 0; i < itempart_num; ++i)
		{
			nItem = Player[nPlayerIndex].m_ItemList.GetEquipment(i);
			if (nItem > 0)
				nCount++;
			nItem = Player[nPlayerIndex].m_ItemList.GetAltEquipment(i);
			if (nItem > 0)
				nCount++;
		}
	}
	Lua_PushNumber(L, nCount);
	return 1;
}
//Player[nIndex].m_ItemList.RemoveAll();

int LuaRemoveAllItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	
	Player[nPlayerIndex].m_ItemList.RemoveAll();
	ITEM_REMOVE_SYNC	sRemove;
	sRemove.ProtocolType = s2c_removeallitem;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
	//Update player data
	Player[nPlayerIndex].UpdataCurData();
	return 0;
}
int LuaGetRepute(Lua_State *L)
{
	int nValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		goto lab_getrepute;
	nValue = Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
lab_getrepute:
	Lua_PushNumber(L, nValue);
	return 1;	
}
#endif

int GetSubWorldIndex(Lua_State* L)
{
	Lua_GetGlobal(L, SCRIPT_SUBWORLDINDEX);
	if (lua_isnil(L, Lua_GetTopIndex(L)))
		return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nIndex >= MAX_SUBWORLD || nIndex < 0)
	{
		_ASSERT(0);
		return -1;
	}
	if (SubWorld[nIndex].m_nIndex >= MAX_SUBWORLD || SubWorld[nIndex].m_nIndex < 0)
	{
		_ASSERT(0);
		return -1;
	}
	return nIndex;
}

int LuaSubWorldIDToIndex(Lua_State* L)//Idx = SubWorldID2Idx(dwID)
{
	int nTargetSubWorld = -1;
	int nSubWorldID = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;

	nSubWorldID = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorld(nSubWorldID);

lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

int LuaSubWorldIndexToID(Lua_State* L)
{
	int nTargetSubWorld = -1;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;

	nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = SubWorld[nSubWorldIndex].m_SubWorldID;

lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}
/*
Say(sMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln)
Say(nMainInfo, nSelCount, sSel1, sSel2, sSel3, .....,sSeln)
Say(nMainInfo, nSelCount, SelTab)
  Say(100, 3, 10, 23,43)
  Say("选择什么？", 2, "是/yes", "否/no");
  Say("选什么呀", 2, SelTab);
*/
int LuaSelectUI(Lua_State* L)
{
	char* strMain = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char* pContent = NULL;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	if (Lua_IsNumber(L, 2))
	{
		nOptionNum = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		_ASSERT(0);
		return 0;
	}

	if (Lua_IsNumber(L, 1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L, 1);
		nDataType = 1;
	}
	else if (Lua_IsString(L, 1))
	{
		strMain = (char*)Lua_ValueToString(L, 1);
		nDataType = 0;
	}
	else
		return 0;

	BOOL bStringTab = FALSE;

	if (Lua_IsString(L, 3))
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))
	{
		bStringTab = TRUE;
	}
	else
	{
		if (nOptionNum > 0)  return 0;
	}

	if (bStringTab == FALSE)
	{
		if (nOptionNum > nParamNum - 2) nOptionNum = nParamNum - 2;
	}

	if (nOptionNum > MAX_ANSWERNUM) nOptionNum = MAX_ANSWERNUM;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_SELECTDIALOG;
	UiInfo.m_bParam1 = nDataType;
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	}
	else if (nDataType == 1)
	{
		*(int*)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}

	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;

	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;

	for (int i = 0; i < nOptionNum; i++)
	{
		char pAnswer[256];
		pAnswer[0] = 0;

		if (bStringTab)
		{
			Lua_PushNumber(L, i + 1);
			Lua_RawGet(L, 3);
			char* pszString = (char*)Lua_ValueToString(L, Lua_GetTopIndex(L));
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 256);
			}
		}
		else
		{
			char* pszString = (char*)Lua_ValueToString(L, i + 3);
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 256);
		}

		char* pFunName = strstr(pAnswer, "/");

		if (pFunName)
		{
			g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
			*pFunName = 0;
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
		else
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
	}

	if (nDataType == 0)
		UiInfo.m_nBufferLen = strlen(pContent);
	else
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaSaySPR(Lua_State* L)//Say new
{
	char* strMain = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char* pContent = NULL;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	if (Lua_IsNumber(L, 2))
	{
		nOptionNum = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		_ASSERT(0);
		return 0;
	}

	if (Lua_IsNumber(L, 1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L, 1);
		nDataType = 1;
	}
	else if (Lua_IsString(L, 1))
	{
		strMain = (char*)Lua_ValueToString(L, 1);
		nDataType = 0;
	}
	else
		return 0;

	BOOL bStringTab = FALSE;

	if (Lua_IsString(L, 3))
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))
	{
		bStringTab = TRUE;
	}
	else
	{
		if (nOptionNum > 0)  return 0;
	}

	if (bStringTab == FALSE)
	{
		if (nOptionNum > nParamNum - 2) nOptionNum = nParamNum - 2;
	}

	if (nOptionNum > MAX_ANSWERNUM) nOptionNum = MAX_ANSWERNUM;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_SELECTDIALOG;
	UiInfo.m_bParam1 = nDataType;
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 1; //SayNew

	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	}
	else if (nDataType == 1)
	{
		*(int*)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}

	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;

	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;

	for (int i = 0; i < nOptionNum; i++)
	{
		char  pAnswer[256];
		pAnswer[0] = 0;

		if (bStringTab)
		{
			Lua_PushNumber(L, i + 1);
			Lua_RawGet(L, 3);
			char* pszString = (char*)Lua_ValueToString(L, Lua_GetTopIndex(L));
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 256);
			}
		}
		else
		{
			char* pszString = (char*)Lua_ValueToString(L, i + 3);
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 256);
		}

		char* pFunName = strstr(pAnswer, "/");

		if (pFunName)
		{
			g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
			*pFunName = 0;
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
		else
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
	}

	if (nDataType == 0)
		UiInfo.m_nBufferLen = strlen(pContent);
	else
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaSelectImage(Lua_State* L)
{
	char* strMain = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char* pContent = NULL;
	int nIdImage = 0;
	BOOL bStringTab = FALSE;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 2) return 0;

	if (nParamNum >= 2)
	{
		if (Lua_IsNumber(L, 2))
		{
			nOptionNum = (int)Lua_ValueToNumber(L, 2);
		}
		else
		{
			_ASSERT(0);
			return 0;
		}

		if (Lua_IsNumber(L, 1))
		{
			nMainInfo = (int)Lua_ValueToNumber(L, 1);
			nDataType = 1;
		}
		else if (Lua_IsString(L, 1))
		{
			strMain = (char*)Lua_ValueToString(L, 1);
			nDataType = 0;
		}
		else
			return 0;

		if (nParamNum >= 3)
		{
			nIdImage = (int)Lua_ValueToNumber(L, 3);
			if (Lua_IsString(L, 4))
				bStringTab = FALSE;
			else if (Lua_IsTable(L, 4))
			{
				bStringTab = TRUE;
			}
			else
			{
				if (nOptionNum > 0)
					return 0;
			}
		}
		else if (nParamNum >= 2)
		{
			nIdImage = 0;
			if (Lua_IsString(L, 3))
				bStringTab = FALSE;
			else if (Lua_IsTable(L, 3))
			{
				bStringTab = TRUE;
			}
			else
			{
				if (nOptionNum > 0)
					return 0;
			}
		}
	}

	Player[nPlayerIndex].SetImageNpcId(nIdImage);

	if (bStringTab == FALSE)
	{
		if (nOptionNum > nParamNum - 2)
			nOptionNum = nParamNum - 2;
	}

	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_SELECTDIALOG;
	UiInfo.m_bParam1 = nDataType;
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 2;

	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		pContent = UiInfo.m_pContent;
	}
	else if (nDataType == 1)
	{
		*(int*)UiInfo.m_pContent = nMainInfo;
		pContent = UiInfo.m_pContent + sizeof(int);
		*pContent = 0;
	}

	if (nOptionNum > MAX_ANSWERNUM)
		nOptionNum = MAX_ANSWERNUM;

	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;

	if (nParamNum >= 2)
	{
		for (int i = 0; i < nOptionNum; i++)
		{
			char  pAnswer[1024];
			pAnswer[0] = 0;

			if (bStringTab)
			{

				if (nParamNum >= 3)
				{
					Lua_PushNumber(L, i + 1);
					Lua_RawGet(L, 4);
				}
				else if (nParamNum >= 2)
				{
					Lua_PushNumber(L, i + 1);
					Lua_RawGet(L, 3);
				}
				char* pszString = (char*)Lua_ValueToString(L, Lua_GetTopIndex(L));
				if (pszString)
				{
					g_StrCpyLen(pAnswer, pszString, 100);
				}
			}
			else
			{
				int a;
				if (nParamNum >= 3)
				{
					a = i + 4;
					char* pszString = (char*)Lua_ValueToString(L, a);
					if (pszString)
						g_StrCpyLen(pAnswer, pszString, 100);
				}
				else if (nParamNum >= 2)
				{
					a = i + 3;
					char* pszString = (char*)Lua_ValueToString(L, a);
					if (pszString)
						g_StrCpyLen(pAnswer, pszString, 100);
				}
			}

			char* pFunName = strstr(pAnswer, "/");

			if (pFunName)
			{
				g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
				*pFunName = 0;
				sprintf(pContent, "%s|%s", pContent, pAnswer);
			}
			else
			{
				strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "Main");
				sprintf(pContent, "%s|%s", pContent, pAnswer);
			}
		}
	}
	if (nDataType == 0)
		UiInfo.m_nBufferLen = strlen(pContent);
	else
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaSendMessageInfo(Lua_State* L)//PutMsg(szMsg/MsgId)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_MSGINFO;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else
	{

		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaAddGlobalNews(Lua_State* L)//AddGlobalNews(Newsstr)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddLocalNews(Lua_State* L)//AddLocalNews(Newsstr)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalCountNews(Lua_State* L)//AddGlobalCountNews(strNew/newid, time)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	int nTime = (int)Lua_ValueToNumber(L, 2);

	if (nTime <= 0)
		nTime = 1;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int*)((char*)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		*(int*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddLocalCountNews(Lua_State* L)//AddLocalCountNews(strNew/newid, time)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	int nTime = (int)Lua_ValueToNumber(L, 2);

	if (nTime <= 0)
		nTime = 1;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int*)((char*)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		*(int*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalTimeNews(Lua_State* L)//AddGlobalTimeNews(strNew/newid, year,month,day,hour,mins)
{
	if (Lua_GetTopIndex(L) < 6)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}

	SYSTEMTIME* pSystemTime = (SYSTEMTIME*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));

	SYSTEMTIME LocalTime;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));

	LocalTime.wYear = (WORD)Lua_ValueToNumber(L, 2);
	LocalTime.wMonth = (WORD)Lua_ValueToNumber(L, 3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L, 5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L, 6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddLocalTimeNews(Lua_State* L)//AddLocalTimeNews(strNew/newid, year,month,day,hour,mins)
{
	if (Lua_GetTopIndex(L) < 6)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}

	SYSTEMTIME* pSystemTime = (SYSTEMTIME*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));

	SYSTEMTIME LocalTime;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));

	LocalTime.wYear = (WORD)Lua_ValueToNumber(L, 2);
	LocalTime.wMonth = (WORD)Lua_ValueToNumber(L, 3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L, 5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L, 6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalNewsEx(Lua_State* L)//AddGlobalNewsEx(Newsstr)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO_1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL_1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddLocalNewsEx(Lua_State* L)//AddLocalNewsEx(Newsstr)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO_1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL_1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalCountNewsEx(Lua_State* L)//AddGlobalCountNewsEx(strNew/newid, time)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO_1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING_1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	int nMsgId = 0;

	int nTime = (int)Lua_ValueToNumber(L, 2);

	if (nTime <= 0)
		nTime = 1;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int*)((char*)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		*(int*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddLocalCountNewsEx(Lua_State* L)//AddLocalCountNewsEx(strNew/newid, time)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO_1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_COUNTING_1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	int nMsgId = 0;

	int nTime = (int)Lua_ValueToNumber(L, 2);

	if (nTime <= 0)
		nTime = 1;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		*(int*)((char*)UiInfo.m_pContent + sizeof(int)) = nTime;
		UiInfo.m_nBufferLen = sizeof(int) * 2;
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
		*(int*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen) = nTime;
		UiInfo.m_nBufferLen += sizeof(int);
		UiInfo.m_bParam1 = 0;
	}

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddGlobalTimeNewsEx(Lua_State* L)//AddGlobalTimeNewsEx(strNew/newid, year,month,day,hour,mins)
{
	if (Lua_GetTopIndex(L) < 6)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO_1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND_1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}

	SYSTEMTIME* pSystemTime = (SYSTEMTIME*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));

	SYSTEMTIME LocalTime;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));

	LocalTime.wYear = (WORD)Lua_ValueToNumber(L, 2);
	LocalTime.wMonth = (WORD)Lua_ValueToNumber(L, 3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L, 5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L, 6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastGlobal(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddLocalTimeNewsEx(Lua_State* L)//AddLocalTimeNewsEx(strNew/newid, year,month,day,hour,mins)
{
	if (Lua_GetTopIndex(L) < 6)
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO_1;
	UiInfo.m_bOptionNum = NEWSMESSAGE_TIMEEND_1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_Select = 0;

	int nMsgId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nMsgId = (int)Lua_ValueToNumber(L, 1);
		*((int*)(UiInfo.m_pContent)) = nMsgId;
		UiInfo.m_bParam1 = 1;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(SYSTEMTIME);
	}
	else
	{
		g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), 256);
		UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent)) + sizeof(SYSTEMTIME);
		UiInfo.m_bParam1 = 0;
	}

	SYSTEMTIME* pSystemTime = (SYSTEMTIME*)((char*)UiInfo.m_pContent + UiInfo.m_nBufferLen - sizeof(SYSTEMTIME));
	memset(pSystemTime, 0, sizeof(SYSTEMTIME));

	SYSTEMTIME LocalTime;
	memset(&LocalTime, 0, sizeof(SYSTEMTIME));

	LocalTime.wYear = (WORD)Lua_ValueToNumber(L, 2);
	LocalTime.wMonth = (WORD)Lua_ValueToNumber(L, 3);
	LocalTime.wDay = (WORD)Lua_ValueToNumber(L, 4);
	LocalTime.wHour = (WORD)Lua_ValueToNumber(L, 5);
	LocalTime.wMinute = (WORD)Lua_ValueToNumber(L, 6);
	FILETIME ft;
	FILETIME sysft;
#ifdef WIN32
	SystemTimeToFileTime(&LocalTime, &ft);
	LocalFileTimeToFileTime(&ft, &sysft);
	FileTimeToSystemTime(&sysft, pSystemTime);
#else
	memcpy(pSystemTime, &LocalTime, sizeof(LocalTime));
#endif

#ifndef _SERVER
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaAddNote(Lua_State* L)//AddNote(str/strid)
{
	char* strMain = NULL;
	int nMainInfo = 0;
	int nDataType = 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nParam2 = 0;
	if (Lua_IsNumber(L, 1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L, 1);
		nDataType = 1;
	}
	else if (Lua_IsString(L, 1))
	{
		strMain = (char*)Lua_ValueToString(L, 1);
		nDataType = 0;
	}
	else
		return 0;

	if (nParamNum > 1)
	{
		nParam2 = (int)Lua_ValueToNumber(L, 2);
	}

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NOTEINFO;
	UiInfo.m_bParam1 = nDataType;
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	if (nDataType == 0)
	{
		if (strMain)
			sprintf(UiInfo.m_pContent, "%s", strMain);
		int nLen = strlen(strMain);
		*(int*)(UiInfo.m_pContent + nLen) = nParam2;
		UiInfo.m_nBufferLen = nLen + sizeof(int);
	}
	else if (nDataType == 1)
	{
		*(int*)UiInfo.m_pContent = nMainInfo;
		*(int*)(UiInfo.m_pContent + sizeof(int)) = nParam2;
		UiInfo.m_nBufferLen = sizeof(int) + sizeof(int);
	}

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaTalkUI(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char* pContent = NULL;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;

	if (Lua_IsNumber(L, 1))
	{
		nOptionNum = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		_ASSERT(0);
		return 0;
	}

	const char* pCallBackFun = Lua_ValueToString(L, 2);

	if (Lua_IsNumber(L, 3))
	{
		nDataType = 1;
	}
	else if (Lua_IsString(L, 3))
	{
		nDataType = 0;
	}
	else
		return 0;

	if (nOptionNum > nParamNum - 2)
		nOptionNum = nParamNum - 2;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_TALKDIALOG;
	UiInfo.m_bParam1 = nDataType;
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	pContent = UiInfo.m_pContent;
	pContent[0] = 0;
	size_t nContentLen = 0;
	for (int i = 0; i < nOptionNum; i++)
	{
		const char* pString = NULL;
		if (!nDataType)//StringInfo
		{
			pString = Lua_ValueToString(L, i + 3);
			if (nContentLen + strlen(pString) >= MAX_SCIRPTACTION_BUFFERNUM)
			{
				nOptionNum = i;
				UiInfo.m_bOptionNum = nOptionNum;
				break;
			}
			nContentLen += strlen(pString);
			sprintf(pContent, "%s%s|", pContent, pString);
		}
		else
		{
			int j = (int)Lua_ValueToNumber(L, i + 3);
			sprintf(pContent, "%s%d|", pContent, j);
		}
	}
	UiInfo.m_nBufferLen = strlen(pContent);

	if (!pCallBackFun || strlen(pCallBackFun) <= 0)
	{
		UiInfo.m_nParam = 0;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 0;
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		UiInfo.m_nParam = 1;
		Player[nPlayerIndex].m_nAvailableAnswerNum = 1;
		g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[0], pCallBackFun, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaTalkUI2(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);

	const char* szInitString;
	BYTE  nImage;
	if (nParamNum > 3)
		return 0;

	szInitString = Lua_ValueToString(L, 1);

	if (nParamNum == 3)
		nImage = (BYTE)Lua_ValueToNumber(L, 2);
	else
		nImage = 0;
#ifdef _SERVER
	S2C_TALK_EX NetCommand;
	NetCommand.ProtocolType = s2c_talkex;
	strcpy(NetCommand.Value, szInitString);
	NetCommand.Value1 = nImage;
	if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_TALK_EX));
#endif
	return 0;
}

int LuaIncludeFile(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	if (Lua_IsString(L, 1))
	{
		const char* pFileName = lua_tostring(L, 1);
		char lszCurrentDirectory[MAX_PATH];
		int nLen = 0;
		if (pFileName[0] != '\\' && pFileName[0] != '/')
		{
			getcwd(lszCurrentDirectory, MAX_PATH);
			nLen = strlen(lszCurrentDirectory);
			if (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/')
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
#else
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName);
			for (int i = 0; lszCurrentDirectory[i]; i++)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}
		else
		{
			g_GetRootPath(lszCurrentDirectory);
			nLen = strlen(lszCurrentDirectory);
			if (lszCurrentDirectory[nLen - 1] == '\\' || lszCurrentDirectory[nLen - 1] == '/')
				lszCurrentDirectory[nLen - 1] = 0;
#ifdef WIN32
			g_StrCat(lszCurrentDirectory, "\\");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
#else
			g_StrCat(lszCurrentDirectory, "/");
			g_StrCat(lszCurrentDirectory, (char*)pFileName + 1);
			for (int i = 0; lszCurrentDirectory[i]; i++)
			{
				if (lszCurrentDirectory[i] == '\\')
					lszCurrentDirectory[i] = '/';
			}
#endif
		}
		strlwr(lszCurrentDirectory + nLen);
		lua_dofile(L, lszCurrentDirectory);
		return 0;
	}
	else
		return 0;
}

int LuaGetTaskValue(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Player[nPlayerIndex].m_cTask.GetSaveVal((int)Lua_ValueToNumber(L, 1));
		Lua_PushNumber(L, nValue);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetTaskValue(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nValueIndex = (int)Lua_ValueToNumber(L, 1);
	int nValue = (int)Lua_ValueToNumber(L, 2);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	Player[nPlayerIndex].m_cTask.SetSaveVal(nValueIndex, nValue);
	Lua_PushNumber(L, 1);
	return 1;
}

#ifndef _SERVER
#define MAX_TEMPVALUENUM_INCLIENT 500
int g_TempValue[MAX_TEMPVALUENUM_INCLIENT];
#endif

int LuaGetTempTaskValue(Lua_State* L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));

#ifdef _SERVER
	if (nTempIndex >= MAX_TEMP_TASK)
	{
		Lua_PushNil(L);
		return 1;
	}
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNil(L);
		return 1;
	}

	int nValue = Player[nPlayerIndex].m_cTask.GetClearVal(nTempIndex);
	Lua_PushNumber(L, nValue);
#else

	if (nTempIndex >= 0 && nTempIndex < MAX_TEMPVALUENUM_INCLIENT)
		Lua_PushNumber(L, g_TempValue[nTempIndex]);
	else
		Lua_PushNil(L);
#endif
	return 1;
}

int LuaSetTempTaskValue(Lua_State* L)
{
	int nTempIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L) - 1);
	int nValue = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
#ifdef _SERVER	
	Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
	int nPlayerIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].m_cTask.SetClearVal(nTempIndex, nValue);
#else
	g_TempValue[nTempIndex] = nValue;
#endif
	return 0;
}

#ifdef _SERVER

int LuaSale(Lua_State* L)//Sale(id)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nShopId = (int)Lua_ValueToNumber(L, 1);
		int nShopMoneyUnit = moneyunit_money;
		if (Lua_GetTopIndex(L) > 2)
			nShopMoneyUnit = (int)Lua_ValueToNumber(L, 2);
		BuySell.OpenSale(nPlayerIndex, nShopId - 1, nShopMoneyUnit);
	}
	return 0;
}

int LuaNewSale(Lua_State* L)
{
	int i = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4) return 0;

	int nShopNum = (int)Lua_ValueToNumber(L, 3);

	if (nShopNum > MAX_SUPERSHOP_SHOPTAB)
		nShopNum = MAX_SUPERSHOP_SHOPTAB;

	int nShopId[MAX_SUPERSHOP_SHOPTAB];
	for (i = 0; i < nShopNum; i++)
		nShopId[i] = (int)Lua_ValueToNumber(L, 4 + i) - 1;

	for (i; i < MAX_SUPERSHOP_SHOPTAB; i++)
		nShopId[i] = -1;

	BuySell.OpenSale(nPlayerIndex, (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2), nShopNum, nShopId);
	return 0;
}

int LuaTrade(Lua_State* L)
{
	return 0;
}

int LuaOpenBox(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	BYTE	NetCommand = (BYTE)s2c_openstorebox;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 0;
}

int LuaSetTimer(Lua_State* L)//SetTimer(Time, TimerTaskId)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].SetTimer((DWORD)(int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	return 0;
}

int LuaStopTimer(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].CloseTimer();
	return 0;
}

int LuaGetCurTimerId(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nTimerId = Player[nPlayerIndex].m_TimerTask.GetTaskId();
	Lua_PushNumber(L, nTimerId);
	return 1;
}

int LuaGetRestTime(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNil(L);
		return 1;
	}
	int nRestTime = Player[nPlayerIndex].m_TimerTask.GetRestTime();//m_dwTimeTaskTime - g_SubWorldSet.GetGameTime();

	if (nRestTime > 0)
		Lua_PushNumber(L, nRestTime);
	else
		Lua_PushNumber(L, 0);

	return 1;
}

int LuaGetMissionRestTime(Lua_State* L)
{
	int RestTime = 0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex >= 0)
		{
			int nMissionId = (int)Lua_ValueToNumber(L, 1);
			int nTimerId = (int)Lua_ValueToNumber(L, 2);

			if (nMissionId < 0 || nTimerId < 0)
				goto lab_getmissionresttime;

			KMission Mission;
			Mission.SetMissionId(nMissionId);
			KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
			if (pMission)
			{
				RestTime = (int)pMission->GetTimerRestTimer(nTimerId);
			}
		}
	}

lab_getmissionresttime:
	Lua_PushNumber(L, RestTime);
	return 1;
}

int LuaIsLeader(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag && Player[nPlayerIndex].m_cTeam.m_nFigure == TEAM_CAPTAIN)
			Lua_PushNumber(L, 1);
		else
			Lua_PushNumber(L, 0);

	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetTeamId(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
			Lua_PushNumber(L, Player[nPlayerIndex].m_cTeam.m_nID);
		else Lua_PushNil(L);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetTeamSize(Lua_State* L)
{
	int nTeamSize = 0;
	int nTeamId = -1;
	if (Lua_GetTopIndex(L) >= 1)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (Player[nPlayerIndex].m_cTeam.m_nFlag)
				nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			else
				nTeamId = -1;
		}
	}

	if (nTeamId < 0)
		nTeamSize = 0;
	else
		nTeamSize = g_Team[nTeamId].m_nMemNum + 1;
	Lua_PushNumber(L, nTeamSize);
	return 1;
}

int LuaLeaveTeam(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			PLAYER_APPLY_LEAVE_TEAM	sLeaveTeam;
			sLeaveTeam.ProtocolType = c2s_teamapplyleave;
			Player[nPlayerIndex].LeaveTeam((BYTE*)&sLeaveTeam);
		}
	}
	return 0;
}

int LuaSetCreateTeamOption(Lua_State* L)
{
	int nState = (int)Lua_ValueToNumber(L, 1);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (nState)
			Player[nPlayerIndex].m_cTeam.SetCanTeamFlag(nPlayerIndex, TRUE);
		else
			Player[nPlayerIndex].m_cTeam.SetCanTeamFlag(nPlayerIndex, FALSE);
	}
	return 0;
}

int LuaGetPlayerItemIsTimeLimit(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
		return 0;

	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
		return 0;

	int nResult = Item[mItemIdx].GetPlayerItemTimeLimit();
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerItemIsLock(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
		return 0;

	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
		return 0;

	int nResult = Item[mItemIdx].GetPlayerItemLock();
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerItemSecondLock(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
		return 0;

	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
		return 0;

	time_t baygio = time(0);
	int s_chomk = Item[mItemIdx].GetPlayerItemHLock();
	int s_conlai = s_chomk - baygio;
	int nResult = 0;
	if (s_chomk > 0)
		nResult = s_conlai;
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerItemHourLock(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
		return 0;

	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
		return 0;

	int pItemIdx = Player[m_PlayerIdx].GetItemIdxBymItemListIdx(mItemIdx);
	if (pItemIdx < 0)
		return 0;

	int nResult = Item[pItemIdx].GetPlayerItemHLock();
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaSetPlayerItemUnLockF(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
		return 0;
	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
		return 0;

	Item[mItemIdx].SetPlayerItemLock(0);
	Item[mItemIdx].SetPlayerItemHLock(0);
	PLAYER_ITEM_LOCK_SYNC	sMoney;
	sMoney.ProtocolType = s2c_playeritemlocksync;
	sMoney.m_ItemIdx = Item[mItemIdx].GetID();
	sMoney.m_InsuranceCourse = Item[mItemIdx].GetPlayerItemLock();
	sMoney.m_InsuranceHourCourse = Item[mItemIdx].GetPlayerItemHLock();
	sMoney.sPrice = 0;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_ITEM_LOCK_SYNC));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaSetPlayerItemUnLock(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
		return 0;
	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
		return 0;

	time_t baygio = time(0);
	Item[mItemIdx].SetPlayerItemLock(1);
	Item[mItemIdx].SetPlayerItemHLock(baygio + h_186);
	PLAYER_ITEM_LOCK_SYNC	sMoney;
	sMoney.ProtocolType = s2c_playeritemlocksync;
	sMoney.m_ItemIdx = Item[mItemIdx].GetID();
	sMoney.m_InsuranceCourse = Item[mItemIdx].GetPlayerItemLock();
	sMoney.m_InsuranceHourCourse = Item[mItemIdx].GetPlayerItemHLock();
	sMoney.sPrice = 0;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_ITEM_LOCK_SYNC));
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaOpenGiveBox(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);

	const char* szTitle;
	const char* szInitString;
	const char* szScript;
	const char* szAction1;

	if (nParamNum < 4)
		return 0;

	if (nParamNum >= 5)
	{
		szTitle = Lua_ValueToString(L, 1);
		szInitString = Lua_ValueToString(L, 2);
		szAction1 = Lua_ValueToString(L, 4);
		char* szScript = (char*)Lua_ValueToString(L, 3);
		Player[nPlayerIndex].m_dwGiveBoxId = g_FileName2Id(szScript);
	}
	else
	{
		szTitle = Lua_ValueToString(L, 1);
		szInitString = Lua_ValueToString(L, 2);
		szAction1 = Lua_ValueToString(L, 3);
		Player[nPlayerIndex].m_dwGiveBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	}

	S2C_GIVE_BOX NetCommand;
	NetCommand.ProtocolType = s2c_openaffairbox;
	NetCommand.nType = 1;
	strcpy(NetCommand.Value, szTitle);
	strcpy(NetCommand.Value1, szInitString);
	strcpy(NetCommand.Value2, szAction1);
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szAction1, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_GIVE_BOX));
	return 0;
}

KTabFile g_TabFileLib;
int LuaTabFile_Load(Lua_State* L)
{
	int result = 0;
	BOOL v4;
	char* szFileName;
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum >= 2)
	{
		if (Lua_IsString(L, 1))
		{
			szFileName = (char*)lua_tostring(L, 1);
			v4 = g_TabFileLib.Load(szFileName);
			lua_pushnumber(L, (long double)v4);
			result = 1;
		}
	}
	return result;
}

int LuaTabFile_GetCell(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	char szString[128];
	if (nParamNum >= 3 && g_TabFileLib.GetHeight())
	{
		if (Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			int nColumn = (int)Lua_ValueToNumber(L, 3);
			g_TabFileLib.GetString(nRow, nColumn, "", szString, sizeof(szString));
		}
		else if (Lua_IsNumber(L, 2) && Lua_IsString(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			char szColumn[32];
			strcpy(szColumn, Lua_ValueToString(L, 3));
			g_TabFileLib.GetString(nRow, szColumn, "", szString, sizeof(szString));
		}
		else if (Lua_IsString(L, 2) && Lua_IsString(L, 3))
		{
			char szRow[32];
			char szColumn[32];
			strcpy(szRow, Lua_ValueToString(L, 2));
			strcpy(szColumn, Lua_ValueToString(L, 3));
			g_TabFileLib.GetString(szRow, szColumn, "", szString, sizeof(szString));
		}
		else
			return 0;

		Lua_PushString(L, szString);
		return 1;
	}
	return 0;
}

int LuaTabFile_GetRowCount(Lua_State* L)
{
	if (g_TabFileLib.GetHeight())
	{
		int nCount = g_TabFileLib.GetHeight();
		Lua_PushNumber(L, nCount);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return  0;
}

int LuaTabFile_UnLoad(Lua_State* L)
{
	if (g_TabFileLib.GetHeight())
	{
		g_TabFileLib.Clear();
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaGetLocalDate(Lua_State* L)
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	const char* pszKey = (char*)Lua_ValueToString(L, 1);

	char pszTimeFormat[256];
	if (strftime(pszTimeFormat, sizeof(pszTimeFormat), pszKey, timeinfo))
	{
		Lua_PushString(L, pszTimeFormat);
		return 1;
	}
	return 0;
}

int LuaGetTimeByMiao(Lua_State* L)
{
	time_t rawtime;                  //定义一个long 型存放秒数
	time(&rawtime);                 //距离现在的时间（秒）
	char nTimeInfo[32] = { 0 };
	sprintf(nTimeInfo, "%u", rawtime);
	//printf("---测试时间:%s ----\n",nTimeInfo);
	Lua_PushString(L, nTimeInfo);
	return 1;
}

int LuaRepairItemGetNumCoin(Lua_State* L)//#do ben trang bi hong bang 0
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	else
	{
		int corlorItem = Item[mItemIdx].GetColorItem();
		int numCoin = 0;
		if (corlorItem == gold_item)
		{
			numCoin = 40;
		}
		else if (corlorItem == purple_item)
		{
			numCoin = 60;
		}
		else if (corlorItem == green_item)
		{
			numCoin = 30;
		}
		else
		{
			numCoin = 10;
		}
		Lua_PushNumber(L, numCoin);
		return 1;
	}
}

int LuaRepairItemByCoin(Lua_State* L)//#do ben trang bi hong bang 0
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	if (mItemIdx < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	else
	{
		int curDur = Item[mItemIdx].GetDurability();
		if (curDur == -1)
		{
			Lua_PushNumber(L, 0);
			return 0;
		}
		Item[mItemIdx].SetDurability(Item[mItemIdx].GetMaxDurability());
		ITEM_DURABILITY_CHANGE	IDC;
		IDC.ProtocolType = s2c_itemdurabilitychange;
		IDC.dwItemID = Item[mItemIdx].GetID();
		IDC.nChange = Item[mItemIdx].GetMaxDurability() - curDur;
		if (g_pServer && IDC.nChange)
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &IDC, sizeof(ITEM_DURABILITY_CHANGE));
		Lua_PushNumber(L, IDC.dwItemID);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaConsumeItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nItemNature, nItemGenre, nDetailType, nItemParticular, nLevel, nSeries, Place = pos_equiproom, nResult = 0;
			nItemNature = nItemGenre = nDetailType = nItemParticular = nLevel = nSeries = -1;

			int nDelNum = (int)Lua_ValueToNumber(L, 1);

			if (nDelNum)
			{
				nItemNature = (int)Lua_ValueToNumber(L, 2);
				nItemGenre = (int)Lua_ValueToNumber(L, 3);

				if (nParamNum > 3)
					nDetailType = (int)Lua_ValueToNumber(L, 4);

				if (nParamNum > 4)
					nItemParticular = (int)Lua_ValueToNumber(L, 5);

				if (nParamNum > 5)
					nLevel = (int)Lua_ValueToNumber(L, 6);

				if (nParamNum > 6)
					nSeries = (int)Lua_ValueToNumber(L, 7);

				if (nParamNum > 7)
					Place = (int)Lua_ValueToNumber(L, 8);

				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nDelNum, nItemNature, nItemGenre, nDetailType, nItemParticular, nLevel, nSeries, Place));
				return 1;
			}
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaRemoveItemIdx(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	int nIdx, nNum;

	if (nParamNum > 1)
	{
		nIdx = (int)Lua_ValueToNumber(L, 1);
		nNum = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nIdx = (int)Lua_ValueToNumber(L, 1);
		nNum = 1;
	}
	BOOL result = Player[nPlayerIndex].m_ItemList.RemoveItemIdx(nIdx, nNum);
	if (result)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaEndGiveBox(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	S2C_GIVE_BOX NetCommand;
	NetCommand.ProtocolType = s2c_openaffairbox;
	NetCommand.nType = 2;
	strcpy(NetCommand.Value, "");
	strcpy(NetCommand.Value1,"");
	strcpy(NetCommand.Value2,"");
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, "", sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_GIVE_BOX));
	return 0;
}

int LuaSetPlayerItemLock(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0)
	{
		return 0;
	}

	//int typeLock = 1;
	int typeLock = LOCK_STATE_UNLOCK;
	int mItemIdx = (int)Lua_ValueToNumber(L, 1);
	int m_PlayerIdx = GetPlayerIndex(L);

	if (mItemIdx < 0)
	{
		return 0;
	}

	if (m_PlayerIdx < 0)
	{
		return 0;
	}

	if (Player[m_PlayerIdx].m_ItemList.GetPrice(mItemIdx)) //#fix loi khi vat pham khoa van bay ban duoc
	{
		Player[m_PlayerIdx].m_ItemList.SetPriceFromScript(mItemIdx, 0);
	}

	if (Lua_IsNumber(L, 2))
	{
		typeLock = (int)Lua_ValueToNumber(L, 2);
	}

	Item[mItemIdx].SetPlayerItemLock(typeLock);
	Item[mItemIdx].SetPlayerItemHLock(0);
	PLAYER_ITEM_LOCK_SYNC	sMoney;
	sMoney.ProtocolType = s2c_playeritemlocksync;
	sMoney.m_ItemIdx = Item[mItemIdx].GetID();
	sMoney.m_InsuranceCourse = Item[mItemIdx].GetPlayerItemLock();
	sMoney.m_InsuranceHourCourse = Item[mItemIdx].GetPlayerItemHLock();
	sMoney.sPrice = 0;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_ITEM_LOCK_SYNC));
	Lua_PushNumber(L, 1);
	return 1;
}

int	LuaMsgToPlayer(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		const char* szMsg = Lua_ValueToString(L, 1);
		if (szMsg)
			KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char*)szMsg, strlen(szMsg));
	}

	return 0;
}

int LuaMsgToTeam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Player[nPlayerIndex].m_cTeam.m_nID >= 0)
		{
			const	char* szMsg = Lua_ValueToString(L, 1);
			int nTeamLeaderId = g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nCaptain;
			if (nTeamLeaderId > 0)
				KPlayerChat::SendSystemInfo(1, nTeamLeaderId, "SendSystemInfo nTeamLeaderId lon hon 0", (char*)szMsg, strlen(szMsg));

			for (int i = 0; i < MAX_TEAM_MEMBER; i++)
			{
				int nMemberId = g_Team[Player[nPlayerIndex].m_cTeam.m_nID].m_nMember[i];
				if (nMemberId > 0)
				{
					if (szMsg)
						KPlayerChat::SendSystemInfo(1, nMemberId, "SendSystemInfo szMsg", (char*)szMsg, strlen(szMsg));
				}
			}
		}
	}
	return 0;
}

int LuaMsgToSubWorld(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;

	const char* szMsg = Lua_ValueToString(L, 1);
	if (szMsg)
		KPlayerChat::SendSystemInfo(0, 0, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char*)szMsg, strlen(szMsg));
	return 0;
}

int LuaMsgToAroundRegion(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum <= 0) return 0;

	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));
	const char* szMsg = Lua_ValueToString(L, 2);
	int nChannelID = -1;
	if (nParamNum >= 3)
		nChannelID = (int)Lua_ValueToNumber(L, 3);

	if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
	{
		if (szMsg)
		{
			int nIndex = PlayerSet.GetFirstPlayer();
			while (nIndex > 0)
			{
				if (Npc[Player[nIndex].m_nIndex].m_SubWorldIndex == nSubWorldIndex)
					KPlayerChat::SendSystemInfo(1, nIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char*)szMsg, strlen(szMsg), nChannelID);

				nIndex = PlayerSet.GetNextPlayer();
			}
		}
	}
	return 0;
}


int LuaMsgToFaction(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum <= 0) return 0;

	int nPlayerIndex, nFaction;
	const char* szMsg;
	int nChannelID = -1;

	if (nParamNum < 4)
	{
		nPlayerIndex = GetPlayerIndex(L);
		nFaction = (int)Lua_ValueToNumber(L, 1);
		szMsg = Lua_ValueToString(L, 2);
		nChannelID = (int)Lua_ValueToNumber(L, 3);
	}
	else
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
		nFaction = (int)Lua_ValueToNumber(L, 2);
		szMsg = Lua_ValueToString(L, 3);
		nChannelID = (int)Lua_ValueToNumber(L, 4);
	}

	if (nPlayerIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_cFaction.m_nCurFaction < 0)
		return 0;

	if (szMsg)
	{
		int nIndex = PlayerSet.GetFirstPlayer();
		while (nIndex > 0)
		{
			if (Player[nIndex].m_cFaction.m_nCurFaction == nFaction)
				KPlayerChat::SendSystemInfo(1, nIndex, MESSAGE_SYSTEM_FACTION_HEAD, (char*)szMsg, strlen(szMsg), nChannelID);

			nIndex = PlayerSet.GetNextPlayer();
		}
	}
	return 0;
}

int LuaMsgToTong(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum <= 0) return 0;

	int nPlayerIndex;
	DWORD dwTongID;
	const char* szMsg;
	int nChannelID = -1;

	if (nParamNum < 4)
	{
		nPlayerIndex = GetPlayerIndex(L);
		dwTongID = (DWORD)Lua_ValueToNumber(L, 1);
		szMsg = Lua_ValueToString(L, 2);
		nChannelID = (int)Lua_ValueToNumber(L, 3);
	}
	else
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
		dwTongID = (DWORD)Lua_ValueToNumber(L, 2);
		szMsg = Lua_ValueToString(L, 3);
		nChannelID = (int)Lua_ValueToNumber(L, 4);
	}

	if (nPlayerIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_cTong.GetTongNameID() == -1)
		return 0;

	if (szMsg)
	{
		int nIndex = PlayerSet.GetFirstPlayer();
		while (nIndex > 0)
		{
			if (Player[nIndex].m_cTong.GetTongNameID() == dwTongID)
				KPlayerChat::SendSystemInfo(1, nIndex, MESSAGE_SYSTEM_TONG_HEAD, (char*)szMsg, strlen(szMsg), nChannelID);

			nIndex = PlayerSet.GetNextPlayer();
		}
	}
	return 0;
}

int LuaAddTrap(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	int nRange = 1;
	int nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	int nMpsX = (int)Lua_ValueToNumber(L, 2);
	int nMpsY = (int)Lua_ValueToNumber(L, 3);
	DWORD nTrapID = (DWORD)g_FileName2Id((char*)Lua_ValueToString(L, 4));
	if (nSubWorldIndex < 0 || nSubWorldIndex > MAX_SUBWORLD)
		return 1;
	SubWorld[nSubWorldIndex].SetTrap(nTrapID, nMpsX, nMpsY, nRange);
	Lua_PushNumber(L, 1);
	return 1;
}
int LuaAddObj(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 5)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nObjID = (int)Lua_ValueToNumber(L, 1);
	if (nObjID <= 0 || nObjID >= ObjSet.m_cTabFile.GetHeight())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nSubWorldIndex = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 2));

	if (nSubWorldIndex == -1)
		return 0;

	KMapPos	Pos;

	Pos.nSubWorld = nSubWorldIndex;
	SubWorld[nSubWorldIndex].Mps2Map((int)Lua_ValueToNumber(L, 3), (int)Lua_ValueToNumber(L, 4),
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY,
		&Pos.nOffX, &Pos.nOffY);
	KObjItemInfo	sInfo;
	sInfo.m_nItemID = 0;
	sInfo.m_nItemWidth = 0;
	sInfo.m_nItemHeight = 0;
	sInfo.m_nMoneyNum = 0;
	sInfo.m_szName[0] = 0;
	sInfo.m_nColorID = 0;
	sInfo.m_nGenre = 0;
	sInfo.m_nDetailType = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	sInfo.m_dwNpcId1 = 0;
	sInfo.m_nParticularType = 0;
//	sInfo.m_bOverLook = 0;
	int nObj = ObjSet.Add(nObjID, Pos, sInfo);
	if (nObj <= 0) return 0;

	Object[nObj].SetScriptFile((char*)Lua_ValueToString(L, 5));
	if (nParamNum > 5)
		Object[nObj].SetImageDir((int)Lua_ValueToNumber(L, 6));
	if (nParamNum > 6)
		Object[nObj].SetState((int)Lua_ValueToNumber(L, 7));

	Lua_PushNumber(L, nObj);
	return 1;
}
int LuaAddObstacle(Lua_State* L) //#Set V藅 C秐
{
	if (Lua_GetTopIndex(L) < 4)
	{
		Lua_PushNumber(L, 1);
		return 1;
	}
	int nRange = 1;
	int nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	int nMpsX = (int)Lua_ValueToNumber(L, 2);
	int nMpsY = (int)Lua_ValueToNumber(L, 3);
	long value = (long)Lua_ValueToNumber(L, 4);
	SubWorld[nSubWorldIndex].SetObstacle(value, nMpsX, nMpsY, nRange);
	Lua_PushNumber(L, 1);
	return 1;
}

int  LuaEnterNewWorld(Lua_State* L) //NewWorld(WorldId, X,Y)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	int nResult = 0;
	if (Lua_GetTopIndex(L) >= 3)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 1);
		nResult = Npc[Player[nPlayerIndex].m_nIndex].ChangeWorld(dwWorldId, (int)Lua_ValueToNumber(L, 2) * 32, (int)Lua_ValueToNumber(L, 3) * 32);
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaSetPlayerPKState(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);

	int nPlayerIndex;
	BYTE nPKFlag;
	BOOL nbLockPK;

	if (nParamNum < 3)
	{
		nPlayerIndex = GetPlayerIndex(L);
		nPKFlag = (BYTE)Lua_ValueToNumber(L, 1);
		nbLockPK = (BOOL)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
		nPKFlag = (BYTE)Lua_ValueToNumber(L, 2);
		nbLockPK = (BOOL)Lua_ValueToNumber(L, 3);
	}

	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cPK.SetNormalPKState(nPKFlag, nbLockPK);
	return 0;
}

int LuaSetMask(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nMaskType = (int)Lua_ValueToNumber(L, 1);
	if (nMaskType > 0)
	{
		Player[nPlayerIndex].m_ItemList.SetMaskLock(TRUE);
		Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = nMaskType;
	}
	else
	{
		Player[nPlayerIndex].m_ItemList.SetMaskLock(FALSE);
		int nMaskIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mask);
		if (nMaskIdx > 0)
			g_MaskChangeRes.GetInteger(Item[nMaskIdx].GetParticular() + 2, 2, 0, &Npc[Player[nPlayerIndex].m_nIndex].m_MaskType);
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = 0;
	}
	return 0;
}

int LuaReSetMask(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex;
	if (nParamNum < 1)
	{
		nPlayerIndex = GetPlayerIndex(L);
	}
	else
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
	}

	if (nPlayerIndex <= 0) return 0;

	Player[nPlayerIndex].m_ItemList.SetMaskLock(FALSE);
	int nIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mask);

	Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = Item[nIdx].GetBaseMagic();
	return 0;
}
int LuaRandomNew(Lua_State* L)
{

	int nMin = (int)Lua_ValueToNumber(L, 1);
	int nMax = (int)Lua_ValueToNumber(L, 2);
	if (nMin > nMax)
		return 0;


	int grandommax = g_Random(nMax - nMin + 1);
	int nValue = grandommax + nMin;
	//printf("Test: %d - %d - %d - %d\n",nMin,nMax,grandommax,nValue);
	Lua_PushNumber(L, nValue);

	return 1;
}
int LuaSetPos(Lua_State* L)//SetPos(X,Y)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount != 2) return 0;
	int nPlayerIndex = GetPlayerIndex(L);

	int nX = (int)Lua_ValueToNumber(L, 1);
	int nY = (int)Lua_ValueToNumber(L, 2);

	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetPos(nX * 32, nY * 32);
	}
	return 0;
}

int LuaGetPos(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY);
		Lua_PushNumber(L, nPosX);
		Lua_PushNumber(L, nPosY);
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex);
	}
	else
		return 0;
	return 3;
}

int LuaGetNewWorldPos(Lua_State* L)//W,X,Y = GetWorldPos()
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nPosX, &nPosY);

		int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}

		Lua_PushNumber(L, nSubWorldID);
		Lua_PushNumber(L, ((int)(nPosX / 32)));
		Lua_PushNumber(L, ((int)(nPosY / 32)));
	}
	else
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		return 3;
	}
	return 3;
}
int LuaGetNpcLevel(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNil(L);
		return 1;
	}

	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if ((nNpcIdx <= 0) || (nNpcIdx >= MAX_NPC))
	{
		Lua_PushNil(L);
		return 1;
	}

	Lua_PushNumber(L, Npc[nNpcIdx].m_Level);
	return 1;
}
int LuaDropNpcMoney(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nIndex = (int)Lua_ValueToNumber(L, 1);
	int nMoneyNum = (int)Lua_ValueToNumber(L, 2);
	if (nMoneyNum <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int		nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	Npc[nIndex].GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	SubWorld[Npc[nIndex].m_SubWorldIndex].GetFreeObjPos(ptLocal);

	Pos.nSubWorld = Npc[nIndex].m_SubWorldIndex;
	SubWorld[Npc[nIndex].m_SubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y,
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY,
		&Pos.nOffX, &Pos.nOffY);

	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoneyNum);
	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(nPlayerIndex);
	}

	Lua_PushNumber(L, 1);
	return 1;
}

int LuaDropRateItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6) // Increase the parameter check to 7
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0)
		return 0;

	int nSubWorldIndex = 0;
	int nX, nY;

	Npc[nNpcIndex].GetMpsPos(&nX, &nY);
	nSubWorldIndex = Npc[nNpcIndex].GetSubWorldIndex();

	int nCount = (int)Lua_ValueToNumber(L, 2);
	const char* pFileName = (char*)Lua_ValueToString(L, 3);
	int nUnknow = (int)Lua_ValueToNumber(L, 4);
	int nItemLevel = (int)Lua_ValueToNumber(L, 5);
	int nItemSeries = (int)Lua_ValueToNumber(L, 6);
	// int nLuck = (int)Lua_ValueToNumber(L, 7); // Add nLuck parameter

	Npc[nNpcIndex].DropRateItem(nCount, pFileName, nUnknow, nItemLevel, nItemSeries, nPlayerIndex); //

	return 0;
}

int LuaDropItem(Lua_State* L)//#lua drop item
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nIndex = 0;
	int nSubWorldIndex = 0;
	int nX, nY;
	int nIdx = 0;

	nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex <= 0)
		nIndex = Player[nPlayerIndex].m_nIndex;

	Npc[nIndex].GetMpsPos(&nX, &nY);
	nSubWorldIndex = Npc[nIndex].GetSubWorldIndex();

	if (nSubWorldIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nItemClass = (int)Lua_ValueToNumber(L, 2);
	int nDetailType = (int)Lua_ValueToNumber(L, 3);
	int nParticularType = (int)Lua_ValueToNumber(L, 4);
	int nLevel = (int)Lua_ValueToNumber(L, 5);
	if (nLevel > 10) //max level item l?10
	{
		nLevel = 10;
	}
	int nSeries = (int)Lua_ValueToNumber(L, 6);
	int nLuck = (int)Lua_ValueToNumber(L, 7);
	//-------------------------------Fix by Fong Ki襲 th猰 MagicLevel khi drop item
	int nMagicLevel = 1;
	if (nParamNum > 7)
		nMagicLevel = (int)Lua_ValueToNumber(L, 8);
	if (nMagicLevel > 10)
		nMagicLevel = 10;
	int nItemLevel[6];
	ZeroMemory(nItemLevel, sizeof(nItemLevel));
	nItemLevel[0] = nMagicLevel;
	//---end MagicLevel
	int nStackNum = 1;
	int nEnChance = 0;
	int nPoint = 0;
	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;
	int nHour = 0;
	//
	BOOL bSkip = FALSE;
	for (int i = 0; i < 6; i++)
	{
		if (!bSkip)
		{
			if (g_Random(2 + nLuck))
			{
				nItemLevel[i] = nMagicLevel;
			}
			else
			{
				nItemLevel[i] = 0;
				bSkip = TRUE;
			}
		}
		else
		{
			nItemLevel[i] = 0;
		}
	}
	//
	int nTimeBelong = 0;
	if (nParamNum > 8)
		nMagicLevel = (int)Lua_ValueToNumber(L, 9);
	nIdx = Npc[nIndex].DropItemFromLuaScript(nPlayerIndex, nItemClass, nDetailType, nParticularType, nSeries, nLevel, nLuck, nItemLevel, nTimeBelong);
	//
	if (nIdx <= 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	//
	Lua_PushNumber(L, nIdx);
	return 1;
}

int LuaDropItemPUBG(Lua_State* L)//#lua drop item
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nIndex = 0;
	int nSubWorldIndex = 0;
	int nX, nY;
	int nIdx = 0;

	nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex <= 0)
		nIndex = Player[nPlayerIndex].m_nIndex;

	Npc[nIndex].GetMpsPos(&nX, &nY);
	nSubWorldIndex = Npc[nIndex].GetSubWorldIndex();

	if (nSubWorldIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nItemClass = (int)Lua_ValueToNumber(L, 2);
	int nDetailType = (int)Lua_ValueToNumber(L, 3);
	int nParticularType = (int)Lua_ValueToNumber(L, 4);
	int nLevel = (int)Lua_ValueToNumber(L, 5);
	if (nLevel > 10) //max level item l?10
	{
		nLevel = 10;
	}
	int nSeries = (int)Lua_ValueToNumber(L, 6);
	int nLuck = (int)Lua_ValueToNumber(L, 7);
	int nKind = (int)Lua_ValueToNumber(L, 8); //gold, platina
	//-------------------------------Fix by Fong Ki襲 th猰 MagicLevel khi drop item
	int nMagicLevel = 1;
	if (nParamNum > 8)
		nMagicLevel = (int)Lua_ValueToNumber(L, 9);
	if (nMagicLevel > 10)
		nMagicLevel = 10;
	int nItemLevel[6];
	ZeroMemory(nItemLevel, sizeof(nItemLevel));
	nItemLevel[0] = nMagicLevel;
	//---end MagicLevel
	int nStackNum = 1;
	int nEnChance = 0;
	int nPoint = 0;
	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;
	int nHour = 0;
	//
	BOOL bSkip = FALSE;
	for (int i = 0; i < 6; i++)
	{
		if (!bSkip)
		{
			if (g_Random(2 + nLuck))
			{
				nItemLevel[i] = nMagicLevel;
			}
			else
			{
				nItemLevel[i] = 0;
				bSkip = TRUE;
			}
		}
		else
		{
			nItemLevel[i] = 0;
		}
	}
	//
	int nTimeBelong = 0;
	if (nParamNum > 8)
		nMagicLevel = (int)Lua_ValueToNumber(L, 9);
	nIdx = Npc[nIndex].DropPUBGItemFromLuaScript(nKind, nItemClass, nDetailType, nParticularType, nSeries, nLevel, nLuck, NULL, nTimeBelong);
	//
	if (nIdx <= 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	//
	Lua_PushNumber(L, nIdx);
	return 1;
}
KTabFile g_EventItemTab;
int LuaAddEventItem(Lua_State *L)/*AddEventItem(id)*/
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nEventId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		char * szEventItm = (char *)Lua_ValueToString(L,1);
		
		if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
		{
			Lua_PushNumber(L,0);
			return 1;
		}
	}
	int nStackNum = 1;
	int nEnChance = 0;
	int nPoint = 0;
	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;
	int nHour = 0;
	if (Lua_GetTopIndex(L) > 2)
	{
		nStackNum = (int)Lua_ValueToNumber(L,2);
	}
	int xOpt = ItemSet.genXOpt(1);
	int nIndex = ItemSet.AddItemSet2(item_task, 0, 0, 0, nEventId, 0,  0, g_SubWorldSet.GetGameVersion(),0,nStackNum,nEnChance,nPoint, nYear,nMonth,nDay,nHour, 0, 0, xOpt);
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y, false, true);
	}
	else
	{
		int		nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			
			KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0 ,0);
	}		
	Lua_PushNumber(L, 1);
	return 1;
}

KTabFile g_GoldItemTab;
int LuaAddGoldItem(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0;
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		nEventId = ::GetRandomNumber(1, 200);
	}
	
	int nSerise = 0;
	if(!g_GoldItemTab.GetInteger(nEventId + 2, 10, 0, &nSerise)) //"Series" column 10
	{
		nSerise = 0;
	}
	int xOpt = ItemSet.genXOpt(10);
	int nIndex = ItemSet.AddGoldItem(nEventId,NULL,nSerise,0,nYear,nMonth,nDay,nHour,0,0, xOpt);
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int	x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y);
	}
	else
	{
		int	nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			
			KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0 ,0);
	}		

	Lua_PushNumber(L,nIndex);
	return 1;
}

int LuaAddTimeItem(Lua_State* L)
{
	int m_PlayerIdx = GetPlayerIndex(L);
	if (m_PlayerIdx < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		g_DebugLog("[Script]使用AddItem参数数量不符!");
		Lua_PushNumber(L, 0);
		return 0;
	}
	int nIdx = (int)Lua_ValueToNumber(L, 1);
	int nYear = (int)Lua_ValueToNumber(L, 2);
	if (nIdx < 0 || nYear <= 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	Item[nIdx].SetExpTime(time(0) + nYear, 0, 0, 0);

	PLAYER_ITEM_TIME_SYNC	sMoney;
	sMoney.ProtocolType = s2c_playeritemtimesync;
	sMoney.m_ItemIdx = Item[nIdx].GetID();
	sMoney.m_time = Item[nIdx].GetTime()->bYear;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_ITEM_TIME_SYNC));

	Lua_PushNumber(L, 1);
	return 1;
}

int LuaAddItem(Lua_State * L)/*AddItem(nItemClass, nDetailType, nParticualrType, nLevel, nSeries, nLuck, nItemLevel..6)*/
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 7)
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int nItemClass		= (int)Lua_ValueToNumber(L, 1);
	int nDetailType		= (int)Lua_ValueToNumber(L, 2);
	int nParticularType	= (int)Lua_ValueToNumber(L, 3);
	int nLevel			= (int)Lua_ValueToNumber(L, 4);
	int nSeries			= (int)Lua_ValueToNumber(L, 5);
	int nLuck			= (int)Lua_ValueToNumber(L, 6);
	int nItemLevel[MAX_ITEM_MAGICATTRIB];
	int nStackNum = 1;
	int nEnChance = 0;
	int nPoint = 0;
	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;
	int nHour = 0;
	int bLock = 0;
	int sLock = 0;
	
	ZeroMemory(nItemLevel, sizeof(nItemLevel));
	nItemLevel[0] = (int)Lua_ValueToNumber(L, 7);
	
	if (nParamNum >= 15)
	{
		nItemLevel[1] = (int)Lua_ValueToNumber(L, 8);
		nItemLevel[2] = (int)Lua_ValueToNumber(L, 9);
		nItemLevel[3] = (int)Lua_ValueToNumber(L, 10);
		nItemLevel[4] = (int)Lua_ValueToNumber(L, 11);
		nItemLevel[5] = (int)Lua_ValueToNumber(L, 12);
		nStackNum = (int)Lua_ValueToNumber(L, 13);
		nEnChance = (int)Lua_ValueToNumber(L, 14);
		nPoint	= (int)Lua_ValueToNumber(L, 15);
	}
	else
	{
		for (int i = 0; i < 5; i ++)
			nItemLevel[i + 1] = nItemLevel[0];
	}

	if (nParamNum >= 19)
	{
		nYear = (int)Lua_ValueToNumber(L, 16);
		nMonth = (int)Lua_ValueToNumber(L, 17);
		nDay = (int)Lua_ValueToNumber(L, 18);
		nHour	= (int)Lua_ValueToNumber(L, 19);
		bLock = (int)Lua_ValueToNumber(L, 20);
		sLock = (int)Lua_ValueToNumber(L, 21);

		if(nYear > 0)
			nYear =	time(0) + nYear;
	}
	int xOpt = ItemSet.genXOpt(nLuck);
	int nIndex = ItemSet.AddItemSet2(nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType, nItemLevel, g_SubWorldSet.GetGameVersion(),0,nStackNum,nEnChance,nPoint, nYear,nMonth,nDay,nHour, bLock, sLock, xOpt);
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y, false, true);
	}
	else
	{
		int		nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			
			KMapPos sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0 ,0);
	}
	
	Lua_PushNumber(L,nIndex);
	//Lua_PushNumber(L,1);
	return 1;
}

int LuaAddItem2(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0;

	int nItemNature = 0;
	int nItemClass = 0;
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nSeries = series_num;
	int nLuck = 0;
	int nItemLevel[MAX_ITEM_MAGICLEVEL];
	ZeroMemory(nItemLevel, sizeof(nItemLevel));

	nItemNature = (int)Lua_ValueToNumber(L, 1);
	nItemClass = (int)Lua_ValueToNumber(L, 2);
	nDetailType = (int)Lua_ValueToNumber(L, 3);
	nParticularType = (int)Lua_ValueToNumber(L, 4);
	nLevel = (int)Lua_ValueToNumber(L, 5);
	nSeries = (int)Lua_ValueToNumber(L, 6);

	if (nParamNum > 6)
		nLuck = (int)Lua_ValueToNumber(L, 7);

	nItemLevel[0] = 0;
	if (nParamNum > 7)
		nItemLevel[0] = (int)Lua_ValueToNumber(L, 8);
	if (nParamNum > 12)
	{
		nItemLevel[1] = (int)Lua_ValueToNumber(L, 9);
		nItemLevel[2] = (int)Lua_ValueToNumber(L, 10);
		nItemLevel[3] = (int)Lua_ValueToNumber(L, 11);
		nItemLevel[4] = (int)Lua_ValueToNumber(L, 12);
		nItemLevel[5] = (int)Lua_ValueToNumber(L, 13);
	}
	else
	{
		for (int i = 1; i < MAX_ITEM_NORMAL_MAGICATTRIB; i++)
			nItemLevel[i] = nItemLevel[0];
	}

	for (int i = MAX_ITEM_NORMAL_MAGICATTRIB; i < MAX_ITEM_NORMAL_MAGICATTRIB; i++)
		nItemLevel[i] = 0;

	int nIndex = 0;
	int xOpt = ItemSet.genXOpt(nLuck);
	switch (nItemNature)
	{
	case NATURE_GOLD:
	case NATURE_PLATINA:
		nIndex = ItemSet.Add(nItemNature, nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType, nItemLevel[0] == 0 ? NULL : nItemLevel, g_SubWorldSet.GetGameVersion(), 0, xOpt);
		break;
	default:
		nIndex = ItemSet.Add(nItemNature, nItemClass, nSeries, nLevel, nLuck, nDetailType, nParticularType, nItemLevel, g_SubWorldSet.GetGameVersion(), 0, xOpt);
		break;
	}
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y, false, true);
	}
	else
	{
		int		nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);

			KMapPos sMapPos;
			KObjItemInfo	sInfo;

			Player[nPlayerIndex].GetAboutPos(&sMapPos);

			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;

			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0, 0);
	}
	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaUpgradePlatinaItem(Lua_State* L)
{
	int index; // ebx
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	if (lua_gettop(L) <= 0)
		return 0;
	index = lua_tonumber(L, 1);
	if (index <= 0 || index >= MAX_ITEM)
		return 0;
	if (Item[index].GetNature() == NATURE_PLATINA || Item[index].GetNature() == NATURE_GOLD)
	{
		if (ItemSet.UpgradePlatinaEquip(g_SubWorldSet.GetGameVersion(), &Item[index]))
		{
			Player[nPlayerIndex].m_ItemList.SyncItem(index);
			lua_pushnumber(L, 1.0);
		}
		else
		{
			lua_pushnumber(L, 0.0);
		}
		return 1;
	}
	else
	{
		lua_pushnil(L);
		return 1;
	}
}

int LuaResetGoldEquipItem(Lua_State* L)
{
	int index; // ebx
	int nPlayerIndex = GetPlayerIndex(L);
	int nItemLevel[MAX_ITEM_MAGICLEVEL];
	ZeroMemory(nItemLevel, sizeof(nItemLevel));
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	if (lua_gettop(L) <= 0)
		return 0;
	index = lua_tonumber(L, 1);
	if (index <= 0 || index >= MAX_ITEM)
		return 0;
	if (Item[index].GetNature() == NATURE_GOLD)
	{

		int newItemIdx = ItemSet.Add(Item[index].GetNature(), Item[index].GetGenre(), Item[index].GetSeries(), Item[index].GetLevel(), 0, Item[index].GetRow()\
			, Item[index].GetParticular(), NULL, g_SubWorldSet.GetGameVersion(), 0, Item[index].GetMaxOptMultiply());
		if (newItemIdx)
		{
			BOOL result = Player[nPlayerIndex].m_ItemList.Remove(index);
			if (!result)
			{//remove failed
				Lua_PushNumber(L, 0);
				return 1;
			}
			int		x, y;
			if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[newItemIdx].GetWidth(), Item[newItemIdx].GetHeight(), &x, &y))
			{
				Player[nPlayerIndex].m_ItemList.AddKIL(newItemIdx, pos_equiproom, x, y, false, true);
			}
			else
			{
				int		nIdx = Player[nPlayerIndex].m_ItemList.Hand();
				if (nIdx)
				{
					Player[nPlayerIndex].m_ItemList.Remove(nIdx);

					KMapPos sMapPos;
					KObjItemInfo	sInfo;

					Player[nPlayerIndex].GetAboutPos(&sMapPos);

					sInfo.m_nItemID = nIdx;
					sInfo.m_nItemWidth = Item[nIdx].GetWidth();
					sInfo.m_nItemHeight = Item[nIdx].GetHeight();
					sInfo.m_nMoneyNum = 0;
					char szNameTemp[OBJ_NAME_LENGHT];
					if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
					{
						sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
						strcpy(sInfo.m_szName, szNameTemp);
					}
					else
						strcpy(sInfo.m_szName, Item[nIdx].GetName());
					sInfo.m_nColorID = Item[nIdx].GetColorItem();
					sInfo.m_nGenre = Item[nIdx].GetGenre();
					sInfo.m_nDetailType = Item[nIdx].GetDetailType();
					sInfo.m_nParticularType = Item[nIdx].GetParticular();
					sInfo.m_nMovieFlag = 1;
					sInfo.m_nSoundFlag = 1;
					sInfo.m_dwNpcId1 = 0;

					int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
					if (nObj >= 0)
					{
						if (Item[nIdx].GetGenre() == item_task)
						{
							Object[nObj].SetEntireBelong(nPlayerIndex);
						}
						else
						{
							Object[nObj].SetItemBelong(nPlayerIndex);
						}
					}
				}
				Player[nPlayerIndex].m_ItemList.AddKIL(newItemIdx, pos_hand, 0, 0);
			}
			if (newItemIdx <= 0)
			{
				Lua_PushNumber(L, 0);
				return 1;
			}

			Lua_PushNumber(L, newItemIdx);
			return 1;
		}
		else
		{
			lua_pushnumber(L, 0.0);
		}
		return 1;
	}
	else
	{
		lua_pushnil(L);
		return 1;
	}
}

int LuaAddItemID(Lua_State* L)
{
	//int nPlayerIndex = GetPlayerIndex(L);

	//if (nPlayerIndex <= 0)
	//{
	//	Lua_PushNumber(L, 0);
	//	return 1;
	//}

	//int nParamNum = Lua_GetTopIndex(L);
	//if (nParamNum < 2)
	//{
	//	Lua_PushNumber(L, 0);
	//	return 1;
	//}

	//int nIndex = (int)Lua_ValueToNumber(L, 1);

	//if (nIndex <= 0)
	//{
	//	Lua_PushNumber(L, 0);
	//	return 1;
	//}

	//int	P = pos_equiproom, x = 0, y = 0;
	//POINT	pPos;
	//if (nParamNum > 2)
	//{
	//	P = (int)Lua_ValueToNumber(L, 2);
	//}
	//if (nParamNum > 3)
	//{
	//	P = (int)Lua_ValueToNumber(L, 2);
	//	x = (int)Lua_ValueToNumber(L, 3);
	//}
	//if (nParamNum > 4)
	//{
	//	P = (int)Lua_ValueToNumber(L, 2);
	//	x = (int)Lua_ValueToNumber(L, 3);
	//	y = (int)Lua_ValueToNumber(L, 4);
	//}
	//if (P < pos_hand || P > pos_num)
	//	return 0;

	//if (nParamNum == 5 && Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom(P)].CheckRoom(x, y, Item[nIndex].GetWidth(), Item[nIndex].GetHeight()))
	//	Player[nPlayerIndex].m_ItemList.Add(nIndex, P, x, y, false);
	//else if (nParamNum == 4 && Player[nPlayerIndex].m_ItemList.PositionToIndex(P, x) <= 0)
	//	Player[nPlayerIndex].m_ItemList.Add(nIndex, P, x, 0, false);
	//else if (nParamNum == 3 && Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom(P)].FindRoom(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &pPos))
	//	Player[nPlayerIndex].m_ItemList.Add(nIndex, P, pPos.x, pPos.y, false);
	//else
	//	Player[nPlayerIndex].m_ItemList.InsertEquipment(nIndex, false);

	Lua_PushNumber(L, 0);
	return 1;
}

KTabFile g_MineItemTab;
int LuaAddMineItem(Lua_State *L)
{
	// modify by spe 03/06/13
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nEventId = 0;
	int nYear = 0,nMonth = 0,nDay = 0,nHour = 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (Lua_IsNumber(L, 1))
	{
		nEventId = (int)Lua_ValueToNumber(L,1);
	}
	else
	{
		char * szEventItm = (char *)Lua_ValueToString(L,1);
		
		if (!g_MineItemTab.GetInteger(szEventItm, "ParticularType", 0, &nEventId))
		{
			Lua_PushNumber(L,0);
			return 1;
		}
	}

	int nLevel = 1;
	int nSerise = 0;
	
	if (Lua_IsNumber(L, 2))
	{
		nLevel = (int)Lua_ValueToNumber(L,2);
	}

	if (Lua_IsNumber(L, 3))
	{
		nSerise = (int)Lua_ValueToNumber(L,3);
	}

	if (nParamNum >= 7)
	{
		nYear = (int)Lua_ValueToNumber(L,4);
		nMonth = (int)Lua_ValueToNumber(L,5);
		nDay = (int)Lua_ValueToNumber(L,6);
		nHour =(int)Lua_ValueToNumber(L,7);
	}
	int nXOpt = ItemSet.genXOpt(10); //luck = 10
	int nIndex = ItemSet.AddItemSet2(item_mine, nSerise, nLevel, 1, 1, nEventId, 0, 0, 0, 1, 0, 0, nYear, nMonth, nDay, nHour, 0, 0, nXOpt);
	
	if (nIndex <= 0) 
	{
		Lua_PushNumber(L,0);
		return 1;
	}
	
	int		x, y;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y);
	}
	else
	{
		int		nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);
			
			KMapPos			sMapPos;
			KObjItemInfo	sInfo;
			
			Player[nPlayerIndex].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIdx;
			sInfo.m_nItemWidth = Item[nIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				sprintf(szNameTemp, "%s x %d", Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				strcpy(sInfo.m_szName, szNameTemp);
			}
			else
				strcpy(sInfo.m_szName, Item[nIdx].GetName());
			sInfo.m_nColorID = Item[nIdx].GetColorItem();
			sInfo.m_nGenre = Item[nIdx].GetGenre();
			sInfo.m_nDetailType = Item[nIdx].GetDetailType();
			sInfo.m_nParticularType = Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_dwNpcId1 = 0;
			
			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
				{
					Object[nObj].SetEntireBelong(nPlayerIndex);
				}
				else
				{
					Object[nObj].SetItemBelong(nPlayerIndex);
				}
			}
		}
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0 ,0);
	}		
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaCheckFreeBoxItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nItemClass = (int)Lua_ValueToNumber(L, 1);
	int nDetailType = (int)Lua_ValueToNumber(L, 2);
	int nParticularType = (int)Lua_ValueToNumber(L, 3);
	int nLevel = (int)Lua_ValueToNumber(L, 4);
	int nWidth, nHeight;
	if (!ItemSet.Get_SizeItem(nItemClass, nDetailType, nParticularType, nLevel, &nWidth, &nHeight))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int	nx, ny;
	if (!Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(nWidth, nHeight, &nx, &ny))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaGetCBItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Item[nIndex].GetKind());
		Lua_PushNumber(L, Item[nIndex].GetGenre());
		Lua_PushNumber(L, Item[nIndex].GetDetailType());
		Lua_PushNumber(L, Item[nIndex].GetParticular());
		Lua_PushNumber(L, Item[nIndex].GetLevel());
		Lua_PushNumber(L, Item[nIndex].GetSeries());
		Lua_PushNumber(L, Item[nIndex].GetLine());
	}
	return 7;
}

int LuaGetOTItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[0].nAttribType);//hien 1 type
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[0].nValue[0]);//hien 1 value
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[1].nAttribType);//hien 2 type
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[1].nValue[0]);//hien 2 value
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[2].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[2].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[3].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[3].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[4].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[4].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[5].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[5].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[6].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[6].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[7].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[7].nValue[0]);
	}
	return 16;
}

int LuaGetOTLVItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[0]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[1]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[2]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[3]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[4]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[5]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[6]);
		Lua_PushNumber(L, Item[nIndex].m_GeneratorParam.nGeneratorLevel[7]);
	}
	return 8;
}

int LuaGetBASItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[0].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[0].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[1].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[1].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[2].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[2].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[3].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[3].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[4].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[4].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[5].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[5].nValue[0]);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[6].nAttribType);
		Lua_PushNumber(L, Item[nIndex].m_aryBaseAttrib[6].nValue[0]);
	}
	return 14;
}

int LuaAddPlayerTranslifeValue(Lua_State* L)//#trung sinh
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nTranslifeValue = (int)Lua_ValueToNumber(L, 1);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Npc[Player[nPlayerIndex].m_nIndex].nReBorn += nTranslifeValue;
	return 0;
}

//==================Trung sinh=================
int LuaSetPlayerReBornValue(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nReBornValue = (int)Lua_ValueToNumber(L, 1);
	int nReBornLevel = (int)Lua_ValueToNumber(L, 2);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cReBorn.AddReBornValue(nReBornValue, nReBornLevel);
	return 0;
}


int LuaGetPlayerReBornNum(Lua_State* L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerrebornvalue;

	nReBornValue = Player[nPlayerIndex].m_cReBorn.GetReBornNum();

lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;
}

int LuaSetPlayerReBornNum(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_setplayerrebornvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_setplayerrebornvalue;


	int nReBornValue = (int)Lua_ValueToNumber(L, 1);
	if (nReBornValue < 0)
		nReBornValue = 0;

	Player[nPlayerIndex].m_cReBorn.SetReBornValue(nReBornValue);

lab_setplayerrebornvalue:
	return 0; 
}

int LuaGetPlayerReBornValue(Lua_State* L)
{
	int nReBornValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerrebornvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerrebornvalue;

	nReBornValue = Player[nPlayerIndex].m_cReBorn.GetReBornValue();

lab_getplayerrebornvalue:
	Lua_PushNumber(L, nReBornValue);
	return 1;

}
int LuaGetMateName(Lua_State* L) //#MateName
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	Lua_PushString(L, Player[nPlayerIndex].m_PlayerMateName);
	return 1;
}

int LuaMakeMate(Lua_State* L)//#MateName
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;

	strcpy(Player[nPlayerIndex].m_PlayerMateName, (char*)Lua_ValueToString(L, 1));
	return 0;
}

int LuaDeleteMate(Lua_State* L) //#MateName
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	const char* szMateName = Player[nPlayerIndex].m_PlayerMateName;
	if (szMateName[0])
		strcpy(Player[nPlayerIndex].m_PlayerMateName, "");

	return 0;
}

int LuaGetPlayerNpcIdx(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex = nPlayerIndex;
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
			nIndex = (int)Lua_ValueToNumber(L, 1);
		Lua_PushNumber(L, Player[nIndex].m_nIndex);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetCurServerSec(Lua_State* L)
{
	Lua_PushNumber(L, KSG_GetCurSec());
	return 1;
}

int LuaSetNumber(Lua_State* L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 4)
		goto lab_setnumber;

	nResult = KSG_StringSetValue((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3), (int)Lua_ValueToNumber(L, 4));

lab_setnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}
int LuaSetNumber2(Lua_State* L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 4)
		goto lab_setnumber;

	nResult = KSG_StringSetValue((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3), (int)Lua_ValueToNumber(L, 4));

lab_setnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNumber(Lua_State* L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 3)
		goto lab_getnumber;
	nResult = KSG_StringGetValue((int)Lua_ValueToNumber(L, 1),
		(int)Lua_ValueToNumber(L, 2),
		(int)Lua_ValueToNumber(L, 3));

lab_getnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}



int LuaGetNumber2(Lua_State* L)
{
	int nResult = 0;
	if (Lua_GetTopIndex(L) < 3)
		goto lab_getnumber;
	nResult = KSG_StringGetValue((int)Lua_ValueToNumber(L, 1),
		(int)Lua_ValueToNumber(L, 2),
		(int)Lua_ValueToNumber(L, 3));

lab_getnumber:
	Lua_PushNumber(L, nResult);
	return 1;
}
int LuaSetItemParam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;

	int nItemIndex = (int)Lua_ValueToNumber(L, 1);
	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM)
		return 0;

	int nKind = (int)Lua_ValueToNumber(L, 2);
	int nValue = (int)Lua_ValueToNumber(L, 3);

	switch (nKind)
	{
	case 1:
		Item[nItemIndex].SetStackNum(nValue);
		break;
	default:
		break;
	}

	return 0;
}

int LuaGetItemParam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNil(L);
		return 1;
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;

	int nItemIndex = (int)Lua_ValueToNumber(L, 1);
	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM)
	{
		Lua_PushNil(L);
		return 1;
	}
	int nKind = (int)Lua_ValueToNumber(L, 2);
	int nResult = -1;
	switch (nKind)
	{
	case 1:
		if (Item[nItemIndex].CanStack())
			nResult = (int)Item[nItemIndex].GetStackNum();
		else
			nResult = 1;
		break;
	default:
		break;
	}


	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaFindPlayer(Lua_State* L)
{
	int nIndex = 0;
	if (Lua_IsNumber(L, 1))
	{
		DWORD dwID = (DWORD)Lua_ValueToNumber(L, 1);
		nIndex = NpcSet.SearchUUID(dwID);
	}
	else if (Lua_IsString(L, 1))
	{
		const char* pszName = (const char*)Lua_ValueToString(L, 1);
		nIndex = PlayerSet.GetFirstPlayer();
		while (nIndex > 0)
		{
			if (strcmp(Player[nIndex].m_PlayerName, pszName) == 0)
				break;

			nIndex = PlayerSet.GetNextPlayer();
		}
	}
	Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaGetPlayerTranslifeValue(Lua_State* L)//#trung sinh
{
	int nTranslifeValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayertranslifevalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayertranslifevalue;
	nTranslifeValue = Npc[Player[nPlayerIndex].m_nIndex].nReBorn;

lab_getplayertranslifevalue:
	Lua_PushNumber(L, nTranslifeValue);
	return 1;
}

int LuaGetROItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	BYTE nX, nY, nRoom;
	int nIndex = 0;
	nRoom = (BYTE)Lua_ValueToNumber(L,1);
	if (nRoom < room_equipment || nRoom >= room_num)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	nX = (BYTE)Lua_ValueToNumber(L, 2);
	nY = (BYTE)Lua_ValueToNumber(L, 3);
	nIndex = Player[nPlayerIndex].m_ItemList.m_Room[nRoom].FindItem(nX, nY);
	if (nPlayerIndex > 0)
		Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaCalcFreeItemCellCount(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nWidth = 1;
	int nHeight = 1;
	int nRoom = room_equipment;
	if (Lua_GetTopIndex(L) > 2)
	{
		nWidth = (int)Lua_ValueToNumber(L, 1);
		nHeight = (int)Lua_ValueToNumber(L, 2);
	}
	if (Lua_GetTopIndex(L) > 3)
	{
		nRoom = (int)Lua_ValueToNumber(L, 3);
	}
	if (nRoom < room_equipment || nRoom > room_num)
		return 0;
	int nCount = Player[nPlayerIndex].m_ItemList.CalcFreeItemCellCount(nWidth, nHeight, nRoom);
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaCheckRoom(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nIndex, nWidth = 0, nHeight = 0;;

	if (Lua_GetTopIndex(L) >= 2)
	{
		nIndex = (int)Lua_ValueToNumber(L, 1);
		nWidth = Item[nIndex].GetWidth();
		nHeight = Item[nIndex].GetHeight();
	}
	if (Lua_GetTopIndex(L) >= 3)
	{
		nWidth = (int)Lua_ValueToNumber(L, 1);
		nHeight = (int)Lua_ValueToNumber(L, 2);
	}
	if (nPlayerIndex <= 0)
		return 0;

	if (nWidth <= 0 || nHeight <= 0)
		return 0;

	ItemPos	sItemPos;
	if (FALSE == Player[nPlayerIndex].m_ItemList.SearchPosition(nWidth, nHeight, &sItemPos))
		Lua_PushNumber(L, 0);
	else
		Lua_PushNumber(L, 1);

	return 1;
}

int LuaGetItemCount(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nItemNature = 0, nItemGenre = 0, nDetailType = 0, nParticularType = 0, nLevel = 0, nSeries = 0, Place = pos_equiproom;
			nItemNature = nItemGenre = nDetailType = nLevel = nSeries = -1;

			nItemNature = (int)Lua_ValueToNumber(L, 1);
			nItemGenre = (int)Lua_ValueToNumber(L, 2);

			if (nParamNum > 2)
				nDetailType = (int)Lua_ValueToNumber(L, 3);

			if (nParamNum > 3)
				nParticularType = (int)Lua_ValueToNumber(L, 4);

			if (nParamNum > 4)
				nLevel = (int)Lua_ValueToNumber(L, 5);

			if (nParamNum > 5)
				nSeries = (int)Lua_ValueToNumber(L, 6);

			if (nParamNum > 6)
				Place = (int)Lua_ValueToNumber(L, 7);	//Place

			int rCount = Player[nPlayerIndex].m_ItemList.CountCommonItem(nItemNature, nItemGenre, nDetailType, nParticularType, nLevel, nSeries, Place);
			Lua_PushNumber(L, rCount);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetTaskItemCount(Lua_State* L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L, 1);
			}
			else
			{
				char* szEventItm = (char*)Lua_ValueToString(L, 1);

				if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
				{
					Lua_PushNumber(L, 0);
					return 1;
				}
			}

			int nCount = Player[nPlayerIndex].m_ItemList.GetTaskItemNum(nEventId);
			Lua_PushNumber(L, nCount);
		}
		else
		{
			g_DebugLog("GetItemCount nPlayerIndex <= 0");
			Lua_PushNumber(L, 0);
		}
	}
	else
	{
		g_DebugLog("GetItem 参数不够!");
		Lua_PushNumber(L, 0);
	}

	return 1;
}

int LuaDelItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 9)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	BYTE nKind = (BYTE)Lua_ValueToNumber(L, 1);
	int nLine = (int)Lua_ValueToNumber(L, 2);
	int nGenre = (int)Lua_ValueToNumber(L, 3);
	int nDetail = (int)Lua_ValueToNumber(L, 4);
	int nParticular = (int)Lua_ValueToNumber(L, 5);
	int nLevel = (int)Lua_ValueToNumber(L, 6);
	int nSeries = (int)Lua_ValueToNumber(L, 7);
	int nRoom = (int)Lua_ValueToNumber(L, 8);
	int nNum = (int)Lua_ValueToNumber(L, 9);
	if (nRoom < pos_hand || nRoom > pos_num)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	
	int nResult = Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nNum, nKind, nGenre, nDetail, nParticular, nLevel, nSeries, nRoom);
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNameItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushString(L, Item[nIdx].GetName());
		return 1;
	}
	return 0;
}

int LuaFindItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			if (nParamNum <= 1)
			{
				int nIdx = Player[nPlayerIndex].m_ItemList.FindSame((int)Lua_ValueToNumber(L, 1));
				if (nIdx)
				{
					Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nPlace);
					Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nX);
					Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nY);
					return 3;
				}
				else
					return 0;
			}
			int nItemGenre, nDetailType, nLevel, nSeries;
			nItemGenre = nDetailType = nLevel = nSeries = -1;

			nItemGenre = (int)Lua_ValueToNumber(L, 1);

			if (nParamNum > 1)
				nDetailType = (int)Lua_ValueToNumber(L, 2);

			if (nParamNum > 2)
				nLevel = (int)Lua_ValueToNumber(L, 3);

			if (nParamNum > 3)
				nSeries = (int)Lua_ValueToNumber(L, 4);

			int nIdx = Player[nPlayerIndex].m_ItemList.IsItemExist(nItemGenre, nDetailType, -1, nSeries, nLevel);

			if (nIdx)
			{
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nIdx);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nPlace);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nX);
				Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.m_Items[nIdx].nY);
				return 4;
			}
		}
	}
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, 0);
	return 4;
}

int LuaFindItemEx(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum <= 2)
	{
		int nIndex, nItemGenre = -1, nDetail = -1, nParticur = -1, nLevel = -1, nSeries = series_num, nLuck = 0, nStackNum = 0;
		nIndex = (int)Lua_ValueToNumber(L, 1);
		if (nIndex > 0)
		{
			nItemGenre = Item[nIndex].GetGenre();
			nDetail = Item[nIndex].GetDetailType();
			nParticur = Item[nIndex].GetParticular();
			nLevel = Item[nIndex].GetLevel();
			nSeries = Item[nIndex].GetSeries();
			nLuck = Item[nIndex].m_GeneratorParam.nLuck;
			nStackNum = Item[nIndex].GetStackNum();
		}

		Lua_PushNumber(L, nItemGenre);
		Lua_PushNumber(L, nDetail);
		Lua_PushNumber(L, nParticur);
		Lua_PushNumber(L, nLevel);
		Lua_PushNumber(L, nSeries);
		Lua_PushNumber(L, nLuck);
		Lua_PushNumber(L, nStackNum);
		return 7;
	}

	int nIndex = 0, nItemGenre = -1, nDetail = -1, nParticur = -1, nLevel = -1, nSeries = series_num, nLuck = 0, nStackNum = 0;
	if (nParamNum > 2)
	{
		nIndex = Player[nPlayerIndex].m_ItemList.PositionToIndex((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	}
	if (nParamNum > 3)
	{
		nIndex = Player[nPlayerIndex].m_ItemList.m_Room[PositionToRoom((int)Lua_ValueToNumber(L, 1))].FindItem((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3));
	}

	if (nIndex > 0)
	{
		nItemGenre = Item[nIndex].GetGenre();
		nDetail = Item[nIndex].GetDetailType();
		nParticur = Item[nIndex].GetParticular();
		nLevel = Item[nIndex].GetLevel();
		nSeries = Item[nIndex].GetSeries();
		nLuck = Item[nIndex].m_GeneratorParam.nLuck;
		nStackNum = Item[nIndex].GetStackNum();
	}
	Lua_PushNumber(L, nIndex);
	Lua_PushNumber(L, nItemGenre);
	Lua_PushNumber(L, nDetail);
	Lua_PushNumber(L, nParticur);
	Lua_PushNumber(L, nLevel);
	Lua_PushNumber(L, nSeries);
	Lua_PushNumber(L, nLuck);
	Lua_PushNumber(L, nStackNum);
	return 8;
}

int LuaGetMantleItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetMantle());
		return 1;
	}
	return 0;
}

int LuaSetMantleItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		if (Item[nIdx].GetGenre() == item_equip &&
			Item[nIdx].GetDetailType() == equip_armor)
		{
			Item[nIdx].SetMantle((int)Lua_ValueToNumber(L, 2));
			Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);

			if (Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_mantle) <= 0)
				Npc[Player[nPlayerIndex].m_nIndex].m_MantleType = Item[nIdx].GetMantle(); //#phi phong
		}
		Lua_PushNumber(L, nIdx);
		return 1;
	}
	return 0;
}

int LuaGetParamItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetParam());
		return 1;
	}
	return 0;
}

int LuaSetParamItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetParam((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L, nIdx);
		return 1;
	}
	return 0;
}

int LuaGetGlowLightItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);
	if (nIdx > 0)
	{
		Lua_PushNumber(L, Item[nIdx].GetItemGlowLight());
		return 1;
	}
	return 0;
}

int LuaSetGlowLightItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	int nIdx = (int)Lua_ValueToNumber(L, 1);

	if (nIdx > 0)
	{
		Item[nIdx].SetItemGlowLight((int)Lua_ValueToNumber(L, 2));
		Player[nPlayerIndex].m_ItemList.SyncItem(nIdx);
		Lua_PushNumber(L, nIdx);
		return 1;
	}
	return 0;
}

int LuaHaveItem(Lua_State* L)/*HaveItem(nItemTemplateId);*/
{
	if (Lua_GetTopIndex(L) > 0)
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0)
		{
			int nEventId = 0;
			if (Lua_IsNumber(L, 1))
			{
				nEventId = (int)Lua_ValueToNumber(L, 1);
			}
			else
			{
				char* szEventItm = (char*)Lua_ValueToString(L, 1);

				if (!g_EventItemTab.GetInteger(szEventItm, "具体类别", 0, &nEventId))
				{
					Lua_PushNumber(L, 0);
					return 1;
				}
			}

			int nResult = Player[nPlayerIndex].m_ItemList.IsTaskItemExist(nEventId);
			Lua_PushNumber(L, (nResult != 0));
		}
		else
		{
			g_DebugLog("HaveItem nPlayerIndex <= 0");
			Lua_PushNumber(L, 0);
		}
	}
	else
	{
		g_DebugLog("HaveItem 参数不够!");
		Lua_PushNumber(L, 0);
	}

	return 1;
}

int LuaGetMagicAttrib(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nIndex;
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	nIndex = (int)Lua_ValueToNumber(L, 1);
	if (nIndex > 0)
	{
		for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[i].nAttribType);
			Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[i].nValue[0]);
			Lua_PushNumber(L, Item[nIndex].m_aryMagicAttrib[i].nValue[2]);
		}
		return MAX_ITEM_MAGICATTRIB * 3;
	}
	else
	{
		for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			Lua_PushNumber(L, 0);
			Lua_PushNumber(L, 0);
			Lua_PushNumber(L, 0);
		}
		return MAX_ITEM_MAGICATTRIB * 3;
	}
	return 0;
}

int LuaSetMagicAttrib(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
		return 0;

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 24)
		return 0;

	int nIndex = (int)Lua_ValueToNumber(L, 1);

	if (nIndex > 0 && Item[nIndex].GetGenre() == item_equip)
	{
		int i = 0, k = 2;
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = (int)Lua_ValueToNumber(L, k);
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = MAKELONG((int)Lua_ValueToNumber(L, k + 1),
				(int)Lua_ValueToNumber(L, k + 2));
			Item[nIndex].m_aryMagicAttrib[i].nAttribType = (int)Lua_ValueToNumber(L, k);
			Item[nIndex].m_aryMagicAttrib[i].nValue[0] = (int)Lua_ValueToNumber(L, k + 1);
			Item[nIndex].m_aryMagicAttrib[i].nValue[1] = -1;
			Item[nIndex].m_aryMagicAttrib[i].nValue[2] = (int)Lua_ValueToNumber(L, k + 2);
			k += 3;
		}
		for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i] = 0;
			Item[nIndex].m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nAttribType = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[0] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[1] = 0;
			Item[nIndex].m_aryMagicAttrib[i].nValue[2] = 0;
		}
		Player[nPlayerIndex].m_ItemList.SyncItemMagicAttrib(nIndex);
	}
	return 0;
}

/*
AddMagic(nPlayerIndex, nMagicID, nLevel)
DelMagic(nPlayerIndex, nMagicId)
HaveMagic(nPlayerIndex, nMagicId)
GetMagicLevel(nPlayerIndex, nMagicId)
SetMagicLevel(nPlayerIndex, nMagicId, nLevel)
ModifyMagicLevel(nPlayerIndex ,nMagicId, nDLevel)
*/
int LuaAddMagic(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;
	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		const char* sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0) return 0;
	}
	Player[nPlayerIndex].m_nIndex;
	int nLevel = 0;
	if (nParamCount >= 2)
		nLevel = (int)Lua_ValueToNumber(L, 2);
	else
		nLevel = 0;

	int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nLevel);
	if (nRet)
	{
		PLAYER_SKILL_LEVEL_SYNC NewSkill;
		NewSkill.ProtocolType = s2c_playerskilllevel;
		NewSkill.m_nSkillID = nSkillId;
		NewSkill.m_nSkillLevel = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId);
		NewSkill.m_nAddLevel = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetAddLevel(nSkillId);
		NewSkill.m_nSkillExp = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetExp(nSkillId);
		NewSkill.m_bTempSkill = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.IsTempSkill(nSkillId);
		NewSkill.m_nLeavePoint = Player[nPlayerIndex].m_nSkillPoint;
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
	}

	return 0;
}

int LuaDelMagic(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;

	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0) return 0;

	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		const char* sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0) return 0;
	}
	Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Remove(nSkillId);
	return 0;

}

int LuaHaveMagic(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 1) return 0;

	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0) return 0;

	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		const char* sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0)
		{
			Lua_PushNumber(L, -1);
		}
		return 1;
	}

	if (Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.FindSame(nSkillId))
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId));
	}
	else
	{
		Lua_PushNumber(L, -1);
	}

	return 1;
}

int LuaGetSkillName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	if (nSkillId > 0)
	{
		char nSkillName[64];
		memset(nSkillName, 0, sizeof(nSkillName));
		ISkill* pISkill = g_SkillManager.GetSkill(nSkillId, 1);
		strcpy(nSkillName, pISkill->GetSkillName());
		Lua_PushString(L, nSkillName);
		return 1;
	}
	Lua_PushString(L, "none");
	return 0;
}

int LuaGetSkillIdInSkillList(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nSkillIndex = (int)Lua_ValueToNumber(L, 1);
	int nSkillId = 0;
	if (nSkillIndex > 0)
	{
		nSkillId = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetSkillId(nSkillIndex);
	}
	Lua_PushNumber(L, nSkillId);
	return 1;
}

int LuaSetSkillLevel(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nSkillId = 0;

	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		const char* sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0) return 0;
	}
	int nLevel = (int)Lua_ValueToNumber(L, 2);
	if (nLevel >= 0)
		Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.SetSkillLevelDirectlyUsingId(nSkillId, nLevel);
	return 0;
}

int LuaRollBackSkills(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nSkillId = 0;

	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) return 0;
	int rAll = (int)Lua_ValueToNumber(L, 1);

	int nTotalSkill = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.RollBackSkills(rAll);
	Lua_PushNumber(L, nTotalSkill);
	return 1;
}

int LuaUpdateSkillList(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	Player[nPlayerIndex].SendSyncData_Skill();
	return 0;
}

int LuaGetMagicLevel(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;

	if (nParamCount < 1) return 0;
	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0) return 0;

	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
	{
		nSkillId = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		const char* sSkillName = Lua_ValueToString(L, 1);
		g_OrdinSkillsSetting.GetInteger((char*)sSkillName, "SkillId", 0, &nSkillId);
		if (nSkillId <= 0) return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId));
	return 1;

}
/*
int LuaSetMagicLevel(Lua_State * L)
{
int nParamCount = Lua_GetTopIndex(L);
int nPlayerIndex = 0;
int nTemp = 0;
if (nParamCount < 1) return 0;

  nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0) return 0;

	  int nSkillId = 0;
	  if (Lua_IsNumber(L, nTemp))
	  {
	  nSkillId = (int)Lua_ValueToNumber (L, 1);
	  }
	  else
	  {
	  const char * sSkillName = Lua_ValueToString(L, 1);
	  nSkillId = g_OrdinSkillsSetting.FindRow((char *)sSkillName) - 2;
	  if (nSkillId <= 0 ) return 0;
	  }
	  int nNpcIndex = Player[nPlayerIndex].m_nIndex;
	  if (nNpcIndex > 0)
	  Lua_PushNumber(L,Npc[nNpcIndex].m_SkillList.SetSkillLevel(nSkillId, (int)Lua_ValueToNumber(L, 2)));
	  return 0;
	  }
*/

/*nNpcTemplateId GetNpcTmpId(sName)
sName:Npc名称
nNpcTemplateID:模板中Id
*/
int LuaGetNpcTemplateID(Lua_State* L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		if (Lua_IsString(L, 1))
		{
			const char* pName = lua_tostring(L, 1);
			int nId = g_NpcSetting.FindRow((char*)pName) - 2;
			Lua_PushNumber(L, nId);
		}
		else
			return 0;
	}
	else
		return 0;
	return 1;
}

int LuaSetNpcWalkOriginPos(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SendCommand(do_walk, Npc[nNpcIndex].m_OriginX, Npc[nNpcIndex].m_OriginY);
	return 1;
}
//m_nDamageReduction
int LuaSetNpcDamageReduction(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	int reduction = (int)lua_tonumber(L, 2);
	Npc[nNpcIndex].m_nDamageReduction = reduction; // 0->1.0
	return 1;
}

int LuaSetNpcAIType(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	int AIType = (int)lua_tonumber(L, 2);
	Npc[nNpcIndex].m_AiMode = AIType; // 1,2,3,4,5,6
	return 1;
}

int LuaNpcWalk(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	int x = (int) lua_tonumber(L, 2);
	int y = (int) lua_tonumber(L, 3);
	if(x > 0 && y > 0)
		Npc[nNpcIndex].SendCommand(do_walk, x, y);
	return 1;
}

int LuaNpcChat(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	char* pszMsg = (char *)lua_tostring(L, 2);
	int nShowInMsgPad = 0;
	if (nParamNum >= 3)
		nShowInMsgPad = (int)Lua_ValueToNumber(L, 3);
	if (strlen(pszMsg) > 64)
		return 0;
	KPlayerChat::NpcChat(nNpcIndex, pszMsg, strlen(pszMsg), nShowInMsgPad == 1);
	return 1;
}

int LuaSetNpcFindPathTime(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	DWORD dwTime = (DWORD)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (dwTime != -1)
		Npc[nNpcIndex].m_uFindPathMaxTime = dwTime;
	else
		Npc[nNpcIndex].m_uFindPathMaxTime = -1;
	return 0;
}

int LuaSetNpcOwner(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	strcpy(Npc[nNpcIndex].Owner, (char*)Lua_ValueToString(L, 2));

	if (nParamNum >= 3)
		Npc[nNpcIndex].m_bNpcFollowFindPath = (BOOL)Lua_ValueToNumber(L, 3);

	Npc[nNpcIndex].m_uFindPathTime = g_SubWorldSet.GetGameTime();
	return 0;
}

int LuaGetNpcOwner(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushString(L, Npc[nNpcIndex].Owner);
	return 1;
}

/*
nNpcIndex AddNpc(nNpcTemplateId,nLevel, nSubWorldIndex, nPosX, nPosY )

  功能：增加一个特定的NPC
  参数：
  nNpcTemplateId: NPC在NPC模板中的id
  nLevel:Npc的等级
  nSubWorldIndex:所处的世界id
  nPosX：X (点坐标)
  nPosY：Y (点坐标)
  nNpcIndex:增加后，将返回该Npc在游戏世界的Index，如果不成功返回nil
*/

int LuaAddNpc(Lua_State* L)
{
	char* pName = NULL;
	int	   nId = 0;
	if (Lua_GetTopIndex(L) < 5) return 0;
	if (Lua_IsNumber(L, 1))
	{
		nId = (int)Lua_ValueToNumber(L, 1);
	}
	/*else if	(Lua_IsString(L,1))
	{
		pName = (char *)lua_tostring(L,1);
		nId = g_NpcSetting.FindRow((char*)pName) - 2;
	}*/
	else return 0;
	if (nId < 0) nId = 0;
	int nLevel = (int)lua_tonumber(L, 2);
	if (nLevel >= 128) nLevel = 127;
	if (nLevel < 0) nLevel = 1;
	int	nNpcIdxInfo = MAKELONG(nLevel, nId);//(nId << 7) + nLevel;
	int nSeries = (int)lua_tonumber(L, 6);
	int nNpcIdx = NpcSet.AddNpcSet2(nNpcIdxInfo, nSeries, (int)lua_tonumber(L, 3), (int)lua_tonumber(L, 4), (int)lua_tonumber(L, 5));
	if (Lua_GetTopIndex(L) >= 7 && Lua_IsString(L, 7))
	{
		pName = (char*)lua_tostring(L, 7);
		if (pName && pName[0])
			g_StrCpy(Npc[nNpcIdx].Name, (char*)pName);
	}
	Lua_PushNumber(L, nNpcIdx);
	return 1;
}

int LuaNoReloadNpcAttr(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) {
		Lua_PushNumber(L, -1);
		return 0;
	}
	Npc[nNpcIndex].m_bNoReloadAttr = TRUE;
	Lua_PushNumber(L, 0);
	return 1;
}






int LuaGetNpcBoss(Lua_State* L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
		int nIsBoss = 0;
		if (nNpcIndex > 0)
		{
			nIsBoss = (int)Npc[nNpcIndex].m_cGold.GetGoldType();
			Lua_PushNumber(L, nIsBoss);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}


int LuaSetNpcGoldBoss(Lua_State* L)
{
	int	   nNpcIdx = 0;
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	if (Lua_IsNumber(L, 1))
	{
		nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	}

	if (nNpcIdx < 0)
		return 0;

	Npc[nNpcIdx].m_cGold.SetGoldTypeAndBackData();
	Npc[nNpcIdx].m_cGold.ChangeGold();

	Lua_PushNumber(L, nNpcIdx);
	return 1;
}

/*nResult DelNpc (nNpcIndex)
nResult:返回成功与否,1为成功,0为失败
*/
int LuaDelNpc(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
	{
		if (!Npc[nNpcIndex].IsPlayer() && Npc[nNpcIndex].m_SubWorldIndex >= 0 && Npc[nNpcIndex].m_RegionIndex >= 0)
		{
			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].RemoveNpc(nNpcIndex);
			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].DecRef(Npc[nNpcIndex].m_MapX, Npc[nNpcIndex].m_MapY, obj_npc);
			NpcSet.Remove(nNpcIndex);
		}

	}
	return 0;
}

/*
nDelCount DelNpcsInRgn(nSubWorld,nRegionId, nKind)
功能：删除某个游戏世界中某个Region内的所有某类的NPC
返回:删除的Npc个数
*/
int LuaDelNpcsInRgn(Lua_State* L)
{
	//Question
	return 0;
}
/*
nDelCount DelNpcsInWld(nSubWorldId, nKind)
功能：删除某个游戏世界中的所有Npc
*/
int LuaDelNpcsInWld(Lua_State* L)
{
	return 0;
}

/*
SetNpcPos (nNpcIndex, x, y)
功能：设置/修改一个NPC的位置
参数：
nNpcIndex:Npc的id
x：X坐标
y：Y坐标
*/
int LuaSetNpcPos(Lua_State* L)
{
	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 3) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0) return 0;
	Npc[nNpcIndex].m_MapX = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_MapY = (int)Lua_ValueToNumber(L, 3);

	return 0;
}

int LuaSetNpcActionScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	//
	char* szScript = (char*)Lua_ValueToString(L, 2);
	strcpy(Npc[nNpcIndex].ActionScript, szScript);
	Npc[nNpcIndex].m_ActionScriptID = g_FileName2Id((char*)Lua_ValueToString(L, 2));
	//
	if (Npc[nNpcIndex].m_Kind == kind_normal)
	{
		Npc[nNpcIndex].ExecuteScript(szScript, "OnRevive", nNpcIndex);		// monster #idx c馻 npc khi h錳 sinh
	}
	//
	return 0;
}
int LuaSetNpcActiveRange(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2 ) 
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) 
		return 0;
	//
	int nRange = (int)Lua_ValueToNumber(L,2);
	if (nRange < 0)
		return 0;
	Npc[nNpcIndex].m_CurrentActiveRadius = nRange;
	
	return 0;
}
int LuaSetNpcDropScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	strcpy(Npc[nNpcIndex].DropRateScript, Lua_ValueToString(L, 2));
	Npc[nNpcIndex].m_DropRateScriptID = g_FileName2Id((char*)Lua_ValueToString(L, 2));
	return 0;
}

int LuaPlayerDropAllItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (Lua_GetTopIndex(L) == 1)
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
	}
	if (nPlayerIndex <= 0) return 0;
	// drop all item
	Player[nPlayerIndex].ServerThrowAllItem();
	return 0;
}
int LuaSetNpcSeries(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0)
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	//
	BYTE nValue = (BYTE)Lua_ValueToNumber(L, 2);
	if (nValue < 0 || nValue > 4)
		return 0;
	//
	Npc[nNpcIndex].m_Series = nValue;
	return 0;
}

int LuaGetNpcSeries(Lua_State* L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIndex > 0 && nNpcIndex < MAX_NPC)
		{
			Lua_PushNumber(L, Npc[nNpcIndex].m_Series);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaSetNpcName(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (Lua_IsNumber(L, 2))
	{
		KTabFile Replace;
		Replace.Load(RENAME_NPC_SETTING_TABFILE);
		Replace.GetString((int)Lua_ValueToNumber(L, 2) + 2, "targetname", "", Npc[nNpcIndex].Name, sizeof(Npc[nNpcIndex].Name));
	}
	else if (Lua_IsString(L, 2))
		strcpy(Npc[nNpcIndex].Name, (char*)Lua_ValueToString(L, 2));

	return 0;
}

int LuaGetNpcName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Lua_PushString(L, Npc[nNpcIndex].Name);
	return 1;
}

int LuaSetNpcParam(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 2)
		Npc[nNpcIndex].m_nNpcParam[(int)Lua_ValueToNumber(L, 2)] = (int)Lua_ValueToNumber(L, 3);
	else
		Npc[nNpcIndex].m_nNpcParam[0] = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaGetNpcParam(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if (nParamNum > 1)
		Lua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[(int)Lua_ValueToNumber(L, 2)]);
	else
		Lua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[0]);
	return 1;
}

int LuaSetNpcTimer(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_nNpcTimeout = (int)Lua_ValueToNumber(L, 2) + g_SubWorldSet.GetGameTime();
	return 0;
}

int LuaGetNpcTimer(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, g_SubWorldSet.GetGameTime() - Npc[nNpcIndex].m_nNpcTimeout < 0 ? 0 : g_SubWorldSet.GetGameTime() - Npc[nNpcIndex].m_nNpcTimeout);
	return 1;
}

int LuaSetNpcExp(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nExp = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentExperience = nExp;
	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_Experience = nExp;
	return 0;
}

int LuaSetNpcLife(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nLife = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentLife = nLife;
	Npc[nNpcIndex].m_CurrentLifeMax = nLife;

	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_LifeMax = nLife;
	return 0;
}

int LuaSetNpcAR(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nAttackR = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentAttackRating = nAttackR;
	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_AttackRating = nAttackR;
	return 0;
}

int LuaSetNpcDefense(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nDefense = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentDefend = nDefense;
	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_Defend = nDefense;
	return 0;
}

int LuaSetNpcDamage(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SetPhysicsDamage((int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3));
	return 0;
}


int LuaSetNpcDmgEx(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	if ((nParamNum > 6) && (int)Lua_ValueToNumber(L, 7))
	{
		Npc[nNpcIndex].m_CurrentAddPhysicsDamage = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] = (int)Lua_ValueToNumber(L, 3);
		if (Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] > 0)
		{
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[1] = POISON_DAMAGE_TIME;
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[2] = POISON_DAMAGE_INTERVAL;
		}
		else
		{
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[1] = 0;
			Npc[nNpcIndex].m_CurrentPoisonDamage.nValue[2] = 0;
		}
		Npc[nNpcIndex].m_CurrentColdDamage.nValue[0] = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_CurrentColdDamage.nValue[2] = (int)Lua_ValueToNumber(L, 4);
		if (Npc[nNpcIndex].m_CurrentColdDamage.nValue[0] > 0 && Npc[nNpcIndex].m_CurrentColdDamage.nValue[2] > 0)
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[1] = COLD_DAMAGE_TIME;
		else
		{
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[2] = 0;
			Npc[nNpcIndex].m_CurrentColdDamage.nValue[1] = 0;
		}
		Npc[nNpcIndex].m_CurrentFireDamage.nValue[0] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentFireDamage.nValue[2] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentLightDamage.nValue[0] = (int)Lua_ValueToNumber(L, 6);
		Npc[nNpcIndex].m_CurrentLightDamage.nValue[2] = (int)Lua_ValueToNumber(L, 6);
	}
	else
	{
		Npc[nNpcIndex].m_CurrentAddPhysicsMagic = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[0] = (int)Lua_ValueToNumber(L, 3);
		if (Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[0] > 0)
		{
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[1] = POISON_DAMAGE_TIME;
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[2] = POISON_DAMAGE_INTERVAL;
		}
		else
		{
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[1] = 0;
			Npc[nNpcIndex].m_CurrentPoisonMagic.nValue[2] = 0;
		}
		Npc[nNpcIndex].m_CurrentColdMagic.nValue[0] = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] = (int)Lua_ValueToNumber(L, 4);
		if (Npc[nNpcIndex].m_CurrentColdMagic.nValue[0] > 0 && Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] > 0)
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[1] = COLD_DAMAGE_TIME;
		else
		{
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[0] = 0;
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[2] = 0;
			Npc[nNpcIndex].m_CurrentColdMagic.nValue[1] = 0;
		}
		Npc[nNpcIndex].m_CurrentFireMagic.nValue[0] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentFireMagic.nValue[2] = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_CurrentLightMagic.nValue[0] = (int)Lua_ValueToNumber(L, 6);
		Npc[nNpcIndex].m_CurrentLightMagic.nValue[2] = (int)Lua_ValueToNumber(L, 6);
	}
	return 0;
}

int LuaGetTeamMem(Lua_State* L)
{
	int nTeamId = -1;
	BYTE nPos = -1;
	int nMemberId = 0;

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum >= 2)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
		if (nPos)
		{
			nPos = Lua_ValueToNumber(L, 2);
			nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
		}
		else
			nMemberId = g_Team[nTeamId].m_nCaptain;
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			if (nPos)
			{
				nPos = Lua_ValueToNumber(L, 1);
				nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
			}
			else
				nMemberId = g_Team[nTeamId].m_nCaptain;
		}
	}
	//  m_nCaptain	 0 - 6
	Lua_PushNumber(L, nMemberId);
	return 1;
}

int LuaSetNpcResist(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 6)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentPhysicsResistMax = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_CurrentPoisonResistMax = (int)Lua_ValueToNumber(L, 3);
	Npc[nNpcIndex].m_CurrentLightResistMax = (int)Lua_ValueToNumber(L, 4);
	Npc[nNpcIndex].m_CurrentFireResistMax = (int)Lua_ValueToNumber(L, 5);
	Npc[nNpcIndex].m_CurrentColdResistMax = (int)Lua_ValueToNumber(L, 6);

	Npc[nNpcIndex].m_CurrentPhysicsResist = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].m_CurrentPoisonResist = (int)Lua_ValueToNumber(L, 3);
	Npc[nNpcIndex].m_CurrentLightResist = (int)Lua_ValueToNumber(L, 4);
	Npc[nNpcIndex].m_CurrentFireResist = (int)Lua_ValueToNumber(L, 5);
	Npc[nNpcIndex].m_CurrentColdResist = (int)Lua_ValueToNumber(L, 6);

	if (nParamNum > 6)
	{
		Npc[nNpcIndex].m_PhysicsResist = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_PoisonResist = (int)Lua_ValueToNumber(L, 3);
		Npc[nNpcIndex].m_LightResist = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_FireResist = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_ColdResist = (int)Lua_ValueToNumber(L, 6);

		Npc[nNpcIndex].m_PhysicsResistMax = (int)Lua_ValueToNumber(L, 2);
		Npc[nNpcIndex].m_PoisonResistMax = (int)Lua_ValueToNumber(L, 3);
		Npc[nNpcIndex].m_LightResistMax = (int)Lua_ValueToNumber(L, 4);
		Npc[nNpcIndex].m_FireResistMax = (int)Lua_ValueToNumber(L, 5);
		Npc[nNpcIndex].m_ColdResistMax = (int)Lua_ValueToNumber(L, 6);
	}
	return 0;
}

int LuaSetNpcRevTime(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].SetReviveFrame((int)Lua_ValueToNumber(L, 2));
	return 0;
}

int LuaSetNpcSpeed(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentWalkSpeed = (int)Lua_ValueToNumber(L, 2);
	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_WalkSpeed = (int)Lua_ValueToNumber(L, 2);
	return 0;
}

int LuaSetPlayerSpeed(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nPlayerIdx = (int)Lua_ValueToNumber(L, 1);

	if (nPlayerIdx < 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;

	if ((int)Lua_ValueToNumber(L, 2) == -1) {
		// Reset speed control
		Player[nPlayerIdx].m_bSpeedControl = FALSE;
		Player[nPlayerIdx].UpdataCurData();
		return 0;
	}
	Npc[Player[nPlayerIdx].m_nIndex].m_CurrentWalkSpeed = (int)Lua_ValueToNumber(L, 2);
	Npc[Player[nPlayerIdx].m_nIndex].m_CurrentRunSpeed = (int)Lua_ValueToNumber(L, 2);
	Player[nPlayerIdx].m_bSpeedControl = TRUE;
	return 0;
}

int LuaSetNpcHitRecover(Lua_State *L) //Set th阨 gian ph鬰 h錳
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	//
	Npc[nNpcIndex].m_CurrentHitRecover = (int)Lua_ValueToNumber(L, 2);
	//
	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_HitRecover = (int)Lua_ValueToNumber(L, 2);
	return 0;
}
int LuaSetNpcHonorId(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	//
	Npc[nNpcIndex].m_btHonorId = (int)Lua_ValueToNumber(L, 2);

	return 0;
}

int LuaSetPlayerHonorId(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	//
	int nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	//
	Npc[Player[nPlayerIndex].m_nIndex].m_btHonorId = (int)Lua_ValueToNumber(L, 2);

	return 0;
}

int LuaGetNpcID(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;
	//
	Lua_PushNumber(L, Npc[nNpcIndex].m_dwID);
	return 1;
}

int LuaRemoveNpcSkill(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	//
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	//
	int nSkillPos = (int)Lua_ValueToNumber(L, 2);
	if (nSkillPos < 0 || nSkillPos > MAX_NPCSKILL)
		return 0;
	//
	Npc[nNpcIdx].m_SkillList.RemoveNpcSkill(nSkillPos);
	return 1;
}

int LuaSetNpcSkill(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4)
		return 0;
	//
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	//
	int nSkillID = (int)Lua_ValueToNumber(L, 2);
	if (nSkillID <= 0)
		return 0;
	//
	int nSkillLevel = (int)Lua_ValueToNumber(L, 3);
	if (nSkillLevel < 1)
		nSkillLevel = 1;
	//
	int nSkillPos = (int)Lua_ValueToNumber(L, 4);
	if (nSkillPos < 0 || nSkillPos > MAX_NPCSKILL)
		return 0;
	//
	Npc[nNpcIdx].m_SkillList.SetNpcSkill(nSkillPos, nSkillID, nSkillLevel);
	return 1;
}

int LuaNpcEnterNewWorld(Lua_State* L)
{
	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 3) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0) return 0;

	int nResult = 0;
	if (Lua_GetTopIndex(L) > 3)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 2);
		nResult = Npc[nNpcIndex].ChangeWorld(dwWorldId, (int)Lua_ValueToNumber(L, 3) * 32, (int)Lua_ValueToNumber(L, 4) * 32);
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNpcPos(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nNpcIndex;

	nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[nNpcIndex].GetMpsPos(&nPosX, &nPosY);
		Lua_PushNumber(L, nPosX);
		Lua_PushNumber(L, nPosY);
		Lua_PushNumber(L, Npc[nNpcIndex].m_SubWorldIndex);
	}
	else
		return 0;
	return 3;
}

//删除技能（技能编号）
int LuaDelAllMagic(Lua_State* L)
{
	int nPlayerIndex = 0;
	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0)
		return 0;

	Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.RemoveAllSkill();

	if (Npc[Player[nPlayerIndex].m_nIndex].m_Kind == kind_player)
	{
		Player[nPlayerIndex].SendSyncData_Skill();
	}

	return 0;

}

int LuaClearFactionIfnfo(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Player[nPlayerIndex].m_cFaction.Release();//
	}
	//Lua_PushNumber(L, nResult);
	return 0;
}

int LuaFindNpcFrID(Lua_State * L)
{
	int nIndex = 0;
	DWORD dwID = (DWORD)Lua_ValueToNumber(L, 1);
	nIndex = NpcSet.SearchID(dwID);
	Lua_PushNumber(L, nIndex);
	return 1;
}

int LuaFindNearNpc(Lua_State* L)
{
	int nIndex = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		switch ((BYTE)Lua_ValueToNumber(L, 1))
		{
		case 1:
			nIndex = Player[nPlayerIndex].FindNearNpc((int)Lua_ValueToNumber(L, 2),
				Lua_GetTopIndex(L) > 3 ? (int)Lua_ValueToNumber(L, 3) : 0);
			break;
		case 2:
			nIndex = Player[nPlayerIndex].FindNearNpc((char*)Lua_ValueToString(L, 2),
				Lua_GetTopIndex(L) > 3 ? (int)Lua_ValueToNumber(L, 3) : 0);
			break;
		case 3:
			nIndex = Player[nPlayerIndex].FindNearNpc((DWORD)Lua_ValueToNumber(L, 2),
				Lua_GetTopIndex(L) > 3 ? (int)Lua_ValueToNumber(L, 3) : 0);
			break;
		}
		Lua_PushNumber(L, nIndex);
	}
	return 1;
}

int LuaFindAroundNpc(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Lua_IsNumber(L, 1))
		{
			int nIndex = Player[nPlayerIndex].FindAroundNpc((DWORD)Lua_ValueToNumber(L, 1));
			Lua_PushNumber(L, nIndex);
			return 1;
		}
		else
			Lua_PushNil(L);
	}
	return 0;
}

int LuaGetNpcCurCamp(Lua_State* L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIndex > 0)
		{
			Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentCamp);
			return 1;
		}
	}
	Lua_PushNumber(L, camp_free);
	return 1;
}

int LuaSetNpcCamp(Lua_State* L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 && nNpcIndex > MAX_NPC) return 0;
	int nValue = (int)Lua_ValueToNumber(L, 2);
	if (nValue >= camp_num) return 0;
	Npc[nNpcIndex].SetCamp(nValue);
	return 0;
}

int LuaSetNpcFightState(Lua_State* L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 && nNpcIndex > MAX_NPC) return 0;
	int nValue = (int)Lua_ValueToNumber(L, 2);
	Npc[nNpcIndex].SetFightMode(nValue != 0);
	return 0;
}

int LuaGetNpcFightState(Lua_State* L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIndex > 0)
		{
			Lua_PushNumber(L, Npc[nNpcIndex].m_FightMode);
			return 1;
		}
	}
	return 0;
}

int LuaGetNpcCamp(Lua_State* L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
		if (nNpcIndex > 0)
		{
			Lua_PushNumber(L, Npc[nNpcIndex].m_Camp);
			return 1;
		}
	}
	Lua_PushNumber(L, camp_free);
	return 1;
}

int LuaGetNpcNewWorldPos(Lua_State* L)//W,X,Y = GetNpcWorldPos()
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum > 1)
		return 0;

	int nNpcIndex;

	nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex > 0)
	{
		int nPosX = 0;
		int nPosY = 0;
		Npc[nNpcIndex].GetMpsPos(&nPosX, &nPosY);

		int nSubWorldIndex = Npc[nNpcIndex].m_SubWorldIndex;
		int nSubWorldID = 0;
		if (nSubWorldIndex >= 0 && nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}

		Lua_PushNumber(L, nSubWorldID);
		Lua_PushNumber(L, ((int)(nPosX / 32)));
		Lua_PushNumber(L, ((int)(nPosY / 32)));
	}
	else
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		return 3;
	}
	return 3;
}

int LuaSetNpcLifeReplenish(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nLifeRep = (int)Lua_ValueToNumber(L, 2);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_CurrentLifeReplenish = nLifeRep;
	if (nParamNum > 2 && (int)Lua_ValueToNumber(L, 3) > 0)
		Npc[nNpcIndex].m_LifeReplenish = nLifeRep;
	return 0;
}

int LuaGetNpcLife(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_CurrentLifeMax);
	return 1;
}

int LuaSetPlayerRevivalPos(Lua_State* L) //SetRevivalPos(subworldid = -1, revid )
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	nPlayerIndex = GetPlayerIndex(L);
	int nSubWorldId = 0;
	int nRevId = 0;
	if (nPlayerIndex < 0)
	{
		return 0;
	}

	if (nParamCount >= 2)
	{
		nSubWorldId = (int)Lua_ValueToNumber(L, 1);
		nRevId = (int)Lua_ValueToNumber(L, 2);
	}
	else if (nParamCount == 1)
	{
		nSubWorldId = -1;
		nRevId = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		return 0;
	}

	Player[nPlayerIndex].SetRevivalPos(nSubWorldId, nRevId);
	return 0;
}

int LuaGetPlayerRevivalPos(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		return 0;
	int nSubWorldId = 0;
	int nRevId = 0;

	if (nParamCount > 2)
	{
		nSubWorldId = (int)Lua_ValueToNumber(L, 1);
		nRevId = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nRevId = (int)Lua_ValueToNumber(L, 1);
	}

	POINT Pos;
	if(g_SubWorldSet.GetRevivalPosFromId(
		(nSubWorldId>0) ? nSubWorldId : SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID, nRevId, &Pos))
	{
		Lua_PushNumber(L, nSubWorldId);
		Lua_PushNumber(L, Pos.x);
		Lua_PushNumber(L, Pos.y);
	}
	else
	{
		Lua_PushNumber(L, 53);
		Lua_PushNumber(L, 52032);
		Lua_PushNumber(L, 101696);
	}
	return 3;
}

#define MacroFun_GetPlayerInfoInt(L, MemberName) { int nPlayerIndex = GetPlayerIndex(L);\
	if (nPlayerIndex > 0){	int nNpcIndex = Player[nPlayerIndex].m_nIndex;	if (nNpcIndex > 0)Lua_PushNumber(L, Npc[nNpcIndex].MemberName);\
	else Lua_PushNil(L);}\
	else Lua_PushNil(L);\
return 1;}														

int LuaGetPlayerCurrentCamp(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_CurrentCamp);
}

int LuaGetPlayerCamp(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_Camp);
}

int LuaSetPlayerCamp(Lua_State* L)
{
	int nValue = (int)Lua_ValueToNumber(L, 1);
	if (nValue < 0) return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetCamp(nValue);
	}
	return 0;
}

int LuaSetPlayerCurrentCamp(Lua_State* L)
{
	int nValue = (int)Lua_ValueToNumber(L, 1);
	if (nValue < 0) return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetCurrentCamp(nValue);
	}
	return 0;
}

int LuaSetNpcCurCamp(Lua_State* L)
{
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 && nNpcIndex > MAX_NPC) return 0;
	int nValue = (int)Lua_ValueToNumber(L, 2);
	if (nValue >= camp_num) return 0;
	Npc[nNpcIndex].SetCurrentCamp(nValue);
	return 0;
}

int LuaRestorePlayerCamp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreCurrentCamp();
	}
	return 0;
}

int LuaOpenTong(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_OPENTONGUI;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	UiInfo.m_bParam1 = 0;
	UiInfo.m_nBufferLen = sizeof(int);

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#else
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&UiInfo, UiInfo.m_wProtocolLong + 1);
#endif
	return 0;
}

int LuaGetTongName(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushString(L, Player[nPlayerIndex].m_cTong.m_szName);
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_dwTongNameID);
		return 2;
	}
	return 0;
}

int LuaSetThueTongOwnCity(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nThue = (int)Lua_ValueToNumber(L, 1);
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(78);//tuong duong
	if (SubWorld[nTargetSubWorld].m_CityTax != nThue)
		SubWorld[nTargetSubWorld].m_CityTax = nThue;
	return 1;
}

int LuaSetViewTongOwnCity(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	const char* pszValue = (const char*)Lua_ValueToString(L, 1);
	int nTargetSubWorld = g_SubWorldSet.SearchWorld(78);//tuong duong
	if (strcmp(SubWorld[nTargetSubWorld].m_CityOwnTong, pszValue) != 0)
		strcpy(SubWorld[nTargetSubWorld].m_CityOwnTong, pszValue);
	return 1;
}

int LuaGetTongFlag(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.m_nFlag);
		return 1;
	}
	return 0;
}
int LuaGetTongInfo(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	int nInfo = (int)Lua_ValueToNumber(L, 1);
	if (nInfo == 0)
	{
		int nTongID = (int)Player[nPlayerIndex].m_cTong.GetTongNameID();
		Lua_PushNumber(L, nTongID);
	}
	if (nInfo == 1)
	{
		char szTongName[32];
		Player[nPlayerIndex].m_cTong.GetTongName(szTongName);
		Lua_PushString(L, szTongName);
	}
	if (nInfo == 2)
	{
		char szTongMasterName[32];
		Player[nPlayerIndex].m_cTong.GetTongMasterName(szTongMasterName);
		Lua_PushString(L, szTongMasterName);
	}
	if (nInfo == 3)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.GetTongMoney());
	}
	if (nInfo == 4)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.GetCamp());
	}
	if (nInfo == 5)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.GetFigure());
	}

	return 1;
}
int LuaGetTongLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.GetTongLevel());
	return 1;
}
int LuaSetTongLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nLevel = (int)Lua_ValueToNumber(L, 1);
	if (nLevel <= 0 || nLevel > 10) return 0;

	STONG_CHANGE_LEVEL_COMMAND	sChange;

	sChange.ProtocolFamily = pf_tong;
	sChange.ProtocolID = enumC2S_TONG_CHANGE_LEVEL;
	sChange.m_nTongLevel = nLevel;
	sChange.m_btFigure = Player[nPlayerIndex].m_cTong.GetFigure();
	sChange.m_dwParam = nPlayerIndex;
	sChange.m_dwTongNameID = Player[nPlayerIndex].m_cTong.GetTongNameID();
	Player[nPlayerIndex].m_cTong.GetTongName(sChange.m_szName);
	g_NewProtocolProcess.PushMsgInTong((const void*)&sChange, sizeof(sChange));

	Player[nPlayerIndex].m_cTong.SetTongLevel(nLevel);
	return 1;
}
int LuaGetTongExp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	Lua_PushNumber(L, Player[nPlayerIndex].m_cTong.GetTongExp());
	return 1;
}
int LuaSetTongExp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nExp = (int)Lua_ValueToNumber(L, 1);
	if (nExp <= 0) return 0;
	STONG_CHANGE_EXP_COMMAND	sChange;

	sChange.ProtocolFamily = pf_tong;
	sChange.ProtocolID = enumC2S_TONG_CHANGE_EXP;
	sChange.m_nTongExp = nExp;
	sChange.m_btFigure = Player[nPlayerIndex].m_cTong.GetFigure();
	sChange.m_dwParam = nPlayerIndex;
	sChange.m_dwTongNameID = Player[nPlayerIndex].m_cTong.GetTongNameID();
	Player[nPlayerIndex].m_cTong.GetTongName(sChange.m_szName);
	g_NewProtocolProcess.PushMsgInTong((const void*)&sChange, sizeof(sChange));

	Player[nPlayerIndex].m_cTong.SetTongExp(nExp);
	return 1;
}
int LuaGetPlayerFaction(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		char FactionName[100];
		Player[nPlayerIndex].GetFactionName(FactionName, 100);
		Lua_PushString(L, FactionName);
	}
	else
	{
		Lua_PushString(L, "");
	}
	return 1;
}


int LuaGetPlayerFirstAddFaction(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Lua_PushNumber(L, Player[nPlayerIndex].GetFirstAddFaction());
	else
		Lua_PushNumber(L, -1);

	return 1;
}

int LuaGetPlayerFactionNo(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Lua_PushNumber(L, Player[nPlayerIndex].GetFactionNo());
	else
		Lua_PushNumber(L, -1);

	return 1;
}

int LuaGetPlayerFactionValueName(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		char FactionName[100];
		Player[nPlayerIndex].GetFactionValueName(FactionName, 100);
		if (FactionName[0])
			Lua_PushString(L, FactionName);
		else
			Lua_PushString(L, "none");
	}
	else
	{
		Lua_PushString(L, "none");
	}
	return 1;
}

int LuaChangePlayerFaction(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nResult = 0;
	if (nPlayerIndex > 0)
	{
		const char* szFactionName = Lua_ValueToString(L, 1);
		Player[nPlayerIndex].LeaveCurFaction();
		if (strlen(szFactionName) == 0)
		{
			nResult = 1;
		}
		else
		{
			nResult = Player[nPlayerIndex].AddFaction((char*)szFactionName);
		}
	}
	Lua_PushNumber(L, nResult);
	return 1;
}

//0表示当前,1表示原始的,2表示最大的
int LuaGetPlayerColdResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentColdResist); break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ColdResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax); break;
		default:
			Lua_PushNil(L);
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerColdResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax;

		int nType = (int)Lua_ValueToNumber(L, 1);

		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentColdResist = nValue;
			break;

		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_ColdResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_ColdResistMax = nValue;
			break;
		}
	}
	return 0;
}

int LuaGetPlayerFireResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{

		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentFireResist); break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FireResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax); break;
		default:
			Lua_PushNil(L);
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerFireResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{


		int nValue = (int)Lua_ValueToNumber(L, 2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax;

		int nType = (int)Lua_ValueToNumber(L, 1);

		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentFireResist = nValue;
			break;

		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_FireResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_FireResistMax = nValue;
			break;
		}
	}
	return 0;
}

int LuaGetPlayerLightResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLightResist); break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LightResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax); break;
		default:
			Lua_PushNil(L);
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerLightResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{

		int nValue = (int)Lua_ValueToNumber(L, 2);
		if (nValue < 0) nValue = 0;
		if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax;

		int nType = (int)Lua_ValueToNumber(L, 1);

		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLightResist = nValue;
			break;
		case 1:
			Npc[Player[nPlayerIndex].m_nIndex].m_LightResist = nValue;
			break;
		case 2:
			Npc[Player[nPlayerIndex].m_nIndex].m_LightResistMax = nValue;
			break;
		}
	}
	else
		Lua_PushNil(L);
	return 0;
}

int LuaGetPlayerPoisonResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPoisonResist); break;
		case 1:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResist); break;
		case 2:
			Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax); break;
		default:
			Lua_PushNil(L);
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerPoisonResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nValue = (int)Lua_ValueToNumber(L, 2);
			if (nValue < 0) nValue = 0;
			if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax;

			int nType = (int)Lua_ValueToNumber(L, 1);

			switch ((int)Lua_ValueToNumber(L, 1))
			{
			case 0:
				Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPoisonResist = nValue;
				break;

			case 1:
				Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResist = nValue;
				break;
			case 2:
				Npc[Player[nPlayerIndex].m_nIndex].m_PoisonResistMax = nValue;
				break;
			}
		}
	}
	return 0;
}

int LuaGetPlayerPhysicsResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nType = (int)Lua_ValueToNumber(L, 1);
			switch ((int)Lua_ValueToNumber(L, 1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPhysicsResist); break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResist); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax); break;
			default:
				Lua_PushNil(L);
			}
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaSetPlayerPhysicsResist(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nValue = (int)Lua_ValueToNumber(L, 2);
			if (nValue < 0) nValue = 0;
			if (nValue > Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax) nValue = Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax;

			int nType = (int)Lua_ValueToNumber(L, 1);

			switch ((int)Lua_ValueToNumber(L, 1))
			{
			case 0:
				Npc[Player[nPlayerIndex].m_nIndex].m_CurrentPhysicsResist = nValue;
				break;
			case 1:
				Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResist = nValue;
				break;
			case 2:
				Npc[Player[nPlayerIndex].m_nIndex].m_PhysicsResistMax = nValue;
				break;
			}
		}
	}
	return 0;
}

int LuaGetNextExp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_nNextLevelExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerExp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_nExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetNpcVip(Lua_State* L)//VIP
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentExpSkillsVip);
	return 1;
}
int LuaGetNpcExpSkillsRate(Lua_State* L)//TamLTM expskills x2
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentExpSkillsEnchance);
	return 1;
}
int LuaModifyPlayerExp(Lua_State* L)//AddExp(200,10,0)
{
	int bAllTeamGet = 0;
	if (Lua_GetTopIndex(L) >= 3)		bAllTeamGet = (int)Lua_ValueToNumber(L, 3);

	int nDValue = (int)Lua_ValueToNumber(L, 1);
	int nTarLevel = (int)Lua_ValueToNumber(L, 2);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (bAllTeamGet)
			Player[nPlayerIndex].AddExp(nDValue, nTarLevel);
		else
			Player[nPlayerIndex].AddSelfExp(nDValue, nTarLevel);
	}
	return 0;
}

int LuaAddOwnExp(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		double nExp = (double)Lua_ValueToNumber(L, 1);
		if (nExp >= 0)
			Player[nPlayerIndex].DirectAddExp((double)Lua_ValueToNumber(L, 1));
	}
	return 0;
}

int LuaAddSumExp(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		double nExp = (double)Lua_ValueToNumber(L, 1);
		if (nExp >= 0)
		{
			while (nExp > 0)
			{
				double nExpAdd = Player[nPlayerIndex].m_nNextLevelExp - Player[nPlayerIndex].m_nExp;
				if (nExp >= nExpAdd)
				{
					nExp = nExp - nExpAdd;
				}
				else
				{
					nExpAdd = nExp;
					nExp = 0;
				}
				Player[nPlayerIndex].DirectAddExp(nExpAdd);
			}
		}
	}
	return 0;
}

int LuaGetPlayerLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerLife(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nType = (int)Lua_ValueToNumber(L, 1);
			switch ((int)Lua_ValueToNumber(L, 1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentLife); break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax); break;
			default:
				Lua_PushNil(L);
			}
		}

	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaRestorePlayerLife(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreLife();
	}
	return 0;
}

int LuaRestorePlayerMana(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreMana();
	}
	return 0;
}

int LuaRestorePlayerStamina(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].RestoreStamina();
	}
	return 0;
}

int LuaGetPlayerLifeReplenish(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_LifeReplenish);
}

int LuaGetPlayerMana(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nType = (int)Lua_ValueToNumber(L, 1);
			switch ((int)Lua_ValueToNumber(L, 1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentMana); break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax); break;
			default:
				Lua_PushNil(L);
			}
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerStamina(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		{
			int nType = (int)Lua_ValueToNumber(L, 1);
			switch ((int)Lua_ValueToNumber(L, 1))
			{
			case 0:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentStamina); break;
			case 1:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax); break;
			case 2:
				Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax); break;
			default:
				Lua_PushNil(L);
			}
		}
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerManaReplenish(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_ManaReplenish);
}

int LuaGetPlayerDefend(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_Defend);
}

int LuaGetPlayerSex(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_nSex);
}
int LuaSetPlayerSex(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		Npc[Player[nPlayerIndex].m_nIndex].SetSex(nValue);
	}
	return 0;

}
int LuaGetPlayerIndex(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, GetPlayerIdx());
	return 0;
}

int LuaGetPlayerSeries(Lua_State* L)
{
	MacroFun_GetPlayerInfoInt(L, m_Series);
}

int LuaSetPlayerSeries(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		Npc[Player[nPlayerIndex].m_nIndex].SetSeries(nValue);
	}
	return 0;
}

int LuaGetPlayerCount(Lua_State* L)
{
	Lua_PushNumber(L, PlayerSet.GetPlayerNumber());
	return 1;
}
// LuaGetSubWorldPlayerIndexes
int LuaGetCountPlayerMax(Lua_State * L)
{
	Lua_PushNumber(L, PlayerSet.GetPlayerMaxNumber());
	return 1;
}

int LuaGetSubWorldPlayerIndexes(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nSubWorldIndex;
	if (nParamNum <= 0 || !Lua_IsNumber(L, 1))
	{
		return 0;
	}
	nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
	{
		Lua_PushNil(L);
		return 1;
	}
	
	std::vector<int> playerIndexes = SubWorld[nSubWorldIndex].GetAllPlayerIndexes();
	lua_newtable(L); // create table
	for (size_t i = 0; i < playerIndexes.size(); ++i) {
		Lua_PushNumber(L, i + 1);                // key
		Lua_PushNumber(L, playerIndexes[i]);     // value
		Lua_SetTable(L, -3);                      // table[key] = value
	}
	return 1;
}
int LuaGetSubWorldPlayerCount(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int ulCount = 0;
	int nSubWorldIndex;
	if (nParamNum <= 0)
	{
		nSubWorldIndex = GetSubWorldIndex(L);    //地图 
		if (nSubWorldIndex >= 0)
			ulCount = SubWorld[nSubWorldIndex].CountAllPlayer();
	}
	else if (Lua_IsNumber(L, 1))
	{
		nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
		if (nSubWorldIndex >= 0)
			ulCount = SubWorld[nSubWorldIndex].CountAllPlayer();
	}
	Lua_PushNumber(L, ulCount);
	return 1;
}
//LuaGetSubWorldLastPlayer
int LuaGetSubWorldLastPlayer(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	int nSubWorldIndex;
	int nCurrentPlayerIndex;
	if (nParamNum <= 0)
	{
		return 0;
	}
	else if (Lua_IsNumber(L, 1))
	{
		nSubWorldIndex = GetSubWorldIndex(L);
		nCurrentPlayerIndex = (int)Lua_ValueToNumber(L, 1);
		if (nSubWorldIndex >= 0)
			nPlayerIndex = SubWorld[nSubWorldIndex].GetLastPlayerIndex(nCurrentPlayerIndex);
	}
	Lua_PushNumber(L, nPlayerIndex);
	return 1;
}

//LuaGenNewPUBGMap
int LuaGenNewPUBGMap(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Point ptCenter;
	ptCenter.x = (int)Lua_ValueToNumber(L, 1);

	ptCenter.y = (int)Lua_ValueToNumber(L, 2);
	//map boundary for BienKinh 37
	g_MapHandler.setMapBoundary({
	{51734, 74523}, {51170, 75269}, {50667, 77483}, {49914, 78944},
	{48910, 78893}, {47711, 79458}, {46591, 76545}, {45949, 75968},
	{46302, 78655}, {44579, 82146}, {43755, 82774}, {42648, 84427},
	{41313, 86132}, {39736, 87947}, {38943, 94305}, {35842, 103657},
	{41666, 109988}, {44091, 115493}, {44686, 116864}, {49764, 119644},
	{61975, 123739}, {70676, 101694}, {71005, 95243}, {70185, 94552},
	{67108, 78410}, {62144, 78657}, {61694, 78207}
		});
	g_MapHandler.generateTrapLayers(ptCenter);
	Lua_PushNumber(L, 1);
	return 1;
}
//ForceClearStateSkillEffect
int LuaNpcForceClearStateSkillEffect(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].ForceClearStateSkillEffect();
		Lua_PushNumber(L, 1);
	}
	else
		Lua_PushNil(L);
	return 1;
}

//ForceClearStateSkillEffect with skill id
int LuaNpcForceClearStateSkillEffectId(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].ForceClearStateSkillEffect(nSkillId);
		Lua_PushNumber(L, 1);
	}
	else
		Lua_PushNil(L);
	return 1;
}

//getLayerCount
int LuaGetLayerCount(Lua_State* L)
{
	int nCount = g_MapHandler.getLayerCount();
	Lua_PushNumber(L, nCount);
	return 1;
}
//UpdatePubgCircle
int LuaUpdatePubgCircle(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		return 0;
	}
	int nSubWorldIndex;
	nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
	{
		Lua_PushNil(L);
		return 1;
	}

	std::vector<int> playerIndexes = SubWorld[nSubWorldIndex].GetAllPlayerIndexes();

	int nlayer = (int)Lua_ValueToNumber(L, 2);
	g_MapHandler.setCurrentLayer(nlayer);
	int radius = g_MapHandler.getRadius(nlayer);

	for (size_t i = 0; i < playerIndexes.size(); ++i) {
		int nPlayerIndex = playerIndexes[i];
		if (nPlayerIndex > 0 && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		{
			S2C_SEND_POINT sCommand;
			sCommand.ProtocolType = s2c_setobstacle;
			sCommand.pValue = -2; //send to client pubg circle
			sCommand.pMapX = radius;
			sCommand.pMapY = 0;
			if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sCommand, sizeof(S2C_SEND_POINT));
		}
	}
	return 0;
}
//getPointCountInLayer
int LuaGetPointCountInLayer(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nLayerIndex = (int)Lua_ValueToNumber(L, 1);
	int nCount = g_MapHandler.getPointCountInLayer(nLayerIndex);
	Lua_PushNumber(L, nCount);
	return 1;
}
//getPoint
int LuaGetPoint(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		Lua_PushNil(L);
		return 1;
	}
	int nLayerIndex = (int)Lua_ValueToNumber(L, 1);
	int nPointIndex = (int)Lua_ValueToNumber(L, 2);
	Point pt = g_MapHandler.getPoint(nLayerIndex, nPointIndex);
	if (pt.x == -1 && pt.y == -1)
	{
		Lua_PushNil(L);
	}
	else
	{
		Lua_PushNumber(L, pt.x);
		Lua_PushNumber(L, pt.y);
		return 2;
	}
	return 1;
}
//send client point to draw
int LuaSendClientPoint(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
	{
		return 0;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNil(L);
		return 0;
	}
	int X = (int)Lua_ValueToNumber(L, 1);
	int Y = (int)Lua_ValueToNumber(L, 2);
	S2C_SEND_POINT sCommand;
	sCommand.ProtocolType = s2c_setobstacle;
	sCommand.pValue = -1; //draw on minimap
	sCommand.pMapX = X;
	sCommand.pMapY = Y;
	if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sCommand, sizeof(S2C_SEND_POINT));
	return 0;
}
int LuaGetPlayerName(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushString(L, Player[nPlayerIndex].m_PlayerName);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerAccount(Lua_State* L)//edit by phong kieu ham lua viet them GetAccount
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushString(L, Player[nPlayerIndex].m_AccoutName);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerID(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_dwID);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerLeadExp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_dwLeadExp);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerLeadLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_dwLeadLevel);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerRestAttributePoint(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_nAttributePoint);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerRestSkillPoint(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaGetPlayerLucky(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{

		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurLucky); break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nLucky); break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nLucky); break;
		default:
			Lua_PushNil(L);
		}
	}
	return 1;
}

int LuaGetPlayerEngergy(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurEngergy); break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nEngergy); break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nEngergy); break;
		default:
			Lua_PushNil(L);
		}
	}

	return 1;
}

int LuaGetPlayerDexterity(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurDexterity); break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nDexterity); break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nDexterity); break;
		default:
			Lua_PushNil(L);
		}
	}
	return 1;
}

int LuaGetPlayerStrength(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurStrength); break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nStrength); break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nStrength); break;
		default:
			Lua_PushNil(L);
		}
	}
	return 1;
}

int LuaSetPlayerEngergy(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nEngergy - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseEngergy(nValue);
	}
	return 1;
}

int LuaSetPlayerDexterity(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nDexterity - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseDexterity(nValue);
	}
	return 1;
}

int LuaSetPlayerStrength(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nStrength - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseStrength(nValue);
	}
	return 1;
}

int LuaSetPlayerVitality(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nValue = (int)Lua_ValueToNumber(L, 1);
		if (nValue < 0 && Player[nPlayerIndex].m_nVitality - nValue <= 0)
			nValue = 0;
		Player[nPlayerIndex].m_nAttributePoint -= nValue;
		Player[nPlayerIndex].SetBaseVitality(nValue);
	}
	return 1;
}

int LuaGetPlayerVitality(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{

		int nType = (int)Lua_ValueToNumber(L, 1);
		switch ((int)Lua_ValueToNumber(L, 1))
		{
		case 0:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nCurVitality); break;
		case 1:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nVitality); break;
		case 2:
			Lua_PushNumber(L, Player[nPlayerIndex].m_nVitality); break;
		default:
			Lua_PushNil(L);
		}
	}
	return 1;
}

int LuaGetSaveMoney(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_repository));
	}
	else Lua_PushNumber(L, 0);

	return 1;
}

int LuaGetPlayerCashMoney(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex > 0)
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_equipment));
	}
	else Lua_PushNumber(L, 0);

	return 1;
}

int LuaPlayerPayMoney(Lua_State* L)
{

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nMoney = (int)Lua_ValueToNumber(L, 1);
		if (nMoney <= 0) return 0;
		if (Player[nPlayerIndex].Pay(nMoney))
			Lua_PushNumber(L, 1);
		else
			Lua_PushNumber(L, 0);
	}
	else
		Lua_PushNumber(L, 0);

	return 1;
}

int LuaPlayerEarnMoney(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nMoney = (int)Lua_ValueToNumber(L, 1);
		if (nMoney <= 0) return 0;
		Player[nPlayerIndex].Earn(nMoney);
	}
	return 0;
}

int LuaPlayerPrePayMoney(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nMoney = (int)Lua_ValueToNumber(L, 1);
		if (nMoney <= 0) return 0;
		if (Player[nPlayerIndex].PrePay(nMoney))
			Lua_PushNumber(L, 1);
		else
			Lua_PushNumber(L, 0);
	}
	else
		Lua_PushNumber(L, 0);

	return 1;
}

int LuaSetPlayerChatForbiddenFlag(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
		return 0;
	int nFlag = Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].SetChatForbiddenFlag(nFlag);
	return 0;
}

int LuaIsLixian(Lua_State* L)
{
	int nIndex = GetPlayerIndex(L);
	if (nIndex <= 0)
		return 0;

	if (Player[nIndex].m_nNetConnectIdx >= 0 && Player[nIndex].m_nLixian == 0 && !Npc[Player[nIndex].m_nIndex].m_FightMode) //0 khong uy thac ;1 dang uy thac; 2 ket thuc uy thac
	{
		Player[nIndex].m_nLixian = 1;
		Player[nIndex].m_uMustSave = SAVE_REQUEST;
		Player[nIndex].m_bIsQuiting = FALSE;
		Lua_PushNumber(L, Player[nIndex].m_nIndex);
		return 1;
	}
	return 0;
}

int LuaSetPlayerSaveState(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1)
			return 0;
	
		int nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIndex < 0)
			return 0;
	
		int nState = (int)Lua_ValueToNumber(L, 2);
	if (nState < 0 || nState > 2)
			return 0;
	
		Player[nPlayerIndex].SetEnablePlayerSave(nState);
	return 0;
}
int LuaKickOutPlayer(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	
	int nIndex = PlayerSet.GetFirstPlayer();
	bool bFound = false;
	while(nIndex > 0)
	{
		if (strcmp(Player[nIndex].m_PlayerName, pszName) == 0) {
			bFound = true;
			break;
		}
		
		nIndex = PlayerSet.GetNextPlayer();
	}
	if (bFound)
	{
		if (nIndex)
		{
			printf("=> GM Kick out specific player <= \n");
			Player[nIndex].m_bIsQuiting = TRUE;
			Player[nIndex].m_bForeQuit = TRUE;
			//if (Player[nIndex].m_nNetConnectIdx >= 0)
			//	g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
			Lua_PushNumber(L, Player[nIndex].m_nIndex);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaKickOutAccount(Lua_State *L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	const char*	pszName = (const char*)Lua_ValueToString(L, 1);
	
	int nIndex = PlayerSet.GetFirstPlayer();
	bool bFound = false;
	while(nIndex > 0)
	{
		if (strcmpi(Player[nIndex].m_AccoutName, pszName) == 0) {
			bFound = true;
			break;
		}
		
		nIndex = PlayerSet.GetNextPlayer();
	}
	if (bFound)
	{
		if (nIndex)
		{
			printf("=> GM Kick out specific player %s<= \n", pszName);
			Player[nIndex].m_bIsQuiting = TRUE;
			Player[nIndex].m_bForeQuit = TRUE;
			//if(Player[nIndex].m_nNetConnectIdx >= 0)
			//	g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
			Lua_PushNumber(L, Player[nIndex].m_nIndex);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaOpenTimeBox(Lua_State* L)// TimeBox
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	int nTime = 0;
	const char* szAction;
	const char* Title;

	if (nParamNum < 1)
		return 0;
	if (nParamNum > 4)
	{
		Title = Lua_ValueToString(L, 1);
		nTime = (int)Lua_ValueToNumber(L, 2);
		szAction = Lua_ValueToString(L, 3);
		char* szScript = (char*)Lua_ValueToString(L, 4);
		Player[nPlayerIndex].m_dwTimeBoxId = g_FileName2Id(szScript);
	}
	else
	{
		Title = Lua_ValueToString(L, 1);
		nTime = (int)Lua_ValueToNumber(L, 2);
		szAction = Lua_ValueToString(L, 3);
		Player[nPlayerIndex].m_dwTimeBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	}

	S2C_TIME_BOX NetCommand;
	NetCommand.ProtocolType = s2c_timebox;
	strcpy(NetCommand.Value, Title);
	NetCommand.Value1 = nTime;
	strcpy(NetCommand.Value2, szAction);
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szAction, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	if(g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx,&NetCommand,sizeof(S2C_TIME_BOX));
	
	return 0;
}

int LuaSaveNow(Lua_State* L)
{
	int nIndex = GetPlayerIndex(L);

	if (nIndex <= 0)
		return 0;

	if (Player[nIndex].CanSave())
	{
		BOOL result = Player[nIndex].Save();
		if (result)
		{
			Player[nIndex].m_uMustSave = SAVE_REQUEST;
			printf("  ===> Da luu thong tin nhan vat Index %d .\n", nIndex);
			return 1;
		}

		return 0;
	}
	return 0;
}

int LuaKickOutSelf(Lua_State* L)
{
	int nIndex = GetPlayerIndex(L);

	if (nIndex <= 0)
		return 0;

	if (Player[nIndex].m_nNetConnectIdx >= 0)
	{
		printf("=> GM Kick out player one <= \n");
	//	Player[nIndex].m_bIsQuiting = TRUE;
		g_pServer->ShutdownClient(Player[nIndex].m_nNetConnectIdx);
		Lua_PushNumber(L, 1);
		return 1;
	}
	return 0;
}

int LuaKickOutSelf2(Lua_State* L)
{
	int nIndex = GetPlayerIndex(L);

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum >= 1)
		nIndex = (int)Lua_ValueToNumber(L, 1);

	if (nIndex <= 0)
		return 0;

	BYTE	NetCommand = (BYTE)s2c_exitgame;
	g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 1;
}

int LuaSetFightState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].SetFightMode(Lua_ValueToNumber(L, 1) != 0);
	Player[nPlayerIndex].SetLastNetOperationTime(g_SubWorldSet.GetGameTime());
	return 0;
}

int LuaGetFightState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_FightMode);
	return 1;
}

int LuaGetLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	if (Player[nPlayerIndex].m_nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	return 1;
}

int LuaGetKhoa(Lua_State* L)
{
	int nKhoaValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerkhoavalue;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerkhoavalue;
	nKhoaValue = Player[nPlayerIndex].m_CUnlocked;

lab_getplayerkhoavalue:
	Lua_PushNumber(L, nKhoaValue);
	return 1;
}

int LuaSetPKState(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);

	int nPlayerIndex;
	BYTE nPKFlag;
	BOOL nbLockPK;

	if (nParamNum < 3)
	{
		nPlayerIndex = GetPlayerIndex(L);
		nPKFlag = (BYTE)Lua_ValueToNumber(L, 1);
		nbLockPK = (BOOL)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
		nPKFlag = (BYTE)Lua_ValueToNumber(L, 2);
		nbLockPK = (BOOL)Lua_ValueToNumber(L, 3);
	}

	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cPK.SetNormalPKState(nPKFlag, nbLockPK);
	return 0;
}

int LuaGetPKState(Lua_State* L)
{
	int nPKValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerpkvalue;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerpkvalue;
	nPKValue = Player[nPlayerIndex].m_cPK.GetNormalPKState();

lab_getplayerpkvalue:
	Lua_PushNumber(L, nPKValue);
	return 1;
}

int	LuaUseTownPortal(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Player[nPlayerIndex].UseTownPortal();
	return 0;
}

int LuaReturnFromTownPortal(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	Player[nPlayerIndex].BackToTownPortal();
	return 0;
}
#endif

int GetPlayerIndex(Lua_State* L)
{
	Lua_GetGlobal(L, SCRIPT_PLAYERINDEX);
	if (lua_isnil(L, Lua_GetTopIndex(L)))
		return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nIndex >= MAX_PLAYER || nIndex <= 0)
	{
		//_ASSERT(0);
		return -1;
	}
	if (Player[nIndex].m_nIndex >= MAX_NPC || Player[nIndex].m_nIndex < 0)
	{
		//_ASSERT(0);
		return -1;
	}
	return nIndex;
}

int GetObjIndex(Lua_State* L)
{
	Lua_GetGlobal(L, SCRIPT_OBJINDEX);
	if (lua_isnil(L, Lua_GetTopIndex(L)))
		return -1;
	int nIndex = (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L));
	if (nIndex >= MAX_OBJECT || nIndex <= 0)
	{
		_ASSERT(0);
		return -1;
	}
	if (Object[nIndex].m_nIndex != nIndex)
	{
		_ASSERT(0);
		return -1;
	}
	return nIndex;
}

int  LuaMessage(Lua_State* L)
{
	const char* szString;
	szString = lua_tostring(L, 1);
	g_DebugLog((char*)szString);
	return 0;
}

#ifdef _SERVER
int LuaAddPlayerWayPoint(Lua_State* L)//AddStation(N)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nWayPoint = (int)Lua_ValueToNumber(L, 1);
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	{
		KIndexNode* pNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerWayPointList.GetHead();
		while (pNode)
		{
			if (pNode->m_nIndex == nWayPoint) return 0;
			pNode = (KIndexNode*)pNode->GetNext();
		}

		KIndexNode* pNewNode = new KIndexNode;
		pNewNode->m_nIndex = nWayPoint;
		int nCount = Player[nPlayerIndex].m_PlayerWayPointList.GetNodeCount();
		for (int i = 0; i < nCount - 2; i++)
		{
			KIndexNode* pDelNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerWayPointList.RemoveHead();
			delete pDelNode;
		}
		Player[nPlayerIndex].m_PlayerWayPointList.AddTail(pNewNode);
	}
	return 0;
}

int LuaAddPlayerStation(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nStation = (int)Lua_ValueToNumber(L, 1);
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	{
		KIndexNode* pNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerStationList.GetHead();
		while (pNode)
		{
			if (pNode->m_nIndex == nStation) return 0;
			pNode = (KIndexNode*)pNode->GetNext();
		}

		KIndexNode* pNewNode = new KIndexNode;
		pNewNode->m_nIndex = nStation;
		Player[nPlayerIndex].m_PlayerStationList.AddTail(pNewNode);
	}
	return 0;
}

int LuaGetPlayerStationCount(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Player[nPlayerIndex].m_PlayerStationList.GetNodeCount());
	return 1;
}

int LuaGetPlayerStation(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nStationId = 0;
	KIndexNode* pNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerStationList.GetHead();
	if (pNode)
	{
		int nNo = (int)Lua_ValueToNumber(L, 1);
		int nCurStation = (int)Lua_ValueToNumber(L, 2);
		int nVisitNo = 0;
		while (pNode)
		{
			if (pNode->m_nIndex != nCurStation && g_GetPriceToStation(nCurStation, pNode->m_nIndex) > 0)
			{
				nVisitNo++;
				if (nVisitNo == nNo)
				{
					nStationId = pNode->m_nIndex;
					break;
				}
			}
			pNode = (KIndexNode*)pNode->GetNext();
		}
	}
	Lua_PushNumber(L, nStationId);
	return 1;
}

int LuaGetPlayerWayPoint(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	KIndexNode* pNode = (KIndexNode*)Player[nPlayerIndex].m_PlayerWayPointList.GetHead();
	if (pNode)
	{
		int nNo = (int)Lua_ValueToNumber(L, 1);
		if (nNo > TASKVALUE_MAXWAYPOINT_COUNT)
			Lua_PushNumber(L, 0);
		else
		{
			for (int i = 0; i < nNo - 1; i++)
			{
				if (pNode == NULL)
					break;
				pNode = (KIndexNode*)pNode->GetNext();
			}

			if (pNode)
				Lua_PushNumber(L, pNode->m_nIndex);
			else
				Lua_PushNumber(L, 0);
		}
	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetStationName(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) <= 0)
	{
		Lua_PushString(L, "");
		return 1;
	}
	int nStationId = (int)Lua_ValueToNumber(L, 1);
	char szName[50];
	g_StationTabFile.GetString(nStationId + 1, "DESC", "无名城", szName, 50);
	Lua_PushString(L, szName);
	return 1;
}

int LuaGetWayPointName(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	if (Lua_GetTopIndex(L) <= 0)
	{
		Lua_PushString(L, "");
		return 1;
	}
	int nWayPointId = (int)Lua_ValueToNumber(L, 1);
	char szName[50];
	g_WayPointTabFile.GetString(nWayPointId + 1, "DESC", "未记录", szName, 50);
	Lua_PushString(L, szName);
	return 1;
}

int LuaGetAllStationCount(Lua_State* L)//GetCityCount 
{
	int nCityCount = g_StationTabFile.GetHeight() - 1;
	if (nCityCount < 0) nCityCount = 0;
	Lua_PushNumber(L, nCityCount);
	return 1;
}

int LuaGetCity(Lua_State* L)//cityid, price = GetCity(citynum, curcity)
{
	return 0;
}

int LuaOpenResetPass(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	BYTE	NetCommand = (BYTE)s2c_openresetpass;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 0;
}

int LuaDaTauBox(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	BYTE	NetCommand = (BYTE)s2c_opendataubox;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 0;
}

int LuaDaTau1Box(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	BYTE	NetCommand = (BYTE)s2c_opendatau1box;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 0;
}

int LuaOpenTrembleItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	Player[nPlayerIndex].m_dwTrembleItemId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;

	OPEN_TREMBLEITEM	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_opentrembleitem;
	NetCommand.m_nType = 1;
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, "TrembleItem", sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(OPEN_TREMBLEITEM));
	return 0;
}

int LuaEndTrembleItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	Player[nPlayerIndex].m_dwTrembleItemId = 0;

	OPEN_TREMBLEITEM	NetCommand;
	NetCommand.ProtocolType = (BYTE)s2c_opentrembleitem;
	NetCommand.m_nType = 0;
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, "", sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(OPEN_TREMBLEITEM));
	return 0;
}

int LuaOpenCompoundItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	BYTE	NetCommand = (BYTE)s2c_opencompounditem;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(BYTE));
	return 0;
}

int LuaGetIdItem(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nPos = 0;
	int nSocket = -1;

	nPos = (int)Lua_ValueToNumber(L, 1);
	if (Lua_GetTopIndex(L) >= 2)
	{
		nSocket = (int)Lua_ValueToNumber(L, 2);
	}
	if (nPos > 9)
		return 0;

	switch (nPos)
	{
	case 1:
		if (nSocket > 11)
		{
			nSocket = 11;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetEquipment(nSocket));
		break;
	case 2:
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetGiveItem());
		break;
	case 3:
		if (nSocket > 7)
		{
			nSocket = 7;
		}
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetTrembleItem(nSocket));
		break;
	case 4:
		if (nSocket > 3)
		{
			nSocket = 3;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetCompOneItem(nSocket));
		break;
	case 5:
		if (nSocket > 3)
		{
			nSocket = 3;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetCompTwoItem(nSocket)); 
		break;
	case 6:
		if (nSocket > 3)
		{
			nSocket = 3;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetCompThreeItem(nSocket)); 
		break;
	case 7:
		if (nSocket > 11)
		{
			nSocket = 11;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetDistillItem(nSocket)); 
		break;
	case 8:
		if (nSocket > 2)
		{
			nSocket = 2;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetForgeItem(nSocket)); 
		break;
	case 9:
		if (nSocket > 11)
		{
			nSocket = 11;
		}
		//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.GetEnchaseItem(nSocket)); 
		break;
	default:
		break;
	}

	return 1;
}

int LuaSetLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)    return 0;

	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int nValue = (int)Lua_ValueToNumber(L, 1);

	if (nValue < 1)
		return 0;

	Npc[Player[nPlayerIndex].m_nIndex].m_Level = 1;	           //等级为1级

	Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);	   //返回原来的剩余的技能点
	Lua_PushNumber(L, Player[nPlayerIndex].m_nAttributePoint); //返回原来的属性点 

	Player[nPlayerIndex].m_nSkillPoint = 0;//Player[nPlayerIndex].m_cReBorn.GetReBornKeepJpiont();		//技能点归零
	Player[nPlayerIndex].m_nAttributePoint = 0;//Player[nPlayerIndex].m_cReBorn.GetReBornKeepQpiont();	    //属性点归零

	if (Npc[Player[nPlayerIndex].m_nIndex].m_Series == 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax = 204;
		Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax = 16;

	}
	if (Npc[Player[nPlayerIndex].m_nIndex].m_Series == 1)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax = 103;
		Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax = 77;

	}

	if (Npc[Player[nPlayerIndex].m_nIndex].m_Series == 2)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax = 153;
		Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax = 77;

	}
	if (Npc[Player[nPlayerIndex].m_nIndex].m_Series == 3)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax = 213;
		Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax = 41;

	}
	if (Npc[Player[nPlayerIndex].m_nIndex].m_Series == 4)
	{
		Npc[Player[nPlayerIndex].m_nIndex].m_LifeMax = 76;
		Npc[Player[nPlayerIndex].m_nIndex].m_ManaMax = 163;

	}
	Npc[Player[nPlayerIndex].m_nIndex].m_StaminaMax = 100;

	for (int chay = 1; chay <= nValue - 1; chay++)
	{//循环升级
		Player[nPlayerIndex].LevelUp();
	}

	return 2;
}

// 重置基本属性
// ResetBaseAttribute(type,data);
int LuaResetBaseAttribute(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 2) return 0;

	if (nPlayerIndex > 0)
	{
		PLAYER_ADD_BASE_ATTRIBUTE_COMMAND cmd;
		cmd.m_btAttribute = (int)Lua_ValueToNumber(L, 1);;
		cmd.m_nAddNo = (int)Lua_ValueToNumber(L, 2);;
		cmd.ProtocolType = c2s_playeraddbaseattribute;
		Player[nPlayerIndex].ResetBaseAttribute((BYTE*)&cmd);
	}

	return 1;
}

int LuaSetBasePoint(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4)
		return 1;
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		return 1;
	if (Lua_IsNumber(L, 1) &&
		Lua_IsNumber(L, 2) &&
		Lua_IsNumber(L, 3) &&
		Lua_IsNumber(L, 4))
	{
		Player[nPlayerIndex].m_nStrength = Lua_ValueToNumber(L, 1);
		Player[nPlayerIndex].m_nDexterity = Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].m_nVitality = Lua_ValueToNumber(L, 2);
		Player[nPlayerIndex].m_nEngergy = Lua_ValueToNumber(L, 4);
		Player[nPlayerIndex].m_nAttributePoint = (Npc[Player[nPlayerIndex].m_nIndex].m_Level - 1) * 5 + Player[nPlayerIndex].m_cReBorn.GetReBornKeepQpiont();
	}

	return 1;
}

int LuaChangeInfoItem(Lua_State* L)
{
	if (Lua_GetTopIndex(L) <= 0)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nType = 0;
	int nIndex = 0;

	nType = (int)Lua_ValueToNumber(L, 1);
	nIndex = (int)Lua_ValueToNumber(L, 2);

	switch (nType)
	{
	case 1:
	{
		int nLevel = 0;
		nLevel = (int)Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].m_ItemList.SetLevelItem(nIndex, nLevel);
	}
	break;
	case 2:
	{
		int nSeries = 0;
		nSeries = (int)Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].m_ItemList.SetSeriesItem(nIndex, nSeries);
	}
	break;
	case 3:
	{
		int nLevelMagic = 0;
		nLevelMagic = (int)Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].m_ItemList.ChangeSpiritItem(nIndex, nLevelMagic);
	}
	break;
	case 4:
	{
		int nType = 0;
		int nOption = 0;
		int nLevel = 0;
		nType = (int)Lua_ValueToNumber(L, 3);
		nOption = (int)Lua_ValueToNumber(L, 4);
		nLevel = (int)Lua_ValueToNumber(L, 5);
		Player[nPlayerIndex].m_ItemList.SetMagic2Item(nIndex, nType, nOption, nLevel);
	}
	break;
	case 5:
	{
		int nPoint = 0;
		nPoint = (int)Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].m_ItemList.SetPointPurpleItem(nIndex, nPoint);
	}
	break;
	default:
		break;
	}

	return 1;

}

int LuaOpenQuestFinish(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;

	QUEST_FINISH_DLG_SYNC FinishSync;
	FinishSync.ProtocolType = (BYTE)s2c_openquestfinishdlg;
	const char* szNotice = (char*)Lua_ValueToString(L, 1);
	if (sizeof(szNotice) > sizeof(FinishSync.m_szNotice))
		return 0;
	strcpy(FinishSync.m_szNotice, szNotice);
	FinishSync.m_bType = (int)Lua_ValueToNumber(L, 2);
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &FinishSync, sizeof(QUEST_FINISH_DLG_SYNC));

	if (FinishSync.m_bType <= 4)
		Player[nPlayerIndex].m_dwRewardId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	else
		Player[nPlayerIndex].m_dwRewardExId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;

	return 1;
}

int LuaGetPriceToWayPoint(Lua_State* L)
{
	int nCurStation = (int)Lua_ValueToNumber(L, 1);
	int nDesWayPoint = (int)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, g_GetPriceToWayPoint(nCurStation, nDesWayPoint));
	return 1;
}

int LuaGetPriceToStation(Lua_State* L)
{
	int nCurStation = (int)Lua_ValueToNumber(L, 1);
	int nNextStation = (int)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, g_GetPriceToStation(nCurStation, nNextStation));
	return 1;
}

int LuaGetStationPos(Lua_State* L)
{
	int nStationId = (int)Lua_ValueToNumber(L, 1);
	char szPos[100];
	int nCount = 0;
	int nRow = g_StationTabFile.FindColumn("COUNT");
	g_StationTabFile.GetInteger(nStationId + 1, nRow, 0, &nCount);
	if (nCount <= 0) return 0;
	int nRandSect = g_Random(100) % nCount + 1;
	char szSectName[32];
	sprintf(szSectName, "SECT%d", nRandSect);
	char szValue[100];
	nRow = g_StationTabFile.FindColumn(szSectName);
	g_StationTabFile.GetString(nStationId + 1, nRow, "0,0,0", szValue, 100);

	int nX, nY, nWorld;
	const char* pcszTemp = szValue;

	nWorld = KSG_StringGetInt(&pcszTemp, 0);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	nX = KSG_StringGetInt(&pcszTemp, 0);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);

	Lua_PushNumber(L, nWorld);
	Lua_PushNumber(L, nX);
	Lua_PushNumber(L, nY);
	return 3;
}

int LuaGetWayPointPos(Lua_State* L)
{
	int nWayPointId = (int)Lua_ValueToNumber(L, 1);
	char szPos[100];
	int nCount = 0;
	char szValue[30];
	int nRow;
	nRow = g_WayPointTabFile.FindColumn("SECT");
	g_WayPointTabFile.GetString(nWayPointId + 1, nRow, "0,0,0", szValue, 30);
	int nX, nY, nWorld;
	const char* pcszTemp = szValue;

	nWorld = KSG_StringGetInt(&pcszTemp, 0);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	nX = KSG_StringGetInt(&pcszTemp, 0);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);

	Lua_PushNumber(L, nWorld);
	Lua_PushNumber(L, nX);
	Lua_PushNumber(L, nY);
	return 3;
}

int LuaOpenGetString(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 3)
		return 0;

	const char * szTitle;
	const char * szAction;
	const char * szScript;

	if (nParamNum >= 4)
	{
		szTitle	= Lua_ValueToString(L, 1);
		szAction = Lua_ValueToString(L, 3);
		char * szScript = (char *)Lua_ValueToString(L, 2);
		Player[nPlayerIndex].m_dwStrBoxId = g_FileName2Id(szScript);
	}
	else
	{
		szTitle	= Lua_ValueToString(L, 1);
		szAction = Lua_ValueToString(L, 2);
		Player[nPlayerIndex].m_dwStrBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	}
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szAction, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	S2C_INPUT_BOX NetCommand;
	NetCommand.ProtocolType = s2c_inputbox;
	NetCommand.nType = 1;
	strcpy(NetCommand.Value, szTitle);
	strcpy(NetCommand.Value1, szAction);
	if(g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx,&NetCommand,sizeof(S2C_INPUT_BOX));
	
	return 0;
}

int LuaOpenGetNumber(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 3)
		return 0;

	const char * szTitle;
	const char * szAction;
	const char * szScript;

	if (nParamNum >= 4)
	{
		szTitle	= Lua_ValueToString(L, 1);
		szAction = Lua_ValueToString(L, 3);
		char * szScript = (char *)Lua_ValueToString(L, 2);
		Player[nPlayerIndex].m_dwNumberBoxId = g_FileName2Id(szScript);
	}
	else
	{
		szTitle	= Lua_ValueToString(L, 1);
		szAction = Lua_ValueToString(L, 2);
		Player[nPlayerIndex].m_dwNumberBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	}
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szAction, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	S2C_INPUT_BOX NetCommand;
	NetCommand.ProtocolType = s2c_inputbox;
	NetCommand.nType = 2;
	strcpy(NetCommand.Value, szTitle);
	strcpy(NetCommand.Value1, szAction);
	if(g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx,&NetCommand,sizeof(S2C_INPUT_BOX));
	
	return 0;
}

int LuaGetStringFromUI(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Lua_PushString(L, Player[nPlayerIndex].szStringInput);
	return 1;
}

int LuaGetNumberFromUI(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Lua_PushNumber(L, Player[nPlayerIndex].m_nStringNum);
	return 1;
}

int LuaGetWordRank(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Player[nPlayerIndex].m_nWorldStat);
	return 1;
}

int LuaGetRank(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_btRankId);
	return 1;
}

int LuaGetPlayerTitle(Lua_State* L) //#PlayerTitle
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_btPlayerTitle);
	return 1;
}

int LuaGetRankBattle(Lua_State* L) //#RankBattle
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_btRankBattleId);
	return 1;
}

int LuaSetRank(Lua_State* L)
{
	BYTE btRank = (BYTE)Lua_ValueToNumber(L, 1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_btRankId = btRank;
	return 0;
}

int LuaSetPlayerTitle(Lua_State* L) //#PlayerTitle
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum <= 0) return 0;

	DWORD btRank;
	DWORD nTime;
	DWORD overlook;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;

	if (nParamNum < 3)
	{
		return 0;
	}
	else
	{
		btRank = (DWORD)Lua_ValueToNumber(L, 1);
		nTime = (DWORD)Lua_ValueToNumber(L, 2);
		overlook = (DWORD)Lua_ValueToNumber(L, 3);
	}

	Npc[Player[nPlayerIndex].m_nIndex].SetPlayerTitle(btRank, nTime, overlook);
	return 0;
}

int LuaRemovePlayerTitle(Lua_State* L) //#PlayerTitle
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	DWORD btRank = Npc[Player[nPlayerIndex].m_nIndex].m_btPlayerTitle;
	if (btRank > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetPlayerTitle(btRank, 1, 1); //#nTime = 1 and OverLook = 1 delete
	}
	return 0;
}

int LuaSetCurPlayerTitle(Lua_State* L) //#PlayerTitle
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum <= 0) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	DWORD btRank = (DWORD)Lua_ValueToNumber(L, 1);
	Npc[Player[nPlayerIndex].m_nIndex].SetCurPlayerTitle(btRank, 0);
	return 1;
}

int LuaSetRankBattle(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum <= 0) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	DWORD btRank;
	int nTime;
	int overlook;
	if (nParamNum < 3)
	{
		return 0;
	}
	else
	{
		btRank = (DWORD)Lua_ValueToNumber(L, 1);
		nTime = (int)Lua_ValueToNumber(L, 2);
		overlook = (int)Lua_ValueToNumber(L, 3);
	}
	Npc[Player[nPlayerIndex].m_nIndex].SetRankBattle(btRank, nTime, overlook);
	return 0;
}

int LuaRemoveRankBattle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	DWORD btRank = Npc[Player[nPlayerIndex].m_nIndex].m_btRankBattleId;
	if (btRank > 0)
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetRankBattle(btRank, 0, 1);
		return 1;
	}
	return 0;
}

int LuaGetExItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ExItemId);
	return 1;
}

int LuaSetExItem(Lua_State* L)
{
	BYTE ExItem = (BYTE)Lua_ValueToNumber(L, 1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_ExItemId = ExItem;
	return 0;
}

int LuaGetExBox(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_ExBoxId);
	return 1;
}

int LuaSetExBox(Lua_State* L)
{
	BYTE ExBox = (BYTE)Lua_ValueToNumber(L, 1);
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	Npc[Player[nPlayerIndex].m_nIndex].m_ExBoxId = ExBox;
	return 0;
}

int LuaSetObjPropState(Lua_State* L)
{
	int  nParamNum = (int)Lua_GetTopIndex(L);
	int nState = 1;

	if (nParamNum >= 1)
	{
		nState = (int)Lua_ValueToNumber(L, 1);
		nState = (nState == 0) ? 0 : 1;
	}

	int nIndex = 0;
	if ((nIndex = GetObjIndex(L)) < 0)
		return 0;

	Object[nIndex].SetState(nState);
	return 0;
}


int	LuaGetServerName(Lua_State* L)
{
	char szServerName[100];
	unsigned long   stServerNameLen = 100;

#ifndef __linux
	if (GetComputerName(szServerName, &stServerNameLen))
	{
		Lua_PushString(L, szServerName);
	}
	else
#else
	if (SOCKET_ERROR != gethostname(szServerName, sizeof(szServerName)))
	{
		Lua_PushString(L, szServerName);
	}
	else
#endif
		Lua_PushString(L, "");

	return 1;
}

int LuaGetDockCount(Lua_State* L)//GetWharfCount(nDock)
{
	int nCount = 0;
	int nCurStation = 0;
	int nTotalCount = 0;
	int i = 0;
	if (Lua_GetTopIndex(L) < 1)
	{
		goto DockCount;
	}

	nCurStation = (int)Lua_ValueToNumber(L, 1);
	nTotalCount = g_DockPriceTabFile.GetHeight() - 1;

	for (i = 0; i < nTotalCount; i++)
	{
		int nPrice = g_GetPriceToDock(nCurStation, i + 1);
		if (nPrice > 0) nCount++;
	}

DockCount:
	Lua_PushNumber(L, nCount);
	return 1;
}

int LuaGetDockPrice(Lua_State* L)
{
	int nCurDock = (int)Lua_ValueToNumber(L, 1);
	int nDesDock = (int)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, g_GetPriceToDock(nCurDock, nDesDock));
	return 1;
}

int LuaGetDock(Lua_State* L)
{
	int nCurDock = (int)Lua_ValueToNumber(L, 1);
	int nDock = (int)Lua_ValueToNumber(L, 2);
	int nCount = 0;
	int nTotalCount = g_DockPriceTabFile.GetHeight() - 1;
	int nGetDock = 0;

	for (int i = 0; i < nTotalCount; i++)
	{
		int nPrice = g_GetPriceToDock(nCurDock, i + 1);
		if (nPrice > 0)
		{
			nCount++;
			if (nCount == nDock)
			{
				nGetDock = i + 1;
				break;
			}
		}
	}
	Lua_PushNumber(L, nGetDock);
	return 1;
}

int LuaGetDockName(Lua_State* L)
{
	int nDock = (int)Lua_ValueToNumber(L, 1);
	char szName[100];

	if (nDock > g_DockPriceTabFile.GetHeight() - 1)
	{
		strcpy(szName, "未知码头");
		goto DockName;
	}

	g_DockTabFile.GetString(nDock + 1, "DESC", "未知码头", szName, 100);

DockName:
	Lua_PushString(L, szName);
	return 1;
}

int LuaGetDockPos(Lua_State* L)
{
	int nDock = (int)Lua_ValueToNumber(L, 1);
	if (nDock > g_DockTabFile.GetHeight() - 1)
	{
		printf("GetWharfPos Script Is Error!");
		return 0;
	}

	char szPos[100];
	int nCount = 0;
	int nRow = g_DockTabFile.FindColumn("COUNT");
	g_DockTabFile.GetInteger(nDock + 1, nRow, 0, &nCount);
	if (nCount <= 0) return 0;
	int nRandSect = g_Random(100) % nCount + 1;
	char szSectName[32];
	sprintf(szSectName, "SECT%d", nRandSect);
	char szValue[100];
	nRow = g_DockTabFile.FindColumn(szSectName);
	g_DockTabFile.GetString(nDock + 1, nRow, "0,0,0", szValue, 100);
	int nX, nY, nWorld;
	const char* pcszTemp = szValue;

	nWorld = KSG_StringGetInt(&pcszTemp, 0);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	nX = KSG_StringGetInt(&pcszTemp, 0);
	KSG_StringSkipSymbol(&pcszTemp, ',');
	nY = KSG_StringGetInt(&pcszTemp, 0);
	//sscanf(szValue, "%d,%d,%d", &nWorld, &nX, &nY);

	Lua_PushNumber(L, nWorld);
	Lua_PushNumber(L, nX);
	Lua_PushNumber(L, nY);
	return 3;
}


int LuaGetWayPointFight(Lua_State* L)
{
	int nWayPointId = (int)Lua_ValueToNumber(L, 1);
	int nFight;
	int nRow;
	nRow = g_WayPointTabFile.FindColumn("FightState");
	g_WayPointTabFile.GetInteger(nWayPointId + 1, nRow, 0, &nFight);

	Lua_PushNumber(L, nFight);
	return 1;
}


int LuaGetWayPointFightState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) return 0;
	int nFightState = 0;
	if (Lua_GetTopIndex(L) > 0)
	{
		int nWayPointId = (int)Lua_ValueToNumber(L, 1);
		g_WayPointTabFile.GetInteger(nWayPointId + 1, "FightState", 0, &nFightState);
	}
	Lua_PushNumber(L, nFightState);
	return 1;
}

int LuaSetMission(Lua_State* L)// SetMissionValue(valueid, value)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;

	int nValueId = (int)Lua_ValueToNumber(L, 1);
	char* szValue = (char*)Lua_ValueToString(L, 2);

	if (nValueId < 0)
		return 0;

	SubWorld[nSubWorldIndex].m_MissionArray.SetMission(nValueId, szValue);
	return 0;
}

int LuaGetMissionString(Lua_State* L)
{
	int nSubWorldIndex = -1;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		goto lab_getmissionstring;

	nSubWorldIndex = GetSubWorldIndex(L);

	if (nSubWorldIndex >= 0)
	{
		int  nValueId = (int)Lua_ValueToNumber(L, 1);
		if (nValueId >= 0)
		{
			Lua_PushString(L, SubWorld[nSubWorldIndex].m_MissionArray.GetMissionString(nValueId));
			return 1;
		}
	}

lab_getmissionstring:
	Lua_PushNil(L);
	return 1;
}

int LuaGetMissionValue(Lua_State* L)
{
	int nResultValue = 0;
	int nSubWorldIndex = -1;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		goto lab_getmissionvalue;

	nSubWorldIndex = GetSubWorldIndex(L);

	if (nSubWorldIndex >= 0)
	{
		int  nValueId = (int)Lua_ValueToNumber(L, 1);
		if (nValueId > 0)
			nResultValue = SubWorld[nSubWorldIndex].m_MissionArray.GetMissionValue(nValueId);
	}

lab_getmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}
int LuaSetGlobalMissionValue(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;

	int nValueId = (int)Lua_ValueToNumber(L, 1);
	int  szValue = (int)Lua_ValueToNumber(L, 2);

	if (nValueId < 0)
		return 0;
	g_GlobalMissionArray.SetMissionValue(nValueId, szValue);
	return 0;
}
int LuaSetGlobalMission(Lua_State* L)// SetMissionValue(mapid/mapname, valueid, value)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;

	int nValueId = (int)Lua_ValueToNumber(L, 1);
	char* szValue = (char*)Lua_ValueToString(L, 2);

	if (nValueId < 0)
		return 0;
	g_GlobalMissionArray.SetMission(nValueId, szValue);
	return 0;
}

int LuaGetGlobalMissionValue(Lua_State* L)
{
	int nResultValue = 0;
	int nValueId = 0;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		goto lab_getglobalmissionvalue;
	nValueId = (int)Lua_ValueToNumber(L, 1);
	if (nValueId < 0)
		goto lab_getglobalmissionvalue;

	nResultValue = g_GlobalMissionArray.GetMissionValue(nValueId);

lab_getglobalmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}
int LuaGetGlobalMissionValueC(Lua_State* L)
{
	int nResultValue = 0;
	int nValueId = 0;
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		goto lab_getglobalmissionvalue;
	nValueId = (int)Lua_ValueToNumber(L, 1);
	if (nValueId < 0)
		goto lab_getglobalmissionvalue;

	nResultValue = g_GlobalMissionArray.GetMissionValueC(nValueId);

lab_getglobalmissionvalue:
	Lua_PushNumber(L, nResultValue);
	return 1;
}

int LuaInitMission(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		_ASSERT(0);
		return 0;
	}

	pMission = SubWorld[nSubWorldIndex].m_MissionArray.Add();
	if (pMission)
	{
		pMission->m_MissionPlayer.Clear();
		pMission->SetMissionId(nMissionId);
		if (Lua_GetTopIndex(L) > 4) //add by phong ki襲 using t鑞g kim
		{
			int nParam[MAX_GLBMISSION_PARAM];
			for (int i = 0; i < MAX_GLBMISSION_PARAM; i++)
			{
				nParam[i] = (int)Lua_ValueToNumber(L, 4 + i);
			}
			pMission->SetMissionLadder((char*)Lua_ValueToString(L, 2), (int)Lua_ValueToNumber(L, 3), nParam);
		}
		char szScript[MAX_PATH];
		g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
		if (szScript[0])
		{
			pMission->ExecuteScript(szScript, "BeginMission", 0);
		}
	}

	return 0;
}

int LuaRunMission(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		char szScript[MAX_PATH];
		sprintf(szScript, "\\script\\misions\\mision%02d.lua", nMissionId);
		if (szScript[0])
		{
			KLuaScript* pScript = (KLuaScript*)g_GetScript(szScript);
			Lua_PushNumber(pScript->m_LuaState, nSubWorldIndex);
			pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
			pScript->CallFunction("RunMission", 0, "d", nMissionId);
			//pMission->ExecuteScript(szScript,g_FileName2Id(szScript), "RunMission", 0);
		}
	}

	return 0;
}

int LuaGetMissionName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);

	if (pMission)
	{
		Lua_PushString(L, pMission->GetMissionName());
		return 1;
	}
	return 0;
}


int LuaReLoadScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	char* szScript = (char*)Lua_ValueToString(L, 1);
	ReLoadScript(szScript);
	return 0;
}

int LuaCloseMission(Lua_State* L)//CloseMission(missionId)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	//
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;
	//
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;
	//
	KMission StopMission;
	StopMission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&StopMission);
	if (pMission)
	{
		char szScript[MAX_PATH];
		g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
		if (szScript[0])
		{
			pMission->ExecuteScript(szScript, "EndMission", 0);
		}
		pMission->StopMission();
		SubWorld[nSubWorldIndex].m_MissionArray.Remove(pMission);
	}
	return 0;
}

int LuaStopMissionTimer(Lua_State* L)//StopMissionTimer(missionid, timerid)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nTimerId = (int)Lua_ValueToNumber(L, 2);
	int nSubWorldIndex = GetSubWorldIndex(L);

	if (nMissionId < 0 || nTimerId < 0)
		return 0;

	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			KTimerTaskFun StopTimer;
			StopTimer.SetTimer(1, nTimerId);
			KTimerTaskFun* pTimer = pMission->m_cTimerTaskSet.GetData(&StopTimer);
			if (pTimer)
			{
				pTimer->CloseTimer();
				pMission->m_cTimerTaskSet.Remove(pTimer);
			}
		}
	}
	return 0;
}

int LuaStartMissionTimer(Lua_State* L)//StartMissionTimer(missionid, timerid, time)
{
	if (Lua_GetTopIndex(L) < 3)
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nTimerId = (int)Lua_ValueToNumber(L, 2);
	int nTimeInterval = (int)Lua_ValueToNumber(L, 3);
	int nSubWorldIndex = GetSubWorldIndex(L);

	if (nMissionId < 0 || nTimerId < 0 || nTimeInterval < 0)
		return 0;

	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			KTimerTaskFun* pTimer = pMission->m_cTimerTaskSet.Add();
			if (pTimer)
			{
				pTimer->SetTimer(nTimeInterval, nTimerId);
			}
		}

	}
	return 0;
}

int LuaSetDeathRevivalPos(Lua_State* L)//SetTempRev(worldid, x, y) khi ch誸 s?quay v?
{
	int nPlayerIndex = GetPlayerIndex(L);
	
	if (nPlayerIndex < 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	
	PLAYER_REVIVAL_POS * pTempRev = Player[nPlayerIndex].GetDeathRevivalPos();
	
	if (nParamCount > 2)
	{
		pTempRev->m_nSubWorldID  = (int) Lua_ValueToNumber(L, 1);
		pTempRev->m_nMpsX = (int) Lua_ValueToNumber(L, 2);
		pTempRev->m_nMpsY = (int) Lua_ValueToNumber(L, 3); 
	}
	else if (nParamCount == 1)
	{
		pTempRev->m_nSubWorldID = SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
		POINT Pos;
		int nRevId = (int) Lua_ValueToNumber(L, 1);
		if(g_SubWorldSet.GetRevivalPosFromId(pTempRev->m_nSubWorldID, nRevId, &Pos))
		{
			pTempRev->m_ReviveID = nRevId;
			pTempRev->m_nMpsX = Pos.x;
			pTempRev->m_nMpsY = Pos.y;
		}
		else
		{
			pTempRev->m_nSubWorldID  = 53;
			pTempRev->m_nMpsX = 52032;
			pTempRev->m_nMpsY = 101696; 
		}
	}
	else 
	{
		return 0;
	}
	
	return 0;
}

int LuaAddMissionPlayer(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;
	int nMissionId = 0;
	int nPlayerIndex = 0;
	int nGroupId = 0;
	if (nParamCount >= 3)
	{
		nMissionId = (int)Lua_ValueToNumber(L, 1);
		nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
		nGroupId = (int)Lua_ValueToNumber(L, 3);
	}
	else
	{
		nMissionId = (int)Lua_ValueToNumber(L, 1);
		nGroupId = (int)Lua_ValueToNumber(L, 2);
		nPlayerIndex = GetPlayerIndex(L);
	}

	if (nMissionId < 0 || nPlayerIndex <= 0 || nGroupId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			int nPlayerDataIdx = pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nGroupId);
			Lua_PushNumber(L, nPlayerDataIdx);
			return 1;
		}
	}
	return 0;
}

int LuaDelAllNpcInWro(Lua_State * L)
{
//	int nSubWorldIndex = GetSubWorldIndex(L); //地图
	int nParamNum = Lua_GetTopIndex(L);
    int ulCount=0;
	int nSubWorldIndex;
	if (nParamNum <=0)
	{
       nSubWorldIndex= GetSubWorldIndex(L);    //地图 
       if (nSubWorldIndex >= 0) 
	      ulCount=SubWorld[nSubWorldIndex].DelAllNpcInWro();
	}
	else if (Lua_IsNumber(L, 1))
	{
	   nSubWorldIndex = (int)Lua_ValueToNumber(L,1);
       if (nSubWorldIndex >= 0) 
		  ulCount=SubWorld[nSubWorldIndex].DelAllNpcInWro();	
	}
	Lua_PushNumber(L, ulCount);	
	return 1;
}
int LuaDelAllNpcInWroName(Lua_State* L)
{
	//	int nSubWorldIndex = GetSubWorldIndex(L); //地图
	int nParamNum = Lua_GetTopIndex(L);
	int ulCount = 0;
	int nSubWorldIndex;
	if (nParamNum <= 0)
	{
		nSubWorldIndex = GetSubWorldIndex(L);    //地图 
		if (nSubWorldIndex >= 0)
			ulCount = SubWorld[nSubWorldIndex].DelAllNpcInWro();
	}
	else if (Lua_IsNumber(L, 1))
	{
		if (Lua_IsString(L, 2))
		{
			char* szName = (char*)Lua_ValueToString(L, 2);
			nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
			if (nSubWorldIndex >= 0)
				ulCount = SubWorld[nSubWorldIndex].DelAllNpcInWro(szName);
		}
		else {
			nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
			if (nSubWorldIndex >= 0)
				ulCount = SubWorld[nSubWorldIndex].DelAllNpcInWro();
		}
	}
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaRevivalAllNpc(Lua_State* L)
{
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		SubWorld[nSubWorldIndex].RevivalAllNpc();
	}
	return 0;
}

int LuaRemoveMissionPlayer(Lua_State* L)//RemoveMSPlayer(MissionId, PlayerIndex, groupid)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		return 0;
	int nMissionId = 0;
	int nPlayerIndex = 0;
	if (nParamCount >= 2)
	{
		nMissionId = (int)Lua_ValueToNumber(L, 1);
		nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
	}
	else
	{
		nMissionId = (int)Lua_ValueToNumber(L, 1);
		nPlayerIndex = GetPlayerIndex(L);
	}

	if (nMissionId < 0 || nPlayerIndex <= 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->RemovePlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID);
		}
	}
	return 0;
}

int LuaAddMissionNpc(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int	nNpcIndex = (int)Lua_ValueToNumber(L, 2);
	int nGroupId = 0;
	if (nParamCount >= 3)
		nGroupId = (int)Lua_ValueToNumber(L, 3);

	if (nMissionId < 0 || nNpcIndex <= 0 || nGroupId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->AddNpc(nNpcIndex, Npc[nNpcIndex].m_dwID, nGroupId);
		}
	}
	return 0;
}

int LuaSetMissionGroup(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 2)
		return 0;

	Npc[Player[nPlayerIndex].m_nIndex].m_nMissionGroup = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].SendMSGroup();
	return 0;
}

int LuaRemoveMissionNpc(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int	nNpcIndex = (int)Lua_ValueToNumber(L, 2);

	if (nMissionId < 0 || nNpcIndex <= 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->RemoveNpc(nNpcIndex, Npc[nNpcIndex].m_dwID);
		}
	}
	return 0;
}

int LuaGetNextPlayer(Lua_State* L)//GetNextPlayer(mission, idx,group)
{
	unsigned long nPlayerIndex = 0;

	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		return 2;
	}

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	int nIdx = (int)Lua_ValueToNumber(L, 2);
	int nGroup = (int)Lua_ValueToNumber(L, 3);
	int nSubWorldIndex = GetSubWorldIndex(L);
	int nResultIdx = 0;

	if (nMissionId < 0 || nIdx < 0 || nGroup < 0)
		goto lab_getnextplayer;

	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResultIdx = pMission->GetNextPlayerC(nIdx, nGroup, nPlayerIndex);
		}
	}

lab_getnextplayer:
	Lua_PushNumber(L, nResultIdx);
	Lua_PushNumber(L, nPlayerIndex);
	return 2;
}

int LuaMissionMsg2Group(Lua_State* L)//MSMsg2Group(missionid, string , group)
{
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	char* strMsg = (char*)Lua_ValueToString(L, 2);
	int	nGroupId = (int)Lua_ValueToNumber(L, 3);

	if (nMissionId < 0 || !strMsg || nGroupId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->Msg2Group(strMsg, nGroupId);
		}
	}

	return 0;
}

int LuaMissionMsg2All(Lua_State* L)//MSMsg2Group(missionid, string)
{
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	char* strMsg = (char*)Lua_ValueToString(L, 2);

	if (nMissionId < 0 || !strMsg)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->Msg2All(strMsg);
		}
	}

	return 0;
}

int LuaMissionMsg2Player(Lua_State* L)//MSMsg2Group(missionid, string , group)
{
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	char* strMsg = (char*)Lua_ValueToString(L, 2);
	int	nPlayerIndex = (int)Lua_ValueToNumber(L, 3);

	if (nMissionId < 0 || !strMsg || nPlayerIndex < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			pMission->Msg2Group(strMsg, nPlayerIndex);
		}
	}

	return 0;
}

int LuaMissionNpcCount(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	unsigned long ulCount = 0;
	int nMissionId = 0;
	int nGroupId = -1;
	int nSubWorldIndex = 0;
	if (nParamCount < 1)
		goto lab_getmissionnpccount;

	nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nParamCount > 1)
		nGroupId = (int)Lua_ValueToNumber(L, 2);

	if (nMissionId < 0)
		goto lab_getmissionnpccount;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			if (nGroupId >= 0)
				ulCount = pMission->GetGroupNpcCount(nGroupId);
			else
				ulCount = pMission->GetNpcCount();
		}
	}

lab_getmissionnpccount:
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaMissionPlayerCount(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	unsigned long ulCount = 0;
	int nMissionId = 0;
	int nGroupId = -1;
	int nSubWorldIndex = 0;
	if (nParamCount < 1)
		goto lab_getmissionplayercount;

	nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nParamCount > 1)
		nGroupId = (int)Lua_ValueToNumber(L, 2);

	if (nMissionId < 0)
		goto lab_getmissionplayercount;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			if (nGroupId >= 0)
				ulCount = pMission->GetGroupPlayerCount(nGroupId);
			else
				ulCount = pMission->GetPlayerCount();
		}
	}

lab_getmissionplayercount:
	Lua_PushNumber(L, ulCount);
	return 1;
}

int LuaSetPlayerDeathScript(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	char* szScript = (char*)Lua_ValueToString(L, 1);
	Player[nPlayerIndex].m_dwDeathScriptId = g_FileName2Id(szScript);
	return 0;
}

int LuaSetLogoutScript(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	int nPlayerIndex;
	char* szScript;
	if (nParamNum < 2)
	{
		nPlayerIndex = GetPlayerIndex(L);
		szScript = (char*)Lua_ValueToString(L, 1);
	}
	else
	{
		nPlayerIndex = (int)Lua_ValueToNumber(L, 1);
		szScript = (char*)Lua_ValueToString(L, 2);
	}

	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_dwLogoutScriptID = g_FileName2Id(szScript);
	return 0;
}

int LuaUpdateBattleBox(Lua_State* L)// UpdateBattleBox
{
	int nParamCount = Lua_GetTopIndex(L);
	BYTE nKind = 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	int nSubWorldIndex = -1;
	char szBattleDesc[128];
	BYTE nType = 0;
	int nValue[2];

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);

	if (!pMission)
		return 0;

	if (nParamCount == 2)
		nKind = (BYTE)Lua_ValueToNumber(L, 2);

	if (nParamCount == 3)
	{
		nValue[0] = (int)Lua_ValueToNumber(L, 2);
		nType = (BYTE)Lua_ValueToNumber(L, 3);

		if (nType == 1)
		{
			sprintf(szBattleDesc, "%d", nValue[0]);
			nKind = 1;
		}

		if (nType == 3)
		{
			if (nValue[0] <= 0)
				return 0;

			BYTE nTT = 11;
			char szName[32] = "";//pMission->GetMissionPlayer_Name(nValue[0]);
			int nPlayerIndex = pMission->GetMissionPlayer_PlayerIndex(nValue[0]);
			if (nPlayerIndex > 0)
			{
				strcpy(szName, Player[nPlayerIndex].m_PlayerName);
			}
			BYTE m_btGroup = pMission->GetMissionPlayer_GroupId(nValue[0]);
			int m_nKPlayer = pMission->m_MissionPlayer.GetParam(nValue[0], 2);//kill player
			int m_nKNpc = pMission->m_MissionPlayer.GetParam(nValue[0], 3);//kill npc
			int m_nCurKill = pMission->m_MissionPlayer.GetParam(nValue[0], 7);//lien tram hien tai

			BYTE m_btRobFlag = pMission->m_MissionPlayer.GetParam(nValue[0], 9);//nh苩 c?
			int m_nTPPoint = pMission->m_MissionPlayer.GetParam(nValue[0], 6);//t輈h lu?
			int m_nTPDeath = pMission->m_MissionPlayer.GetParam(nValue[0], 4);//s?l莕 ch誸
			int m_nTPKill = pMission->m_MissionPlayer.GetParam(nValue[0], 5);//max li猲 tr秏

			sprintf(szBattleDesc, "%s|%d|%d|%d|%d|%d|%d|%d|%d|%d", szName, nTT, m_btGroup, m_nKPlayer, m_nKNpc, m_nTPPoint, m_nTPDeath, m_nCurKill, m_nTPKill, m_btRobFlag);
			nKind = 5;
		}
	}

	if (nParamCount == 4)
	{
		nType = (BYTE)Lua_ValueToNumber(L, 2);
		nValue[0] = (int)Lua_ValueToNumber(L, 3);
		nValue[1] = (int)Lua_ValueToNumber(L, 4);

		if (nValue[0] < 0 || nValue[1] < 0)
			return 0;

		if (nType == 2)
		{
			sprintf(szBattleDesc, "%d|%d", nValue[0], nValue[1]);
			nKind = 2;
		}

		if (nType == 5)
		{
			BYTE nTT = nValue[1];
			char szName[32] = "";//pMission->GetMissionPlayer_Name(nValue[0]);
			int nPlayerIndex = pMission->GetMissionPlayer_PlayerIndex(nValue[0]);
			if (nPlayerIndex > 0)
			{
				strcpy(szName, Player[nPlayerIndex].m_PlayerName);
			}
			BYTE m_btGroup = pMission->GetMissionPlayer_GroupId(nValue[0]);
			int m_nKPlayer = pMission->m_MissionPlayer.GetParam(nValue[0], 2);//kill player
			int m_nKNpc = pMission->m_MissionPlayer.GetParam(nValue[0], 3);//kill npc
			int m_nCurKill = pMission->m_MissionPlayer.GetParam(nValue[0], 7);//li猲 tr秏 hi謓 t筰

			BYTE m_btRobFlag = pMission->m_MissionPlayer.GetParam(nValue[0], 9);//nh苩 c?
			int m_nTPPoint = pMission->m_MissionPlayer.GetParam(nValue[0], 6);//t輈h lu?
			int m_nTPDeath = pMission->m_MissionPlayer.GetParam(nValue[0], 4);//s?l莕 ch誸
			int m_nTPKill = pMission->m_MissionPlayer.GetParam(nValue[0], 5);//max li猲 tr秏

			sprintf(szBattleDesc, "%s|%d|%d|%d|%d|%d|%d|%d|%d|%d", szName, nTT, m_btGroup, m_nKPlayer, m_nKNpc, m_nTPPoint, m_nTPDeath, m_nCurKill, m_nTPKill, m_btRobFlag);
			nKind = 3;
		}
	}

	if (nParamCount == 5)
	{
		int m_nPointPlayerT = Lua_ValueToNumber(L, 2);
		int m_nPointPlayerK = Lua_ValueToNumber(L, 3);
		int m_nPointPlayer = Lua_ValueToNumber(L, 4);
		sprintf(szBattleDesc, "%d|%d|%d", m_nPointPlayerT, m_nPointPlayerK, m_nPointPlayer);
		nKind = (BYTE)Lua_ValueToNumber(L, 5);
	}

	S2C_BATTLE_BOX NetCommand;
	NetCommand.ProtocolType = s2c_battlebox;
	NetCommand.nType = nKind;
	strcpy(NetCommand.szBattleDesc, szBattleDesc);
	if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_BATTLE_BOX));
	return 0;
}


int LuaNpcIndexToPlayerIndex(Lua_State* L)
{
	int nResult = 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		goto lab_npcindextoplayerindex;

	if (Npc[nNpcIndex].m_Index > 0 && Npc[nNpcIndex].IsPlayer())
	{
		if (Npc[nNpcIndex].GetPlayerIdx() > 0)
			nResult = Npc[nNpcIndex].GetPlayerIdx();
	}

lab_npcindextoplayerindex:
	Lua_PushNumber(L, nResult);
	return 1;

}
int LuaGiftcodeIsValid(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	char* szGiftType = (char*)Lua_ValueToString(L, 1);
	char* szGiftCode = (char*)Lua_ValueToString(L, 2);
	if (!szGiftCode || !szGiftCode[0])
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	if (!strcmp(szGiftType, "Code_Tuan"))
	{
		if (g_GiftCodeFanCungManager.IsValidCode(szGiftCode))
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
		else
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
	}
	else if (!strcmp(szGiftType, "Code_New"))
	{
		if (g_GiftCodeNewManager.IsValidCode(szGiftCode))
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
		else
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
	}

	Lua_PushNumber(L, 0);
	return 1;
}

int LuaUseGiftcodeS(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;
	char* szGiftType = (char*)Lua_ValueToString(L, 1);
	char* szGiftCode = (char*)Lua_ValueToString(L, 2);
	if (!szGiftCode || !szGiftCode[0])
		return 0;
	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;
	if (!strcmp(szGiftType, "Code_Tuan"))
	{
		if (g_GiftCodeFanCungManager.UseCode(szGiftCode)) {
			Lua_PushNumber(L, 1);
			return 1;
		}
		else {
			Lua_PushNumber(L, 0);
			return 1;
		}
	}
	else if (!strcmp(szGiftType, "Code_New"))
	{
		if (g_GiftCodeNewManager.UseCode(szGiftCode)) {
			Lua_PushNumber(L, 1);
			return 1;
		}
		else {
			Lua_PushNumber(L, 0);
			return 1;
		}
	}
	return 0;
}
int LuaGetMissionPlayer_PlayerIndex(Lua_State* L)
{
	unsigned long nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2)
		goto lab_getmissionplayer_npcindex;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		if (nMissionId < 0 || nDataIndex < 0)
			goto lab_getmissionplayer_npcindex;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_PlayerIndex(nDataIndex);
		}
	}

lab_getmissionplayer_npcindex:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetMissionPlayer_DataIndex(Lua_State* L)
{
	unsigned long nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2)
		goto lab_getmissionplayer_dataindex;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
		if (nMissionId < 0 || nPlayerIndex < 0)
			goto lab_getmissionplayer_dataindex;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_DataIndex(nPlayerIndex, Player[nPlayerIndex].m_dwID);
		}
	}

lab_getmissionplayer_dataindex:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaSetMissionPlayerParam(Lua_State* L)//SetMPParam(missionid, nDidx, vid, v)
{
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 4)
		return 0;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId = (int)Lua_ValueToNumber(L, 3);
		int nValue = (int)Lua_ValueToNumber(L, 4);

		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			return 0;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			//pMission->m_MissionPlayer.SetParam(nDataIndex, nParamId, nValue);
			pMission->SetPlayerParam(nDataIndex, nParamId, nValue); //add by phong ki襲 using t鑞g kim
		}
	}
	return 0;
}

int LuaGetMissionPlayerParam(Lua_State* L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 3)
		goto lab_getmissionplayerparam;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
		int nParamId = (int)Lua_ValueToNumber(L, 3);

		if (nMissionId < 0 || nDataIndex < 0 || nParamId > MAX_MISSION_PARAM)
			goto lab_getmissionplayerparam;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->m_MissionPlayer.GetParam(nDataIndex, nParamId);
		}
	}
lab_getmissionplayerparam:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlayerMissionGroup(Lua_State* L)
{
	int nResult = 0;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 2)
		goto lab_getmissionplayergroup;

	nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex >= 0)
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nNpcIndex = (int)Lua_ValueToNumber(L, 2);

		if (nMissionId < 0 || nNpcIndex < 0)
			goto lab_getmissionplayergroup;

		KMission Mission;
		Mission.SetMissionId(nMissionId);
		KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
		if (pMission)
		{
			nResult = pMission->GetMissionPlayer_GroupId(nNpcIndex);
		}
	}
lab_getmissionplayergroup:
	Lua_PushNumber(L, nResult);
	return 1;

}

int LuaIsMission(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;

	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
		Lua_PushNumber(L, 1);
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetMSLadder(Lua_State* L)//add by phong ki襲 using t鑞g kim
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;

	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	if (nMissionId < 0)
		return 0;

	int nSubWorldIndex = GetSubWorldIndex(L);
	if (nSubWorldIndex < 0)
		return 0;
	KMission Mission;
	Mission.SetMissionId(nMissionId);
	KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
	if (pMission)
	{
		int nOrdinal = (int)Lua_ValueToNumber(L, 2);
		if (nOrdinal >= 0 && nOrdinal < MISSION_STATNUM)
		{
			Lua_PushString(L, pMission->m_MissionLadder[nOrdinal].Name);
			Lua_PushNumber(L, pMission->m_MissionLadder[nOrdinal].ucGroup);
			Lua_PushNumber(L, pMission->m_MissionLadder[nOrdinal].nParam[pMission->GetMissionLadderParam()]);
			return 3;
		}

	}
	return 0;
}

int LuaSetPlayerRevivalOptionWhenLogout(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 1);

	if (nType)
		Player[nPlayerIndex].SetLoginType(1);
	else
		Player[nPlayerIndex].SetLoginType(0);

	return 0;
}

int LuaSetPlayerPKValue(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nPKValue = (int)Lua_ValueToNumber(L, 1);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	Player[nPlayerIndex].m_cPK.SetPKValue(nPKValue);
	return 0;
}

int LuaGetPlayerPKValue(Lua_State* L)
{
	int nPKValue = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getplayerpkvalue;

	if (Player[nPlayerIndex].m_nIndex <= 0)
		goto lab_getplayerpkvalue;
	nPKValue = Player[nPlayerIndex].m_cPK.GetPKValue();

lab_getplayerpkvalue:
	Lua_PushNumber(L, nPKValue);
	return 1;
}

int	LuaGetCurNpcIndex(Lua_State* L)
{
	int nNpcIndex = 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_getcurnpcindex;

lab_getcurnpcindex:
	Lua_PushNumber(L, Player[nPlayerIndex].m_nIndex);
	return 1;
}

int LuaShowLadder(Lua_State* L)//showladder(count, ladderid1, ladderid2.....)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 2)
		return 0;
	int nLadderCount = (DWORD)Lua_ValueToNumber(L, 1);
	if (nLadderCount <= 0)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	if (nLadderCount > nParamCount - 1)
		nLadderCount = nParamCount - 1;
	BYTE Buffer[sizeof(LADDER_LIST) + 50 * sizeof(DWORD)];
	LADDER_LIST* pLadderList = (LADDER_LIST*)&Buffer;
	pLadderList->ProtocolType = s2c_ladderlist;
	pLadderList->nCount = nLadderCount;
	pLadderList->wSize = sizeof(LADDER_LIST) + nLadderCount * sizeof(DWORD) - 1;
	for (int i = 0; i < nLadderCount; i++)
	{
		pLadderList->dwLadderID[i] = (DWORD)Lua_ValueToNumber(L, i + 2);
	}
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &Buffer, pLadderList->wSize + 1);
	return 0;
}

int LuaSwearBrother(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nTeamId = Lua_ValueToNumber(L, 1);

	if (nTeamId >= MAX_TEAM || nTeamId < 0)
		return 0;

	KPlayerChat::STRINGLIST BrotherList;
	_ASSERT(g_Team[nTeamId].m_nCaptain > 0);

	std::string strCapName;
	strCapName = Npc[Player[g_Team[nTeamId].m_nCaptain].m_nIndex].Name;
	BrotherList.push_back(strCapName);

	for (int i = 0; i < g_Team[nTeamId].m_nMemNum; i++)
	{
		int nPlayerIndex = g_Team[nTeamId].m_nMember[i];
		if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		{
			std::string strName;
			strName = Npc[Player[nPlayerIndex].m_nIndex].Name;
			BrotherList.push_back(strName);
		}
	}

	KPlayerChat::MakeBrother(BrotherList);
	return 0;
}

int LuaMakeEnemy(Lua_State* L)
{
	return 0;
}

int LuaAddLeadExp(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;

	int nLeadExp = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].AddLeadExp(nLeadExp);
	return 0;
}

int LuaGetLeadLevel(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nLeadLevel = 0;
	if (nPlayerIndex <= 0)
		goto lab_getleadlevel;
	nLeadLevel = (int)Player[nPlayerIndex].m_dwLeadLevel;

lab_getleadlevel:
	Lua_PushNumber(L, nLeadLevel);
	return 1;
}

int LuaAddMagicPoint(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nSkillPoint = (int)Lua_ValueToNumber(L, 1);

	Player[nPlayerIndex].m_nSkillPoint += nSkillPoint;
	if (Player[nPlayerIndex].m_nSkillPoint < 0)
		Player[nPlayerIndex].m_nSkillPoint = 0;

	S2C_PLAYER_SYNC_M_A	sMsg;
	sMsg.ProtocolType = s2c_playersync_magic_attr;
	sMsg.nPoint = Player[nPlayerIndex].m_nSkillPoint;
	sMsg.nType = enumS2C_PLAYERSYNC_ID_MAGICPOINT;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sMsg, sizeof(S2C_PLAYER_SYNC_M_A));

	return 0;
}

int LuaIncSkill(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 2) return 0;

	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0) return 0;

	int nSkillId = 0, nAddLevel = 0;
	nSkillId = (int)Lua_ValueToNumber(L, 1);
	nAddLevel = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId <= 0 || nAddLevel <= 0)
		return 0;
	Player[nPlayerIndex].IncSkillLevel(nSkillId, nAddLevel);
	return 0;
}

int LuaGetNpcExpRate(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}
	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_CurrentExpEnhance);
	return 1;
}

int LuaIncSkillExp(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	if (nParamCount < 2) return 0;

	nPlayerIndex = GetPlayerIndex(L);

	if (nPlayerIndex <= 0) return 0;

	int nSkillId = 0, nAddExp = 0;
	nSkillId = (int)Lua_ValueToNumber(L, 1);
	nAddExp = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId <= 0 || nAddExp <= 0)
		return 0;
	Player[nPlayerIndex].IncSkillExp(nSkillId, nAddExp);
	return 0;
}

int LuaCheckItemEquipCS(Lua_State* L)
{
	int nNumberPrama = Lua_GetTopIndex(L);

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;

	if (Player[nPlayerIndex].m_nIndex <= 0 || Player[nPlayerIndex].m_nIndex >= MAX_NPC) return 0;

	if (nNumberPrama < 0)
		return 0;



	int nCheck = Player[nPlayerIndex].m_ItemList.CheckItemEquipCS();

	Lua_PushNumber(L, nCheck);

	return 1;
}


int LuaGetMagicPoint(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nSkillPoint = 0;
	if (nPlayerIndex <= 0)
		goto lab_getmagicpoint;

lab_getmagicpoint:
	Lua_PushNumber(L, Player[nPlayerIndex].m_nSkillPoint);
	return 1;
}
int LuaResetProp(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);

	int nResult = 0;
	if (nPlayerIndex > 0)
		nResult = Player[nPlayerIndex].ResetProp();

	Lua_PushNumber(L, nResult);
	return 1;

}
int LuaAddPropPoint(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		return 0;
	nPropPoint = (int)Lua_ValueToNumber(L, 1);
	Player[nPlayerIndex].m_nAttributePoint += nPropPoint;
	if (Player[nPlayerIndex].m_nAttributePoint < 0)
		Player[nPlayerIndex].m_nAttributePoint = 0;

	S2C_PLAYER_SYNC_M_A	sMsg;
	sMsg.ProtocolType = s2c_playersync_magic_attr;
	sMsg.nPoint = Player[nPlayerIndex].m_nAttributePoint;
	sMsg.nType = enumS2C_PLAYERSYNC_ID_PROPPOINT;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sMsg, sizeof(S2C_PLAYER_SYNC_M_A));

	return 0;
}

int LuaGetTimeZero(Lua_State* L)
{
	Lua_PushNumber(L, time(0));
	return 1;
}

int LuaGetTimeNow(Lua_State* L)
{
	SYSTEMTIME		tm;
	GetLocalTime(&tm);
	DWORD nYear = 0;
	DWORD nMonth = 0;
	DWORD nDay = 0;
	DWORD nHour = 0;
	DWORD nMinute = 0;
	DWORD nSecond = 0;
	DWORD nDayOfWeek = 0;

	nYear = (tm.wYear % 100) + 2000;
	nMonth = tm.wMonth;
	nDay = tm.wDay;
	nHour = tm.wHour;
	nMinute = tm.wMinute;
	nSecond = tm.wSecond;
	nDayOfWeek = tm.wDayOfWeek;

	Lua_PushNumber(L, nYear);
	Lua_PushNumber(L, nMonth);
	Lua_PushNumber(L, nDay);
	Lua_PushNumber(L, nHour);
	Lua_PushNumber(L, nMinute);
	Lua_PushNumber(L, nSecond);
	Lua_PushNumber(L, nDayOfWeek);
	return 7;
}

int LuaCastSkill(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;

	if (Lua_GetTopIndex(L) < 3)
		return 0;

	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)
		Npc[Player[nPlayerIndex].m_nIndex].Cast(nSkillId, nSkillLevel);
	return 0;
}
int LuaNpcCastSkill(Lua_State* L)
{

	if (Lua_GetTopIndex(L) < 3)
		return 0;


	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	int nSkillId = (int)Lua_ValueToNumber(L, 2);

	int nSkillLevel = (int)Lua_ValueToNumber(L, 3);

	if (nSkillId < MAX_SKILL && nSkillLevel < MAX_SKILLLEVEL)
	{
		Npc[nNpcIndex].Cast(nSkillId, nSkillLevel);
	}

	return 0;
}

int LuaIgnoreState(Lua_State* L) //Lo筰 b?c竎 miniskill tr猲 ngi, l謓h b礽, chi課 c?
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	//
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	//
	Npc[Player[nPlayerIndex].m_nIndex].IgnoreState(FALSE);
	return 0;
}

int LuaIsMyItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		if (Lua_GetTopIndex(L) > 1)
		{
			int nGameIdx = (int)Lua_ValueToNumber(L, 1);
			if (Player[nPlayerIndex].m_ItemList.SearchID(Item[nGameIdx].GetID()))
				Lua_PushNumber(L, 1);
			else
				Lua_PushNumber(L, 0);
		}
	}
	return 1;
}

int LuaSetProtectTime(Lua_State* L) //vong tron bat tu, v遪g tr遪 b蕋 t?
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;

	int nTime = (int)Lua_ValueToNumber(L, 1);
	Npc[Player[nPlayerIndex].m_nIndex].SetProtectTime(nTime);
	return 1;
}

int LuaAddSkillState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	//
	if (Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	//
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 4)
		return 0;
	//
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 2);
	int nIfMagic = (int)Lua_ValueToNumber(L, 3);
	int nTime = (int)Lua_ValueToNumber(L, 4);
	BOOL bOverLook = FALSE;
	if (nParamNum > 5)
	{
		bOverLook = (BOOL)Lua_ValueToNumber(L, 5);
	}
	//
	if (nTime <= 0)
		nTime = -1;
	//
	if (nIfMagic)//kh玭g a thu閏 t輓h skill v祇
	{
		KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pSkill->CastStateSkill(Player[nPlayerIndex].m_nIndex, 0, 0, nTime, bOverLook);
	}
	else//a thu閏 t輓h skill v祇
	{
		KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
		memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
		DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
		DamageMagicAttribs[0].nValue[0] = 0;
		DamageMagicAttribs[0].nValue[1] = 0;
		DamageMagicAttribs[0].nValue[2] = 0;
		Npc[Player[nPlayerIndex].m_nIndex].SetStateSkillEffect(Player[nPlayerIndex].m_nIndex, nSkillId, nSkillLevel, DamageMagicAttribs, 1, nTime, bOverLook);
	}
	return 0;
}

int LuaAddNpcSkillState(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 5)
		return 0;
	//
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
		return 0;
	//
	int nSkillId = (int)Lua_ValueToNumber(L, 2);
	int nSkillLevel = (int)Lua_ValueToNumber(L, 3);
	int nIfMagic = (int)Lua_ValueToNumber(L, 4);
	int nTime = (int)Lua_ValueToNumber(L, 5);
	//
	if (nIfMagic)//kh玭g a thu閏 t輓h skill v祇
	{
		KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nSkillId, nSkillLevel);
		pSkill->CastStateSkill(nNpcIndex, 0, 0, nTime, TRUE);
	}
	else//a thu閏 t輓h skill v祇
	{
		KMagicAttrib DamageMagicAttribs[MAX_MISSLE_DAMAGEATTRIB];
		memset(DamageMagicAttribs, 0, sizeof(DamageMagicAttribs));
		DamageMagicAttribs[0].nAttribType = magic_attackrating_v;
		DamageMagicAttribs[0].nValue[0] = 0;
		DamageMagicAttribs[0].nValue[1] = 0;
		DamageMagicAttribs[0].nValue[2] = 0;
		Npc[nNpcIndex].SetStateSkillEffect(nNpcIndex, nSkillId, nSkillLevel, DamageMagicAttribs, 1, nTime, TRUE);
	}
	return 0;
}

int LuaPayExtPoint(Lua_State* L)//PayExtPoint
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_payextpoint;

	nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		goto lab_payextpoint;
	nPay = Lua_ValueToNumber(L, 1);
	if (nPay < 0)
		goto lab_payextpoint;
	nResult = Player[nPlayerIndex].PayExtPoint(nPay);

lab_payextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetRestPropPoint(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nPropPoint = 0;
	if (nPlayerIndex <= 0)
		goto lab_getrestproppoint;
	nPropPoint = Player[nPlayerIndex].m_nAttributePoint;

lab_getrestproppoint:
	Lua_PushNumber(L, nPropPoint);
	return 1;
}

int LuaMsgToGameMaster(Lua_State* L)//Msg2GM(str, id)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		return 0;
	int nParamID = 0;
	const char* szMsg = Lua_ValueToString(L, 1);
	if (!szMsg)
		return 0;

	if (nParamCount < 2)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int)Lua_ValueToNumber(L, 2);
	}

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		char szID[32];

		sprintf(szID, "%d", nParamID);
		KPlayerChat::SendInfoToGM("GM", szID, (char*)szMsg, strlen(szMsg));
	}
	return 0;
}

int LuaMsgToIP(Lua_State* L)//Msg2IP(IP, ID, str)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 3)
		return 0;
	int nIP = 0;
	const char* szIP = Lua_ValueToString(L, 1);
	nIP = _a2ip(szIP);
	if (nIP == 0)
		return 0;

	int nID = (int)Lua_ValueToNumber(L, 2);
	const char* szMsg = Lua_ValueToString(L, 3);
	if (!szMsg)
		return 0;

	int nParamID = 0;
	if (nParamCount < 4)
	{
		nParamID = 0;
	}
	else
	{
		nParamID = (int)Lua_ValueToNumber(L, 4);
	}
	char szID[32];
	sprintf(szID, "%d", nParamID);
	KPlayerChat::SendInfoToIP(nIP, nID, "GM", szID, (char*)szMsg, strlen(szMsg));
	return 0;
}

int LuaGetPlayerInfo(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char szDesMsg[250];
	char szMsg[800];
	int nNpcIdx = 0;
	KNpc* pNpc = NULL;
	KPlayer* pPlayer = NULL;
	szDesMsg[0] = 0;
	if (nPlayerIndex <= 0)
		goto lab_getplayerinfo;

	pPlayer = &Player[nPlayerIndex];
	nNpcIdx = pPlayer->m_nIndex;
	pNpc = &Npc[nNpcIdx];
	sprintf(szMsg, "Name:%s,Lvl:%d,IP:%s,Lf:%d|%d,Mn:%d|%d,Mny:%d,sp:%d,ap:%d", pNpc->Name, pNpc->m_Level, g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx), pNpc->m_CurrentLife, pNpc->m_CurrentLifeMax, pNpc->m_CurrentMana, pNpc->m_CurrentManaMax, pPlayer->m_ItemList.GetMoney(room_equipment), pPlayer->m_nSkillPoint, pPlayer->m_nAttributePoint);
	g_StrCpyLen(szDesMsg, szMsg, 250);

lab_getplayerinfo:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaSetNpcTimeIdle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;
		if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
			return 0;
		int nTime = (int)Lua_ValueToNumber(L, 1);
		Npc[nNpcIndex].m_nTimeIdleValue = nTime;
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaGetNpcTimeIdle(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;
		if (nNpcIndex < 0 || nNpcIndex >= MAX_NPC)
			return 0;
		int nTime = Npc[nNpcIndex].m_nTimeIdleCounter;
		Lua_PushNumber(L, nTime);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaGetHWID(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char szDesMsg[128];
	szDesMsg[0] = 0;
	KPlayer* pPlayer = NULL;
	if (nPlayerIndex <= 0)
		goto lab_getplayerip;
	pPlayer = &Player[nPlayerIndex];
	strcpy(szDesMsg, pPlayer->m_nPlayerHWID);

lab_getplayerip:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaGetIP(Lua_State * L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	char szDesMsg[256];
	szDesMsg[0] = 0;
	KPlayer * pPlayer = NULL;
	if (nPlayerIndex <= 0)
		goto lab_getplayerip;
	pPlayer = &Player[nPlayerIndex];
	const char* tmp = g_pServer->GetClientInfo(pPlayer->m_nNetConnectIdx);
	if (tmp == NULL) {
		printf("LuaGetIP: GetClientInfo return NULL\n");
		goto lab_getplayerip;
	}
	strncpy(szDesMsg, tmp, sizeof(szDesMsg));
	
lab_getplayerip:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaSetDeathPunish(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		int nState = Lua_ValueToNumber(L, 1);
		if (nState == 0)
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = 0;
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = enumDEATH_MODE_PKBATTLE_PUNISH;
	}
	return 0;
}

int LuaHideNpc(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	int nNpcIndex = 0;

	if (Lua_IsNumber(L, 1))
	{
		nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	}
	else
	{
		const char* szName = Lua_ValueToString(L, 1);
		int nSubWorldIndex = GetSubWorldIndex(L);
		if (nSubWorldIndex < 0)
			return 0;

		nNpcIndex = SubWorld[nSubWorldIndex].FindNpcFromName(szName);
	}

	if (nNpcIndex > 0 || nNpcIndex < MAX_NPC)
	{
		int nFrame = Lua_ValueToNumber(L, 2);
		if (nFrame <= 0)
			nFrame = 1;
		if(!Npc[nNpcIndex].IsPlayer())
		{
		Npc[nNpcIndex].ExecuteRevive();
		Npc[nNpcIndex].m_Frames.nTotalFrame = nFrame;
		Npc[nNpcIndex].m_Frames.nCurrentFrame = 0;
		}
	}

	return 0;
}

#endif
int LuaRANDOM(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 1)
		return 0;
	int nResult = 0;
	if (nParamNum > 1)
		nResult = GetRandomNumber((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	else
		nResult = GetRandomNumber(0, (int)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, nResult);
	return 1;
}
int LuaRANDOMC(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 2)
		return 0;

	srand((unsigned)time(NULL));
	if (Lua_IsTable(L, 1))
	{
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, 2));
		Lua_RawGet(L, 1);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L)));
	}
	else if (Lua_IsTable(L, 2))
	{
		int nResult = ::GetRandomNumber(1, (int)Lua_ValueToNumber(L, 1));
		Lua_PushNumber(L, nResult);
		Lua_RawGet(L, 2);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, (int)Lua_ValueToNumber(L, Lua_GetTopIndex(L))));
	}
	else
	{
		int nResult = ::GetRandomNumber(1, nParamNum);
		Lua_PushNumber(L, (int)Lua_ValueToNumber(L, nResult));
	}
	return 1;
}
int LuaSetNpcKind(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	int nKind = (int)Lua_ValueToNumber(L, 2);
	if (nKind < kind_normal || nKind >= kind_num)
		return 0;

	Npc[nNpcIndex].m_Kind = nKind;
	return 0;
}
int LuaSetNpcBoss2(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);

	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC)
		return 0;

	Npc[nNpcIndex].m_Type = (int)Lua_ValueToNumber(L, 2);
	return 0;
}




int LuaPlayMusic(Lua_State* L)//PlayMusic(musicname,loop=1, vol );
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;

	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_PLAYMUSIC;
	UiInfo.m_bOptionNum = 1;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;

	int nMsgId = 0;

	g_StrCpyLen(UiInfo.m_pContent, Lua_ValueToString(L, 1), sizeof(UiInfo.m_pContent));
	UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
	UiInfo.m_bParam1 = 0;

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

int LuaFadeInMusic(Lua_State* L)
{
	return 0;
}

int LuaFadeOutMusic(Lua_State* L)
{
	return 0;
}

bool is_ascii_number(const char* token) {
	if (token == NULL || *token == '\0') return false;

	// Optional: check for leading sign
	if (*token == '+' || *token == '-') token++;

	if (*token == '\0') return false;  // only sign, no digits

	while (*token) {
		if (!isdigit((unsigned char)*token)) return false;
		token++;
	}
	return true;
}

int LuaStrSplit(Lua_State* L)
{
	char* strMain = NULL;
	char tmp[32];
	int idx = 0;

	// Validate arguments
	if (Lua_GetTopIndex(L) < 2)
		return 0; // Not enough arguments

	if (Lua_IsString(L, 1))
	{
		strMain = (char*)Lua_ValueToString(L, 1);
		strcpy(tmp, strMain);
	}
	if (Lua_IsNumber(L, 2))
	{
		idx = Lua_ValueToNumber(L, 2);
	}

	// Check for valid input
	if (!strMain || idx < 0)
		return 0; // Invalid arguments

	// Split the string based on '|'
	const char* delimiter = "|";
	char* token = strtok(tmp, delimiter);
	int currentIdx = 0;

	while (token != NULL)
	{
		if (currentIdx == idx)
		{
			// Convert the token to a number
			if (is_ascii_number(token)) {
				int number = atoi(token);

				// Push the result to Lua stack
				Lua_PushNumber(L, number);
			}
				else {
				// If the token is not a valid number, push it as a string
				Lua_PushString(L, token);
			}
			return 1; // Return 1 value to Lua
		}
		token = strtok(NULL, delimiter);
		currentIdx++;
	}

	// If index is out of bounds, return nil
	Lua_PushNil(L);
	return 1;
}

#ifndef _SERVER
int LuaPlaySound(Lua_State* L)
{
	return 0;
}

int LuaPlaySprMovie(Lua_State* L)
{
	return 0;
}
#endif
#ifdef _SERVER
// ==== JX2 port: ham bang hoi kieu JX2 (dinh nghia o KTongJX2.cpp) ====
extern int LuaTONG_IsExist(Lua_State* L);
extern int LuaTONG_GetName(Lua_State* L);
extern int LuaTONG_Name2ID(Lua_State* L);
extern int LuaTONG_GetTongCount(Lua_State* L);
extern int LuaTONG_GetFirstTong(Lua_State* L);
extern int LuaTONG_GetNextTong(Lua_State* L);
extern int LuaTONG_GetTongByRoleName(Lua_State* L);
extern int LuaTONG_GetMemberCount(Lua_State* L);
extern int LuaTONG_GetOnlineCount(Lua_State* L);
extern int LuaTONG_GetFirstMember(Lua_State* L);
extern int LuaTONG_GetNextMember(Lua_State* L);
extern int LuaTONG_GetMaster(Lua_State* L);
extern int LuaTONG_GetTaskValue(Lua_State* L);
extern int LuaTONG_GetUTaskValue(Lua_State* L);
extern int LuaTONG_GetTaskTemp(Lua_State* L);
extern int LuaTONG_SetTaskTemp(Lua_State* L);
extern int LuaTONG_GetCredit(Lua_State* L);
extern int LuaTONG_GetExp(Lua_State* L);
extern int LuaTONG_GetUnionID(Lua_State* L);
extern int LuaTONG_GetWarState(Lua_State* L);
extern int LuaTONG_GetBuildFund(Lua_State* L);
extern int LuaTONG_GetBuildLevel(Lua_State* L);
extern int LuaTONG_GetWarBuildFund(Lua_State* L);
extern int LuaTONG_GetMaintainFund(Lua_State* L);
extern int LuaTONG_GetPerStandFund(Lua_State* L);
extern int LuaTONG_GetStoredOffer(Lua_State* L);
extern int LuaTONG_GetStoredBuildFund(Lua_State* L);
extern int LuaTONG_GetDay(Lua_State* L);
extern int LuaTONG_GetWeek(Lua_State* L);
extern int LuaTONG_GetWeekBuildFund(Lua_State* L);
extern int LuaTONG_GetWeekBuildUpper(Lua_State* L);
extern int LuaTONG_GetTotalBuildFund(Lua_State* L);
extern int LuaTONG_GetPauseState(Lua_State* L);
extern int LuaTONG_GetTongMap(Lua_State* L);
extern int LuaTONG_GetTongMapTemplate(Lua_State* L);
extern int LuaTONG_GetTongMapBan(Lua_State* L);
extern int LuaTONG_GetOccupyCityDay(Lua_State* L);
extern int LuaTONG_GetMoney(Lua_State* L);
extern int LuaTONG_GetStandFund(Lua_State* L);
extern int LuaTONG_ApplySetTaskValue(Lua_State* L);
extern int LuaTONG_ApplyAddTaskValue(Lua_State* L);
extern int LuaTONG_ApplyAddUTaskValue(Lua_State* L);
extern int LuaTONG_ApplySetMoney(Lua_State* L);
extern int LuaTONG_ApplyAddMoney(Lua_State* L);
extern int LuaTONGM_IsExist(Lua_State* L);
extern int LuaTONGM_GetName(Lua_State* L);
extern int LuaTONGM_GetFigure(Lua_State* L);
extern int LuaTONGM_GetSex(Lua_State* L);
extern int LuaTONGM_GetOnline(Lua_State* L);
extern int LuaTONGM_GetOffer(Lua_State* L);
extern int LuaTONGM_GetTaskValue(Lua_State* L);
extern int LuaTONGM_GetUTaskValue(Lua_State* L);
extern int LuaTONGM_CheckRight(Lua_State* L);
extern int LuaTONGM_ApplySetTaskValue(Lua_State* L);
extern int LuaTONGM_ApplyAddTaskValue(Lua_State* L);
extern int LuaTONGM_ApplyAddOffer(Lua_State* L);
extern int LuaTONGM_ApplySetRetireDate(Lua_State* L);
extern int LuaTONGM_ApplySetLastOnlineDate(Lua_State* L);
extern int LuaTONGM_ApplyAddRight(Lua_State* L);
extern int LuaTONGM_ApplyDelRight(Lua_State* L);
// ==== JX2 port dot 2 ====
extern int LuaTONG_GetSelfCamp(Lua_State* L);
extern int LuaTONG_GetCurCamp(Lua_State* L);
extern int LuaTONG_GetExpLevel(Lua_State* L);
extern int LuaTONG_GetPremium(Lua_State* L);
extern int LuaTONG_GetCurWeekGoalLevel(Lua_State* L);
extern int LuaTONG_GetWeekGoalEvent(Lua_State* L);
extern int LuaTONG_GetWeekGoalLevel(Lua_State* L);
extern int LuaTONG_GetWeekGoalTotal(Lua_State* L);
extern int LuaTONG_GetWeekGoalPlayer(Lua_State* L);
extern int LuaTONG_GetWeekGoalValue(Lua_State* L);
extern int LuaTONG_GetWeekGoalPriceTong(Lua_State* L);
extern int LuaTONG_GetWeekGoalPricePlayer(Lua_State* L);
extern int LuaTONG_GetLWeekGoalEvent(Lua_State* L);
extern int LuaTONG_GetLWeekGoalLevel(Lua_State* L);
extern int LuaTONG_GetLWeekGoalTotal(Lua_State* L);
extern int LuaTONG_GetLWeekGoalPlayer(Lua_State* L);
extern int LuaTONG_GetLWeekGoalValue(Lua_State* L);
extern int LuaTONG_GetLWeekGoalPriceTong(Lua_State* L);
extern int LuaTONG_GetLWeekGoalPricePlayer(Lua_State* L);
extern int LuaTONG_GetAnnouncement(Lua_State* L);
extern int LuaTONG_ApplySetBuildFund(Lua_State* L);
extern int LuaTONG_ApplySetCurWeekGoalLevel(Lua_State* L);
extern int LuaTONG_ApplySetDay(Lua_State* L);
extern int LuaTONG_ApplySetWeek(Lua_State* L);
extern int LuaTONG_ApplySetMaintainFund(Lua_State* L);
extern int LuaTONG_ApplySetOccupyCityDay(Lua_State* L);
extern int LuaTONG_ApplySetPauseState(Lua_State* L);
extern int LuaTONG_ApplySetPerStandFund(Lua_State* L);
extern int LuaTONG_ApplySetStoredBuildFund(Lua_State* L);
extern int LuaTONG_ApplySetStoredOffer(Lua_State* L);
extern int LuaTONG_ApplySetTotalBuildFund(Lua_State* L);
extern int LuaTONG_ApplySetWarBuildFund(Lua_State* L);
extern int LuaTONG_ApplySetWeekBuildFund(Lua_State* L);
extern int LuaTONG_ApplySetWeekBuildUpper(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalEvent(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalLevel(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalTotal(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalPlayer(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalValue(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalPriceTong(Lua_State* L);
extern int LuaTONG_ApplySetWeekGoalPricePlayer(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalEvent(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalLevel(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalTotal(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalPlayer(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalValue(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalPriceTong(Lua_State* L);
extern int LuaTONG_ApplySetLWeekGoalPricePlayer(Lua_State* L);
extern int LuaTONG_ApplySetTongMap(Lua_State* L);
extern int LuaTONG_ApplySetTongMapBan(Lua_State* L);
extern int LuaTONG_ApplySetAnnouncement(Lua_State* L);
extern int LuaTONG_ApplySetStunt(Lua_State* L);
extern int LuaTONG_ApplyAddBuildFund(Lua_State* L);
extern int LuaTONG_ApplyAddWarBuildFund(Lua_State* L);
extern int LuaTONG_ApplyAddPerStandFund(Lua_State* L);
extern int LuaTONG_ApplyAddDay(Lua_State* L);
extern int LuaTONG_ApplyAddWeek(Lua_State* L);
extern int LuaTONG_ApplyAddStoredBuildFund(Lua_State* L);
extern int LuaTONG_ApplyAddStoredOffer(Lua_State* L);
extern int LuaTONG_ApplyAddTotalBuildFund(Lua_State* L);
extern int LuaTONG_ApplyAddWeekBuildFund(Lua_State* L);
extern int LuaTONG_ApplyAddWeekGoalValue(Lua_State* L);
extern int LuaTONG_ApplyAddLWeekGoalValue(Lua_State* L);
extern int LuaTONG_ApplyAddEventRecord(Lua_State* L);
extern int LuaTONG_ApplyAddHistoryRecord(Lua_State* L);
extern int LuaTONG_AddTaskTemp(Lua_State* L);
extern int LuaTONG_WriteLog(Lua_State* L);
extern int LuaTONG_ApplyInit(Lua_State* L);
extern int LuaTONG_ApplyUpgrade(Lua_State* L);
extern int LuaTONG_ApplyDegrade(Lua_State* L);
extern int LuaTONG_ApplyMaintain(Lua_State* L);
extern int LuaTONG_ApplyWeeklyMaintain(Lua_State* L);
extern int LuaTONG_ApplyDeleteMap(Lua_State* L);
extern int LuaTONG_ApplyKickMember(Lua_State* L);
extern int LuaTONG_ApplyDeleteMember(Lua_State* L);
extern int LuaTONG_ApplyCreatMap(Lua_State* L);
extern int LuaTONG_ChangeAllMemberFeature(Lua_State* L);
extern int LuaTONG_ContributeOffer(Lua_State* L);
extern int LuaTONG_DistributeOfferToGroup(Lua_State* L);
extern int LuaTONG_DistributeOfferToMember(Lua_State* L);
extern int LuaTONGM_GetJoinTime(Lua_State* L);
extern int LuaTONGM_GetJoinDay(Lua_State* L);
extern int LuaTONGM_GetMoney(Lua_State* L);
extern int LuaTONGM_GetLWeekGoalOffer(Lua_State* L);
extern int LuaTONGM_GetLWeeklyOffer(Lua_State* L);
extern int LuaTONGM_GetLastOnlineDate(Lua_State* L);
extern int LuaTONGM_GetRetireDate(Lua_State* L);
extern int LuaTONGM_ApplySetLWeekGoalOffer(Lua_State* L);
extern int LuaTONGM_ApplySetLWeeklyOffer(Lua_State* L);
extern int LuaTONGM_ApplyAddUTaskValue(Lua_State* L);
extern int LuaTONGM_ApplyAddOfferEx(Lua_State* L);
extern int LuaTWS_IsExist(Lua_State* L);
extern int LuaTWS_IsOpen(Lua_State* L);
extern int LuaTWS_GetType(Lua_State* L);
extern int LuaTWS_GetLevel(Lua_State* L);
extern int LuaTWS_GetWorkshopCount(Lua_State* L);
extern int LuaTWS_GetFirstWorkshop(Lua_State* L);
extern int LuaTWS_GetNextWorkshop(Lua_State* L);
extern int LuaTWS_GetDayOutput(Lua_State* L);
extern int LuaTWS_GetUseLevel(Lua_State* L);
extern int LuaTWS_GetUseLevelSet(Lua_State* L);
extern int LuaTWS_GetBuildingNpc(Lua_State* L);
extern int LuaTWS_SetBuildingNpc(Lua_State* L);
extern int LuaTWS_GetTaskValue(Lua_State* L);
extern int LuaTWS_GetUTaskValue(Lua_State* L);
extern int LuaTWS_ApplySetTaskValue(Lua_State* L);
extern int LuaTWS_ApplyAddTaskValue(Lua_State* L);
extern int LuaTWS_ApplyAddUTaskValue(Lua_State* L);
extern int LuaTWS_ApplySetDayOutput(Lua_State* L);
extern int LuaTWS_ApplyAddDayOutput(Lua_State* L);
extern int LuaTWS_ApplySetUseLevel(Lua_State* L);
extern int LuaTWS_ApplySetUseLevelSet(Lua_State* L);
extern int LuaTWS_ApplyAdd(Lua_State* L);
extern int LuaTWS_ApplyRemove(Lua_State* L);
extern int LuaTWS_ApplyOpen(Lua_State* L);
extern int LuaTWS_ApplyClose(Lua_State* L);
extern int LuaTWS_ApplyUpgrade(Lua_State* L);
extern int LuaTWS_ApplyDegrade(Lua_State* L);
extern int LuaTWS_ApplyMaintain(Lua_State* L);
extern int LuaTWS_ApplyUse(Lua_State* L);
#endif

TLua_Funcs GameScriptFuns[] =
{
	{"Say", LuaSelectUI},
	{"SayNew", LuaSaySPR},
	{"SayImg", LuaSelectImage},
	{"Talk", LuaTalkUI},
	{"TalkEx",			LuaTalkUI2},
	{"GetTaskTemp", LuaGetTempTaskValue},
	{"SetTaskTemp", LuaSetTempTaskValue},
	{"Message", LuaMessage},
	{"GetBit",	LuaGetBit},
	{"GetByte",	LuaGetByte},
	{"SetBit",	LuaSetBit},
	{"SetByte",	LuaSetByte},
	{"Include",LuaIncludeFile},
	{"PutMessage", LuaSendMessageInfo},
	{"AddGlobalNews",LuaAddGlobalNews},
	{"AddGlobalTimeNews",LuaAddGlobalTimeNews},
	{"AddGlobalCountNews",LuaAddGlobalCountNews	},
	{"AddLocalNews",LuaAddLocalNews},
	{"AddLocalTimeNews",LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews	},
	{"AddGlobalNewsEx",	LuaAddGlobalNewsEx},
	{"AddGlobalTimeNewsEx",LuaAddGlobalTimeNews},
	{"AddGlobalCountNewsEx",LuaAddGlobalCountNewsEx	},
	{"AddLocalNewsEx",	LuaAddLocalNewsEx},
	{"AddLocalTimeNewsEx",LuaAddLocalTimeNewsEx},
	{"AddLocalCountNewsEx",LuaAddLocalCountNewsEx	},
#ifdef _SERVER
	{"ST_IsRideHorse", LuaST_IsRideHorse},//add by phong ki襲 check player tr猲 ng鵤
	{"ModifyAttrib",	LuaModifyAttrib}, //add by phong ki襲 v藅 ph萴 m竨 l韓 t鑞g kim
	{"GetEquipCount",           LuaCountEquipPlayer},
	{"RemoveAllItem", LuaRemoveAllItem},
	{"AddRepute", LuaModifyRepute},	
	{"GetRepute", LuaGetRepute},
	{"SetRepute", LuaSetPlayerReputeValue},
	{"SetReBorn", LuaSetPlayerReBornValue},
	{"GetReBorn",LuaGetPlayerReBornValue},
	{"GetReBornQPoint",LuaGetReBornQPoint},
	{"GetReBornJPoint",LuaGetReBornJPoint},
	{"AddReBorn", LuaSetPlayerReBornValue},
	{"ST_GetTransLifeCount",LuaGetPlayerReBornValue},
	{"GetReBornNum",LuaGetPlayerReBornNum},
	{"SetPlayerReBornNum",LuaSetPlayerReBornNum},
	{"AddFuYuan",		LuaAddPlayerFuYuanValue},
	{"GetFuYuan",		LuaGetPlayerFuYuanValue},
	{"SetFuYuan", LuaSetPlayerFuYuanValue},
	{"SetMeridian", LuaSetPlayerMeridianValue},
	{"GetGameTime",	LuaGetGameTime},
	{"GetExtPoint", LuaGetExtPoint},
	{"SetExtPoint", LuaSetExtPoint},
	{"AddExtPoint", LuaAddExtPoint},
	{"PayExtPoint", LuaPayExtPoint},
	{"IsMyItem",		LuaIsMyItem},
	{"AddSkillState",	LuaAddSkillState},
	{"SetProtectTime",	LuaSetProtectTime}, //vong tron bat tu, v遪g tr遪 b蕋 t?
	{"AddNpcSkillState",	LuaAddNpcSkillState},
	{"IgnoreState",		LuaIgnoreState},
	{"CastSkill",		LuaCastSkill},
	{"CastNpcSkill",		LuaNpcCastSkill},
	{"GetNpcIdx", LuaGetCurNpcIndex},
	{"SetTimer",		LuaSetTimer},		//SetTimer(时间量, 时间TaskId):给玩家打开计时器,时间到时将自动调用OnTimer函数
	{"StopTimer",		LuaStopTimer},		//StopTimer()：关闭当前玩家的计时器
	{"GetRestTime",		LuaGetRestTime},	//GetRestTime:获得计时器将触发的剩于时间	
	{"GetTimerId",		LuaGetCurTimerId},	//CurTimerId = GetTimerId():获得当前执行的计时器的id,如果没有则返回0
	{"GetTask",			LuaGetTaskValue},	//GetTask(任务号):获得当前玩家该任务号的值
	{"SetTask",			LuaSetTaskValue},	//SetTask(任务号,值):设置任务值
	{"IsCaptain",		LuaIsLeader},		//IsCaptain()是否为队长
	{"GetTeam",			LuaGetTeamId},		//GetTeam()
	{"GetTeamSize",		LuaGetTeamSize},	//GetTeamSize()
	{"LeaveTeam",		LuaLeaveTeam},		//LeaveTeam()
	{"GetTeamMem",		LuaGetTeamMem},
	{"Msg2Player",		LuaMsgToPlayer	},
	{"Msg2Team",		LuaMsgToTeam},
	{"Msg2SubWorld",	LuaMsgToSubWorld},
	{"Msg2Region",		LuaMsgToAroundRegion},
	{"Msg2GM",			LuaMsgToGameMaster}, //Msg2GM(StrInfo)
	{"Msg2IP",			LuaMsgToIP}, //Msg2IP(IP, ID, StrInfo)
	{"Msg2Faction",		LuaMsgToFaction},
	{"Msg2Tong",		LuaMsgToTong},
	{"GetInfo",			LuaGetPlayerInfo}, //str = GetInfo()
	{"GetIP",			LuaGetIP},
	{"GetHWID",			LuaGetHWID},
	{"GetNpcTimeIdle",	LuaGetNpcTimeIdle},
	{"SetNpcTimeIdle",	LuaSetNpcTimeIdle},
	{"OpenGiveBox",		LuaOpenGiveBox},
	{"EndGiveBox",		LuaEndGiveBox},
	{"RemoveItem",		LuaRemoveItemIdx},
	{"ConsumeItem",			LuaConsumeItem},
	{"DropAllPlayerItems", LuaPlayerDropAllItem},
	{"SetPlayerItemLock",			LuaSetPlayerItemLock},
	{"SetPlayerItemUnLock",			LuaSetPlayerItemUnLock},
	{"SetPlayerItemUnLockF",			LuaSetPlayerItemUnLockF},
	{"GetPlayerItemIsTimeLimit", LuaGetPlayerItemIsTimeLimit},
	{"GetPlayerItemIsLock",			LuaGetPlayerItemIsLock},
	{"GetPlayerItemHourLock",			LuaGetPlayerItemHourLock},
	{"GetPlayerItemSecondLock",			LuaGetPlayerItemSecondLock},
	{"RepairItemByCoin",			LuaRepairItemByCoin},
	{"RepairItemGetNumCoin",			LuaRepairItemGetNumCoin},
	{"SetMask",			LuaSetMask},
	{"ReSetMask",		LuaReSetMask},
	{"SetPos",			LuaSetPos},			//SetPos(x,y)
	{"GetPos",			LuaGetPos},			//GetPos() return x,y,subworldindex
	{"GetWorldPos",		LuaGetNewWorldPos},	//W,X,Y = GetWorldPos()
	{"NewWorld",		LuaEnterNewWorld},
	{"RandomNew",			LuaRandomNew},	//SetTask(任务号,值):设置任务值
	{"AddTrap",			LuaAddTrap},
	{"AddObj",			LuaAddObj},
	{"AddObstacle",			LuaAddObstacle}, //#Set V藅 C秐
	{"DropItem",		LuaDropItem},		//DropItem
	{"DropItemPUBG", LuaDropItemPUBG}, //DropItemPUBG DropItemPUBG(NpcIndex, 4, 353, 0, 0, 0, 0, nKind) nKind = gold or platina
	{"DropRateItem",		LuaDropRateItem},
	{"DropNpcMoney",	LuaDropNpcMoney},
	{"GetNpcLevel",			LuaGetNpcLevel},
	{"AddItem",			LuaAddItem},		//AddItem(nItemClass, nDetailType, nParticualrType, nLevel, nSeries, nLuck, nItemLevel..6)
	{"AddItem2",		LuaAddItem2 },		//AddItem2(nItemNature, nItemClass, nDetailType, nParticualrType, nLevel, nSeries, nLuck, nItemLevel..6)
	{"UpgradePlatinaItem", LuaUpgradePlatinaItem },
	{ "ResetGoldEquipItem", LuaResetGoldEquipItem },
	{"AddItemID",			LuaAddItemID },
	//{ "AddItemIdx",			LuaAddItemIdx },
	{"AddTimeItem",			LuaAddTimeItem},
	{"AddGoldItem",			LuaAddGoldItem},
	{"AddEventItem",	LuaAddEventItem	},	//AddEventItem(事件物品ID)
	{"AddMineItem",		LuaAddMineItem	},	//AddMineItem(事件物品ID)	
	{"DelItem",			LuaDelItem},
	{"HaveItem",		LuaHaveItem},
	{"GetNameItem",		LuaGetNameItem},
	{"GetItemName",		LuaGetNameItem},
	{"GetTaskItemCount",	LuaGetTaskItemCount},
	{"GetItemCount",		LuaGetItemCount},
	{"CheckRoom",			LuaCheckRoom},
	{"CheckFreeBoxItem",LuaCheckFreeBoxItem},
	{"CalcFreeItemCellCount",LuaCalcFreeItemCellCount},
	{"FindItem",		LuaFindItem},
	{"FindItemEx",		LuaFindItemEx},
	{"GetMantleItem",	LuaGetMantleItem},
	{"SetMantleItem",	LuaSetMantleItem},
	{"GetParamItem",	LuaGetParamItem},
	{"SetParamItem",	LuaSetParamItem},
	{ "SetItemParam",		LuaSetItemParam },
	{ "GetItemParam",		LuaGetItemParam },
	{"GetGlowLightItem",	LuaGetGlowLightItem},
	{"SetGlowLightItem",	LuaSetGlowLightItem},
	{"AddTranslife", LuaAddPlayerTranslifeValue},
	{"GetTranslife",LuaGetPlayerTranslifeValue},
	{"GetMateName",		LuaGetMateName},
	{"SetMateName", LuaMakeMate}, // ret = MakeMate(matename);
	{"DelMateName", LuaDeleteMate}, // ret = DeleteMate(matename);
	{"GetPlayerNpcIdx",	LuaGetPlayerNpcIdx},
	{"FindPlayer",		LuaFindPlayer},
	{"GetCurServerSec",		LuaGetCurServerSec},
	{"SetNumber",			LuaSetNumber},
	{"GetNumber",			LuaGetNumber},
	{ "SetNumber2",			LuaSetNumber2},
	{ "GetNumber2",			LuaGetNumber2},
	{"GetROItem",		LuaGetROItem},
	{"GetCBItem",		LuaGetCBItem},
	{"GetBASItem",		LuaGetBASItem},		// option base
	{"GetOTItem",		LuaGetOTItem},
	{"GetOTLVItem",		LuaGetOTLVItem},

	{ "GetMagicAttrib",		LuaGetMagicAttrib },
	{ "SetMagicAttrib",		LuaSetMagicAttrib },

	{"AddMagic",		LuaAddMagic},		//AddMagic
	{"DelMagic",		LuaDelMagic},		//DelMagic
	{"HaveMagic",		LuaHaveMagic},		//HaveMagic
	{"GetMagicLevel",	LuaGetMagicLevel},	//GetMagicLevel
	{ "DelAllMagic",		LuaDelAllMagic },    //删除全部技能	
	{"AddMagicPoint",	LuaAddMagicPoint},
	{"GetMagicPoint",	LuaGetMagicPoint},
	{"CheckItemEquipCS",LuaCheckItemEquipCS},
	{"IncSkill",		LuaIncSkill},
	{"IncSkillExp",		LuaIncSkillExp},
	{"GetNpcExpRate",	LuaGetNpcExpRate},
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx
	{"SubWorldIdx2ID",	LuaSubWorldIndexToID}, //SubWorldIdx2ID
	{"AddLeadExp",		LuaAddLeadExp},
	{"GetLeadLevel",	LuaGetLeadLevel},
	{"SetFightState",	LuaSetFightState},
	{"GetFightState",	LuaGetFightState},
	{"SetNpcWalkOriginPos",	LuaSetNpcWalkOriginPos},

	{"SetNpcDamageReduction", LuaSetNpcDamageReduction},
	{"SetNpcAIType", LuaSetNpcAIType},
	{ "NpcWalk",	LuaNpcWalk },
	{ "NpcChat", LuaNpcChat },
	{"SetNpcOwner",		LuaSetNpcOwner},
	{"GetNpcOwner",		LuaGetNpcOwner},
	{"SetNpcFindPathTime", LuaSetNpcFindPathTime},
	{"AddNpc",			LuaAddNpc},			//AddNpc
	{"NoReloadNpcAttr", LuaNoReloadNpcAttr },
	{"DelNpc",			LuaDelNpc},			//DelNpc(Npcid)
	{"SetNpcBoss",	LuaSetNpcGoldBoss},
	{"GetNpcBoss",	LuaGetNpcBoss},
	{"SetNpcScript",	LuaSetNpcActionScript},	//SetNpcScript
	{ "SetNpcActiveRange",	LuaSetNpcActiveRange },
	{"SetNpcDropScript",LuaSetNpcDropScript},
	{"SetNpcSeries",	LuaSetNpcSeries}, // //SetNpcSeries(npcid, series)
	{"GetNpcSeries",	LuaGetNpcSeries},//GetNpcSeries(IndexNpc)
	{"SetNpcName",		LuaSetNpcName}, // //SetNpcName(npcid, name)
	{"GetNpcName",		LuaGetNpcName},
	{"SetNpcParam",		LuaSetNpcParam},
	{"SetNpcValue",		LuaSetNpcParam},
	{"GetNpcParam",		LuaGetNpcParam},
	{"GetNpcValue",		LuaGetNpcParam},
	{"SetNpcTimer",		LuaSetNpcTimer},
	{"GetNpcTimer",		LuaGetNpcTimer},
	{"SetNpcExp",		LuaSetNpcExp},
	{"SetNpcLife",		LuaSetNpcLife},
	{"GetNpcLife",		LuaGetNpcLife},
	{"SetNpcReplenish",	LuaSetNpcLifeReplenish},
	{"SetNpcAR",		LuaSetNpcAR},
	{"SetNpcDefense",	LuaSetNpcDefense},
	{"SetNpcDamage",	LuaSetNpcDamage},
	{"SetNpcDmgEx",		LuaSetNpcDmgEx},
	{"SetNpcResist",	LuaSetNpcResist},
	{"SetNpcRevTime",	LuaSetNpcRevTime},	//Set th阨 gian h錳 sinh
	{"SetNpcSpeed",		LuaSetNpcSpeed},
	{"SetPlayerSpeed",	LuaSetPlayerSpeed },
	{"SetNpcHitRecover",LuaSetNpcHitRecover},
	{ "SetNpcHonorId", LuaSetNpcHonorId },	//SetNpcHonorId(npcidex, honorid)
	{ "SetPlayerHonorId", LuaSetPlayerHonorId },	//SetPlayerHonorId(playerindex, honorid)
	{"GetNpcID",		LuaGetNpcID},
	{"SetNpcSkill",		LuaSetNpcSkill},
	{"RemoveNpcSkill",		LuaRemoveNpcSkill},
	{"NpcNewWorld",		LuaNpcEnterNewWorld},
	{"SetNpcPos",		LuaSetNpcPos},			//SetNpcPos(x,y)进入某点	
	{"GetNpcPos",		LuaGetNpcPos},			//GetNpcPos() return x,y,subworldindex
	{"GetNpcWorldPos",	LuaGetNpcNewWorldPos},
	{"GetNpcCamp",		LuaGetNpcCamp},
	{"SetNpcCamp",		LuaSetNpcCamp},
	{"GetNpcCurCamp",	LuaGetNpcCurCamp},
	{"FindNpcFrID",		LuaFindNpcFrID},	// id
	{"FindNearNpc",		LuaFindNearNpc}, // indexsetting
	{"FindAroundNpc",	LuaFindAroundNpc},	// indexsetting
	{"SetNpcFightState", LuaSetNpcFightState},
	{"GetNpcFightState", LuaGetNpcFightState},
	{"Rev2Pos",			LuaGetPlayerRevivalPos},
	{"SetRevPos",		LuaSetPlayerRevivalPos},//SetRevPos(点位置X，点位置Y)设置玩家的当前世界的等入点位置
	{"SetTempRevPos",	LuaSetDeathRevivalPos}, //SetTempRevPos(subworldid, x, y ) or SetTempRevPos(id);
	{"GetCurCamp",		LuaGetPlayerCurrentCamp},//GetCurCamp()获得玩家的当前阵营
	{"GetCamp",			LuaGetPlayerCamp	},//GetCamp()获得玩家阵营
	{"SetCurCamp",		LuaSetPlayerCurrentCamp},//SetCurCamp(阵营号):设置玩家当前阵营
	{"SetCamp",			LuaSetPlayerCamp},		  //SetCamp(阵营号):设置阵营	
	{"RestoreCamp",		LuaRestorePlayerCamp	},//RestoreCamp()恢复阵营
	{"GetFaction",		LuaGetPlayerFaction,	},//GetFaction()获得玩家的门派名 
	{"GetFactionValueName",		LuaGetPlayerFactionValueName,	},//GetFaction()获得玩家的门派名 
	{"GetFactionNo",	LuaGetPlayerFactionNo},
	{"GetFirstAddFaction",	LuaGetPlayerFirstAddFaction},
	{"SetFaction",		LuaChangePlayerFaction},  //SetFaction(门派名):设置玩家门派名
	{"ClearFactionIfnfo",LuaClearFactionIfnfo },
	{"GetColdR",		LuaGetPlayerColdResist},	
	{"SetColdR",		LuaGetPlayerColdResist},
	{"GetFireR",		LuaGetPlayerFireResist	},
	{"SetFireR",		LuaSetPlayerFireResist	},
	{"GetLightR",		LuaGetPlayerLightResist	},
	{"SetLightR",		LuaSetPlayerLightResist},
	{"GetPoisonR",		LuaGetPlayerPoisonResist},
	{"SetPoisonR",		LuaSetPlayerPoisonResist},
	{"GetPhyR",			LuaGetPlayerPhysicsResist	},
	{"SetPhyR",			LuaSetPlayerPhysicsResist	},
	{"GetExp",			LuaGetPlayerExp	},			//GetExp():获得玩家的当前经验值
	{"GetNextExp",		LuaGetNextExp},			//GetExp():获得玩家的当前经验值
	{"AddExp",			LuaModifyPlayerExp},		//AddExp(经验值，对方等级，是否组队共享经验值)
	{"GetNpcSkillsExpRate",		LuaGetNpcExpSkillsRate },			//GetExp2Skill():获得玩家的当前经验值
	{ "GetNpcVip",		LuaGetNpcVip },			//GetExp2Skill():获得玩家的当前经验值
	{"AddOwnExp",		LuaAddOwnExp	},			//AddOwnExp(Exp)，给玩家直接加经验
	{"AddSumExp",		LuaAddSumExp	},
	{"SetLevel",		LuaSetLevel },
	{"ResetBaseAttrib",	LuaResetBaseAttribute },
	{"SetBasePoint",	  LuaSetBasePoint },       //设置基本属性点
	{"GetLife",			LuaGetPlayerLife},			//GetLife()获得玩家的生命值
	{"RestoreLife",		LuaRestorePlayerLife},		//RestoreLife()恢复玩家的生命
	{"GetMana",			LuaGetPlayerMana},			//GetMana()获得玩家的Mana
	{"RestoreMana",		LuaRestorePlayerMana},		//RestoreMana()恢复玩家的Mana
	{"GetStamina",		LuaGetPlayerStamina},		//GetStamina()获得玩家Stamina
	{"RestoreStamina",	LuaRestorePlayerStamina},	//RestoreMana()恢复玩家的Stamina
	{"GetDefend",		LuaGetPlayerDefend},		//GetDefend()获得玩家的防御力
	{"GetSex",			LuaGetPlayerSex},			//GetSex()获得玩家的性别
	{"SetSex",          LuaSetPlayerSex},
	{"GetSeries",		LuaGetPlayerSeries},		//GetSeries()获得玩家的系0man/1woman
	{"SetSeries",		LuaSetPlayerSeries},		//SetSeries(性别号)
	{"GetName",			LuaGetPlayerName},			//GetName()获得玩家的姓名
	{"GetAccount",		LuaGetPlayerAccount},		//Edit by phong kieu ham lua viet them GetAccount
	{"GetUUID",			LuaGetPlayerID},			//GetUUID()获得玩家的唯一ID
	{"GetLeadExp",		LuaGetPlayerLeadExp},		//GetLeadExp()获得玩家的统率经验值
	{"GetLeadLevel",	LuaGetPlayerLeadLevel},		//GetLeadLevel()获得玩家的统率等级
	{"GetLevel",		LuaGetLevel},				//GetLevel()GetPlayers Level
	{"KiemTraKhoaRuong",LuaGetKhoa},				//Get status lock
	{"SetPKState",		LuaSetPKState},				//Set PK status
	{"GetPKState",		LuaGetPKState},				//Get PK status
	{"SetPKMode", 		LuaSetPlayerPKState},  //pkValue = GetPK() 
	{"GetRestAP",		LuaGetPlayerRestAttributePoint},//GetRestAP()获得玩家的剩于属性点数
	{"GetRestSP",		LuaGetPlayerRestSkillPoint},	//GetRestSP()获得玩家的剩于技能点数	
	{"GetLucky",		LuaGetPlayerLucky},			//GetLucky()获得玩家的幸运值
	{"GetEng",			LuaGetPlayerEngergy},		//GetEng()获得玩家的力量值Eng
	{"AddEng",			LuaSetPlayerEngergy},		//AddEng(Value)获得玩家的力量值Eng
	{"GetDex",			LuaGetPlayerDexterity},		//GetDex()获得玩家的Dex
	{"AddDex",			LuaSetPlayerDexterity},		//AddDex(Value)获得玩家的力量值Eng
	{"GetStrg",			LuaGetPlayerStrength},		//GetStrg()
	{"AddStrg",			LuaSetPlayerStrength},		//AddStrg(Value)获得玩家的力量值Eng
	{"GetVit",			LuaGetPlayerVitality},		//GetVit()
	{"AddVit",			LuaSetPlayerVitality},		//AddVit(Value)获得玩家的力量值Eng
	{"GetCash",			LuaGetPlayerCashMoney},		//GetCash()获得玩家的现金
	{"GetSaveMoney",	LuaGetSaveMoney},
	{"Pay",				LuaPlayerPayMoney},			//Pay(金额数)扣除玩家金钱成功返回1，失败返回0
	{"Earn",			LuaPlayerEarnMoney},		//Earn(金额数)增加玩家金钱
	{"PrePay",			LuaPlayerPrePayMoney},		//付定金，成功返回1，失败返回0
	{"Sale",			LuaSale},					//Sale(SaleId)买卖，SaleId为便卖的物品信息列表id
	{"NewSale",			LuaNewSale},
	{"UseTownPortal",	LuaUseTownPortal	},
	{"ReturnFromPortal",LuaReturnFromTownPortal	},
	{"SetNpcCurCamp",	LuaSetNpcCurCamp},
	{"OpenBox",			LuaOpenBox},
	{"AddStation",		LuaAddPlayerStation},
	{"AddTermini",		LuaAddPlayerWayPoint},//add nh鱪g n琲  甶 qua
	{"GetStation",		LuaGetPlayerStation	},
	{"GetStationCount", LuaGetPlayerStationCount},
	{"GetCityCount", LuaGetAllStationCount},
	{"GetCity", LuaGetCity},
	{"OpenResetPass",	LuaOpenResetPass},			//lua reset mk ruong
	{"OpenDatauBox",	LuaDaTauBox},
	{"OpenDatauBox1",	LuaDaTau1Box},
	{"OpenQuestFinish",	LuaOpenQuestFinish},
	{"OpenTrembleItem",		LuaOpenTrembleItem},
	{"EndTrembleItem",		LuaEndTrembleItem},
	{"OpenCompoundItem", LuaOpenCompoundItem},
	{"GetIdItem", LuaGetIdItem},
	{"ChangeInfoItem", LuaChangeInfoItem},
	{"GetWayPoint",		LuaGetPlayerWayPoint},
	{"GetStationName",	LuaGetStationName},
	{"GetWayPointName", LuaGetWayPointName},
	{"GetPrice2Station", LuaGetPriceToStation},
	{"GetPrice2WayPoint", LuaGetPriceToWayPoint	},
	{"GetStationPos",	LuaGetStationPos},
	{"GetWayPointPos",	LuaGetWayPointPos},
	{"GetPlayerCount",	LuaGetPlayerCount},
	{"GetSubWorldPlayerCount",	LuaGetSubWorldPlayerCount },
	{"GetSubWorldLastPlayer",	LuaGetSubWorldLastPlayer },
	{"GetWordRank",		LuaGetWordRank},//GetRankInW X誴 h筺g th?gi韎
	{"OpenGetString",	LuaOpenGetString},
	{"OpenGetNumber",	LuaOpenGetNumber},
	{"GetStringFromUI", LuaGetStringFromUI},
	{"GetNumberFromUI", LuaGetNumberFromUI},
	{"GetRank",			LuaGetRank},//GetRank()
	{"SetRank",			LuaSetRank},//SetRank(id)
	{"GetPlayerTitle",			LuaGetPlayerTitle}, //#PlayerTitle
	{"SetPlayerTitle",			LuaSetPlayerTitle},
	{"RemovePlayerTitle",			LuaRemovePlayerTitle},
	{"SetCurPlayerTitle",			LuaSetCurPlayerTitle},
	{"GetRankBattle",			LuaGetRankBattle}, //#RankBattle 
	{"SetRankBattle",			LuaSetRankBattle},
	{"RemoveRankBattle",			LuaRemoveRankBattle},
	{"SetPropState",	LuaSetObjPropState},//SetPropState( hide = 1) hide obj
	{"GetServerName",	LuaGetServerName},
	{"GetExItem",		LuaGetExItem},//GetExItem()
	{"SetExItem",		LuaSetExItem},//SetExItem(id)
	{"GetExBox",		LuaGetExBox},//GetExItem()
	{"SetExBox",		LuaSetExBox},//SetExItem(id)
	//------------------Station Script ---------------
	{"GetWharfName",	LuaGetDockName},
	{"GetWharfCount",	LuaGetDockCount},
	{"GetWharfPrice",	LuaGetDockPrice},
	{"GetWharf",		LuaGetDock},
	{"GetWharfPos",		LuaGetDockPos},
	{"GetTerminiFState", LuaGetWayPointFightState},
	{"GetWayPointFight",	LuaGetWayPointFight},
	//------------------------------------------------
	{"KickOutPlayer",	LuaKickOutPlayer},
	{ "KickOutAccount",	LuaKickOutAccount },
	{"KickOutSelf",		LuaKickOutSelf},
	{"KickOutSelf2",	LuaKickOutSelf2},
	{"IsLixian",			LuaIsLixian},								//#uy thac
	{ "SetPlayerSaveState", LuaSetPlayerSaveState },
	{"GetSkillId",		LuaGetSkillIdInSkillList},
	{"GetSkillName",		LuaGetSkillName},
	{"SetSkillLevel",	LuaSetSkillLevel},
	{"SetChatFlag",		LuaSetPlayerChatForbiddenFlag},
	{"SaveNow",			LuaSaveNow},
	{"PaceBar",				LuaOpenTimeBox},
	{"TimeBox",				LuaOpenTimeBox},
	{"GetCountPlayerMax",	LuaGetCountPlayerMax},
	{"ReLoadScript", LuaReLoadScript},
	//	{"GetPramaItemIdx",LuaGetGetPramaItemIdx},

		//------------------------------------------------
		{"AddNote", LuaAddNote},
		//-----------------Mission Script-----------------
		{"GetMissionV", LuaGetMissionValue},//GetMissionV(Vid)
		{"GetMissionS", LuaGetMissionString},
		{"SetMission", LuaSetMission},//SetMissionV(Vid, Value)
		{"GetGlbMissionV", LuaGetGlobalMissionValue	},
		{"GetGlbMissionVC", LuaGetGlobalMissionValueC},
		{"SetGlbMissionV", LuaSetGlobalMissionValue	},
		{"SetGlbMission", LuaSetGlobalMission	},
		{"OpenMission", LuaInitMission},//OpenMission(missionid)
		{"RunMission", LuaRunMission},
		{"CloseMission", LuaCloseMission},//CloseMission(missionid)
		{"GetMissionName", LuaGetMissionName},
		{"StartMissionTimer", LuaStartMissionTimer},////StartMissionTimer(missionid, timerid, time)
		{"StopMissionTimer", LuaStopMissionTimer},
		{"GetMSRestTime", LuaGetMissionRestTime}, //GetMSRestTime(missionid, timerid)
		{"GetMSIdxGroup",LuaGetPlayerMissionGroup},//GetPlayerGroup(missionid, playerid);
		{"IsMission",LuaIsMission},
		{"GetMSLadder",LuaGetMSLadder}, //add by phong ki襲 using t鑞g kim
		{"AddMSPlayer", LuaAddMissionPlayer},			//AddMSPlayer(MissionId, PlayerIndex, groupid); / AddMSPlayer(MissionId, groupid)
		{"DelMSPlayer", LuaRemoveMissionPlayer},
		{"AddMSNpc",	LuaAddMissionNpc},
		{"DelMSNpc",	LuaRemoveMissionNpc},
		{"SetMSGroup", LuaSetMissionGroup},
		{"GetNextPlayer", LuaGetNextPlayer},
		{"PIdx2MSDIdx", LuaGetMissionPlayer_DataIndex},//(missionid, pidx)
		{"MSDIdx2PIdx", LuaGetMissionPlayer_PlayerIndex},//(missionid, dataidx)
		{"NpcIdx2PIdx", LuaNpcIndexToPlayerIndex},
		{"GetMSPlayerCount", LuaMissionPlayerCount},//GetMSPlayerCount(missionid, group = 0)
		{"GetMSNpcCount", LuaMissionNpcCount},
		{"RevivalAllNpc",	LuaRevivalAllNpc},
		{"DelAllNpc",	      LuaDelAllNpcInWro},    //删除NPC
		{"DelAllNpcName",	      LuaDelAllNpcInWroName },
		{"SetPMParam", LuaSetMissionPlayerParam }, //add by phong ki襲 using t鑞g kim
		{"GetPMParam", LuaGetMissionPlayerParam},
		{"Msg2MSGroup", LuaMissionMsg2Group},
		{"Msg2MSAll", LuaMissionMsg2All},
		{"Msg2MSPlayer", LuaMissionMsg2Player},
		{"UpdateBattleBox",	LuaUpdateBattleBox},
		{"SetDeathScript", LuaSetPlayerDeathScript},
		{"SetLogoutScript", LuaSetLogoutScript},
		{"HideNpc", LuaHideNpc}	,//HideNpc(npcindex/npcname, hidetime)
		{"SetLogoutRV", LuaSetPlayerRevivalOptionWhenLogout},
		{"SetCreateTeam",LuaSetCreateTeamOption},
		{"GetPK", LuaGetPlayerPKValue},  //pkValue = GetPK() 
		{"SetPK", LuaSetPlayerPKValue}, //SetPK(pkValue)
		//------------------------------------------------
		{"ShowLadder", LuaShowLadder}, //ShowLadder(LadderCount, LadderId1,LadderId2,...);
		//------------------------------------------------
		{"OpenTong",	LuaOpenTong},	//OpenTong()通知玩家打开帮会界面
		{"GetTongName",			LuaGetTongName},
		{"GetTongInfo"	 ,	LuaGetTongInfo},
		{"GetTongFlag",			LuaGetTongFlag},
		{"SetViewTongOwnCity", LuaSetViewTongOwnCity},
		{"SetThueTongOwnCity", LuaSetThueTongOwnCity},
		{"SetPunish",	LuaSetDeathPunish},// SetPunish(0/1) 0表示不受任何惩罚
		{ "GetTongLevel",			LuaGetTongLevel },
		{ "SetTongLevel",			LuaSetTongLevel },
		{ "GetTongExp",			LuaGetTongExp },
		{ "SetTongExp",			LuaSetTongExp },
#ifdef _SERVER
		// ==== JX2 port: bang hoi kieu JX2 (xem KTongJX2.cpp) ====
		{ "TONG_IsExist",	LuaTONG_IsExist },
		{ "TONG_GetName",	LuaTONG_GetName },
		{ "TONG_Name2ID",	LuaTONG_Name2ID },
		{ "TONG_GetTongCount",	LuaTONG_GetTongCount },
		{ "TONG_GetFirstTong",	LuaTONG_GetFirstTong },
		{ "TONG_GetNextTong",	LuaTONG_GetNextTong },
		{ "TONG_GetTongByRoleName",	LuaTONG_GetTongByRoleName },
		{ "TONG_GetMemberCount",	LuaTONG_GetMemberCount },
		{ "TONG_GetOnlineCount",	LuaTONG_GetOnlineCount },
		{ "TONG_GetFirstMember",	LuaTONG_GetFirstMember },
		{ "TONG_GetNextMember",	LuaTONG_GetNextMember },
		{ "TONG_GetMaster",	LuaTONG_GetMaster },
		{ "TONG_GetTaskValue",	LuaTONG_GetTaskValue },
		{ "TONG_GetUTaskValue",	LuaTONG_GetUTaskValue },
		{ "TONG_GetTaskTemp",	LuaTONG_GetTaskTemp },
		{ "TONG_SetTaskTemp",	LuaTONG_SetTaskTemp },
		{ "TONG_GetCredit",	LuaTONG_GetCredit },
		{ "TONG_GetExp",	LuaTONG_GetExp },
		{ "TONG_GetUnionID",	LuaTONG_GetUnionID },
		{ "TONG_GetWarState",	LuaTONG_GetWarState },
		{ "TONG_GetBuildFund",	LuaTONG_GetBuildFund },
		{ "TONG_GetBuildLevel",	LuaTONG_GetBuildLevel },
		{ "TONG_GetWarBuildFund",	LuaTONG_GetWarBuildFund },
		{ "TONG_GetMaintainFund",	LuaTONG_GetMaintainFund },
		{ "TONG_GetPerStandFund",	LuaTONG_GetPerStandFund },
		{ "TONG_GetStoredOffer",	LuaTONG_GetStoredOffer },
		{ "TONG_GetStoredBuildFund",	LuaTONG_GetStoredBuildFund },
		{ "TONG_GetDay",	LuaTONG_GetDay },
		{ "TONG_GetWeek",	LuaTONG_GetWeek },
		{ "TONG_GetWeekBuildFund",	LuaTONG_GetWeekBuildFund },
		{ "TONG_GetWeekBuildUpper",	LuaTONG_GetWeekBuildUpper },
		{ "TONG_GetTotalBuildFund",	LuaTONG_GetTotalBuildFund },
		{ "TONG_GetPauseState",	LuaTONG_GetPauseState },
		{ "TONG_GetTongMap",	LuaTONG_GetTongMap },
		{ "TONG_GetTongMapTemplate",	LuaTONG_GetTongMapTemplate },
		{ "TONG_GetTongMapBan",	LuaTONG_GetTongMapBan },
		{ "TONG_GetOccupyCityDay",	LuaTONG_GetOccupyCityDay },
		{ "TONG_GetMoney",	LuaTONG_GetMoney },
		{ "TONG_GetStandFund",	LuaTONG_GetStandFund },
		{ "TONG_ApplySetTaskValue",	LuaTONG_ApplySetTaskValue },
		{ "TONG_ApplyAddTaskValue",	LuaTONG_ApplyAddTaskValue },
		{ "TONG_ApplyAddUTaskValue",	LuaTONG_ApplyAddUTaskValue },
		{ "TONG_ApplySetMoney",	LuaTONG_ApplySetMoney },
		{ "TONG_ApplyAddMoney",	LuaTONG_ApplyAddMoney },
		{ "TONGM_IsExist",	LuaTONGM_IsExist },
		{ "TONGM_GetName",	LuaTONGM_GetName },
		{ "TONGM_GetFigure",	LuaTONGM_GetFigure },
		{ "TONGM_GetSex",	LuaTONGM_GetSex },
		{ "TONGM_GetOnline",	LuaTONGM_GetOnline },
		{ "TONGM_GetOffer",	LuaTONGM_GetOffer },
		{ "TONGM_GetTaskValue",	LuaTONGM_GetTaskValue },
		{ "TONGM_GetUTaskValue",	LuaTONGM_GetUTaskValue },
		{ "TONGM_CheckRight",	LuaTONGM_CheckRight },
		{ "TONGM_ApplySetTaskValue",	LuaTONGM_ApplySetTaskValue },
		{ "TONGM_ApplyAddTaskValue",	LuaTONGM_ApplyAddTaskValue },
		{ "TONGM_ApplyAddOffer",	LuaTONGM_ApplyAddOffer },
		{ "TONGM_ApplySetRetireDate",	LuaTONGM_ApplySetRetireDate },
		{ "TONGM_ApplySetLastOnlineDate",	LuaTONGM_ApplySetLastOnlineDate },
		{ "TONGM_ApplyAddRight",	LuaTONGM_ApplyAddRight },
		{ "TONGM_ApplyDelRight",	LuaTONGM_ApplyDelRight },
		// ==== JX2 port dot 2 ====
		{ "TONG_GetSelfCamp",	LuaTONG_GetSelfCamp },
		{ "TONG_GetCurCamp",	LuaTONG_GetCurCamp },
		{ "TONG_GetExpLevel",	LuaTONG_GetExpLevel },
		{ "TONG_GetPremium",	LuaTONG_GetPremium },
		{ "TONG_GetCurWeekGoalLevel",	LuaTONG_GetCurWeekGoalLevel },
		{ "TONG_GetWeekGoalEvent",	LuaTONG_GetWeekGoalEvent },
		{ "TONG_GetWeekGoalLevel",	LuaTONG_GetWeekGoalLevel },
		{ "TONG_GetWeekGoalTotal",	LuaTONG_GetWeekGoalTotal },
		{ "TONG_GetWeekGoalPlayer",	LuaTONG_GetWeekGoalPlayer },
		{ "TONG_GetWeekGoalValue",	LuaTONG_GetWeekGoalValue },
		{ "TONG_GetWeekGoalPriceTong",	LuaTONG_GetWeekGoalPriceTong },
		{ "TONG_GetWeekGoalPricePlayer",	LuaTONG_GetWeekGoalPricePlayer },
		{ "TONG_GetLWeekGoalEvent",	LuaTONG_GetLWeekGoalEvent },
		{ "TONG_GetLWeekGoalLevel",	LuaTONG_GetLWeekGoalLevel },
		{ "TONG_GetLWeekGoalTotal",	LuaTONG_GetLWeekGoalTotal },
		{ "TONG_GetLWeekGoalPlayer",	LuaTONG_GetLWeekGoalPlayer },
		{ "TONG_GetLWeekGoalValue",	LuaTONG_GetLWeekGoalValue },
		{ "TONG_GetLWeekGoalPriceTong",	LuaTONG_GetLWeekGoalPriceTong },
		{ "TONG_GetLWeekGoalPricePlayer",	LuaTONG_GetLWeekGoalPricePlayer },
		{ "TONG_GetAnnouncement",	LuaTONG_GetAnnouncement },
		{ "TONG_ApplySetBuildFund",	LuaTONG_ApplySetBuildFund },
		{ "TONG_ApplySetCurWeekGoalLevel",	LuaTONG_ApplySetCurWeekGoalLevel },
		{ "TONG_ApplySetDay",	LuaTONG_ApplySetDay },
		{ "TONG_ApplySetWeek",	LuaTONG_ApplySetWeek },
		{ "TONG_ApplySetMaintainFund",	LuaTONG_ApplySetMaintainFund },
		{ "TONG_ApplySetOccupyCityDay",	LuaTONG_ApplySetOccupyCityDay },
		{ "TONG_ApplySetPauseState",	LuaTONG_ApplySetPauseState },
		{ "TONG_ApplySetPerStandFund",	LuaTONG_ApplySetPerStandFund },
		{ "TONG_ApplySetStoredBuildFund",	LuaTONG_ApplySetStoredBuildFund },
		{ "TONG_ApplySetStoredOffer",	LuaTONG_ApplySetStoredOffer },
		{ "TONG_ApplySetTotalBuildFund",	LuaTONG_ApplySetTotalBuildFund },
		{ "TONG_ApplySetWarBuildFund",	LuaTONG_ApplySetWarBuildFund },
		{ "TONG_ApplySetWeekBuildFund",	LuaTONG_ApplySetWeekBuildFund },
		{ "TONG_ApplySetWeekBuildUpper",	LuaTONG_ApplySetWeekBuildUpper },
		{ "TONG_ApplySetWeekGoalEvent",	LuaTONG_ApplySetWeekGoalEvent },
		{ "TONG_ApplySetWeekGoalLevel",	LuaTONG_ApplySetWeekGoalLevel },
		{ "TONG_ApplySetWeekGoalTotal",	LuaTONG_ApplySetWeekGoalTotal },
		{ "TONG_ApplySetWeekGoalPlayer",	LuaTONG_ApplySetWeekGoalPlayer },
		{ "TONG_ApplySetWeekGoalValue",	LuaTONG_ApplySetWeekGoalValue },
		{ "TONG_ApplySetWeekGoalPriceTong",	LuaTONG_ApplySetWeekGoalPriceTong },
		{ "TONG_ApplySetWeekGoalPricePlayer",	LuaTONG_ApplySetWeekGoalPricePlayer },
		{ "TONG_ApplySetLWeekGoalEvent",	LuaTONG_ApplySetLWeekGoalEvent },
		{ "TONG_ApplySetLWeekGoalLevel",	LuaTONG_ApplySetLWeekGoalLevel },
		{ "TONG_ApplySetLWeekGoalTotal",	LuaTONG_ApplySetLWeekGoalTotal },
		{ "TONG_ApplySetLWeekGoalPlayer",	LuaTONG_ApplySetLWeekGoalPlayer },
		{ "TONG_ApplySetLWeekGoalValue",	LuaTONG_ApplySetLWeekGoalValue },
		{ "TONG_ApplySetLWeekGoalPriceTong",	LuaTONG_ApplySetLWeekGoalPriceTong },
		{ "TONG_ApplySetLWeekGoalPricePlayer",	LuaTONG_ApplySetLWeekGoalPricePlayer },
		{ "TONG_ApplySetTongMap",	LuaTONG_ApplySetTongMap },
		{ "TONG_ApplySetTongMapBan",	LuaTONG_ApplySetTongMapBan },
		{ "TONG_ApplySetAnnouncement",	LuaTONG_ApplySetAnnouncement },
		{ "TONG_ApplySetStunt",	LuaTONG_ApplySetStunt },
		{ "TONG_ApplyAddBuildFund",	LuaTONG_ApplyAddBuildFund },
		{ "TONG_ApplyAddWarBuildFund",	LuaTONG_ApplyAddWarBuildFund },
		{ "TONG_ApplyAddPerStandFund",	LuaTONG_ApplyAddPerStandFund },
		{ "TONG_ApplyAddDay",	LuaTONG_ApplyAddDay },
		{ "TONG_ApplyAddWeek",	LuaTONG_ApplyAddWeek },
		{ "TONG_ApplyAddStoredBuildFund",	LuaTONG_ApplyAddStoredBuildFund },
		{ "TONG_ApplyAddStoredOffer",	LuaTONG_ApplyAddStoredOffer },
		{ "TONG_ApplyAddTotalBuildFund",	LuaTONG_ApplyAddTotalBuildFund },
		{ "TONG_ApplyAddWeekBuildFund",	LuaTONG_ApplyAddWeekBuildFund },
		{ "TONG_ApplyAddWeekGoalValue",	LuaTONG_ApplyAddWeekGoalValue },
		{ "TONG_ApplyAddLWeekGoalValue",	LuaTONG_ApplyAddLWeekGoalValue },
		{ "TONG_ApplyAddEventRecord",	LuaTONG_ApplyAddEventRecord },
		{ "TONG_ApplyAddHistoryRecord",	LuaTONG_ApplyAddHistoryRecord },
		{ "TONG_AddTaskTemp",	LuaTONG_AddTaskTemp },
		{ "TONG_WriteLog",	LuaTONG_WriteLog },
		{ "TONG_ApplyInit",	LuaTONG_ApplyInit },
		{ "TONG_ApplyUpgrade",	LuaTONG_ApplyUpgrade },
		{ "TONG_ApplyDegrade",	LuaTONG_ApplyDegrade },
		{ "TONG_ApplyMaintain",	LuaTONG_ApplyMaintain },
		{ "TONG_ApplyWeeklyMaintain",	LuaTONG_ApplyWeeklyMaintain },
		{ "TONG_ApplyDeleteMap",	LuaTONG_ApplyDeleteMap },
		{ "TONG_ApplyKickMember",	LuaTONG_ApplyKickMember },
		{ "TONG_ApplyDeleteMember",	LuaTONG_ApplyDeleteMember },
		{ "TONG_ApplyCreatMap",	LuaTONG_ApplyCreatMap },
		{ "TONG_ChangeAllMemberFeature",	LuaTONG_ChangeAllMemberFeature },
		{ "TONG_ContributeOffer",	LuaTONG_ContributeOffer },
		{ "TONG_DistributeOfferToGroup",	LuaTONG_DistributeOfferToGroup },
		{ "TONG_DistributeOfferToMember",	LuaTONG_DistributeOfferToMember },
		{ "TONGM_GetJoinTime",	LuaTONGM_GetJoinTime },
		{ "TONGM_GetJoinDay",	LuaTONGM_GetJoinDay },
		{ "TONGM_GetMoney",	LuaTONGM_GetMoney },
		{ "TONGM_GetLWeekGoalOffer",	LuaTONGM_GetLWeekGoalOffer },
		{ "TONGM_GetLWeeklyOffer",	LuaTONGM_GetLWeeklyOffer },
		{ "TONGM_GetLastOnlineDate",	LuaTONGM_GetLastOnlineDate },
		{ "TONGM_GetRetireDate",	LuaTONGM_GetRetireDate },
		{ "TONGM_ApplySetLWeekGoalOffer",	LuaTONGM_ApplySetLWeekGoalOffer },
		{ "TONGM_ApplySetLWeeklyOffer",	LuaTONGM_ApplySetLWeeklyOffer },
		{ "TONGM_ApplyAddUTaskValue",	LuaTONGM_ApplyAddUTaskValue },
		{ "TONGM_ApplyAddOfferEx",	LuaTONGM_ApplyAddOfferEx },
		{ "TWS_IsExist",	LuaTWS_IsExist },
		{ "TWS_IsOpen",	LuaTWS_IsOpen },
		{ "TWS_GetType",	LuaTWS_GetType },
		{ "TWS_GetLevel",	LuaTWS_GetLevel },
		{ "TWS_GetWorkshopCount",	LuaTWS_GetWorkshopCount },
		{ "TWS_GetFirstWorkshop",	LuaTWS_GetFirstWorkshop },
		{ "TWS_GetNextWorkshop",	LuaTWS_GetNextWorkshop },
		{ "TWS_GetDayOutput",	LuaTWS_GetDayOutput },
		{ "TWS_GetUseLevel",	LuaTWS_GetUseLevel },
		{ "TWS_GetUseLevelSet",	LuaTWS_GetUseLevelSet },
		{ "TWS_GetBuildingNpc",	LuaTWS_GetBuildingNpc },
		{ "TWS_SetBuildingNpc",	LuaTWS_SetBuildingNpc },
		{ "TWS_GetTaskValue",	LuaTWS_GetTaskValue },
		{ "TWS_GetUTaskValue",	LuaTWS_GetUTaskValue },
		{ "TWS_ApplySetTaskValue",	LuaTWS_ApplySetTaskValue },
		{ "TWS_ApplyAddTaskValue",	LuaTWS_ApplyAddTaskValue },
		{ "TWS_ApplyAddUTaskValue",	LuaTWS_ApplyAddUTaskValue },
		{ "TWS_ApplySetDayOutput",	LuaTWS_ApplySetDayOutput },
		{ "TWS_ApplyAddDayOutput",	LuaTWS_ApplyAddDayOutput },
		{ "TWS_ApplySetUseLevel",	LuaTWS_ApplySetUseLevel },
		{ "TWS_ApplySetUseLevelSet",	LuaTWS_ApplySetUseLevelSet },
		{ "TWS_ApplyAdd",	LuaTWS_ApplyAdd },
		{ "TWS_ApplyRemove",	LuaTWS_ApplyRemove },
		{ "TWS_ApplyOpen",	LuaTWS_ApplyOpen },
		{ "TWS_ApplyClose",	LuaTWS_ApplyClose },
		{ "TWS_ApplyUpgrade",	LuaTWS_ApplyUpgrade },
		{ "TWS_ApplyDegrade",	LuaTWS_ApplyDegrade },
		{ "TWS_ApplyMaintain",	LuaTWS_ApplyMaintain },
		{ "TWS_ApplyUse",	LuaTWS_ApplyUse },
#endif
		//-------------------------------------------------
		{ "SwearBrother", LuaSwearBrother}, // ret = SwearBrother(TeamId);
		{"MakeEnemy",	LuaMakeEnemy}, //结仇 MakeEnemy(enemyname)
		{"RollbackSkill", LuaRollBackSkills},
		{"UpdateSkill", LuaUpdateSkillList},
		//-------------------------------------------------
		{"AddProp",		LuaAddPropPoint},//加玩家属性点
		{"AddPropPoint",LuaAddPropPoint},//加玩家属性点
		{"ResetProp",		LuaResetProp},
		{"GetProp",		LuaGetRestPropPoint },
		{"GetTimeNow",		LuaGetTimeNow},
		{"GetTimeZero",		LuaGetTimeZero},
		{"GetLocalDate",		LuaGetLocalDate},
		{"GetLocalDateEx", LuaGetTimeByMiao},
		{"TabFile_Load",		LuaTabFile_Load},
		{"TabFile_GetCell",		LuaTabFile_GetCell},
		{"TabFile_GetRowCount",		LuaTabFile_GetRowCount},
		{"TabFile_UnLoad",		LuaTabFile_UnLoad},
		//{"Trade",			LuaTrade	},				//Trade("maininfo", "IniFileName.ini的路径名")
		//Trade("MainInfo", n, "item1|price1|function1", "item2|price2|function2", ......, "itemn|pricen|functionn")
		{"RANDOM",				LuaRANDOM},  // th猰 h祄 m韎 load npc theo tuy謙 th?
		{"RANDOMC",				LuaRANDOMC}, // th猰 h祄 m韎 load npc theo tuy謙 th?
		{"SetNpcKind",		LuaSetNpcKind}, // th猰 h祄 m韎 load npc theo tuy謙 th?
		{"SetNpcBoss2",	LuaSetNpcBoss2},
		//	{"IsBlueBoss",		LuaIsBlueBoss},
		{ "GenNewPUBGMap", LuaGenNewPUBGMap },
		{"GetSubWorldPlayerIndexes", LuaGetSubWorldPlayerIndexes },
		{"ForceClearStateSkillEffect", LuaNpcForceClearStateSkillEffect }, //player idx
		{ "ForceClearStateSkillEffectSkillId", LuaNpcForceClearStateSkillEffectId }, //player idx, skill id
		{"UpdatePubgCircle", LuaUpdatePubgCircle }, //nWorldIdx, layer
		{ "GetLayerCount", LuaGetLayerCount },
		{ "GetPointCountInLayer", LuaGetPointCountInLayer },
		{ "GetPoint", LuaGetPoint },
		{"SendClientPoint", LuaSendClientPoint },
	
		{ "UseGiftcodeS", LuaUseGiftcodeS },
		{ "GiftcodeIsValid", LuaGiftcodeIsValid },
		#else 
			{"PlaySound", LuaPlaySound}, //PlaySound(Sound);
			{"PlaySprMovie",LuaPlaySprMovie},//PlaySprMovie(npcindex, Movie, times)
		#endif
			{"PlayMusic", LuaPlayMusic}, //PlayMusic(Music,Loop)
			{"FadeInMusic",LuaFadeInMusic},
			{"FadeOutMusic",LuaFadeOutMusic},
			{"StrSplit",LuaStrSplit},
};

TLua_Funcs WorldScriptFuns[] =
{
	{"AddLocalNews",LuaAddLocalNews},
	{"AddLoaclTimeNews",LuaAddLocalTimeNews},
	{"AddLocalCountNews",LuaAddLocalCountNews	},
#ifdef _SERVER
	{"Msg2SubWorld",	LuaMsgToSubWorld},	//Msg2SubWorld(消息)通知世界
	{"Msg2IP",			LuaMsgToIP}, //Msg2IP(IP, ID, StrInfo)	
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx
	{"GetServerName",	LuaGetServerName},
	{"KickOutPlayer",	LuaKickOutPlayer},
	{"KickOutAccount",	LuaKickOutAccount},
#endif
};

int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns) / sizeof(GameScriptFuns[0]);
}

int g_GetWorldScriptFunNum()
{
	return sizeof(WorldScriptFuns) / sizeof(WorldScriptFuns[0]);
}
