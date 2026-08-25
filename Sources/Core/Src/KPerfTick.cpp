//---------------------------------------------------------------------------
// KPerfTick.cpp - [PerfLog 24/08] Xem mo ta o KPerfTick.h.
//
// Toan bo tep nay nam trong #ifdef _SERVER: ban client bien dich ra rong.
//---------------------------------------------------------------------------
#include "KCore.h"		// PCH cua du an - PHAI la include dau tien
#include "KPerfTick.h"

#ifdef _SERVER

#include <stdio.h>
#include <string.h>

//---------------------------------------------------------------------------
// Ten giai doan - PHAI cung thu tu voi enum KPERF_STAGE o KPerfTick.h
//---------------------------------------------------------------------------
static const char* s_szStage[PERF_COUNT] =
{
	"TICK",
	"SCRIPT_TIME",
	"GLBMISSION",
	"CITYWAR",
	"SW_MSGLOOP",
	"SW_MAINLOOP",
	"SW_ACTIVATE",
	"AUTOSAVE",
	"BAUCUA",
	"GS_MSGLOOP",
	"GS_MAINLOOP",
};

// Bang phan bo: mot o cho moi mili-giay tron, o cuoi gom tat ca >= PERF_BUCKETS-1.
#define PERF_BUCKETS	512

struct KPerfStat
{
	double	dSum;			// tong thoi gian (ms) trong khoang bao cao
	double	dMax;			// lan lau nhat (ms)
	int		nCount;			// so lan do
	int		nBucket[PERF_BUCKETS];
};

static int			s_nPerfOn = -1;			// -1 = chua doc cau hinh
static int			s_nInterval = 60;		// giay giua hai bao cao
static double		s_dTreMs = 55.0;		// nguong coi la "tick tre" (1000/18)
static int			s_nConsole = 0;			// 1 = in ra cua so GameServer
static KPerfStat	s_Stat[PERF_COUNT];
static int			s_nFrame = 0;			// so tick trong khoang bao cao
static int			s_nLate = 0;			// so tick vuot s_dTreMs
static double		s_dLastTick = 0.0;		// thoi gian tick vua roi (ms)
static double		s_dWindowStart = 0.0;	// moc bat dau khoang bao cao (ms)
static FILE*		s_pFile = NULL;			// giu tep MO SAN nhu g_AutoLog
static double		s_dFreq = 0.0;			// tick dong ho / mili-giay
static __int64		s_nRegionSum = 0;		// khoi luong: tong so region hoat dong
static int			s_nRegionMax = 0;
static __int64		s_nNpcSum = 0;			// khoi luong: tong so NPC trong region hoat dong
static int			s_nNpcMax = 0;

//---------------------------------------------------------------------------
double g_PerfNowMs()
{
	LARGE_INTEGER li;
	if (s_dFreq <= 0.0)
	{
		LARGE_INTEGER f;
		if (!QueryPerformanceFrequency(&f) || f.QuadPart <= 0)
			s_dFreq = -1.0;					// khong co dong ho phan giai cao
		else
			s_dFreq = (double)f.QuadPart / 1000.0;
	}
	if (s_dFreq < 0.0)
		return (double)timeGetTime();
	QueryPerformanceCounter(&li);
	return (double)li.QuadPart / s_dFreq;
}

//---------------------------------------------------------------------------
void g_PerfSet(int nOn)
{
	s_nPerfOn = nOn ? 1 : 0;
	if (!s_nPerfOn && s_pFile)
	{
		fflush(s_pFile);
		fclose(s_pFile);
		s_pFile = NULL;
	}
}

//---------------------------------------------------------------------------
int g_PerfOn()
{
	if (s_nPerfOn < 0)
	{
		s_nPerfOn = (int)GetPrivateProfileIntA("PerfLog", "On", 0, ".\\config.ini");
		s_nInterval = (int)GetPrivateProfileIntA("PerfLog", "IntervalSec", 60, ".\\config.ini");
		if (s_nInterval < 5)
			s_nInterval = 5;
		int nTre = (int)GetPrivateProfileIntA("PerfLog", "TreMs", 55, ".\\config.ini");
		if (nTre < 1)
			nTre = 55;
		s_dTreMs = (double)nTre;
		s_nConsole = (int)GetPrivateProfileIntA("PerfLog", "Console", 0, ".\\config.ini");
		memset(s_Stat, 0, sizeof(s_Stat));
		s_nFrame = 0;
		s_nLate = 0;
		s_dWindowStart = 0.0;
	}
	return s_nPerfOn;
}

//---------------------------------------------------------------------------
void g_PerfAdd(int nStage, double dMs)
{
	if (nStage < 0 || nStage >= PERF_COUNT)
		return;
	if (dMs < 0.0)			// dong ho nhay lui (doi loi CPU) - bo qua lan do nay
		return;
	KPerfStat* p = &s_Stat[nStage];
	p->dSum += dMs;
	p->nCount++;
	if (dMs > p->dMax)
		p->dMax = dMs;
	int nB = (int)dMs;
	if (nB < 0)
		nB = 0;
	if (nB >= PERF_BUCKETS)
		nB = PERF_BUCKETS - 1;
	p->nBucket[nB]++;
	if (nStage == PERF_TICK)
		s_dLastTick = dMs;
}

