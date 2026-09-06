//////////////////////////////////////////////////////////////////////////////
// RelayTaskCentre.cpp  (S3Relay)  [RELAYHT 06/09]  -- xem RelayTaskCentre.h
//
// Nhip: TaskCentre_Tick() goi moi giay tu WM_TIMER cua luong chinh, nen kich ban
// tac vu KHONG chay tren luong mang -> khong phai khoa Lua.
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Global.h"
#include "S3Relay.h"
#include "RelayScript.h"
#include "RelayTaskCentre.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////////
// Trang thai
//////////////////////////////////////////////////////////////////////////////
struct TC_TASK
{
	std::string	strFile;		// duong dan kieu "\\relaysetting\\task\\announce.lua"
	std::string	strName;		// TaskName()
	int			nIntervalMin;	// TaskInterval() -- so PHUT; 0 = chi chay mot lan
	int			nHour;			// TaskTime() -- -1 = khong dat, chay ngay
	int			nMin;
	int			nCountLimit;	// TaskCountLimit() -- 0 = khong gioi han
	int			nStartDay;		// TaskSetStartDay() -- 0 = moi ngay, 1..7 = thu (1 = CN)
	int			nMode;			// TaskSetMode() -- giu lai, chua dung
	int			nExecuted;
	time_t		tNextRun;		// 0 = da tat
};

static std::vector<TC_TASK>	s_vecTask;
static TC_TASK*				s_pLoading = NULL;	// tac vu dang chay TaskShedule()
static TC_TASK*				s_pRunning = NULL;	// tac vu dang chay TaskContent()
static BOOL					s_bReady = FALSE;

static void TcLog(const char* fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = 0;
	rTRACE("[TaskCentre] %s", buf);
}

//////////////////////////////////////////////////////////////////////////////
// Tinh lan chay ke tiep
//////////////////////////////////////////////////////////////////////////////
static time_t TcCalcNextRun(TC_TASK& t, time_t tNow)
{
	if (t.nHour < 0)
	{
		// khong dat TaskTime -> chay ngay khi relay khoi dong (giong ghi chu
		// trong announce.lua ban Linux: "khong dat TaskTme, chay tu luc relay len")
		return tNow;
	}

	struct tm tmNow;
	struct tm* p = localtime(&tNow);
	if (!p)
		return tNow;
	tmNow = *p;
	tmNow.tm_hour = t.nHour;
	tmNow.tm_min = t.nMin;
	tmNow.tm_sec = 0;
	tmNow.tm_isdst = -1;
	time_t tRun = mktime(&tmNow);

	if (tRun <= tNow)
	{
		if (t.nIntervalMin > 0)
		{
			// nhay tung buoc interval cho toi khi vuot qua hien tai
			int nStep = t.nIntervalMin * 60;
			time_t tDiff = tNow - tRun;
			time_t tAdd = ((tDiff / nStep) + 1) * (time_t)nStep;
			tRun += tAdd;
		}
		else
		{
			tRun += 24 * 3600;		// chi mot lan/ngay
		}
	}
	return tRun;
}

//////////////////////////////////////////////////////////////////////////////
// Ham Lua cho kich ban tac vu khai bao lich
//////////////////////////////////////////////////////////////////////////////
static int LuaTC_TaskName(Lua_State* L)
{
	if (s_pLoading && Lua_IsString(L, 1))
		s_pLoading->strName = Lua_ValueToString(L, 1);
	return 0;
}

static int LuaTC_TaskInterval(Lua_State* L)
{
	if (s_pLoading && Lua_IsNumber(L, 1))
	{
		s_pLoading->nIntervalMin = (int)Lua_ValueToNumber(L, 1);
		if (s_pLoading->nIntervalMin < 0)
			s_pLoading->nIntervalMin = 0;
	}
	return 0;
}

static int LuaTC_TaskTime(Lua_State* L)
{
	if (s_pLoading && Lua_GetTopIndex(L) >= 2)
	{
		s_pLoading->nHour = (int)Lua_ValueToNumber(L, 1);
		s_pLoading->nMin = (int)Lua_ValueToNumber(L, 2);
		if (s_pLoading->nHour < 0 || s_pLoading->nHour > 23)
			s_pLoading->nHour = 0;
		if (s_pLoading->nMin < 0 || s_pLoading->nMin > 59)
			s_pLoading->nMin = 0;
	}
	return 0;
}

static int LuaTC_TaskCountLimit(Lua_State* L)
{
	if (s_pLoading && Lua_IsNumber(L, 1))
	{
		s_pLoading->nCountLimit = (int)Lua_ValueToNumber(L, 1);
		if (s_pLoading->nCountLimit < 0)
			s_pLoading->nCountLimit = 0;
	}
	return 0;
}

static int LuaTC_TaskSetStartDay(Lua_State* L)
{
	if (s_pLoading && Lua_IsNumber(L, 1))
		s_pLoading->nStartDay = (int)Lua_ValueToNumber(L, 1);
	return 0;
}

