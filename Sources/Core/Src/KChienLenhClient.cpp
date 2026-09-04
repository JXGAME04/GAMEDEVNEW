// KChienLenhClient.cpp - [CL 04/09 DOT2] cau noi Lua <-> cua so CHIEN LENH phia client.
// Khuon y het KAuctionClient.cpp: \script\ui\uichienlenh.lua nhan goi tu may chu roi goi cac ham C++ nay;
// ta doi thanh thong bao GDCNI_CHIENLENH_UI sang S3Client (UiChienLenh.cpp). Chieu nguoc: CoreShell.cpp
// case GOI_CHIENLENH_UI -> ChienLenhUi_OnRequest -> chay "UIChienLenh:xxx(...)" trong state cua uichienlenh.lua.
// Hop dong: KChienLenhUiDef.h.
//
// Vat pham thuong truyen bang chuoi 6 so (genre,detail,particular,level,series,luck) -> ChatItem qua
// Auc_FillChatItemInfo (dung chung voi hop thu / dau gia) de client dung lai bieu tuong + chu giai THAT.
#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KChienLenhUiDef.h"
#include "KScriptProtocol.h"
#include "KChienLenhClient.h"

#ifndef _SERVER
#include "CoreShell.h"
#include "GameDataDef.h"
#include "KAuctionClient.h"		// Auc_FillChatItemInfo
#include <stdio.h>
#include <string.h>

extern int CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam);

#define UICHIENLENH_SCRIPT	"\\script\\ui\\uichienlenh.lua"

static int s_bOpen = 0;

static void sCopyStr(char* pDst, int nSize, const char* pSrc)
{
	if (!pDst || nSize <= 0)
		return;
	if (!pSrc)
	{
		pDst[0] = 0;
		return;
	}
	strncpy(pDst, pSrc, nSize - 1);
	pDst[nSize - 1] = 0;
}

static const char* sArgStr(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsString(L, n)) ? Lua_ValueToString(L, n) : "";
}

static int sArgInt(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (int)Lua_ValueToNumber(L, n) : 0;
}

static void sNotify(int nCmd, int nParam)
{
	CoreDataChanged(GDCNI_CHIENLENH_UI, (unsigned int)nCmd, nParam);
}

// ---------------------------------------------------------------- Lua -> UI
int LuaCLUi_SetIconVisible(Lua_State* L)
{
	sNotify(CLUI_CMD_ICON_VISIBLE, sArgInt(L, 1) ? 1 : 0);
	return 0;
}

int LuaCLUi_Open(Lua_State* L)
{
	s_bOpen = 1;
	sNotify(CLUI_CMD_OPEN, 0);
	return 0;
}

int LuaCLUi_Close(Lua_State* L)
{
	s_bOpen = 0;
	sNotify(CLUI_CMD_CLOSE, 0);
	return 0;
}

int LuaCLUi_IsOpen(Lua_State* L)
{
	Lua_PushNumber(L, s_bOpen);
	return 1;
}

int LuaCLUi_Clear(Lua_State* L)
{
	sNotify(CLUI_CMD_CLEAR, 0);
	return 0;
}

int LuaCLUi_SetInfo(Lua_State* L)
{
	KCLUiInfo inf;
	memset(&inf, 0, sizeof(inf));
	inf.nScore		= sArgInt(L, 1);
	inf.nCap		= sArgInt(L, 2);
	inf.nVip		= sArgInt(L, 3);
	inf.nGotLow		= sArgInt(L, 4);
	inf.nGotVip		= sArgInt(L, 5);
	inf.nSoMoc		= sArgInt(L, 6);
	inf.nLevelScore	= sArgInt(L, 7);
	inf.nCloseTime	= sArgInt(L, 8);
	inf.nDangMo		= sArgInt(L, 9);
	inf.nVipPriceXu	= sArgInt(L, 10);
	sNotify(CLUI_CMD_SET_INFO, (int)&inf);
	return 0;
}

