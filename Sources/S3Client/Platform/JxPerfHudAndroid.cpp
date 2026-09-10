//---------------------------------------------------------------------------
// [ANDROID 11/09 HUD] Bang do hieu nang trong game cho Android: FPS / chi phi ve / logic / nhip / CPU / RAM / PIN / GPU.
// Chu: "them hien thi FPS o trong game - Pin - GPU nhu ban du an USVOLAM de de biet"; "lam hien thi cho co tham my de sau
// nay phat hanh luon"; "khung ve con rat cao" (16,5 ms cua va 77 la NHIP 60 Hz, khong phai chi phi ve -> va 78 do that).
// Ui/PerfHud.cpp cua ban Windows dung psapi/pdh nen bi loai khoi Android (android/lists); tep nay thay the ba ham PerfHud_*.
//   - FPS ve / vong logic / ping: UiShell.cpp dua vao (nhu Windows).
//   - ve / logic: g_uJxHudVeUs / g_uJxHudLogicUs do trong S3Client.cpp (UiPaint, Breathe+UiHeartBeat), lam muot.
//   - nhip: thoi gian giua hai lan ve (60 Hz = 16,7 ms) + te nhat trong 1 giay (khung bi rot).
//   - CPU: /proc/self/stat (utime + stime) so voi thoi gian thuc, chia so nhan. RAM: /proc/self/statm + /proc/meminfo.
//   - Pin: SDL_GetPowerInfo. GPU: Rep3_ThongKeGpu cua Represent3 (driver, texture MB, atlas, trinh chieu, lenh ve).
// Ve: nen mo (spr\ui3\uiskillcontrol\hud_nen.spr keo can) + 3 dong chu 12 px vien den. Bat/tat: [Client] PerfHud=1;
// vi tri PerfHudX / PerfHudY (-1 = tu tinh: giua, duoi hang nut menu tren cung). Chi bien dich khi JX_ANDROID.
// Chu Viet trong chuoi: TCVN3 (ghi bang vn_edit.py), khong dung Edit/Write thuong.
//---------------------------------------------------------------------------
#ifdef JX_ANDROID
#include "KWin32.h"
#include "../Ui/PerfHud.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern iRepresentShell*	g_pRepresentShell;
extern int				SCREEN_WIDTH;
extern int				SCREEN_HEIGHT;
extern unsigned			g_uJxHudLogicUs;	// S3Client.cpp [ANDROID 11/09 HUD b]
extern unsigned			g_uJxHudVeUs;

#define PH_FONT			12
#define PH_LINE			15
#define PH_LE_X			8
#define PH_LE_Y			5
#define PH_COL_TEXT		0xFFF0F0F0
#define PH_COL_NHAN		0xFFA0C8FF	// nhan (FPS, CPU...) xanh nhat
#define PH_COL_GOOD		0xFF8CFF8C
#define PH_COL_WARN		0xFFFFD040
#define PH_COL_BAD		0xFFFF6A6A
#define PH_COL_BORDER	0xFF000000
#define PH_SAMPLE_MS	500
#define PH_ANH_NEN		"\\spr\\Ui3\\UiSkillControl\\hud_nen.spr"

typedef int (*PFN_Rep3ThongKeGpu)(char* sz, int n);

static int				s_nEnable = 0;
static int				s_nX = -1, s_nY = -1;				// -1 = tu tinh (giua, duoi hang nut menu tren cung)
static int				s_nDaDocIni = 0;
static int				s_nCores = 1;
static int				s_nCpuPerMille = -1;
static int				s_nRamProcMB = -1;
static int				s_nRamFreeMB = -1;
static int				s_nRamTotalMB = -1;
static int				s_nPinPhanTram = -1;
static int				s_nPinTrangThai = 0;				// SDL_PowerState
static int				s_nNhipMs10 = 0, s_nNhipMaxMs10 = 0, s_nNhipMaxCur = 0;
static int				s_nVeUs = 0, s_nLogicUs = 0;			// lam muot
static unsigned int		s_uMaxReset = 0, s_uLastSample = 0, s_uLastFrame = 0;
static unsigned long long s_ullPrevProcTicks = 0;
static unsigned int		s_uPrevWallMs = 0;
static char				s_szGpu[200] = "";
static PFN_Rep3ThongKeGpu s_pfnGpu = NULL;
static int				s_nGpuThu = 0;
static int				s_nNenCoAnh = -1;

static void PerfHud_DocIni()
{
	if (s_nDaDocIni)
		return;
	s_nDaDocIni = 1;
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	s_nX = GetPrivateProfileInt("Client", "PerfHudX", -1, szCfg);
	s_nY = GetPrivateProfileInt("Client", "PerfHudY", -1, szCfg);
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
				for (i = 3; tok && i <= 15; i++)	// tok = truong 3 (state); 14, 15 = utime, stime
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
	f = fopen("/proc/self/statm", "r");	// RAM tien trinh: truong 2 = resident (trang)
	if (f)
	{
		unsigned long uSize = 0, uRes = 0;
		if (fscanf(f, "%lu %lu", &uSize, &uRes) == 2)
			s_nRamProcMB = (int)((unsigned long long)uRes * (unsigned long long)getpagesize() / (1024ull * 1024ull));
		fclose(f);
	}
	f = fopen("/proc/meminfo", "r");	// RAM may
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
			s_nNhipMs10 = (s_nNhipMs10 * 7 + (int)d * 30) / 10;
			if ((int)d * 10 > s_nNhipMaxCur) s_nNhipMaxCur = (int)d * 10;
		}
	}
	if (!s_uMaxReset || uNow - s_uMaxReset >= 1000)
	{
		s_uMaxReset = uNow; s_nNhipMaxMs10 = s_nNhipMaxCur; s_nNhipMaxCur = 0;
	}
	s_uLastFrame = uNow;
	s_nVeUs = (s_nVeUs * 7 + (int)g_uJxHudVeUs * 3) / 10;
	s_nLogicUs = (s_nLogicUs * 7 + (int)g_uJxHudLogicUs * 3) / 10;
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
		if (s_pfnGpu(s_szGpu, sizeof(s_szGpu)) <= 0) s_szGpu[0] = 0;
	}
	else
		s_szGpu[0] = 0;
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

