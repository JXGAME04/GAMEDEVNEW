//---------------------------------------------------------------------------
// [ANDROID 11/09 HUD] Bang do hieu nang trong game cho Android: FPS / khung ve / CPU / RAM / PIN / GPU.
// Chu: "them hien thi FPS o trong game - Pin - GPU nhu ban du an USVOLAM de de biet".
// Ui/PerfHud.cpp cua ban Windows dung psapi/pdh nen bi loai khoi Android (android/lists); tep nay thay the
// ba ham PerfHud_* cho Android (JxAndroidStubs.cpp khong con stub PerfHud nua).
//   - FPS ve / vong logic / ping: UiShell.cpp dua vao (nhu Windows).
//   - Khung ve: thoi gian giua hai lan ve (lam muot) + lau nhat trong 1 giay.
//   - CPU: /proc/self/stat (utime + stime) so voi thoi gian thuc, chia so nhan.
//   - RAM: /proc/self/statm (RSS) + /proc/meminfo (MemAvailable).
//   - Pin: SDL_GetPowerInfo (phan tram + dang sac / het sac / khong pin).
//   - GPU: Rep3_ThongKeGpu cua Represent3 (driver, texture MB, trang atlas, trinh chieu, lenh ve/khung) qua GetProcAddress.
// Bat/tat: config.ini [Client] PerfHud=1 (S3Client GameInit goi PerfHud_SetEnable). Vi tri: PerfHudX / PerfHudY (mac dinh
// giua-tren man hinh). Chi bien dich khi JX_ANDROID.
//---------------------------------------------------------------------------
#ifdef JX_ANDROID
#include "KWin32.h"
#include "../Ui/PerfHud.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern iRepresentShell*	g_pRepresentShell;
extern int				SCREEN_WIDTH;
extern int				SCREEN_HEIGHT;

#define PH_FONT			12
#define PH_LINE			14
#define PH_COL_TEXT		0xFFFFFFFF
#define PH_COL_GOOD		0xFF80FF80
#define PH_COL_WARN		0xFFFFD040
#define PH_COL_BAD		0xFFFF6060
#define PH_COL_BORDER	0xFF000000
#define PH_SAMPLE_MS	500

typedef int (*PFN_Rep3ThongKeGpu)(char* sz, int n);

static int				s_nEnable = 0;
static int				s_nX = -1, s_nY = 4;				// -1 = tu tinh (giua-tren)
static int				s_nDaDocIni = 0;
static int				s_nCores = 1;
static int				s_nCpuPerMille = -1;
static int				s_nRamProcMB = -1;
static int				s_nRamFreeMB = -1;
static int				s_nRamTotalMB = -1;
static int				s_nPinPhanTram = -1;
static int				s_nPinTrangThai = 0;				// SDL_PowerState
static int				s_nFrameMs10 = 0, s_nFrameMaxMs10 = 0, s_nFrameMaxCur = 0;
static unsigned int		s_uMaxReset = 0, s_uLastSample = 0, s_uLastFrame = 0;
static unsigned long long s_ullPrevProcTicks = 0;
static unsigned int		s_uPrevWallMs = 0;
static char				s_szGpu[160] = "";
static PFN_Rep3ThongKeGpu s_pfnGpu = NULL;
static int				s_nGpuThu = 0;

static void PerfHud_DocIni()
{
	if (s_nDaDocIni)
		return;
	s_nDaDocIni = 1;
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	s_nX = GetPrivateProfileInt("Client", "PerfHudX", -1, szCfg);
	s_nY = GetPrivateProfileInt("Client", "PerfHudY", 4, szCfg);
	long n = sysconf(_SC_NPROCESSORS_ONLN);
	s_nCores = (n > 0) ? (int)n : 1;
}

