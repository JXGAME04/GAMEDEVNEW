//////////////////////////////////////////////////////////////////////////////
// RelayScript.cpp  (S3Relay)  [RELAYHT 06/09]  -- xem RelayScript.h
//
// Cach nap kich ban GIONG Core: moi tep .lua mot Lua_State rieng; Include nap
// tep khac VAO CHINH state dang chay; DynamicExecute dung lua_dostring giong
// Core\Src\ScriptFuns.cpp LuaDynamicExecute (da chay that voi shim Lua 5.4).
//
// LUU Y THU TU: dat con tro script vao bang TRUOC khi chay than chunk, neu
// khong hai tep Include lan nhau se nap vo han.
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "DoScript.h"
#include "RelayScript.h"
#include <map>
#include <set>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////////
// 0) Tien ich
//////////////////////////////////////////////////////////////////////////////
static BOOL		s_bReady = FALSE;

// Chuan hoa duong dan: ha chu thuong, '/' -> '\\', bao dam co '\\' dau chuoi.
static void RsNormalize(const char* szIn, std::string& strOut)
{
	char szBuf[MAX_PATH];
	int n = 0;
	if (szIn[0] != '\\' && szIn[0] != '/')
		szBuf[n++] = '\\';
	for (int i = 0; szIn[i] && n < MAX_PATH - 1; i++)
	{
		char c = szIn[i];
		if (c == '/')
			c = '\\';
		else if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		szBuf[n++] = c;
	}
	szBuf[n] = 0;
	strOut = szBuf;
}

// Doc ca tep vao bo nho. Nguoi goi phai free().
static char* RsReadFile(const std::string& strKey, int* pnLen)
{
	char szPath[MAX_PATH * 2];
	_snprintf(szPath, sizeof(szPath) - 1, ".%s", strKey.c_str());
	szPath[sizeof(szPath) - 1] = 0;

	FILE* f = fopen(szPath, "rb");
	if (!f)
		return NULL;
	fseek(f, 0, SEEK_END);
	long lLen = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (lLen <= 0 || lLen > 4 * 1024 * 1024)
	{
		fclose(f);
		return NULL;
	}
	char* pBuf = (char*)malloc(lLen + 4);
	if (!pBuf)
	{
		fclose(f);
		return NULL;
	}
	size_t nRead = fread(pBuf, 1, lLen, f);
	fclose(f);
	pBuf[nRead] = 0;
	if (pnLen)
		*pnLen = (int)nRead;
	return pBuf;
}

//////////////////////////////////////////////////////////////////////////////
// 1) ObjBuffer -- dinh dang PHAI giong Core\Src\KJx2SharedStore.cpp
//////////////////////////////////////////////////////////////////////////////
struct RS_OBJBUF
{
	BYTE	Buf[RS_OB_BUF_SIZE];
	int		nWrite;
	int		nRead;
};

static std::map<int, RS_OBJBUF*>	s_mapOB;
static int							s_nOBNext = 0;

int RelayOB_Alloc()
{
	RS_OBJBUF* p = new RS_OBJBUF;
	if (!p)
		return 0;
	p->nWrite = 0;
	p->nRead = 0;
	s_nOBNext++;
	if (s_nOBNext <= 0)
		s_nOBNext = 1;
	s_mapOB[s_nOBNext] = p;
	return s_nOBNext;
}

void RelayOB_Free(int nHandle)
{
	std::map<int, RS_OBJBUF*>::iterator it = s_mapOB.find(nHandle);
	if (it == s_mapOB.end())
		return;
	delete it->second;
	s_mapOB.erase(it);
}

static RS_OBJBUF* RsOBFind(int nHandle)
{
	std::map<int, RS_OBJBUF*>::iterator it = s_mapOB.find(nHandle);
	return (it == s_mapOB.end()) ? NULL : it->second;
}

static RS_OBJBUF* RsOBArg(Lua_State* L, int nArg)
{
	if (!Lua_IsNumber(L, nArg))
		return NULL;
	return RsOBFind((int)Lua_ValueToNumber(L, nArg));
}

BOOL RelayOB_Peek(int nHandle, const BYTE** ppData, int* pnLen)
{
	RS_OBJBUF* p = RsOBFind(nHandle);
	if (!p)
		return FALSE;
	if (ppData)
		*ppData = p->Buf;
	if (pnLen)
		*pnLen = p->nWrite;
	return TRUE;
}