//---------------------------------------------------------------------------
void g_PerfCount(int nRegion, int nNpc)
{
	// PHAI la "<= 0": s_nPerfOn khoi tao -1 (chua doc cau hinh) va !(-1) la SAI,
	// nen viet "if (!s_nPerfOn)" se van cong don khi chua biet bat hay tat.
	if (s_nPerfOn <= 0)
		return;
	s_nRegionSum += nRegion;
	s_nNpcSum += nNpc;
	if (nRegion > s_nRegionMax)
		s_nRegionMax = nRegion;
	if (nNpc > s_nNpcMax)
		s_nNpcMax = nNpc;
}

//---------------------------------------------------------------------------
// Muc phan vi thu nPercent (theo mili-giay tron, sai so 1ms - du de xep hang).
//---------------------------------------------------------------------------
static int PerfPercentile(const KPerfStat* p, int nPercent)
{
	if (p->nCount <= 0)
		return 0;
	int nNeed = (p->nCount * nPercent) / 100;
	if (nNeed < 1)
		nNeed = 1;
	int nAcc = 0;
	for (int i = 0; i < PERF_BUCKETS; i++)
	{
		nAcc += p->nBucket[i];
		if (nAcc >= nNeed)
			return i;
	}
	return PERF_BUCKETS - 1;
}

//---------------------------------------------------------------------------
static int s_nMoTepHong = 0;	// 1 = da thu mo tep va that bai, khoi thu lai moi dong

static void PerfWrite(const char* szLine)
{
	if (s_nConsole)
		printf("%s\n", szLine);
	if (!s_pFile && !s_nMoTepHong)
	{
		s_pFile = fopen("jx_perf_server.log", "a");
		if (!s_pFile)
		{
			// Theo dung cach g_AutoLog xu ly (KCore.cpp:815-819): khong mo duoc tep
			// thi THOI HAN, khong thu lai moi dong. Chuyen sang in man hinh de bao
			// cao khong bien mat lang le.
			s_nMoTepHong = 1;
			s_nConsole = 1;
			printf("%s\n", szLine);
			return;
		}
	}
	if (s_pFile)
		fprintf(s_pFile, "%s\n", szLine);
}

//---------------------------------------------------------------------------
static void PerfReport(double dNow, int nOnlinePlayer)
{
	double dWindow = dNow - s_dWindowStart;
	if (dWindow <= 0.0)
		dWindow = 1.0;

	SYSTEMTIME st;
	GetLocalTime(&st);

	char szLine[512];
	_snprintf(szLine, sizeof(szLine) - 1,
		"[PERF] %02d:%02d:%02d tick=%d tre=%d (%.1f%%) online=%d khoang=%.1fs",
		st.wHour, st.wMinute, st.wSecond,
		s_nFrame, s_nLate,
		s_nFrame > 0 ? (s_nLate * 100.0 / s_nFrame) : 0.0,
		nOnlinePlayer, dWindow / 1000.0);
	szLine[sizeof(szLine) - 1] = 0;
	PerfWrite(szLine);

	if (s_nFrame > 0 && (s_nRegionSum > 0 || s_nNpcSum > 0))
	{
		_snprintf(szLine, sizeof(szLine) - 1,
			"   %-12s region tb=%d max=%d | NPC tb=%d max=%d  (khoi luong moi tick)",
			"KHOILUONG",
			(int)(s_nRegionSum / s_nFrame), s_nRegionMax,
			(int)(s_nNpcSum / s_nFrame), s_nNpcMax);
		szLine[sizeof(szLine) - 1] = 0;
		PerfWrite(szLine);
	}

	for (int i = 0; i < PERF_COUNT; i++)
	{
		const KPerfStat* p = &s_Stat[i];
		if (p->nCount <= 0)
			continue;
		_snprintf(szLine, sizeof(szLine) - 1,
			"   %-12s n=%-6d tb=%7.2fms max=%8.2fms p95=%4dms chiem=%5.1f%%",
			s_szStage[i], p->nCount,
			p->dSum / p->nCount, p->dMax,
			PerfPercentile(p, 95),
			p->dSum * 100.0 / dWindow);
		szLine[sizeof(szLine) - 1] = 0;
		PerfWrite(szLine);
	}

	if (s_pFile)
	{
		fflush(s_pFile);
		if (ftell(s_pFile) > 16 * 1024 * 1024)
		{
			fclose(s_pFile);
			s_pFile = NULL;
			remove("jx_perf_server.log.1");
			rename("jx_perf_server.log", "jx_perf_server.log.1");
		}
	}

	memset(s_Stat, 0, sizeof(s_Stat));
	s_nFrame = 0;
	s_nLate = 0;
	s_nRegionSum = 0;
	s_nRegionMax = 0;
	s_nNpcSum = 0;
	s_nNpcMax = 0;
	s_dWindowStart = dNow;
}

//---------------------------------------------------------------------------
void g_PerfFrame(int nOnlinePlayer)
{
	if (!g_PerfOn())
		return;
	double dNow = g_PerfNowMs();
	if (s_dWindowStart <= 0.0)
	{
		s_dWindowStart = dNow;
		return;
	}
	s_nFrame++;
	if (s_dLastTick > s_dTreMs)
		s_nLate++;
	s_dLastTick = 0.0;
	if (dNow - s_dWindowStart >= s_nInterval * 1000.0)
		PerfReport(dNow, nOnlinePlayer);
}

#endif	// _SERVER
