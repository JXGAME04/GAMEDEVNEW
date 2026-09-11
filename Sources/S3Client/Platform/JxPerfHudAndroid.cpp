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
#include "../Ui/Elem/UiToaDo.h"	// [THONGTIN 12/09] thong tin goc phai keo duoc trong Sua giao dien
#include <stdarg.h>		// [DONHIP 12/09] ban do nhip
#include <time.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdint.h>
#include "../Ui/UiCase/UiToolsControlBar.h"	// [DONHIP 12/09] thanh cong cu = dau hieu da vao the gioi (nhu JxCanDieuKhien.cpp)

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

//---------------------------------------------------------------------------
// [DONHIP 12/09] BAN DO NHIP VE tren dien thoai that. Chu: "lay log tu Fold 7 ... ban cu up apk toi tai ve test roi ban ghi log
// ve may tinh toi". Bat bang config.ini [DoNhip] Bat=1 (mac dinh 0 = khong doi gi). Vao the gioi 10 s thi chay lan luot cac pha
// (GiayMoiPha giay moi pha, LanLap vong); moi pha THEM mot thay doi so voi pha truoc (bang s_aDnPha); het thi tra ve cau hinh cu.
// Moi 10 s + cuoi moi pha ghi jx_nhip.log: cach khung ve, cho swapchain (Represent3 Rep3_DoNhipLay), dem SUBOPTIMAL / dung lai
// swapchain (hint cua SDL da va bang android/va_sdl3_donhip.py), cau hinh dang chay. JxDoNhip.java gui log ve may chu tai.
//---------------------------------------------------------------------------
void JxDoNhip_DatNhip(int nPaintFps, int nVsync, int nSmooth);	// S3Client.cpp
void JxDoNhip_LayNhip(int* pFps, int* pVsync, int* pSmooth);
typedef void    (*PFN_Rep3DoNhipDat)(int nChepKhung, int nKhungBay);
typedef int     (*PFN_Rep3DoNhipLay)(unsigned* pHist, int nBins, double* pSo, int nSo);
typedef int32_t (*PFN_AnwSetFrameRate)(void* pWin, float fHz, int8_t nTuongThich);

// [DONHIP 12/09 b] SDL Android = 3.2.30 (co bca30aa: KHONG dung lai swapchain vi VK_SUBOPTIMAL_KHR). nSdlCu = 1 -> hint
// JX_DUNG_LAI_SUBOPTIMAL=1 = hanh vi 3.2.14 (chi de doi chung). Ket qua Fold 7 10/09 (BANGIAO muc 8): bo qua SUBOPTIMAL 45 -> 120 fps
// cung muc dien; nhip PC deu hon chut; 1 khung bay KHONG on dinh (82 -> 55 fps khi CPU nang) -> mac dinh 2 khung bay.
struct DnPha { const char* szTen; int nSdlCu, nFps, nVsync, nSmooth, nChep, nBay, nXinHz; };	// -2 = giu cau hinh luc mo app
static const DnPha s_aDnPha[] = {
	{ "SDL moi, nhip cu",         0,  -2,  0,  1, 1, 2,   0 },	// 0: nhu ban phat hanh sau khi nang SDL
	{ "+nhip PC",                 0,  -2,  1,  2, 1, 2,   0 },	// 1: + PaintVsync=1, PaintSmooth=2 ([NHIP a-e] cua PC)
	{ "+bo chep khung, 2 bay",    0,  -2,  1,  2, 0, 2,   0 },	// 2: + khong chep swapchain moi khung (M2), van 2 khung bay
	{ "+xin 60 Hz, PaintFps 60",  0,  60,  1,  2, 0, 2,  60 },	// 3: nac tiet kiem pin (man 120 Hz ha 60)
	{ "1 khung bay (doi chung)",  0,  -2,  1,  2, 0, 1,   0 },	// 4: nhu 2 nhung 1 khung bay
	{ "SDL cu (doi chung)",       1,  -2,  0,  1, 1, 2,   0 },	// 5: dung lai swapchain moi khung nhu 3.2.14 (khong trong danh sach mac dinh)
};
#define DN_SO_PHA	((int)(sizeof(s_aDnPha) / sizeof(s_aDnPha[0])))
#define DN_BIN		400		// o 0,25 ms: 0..100 ms, o cuoi = tran

