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
#include "KGameKV.h"
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

// ---------------------------------------------------------------------------
// SubWorldIdx2MapCopy(nSubWorldIdx) - port he Bang hoi/Boss bang hoi (21/08)
//
// Ban Linux (JX2) phan biet BAN DO GOC voi BAN SAO phong ban (dungeon copy):
// ham nay tra ve id ban do GOC cua mot ban sao. Engine JX1 cua du an KHONG co
// co che ban sao ban do (khong co PreApplyDungeonMap/ApplyDungeonMap), moi
// subworld deu la ban do that, nen id goc == id that.
// => tra ve dung nhu SubWorldIdx2ID. Giu ten rieng de script goc chep sang
//    KHONG phai sua mot ky tu nao (vd script\item\bosscharm.lua:8).
// ---------------------------------------------------------------------------
int LuaSubWorldIdx2MapCopy(Lua_State* L)
{
	int nTargetSubWorld = -1;
	int nSubWorldIndex = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldidx2mapcopy;

	nSubWorldIndex = (int)Lua_ValueToNumber(L, 1);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		goto lab_subworldidx2mapcopy;

	nTargetSubWorld = SubWorld[nSubWorldIndex].m_SubWorldID;

lab_subworldidx2mapcopy:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

// FileName2Id(szName) -> DWORD : port Hoat dong phuong bang hoi (21/08), Linux 0x08100E80.
// LECH CO CHU DICH: Linux ha chu hoa->thuong va '/'->'\\' truoc khi bam (0x0821DEA0);
// g_FileName2Id cua JX1 KHONG - va engine ta dang dung chinh no cho m_dwID, SetNpcScript,
// AddTrap... nen script phai bam cung ham de so sanh duoc voi gia tri engine.
int LuaFileName2Id(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char* szName = (char*)Lua_ValueToString(L, 1);
	if (!szName)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwId = g_FileName2Id(szName);
	Lua_PushNumber(L, (double)(unsigned int)dwId);
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
// FIX 14/08: noi dap an CO CHAN BIEN vao dem goi PLAYER_SCRIPTACTION_SYNC
// (m_pContent[MAX_SCIRPTACTION_BUFFERNUM] nam tren STACK cua ham goi).
// Ban cu dung sprintf(pContent, "%s|%s", pContent, pAnswer): vua khong chan
// bien vua UB (nguon trung dich). Bang dau gia khieu chien lenh
// (infocenter_head.lua) ~51 byte/bang => tu khoang 9 bang la vuot 512 va
// SAP GameServer. pCur co the tro giua dem (nDataType 1 chua 4 byte id).
static void sUiAppendAnswer(char* pBufStart, char* pCur, const char* pAdd)
{
	if (!pBufStart || !pCur || !pAdd)
		return;
	int nCap = MAX_SCIRPTACTION_BUFFERNUM - (int)(pCur - pBufStart);
	if (nCap <= 1)
		return;
	int nLen = (int)strlen(pCur);
	if (nLen + 1 >= nCap)
		return;
	pCur[nLen++] = '|';
	pCur[nLen] = 0;
	int nRoom = nCap - nLen - 1;
	if (nRoom > 0)
		strncat(pCur, pAdd, nRoom);
}

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
			g_StrCpyLen(UiInfo.m_pContent, strMain, MAX_SCIRPTACTION_BUFFERNUM);
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
			sUiAppendAnswer(UiInfo.m_pContent, pContent, pAnswer);
		}
		else
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			sUiAppendAnswer(UiInfo.m_pContent, pContent, pAnswer);
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
			g_StrCpyLen(UiInfo.m_pContent, strMain, MAX_SCIRPTACTION_BUFFERNUM);
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
			sUiAppendAnswer(UiInfo.m_pContent, pContent, pAnswer);
		}
		else
		{
			strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			sUiAppendAnswer(UiInfo.m_pContent, pContent, pAnswer);
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
			g_StrCpyLen(UiInfo.m_pContent, strMain, MAX_SCIRPTACTION_BUFFERNUM);
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
				sUiAppendAnswer(UiInfo.m_pContent, pContent, pAnswer);
			}
			else
			{
				strcpy(Player[nPlayerIndex].m_szTaskAnswerFun[i], "Main");
				sUiAppendAnswer(UiInfo.m_pContent, pContent, pAnswer);
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
			g_StrCpyLen(UiInfo.m_pContent, strMain, MAX_SCIRPTACTION_BUFFERNUM);
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

// JX2 port: script bang hoi Linux goi \script\tong\... va \script\lib\...
// nhung cay that nam o scriptjx2\ (ngoai script\ vi GS tu chay moi .lua trong
// script\ luc boot). Neu file khong ton tai -> thu duong scriptjx2 tuong ung.
static void sJX2RemapScriptPath(char* szFull)
{
	FILE* f = fopen(szFull, "rb");
	if (f)
	{
		fclose(f);
		return;
	}
	static const char* szFrom[2] = { "script\\tong\\", "script\\lib\\" };
	static const char* szTo[2]   = { "scriptjx2\\tong_vn\\", "scriptjx2\\lib\\" };
	char szBuf[MAX_PATH * 2];
	for (int k = 0; k < 2; k++)
	{
		char* p = strstr(szFull, szFrom[k]);
		if (!p)
			continue;
		int nHead = (int)(p - szFull);
		memcpy(szBuf, szFull, nHead);
		szBuf[nHead] = 0;
		strcat(szBuf, szTo[k]);
		strcat(szBuf, p + strlen(szFrom[k]));
		f = fopen(szBuf, "rb");
		if (f)
		{
			fclose(f);
			strcpy(szFull, szBuf);
			return;
		}
	}
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
		sJX2RemapScriptPath(lszCurrentDirectory);	// JX2 port
		// FIX 14/08 (A3): Include file KHONG TON TAI truoc day im lang tuyet
		// doi (lua_dofile tra LUA_ERRFILE, khong nem loi, khong ghi log) ->
		// khong the dung log de ket luan cay script sach.
		if (lua_dofile(L, lszCurrentDirectory) != 0)
			g_DebugLog((LPSTR)"[script] Include HONG: %.200s", lszCurrentDirectory);
		return 0;
	}
	else
		return 0;
}

// ============================================================================
// == DA TAU TASKLINK (JX2 port) 15/08/2026 =================================
// Cac ham may chu Linux goc (jx_linux_y) cung cap cho he chuoi nhiem vu
// Da Tau (script\task\newtask\tasklink + script\global\seasonnpc.lua).
// Doi chieu day du: D:\GAMEDEVNEW\DANHSACH_DATAU_PORT.md (muc D).
// ============================================================================

// C_Random(nMin, nMax) - so nguyen ngau nhien [nMin, nMax] tren RNG engine
// (g_Random). PHAI cung nguon voi SetRandSeed: tasklink_award khoa bo 3 phan
// thuong theo seed luu o task 1037 (chong thoat/vao lai de doi thuong).
int LuaC_Random(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nMin = (int)Lua_ValueToNumber(L, 1);
	int nMax = (int)Lua_ValueToNumber(L, 2);
	if (nMax < nMin)
	{
		int nTmp = nMin; nMin = nMax; nMax = nTmp;
	}
	Lua_PushNumber(L, GetRandomNumber(nMin, nMax));
	return 1;
}

// SetRandSeed(nSeed) -> seed CU. [FIX 16/08] tasklink_award.lua:55 luu
// _nSeed = SetRandSeed(...) roi khoi phuc SetRandSeed(_nSeed); ban dau khong
// tra gi -> _nSeed = nil -> moi lan boc thuong dat seed LCG TOAN SERVER = 0
// (g_Random dung chung boi GetRandomNumber - KCore.h:200) -> ngau nhien toan
// cuc lap lai/du doan duoc. Tra seed cu de script khoi phuc dung nguyen ban.
int LuaSetRandSeed(Lua_State* L)
{
	UINT uOld = g_GetRandomSeed();
	if (Lua_GetTopIndex(L) >= 1)
		g_RandomSeed((UINT)(DWORD)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, (double)uOld);
	return 1;
}

// GetTiredDegree() - JX1 khong co he do met. Ban goc trong TireReduce cung
// tu gan de TireDegree = 0 ngay sau khi doc, nen tra 0 = DUNG nguyen ban.
int LuaGetTiredDegree(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// GetBitTask(nTaskId, nStart, nLen) - doc cum bit trong bien task luu.
int LuaGetBitTask(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nTaskId = (int)Lua_ValueToNumber(L, 1);
	int nStart = (int)Lua_ValueToNumber(L, 2);
	int nLen = (int)Lua_ValueToNumber(L, 3);
	if (nStart < 0 || nLen <= 0 || nStart + nLen > 32)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	DWORD dwVal = (DWORD)Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);
	DWORD dwMask = (nLen >= 32) ? 0xFFFFFFFF : ((1UL << nLen) - 1);
	Lua_PushNumber(L, (double)((dwVal >> nStart) & dwMask));
	return 1;
}

// SetBitTask(nTaskId, nStart, nLen, nValue) - ghi cum bit vao bien task luu.
int LuaSetBitTask(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return 0;
	int nTaskId = (int)Lua_ValueToNumber(L, 1);
	int nStart = (int)Lua_ValueToNumber(L, 2);
	int nLen = (int)Lua_ValueToNumber(L, 3);
	DWORD dwNew = (DWORD)Lua_ValueToNumber(L, 4);
	if (nStart < 0 || nLen <= 0 || nStart + nLen > 32)
		return 0;
	DWORD dwMask = (nLen >= 32) ? 0xFFFFFFFF : ((1UL << nLen) - 1);
	DWORD dwVal = (DWORD)Player[nPlayerIndex].m_cTask.GetSaveVal(nTaskId);
	dwVal = (dwVal & ~(dwMask << nStart)) | ((dwNew & dwMask) << nStart);
	Player[nPlayerIndex].m_cTask.SetSaveVal(nTaskId, (int)dwVal);
	return 0;
}

// GetItemMagicAttrib(nItemIndex, i) -> nAttribType, nValue1, nValue2, nValue3
// (o ma phap thu i, 1-based). tasklink loai 2 (tim do co thuoc tinh) va
// loai 3 (khoe do) lap i=1..6 de so khop MagicEnName/MinValue/MaxValue.
int LuaGetItemMagicAttrib(Lua_State* L)
{
	int nItemIndex = 0;
	int nSlot = 0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		nItemIndex = (int)Lua_ValueToNumber(L, 1);
		nSlot = (int)Lua_ValueToNumber(L, 2);
	}
	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM ||
		nSlot < 1 || nSlot > MAX_ITEM_MAGICATTRIB)
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
		return 4;
	}
	Lua_PushNumber(L, Item[nItemIndex].m_aryMagicAttrib[nSlot - 1].nAttribType);
	Lua_PushNumber(L, Item[nItemIndex].m_aryMagicAttrib[nSlot - 1].nValue[0]);
	Lua_PushNumber(L, Item[nItemIndex].m_aryMagicAttrib[nSlot - 1].nValue[1]);
	Lua_PushNumber(L, Item[nItemIndex].m_aryMagicAttrib[nSlot - 1].nValue[2]);
	return 4;
}

// SetItemMagicLevel(nItemIndex, nSlot, nValue) - STUB co chu dich.
// Chi duong "Tich luy Da Tau" (item 6/1/1475, phat khi TireDegree==2) dung;
// duong do NGU DONG tren JX1 (GetTiredDegree=0 va 1475 khong co trong
// award_basic/link/loop). Phia doc cua 1475 la GetItemParam(idx,2/3) cua JX1
// von tra -1 nen KHONG ghi that de khoi lech doc/ghi im lang.
int LuaSetItemMagicLevel(Lua_State* L)
{
	g_DebugLog((LPSTR)"[DaTau] SetItemMagicLevel: duong 1475 ngu dong, bo qua (xem DANHSACH_DATAU_PORT.md)");
	Lua_PushNumber(L, 0);
	return 1;
}

// SyncItem(nItemIndex) - dong bo 1 item xuong client (mau LuaSetParamItem).
int LuaSyncItemJX2(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 1)
		return 0;
	int nItemIndex = (int)Lua_ValueToNumber(L, 1);
	if (nItemIndex > 0 && nItemIndex < MAX_ITEM)
#ifdef _SERVER
		// KItemList::SyncItem chi ton tai o ban server (KItemList.h:186 nam trong
		// #ifdef _SERVER). Khong boc thi build Client bao C2039 "khong phai thanh vien".
		Player[nPlayerIndex].m_ItemList.SyncItem(nItemIndex);
#endif
	return 0;
}

