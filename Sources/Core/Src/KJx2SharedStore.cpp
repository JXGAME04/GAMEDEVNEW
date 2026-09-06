// KJx2SharedStore.cpp - xem KJx2SharedStore.h. DOT E cong thanh JX2.
// Persist Ladder theo khuon tmp + MoveFileEx REPLACE (KIniFile::Save cat trang
// file truoc khi ghi nen KHONG dung cho du lieu song - phan bien E ky thuat F6).

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KGameKV.h"
#include "KWin32.h"

// [MAIL 03/09] ObjBuffer bien dich cho CA client (kenh ScriptProtocol); RemoteExecute, ShareData,
// Ladder, GlbValue van nam trong #ifdef _SERVER phia duoi.

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KJx2SharedStore.h"
#include "KNewProtocolProcess.h"	// [RELAYHT 06/09] gui goi len relay
#include "KSortScript.h"	// [RELAYHT 06/09 VA2] g_GetScriptNameByState
#include <string>
#include <map>
#include <vector>
#include <string.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// 1) ObjBuffer
//////////////////////////////////////////////////////////////////////
#define JX2OB_BUF_SIZE 4096

struct KJx2ObjBuffer
{
	unsigned char	Buf[JX2OB_BUF_SIZE];
	int				nWrite;
	int				nRead;
};

static std::map<int, KJx2ObjBuffer*>	s_OBMap;
static int								s_nOBNextHandle = 0;

static KJx2ObjBuffer* sOBGet(Lua_State* L, int nArg)
{
	if (!Lua_IsNumber(L, nArg))
		return NULL;
	int h = (int)Lua_ValueToNumber(L, nArg);
	if (h <= 0)
		return NULL;
	std::map<int, KJx2ObjBuffer*>::iterator it = s_OBMap.find(h);
	return (it == s_OBMap.end()) ? NULL : it->second;
}

static bool sOBWrite(KJx2ObjBuffer* p, const void* pData, int nLen)
{
	if (p->nWrite + nLen > JX2OB_BUF_SIZE)
		return false;	// day - bo qua (goc buffer cung 4KB)
	memcpy(p->Buf + p->nWrite, pData, nLen);
	p->nWrite += nLen;
	return true;
}

static bool sOBRead(KJx2ObjBuffer* p, void* pData, int nLen)
{
	if (p->nRead + nLen > p->nWrite)
		return false;
	memcpy(pData, p->Buf + p->nRead, nLen);
	p->nRead += nLen;
	return true;
}

int LuaOB_Create(Lua_State* L)
{
	KJx2ObjBuffer* p = new KJx2ObjBuffer;
	if (!p)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	p->nWrite = 0;
	p->nRead = 0;
	int h = ++s_nOBNextHandle;
	s_OBMap[h] = p;
	Lua_PushNumber(L, h);
	return 1;
}