struct DnDem
{
	unsigned aVe[DN_BIN], aTra[DN_BIN];		// cach giua hai lan ve / hai lan tra swapchain
	unsigned uCat;							// so khung ve nhip tick cat ngang (POSSHIFT tra 2)
	double   aSo[8];						// cong don tu Rep3_DoNhipLay (o 3 lay max)
	int      nSub, nDung;					// SDL: so SUBOPTIMAL / dung lai swapchain trong khoang
	unsigned uLuc;							// luc bat dau khoang (ms)
};
static int		s_nDnBat = -1;				// -1 = chua doc ini
static int		s_nDnGiay = 60, s_nDnLap = 2, s_nDnSo = 0, s_aDnThuTu[16];
static int		s_nDnBuoc = -1, s_nDnXong = 0;	// buoc hien tai trong s_nDnSo * s_nDnLap; -1 = chua bat dau
static unsigned	s_uDnVaoGame = 0, s_uDnPhaLuc = 0, s_uDnCuaLuc = 0;
static int		s_nDnFps0 = 0, s_nDnVsync0 = 0, s_nDnSmooth0 = 1, s_nDnXin = 0;
static int		s_nDnSub0 = 0, s_nDnDung0 = 0;	// gia tri hint SDL o lan doc truoc
static Uint64	s_uDnVeTruoc = 0;
static unsigned	s_uDnVongTruoc = 0;		// [DONHIP 12/09 b] lan goi JxDoNhip_Vong truoc: cach > 2 s = app ra nen -> khong tinh vao pha
static DnDem	s_DnC, s_DnP;				// khoang 10 s / ca pha
static PFN_Rep3DoNhipDat s_pfnDnDat = NULL;
static PFN_Rep3DoNhipLay s_pfnDnLay = NULL;

static void DnGhi(const char* szThe, const char* szDinhDang, ...)
{
	FILE* f = fopen("jx_nhip.log", "a");
	if (!f)
		return;
	struct timeval tv; gettimeofday(&tv, NULL);
	struct tm t; localtime_r(&tv.tv_sec, &t);
	fprintf(f, "%s %02d:%02d:%02d.%03d t=%u ", szThe, t.tm_hour, t.tm_min, t.tm_sec, (int)(tv.tv_usec / 1000), (unsigned)SDL_GetTicks());
	va_list ap; va_start(ap, szDinhDang); vfprintf(f, szDinhDang, ap); va_end(ap);
	fputc('\n', f);
	fclose(f);
}

static int DnHint(const char* szTen)
{
	const char* s = SDL_GetHint(szTen);
	return s ? atoi(s) : 0;
}

static SDL_Window* DnCuaSo()
{
	int n = 0;
	SDL_Window** ds = SDL_GetWindows(&n);
	SDL_Window* w = (ds && n > 0) ? ds[0] : NULL;
	if (ds) SDL_free(ds);
	return w;
}

static unsigned DnTong(const unsigned* a) { unsigned t = 0; for (int i = 0; i < DN_BIN; i++) t += a[i]; return t; }
static unsigned DnTren(const unsigned* a, double dMs) { unsigned t = 0; int k = (int)(dMs * 4.0); for (int i = (k < 0 ? 0 : k); i < DN_BIN; i++) t += a[i]; return t; }
static double DnMax(const unsigned* a) { for (int i = DN_BIN - 1; i >= 0; i--) if (a[i]) return (i + 1) * 0.25; return 0.0; }
static double DnPhanVi(const unsigned* a, double dPhan)	// phan vi (ms) tu bieu do o 0,25 ms
{
	const unsigned tong = DnTong(a);
	if (!tong)
		return 0.0;
	unsigned can = (unsigned)(dPhan * (double)tong + 0.5), cd = 0;
	if (can < 1) can = 1;
	for (int i = 0; i < DN_BIN; i++) { cd += a[i]; if (cd >= can) return (i + 0.5) * 0.25; }
	return (DN_BIN - 0.5) * 0.25;
}

static void DnXoa(DnDem& d, unsigned uLuc) { memset(&d, 0, sizeof(d)); d.uLuc = uLuc; }

static void DnGop(DnDem& p, const DnDem& c)
{
	for (int i = 0; i < DN_BIN; i++) { p.aVe[i] += c.aVe[i]; p.aTra[i] += c.aTra[i]; }
	p.uCat += c.uCat;
	p.aSo[0] += c.aSo[0]; p.aSo[1] += c.aSo[1]; p.aSo[2] += c.aSo[2]; p.aSo[4] += c.aSo[4];
	if (c.aSo[3] > p.aSo[3]) p.aSo[3] = c.aSo[3];
	if (c.aSo[5] > 0) { p.aSo[5] = c.aSo[5]; p.aSo[6] = c.aSo[6]; }
	p.aSo[7] = c.aSo[7];
	p.nSub += c.nSub; p.nDung += c.nDung;
}

