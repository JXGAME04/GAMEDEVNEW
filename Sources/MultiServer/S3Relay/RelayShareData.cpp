//////////////////////////////////////////////////////////////////////////////
// RelayShareData.cpp  (S3Relay)  [RELAYHT 06/09]  -- xem RelayShareData.h
//
// Nhu ban Linux: nap HET vao bo nho luc khoi dong, sau do doc = bo nho, ghi =
// bo nho + MySQL (REPLACE INTO). Du lieu la BLOB nhi phan nen moi chuoi deu qua
// mysql_real_escape_string (co do dai, chiu duoc byte 0), khong noi chuoi tho.
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "RelayScript.h"
#include "RelayShareData.h"
#include <mysql.h>
#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define SD_INI			".\\DataBase.ini"		// cung tep/section voi DBTable_MySQL.cpp
#define SD_SECTION		"relaydb"
#define SD_TABLE		"relay_sharedata"
#define SD_STR			128

//////////////////////////////////////////////////////////////////////////////
// Trang thai
//////////////////////////////////////////////////////////////////////////////
struct SD_KEY
{
	std::string	strKey;
	DWORD		dwP1;
	DWORD		dwP2;

	bool operator < (const SD_KEY& o) const
	{
		if (strKey != o.strKey)	return strKey < o.strKey;
		if (dwP1 != o.dwP1)		return dwP1 < o.dwP1;
		return dwP2 < o.dwP2;
	}
};

struct SD_VAL
{
	int		nLen;
	BYTE	Data[SD_DATA_MAX];
};

static MYSQL*						s_conn = NULL;
static int							s_enable = 0;
static char							s_dbHost[SD_STR], s_dbUser[SD_STR], s_dbPass[SD_STR], s_dbName[SD_STR];
static unsigned						s_dbPort = 3306;
static std::map<SD_KEY, SD_VAL*>	s_mapData;

static void SdLog(const char* fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = 0;
	rTRACE("[ShareData] %s", buf);
}

static void SdIniStr(const char* key, const char* def, char* out, int cap)
{
	GetPrivateProfileStringA(SD_SECTION, key, def, out, cap - 1, SD_INI);
	out[cap - 1] = 0;
	int n = (int)strlen(out);
	while (n > 0 && (out[n - 1] == ' ' || out[n - 1] == '\t' || out[n - 1] == '\r' || out[n - 1] == '\n'))
		out[--n] = 0;
	int i = 0;
	while (out[i] == ' ' || out[i] == '\t') i++;
	if (i) memmove(out, out + i, strlen(out + i) + 1);
}

static int SdConnect()
{
	if (s_conn) { mysql_close(s_conn); s_conn = NULL; }
	s_conn = mysql_init(NULL);
	if (!s_conn) { SdLog("mysql_init that bai"); return 0; }
	mysql_options(s_conn, MYSQL_SET_CHARSET_NAME, "latin1");
	unsigned int to = 5;
	mysql_options(s_conn, MYSQL_OPT_CONNECT_TIMEOUT, &to);
	if (!mysql_real_connect(s_conn, s_dbHost, s_dbUser, s_dbPass, s_dbName, s_dbPort, NULL, 0))
	{
		SdLog("mysql_real_connect %s:%u/%s LOI: %s", s_dbHost, s_dbPort, s_dbName, mysql_error(s_conn));
		mysql_close(s_conn);
		s_conn = NULL;
		return 0;
	}
	mysql_autocommit(s_conn, 1);
	return 1;
}

static int SdEnsure()
{
	if (!s_enable) return 0;
	if (s_conn && mysql_ping(s_conn) == 0) return 1;
	SdLog("mat ket noi MySQL -- dang noi lai");
	return SdConnect();
}

