//---------------------------------------------------------------------------
//  PerfHud.cpp - do va hien FPS / CPU / GPU / RAM ngay trong game.
//
//  Nguyen tac: KHONG them thu vien lien ket, KHONG dung header SDK moi.
//  Moi thu goi qua con tro ham nap dong (LoadLibrary/GetProcAddress) nen
//  build khong the vo vi thieu SDK, va may thieu API thi chi hien "n/a".
//
//  Chi phi: lay mau 500ms/lan (GPU 1000ms/lan), giua cac lan chi ve chu.
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "PerfHud.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include <stdio.h>
#include <stdlib.h>

extern iRepresentShell* g_pRepresentShell;

#define PH_FONT             12
#define PH_X                10
#define PH_Y                26
#define PH_LINE             15
#define PH_COL_TEXT         0xFFFFFFFF
#define PH_COL_GOOD         0xFF80FF80
#define PH_COL_WARN         0xFFFFD040
#define PH_COL_BAD          0xFFFF6060
#define PH_COL_BORDER       0xFF000000
#define PH_SAMPLE_MS        500
#define PH_GPU_SAMPLE_MS    1000

static int      s_nEnable        = 0;

// ---- so lieu da do ----
static int      s_nCpuPercent    = -1;   // CPU cua tien trinh game (da chia so nhan)
static int      s_nCpuCores      = 1;
static int      s_nRamProcMB     = -1;   // RAM tien trinh game dang giu
static int      s_nRamSysPercent = -1;   // % RAM toan may dang dung
static int      s_nRamSysFreeMB  = -1;
static int      s_nGpuPercent    = -1;   // % GPU (tong cac engine cua game)
static int      s_nGpuMemMB      = -1;   // VRAM tien trinh game dang giu
static int      s_nFrameMs10     = 0;    // thoi gian 1 khung ve, nhan 10

static DWORD    s_dwLastSample   = 0;
static DWORD    s_dwLastGpu      = 0;
static DWORD    s_dwLastFrameTk  = 0;

// ---- CPU ----
static ULONGLONG s_ullPrevProcTime = 0;
static ULONGLONG s_ullPrevWallTime = 0;

// ---- psapi (RAM tien trinh) ----
typedef struct _PH_PROCESS_MEMORY_COUNTERS {
    DWORD  cb;
    DWORD  PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} PH_PROCESS_MEMORY_COUNTERS;

typedef BOOL (WINAPI *PFN_GetProcessMemoryInfo)(HANDLE, PH_PROCESS_MEMORY_COUNTERS*, DWORD);
static PFN_GetProcessMemoryInfo s_pfnGetProcMem = NULL;

// ---- pdh (GPU) ----
typedef struct _PH_FMT_COUNTERVALUE {
    DWORD   CStatus;
    union {
        LONG        longValue;
        double      doubleValue;
        LONGLONG    largeValue;
        LPCSTR      AnsiStringValue;
    };
} PH_FMT_COUNTERVALUE;

typedef struct _PH_FMT_COUNTERVALUE_ITEM_A {
    LPSTR               szName;
    PH_FMT_COUNTERVALUE FmtValue;
} PH_FMT_COUNTERVALUE_ITEM_A;

typedef LONG (WINAPI *PFN_PdhOpenQueryA)(LPCSTR, DWORD_PTR, HANDLE*);
typedef LONG (WINAPI *PFN_PdhAddCounterA)(HANDLE, LPCSTR, DWORD_PTR, HANDLE*);
typedef LONG (WINAPI *PFN_PdhCollectQueryData)(HANDLE);
typedef LONG (WINAPI *PFN_PdhGetFormattedCounterArrayA)(HANDLE, DWORD, LPDWORD, LPDWORD, PH_FMT_COUNTERVALUE_ITEM_A*);

static PFN_PdhOpenQueryA                s_pfnPdhOpen    = NULL;
static PFN_PdhAddCounterA               s_pfnPdhAdd     = NULL;
static PFN_PdhCollectQueryData          s_pfnPdhCollect = NULL;
static PFN_PdhGetFormattedCounterArrayA s_pfnPdhArray   = NULL;

static HANDLE   s_hPdhQuery   = NULL;
static HANDLE   s_hGpuUtil    = NULL;
static HANDLE   s_hGpuMem     = NULL;
static int      s_nPdhState   = 0;      // 0 = chua thu, 1 = dung duoc, -1 = bo cuoc