// curpack()/usepack(n) - may ao Linux: 1 Lua_State + N bang global chuyen
// duoc ("pack"). JX1: moi file .lua mot Lua_State rieng nen "pack hien tai"
// luon la 0 va chuyen pack la no-op. awardtype\simple.lua (SimpleType.nPak,
// usepack trong Give) chay dung voi stub nay vi handler va nguoi goi cung state.
int LuaCurPack(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaUsePack(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// CallPlayerFunction(nPlayerIdx, pFun, ...) - goi pFun voi PlayerIndex tam
// thoi doi sang nPlayerIdx (GetPlayerIndex doc global "PlayerIndex" - xem
// ScriptFuns.cpp GetPlayerIndex). Tra ve moi ket qua cua pFun.
int LuaCallPlayerFunction(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	int nPlayerIdx = (int)Lua_ValueToNumber(L, 1);
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;
	// luu PlayerIndex cu
	lua_getglobal(L, SCRIPT_PLAYERINDEX);
	int nOldTop = lua_gettop(L);	// vi tri gia tri cu tren stack
	// dat PlayerIndex moi
	Lua_PushNumber(L, nPlayerIdx);
	lua_setglobal(L, SCRIPT_PLAYERINDEX);
	// dung ham + tham so: pFun o vi tri 2, tham so 3..nParamNum
	lua_pushvalue(L, 2);
	int nArgs = 0;
	for (int i = 3; i <= nParamNum; i++)
	{
		lua_pushvalue(L, i);
		nArgs++;
	}
	lua_rawcall(L, nArgs, 0);
	// khoi phuc PlayerIndex cu (van nam o nOldTop)
	lua_pushvalue(L, nOldTop);
	lua_setglobal(L, SCRIPT_PLAYERINDEX);
	return 0;
}

// GetLastFactionNumber() - JX2: so hieu mon phai 0..9 (Thieu Lam=0 ... Con
// Lon=9), -1 = chua vao phai. JX1 GetFactionNo() cung he so (KPlayer.cpp).
int LuaGetLastFactionNumber(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Lua_PushNumber(L, Player[nPlayerIndex].GetFactionNo());
	else
		Lua_PushNumber(L, -1);
	return 1;
}

// TM_SetTimer / TM_GetRestCount - he hen gio storm cua JX2. Stub co chu
// dich: TM_GetRestCount tra NIL -> storm/custom.lua thay "chua co timer"
// -> storm_valid_game=false -> storm_addpoint bo qua (storm ngu dong).
int LuaTM_SetTimer(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaTM_GetRestCount(Lua_State* L)
{
	Lua_PushNil(L);
	return 1;
}

// BT_GetGameData / BT_GetData - kho du lieu tran Tong Kim (battle) cua JX2.
// Tra 0: moi nhanh storm/tong-kim trong bao dong Da Tau tu tat.
int LuaBT_GetDataStub(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// Prise(szMsg, szOpt1, szOpt2, szOpt3) - cua so chon 1 trong 3 phan thuong
// cua Da Tau. Moi szOpt dang "nhan/loai-icon/thamso/TenHam" (thamso co the
// nhieu so cach nhau boi dau phay). Client JX2 co UI rieng; JX1 dung hop
// thoai chon chuan: bien doi thanh "nhan/#TenHam(thamso)" roi giao cho
// LuaSelectUI (cung co che callback m_szTaskAnswerFun).
// LUU Y: mySG can 6 tham so -> MAX_PARAMLIST_COUNT (KPlayer.cpp) da noi 5->8.
int LuaPrise(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	char szMsg[MAX_SCIRPTACTION_BUFFERNUM];
	szMsg[0] = 0;
	if (Lua_IsString(L, 1))
		g_StrCpyLen(szMsg, (char*)Lua_ValueToString(L, 1), sizeof(szMsg));
	char szOpt[3][256];
	int nOpt = 0;
	for (int i = 2; i <= nParamNum && nOpt < 3; i++)
	{
		if (!Lua_IsString(L, i))
			continue;
		char szRaw[256];
		g_StrCpyLen(szRaw, (char*)Lua_ValueToString(L, i), sizeof(szRaw));
		if (!szRaw[0])
			continue;
		// tach tu PHAI: .../fn , truoc do /thamso , truoc do /icon , con lai la nhan
		char* pFn = strrchr(szRaw, '/');
		if (!pFn)
		{
			// khong dung dinh dang -> giu nguyen ca chuoi lam nhan, callback main
			g_StrCpyLen(szOpt[nOpt], szRaw, sizeof(szOpt[0]));
			nOpt++;
			continue;
		}
		*pFn = 0; pFn++;
		char* pParam = strrchr(szRaw, '/');
		if (!pParam)
		{
			szOpt[nOpt][0] = 0;
			g_StrCpyLen(szOpt[nOpt], szRaw, sizeof(szOpt[0]));
			strcat(szOpt[nOpt], "/");
			strcat(szOpt[nOpt], pFn);
			nOpt++;
			continue;
		}
		*pParam = 0; pParam++;
		char* pIcon = strrchr(szRaw, '/');
		if (pIcon)
			*pIcon = 0;	// bo truong icon - hop thoai JX1 khong ve icon rieng
		// ghep: nhan/#fn(thamso) - cat bot NHAN neu tong vuot 255 byte
		int nMaxLabel = (int)sizeof(szOpt[0]) - (int)strlen(pFn) - (int)strlen(pParam) - 8;
		if (nMaxLabel < 0)
			nMaxLabel = 0;
		if ((int)strlen(szRaw) > nMaxLabel)
			szRaw[nMaxLabel] = 0;
		sprintf(szOpt[nOpt], "%s/#%s(%s)", szRaw, pFn, pParam);
		nOpt++;
	}
	if (nOpt == 0)
		return 0;
	// dung lai stack: (msg, nOpt, opt1..optN) roi giao cho LuaSelectUI
	lua_settop(L, 0);
	Lua_PushString(L, szMsg);
	Lua_PushNumber(L, nOpt);
	for (int k = 0; k < nOpt; k++)
		Lua_PushString(L, szOpt[k]);
	return LuaSelectUI(L);
}

// DynamicExecuteByPlayer(nPlayerIdx, szScript, szFun, ...) - goi ham trong
// MOT SCRIPT KHAC voi boi canh nguoi choi nPlayerIdx. szFun cho phep dang
// method "tbX:Fun". Script chua nap (g_GetScript khong tu nap - KSortScript
// .cpp:60) -> ghi log roi bo qua EM DEM (hook huoyuedu ngu dong tren JX1).
int LuaDynamicExecuteByPlayer(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 3)
		return 0;
	int nPlayerIdx = (int)Lua_ValueToNumber(L, 1);
	const char* szScript = Lua_ValueToString(L, 2);
	const char* szFun = Lua_ValueToString(L, 3);
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || !szScript || !szFun)
		return 0;
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szScript);
	if (!pScript)
	{
		g_DebugLog((LPSTR)"[DaTau] DynamicExecuteByPlayer: script chua nap, bo qua: %.128s -> %.64s", szScript, szFun);
		return 0;
	}
	// ghep chuoi goi: Fun(a1,a2,...) - so -> %.0f, chuoi -> "..."
	char szCall[512];
	int nPos = 0;
	nPos += sprintf(szCall + nPos, "%s(", szFun);
	for (int i = 4; i <= nParamNum && nPos < (int)sizeof(szCall) - 80; i++)
	{
		if (i > 4)
			szCall[nPos++] = ',';
		if (Lua_IsNumber(L, i))
			nPos += sprintf(szCall + nPos, "%.0f", (double)Lua_ValueToNumber(L, i));
		else if (Lua_IsString(L, i))
			nPos += sprintf(szCall + nPos, "\"%.64s\"", Lua_ValueToString(L, i));
		else
			nPos += sprintf(szCall + nPos, "nil");
	}
	szCall[nPos++] = ')';
	szCall[nPos] = 0;
	// dat boi canh nguoi choi cho state dich (mau KPlayer::ExecuteScript)
	Lua_PushNumber(pScript->m_LuaState, nPlayerIdx);
	pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
	int nTopIndex = 0;
	pScript->SafeCallBegin(&nTopIndex);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		g_DebugLog((LPSTR)"[DaTau] DynamicExecuteByPlayer LOI: %.128s -> %.200s", szScript, szCall);
	pScript->SafeCallEnd(nTopIndex);
	return 0;
}

// [WLLS 20/08] DynamicExecute(szScript, szFun, ...) - nhu DynamicExecuteByPlayer
// nhung KHONG boi canh nguoi choi (Linux global\autoexec.lua goi ~10 lan;
// startgame.lua dung de khoi dong driver lien dau trong state rieng cua no).
int LuaDynamicExecute(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	const char* szScript = Lua_ValueToString(L, 1);
	const char* szFun = Lua_ValueToString(L, 2);
	if (!szScript || !szFun)
		return 0;
	char szLow[MAX_PATH];
	g_StrCpyLen(szLow, (char*)szScript, MAX_PATH);
	g_StrLower(szLow);
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
	if (!pScript)
	{
		g_DebugLog((LPSTR)"[WLLS] DynamicExecute: script chua nap, bo qua: %.128s -> %.64s", szLow, szFun);
		return 0;
	}
	char szCall[512];
	int nPos = 0;
	nPos += sprintf(szCall + nPos, "%s(", szFun);
	for (int i = 3; i <= nParamNum && nPos < (int)sizeof(szCall) - 80; i++)
	{
		if (i > 3)
			szCall[nPos++] = ',';
		if (Lua_IsNumber(L, i))
			nPos += sprintf(szCall + nPos, "%.0f", (double)Lua_ValueToNumber(L, i));
		else if (Lua_IsString(L, i))
			nPos += sprintf(szCall + nPos, "\"%.64s\"", Lua_ValueToString(L, i));
		else
			nPos += sprintf(szCall + nPos, "nil");
	}
	szCall[nPos++] = ')';
	szCall[nPos] = 0;
	int nTopIndex = 0;
	pScript->SafeCallBegin(&nTopIndex);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		g_DebugLog((LPSTR)"[WLLS] DynamicExecute LOI: %.128s -> %.200s", szLow, szCall);
	pScript->SafeCallEnd(nTopIndex);
	return 0;
}

// [WLLS 21/08] _ALERT: Lua 4 mac dinh in loi runtime ra stderr (console) nen
// chay dich vu la MAT SACH thong diep ("attempt to index...", so dong) - chi
// con ma so + ten ham trong ScriptError.log. Dang ky ban C de _ERRORMESSAGE
// goi ve day, ghi nguyen van vao CUNG file ScriptError.log (fopen append
// tuong doi nhu KLuaScript::WriteLogScriptErrorFile). Ap dung ca dofile/
// dostring that bai (deferred dw, DynamicExecute) truoc nay chi thay o console.
int LuaGameAlert(Lua_State* L)
{
	if (Lua_IsString(L, 1))
	{
		const char* psz = (const char*)Lua_ValueToString(L, 1);
		FILE* pFile = fopen("ScriptError.log", "a");
		if (pFile)
		{
			fprintf(pFile, "%s\n", psz);
			fclose(pFile);
		}
	}
	return 0;
}

// IncludeLib("TEN_MODULE") cua script JX2 -> nap file lib tuong ung o scriptjx2.
// Module la nhan rieng cua engine JX2; module khong co trong bang thi bo qua im lang.
int LuaIncludeLib(Lua_State* L)
{
	// DOT E (E5): +6 module cong thanh - ham that da nam o C (LG_/BT_/Title_...),
	// tro noop.lua chi de dofile khong loi. CAM tro LEAGUE vao jx2compat (de ham C).
	// [WLLS 20/08] +ITEM (leaguematch head.lua:10 IL("ITEM") - ham item da o C)
	static const char* szMod[21] = {
		"TONG", "FILE", "LOG", "STRING", "BASIC", "COMMON", "SAY",
		"PLAYER", "AWARD", "TIMERLIST", "TOPLIST", "MAPDB", "GB_TASK", "FILESYS",
		"SETTING", "BATTLE", "RELAYLADDER", "TITLE", "LEAGUE", "PARTNER",
		"ITEM",
	};
	static const char* szFile[21] = {
		"scriptjx2\\tong_vn\\tong_header.lua", "scriptjx2\\lib\\file.lua",
		"scriptjx2\\lib\\log.lua", "scriptjx2\\lib\\string.lua",
		"scriptjx2\\lib\\basic.lua", "scriptjx2\\lib\\common.lua",
		"scriptjx2\\lib\\say.lua", "scriptjx2\\lib\\player.lua",
		"scriptjx2\\lib\\award.lua", "scriptjx2\\lib\\timerlist.lua",
		"scriptjx2\\lib\\toplist.lua", "scriptjx2\\lib\\mapdb.lua",
		"scriptjx2\\lib\\gb_taskfuncs.lua", "scriptjx2\\lib\\file.lua",
		"scriptjx2\\lib\\noop.lua", "scriptjx2\\lib\\noop.lua",
		"scriptjx2\\lib\\noop.lua", "scriptjx2\\lib\\noop.lua",
		"scriptjx2\\lib\\noop.lua", "scriptjx2\\lib\\noop.lua",
		"scriptjx2\\lib\\noop.lua",
	};
	if (Lua_GetTopIndex(L) <= 0 || !Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* pszName = lua_tostring(L, 1);
	// FIX stack-overflow boot 14/08: goc JX2 IncludeLib = REQUIRE-ONCE moi state;
	// ta dofile moi lan goi nen lib tu-goi (file.lua:1 = IncludeLib("FILESYS"),
	// tong_setting.lua:1 y het) de quy VO HAN ngay khi co diem goi FILESYS dau
	// tien (citywar head.lua E5). Cam co global __INCLIB_<MOD> TRUOC khi dofile
	// de chan ca tu-de-quy lan nap lai. Dong thoi sua tran vong lap 14 -> 20
	// (6 module moi SETTING/BATTLE/RELAYLADDER/TITLE/LEAGUE/PARTNER bi cam).
	for (int k = 0; k < 21; k++)
	{
		if (strcmp(pszName, szMod[k]) != 0)
			continue;
		char szFlag[64];
		sprintf(szFlag, "__INCLIB_%s", szMod[k]);
		Lua_GetGlobal(L, szFlag);
		if (!lua_isnil(L, Lua_GetTopIndex(L)))
		{
			// da nap trong state nay roi
			Lua_PushNumber(L, 1);
			return 1;
		}
		Lua_PushNumber(L, 1);
		Lua_SetGlobal(L, szFlag);
		char szPath[MAX_PATH * 2];
		g_GetRootPath(szPath);
		int nL = (int)strlen(szPath);
		if (nL > 0 && (szPath[nL - 1] == '\\' || szPath[nL - 1] == '/'))
			szPath[nL - 1] = 0;
		strcat(szPath, "\\");
		strcat(szPath, szFile[k]);
		// A5 (14/08): nap HONG thi GO co - khong thi module bi cam VINH VIEN
		// trong state do (co da cam TRUOC dofile de chan tu-de-quy).
		if (lua_dofile(L, szPath) != 0)
		{
			Lua_PushNil(L);
			Lua_SetGlobal(L, szFlag);
			g_DebugLog((LPSTR)"[script] IncludeLib HONG: %.20s -> %.200s", szMod[k], szPath);
		}
		Lua_PushNumber(L, 1);
		return 1;
	}
	// A4 (14/08): module KHONG co trong bang - truoc day bo qua IM LANG nen loi
	// doi tu "no luc boot" sang "ham nil luc nguoi choi bam" (kho tim hon).
	g_DebugLog((LPSTR)"[script] IncludeLib: module la [%.20s] - bo qua", pszName);
	Lua_PushNumber(L, 0);
	return 1;
}

// vung phat hanh (script JX2 phan nhanh cn_ib/vn) - ban VN co dinh
int LuaGetProductRegion(Lua_State* L)
{
	Lua_PushString(L, (char*)"vn");
	return 1;
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
// PHAN BIEN D1: mot o TabFile toan cuc lam workshop_setting.lua chet giua
// file luc boot (nap 2 bang, bang sau DE bang truoc -> GetCell tra "" ->
// tonumber = nil -> "table index is nil"; co ScriptError.log 13/08 lam
// bang chung). Moi caller von DA truyen TEN bang (arg2 cua Load, arg1 cua
// GetCell/GetRowCount/UnLoad - truoc gio bi bo qua) nen chi can TON TRONG
// ten do. Goi 1 tham so / ten chua nap -> ve o toan cuc nhu cu.
static std::map<std::string, KTabFile*> s_mapTabFiles;

static KTabFile* sGetTabFileByName(Lua_State* L, int nArg)
{
	if (Lua_GetTopIndex(L) >= nArg && Lua_IsString(L, nArg))
	{
		const char* szName = lua_tostring(L, nArg);
		if (szName && szName[0])
		{
			std::map<std::string, KTabFile*>::iterator it =
				s_mapTabFiles.find(szName);
			if (it != s_mapTabFiles.end())
				return it->second;
		}
	}
	return &g_TabFileLib;
}
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
			KTabFile* pTab = &g_TabFileLib;
			if (Lua_IsString(L, 2))
			{
				const char* szName = lua_tostring(L, 2);
				if (szName && szName[0])
				{
					std::map<std::string, KTabFile*>::iterator it =
						s_mapTabFiles.find(szName);
					if (it == s_mapTabFiles.end())
					{
						pTab = new KTabFile;
						s_mapTabFiles[szName] = pTab;	// song suot doi tien trinh
					}
					else
					{
						pTab = it->second;
						pTab->Clear();	// nap lai cung ten = thay noi dung
					}
				}
			}
			v4 = pTab->Load(szFileName);
			lua_pushnumber(L, (long double)v4);
			result = 1;
		}
	}
	return result;
}