static int SdExec(const char* sql, unsigned long nLen)
{
	if (mysql_real_query(s_conn, sql, nLen))
	{
		SdLog("SQL LOI %u: %s | %.80s", mysql_errno(s_conn), mysql_error(s_conn), sql);
		return 0;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
// Bo nho
//////////////////////////////////////////////////////////////////////////////
static void SdMakeKey(SD_KEY& k, const char* szKey, DWORD dwP1, DWORD dwP2)
{
	char szBuf[SD_KEY_MAX + 1];
	strncpy(szBuf, szKey ? szKey : "", SD_KEY_MAX);
	szBuf[SD_KEY_MAX] = 0;
	k.strKey = szBuf;
	k.dwP1 = dwP1;
	k.dwP2 = dwP2;
}

static void SdCacheSet(const char* szKey, DWORD dwP1, DWORD dwP2, const void* pData, int nLen)
{
	SD_KEY k;
	SdMakeKey(k, szKey, dwP1, dwP2);
	std::map<SD_KEY, SD_VAL*>::iterator it = s_mapData.find(k);
	SD_VAL* pv = (it == s_mapData.end()) ? NULL : it->second;
	if (!pv)
	{
		pv = new SD_VAL;
		if (!pv)
			return;
		s_mapData[k] = pv;
	}
	if (nLen > SD_DATA_MAX)
		nLen = SD_DATA_MAX;
	pv->nLen = nLen;
	if (nLen > 0)
		memcpy(pv->Data, pData, nLen);
}

static SD_VAL* SdCacheGet(const char* szKey, DWORD dwP1, DWORD dwP2)
{
	SD_KEY k;
	SdMakeKey(k, szKey, dwP1, dwP2);
	std::map<SD_KEY, SD_VAL*>::iterator it = s_mapData.find(k);
	return (it == s_mapData.end()) ? NULL : it->second;
}

//////////////////////////////////////////////////////////////////////////////
// API C++
//////////////////////////////////////////////////////////////////////////////
BOOL ShareData_Enabled()
{
	return s_enable ? TRUE : FALSE;
}

BOOL ShareData_Set(const char* szKey, DWORD dwP1, DWORD dwP2, const void* pData, int nLen)
{
	if (!szKey || !szKey[0] || nLen < 0 || nLen > SD_DATA_MAX)
		return FALSE;

	SdCacheSet(szKey, dwP1, dwP2, pData, nLen);

	if (!SdEnsure())
		return FALSE;

	char szKeyEsc[SD_KEY_MAX * 2 + 2];
	unsigned long nKeyLen = (unsigned long)strlen(szKey);
	if (nKeyLen > SD_KEY_MAX)
		nKeyLen = SD_KEY_MAX;
	mysql_real_escape_string(s_conn, szKeyEsc, szKey, nKeyLen);

	static char szSql[SD_DATA_MAX * 2 + 512];
	int nPos = _snprintf(szSql, sizeof(szSql) - 1,
		"REPLACE INTO " SD_TABLE " (skey,p1,p2,sdata) VALUES ('%s',%u,%u,'", szKeyEsc, dwP1, dwP2);
	if (nPos <= 0)
		return FALSE;
	if (nLen > 0)
		nPos += (int)mysql_real_escape_string(s_conn, szSql + nPos, (const char*)pData, (unsigned long)nLen);
	nPos += _snprintf(szSql + nPos, sizeof(szSql) - nPos - 1, "')");
	szSql[nPos] = 0;
	return SdExec(szSql, (unsigned long)nPos) ? TRUE : FALSE;
}

int ShareData_Get(const char* szKey, DWORD dwP1, DWORD dwP2, void* pOut, int nCap)
{
	SD_VAL* pv = SdCacheGet(szKey, dwP1, dwP2);
	if (!pv)
		return -1;
	int n = pv->nLen;
	if (n > nCap)
		n = nCap;
	if (n > 0 && pOut)
		memcpy(pOut, pv->Data, n);
	return n;
}

BOOL ShareData_Del(const char* szKey, DWORD dwP1, DWORD dwP2)
{
	if (!szKey || !szKey[0])
		return FALSE;

	SD_KEY k;
	SdMakeKey(k, szKey, dwP1, dwP2);
	std::map<SD_KEY, SD_VAL*>::iterator it = s_mapData.find(k);
	if (it != s_mapData.end())
	{
		delete it->second;
		s_mapData.erase(it);
	}

	if (!SdEnsure())
		return FALSE;
	char szKeyEsc[SD_KEY_MAX * 2 + 2];
	unsigned long nKeyLen = (unsigned long)strlen(szKey);
	if (nKeyLen > SD_KEY_MAX)
		nKeyLen = SD_KEY_MAX;
	mysql_real_escape_string(s_conn, szKeyEsc, szKey, nKeyLen);
	char szSql[512];
	int nPos = _snprintf(szSql, sizeof(szSql) - 1,
		"DELETE FROM " SD_TABLE " WHERE skey='%s' AND p1=%u AND p2=%u", szKeyEsc, dwP1, dwP2);
	szSql[nPos] = 0;
	return SdExec(szSql, (unsigned long)nPos) ? TRUE : FALSE;
}

//////////////////////////////////////////////////////////////////////////////
// Ham Lua
//////////////////////////////////////////////////////////////////////////////
static int LuaSD_OB_SaveShareData(Lua_State* L)
{
	// OB_SaveShareData(handle, szKey, nP1, nP2)
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nHandle = (int)Lua_ValueToNumber(L, 1);
	const char* szKey = Lua_ValueToString(L, 2);
	DWORD dwP1 = (DWORD)(int)Lua_ValueToNumber(L, 3);
	DWORD dwP2 = (DWORD)(int)Lua_ValueToNumber(L, 4);

	const BYTE* pData = NULL;
	int nLen = 0;
	if (!RelayOB_Peek(nHandle, &pData, &nLen))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, ShareData_Set(szKey, dwP1, dwP2, pData, nLen) ? 1 : 0);
	return 1;
}

static int LuaSD_OB_LoadShareData(Lua_State* L)
{
	// OB_LoadShareData(handle, szKey, nP1, nP2)
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsNumber(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nHandle = (int)Lua_ValueToNumber(L, 1);
	const char* szKey = Lua_ValueToString(L, 2);
	DWORD dwP1 = (DWORD)(int)Lua_ValueToNumber(L, 3);
	DWORD dwP2 = (DWORD)(int)Lua_ValueToNumber(L, 4);

	BYTE Tmp[SD_DATA_MAX];
	int nLen = ShareData_Get(szKey, dwP1, dwP2, Tmp, sizeof(Tmp));
	if (nLen < 0)
	{
		// khong co ban ghi -> lam rong bo dem, tra ve 0
		RelayOB_Assign(nHandle, NULL, 0);
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, RelayOB_Assign(nHandle, Tmp, nLen) ? 1 : 0);
	return 1;
}

static int LuaSD_OB_DeleteShareData(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, ShareData_Del(Lua_ValueToString(L, 1),
		(DWORD)(int)Lua_ValueToNumber(L, 2), (DWORD)(int)Lua_ValueToNumber(L, 3)) ? 1 : 0);
	return 1;
}

// SaveStringToSDB(szKey, nP1, nP2, szValue) -- co roi thi KHONG ghi de
// SaveStringToSDBOw(...)                    -- ghi de
static int SdSaveString(Lua_State* L, bool bOverwrite)
{
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 1) || !Lua_IsString(L, 4))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* szKey = Lua_ValueToString(L, 1);
	DWORD dwP1 = (DWORD)(int)Lua_ValueToNumber(L, 2);
	DWORD dwP2 = (DWORD)(int)Lua_ValueToNumber(L, 3);
	const char* szVal = Lua_ValueToString(L, 4);

	if (!bOverwrite && SdCacheGet(szKey, dwP1, dwP2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	// dinh dang: 1 byte kieu ('s') + chuoi
	char szBuf[SD_DATA_MAX];
	int nLen = (int)strlen(szVal);
	if (nLen > SD_DATA_MAX - 2)
		nLen = SD_DATA_MAX - 2;
	szBuf[0] = 's';
	memcpy(szBuf + 1, szVal, nLen);
	Lua_PushNumber(L, ShareData_Set(szKey, dwP1, dwP2, szBuf, nLen + 1) ? 1 : 0);
	return 1;
}

static int LuaSD_SaveStringToSDB(Lua_State* L)		{ return SdSaveString(L, false); }
static int LuaSD_SaveStringToSDBOw(Lua_State* L)	{ return SdSaveString(L, true); }

static int LuaSD_GetStringFromSDB(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsString(L, 1))
		return 0;
	BYTE Tmp[SD_DATA_MAX + 1];
	int nLen = ShareData_Get(Lua_ValueToString(L, 1),
		(DWORD)(int)Lua_ValueToNumber(L, 2), (DWORD)(int)Lua_ValueToNumber(L, 3), Tmp, SD_DATA_MAX);
	if (nLen <= 1 || Tmp[0] != 's')
	{
		Lua_PushString(L, "");
		return 1;
	}
	Tmp[nLen] = 0;
	Lua_PushString(L, (char*)(Tmp + 1));
	return 1;
}

