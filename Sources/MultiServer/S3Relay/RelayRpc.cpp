//////////////////////////////////////////////////////////////////////////////
// RelayRpc.cpp  (S3Relay)  [RELAYHT 06/09]  -- xem RelayRpc.h
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "DoScript.h"
#include "RelayScript.h"
#include "RelayRpc.h"
#include <list>
#include <map>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
// Trang thai
//////////////////////////////////////////////////////////////////////////////
struct RPC_INPKG
{
	std::string		strData;		// than goi (khong ke byte s2s_script)
	unsigned long	ulConnId;		// ket noi GameServer gui toi
};

struct RPC_PENDING					// lenh relay GUI DI, dang cho ket qua
{
	std::string		strScript;		// kich ban DA GOI (de tim state chay callback)
	std::string		strCbFunc;
	int				nCbParam;
};

static CRITICAL_SECTION			s_csQueue;
static BOOL						s_bCsReady = FALSE;
static std::list<RPC_INPKG>		s_lstIn;
static std::map<DWORD, RPC_PENDING>	s_mapPending;
static DWORD					s_dwNextCallId = 0;
static BOOL						s_bReady = FALSE;

static void RpcLog(const char* fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = 0;
	rTRACE("[RelayRpc] %s", buf);
}

//////////////////////////////////////////////////////////////////////////////
// Dong goi va gui
//////////////////////////////////////////////////////////////////////////////
// Dung mot goi hoan chinh: RELAY_DATA + s2s_script + RELAY_SCRIPT_CALL + 3 khoi.
// Tra ve so byte da ghi vao pOut, 0 = khong du cho.
static size_t RpcBuild(void* pOut, size_t nCap,
					   DWORD dwCallId, DWORD dwGameSvrId, BYTE byIsResult,
					   const char* szScript, const char* szFunc,
					   const BYTE* pData, int nDataLen)
{
	if (!szScript) szScript = "";
	if (!szFunc)   szFunc = "";
	if (nDataLen < 0) nDataLen = 0;

	size_t nScriptLen = strlen(szScript) + 1;
	size_t nFuncLen = strlen(szFunc) + 1;
	size_t nBody = 1 + sizeof(RELAY_SCRIPT_CALL) + nScriptLen + nFuncLen + nDataLen;
	size_t nTotal = sizeof(RELAY_DATA) + nBody;
	if (nTotal > nCap)
		return 0;

	RELAY_DATA* pRD = (RELAY_DATA*)pOut;
	pRD->ProtocolFamily = pf_relay;
	pRD->ProtocolID = relay_c2c_data;
	pRD->nToIP = 0;						// 0 = da toi noi (ben kia tu xu ly than goi)
	pRD->nToRelayID = 0;
	pRD->nFromIP = 0;
	pRD->nFromRelayID = 0;
	pRD->routeDateLength = (WORD)nBody;

	BYTE* p = (BYTE*)pOut + sizeof(RELAY_DATA);
	*p++ = (BYTE)s2s_script;

	RELAY_SCRIPT_CALL* pSC = (RELAY_SCRIPT_CALL*)p;
	pSC->ProtocolFamily = pf_relay;
	pSC->ProtocolID = relay_s2c_script;	// [RELAYHT 06/09] de GameServer dinh tuyen duoc
	pSC->dwCallId = dwCallId;
	pSC->dwGameSvrId = dwGameSvrId;
	pSC->byIsResult = byIsResult;
	pSC->wScriptLen = (WORD)nScriptLen;
	pSC->wFuncLen = (WORD)nFuncLen;
	pSC->wDataLen = (WORD)nDataLen;
	p += sizeof(RELAY_SCRIPT_CALL);

	memcpy(p, szScript, nScriptLen);	p += nScriptLen;
	memcpy(p, szFunc, nFuncLen);		p += nFuncLen;
	if (nDataLen > 0)
	{
		memcpy(p, pData, nDataLen);
		p += nDataLen;
	}
	return nTotal;
}

