# -*- coding: utf-8 -*-
#
# [DONHIP 12/09] Ban DO NHIP VE tren dien thoai that. Chu 12/09: "Lay log tu Fold 7 bang: toi dung chung mang nen ban cu up apk
# toi tai ve test roi ban ghi log ve may tinh toi". CHI JX_ANDROID - Game.exe PC (D3D11) va GameSDL.exe khong doi hanh vi.
#   S3Client.cpp         : JxDoNhip_DatNhip / JxDoNhip_LayNhip (doi / doc cau hinh nhip ve giua chung); goi JxDoNhip_Vong moi
#                          vong GameLoop va JxDoNhip_KhungVe ngay truoc UiPaint cua nhanh PaintFps.
#   D3D9onGPUDev.cpp     : do thoi gian cho swapchain + cach giua hai lan tra swapchain; cong tac bo chep swapchain moi khung;
#                          xuat "C" Rep3_DoNhipDat (chep khung, so khung bay) / Rep3_DoNhipLay (so lieu).
#   JxPerfHudAndroid.cpp : bo dieu khien pha + ghi jx_nhip.log + dong chu ten pha tren man hinh.
# Di kem: android/va_sdl3_donhip.py (dem SUBOPTIMAL / dung lai swapchain trong SDL), JxDoNhip.java (log thiet bi + gui ve may chu
# tai), may_chu_tai_du_lieu.py (POST /nhatky), config.ini [DoNhip] (mac dinh Bat=0 = khong doi gi).
# Nguon co byte cao (TCVN3/GBK): doc/ghi latin-1, so byte cao truoc/sau phai bang nhau, moi moc phai khop dung 1 cho.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[DONHIP 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def khoi(txt):
    """khoi C++ trong chuoi raw -> danh sach dong (bo dong trong dau/cuoi do dau ''' tao ra)"""
    ds = txt.split("\n")
    if ds and ds[0] == "":
        ds = ds[1:]
    if ds and ds[-1] == "":
        ds = ds[:-1]
    return ds


def mot(L, sub, ten, loc=None):
    ds = [i for i, x in enumerate(L) if sub in x and (loc is None or loc(i))]
    if len(ds) != 1:
        raise SystemExit("moc '%s' khop %d cho (can dung 1): %s" % (sub, len(ds), ten))
    return ds[0]


def le(dong):
    return dong[: len(dong) - len(dong.lstrip(" \t"))]


def va(p, ham):
    s = doc(p)
    if DAU in s:
        print("da va roi, bo qua:", p)
        return
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    L = s.split(nl)
    L2 = ham(L)
    s2 = nl.join(L2)
    if cao(s) != cao(s2):
        raise SystemExit("so byte cao doi o %s (%d -> %d) - khong ghi" % (p, cao(s), cao(s2)))
    ghi(p, s2)
    print("da va: %s (%d -> %d dong, byte cao %d)" % (p, len(L), len(L2), cao(s2)))


# ================================================================ S3Client.cpp
S3_KHOI = r'''
#ifdef JX_ANDROID
// [DONHIP 12/09] Ban do nhip ve tren may that (bo dieu khien: Platform/JxPerfHudAndroid.cpp, bat bang config.ini [DoNhip] Bat=1):
// doi cau hinh nhip ve giua chung (tung pha do) va doc lai cau hinh luc mo app. Chi goi tu luong chinh (GameLoop).
void JxDoNhip_DatNhip(int nPaintFps, int nVsync, int nSmooth)
{
	if (nPaintFps >= 0) g_nPaintFps = (nPaintFps > 240) ? 240 : nPaintFps;
	if (nVsync >= 0) g_nPaintVsync = nVsync;
	if (nSmooth >= 0) g_nPaintSmooth = nSmooth;
	g_SetLoopInterval((g_nPaintFps > 60 || g_nPaintVsync > 0) ? 1 : 8);	// nhu GameInit: luoi vong bom 1 ms khi ve > 60 fps hoac vsync
}
void JxDoNhip_LayNhip(int* pFps, int* pVsync, int* pSmooth)
{
	if (pFps) *pFps = g_nPaintFps;
	if (pVsync) *pVsync = g_nPaintVsync;
	if (pSmooth) *pSmooth = g_nPaintSmooth;
}
void JxDoNhip_Vong(void);				// JxPerfHudAndroid.cpp: moi vong GameLoop (doi pha, ghi jx_nhip.log)
void JxDoNhip_KhungVe(int nCatNgang);	// JxPerfHudAndroid.cpp: ngay truoc UiPaint cua nhanh PaintFps (do cach khung ve)
#endif
'''