static int LuaTC_TaskSetMode(Lua_State* L)
{
	if (s_pLoading && Lua_IsNumber(L, 1))
		s_pLoading->nMode = (int)Lua_ValueToNumber(L, 1);
	return 0;
}

// GetTaskCurCount() -- so lan tac vu DANG CHAY da chay (ke ca lan nay)
static int LuaTC_GetTaskCurCount(Lua_State* L)
{
	TC_TASK* p = s_pRunning ? s_pRunning : s_pLoading;
	Lua_PushNumber(L, p ? p->nExecuted : 0);
	return 1;
}

static BOOL TcAddTask(const char* szFile);

// TaskLoad("x.lua") -- them mot tac vu luc dang chay
static int LuaTC_TaskLoad(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, TcAddTask(Lua_ValueToString(L, 1)) ? 1 : 0);
	return 1;
}

// TaskRelease("x.lua") -- tat mot tac vu (khong go kich ban khoi bo nho)
static int LuaTC_TaskRelease(Lua_State* L)
{
	if (!Lua_IsString(L, 1))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char szKey[MAX_PATH];
	_snprintf(szKey, sizeof(szKey) - 1, "%s%s", TC_TASK_DIR, Lua_ValueToString(L, 1));
	szKey[sizeof(szKey) - 1] = 0;
	for (char* pc = szKey; *pc; pc++)
	{
		if (*pc >= 'A' && *pc <= 'Z')
			*pc += 'a' - 'A';
	}
	int nOff = 0;
	for (size_t i = 0; i < s_vecTask.size(); i++)
	{
		if (s_vecTask[i].strFile == szKey)
		{
			s_vecTask[i].tNextRun = 0;
			nOff++;
		}
	}
	Lua_PushNumber(L, nOff);
	return 1;
}

TLua_Funcs g_TaskCentreFuns[] =
{
	{ "TaskName",			LuaTC_TaskName },
	{ "TaskInterval",		LuaTC_TaskInterval },
	{ "TaskTime",			LuaTC_TaskTime },
	{ "TaskCountLimit",		LuaTC_TaskCountLimit },
	{ "TaskSetStartDay",	LuaTC_TaskSetStartDay },
	{ "TaskSetMode",		LuaTC_TaskSetMode },
	{ "GetTaskCurCount",	LuaTC_GetTaskCurCount },
	{ "TaskLoad",			LuaTC_TaskLoad },
	{ "TaskRelease",		LuaTC_TaskRelease },
};

int g_GetTaskCentreFunNum()
{
	return sizeof(g_TaskCentreFuns) / sizeof(TLua_Funcs);
}

//////////////////////////////////////////////////////////////////////////////
// Nap danh sach
//////////////////////////////////////////////////////////////////////////////
static BOOL TcAddTask(const char* szFile)
{
	if (!szFile || !szFile[0])
		return FALSE;
	if ((int)s_vecTask.size() >= TC_MAX_TASK)
	{
		TcLog("TRAN so tac vu (%d), bo qua %s", TC_MAX_TASK, szFile);
		return FALSE;
	}

	char szKey[MAX_PATH];
	_snprintf(szKey, sizeof(szKey) - 1, "%s%s", TC_TASK_DIR, szFile);
	szKey[sizeof(szKey) - 1] = 0;
	for (char* pc = szKey; *pc; pc++)
	{
		if (*pc >= 'A' && *pc <= 'Z')
			*pc += 'a' - 'A';
	}

	TC_TASK t;
	t.strFile = szKey;
	t.nIntervalMin = 0;
	t.nHour = -1;
	t.nMin = 0;
	t.nCountLimit = 0;
	t.nStartDay = 0;
	t.nMode = 0;
	t.nExecuted = 0;
	t.tNextRun = 0;

	KLuaScript* pScript = RelayScript_Get(szKey, TRUE);
	if (!pScript)
	{
		TcLog("KHONG nap duoc %s", szKey);
		return FALSE;
	}

	// chay TaskShedule() de kich ban khai bao lich (chu 'Shedule' thieu 'c' la
	// dung theo ban goc). s_pLoading tro vao BIEN CUC BO: neu TaskShedule() lai
	// goi TaskLoad() thi vector co the doi cho, con tro vao vector se hong.
	s_pLoading = &t;
	{
		int nTop = 0;
		pScript->SafeCallBegin(&nTop);
		pScript->CallFunction((LPSTR)"TaskShedule", 0, (LPSTR)"");
		pScript->SafeCallEnd(nTop);
	}
	s_pLoading = NULL;

	t.tNextRun = TcCalcNextRun(t, time(NULL));
	s_vecTask.push_back(t);
	TC_TASK& tt = s_vecTask[s_vecTask.size() - 1];

	char szWhen[64];
	if (tt.nHour < 0)
		strcpy(szWhen, "ngay lap tuc");
	else
		_snprintf(szWhen, sizeof(szWhen) - 1, "%02d:%02d", tt.nHour, tt.nMin);
	szWhen[sizeof(szWhen) - 1] = 0;
	TcLog("+ %s [%s] moc %s, moi %d phut, toi da %d lan",
		szFile, tt.strName.empty() ? "?" : tt.strName.c_str(),
		szWhen, tt.nIntervalMin, tt.nCountLimit);
	return TRUE;
}