BOOL RelayOB_Assign(int nHandle, const void* pData, int nLen)
{
	RS_OBJBUF* p = RsOBFind(nHandle);
	if (!p || nLen < 0 || nLen > RS_OB_BUF_SIZE)
		return FALSE;
	if (nLen > 0)
		memcpy(p->Buf, pData, nLen);
	p->nWrite = nLen;
	p->nRead = 0;
	return TRUE;
}

static BOOL RsOBWrite(RS_OBJBUF* p, const void* pData, int nLen)
{
	if (p->nWrite + nLen > RS_OB_BUF_SIZE)
		return FALSE;		// day -> bo qua, giong ban goc (dem cung 4KB)
	memcpy(p->Buf + p->nWrite, pData, nLen);
	p->nWrite += nLen;
	return TRUE;
}

static BOOL RsOBRead(RS_OBJBUF* p, void* pData, int nLen)
{
	if (p->nRead + nLen > p->nWrite)
		return FALSE;
	memcpy(pData, p->Buf + p->nRead, nLen);
	p->nRead += nLen;
	return TRUE;
}

static int LuaRS_OB_Create(Lua_State* L)
{
	Lua_PushNumber(L, RelayOB_Alloc());
	return 1;
}

static int LuaRS_OB_Release(Lua_State* L)
{
	if (Lua_IsNumber(L, 1))
		RelayOB_Free((int)Lua_ValueToNumber(L, 1));
	return 0;
}

static int LuaRS_OB_Clear(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	if (p)
	{
		p->nWrite = 0;
		p->nRead = 0;
	}
	return 0;
}

static int LuaRS_OB_IsEmpty(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	// handle sai cung tinh la RONG (= 1), giong ban goc
	Lua_PushNumber(L, (p == NULL || p->nRead >= p->nWrite) ? 1 : 0);
	return 1;
}

static int LuaRS_OB_Copy(Lua_State* L)
{
	RS_OBJBUF* pDst = RsOBArg(L, 1);
	RS_OBJBUF* pSrc = RsOBArg(L, 2);
	if (!pDst || !pSrc)
		return 0;
	memcpy(pDst->Buf, pSrc->Buf, pSrc->nWrite);
	pDst->nWrite = pSrc->nWrite;
	pDst->nRead = pSrc->nRead;
	return 0;
}

static int LuaRS_OB_Append(Lua_State* L)
{
	RS_OBJBUF* pDst = RsOBArg(L, 1);
	RS_OBJBUF* pSrc = RsOBArg(L, 2);
	if (!pDst || !pSrc)
		return 0;
	int nLeft = pSrc->nWrite - pSrc->nRead;
	if (nLeft > 0)
		RsOBWrite(pDst, pSrc->Buf + pSrc->nRead, nLeft);
	return 0;
}

static int LuaRS_OB_PushByte(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	if (!p || !Lua_IsNumber(L, 2))
		return 0;
	BYTE b = (BYTE)(int)Lua_ValueToNumber(L, 2);
	RsOBWrite(p, &b, 1);
	return 0;
}

static int LuaRS_OB_PopByte(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	BYTE b = 0;
	if (!p || !RsOBRead(p, &b, 1))
		return 0;
	Lua_PushNumber(L, b);
	return 1;
}

static int LuaRS_OB_PushInt(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	if (!p || !Lua_IsNumber(L, 2))
		return 0;
	int n = (int)Lua_ValueToNumber(L, 2);
	RsOBWrite(p, &n, sizeof(int));
	return 0;
}

static int LuaRS_OB_PopInt(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	int n = 0;
	if (!p || !RsOBRead(p, &n, sizeof(int)))
		return 0;
	Lua_PushNumber(L, n);
	return 1;
}

static int LuaRS_OB_PushDouble(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	if (!p || !Lua_IsNumber(L, 2))
		return 0;
	double d = (double)Lua_ValueToNumber(L, 2);
	RsOBWrite(p, &d, sizeof(double));
	return 0;
}

static int LuaRS_OB_PopDouble(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	double d = 0;
	if (!p || !RsOBRead(p, &d, sizeof(double)))
		return 0;
	Lua_PushNumber(L, d);
	return 1;
}

