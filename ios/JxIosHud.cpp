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
#include "../Sources/S3Client/Ui/Elem/UiToaDo.h"			// [THONGTIN 11/09] dong goc phai keo duoc trong Sua giao dien
#include "../Sources/S3Client/Ui/UiCase/UiToolsControlBar.h"	// thanh cong cu = dau hieu da vao the gioi
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern iRepresentShell*	g_pRepresentShell;
extern int				SCREEN_WIDTH;
extern int				SCREEN_HEIGHT;

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

//---------------------------------------------------------------------------
// [THONGTIN 11/09] Dong "60 FPS | CPU | GPU | Pin | ms" o goc PHAI-TREN, KHONG phu thuoc PerfHud.
// Doi xung voi ThongTin_Ve() cua ban Android (Platform/JxPerfHudAndroid.cpp:675) de hai ban giong nhau:
// cung khoa config [Client] ThongTinGoc, cung khoa keo "ThongTinGoc" trong Sua giao dien,
// cung neo mep phai-tren trong vung an toan (tai tho).
// Khac ban Android o hai cho, vi iOS khong cho doc:
//   - GPU %: iOS khong mo ra neu khong dung API rieng tu -> hien "-".
//   - Nhiet: khong doc duoc do C cua pin, thay bang muc nhiet he thong (NSProcessInfo.thermalState).
// Khong ve nen anh (PH_ANH_NEN cua ban Android): chu da co vien den nen van doc duoc tren moi nen.
//---------------------------------------------------------------------------
static int	s_nTtBat = -1;
static int	s_nTtX = -1, s_nTtY = -1;		// neo: mep PHAI, mep TREN; -1 = mac dinh theo vung an toan
static int	s_nTtRong = 0, s_nTtCao = 0;	// khung vua ve (de do cham khi keo)
static bool	s_bTtDangKy = false;

static int JxHud_RongChu(const char* sz)	// font 12 px: ~6,2 px moi ky tu (nhu ban Android)
{
	return (int)(strlen(sz) * 62 / 10);
}

static SDL_Window* JxHud_CuaSo(void)
{
	int n = 0;
	SDL_Window** ds = SDL_GetWindows(&n);
	SDL_Window* w = (ds && n > 0) ? ds[0] : NULL;
	if (ds) SDL_free(ds);
	return w;
}

static void ThongTin_MacDinh(int* pnX, int* pnY)	// goc phai-tren TRONG vung an toan
{
	int nPhai = SCREEN_WIDTH - 6, nTren = 4;
	SDL_Window* w = JxHud_CuaSo();
	SDL_Rect r; int nW = 0, nH = 0;
	if (w && SDL_GetWindowSafeArea(w, &r) && SDL_GetWindowSize(w, &nW, &nH) && nW > 0 && nH > 0 && r.w > 0 && r.h > 0)
	{
		nPhai = (r.x + r.w) * SCREEN_WIDTH / nW - 6;
		nTren = r.y * SCREEN_HEIGHT / nH + 4;
	}
	*pnX = nPhai; *pnY = nTren;
}
static void ThongTin_Neo(int* px, int* py) { if (s_nTtX >= 0 && s_nTtY >= 0) { *px = s_nTtX; *py = s_nTtY; } else ThongTin_MacDinh(px, py); }
#ifdef JX_MOBILE	// bon ham duoi chi dung khi dang ky keo tha (giao dien dien thoai)
static bool ThongTin_ORiengTrung(void* p, int x, int y) { int nX, nY; (void)p; ThongTin_Neo(&nX, &nY); return x >= nX - s_nTtRong && x <= nX && y >= nY && y <= nY + s_nTtCao; }
static void ThongTin_ORiengLay(void* p, int* px, int* py) { (void)p; ThongTin_Neo(px, py); }
static void ThongTin_ORiengDat(void* p, int x, int y) { (void)p; s_nTtX = x; s_nTtY = y; }
static void ThongTin_ORiengHinh(void* p, int* pl, int* pt, int* pw, int* ph) { int nX, nY; (void)p; ThongTin_Neo(&nX, &nY); *pl = nX - s_nTtRong; *pt = nY; *pw = s_nTtRong; *ph = s_nTtCao; }
#endif

static void ThongTin_Ve(int nFps, unsigned int dwPing)
{
	char sz1[32], sz2[192], szPin[48];
	int nX, nY;
	if (s_nTtBat < 0)
	{
		s_nTtBat = GetPrivateProfileInt("Client", "ThongTinGoc", 1, ".\\config.ini") ? 1 : 0;
		SDL_Log("[THONGTIN] [Client] ThongTinGoc=%d (1 = hien dong FPS/CPU/GPU/Pin goc phai-tren khi da vao the gioi)", s_nTtBat);
	}
	if (!s_nTtBat || !g_pRepresentShell || KUiToolsControlBar::GetSelf() == NULL)
		return;
#ifdef JX_MOBILE	// keo tha trong "Sua giao dien" la tinh nang cua giao dien dien thoai (UiToaDo.h rao JX_MOBILE).
	// macOS dung giao dien ban PC nen khong co trinh chinh do: chi VE dong, khong dang ky keo.
	if (!s_bTtDangKy)
	{
		s_bTtDangKy = true;
		UiToaDo_DangKyORieng("ThongTinGoc", ThongTin_ORiengTrung, ThongTin_ORiengLay, ThongTin_ORiengDat, NULL);
		UiToaDo_DangKyORiengHinh("ThongTinGoc", ThongTin_ORiengHinh);
	}
#endif

	double d = JxHud_Giay();
	if (d - s_dLanDo >= 0.5)
	{
		JxIosDo_Lay(&s_fCpu, &s_dRam, &s_dRamCon, &s_nNhiet, &s_fPin);
		s_dLanDo = d;
	}

	static const char* s_szNhietNgan[4] = { "", " am", " nong", " rat nong" };
	_snprintf(sz1, sizeof(sz1) - 1, "%d FPS", nFps); sz1[sizeof(sz1) - 1] = 0;
	if (s_fPin >= 0.0f)
		_snprintf(szPin, sizeof(szPin) - 1, "%.0f%%%s", s_fPin * 100.0f,
			(s_nNhiet > 0 && s_nNhiet <= 3) ? s_szNhietNgan[s_nNhiet] : "");
	else
		strcpy(szPin, "-");
	szPin[sizeof(szPin) - 1] = 0;
	if (s_fCpu >= 0.0f)
		_snprintf(sz2, sizeof(sz2) - 1, "  |  CPU %.0f%%  |  GPU -  |  Pin %s  |  %u ms", s_fCpu, szPin, dwPing);
	else
		_snprintf(sz2, sizeof(sz2) - 1, "  |  CPU -  |  GPU -  |  Pin %s  |  %u ms", szPin, dwPing);
	sz2[sizeof(sz2) - 1] = 0;

	const int nR1 = JxHud_RongChu(sz1), nR2 = JxHud_RongChu(sz2);
	ThongTin_Neo(&nX, &nY);
	s_nTtRong = nR1 + nR2;
	s_nTtCao  = JXH_LINE;
	const int nTrai = nX - s_nTtRong;
	JxHud_Chu(sz1, nTrai, nY, JxHud_MauFps(nFps));
	JxHud_Chu(sz2, nTrai + nR1, nY, JXH_COL_TEXT);
}

void PerfHud_Draw(int nPaintFps, int nLogicFps, unsigned int dwPing)
{
	ThongTin_Ve(nPaintFps, dwPing);	// [THONGTIN 11/09] dong goc phai: khong phu thuoc PerfHud
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