def va_s3(L):
    i = mot(L, "g_nPaintLog = 0;", "khai bao g_nPaintLog", lambda k: "[Client] PaintLog" in L[k])
    L = L[:i + 1] + khoi(S3_KHOI) + L[i + 1:]
    i = mot(L, "JxWAuto_NhipVongLap();", "goi JxWAuto_NhipVongLap trong GameLoop")
    L = L[:i + 1] + [le(L[i]) + "JxDoNhip_Vong();\t// [DONHIP 12/09] ban do nhip: doi pha / ghi jx_nhip.log (khong lam gi khi [DoNhip] Bat=0)"] + L[i + 1:]
    i = mot(L, "UiPaint(nGameFps); g_uJxHudVeUs = JxHudUs(liHudVe); }", "UiPaint cua nhanh PaintFps",
            lambda k: any("bPainted = TRUE;" in L[m] for m in range(k + 1, min(k + 6, len(L)))))
    if L[i - 1].strip() != "#ifdef JX_ANDROID":
        raise SystemExit("dong truoc UiPaint nhanh PaintFps khong phai #ifdef JX_ANDROID: " + L[i - 1])
    L = L[:i] + [le(L[i]) + "JxDoNhip_KhungVe(nLogCross);\t// [DONHIP 12/09] moc khung ve cho ban do nhip (khong lam gi khi [DoNhip] Bat=0)"] + L[i:]
    return L


# ================================================================ D3D9onGPUDev.cpp
REP3_TREN = r'''
// [DONHIP 12/09] ban do nhip tren may that (JxPerfHudAndroid.cpp goi Rep3_DoNhipDat / Rep3_DoNhipLay qua GetProcAddress)
static int      s_nJxChepKhung = 1;		// 0 = bo chep swapchain moi khung (ban sao chi de chup man hinh)
static int      s_nJxKhungBay = 2;		// so khung bay dang dat (SDL mac dinh 2)
static unsigned s_aJxTraHist[400];		// cach giua hai lan tra swapchain, o 0,25 ms (0..100 ms, o cuoi = tran)
static double   s_aJxSo[8];				// xem Rep3_DoNhipLay
static Uint64   s_uJxTraTruoc = 0;
static void JxNhipGhiCho(Uint64 uT0, bool bCoSwap, Uint32 swW, Uint32 swH)
{
	const Uint64 uT1 = SDL_GetPerformanceCounter();
	const double dF = (double)SDL_GetPerformanceFrequency();
	const double dCho = (double)(uT1 - uT0) * 1000.0 / dF;
	s_aJxSo[0] += 1.0;
	if (!bCoSwap) s_aJxSo[1] += 1.0;
	s_aJxSo[2] += dCho;
	if (dCho > s_aJxSo[3]) s_aJxSo[3] = dCho;
	if (dCho > 4.0) s_aJxSo[4] += 1.0;
	if (swW && swH) { s_aJxSo[5] = (double)swW; s_aJxSo[6] = (double)swH; }
	if (!bCoSwap)
		return;
	if (s_uJxTraTruoc)
	{
		const int b = (int)((double)(uT1 - s_uJxTraTruoc) * 4000.0 / dF);
		s_aJxTraHist[b < 0 ? 0 : (b > 399 ? 399 : b)]++;
	}
	s_uJxTraTruoc = uT1;
}
'''