// doc Represent3 + bo dem SDL vao khoang hien tai (s_DnC); Represent3 tu dat lai sau khi doc
static void DnLayRep3()
{
	static unsigned aTra[DN_BIN];
	double aSo[8] = { 0 };
	if (s_pfnDnLay) s_pfnDnLay(aTra, DN_BIN, aSo, 8); else memset(aTra, 0, sizeof(aTra));
	DnDem c; memset(&c, 0, sizeof(c));
	memcpy(c.aTra, aTra, sizeof(aTra));
	memcpy(c.aSo, aSo, sizeof(aSo));
	const int nSub = DnHint("JX_DEM_SUBOPTIMAL"), nDung = DnHint("JX_DEM_DUNG_LAI_SWAPCHAIN");
	c.nSub = nSub - s_nDnSub0; c.nDung = nDung - s_nDnDung0;
	s_nDnSub0 = nSub; s_nDnDung0 = nDung;
	DnGop(s_DnC, c);
}

static void DnGhiDem(const char* szThe, const DnDem& d, unsigned uNay, int nPha)
{
	int nFps = 0, nVsync = 0, nSmooth = 0;
	JxDoNhip_LayNhip(&nFps, &nVsync, &nSmooth);
	const double dT = 1000.0 / (double)(nFps > 0 ? nFps : 60);
	const unsigned uVe = DnTong(d.aVe);
	const double dGiay = (uNay > d.uLuc) ? (uNay - d.uLuc) / 1000.0 : 0.0;
	DnGhi(szThe, "pha %d (%s) buoc %d/%d %.0f s | ve %u khung %.1f/s: cach p50 %.2f p95 %.2f p99 %.2f max %.1f ms, tre>1.5T %u, cat ngang %u"
		" | swapchain %u lan (khong co %u): cho TB %.2f max %.1f ms, >4ms %u; cach tra p50 %.2f p95 %.2f p99 %.2f max %.1f ms, tre>1.5T %u"
		" | SDL suboptimal +%d dung lai +%d | cfg PaintFps %d vsync %d smooth %d chep %d bay %d xin %d Hz | swapchain %dx%d",
		nPha, s_aDnPha[nPha].szTen, s_nDnBuoc + 1, s_nDnSo * s_nDnLap, dGiay,
		uVe, dGiay > 0.0 ? uVe / dGiay : 0.0, DnPhanVi(d.aVe, 0.5), DnPhanVi(d.aVe, 0.95), DnPhanVi(d.aVe, 0.99), DnMax(d.aVe), DnTren(d.aVe, dT * 1.5), d.uCat,
		(unsigned)d.aSo[0], (unsigned)d.aSo[1], d.aSo[0] > 0.0 ? d.aSo[2] / d.aSo[0] : 0.0, d.aSo[3], (unsigned)d.aSo[4],
		DnPhanVi(d.aTra, 0.5), DnPhanVi(d.aTra, 0.95), DnPhanVi(d.aTra, 0.99), DnMax(d.aTra), DnTren(d.aTra, dT * 1.5),
		d.nSub, d.nDung, nFps, nVsync, nSmooth, s_aDnPha[nPha].nChep, (int)d.aSo[7], s_nDnXin, (int)d.aSo[5], (int)d.aSo[6]);
}

// ANativeWindow_setFrameRate (Android 11+, dlsym vi minSdk 24): xin man chay nHz; 0 = bo yeu cau (he thong tu chon)
static void DnXinHz(int nHz)
{
	static PFN_AnwSetFrameRate s_pfn = NULL;
	static int s_nDaTim = 0;
	if (!s_nDaTim)
	{
		s_nDaTim = 1;
		void* h = dlopen("libandroid.so", RTLD_NOW);
		if (h) s_pfn = (PFN_AnwSetFrameRate)dlsym(h, "ANativeWindow_setFrameRate");
	}
	SDL_Window* w = DnCuaSo();
	void* nw = w ? SDL_GetPointerProperty(SDL_GetWindowProperties(w), SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, NULL) : NULL;
	const int r = (s_pfn && nw) ? (int)s_pfn(nw, (float)nHz, 0) : -9999;
	s_nDnXin = nHz;
	DnGhi("[NHIP-XIN]", "ANativeWindow_setFrameRate(%d Hz) -> %d%s", nHz, r, s_pfn ? "" : " (khong co ham: Android < 11)");
}