static BYTE*    s_pPdhBuf     = NULL;
static DWORD    s_dwPdhBufLen = 0;

#define PH_FMT_DOUBLE       0x00000200
#define PH_MORE_DATA        ((LONG)0x800007D2L)

static void PerfHud_InitOnce()
{
    static int s_bInited = 0;
    if (s_bInited)
        return;
    s_bInited = 1;

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    s_nCpuCores = (int)si.dwNumberOfProcessors;
    if (s_nCpuCores <= 0)
        s_nCpuCores = 1;

    HMODULE hPsapi = LoadLibraryA("psapi.dll");
    if (hPsapi)
        s_pfnGetProcMem = (PFN_GetProcessMemoryInfo)GetProcAddress(hPsapi, "GetProcessMemoryInfo");
}

// cong don moi phan tu cua mot counter dang wildcard
static int PerfHud_SumCounter(HANDLE hCounter, double fScale)
{
    if (!s_pfnPdhArray || !hCounter)
        return -1;

    DWORD dwSize  = s_dwPdhBufLen;
    DWORD dwCount = 0;
    LONG  lRet = s_pfnPdhArray(hCounter, PH_FMT_DOUBLE, &dwSize, &dwCount,
                               (PH_FMT_COUNTERVALUE_ITEM_A*)s_pPdhBuf);
    if (lRet == PH_MORE_DATA)
    {
        if (dwSize > 4 * 1024 * 1024)
            return -1;
        BYTE* pNew = (BYTE*)realloc(s_pPdhBuf, dwSize);
        if (!pNew)
            return -1;
        s_pPdhBuf     = pNew;
        s_dwPdhBufLen = dwSize;
        dwSize = s_dwPdhBufLen;
        lRet = s_pfnPdhArray(hCounter, PH_FMT_DOUBLE, &dwSize, &dwCount,
                             (PH_FMT_COUNTERVALUE_ITEM_A*)s_pPdhBuf);
    }
    if (lRet != 0 || !s_pPdhBuf)
        return -1;

    PH_FMT_COUNTERVALUE_ITEM_A* pItems = (PH_FMT_COUNTERVALUE_ITEM_A*)s_pPdhBuf;
    double fSum = 0.0;
    for (DWORD i = 0; i < dwCount; i++)
    {
        if (pItems[i].FmtValue.CStatus == 0)
            fSum += pItems[i].FmtValue.doubleValue;
    }
    return (int)(fSum * fScale);
}

static void PerfHud_SampleGpu()
{
    if (s_nPdhState < 0)
        return;

    if (s_nPdhState == 0)
    {
        s_nPdhState = -1;               // bo cuoc, tru khi di het duoc duong duoi
        HMODULE hPdh = LoadLibraryA("pdh.dll");
        if (!hPdh)
            return;
        s_pfnPdhOpen    = (PFN_PdhOpenQueryA)GetProcAddress(hPdh, "PdhOpenQueryA");
        s_pfnPdhAdd     = (PFN_PdhAddCounterA)GetProcAddress(hPdh, "PdhAddCounterA");
        s_pfnPdhCollect = (PFN_PdhCollectQueryData)GetProcAddress(hPdh, "PdhCollectQueryData");
        s_pfnPdhArray   = (PFN_PdhGetFormattedCounterArrayA)GetProcAddress(hPdh, "PdhGetFormattedCounterArrayA");
        if (!s_pfnPdhOpen || !s_pfnPdhAdd || !s_pfnPdhCollect || !s_pfnPdhArray)
            return;
        if (s_pfnPdhOpen(NULL, 0, &s_hPdhQuery) != 0 || !s_hPdhQuery)
            return;

        // "GPU Engine" / "GPU Process Memory" chi co tu Windows 10 1709 tro len.
        // Loc theo pid cua chinh minh de khong dem GPU cua ung dung khac.
        char  szPath[256];
        DWORD dwPid = GetCurrentProcessId();

        _snprintf(szPath, sizeof(szPath) - 1,
                  "\\GPU Engine(pid_%lu*)\\Utilization Percentage", (unsigned long)dwPid);
        szPath[sizeof(szPath) - 1] = 0;
        s_pfnPdhAdd(s_hPdhQuery, szPath, 0, &s_hGpuUtil);

        _snprintf(szPath, sizeof(szPath) - 1,
                  "\\GPU Process Memory(pid_%lu*)\\Local Usage", (unsigned long)dwPid);
        szPath[sizeof(szPath) - 1] = 0;
        s_pfnPdhAdd(s_hPdhQuery, szPath, 0, &s_hGpuMem);

        if (!s_hGpuUtil && !s_hGpuMem)
            return;

        s_pfnPdhCollect(s_hPdhQuery);   // lan dau chi de lay moc
        s_nPdhState = 1;
        return;
    }

    if (s_pfnPdhCollect(s_hPdhQuery) != 0)
        return;

    if (s_hGpuUtil)
        s_nGpuPercent = PerfHud_SumCounter(s_hGpuUtil, 1.0);
    if (s_hGpuMem)
        s_nGpuMemMB = PerfHud_SumCounter(s_hGpuMem, 1.0 / (1024.0 * 1024.0));
}