int LuaTabFile_GetCell(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	// FIX 14/08 (crash 0xC0000409 luc boot): dem 128 QUA NHO cho bang co o mo
	// ta dai (mo ta tuyet ky bang hoi >= 128 byte), lai gap off-by-one cua
	// KTabFile::GetValue (nhanh CAT ghi lpRString[dwSize] = TRAN 1 byte).
	// => cap 2048 nhung chi bao voi engine 2047: byte thua nuot cu off-by-one,
	// an toan KE CA khi Engine.lib chua build lai duoc (Engine.vcxproj hong
	// san: thieu include ipc_shared.h). Ten hang/cot copy bang strncpy (chuoi
	// Lua dai tuy y - strcpy vao [32] la lo tran thu hai trong chinh ham nay).
	char szString[2048];
	const DWORD dwCellMax = sizeof(szString) - 1;
	KTabFile* pTabC = sGetTabFileByName(L, 1);
	if (nParamNum >= 3 && pTabC->GetHeight())
	{
		if (Lua_IsNumber(L, 2) && Lua_IsNumber(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			int nColumn = (int)Lua_ValueToNumber(L, 3);
			pTabC->GetString(nRow, nColumn, "", szString, dwCellMax);
		}
		else if (Lua_IsNumber(L, 2) && Lua_IsString(L, 3))
		{
			int nRow = (int)Lua_ValueToNumber(L, 2);
			char szColumn[64];
			strncpy(szColumn, Lua_ValueToString(L, 3), sizeof(szColumn) - 1);
			szColumn[sizeof(szColumn) - 1] = 0;
			pTabC->GetString(nRow, szColumn, "", szString, dwCellMax);
		}
		else if (Lua_IsString(L, 2) && Lua_IsString(L, 3))
		{
			char szRow[64];
			char szColumn[64];
			strncpy(szRow, Lua_ValueToString(L, 2), sizeof(szRow) - 1);
			szRow[sizeof(szRow) - 1] = 0;
			strncpy(szColumn, Lua_ValueToString(L, 3), sizeof(szColumn) - 1);
			szColumn[sizeof(szColumn) - 1] = 0;
			pTabC->GetString(szRow, szColumn, "", szString, dwCellMax);
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
	KTabFile* pTabR = sGetTabFileByName(L, 1);
	if (pTabR->GetHeight())
	{
		int nCount = pTabR->GetHeight();
		Lua_PushNumber(L, nCount);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return  0;
}

int LuaTabFile_UnLoad(Lua_State* L)
{
	KTabFile* pTabU = sGetTabFileByName(L, 1);
	if (pTabU->GetHeight())
	{
		pTabU->Clear();
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 0;
}

// ---- IniFile_* : port Hoat dong phuong bang hoi (21/08) ----
// Linux: kho INI toan cuc theo TEN (0x978265C). IniFile_Load(szFile, szName) -> 1/0:
// da nap cung ten + cung tep -> ref++ tra 1; cung ten khac tep -> tra 0, KHONG nap lai.
// IniFile_GetData(szName, szSect, szKey) LUON tra 1 chuoi ("" khi thieu/khong co).
// Khuon = s_mapTabFiles o tren. Trong missions\tong chi dung qua helper getinifiledata()
// (khong ai goi) nhung tongwar/battles/lib\file.lua dung that.
struct KTongIniEntry { KIniFile Ini; std::string strFile; int nRef; };
static std::map<std::string, KTongIniEntry*> s_mapTongIniFiles;

static KTongIniEntry* sGetTongIniByName(Lua_State* L, int nArg)
{
	if (Lua_GetTopIndex(L) >= nArg && Lua_IsString(L, nArg))
	{
		const char* szName = (const char*)Lua_ValueToString(L, nArg);
		if (szName && szName[0])
		{
			std::map<std::string, KTongIniEntry*>::iterator it = s_mapTongIniFiles.find(szName);
			if (it != s_mapTongIniFiles.end())
				return it->second;
		}
	}
	return NULL;
}

int LuaIniFile_Load(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
	const char* szFile = (const char*)Lua_ValueToString(L, 1);
	const char* szName = (const char*)Lua_ValueToString(L, 2);
	if (!szFile || !szName || !szFile[0] || !szName[0]) { Lua_PushNumber(L, 0); return 1; }
	std::map<std::string, KTongIniEntry*>::iterator it = s_mapTongIniFiles.find(szName);
	if (it != s_mapTongIniFiles.end())
	{
		if (it->second->strFile != szFile) { Lua_PushNumber(L, 0); return 1; }
		it->second->nRef++;
		Lua_PushNumber(L, 1);
		return 1;
	}
	KTongIniEntry* p = new KTongIniEntry;
	p->nRef = 0;
	if (!p->Ini.Load(szFile)) { delete p; Lua_PushNumber(L, 0); return 1; }
	p->strFile = szFile;
	p->nRef = 1;
	s_mapTongIniFiles[szName] = p;
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaIniFile_GetData(Lua_State* L)
{
	char szBuf[1024];
	szBuf[0] = 0;
	if (Lua_GetTopIndex(L) >= 3)
	{
		KTongIniEntry* p = sGetTongIniByName(L, 1);
		if (p)
		{
			const char* szSect = (const char*)Lua_ValueToString(L, 2);
			const char* szKey = (const char*)Lua_ValueToString(L, 3);
			p->Ini.GetString(szSect ? szSect : "", szKey ? szKey : "", "", szBuf, sizeof(szBuf));
		}
	}
	Lua_PushString(L, szBuf);
	return 1;
}

int LuaIniFile_UnLoad(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1)) return 0;
	const char* szName = (const char*)Lua_ValueToString(L, 1);
	if (!szName || !szName[0]) return 0;
	std::map<std::string, KTongIniEntry*>::iterator it = s_mapTongIniFiles.find(szName);
	if (it == s_mapTongIniFiles.end()) return 0;
	if (--it->second->nRef <= 0)
	{
		delete it->second;
		s_mapTongIniFiles.erase(it);
	}
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

			// [TIN SU 21/08] DANG JX2: ConsumeItem(nPos, nCount, g, d, p[, lvl]) - nPos = 3 (pos_equiproom)
			// hoac -1 (tay + hanh trang + tui mo rong), nCount >= 1. Phan biet voi dang JX1
			// (nCount, nNature, g, d, p, ...) bang tham so 2: moi caller JX1 trong cay du an deu
			// truyen nNature = 0 (NATURE_NORMAL); tham so 2 != 0 chi co o script port JX2
			// (songjin_shophead.lua:139, xinshibaoxiang.lua:146/151, tong_springfestival\head.lua:251
			// - truoc day 3 noi nay la NO-OP vi FindSameToRemove doi nature == 1/1000).
			if (nParamNum >= 5 && Lua_IsNumber(L, 2) && (int)Lua_ValueToNumber(L, 2) != 0
				&& (nDelNum == -1 || (nDelNum >= pos_hand && nDelNum < pos_num)))
			{
				int nPos = nDelNum;
				int nCnt = (int)Lua_ValueToNumber(L, 2);
				int g = (int)Lua_ValueToNumber(L, 3);
				int d = (int)Lua_ValueToNumber(L, 4);
				int p = (int)Lua_ValueToNumber(L, 5);
				int lv = nParamNum > 5 ? (int)Lua_ValueToNumber(L, 6) : -1;
				int nDone = 0;
				if (nCnt > 0)
				{
					if (nPos == -1 || nPos == pos_equiproom)
					{
						nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_equiproom);
						if (nDone < nCnt)
							nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_equiproomex);
					}
					else
						nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, nPos);
					if (nPos == -1 && nDone < nCnt)
						nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_hand);
				}
				Lua_PushNumber(L, nDone);
				return 1;
			}

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

// ConsumeEquiproomItem(nCount, g, d, p[, lvl]) -> so item da tru : port Tin Su (21/08).
// Linux (posthouse.lua:503, wuxingfu.lua:25) kiem "== 1". Chi hanh trang (+ tui mo rong);
// bang voi CalcEquiproomItemCount cung cap (WLLS). Khong dung tren quay (pos_hand).
int LuaConsumeEquiproomItem(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nDone = 0;
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 4)
	{
		int nCnt = (int)Lua_ValueToNumber(L, 1);
		int g = (int)Lua_ValueToNumber(L, 2);
		int d = (int)Lua_ValueToNumber(L, 3);
		int p = (int)Lua_ValueToNumber(L, 4);
		int lv = Lua_GetTopIndex(L) >= 5 ? (int)Lua_ValueToNumber(L, 5) : -1;
		if (nCnt > 0)
		{
			nDone = Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt, 0, g, d, p, lv, -1, pos_equiproom);
			if (nDone < nCnt)
				nDone += Player[nPlayerIndex].m_ItemList.RemoveCommonItem(nCnt - nDone, 0, g, d, p, lv, -1, pos_equiproomex);
		}
	}
	Lua_PushNumber(L, nDone);
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

// AddMapTrap(nMapID, nMpsX, nMpsY, szScript|nScriptId [, nParam]) -> 1/0
// port Hoat dong phuong (21/08), Linux 0x08102700. KHAC AddTrap cua ta: tham so 1 la
// MAP ID (qua SearchWorld), dat DUNG 1 o (nRange = 0), tham so 4 la so thi dung thang
// lam id. LECH CO CHU DICH: nParam (tham so 5) JX1 khong luu theo o (KRegion chi co
// m_dwTrap) - 0 call site trong missions\tong truyen no.
int LuaAddMapTrap(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop < 4)
		return 0;
	DWORD dwMapID = (DWORD)Lua_ValueToNumber(L, 1);
	int nMpsX = (int)Lua_ValueToNumber(L, 2);
	int nMpsY = (int)Lua_ValueToNumber(L, 3);
	DWORD dwTrapID = 0;
	if (Lua_IsNumber(L, 4))
		dwTrapID = (DWORD)Lua_ValueToNumber(L, 4);
	else
	{
		char* szScript = (char*)Lua_ValueToString(L, 4);
		if (!szScript) { Lua_PushNumber(L, 0); return 1; }
		dwTrapID = (DWORD)g_FileName2Id(szScript);
	}
	int nSubWorldIndex = g_SubWorldSet.SearchWorld(dwMapID);
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD) { Lua_PushNumber(L, 0); return 1; }
	int nRegion = -1, nMapX = 0, nMapY = 0, nOffX = 0, nOffY = 0;
	SubWorld[nSubWorldIndex].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0) { Lua_PushNumber(L, 0); return 1; }
	SubWorld[nSubWorldIndex].SetTrap(dwTrapID, nMpsX, nMpsY, 0);
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

// ChangeOwnFeature(nType, nTime, nIdx [, nHelm, nArmor, nWeapon, nHorse]) : port Hoat dong
// phuong (21/08), Linux 0x08131350 -> KNpc::ChangeFeature/SetFeature/SyncFeature.
// nType 0 = doi hinh VINH VIEN (toi khi RestoreOwnFeature); 1 = co han nTime frame.
// JX1 khong co "feature" rieng -> dung co che MAT NA co san (SetMask: m_MaskType dong bo qua
// PLAYER_NORMAL_SYNC, client ve bang template NPC). nIdx = dong npcs.txt cua JX1 (script
// port PHAI remap tu id Linux). LECH CO CHU DICH: nType 1 (co han) va nhanh nIdx<0 (doi
// 4 phan helm/armor/weapon/horse) khong co co che tuong duong -> coi nhu nType 0;
// call site duy nhat trong missions\tong la ChangeOwnFeature(0,0,<id>).
int LuaChangeOwnFeature(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop != 3 && nTop != 7)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 1);
	int nIdx = (int)Lua_ValueToNumber(L, 3);
	if (nType != 0 && nType != 1)
		return 0;
	if (nIdx <= 0)
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0 || Player[nPlayerIndex].m_nIndex >= MAX_NPC)
		return 0;
	Player[nPlayerIndex].m_ItemList.SetMaskLock(TRUE);
	Npc[Player[nPlayerIndex].m_nIndex].m_MaskType = nIdx;
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
	case 1: case 2: case 3: case 4: case 5: case 6:
		// [TIN SU 21/08] kind 1 truoc tra so luong chong - KHONG script nao trong cay dung
		// (quet 21/08: chi item\messenger\toll_*.lua + seasonnpc_expitem.lua goi (idx,1) va
		// ca hai deu theo nghia Linux = o tham so 1 - bo dem so lan dung yeu bai Tin Su).
		// [TONG 21/08] o tham so rieng 2..6 = SetSpecItemParam (Linux GetItemParam(idx,k) doc
		// KItem+0x1e0+k*4 = m_GeneratorParam.nGeneratorLevel[k-1]). Kind 1 giu = so luong chong.
		if (nKind - 1 < MAX_ITEM_MAGICLEVEL)
			nResult = Item[nItemIndex].m_GeneratorParam.nGeneratorLevel[nKind - 1];
		break;
	default:
		break;
	}


	Lua_PushNumber(L, nResult);
	return 1;
}

// SetSpecItemParam(nItemIndex, nParamIdx, nValue) -> 1/0 : port Hoat dong phuong (21/08),
// Linux 0x080FF360 ghi int vao KItem+0x1e0+k*4 (k=1..6) = m_GeneratorParam.nGeneratorLevel[k-1]
// (cung bo cuc KItemGeneratorParam). KHONG doc PlayerIndex (goi duoc tu timer / item roi dat).
// LUU Y: tren trang bi, nGeneratorLevel la chi so sinh magic-attrib (Gen_ExistEquipment) -
// script goc chi dung tren item nhiem vu/hop, giu nguyen rang buoc do.
int LuaSetSpecItemParam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
	int nItemIndex = (int)Lua_ValueToNumber(L, 1);
	int nParamIdx = (int)Lua_ValueToNumber(L, 2);
	int nValue = (int)Lua_ValueToNumber(L, 3);
	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM) { Lua_PushNumber(L, 0); return 1; }
	if (Item[nItemIndex].GetIndex() <= 0) { Lua_PushNumber(L, 0); return 1; }
	if (nParamIdx < 1 || nParamIdx > 6 || nParamIdx - 1 >= MAX_ITEM_MAGICLEVEL) { Lua_PushNumber(L, 0); return 1; }
	Item[nItemIndex].m_GeneratorParam.nGeneratorLevel[nParamIdx - 1] = nValue;
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaGetSpecItemParam(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
	int nItemIndex = (int)Lua_ValueToNumber(L, 1);
	int nParamIdx = (int)Lua_ValueToNumber(L, 2);
	if (nItemIndex <= 0 || nItemIndex >= MAX_ITEM || nParamIdx < 1 || nParamIdx > 6 || nParamIdx - 1 >= MAX_ITEM_MAGICLEVEL)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, Item[nItemIndex].m_GeneratorParam.nGeneratorLevel[nParamIdx - 1]);
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

// AddTempMagic(nSkillId|szSkillName, nLevelDelta) : port Hoat dong phuong (21/08),
// Linux 0x0812C1B0 -> KSkillList::AllSkillV (tao o TempSkill / cong tru / xoa khi <=0)
// + danh sach ban ghi (m_SkillList+0xf08) de chieu tam SONG SOT qua UpdataCurData.
// JX1: AllSkillV co san; ban ghi = KPlayer::m_TongTempMagic, ap lai trong UpdataCurData.
int LuaTongAddTempMagic(Lua_State* L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1)
		return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return 0;
	int nSkillId = 0;
	if (Lua_IsNumber(L, 1))
		nSkillId = (int)Lua_ValueToNumber(L, 1);
	else
	{
		char* szSkill = (char*)Lua_ValueToString(L, 1);
		if (szSkill)
			g_OrdinSkillsSetting.GetInteger(szSkill, "SkillId", 0, &nSkillId);
	}
	if (nSkillId <= 0 || nSkillId >= MAX_SKILL)
		return 0;
	int nLevel = (nParamCount >= 2) ? (int)Lua_ValueToNumber(L, 2) : 0;
	if (nLevel == 0)
		return 0;
	int nNpcIdx = Player[nPlayerIndex].m_nIndex;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;

	Npc[nNpcIdx].m_SkillList.AllSkillV(nSkillId, nLevel);
	Player[nPlayerIndex].TongTempMagicRecord(nSkillId, nLevel);

	PLAYER_SKILL_LEVEL_SYNC NewSkill;
	NewSkill.ProtocolType = s2c_playerskilllevel;
	NewSkill.m_nSkillID = nSkillId;
	NewSkill.m_nSkillLevel = Npc[nNpcIdx].m_SkillList.GetLevel(nSkillId);
	NewSkill.m_nAddLevel = Npc[nNpcIdx].m_SkillList.GetAddLevel(nSkillId);
	NewSkill.m_nSkillExp = Npc[nNpcIdx].m_SkillList.GetExp(nSkillId);
	NewSkill.m_bTempSkill = Npc[nNpcIdx].m_SkillList.IsTempSkill(nSkillId);
	NewSkill.m_nLeavePoint = Player[nPlayerIndex].m_nSkillPoint;
	g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&NewSkill, sizeof(PLAYER_SKILL_LEVEL_SYNC));
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

