//---------------------------------------------------------------------------
// [IOS-HUD 11/09] Bang do hien NGAY TRONG GAME cho ban iOS: FPS / CPU / GPU / RAM / nhiet / pin.
//
// Ban Android co ban rieng (Platform/JxPerfHudAndroid.cpp) doc /proc va /sys. iOS lay so bang
// API cua Apple qua ios/JxIosDoNhip.mm. Ve theo dung loi cua ban Android: chu 12 px vien den,
// goi tu PerfHud_Draw moi khung ngay truoc RepresentEnd.
//
// Bat/tat: [Client] PerfHud=1 trong config.ini. Tren Apple mac dinh BAT (chu xin xem so trong game).
// Vi tri: goc trai-tren, duoi hang nut menu ~100 px, giong ban Android.
//
// GPU %: iOS KHONG cho doc neu khong dung API rieng tu -> hien "GPU --". Thay vao do hien nhiet
// cua he thong, thu ma iOS that su dung de ha xung khi may nong.
//
// Chuoi trong tep nay CHI dung ASCII de khoi vuong bang ma TCVN3.
//---------------------------------------------------------------------------
#ifdef JX_APPLE	// dung chung cho iOS va macOS
#include "KWin32.h"
#include "../Sources/S3Client/Ui/PerfHud.h"
#include "../Sources/Represent/iRepresent/iRepresentShell.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

extern iRepresentShell*	g_pRepresentShell;
extern int				SCREEN_WIDTH;

extern "C" void JxIosDo_Lay(float* pCpuPhanTram, double* pRamMB, double* pRamConMB,
                            int* pNhiet, float* pPin);

#define JXH_FONT		12
#define JXH_LINE		15
#define JXH_X			10
#define JXH_Y			100
#define JXH_COL_TEXT	0xFFF0F0F0
#define JXH_COL_GOOD	0xFF8CFF8C
#define JXH_COL_WARN	0xFFFFD040
#define JXH_COL_BAD		0xFFFF6A6A
#define JXH_COL_VIEN	0xFF000000

static int		s_nBat = 1;			// iOS: mac dinh BAT
static float	s_fCpu = -1.0f, s_fPin = -1.0f;
static double	s_dRam = -1.0, s_dRamCon = -1.0;
static int		s_nNhiet = -1;
static double	s_dLanDo = 0.0;

void PerfHud_SetEnable(int nOn)	{ s_nBat = nOn ? 1 : 0; }
int  PerfHud_IsEnable()			{ return s_nBat; }

static double JxHud_Giay(void)
{
	struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static DWORD JxHud_MauFps(int nFps)
{
	if (nFps <= 0)  return JXH_COL_TEXT;
	if (nFps >= 50) return JXH_COL_GOOD;
	if (nFps >= 30) return JXH_COL_WARN;
	return JXH_COL_BAD;
}

static DWORD JxHud_MauTai(float fCpu)
{
	if (fCpu < 0.0f)   return JXH_COL_TEXT;
	if (fCpu < 60.0f)  return JXH_COL_GOOD;
	if (fCpu < 120.0f) return JXH_COL_WARN;
	return JXH_COL_BAD;
}

static void JxHud_Chu(const char* sz, int nX, int nY, DWORD uMau)
{
	g_pRepresentShell->OutputText(JXH_FONT, (char*)sz, KRF_ZERO_END, nX, nY, uMau, 0,
		TEXT_IN_SINGLE_PLANE_COORD, JXH_COL_VIEN);
}

void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing)
{
	if (!s_nBat || !g_pRepresentShell)
		return;

	// do lai moi nua giay: goi API he thong moi khung se ton vo ich
	double d = JxHud_Giay();
	if (d - s_dLanDo >= 0.5)
	{
		JxIosDo_Lay(&s_fCpu, &s_dRam, &s_dRamCon, &s_nNhiet, &s_fPin);
		s_dLanDo = d;
	}

	static const char* s_szNhiet[4] = { "binh thuong", "am", "nong", "nghiem trong" };
	char sz1[192], sz2[192];

	_snprintf(sz1, sizeof(sz1) - 1, "FPS %d   logic %d   ping %u ms", nPaintFps, nLogicFps, dwPing);
	sz1[sizeof(sz1) - 1] = 0;

	if (s_fCpu >= 0.0f)
		_snprintf(sz2, sizeof(sz2) - 1, "CPU %.0f%%   RAM %.0f MB (con %.0f)   nhiet %s   pin %.0f%%   GPU --",
			s_fCpu, s_dRam, s_dRamCon,
			(s_nNhiet >= 0 && s_nNhiet <= 3) ? s_szNhiet[s_nNhiet] : "?",
			s_fPin >= 0.0f ? s_fPin * 100.0f : -1.0f);
	else
		_snprintf(sz2, sizeof(sz2) - 1, "CPU --   RAM %.0f MB (con %.0f)   GPU --", s_dRam, s_dRamCon);
	sz2[sizeof(sz2) - 1] = 0;

	JxHud_Chu(sz1, JXH_X, JXH_Y, JxHud_MauFps(nPaintFps));
	JxHud_Chu(sz2, JXH_X, JXH_Y + JXH_LINE, JxHud_MauTai(s_fCpu));
}
#endif // JX_APPLE
