// KJx2League.cpp - xem KJx2League.h. DOT E cong thanh JX2.
// Persist: \settings\jx2league.txt (tmp + MoveFileEx REPLACE - khong dung KIniFile::Save).

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KTaskFuns.h"
#include "TaskDef.h"
#include "KJx2League.h"
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Store
//////////////////////////////////////////////////////////////////////
struct KJx2LgMember
{
	char				szName[64];
	int					nJob;
	std::map<int, int>	mapTask;
};

struct KJx2League
{
	int							nLid;
	int							nType;
	char						szName[64];
	int							nCreateTime;
	std::vector<KJx2LgMember>	vMembers;
	std::map<int, int>			mapTask;
};

// obj TAM (form dien truoc khi Apply)
struct KJx2LgTmpLeague
{
	int							nType;
	char						szName[64];
	std::vector<KJx2LgMember>	vMembers;
};
struct KJx2LgTmpMember
{
	char	szName[64];
	int		nJob;
	int		nType;			// dia chi league dich (LGM_SetMemberInfo mang du)
	char	szLgName[64];
};

#define JX2LG_TMP_LEAGUE_BASE	1000000
#define JX2LG_TMP_MEMBER_BASE	2000000
#define JX2LG_FILE				"\\settings\\jx2league.txt"

static std::map<int, KJx2League*>		s_LeagueByLid;		// lid tang dan -> league
static int								s_nNextLid = 0;
static std::map<int, KJx2LgTmpLeague*>	s_TmpLeague;
static int								s_nNextTmpLeague = JX2LG_TMP_LEAGUE_BASE;
static std::map<int, KJx2LgTmpMember*>	s_TmpMember;
static int								s_nNextTmpMember = JX2LG_TMP_MEMBER_BASE;
static bool								s_bLeagueLoaded = false;

typedef std::map<int, KJx2League*>::iterator TLgIt;

static void sStrCpy(char* szDst, const char* szSrc, int nDstSize)
{
	strncpy(szDst, szSrc ? szSrc : "", nDstSize - 1);
	szDst[nDstSize - 1] = 0;
}

static KJx2League* sFindLeague(int nType, const char* szName)
{
	if (!szName)
		return NULL;
	for (TLgIt it = s_LeagueByLid.begin(); it != s_LeagueByLid.end(); ++it)
	{
		if (it->second->nType == nType && strcmp(it->second->szName, szName) == 0)
			return it->second;
	}
	return NULL;
}

static KJx2League* sGetLeagueByLid(int nLid)
{
	TLgIt it = s_LeagueByLid.find(nLid);
	return (it == s_LeagueByLid.end()) ? NULL : it->second;
}