static void PerfHud_LayMauCpuRam()
{
	// CPU cua tien trinh: /proc/self/stat truong 14 (utime) + 15 (stime), don vi tick (CLK_TCK)
	FILE* f = fopen("/proc/self/stat", "r");
	if (f)
	{
		char sz[1024] = { 0 };
		if (fgets(sz, sizeof(sz) - 1, f))
		{
			char* p = strrchr(sz, ')');	// sau ten tien trinh "(...)"
			if (p)
			{
				unsigned long long ut = 0, st = 0; int i = 0; char* tok = strtok(p + 1, " ");
				// tok = truong 3 (state); truong 14, 15 = utime, stime
				for (i = 3; tok && i <= 15; i++)
				{
					if (i == 14) ut = strtoull(tok, NULL, 10);
					if (i == 15) st = strtoull(tok, NULL, 10);
					tok = strtok(NULL, " ");
				}
				unsigned long long ticks = ut + st;
				unsigned int uNow = (unsigned int)SDL_GetTicks();
				long clk = sysconf(_SC_CLK_TCK); if (clk <= 0) clk = 100;
				if (s_uPrevWallMs && uNow > s_uPrevWallMs && ticks >= s_ullPrevProcTicks)
				{
					double dCpuMs = (double)(ticks - s_ullPrevProcTicks) * 1000.0 / (double)clk;
					double dWallMs = (double)(uNow - s_uPrevWallMs) * (double)s_nCores;
					int nPm = (dWallMs > 0.0) ? (int)(dCpuMs * 1000.0 / dWallMs) : -1;
					if (nPm > 1000) nPm = 1000;
					s_nCpuPerMille = nPm;
				}
				s_ullPrevProcTicks = ticks; s_uPrevWallMs = uNow;
			}
		}
		fclose(f);
	}
	// RAM tien trinh: /proc/self/statm truong 2 (resident, don vi trang)
	f = fopen("/proc/self/statm", "r");
	if (f)
	{
		unsigned long uSize = 0, uRes = 0;
		if (fscanf(f, "%lu %lu", &uSize, &uRes) == 2)
			s_nRamProcMB = (int)((unsigned long long)uRes * (unsigned long long)getpagesize() / (1024ull * 1024ull));
		fclose(f);
	}
	// RAM may: /proc/meminfo
	f = fopen("/proc/meminfo", "r");
	if (f)
	{
		char sz[256]; unsigned long kb = 0;
		while (fgets(sz, sizeof(sz), f))
		{
			if (sscanf(sz, "MemTotal: %lu", &kb) == 1) s_nRamTotalMB = (int)(kb / 1024);
			else if (sscanf(sz, "MemAvailable: %lu", &kb) == 1) s_nRamFreeMB = (int)(kb / 1024);
		}
		fclose(f);
	}
}

static void PerfHud_LayMau()
{
	unsigned int uNow = (unsigned int)SDL_GetTicks();
	if (s_uLastFrame)
	{
		unsigned int d = uNow - s_uLastFrame;
		if (d < 1000)
		{
			s_nFrameMs10 = (s_nFrameMs10 * 7 + (int)d * 30) / 10;
			if ((int)d * 10 > s_nFrameMaxCur) s_nFrameMaxCur = (int)d * 10;
		}
	}
	if (!s_uMaxReset || uNow - s_uMaxReset >= 1000)
	{
		s_uMaxReset = uNow; s_nFrameMaxMs10 = s_nFrameMaxCur; s_nFrameMaxCur = 0;
	}
	s_uLastFrame = uNow;
	if (s_uLastSample && uNow - s_uLastSample < PH_SAMPLE_MS)
		return;
	s_uLastSample = uNow;
	PerfHud_LayMauCpuRam();
	{
		int nGiay = 0, nPt = -1;
		s_nPinTrangThai = (int)SDL_GetPowerInfo(&nGiay, &nPt);
		s_nPinPhanTram = nPt;
	}
	if (!s_pfnGpu && s_nGpuThu < 8)
	{
		s_nGpuThu++;
		HMODULE h = GetModuleHandleA("Represent3.dll");	// lop tuong thich: dlopen("libRepresent3.so")
		if (h) s_pfnGpu = (PFN_Rep3ThongKeGpu)GetProcAddress(h, "Rep3_ThongKeGpu");
	}
	if (s_pfnGpu)
	{
		if (s_pfnGpu(s_szGpu, sizeof(s_szGpu)) <= 0) strcpy(s_szGpu, "GPU  (chua co so lieu)");
	}
	else
		strcpy(s_szGpu, "GPU  n/a");
}