static void DnApPha(int nPha, unsigned uNay)
{
	const DnPha& p = s_aDnPha[nPha];
	SDL_SetHint("JX_DUNG_LAI_SUBOPTIMAL", p.nSdlCu ? "1" : "0");	// [DONHIP 12/09 b] 1 = dung lai swapchain nhu 3.2.14 (doi chung)
	JxDoNhip_DatNhip(p.nFps == -2 ? s_nDnFps0 : p.nFps, p.nVsync == -2 ? s_nDnVsync0 : p.nVsync, p.nSmooth == -2 ? s_nDnSmooth0 : p.nSmooth);
	if (s_pfnDnDat) s_pfnDnDat(p.nChep, p.nBay);
	if (p.nXinHz != s_nDnXin) DnXinHz(p.nXinHz);
	DnLayRep3();	// bo so lieu luc chuyen pha
	DnXoa(s_DnC, uNay);
	DnXoa(s_DnP, uNay);
	s_uDnVeTruoc = 0; s_uDnPhaLuc = uNay; s_uDnCuaLuc = uNay;
	DnGhi("[NHIP-PHA-BAT]", "pha %d (%s) buoc %d/%d", nPha, p.szTen, s_nDnBuoc + 1, s_nDnSo * s_nDnLap);
}

static void DnDocIni()
{
	s_nDnBat = GetPrivateProfileInt("DoNhip", "Bat", 0, ".\\config.ini") ? 1 : 0;
	if (!s_nDnBat)
		return;
	s_nDnGiay = GetPrivateProfileInt("DoNhip", "GiayMoiPha", 60, ".\\config.ini");
	if (s_nDnGiay < 10) s_nDnGiay = 10;
	if (s_nDnGiay > 600) s_nDnGiay = 600;
	s_nDnLap = GetPrivateProfileInt("DoNhip", "LanLap", 2, ".\\config.ini");
	if (s_nDnLap < 1) s_nDnLap = 1;
	if (s_nDnLap > 10) s_nDnLap = 10;
	char sz[128] = "";
	GetPrivateProfileString("DoNhip", "Pha", "0,1,2,3,4", sz, sizeof(sz), ".\\config.ini");	// [DONHIP 12/09 b] pha 5 (SDL cu) chi khi ghi ro
	s_nDnSo = 0;
	for (char* p = sz; *p && s_nDnSo < 16; )
	{
		if (*p < '0' || *p > '9') { p++; continue; }
		const int n = (int)strtol(p, &p, 10);
		if (n >= 0 && n < DN_SO_PHA) s_aDnThuTu[s_nDnSo++] = n;
	}
	if (!s_nDnSo) { s_aDnThuTu[0] = 0; s_nDnSo = 1; }
}

static void DnBatDau(unsigned uNay)
{
	JxDoNhip_LayNhip(&s_nDnFps0, &s_nDnVsync0, &s_nDnSmooth0);
	HMODULE h = GetModuleHandleA("Represent3.dll");	// lop tuong thich: dlopen("libRepresent3.so")
	if (h)
	{
		s_pfnDnDat = (PFN_Rep3DoNhipDat)GetProcAddress(h, "Rep3_DoNhipDat");
		s_pfnDnLay = (PFN_Rep3DoNhipLay)GetProcAddress(h, "Rep3_DoNhipLay");
	}
	s_nDnSub0 = DnHint("JX_DEM_SUBOPTIMAL");
	s_nDnDung0 = DnHint("JX_DEM_DUNG_LAI_SWAPCHAIN");
	SDL_Window* w = DnCuaSo();
	int nW = 0, nH = 0;
	if (w) SDL_GetWindowSizeInPixels(w, &nW, &nH);
	const SDL_DisplayID id = w ? SDL_GetDisplayForWindow(w) : 0;
	const SDL_DisplayMode* m = id ? SDL_GetCurrentDisplayMode(id) : NULL;
	char szThuTu[64] = "";
	for (int i = 0; i < s_nDnSo; i++) { size_t k = strlen(szThuTu); snprintf(szThuTu + k, sizeof(szThuTu) - k, i ? ",%d" : "%d", s_aDnThuTu[i]); }
	DnGhi("[NHIP-BAT]", "cua so %dx%d px, khung ve %dx%d, SDL bao man %.2f Hz, huong goc %d hien tai %d | luc mo app: PaintFps %d vsync %d smooth %d"
		" | Represent3: Dat %s Lay %s | SDL dem: suboptimal %d dung lai %d%s | pha %s x %d s x %d vong",
		nW, nH, SCREEN_WIDTH, SCREEN_HEIGHT, m ? m->refresh_rate : 0.0f,
		id ? (int)SDL_GetNaturalDisplayOrientation(id) : -1, id ? (int)SDL_GetCurrentDisplayOrientation(id) : -1,
		s_nDnFps0, s_nDnVsync0, s_nDnSmooth0, s_pfnDnDat ? "co" : "KHONG", s_pfnDnLay ? "co" : "KHONG",
		s_nDnSub0, s_nDnDung0, (SDL_GetHint("JX_DEM_SUBOPTIMAL") || SDL_GetHint("JX_DEM_DUNG_LAI_SWAPCHAIN")) ? "" : " (chua co hint: SDL chua va hoac chua co su kien)",
		szThuTu, s_nDnGiay, s_nDnLap);
	s_nDnBuoc = 0;
	DnApPha(s_aDnThuTu[0], uNay);
}