static KJx2LgMember* sFindMember(KJx2League* pLg, const char* szName)
{
	if (!pLg || !szName)
		return NULL;
	for (size_t i = 0; i < pLg->vMembers.size(); i++)
	{
		if (strcmp(pLg->vMembers[i].szName, szName) == 0)
			return &pLg->vMembers[i];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Persist (dinh dang dong: ten de CUOI dong vi ten TCVN3 co the chua space)
//   G <type> <createtime> <name...>
//   M <job> <name...>          - member cua G gan nhat
//   T <taskid> <value>         - league task cua G gan nhat
//   U <taskid> <value>         - member task cua M gan nhat
//////////////////////////////////////////////////////////////////////
static void sLeaguePath(char* szOut, bool bTmp)
{
	char szRoot[MAX_PATH];
	g_GetRootPath(szRoot);
	sprintf(szOut, "%s%s%s", szRoot, JX2LG_FILE, bTmp ? ".tmp" : "");
}

static char* sChopEol(char* sz)
{
	int n = (int)strlen(sz);
	while (n > 0 && (sz[n - 1] == '\n' || sz[n - 1] == '\r'))
		sz[--n] = 0;
	return sz;
}

static void sLeagueLoad()
{
	if (s_bLeagueLoaded)
		return;
	s_bLeagueLoaded = true;
	char szPath[MAX_PATH];
	sLeaguePath(szPath, false);
	FILE* f = fopen(szPath, "rb");
	if (!f)
		return;
	char szLine[512];
	KJx2League* pLg = NULL;
	KJx2LgMember* pMem = NULL;
	while (fgets(szLine, sizeof(szLine), f))
	{
		sChopEol(szLine);
		if (szLine[0] == 'G')
		{
			int nType = 0, nTime = 0, nPos = 0;
			if (sscanf(szLine + 2, "%d %d %n", &nType, &nTime, &nPos) >= 2 && nPos > 0)
			{
				pLg = new KJx2League;
				pLg->nLid = ++s_nNextLid;
				pLg->nType = nType;
				pLg->nCreateTime = nTime;
				sStrCpy(pLg->szName, szLine + 2 + nPos, sizeof(pLg->szName));
				s_LeagueByLid[pLg->nLid] = pLg;
				pMem = NULL;
			}
		}
		else if (szLine[0] == 'M' && pLg)
		{
			int nJob = 0, nPos = 0;
			if (sscanf(szLine + 2, "%d %n", &nJob, &nPos) >= 1 && nPos > 0)
			{
				KJx2LgMember m;
				m.nJob = nJob;
				sStrCpy(m.szName, szLine + 2 + nPos, sizeof(m.szName));
				pLg->vMembers.push_back(m);
				pMem = &pLg->vMembers.back();
			}
		}
		else if (szLine[0] == 'T' && pLg)
		{
			int nId = 0, nVal = 0;
			if (sscanf(szLine + 2, "%d %d", &nId, &nVal) == 2)
				pLg->mapTask[nId] = nVal;
		}
		else if (szLine[0] == 'U' && pMem)
		{
			int nId = 0, nVal = 0;
			if (sscanf(szLine + 2, "%d %d", &nId, &nVal) == 2)
				pMem->mapTask[nId] = nVal;
		}
	}
	fclose(f);
}

static void sLeagueSave()
{
	char szTmp[MAX_PATH], szPath[MAX_PATH];
	sLeaguePath(szTmp, true);
	sLeaguePath(szPath, false);
	FILE* f = fopen(szTmp, "wb");
	if (!f)
		return;
	for (TLgIt it = s_LeagueByLid.begin(); it != s_LeagueByLid.end(); ++it)
	{
		KJx2League* p = it->second;
		fprintf(f, "G %d %d %s\n", p->nType, p->nCreateTime, p->szName);
		std::map<int, int>::iterator t;
		for (t = p->mapTask.begin(); t != p->mapTask.end(); ++t)
			fprintf(f, "T %d %d\n", t->first, t->second);
		for (size_t i = 0; i < p->vMembers.size(); i++)
		{
			KJx2LgMember& m = p->vMembers[i];
			fprintf(f, "M %d %s\n", m.nJob, m.szName);
			for (t = m.mapTask.begin(); t != m.mapTask.end(); ++t)
				fprintf(f, "U %d %d\n", t->first, t->second);
		}
	}
	fclose(f);
	MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
}

//////////////////////////////////////////////////////////////////////
// Callback dong bo: fn(nType, szLeagueName, szMemberName, bSucceed) - 4 doi
// (league_cityinfo.lua:47,57; gb_taskfuncs.lua:36). Bo qua khi cb rong.
//////////////////////////////////////////////////////////////////////
static void sRunCallback(const char* szScript, const char* szFunc,
	int nType, const char* szName, const char* szMember, int nOk)
{
	if (!szScript || !szScript[0] || !szFunc || !szFunc[0])
		return;
	char szLow[MAX_PATH];
	sStrCpy(szLow, szScript, sizeof(szLow));
	for (char* p = szLow; *p; p++)
	{
		if (*p >= 'A' && *p <= 'Z')
			*p += 'a' - 'A';
	}
	KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
	if (pScript)
		pScript->CallFunction((char*)szFunc, 0, (char*)"dssd",
			nType, (char*)(szName ? szName : ""), (char*)(szMember ? szMember : ""), nOk);
	else
		g_DebugLog((LPSTR)"KJx2League: callback script khong tim thay [%s]", szLow);
}

//////////////////////////////////////////////////////////////////////
// Vong doi obj tam + dang ky
//////////////////////////////////////////////////////////////////////
int LuaLG_CreateLeagueObj(Lua_State* L)
{
	KJx2LgTmpLeague* p = new KJx2LgTmpLeague;
	p->nType = 0;
	p->szName[0] = 0;
	int h = ++s_nNextTmpLeague;
	s_TmpLeague[h] = p;
	Lua_PushNumber(L, h);
	return 1;
}

int LuaLG_FreeLeagueObj(Lua_State* L)
{
	if (Lua_IsNumber(L, 1))
	{
		int h = (int)Lua_ValueToNumber(L, 1);
		std::map<int, KJx2LgTmpLeague*>::iterator it = s_TmpLeague.find(h);
		if (it != s_TmpLeague.end())
		{
			delete it->second;
			s_TmpLeague.erase(it);
		}
	}
	return 0;
}

int LuaLG_SetLeagueInfo(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2) && Lua_IsString(L, 3))
	{
		int h = (int)Lua_ValueToNumber(L, 1);
		std::map<int, KJx2LgTmpLeague*>::iterator it = s_TmpLeague.find(h);
		if (it != s_TmpLeague.end())
		{
			it->second->nType = (int)Lua_ValueToNumber(L, 2);
			sStrCpy(it->second->szName, Lua_ValueToString(L, 3), sizeof(it->second->szName));
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// infocenter_head.lua:687 truyen NHAM lid dang ky vao doi 1 - phai im lang bo qua
int LuaLG_AddMemberToObj(Lua_State* L)
{
	int nOk = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		int hLg = (int)Lua_ValueToNumber(L, 1);
		int hMem = (int)Lua_ValueToNumber(L, 2);
		std::map<int, KJx2LgTmpLeague*>::iterator itL = s_TmpLeague.find(hLg);
		std::map<int, KJx2LgTmpMember*>::iterator itM = s_TmpMember.find(hMem);
		if (itL != s_TmpLeague.end() && itM != s_TmpMember.end())
		{
			KJx2LgMember m;
			sStrCpy(m.szName, itM->second->szName, sizeof(m.szName));
			m.nJob = itM->second->nJob;
			itL->second->vMembers.push_back(m);
			nOk = 1;
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

// (h [,szCbScript,szCbFunc]) - idempotent: league da ton tai van coi la thanh cong
int LuaLG_ApplyAddLeague(Lua_State* L)
{
	sLeagueLoad();
	int nOk = 0;
	char szName[64] = "";
	int nType = 0;
	if (Lua_IsNumber(L, 1))
	{
		int h = (int)Lua_ValueToNumber(L, 1);
		std::map<int, KJx2LgTmpLeague*>::iterator it = s_TmpLeague.find(h);
		if (it != s_TmpLeague.end() && it->second->szName[0])
		{
			KJx2LgTmpLeague* pTmp = it->second;
			nType = pTmp->nType;
			sStrCpy(szName, pTmp->szName, sizeof(szName));
			KJx2League* pLg = sFindLeague(nType, szName);
			if (!pLg)
			{
				pLg = new KJx2League;
				pLg->nLid = ++s_nNextLid;
				pLg->nType = nType;
				sStrCpy(pLg->szName, szName, sizeof(pLg->szName));
				pLg->nCreateTime = (int)time(NULL);
				s_LeagueByLid[pLg->nLid] = pLg;
			}
			// dem theo member da gan vao form (khong trung ten)
			for (size_t i = 0; i < pTmp->vMembers.size(); i++)
			{
				if (!sFindMember(pLg, pTmp->vMembers[i].szName))
					pLg->vMembers.push_back(pTmp->vMembers[i]);
			}
			sLeagueSave();
			nOk = 1;
		}
	}
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsString(L, 2) && Lua_IsString(L, 3))
		sRunCallback(Lua_ValueToString(L, 2), Lua_ValueToString(L, 3), nType, szName, "", nOk);
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaLG_ApplyRemoveLeague(Lua_State* L)
{
	sLeagueLoad();
	int nOk = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2))
	{
		KJx2League* pLg = sFindLeague((int)Lua_ValueToNumber(L, 1), Lua_ValueToString(L, 2));
		if (pLg)
		{
			s_LeagueByLid.erase(pLg->nLid);
			delete pLg;
			sLeagueSave();
			nOk = 1;
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Truy van
//////////////////////////////////////////////////////////////////////
int LuaLG_GetLeagueObj(Lua_State* L)
{
	sLeagueLoad();
	int nLid = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2))
	{
		KJx2League* pLg = sFindLeague((int)Lua_ValueToNumber(L, 1), Lua_ValueToString(L, 2));
		if (pLg)
			nLid = pLg->nLid;
	}
	Lua_PushNumber(L, nLid);
	return 1;
}

int LuaLG_GetLeagueObjByRole(Lua_State* L)
{
	sLeagueLoad();
	int nLid = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2))
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		const char* szMember = Lua_ValueToString(L, 2);
		for (TLgIt it = s_LeagueByLid.begin(); it != s_LeagueByLid.end(); ++it)
		{
			if (it->second->nType == nType && sFindMember(it->second, szMember))
			{
				nLid = it->second->nLid;
				break;
			}
		}
	}
	Lua_PushNumber(L, nLid);
	return 1;
}

int LuaLG_GetFirstLeague(Lua_State* L)
{
	sLeagueLoad();
	int nLid = 0;
	if (Lua_IsNumber(L, 1))
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		for (TLgIt it = s_LeagueByLid.begin(); it != s_LeagueByLid.end(); ++it)
		{
			if (it->second->nType == nType)
			{
				nLid = it->first;
				break;
			}
		}
	}
	Lua_PushNumber(L, nLid);
	return 1;
}