static int SdSaveInteger(Lua_State* L, bool bOverwrite)
{
	if (Lua_GetTopIndex(L) < 4 || !Lua_IsString(L, 1) || !Lua_IsNumber(L, 4))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	const char* szKey = Lua_ValueToString(L, 1);
	DWORD dwP1 = (DWORD)(int)Lua_ValueToNumber(L, 2);
	DWORD dwP2 = (DWORD)(int)Lua_ValueToNumber(L, 3);
	double dVal = (double)Lua_ValueToNumber(L, 4);

	if (!bOverwrite && SdCacheGet(szKey, dwP1, dwP2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char szBuf[1 + sizeof(double)];
	szBuf[0] = 'i';
	memcpy(szBuf + 1, &dVal, sizeof(double));
	Lua_PushNumber(L, ShareData_Set(szKey, dwP1, dwP2, szBuf, sizeof(szBuf)) ? 1 : 0);
	return 1;
}

static int LuaSD_SaveIntegerToSDB(Lua_State* L)		{ return SdSaveInteger(L, false); }
static int LuaSD_SaveIntegerToSDBOw(Lua_State* L)	{ return SdSaveInteger(L, true); }

static int LuaSD_GetIntegerFromSDB(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsString(L, 1))
		return 0;
	BYTE Tmp[SD_DATA_MAX];
	int nLen = ShareData_Get(Lua_ValueToString(L, 1),
		(DWORD)(int)Lua_ValueToNumber(L, 2), (DWORD)(int)Lua_ValueToNumber(L, 3), Tmp, sizeof(Tmp));
	if (nLen != (int)(1 + sizeof(double)) || Tmp[0] != 'i')
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	double d = 0;
	memcpy(&d, Tmp + 1, sizeof(double));
	Lua_PushNumber(L, d);
	return 1;
}

// ClearRecordOnShareDB(szKey, nP1, nP2, bP1Valid, bP2Valid)
// bP1Valid/bP2Valid = 0 -> bo qua tham so do (xoa theo nhom), giong ban Linux.
static int LuaSD_ClearRecordOnShareDB(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char szKey[SD_KEY_MAX + 1];
	strncpy(szKey, Lua_ValueToString(L, 1), SD_KEY_MAX);
	szKey[SD_KEY_MAX] = 0;
	DWORD dwP1 = (DWORD)(int)Lua_ValueToNumber(L, 2);
	DWORD dwP2 = (DWORD)(int)Lua_ValueToNumber(L, 3);
	int bP1 = (Lua_GetTopIndex(L) >= 4) ? (int)Lua_ValueToNumber(L, 4) : 1;
	int bP2 = (Lua_GetTopIndex(L) >= 5) ? (int)Lua_ValueToNumber(L, 5) : 1;

	// gom truoc roi xoa, tranh hong vong lap
	std::vector<SD_KEY> vecDel;
	std::map<SD_KEY, SD_VAL*>::iterator it;
	for (it = s_mapData.begin(); it != s_mapData.end(); ++it)
	{
		if (it->first.strKey != szKey)
			continue;
		if (bP1 && it->first.dwP1 != dwP1)
			continue;
		if (bP2 && it->first.dwP2 != dwP2)
			continue;
		vecDel.push_back(it->first);
	}
	for (size_t i = 0; i < vecDel.size(); i++)
		ShareData_Del(vecDel[i].strKey.c_str(), vecDel[i].dwP1, vecDel[i].dwP2);

	Lua_PushNumber(L, (int)vecDel.size());
	return 1;
}

static int LuaSD_GetRecordCount(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	std::string strKey = Lua_ValueToString(L, 1);
	int nCount = 0;
	std::map<SD_KEY, SD_VAL*>::iterator it;
	for (it = s_mapData.begin(); it != s_mapData.end(); ++it)
	{
		if (it->first.strKey == strKey)
			nCount++;
	}
	Lua_PushNumber(L, nCount);
	return 1;
}

// GetFirstRecordFromSDB(szKey) -> nP1, nP2 (khong co -> khong tra gi)
static int LuaSD_GetFirstRecordFromSDB(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;
	std::string strKey = Lua_ValueToString(L, 1);
	std::map<SD_KEY, SD_VAL*>::iterator it;
	for (it = s_mapData.begin(); it != s_mapData.end(); ++it)
	{
		if (it->first.strKey == strKey)
		{
			Lua_PushNumber(L, (double)it->first.dwP1);
			Lua_PushNumber(L, (double)it->first.dwP2);
			return 2;
		}
	}
	return 0;
}

TLua_Funcs g_ShareDataFuns[] =
{
	{ "OB_SaveShareData",		LuaSD_OB_SaveShareData },
	{ "OB_LoadShareData",		LuaSD_OB_LoadShareData },
	{ "OB_DeleteShareData",		LuaSD_OB_DeleteShareData },
	{ "SaveStringToSDB",		LuaSD_SaveStringToSDB },
	{ "SaveStringToSDBOw",		LuaSD_SaveStringToSDBOw },
	{ "GetStringFromSDB",		LuaSD_GetStringFromSDB },
	{ "SaveIntegerToSDB",		LuaSD_SaveIntegerToSDB },
	{ "SaveIntegerToSDBOw",		LuaSD_SaveIntegerToSDBOw },
	{ "GetIntegerFromSDB",		LuaSD_GetIntegerFromSDB },
	{ "ClearRecordOnShareDB",	LuaSD_ClearRecordOnShareDB },
	{ "GetRecordCount",			LuaSD_GetRecordCount },
	{ "GetFirstRecordFromSDB",	LuaSD_GetFirstRecordFromSDB },
};

int g_GetShareDataFunNum()
{
	return sizeof(g_ShareDataFuns) / sizeof(TLua_Funcs);
}

//////////////////////////////////////////////////////////////////////////////
// Khoi dong / dong
//////////////////////////////////////////////////////////////////////////////
static int SdLoadAll()
{
	if (!SdEnsure())
		return 0;
	if (!SdExec("SELECT skey,p1,p2,sdata FROM " SD_TABLE, (unsigned long)strlen("SELECT skey,p1,p2,sdata FROM " SD_TABLE)))
		return 0;
	MYSQL_RES* pRes = mysql_store_result(s_conn);
	if (!pRes)
		return 0;

	int nCount = 0;
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(pRes)) != NULL)
	{
		unsigned long* pLen = mysql_fetch_lengths(pRes);
		if (!pLen || !row[0])
			continue;
		char szKey[SD_KEY_MAX + 1];
		unsigned long nKeyLen = pLen[0];
		if (nKeyLen > SD_KEY_MAX)
			nKeyLen = SD_KEY_MAX;
		memcpy(szKey, row[0], nKeyLen);
		szKey[nKeyLen] = 0;
		DWORD dwP1 = row[1] ? (DWORD)strtoul(row[1], NULL, 10) : 0;
		DWORD dwP2 = row[2] ? (DWORD)strtoul(row[2], NULL, 10) : 0;
		SdCacheSet(szKey, dwP1, dwP2, row[3], row[3] ? (int)pLen[3] : 0);
		nCount++;
	}
	mysql_free_result(pRes);
	return nCount;
}