// ---------------------------------------------------------------------------
// AddNpcEx(nId, nLevel, nSeries, nSubWorldIdx, nX32, nY32, nCamp, szName, nFlag)
//   port he Boss bang hoi / Bang hoi (21/08). 192 diem goi ben ban Linux.
//
// KHAC AddNpc cua du an o THU TU THAM SO: ban Linux dat nSeries o vi tri 3 va
// nSubWorldIdx o vi tri 4, con AddNpc cua ta dat nSubWorldIdx o 3 va nSeries o 6.
// Vi vay KHONG the bao AddNpcEx bang Lua ma khong sua script goc -> lam thanh
// ham engine rieng de script chep tu ban Linux giu nguyen tung byte.
//
// Da dich nguoc ban Linux (jx_linux_y 0x0811BF40) de xac dinh ngu nghia:
//   - tham so 1..6: y het AddNpc, chi khac thu tu; level ep ve [1,127];
//     id ghep bang MAKELONG(nLevel, nId) roi AddNpcSet2 - GIONG HET LuaAddNpc.
//   - tham so 1 nhan CA SO LAN CHUOI (chuoi -> tra bang NpcSetting, FindRow-2).
//   - tham so 7  -> ghi mot co BYTE tai KNpc+0x1824 cua engine Linux.
//   - tham so 8  -> ten NPC (chi dat khi chuoi khac rong).
//   - tham so 9  == 1 -> goi 0x08085250 (nap thuoc tinh NPC tu mau: doc
//                       [esi+0x1530]*0x2d0 + bang mau 0x836eb00) roi dat
//                       KNpc+0x181C = 3 (neu tham so 7 khac 0) hoac = 2.
//                  == 2 -> reset doi tuong con tai KNpc+0x88 roi goi voi 1000000.
//
// LECH CO CHU DICH (KNpc cua JX1 khong co hai truong 0x1824 / 0x181C):
//   * tham so 7 anh xa sang SetCurrentCamp - phe cua NPC. Moi diem goi ban goc
//     deu truyen 1, va camp 1 la gia tri hop le cua ta (camp_num guard).
//   * tham so 9 KHONG lam gi them: viec "nap thuoc tinh tu mau" ma ban Linux
//     lam o day thi AddNpcSet2 cua ta DA TU LAM ben trong. Danh Boss Hoang Kim
//     o ban goc den tu MAU NPC (goldboss.txt) chu khong tu co nay, nen bo qua
//     la dung ngu nghia. Giu tham so trong chu ky de script khong phai sua.
// ---------------------------------------------------------------------------
int LuaAddNpcEx(Lua_State* L)
{
	char*	pName = NULL;
	int		nId = 0;
	int		nTop = Lua_GetTopIndex(L);

	if (nTop < 6)
		return 0;

	if (Lua_IsNumber(L, 1))
	{
		nId = (int)Lua_ValueToNumber(L, 1);
	}
	else if (Lua_IsString(L, 1))
	{
		pName = (char*)lua_tostring(L, 1);
		if (!pName || !pName[0])
			return 0;
		nId = g_NpcSetting.FindRow(pName) - 2;
	}
	else
	{
		return 0;
	}
	if (nId < 0)
		nId = 0;

	int nLevel = (int)lua_tonumber(L, 2);
	if (nLevel >= 128) nLevel = 127;
	if (nLevel < 0)    nLevel = 1;

	int nSeries      = (int)lua_tonumber(L, 3);
	int nSubWorldIdx = (int)lua_tonumber(L, 4);
	int nX           = (int)lua_tonumber(L, 5);
	int nY           = (int)lua_tonumber(L, 6);

	int nNpcIdxInfo = MAKELONG(nLevel, nId);
	int nNpcIdx = NpcSet.AddNpcSet2(nNpcIdxInfo, nSeries, nSubWorldIdx, nX, nY);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, nNpcIdx);
		return 1;
	}

	// tham so 7: phe NPC (ban goc la mot co byte rieng - xem chu thich tren)
	if (nTop >= 7 && Lua_IsNumber(L, 7))
	{
		int nCamp = (int)Lua_ValueToNumber(L, 7);
		if (nCamp >= 0 && nCamp < camp_num)
			Npc[nNpcIdx].SetCurrentCamp(nCamp);
	}

	// tham so 8: ten hien thi - chi dat khi chuoi KHAC RONG (y het ban goc)
	if (nTop >= 8 && Lua_IsString(L, 8))
	{
		pName = (char*)lua_tostring(L, 8);
		if (pName && pName[0])
			g_StrCpy(Npc[nNpcIdx].Name, pName);
	}

	// tham so 9: co nap-thuoc-tinh cua ban Linux - AddNpcSet2 da tu lam, bo qua.

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
	// JX2 port: script bang hoi goi \script\tong\npc\... nhung cay that nam
	// o scriptjx2\tong_vn\npc\. Dung dung ham remap cua LuaIncludeFile - no
	// CHI doi khi duong goc khong ton tai nen khong dung toi NPC JX1 co san.
	// PHAN BIEN B3: sJX2RemapScriptPath quyet dinh bang fopen, ma duong
	// \script\... la TUONG DOI GOC O DIA (E:\script khong ton tai) nen ca
	// hai fopen deu truot va ham tra ve KHONG DOI GI. Phai probe bang duong
	// TUYET DOI (g_GetRootPath) roi luu lai dang TUONG DOI nhu moi NPC khac.
	char szNpcScript[MAX_PATH * 2];
	strncpy(szNpcScript, szScript, sizeof(szNpcScript) - 1);
	szNpcScript[sizeof(szNpcScript) - 1] = 0;
	if (szNpcScript[0] == '\\')
	{
		char szAbs[MAX_PATH * 2];
		g_GetRootPath(szAbs);
		int nRLen = (int)strlen(szAbs);
		if (nRLen > 0 && (szAbs[nRLen - 1] == '\\' || szAbs[nRLen - 1] == '/'))
			szAbs[nRLen - 1] = 0;
		strcat(szAbs, "\\");
		strcat(szAbs, szNpcScript + 1);
		FILE* pProbe = fopen(szAbs, "rb");
		if (pProbe)
			fclose(pProbe);
		else
		{
			// file goc khong co -> thu doi tien to nhu LuaIncludeFile
			static const char* szF[2] = { "\\script\\tong\\", "\\script\\lib\\" };
			static const char* szT[2] = { "\\scriptjx2\\tong_vn\\", "\\scriptjx2\\lib\\" };
			for (int nSw = 0; nSw < 2; nSw++)
			{
				int nFL = (int)strlen(szF[nSw]);
				if (strnicmp(szNpcScript, szF[nSw], nFL) != 0)
					continue;
				char szRel[MAX_PATH * 2];
				strcpy(szRel, szT[nSw]);
				strcat(szRel, szNpcScript + nFL);
				g_GetRootPath(szAbs);
				nRLen = (int)strlen(szAbs);
				if (nRLen > 0 && (szAbs[nRLen - 1] == '\\' || szAbs[nRLen - 1] == '/'))
					szAbs[nRLen - 1] = 0;
				strcat(szAbs, "\\");
				strcat(szAbs, szRel + 1);
				pProbe = fopen(szAbs, "rb");
				if (pProbe)
				{
					fclose(pProbe);
					strncpy(szNpcScript, szRel, sizeof(szNpcScript) - 1);
					szNpcScript[sizeof(szNpcScript) - 1] = 0;
				}
				break;
			}
		}
	}
	// ActionScript chi 80 byte - duong scriptjx2 dai nhat ~45 nen an toan,
	// nhung van cat cho chac
	strncpy(Npc[nNpcIndex].ActionScript, szNpcScript, sizeof(Npc[nNpcIndex].ActionScript) - 1);
	Npc[nNpcIndex].ActionScript[sizeof(Npc[nNpcIndex].ActionScript) - 1] = 0;
	Npc[nNpcIndex].m_ActionScriptID = g_FileName2Id(szNpcScript);
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
	// [WLLS 21/08] idx sai tra "" thay vi nil - script goc Linux concat thang
	// ket qua (wlls_npcname), nil lam gay ca ham thoai.
	if (Lua_GetTopIndex(L) < 1) { Lua_PushString(L, (char*)""); return 1; }
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) { Lua_PushString(L, (char*)""); return 1; }
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

// [PB 17/08] nPos BYTE->int + quy uoc moi: vi tri 0 = DOI TRUONG (m_nCaptain
// khong nam trong m_nMember[]; truoc day khong the lay tu Lua, con arg 0 thi
// doc m_nMember[-1] ngoai mang). Da Tau dung GetTeamMember(0..n) chia to doi.
int LuaGetTeamMem(Lua_State* L)
{
	int nTeamId = -1;
	int nPos = -1;
	int nMemberId = 0;

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum >= 2)
	{
		nTeamId = Lua_ValueToNumber(L, 1);
		nPos = (int)Lua_ValueToNumber(L, 2);
		if (nPos > 0 && nPos <= MAX_TEAM_MEMBER)
			nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
		else if (nPos == 0)
			nMemberId = g_Team[nTeamId].m_nCaptain;
	}
	else
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (Player[nPlayerIndex].m_cTeam.m_nFlag)
		{
			nTeamId = Player[nPlayerIndex].m_cTeam.m_nID;
			nPos = (int)Lua_ValueToNumber(L, 1);
			if (nPos > 0 && nPos <= MAX_TEAM_MEMBER)
				nMemberId = g_Team[nTeamId].m_nMember[nPos - 1];
			else if (nPos == 0)
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

// SetMoveSpeed(nSpeed) : port Hoat dong phuong (21/08), Linux 0x081216C0 - tren NGUOI CHOI
// HIEN HANH (PlayerIndex). nSpeed >= 0 khoa toc do; < 0 (-1/-2) mo khoa. = SetPlayerSpeed
// cua ta voi nPlayerIdx = PlayerIndex (cung m_bSpeedControl / UpdataCurData).
int LuaTongSetMoveSpeed(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nPlayerIdx = GetPlayerIndex(L);
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;
	int nNpcIdx = Player[nPlayerIdx].m_nIndex;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	int nSpeed = (int)Lua_ValueToNumber(L, 1);
	if (nSpeed < 0)
	{
		Player[nPlayerIdx].m_bSpeedControl = FALSE;
		Player[nPlayerIdx].UpdataCurData();
		return 0;
	}
	Npc[nNpcIdx].m_CurrentWalkSpeed = nSpeed;
	Npc[nNpcIdx].m_CurrentRunSpeed = nSpeed;
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

// JX2 port: script mo cua so bang hoi kieu JX2 tren client
int LuaOpenTongJX2(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex < 0) return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_OPENTONGJX2;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
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

// kho city (KJx2CityWar.cpp) - khai bao truoc diem dung
extern BOOL KJx2CityWar_SetTaxByCity(int nCityID, int nTax);
extern BOOL KJx2CityWar_SetOwnerByCity(int nCityID, const char* szOwnerRaw);

// DOT E (E3): het hardcode map 78 + het ghi SubWorld[-1] khi map chua nap.
// Duong ghi di qua kho KJx2CityWar (clamp 0..MaxExchangeTax, ghi MOI instance
// cung map id, persist mirror). Tham so 2 tuy chon = id thanh 1..7,
// mac dinh 5 (Tuong Duong, map 78) giu hanh vi cu.
int LuaSetThueTongOwnCity(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	int nThue = (int)Lua_ValueToNumber(L, 1);
	int nCityID = 5;
	if (nParamNum >= 2 && Lua_IsNumber(L, 2))
		nCityID = (int)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, KJx2CityWar_SetTaxByCity(nCityID, nThue) ? 1 : 0);
	return 1;
}

// DOT E (E3): het hardcode 78 + het strcpy khong gioi han vao m_CityOwnTong[32].
// Doi 1 = TEN THO cua bang (kho tu dem " %s " khi ghi SubWorld - khac Lua cu
// von tu format dem truoc); rong = xoa chu. Tham so 2 tuy chon = id thanh 1..7.
int LuaSetViewTongOwnCity(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0) return 0;
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1)
		return 0;
	const char* pszValue = (const char*)Lua_ValueToString(L, 1);
	int nCityID = 5;
	if (nParamNum >= 2 && Lua_IsNumber(L, 2))
		nCityID = (int)Lua_ValueToNumber(L, 2);
	Lua_PushNumber(L, KJx2CityWar_SetOwnerByCity(nCityID, pszValue) ? 1 : 0);
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
	// [PB 17/08] guard cu so sizeof(CON TRO) voi 64 = hang false -> strcpy
	// khong gioi han co the tran stack. Cat cung theo kich thuoc buffer.
	if (!szNotice)
		return 0;
	strncpy(FinishSync.m_szNotice, szNotice, sizeof(FinishSync.m_szNotice) - 1);
	FinishSync.m_szNotice[sizeof(FinishSync.m_szNotice) - 1] = 0;
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
			// [WLLS 20/08] script leaguematch (Linux) dat ten ham "InitMission",
			// mission cu cua du an dat "BeginMission" -> do ham co that trong
			// state dich roi goi dung ten; khong co InitMission thi giu nep cu.
			char szFun[16];
			strcpy(szFun, "BeginMission");
			{
				char szLowMs[MAX_PATH];
				g_StrCpyLen(szLowMs, szScript, MAX_PATH);
				g_StrLower(szLowMs);
				KLuaScript* pMs = (KLuaScript*)g_GetScript(szLowMs);
				if (pMs && pMs->m_LuaState)
				{
					int nMsTop = Lua_GetTopIndex(pMs->m_LuaState);
					Lua_GetGlobal(pMs->m_LuaState, "InitMission");
					if (lua_isfunction(pMs->m_LuaState, Lua_GetTopIndex(pMs->m_LuaState)))
						strcpy(szFun, "InitMission");
					lua_settop(pMs->m_LuaState, nMsTop);
				}
			}
			pMission->ExecuteScript(szScript, szFun, 0);
		}
	}

	return 0;
}

// [WLLS 20/08] Khoi phuc co che NewWorldScript cua MapList.ini (Linux GS co san,
// engine Windows bo quen): KNpc::ChangeWorld ban "OnLeaveWorld" cho map cu va
// "OnNewWorld" cho map moi; KPlayerSet::PrepareRemove ban "OnLeaveWorld" luc thoat.
// Script dich chay trong STATE RIENG cua file (giong moi mission script), voi
// SubWorld/PlayerIndex duoc bom truoc. Thieu script trong bo nap -> bo qua IM LANG
// (495 muc MapList tro newworldscript.lua von khong ton tai - khong duoc spam log).
#ifdef _SERVER
void KSubWorld_FireMapScript(int nSubWorldIndex, const char* szFun, int nPlayerIndex)
{
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return;
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
		return;
	const char* szScript = SubWorld[nSubWorldIndex].m_szNewWorldScript;
	if (!szScript[0])
		return;
	char szLow[MAX_PATH];
	g_StrCpyLen(szLow, (char*)szScript, MAX_PATH);
	g_StrLower(szLow);
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
	if (!pScript)
		return;
	Lua_PushNumber(pScript->m_LuaState, nSubWorldIndex);
	pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
	Lua_PushNumber(pScript->m_LuaState, nPlayerIndex);
	pScript->SetGlobalName(SCRIPT_PLAYERINDEX);
	int nTopIndex = 0;
	pScript->SafeCallBegin(&nTopIndex);
	pScript->CallFunction((char*)szFun, 0, (char*)"s",
		(void*)SubWorld[nSubWorldIndex].m_szNewWorldParam);
	pScript->SafeCallEnd(nTopIndex);
}
#endif // _SERVER (KSubWorld_FireMapScript)

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
		// [WLLS 20/08] FIX 2 loi lam sap GS ngay tran dau (BANGIAO_LIENDAU 4.1):
		// 1) duong dan sai chinh ta "misions\misionNN.lua" (bo qua missions.txt);
		// 2) g_GetScript tra NULL duoc ma van deref m_LuaState. Nay tra cuu bang
		// missions.txt dung nhu OpenMission/CloseMission + kiem NULL.
		char szScript[MAX_PATH];
		szScript[0] = 0;
		g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
		if (szScript[0])
		{
			g_StrLower(szScript);
			KLuaScript* pScript = (KLuaScript*)g_GetScript(szScript);
			if (pScript)
			{
				Lua_PushNumber(pScript->m_LuaState, nSubWorldIndex);
				pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);
				int nTopIndex = 0;
				pScript->SafeCallBegin(&nTopIndex);
				pScript->CallFunction("RunMission", 0, "d", nMissionId);
				pScript->SafeCallEnd(nTopIndex);
			}
			else
				g_DebugLog("[Mission] RunMission(%d): script chua nap [%.100s]", nMissionId, szScript);
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

	// [WLLS 21/08] ban Linux: DelMSPlayer(id, 0) = go CHINH NGUOI dang chay
	// script (combat\playerdeath.lua:11 + combat\newworld.lua:14 dua vao day).
	// Khong doi thi 0 chet o guard duoi -> nguoi chet/roi san van duoc dem khi
	// phan thang bai. Caller cu duy nhat truyen arg2 != 0 (citywar curcamp)
	// giu nguyen hanh vi.
	if (nPlayerIndex == 0)
		nPlayerIndex = GetPlayerIndex(L);

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
//////////////////////////////////////////////////////////////////////////////
// (20/08) Nam ham cho Lua de bo han duong ghi tep.
//
// Duong cu: script\lib\lib_ham.lua dung `openfile(f,"w+")` -- CAT TRANG tep roi
// moi ghi lai, khong nguyen tu, khong co lich su. Mat dien giua chung la mat CA
// BANG (so du ngan luong, tien vang song bac, boss hoang kim, xep hang su kien...).
//
// Duong moi: KV_Set ghi mot dong trong bang jx1_game.game_kv -- nguyen tu san, va
// ban cu duoc chup vao game_kv_history nen quay lui duoc.
//
//   KV_Set(ns, key, value)   -> 1 neu ghi duoc
//   KV_Get(ns, key)          -> chuoi gia tri, hoac nil neu khong co
//   KV_Del(ns, key)          -> 1 neu xoa duoc
//   GhiNhatKy(loai, ten, noidung) -> 1; LUON bat dong bo, khong lam cham game
//   LayLenhQuanTri(cmd)      -> chuoi tham so cua lenh cho xu ly cu nhat, hoac nil
//                               (thay cho viec doc tep dulieu/username_kick.txt)
//////////////////////////////////////////////////////////////////////////////
int LuaKV_Set(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
	char* szNs  = (char*)Lua_ValueToString(L, 1);
	char* szKey = (char*)Lua_ValueToString(L, 2);
	char* szVal = (char*)Lua_ValueToString(L, 3);
	if (!szNs || !szKey || !szVal) { Lua_PushNumber(L, 0); return 1; }
	// Bat dong bo: Lua goi ham nay trong vong lap game.
	int nOk = KGameKV::Put(szNs, szKey, szVal, (int)strlen(szVal), true) ? 1 : 0;
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaKV_Get(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) return 0;
	char* szNs  = (char*)Lua_ValueToString(L, 1);
	char* szKey = (char*)Lua_ValueToString(L, 2);
	if (!szNs || !szKey) return 0;
	// Lua 4 khong chiu duoc chuoi co byte 0 o giua, nen cap la chuoi ket NUL.
	static char s_szBuf[256 * 1024];
	int n = KGameKV::Get(szNs, szKey, s_szBuf, sizeof(s_szBuf) - 1);
	if (n < 0) return 0;
	s_szBuf[n] = 0;
	Lua_PushString(L, s_szBuf);
	return 1;
}

int LuaKV_Del(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) { Lua_PushNumber(L, 0); return 1; }
	char* szNs  = (char*)Lua_ValueToString(L, 1);
	char* szKey = (char*)Lua_ValueToString(L, 2);
	if (!szNs || !szKey) { Lua_PushNumber(L, 0); return 1; }
	Lua_PushNumber(L, KGameKV::Del(szNs, szKey) ? 1 : 0);
	return 1;
}

