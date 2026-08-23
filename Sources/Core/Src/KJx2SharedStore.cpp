// KJx2SharedStore.cpp - xem KJx2SharedStore.h. DOT E cong thanh JX2.
// Persist Ladder theo khuon tmp + MoveFileEx REPLACE (KIniFile::Save cat trang
// file truoc khi ghi nen KHONG dung cho du lieu song - phan bien E ky thuat F6).

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KGameKV.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KJx2SharedStore.h"
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

// [PORT5 23/08] RemoteExecute (Linux GS 0x08100740 / relay 0x0810363A): RPC GS<->relay qua
// ObjBuffer. Du an 1 GS khong relay -> thuc thi TAI CHO dong bo: fn(hParam, hRes, 0) trong
// state cua szScript (co remap \script\lib -> \scriptjx2\lib nhu Include); co callback ->
// cb(nCbParam, hRes) trong state DANG GOI; dwGameSvrId (tham so 6) bo qua. Caller tu
// OB_Release hParam; hRes do ham nay cap va huy.
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
		g_DebugLog((LPSTR)"[PORT5] RemoteExecute: script chua nap %.128s", szLow);
		return 0;
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