static void DnKetThuc()
{
	SDL_SetHint("JX_DUNG_LAI_SUBOPTIMAL", "0");	// [DONHIP 12/09 b] tra ve hanh vi SDL 3.2.30
	JxDoNhip_DatNhip(s_nDnFps0, s_nDnVsync0, s_nDnSmooth0);
	if (s_pfnDnDat) s_pfnDnDat(0, 2);	// [DONHIP 12/09 d] mac dinh Android: khong chep khung, 2 khung bay
	if (s_nDnXin) DnXinHz(0);
	s_nDnXong = 1;
	DnGhi("[NHIP-XONG]", "het %d buoc - tra ve cau hinh luc mo app (PaintFps %d vsync %d smooth %d, khong chep khung, 2 khung bay, khong xin tan so)",
		s_nDnSo * s_nDnLap, s_nDnFps0, s_nDnVsync0, s_nDnSmooth0);
}

// S3Client.cpp GameLoop: moi vong bom
void JxDoNhip_Vong(void)
{
	if (s_nDnBat < 0)
		DnDocIni();
	if (s_nDnBat <= 0 || s_nDnXong)
		return;
	const unsigned uNay = (unsigned)SDL_GetTicks();
	if (s_nDnBuoc >= 0 && s_uDnVongTruoc && uNay - s_uDnVongTruoc > 2000)
	{	// [DONHIP 12/09 b] app ra nen (SDL chan vong lap) -> doi moc thoi gian, khong tinh vao pha (Fold 7 10/09: buoc 1 keo dai 412 s)
		const unsigned uNghi = uNay - s_uDnVongTruoc;
		s_uDnPhaLuc += uNghi; s_uDnCuaLuc += uNghi; s_DnC.uLuc += uNghi; s_DnP.uLuc += uNghi;
		s_uDnVeTruoc = 0;
		DnGhi("[NHIP-NGHI]", "app ra nen %u ms - khong tinh vao pha", uNghi);
	}
	s_uDnVongTruoc = uNay;
	if (s_nDnBuoc < 0)
	{
		if (KUiToolsControlBar::GetSelf() == NULL) { s_uDnVaoGame = 0; return; }
		if (!s_uDnVaoGame) { s_uDnVaoGame = uNay ? uNay : 1; return; }
		if (uNay - s_uDnVaoGame >= 10000)
			DnBatDau(uNay);
		return;
	}
	const int nPha = s_aDnThuTu[s_nDnBuoc % s_nDnSo];
	const bool bHetPha = (uNay - s_uDnPhaLuc) >= (unsigned)s_nDnGiay * 1000u;
	if (bHetPha || uNay - s_uDnCuaLuc >= 10000)
	{
		DnLayRep3();
		DnGhiDem("[NHIP]", s_DnC, uNay, nPha);
		DnGop(s_DnP, s_DnC);
		DnXoa(s_DnC, uNay);
		s_uDnCuaLuc = uNay;
	}
	if (!bHetPha)
		return;
	DnGhiDem("[NHIP-PHA]", s_DnP, uNay, nPha);
	s_nDnBuoc++;
	if (s_nDnBuoc >= s_nDnSo * s_nDnLap) { DnKetThuc(); return; }
	DnApPha(s_aDnThuTu[s_nDnBuoc % s_nDnSo], uNay);
}

// S3Client.cpp GameLoop: ngay truoc UiPaint cua nhanh PaintFps
void JxDoNhip_KhungVe(int nCatNgang)
{
	if (s_nDnBat <= 0 || s_nDnBuoc < 0 || s_nDnXong)
		return;
	const Uint64 u = SDL_GetPerformanceCounter();
	if (s_uDnVeTruoc)
	{
		const int b = (int)((double)(u - s_uDnVeTruoc) * 4000.0 / (double)SDL_GetPerformanceFrequency());
		s_DnC.aVe[b < 0 ? 0 : (b >= DN_BIN ? DN_BIN - 1 : b)]++;
	}
	s_uDnVeTruoc = u;
	if (nCatNgang)
		s_DnC.uCat++;
}