int LuaGhiNhatKy(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3) { Lua_PushNumber(L, 0); return 1; }
	char* szLoai = (char*)Lua_ValueToString(L, 1);
	char* szTen  = (char*)Lua_ValueToString(L, 2);
	char* szND   = (char*)Lua_ValueToString(L, 3);
	if (!szLoai || !szND) { Lua_PushNumber(L, 0); return 1; }
	Lua_PushNumber(L, KGameKV::LogStr(szLoai, szTen, szND) ? 1 : 0);
	return 1;
}

int LuaLayLenhQuanTri(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	char* szCmd = (char*)Lua_ValueToString(L, 1);
	if (!szCmd) return 0;
	char szArg[256];
	if (!KGameKV_LayLenhQuanTri(szCmd, szArg, sizeof(szArg)))
		return 0;
	Lua_PushString(L, szArg);
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

	// (20/08) Lay ten nhan vat de bang giftcode ghi lai AI da nhan ma.
	// Truoc day khong luu gi ca nen khong truy vet duoc khi co tranh chap.
	const char* szRole = 0;
	{
		int nPlayerIndex = GetPlayerIndex(L);
		if (nPlayerIndex > 0 && Player[nPlayerIndex].m_PlayerName[0])
			szRole = Player[nPlayerIndex].m_PlayerName;
	}

	if (!strcmp(szGiftType, "Code_Tuan"))
	{
		if (g_GiftCodeFanCungManager.UseCode(szGiftCode, szRole)) {
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
		if (g_GiftCodeNewManager.UseCode(szGiftCode, szRole)) {
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

// RemoveSkillState(nSkillId) - thu hoi mot trang thai skill dang gan tren
// nhan vat (hao quang / ky nang dai than khi cach chuc). Ban goc JX2 co ham
// cung ten; JX1 co san KNpc::ForceClearStateSkillEffect(nSkillId).
int LuaRemoveSkillState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Player[nPlayerIndex].m_nIndex <= 0)
		return 0;
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	if (nSkillId <= 0)
		return 0;
	Npc[Player[nPlayerIndex].m_nIndex].ForceClearStateSkillEffect(nSkillId);
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
		// (21/08) BOT khong co ket noi mang nen GetClientInfo LUON tra NULL - dong
		// nay ban ra MOI LAN bot cham vao GetIP(), hang tram lan mot tran Tong Kim.
		// Van tra chuoi rong nhu cu, chi bo khau in.
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
extern int LuaTONG_ApplyJoin(Lua_State* L);
extern int LuaTONG_GetApplyCount(Lua_State* L);
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
// ==== JX2 port dot 3: ham script Linux goi ma JX1 thieu ====
extern int LuaJX2_AddContribution(Lua_State* L);
extern int LuaJX2_AddCumulateOffer(Lua_State* L);
extern int LuaJX2_AddTongMTask(Lua_State* L);
extern int LuaJX2_AddWeekGoalOffer(Lua_State* L);
extern int LuaJX2_AddWeeklyOffer(Lua_State* L);
extern int LuaJX2_AskClientForNumber(Lua_State* L);
extern int LuaJX2_CheckTongMasterPower(Lua_State* L);
extern int LuaJX2_FormatTime2Number(Lua_State* L);
extern int LuaJX2_FormatTime2String(Lua_State* L);
extern int LuaJX2_GetContribution(Lua_State* L);
extern int LuaJX2_GetCumulateOffer(Lua_State* L);
extern int LuaJX2_GetCurServerTime(Lua_State* L);
extern int LuaJX2_GetNpcTong(Lua_State* L);
extern int LuaJX2_GetTong(Lua_State* L);
extern int LuaJX2_GetTongFigure(Lua_State* L);
extern int LuaJX2_GetTongLogData(Lua_State* L);
extern int LuaJX2_GetTongMTask(Lua_State* L);
extern int LuaJX2_GetTongMemberID(Lua_State* L);
extern int LuaJX2_GetWeekGoalOffer(Lua_State* L);
extern int LuaJX2_GetWeeklyOffer(Lua_State* L);
extern int LuaJX2_GlobalExecute(Lua_State* L);
extern int LuaJX2_OutputMsg(Lua_State* L);
extern int LuaJX2_SetTongMTask(Lua_State* L);
extern int LuaJX2_SetTongMaster(Lua_State* L);
extern int LuaJX2_SetWeeklyOffer(Lua_State* L);
extern int LuaJX2_String2Id(Lua_State* L);
extern int LuaJX2_SyncTaskValue(Lua_State* L);
extern int LuaJX2_TongClaimWar(Lua_State* L);
extern int LuaJX2_GetTongDuty(Lua_State* L);
extern int LuaJX2_Msg2PlayerByName(Lua_State* L);
extern int LuaJX2_WriteLog(Lua_State* L);
extern int LuaJX2_WriteStringToFile(Lua_State* L);
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
extern int LuaTWS_MaintainAll(Lua_State* L);
extern int LuaTONG_DailyMaintainAll(Lua_State* L);
// ==== DOT E cong thanh JX2: kho chia se giua cac Lua state (KJx2SharedStore.cpp) ====
extern int LuaOB_Create(Lua_State* L);
extern int LuaOB_Release(Lua_State* L);
extern int LuaOB_IsEmpty(Lua_State* L);
extern int LuaOB_Clear(Lua_State* L);
extern int LuaOB_Append(Lua_State* L);
extern int LuaOB_Copy(Lua_State* L);
extern int LuaOB_PushByte(Lua_State* L);
extern int LuaOB_PopByte(Lua_State* L);
extern int LuaOB_PushInt(Lua_State* L);
extern int LuaOB_PopInt(Lua_State* L);
extern int LuaOB_PushDouble(Lua_State* L);
extern int LuaOB_PopDouble(Lua_State* L);
extern int LuaOB_PushString(Lua_State* L);
extern int LuaOB_PopString(Lua_State* L);
extern int LuaLadder_NewLadder(Lua_State* L);
extern int LuaLadder_ClearLadder(Lua_State* L);
extern int LuaLadder_GetLadderInfo(Lua_State* L);
extern int LuaSetGlbValue(Lua_State* L);
extern int LuaGetGlbValue(Lua_State* L);
// ==== DOT E cong thanh JX2: League/GlbMission engine-side (KJx2League.cpp) ====
extern int LuaLG_CreateLeagueObj(Lua_State* L);
extern int LuaLG_FreeLeagueObj(Lua_State* L);
extern int LuaLG_SetLeagueInfo(Lua_State* L);
extern int LuaLG_AddMemberToObj(Lua_State* L);
extern int LuaLG_ApplyAddLeague(Lua_State* L);
extern int LuaLG_ApplyRemoveLeague(Lua_State* L);
extern int LuaLG_GetLeagueObj(Lua_State* L);
extern int LuaLG_GetLeagueObjByRole(Lua_State* L);
extern int LuaLG_GetFirstLeague(Lua_State* L);
extern int LuaLG_GetNextLeague(Lua_State* L);
extern int LuaLG_GetLeagueInfo(Lua_State* L);
extern int LuaLG_GetLeagueCreateTime(Lua_State* L);
extern int LuaLG_GetMemberCount(Lua_State* L);
extern int LuaLG_GetMemberInfo(Lua_State* L);
extern int LuaLG_GetMemberObj(Lua_State* L);
extern int LuaLG_GetMemberJoinTime(Lua_State* L);
extern int LuaLG_GetLeagueTask(Lua_State* L);
extern int LuaLG_ApplySetLeagueTask(Lua_State* L);
extern int LuaLG_ApplyAppendLeagueTask(Lua_State* L);
extern int LuaLG_GetMemberTask(Lua_State* L);
extern int LuaLG_ApplySetMemberTask(Lua_State* L);
extern int LuaLG_ApplyAppendMemberTask(Lua_State* L);
extern int LuaLGM_CreateMemberObj(Lua_State* L);
extern int LuaLGM_SetMemberInfo(Lua_State* L);
extern int LuaLGM_ApplyAddMember(Lua_State* L);
extern int LuaLGM_ApplyRemoveMember(Lua_State* L);
extern int LuaLGM_FreeMemberObj(Lua_State* L);
extern int LuaLG_ApplyDoScript(Lua_State* L);
extern int LuaOpenGlbMission(Lua_State* L);
extern int LuaStartGlbMSTimer(Lua_State* L);
extern int LuaStopGlbMSTimer(Lua_State* L);
// ==== DOT E cong thanh JX2: nhom CITY 7 thanh (KJx2CityWar.cpp) ====
extern int LuaGetCityOwner(Lua_State* L);
extern int LuaGetCityWarBothSides(Lua_State* L);
extern int LuaGetCityAreaName(Lua_State* L);
extern int LuaGetCityArea(Lua_State* L);
extern int LuaGetCitySummary(Lua_State* L);
extern int LuaGetAllCitySummary(Lua_State* L);
extern int LuaSyncCitySummary(Lua_State* L);
extern int LuaOpenCityManageUI(Lua_State* L);
extern int LuaHaveBeginWar(Lua_State* L);
extern int LuaNotifyWarResult(Lua_State* L);
extern int LuaAppointViceroy(Lua_State* L);
extern int LuaAppointChallenger(Lua_State* L);
extern int LuaIsSigningUp(Lua_State* L);
extern int LuaNumOfSignUpTongs(Lua_State* L);
extern int LuaGetSignUpTongName(Lua_State* L);
extern int LuaDisabledChatCity(Lua_State* L);
extern int LuaIsDisabledChatCity(Lua_State* L);
extern int LuaCTC_JX2_SetCityState(Lua_State* L);
// ==== DOT E cong thanh JX2 (E4): Arena idle + ArenaCredits (KJx2CityWar.cpp) ====
extern int LuaIsArenaBegin(Lua_State* L);
extern int LuaGetArenaBothSides(Lua_State* L);
extern int LuaGetArenaCityArea(Lua_State* L);
extern int LuaGetArenaLevel(Lua_State* L);
extern int LuaGetArenaTargetCity(Lua_State* L);
extern int LuaGetArenaTotalLevel(Lua_State* L);
extern int LuaGetArenaTotalLevelByCity(Lua_State* L);
extern int LuaGetArenaSchedule(Lua_State* L);
extern int LuaGetArenaInfoByCity(Lua_State* L);
extern int LuaNotifyArenaResult(Lua_State* L);
// [LOI DAI CN 21/08] 4 pha + bao danh (ten trung API relay goc)
extern int LuaStartSignUp(Lua_State* L);
extern int LuaEndSignUp(Lua_State* L);
extern int LuaStartArena(Lua_State* L);
extern int LuaStartCityWar(Lua_State* L);
extern int LuaSignUpCityWarArena(Lua_State* L);
extern int LuaGetCityWarTongCamp(Lua_State* L);
extern int LuaGetArenaCredits(Lua_State* L);
extern int LuaSetArenaCredits(Lua_State* L);
extern int LuaAddArenaCredits(Lua_State* L);
extern int LuaReduceArenaCredits(Lua_State* L);
// ==== DOT E cong thanh JX2 (E4): danh hieu doc lap (KJx2Title.cpp) ====
extern int LuaTitle_AddTitle(Lua_State* L);
extern int LuaTitle_ActiveTitle(Lua_State* L);
extern int LuaTitle_RemoveTitle(Lua_State* L);
extern int LuaTitle_GetTitleInfo(Lua_State* L);
extern int LuaTitle_GetTitleName(Lua_State* L);
extern int LuaTitle_GetActiveTitle(Lua_State* L);
extern int LuaTitle_GetTitleTab(Lua_State* L);
// ==== DOT E cong thanh JX2 (E4): khung BT_ battle (KJx2Battle.cpp) ====
extern int LuaBT_SetType2Task(Lua_State* L);
extern int LuaBT_GetData(Lua_State* L);
extern int LuaBT_SetData(Lua_State* L);
extern int LuaBT_SetTypeBonus(Lua_State* L);
extern int LuaBT_GetTypeBonus(Lua_State* L);
extern int LuaBT_SetView(Lua_State* L);
extern int LuaBT_SetMissionName(Lua_State* L);
extern int LuaBT_SetGameData(Lua_State* L);
extern int LuaBT_SetRestTime(Lua_State* L);
extern int LuaBT_SortLadder(Lua_State* L);
extern int LuaBT_GetTopTenInfo(Lua_State* L);
extern int LuaBT_UpdateMemberCount(Lua_State* L);
extern int LuaBT_ClearBattle(Lua_State* L);
extern int LuaBT_BroadView(Lua_State* L);
extern int LuaBT_BroadGameData(Lua_State* L);
extern int LuaBT_BroadAllLadder(Lua_State* L);
extern int LuaBT_BroadSelf(Lua_State* L);
extern int LuaBT_ClearPlayerData(Lua_State* L);
extern int LuaBT_LeaveBattle(Lua_State* L);
extern int LuaJx2SetMissionString(Lua_State* L);
extern int LuaJx2GetMissionString(Lua_State* L);
extern int LuaCTC_JX2_SetTax(Lua_State* L);
extern int LuaCTC_JX2_GetTax(Lua_State* L);
// ==== DOT E cong thanh JX2 (E4): ha tang tran (KJx2WarInfra.cpp) ====
extern int LuaSetNpcDeathScript(Lua_State* L);
extern int LuaClearMapNpc(Lua_State* L);
extern int LuaClearMapObj(Lua_State* L);			// [TONG 21/08] KJx2WarInfra.cpp
extern int LuaClearMapNpcWithName(Lua_State* L);
extern int LuaGetMapNpcWithName(Lua_State* L);
extern int LuaGetAroundNpcList(Lua_State* L);	// [TIN SU 21/08] KJx2WarInfra.cpp
// ==== Port SimCity (KSimCity.cpp): co danh dau bot giu can bang ====
extern int LuaSC_SetBotFlag(Lua_State* L);
extern int LuaSC_GetBotFlag(Lua_State* L);
extern int LuaSC_AddBot(Lua_State* L);
extern int LuaSC_DelBot(Lua_State* L);
extern int LuaSC_ClearBots(Lua_State* L);
extern int LuaSC_MoveOn(Lua_State* L);
extern int LuaSC_MoveOff(Lua_State* L);
extern int LuaSC_Goto(Lua_State* L);
extern int LuaSC_PatrolBox(Lua_State* L);
extern int LuaSC_LoadPreset(Lua_State* L);
extern int LuaSC_SetBotRoute(Lua_State* L);
extern int LuaSC_LoadChat(Lua_State* L);
extern int LuaSC_ChatChance(Lua_State* L);
extern int LuaSC_ClearChat(Lua_State* L);
// Port SimCity GD4 - lop thong tin bot
extern int LuaSC_SetBotLook(Lua_State* L);
extern int LuaSC_DressBot(Lua_State* L);
extern int LuaSC_SetBotFaction(Lua_State* L);
extern int LuaSC_SetBotInfo(Lua_State* L);
extern int LuaSC_SetBotTitle(Lua_State* L);
extern int LuaSC_SetBotTong(Lua_State* L);
extern int LuaSC_SetBotMate(Lua_State* L);
// Trang tri thanh thi 18/08 (KSimCity.cpp GD5)
extern int LuaSC_SetBotStall(Lua_State* L);
extern int LuaSC_SetBotSit(Lua_State* L);
extern int LuaSC_CityNodes(Lua_State* L);
extern int LuaSC_PickSpawn(Lua_State* L);
extern int LuaSC_RandomName(Lua_State* L);
// Bot KPlayer that (KPlayerBot.cpp) - khac bot NPC SimCity o tren
extern int LuaPB_AddBot(Lua_State* L);
extern int LuaPB_BotCount(Lua_State* L);
extern int LuaPB_ClearBot(Lua_State* L);
extern int LuaPB_JoinFaction(Lua_State* L);
extern int LuaPB_SetFight(Lua_State* L);
extern int LuaPB_SetChat(Lua_State* L);
extern int LuaPB_SaveAll(Lua_State* L);
extern int LuaPB_SetBuff(Lua_State* L);
extern int LuaPB_SetDaTau(Lua_State* L);
extern int LuaPB_SetBanSap(Lua_State* L);
extern int LuaPB_SetVeThanh(Lua_State* L);
extern int LuaPB_SetNpcChan(Lua_State* L);
extern int LuaPB_SetTongKim(Lua_State* L);
extern int LuaPB_SetTongKimTran(Lua_State* L);
extern int LuaPB_TongKimGoi(Lua_State* L);
extern int PB_IsBot(int nPlayerIdx);

// IsBot([nPlayerIdx]) -> 1 neu khe do la BOT do he KPlayerBot sinh ra, 0 neu KHONG.
//
// VI SAO CAN: bot la KPlayer THAT nen moi thu script nhin thay deu giong nguoi that -
// co ten, co phai, co trang bi, vao duoc to doi, an duoc su kien. Khong co ham nay thi
// moi script thuong/su kien deu co the tra thuong nham cho bot.
//
// KHONG duoc thay bang "m_nNetConnectIdx == -1": NGUOI CHOI UY THAC that cung mang -1.
// PB_IsBot doi chieu voi so bot dang song nen chi dung bot moi khop.
//
// Khong truyen tham so thi hoi ve CHINH nhan vat dang chay script.
int LuaIsBot(Lua_State* L)
{
	int nPlayerIndex = (Lua_GetTopIndex(L) >= 1)
		? (int)Lua_ValueToNumber(L, 1)
		: GetPlayerIndex(L);
	Lua_PushNumber(L, PB_IsBot(nPlayerIndex));
	return 1;
}
extern int LuaAddObstacleObj(Lua_State* L);
extern int LuaClearObstacleObj(Lua_State* L);
extern int LuaGetLoop(Lua_State* L);
extern int LuaGetNpcSettingIdx(Lua_State* L);
extern int LuaGetLastDiagNpc(Lua_State* L);
extern int LuaSetPKFlag(Lua_State* L);
extern int LuaForbidChangePK(Lua_State* L);
extern int LuaDisabledUseTownP(Lua_State* L);
extern int LuaRestoreOwnFeature(Lua_State* L);
extern int LuaGetPlayerRev(Lua_State* L);
extern int LuaSearchPlayer(Lua_State* L);
extern int LuaGetJoinTongTime(Lua_State* L);
extern int LuaGetTongMaster(Lua_State* L);
extern int LuaAddTongExp(Lua_State* L);
extern int LuaGetItemCountEx(Lua_State* L);
extern int LuaDelItemEx(Lua_State* L);
extern int LuaGetItemLife(Lua_State* L);
extern int LuaGetItemProp(Lua_State* L);
extern int LuaGiveItemUI(Lua_State* L);
extern int LuaGetGiveItemUnit(Lua_State* L);
extern int LuaRemoveItemByIndex(Lua_State* L);
extern int LuaPushString(Lua_State* L);
extern int LuaAppendString(Lua_State* L);
extern int LuaReplaceString(Lua_State* L);
extern int LuaPopString(Lua_State* L);
extern int LuaNW_GetSealInfo(Lua_State* L);
extern int LuaPARTNER_GetCurPartner(Lua_State* L);
extern int LuaPARTNER_GetSettingIdx(Lua_State* L);
extern int LuaSetSiegeVoitureParam(Lua_State* L);
extern int LuaSetMangonelParam(Lua_State* L);
#endif

// (dat NGOAI #ifdef _SERVER: bang dang ky duoc bien dich o CA client
// lan server nen dinh nghia cung phai co o ca hai)
// ---- stub cho script tac phuong JX2 (dieu tra dot C) ----
// GetPartnerBagLevel/SetPartnerBagLevel: he tui hanh trang dong hanh cua
// JX2 chua co tren JX1 - stub de menu 2 khu Le vat khong nil-crash.
int LuaJX2_PartnerBagStub(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// ChangeNpcFeature(npc,a,b,figure): doi hinh toa xuong khi len cap -
// JX1 chua co duong doi feature dong; stub no-op (chi mat hieu ung hinh).
int LuaJX2_ChangeNpcFeatureStub(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}


// ============================================================================
// == WLLS / leaguematch port 20/08/2026 - cac ham Lua bo sung ================
// == (xem D:\GAMEDEVNEW\THICONG_LIENDAU_PORT.md)              ================
// ============================================================================
#ifdef _SERVER
extern int LuaCloseGlbMission(Lua_State* L);
extern int LuaWllsTaskCentreStub(Lua_State* L);
extern int LuaWllsRandom(Lua_State* L);
extern int LuaNumber2Int(Lua_State* L);
extern int LuaTime2Tm(Lua_State* L);
extern int LuaGetGateWayClientID(Lua_State* L);
extern int LuaWllsIsCharged(Lua_State* L);
extern int LuaWllsLoadScript(Lua_State* L);
extern int LuaJX2_SyncTaskValueMore(Lua_State* L);

// AskClientForString(szCbFun, szDefault, nMin, nMax, szPrompt):
// mo hop nhap chuoi phia client (S2C_INPUT_BOX nhu OpenGetString), khi client
// tra loi thi goi szCbFun(chuoi) trong STATE cua script NPC dang thoai
// (KProtocolProcess.cpp nhanh m_bWllsAskStrArg). nMin/nMax: client goc tu gioi
// han; phia ta chuoi da bi chan 63 byte boi szStringInput, script tu strsub.
int LuaWllsAskClientForString(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || Lua_GetTopIndex(L) < 5)
		return 0;
	const char* szAction = Lua_ValueToString(L, 1);
	const char* szPrompt = Lua_ValueToString(L, 5);
	if (!szAction || !szAction[0])
		return 0;
	Player[nPlayerIndex].m_dwStrBoxId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;
	strncpy(Player[nPlayerIndex].m_szTaskExcuteFun, szAction, sizeof(Player[nPlayerIndex].m_szTaskExcuteFun) - 1);
	Player[nPlayerIndex].m_bWllsAskStrArg = 1;
	S2C_INPUT_BOX NetCommand;
	NetCommand.ProtocolType = s2c_inputbox;
	NetCommand.nType = 1;
	strncpy(NetCommand.Value, szPrompt ? szPrompt : "", sizeof(NetCommand.Value) - 1);
	NetCommand.Value[sizeof(NetCommand.Value) - 1] = 0;
	strncpy(NetCommand.Value1, szAction, sizeof(NetCommand.Value1) - 1);
	NetCommand.Value1[sizeof(NetCommand.Value1) - 1] = 0;
	if (g_pServer && Player[nPlayerIndex].m_nNetConnectIdx != -1)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_INPUT_BOX));
	return 0;
}

// GetRespect/AddRespect/SetRespect - uy danh = task 39 (TASKVALUE_STATTASK_RESPECT,
// KBuySell da biet tinh gia bang uy danh - moneyunit_respect).
int LuaWllsGetRespect(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	Lua_PushNumber(L, nPlayerIndex > 0 ?
		(int)Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT) : 0);
	return 1;
}
int LuaWllsAddRespect(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
		Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_RESPECT,
			Player[nPlayerIndex].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT) + (int)Lua_ValueToNumber(L, 1));
	return 0;
}
int LuaWllsSetRespect(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
		Player[nPlayerIndex].m_cTask.SetSaveVal(TASKVALUE_STATTASK_RESPECT, (int)Lua_ValueToNumber(L, 1));
	return 0;
}

