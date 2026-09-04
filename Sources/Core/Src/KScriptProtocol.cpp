// KScriptProtocol.cpp - [MAIL 03/09] kenh ScriptProtocol (ObjBuffer) hai chieu. Xem KScriptProtocol.h.
// Bien dich cho CA client (Win32) lan may chu (x64): phan khac nhau nam trong #ifdef _SERVER.
// LUU Y: Core build voi PCH qua KCore.h - KCore.h phai dung dau tien.
#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include "KSortScript.h"
#include "KProtocol.h"
#include "KPlayerDef.h"
#include "KJx2SharedStore.h"
#include "KScriptProtocol.h"
#ifdef _SERVER
#include "KNpc.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#else
#include "../../Headers/IClient.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#ifndef _SERVER
#include <map>
#include <string>
#include "LuaFuns.h"	// GameScriptFuns / g_GetGameScriptFunNum
#endif

#define SP_SCRIPT_GS	"\\script\\script_protocol\\protocol_def_gs.lua"
#define SP_SCRIPT_C		"\\script\\script_protocol\\protocol_def_c.lua"

//////////////////////////////////////////////////////////////////////
// Dong goi / boc goi
//////////////////////////////////////////////////////////////////////
int SP_BuildPacket(BYTE btType, int nProtocolId, int hOB, BYTE* pOut, int nOutSize)
{
	const unsigned char* pData = NULL;
	int nLen = KJx2OB_GetBytes(hOB, &pData);
	if (nLen < 0 || !pData || !pOut)
		return 0;
	if (nProtocolId <= 0 || nProtocolId > 0xFFFF)
		return 0;
	int nTotal = (int)sizeof(SCRIPT_DATA_HEAD) + nLen;
	if (nLen > SCRIPT_DATA_MAXLEN || nTotal > nOutSize)
		return 0;
	SCRIPT_DATA_HEAD* pHead = (SCRIPT_DATA_HEAD*)pOut;
	pHead->ProtocolType = btType;
	pHead->wLength = (WORD)(nTotal - 1);
	pHead->wProtocolId = (WORD)nProtocolId;
	pHead->wDataLen = (WORD)nLen;
	if (nLen > 0)
		memcpy(pOut + sizeof(SCRIPT_DATA_HEAD), pData, nLen);
	return nTotal;
}

// Tra so byte du lieu (>= 0), -1 neu goi hong. *ppData tro vao trong pMsg.
static int SP_ParsePacket(BYTE* pMsg, BYTE btType, int* pnProtocolId, const BYTE** ppData)
{
	if (!pMsg || !pnProtocolId || !ppData)
		return -1;
	SCRIPT_DATA_HEAD* pHead = (SCRIPT_DATA_HEAD*)pMsg;
	if (pHead->ProtocolType != btType)
		return -1;
	if ((int)pHead->wLength < (int)sizeof(SCRIPT_DATA_HEAD) - 1)
		return -1;
	int nLen = (int)pHead->wDataLen;
	if (nLen > SCRIPT_DATA_MAXLEN)
		return -1;
	if ((int)pHead->wLength != (int)sizeof(SCRIPT_DATA_HEAD) - 1 + nLen)
		return -1;
	if (pHead->wProtocolId == 0)
		return -1;
	*pnProtocolId = (int)pHead->wProtocolId;
	*ppData = pMsg + sizeof(SCRIPT_DATA_HEAD);
	return nLen;
}

//////////////////////////////////////////////////////////////////////
// Tim / nap script va goi bo dieu phoi Lua
//////////////////////////////////////////////////////////////////////
// g_FileName2Id KHONG doi chu thuong, con LoadScriptToSortList luu ten CHU THUONG
// (KSortScript.cpp) -> luon ha chu truoc khi tra.
#ifndef _SERVER
// [MAIL 03/09 D5] CLIENT: g_ScriptSet chi co MAX_SCRIPT_IN_SET = 5 o (KSortScript.h) -> LoadAllScript("\\script") nap 5 tep
// lib\ dau tien roi day, ReLoadScript cung that bai => bo dieu phoi + uimail.lua "khong nap duoc" (jx_mail.log 17:39).
// Client giu bang KLuaScript rieng, nap theo yeu cau, dang ky GameScriptFuns y nhu KSortScript.cpp:LoadScriptToSortListA.
static std::map<std::string, KLuaScript*> s_SpClientScripts;

static KLuaScript* sClientLoad(const char* szLow)
{
	std::map<std::string, KLuaScript*>::iterator it = s_SpClientScripts.find(szLow);
	if (it != s_SpClientScripts.end())
		return it->second;
	KLuaScript* p = new KLuaScript;
	if (!p)
		return NULL;
	p->Init();
	p->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
	Lua_PushNumber(p->m_LuaState, 1);
	p->SetGlobalName((LPSTR)"MODEL_GAMECLIENT");
	g_StrCpyLen(p->m_szScriptName, (char*)szLow, 100);
	int nOk = p->Load((char*)szLow);
	s_SpClientScripts[szLow] = p;	// giu ca khi than chunk loi (nhu g_ScriptSet), khoi nap lai vo han
	SP_ClientLog("[SP] nap %s vao bang rieng: %s", szLow, nOk ? "ok" : "LOI than chunk (xem ScriptError.log)");
	return p;
}
#endif