// dong chu vang giua man hinh: pha dang do (ve ca khi tat bang do PerfHud)
static void DoNhip_VeNhan()
{
	if (s_nDnBat <= 0 || !g_pRepresentShell)
		return;
	char sz[200];
	if (s_nDnXong)
		snprintf(sz, sizeof(sz), "DO NHIP: xong %d buoc - da tra ve cau hinh cu, log gui ve may tinh", s_nDnSo * s_nDnLap);
	else if (s_nDnBuoc < 0)
		snprintf(sz, sizeof(sz), "DO NHIP: vao the gioi 10 s thi bat dau (%d pha x %d s x %d vong)", s_nDnSo, s_nDnGiay, s_nDnLap);
	else
	{
		const int nPha = s_aDnThuTu[s_nDnBuoc % s_nDnSo];
		const int nCon = s_nDnGiay - (int)(((unsigned)SDL_GetTicks() - s_uDnPhaLuc) / 1000);
		snprintf(sz, sizeof(sz), "DO NHIP  pha %d: %s   buoc %d/%d   con %d s", nPha, s_aDnPha[nPha].szTen, s_nDnBuoc + 1, s_nDnSo * s_nDnLap, nCon < 0 ? 0 : nCon);
	}
	const int nRong = PerfHud_RongChu(sz);
	const int nX = (SCREEN_WIDTH - nRong) / 2;
	const int nY = ((s_nY >= 0) ? s_nY : 100) + (s_nEnable ? PH_LINE * 3 + PH_LE_Y * 2 + 6 : 0);
	PerfHud_VeNen(nX - PH_LE_X, nY - PH_LE_Y, nRong + PH_LE_X * 2, PH_LINE + PH_LE_Y * 2);
	PerfHud_Chu(sz, nX, nY, PH_COL_WARN);
}

//---------------------------------------------------------------------------
// [FPS 12/09] Muc khung hinh/giay nguoi choi chon trong Cai dat (UiOptions.cpp): 0 = tu dong theo man hinh (chu: "khong co
// mac dinh, tuy cau hinh may"), 1..5 = 30/45/60/90/120. Ap = PaintFps (JxDoNhip_DatNhip) + xin tan so man hinh
// (ANativeWindow_setFrameRate: chon 60 tren man 120 Hz thi man ha 60 -> do pin). Luu UserData\UiCommon.ini [Options] FpsMuc.
//---------------------------------------------------------------------------
static const int s_aFpsMuc[] = { 0, 30, 45, 60, 90, 120 };
#define FPS_SO_MUC	((int)(sizeof(s_aFpsMuc) / sizeof(s_aFpsMuc[0])))
static int s_nFpsMuc = 0;

static int Nhip_ManHz()	// tan so man hinh SDL bao hien tai (59 -> 60; khong biet -> 60)
{
	SDL_Window* w = DnCuaSo();
	const SDL_DisplayID id = w ? SDL_GetDisplayForWindow(w) : 0;
	const SDL_DisplayMode* m = id ? SDL_GetCurrentDisplayMode(id) : NULL;
	int nHz = (m && m->refresh_rate > 0.0f) ? (int)(m->refresh_rate + 0.5f) : 60;
	if (nHz == 59) nHz = 60;
	if (nHz < 30) nHz = 60;
	return nHz;
}

void JxNhip_DatMuc(int nMuc)
{
	if (nMuc < 0) nMuc = 0;
	if (nMuc >= FPS_SO_MUC) nMuc = FPS_SO_MUC - 1;
	s_nFpsMuc = nMuc;
	const int nFps = s_aFpsMuc[nMuc] ? s_aFpsMuc[nMuc] : Nhip_ManHz();
	JxDoNhip_DatNhip(nFps, -1, -1);
	DnXinHz(s_aFpsMuc[nMuc]);	// 0 = bo yeu cau (he thong tu chon)
	SDL_Log("[FPS] muc %d -> PaintFps %d, xin man %d Hz", nMuc, nFps, s_aFpsMuc[nMuc]);
}

void JxNhip_ChuMuc(int nMuc, char* sz, int n)
{
	if (!sz || n < 8) return;
	if (nMuc <= 0 || nMuc >= FPS_SO_MUC)
		snprintf(sz, (size_t)n, "T\371 \256\351ng (%d)", Nhip_ManHz());	// "Tu dong (120)" - TCVN3 viet bang octal
	else
		snprintf(sz, (size_t)n, "%d", s_aFpsMuc[nMuc]);
	sz[n - 1] = 0;
}

void JxNhip_VeNen(int nX, int nY, int nRong, int nCao) { PerfHud_VeNen(nX, nY, nRong, nCao); }