// Gui toi mot ket noi GameServer (ulConnId) hoac toi TAT CA (ulConnId == 0).
static BOOL RpcSend(unsigned long ulConnId, const void* pData, size_t nSize)
{
	if (!pData || nSize == 0)
		return FALSE;
	if (ulConnId == 0)
	{
		g_HostServer.BroadPackage(pData, nSize);
		return TRUE;
	}
	CNetConnectDup conndup = g_HostServer.FindNetConnect(ulConnId);
	if (!conndup.IsValid())
		return FALSE;
	conndup.SendPackage(pData, nSize);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Nhan tu luong mang -- CHI XEP HANG
//////////////////////////////////////////////////////////////////////////////
void RelayRpc_OnPacket(const void* pData, size_t nSize, unsigned long ulConnId)
{
	if (!s_bReady || !s_bCsReady || !pData || nSize < sizeof(RELAY_SCRIPT_CALL))
		return;

	::EnterCriticalSection(&s_csQueue);
	if ((int)s_lstIn.size() < RPC_MAX_QUEUE)
	{
		RPC_INPKG pkg;
		pkg.strData.assign((const char*)pData, nSize);
		pkg.ulConnId = ulConnId;
		s_lstIn.push_back(pkg);
	}
	::LeaveCriticalSection(&s_csQueue);
}

int RelayRpc_QueueSize()
{
	if (!s_bCsReady)
		return 0;
	::EnterCriticalSection(&s_csQueue);
	int n = (int)s_lstIn.size();
	::LeaveCriticalSection(&s_csQueue);
	return n;
}

//////////////////////////////////////////////////////////////////////////////
// Chay mot goi (LUONG CHINH)
//////////////////////////////////////////////////////////////////////////////
static void RpcRunOne(const RPC_INPKG& pkg)
{
	const BYTE* p = (const BYTE*)pkg.strData.data();
	size_t nSize = pkg.strData.size();
	if (nSize < sizeof(RELAY_SCRIPT_CALL))
		return;

	RELAY_SCRIPT_CALL sc;
	memcpy(&sc, p, sizeof(sc));
	const BYTE* q = p + sizeof(RELAY_SCRIPT_CALL);
	size_t nLeft = nSize - sizeof(RELAY_SCRIPT_CALL);
	if ((size_t)sc.wScriptLen + sc.wFuncLen + sc.wDataLen > nLeft)
	{
		RpcLog("goi hong: do dai khong khop (%u+%u+%u > %u)",
			sc.wScriptLen, sc.wFuncLen, sc.wDataLen, (unsigned)nLeft);
		return;
	}
	const char* szScript = (const char*)q;					q += sc.wScriptLen;
	const char* szFunc = (const char*)q;					q += sc.wFuncLen;
	const BYTE* pData = q;

	// bao dam ket thuc bang 0 (goi tu ben ngoai, khong tin)
	if (sc.wScriptLen == 0 || szScript[sc.wScriptLen - 1] != 0) return;
	if (sc.wFuncLen == 0 || szFunc[sc.wFuncLen - 1] != 0) return;

	if (sc.byIsResult)
	{
		// ---- KET QUA tra ve cho lenh relay da gui di ----
		std::map<DWORD, RPC_PENDING>::iterator it = s_mapPending.find(sc.dwCallId);
		if (it == s_mapPending.end())
			return;
		RPC_PENDING pend = it->second;
		s_mapPending.erase(it);
		if (pend.strCbFunc.empty())
			return;

		KLuaScript* pScript = RelayScript_Get(pend.strScript.c_str(), TRUE);
		if (!pScript)
			return;
		int hRes = RelayOB_Alloc();
		RelayOB_Assign(hRes, pData, sc.wDataLen);
		char szCall[600];
		_snprintf(szCall, sizeof(szCall) - 1, "%s(%d,%d)", pend.strCbFunc.c_str(), pend.nCbParam, hRes);
		szCall[sizeof(szCall) - 1] = 0;
		int nTop = 0;
		pScript->SafeCallBegin(&nTop);
		if (lua_dostring(pScript->m_LuaState, szCall) != 0)
			RpcLog("callback LOI: %.200s", szCall);
		pScript->SafeCallEnd(nTop);
		RelayOB_Free(hRes);
		return;
	}

	// ---- LENH GOI TU GameServer ----
	KLuaScript* pScript = RelayScript_Get(szScript, TRUE);
	if (!pScript)
	{
		RpcLog("khong nap duoc kich ban [%.128s]", szScript);
		return;
	}

	int hParam = RelayOB_Alloc();
	int hRes = RelayOB_Alloc();
	RelayOB_Assign(hParam, pData, sc.wDataLen);

	char szCall[600];
	_snprintf(szCall, sizeof(szCall) - 1, "%s(%d,%d,%u)", szFunc, hParam, hRes, sc.dwGameSvrId);
	szCall[sizeof(szCall) - 1] = 0;
	int nTop = 0;
	pScript->SafeCallBegin(&nTop);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		RpcLog("chay LOI: %.128s -> %.200s", szScript, szCall);
	pScript->SafeCallEnd(nTop);

	// tra ket qua ve neu ben goi doi
	if (sc.dwCallId != 0)
	{
		const BYTE* pRes = NULL;
		int nResLen = 0;
		RelayOB_Peek(hRes, &pRes, &nResLen);
		char szBuf[8192];
		size_t n = RpcBuild(szBuf, sizeof(szBuf), sc.dwCallId, sc.dwGameSvrId, 1,
							szScript, "", pRes, nResLen);
		if (n > 0)
			RpcSend(pkg.ulConnId, szBuf, n);
		else
			RpcLog("ket qua qua lon, bo (%d byte)", nResLen);
	}

	RelayOB_Free(hParam);
	RelayOB_Free(hRes);
}

void RelayRpc_Tick()
{
	if (!s_bReady || !s_bCsReady)
		return;

	for (;;)
	{
		RPC_INPKG pkg;
		::EnterCriticalSection(&s_csQueue);
		if (s_lstIn.empty())
		{
			::LeaveCriticalSection(&s_csQueue);
			break;
		}
		pkg = s_lstIn.front();
		s_lstIn.pop_front();
		::LeaveCriticalSection(&s_csQueue);

		RpcRunOne(pkg);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Ham Lua cua relay
//////////////////////////////////////////////////////////////////////////////
// RemoteExecute(szScript, szFunc, hParam [, szCallBackFunc, nCallBackParam, dwGameSvrId])
// Gui lenh sang GameServer. dwGameSvrId = 0 (hoac thieu) = gui TAT CA may chu.
static int LuaRpc_RemoteExecute(Lua_State* L)
{
	int nParam = Lua_GetTopIndex(L);
	if (nParam < 3 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* szScript = Lua_ValueToString(L, 1);
	const char* szFunc = Lua_ValueToString(L, 2);
	int hParam = (int)Lua_ValueToNumber(L, 3);
	const char* szCb = (nParam >= 4 && Lua_IsString(L, 4)) ? Lua_ValueToString(L, 4) : NULL;
	int nCbParam = (nParam >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : 0;
	DWORD dwGameSvrId = (nParam >= 6 && Lua_IsNumber(L, 6)) ? (DWORD)(int)Lua_ValueToNumber(L, 6) : 0;

	const BYTE* pData = NULL;
	int nLen = 0;
	RelayOB_Peek(hParam, &pData, &nLen);		// handle sai -> goi rong, van gui

	DWORD dwCallId = 0;
	if (szCb && szCb[0])
	{
		if ((int)s_mapPending.size() >= RPC_MAX_PENDING)
		{
			RpcLog("TRAN so lenh cho ket qua (%d) -- bo lenh moi", RPC_MAX_PENDING);
			Lua_PushNumber(L, 0);
			return 1;
		}
		dwCallId = ++s_dwNextCallId;
		if (dwCallId == 0)
			dwCallId = ++s_dwNextCallId;
		RPC_PENDING pend;
		// callback chay trong state cua CHINH kich ban dang goi
		pend.strScript = szScript;
		pend.strCbFunc = szCb;
		pend.nCbParam = nCbParam;
		s_mapPending[dwCallId] = pend;
	}

	char szBuf[8192];
	size_t n = RpcBuild(szBuf, sizeof(szBuf), dwCallId, dwGameSvrId, 0,
						szScript, szFunc, pData, nLen);
	if (n == 0)
	{
		RpcLog("goi qua lon, bo: %.128s", szScript);
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, RpcSend(dwGameSvrId, szBuf, n) ? 1 : 0);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Thong tin may chu
//////////////////////////////////////////////////////////////////////////////
static int LuaRpc_GetHostPlayerCount(Lua_State* L)
{
	Lua_PushNumber(L, (double)g_HostServer.GetPlayerCount());
	return 1;
}

static int LuaRpc_IsGameServerReady(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	unsigned long ulId = (unsigned long)(int)Lua_ValueToNumber(L, 1);
	CNetConnectDup conndup = g_HostServer.FindNetConnect(ulId);
	Lua_PushNumber(L, conndup.IsValid() ? 1 : 0);
	return 1;
}

// Ben ta dwGameSvrId CHINH LA so hieu ket noi (CHostConnect::GetID()) nen hai ham
// doi chieu cua ban Linux la anh xa dong nhat. Giu lai cho kich ban chep sang chay duoc.
static int LuaRpc_ConnectIdx2GameServerId(Lua_State* L)
{
	Lua_PushNumber(L, Lua_IsNumber(L, 1) ? (double)Lua_ValueToNumber(L, 1) : 0);
	return 1;
}

static int LuaRpc_GameServerId2ConnectIdx(Lua_State* L)
{
	Lua_PushNumber(L, Lua_IsNumber(L, 1) ? (double)Lua_ValueToNumber(L, 1) : 0);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Nhan tin -- deu di qua duong GM "dw <lenh Lua>" co san cua GameServer
//////////////////////////////////////////////////////////////////////////////
// Doi dau nhay don trong chuoi thanh dau cach de khong pha cu phap [[...]]
static void RpcSafeText(const char* szIn, char* szOut, int nCap)
{
	int j = 0;
	for (int i = 0; szIn[i] && j < nCap - 1; i++)
	{
		char c = szIn[i];
		if (c == '[' && szIn[i + 1] == '[') { szOut[j++] = '['; if (j < nCap - 1) szOut[j++] = ' '; i++; continue; }
		if (c == ']' && szIn[i + 1] == ']') { szOut[j++] = ']'; if (j < nCap - 1) szOut[j++] = ' '; i++; continue; }
		szOut[j++] = c;
	}
	szOut[j] = 0;
}

static void RpcGlobalLua(const char* szLua)
{
	if (!szLua || !szLua[0])
		return;
	int nLen = (int)strlen(szLua);
	size_t nPck = sizeof(GM_EXECUTE_COMMAND) + nLen;
	char* pBuf = new char[nPck];
	if (!pBuf)
		return;
	GM_EXECUTE_COMMAND exe;
	exe.ProtocolFamily = pf_gamemaster;
	exe.ProtocolType = gm_c2s_execute;
	strncpy(exe.AccountName, "GM", 32);
	exe.wLength = (WORD)nLen;
	exe.wExecuteID = 1;
	memcpy(pBuf, &exe, sizeof(GM_EXECUTE_COMMAND));
	memcpy(pBuf + sizeof(GM_EXECUTE_COMMAND), szLua, nLen);
	BroadGlobal(pBuf, nPck, 0, 0);
	delete[] pBuf;
}

// Msg2PlayerByName(szName, szMsg) / Msg2Tong(nTongId, szMsg) / Msg2Faction(nFaction, szMsg)
static int LuaRpc_Msg2PlayerByName(Lua_State* L)
{
	if (!Lua_IsString(L, 1) || !Lua_IsString(L, 2))
		return 0;
	char szName[128], szMsg[1024], szLua[1400];
	RpcSafeText(Lua_ValueToString(L, 1), szName, sizeof(szName));
	RpcSafeText(Lua_ValueToString(L, 2), szMsg, sizeof(szMsg));
	_snprintf(szLua, sizeof(szLua) - 1, "dw Msg2PlayerByName([[%s]],[[%s]]);", szName, szMsg);
	szLua[sizeof(szLua) - 1] = 0;
	RpcGlobalLua(szLua);
	return 0;
}

static int LuaRpc_Msg2Tong(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
		return 0;
	char szMsg[1024], szLua[1200];
	RpcSafeText(Lua_ValueToString(L, 2), szMsg, sizeof(szMsg));
	_snprintf(szLua, sizeof(szLua) - 1, "dw Msg2Tong(%d,[[%s]]);", (int)Lua_ValueToNumber(L, 1), szMsg);
	szLua[sizeof(szLua) - 1] = 0;
	RpcGlobalLua(szLua);
	return 0;
}

static int LuaRpc_Msg2Faction(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
		return 0;
	char szMsg[1024], szLua[1200];
	RpcSafeText(Lua_ValueToString(L, 2), szMsg, sizeof(szMsg));
	_snprintf(szLua, sizeof(szLua) - 1, "dw Msg2Faction(%d,[[%s]]);", (int)Lua_ValueToNumber(L, 1), szMsg);
	szLua[sizeof(szLua) - 1] = 0;
	RpcGlobalLua(szLua);
	return 0;
}

// AddGlobalNews(szMsg [, nCount]) -- doc thong bao toan may chu
static int LuaRpc_AddGlobalNews(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	int nCount = (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2)) ? (int)Lua_ValueToNumber(L, 2) : 1;
	if (nCount < 1) nCount = 1;
	char szMsg[1024], szLua[1200];
	RpcSafeText(Lua_ValueToString(L, 1), szMsg, sizeof(szMsg));
	_snprintf(szLua, sizeof(szLua) - 1, "dw AddLocalCountNews([[%s]],%d);", szMsg, nCount);
	szLua[sizeof(szLua) - 1] = 0;
	RpcGlobalLua(szLua);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Bao GameServer rang mot ban ghi ShareData vua doi
//////////////////////////////////////////////////////////////////////////////
// NotifySDBRecordChanged(szKey, nP1, nP2, nFlag) -- bao TAT CA may chu
// NotifySDBRChanged1Svr(dwGameSvrId, szKey, nP1, nP2, nFlag) -- bao MOT may chu
//
// Ben ta bao bang cach goi ham Lua SDB_OnRecordChanged trong kich ban
// \script\lib\sdbnotify.lua cua GameServer (neu co). Kich ban nao muon nghe thi
// tu dat ham do; khong co thi GameServer chi ghi mot dong loi, khong sao.
static void RpcNotifySDB(unsigned long ulConnId, const char* szKey, int nP1, int nP2, int nFlag)
{
	char szSafe[128], szLua[512];
	RpcSafeText(szKey ? szKey : "", szSafe, sizeof(szSafe));
	_snprintf(szLua, sizeof(szLua) - 1,
		// [RELAYHT 06/09 VA2] KHONG dung duoc "dw": GMDoWorldScriptAction chi dang ky 9 ham
		// WorldScriptFuns (ScriptFuns.cpp:16334) va DynamicExecute KHONG nam trong do.
		// Goi thang bang duong RPC s2s_script (Core xu ly o KJx2_OnRelayScriptPacket).
		"dw DynamicExecute([[\\script\\lib\\sdbnotify.lua]],[[SDB_OnRecordChanged]],[[%s]],%d,%d,%d);",
		szSafe, nP1, nP2, nFlag);
	szLua[sizeof(szLua) - 1] = 0;
	if (ulConnId == 0)
	{
		RpcGlobalLua(szLua);
		return;
	}
	int nLen = (int)strlen(szLua);
	size_t nPck = sizeof(GM_EXECUTE_COMMAND) + nLen;
	char* pBuf = new char[nPck];
	if (!pBuf)
		return;
	GM_EXECUTE_COMMAND exe;
	exe.ProtocolFamily = pf_gamemaster;
	exe.ProtocolType = gm_c2s_execute;
	strncpy(exe.AccountName, "GM", 32);
	exe.wLength = (WORD)nLen;
	exe.wExecuteID = 1;
	memcpy(pBuf, &exe, sizeof(GM_EXECUTE_COMMAND));
	memcpy(pBuf + sizeof(GM_EXECUTE_COMMAND), szLua, nLen);

	char szOut[1024];
	RELAY_DATA* pRD = (RELAY_DATA*)szOut;
	pRD->ProtocolFamily = pf_relay;
	pRD->ProtocolID = relay_c2c_data;
	pRD->nToIP = 0;
	pRD->nToRelayID = 0;
	pRD->nFromIP = 0;
	pRD->nFromRelayID = 0;
	pRD->routeDateLength = (WORD)(1 + nPck);
	if (sizeof(RELAY_DATA) + 1 + nPck <= sizeof(szOut))
	{
		BYTE* p = (BYTE*)szOut + sizeof(RELAY_DATA);
		*p++ = (BYTE)s2s_execute;
		memcpy(p, pBuf, nPck);
		RpcSend(ulConnId, szOut, sizeof(RELAY_DATA) + 1 + nPck);
	}
	delete[] pBuf;
}

static int LuaRpc_NotifySDBRecordChanged(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	RpcNotifySDB(0, Lua_ValueToString(L, 1),
		(int)Lua_ValueToNumber(L, 2), (int)Lua_ValueToNumber(L, 3),
		(Lua_GetTopIndex(L) >= 4) ? (int)Lua_ValueToNumber(L, 4) : 1);
	return 0;
}

static int LuaRpc_NotifySDBRChanged1Svr(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
		return 0;
	RpcNotifySDB((unsigned long)(int)Lua_ValueToNumber(L, 1), Lua_ValueToString(L, 2),
		(int)Lua_ValueToNumber(L, 3), (int)Lua_ValueToNumber(L, 4),
		(Lua_GetTopIndex(L) >= 5) ? (int)Lua_ValueToNumber(L, 5) : 1);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Bang ham
//////////////////////////////////////////////////////////////////////////////
TLua_Funcs g_RelayRpcFuns[] =
{
	{ "RemoteExecute",				LuaRpc_RemoteExecute },
	{ "GetHostPlayerCount",			LuaRpc_GetHostPlayerCount },
	{ "IsGameServerReady",			LuaRpc_IsGameServerReady },
	{ "ConnectIdx2GameServerId",	LuaRpc_ConnectIdx2GameServerId },
	{ "GameServerId2ConnectIdx",	LuaRpc_GameServerId2ConnectIdx },
	{ "Msg2PlayerByName",			LuaRpc_Msg2PlayerByName },
	{ "Msg2Tong",					LuaRpc_Msg2Tong },
	{ "Msg2Faction",				LuaRpc_Msg2Faction },
	{ "AddGlobalNews",				LuaRpc_AddGlobalNews },
	{ "NotifySDBRecordChanged",		LuaRpc_NotifySDBRecordChanged },
	{ "NotifySDBRChanged1Svr",		LuaRpc_NotifySDBRChanged1Svr },
};

int g_GetRelayRpcFunNum()
{
	return sizeof(g_RelayRpcFuns) / sizeof(TLua_Funcs);
}

//////////////////////////////////////////////////////////////////////////////
// Khoi dong / dong
//////////////////////////////////////////////////////////////////////////////
BOOL RelayRpc_Init()
{
	if (s_bReady)
		return TRUE;
	::InitializeCriticalSection(&s_csQueue);
	s_bCsReady = TRUE;
	s_bReady = TRUE;
	rTRACE("[RelayRpc] san sang (hang doi %d, cho ket qua %d)", RPC_MAX_QUEUE, RPC_MAX_PENDING);
	return TRUE;
}

void RelayRpc_Uninit()
{
	s_bReady = FALSE;
	if (s_bCsReady)
	{
		::EnterCriticalSection(&s_csQueue);
		s_lstIn.clear();
		::LeaveCriticalSection(&s_csQueue);
		::DeleteCriticalSection(&s_csQueue);
		s_bCsReady = FALSE;
	}
	s_mapPending.clear();
}