int LuaLG_GetNextLeague(Lua_State* L)
{
	sLeagueLoad();
	int nLid = 0;
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		int nType = (int)Lua_ValueToNumber(L, 1);
		int nCur = (int)Lua_ValueToNumber(L, 2);
		TLgIt it = s_LeagueByLid.upper_bound(nCur);
		for (; it != s_LeagueByLid.end(); ++it)
		{
			if (it->second->nType == nType)
			{
				nLid = it->first;
				break;
			}
		}
	}
	Lua_PushNumber(L, nLid);
	return 1;
}

// (lid) -> szName, nCreateTime, nMemberCount  (leaguematch\head.lua:110)
int LuaLG_GetLeagueInfo(Lua_State* L)
{
	sLeagueLoad();
	KJx2League* pLg = Lua_IsNumber(L, 1) ? sGetLeagueByLid((int)Lua_ValueToNumber(L, 1)) : NULL;
	if (pLg)
	{
		Lua_PushString(L, pLg->szName);
		Lua_PushNumber(L, pLg->nCreateTime);
		Lua_PushNumber(L, (double)(int)pLg->vMembers.size());
	}
	else
	{
		Lua_PushString(L, (char*)"");
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, 0);
	}
	return 3;
}

int LuaLG_GetLeagueCreateTime(Lua_State* L)
{
	sLeagueLoad();
	KJx2League* pLg = Lua_IsNumber(L, 1) ? sGetLeagueByLid((int)Lua_ValueToNumber(L, 1)) : NULL;
	Lua_PushNumber(L, pLg ? pLg->nCreateTime : 0);
	return 1;
}