// Do rong chu 12 px: uoc ~6,2 px/ky tu (font game co dinh be rong theo chu) - du de ve nen
static int PerfHud_RongChu(const char* sz)
{
	return (int)(strlen(sz) * 62 / 10);
}

static void PerfHud_VeNen(int nX, int nY, int nRong, int nCao)
{
	if (s_nNenCoAnh < 0)
	{
		KImageParam oTs; memset(&oTs, 0, sizeof(oTs));
		s_nNenCoAnh = (g_pRepresentShell->GetImageParam(PH_ANH_NEN, &oTs, ISI_T_SPR) && oTs.nNumFrames > 0) ? 1 : 0;
	}
	if (!s_nNenCoAnh)
		return;
	KRUImage a;
	memset(&a, 0, sizeof(a));
	a.nType = ISI_T_SPR;
	a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	a.Color.Color_dw = 0xffffffff;
	a.nISPosition = IMAGE_IS_POSITION_INIT;
	a.nFrame = 0;
	strncpy(a.szImage, PH_ANH_NEN, sizeof(a.szImage) - 1);
	a.oPosition.nX = nX; a.oPosition.nY = nY; a.oPosition.nZ = 0;
	a.oEndPos.nX = nX + nRong; a.oEndPos.nY = nY + nCao; a.oEndPos.nZ = 0;
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);
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

	char sz1[200], sz2[200], sz3[200], szPin[48];
	// dong 1: FPS | ve | logic | nhip (te nhat) | ping
	_snprintf(sz1, sizeof(sz1) - 1, "FPS %d   vÏ %d,%d ms   logic %d,%d ms   nhÞp %d,%d ms (tÖ nhÊt %d)   ping %u ms",
		nPaintFps, s_nVeUs / 1000, (s_nVeUs % 1000) / 100, s_nLogicUs / 1000, (s_nLogicUs % 1000) / 100,
		s_nNhipMs10 / 10, s_nNhipMs10 % 10, s_nNhipMaxMs10 / 10, dwPing);
	sz1[sizeof(sz1) - 1] = 0;
	// dong 2: CPU | RAM | pin
	if (s_nPinPhanTram >= 0)
	{
		const char* szTt = (s_nPinTrangThai == SDL_POWERSTATE_CHARGING) ? " ®ang s¹c" : ((s_nPinTrangThai == SDL_POWERSTATE_CHARGED) ? " ®Çy" : "");
		_snprintf(szPin, sizeof(szPin) - 1, "pin %d%%%s", s_nPinPhanTram, szTt);
	}
	else
		strcpy(szPin, "pin: kh«ng cã");
	szPin[sizeof(szPin) - 1] = 0;
	if (s_nCpuPerMille >= 0)
		_snprintf(sz2, sizeof(sz2) - 1, "CPU %d%% (%d nh©n)   RAM game %d MB, m¸y cßn %d / %d MB   %s",
			(s_nCpuPerMille + 5) / 10, s_nCores, s_nRamProcMB, s_nRamFreeMB, s_nRamTotalMB, szPin);
	else
		_snprintf(sz2, sizeof(sz2) - 1, "CPU --   RAM game %d MB, m¸y cßn %d / %d MB   %s", s_nRamProcMB, s_nRamFreeMB, s_nRamTotalMB, szPin);
	sz2[sizeof(sz2) - 1] = 0;
	// dong 3: GPU
	strncpy(sz3, s_szGpu[0] ? s_szGpu : "GPU --", sizeof(sz3) - 1); sz3[sizeof(sz3) - 1] = 0;

	int nRong = PerfHud_RongChu(sz1);
	if (PerfHud_RongChu(sz2) > nRong) nRong = PerfHud_RongChu(sz2);
	if (PerfHud_RongChu(sz3) > nRong) nRong = PerfHud_RongChu(sz3);
	int nCao = PH_LINE * 3;
	int nX = (s_nX >= 0) ? s_nX : (SCREEN_WIDTH - nRong) / 2;	// [ANDROID 11/09 HUD c] giua man hinh
	int nY = (s_nY >= 0) ? s_nY : 100;	// [ANDROID 11/09 HUD c] duoi hang nut menu tren cung (~95 px); goc trai-duoi de len nhat ky chat (da xem anh)

	PerfHud_VeNen(nX - PH_LE_X, nY - PH_LE_Y, nRong + PH_LE_X * 2, nCao + PH_LE_Y * 2);
	PerfHud_Chu(sz1, nX, nY, (s_nNhipMaxMs10 >= 250) ? PH_COL_BAD : ((s_nNhipMaxMs10 >= 100) ? PH_COL_WARN : PerfHud_MauFps(nPaintFps)));
	PerfHud_Chu(sz2, nX, nY + PH_LINE, PerfHud_MauTai(s_nCpuPerMille >= 0 ? s_nCpuPerMille / 10 : -1));
	PerfHud_Chu(sz3, nX, nY + PH_LINE * 2, PH_COL_NHAN);
}
#endif // JX_ANDROID