REP3_CUOI = r'''

// [DONHIP 12/09] ban do nhip: bat/tat chep swapchain moi khung + so khung bay (SDL_SetGPUAllowedFramesInFlight 1..3). -1 = giu nguyen.
// Goi tu luong chinh giua hai khung. Doi so khung bay lam SDL cho het hang lenh va dung lai swapchain -> chi goi khi so thay doi.
extern "C" void Rep3_DoNhipDat(int nChepKhung, int nKhungBay)
{
	if (nChepKhung >= 0)
		s_nJxChepKhung = nChepKhung ? 1 : 0;
	CDevGpu* d = g_pRep3DevGpu;
	if (nKhungBay >= 1 && nKhungBay <= 3 && nKhungBay != s_nJxKhungBay && d && d->m_pGpu)
	{
		d->Lock();
		if (d->m_bFrameOpen || !d->m_cmds.empty() || !d->m_texUploads.empty()) d->SubmitFrame(false);
		if (SDL_SetGPUAllowedFramesInFlight(d->m_pGpu, (Uint32)nKhungBay)) s_nJxKhungBay = nKhungBay;
		else RgLog("[DONHIP] SetGPUAllowedFramesInFlight(%d) that bai: %s", nKhungBay, SDL_GetError());
		s_uJxTraTruoc = 0;
		d->Unlock();
	}
}
// [DONHIP 12/09] so lieu tu lan lay truoc roi dat lai. pHist[0..nBins) = cach giua hai lan tra swapchain (o 0,25 ms).
// pSo: 0 so lan lay swapchain, 1 lan khong co swapchain, 2 tong ms cho, 3 cho lau nhat (ms), 4 so lan cho > 4 ms,
//      5 rong swapchain, 6 cao swapchain, 7 so khung bay dang dat. Tra so o bieu do da chep.
extern "C" int Rep3_DoNhipLay(unsigned* pHist, int nBins, double* pSo, int nSo)
{
	const int n = (nBins < 400) ? nBins : 400;
	if (pHist) for (int i = 0; i < n; i++) pHist[i] = s_aJxTraHist[i];
	s_aJxSo[7] = (double)s_nJxKhungBay;
	if (pSo) for (int i = 0; i < nSo && i < 8; i++) pSo[i] = s_aJxSo[i];
	memset(s_aJxTraHist, 0, sizeof(s_aJxTraHist));
	const double w = s_aJxSo[5], h = s_aJxSo[6];
	memset(s_aJxSo, 0, sizeof(s_aJxSo));
	s_aJxSo[5] = w; s_aJxSo[6] = h;
	s_uJxTraTruoc = 0;
	return n;
}
'''


def va_rep3(L):
    i = mot(L, "s_uRep3GpuLenhVe = 0, s_uRep3GpuQuad = 0;", "bien HUD dau tep")
    L = L[:i + 1] + khoi(REP3_TREN) + L[i + 1:]
    i = mot(L, "SDL_WaitAndAcquireGPUSwapchainTexture(cb, m_pWin, &pSwap, &swW, &swH)", "lay swapchain trong SubmitFrame")
    t = le(L[i])
    L = (L[:i]
         + ["#ifdef JX_ANDROID",
            t + "const Uint64 uJxT0 = SDL_GetPerformanceCounter();\t// [DONHIP 12/09] do thoi gian cho swapchain (vblank / khung bay / dung lai swapchain)",
            "#endif", L[i],
            "#ifdef JX_ANDROID",
            t + "JxNhipGhiCho(uJxT0, pSwap != NULL, swW, swH);\t// [DONHIP 12/09]",
            "#endif"]
         + L[i + 1:])
    i = mot(L, "if (pSwap && swW && swH)", "dieu kien chep khung", lambda k: L[k].strip() == "if (pSwap && swW && swH)")
    if "ban sao khung de chup man hinh" not in L[i - 1]:
        raise SystemExit("dong truoc dieu kien chep khung la: " + L[i - 1])
    t = le(L[i])
    L = (L[:i]
         + ["#ifdef JX_ANDROID",
            t + "if (pSwap && swW && swH && s_nJxChepKhung)\t// [DONHIP 12/09] Rep3_DoNhipDat(0, ..) tat de do (swapchain SDL tren Android khong co TRANSFER_SRC)",
            "#else", L[i], "#endif"]
         + L[i + 1:])
    i = mot(L, 'extern "C" int Rep3_ThongKeGpu(char* sz, int n)', "ham Rep3_ThongKeGpu")
    j = i + 1
    while j < len(L) and L[j] != "}":
        j += 1
    if j + 1 >= len(L) or L[j + 1].strip() != "#endif":
        raise SystemExit("khong thay cuoi ham Rep3_ThongKeGpu")
    L = L[:j + 1] + khoi(REP3_CUOI) + L[j + 1:]
    return L