int LuaLG_GetMemberCount(Lua_State* L)
{
	sLeagueLoad();
	KJx2League* pLg = Lua_IsNumber(L, 1) ? sGetLeagueByLid((int)Lua_ValueToNumber(L, 1)) : NULL;
	Lua_PushNumber(L, pLg ? (double)(int)pLg->vMembers.size() : 0);
	return 1;
}

// (lid, nIndex 0-based) -> szName, nJob ; ngoai dai -> ("",0) KHONG nil
// (infocenter_head.lua:170 lap thua 1 vong roi AppendString - nil la chet script)
int LuaLG_GetMemberInfo(Lua_State* L)
{
	sLeagueLoad();
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		KJx2League* pLg = sGetLeagueByLid((int)Lua_ValueToNumber(L, 1));
		int nIndex = (int)Lua_ValueToNumber(L, 2);
		if (pLg && nIndex >= 0 && (size_t)nIndex < pLg->vMembers.size())
		{
			Lua_PushString(L, pLg->vMembers[nIndex].szName);
			Lua_PushNumber(L, pLg->vMembers[nIndex].nJob);
			return 2;
		}
	}
	Lua_PushString(L, (char*)"");
	Lua_PushNumber(L, 0);
	return 2;
}

int LuaLG_GetMemberObj(Lua_State* L)
{
	Lua_PushNumber(L, 0);	// 0 call site Lua - du ten cho khoi nil
	return 1;
}