// GetSkillState(nSkillId) -> level trang thai / -1
int LuaWllsGetSkillState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nRet = -1;
	if (nPlayerIndex > 0 && Player[nPlayerIndex].m_nIndex > 0 && Lua_IsNumber(L, 1))
		nRet = Npc[Player[nPlayerIndex].m_nIndex].GetStateSkillLevel((int)Lua_ValueToNumber(L, 1));
	Lua_PushNumber(L, nRet);
	return 1;
}

// GetLastAddFaction() -> TEN mon phai hien tai (schedule.lua:136 in danh sach doi thu)
int LuaWllsGetLastAddFaction(Lua_State* L)
{
	static char* s_szFaction[11] = {
		(char*)"Thi誹 L﹎",
		(char*)"Thi猲 Vng",
		(char*)"Л阯g M玭",
		(char*)"Ng� чc",
		(char*)"Nga Mi",
		(char*)"Th髖 Y猲",
		(char*)"C竔 Bang",
		(char*)"Thi猲 Nh蒼",
		(char*)"V� ng",
		(char*)"C玭 L玭",
		(char*)"Hoa S琻"
	};
	int nPlayerIndex = GetPlayerIndex(L);
	int nNo = -1;
	if (nPlayerIndex > 0)
		nNo = Player[nPlayerIndex].GetFactionNo();
	if (nNo >= 0 && nNo < 11)
		Lua_PushString(L, s_szFaction[nNo]);
	else
		Lua_PushString(L, (char*)"");
	return 1;
}

// GetBoxLockState() -> 1 khi ruong dang KHOA (chua mo khoa phien nay), 0 = tu do
int LuaWllsGetBoxLockState(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nLock = 0;
	if (nPlayerIndex > 0 && !Player[nPlayerIndex].m_CUnlocked)
		nLock = 1;
	Lua_PushNumber(L, nLock);
	return 1;
}

// CheckGlobalTradeFlag() -> cong tac giao dich toan cum cua relay goc; ta khong
// co cong tac nay -> luon cho phep (1) nhu mac dinh may chu goc.
int LuaWllsCheckGlobalTradeFlag(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}

// ST_CheckTextFilter(sz) -> 1 = ten sach, 0 = pham tu cam. Du an chua noi bo
// loc tu (FilterText chi dung phia client) -> chap nhan moi ten nhu hien trang.
int LuaWllsSTCheckTextFilter(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}

// safeshow(sz) - relay goc escape chuoi de hien thi trong hop thoai; ten nhan
// vat/doi cua ta khong chua ky tu pha hop thoai -> tra nguyen van.
int LuaWllsSafeshow(Lua_State* L)
{
	if (Lua_IsString(L, 1))
		Lua_PushString(L, (char*)Lua_ValueToString(L, 1));
	else
		Lua_PushString(L, (char*)"");
	return 1;
}

// CalcEquiproomItemCount(g,d,p,l) - dem item nguoi choi MANG THEO: hanh trang
// + tui mo rong. [21/08] cong them pos_equiproomex vi du an co tui ex (Linux
// 2010 chua co) - khong cong thi wlls_en_check lot do cam giau trong tui ex,
// va cac caller cu (bank/hoa/compose) dem thieu do dang mang.
int LuaWllsCalcEquiproomItemCount(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nCount = 0;
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 3)
	{
		int g = (int)Lua_ValueToNumber(L, 1);
		int d = (int)Lua_ValueToNumber(L, 2);
		int pt = (int)Lua_ValueToNumber(L, 3);
		int lv = Lua_GetTopIndex(L) >= 4 ? (int)Lua_ValueToNumber(L, 4) : -1;
		nCount = Player[nPlayerIndex].m_ItemList.CountCommonItem(0, g, d, pt, lv, -1, pos_equiproom)
			+ Player[nPlayerIndex].m_ItemList.CountCommonItem(0, g, d, pt, lv, -1, pos_equiproomex);
	}
	Lua_PushNumber(L, nCount);
	return 1;
}
// CalcItemCount(nPos,g,d,p[,l]) - dem item tai vi tri ITEM_POSITION nPos.
// [21/08] arg1 truyen THANG lam Place, khong hardcode: cap goc trong
// songjin_shophead.lua:132/139 dung CalcItemCount(3,..)+ConsumeItem voi
// 3 = pos_equiproom; league goi (1,..) = pos_hand de soat ca do CAM dang
// cam TREN TAY khi vao dau truong (wlls_en_check). nPos = -1: dem ca
// tay + hanh trang + tui mo rong (playerfunlib.lua:250).
int LuaWllsCalcItemCount(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nCount = 0;
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 4)
	{
		int nPos = (int)Lua_ValueToNumber(L, 1);
		int g = (int)Lua_ValueToNumber(L, 2);
		int d = (int)Lua_ValueToNumber(L, 3);
		int pt = (int)Lua_ValueToNumber(L, 4);
		int lv = Lua_GetTopIndex(L) >= 5 ? (int)Lua_ValueToNumber(L, 5) : -1;
		if (nPos >= pos_hand && nPos < pos_num)
			nCount = Player[nPlayerIndex].m_ItemList.CountCommonItem(0, g, d, pt, lv, -1, nPos);
		else if (nPos == -1)
			nCount = Player[nPlayerIndex].m_ItemList.CountCommonItem(0, g, d, pt, lv, -1, pos_hand)
				+ Player[nPlayerIndex].m_ItemList.CountCommonItem(0, g, d, pt, lv, -1, pos_equiproom)
				+ Player[nPlayerIndex].m_ItemList.CountCommonItem(0, g, d, pt, lv, -1, pos_equiproomex);
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

// ITEM_GetImmediaItemIndex(i) -> item index trong o dung-ngay thu i / 0.
// [21/08] Linux chi co 3 o, du an 9 o (IMMEDIACY_ROOM_WIDTH) - nhan du 9 de
// wlls_en_check (da doi vong lap 1..9) khong lot do cam dat o o 4-9.
int LuaWllsGetImmediaItemIndex(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nRet = 0;
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int i = (int)Lua_ValueToNumber(L, 1);
		if (i >= 1 && i <= IMMEDIACY_ROOM_WIDTH)
			nRet = Player[nPlayerIndex].m_ItemList.m_Room[room_immediacy].FindItem(i - 1, 0);
	}
	Lua_PushNumber(L, nRet);
	return 1;
}

// CountFreeRoomByWH(w,h[,nNeed]) -> SO cho trong WxH dat duoc (hanh trang +
// tui mo rong neu con han). [21/08] truoc tra 1/0 theo FindRoom - du cho
// league (hongyin/jindan so sanh < 1) nhung composeex.lua:333 nhan limit
// theo so lan ghep, itemblue.lua:129 so voi so luong nhap -> can SO THAT;
// dung KItemList::CalcFreeItemCellCount (KInventory::FindFreeCell dem cho).
int LuaWllsCountFreeRoomByWH(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nRet = 0;
	if (nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 2)
	{
		int w = (int)Lua_ValueToNumber(L, 1);
		int h = (int)Lua_ValueToNumber(L, 2);
		if (w > 0 && h > 0)
		{
			nRet = Player[nPlayerIndex].m_ItemList.CalcFreeItemCellCount(w, h, room_equipment);
			if (Player[nPlayerIndex].m_dwEquipExpandTime - KSG_GetCurSec() > 0)
				nRet += Player[nPlayerIndex].m_ItemList.CalcFreeItemCellCount(w, h, room_equipmentex);
		}
	}
	Lua_PushNumber(L, nRet);
	return 1;
}