// CLUi_SetAward(idx, need, cap, branch, count, szName, szItemInfo)
int LuaCLUi_SetAward(Lua_State* L)
{
	KCLUiAward a;
	memset(&a, 0, sizeof(a));
	a.nIdx			= sArgInt(L, 1);
	a.nNeedScore	= sArgInt(L, 2);
	a.nCap			= sArgInt(L, 3);
	a.nBranch		= sArgInt(L, 4);
	a.nCount		= sArgInt(L, 5);
	sCopyStr(a.szName, sizeof(a.szName), sArgStr(L, 6));
	const char* szInfo = sArgStr(L, 7);
	if (szInfo && szInfo[0])
	{
		Auc_FillChatItemInfo(&a.Item, szInfo);
		a.Item.m_nID = 1;	// co bieu tuong
		a.Item.m_bStack = (a.nCount > 0 && a.nCount <= 255) ? (unsigned char)a.nCount : 1;
	}
	sNotify(CLUI_CMD_SET_AWARD, (int)&a);
	return 0;
}

// CLUi_SetMission(id, kind, score, target, prog, state, szTitle, szTips)
int LuaCLUi_SetMission(Lua_State* L)
{
	KCLUiMission m;
	memset(&m, 0, sizeof(m));
	m.nId		= sArgInt(L, 1);
	m.nKind		= sArgInt(L, 2);
	m.nScore	= sArgInt(L, 3);
	m.nTarget	= sArgInt(L, 4);
	m.nProg		= sArgInt(L, 5);
	m.nState	= sArgInt(L, 6);
	sCopyStr(m.szTitle, sizeof(m.szTitle), sArgStr(L, 7));
	sCopyStr(m.szTips, sizeof(m.szTips), sArgStr(L, 8));
	sNotify(CLUI_CMD_SET_MISSION, (int)&m);
	return 0;
}

int LuaCLUi_Refresh(Lua_State* L)
{
	sNotify(CLUI_CMD_REFRESH, 0);
	return 0;
}

int LuaCLUi_Msg(Lua_State* L)
{
	const char* sz = sArgStr(L, 1);
	sNotify(CLUI_CMD_MSG, (int)sz);
	return 0;
}

// ---------------------------------------------------------------- UI -> Lua
void ChienLenhUi_OnRequest(unsigned int uParam, int nParam)
{
	int nOp = (int)uParam;
	const KCLUiReq* pReq = (const KCLUiReq*)nParam;
	char szCall[256];
	szCall[0] = 0;
	switch (nOp)
	{
	case CLUI_OP_OPEN:
		strcpy(szCall, "UIChienLenh:RequestOpen()");
		break;
	case CLUI_OP_CLOSE:
		s_bOpen = 0;
		strcpy(szCall, "UIChienLenh:OnWindowClosed()");
		break;
	case CLUI_OP_GET_AWARD:
		if (pReq)
			sprintf(szCall, "UIChienLenh:RequestGetAward(%d, %d)", pReq->nA, pReq->nB);
		break;
	case CLUI_OP_GET_MISSION:
		sprintf(szCall, "UIChienLenh:RequestGetMission(%d)", nParam);
		break;
	case CLUI_OP_GOTO_MISSION:
		sprintf(szCall, "UIChienLenh:OnGotoMission(%d)", nParam);
		break;
	case CLUI_OP_BUY_VIP:
		strcpy(szCall, "UIChienLenh:OnBuyVip()");
		break;
	case CLUI_OP_HELP:
		strcpy(szCall, "UIChienLenh:OnHelp()");
		break;
	case CLUI_OP_RESET:
		s_bOpen = 0;
		strcpy(szCall, "UIChienLenh:Reset()");
		break;
	default:
		break;
	}
	if (szCall[0])
		SP_RunClientLua(UICHIENLENH_SCRIPT, szCall);
}

#endif // !_SERVER