//---------------------------------------------------------------------------
// [THONGTIN 12/09] Thong tin goc PHAI tren man hinh nhu game mobile khac (chu: "hien FPS - CPU - GPU - pin o goc phai ... chu
// khong phai che do PerfHud"): mot dong "60 FPS | CPU 23% | GPU 41% | Pin 87% 34C | 25 ms". Hien khi da vao the gioi;
// [Client] ThongTinGoc=0 de tat; keo duoc trong "Sua giao dien" (khoa ThongTinGoc, neo = mep phai-tren, UserData\UiToaDo.ini).
// GPU % doc sysfs (Adreno kgsl / Mali / Samsung /sys/kernel/gpu); may khong cho doc thi hien "-".
//---------------------------------------------------------------------------
static int		s_nTtBat = -1;
static int		s_nTtX = -1, s_nTtY = -1;			// neo: mep PHAI, mep TREN (toa do khung ve); -1 = mac dinh theo vung an toan
static int		s_nTtRong = 0, s_nTtCao = 0;		// khung vua ve (de do cham khi keo)
static unsigned	s_uTtMauLuc = 0;
static int		s_nTtGpu = -1, s_nTtGpuTim = 0;		// % GPU; tim duong: 0 chua, 1 co, -1 khong co
static char		s_szTtGpuDuong[96] = "";
static int		s_nTtPin = -1, s_nTtPinNhiet = -1000;	// %, phan muoi do C
static bool		s_bTtDangKy = false;

static int ThongTin_DocDong(const char* szDuong, char* sz, int n)
{
	FILE* f = fopen(szDuong, "r");
	if (!f) return 0;
	sz[0] = 0;
	const int ok = fgets(sz, n, f) != NULL;
	fclose(f);
	return ok;
}

static int ThongTin_DocGpu()
{
	static const char* aDuong[] = {
		"/sys/class/kgsl/kgsl-3d0/gpu_busy_percentage",	// Adreno (Qualcomm)
		"/sys/class/kgsl/kgsl-3d0/gpubusy",				// Adreno cu: "ban tong" ke tu lan doc truoc
		"/sys/kernel/gpu/gpu_busy",						// Samsung
		"/sys/class/misc/mali0/device/utilization",		// Mali
		"/sys/devices/platform/mali.0/utilization",
		"/sys/module/mali/parameters/mali_utilization",
	};
	char sz[96];
	unsigned long long a = 0, b = 0;
	if (s_nTtGpuTim < 0) return -1;
	if (s_nTtGpuTim == 0)
	{
		s_nTtGpuTim = -1;
		for (int i = 0; i < (int)(sizeof(aDuong) / sizeof(aDuong[0])); i++)
			if (ThongTin_DocDong(aDuong[i], sz, sizeof(sz))) { strncpy(s_szTtGpuDuong, aDuong[i], sizeof(s_szTtGpuDuong) - 1); s_nTtGpuTim = 1; break; }
		SDL_Log("[THONGTIN] GPU %%: %s", s_nTtGpuTim > 0 ? s_szTtGpuDuong : "khong doc duoc sysfs nao");
		if (s_nTtGpuTim < 0) return -1;
	}
	if (!ThongTin_DocDong(s_szTtGpuDuong, sz, sizeof(sz))) return -1;
	const int n = sscanf(sz, "%llu %llu", &a, &b);
	if (n < 1) return -1;
	if (n >= 2 && b > 0) return (int)(a * 100ull / b);
	return (a > 100) ? 100 : (int)a;
}

static void ThongTin_LayMau()
{
	const unsigned uNay = (unsigned)SDL_GetTicks();
	char sz[32];
	if (s_uTtMauLuc && uNay - s_uTtMauLuc < PH_SAMPLE_MS) return;
	s_uTtMauLuc = uNay;
	if (!s_nEnable) PerfHud_LayMauCpuRam();	// bang do PerfHud dang bat thi no da lay CPU roi
	s_nTtGpu = ThongTin_DocGpu();
	{ int nGiay = 0, nPt = -1; SDL_GetPowerInfo(&nGiay, &nPt); s_nTtPin = nPt; }
	s_nTtPinNhiet = ThongTin_DocDong("/sys/class/power_supply/battery/temp", sz, sizeof(sz)) ? atoi(sz) : -1000;
}