static unsigned int PerfHud_MauFps(int nFps)
{
	if (nFps <= 0) return PH_COL_TEXT;
	if (nFps >= 50) return PH_COL_GOOD;
	if (nFps >= 25) return PH_COL_WARN;
	return PH_COL_BAD;
}

static unsigned int PerfHud_MauTai(int nPercent)
{
	if (nPercent < 0) return PH_COL_TEXT;
	if (nPercent >= 90) return PH_COL_BAD;
	if (nPercent >= 70) return PH_COL_WARN;
	return PH_COL_GOOD;
}

static void PerfHud_Chu(const char* sz, int nX, int nY, unsigned int uMau)
{
	g_pRepresentShell->OutputText(PH_FONT, (char*)sz, KRF_ZERO_END, nX, nY, uMau, 0, TEXT_IN_SINGLE_PLANE_COORD, PH_COL_BORDER);
}

void PerfHud_SetEnable(int nOn)
{
	s_nEnable = nOn ? 1 : 0;
	if (s_nEnable)
		PerfHud_DocIni();
}

int PerfHud_IsEnable()
{
	return s_nEnable;
}

void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing)
{
	if (!s_nEnable || !g_pRepresentShell)
		return;
	PerfHud_DocIni();
	PerfHud_LayMau();

	char sz[200];
	int nX = (s_nX >= 0) ? s_nX : (SCREEN_WIDTH / 2 - 150);
	int nY = s_nY;

	// dong 1: FPS ve | vong logic | ping | khung ve
	_snprintf(sz, sizeof(sz) - 1, "FPS %d | logic %d | ping %u ms | khung %d.%d ms (lau nhat %d ms)",
		nPaintFps, nLogicFps, dwPing, s_nFrameMs10 / 10, s_nFrameMs10 % 10, s_nFrameMaxMs10 / 10);
	sz[sizeof(sz) - 1] = 0;
	PerfHud_Chu(sz, nX, nY, (s_nFrameMaxMs10 >= 250) ? PH_COL_BAD : ((s_nFrameMaxMs10 >= 100) ? PH_COL_WARN : PerfHud_MauFps(nPaintFps)));
	nY += PH_LINE;

	// dong 2: CPU | RAM | pin
	char szPin[48];
	if (s_nPinPhanTram >= 0)
	{
		const char* szTt = (s_nPinTrangThai == SDL_POWERSTATE_CHARGING) ? " dang sac" : ((s_nPinTrangThai == SDL_POWERSTATE_CHARGED) ? " day" : "");
		_snprintf(szPin, sizeof(szPin) - 1, "pin %d%%%s", s_nPinPhanTram, szTt);
	}
	else if (s_nPinTrangThai == SDL_POWERSTATE_NO_BATTERY)
		strcpy(szPin, "pin: khong co");
	else
		strcpy(szPin, "pin n/a");
	szPin[sizeof(szPin) - 1] = 0;
	if (s_nCpuPerMille >= 0)
		_snprintf(sz, sizeof(sz) - 1, "CPU %d.%d%% (%d nhan) | RAM game %d MB, may con %d/%d MB | %s",
			s_nCpuPerMille / 10, s_nCpuPerMille % 10, s_nCores, s_nRamProcMB, s_nRamFreeMB, s_nRamTotalMB, szPin);
	else
		_snprintf(sz, sizeof(sz) - 1, "CPU n/a | RAM game %d MB, may con %d/%d MB | %s", s_nRamProcMB, s_nRamFreeMB, s_nRamTotalMB, szPin);
	sz[sizeof(sz) - 1] = 0;
	PerfHud_Chu(sz, nX, nY, PerfHud_MauTai(s_nCpuPerMille >= 0 ? s_nCpuPerMille / 10 : -1));
	nY += PH_LINE;

	// dong 3: GPU (Represent3)
	PerfHud_Chu(s_szGpu[0] ? s_szGpu : "GPU  n/a", nX, nY, PH_COL_TEXT);
}
#endif // JX_ANDROID