// DisabledStall / ForbitTrade / ForbitStamina (wlls_set_pl_state / clear)
int LuaWllsDisabledStall(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Player[nPlayerIndex].m_bWllsDisableStall = (Lua_IsNumber(L, 1) && (int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
	return 0;
}
int LuaWllsForbitTrade(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Player[nPlayerIndex].m_bWllsForbidTrade = (Lua_IsNumber(L, 1) && (int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
	return 0;
}
int LuaWllsForbitStamina(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Player[nPlayerIndex].m_bWllsForbidStamina = (Lua_IsNumber(L, 1) && (int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
	return 0;
}

// =====================================================================
// [TONG 21/08] port 3 Hoat dong Phuong bang hoi (missions\tong) - dac ta tung ham dich
// nguoc tu jx_linux_y: ReverseTools\dac_ta_17_ham_hoatdong_phuong.json
// =====================================================================

// ForbitSkill(nFlag): Linux 0x08121620 ghi co cam TOAN BO (KSkillList+4) + tung khe
// (+0x2C), diem chan KSkillList::CanCast. JX1: KPlayer::m_bTongForbidSkill, chan tai
// KSkill::CanCastSkill. LECH: Linux gui goi 0x63/0x13 de client xam thanh chieu - JX1 khong.
int LuaTongForbitSkill(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
	Player[nPlayerIndex].m_bTongForbidSkill = ((int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
	return 0;
}

// SetAForbitSkill(nSkillId, nFlag): Linux 0x08121580 ghi co vao khe NPCSKILL (+0x2C).
// JX1: danh sach 8 o tren KPlayer (khong doi layout NPCSKILL dung chung client).
int LuaTongSetAForbitSkill(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nFlag = (int)Lua_ValueToNumber(L, 2);
	if (nSkillId <= 0 || nSkillId >= MAX_SKILL) return 0;
	int* pList = Player[nPlayerIndex].m_nTongForbidSkillId;
	int i;
	for (i = 0; i < 8; i++)
		if (pList[i] == nSkillId) break;
	if (nFlag)
	{
		if (i < 8) return 0;
		for (i = 0; i < 8; i++)
			if (pList[i] == 0) { pList[i] = nSkillId; break; }
	}
	else if (i < 8)
		pList[i] = 0;
	return 0;
}

// ForbitAura(nFlag): Linux 0x08111560 ghi Player+0x375; flag != 0 thi SetAuraSkill(0)
// tat ngay; diem chan = KProtocolProcess::ChangeAuraSkill. Linux KHONG kiem gettop.
int LuaTongForbitAura(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
	int nFlag = (Lua_IsNumber(L, 1) && (int)Lua_ValueToNumber(L, 1) != 0) ? 1 : 0;
	if (nFlag)
	{
		int nNpc = Player[nPlayerIndex].m_nIndex;
		if (nNpc > 0 && nNpc < MAX_NPC)
			Npc[nNpc].SetAuraSkill(0);
	}
	Player[nPlayerIndex].m_bTongForbidAura = (BYTE)nFlag;
	return 0;
}

// ForbidEnmity(nFlag): Linux 0x0810B0F0 ghi Player+0x5a5c = (nFlag == 1) - chu y DUNG
// BANG 1 (sete), khong phai != 0; diem doc duy nhat = c2sPKApplyEnmity (tu choi im lang).
int LuaTongForbidEnmity(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
	Player[nPlayerIndex].m_bTongForbidEnmity = ((int)Lua_ValueToNumber(L, 1) == 1) ? 1 : 0;
	return 0;
}

// SetImmedSkill(nSlot, nSkillId): Linux 0x08114C70 CHI gui goi client (proto 0x63 sub 0x16),
// khong ghi gi phia server. JX1: S2C_PLAYER_SYNC_M_A (x64/Win32 an toan) voi sub-id moi
// enumS2C_PLAYERSYNC_ID_IMMEDSKILL (them CUOI enum), client SetRightSkill/SetLeftSkill.
// Thu tu trong Lua phai giu: AddTempMagic TRUOC SetImmedSkill (client can biet chieu).
int LuaTongSetImmedSkill(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2) return 0;
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER) return 0;
	int nSlot = (int)Lua_ValueToNumber(L, 1);
	int nSkillId = (int)Lua_ValueToNumber(L, 2);
	if (nSlot < 0 || nSlot > 1 || nSkillId < 0) return 0;
	S2C_PLAYER_SYNC_M_A sMsg;
	sMsg.ProtocolType = s2c_playersync_magic_attr;
	sMsg.nPoint = (DWORD)(((DWORD)nSlot << 24) | ((DWORD)nSkillId & 0xFFFFFF));
	sMsg.nType = enumS2C_PLAYERSYNC_ID_IMMEDSKILL;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sMsg, sizeof(S2C_PLAYER_SYNC_M_A));
	return 0;
}

// TaskNo(szTaskName) -> TaskID | nil : tra bang \settings\task\task_id.txt (cot 1 TaskID,
// cot 2 TaskName, du lieu tu dong 3). Linux nap luc boot vao std::map (0x09786620);
// JX1 nap lazy lan goi dau. Trung ten: giu ban DAU (map::insert). So sanh phan biet hoa/thuong.
static std::map<std::string, int> s_mapTongTaskName2Id;
static int s_nTongTaskIdLoaded = 0;
static void sTongLoadTaskIdTab()
{
	s_nTongTaskIdLoaded = 1;
	KTabFile cTab;
	if (!cTab.Load("\\settings\\task\\task_id.txt"))
		return;
	char szName[260];
	for (int nRow = 3; nRow <= cTab.GetHeight(); nRow++)
	{
		int nId = 0;
		szName[0] = 0;
		if (!cTab.GetInteger(nRow, 1, 0, &nId) || nId <= 0) continue;
		if (!cTab.GetString(nRow, 2, "", szName, sizeof(szName) - 1) || !szName[0]) continue;
		if (s_mapTongTaskName2Id.find(szName) == s_mapTongTaskName2Id.end())
			s_mapTongTaskName2Id[szName] = nId;
	}
}
int LuaTongTaskNo(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1) return 0;
	const char* szName = (const char*)Lua_ValueToString(L, 1);
	if (!szName) return 0;
	if (!s_nTongTaskIdLoaded) sTongLoadTaskIdTab();
	std::map<std::string, int>::iterator it = s_mapTongTaskName2Id.find(szName);
	if (it == s_mapTongTaskName2Id.end()) { Lua_PushNil(L); return 1; }
	Lua_PushNumber(L, it->second);
	return 1;
}

// ST_*DamageCounter - bo dem sat thuong HUNG CHIU (hook o KNpc.cpp truoc khoi
// chuyen-noi-luc; xem THICONG muc C5). Start reset ve 0, Stop giu gia tri.
int LuaWllsSTStartDamageCounter(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
	{
		Player[nPlayerIndex].m_nWllsDmgCounter = 0;
		Player[nPlayerIndex].m_bWllsDmgCounterOn = 1;
	}
	return 0;
}
int LuaWllsSTStopDamageCounter(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0)
		Player[nPlayerIndex].m_bWllsDmgCounterOn = 0;
	return 0;
}
int LuaWllsSTGetDamageCounter(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	Lua_PushNumber(L, nPlayerIndex > 0 ? Player[nPlayerIndex].m_nWllsDmgCounter : 0);
	return 1;
}
int LuaWllsSTIncreaseDamageCounter(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
		Player[nPlayerIndex].m_nWllsDmgCounter += (int)Lua_ValueToNumber(L, 1);
	return 0;
}

// GetStringTask/SetStringTask - o chuoi PHIEN (khong luu DB): helper.lua chi
// dung id 5 lam con tro trang duyet danh sach; relog mat con tro = vo hai.
int LuaWllsGetStringTask(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		if (nId >= 0 && nId < 8)
		{
			Lua_PushString(L, Player[nPlayerIndex].m_szWllsStrTask[nId]);
			return 1;
		}
	}
	Lua_PushString(L, (char*)"");
	return 1;
}
int LuaWllsSetStringTask(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2))
	{
		int nId = (int)Lua_ValueToNumber(L, 1);
		if (nId >= 0 && nId < 8)
		{
			strncpy(Player[nPlayerIndex].m_szWllsStrTask[nId], Lua_ValueToString(L, 2), 63);
			Player[nPlayerIndex].m_szWllsStrTask[nId][63] = 0;
		}
	}
	return 0;
}
#endif // _SERVER (khoi ham WLLS)