static int LuaRS_OB_PushString(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	if (!p || !Lua_IsString(L, 2))
		return 0;
	const char* sz = Lua_ValueToString(L, 2);
	int nLen = (int)strlen(sz);
	if (nLen > RS_OB_BUF_SIZE - 8)
		nLen = RS_OB_BUF_SIZE - 8;
	RsOBWrite(p, &nLen, sizeof(int));
	if (nLen > 0)
		RsOBWrite(p, sz, nLen);
	return 0;
}

static int LuaRS_OB_PopString(Lua_State* L)
{
	RS_OBJBUF* p = RsOBArg(L, 1);
	int nLen = 0;
	if (!p || !RsOBRead(p, &nLen, sizeof(int)))
		return 0;
	if (nLen < 0 || nLen > RS_OB_BUF_SIZE)
		return 0;
	char szTmp[RS_OB_BUF_SIZE + 1];
	if (nLen > 0 && !RsOBRead(p, szTmp, nLen))
		return 0;
	szTmp[nLen] = 0;
	Lua_PushString(L, szTmp);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// 2) Bo kich ban
//////////////////////////////////////////////////////////////////////////////
static std::map<std::string, KLuaScript*>		s_mapScript;
static std::map<Lua_State*, std::string>		s_mapStateFile;
static std::map<Lua_State*, KLuaScript*>		s_mapStateScript;
static std::map<Lua_State*, std::set<std::string> >	s_mapIncluded;
static std::vector<TLua_Funcs>					s_vecFuns;

BOOL RelayScript_AddFuncTable(TLua_Funcs* pFuns, int nCount)
{
	if (!pFuns || nCount <= 0)
		return FALSE;
	for (int i = 0; i < nCount; i++)
		s_vecFuns.push_back(pFuns[i]);
	return TRUE;
}

static KLuaScript* RsFindByState(Lua_State* L)
{
	std::map<Lua_State*, KLuaScript*>::iterator it = s_mapStateScript.find(L);
	return (it == s_mapStateScript.end()) ? NULL : it->second;
}

static KLuaScript* RsLoad(const std::string& strKey)
{
	int nLen = 0;
	char* pBuf = RsReadFile(strKey, &nLen);
	if (!pBuf)
	{
		rTRACE("[RelayScript] khong mo duoc [%s]", strKey.c_str());
		return NULL;
	}

	if ((int)s_mapScript.size() >= RS_MAX_SCRIPT)
	{
		rTRACE("[RelayScript] TRAN so kich ban (%d), bo qua [%s]", RS_MAX_SCRIPT, strKey.c_str());
		free(pBuf);
		return NULL;
	}

	KLuaScript* pScript = new KLuaScript();
	if (!pScript || !pScript->Init())
	{
		rTRACE("[RelayScript] Init() that bai [%s]", strKey.c_str());
		if (pScript)
			delete pScript;
		free(pBuf);
		return NULL;
	}

	// PHAI dang ky ham TRUOC khi chay than chunk (bai hoc 06/09: thieu
	// RegisterFunctions -> Include = nil -> than chunk chet im lang)
	if (!s_vecFuns.empty())
		pScript->RegisterFunctions(&s_vecFuns[0], (int)s_vecFuns.size());
	pScript->SetScriptName((LPSTR)strKey.c_str());

	// dat vao bang TRUOC khi chay: chong Include vong tron
	s_mapScript[strKey] = pScript;
	s_mapStateScript[pScript->m_LuaState] = pScript;
	s_mapStateFile[pScript->m_LuaState] = strKey;

	if (!pScript->LoadBuffer((PBYTE)pBuf, (DWORD)nLen))
		rTRACE("[RelayScript] bien dich LOI [%s]", strKey.c_str());
	else if (!pScript->ExecuteCode())
		rTRACE("[RelayScript] chay than chunk LOI [%s]", strKey.c_str());

	free(pBuf);
	return pScript;
}

KLuaScript* RelayScript_Get(const char* szFile, BOOL bLoadIfNeed)
{
	if (!szFile || !szFile[0])
		return NULL;
	std::string strKey;
	RsNormalize(szFile, strKey);
	std::map<std::string, KLuaScript*>::iterator it = s_mapScript.find(strKey);
	if (it != s_mapScript.end())
		return it->second;
	if (!bLoadIfNeed)
		return NULL;
	return RsLoad(strKey);
}

// [RELAYHT 06/09 VA2] s_mapStateFile da co san tu luc nap, chi thieu duong tra ra.
const char* RelayScript_FileOfState(Lua_State* L)
{
	std::map<Lua_State*, std::string>::iterator it = s_mapStateFile.find(L);
	return (it == s_mapStateFile.end()) ? "" : it->second.c_str();
}

BOOL RelayScript_CallVoid(const char* szFile, const char* szFunc)
{
	KLuaScript* pScript = RelayScript_Get(szFile, TRUE);
	if (!pScript || !szFunc || !szFunc[0])
		return FALSE;
	int nTop = 0;
	pScript->SafeCallBegin(&nTop);
	BOOL bRet = pScript->CallFunction((LPSTR)szFunc, 0, (LPSTR)"");
	pScript->SafeCallEnd(nTop);
	return bRet;
}

int RelayScript_ReloadAll()
{
	std::vector<std::string> vecKey;
	std::map<std::string, KLuaScript*>::iterator it;
	for (it = s_mapScript.begin(); it != s_mapScript.end(); ++it)
		vecKey.push_back(it->first);

	RelayScript_Uninit();
	s_bReady = TRUE;

	int nOK = 0;
	for (size_t i = 0; i < vecKey.size(); i++)
	{
		if (RsLoad(vecKey[i]))
			nOK++;
	}
	rTRACE("[RelayScript] nap lai %d/%d tep", nOK, (int)vecKey.size());
	return nOK;
}

BOOL RelayScript_IsReady()
{
	return s_bReady;
}

//////////////////////////////////////////////////////////////////////////////
// 3) Ham Lua chung
//////////////////////////////////////////////////////////////////////////////
static int LuaRS_OutputMsg(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	const char* sz = Lua_IsString(L, 1) ? Lua_ValueToString(L, 1) : NULL;
	if (sz)
		rTRACE("[Script] %.480s", sz);
	return 0;
}

static int LuaRS_Include(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;
	std::string strKey;
	RsNormalize(Lua_ValueToString(L, 1), strKey);

	KLuaScript* pCur = RsFindByState(L);
	if (!pCur)
		return 0;

	std::set<std::string>& setDone = s_mapIncluded[L];
	if (setDone.find(strKey) != setDone.end())
		return 0;		// da include roi -> bo qua (chong lap)
	setDone.insert(strKey);

	int nLen = 0;
	char* pBuf = RsReadFile(strKey, &nLen);
	if (!pBuf)
	{
		rTRACE("[RelayScript] Include khong mo duoc [%s]", strKey.c_str());
		return 0;
	}
	if (pCur->LoadBuffer((PBYTE)pBuf, (DWORD)nLen))
		pCur->ExecuteCode();
	else
		rTRACE("[RelayScript] Include bien dich LOI [%s]", strKey.c_str());
	free(pBuf);
	return 0;
}

// Ban Linux dung IncludeLib("FILESYS") de nap thu vien ham C++ theo ten. Ben ta
// moi thu vien deu dang ky san cho MOI script nen chi ghi nhan roi tra ve 1.
static int LuaRS_IncludeLib(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}

static int LuaRS_LoadScript(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;
	KLuaScript* p = RelayScript_Get(Lua_ValueToString(L, 1), TRUE);
	Lua_PushNumber(L, p ? 1 : 0);
	return 1;
}

// DynamicExecute(szScript, szFun, ...) -- giong Core\Src\ScriptFuns.cpp
static int LuaRS_DynamicExecute(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2)
		return 0;
	const char* szScript = Lua_ValueToString(L, 1);
	const char* szFun = Lua_ValueToString(L, 2);
	if (!szScript || !szFun)
		return 0;

	KLuaScript* pScript = RelayScript_Get(szScript, TRUE);
	if (!pScript)
	{
		rTRACE("[RelayScript] DynamicExecute: khong nap duoc %.128s", szScript);
		return 0;
	}

	char szCall[4096];
	int nPos = 0;
	nPos += _snprintf(szCall + nPos, sizeof(szCall) - nPos - 1, "%s(", szFun);
	for (int i = 3; i <= nParamNum && nPos < (int)sizeof(szCall) - 2200; i++)
	{
		if (i > 3)
			szCall[nPos++] = ',';
		if (Lua_IsNumber(L, i))
			nPos += _snprintf(szCall + nPos, sizeof(szCall) - nPos - 1, "%.0f", (double)Lua_ValueToNumber(L, i));
		else if (Lua_IsString(L, i))
		{
			// chuoi dai [[...]] chiu duoc dau nhay kep; vo hieu "]]" va "[[" ben trong
			char szArgTmp[2049];
			strncpy(szArgTmp, Lua_ValueToString(L, i), sizeof(szArgTmp) - 1);
			szArgTmp[sizeof(szArgTmp) - 1] = 0;
			for (char* q = strstr(szArgTmp, "]]"); q; q = strstr(q + 2, "]]"))
				q[1] = ' ';
			for (char* q2 = strstr(szArgTmp, "[["); q2; q2 = strstr(q2 + 2, "[["))
				q2[1] = ' ';
			int nTailFix = (szArgTmp[0] && szArgTmp[strlen(szArgTmp) - 1] == ']') ? 1 : 0;
			nPos += _snprintf(szCall + nPos, sizeof(szCall) - nPos - 1,
				nTailFix ? "[[%s ]]" : "[[%s]]", szArgTmp);
		}
		else
			nPos += _snprintf(szCall + nPos, sizeof(szCall) - nPos - 1, "nil");
	}
	szCall[nPos++] = ')';
	szCall[nPos] = 0;

	int nTop = 0;
	pScript->SafeCallBegin(&nTop);
	if (lua_dostring(pScript->m_LuaState, szCall) != 0)
		rTRACE("[RelayScript] DynamicExecute LOI: %.128s -> %.200s", szScript, szCall);
	pScript->SafeCallEnd(nTop);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// 4) Thoi gian / so ngau nhien / tep
//////////////////////////////////////////////////////////////////////////////
static int LuaRS_GetCurrentDate(Lua_State* L)
{
	time_t t = time(NULL);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	Lua_PushNumber(L, p->tm_year + 1900);
	Lua_PushNumber(L, p->tm_mon + 1);
	Lua_PushNumber(L, p->tm_mday);
	Lua_PushNumber(L, p->tm_wday);
	return 4;
}

static int LuaRS_GetCurrentTime(Lua_State* L)
{
	time_t t = time(NULL);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	Lua_PushNumber(L, p->tm_hour);
	Lua_PushNumber(L, p->tm_min);
	Lua_PushNumber(L, p->tm_sec);
	return 3;
}

static int LuaRS_GetSysCurrentTime(Lua_State* L)
{
	Lua_PushNumber(L, (double)time(NULL));
	return 1;
}

// Tm2Time(year, mon, day, hour, min, sec) -> time_t
static int LuaRS_Tm2Time(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 6)
		return 0;
	struct tm tmv;
	memset(&tmv, 0, sizeof(tmv));
	tmv.tm_year = (int)Lua_ValueToNumber(L, 1) - 1900;
	tmv.tm_mon = (int)Lua_ValueToNumber(L, 2) - 1;
	tmv.tm_mday = (int)Lua_ValueToNumber(L, 3);
	tmv.tm_hour = (int)Lua_ValueToNumber(L, 4);
	tmv.tm_min = (int)Lua_ValueToNumber(L, 5);
	tmv.tm_sec = (int)Lua_ValueToNumber(L, 6);
	tmv.tm_isdst = -1;
	Lua_PushNumber(L, (double)mktime(&tmv));
	return 1;
}