BOOL TaskCentre_Init()
{
	if (s_bReady)
		return TRUE;
	s_bReady = TRUE;

	// giu san cho: push_back sau nay khong doi cho vung nho nen s_pRunning /
	// tham chieu TC_TASK& trong Tick khong bao gio hong
	s_vecTask.reserve(TC_MAX_TASK);

	int nCount = GetPrivateProfileIntA("List", "Count", 0, TC_LIST_FILE);
	if (nCount <= 0)
	{
		TcLog("khong co tac vu nao (%s [List] Count=%d)", TC_LIST_FILE, nCount);
		return TRUE;
	}
	if (nCount > TC_MAX_TASK)
		nCount = TC_MAX_TASK;

	int nOK = 0;
	for (int i = 0; i < nCount; i++)
	{
		char szSec[64], szFile[MAX_PATH];
		_snprintf(szSec, sizeof(szSec) - 1, "Task_%d", i);
		szSec[sizeof(szSec) - 1] = 0;
		GetPrivateProfileStringA(szSec, "TaskFile", "", szFile, sizeof(szFile) - 1, TC_LIST_FILE);
		szFile[sizeof(szFile) - 1] = 0;
		if (!szFile[0])
			continue;
		// cat khoang trang cuoi
		int n = (int)strlen(szFile);
		while (n > 0 && (szFile[n - 1] == ' ' || szFile[n - 1] == '\t' || szFile[n - 1] == '\r'))
			szFile[--n] = 0;
		if (TcAddTask(szFile))
			nOK++;
	}
	TcLog("nap %d/%d tac vu tu %s", nOK, nCount, TC_LIST_FILE);
	return TRUE;
}

void TaskCentre_Uninit()
{
	s_vecTask.clear();
	s_pLoading = NULL;
	s_pRunning = NULL;
	s_bReady = FALSE;
}

int TaskCentre_Reload()
{
	TaskCentre_Uninit();
	RelayScript_ReloadAll();
	TaskCentre_Init();
	return (int)s_vecTask.size();
}

int TaskCentre_GetCount()
{
	return (int)s_vecTask.size();
}

//////////////////////////////////////////////////////////////////////////////
// Nhip
//////////////////////////////////////////////////////////////////////////////
void TaskCentre_Tick()
{
	if (!s_bReady || s_vecTask.empty())
		return;

	time_t tNow = time(NULL);
	for (size_t i = 0; i < s_vecTask.size(); i++)
	{
		TC_TASK& t = s_vecTask[i];
		if (t.tNextRun == 0 || tNow < t.tNextRun)
			continue;

		// loc theo thu trong tuan neu co dat
		if (t.nStartDay > 0)
		{
			struct tm* p = localtime(&tNow);
			if (p && (p->tm_wday + 1) != t.nStartDay)
			{
				t.tNextRun = TcCalcNextRun(t, tNow + 60);
				continue;
			}
		}

		KLuaScript* pScript = RelayScript_Get(t.strFile.c_str(), TRUE);
		if (pScript)
		{
			t.nExecuted++;
			s_pRunning = &t;
			int nTop = 0;
			pScript->SafeCallBegin(&nTop);
			pScript->CallFunction((LPSTR)"TaskContent", 0, (LPSTR)"");
			pScript->SafeCallEnd(nTop);
			s_pRunning = NULL;
		}

		if (t.nCountLimit > 0 && t.nExecuted >= t.nCountLimit)
		{
			t.tNextRun = 0;		// het so lan -> tat
			TcLog("- %s da chay du %d lan, dung", t.strFile.c_str(), t.nCountLimit);
		}
		else if (t.nIntervalMin > 0)
		{
			t.tNextRun = tNow + (time_t)t.nIntervalMin * 60;
		}
		else
		{
			t.tNextRun = 0;		// khong co chu ky -> chi chay mot lan
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// Bao GameServer noi / san sang
//////////////////////////////////////////////////////////////////////////////
static void TcCallForAll(const char* szFunc, DWORD dwIP)
{
	if (!s_bReady)
		return;
	for (size_t i = 0; i < s_vecTask.size(); i++)
	{
		KLuaScript* pScript = RelayScript_Get(s_vecTask[i].strFile.c_str(), FALSE);
		if (!pScript)
			continue;
		int nTop = 0;
		pScript->SafeCallBegin(&nTop);
		pScript->CallFunction((LPSTR)szFunc, 0, (LPSTR)"d", (int)dwIP);
		pScript->SafeCallEnd(nTop);
	}
}

void TaskCentre_OnGameSvrConnected(DWORD dwIP)
{
	TcCallForAll("GameSvrConnected", dwIP);
}

void TaskCentre_OnGameSvrReady(DWORD dwIP)
{
	TcCallForAll("GameSvrReady", dwIP);
}