int LuaOB_Release(Lua_State* L)
{
	if (Lua_IsNumber(L, 1))
	{
		int h = (int)Lua_ValueToNumber(L, 1);
		std::map<int, KJx2ObjBuffer*>::iterator it = s_OBMap.find(h);
		if (it != s_OBMap.end())
		{
			delete it->second;
			s_OBMap.erase(it);
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

#ifdef _SERVER	// [MAIL 03/09] RemoteExecute + ShareData: chi may chu
// [PORT5 23/08] RemoteExecute (Linux GS 0x08100740 / relay 0x0810363A): RPC GS<->relay qua
// ObjBuffer. Du an 1 GS khong relay -> thuc thi TAI CHO dong bo: fn(hParam, hRes, 0) trong
// state cua szScript (co remap \script\lib -> \scriptjx2\lib nhu Include); co callback ->
// cb(nCbParam, hRes) trong state DANG GOI; dwGameSvrId (tham so 6) bo qua. Caller tu
// OB_Release hParam; hRes do ham nay cap va huy.
//////////////////////////////////////////////////////////////////////////////
// [RELAYHT 06/09] REMOTEEXECUTE THAT: GameServer -> S3Relay
//
// Goi nam trong RELAY_DATA (nToIP = 0 nghia la 'da toi noi' -> relay tu xu ly),
// byte dau than goi la s2s_script, roi RELAY_SCRIPT_CALL, roi 3 khoi:
//     szScript / szFunc / du lieu ObjBuffer.
// Ham callback duoc tim trong state cua CHINH tep kich ban da goi -- dung theo
// loi remoteexc.lua ban Linux (callback dinh nghia cung tep voi ham duoc goi).
//////////////////////////////////////////////////////////////////////////////
#define JX2RPC_MAX_PENDING	512
#define JX2RPC_MAX_PACKET	8192

struct KJx2RpcPending
{
	std::string	strScript;
	std::string	strCbFunc;
	int			nCbParam;
};

static std::map<DWORD, KJx2RpcPending>	s_mapRpcPending;
static DWORD							s_dwRpcNextId = 0;

// Tra ve 1 = da xep vao hang gui len relay.
static int Jx2Relay_SendRemote(const char* szScript, const char* szFunc,
							   const BYTE* pData, int nDataLen,
							   const char* szCbFunc, int nCbParam,
							   const char* szCallerScript)
{
	if (!szScript || !szScript[0] || !szFunc || !szFunc[0])
		return 0;
	if (nDataLen < 0)
		nDataLen = 0;

	size_t nScriptLen = strlen(szScript) + 1;
	size_t nFuncLen = strlen(szFunc) + 1;
	size_t nBody = 1 + sizeof(RELAY_SCRIPT_CALL) + nScriptLen + nFuncLen + nDataLen;
	size_t nTotal = sizeof(RELAY_DATA) + nBody;
	if (nTotal > JX2RPC_MAX_PACKET)
	{
		g_DebugLog((LPSTR)"[RELAYHT] RemoteExecute: goi qua lon (%d byte) %.128s", (int)nTotal, szScript);
		return 0;
	}

	DWORD dwCallId = 0;
	if (szCbFunc && szCbFunc[0])
	{
		if (s_mapRpcPending.size() >= JX2RPC_MAX_PENDING)
		{
			g_DebugLog((LPSTR)"[RELAYHT] RemoteExecute: tran so lenh cho ket qua");
			return 0;
		}
		dwCallId = ++s_dwRpcNextId;
		if (dwCallId == 0)
			dwCallId = ++s_dwRpcNextId;
		KJx2RpcPending pend;
		// [RELAYHT 06/09 VA2] callback PHAI chay trong state cua TEP DANG GOI.
		// Truoc day ghi szScript = duong dan DICH, ma tep dich chinh la tep
		// GameServer KHONG co (do la ly do phai gui di) -> g_GetScript luon that
		// bai -> callback khong bao gio chay.
		pend.strScript = (szCallerScript && szCallerScript[0]) ? szCallerScript : szScript;
		pend.strCbFunc = szCbFunc;
		pend.nCbParam = nCbParam;
		s_mapRpcPending[dwCallId] = pend;
	}

	char szBuf[JX2RPC_MAX_PACKET];
	RELAY_DATA* pRD = (RELAY_DATA*)szBuf;
	pRD->ProtocolFamily = pf_relay;
	pRD->ProtocolID = relay_c2c_data;
	pRD->nToIP = 0;				// 0 = da toi noi: relay tu xu ly than goi
	pRD->nToRelayID = 0;
	pRD->nFromIP = 0;
	pRD->nFromRelayID = 0;
	pRD->routeDateLength = (WORD)nBody;

	BYTE* p = (BYTE*)szBuf + sizeof(RELAY_DATA);
	*p++ = (BYTE)s2s_script;
	RELAY_SCRIPT_CALL* pSC = (RELAY_SCRIPT_CALL*)p;
	pSC->ProtocolFamily = pf_relay;
	pSC->ProtocolID = relay_c2s_script;
	pSC->dwCallId = dwCallId;
	pSC->dwGameSvrId = 0;
	pSC->byIsResult = 0;
	pSC->wScriptLen = (WORD)nScriptLen;
	pSC->wFuncLen = (WORD)nFuncLen;
	pSC->wDataLen = (WORD)nDataLen;
	p += sizeof(RELAY_SCRIPT_CALL);

	memcpy(p, szScript, nScriptLen);	p += nScriptLen;
	memcpy(p, szFunc, nFuncLen);		p += nFuncLen;
	if (nDataLen > 0)
		memcpy(p, pData, nDataLen);

	g_NewProtocolProcess.PushMsgInTransfer(szBuf, nTotal);
	return 1;
}

// Relay tra ket qua ve. Goi tu KNewProtocolProcess (bang giao thuc).
void KJx2_OnRelayScriptPacket(const void* pData, int nSize)
{
	if (!pData || nSize < (int)sizeof(RELAY_SCRIPT_CALL))
		return;
	RELAY_SCRIPT_CALL sc;
	memcpy(&sc, pData, sizeof(sc));
	const BYTE* q = (const BYTE*)pData + sizeof(RELAY_SCRIPT_CALL);
	int nLeft = nSize - (int)sizeof(RELAY_SCRIPT_CALL);
	if ((int)sc.wScriptLen + sc.wFuncLen + sc.wDataLen > nLeft)
		return;
	const BYTE* pRes = q + sc.wScriptLen + sc.wFuncLen;

	const char* szScriptIn = (const char*)q;
	const char* szFuncIn = (const char*)q + sc.wScriptLen;
	if (sc.wScriptLen == 0 || szScriptIn[sc.wScriptLen - 1] != 0)
		return;
	if (sc.wFuncLen == 0 || szFuncIn[sc.wFuncLen - 1] != 0)
		return;

	if (!sc.byIsResult)
	{
		// [RELAYHT 06/09 VA2] RELAY GOI XUONG: chay szFuncIn trong state cua szScriptIn,
		// giong het cach relay chay lenh cua GameServer (RelayRpc.cpp RpcRunOne).
		// Nho nhanh nay ma relay goi duoc ham cua GameServer bang duong RPC that,
		// khong phai muon duong GM "dw" (chi co 9 ham WorldScriptFuns).
		char szLowIn[MAX_PATH];
		strncpy(szLowIn, szScriptIn, MAX_PATH - 1);
		szLowIn[MAX_PATH - 1] = 0;
		for (char* pc = szLowIn; *pc; pc++)
		{
			if (*pc >= 'A' && *pc <= 'Z')
				*pc += 'a' - 'A';
		}
		KLuaScript* pIn = (KLuaScript*)g_GetScript(szLowIn);
		if (!pIn || !pIn->m_LuaState)
		{
			g_DebugLog((LPSTR)"[RELAYHT] relay goi xuong: khong thay kich ban %.128s", szLowIn);
			return;
		}
		KJx2ObjBuffer* pP = new KJx2ObjBuffer;
		KJx2ObjBuffer* pR = new KJx2ObjBuffer;
		if (!pP || !pR)
			return;
		pP->nRead = 0;
		pP->nWrite = (sc.wDataLen > JX2OB_BUF_SIZE) ? JX2OB_BUF_SIZE : sc.wDataLen;
		if (pP->nWrite > 0)
			memcpy(pP->Buf, pRes, pP->nWrite);
		pR->nRead = 0;
		pR->nWrite = 0;
		int hP = ++s_nOBNextHandle;
		s_OBMap[hP] = pP;
		int hR = ++s_nOBNextHandle;
		s_OBMap[hR] = pR;
		char szCallIn[600];
		sprintf(szCallIn, "%s(%d,%d,0)", szFuncIn, hP, hR);
		int nTopIn = lua_gettop(pIn->m_LuaState);
		if (lua_dostring(pIn->m_LuaState, szCallIn) != 0)
			g_DebugLog((LPSTR)"[RELAYHT] relay goi xuong LOI: %.200s", szCallIn);
		lua_settop(pIn->m_LuaState, nTopIn);
		{
			std::map<int, KJx2ObjBuffer*>::iterator i1 = s_OBMap.find(hP);
			if (i1 != s_OBMap.end()) { delete i1->second; s_OBMap.erase(i1); }
			std::map<int, KJx2ObjBuffer*>::iterator i2 = s_OBMap.find(hR);
			if (i2 != s_OBMap.end()) { delete i2->second; s_OBMap.erase(i2); }
		}
		return;
	}

	std::map<DWORD, KJx2RpcPending>::iterator it = s_mapRpcPending.find(sc.dwCallId);
	if (it == s_mapRpcPending.end())
		return;
	KJx2RpcPending pend = it->second;
	s_mapRpcPending.erase(it);
	if (pend.strCbFunc.empty())
		return;

	KLuaScript* pScript = (KLuaScript*)g_GetScript((char*)pend.strScript.c_str());
	if (!pScript || !pScript->m_LuaState)
	{
		g_DebugLog((LPSTR)"[RELAYHT] callback: khong thay kich ban %.128s", pend.strScript.c_str());
		return;
	}

	KJx2ObjBuffer* pOb = new KJx2ObjBuffer;
	if (!pOb)
		return;
	pOb->nRead = 0;
	pOb->nWrite = (sc.wDataLen > JX2OB_BUF_SIZE) ? JX2OB_BUF_SIZE : sc.wDataLen;
	if (pOb->nWrite > 0)
		memcpy(pOb->Buf, pRes, pOb->nWrite);
	int hRes = ++s_nOBNextHandle;
	s_OBMap[hRes] = pOb;

	char szCall[600];
	sprintf(szCall, "%s(%d,%d)", pend.strCbFunc.c_str(), pend.nCbParam, hRes);
	int nTop = lua_gettop(pScript->m_LuaState);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		g_DebugLog((LPSTR)"[RELAYHT] callback LOI: %.200s", szCall);
	lua_settop(pScript->m_LuaState, nTop);

	{
		std::map<int, KJx2ObjBuffer*>::iterator itR = s_OBMap.find(hRes);
		if (itR != s_OBMap.end())
		{
			delete itR->second;
			s_OBMap.erase(itR);
		}
	}
}

int LuaJX2_RemoteExecute(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2) || !Lua_IsNumber(L, 3))
		return 0;
	const char* szScript = Lua_ValueToString(L, 1);
	const char* szFunc = Lua_ValueToString(L, 2);
	int nHandle = (int)Lua_ValueToNumber(L, 3);
	if (!szScript || !szScript[0] || !szFunc || !szFunc[0] || nHandle < 0)
		return 0;
	char szLow[MAX_PATH];
	strncpy(szLow, szScript, MAX_PATH - 1);
	szLow[MAX_PATH - 1] = 0;
	{
		for (char* pc = szLow; *pc; pc++)
		{
			if (*pc >= 'A' && *pc <= 'Z')
				*pc += 'a' - 'A';
		}
	}
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
	if (!pScript)
	{
		char szAlt[MAX_PATH + 16];
		const char* pLib = strstr(szLow, "\\script\\lib\\");
		if (pLib)
		{
			int nPre = (int)(pLib - szLow);
			memcpy(szAlt, szLow, nPre);
			szAlt[nPre] = 0;
			strcat(szAlt, "\\scriptjx2\\lib\\");
			strcat(szAlt, pLib + 12);
			pScript = (KLuaScript*)g_GetScript(szAlt);
		}
	}
	if (!pScript || !pScript->m_LuaState)
	{
		// [RELAYHT 06/09] khong co kich ban nay trong cay GameServer -> GUI LEN S3RELAY
		// chay (truoc day chi ghi log roi bo qua). Kich ban CO SAN van chay tai cho
		// nhu cu nen cac cho goi dang chay khong doi hanh vi.
		const BYTE* pOut = NULL;
		int nOutLen = 0;
		{
			std::map<int, KJx2ObjBuffer*>::iterator itP = s_OBMap.find(nHandle);
			if (itP != s_OBMap.end())
			{
				pOut = itP->second->Buf;
				nOutLen = itP->second->nWrite;
			}
		}
		const char* szCbR = (Lua_GetTopIndex(L) >= 4 && Lua_IsString(L, 4)) ? Lua_ValueToString(L, 4) : NULL;
		int nCbIdR = (Lua_GetTopIndex(L) >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : 0;
		const char* szSelf = g_GetScriptNameByState(L);	// [RELAYHT 06/09 VA2]
		int nSent = Jx2Relay_SendRemote(szLow, szFunc, pOut, nOutLen, szCbR, nCbIdR, szSelf);
		if (!nSent)
			g_DebugLog((LPSTR)"[RELAYHT] RemoteExecute: khong nap duoc, gui len relay cung hong: %.128s", szLow);
		Lua_PushNumber(L, nSent);
		return 1;
	}
	const char* szCb = (Lua_GetTopIndex(L) >= 4 && Lua_IsString(L, 4)) ? Lua_ValueToString(L, 4) : NULL;
	int nCbId = (Lua_GetTopIndex(L) >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : 0;
	KJx2ObjBuffer* pRes = new KJx2ObjBuffer;
	pRes->nWrite = 0;
	pRes->nRead = 0;
	int hRes = ++s_nOBNextHandle;
	s_OBMap[hRes] = pRes;
	char szCall[600];
	sprintf(szCall, "%s(%d,%d,0)", szFunc, nHandle, hRes);
	int nTop0 = 0;
	pScript->SafeCallBegin(&nTop0);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		g_DebugLog((LPSTR)"[PORT5] RemoteExecute LOI: %.128s -> %.200s", szLow, szCall);
	pScript->SafeCallEnd(nTop0);
	if (szCb && szCb[0])
	{
		sprintf(szCall, "%s(%d,%d)", szCb, nCbId, hRes);
		int nTopL = lua_gettop(L);
		if (lua_dostring(L, szCall) != 0)
			g_DebugLog((LPSTR)"[PORT5] RemoteExecute cb LOI: %.200s", szCall);
		lua_settop(L, nTopL);
	}
	{
		std::map<int, KJx2ObjBuffer*>::iterator itR = s_OBMap.find(hRes);
		if (itR != s_OBMap.end())
		{
			delete itR->second;
			s_OBMap.erase(itR);
		}
	}
	Lua_PushNumber(L, 1);
	return 1;
}

// [PORT5 23/08] OB_Save/LoadShareData (relay s3relay_y 0x08102F54 / 0x08102D58): Linux luu MySQL
// bang ShareData khoa (ShareKey, Param1, Param2). Du an persist ra tep
// \settings\jx2sharedata\<key>_<p1>_<p2>.bin theo khuon tmp + MoveFileEx cua Ladder.
// tongcastle luu diem bang/nguoi choi + trang thai cay than.
static void sShareDataPath(char* szOut, const char* szKey, int nP1, int nP2, bool bTmp)
{
	char szRoot[MAX_PATH];
	g_GetRootPath(szRoot);
	char szSafe[64];
	int i = 0;
	for (; szKey[i] && i < 60; i++)
	{
		char c = szKey[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_')
			szSafe[i] = c;
		else
			szSafe[i] = '_';
	}
	szSafe[i] = 0;
	sprintf(szOut, "%s\\settings\\jx2sharedata", szRoot);
	CreateDirectory(szOut, NULL);
	sprintf(szOut, "%s\\settings\\jx2sharedata\\%s_%d_%d.bin%s", szRoot, szSafe, nP1, nP2, bTmp ? ".tmp" : "");
}

int LuaOB_SaveShareData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KJx2ObjBuffer* p = sOBGet(L, 1);
	const char* szKey = Lua_ValueToString(L, 2);
	if (!p || !szKey || !szKey[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nP1 = (int)Lua_ValueToNumber(L, 3);
	int nP2 = (int)Lua_ValueToNumber(L, 4);
	char szTmp[MAX_PATH], szPath[MAX_PATH];
	sShareDataPath(szTmp, szKey, nP1, nP2, true);
	sShareDataPath(szPath, szKey, nP1, nP2, false);
	FILE* f = fopen(szTmp, "wb");
	if (!f)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (p->nWrite > 0)
		fwrite(p->Buf, 1, p->nWrite, f);
	fclose(f);
	MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaOB_LoadShareData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KJx2ObjBuffer* p = sOBGet(L, 1);
	const char* szKey = Lua_ValueToString(L, 2);
	if (!p || !szKey || !szKey[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nP1 = (int)Lua_ValueToNumber(L, 3);
	int nP2 = (int)Lua_ValueToNumber(L, 4);
	p->nWrite = 0;
	p->nRead = 0;
	char szPath[MAX_PATH];
	sShareDataPath(szPath, szKey, nP1, nP2, false);
	FILE* f = fopen(szPath, "rb");
	if (f)
	{
		p->nWrite = (int)fread(p->Buf, 1, JX2OB_BUF_SIZE, f);
		fclose(f);
	}
	// khong co tep -> buffer rong (OB_IsEmpty == 1) nhu Linux khong co ban ghi
	Lua_PushNumber(L, 1);
	return 1;
}
#endif // _SERVER (RemoteExecute + ShareData) [MAIL 03/09]

int LuaOB_IsEmpty(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	// handle xau cung tinh la RONG (= 1) theo binary goc
	Lua_PushNumber(L, (p == NULL || p->nRead >= p->nWrite) ? 1 : 0);
	return 1;
}

int LuaOB_Clear(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	if (p)
	{
		p->nWrite = 0;
		p->nRead = 0;
	}
	Lua_PushNumber(L, p ? 1 : 0);
	return 1;
}

int LuaOB_Append(Lua_State* L)
{
	KJx2ObjBuffer* pDst = sOBGet(L, 1);
	KJx2ObjBuffer* pSrc = sOBGet(L, 2);
	int nOk = 0;
	if (pDst && pSrc && sOBWrite(pDst, pSrc->Buf, pSrc->nWrite))
		nOk = 1;
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaOB_Copy(Lua_State* L)
{
	KJx2ObjBuffer* pDst = sOBGet(L, 1);
	KJx2ObjBuffer* pSrc = sOBGet(L, 2);
	int nOk = 0;
	if (pDst && pSrc)
	{
		memcpy(pDst->Buf, pSrc->Buf, JX2OB_BUF_SIZE);
		pDst->nWrite = pSrc->nWrite;
		pDst->nRead = pSrc->nRead;
		nOk = 1;
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaOB_PushByte(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	int nOk = 0;
	if (p && Lua_IsNumber(L, 2))
	{
		unsigned char b = (unsigned char)(int)Lua_ValueToNumber(L, 2);
		nOk = sOBWrite(p, &b, 1) ? 1 : 0;
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

// PopByte HET du lieu phai tra NIL (objbuffer_head.lua:53 kiem "== nil")
int LuaOB_PopByte(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	unsigned char b;
	if (p && sOBRead(p, &b, 1))
	{
		Lua_PushNumber(L, b);
		return 1;
	}
	return 0;
}

int LuaOB_PushInt(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	int nOk = 0;
	if (p && Lua_IsNumber(L, 2))
	{
		int n = (int)Lua_ValueToNumber(L, 2);
		nOk = sOBWrite(p, &n, sizeof(int)) ? 1 : 0;
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaOB_PopInt(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	int n;
	if (p && sOBRead(p, &n, sizeof(int)))
	{
		Lua_PushNumber(L, n);
		return 1;
	}
	return 0;
}

int LuaOB_PushDouble(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	int nOk = 0;
	if (p && Lua_IsNumber(L, 2))
	{
		double f = Lua_ValueToNumber(L, 2);
		nOk = sOBWrite(p, &f, sizeof(double)) ? 1 : 0;
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaOB_PopDouble(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	double f;
	if (p && sOBRead(p, &f, sizeof(double)))
	{
		Lua_PushNumber(L, f);
		return 1;
	}
	return 0;
}

// Chuoi luu [int nLen][nLen byte] - dinh dang noi bo (2 dau deu la ta)
int LuaOB_PushString(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	int nOk = 0;
	if (p && Lua_IsString(L, 2))
	{
		const char* sz = Lua_ValueToString(L, 2);
		int nLen = (int)strlen(sz);
		if (p->nWrite + (int)sizeof(int) + nLen <= JX2OB_BUF_SIZE)
		{
			sOBWrite(p, &nLen, sizeof(int));
			sOBWrite(p, sz, nLen);
			nOk = 1;
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaOB_PopString(Lua_State* L)
{
	KJx2ObjBuffer* p = sOBGet(L, 1);
	int nLen;
	if (p && sOBRead(p, &nLen, sizeof(int)))
	{
		if (nLen >= 0 && p->nRead + nLen <= p->nWrite && nLen < JX2OB_BUF_SIZE)
		{
			char szBuf[JX2OB_BUF_SIZE];
			memcpy(szBuf, p->Buf + p->nRead, nLen);
			szBuf[nLen] = 0;
			p->nRead += nLen;
			Lua_PushString(L, szBuf);
			return 1;
		}
	}
	return 0;
}

// [MAIL 03/09] truy cap byte tho cho kenh ScriptProtocol (KScriptProtocol.cpp)
int KJx2OB_CreateFromBytes(const void* pData, int nLen)
{
	if (nLen < 0 || nLen > JX2OB_BUF_SIZE || (nLen > 0 && !pData))
		return 0;
	KJx2ObjBuffer* p = new KJx2ObjBuffer;
	if (!p)
		return 0;
	if (nLen > 0)
		memcpy(p->Buf, pData, nLen);
	p->nWrite = nLen;
	p->nRead = 0;
	int h = ++s_nOBNextHandle;
	s_OBMap[h] = p;
	return h;
}

int KJx2OB_GetBytes(int h, const unsigned char** ppData)
{
	if (h <= 0 || !ppData)
		return -1;
	std::map<int, KJx2ObjBuffer*>::iterator it = s_OBMap.find(h);
	if (it == s_OBMap.end())
		return -1;
	*ppData = it->second->Buf;
	return it->second->nWrite;
}

int KJx2OB_Release(int h)
{
	std::map<int, KJx2ObjBuffer*>::iterator it = s_OBMap.find(h);
	if (it == s_OBMap.end())
		return 0;
	delete it->second;
	s_OBMap.erase(it);
	return 1;
}

#ifdef _SERVER	// [MAIL 03/09] Ladder + GlbValue: chi may chu (dong bang #endif cuoi tep)
//////////////////////////////////////////////////////////////////////
// 2) Ladder - top 10 moi id, id > 10000, persist \settings\jx2ladder.txt
//////////////////////////////////////////////////////////////////////
struct KJx2LadderEntry
{
	char			szName[32];
	__int64			i64Value;
	int				nSect;
	unsigned char	btGender;
	unsigned char	btType;
};

#define JX2LADDER_TOP		10
#define JX2LADDER_MIN_ID	10000
#define JX2LADDER_FILE		"\\settings\\jx2ladder.txt"
#define JX2LADDER_KV_NS		"jx2.ladder"

static std::map<unsigned long, std::vector<KJx2LadderEntry> >	s_LadderMap;
static bool	s_bLadderLoaded = false;

static void sLadderPath(char* szOut, bool bTmp)
{
	char szRoot[MAX_PATH];
	g_GetRootPath(szRoot);
	sprintf(szOut, "%s%s%s", szRoot, JX2LADDER_FILE, bTmp ? ".tmp" : "");
}

static void sLadderLoad()
{
	if (s_bLadderLoaded)
		return;
	s_bLadderLoaded = true;
	char szPath[MAX_PATH];
	sLadderPath(szPath, false);
	FILE* f = fopen(szPath, "rb");
	if (!f)
		return;
	char szLine[512];
	unsigned long uCurId = 0;
	while (fgets(szLine, sizeof(szLine), f))
	{
		if (szLine[0] == 'L')
		{
			uCurId = (unsigned long)atol(szLine + 2);
		}
		else if (szLine[0] == 'E' && uCurId > JX2LADDER_MIN_ID)
		{
			// E <value> <sect> <gender> <type> <name den het dong>
			KJx2LadderEntry e;
			memset(&e, 0, sizeof(e));
			__int64 v = 0;
			int nSect = -1, nGender = 0, nType = 0, nPos = 0;
			if (sscanf(szLine + 2, "%I64d %d %d %d %n", &v, &nSect, &nGender, &nType, &nPos) >= 4 && nPos > 0)
			{
				char* szName = szLine + 2 + nPos;
				int nLen = (int)strlen(szName);
				while (nLen > 0 && (szName[nLen - 1] == '\n' || szName[nLen - 1] == '\r'))
					szName[--nLen] = 0;
				if (nLen > 0)
				{
					e.i64Value = v;
					e.nSect = nSect;
					e.btGender = (unsigned char)nGender;
					e.btType = (unsigned char)nType;
					strncpy(e.szName, szName, 31);
					if (s_LadderMap[uCurId].size() < JX2LADDER_TOP)
						s_LadderMap[uCurId].push_back(e);
				}
			}
		}
	}
	fclose(f);
}

static void sLadderSave()
{
	char szTmp[MAX_PATH], szPath[MAX_PATH];
	sLadderPath(szTmp, true);
	sLadderPath(szPath, false);
	FILE* f = fopen(szTmp, "wb");
	if (!f)
		return;
	std::map<unsigned long, std::vector<KJx2LadderEntry> >::iterator it;
	for (it = s_LadderMap.begin(); it != s_LadderMap.end(); ++it)
	{
		if (it->second.empty())
			continue;
		fprintf(f, "L %lu\n", it->first);
		for (size_t i = 0; i < it->second.size(); i++)
		{
			KJx2LadderEntry& e = it->second[i];
			fprintf(f, "E %I64d %d %d %d %s\n", e.i64Value, e.nSect,
				(int)e.btGender, (int)e.btType, e.szName);
		}
	}
	fclose(f);
	MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
#ifdef _SERVER
	KGameKV::PutFile(JX2LADDER_KV_NS, "file", szPath, true);
#endif
}

// Ladder_NewLadder(id, szName, nValue [,nType][,nSect][,nGender]) - >=3 tham so, tra 0
int LuaLadder_NewLadder(Lua_State* L)
{
	int nTop = Lua_GetTopIndex(L);
	if (nTop < 3 || !Lua_IsNumber(L, 1) || !Lua_IsString(L, 2) || !Lua_IsNumber(L, 3))
		return 0;
	unsigned long uId = (unsigned long)Lua_ValueToNumber(L, 1);
	const char* szName = Lua_ValueToString(L, 2);
	if (uId <= JX2LADDER_MIN_ID || !szName || !szName[0])
		return 0;
	sLadderLoad();
	KJx2LadderEntry e;
	memset(&e, 0, sizeof(e));
	strncpy(e.szName, szName, 31);
	e.i64Value = (__int64)Lua_ValueToNumber(L, 3);
	e.btType = (nTop >= 4 && Lua_IsNumber(L, 4)) ? (unsigned char)(int)Lua_ValueToNumber(L, 4) : 0;
	e.nSect = (nTop >= 5 && Lua_IsNumber(L, 5)) ? (int)Lua_ValueToNumber(L, 5) : -1;
	e.btGender = (nTop >= 6 && Lua_IsNumber(L, 6)) ? (unsigned char)(int)Lua_ValueToNumber(L, 6) : 0;

	std::vector<KJx2LadderEntry>& tb = s_LadderMap[uId];
	// upsert theo ten
	size_t i;
	for (i = 0; i < tb.size(); i++)
	{
		if (strncmp(tb[i].szName, e.szName, 31) == 0)
		{
			tb[i] = e;
			break;
		}
	}
	if (i == tb.size())
		tb.push_back(e);
	// sap giam dan theo value (on dinh - giu thu tu cu khi bang diem)
	for (size_t a = 1; a < tb.size(); a++)
	{
		KJx2LadderEntry t = tb[a];
		int b = (int)a - 1;
		while (b >= 0 && tb[b].i64Value < t.i64Value)
		{
			tb[b + 1] = tb[b];
			b--;
		}
		tb[b + 1] = t;
	}
	if (tb.size() > JX2LADDER_TOP)
		tb.resize(JX2LADDER_TOP);
	sLadderSave();
	return 0;
}

int LuaLadder_ClearLadder(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1))
		return 0;
	unsigned long uId = (unsigned long)Lua_ValueToNumber(L, 1);
	if (uId <= JX2LADDER_MIN_ID)
		return 0;
	sLadderLoad();
	std::map<unsigned long, std::vector<KJx2LadderEntry> >::iterator it = s_LadderMap.find(uId);
	if (it != s_LadderMap.end())
	{
		s_LadderMap.erase(it);
		sLadderSave();
	}
	return 0;
}

// tra 4 gia tri; o trong -> ("", 0, -1, 0) - CHUOI RONG chu KHONG nil
// (ladderfunlib.lua:20 kiem szName ~= nil luon dung - hanh vi goc)
int LuaLadder_GetLadderInfo(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
		return 0;
	unsigned long uId = (unsigned long)Lua_ValueToNumber(L, 1);
	int nRank = (int)Lua_ValueToNumber(L, 2);
	// goc: rank ngoai 1..10 la FAIL (0 gia tri) - chi o TRONG dai moi tra ("",0,-1,0)
	if (nRank < 1 || nRank > JX2LADDER_TOP || uId == 0)
		return 0;
	sLadderLoad();
	if (uId > 0 && nRank >= 1 && nRank <= JX2LADDER_TOP)
	{
		std::map<unsigned long, std::vector<KJx2LadderEntry> >::iterator it = s_LadderMap.find(uId);
		if (it != s_LadderMap.end() && (size_t)nRank <= it->second.size())
		{
			KJx2LadderEntry& e = it->second[nRank - 1];
			Lua_PushString(L, e.szName);
			Lua_PushNumber(L, (double)e.i64Value);
			Lua_PushNumber(L, e.nSect);
			Lua_PushNumber(L, e.btGender);
			return 4;
		}
	}
	Lua_PushString(L, (char*)"");
	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, -1);
	Lua_PushNumber(L, 0);
	return 4;
}

//////////////////////////////////////////////////////////////////////
// 3) GlbValue - kho int toan cuc trong RAM (nhu goc; khoa league_cityinfo
//    duoc dat lai moi lan boot boi buildAllCityInfoLeague)
//////////////////////////////////////////////////////////////////////
static std::map<int, int>	s_GlbValueMap;

int LuaSetGlbValue(Lua_State* L)
{
	if (Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		s_GlbValueMap[(int)Lua_ValueToNumber(L, 1)] = (int)Lua_ValueToNumber(L, 2);
		Lua_PushNumber(L, 1);
	}
	else
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetGlbValue(Lua_State* L)
{
	int nVal = 0;
	if (Lua_IsNumber(L, 1))
	{
		std::map<int, int>::iterator it = s_GlbValueMap.find((int)Lua_ValueToNumber(L, 1));
		if (it != s_GlbValueMap.end())
			nVal = it->second;
	}
	Lua_PushNumber(L, nVal);
	return 1;
}

#endif // _SERVER