TLua_Funcs GameScriptFuns[] =
{
	{"Say", LuaSelectUI},
	{"Describe", LuaSelectUI},	// script tac phuong JX2 dung Describe nhu Say
	{"GetPartnerBagLevel", LuaJX2_PartnerBagStub},
	{"SetPartnerBagLevel", LuaJX2_PartnerBagStub},
	{"ChangeNpcFeature", LuaJX2_ChangeNpcFeatureStub},
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
	{"IncludeLib",LuaIncludeLib},		// JX2 port
	// FIX 14/08: "IL" la ALIAS ngan cua IncludeLib trong script JX2 goc
	// (tong_vn\tong_statistics.lua:4 IL("TONG"), workshop\ws_huodong.lua:1,
	// global\titlefuncs.lua:1 IL("TITLE")...). Cay ta chua bao gio dinh nghia
	// -> "attempt to call global 'IL'" lam cac file do nap DUT giua chung.
	// Loi CO SAN tu dot C; truoc hom nay khong lo vi boot chet som hon o cho khac.
	{"IL",LuaIncludeLib},
	{"_ALERT",LuaGameAlert},	// [WLLS 21/08] loi runtime Lua ghi nguyen van vao ScriptError.log
	// == DA TAU TASKLINK (JX2 port) 15/08/2026 - xem DANHSACH_DATAU_PORT.md ==
	{"C_Random",			LuaC_Random},
	{"SetRandSeed",			LuaSetRandSeed},
	{"GetTiredDegree",		LuaGetTiredDegree},
#ifdef _SERVER
	// LuaGetTeamMem chi duoc dinh nghia khi _SERVER (ScriptFuns.cpp:6734), giong het dong
	// dang ky goc {"GetTeamMem", ...} von da nam trong #ifdef _SERVER. Alias nay bi bo quen
	// ngoai guard -> build Client bao C2065, keo theo C2070 o sizeof(GameScriptFuns).
	{"GetTeamMember",		LuaGetTeamMem},			// alias GetTeamMem - cung chu ky (nPos 1-based)
#endif
	{"GetBitTask",			LuaGetBitTask},
	{"SetBitTask",			LuaSetBitTask},
	{"GetItemMagicAttrib",	LuaGetItemMagicAttrib},
	{"SetItemMagicLevel",	LuaSetItemMagicLevel},	// stub co chu dich - duong 1475 ngu dong
	{"SyncItem",			LuaSyncItemJX2},
	{"curpack",				LuaCurPack},
	{"usepack",				LuaUsePack},
	{"CallPlayerFunction",	LuaCallPlayerFunction},
	{"GetLastFactionNumber",LuaGetLastFactionNumber},
	{"TM_SetTimer",			LuaTM_SetTimer},		// storm ngu dong
	{"TM_GetRestCount",		LuaTM_GetRestCount},	// tra nil -> storm_valid_game=false
	{"BT_GetGameData",		LuaBT_GetDataStub},
	{"BT_GetData",			LuaBT_GetDataStub},
	{"Prise",				LuaPrise},
	{"DynamicExecuteByPlayer",	LuaDynamicExecuteByPlayer},
	{"GetProductRegion",LuaGetProductRegion},	// JX2 port
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
	{"RemoveSkillState",	LuaRemoveSkillState},
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
	{"ConsumeEquiproomItem",	LuaConsumeEquiproomItem},	// [TIN SU 21/08] chi hanh trang
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
	{"ChangeOwnFeature",	LuaChangeOwnFeature},	// [TONG 21/08] doi ngoai hinh (co che mat na)
	{"ReSetMask",		LuaReSetMask},
	{"SetPos",			LuaSetPos},			//SetPos(x,y)
	{"GetPos",			LuaGetPos},			//GetPos() return x,y,subworldindex
	{"GetWorldPos",		LuaGetNewWorldPos},	//W,X,Y = GetWorldPos()
	{"NewWorld",		LuaEnterNewWorld},
	{"RandomNew",			LuaRandomNew},	//SetTask(任务号,值):设置任务值
	{"AddTrap",			LuaAddTrap},
	{"AddMapTrap",		LuaAddMapTrap},	// [TONG 21/08] (mapId,x,y,script|id) 1 o
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
	{ "SetSpecItemParam",	LuaSetSpecItemParam },	// [TONG 21/08] o tham so rieng 1..6
	{ "GetSpecItemParam",	LuaGetSpecItemParam },
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
	{"AddTempMagic",	LuaTongAddTempMagic},	// [TONG 21/08] chieu tam AllSkillV + ban ghi
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
	{"SubWorldIdx2MapCopy",	LuaSubWorldIdx2MapCopy}, // port Boss bang hoi 21/08
	{"FileName2Id",		LuaFileName2Id},	// [TONG 21/08] bam ten/duong dan -> DWORD
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
	{"AddNpcEx",		LuaAddNpcEx},		//AddNpcEx - port Boss bang hoi 21/08
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
	{"SC_SetBotFlag",	LuaSC_SetBotFlag},	// Port SimCity
	{"SC_GetBotFlag",	LuaSC_GetBotFlag},
	{"SC_AddBot",		LuaSC_AddBot},
	{"SC_DelBot",		LuaSC_DelBot},
	{"SC_ClearBots",	LuaSC_ClearBots},
	{"SC_MoveOn",		LuaSC_MoveOn},		// Port SimCity GD2
	{"SC_MoveOff",		LuaSC_MoveOff},
	{"SC_Goto",		LuaSC_Goto},
	{"SC_PatrolBox",	LuaSC_PatrolBox},
	{"SC_LoadPreset",	LuaSC_LoadPreset},
	{"SC_SetBotRoute",	LuaSC_SetBotRoute},
	{"SC_LoadChat",		LuaSC_LoadChat},	// Port SimCity GD3
	{"SC_ChatChance",	LuaSC_ChatChance},
	{"SC_ClearChat",		LuaSC_ClearChat},
	// Port SimCity GD4 - lop thong tin bot (ngoai trang / mon phai / danh hieu / bang hoi)
	{"SC_SetBotLook",	LuaSC_SetBotLook},
	{"SC_DressBot",		LuaSC_DressBot},
	{"SC_SetBotFaction",	LuaSC_SetBotFaction},
	{"SC_SetBotInfo",	LuaSC_SetBotInfo},
	{"SC_SetBotTitle",	LuaSC_SetBotTitle},
	{"SC_SetBotTong",	LuaSC_SetBotTong},
	{"SC_SetBotMate",	LuaSC_SetBotMate},
	{"SC_SetBotStall",	LuaSC_SetBotStall},	// Trang tri thanh thi 18/08
	{"SC_SetBotSit",	LuaSC_SetBotSit},
	{"SC_CityNodes",	LuaSC_CityNodes},
	{"SC_PickSpawn",	LuaSC_PickSpawn},
	{"SC_RandomName",	LuaSC_RandomName},
	// Bot KPlayer that: goi bot nap tu roledb qua Goddess
	{"PB_AddBot",		LuaPB_AddBot},
	{"PB_BotCount",		LuaPB_BotCount},
	{"PB_ClearBot",		LuaPB_ClearBot},
	{"PB_JoinFaction",	LuaPB_JoinFaction},
	{"PB_SetFight",		LuaPB_SetFight},
	{"PB_SetChat",		LuaPB_SetChat},
	{"PB_SaveAll",		LuaPB_SaveAll},
	{"PB_SetBuff",		LuaPB_SetBuff},		// (19/08) bat/tat cham TTL + Que Hoa Tuu
	{"PB_SetDaTau",		LuaPB_SetDaTau},	// (19/08) gioi han so bot lam Da Tau
	{"PB_SetBanSap",	LuaPB_SetBanSap},	// (19/08) so bot ra thanh ngoi ban sap
	{"PB_SetVeThanh",	LuaPB_SetVeThanh},	// (19/08 toi) goi het bot ve thanh/thon chia deu (1=bat 0=tat -1=doc)
	{"PB_SetNpcChan",	LuaPB_SetNpcChan},	// (20/08 dem) NPC la tuong: 1=bat nhu goc, 0=dung chong len nhau (mac dinh), -1=doc
	{"PB_SetTongKim",	LuaPB_SetTongKim},	// (21/08) bot tu tham gia Tong Kim: 1=bat 0=tat -1=doc
	{"PB_SetTongKimTran",	LuaPB_SetTongKimTran},	// tran so bot moi tran; 0 = KHONG gioi han
	{"PB_TongKimGoi",	LuaPB_TongKimGoi},	// goi bot vao tran NGAY (lenh bai admin)
	{"IsBot",			LuaIsBot},
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
	{"SetMoveSpeed",		LuaTongSetMoveSpeed },	// [TONG 21/08] tren PlayerIndex
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
		{"OpenTongJX2",	LuaOpenTongJX2},	// JX2 port
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
	{ "TONG_ApplyJoin",	LuaTONG_ApplyJoin },
	{ "TONG_GetApplyCount",	LuaTONG_GetApplyCount },
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
		// ==== JX2 port dot 3: ham script Linux can ====
		{ "GetContribution",	LuaJX2_GetContribution },
		{ "AddContribution",	LuaJX2_AddContribution },
		{ "GetWeeklyOffer",	LuaJX2_GetWeeklyOffer },
		{ "AddWeeklyOffer",	LuaJX2_AddWeeklyOffer },
		{ "SetWeeklyOffer",	LuaJX2_SetWeeklyOffer },
		{ "GetCumulateOffer",	LuaJX2_GetCumulateOffer },
		{ "AddCumulateOffer",	LuaJX2_AddCumulateOffer },
		{ "GetWeekGoalOffer",	LuaJX2_GetWeekGoalOffer },
		{ "AddWeekGoalOffer",	LuaJX2_AddWeekGoalOffer },
		{ "GetCurServerTime",	LuaJX2_GetCurServerTime },
		{ "GetSysCurrentTime",	LuaJX2_GetCurServerTime },
		{ "GetCurrentTime",	LuaJX2_GetCurServerTime },
		{ "FormatTime2String",	LuaJX2_FormatTime2String },
		{ "FormatTime2Number",	LuaJX2_FormatTime2Number },
		{ "String2Id",	LuaJX2_String2Id },
		{ "OutputMsg",	LuaJX2_OutputMsg },
		{ "WriteLog",	LuaJX2_WriteLog },
		{ "WriteStringToFile",	LuaJX2_WriteStringToFile },
		{ "GlobalExecute",	LuaJX2_GlobalExecute },
		{ "SyncTaskValue",	LuaJX2_SyncTaskValue },
#ifdef _SERVER
		// == WLLS / leaguematch port 20/08/2026 ==
		{ "SyncTaskValueMore",	LuaJX2_SyncTaskValueMore },
		{ "GetGblInt",	LuaGetGlbValue },
		{ "SetGblInt",	LuaSetGlbValue },
		{ "Random",	LuaWllsRandom },
		{ "Number2Int",	LuaNumber2Int },
		{ "Time2Tm",	LuaTime2Tm },
		{ "TaskName",	LuaWllsTaskCentreStub },
		{ "TaskTime",	LuaWllsTaskCentreStub },
		{ "TaskInterval",	LuaWllsTaskCentreStub },
		{ "TaskCountLimit",	LuaWllsTaskCentreStub },
		{ "CloseGlbMission",	LuaCloseGlbMission },
		{ "GetGateWayClientID",	LuaGetGateWayClientID },
		{ "IsCharged",	LuaWllsIsCharged },
		{ "LoadScript",	LuaWllsLoadScript },
		{ "DynamicExecute",	LuaDynamicExecute },
		{ "AskClientForString",	LuaWllsAskClientForString },
		{ "GetRespect",	LuaWllsGetRespect },
		{ "AddRespect",	LuaWllsAddRespect },
		{ "SetRespect",	LuaWllsSetRespect },
		{ "GetSkillState",	LuaWllsGetSkillState },
		{ "GetLastAddFaction",	LuaWllsGetLastAddFaction },
		{ "GetBoxLockState",	LuaWllsGetBoxLockState },
		{ "CheckGlobalTradeFlag",	LuaWllsCheckGlobalTradeFlag },
		{ "ST_CheckTextFilter",	LuaWllsSTCheckTextFilter },
		{ "safeshow",	LuaWllsSafeshow },
		{ "CalcItemCount",	LuaWllsCalcItemCount },
		{ "CalcEquiproomItemCount",	LuaWllsCalcEquiproomItemCount },
		{ "ITEM_GetImmediaItemIndex",	LuaWllsGetImmediaItemIndex },
		{ "CountFreeRoomByWH",	LuaWllsCountFreeRoomByWH },
		{ "DisabledStall",	LuaWllsDisabledStall },
		{ "ForbitTrade",	LuaWllsForbitTrade },
		{ "ForbitStamina",	LuaWllsForbitStamina },
		// [TONG 21/08] 3 Hoat dong Phuong bang hoi (missions\tong)
		{ "ForbitSkill",		LuaTongForbitSkill },
		{ "SetAForbitSkill",	LuaTongSetAForbitSkill },
		{ "ForbitAura",		LuaTongForbitAura },
		{ "ForbidEnmity",		LuaTongForbidEnmity },
		{ "SetImmedSkill",		LuaTongSetImmedSkill },
		{ "TaskNo",			LuaTongTaskNo },
		{ "ST_StartDamageCounter",	LuaWllsSTStartDamageCounter },
		{ "ST_StopDamageCounter",	LuaWllsSTStopDamageCounter },
		{ "ST_GetDamageCounter",	LuaWllsSTGetDamageCounter },
		{ "ST_IncreaseDamageCounter",	LuaWllsSTIncreaseDamageCounter },
		{ "GetStringTask",	LuaWllsGetStringTask },
		{ "SetStringTask",	LuaWllsSetStringTask },
#endif
		{ "AskClientForNumber",	LuaJX2_AskClientForNumber },
		{ "GetTong",	LuaJX2_GetTong },
		{ "GetCurrentTong",	LuaJX2_GetTong },
		{ "GetTongMemberID",	LuaJX2_GetTongMemberID },
		{ "GetTongFigure",	LuaJX2_GetTongFigure },
		{ "CheckTongMasterPower",	LuaJX2_CheckTongMasterPower },
		{ "GetNpcTong",	LuaJX2_GetNpcTong },
		{ "SetTongMaster",	LuaJX2_SetTongMaster },
		{ "GetTongLogData",	LuaJX2_GetTongLogData },
		{ "TongClaimWar",	LuaJX2_TongClaimWar },
		{ "GetTongDuty",	LuaJX2_GetTongDuty },
		{ "Msg2PlayerByName",	LuaJX2_Msg2PlayerByName },
		{ "GetTongNameByID",	LuaTONG_GetName },
		{ "GetTongMTask",	LuaJX2_GetTongMTask },
		{ "SetTongMTask",	LuaJX2_SetTongMTask },
		{ "AddTongMTask",	LuaJX2_AddTongMTask },
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
		{ "TWS_MaintainAll",	LuaTWS_MaintainAll },	// timerserver goi moi ngay
		{ "TONG_DailyMaintainAll",	LuaTONG_DailyMaintainAll },
		// ==== DOT E cong thanh JX2: ObjBuffer + Ladder + GlbValue (KJx2SharedStore) ====
		{ "OB_Create",	LuaOB_Create },
		{ "OB_Release",	LuaOB_Release },
		{ "OB_IsEmpty",	LuaOB_IsEmpty },
		{ "OB_Clear",	LuaOB_Clear },
		{ "OB_Append",	LuaOB_Append },
		{ "OB_Copy",	LuaOB_Copy },
		{ "OB_PushByte",	LuaOB_PushByte },
		{ "OB_PopByte",	LuaOB_PopByte },
		{ "OB_PushInt",	LuaOB_PushInt },
		{ "OB_PopInt",	LuaOB_PopInt },
		{ "OB_PushDouble",	LuaOB_PushDouble },
		{ "OB_PopDouble",	LuaOB_PopDouble },
		{ "OB_PushString",	LuaOB_PushString },
		{ "OB_PopString",	LuaOB_PopString },
		{ "Ladder_NewLadder",	LuaLadder_NewLadder },
		{ "Ladder_ClearLadder",	LuaLadder_ClearLadder },
		{ "Ladder_GetLadderInfo",	LuaLadder_GetLadderInfo },
		{ "SetGlbValue",	LuaSetGlbValue },
		{ "GetGlbValue",	LuaGetGlbValue },
		// ==== DOT E cong thanh JX2: League + GLOBAL mission timer (KJx2League) ====
		{ "LG_CreateLeagueObj",	LuaLG_CreateLeagueObj },
		{ "LG_FreeLeagueObj",	LuaLG_FreeLeagueObj },
		{ "LG_SetLeagueInfo",	LuaLG_SetLeagueInfo },
		{ "LG_AddMemberToObj",	LuaLG_AddMemberToObj },
		{ "LG_ApplyAddLeague",	LuaLG_ApplyAddLeague },
		{ "LG_ApplyRemoveLeague",	LuaLG_ApplyRemoveLeague },
		{ "LG_GetLeagueObj",	LuaLG_GetLeagueObj },
		{ "LG_GetLeagueObjByRole",	LuaLG_GetLeagueObjByRole },
		{ "LG_GetFirstLeague",	LuaLG_GetFirstLeague },
		{ "LG_GetNextLeague",	LuaLG_GetNextLeague },
		{ "LG_GetLeagueInfo",	LuaLG_GetLeagueInfo },
		{ "LG_GetLeagueCreateTime",	LuaLG_GetLeagueCreateTime },
		{ "LG_GetMemberCount",	LuaLG_GetMemberCount },
		{ "LG_GetMemberInfo",	LuaLG_GetMemberInfo },
		{ "LG_GetMemberObj",	LuaLG_GetMemberObj },
		{ "LG_GetMemberJoinTime",	LuaLG_GetMemberJoinTime },
		{ "LG_GetLeagueTask",	LuaLG_GetLeagueTask },
		{ "LG_ApplySetLeagueTask",	LuaLG_ApplySetLeagueTask },
		{ "LG_ApplyAppendLeagueTask",	LuaLG_ApplyAppendLeagueTask },
		{ "LG_GetMemberTask",	LuaLG_GetMemberTask },
		{ "LG_ApplySetMemberTask",	LuaLG_ApplySetMemberTask },
		{ "LG_ApplyAppendMemberTask",	LuaLG_ApplyAppendMemberTask },
		{ "LGM_CreateMemberObj",	LuaLGM_CreateMemberObj },
		{ "LGM_SetMemberInfo",	LuaLGM_SetMemberInfo },
		{ "LGM_ApplyAddMember",	LuaLGM_ApplyAddMember },
		{ "LGM_ApplyRemoveMember",	LuaLGM_ApplyRemoveMember },
		{ "LGM_FreeMemberObj",	LuaLGM_FreeMemberObj },
		{ "LG_ApplyDoScript",	LuaLG_ApplyDoScript },
		{ "OpenGlbMission",	LuaOpenGlbMission },
		{ "StartGlbMSTimer",	LuaStartGlbMSTimer },
		{ "StopGlbMSTimer",	LuaStopGlbMSTimer },
		// ==== DOT E cong thanh JX2: nhom CITY 7 thanh (KJx2CityWar) ====
		{ "GetCityOwner",	LuaGetCityOwner },
		{ "GetCityWarBothSides",	LuaGetCityWarBothSides },
		{ "GetCityAreaName",	LuaGetCityAreaName },
		{ "GetCityArea",	LuaGetCityArea },
		{ "GetCitySummary",	LuaGetCitySummary },
		{ "GetAllCitySummary",	LuaGetAllCitySummary },
		{ "SyncCitySummary",	LuaSyncCitySummary },
		{ "OpenCityManageUI",	LuaOpenCityManageUI },
		{ "HaveBeginWar",	LuaHaveBeginWar },
		{ "NotifyWarResult",	LuaNotifyWarResult },
		{ "AppointViceroy",	LuaAppointViceroy },
		{ "AppointChallenger",	LuaAppointChallenger },
		{ "IsSigningUp",	LuaIsSigningUp },
		{ "NumOfSignUpTongs",	LuaNumOfSignUpTongs },
		{ "GetSignUpTongName",	LuaGetSignUpTongName },
		{ "DisabledChatCity",	LuaDisabledChatCity },
		{ "IsDisabledChatCity",	LuaIsDisabledChatCity },
		{ "CTC_JX2_SetCityState",	LuaCTC_JX2_SetCityState },
		// ==== DOT E cong thanh JX2 (E4): Arena idle + ArenaCredits ====
		{ "IsArenaBegin",	LuaIsArenaBegin },
		{ "GetArenaBothSides",	LuaGetArenaBothSides },
		{ "GetArenaCityArea",	LuaGetArenaCityArea },
		{ "GetArenaLevel",	LuaGetArenaLevel },
		{ "GetArenaTargetCity",	LuaGetArenaTargetCity },
		{ "GetArenaTotalLevel",	LuaGetArenaTotalLevel },
		{ "GetArenaTotalLevelByCity",	LuaGetArenaTotalLevelByCity },
		{ "GetArenaSchedule",	LuaGetArenaSchedule },
		{ "GetArenaInfoByCity",	LuaGetArenaInfoByCity },
		{ "NotifyArenaResult",	LuaNotifyArenaResult },
		// [LOI DAI CN 21/08]
		{ "StartSignUp",	LuaStartSignUp },
		{ "EndSignUp",	LuaEndSignUp },
		{ "StartArena",	LuaStartArena },
		{ "StartCityWar",	LuaStartCityWar },
		{ "SignUpCityWarArena",	LuaSignUpCityWarArena },
		{ "GetCityWarTongCamp",	LuaGetCityWarTongCamp },
		{ "GetArenaCredits",	LuaGetArenaCredits },
		{ "SetArenaCredits",	LuaSetArenaCredits },
		{ "AddArenaCredits",	LuaAddArenaCredits },
		{ "ReduceArenaCredits",	LuaReduceArenaCredits },
		// ==== DOT E cong thanh JX2 (E4): danh hieu doc lap ====
		{ "Title_AddTitle",	LuaTitle_AddTitle },
		{ "Title_ActiveTitle",	LuaTitle_ActiveTitle },
		{ "Title_RemoveTitle",	LuaTitle_RemoveTitle },
		{ "Title_GetTitleInfo",	LuaTitle_GetTitleInfo },
		{ "Title_GetTitleName",	LuaTitle_GetTitleName },
		{ "Title_GetActiveTitle",	LuaTitle_GetActiveTitle },
		{ "Title_GetTitleTab",	LuaTitle_GetTitleTab },
		// ==== DOT E cong thanh JX2 (E4): khung BT_ battle ====
		{ "BT_SetType2Task",	LuaBT_SetType2Task },
		{ "BT_GetData",	LuaBT_GetData },
		{ "BT_SetData",	LuaBT_SetData },
		{ "BT_SetTypeBonus",	LuaBT_SetTypeBonus },
		{ "BT_GetTypeBonus",	LuaBT_GetTypeBonus },
		{ "BT_SetView",	LuaBT_SetView },
		{ "BT_SetMissionName",	LuaBT_SetMissionName },
		{ "BT_SetGameData",	LuaBT_SetGameData },
		{ "BT_SetRestTime",	LuaBT_SetRestTime },
		{ "BT_SortLadder",	LuaBT_SortLadder },
		{ "BT_GetTopTenInfo",	LuaBT_GetTopTenInfo },
		{ "BT_UpdateMemberCount",	LuaBT_UpdateMemberCount },
		{ "BT_ClearBattle",	LuaBT_ClearBattle },
		{ "BT_BroadView",	LuaBT_BroadView },
		{ "BT_BroadGameData",	LuaBT_BroadGameData },
		{ "BT_BroadAllLadder",	LuaBT_BroadAllLadder },
		{ "BT_BroadSelf",	LuaBT_BroadSelf },
		// ==== DOT E cong thanh JX2 (E4): ha tang tran ====
		{ "SetNpcDeathScript",	LuaSetNpcDeathScript },
		{ "ClearMapNpc",	LuaClearMapNpc },
		{ "ClearMapObj",	LuaClearMapObj },	// [TONG 21/08] KJx2WarInfra.cpp
		{ "ClearMapNpcWithName",	LuaClearMapNpcWithName },
		{ "GetMapNpcWithName",	LuaGetMapNpcWithName },
		{ "GetAroundNpcList",	LuaGetAroundNpcList },	// [TIN SU 21/08] tbList, nCount quanh nguoi choi
		{ "AddObstacleObj",	LuaAddObstacleObj },
		{ "ClearObstacleObj",	LuaClearObstacleObj },
		{ "GetLoop",	LuaGetLoop },
		{ "GetNpcSettingIdx",	LuaGetNpcSettingIdx },
		{ "GetLastDiagNpc",	LuaGetLastDiagNpc },
		{ "SetPKFlag",	LuaSetPKFlag },
		{ "ForbidChangePK",	LuaForbidChangePK },
		{ "DisabledUseTownP",	LuaDisabledUseTownP },
		{ "RestoreOwnFeature",	LuaRestoreOwnFeature },
		{ "GetPlayerRev",	LuaGetPlayerRev },
		{ "SearchPlayer",	LuaSearchPlayer },
		{ "GetJoinTongTime",	LuaGetJoinTongTime },
		{ "GetTongMaster",	LuaGetTongMaster },
		{ "AddTongExp",	LuaAddTongExp },
		{ "GetItemCountEx",	LuaGetItemCountEx },
		{ "DelItemEx",	LuaDelItemEx },
		{ "GetItemLife",	LuaGetItemLife },
		{ "GetItemProp",	LuaGetItemProp },
		{ "GiveItemUI",	LuaGiveItemUI },
		{ "GetGiveItemUnit",	LuaGetGiveItemUnit },
		{ "RemoveItemByIndex",	LuaRemoveItemByIndex },
		{ "PushString",	LuaPushString },
		{ "AppendString",	LuaAppendString },
		{ "ReplaceString",	LuaReplaceString },
		{ "PopString",	LuaPopString },
		{ "NW_GetSealInfo",	LuaNW_GetSealInfo },
		{ "PARTNER_GetCurPartner",	LuaPARTNER_GetCurPartner },
		{ "PARTNER_GetSettingIdx",	LuaPARTNER_GetSettingIdx },
		{ "SetSiegeVoitureParam",	LuaSetSiegeVoitureParam },
		{ "SetMangonelParam",	LuaSetMangonelParam },
		// SetMissionV = LuaSetMission (m_MissionArray). SetMissionS/GetMissionS
		// di KHO CHUOI RIENG (KJx2WarInfra) - phan bien E4 CHAN-5: JX1 dung
		// chung mot mang cho so + chuoi nen SetMissionV(1,..) de mat ten bang
		// thu cua mission.lua:87. GetMissionS dang ky lan 2 o day THANG ban cu
		// (muc sau trong bang de len - tien le "GetLeadLevel" THAP-7).
		{ "SetMissionV",	LuaSetMission },
		{ "SetMissionS",	LuaJx2SetMissionString },
		{ "GetMissionS",	LuaJx2GetMissionString },
		{ "BT_ClearPlayerData",	LuaBT_ClearPlayerData },
		{ "BT_LeaveBattle",	LuaBT_LeaveBattle },
		// RevID2WXY(world, revid) -> (world, x, y) = dung ham "Rev2Pos" san co
		{ "RevID2WXY",	LuaGetPlayerRevivalPos },
		// ==== DOT E (E7): thue qua thoai NPC quan thanh (khong protocol moi) ====
		{ "CTC_JX2_SetTax",	LuaCTC_JX2_SetTax },
		{ "CTC_JX2_GetTax",	LuaCTC_JX2_GetTax },
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
		{"IniFile_Load",		LuaIniFile_Load},	// [TONG 21/08]
		{"IniFile_GetData",	LuaIniFile_GetData},
		{"IniFile_UnLoad",		LuaIniFile_UnLoad},
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
	
		{ "KV_Set", LuaKV_Set },
		{ "KV_Get", LuaKV_Get },
		{ "KV_Del", LuaKV_Del },
		{ "GhiNhatKy", LuaGhiNhatKy },
		{ "LayLenhQuanTri", LuaLayLenhQuanTri },
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