// Time2Tm(time_t) -> year, mon, day, hour, min, sec, wday
static int LuaRS_Time2Tm(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
		return 0;
	time_t t = (time_t)Lua_ValueToNumber(L, 1);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	Lua_PushNumber(L, p->tm_year + 1900);
	Lua_PushNumber(L, p->tm_mon + 1);
	Lua_PushNumber(L, p->tm_mday);
	Lua_PushNumber(L, p->tm_hour);
	Lua_PushNumber(L, p->tm_min);
	Lua_PushNumber(L, p->tm_sec);
	Lua_PushNumber(L, p->tm_wday);
	return 7;
}

// FormatTime2Number([time_t]) -> yyyymmddhhmm (giong "%Y%m%d%H%M" ban Linux)
static int LuaRS_FormatTime2Number(Lua_State* L)
{
	time_t t = (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1)) ? (time_t)Lua_ValueToNumber(L, 1) : time(NULL);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	double d = (p->tm_year + 1900) * 100000000.0 + (p->tm_mon + 1) * 1000000.0
		+ p->tm_mday * 10000.0 + p->tm_hour * 100.0 + p->tm_min;
	Lua_PushNumber(L, d);
	return 1;
}

// FormatTime2String([time_t]) -> "yyyy-mm-dd hh:mm:ss"
static int LuaRS_FormatTime2String(Lua_State* L)
{
	time_t t = (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1)) ? (time_t)Lua_ValueToNumber(L, 1) : time(NULL);
	struct tm* p = localtime(&t);
	if (!p)
		return 0;
	char szBuf[64];
	_snprintf(szBuf, sizeof(szBuf) - 1, "%04d-%02d-%02d %02d:%02d:%02d",
		p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	szBuf[sizeof(szBuf) - 1] = 0;
	Lua_PushString(L, szBuf);
	return 1;
}