static KLuaScript* SP_GetScript(const char* szScript, int bLoad)
{
	if (!szScript || !szScript[0])
		return NULL;
	char szLow[MAX_PATH];
	g_StrCpyLen(szLow, (char*)szScript, MAX_PATH);
	g_StrLower(szLow);
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
#ifdef _SERVER
	if (!pScript && bLoad)
	{
		if (ReLoadScript(szLow))
			pScript = (KLuaScript*)g_GetScript(szLow);
	}
#else
	if (!pScript)
	{
		std::map<std::string, KLuaScript*>::iterator it = s_SpClientScripts.find(szLow);
		if (it != s_SpClientScripts.end())
			pScript = it->second;
		else if (bLoad)
			pScript = sClientLoad(szLow);
	}
#endif
	return pScript;
}

KLuaScript* SP_FindScript(const char* szScript, int bLoad)
{
	return SP_GetScript(szScript, bLoad);
}

static void SP_Dispatch(const char* szScript, int nPlayerIdx, int nProtocolId, int hOB)
{
	KLuaScript* pScript = SP_GetScript(szScript, 1);
	if (!pScript)
	{
		g_DebugLog((LPSTR)"[SP] khong nap duoc bo dieu phoi %.128s (id=%d)", szScript, nProtocolId);
#ifndef _SERVER
		SP_ClientLog("[SP] khong nap duoc bo dieu phoi %.128s (id=%d)", szScript, nProtocolId);
#endif
		return;
	}
	Lua_State* L = pScript->m_LuaState;
	// protocol.lua:ProtocolProcess re nhanh theo MODEL_GAMESERVER / MODEL_GAMECLIENT; may chu con can
	// PlayerIndex cho DynamicExecuteByPlayer (khuon LuaDynamicExecuteByPlayer, ScriptFuns.cpp).
#ifdef _SERVER
	Lua_PushNumber(L, 1);
	pScript->SetGlobalName((LPSTR)"MODEL_GAMESERVER");
	Lua_PushNumber(L, nPlayerIdx);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
#else
	Lua_PushNumber(L, 1);
	pScript->SetGlobalName((LPSTR)"MODEL_GAMECLIENT");
	// [MAIL 03/09 D2] client: PlayerIndex = 1 de Talk/Say trong handler chay duoc
	Lua_PushNumber(L, CLIENT_PLAYER_INDEX);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
#endif
	char szCall[128];
	sprintf(szCall, "ScriptProtocol:ProtocolProcess(%d, %d)", nProtocolId, hOB);
	int nTop = 0;
	pScript->SafeCallBegin(&nTop);
	int nErr = lua_dostring(L, szCall);
	if (nErr != 0)
		g_DebugLog((LPSTR)"[SP] loi chay %.128s: %s", szScript, szCall);
#ifndef _SERVER
	SP_ClientLog("[SP] dispatch %s trong %.128s (loi=%d)", szCall, szScript, nErr);
#endif
	pScript->SafeCallEnd(nTop);
}

int LuaEnsureScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KLuaScript* pScript = SP_GetScript(Lua_ValueToString(L, 1), 1);
	Lua_PushNumber(L, pScript ? 1 : 0);
	return 1;
}

#ifdef _SERVER
//////////////////////////////////////////////////////////////////////
// MAY CHU
//////////////////////////////////////////////////////////////////////
void SP_OnServerRecv(int nIndex, BYTE* pMsg)
{
	if (!pMsg)
		return;
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)
		return;
	if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)
		return;
	int nId = 0;
	const BYTE* pData = NULL;
	int nLen = SP_ParsePacket(pMsg, (BYTE)c2s_scriptdata, &nId, &pData);
	if (nLen < 0)
	{
		g_DebugLog((LPSTR)"[SP] goi c2s_scriptdata hong tu nguoi choi %d", nIndex);
		return;
	}
	int h = KJx2OB_CreateFromBytes(pData, nLen);
	if (h <= 0)
		return;
	SP_Dispatch(SP_SCRIPT_GS, nIndex, nId, h);
	KJx2OB_Release(h);
}

int SP_SendToPlayer(int nPlayerIdx, int nProtocolId, int hOB)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;
	if (Player[nPlayerIdx].m_nNetConnectIdx < 0)	// bot / chua co ket noi
		return 0;
	if (!g_pServer)
		return 0;
	BYTE szBuf[SCRIPT_DATA_MAXLEN + sizeof(SCRIPT_DATA_HEAD) + 8];
	int n = SP_BuildPacket((BYTE)s2c_scriptdata, nProtocolId, hOB, szBuf, sizeof(szBuf));
	if (n <= 0)
		return 0;
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, szBuf, n);
	return 1;
}