int LuaLG_GetMemberJoinTime(Lua_State* L)
{
	Lua_PushNumber(L, 0);	// 0 call site Lua
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Task
//////////////////////////////////////////////////////////////////////
// QUA TAI: (lid, nTask) HOAC (nType, szName, nTask)
int LuaLG_GetLeagueTask(Lua_State* L)
{
	sLeagueLoad();
	KJx2League* pLg = NULL;
	int nTaskId = 0;
	int nTop = Lua_GetTopIndex(L);
	if (nTop >= 3 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2) && Lua_IsNumber(L, 3))
	{
		pLg = sFindLeague((int)Lua_ValueToNumber(L, 1), Lua_ValueToString(L, 2));
		nTaskId = (int)Lua_ValueToNumber(L, 3);
	}
	else if (nTop >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		pLg = sGetLeagueByLid((int)Lua_ValueToNumber(L, 1));
		nTaskId = (int)Lua_ValueToNumber(L, 2);
	}
	int nVal = 0;
	if (pLg)
	{
		std::map<int, int>::iterator it = pLg->mapTask.find(nTaskId);
		if (it != pLg->mapTask.end())
			nVal = it->second;
	}
	Lua_PushNumber(L, nVal);
	return 1;
}

// dung chung cho Set (bSet=1) va Append: (nType,szName,nTask,nVal [,cbS,cbF])
static int sLeagueTaskWrite(Lua_State* L, int bSet)
{
	sLeagueLoad();
	int nOk = 0;
	int nType = 0;
	const char* szName = "";
	if (Lua_GetTopIndex(L) >= 4 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2) &&
		Lua_IsNumber(L, 3) && Lua_IsNumber(L, 4))
	{
		nType = (int)Lua_ValueToNumber(L, 1);
		szName = Lua_ValueToString(L, 2);
		KJx2League* pLg = sFindLeague(nType, szName);
		if (pLg)
		{
			int nTaskId = (int)Lua_ValueToNumber(L, 3);
			int nVal = (int)Lua_ValueToNumber(L, 4);
			if (bSet)
				pLg->mapTask[nTaskId] = nVal;
			else
				pLg->mapTask[nTaskId] += nVal;
			sLeagueSave();
			nOk = 1;
		}
	}
	// dang 6 doi: callback DONG BO (khoa league_cityinfo mo ngay - phan bien E-08)
	if (Lua_GetTopIndex(L) >= 6 && Lua_IsString(L, 5) && Lua_IsString(L, 6))
		sRunCallback(Lua_ValueToString(L, 5), Lua_ValueToString(L, 6), nType, szName, "", nOk);
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaLG_ApplySetLeagueTask(Lua_State* L)		{ return sLeagueTaskWrite(L, 1); }
int LuaLG_ApplyAppendLeagueTask(Lua_State* L)	{ return sLeagueTaskWrite(L, 0); }

// LUON 4 doi: (nType, szName, szMember, nTask)
int LuaLG_GetMemberTask(Lua_State* L)
{
	sLeagueLoad();
	int nVal = 0;
	if (Lua_GetTopIndex(L) >= 4 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2) &&
		Lua_IsString(L, 3) && Lua_IsNumber(L, 4))
	{
		KJx2League* pLg = sFindLeague((int)Lua_ValueToNumber(L, 1), Lua_ValueToString(L, 2));
		KJx2LgMember* pMem = sFindMember(pLg, Lua_ValueToString(L, 3));
		if (pMem)
		{
			std::map<int, int>::iterator it = pMem->mapTask.find((int)Lua_ValueToNumber(L, 4));
			if (it != pMem->mapTask.end())
				nVal = it->second;
		}
	}
	Lua_PushNumber(L, nVal);
	return 1;
}

// (nType,szName,szMember,nTask,nVal [,cbS,cbF]) - Set 5 doi, Append 5/7 doi
static int sMemberTaskWrite(Lua_State* L, int bSet)
{
	sLeagueLoad();
	int nOk = 0;
	int nType = 0;
	const char* szName = "";
	const char* szMember = "";
	if (Lua_GetTopIndex(L) >= 5 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2) &&
		Lua_IsString(L, 3) && Lua_IsNumber(L, 4) && Lua_IsNumber(L, 5))
	{
		nType = (int)Lua_ValueToNumber(L, 1);
		szName = Lua_ValueToString(L, 2);
		szMember = Lua_ValueToString(L, 3);
		KJx2League* pLg = sFindLeague(nType, szName);
		KJx2LgMember* pMem = sFindMember(pLg, szMember);
		if (pMem)
		{
			int nTaskId = (int)Lua_ValueToNumber(L, 4);
			int nVal = (int)Lua_ValueToNumber(L, 5);
			if (bSet)
				pMem->mapTask[nTaskId] = nVal;
			else
				pMem->mapTask[nTaskId] += nVal;
			sLeagueSave();
			nOk = 1;
		}
		else
			g_DebugLog((LPSTR)"KJx2League: member task ghi truot (type %d, lg [%s], mem [%s])",
				nType, szName, szMember);
	}
	if (Lua_GetTopIndex(L) >= 7 && Lua_IsString(L, 6) && Lua_IsString(L, 7))
		sRunCallback(Lua_ValueToString(L, 6), Lua_ValueToString(L, 7), nType, szName, szMember, nOk);
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaLG_ApplySetMemberTask(Lua_State* L)		{ return sMemberTaskWrite(L, 1); }
int LuaLG_ApplyAppendMemberTask(Lua_State* L)	{ return sMemberTaskWrite(L, 0); }