# ================================================================ JxPerfHudAndroid.cpp
HUD_INC = r'''
#include <stdarg.h>		// [DONHIP 12/09] ban do nhip
#include <time.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdint.h>
#include "../Ui/UiCase/UiToolsControlBar.h"	// [DONHIP 12/09] thanh cong cu = dau hieu da vao the gioi (nhu JxCanDieuKhien.cpp)
'''

HUD_KHOI = r'''
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

struct DnPha { const char* szTen; int nSuaSdl, nFps, nVsync, nSmooth, nChep, nBay, nXinHz; };	// -2 = giu cau hinh luc mo app
static const DnPha s_aDnPha[] = {
	{ "hien tai",              0,  -2, -2, -2, 1, 2,   0 },
	{ "+sua SDL",              1,  -2, -2, -2, 1, 2,   0 },
	{ "+nhip PC",              1,  -2,  1,  2, 1, 2,   0 },
	{ "+bo chep, 1 khung bay", 1,  -2,  1,  2, 0, 1,   0 },
	{ "+xin 120 Hz",           1, 120,  1,  2, 0, 1, 120 },
	{ "+xin 60 Hz",            1,  60,  1,  2, 0, 1,  60 },
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
	SDL_SetHint("JX_BO_QUA_SUBOPTIMAL", p.nSuaSdl ? "1" : "0");
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
	GetPrivateProfileString("DoNhip", "Pha", "0,1,2,3,4,5", sz, sizeof(sz), ".\\config.ini");
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
	SDL_SetHint("JX_BO_QUA_SUBOPTIMAL", "0");
	JxDoNhip_DatNhip(s_nDnFps0, s_nDnVsync0, s_nDnSmooth0);
	if (s_pfnDnDat) s_pfnDnDat(1, 2);
	if (s_nDnXin) DnXinHz(0);
	s_nDnXong = 1;
	DnGhi("[NHIP-XONG]", "het %d buoc - tra ve cau hinh luc mo app (PaintFps %d vsync %d smooth %d, chep khung, 2 khung bay, khong xin tan so)",
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
'''


def va_hud(L):
    i = mot(L, "#include <unistd.h>", "include unistd")
    L = L[:i + 1] + khoi(HUD_INC) + L[i + 1:]
    i = mot(L, "void PerfHud_SetEnable(int nOn)", "PerfHud_SetEnable")
    L = L[:i] + khoi(HUD_KHOI) + [""] + L[i:]
    i = mot(L, "if (!s_nEnable || !g_pRepresentShell)", "dau PerfHud_Draw")
    L = L[:i] + [le(L[i]) + "DoNhip_VeNhan();\t// [DONHIP 12/09] ten pha dang do (ca khi tat bang do)"] + L[i:]
    return L


va(os.path.join("Sources", "S3Client", "S3Client.cpp"), va_s3)
va(os.path.join("Sources", "Represent", "Represent3", "D3D9onGPUDev.cpp"), va_rep3)
va(os.path.join("Sources", "S3Client", "Platform", "JxPerfHudAndroid.cpp"), va_hud)