BOOL ShareData_Init()
{
	s_enable = gGetPrivateProfileIntEx("sharedata", "enable", "relay_friendcfg.ini", 1);
	if (!s_enable)
	{
		SdLog("TAT theo cau hinh (relay_friendcfg.ini [sharedata] enable=0)");
		return TRUE;
	}

	SdIniStr("Server", "127.0.0.1", s_dbHost, SD_STR);
	SdIniStr("User", "root", s_dbUser, SD_STR);
	SdIniStr("PassWord", "123456", s_dbPass, SD_STR);
	SdIniStr("DataBase", "jx1_role", s_dbName, SD_STR);
	s_dbPort = (unsigned)GetPrivateProfileIntA(SD_SECTION, "Port", 3306, SD_INI);

	if (!SdConnect())
	{
		s_enable = 0;
		SdLog("KHONG noi duoc MySQL -> tat ShareData (relay van chay binh thuong)");
		return FALSE;
	}
	const char* szCreate =
		"CREATE TABLE IF NOT EXISTS " SD_TABLE " ("
		"skey VARBINARY(64) NOT NULL,"
		"p1 INT UNSIGNED NOT NULL,"
		"p2 INT UNSIGNED NOT NULL,"
		"sdata BLOB,"
		"PRIMARY KEY (skey,p1,p2)"
		") ENGINE=InnoDB DEFAULT CHARSET=binary";
	if (!SdExec(szCreate, (unsigned long)strlen(szCreate)))
	{
		s_enable = 0;
		SdLog("KHONG tao duoc bang %s -> tat ShareData", SD_TABLE);
		return FALSE;
	}

	int nCount = SdLoadAll();
	SdLog("MO: %s:%u/%s bang %s, nap %d ban ghi", s_dbHost, s_dbPort, s_dbName, SD_TABLE, nCount);
	return TRUE;
}

void ShareData_Uninit()
{
	std::map<SD_KEY, SD_VAL*>::iterator it;
	for (it = s_mapData.begin(); it != s_mapData.end(); ++it)
		delete it->second;
	s_mapData.clear();
	if (s_conn) { mysql_close(s_conn); s_conn = NULL; }
	s_enable = 0;
}