//////////////////////////////////////////////////////////////////////
// Member obj
//////////////////////////////////////////////////////////////////////
int LuaLGM_CreateMemberObj(Lua_State* L)
{
	KJx2LgTmpMember* p = new KJx2LgTmpMember;
	p->szName[0] = 0;
	p->nJob = 0;
	p->nType = 0;
	p->szLgName[0] = 0;
	int h = ++s_nNextTmpMember;
	s_TmpMember[h] = p;
	Lua_PushNumber(L, h);
	return 1;
}

// (h, szMember, nJob, nType, szLgName) - obj TU MANG dia chi league dich
int LuaLGM_SetMemberInfo(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 5 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2) &&
		Lua_IsNumber(L, 3) && Lua_IsNumber(L, 4) && Lua_IsString(L, 5))
	{
		std::map<int, KJx2LgTmpMember*>::iterator it =
			s_TmpMember.find((int)Lua_ValueToNumber(L, 1));
		if (it != s_TmpMember.end())
		{
			sStrCpy(it->second->szName, Lua_ValueToString(L, 2), sizeof(it->second->szName));
			it->second->nJob = (int)Lua_ValueToNumber(L, 3);
			it->second->nType = (int)Lua_ValueToNumber(L, 4);
			sStrCpy(it->second->szLgName, Lua_ValueToString(L, 5), sizeof(it->second->szLgName));
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	Lua_PushNumber(L, 0);
	return 1;
}

// (h [,cbS,cbF]) -> 1 = OK (infocenter_head.lua:760 kiem == 1); da la member = OK
int LuaLGM_ApplyAddMember(Lua_State* L)
{
	sLeagueLoad();
	int nOk = 0;
	int nType = 0;
	const char* szLg = "";
	const char* szMember = "";
	if (Lua_IsNumber(L, 1))
	{
		std::map<int, KJx2LgTmpMember*>::iterator it =
			s_TmpMember.find((int)Lua_ValueToNumber(L, 1));
		if (it != s_TmpMember.end() && it->second->szName[0])
		{
			KJx2LgTmpMember* pTmp = it->second;
			nType = pTmp->nType;
			szLg = pTmp->szLgName;
			szMember = pTmp->szName;
			KJx2League* pLg = sFindLeague(nType, szLg);
			if (pLg)
			{
				if (!sFindMember(pLg, szMember))
				{
					KJx2LgMember m;
					sStrCpy(m.szName, szMember, sizeof(m.szName));
					m.nJob = pTmp->nJob;
					pLg->vMembers.push_back(m);
					sLeagueSave();
				}
				nOk = 1;
			}
			else
				g_DebugLog((LPSTR)"KJx2League: AddMember truot - league (type %d, [%s]) chua ton tai", nType, szLg);
		}
	}
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsString(L, 2) && Lua_IsString(L, 3))
		sRunCallback(Lua_ValueToString(L, 2), Lua_ValueToString(L, 3), nType, szLg, szMember, nOk);
	Lua_PushNumber(L, nOk);
	return 1;
}