static void PerfHud_Sample()
{
    DWORD dwNow = timeGetTime();

    // --- thoi gian 1 khung ve (lam muot) ---
    if (s_dwLastFrameTk)
    {
        DWORD dwDelta = dwNow - s_dwLastFrameTk;
        if (dwDelta < 1000)
            s_nFrameMs10 = (s_nFrameMs10 * 7 + (int)dwDelta * 30) / 10;
    }
    s_dwLastFrameTk = dwNow;

    if (s_dwLastSample && (dwNow - s_dwLastSample) < PH_SAMPLE_MS)
        return;
    s_dwLastSample = dwNow;

    // --- CPU cua tien trinh ---
    FILETIME ftCreate, ftExit, ftKernel, ftUser;
    if (GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, &ftKernel, &ftUser))
    {
        ULARGE_INTEGER k, u;
        k.LowPart = ftKernel.dwLowDateTime;  k.HighPart = ftKernel.dwHighDateTime;
        u.LowPart = ftUser.dwLowDateTime;    u.HighPart = ftUser.dwHighDateTime;
        ULONGLONG ullProc = k.QuadPart + u.QuadPart;    // don vi 100ns

        FILETIME ftNow;
        GetSystemTimeAsFileTime(&ftNow);
        ULARGE_INTEGER w;
        w.LowPart = ftNow.dwLowDateTime; w.HighPart = ftNow.dwHighDateTime;
        ULONGLONG ullWall = w.QuadPart;

        if (s_ullPrevWallTime && ullWall > s_ullPrevWallTime)
        {
            ULONGLONG dProc = ullProc - s_ullPrevProcTime;
            ULONGLONG dWall = ullWall - s_ullPrevWallTime;
            s_nCpuPercent = (int)((dProc * 100) / (dWall * (ULONGLONG)s_nCpuCores));
            if (s_nCpuPercent > 100)
                s_nCpuPercent = 100;
        }
        s_ullPrevProcTime = ullProc;
        s_ullPrevWallTime = ullWall;
    }

    // --- RAM tien trinh ---
    if (s_pfnGetProcMem)
    {
        PH_PROCESS_MEMORY_COUNTERS pmc;
        memset(&pmc, 0, sizeof(pmc));
        pmc.cb = sizeof(pmc);
        if (s_pfnGetProcMem(GetCurrentProcess(), &pmc, sizeof(pmc)))
            s_nRamProcMB = (int)(pmc.WorkingSetSize / (1024 * 1024));
    }

    // --- RAM toan may ---
    MEMORYSTATUSEX ms;
    memset(&ms, 0, sizeof(ms));
    ms.dwLength = sizeof(ms);
    if (GlobalMemoryStatusEx(&ms))
    {
        s_nRamSysPercent = (int)ms.dwMemoryLoad;
        s_nRamSysFreeMB  = (int)(ms.ullAvailPhys / (1024 * 1024));
    }

    // --- GPU ---
    if (!s_dwLastGpu || (dwNow - s_dwLastGpu) >= PH_GPU_SAMPLE_MS)
    {
        s_dwLastGpu = dwNow;
        PerfHud_SampleGpu();
    }
}

void PerfHud_SetEnable(int nOn)
{
    s_nEnable = nOn ? 1 : 0;
    if (s_nEnable)
        PerfHud_InitOnce();
}

int PerfHud_IsEnable()
{
    return s_nEnable;
}

static unsigned int PerfHud_FpsColor(int nFps)
{
    if (nFps <= 0)
        return PH_COL_TEXT;
    if (nFps >= 50)
        return PH_COL_GOOD;
    if (nFps >= 25)
        return PH_COL_WARN;
    return PH_COL_BAD;
}