// PlayerIndex cua state dang goi (khuon GetPlayerIndex trong ScriptFuns.cpp)
static int SP_PlayerIdxOf(Lua_State* L)
{
	lua_getglobal(L, SCRIPT_PLAYERINDEX);
	int n = 0;
	if (lua_isnumber(L, -1))
		n = (int)lua_tonumber(L, -1);
	lua_settop(L, -2);
	return n;
}

// SendScriptData(nProtocolId, hOB) -> 1/0
int LuaSendScriptData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nIdx = SP_PlayerIdxOf(L);
	int nOk = SP_SendToPlayer(nIdx, (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2));
	Lua_PushNumber(L, nOk);
	return 1;
}

// SendScriptDataToPlayer(nPlayerIdx, nProtocolId, hOB) -> 1/0
int LuaSendScriptDataToPlayer(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nOk = SP_SendToPlayer((int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L, 2),
		(int)Lua_ValueToNumber(L, 3));
	Lua_PushNumber(L, nOk);
	return 1;
}

#else
//////////////////////////////////////////////////////////////////////
// CLIENT
//////////////////////////////////////////////////////////////////////
// [MAIL 03/09 D4] nhat ky chan doan phia client: jx_mail.log canh Game.exe (g_DebugLog chi bay ra cua so debug,
// khong ra tep nen 03/09 khong biet chuoi 'Nhan thu' dut o dau).
void SP_ClientLog(const char* szFmt, ...)
{
	if (!szFmt)
		return;
	FILE* f = fopen("jx_mail.log", "a");
	if (!f)
		return;
	time_t t = time(NULL);
	struct tm* p = localtime(&t);
	if (p)
		fprintf(f, "%02d:%02d:%02d ", p->tm_hour, p->tm_min, p->tm_sec);
	va_list va;
	va_start(va, szFmt);
	vfprintf(f, szFmt, va);
	va_end(va);
	fprintf(f, "\n");
	fclose(f);
}

void SP_OnClientRecv(BYTE* pMsg)
{
	int nId = 0;
	const BYTE* pData = NULL;
	int nLen = SP_ParsePacket(pMsg, (BYTE)s2c_scriptdata, &nId, &pData);
	if (nLen < 0)
	{
		g_DebugLog((LPSTR)"[SP] goi s2c_scriptdata hong");
		SP_ClientLog("[SP] goi s2c_scriptdata hong (type=%d)", pMsg ? (int)pMsg[0] : -1);
		return;
	}
	int h = KJx2OB_CreateFromBytes(pData, nLen);
	SP_ClientLog("[SP] nhan s2c_scriptdata id=%d len=%d ob=%d", nId, nLen, h);
	if (h <= 0)
		return;
	SP_Dispatch(SP_SCRIPT_C, 0, nId, h);
	KJx2OB_Release(h);
}

// [MAIL 03/09 D2] chay mot cau Lua (vi du "UIMail:OnSelect(5)") trong state cua szScript; nap neu chua.
// Dat MODEL_GAMECLIENT + PlayerIndex nhu SP_Dispatch. Tra 1 neu chay duoc.
int SP_RunClientLua(const char* szScript, const char* szCall)
{
	if (!szCall || !szCall[0])
		return 0;
	KLuaScript* pScript = SP_GetScript(szScript, 1);
	if (!pScript)
	{
		g_DebugLog((LPSTR)"[SP] RunClientLua: khong nap duoc %.128s", szScript);
		SP_ClientLog("[SP] RunClientLua: khong nap duoc %.128s (%.200s)", szScript, szCall);
		return 0;
	}
	Lua_State* L = pScript->m_LuaState;
	Lua_PushNumber(L, 1);
	pScript->SetGlobalName((LPSTR)"MODEL_GAMECLIENT");
	Lua_PushNumber(L, CLIENT_PLAYER_INDEX);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);
	int nTop = 0;
	pScript->SafeCallBegin(&nTop);
	int nRet = 1;
	if (lua_dostring(L, szCall) != 0)
	{
		g_DebugLog((LPSTR)"[SP] RunClientLua loi %.128s: %.200s", szScript, szCall);
		SP_ClientLog("[SP] RunClientLua loi %.128s: %.200s", szScript, szCall);
		nRet = 0;
	}
	pScript->SafeCallEnd(nTop);
	return nRet;
}

// SendScriptDataToServer(nProtocolId, hOB) -> 1/0
int LuaSendScriptDataToServer(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (!g_pClient)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	BYTE szBuf[SCRIPT_DATA_MAXLEN + sizeof(SCRIPT_DATA_HEAD) + 8];
	int n = SP_BuildPacket((BYTE)c2s_scriptdata, (int)Lua_ValueToNumber(L, 1),
		(int)Lua_ValueToNumber(L, 2), szBuf, sizeof(szBuf));
	if (n <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	g_pClient->SendPackToServer(szBuf, n);
	SP_ClientLog("[SP] gui c2s_scriptdata id=%d len=%d", (int)Lua_ValueToNumber(L, 1), n);
	Lua_PushNumber(L, 1);
	return 1;
}
#endif