// Random(n) -> 1..n ; Random(a,b) -> a..b
static int LuaRS_Random(Lua_State* L)
{
	int nParam = Lua_GetTopIndex(L);
	int nMin = 1, nMax = 1;
	if (nParam == 1)
		nMax = (int)Lua_ValueToNumber(L, 1);
	else if (nParam >= 2)
	{
		nMin = (int)Lua_ValueToNumber(L, 1);
		nMax = (int)Lua_ValueToNumber(L, 2);
	}
	else
		return 0;
	if (nMax < nMin)
	{
		int t = nMin; nMin = nMax; nMax = t;
	}
	// rand() chi 15 bit -> ghep 2 lan cho dai so lon
	unsigned int r = ((unsigned int)rand() << 15) ^ (unsigned int)rand();
	Lua_PushNumber(L, nMin + (int)(r % (unsigned int)(nMax - nMin + 1)));
	return 1;
}

static int LuaRS_WriteStringToFile(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2))
		return 0;
	const char* szFile = Lua_ValueToString(L, 1);
	const char* szText = Lua_ValueToString(L, 2);
	char szPath[MAX_PATH * 2];
	if (szFile[0] == '\\' || szFile[0] == '/')
		_snprintf(szPath, sizeof(szPath) - 1, ".%s", szFile);
	else
		_snprintf(szPath, sizeof(szPath) - 1, "%s", szFile);
	szPath[sizeof(szPath) - 1] = 0;
	FILE* f = fopen(szPath, "a+b");
	if (!f)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	fwrite(szText, 1, strlen(szText), f);
	fwrite("\r\n", 1, 2, f);
	fclose(f);
	Lua_PushNumber(L, 1);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// 5) Bien toan cum trong bo nho relay (Get/Set/DelGblInt|Str)