static unsigned int PerfHud_LoadColor(int nPercent)
{
    if (nPercent < 0)
        return PH_COL_TEXT;
    if (nPercent >= 90)
        return PH_COL_BAD;
    if (nPercent >= 70)
        return PH_COL_WARN;
    return PH_COL_GOOD;
}

void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing)
{
    if (!s_nEnable || !g_pRepresentShell)
        return;

    PerfHud_InitOnce();
    PerfHud_Sample();

    char szLine[128];
    int  nY = PH_Y;

    // dong 1: FPS ve / FPS logic / do tre mang
    _snprintf(szLine, sizeof(szLine) - 1, "FPS %3d ve  |  %2d logic  |  ping %u ms",
              nPaintFps, nLogicFps, dwPing);
    szLine[sizeof(szLine) - 1] = 0;
    g_pRepresentShell->OutputText(PH_FONT, szLine, KRF_ZERO_END, PH_X, nY,
                                  PerfHud_FpsColor(nPaintFps), 0,
                                  TEXT_IN_SINGLE_PLANE_COORD, PH_COL_BORDER);
    nY += PH_LINE;

    // dong 2: thoi gian 1 khung ve
    _snprintf(szLine, sizeof(szLine) - 1, "Khung ve  %d.%d ms",
              s_nFrameMs10 / 10, s_nFrameMs10 % 10);
    szLine[sizeof(szLine) - 1] = 0;
    g_pRepresentShell->OutputText(PH_FONT, szLine, KRF_ZERO_END, PH_X, nY,
                                  PH_COL_TEXT, 0, TEXT_IN_SINGLE_PLANE_COORD, PH_COL_BORDER);
    nY += PH_LINE;

    // dong 3: CPU
    if (s_nCpuPercent >= 0)
        _snprintf(szLine, sizeof(szLine) - 1, "CPU  %3d%%  (%d nhan)", s_nCpuPercent, s_nCpuCores);
    else
        _snprintf(szLine, sizeof(szLine) - 1, "CPU  n/a");
    szLine[sizeof(szLine) - 1] = 0;
    g_pRepresentShell->OutputText(PH_FONT, szLine, KRF_ZERO_END, PH_X, nY,
                                  PerfHud_LoadColor(s_nCpuPercent), 0,
                                  TEXT_IN_SINGLE_PLANE_COORD, PH_COL_BORDER);
    nY += PH_LINE;

    // dong 4: GPU
    if (s_nGpuPercent >= 0 && s_nGpuMemMB >= 0)
        _snprintf(szLine, sizeof(szLine) - 1, "GPU  %3d%%  |  VRAM %d MB", s_nGpuPercent, s_nGpuMemMB);
    else if (s_nGpuPercent >= 0)
        _snprintf(szLine, sizeof(szLine) - 1, "GPU  %3d%%", s_nGpuPercent);
    else if (s_nGpuMemMB >= 0)
        _snprintf(szLine, sizeof(szLine) - 1, "GPU  VRAM %d MB", s_nGpuMemMB);
    else
        _snprintf(szLine, sizeof(szLine) - 1, "GPU  n/a (can Windows 10 1709 tro len)");
    szLine[sizeof(szLine) - 1] = 0;
    g_pRepresentShell->OutputText(PH_FONT, szLine, KRF_ZERO_END, PH_X, nY,
                                  PerfHud_LoadColor(s_nGpuPercent), 0,
                                  TEXT_IN_SINGLE_PLANE_COORD, PH_COL_BORDER);
    nY += PH_LINE;

    // dong 5: RAM
    if (s_nRamProcMB >= 0 || s_nRamSysPercent >= 0)
        _snprintf(szLine, sizeof(szLine) - 1, "RAM  game %d MB  |  may %d%%  (con %d MB)",
                  s_nRamProcMB, s_nRamSysPercent, s_nRamSysFreeMB);
    else
        _snprintf(szLine, sizeof(szLine) - 1, "RAM  n/a");
    szLine[sizeof(szLine) - 1] = 0;
    g_pRepresentShell->OutputText(PH_FONT, szLine, KRF_ZERO_END, PH_X, nY,
                                  PerfHud_LoadColor(s_nRamSysPercent), 0,
                                  TEXT_IN_SINGLE_PLANE_COORD, PH_COL_BORDER);
}