// LUON 6 doi (doi 6 luon 0 - moi call site; nhan roi bo qua)
int LuaLGM_ApplyRemoveMember(Lua_State* L)
{
	sLeagueLoad();
	int nOk = 0;
	int nType = 0;
	const char* szName = "";
	const char* szMember = "";
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 1) && Lua_IsString(L, 2) && Lua_IsString(L, 3))
	{
		nType = (int)Lua_ValueToNumber(L, 1);
		szName = Lua_ValueToString(L, 2);
		szMember = Lua_ValueToString(L, 3);
		KJx2League* pLg = sFindLeague(nType, szName);
		if (pLg)
		{
			for (size_t i = 0; i < pLg->vMembers.size(); i++)
			{
				if (strcmp(pLg->vMembers[i].szName, szMember) == 0)
				{
					pLg->vMembers.erase(pLg->vMembers.begin() + i);
					sLeagueSave();
					nOk = 1;
					break;
				}
			}
		}
	}
	if (Lua_GetTopIndex(L) >= 5 && Lua_IsString(L, 4) && Lua_IsString(L, 5))
		sRunCallback(Lua_ValueToString(L, 4), Lua_ValueToString(L, 5), nType, szName, szMember, nOk);
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaLGM_FreeMemberObj(Lua_State* L)
{
	if (Lua_IsNumber(L, 1))
	{
		std::map<int, KJx2LgTmpMember*>::iterator it =
			s_TmpMember.find((int)Lua_ValueToNumber(L, 1));
		if (it != s_TmpMember.end())
		{
			delete it->second;
			s_TmpMember.erase(it);
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// LG_ApplyDoScript - goc: relay chay szScript::szFunc(szParam) trong state relay.
// Ta: thuc thi CUC BO dong bo trong state cua file dich (LoadAllScript da nap ca
// \script\mission\citywar_global\ladder.lua bien the RELAY o duong that).
// Callback: fn(szLeagueName, szMemberName, nResult) - 3 doi
// (wulin_final_match\createleague.lua:5).
//////////////////////////////////////////////////////////////////////
int LuaLG_ApplyDoScript(Lua_State* L)
{
	int nOk = 0;
	const char* szName = "";
	const char* szMember = "";
	if (Lua_GetTopIndex(L) >= 6 && Lua_IsString(L, 4) && Lua_IsString(L, 5) && Lua_IsString(L, 6))
	{
		if (Lua_IsString(L, 2))
			szName = Lua_ValueToString(L, 2);
		if (Lua_IsString(L, 3))
			szMember = Lua_ValueToString(L, 3);
		char szLow[MAX_PATH];
		sStrCpy(szLow, Lua_ValueToString(L, 4), sizeof(szLow));
		for (char* p = szLow; *p; p++)
		{
			if (*p >= 'A' && *p <= 'Z')
				*p += 'a' - 'A';
		}
		KLuaScript* pScript = (KLuaScript*)g_GetScript(szLow);
		if (pScript)
			// (void*) de KHONG khop overload private (LPSTR,int,LPSTR,va_list):
			// tren MSVC va_list = char*, doi cuoi char* se resolve nham vao ban private
			nOk = pScript->CallFunction((char*)Lua_ValueToString(L, 5), 0, (char*)"s",
				(void*)Lua_ValueToString(L, 6)) ? 1 : 0;
		else
			g_DebugLog((LPSTR)"KJx2League: DoScript khong tim thay [%s]", szLow);
	}
	if (Lua_GetTopIndex(L) >= 8 && Lua_IsString(L, 7) && Lua_IsString(L, 8))
	{
		const char* szCbS = Lua_ValueToString(L, 7);
		const char* szCbF = Lua_ValueToString(L, 8);
		if (szCbS[0] && szCbF[0])
		{
			char szLow[MAX_PATH];
			sStrCpy(szLow, szCbS, sizeof(szLow));
			for (char* p = szLow; *p; p++)
			{
				if (*p >= 'A' && *p <= 'Z')
					*p += 'a' - 'A';
			}
			KLuaScript* pCb = (KLuaScript*)g_GetScript(szLow);
			if (pCb)
				pCb->CallFunction((char*)szCbF, 0, (char*)"ssd",
					(char*)szName, (char*)szMember, nOk);
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// GLOBAL mission + timer
// - OpenGlbMission(id): chay InitMission() cua script missions.txt[id] (autoexec goc :142)
// - StartGlbMSTimer(nMissionId, nTimerId, nIntervalFrames): timer LAP VO HAN,
//   moi lan dao han chay OnTimer() cua script TimerTask.txt[nTimerId].
//   18 frame = 1 giay; do phan giai thuc te = nhip Breathe (du cho chu ky >= 5 phut).
//////////////////////////////////////////////////////////////////////
struct KJx2GlbTimer
{
	int		nMissionId;
	int		nTimerId;
	DWORD	dwIntervalMs;
	DWORD	dwNextFire;
};
static std::vector<KJx2GlbTimer>	s_GlbTimers;

int LuaOpenGlbMission(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nMissionId = (int)Lua_ValueToNumber(L, 1);
	char szScript[MAX_PATH];
	szScript[0] = 0;
	g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
	int nOk = 0;
	if (szScript[0])
	{
		for (char* p = szScript; *p; p++)
		{
			if (*p >= 'A' && *p <= 'Z')
				*p += 'a' - 'A';
		}
		KLuaScript* pScript = (KLuaScript*)g_GetScript(szScript);
		if (pScript)
			nOk = pScript->CallFunction((char*)"InitMission", 0, (char*)"") ? 1 : 0;
		else
			g_DebugLog((LPSTR)"KJx2League: OpenGlbMission(%d) script chua nap [%s]", nMissionId, szScript);
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

int LuaStartGlbMSTimer(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3 || !Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2) || !Lua_IsNumber(L, 3))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KJx2GlbTimer t;
	t.nMissionId = (int)Lua_ValueToNumber(L, 1);
	t.nTimerId = (int)Lua_ValueToNumber(L, 2);
	double fFrames = Lua_ValueToNumber(L, 3);
	if (fFrames < 18)
		fFrames = 18;
	t.dwIntervalMs = (DWORD)(fFrames * 1000.0 / 18.0);
	t.dwNextFire = GetTickCount() + t.dwIntervalMs;
	// cung (mission,timer) = doi tan so (leaguematch schedule.lua:173-176 Stop roi Start)
	for (size_t i = 0; i < s_GlbTimers.size(); i++)
	{
		if (s_GlbTimers[i].nMissionId == t.nMissionId && s_GlbTimers[i].nTimerId == t.nTimerId)
		{
			s_GlbTimers[i] = t;
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	s_GlbTimers.push_back(t);
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaStopGlbMSTimer(Lua_State* L)
{
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 1) && Lua_IsNumber(L, 2))
	{
		int nMissionId = (int)Lua_ValueToNumber(L, 1);
		int nTimerId = (int)Lua_ValueToNumber(L, 2);
		for (size_t i = 0; i < s_GlbTimers.size(); i++)
		{
			if (s_GlbTimers[i].nMissionId == nMissionId && s_GlbTimers[i].nTimerId == nTimerId)
			{
				s_GlbTimers.erase(s_GlbTimers.begin() + i);
				break;
			}
		}
	}
	Lua_PushNumber(L, 1);
	return 1;
}

// Goi tu CoreServerShell::Breathe moi tick. Thu thap timer dao han TRUOC roi moi
// chay (OnTimer co the Start/Stop timer -> khong duoc vua lap vua sua vector).
void KJx2GlbMission_Breathe()
{
	if (s_GlbTimers.empty())
		return;
	DWORD dwNow = GetTickCount();
	int nFire[16];
	int nFireCount = 0;
	for (size_t i = 0; i < s_GlbTimers.size() && nFireCount < 16; i++)
	{
		if ((int)(dwNow - s_GlbTimers[i].dwNextFire) >= 0)
		{
			nFire[nFireCount++] = s_GlbTimers[i].nTimerId;
			// re-arm; neu lag qua 1 chu ky thi neo lai tu bay gio
			s_GlbTimers[i].dwNextFire += s_GlbTimers[i].dwIntervalMs;
			if ((int)(dwNow - s_GlbTimers[i].dwNextFire) >= 0)
				s_GlbTimers[i].dwNextFire = dwNow + s_GlbTimers[i].dwIntervalMs;
		}
	}
	for (int k = 0; k < nFireCount; k++)
	{
		char szScript[MAX_PATH];
		szScript[0] = 0;
		g_TimerTask.GetTimerTaskScript(szScript, (unsigned short)nFire[k], MAX_PATH);
		if (szScript[0])
		{
			for (char* p = szScript; *p; p++)
			{
				if (*p >= 'A' && *p <= 'Z')
					*p += 'a' - 'A';
			}
			KLuaScript* pScript = (KLuaScript*)g_GetScript(szScript);
			if (pScript)
				pScript->CallFunction((char*)"OnTimer", 0, (char*)"");
			else
				g_DebugLog((LPSTR)"KJx2League: GlbTimer %d script chua nap [%s]", nFire[k], szScript);
		}
	}
}

#endif // _SERVER