static void ThongTin_MacDinh(int* pnX, int* pnY)	// neo mac dinh: goc phai-tren TRONG vung an toan (tai tho / thanh trang thai)
{
	int nPhai = SCREEN_WIDTH - 6, nTren = 4;
	SDL_Window* w = DnCuaSo();
	SDL_Rect r; int nW = 0, nH = 0;
	if (w && SDL_GetWindowSafeArea(w, &r) && SDL_GetWindowSize(w, &nW, &nH) && nW > 0 && nH > 0 && r.w > 0 && r.h > 0)
	{
		nPhai = (r.x + r.w) * SCREEN_WIDTH / nW - 6;
		nTren = r.y * SCREEN_HEIGHT / nH + 4;
	}
	*pnX = nPhai; *pnY = nTren;
}
static void ThongTin_Neo(int* px, int* py) { if (s_nTtX >= 0 && s_nTtY >= 0) { *px = s_nTtX; *py = s_nTtY; } else ThongTin_MacDinh(px, py); }
static bool ThongTin_ORiengTrung(void* p, int x, int y) { int nX, nY; (void)p; ThongTin_Neo(&nX, &nY); return x >= nX - s_nTtRong && x <= nX && y >= nY && y <= nY + s_nTtCao; }
static void ThongTin_ORiengLay(void* p, int* px, int* py) { (void)p; ThongTin_Neo(px, py); }
static void ThongTin_ORiengDat(void* p, int x, int y) { (void)p; s_nTtX = x; s_nTtY = y; }
// [SUAGD 13/09] hinh chu nhat that cua dong (neo mep phai-tren) cho trinh chinh giao dien moi: khung, dung sai cham, kep vao vung an toan
static void ThongTin_ORiengHinh(void* p, int* pl, int* pt, int* pw, int* ph) { int nX, nY; (void)p; ThongTin_Neo(&nX, &nY); *pl = nX - s_nTtRong; *pt = nY; *pw = s_nTtRong; *ph = s_nTtCao; }

static void ThongTin_Ve(int nFps, unsigned int dwPing)
{
	char sz1[32], sz2[160], szGpu[16], szPin[32];
	int nX, nY;
	if (s_nTtBat < 0)
		s_nTtBat = GetPrivateProfileInt("Client", "ThongTinGoc", 1, ".\\config.ini") ? 1 : 0;
	if (!s_nTtBat || !g_pRepresentShell || KUiToolsControlBar::GetSelf() == NULL)
		return;
	if (!s_bTtDangKy)
	{
		s_bTtDangKy = true;
		UiToaDo_DangKyORieng("ThongTinGoc", ThongTin_ORiengTrung, ThongTin_ORiengLay, ThongTin_ORiengDat, NULL);
		UiToaDo_DangKyORiengHinh("ThongTinGoc", ThongTin_ORiengHinh);	// [SUAGD 13/09] co trong danh sach trang ui/uitoado_danhsach.ini -> keo duoc
	}
	ThongTin_LayMau();
	snprintf(sz1, sizeof(sz1), "%d FPS", nFps);
	if (s_nTtGpu >= 0) snprintf(szGpu, sizeof(szGpu), "%d%%", s_nTtGpu); else strcpy(szGpu, "-");
	if (s_nTtPin >= 0 && s_nTtPinNhiet > -1000) snprintf(szPin, sizeof(szPin), "%d%% %dC", s_nTtPin, (s_nTtPinNhiet + 5) / 10);
	else if (s_nTtPin >= 0) snprintf(szPin, sizeof(szPin), "%d%%", s_nTtPin);
	else strcpy(szPin, "-");
	if (s_nCpuPerMille >= 0) snprintf(sz2, sizeof(sz2), "  |  CPU %d%%  |  GPU %s  |  Pin %s  |  %u ms", (s_nCpuPerMille + 5) / 10, szGpu, szPin, dwPing);
	else snprintf(sz2, sizeof(sz2), "  |  CPU -  |  GPU %s  |  Pin %s  |  %u ms", szGpu, szPin, dwPing);
	const int nR1 = PerfHud_RongChu(sz1), nR2 = PerfHud_RongChu(sz2);
	ThongTin_Neo(&nX, &nY);
	s_nTtRong = nR1 + nR2 + PH_LE_X * 2;
	s_nTtCao = PH_LINE + PH_LE_Y * 2 - 4;
	const int nTrai = nX - s_nTtRong;
	PerfHud_VeNen(nTrai, nY, s_nTtRong, s_nTtCao);
	PerfHud_Chu(sz1, nTrai + PH_LE_X, nY + PH_LE_Y - 2, PerfHud_MauFps(nFps));
	PerfHud_Chu(sz2, nTrai + PH_LE_X + nR1, nY + PH_LE_Y - 2, PH_COL_TEXT);
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
	DoNhip_VeNhan();	// [DONHIP 12/09] ten pha dang do (ca khi tat bang do)
	ThongTin_Ve(nPaintFps, dwPing);	// [THONGTIN 12/09] FPS | CPU | GPU | Pin | ping o goc phai (khong phu thuoc PerfHud)
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