//    Chi song trong mot lan chay relay. Muon ben vung thi dung ShareData.
//////////////////////////////////////////////////////////////////////////////
static std::map<std::string, double>		s_mapGblInt;
static std::map<std::string, std::string>	s_mapGblStr;

static int LuaRS_GetGblInt(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	std::map<std::string, double>::iterator it = s_mapGblInt.find(Lua_ValueToString(L, 1));
	Lua_PushNumber(L, (it == s_mapGblInt.end()) ? 0 : it->second);
	return 1;
}

static int LuaRS_SetGblInt(Lua_State* L)
{
	if (!Lua_IsString(L, 1) || !Lua_IsNumber(L, 2))
		return 0;
	s_mapGblInt[Lua_ValueToString(L, 1)] = (double)Lua_ValueToNumber(L, 2);
	return 0;
}

static int LuaRS_DelGblInt(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	s_mapGblInt.erase(Lua_ValueToString(L, 1));
	return 0;
}

static int LuaRS_GetGblStr(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	std::map<std::string, std::string>::iterator it = s_mapGblStr.find(Lua_ValueToString(L, 1));
	Lua_PushString(L, (it == s_mapGblStr.end()) ? "" : (char*)it->second.c_str());
	return 1;
}

static int LuaRS_SetGblStr(Lua_State* L)
{
	if (!Lua_IsString(L, 1) || !Lua_IsString(L, 2))
		return 0;
	s_mapGblStr[Lua_ValueToString(L, 1)] = Lua_ValueToString(L, 2);
	return 0;
}

static int LuaRS_DelGblStr(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
		return 0;
	s_mapGblStr.erase(Lua_ValueToString(L, 1));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// 6) Bang ham
//////////////////////////////////////////////////////////////////////////////
TLua_Funcs g_RelayScriptFuns[] =
{
	// -- kich ban
	{ "OutputMsg",			LuaRS_OutputMsg },
	{ "Include",			LuaRS_Include },
	{ "IncludeLib",			LuaRS_IncludeLib },
	{ "LoadScript",			LuaRS_LoadScript },
	{ "DynamicExecute",		LuaRS_DynamicExecute },
	// -- thoi gian
	{ "GetCurrentDate",		LuaRS_GetCurrentDate },
	{ "GetCurrentTime",		LuaRS_GetCurrentTime },
	{ "GetSysCurrentTime",	LuaRS_GetSysCurrentTime },
	{ "Tm2Time",			LuaRS_Tm2Time },
	{ "Time2Tm",			LuaRS_Time2Tm },
	{ "FormatTime2Number",	LuaRS_FormatTime2Number },
	{ "FormatTime2String",	LuaRS_FormatTime2String },
	// -- tien ich
	{ "Random",				LuaRS_Random },
	{ "WriteStringToFile",	LuaRS_WriteStringToFile },
	// -- bien toan cum
	{ "GetGblInt",			LuaRS_GetGblInt },
	{ "SetGblInt",			LuaRS_SetGblInt },
	{ "DelGblInt",			LuaRS_DelGblInt },
	{ "GetGblStr",			LuaRS_GetGblStr },
	{ "SetGblStr",			LuaRS_SetGblStr },
	{ "DelGblStr",			LuaRS_DelGblStr },
	// -- ObjBuffer
	{ "OB_Create",			LuaRS_OB_Create },
	{ "OB_Release",			LuaRS_OB_Release },
	{ "OB_Clear",			LuaRS_OB_Clear },
	{ "OB_IsEmpty",			LuaRS_OB_IsEmpty },
	{ "OB_Copy",			LuaRS_OB_Copy },
	{ "OB_Append",			LuaRS_OB_Append },
	{ "OB_PushByte",		LuaRS_OB_PushByte },
	{ "OB_PopByte",			LuaRS_OB_PopByte },
	{ "OB_PushInt",			LuaRS_OB_PushInt },
	{ "OB_PopInt",			LuaRS_OB_PopInt },
	{ "OB_PushDouble",		LuaRS_OB_PushDouble },
	{ "OB_PopDouble",		LuaRS_OB_PopDouble },
	{ "OB_PushString",		LuaRS_OB_PushString },
	{ "OB_PopString",		LuaRS_OB_PopString },
};

int g_GetRelayScriptFunNum()
{
	return sizeof(g_RelayScriptFuns) / sizeof(TLua_Funcs);
}

//////////////////////////////////////////////////////////////////////////////
// 7) Khoi dong / dong
//////////////////////////////////////////////////////////////////////////////
BOOL RelayScript_Init()
{
	if (s_bReady)
		return TRUE;

	// bang ham cua chinh module nay
	RelayScript_AddFuncTable(g_RelayScriptFuns, g_GetRelayScriptFunNum());

	// bang ham co san cua relay (GlobalExecute / GameExecute / Msg2IP / GetIP)
	RelayScript_AddFuncTable(GameScriptFuns, g_GetGameScriptFunNum());

	srand((unsigned int)time(NULL));
	s_bReady = TRUE;
	rTRACE("[RelayScript] san sang, %d ham Lua", (int)s_vecFuns.size());
	return TRUE;
}

void RelayScript_Uninit()
{
	std::map<std::string, KLuaScript*>::iterator it;
	for (it = s_mapScript.begin(); it != s_mapScript.end(); ++it)
	{
		if (it->second)
		{
			it->second->Exit();
			delete it->second;
		}
	}
	s_mapScript.clear();
	s_mapStateScript.clear();
	s_mapStateFile.clear();
	s_mapIncluded.clear();

	std::map<int, RS_OBJBUF*>::iterator ib;
	for (ib = s_mapOB.begin(); ib != s_mapOB.end(); ++ib)
		delete ib->second;
	s_mapOB.clear();

	s_bReady = FALSE;
}
