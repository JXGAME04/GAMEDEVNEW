/*****************************************************************************************
//  ±íÏÖÄ£¿éµÄ¶ÔÍâ½Ó¿ÚµÄÈýÎ¬°æ±¾ÊµÏÖ¡£
//	Copyright : Kingsoft 2002
//	Author	:   cp(Chenpeng)
//	CreateTime:	2003-3-4
*****************************************************************************************/
#include "precompile.h"
#include "KRepresentShell3.h"
#include "..\iRepresent\Font\KFont3.h"
#include "d3d_shell.h"
#include "d3d_device.h"
#include "d3d_utils.h"
//#include "dxerr.h"

#include "TextureRes.h"
#include <psapi.h>	// [REP3 03/09 RAM] GetProcessMemoryInfo
#pragma comment(lib, "psapi.lib")
HWND	g_hWnd = NULL;
int		g_ntest = 0;

D3DFORMAT g_16BitFormat = D3DFMT_R5G6B5;

bool	g_bRefRast = false;
bool	g_bRunWindowed = true;

int32	g_nScreenWidth = DEFAULT_SCREENWIDTH;
int32	g_nScreenHeight = DEFAULT_SCREENHEIGHT;

uint32	g_nAntiAliasFSOverSample = 0;
uint32	g_nBackBufferCount = 1;
bool	g_bForceSwapEffectBlt = false;
bool	g_bForce32Bit = true;

int		g_nFogEnable = 0;

int		g_nWireframe = 0;

RenderModel	g_renderModel = RenderModel3DPerspective;

bool	g_bUse4444Texture = true;

bool	g_bNonPow2Conditional = false;

// [REP3 03/09] cong tac doc tu [Client] config.ini - xem BaseInclude.h
int  g_nRep3Flat      = 1;
int  g_nRep3Composite = 0;
int  g_nRep3Tex32     = 1;
int  g_nRep3Npot      = 1;
int  g_nRep3Vsync     = 0;
int  g_nRep3CacheMB   = 0;
int  g_nRep3Api       = 11;	// [D3D11 08/09] [NAP 08/09 #0] mac dinh 11: CD3D11Shim::Init do IDXGIFactory2 + feature level, khong du -> tu lui D3D9
int  g_nRep3ApiOn     = 9;	// [D3D11 08/09]
int  g_nRep3Atlas     = 1;	// [D3D11 08/09 d] gom texture nho vao trang atlas (chi khi Rep3Api=11)
int  g_nRep3Flip      = 1;	// [D3D11 08/09 f] 1 = flip model (DWM ghep khung tron ven, khong xe hinh; mac dinh), 0 = bitblt cu
int  g_nRep3LocKieu   = 1;	// [LOCTG b] 1 = chon loc, 0 = doi xung
float g_fRep3LocK     = 2.0f;	// [LOCTG b] 255 / Rep3LocToi
int  g_nRep3LocMs     = 0;
int  g_nRep3ChuGiuMs  = 12;	// [CHUGIU 09/09] giu vi tri man hinh cua chu (ms); 0 = tat
unsigned g_uRep3ChuGiu = 0, g_uRep3ChuVe = 0;	// [LOCTG 09/09] hang so thoi gian bo loc trinh khung (ms); 0 = tat
static KRepresentShell3* g_pRep3ShellDuyNhat = NULL;	// [NAPCHIEU 09/09] doi tuong shell (CreateRepresentShell tao dung 1)
static unsigned g_uRep3NapTruoc[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };	// [NAPCHIEU 09/09] [NAPNPC 09/09] [nguon 1 chieu / 2 NPC][ket qua: 0 khong, 1 da co, 2 giao nen]
int g_nRep3NapChieu = 1;	// [NAPCHIEU 09/09 b] cong tac [Client] Rep3NapChieu: 1 = nap truoc anh chieu khi nhan goi 95 (mac dinh), 0 = tat (A/B)
int g_nRep3NapNpc = 1;	// [NAPNPC 09/09] cong tac [Client] Rep3NapNpc: 1 = nap truoc anh than NPC khi gan ten (mac dinh), 0 = tat (A/B)
unsigned g_uRep3LocKhung = 0;	// [LOCTG 09/09] so khung da tron. [ANDROID 10/09 LOCTG] dinh nghia o DAY (nhu g_nRep3LocMs ben tren) chu khong o D3D9on11Dev.cpp - tep do chi co tren Windows, Android link thieu ky hieu.
#ifdef JX_PLATFORM_SDL
#ifdef JX_MOBILE
int g_nRep3AtlasGpu = 1;	// [GPU 11/09 ATLAS] Android: mac dinh BAT
int g_nRep3GpuBoBanCpu = 1;	// [GPU 11/09 BOCPU] Android: mac dinh BAT
int g_nRep3GpuMailbox = 0;	// [ANDROID 11/09 MAILBOX] [ANDROID 11/09 b] mac dinh TAT: nhip trinh chieu khong khop 60 Hz -> rung khi cuon; bat de A/B
#else
int g_nRep3AtlasGpu = 0;	// [GPU 11/09 ATLAS] GameSDL.exe tren Windows: mac dinh TAT (khong doi hanh vi PC); bat bang [Client] Rep3AtlasGpu=1
int g_nRep3GpuBoBanCpu = 0;	// [GPU 11/09 BOCPU] nhu tren
int g_nRep3GpuMailbox = 0;	// [ANDROID 11/09 MAILBOX] Windows GameSDL.exe: tat
#endif
#endif
int  g_nRep3Pal       = 1;	// [D3D11 08/09 r] texture sprite bang mau 2 B/px (chi D3D11)
int  g_nRep3Waitable  = 0;	// [D3D11 08/09 o] 0 = khong dung doi tuong cho (ban n giat)
int  g_nRep3Buffers   = 3;	// [D3D11 08/09 o] 2 nhu ban f
int  g_nRep3NoWait    = 0;	// [D3D11 08/09 l] 0 = Present cho nhu D3D9 (khong bo khung)
int  g_nRep3Latency   = 3;	// [D3D11 08/09 o] 0 = khong dong SetMaximumFrameLatency (nhu ban f: Present tu chan khi hang day)
int  g_nRep3Batch     = 1;	// [D3D11 08/09 j] gop quad cung trang thai thanh mot Draw
int  g_nRep3AtlasMang = 1;	// [MANG 09/09] atlas = Texture2DArray (doi trang khong vo lo quad); 0 = trang rieng + PS cu
int  g_nRep3Tearing   = 0;	// [D3D11 08/09 f] 1 = ALLOW_TEARING khi flip + vsync 0 (xe hinh, do tre thap nhat)
int  g_nRep3Ex        = 0;	// [RAM 08/09] 1 = tao D3D9Ex (ky vong driver khong giu ban sao texture trong RAM)
int  g_nRep3Log       = 1;
int  g_nRep3Pool      = 1;	// [REP3 03/09 RAM]
int  g_nRep3NapNen    = 1;	// [NAP 08/09 b] nap sprite/jpeg o luong nen
int  g_nRep3StatSec   = 30;
bool g_bNpotOK        = false;
int  g_nMaxTexW = 1024, g_nMaxTexH = 1024;

static char s_szRep3Ini[MAX_PATH] = "";
static int Rep3Ini(const char* szKey, int nDef)
{
	if (!s_szRep3Ini[0])
	{
		GetCurrentDirectoryA(MAX_PATH - 16, s_szRep3Ini);
		strcat(s_szRep3Ini, "\\config.ini");
	}
	return (int)GetPrivateProfileIntA("Client", szKey, nDef, s_szRep3Ini);
}

// [FX 07/09] bo dem lop ve (in them vao dong thong ke 30 s, roi dat lai 0). Chi luong ve cham.
unsigned g_uRep3FxTexNull = 0;		// DrawSprite*: texture NULL -> bo qua quad
#ifdef JX_MOBILE
int g_nJxONenLog = 0;	// [ONEN 15/09] [Client] Rep3ONenLog = so lan ghep nen con phai ghi chi tiet (tu giam ve 0)
#endif
unsigned g_uRep3FxAnhNull = 0;		// DrawImage2D*: GetImage NULL / khung ngoai tam -> break
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09] tach hai ly do de biet cay mat hinh la do dau
unsigned g_uJxRongAnh = 0, g_uJxRongKhung = 0, g_uJxRongKhungMax = 0;
#endif
unsigned g_uRep3FxTaoHong = 0;		// CreateTexture16Bit vao nhanh error
unsigned g_uRep3FxKhungKhongTex = 0;	// PrepareFrameData xong ma khung khong co texture
unsigned g_uRep3FxGiaiMa = 0;		// so khung giai ma dong bo tren luong ve
double   g_dRep3FxGiaiMaMs = 0.0;	// tong ms giai ma + tao texture
// [NAP 08/09 a] do NAP tren luong ve, in [REP3-NAP] moi Rep3StatSec giay
Rep3NapDo g_napSpr = {0, 0, 0}, g_napJpeg = {0, 0, 0}, g_napKhung = {0, 0, 0}, g_napGiaiMa = {0, 0, 0}, g_napGpu = {0, 0, 0};
double g_dRep3NapKhung = 0.0, g_dRep3NapKhungMax = 0.0; unsigned g_uRep3NapKhung5 = 0, g_uRep3NapKhung16 = 0;
#ifndef JX_MOBILE	// [VE 11/09] Android: ban co thong ke theo khung ben duoi
void Rep3NapCong(Rep3NapDo& d, double ms) { d.n++; d.ms += ms; if (ms > d.max) d.max = ms; g_dRep3NapKhung += ms; }
#endif
// [NAP 08/09 d] trong pham vi ham GHEP/GHI anh mot lan: bat buoc nap dong bo (ket qua chi dung mot lan, nap nen tra NULL = mat vinh vien)
struct Rep3NapDongBo { TextureResMgr& m; bool b; Rep3NapDongBo(TextureResMgr& mm) : m(mm), b(mm.m_bVeDangDien) { m.m_bVeDangDien = false; } ~Rep3NapDongBo() { m.m_bVeDangDien = b; } };
double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);
unsigned g_uRep3VeLoai[6] = {0, 0, 0, 0, 0, 0}; unsigned g_uRep3VeKhung = 0;	// [VE 08/09 a]
// [VE 08/09 b] thoi gian CPU luong ve: DrawPrimitives (tong trong khung) va ca khung Begin->End
double g_dRep3VeDpKhung = 0.0, g_dRep3VeDpTong = 0.0, g_dRep3VeDpMax = 0.0, g_dRep3VeKhungTong = 0.0, g_dRep3VeKhungMax = 0.0;
extern unsigned g_uRep3RingVong; extern double g_dRep3RingMapMax; extern unsigned g_uRep3TexRiengTao;	// [MANG 09/09 c]
LARGE_INTEGER g_liRep3VeBegin = {0};
int g_nRep3VeMau = 0;	// [VE 09/09 d] 1 = khung nay la khung MAU (1/8): moi do [VE] theo don vi/lenh chi chay tren khung mau (QPC 2 lan/lenh x 5 000 lenh/khung = 4 % thoi gian ve)
struct Rep3VeDpTimer { LARGE_INTEGER a; Rep3VeDpTimer() { if (g_nRep3VeMau) QueryPerformanceCounter(&a); } ~Rep3VeDpTimer() { if (g_nRep3VeMau) { LARGE_INTEGER b; QueryPerformanceCounter(&b); g_dRep3VeDpKhung += Rep3NapMs(a, b); } } };
#ifdef JX_MOBILE
// [VE 11/09] Android: cong tac + thong ke nap KHUNG o luong nen (TextureRes.cpp / TextureResMgr.cpp) va do trinh chieu (D3D9onGPUDev.cpp)
int g_nJxNapKhungNen = 1, g_nJxNapKhungMs = 3, g_nJxNapKhungTruoc = 2, g_nJxNapKhungApMs = 3, g_nJxVeGiatMs = 20;
int g_nJxNapKhungKB = 128;	// [TAI 14/09] [Client] NapKhungKB: ngan sach byte/khung tai dan texture khung NAP TRUOC len GPU (driver Fold 7 ton CPU ~20 ms/MB trong lenh tai)
int g_nJxNapKhungToKB = 128; unsigned g_uJxNapKhungTo = 0;	// [NAPTO 14/09] [Client] NapKhungToKB: khung nen >= nguong giao luong nen ngay (Fold 7: rut dong bo 1 khung to = 24-72 ms)
volatile int g_nJxNenDocPak = 0; unsigned g_uJxNapKhungPakBan = 0;	// [PAKBAN 14/09]
int g_nJxKhoiTruocPal = 4, g_nJxKhoiTruoc32 = 1;	// [KHOITRUOC 14/09] [Client] Rep3KhoiTruocPal / Rep3KhoiTruoc32: so khoi atlas cap san o khung dau (R8G8 64 MB / BGRA8 64 MB moi khoi)
int g_nJxAnhBoVeNen = 0;
int g_nJxHoiKhongDe = 1; unsigned g_uJxNapKhungRong = 0, g_uJxHoiTre = 0;	// [VE 11/09 d]
int g_nJxAtlasKe = 1, g_nJxAtlasTrang = 2048;	// [VE 11/09 e]
int g_nJxPalBuffer = 1;	// [PALBUF 11/09]
int g_nJxBoKhungGiong = 1, g_nJxBoKhungGiongMs = 250;	// [BKG 11/09]
int g_nJxSwapchainLogic = 100;	// [D1 11/09]
int g_nJxPsBuffer = 1, g_nJxBindRing = 1;	// [GOP 11/09]
unsigned g_uJxPsBangMax = 0, g_uJxPsTran = 0;	// [GOP 11/09]
int g_nJxAtlasMang = 0, g_nJxAtlasLop = 8, g_nJxAtlasCumMB = 64;	// [MANG 11/09]
unsigned g_uJxAtlasCum = 0;	// [MANG 11/09]
int g_nJxAtlasManaged = 1;	// [CHUATLAS 11/09]
unsigned g_uJxAtlasODat[2] = { 0, 0 }, g_uJxAtlasOMoi = 0;	// [CHUATLAS 11/09]
int g_nJxCullCpu = 1;	// [CULLCPU 11/09]
int g_nJxAtlasKhoi = 0, g_nJxAtlasKhoiLop = 8;	// [KHOI 11/09]
int g_nJxDoVeChiTiet = 0;	// [VECHITIET 11/09]
double g_dJxTrongVeKhung = 0.0, g_dJxTrongVeCuoi = 0.0;	unsigned g_uJxTrongVeLan = 0;	// [VECHITIET 11/09]
static double s_dJxTrongVeTong = 0.0, s_dJxTrongVeMax = 0.0;	// [VECHITIET 11/09] cong ca ky de in o [VE]
unsigned g_uJxKhoiSo = 0, g_uJxKhoiMB = 0, g_uJxKhoiHet = 0;	// [KHOI 11/09]
#ifdef JX_MOBILE
// [NENDO 13/09] do chi tiet DrawPrimitivesOnImage (ghep nen vung): GetImage (tim/nap) vs RIO (ghi lenh ve) vs doi dich ve; nap = so anh nap dong bo. Chi trong .so nay.
static double s_dJxNenGetMs = 0.0, s_dJxNenRioMs = 0.0, s_dJxNenRtMs = 0.0; static unsigned s_uJxNenGetLan = 0, s_uJxNenNapLan = 0, s_uJxNenBoLan = 0;
#endif
unsigned g_uJxCullGiu = 0, g_uJxCullBo = 0, g_uJxPipeVo[8] = { 0 };	// [CULLCPU 11/09]
unsigned g_uJxKhungGiongBo = 0, g_uJxKhungGiongCoTai = 0, g_uJxKhungGiongEp = 0, g_uJxKhungGiongDem = 0, g_uJxKhungGiongChuoiMax = 0, g_uJxKhungTrinhChieu = 0;	// [BKG 11/09]
unsigned g_uJxNapKhungBoVe = 0, g_uJxNapKhungBoVeKhung = 0, g_uJxNapKhungDongBo = 0, g_uJxNapKhungGiao = 0, g_uJxNapKhungTruocSo = 0, g_uJxNapKhungXong = 0, g_uJxNapKhungHong = 0, g_uJxNapKhungBo = 0, g_uJxNapKhungChoMax = 0;
double g_dJxNapKhungTre = 0.0, g_dJxNapKhungTreMax = 0.0, g_dJxNapNenBan = 0.0, g_dJxNapKhungAp = 0.0, g_dJxNapKhungApMax = 0.0; unsigned g_uJxNapKhungApKhung = 0;
Rep3NapDo g_jxNapNgoaiVe = {0, 0, 0};
static Rep3NapDo s_jxNapKhung[5];		// nap trong KHUNG hien tai theo loai: 0 tep spr, 1 jpeg, 2 rut khung, 3 giai ma, 4 tao GPU -> [VE-GIAT]
static Rep3NapDo s_jxNapKhungCuoi[5]; static double s_dJxNapNgoaiKhung = 0.0, s_dJxNapNgoaiKhungCuoi = 0.0, s_dJxVeCpuCuoi = 0.0;
static double s_dJxVeCpuTong = 0.0, s_dJxVeCpuMax = 0.0; static unsigned s_uJxVeCpuKhung = 0, s_uJxBoVeKhungCuoi = 0;
void Rep3NapCong(Rep3NapDo& d, double ms)
{
	d.n++; d.ms += ms; if (ms > d.max) d.max = ms; g_dRep3NapKhung += ms;
	const int k = (&d == &g_napSpr) ? 0 : (&d == &g_napJpeg) ? 1 : (&d == &g_napKhung) ? 2 : (&d == &g_napGiaiMa) ? 3 : 4;
	s_jxNapKhung[k].n++; s_jxNapKhung[k].ms += ms; if (ms > s_jxNapKhung[k].max) s_jxNapKhung[k].max = ms;
	if (g_pJxTexMgr && !g_pJxTexMgr->m_bVeDangDien) { g_jxNapNgoaiVe.n++; g_jxNapNgoaiVe.ms += ms; if (ms > g_jxNapNgoaiVe.max) g_jxNapNgoaiVe.max = ms; s_dJxNapNgoaiKhung += ms; }
}
// RepresentEnd (truoc khi dat lai g_dRep3NapKhung): chot so nap + thoi gian ve CPU cua khung nay
static void JxVeKhungChot()
{
	LARGE_INTEGER li; QueryPerformanceCounter(&li);
	s_dJxVeCpuCuoi = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, li) : 0.0;
	g_dJxTrongVeCuoi = g_dJxTrongVeKhung; g_dJxTrongVeKhung = 0.0;	// [VECHITIET 11/09] chot phan nam trong lop ve cua khung vua xong
	s_dJxTrongVeTong += g_dJxTrongVeCuoi; if (g_dJxTrongVeCuoi > s_dJxTrongVeMax) s_dJxTrongVeMax = g_dJxTrongVeCuoi;
	memcpy(s_jxNapKhungCuoi, s_jxNapKhung, sizeof(s_jxNapKhung)); memset(s_jxNapKhung, 0, sizeof(s_jxNapKhung));
	s_dJxNapNgoaiKhungCuoi = s_dJxNapNgoaiKhung; s_dJxNapNgoaiKhung = 0.0;
	s_uJxBoVeKhungCuoi = g_uJxNapKhungBoVeKhung; g_uJxNapKhungBoVeKhung = 0;
}
// Ngay sau Present: ve CPU + trinh chieu (hoac nap ngoai luc ve) vuot VeGiatMs -> ghi [VE-GIAT] (toi da 12 dong / 10 s)
static void JxVeGiatGhi(double dTrinhChieu)
{
	s_dJxVeCpuTong += s_dJxVeCpuCuoi; if (s_dJxVeCpuCuoi > s_dJxVeCpuMax) s_dJxVeCpuMax = s_dJxVeCpuCuoi; s_uJxVeCpuKhung++;
	if (g_nJxVeGiatMs <= 0) return;
	const double dViec = s_dJxVeCpuCuoi + dTrinhChieu - g_jxVeKhung.dCho;	// [VE 11/09 c] khong ke cho swapchain (vblank): may ao 60 Hz cho 20-30 ms moi khung
	if (dViec < (double)g_nJxVeGiatMs && s_dJxNapNgoaiKhungCuoi < (double)g_nJxVeGiatMs) return;
	static DWORD s_dwMoc = 0; static int s_nDem = 0;
	const DWORD dwNow = timeGetTime();
	if (s_dwMoc == 0 || dwNow - s_dwMoc >= 10000) { s_dwMoc = dwNow; s_nDem = 0; }
	if (++s_nDem > 12) return;
	const JxVeDo& k = g_jxVeKhung;
	double dNap = 0.0; for (int i = 0; i < 5; i++) dNap += s_jxNapKhungCuoi[i].ms;
	Rep3Log("[VE-GIAT] khung %u: %.1f ms (viec %.1f, khong ke cho) = ve CPU %.1f [trong lop ve %.1f / %u lenh, ngoai %.1f] (nap %.1f ms: tep spr %u/%.1f, rut khung %u/%.1f, giai ma %u/%.1f, tao GPU %u/%.1f; ngoai luc ve %.1f) + trinh chieu %.1f (cho %.1f, chep %.1f [tai %u tex %u KB, ring %u KB], ghi %.1f [%u lenh, %u quad, %u pass, doi tex %u], nop %.1f) | nen: bo ve %u, cho ap %u, ap %u khung %.1f ms | chep: bang mau %u/%.1f, tex map+chep %.1f, lenh tai %.1f, zero %u/%.1f, ring %.1f, xfer %u KB (phinh %u), pal lenh %.1f",
		g_uJxVeKhungSo, s_dJxVeCpuCuoi + dTrinhChieu, dViec, s_dJxVeCpuCuoi, g_dJxTrongVeCuoi, g_uJxTrongVeLan, s_dJxVeCpuCuoi - g_dJxTrongVeCuoi, dNap, s_jxNapKhungCuoi[0].n, s_jxNapKhungCuoi[0].ms, s_jxNapKhungCuoi[2].n, s_jxNapKhungCuoi[2].ms, s_jxNapKhungCuoi[3].n, s_jxNapKhungCuoi[3].ms, s_jxNapKhungCuoi[4].n, s_jxNapKhungCuoi[4].ms, s_dJxNapNgoaiKhungCuoi,
		dTrinhChieu, k.dCho, k.dChep, k.uTai, k.uTaiKB, k.uRingKB, k.dGhi, k.uLenh, k.uQuad, k.uPass, k.uDoiTex, k.dNop,
		s_uJxBoVeKhungCuoi, g_pJxTexMgr ? g_pJxTexMgr->JxNapKhungDangCho() : 0u, g_pJxTexMgr ? g_pJxTexMgr->m_uJxApKhungCuoi : 0u, g_pJxTexMgr ? g_pJxTexMgr->m_dJxApCuoi : 0.0,
		k.uPal, k.dChepPal, k.dChepTexMap, k.dChepTexLenh, k.uZero, k.dChepZero, k.dChepRing, k.uXferKB, k.uXferTang, k.dChepPalLenh);	// [VE 11/09 d] [PALBUF 11/09]
}
// Moi ky Rep3StatSec (khoi thong ke cua RepresentEnd): [VE] trinh chieu, [VE-GOP] doi trang thai / ly do khong gop, [VE-NAP] nap khung nen
static void JxVeKyIn()
{
	const unsigned n = g_uJxVeKhungSo ? g_uJxVeKhungSo : 1;
	const JxVeDo& t = g_jxVeTong; const JxVeDo& m = g_jxVeMax;
	Rep3Log("[VE] %ds trinh chieu %u khung: cho lenh+swapchain TB %.2f ms (max %.1f) | chep len GPU TB %.2f (max %.1f): tai %u texture %u KB (max %u KB/khung), ring TB %u KB (max %u) | ghi lenh TB %.2f (max %.1f): TB %u lenh, %u quad, %u dinh, %u pass/khung | nop TB %.2f (max %.1f) | tong TB %.2f (max %.1f), khung viec (khong ke cho) >8 ms %u, >16 ms %u | ve CPU (Begin->End) TB %.2f (max %.1f) [do chi tiet=%d: trong lop ve TB %.2f (max %.1f)] | chep: bang mau %u hang %.1f ms (max %.2f), tex map+chep %.1f (max %.2f), lenh tai %.1f (max %.2f), zero %u vung %.1f (max %.2f), ring %.1f (max %.2f), xfer phinh %u lan, xfer max %u KB",
		g_nRep3StatSec, g_uJxVeKhungSo, t.dCho / n, m.dCho, t.dChep / n, m.dChep, t.uTai, t.uTaiKB, m.uTaiKB, t.uRingKB / n, m.uRingKB, t.dGhi / n, m.dGhi, t.uLenh / n, t.uQuad / n, t.uDinh / n, t.uPass / n,
		t.dNop / n, m.dNop, t.dTong / n, m.dTong, g_uJxVe8, g_uJxVe16, s_uJxVeCpuKhung ? s_dJxVeCpuTong / s_uJxVeCpuKhung : 0.0, s_dJxVeCpuMax,
		g_nJxDoVeChiTiet, s_uJxVeCpuKhung ? s_dJxTrongVeTong / s_uJxVeCpuKhung : 0.0, s_dJxTrongVeMax,	// [VECHITIET 11/09]

		t.uPal, t.dChepPal, m.dChepPal, t.dChepTexMap, m.dChepTexMap, t.dChepTexLenh, m.dChepTexLenh, t.uZero, t.dChepZero, m.dChepZero, t.dChepRing, m.dChepRing, t.uXferTang, m.uXferKB);	// [VE 11/09 d]
	Rep3Log("[VE-BKG] trinh chieu %u khung, bo vi giong khung truoc %u, giong nhung co tai %u, giong nhung ep %u (cua so doi / qua %d ms), chi dem %u, chuoi bo dai nhat %u | bat=%d | bang mau kieu %s: lenh tai TB %.3f ms/khung (max %.2f)",
		g_uJxKhungTrinhChieu, g_uJxKhungGiongBo, g_uJxKhungGiongCoTai, g_uJxKhungGiongEp, g_nJxBoKhungGiongMs, g_uJxKhungGiongDem, g_uJxKhungGiongChuoiMax, g_nJxBoKhungGiong, g_nJxPalBuffer ? "buffer" : "texture", t.dChepPalLenh / n, m.dChepPalLenh);	// [BKG 11/09] [PALBUF 11/09]
	g_uJxKhungTrinhChieu = g_uJxKhungGiongBo = g_uJxKhungGiongCoTai = g_uJxKhungGiongEp = g_uJxKhungGiongDem = g_uJxKhungGiongChuoiMax = 0;
	Rep3Log("[VE-GOP] doi trang thai/khung TB: pipeline %u, texture/sampler %u (max %u), uniform vs %u, ps %u, cat/viewport %u | quad khong gop (ca ky): stride %u, khong lien tiep %u, pipeline %u, texture0 %u, texture1/sampler %u, vs %u, ps %u, cat/vp %u | atlas ke=%d trang %d: %u trang | ps bang %u muc (tran %u) | atlas mang=%d: %u cum | o atlas: DEFAULT %u, MANAGED %u (managed=%d), xin o moi %u | cull cpu=%d: giu %u bo %u | pipeline vo: fvf %u, topo %u, blend %u, cull %u, fill %u, rt %u, stride %u, khac %u | atlas khoi=%d: %u khoi (%u lop/khoi, %u MB), het khoi %u",
		t.uDoiPipe / n, t.uDoiTex / n, m.uDoiTex, t.uDoiVs / n, t.uDoiPs / n, t.uDoiCat / n, g_uJxGopVo[0], g_uJxGopVo[1], g_uJxGopVo[2], g_uJxGopVo[3], g_uJxGopVo[4], g_uJxGopVo[5], g_uJxGopVo[6], g_uJxGopVo[7], g_nJxAtlasKe, g_nJxAtlasTrang, g_uRep3AtlasPages, g_uJxPsBangMax, g_uJxPsTran, g_nJxAtlasMang, g_uJxAtlasCum, g_uJxAtlasODat[0], g_uJxAtlasODat[1], g_nJxAtlasManaged, g_uJxAtlasOMoi,
		g_nJxCullCpu, g_uJxCullGiu, g_uJxCullBo, g_uJxPipeVo[0], g_uJxPipeVo[1], g_uJxPipeVo[2], g_uJxPipeVo[3], g_uJxPipeVo[4], g_uJxPipeVo[5], g_uJxPipeVo[6], g_uJxPipeVo[7],
		g_nJxAtlasKhoi, g_uJxKhoiSo, (unsigned)g_nJxAtlasKhoiLop, g_uJxKhoiMB, g_uJxKhoiHet);	// [VE 11/09 e] [GOP 11/09] [MANG 11/09]
	memset(&g_jxVeTong, 0, sizeof(g_jxVeTong)); memset(&g_jxVeMax, 0, sizeof(g_jxVeMax)); g_uJxVeKhungSo = 0; g_uJxVe8 = 0; g_uJxVe16 = 0; memset(g_uJxGopVo, 0, sizeof(g_uJxGopVo));
	s_dJxTrongVeTong = 0.0; s_dJxTrongVeMax = 0.0;	// [VECHITIET 11/09]
	s_dJxVeCpuTong = 0.0; s_dJxVeCpuMax = 0.0; s_uJxVeCpuKhung = 0; g_uJxPsBangMax = 0; g_uJxPsTran = 0; g_uJxAtlasODat[0] = g_uJxAtlasODat[1] = 0; g_uJxAtlasOMoi = 0;	// [CHUATLAS 11/09]
	g_uJxCullGiu = g_uJxCullBo = 0; memset(g_uJxPipeVo, 0, sizeof(g_uJxPipeVo));	// [CULLCPU 11/09]	// [GOP 11/09]
	Rep3Log("[VE-NAP] nap khung nen (bat=%d, ngan sach %d ms/khung, nap truoc %d, ap %d ms): giao %u (nap truoc %u) xong %u hong %u bo %u | bo ve %u luot, dong bo trong ngan sach %u | hang cho max %u | tre giao->ap TB %.1f ms (max %.1f) | luong nen ban %.0f ms | ap tren luong ve %u khung %.1f ms (max %.2f/khung) | nap dong bo NGOAI luc ve: %u lan %.1f ms (max %.2f) | khung rong/hong khong giao lai %u | hoi NPC dang nap -> chua co %u (bat=%d)",
		g_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_uJxNapKhungGiao, g_uJxNapKhungTruocSo, g_uJxNapKhungXong, g_uJxNapKhungHong, g_uJxNapKhungBo, g_uJxNapKhungBoVe, g_uJxNapKhungDongBo, g_uJxNapKhungChoMax,
		g_uJxNapKhungXong ? g_dJxNapKhungTre / g_uJxNapKhungXong : 0.0, g_dJxNapKhungTreMax, g_dJxNapNenBan, g_uJxNapKhungApKhung, g_dJxNapKhungAp, g_dJxNapKhungApMax, g_jxNapNgoaiVe.n, g_jxNapNgoaiVe.ms, g_jxNapNgoaiVe.max, g_uJxNapKhungRong, g_uJxHoiTre, g_nJxHoiKhongDe);	// [VE 11/09 d]
	g_uJxNapKhungGiao = g_uJxNapKhungTruocSo = g_uJxNapKhungXong = g_uJxNapKhungHong = g_uJxNapKhungBo = g_uJxNapKhungBoVe = g_uJxNapKhungDongBo = g_uJxNapKhungChoMax = 0;
	g_dJxNapKhungTre = g_dJxNapKhungTreMax = g_dJxNapNenBan = g_dJxNapKhungAp = g_dJxNapKhungApMax = 0.0; g_uJxNapKhungApKhung = 0; memset(&g_jxNapNgoaiVe, 0, sizeof(g_jxNapNgoaiVe)); g_uJxNapKhungRong = 0; g_uJxHoiTre = 0;
	{	// [TAI 14/09] tai dan khung nap truoc + to 0 trang atlas bang chep GPU
		extern unsigned g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_uJxTaiTruocMax, g_uJxZeroChep; extern double g_dJxTaiTruocMs, g_dJxZeroChepMs;
		extern unsigned g_uJxDemSo, g_uJxDemKB;	// [DEM 14/09]
		Rep3Log("[VE-TAI] tai dan khung nap truoc (NapKhungKB=%d): vao hang %u, xong %u, %u KB / %u luot, %.1f ms (hang cho max %u) | trang atlas / o rong to 0 bang chep GPU: %u lan %.1f ms | o BGRA8 tai qua anh dem: %u o %u KB | khung to (>= %d KB) giao nen thay vi rut dong bo: %u | pak ban giao nen: %u",
			g_nJxNapKhungKB, g_uJxTaiTruocSo, g_uJxTaiTruocXong, g_uJxTaiTruocKB, g_uJxTaiTruocLuot, g_dJxTaiTruocMs, g_uJxTaiTruocMax, g_uJxZeroChep, g_dJxZeroChepMs, g_uJxDemSo, g_uJxDemKB, g_nJxNapKhungToKB, g_uJxNapKhungTo, g_uJxNapKhungPakBan);
		g_uJxTaiTruocSo = g_uJxTaiTruocXong = g_uJxTaiTruocKB = g_uJxTaiTruocLuot = g_uJxTaiTruocMax = g_uJxZeroChep = 0; g_dJxTaiTruocMs = g_dJxZeroChepMs = 0.0; g_uJxDemSo = g_uJxDemKB = 0; g_uJxNapKhungTo = 0; g_uJxNapKhungPakBan = 0;
	}
}
#endif
#ifdef JX_MOBILE
// ============================ [TG 13/09] THE GIOI VE VAO RENDER TARGET KHI QUA TAI ============================
// Chu 13/09: "fix mot lan cho iOS va Android, khong anh huong trai nghiem". Chi lam viec khi (a) PaintFps muc tieu >= 2 x ToiThieuHz (117) VA man
// that (SDL_GetCurrentDisplayMode) >= 2 x ToiThieuHz: chu ky = max(1000/PaintFps, 1000/Hz man) - KHONG uoc tu khoang cach present (lan thu 13/09 16:00:
// may ao 60 Hz co hai khung cach nhau 5 ms -> tuong man 120 Hz -> K=2 = the gioi 30 Hz, chu thay nhay man hinh luc vao map / di chuyen);
// (b) viec/khung (ve CPU + chep + ghi + nop, khong ke cho) cua cac khung CO ve the gioi > 70 % chu ky trong cua so 2 s, VA (c) khoang cach
// present thuc > 1,12 chu ky (khung dang roi). Luc do: the gioi ve cach khung (K = 2) vao render target, khung xen giua chi ve lai anh RT
// + giao dien / can dieu khien -> the gioi 60 Hz DEU (bang nac 60 cua thanh FPS), giao dien theo man. Het qua tai (viec < 45 % chu ky
// trong 2 cua so lien) -> K = 1 = duong cu, khong RT, khong them mot lenh nao. The gioi khong bao gio duoi TheGioiToiThieuHz (60).
// Blit 1:1 bang quad point-sampling, cung phep chieu +0,5 cua DrawBitmap16 -> pixel y het; K = 2 chi lam NPC / dan cap nhat 60 Hz.
// Khung chi blit: goc toa do m_nLeft/m_nTop dat lai = goc luc ve RT de lop phu (vong chon, nut ky nang) trung voi anh RT.
// S3Client (Wnds.cpp) goi Rep3_JxTheGioi qua GetProcAddress (libmain.so khong link Represent3); iOS dang ky trong ios/JxIosMain.cpp.
// [Client] TheGioiRT=1 (0 = tat han), TheGioiRTEp=0 (1 = luon RT K=1 de thu pixel, 2 = luon K=2 de thu), TheGioiToiThieuHz=60. Log [TG] moi ky.
#include <math.h>	// [LAC 14/09] cosf / sinf / floorf (xoay anh dem the gioi)
int g_nJxTheGioiEp = 0;	// Rep3_JxEpTrinhChieu (D3D9onGPUDev.cpp): be mat / cua so doi -> khung toi phai ve the gioi that
extern "C" double Rep3_JxManHinhMs();	// D3D9onGPUDev.cpp: 1000 / tan so man SDL bao (0 = khong biet)
static int s_nTgBat = 1, s_nTgEp = 0, s_nTgToiThieuHz = 60;
static int s_nTgPaintFps = 0;					// PaintFps muc tieu S3Client dua sang o lenh 0 (JxDoNhip_LayNhip)
static int s_nTgK = 1;							// 1 = ve the gioi moi khung, 2 = cach khung
static unsigned s_uTgKhung = 0;					// dem RepresentBegin
static unsigned s_uTgVeThat = 0xFFFFFFFFu;		// khung gan nhat that su ve the gioi (duong cu hoac vao RT) - de tinh viec/khung
static unsigned s_uTgRTVe = 0xFFFFFFFFu;		// khung gan nhat ve the gioi VAO RT (chi blit duoc o khung ngay sau no)
static double s_dTgMinCach = 1e9, s_dTgCachTong = 0.0, s_dTgViecTong = 0.0; static unsigned s_uTgCachSo = 0, s_uTgViecSo = 0;
static LARGE_INTEGER s_liTgCuoi = { 0 }; static DWORD s_dwTgCuaSo = 0; static int s_nTgCuaSoNhe = 0;
static double s_dTgChuKy = 0.0, s_dTgViecTB = 0.0, s_dTgCachTB = 0.0;	// ket qua cua so gan nhat (in [TG])
static unsigned s_uTgDemVe = 0, s_uTgDemBlit = 0, s_uTgDemThuong = 0, s_uTgDemDoiK = 0;	// thong ke ky [TG]
static int s_nTgRtCap = 1500, s_nTgRt2Cap = 1150, s_nTgRtSan = 1;	// [TGCAP 14/09]
static int s_nTgNac = 0, s_nTgNacLoc = 1;	// [TGNAC 14/09 d] MAC DINH TAT (chu 16:0x "npc voi player bi mo": ve thang o co nho = moi sprite thu nho luc quet, mat chi tiet; duong cu ve 1:1 vao RT to roi thu ca anh = sieu lay mau, net hon). 1100 = bat tu 110 %; NacLoc 1 = loc bang mau tuyen tinh (muot), 0 = khong ep loc (net hon, co rang cua)
void Rep3Gpu_VpLogic(IDirect3DDevice9* pDev, int nW, int nH);	// D3D9onGPUDev.cpp
void Rep3Gpu_PalLin(IDirect3DDevice9* pDev, int bBat);
void Rep3Gpu_VpEp(IDirect3DDevice9* pDev, IDirect3DSurface9* pSurf, int nW, int nH);	// [TGNAC 14/09 b]
static int s_nTgDaChamRt = 0, s_nTgDaChamRt2 = 0;	// [TGCAP 14/09 b] da cham san RT / RT2 (pass rong CLEAR) sau khi cap; ve 0 khi huy
#include <vector>	// [CHUNET 14/09]
// [CHUNET 14/09] lop chu the gioi ve SAU blit khi nhin rong / phong to / lac (chu 14/09: "luc zoom rong chu bi mo di khong nhin ro"): OutputText /
// OutputVNText / OutputRichText va RU_T_SHADOW toa do the gioi (nen mo sau ten, thanh mau) goi TRONG luc ve RT duoc xep hang; lenh 2 (sau blit)
// goi lai y het -> CoordinateTransform di qua cua toa do (m_nTgTrangThai == 0) nen dung cho, ve o co that (khong thu nho / phong to cung anh RT).
// Khung chi blit (K = 2) ve lai hang cua khung truoc (anh RT cung cu). Zoom 100 % khong lac: khong xep (RT 1:1 da net). [Client] TheGioiRTChu=0 tat.
struct KTgChu { int nLoai, nFontId, nX, nY, nZ, nLineWidth, nCount, nVb; unsigned int Color, BorderColor; KOutputTextParam Param; KRUShadow Bong; KRUImage Anh; };	// [CHUNET 14/09 d] + icon canh ten
static std::vector<KTgChu> s_TgChu; static std::vector<char> s_TgChuVb; static int s_nTgChuNet = 1; static bool s_bTgChuTra = false;
static void TgChuThem(int nLoai, int nFontId, const char* psText, int nCount, int nX, int nY, int nZ, unsigned int Color, int nLineWidth, unsigned int BorderColor, const KOutputTextParam* pParam, const KRUShadow* pBong, const KRUImage* pAnh)
{
	KTgChu e; memset(&e, 0, sizeof(e));
	if (pAnh) e.Anh = *pAnh;	// [CHUNET 14/09 d]
	e.nLoai = nLoai; e.nFontId = nFontId; e.nX = nX; e.nY = nY; e.nZ = nZ; e.nLineWidth = nLineWidth; e.Color = Color; e.BorderColor = BorderColor; e.nCount = nCount; e.nVb = -1;
	if (pParam) e.Param = *pParam;
	if (pBong) e.Bong = *pBong;
	if (psText)
	{	// chep chuoi (toi 1024 byte), nCount >= 0 thi thanh so byte da chep; < 0 (KRF_ZERO_END) giu nguyen
		int nDai = 0; const int nToiDa = (nCount < 0) ? 1024 : ((nCount < 1024) ? nCount : 1024);
		while (nDai < nToiDa && psText[nDai]) nDai++;
		e.nVb = (int)s_TgChuVb.size(); if (nCount >= 0) e.nCount = nDai;
		s_TgChuVb.insert(s_TgChuVb.end(), psText, psText + nDai); s_TgChuVb.push_back(0);
	}
	if (s_TgChu.size() < 8192) s_TgChu.push_back(e);
}	// [TGCAP 14/09] cap RT = khung x RtCap, RT2 = 2 x khung x Rt2Cap (phan nghin); San = cap san RT o khung dau vao the gioi

void KRepresentShell3::JxTheGioiDocIni()
{
	s_nTgBat = Rep3Ini("TheGioiRT", 1) ? 1 : 0;
	s_nTgEp = Rep3Ini("TheGioiRTEp", 0); if (s_nTgEp < 0 || s_nTgEp > 2) s_nTgEp = 0;
	s_nTgToiThieuHz = Rep3Ini("TheGioiToiThieuHz", 60); if (s_nTgToiThieuHz < 30) s_nTgToiThieuHz = 30; if (s_nTgToiThieuHz > 120) s_nTgToiThieuHz = 120;
	Rep3Log("[TG] the gioi RT khi qua tai: bat=%d ep=%d, the gioi toi thieu %d Hz (TheGioiRT / TheGioiRTEp / TheGioiToiThieuHz)", s_nTgBat, s_nTgEp, s_nTgToiThieuHz);
	s_nTgRtCap = Rep3Ini("TheGioiRTCap", 1500); if (s_nTgRtCap < 1000) s_nTgRtCap = 1000; if (s_nTgRtCap > 3000) s_nTgRtCap = 3000;	// [TGCAP 14/09]
	s_nTgRt2Cap = Rep3Ini("TheGioiRT2Cap", 1150); if (s_nTgRt2Cap < 1000) s_nTgRt2Cap = 1000; if (s_nTgRt2Cap > 1500) s_nTgRt2Cap = 1500;
	s_nTgRtSan = Rep3Ini("TheGioiRTSan", 1) ? 1 : 0;
	Rep3Log("[TGCAP] RT cap %d, RT2 cap 2 x %d phan nghin khung, cap san %d (TheGioiRTCap / TheGioiRT2Cap / TheGioiRTSan)", s_nTgRtCap, s_nTgRt2Cap, s_nTgRtSan);
	s_nTgChuNet = Rep3Ini("TheGioiRTChu", 1) ? 1 : 0; Rep3Log("[CHUNET] lop chu the gioi ve sau blit khi zoom / lac: %d (TheGioiRTChu)", s_nTgChuNet);	// [CHUNET 14/09]
	s_nTgNac = Rep3Ini("TheGioiRTNac", 0); if (s_nTgNac < 0) s_nTgNac = 0; if (s_nTgNac > 0 && s_nTgNac < 1001) s_nTgNac = 1001;	// [TGNAC 14/09 d] mac dinh 0 = TAT (net hon)
	s_nTgNacLoc = Rep3Ini("TheGioiRTNacLoc", 1) ? 1 : 0;
	Rep3Log("[TGNAC] nhin rong tu %d phan nghin: ve the gioi thu nho vao vung khung x le, loc bang mau %d (TheGioiRTNac 0 = tat / TheGioiRTNacLoc)", s_nTgNac, s_nTgNacLoc);
}

void KRepresentShell3::JxTheGioiHuy()
{
	if (PD3DDEVICE) { Rep3Gpu_VpLogic(PD3DDEVICE, 0, 0); Rep3Gpu_VpEp(PD3DDEVICE, NULL, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }	// [TGNAC 14/09 c] go con tro RT khoi lop GPU TRUOC khi release (Reset / doi cap giua pha: khong de m_pJxVpEpTex treo)
	if (m_nTgTrangThai == 1 && PD3DDEVICE && m_pTgSurfCu) PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu);
	m_nTgTrangThai = 0;
	SAFE_RELEASE(m_pTgSurfCu); SAFE_RELEASE(m_pTgSB); SAFE_RELEASE(m_pTgSurf); SAFE_RELEASE(m_pTgTex);
	SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = m_nTg2CapW = m_nTg2CapH = 0;	// [ZOOM3D 14/09] [TGCAP 14/09] ca cap
	m_nTgW = m_nTgH = m_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; s_uTgRTVe = 0xFFFFFFFFu; s_nTgDaChamRt = s_nTgDaChamRt2 = 0; s_TgChu.clear(); s_TgChuVb.clear(); m_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;	// [CHUNET 14/09] huy hang
}

// [ZOOM3D 14/09] RT2 = 2 x RT (BGRA8, render target) cho PHONG TO co loc net; [Client] Rep3ZoomNet=0 tat (blit thang LINEAR). Tao lan dau /
// khi RT doi co; huy cung JxTheGioiHuy. Khong tao duoc thi tat luon (khong thu lai moi khung).
bool KRepresentShell3::JxTheGioiNet2()
{
	static int s_nNet = -1;
	if (s_nNet < 0) s_nNet = Rep3Ini("Rep3ZoomNet", 1) ? 1 : 0;
	if (!s_nNet || !m_pTgTex || m_nTgW <= 0 || m_nTgH <= 0) return false;
	const int nW = m_nTgW * 2, nH = m_nTgH * 2;	// phan DUNG cua RT2
	if (m_pTgTex2 && (nW > m_nTg2CapW || nH > m_nTg2CapH)) { SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2CapW = m_nTg2CapH = 0; }	// [TGCAP 14/09] chi cap lai khi KHONG DU cho, khong cap lai moi buoc zoom
	m_nTg2W = nW; m_nTg2H = nH;
	if (!m_pTgTex2)
	{
		int nCapW = (m_nTgCapKhungW * 2 * s_nTgRt2Cap / 1000 + 1) & ~1, nCapH = (m_nTgCapKhungH * 2 * s_nTgRt2Cap / 1000 + 1) & ~1;	// [TGCAP 14/09] cap = 2 x khung x le toi da (TheGioiRT2Cap)
		if (nCapW < nW) nCapW = nW; if (nCapH < nH) nCapH = nH;
		m_nTg2CapW = nCapW; m_nTg2CapH = nCapH;
		if (FAILED(PD3DDEVICE->CreateTexture(nCapW, nCapH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex2, NULL)) || !m_pTgTex2
			|| FAILED(m_pTgTex2->GetSurfaceLevel(0, &m_pTgSurf2)) || !m_pTgSurf2)
		{
			Rep3Log("[ZOOM3D] khong tao duoc RT2 %dx%d -> phong to blit thang LINEAR", nCapW, nCapH);
			SAFE_RELEASE(m_pTgSurf2); SAFE_RELEASE(m_pTgTex2); m_nTg2W = m_nTg2H = m_nTg2CapW = m_nTg2CapH = 0; s_nNet = 0;	// [TGCAP 14/09]
			return false;
		}
		Rep3Log("[ZOOM3D] RT2 cap %dx%d (dung %dx%d) cho phong to co loc net (zoom %d)", nCapW, nCapH, nW, nH, m_nTgZoom);	// [TGCAP 14/09]
	}
	return true;
}

static bool Rep3ChuGiu(const char* psText, int nCount, int nFont, int& nX, int& nY);	// [CHUNET 14/09 b] dinh nghia phia duoi (CHUGIU 09/09)
// [CHUNET 14/09] dang xep hang chu the gioi? (chi trong luc ve RT va co zoom / lac; khong xep khi dang tra hang)
bool KRepresentShell3::TgChuXep()
{
	return s_nTgChuNet && m_nTgTrangThai == 1 && !s_bTgChuTra && (m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000);
}

// [CHUNET 14/09 b] tra hang chu len khung (lenh 2 sau blit, m_nTgTrangThai == 0 -> cua toa do dang mo). Chu giu CO THAT nen KHONG bien doi thang
// toa do goc: Core da can giua moi dong quanh nMpsX theo be rong chu va xep dong theo do cao o toa do 1:1 -> qua zoom thi do lech bi thu phong,
// moi dong lech mot kieu, cac dong chong len nhau (chu 14/09: "ten bang hoi voi danh hieu bi lech"). Cach: gom cac phan tu LIEN TIEP cung tam x
// (+-2) va gan nhau theo y (150 don vi the gioi) thanh mot NHOM = mot NPC; neo nhom = (tam x, y chan = y lon nhat cua dong chu, z 0) di qua
// CoordinateTransform (cua zoom / lac); moi phan tu ve o neo + do lech 1:1 khong thu phong ((x - tam), (y - chan) / 2, - z x 887 / 1024).
// Thanh mau hai doan (RU_T_SHADOW ke nhau cung y / z) gop tam chung. Chu dat cham (Rep3ChuGiu) van ap nhu chu neo the gioi.
void KRepresentShell3::TgChuVe()
{
	if (s_TgChu.empty()) return;
	const int n = (int)s_TgChu.size();
	static std::vector<int> vCx, vY, vZ, vNhom, vCxN, vChan, vZMin; static std::vector<char> vCoChu;	// [CHUNET 14/09 c] + vZ / vZMin
	vCx.resize(n); vY.resize(n); vZ.resize(n); vNhom.resize(n); vCxN.clear(); vChan.clear(); vZMin.clear(); vCoChu.clear();
	for (int k = 0; k < n; k++)
	{	// tam x + y tham chieu (toa do the gioi) cua tung phan tu
		const KTgChu& e = s_TgChu[k];
		if (e.nLoai == 3)
		{
			vCx[k] = (e.Bong.oPosition.nX + e.Bong.oEndPos.nX) / 2; vY[k] = (e.Bong.oPosition.nY + e.Bong.oEndPos.nY) / 2;
			vZ[k] = (e.Bong.oPosition.nZ < e.Bong.oEndPos.nZ) ? e.Bong.oPosition.nZ : e.Bong.oEndPos.nZ;	// [CHUNET 14/09 c]
			if (k > 0 && s_TgChu[k - 1].nLoai == 3)
			{	// chuoi bong ke nhau (thanh mau: doan do + doan xam) cung y / z, dau doan nay = cuoi doan truoc -> tam chung ca chuoi
				const KRUShadow& t = s_TgChu[k - 1].Bong;
				if (t.oPosition.nY == e.Bong.oPosition.nY && t.oEndPos.nY == e.Bong.oEndPos.nY && t.oPosition.nZ == e.Bong.oPosition.nZ && t.oEndPos.nZ == e.Bong.oEndPos.nZ && t.oEndPos.nX == e.Bong.oPosition.nX)
				{
					int j = k - 1;
					while (j > 0 && s_TgChu[j - 1].nLoai == 3 && vCx[j - 1] == vCx[j] && s_TgChu[j - 1].Bong.oEndPos.nX == s_TgChu[j].Bong.oPosition.nX) j--;
					const int cx = (s_TgChu[j].Bong.oPosition.nX + e.Bong.oEndPos.nX) / 2;
					for (int m = j; m <= k; m++) vCx[m] = cx;
				}
			}
		}
		else if (e.nLoai == 4) { vCx[k] = e.Anh.oPosition.nX; vY[k] = e.Anh.oPosition.nY; vZ[k] = e.Anh.oPosition.nZ; }	// [CHUNET 14/09 d] icon: goc anh (lech nMpsX toi ~100)
		else if (e.nLoai == 2) { vCx[k] = e.Param.nX + e.nLineWidth / 2; vY[k] = e.Param.nY; vZ[k] = e.Param.nZ; }	// [CHUNET 14/09 c] + z
		else
		{	// Core can giua: nX = nMpsX - font x byte / 4 -> tam = nX + font x byte / 4
			int nDai = e.nCount; if (nDai < 0) nDai = (e.nVb >= 0) ? (int)strlen(&s_TgChuVb[e.nVb]) : 0;
			vCx[k] = e.nX + e.nFontId * nDai / 4; vY[k] = e.nY; vZ[k] = e.nZ;	// [CHUNET 14/09 c] + z
		}
	}
	int nNhom = -1, nCxNhom = 0, nYNhom = 0;
	for (int k = 0; k < n; k++)
	{	// gom nhom; y chan = y lon nhat cua dong CHU trong nhom (dong ten o nMpsY), chua co chu thi y dau tien
		const int nLoai = s_TgChu[k].nLoai;
		const int nRongX = (nLoai == 4 || (nNhom >= 0 && !vCoChu[nNhom])) ? 200 : 2;	// [CHUNET 14/09 d] icon (hay nhom moi chi co icon) rong hon: icon lech nMpsX toi ~100
		if (nNhom < 0 || vCx[k] < nCxNhom - nRongX || vCx[k] > nCxNhom + nRongX || vY[k] < nYNhom - 150 || vY[k] > nYNhom + 150)
		{
			nNhom = (int)vCxN.size(); nCxNhom = vCx[k]; nYNhom = vY[k];
			vCxN.push_back(vCx[k]); vChan.push_back(vY[k]); vCoChu.push_back((nLoai != 3 && nLoai != 4) ? 1 : 0); vZMin.push_back(vZ[k]);	// [CHUNET 14/09 c] zMin = chieu cao nhan vat
		}
		else
		{
			if (nLoai != 3 && nLoai != 4 && !vCoChu[nNhom]) { vCxN[nNhom] = vCx[k]; nCxNhom = vCx[k]; }	// [CHUNET 14/09 d] dong chu dau tien dinh tam nhom (nhom mo bang icon)
			if (nLoai != 3 && nLoai != 4 && (!vCoChu[nNhom] || vY[k] > vChan[nNhom])) { vChan[nNhom] = vY[k]; vCoChu[nNhom] = 1; }
			if (vZ[k] < vZMin[nNhom]) vZMin[nNhom] = vZ[k];
		}
		vNhom[k] = nNhom;
	}
	s_bTgChuTra = true;
	for (int k = 0; k < n; k++)
	{
		const KTgChu& e = s_TgChu[k];
		const int g = vNhom[k], cxN = vCxN[g], yChan = vChan[g], zMin = vZMin[g];
		int ax = cxN, ay = yChan;
		CoordinateTransform(ax, ay, 0);	// chan nhom -> khung (qua cua zoom / lac)
		ay -= (int)((float)((zMin * 887) >> 10) * 1000.0f / (float)m_nTgZoom + 0.5f);	// [CHUNET 14/09 c] chieu cao nhan vat (z nho nhat cua nhom) CO THEO ZOOM nhu than sprite -> chu / thanh mau bam sat dau
		char* vb = (e.nVb >= 0) ? &s_TgChuVb[e.nVb] : NULL;
		if (e.nLoai == 3)
		{
			KRUShadow b = e.Bong;
			b.oPosition.nX = ax + (e.Bong.oPosition.nX - cxN); b.oEndPos.nX = ax + (e.Bong.oEndPos.nX - cxN);
			b.oPosition.nY = ay + (e.Bong.oPosition.nY - yChan) / 2 - (((e.Bong.oPosition.nZ - zMin) * 887) >> 10);	// [CHUNET 14/09 c] z - zMin
			b.oEndPos.nY = ay + (e.Bong.oEndPos.nY - yChan) / 2 - (((e.Bong.oEndPos.nZ - zMin) * 887) >> 10);
			b.oPosition.nZ = b.oEndPos.nZ = 0;
			DrawPrimitives(1, (KRepresentUnit*)&b, RU_T_SHADOW, TRUE);
		}
		else if (e.nLoai == 4)
		{	// [CHUNET 14/09 d] icon canh ten: co that, lech 1:1 so voi tam nhom
			KRUImage a = e.Anh;
			a.oPosition.nX = ax + (e.Anh.oPosition.nX - cxN);
			a.oPosition.nY = ay + (e.Anh.oPosition.nY - yChan) / 2 - (((e.Anh.oPosition.nZ - zMin) * 887) >> 10);
			a.oPosition.nZ = 0;
			DrawPrimitives(1, (KRepresentUnit*)&a, RU_T_IMAGE, TRUE);
		}
		else if (e.nLoai == 2)
		{
			if (!vb) continue;
			KOutputTextParam p = e.Param;
			p.nX = ax + (e.Param.nX - cxN); p.nY = ay + (e.Param.nY - yChan) / 2 - (((e.Param.nZ - zMin) * 887) >> 10); p.nZ = TEXT_IN_SINGLE_PLANE_COORD;
			OutputRichText(e.nFontId, &p, vb, e.nCount, e.nLineWidth);
		}
		else
		{
			if (!vb) continue;
			int x = ax + (e.nX - cxN), y = ay + (e.nY - yChan) / 2 - (((e.nZ - zMin) * 887) >> 10);	// [CHUNET 14/09 c] z - zMin
			Rep3ChuGiu(vb, e.nCount, e.nFontId, x, y);
			if (e.nLoai == 0) OutputText(e.nFontId, vb, e.nCount, x, y, e.Color, e.nLineWidth, TEXT_IN_SINGLE_PLANE_COORD, e.BorderColor);
			else OutputVNText(e.nFontId, vb, e.nCount, x, y, e.Color, e.nLineWidth, TEXT_IN_SINGLE_PLANE_COORD, e.BorderColor);
		}
	}
	s_bTgChuTra = false;
}

int KRepresentShell3::JxTheGioi(int nLenh, int nThamSo)
{
	if (nLenh == 3) { g_nJxTheGioiEp = 1; return 0; }
	if (nLenh == 4)
	{	// [ZOOM 13/09] dat zoom phan nghin (1000 = tat, toi da 3000) - S3Client (JxLiaCanh) goi khi chum hai ngon; doi thi khung sau ve that
		int nZ = nThamSo; if (nZ < 500) nZ = 500; if (nZ > 3000) nZ = 3000;	// [ZOOM3D 14/09] < 1000 = phong to (toi da 2x)
		if (nZ != m_nTgZoom) { Rep3Log("[ZOOM] zoom %d -> %d (phan nghin)", m_nTgZoom, nZ); m_nTgZoom = nZ; if (nZ == 1000) { m_nZoomDx = m_nZoomDy = 0; } g_nJxTheGioiEp = 1; }
		return m_nTgZoom;
	}
	if (nLenh == 5) return m_nTgZoom;	// [ZOOM 13/09] hoi zoom hien tai
	if (nLenh == 6)
	{	// [LAC 14/09] dat goc xoay (0,01 do, kep +-15 do); 0 = khong xoay. Chi doi blit + cua toa do, khong doi RT
		int nG = nThamSo; if (nG > 1500) nG = 1500; if (nG < -1500) nG = -1500;
		if (nG != m_nTgXoay) { m_nTgXoay = nG; g_nJxTheGioiEp = 1; }
		return m_nTgXoay;
	}
	if (nLenh == 7) return m_nTgXoay;
	if (nLenh == 8)
	{	// [LAC 14/09] le RT khi dang lia (phan nghin, 1000 = khong; kep 1000..1500): RT = khung x zoom x le -> lenh 0 cap lai RT
		int nL = nThamSo; if (nL < 1000) nL = 1000; if (nL > 1500) nL = 1500;
		if (nL != m_nTgLe) { Rep3Log("[LAC] le RT %d -> %d (phan nghin), goc %d", m_nTgLe, nL, m_nTgXoay); m_nTgLe = nL; g_nJxTheGioiEp = 1; }
		return m_nTgLe;
	}
	if (nLenh == 9)
	{	// [LAC 14/09 b] co dan doc (phan nghin, kep 850..1150): keo doc -> camera "cui / ngang" nhe; 1000 = khong. Chi doi blit + cua toa do
		int nD = nThamSo; if (nD < 850) nD = 850; if (nD > 1150) nD = 1150;
		if (nD != m_nTgDoc) { m_nTgDoc = nD; g_nJxTheGioiEp = 1; }
		return m_nTgDoc;
	}
	if (nLenh == 11)
	{	// [TGCAP 14/09 b] S3Client (JxLiaCanh Camera_DocMap) bao ZoomToiThieu phan nghin luc vao map / doi cong tac: < 1000 = co the phong to -> cap san RT2 (TheGioiRTSan + Rep3ZoomNet)
		int nT = nThamSo; if (nT < 500) nT = 500; if (nT > 1000) nT = 1000;
		if (nT != m_nTgZoomMin) Rep3Log("[TGCAP] zoom toi thieu %d -> %d phan nghin (lenh 11)", m_nTgZoomMin, nT);	// [TGCAP 14/09 c]
		m_nTgZoomMin = nT;
		return m_nTgZoomMin;
	}
	if (nLenh == 10) return m_nTgDoc;
	if (!PD3DDEVICE || m_bDeviceLost) return 0;
	if (nLenh == 0)
	{	// hoi: 0 = duong cu, 1 = ve the gioi vao RT roi blit, 2 = chi blit anh RT cua khung truoc
		if (nThamSo > 0 && nThamSo <= 240) s_nTgPaintFps = nThamSo;
		int nK = s_nTgK;
		if (s_nTgEp == 1) nK = 1; else if (s_nTgEp == 2) nK = 2;
		m_nTgZoomRt = (m_nTgLe > 1000) ? m_nTgZoom * m_nTgLe / 1000 : m_nTgZoom;	// [LAC 14/09] RT them le khi dang lia (goc khung khong ho luc xoay)
		const bool bZoom = (m_nTgZoomRt > 1000);	// [ZOOM 13/09] dang nhin rong (hoac RT co le): luon ve the gioi vao RT to hon khung roi thu nho (K = 1)
		const bool bPhongTo = (m_nTgZoomRt < 1000);	// [ZOOM3D 14/09] phong to: RT = co khung (K = 1), lenh 2 blit phong to (co loc net neu Rep3ZoomNet)
		if (bZoom || bPhongTo) nK = 1;
		const bool bDuongCu = (!bZoom && !bPhongTo && (!s_nTgBat || (nK != 2 && s_nTgEp != 1)));	// [TGCAP 14/09] khung nay ve thang (khong qua RT)
		const bool bCapSan = (s_nTgRtSan && s_nTgBat && (!m_pTgTex || (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)));	// [TGCAP 14/09 d] con viec cap san: RT chua co, hoac RT2 chua cap ma S3Client vua bao co phong to (lenh 11 den sau khung dau ~1 s)
		if (bDuongCu && !bCapSan) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }	// [TGCAP 14/09] chua viec cap san: ve thang ngay
		const int nRtW = bZoom ? ((g_nScreenWidth * m_nTgZoomRt / 1000 + 1) & ~1) : g_nScreenWidth;	// [LAC 14/09] theo ti le RT that
		const int nRtH = bZoom ? ((g_nScreenHeight * m_nTgZoomRt / 1000 + 1) & ~1) : g_nScreenHeight;
		const bool bNac = (s_nTgNac > 0 && bZoom && m_nTgZoom >= s_nTgNac && g_nJxTheGioiEp >= 0);	// [TGNAC 14/09] nhin rong: ve thu nho (he so 1000/zoom) vao vung khung x le
		const int nPxW = bNac ? ((nRtW * 1000 / m_nTgZoom + 1) & ~1) : nRtW, nPxH = bNac ? ((nRtH * 1000 / m_nTgZoom + 1) & ~1) : nRtH;
		if ((int)bNac != m_bTgNac) Rep3Log("[TGNAC] zoom %d: %s (vung %dx%d, lo-gic %dx%d)", m_nTgZoom, bNac ? "ve thu nho vao vung khung x le" : "ve 1:1 vao RT to", nPxW, nPxH, nRtW, nRtH);
		m_bTgNac = bNac ? 1 : 0; m_nTgPxW = nPxW; m_nTgPxH = nPxH;
		if (!m_pTgTex || nPxW > m_nTgCapW || nPxH > m_nTgCapH || g_nScreenWidth != m_nTgCapKhungW || g_nScreenHeight != m_nTgCapKhungH)	// [TGNAC 14/09] cap theo vung diem anh that
		{	// [TGCAP 14/09] chi cap lai khi khung doi (gap / mo, xoay) hoac phan dung vuot cap; cap = khung x TheGioiRTCap (nac 250 phan nghin khi vuot)
			// de moi buoc zoom / le chi doi phan dung + uv blit, khong huy / cap lai texture (Fold 7 12:41: ~10 lan cap/giay khi chum ngon = khung 97 ms)
			JxTheGioiHuy();
			int nMuc = s_nTgRtCap;
			if (g_nScreenWidth > 0 && g_nScreenHeight > 0)
			{
				const int nCanW = (nPxW * 1000 + g_nScreenWidth - 1) / g_nScreenWidth, nCanH = (nPxH * 1000 + g_nScreenHeight - 1) / g_nScreenHeight;	// [TGNAC 14/09] theo vung diem anh
				const int nCan = (nCanW > nCanH) ? nCanW : nCanH;
				while (nMuc < nCan && nMuc < 3500) nMuc += 250;	// [TGCAP 14/09 b] kep 3500 (phien do nhip: 12x diem khung = 30 MB store/khung)
			}
			m_nTgCapKhungW = g_nScreenWidth; m_nTgCapKhungH = g_nScreenHeight;
			m_nTgCapW = (g_nScreenWidth * nMuc / 1000 + 1) & ~1; m_nTgCapH = (g_nScreenHeight * nMuc / 1000 + 1) & ~1;
			if (m_nTgCapW < nPxW) m_nTgCapW = nPxW; if (m_nTgCapH < nPxH) m_nTgCapH = nPxH;	// [TGNAC 14/09]
			m_nTgW = nRtW; m_nTgH = nRtH;
			if (m_nTgCapW <= 0 || m_nTgCapH <= 0 || FAILED(PD3DDEVICE->CreateTexture(m_nTgCapW, m_nTgCapH, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_pTgTex, NULL))
				|| !m_pTgTex || FAILED(m_pTgTex->GetSurfaceLevel(0, &m_pTgSurf)) || !m_pTgSurf)
			{
				Rep3Log("[TG] khong tao duoc render target %dx%d -> tat the gioi RT", m_nTgCapW, m_nTgCapH);	// [TGCAP 14/09]
				JxTheGioiHuy(); s_nTgBat = 0; s_uTgDemThuong++; return 0;
			}
			if (FAILED(PD3DDEVICE->CreateStateBlock(D3DSBT_ALL, &m_pTgSB))) m_pTgSB = NULL;
			Rep3Log("[TG] render target the gioi cap %dx%d (dung %dx%d, khung %dx%d, muc %d)", m_nTgCapW, m_nTgCapH, m_nTgW, m_nTgH, g_nScreenWidth, g_nScreenHeight, nMuc);	// [TGCAP 14/09]
		}
		else { m_nTgW = nRtW; m_nTgH = nRtH; }	// [TGCAP 14/09] chi doi phan dung, giu texture
		if (s_nTgRtSan && m_pTgSurf && (!s_nTgDaChamRt || (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)))	// [TGCAP 14/09 c] ca khi khung nay di duong RT (RT cham thua mot pass rong, RT2 van can cap san)
		{	// [TGCAP 14/09 b] cham san RT (+ RT2 khi co phong to) MOT lan = doi dich + Clear + tra dich (lop SDL_GPU mo pass rong load-op CLEAR, XOANEN 13/09)
			// de driver cam ket bo nho ngay khung dau vao the gioi (log Fold 7 12:41: texture lon moi ton 2-3 lan nop 21-43 ms o lan dung dau)
			LPDIRECT3DSURFACE9 pCu = NULL;
			if (SUCCEEDED(PD3DDEVICE->GetRenderTarget(0, &pCu)) && pCu)
			{
				if (!s_nTgDaChamRt && SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf))) PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);
				s_nTgDaChamRt = 1;
				if (!s_nTgDaChamRt2 && m_nTgZoomMin < 1000)
				{
					s_nTgDaChamRt2 = 1;
					if (JxTheGioiNet2() && SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf2))) PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);
				}
				PD3DDEVICE->SetRenderTarget(0, pCu);
				Rep3Log("[TGCAP] cham san RT %dx%d%s (zoom toi thieu %d phan nghin)", m_nTgCapW, m_nTgCapH, m_pTgTex2 ? " + RT2" : "", m_nTgZoomMin);
			}
			SAFE_RELEASE(pCu);
		}
		if (bDuongCu) { s_uTgVeThat = s_uTgKhung; s_uTgDemThuong++; return 0; }	// [TGCAP 14/09] da cap san, khung nay van ve thang
		// chi blit khi khung NGAY TRUOC da ve the gioi vao RT (anh moi dung 1 khung), khong bi ep (be mat doi), K = 2 -> hai khung ve mot lan
		if (nK == 2 && !g_nJxTheGioiEp && m_nTgTrangThai == 0 && s_uTgRTVe == s_uTgKhung - 1)
			return 2;
		s_uTgVeThat = s_uTgKhung;
		return 1;
	}
	if (nLenh == 1)
	{	// bat dau ve the gioi vao RT
		if (!m_pTgSurf || m_nTgTrangThai == 1) return 0;
		if (FAILED(PD3DDEVICE->GetRenderTarget(0, &m_pTgSurfCu))) { m_pTgSurfCu = NULL; return 0; }
		if (FAILED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf))) { SAFE_RELEASE(m_pTgSurfCu); return 0; }
		PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);
		if (m_bTgNac)
		{	// [TGNAC 14/09] ve THU NHO: viewport that = vung m_nTgPxW x m_nTgPxH (khung x le), VS chia theo lo-gic m_nTgW x m_nTgH (Core van ve toa do 1:1), ps loc palette tuyen tinh
			D3DVIEWPORT9 vp; memset(&vp, 0, sizeof(vp)); vp.Width = (DWORD)m_nTgPxW; vp.Height = (DWORD)m_nTgPxH; vp.MaxZ = 1.0f;
			PD3DDEVICE->SetViewport(&vp);
			Rep3Gpu_VpLogic(PD3DDEVICE, m_nTgW, m_nTgH); Rep3Gpu_VpEp(PD3DDEVICE, m_pTgSurf, m_nTgPxW, m_nTgPxH); Rep3Gpu_PalLin(PD3DDEVICE, s_nTgNacLoc);	// [TGNAC 14/09 d] NacLoc = 0 thi khong ep loc (net hon)	// [TGNAC 14/09 b] vung ep gan voi RT (ghep nen dat doi target giua pha)
		}
		m_nTgLeftKhung = m_nLeft; m_nTgTopKhung = m_nTop; m_nTgKhungW = g_nScreenWidth; m_nTgKhungH = g_nScreenHeight;	// [ZOOM 13/09] goc + co khung that
		if (m_nTgZoomRt > 1000)
		{	// [ZOOM 13/09] RT to hon khung: goc RT lui de tieu diem van o giua; cull / cat trong shell theo co RT trong luc ve ([LAC 14/09] ke ca le)
			m_nZoomDx = (m_nTgW - g_nScreenWidth) / 2; m_nZoomDy = (m_nTgH - g_nScreenHeight) / 2;
			m_nLeft -= m_nZoomDx; m_nTop -= m_nZoomDy; g_nScreenWidth = m_nTgW; g_nScreenHeight = m_nTgH;
		}
		else { m_nZoomDx = m_nZoomDy = 0; }
		m_nTgTrangThai = 1; m_nTgLeft = m_nLeft; m_nTgTop = m_nTop;
		s_TgChu.clear(); s_TgChuVb.clear();	// [CHUNET 14/09] hang moi cho khung nay
		g_nJxTheGioiEp = 0; s_uTgRTVe = s_uTgKhung; s_uTgDemVe++;
		return 1;
	}
	if (nLenh == 2)
	{	// ket thuc (neu dang ve) + blit anh RT len backbuffer
		if (m_nTgTrangThai == 1)
		{
			PD3DDEVICE->SetRenderTarget(0, m_pTgSurfCu); SAFE_RELEASE(m_pTgSurfCu); m_nTgTrangThai = 0;
			if (m_bTgNac) { Rep3Gpu_VpLogic(PD3DDEVICE, 0, 0); Rep3Gpu_VpEp(PD3DDEVICE, NULL, 0, 0); Rep3Gpu_PalLin(PD3DDEVICE, 0); }	// [TGNAC 14/09] tra viewport lo-gic + loc palette (SetRenderTarget da tra viewport that)
			if (m_nTgZoomRt > 1000) { g_nScreenWidth = m_nTgKhungW; g_nScreenHeight = m_nTgKhungH; m_nLeft = m_nTgLeftKhung; m_nTop = m_nTgTopKhung; }	// [ZOOM 13/09] tra co khung + goc khung ([LAC 14/09] ke ca le)
			PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, m_dwTgMauXoa, 1.0f, 0L);	// lenh xoa dau khung cua RepresentBegin bi doi dich ve nuot -> xoa lai backbuffer
		}
		else
		{	// khung chi blit: lop phu ve sau (vong chon, nut ky nang, chu the gioi) dung goc toa do luc ve RT de trung anh
			m_nLeft = m_nTgLeft; m_nTop = m_nTgTop; s_uTgDemBlit++;
		}
		if (!m_pTgTex) return 0;
		if (m_pTgSB) m_pTgSB->Capture();
		PD3DDEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		PD3DDEVICE->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		PD3DDEVICE->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		PD3DDEVICE->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF);
		PD3DDEVICE->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		PD3DDEVICE->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1); PD3DDEVICE->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		PD3DDEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); PD3DDEVICE->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		PD3DDEVICE->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); PD3DDEVICE->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		{ const DWORD dwLoc = (m_nTgZoomRt != 1000 /*[ZOOM3D 14/09] ca phong to*/ || m_nTgXoay || m_nTgDoc != 1000) ? D3DTEXF_LINEAR : D3DTEXF_POINT; PD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, dwLoc); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, dwLoc); }	// [ZOOM 13/09] thu nho thi loc tuyen tinh
		PD3DDEVICE->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP); PD3DDEVICE->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		LPDIRECT3DTEXTURE9 pTexBlit = m_pTgTex;
		if (m_nTgZoom < 1000 && JxTheGioiNet2())
		{	// [ZOOM3D 14/09] phong to co loc net: buoc 1 RT -> RT2 (co 2x, POINT = nhan doi diem sac), buoc 2 RT2 -> khung (LINEAR) = "sharp bilinear"
			LPDIRECT3DSURFACE9 pDichCu = NULL;
			if (SUCCEEDED(PD3DDEVICE->GetRenderTarget(0, &pDichCu)) && pDichCu && SUCCEEDED(PD3DDEVICE->SetRenderTarget(0, m_pTgSurf2)))
			{
				VERTEX2D q[4];
				PD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
				PD3DDEVICE->SetTexture(0, m_pTgTex); PD3DDEVICE->SetTexture(1, NULL);
				PD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);
				const float fU1 = (float)m_nTgPxW / (float)m_nTgCapW, fV1 = (float)m_nTgPxH / (float)m_nTgCapH;	/* [TGNAC 14/09] vung diem anh */	// [TGCAP 14/09] uv = phan dung / cap cua RT
				for (int i = 0; i < 4; i++)
				{	// quad phu kin RT2 (toa do pixel theo viewport cua target = RT2)
					q[i].position = D3DXVECTOR4((i & 1) ? (float)m_nTg2W : 0.f, (i & 2) ? (float)m_nTg2H : 0.f, 100, 1);
					q[i].color = 0xffffffff; q[i].tu = (i & 1) ? fU1 : 0.0f; q[i].tv = (i & 2) ? fV1 : 0.0f;	// [TGCAP 14/09]
				}
				PD3DDEVICE->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, q, sizeof(VERTEX2D));
				PD3DDEVICE->SetRenderTarget(0, pDichCu);
				PD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				pTexBlit = m_pTgTex2;
			}
			SAFE_RELEASE(pDichCu);
		}
		PD3DDEVICE->SetTexture(0, pTexBlit); PD3DDEVICE->SetTexture(1, NULL);
		PD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);
		VERTEX2D v[4];
		{	// [ZOOM 13/09] dich blit = KHUNG (da tra lai o tren); [LAC 14/09] = RT thu theo zoom nguoi choi (khung x le khi dang lia) roi XOAY goc m_nTgXoay
			// quanh tam khung theo phep quay mat dat 2:1 (x' = c.x - 2s.y, y' = s.x/2 + c.y). Goc 0, le 1000: dung bang khung nhu cu.
			const float fKW = (float)g_nScreenWidth, fKH = (float)g_nScreenHeight;
			const float fTl = 1000.0f / (float)m_nTgZoom;
			const float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc), fK = (float)m_nTgDoc / 1000.0f;	// [LAC 14/09 b] fK = co dan doc
			const float fHx = (float)m_nTgW * 0.5f, fHy = (float)m_nTgH * 0.5f;
			const float fU = (pTexBlit == m_pTgTex2) ? (float)m_nTg2W / (float)m_nTg2CapW : (float)m_nTgPxW / (float)m_nTgCapW;	/* [TGNAC 14/09] vung diem anh */	// [TGCAP 14/09] uv = phan dung / cap (RT2 hay RT)
			const float fV = (pTexBlit == m_pTgTex2) ? (float)m_nTg2H / (float)m_nTg2CapH : (float)m_nTgPxH / (float)m_nTgCapH;
			for (int i = 0; i < 4; i++)
			{
				const float qx = ((i & 1) ? fHx : -fHx) * fTl, qy = ((i & 2) ? fHy : -fHy) * fTl;
				v[i].position = D3DXVECTOR4(fKW * 0.5f + fC * qx - 2.0f * fS * fK * qy, fKH * 0.5f + 0.5f * fS * qx + fC * fK * qy, 100, 1);
				v[i].color = 0xffffffff; v[i].tu = (i & 1) ? fU : 0.0f; v[i].tv = (i & 2) ? fV : 0.0f;	// [TGCAP 14/09]
			}
		}
		PD3DDEVICE->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX2D));
		PD3DDEVICE->SetTexture(0, NULL);
		if (m_pTgSB) m_pTgSB->Apply();
		TgChuVe();	// [CHUNET 14/09] lop chu the gioi len khung (sau blit, truoc giao dien; khung chi blit ve lai hang khung truoc)
		return 1;
	}
	return 0;
}

// sau Present moi khung: chu ky = max(1000/PaintFps muc tieu, 1000/Hz man that); viec/khung cua cac khung co ve the gioi; quyet K moi 2 s
void KRepresentShell3::JxTheGioiCapNhat(double dTrinhChieuMs)
{
	(void)dTrinhChieuMs;
	LARGE_INTEGER li; QueryPerformanceCounter(&li);
	if (s_liTgCuoi.QuadPart)
	{
		const double dCach = Rep3NapMs(s_liTgCuoi, li);
		if (dCach > 0.5 && dCach < 200.0) { if (dCach < s_dTgMinCach) s_dTgMinCach = dCach; s_dTgCachTong += dCach; s_uTgCachSo++; }
	}
	s_liTgCuoi = li;
	if (s_uTgVeThat == s_uTgKhung)
	{	// khung nay CO ve the gioi (duong cu hoac RT): viec = ve CPU + (chep + ghi + nop), khong ke cho swapchain
		const double dViec = s_dJxVeCpuCuoi + (g_jxVeKhung.dTong - g_jxVeKhung.dCho);
		if (dViec > 0.0 && dViec < 500.0) { s_dTgViecTong += dViec; s_uTgViecSo++; }
	}
	const DWORD dwNow = timeGetTime();
	if (s_dwTgCuaSo == 0) { s_dwTgCuaSo = dwNow; return; }
	if (dwNow - s_dwTgCuaSo < 2000) return;
	s_dwTgCuaSo = dwNow;
	// chu ky = MAX(1000 / PaintFps muc tieu, 1000 / Hz man that): nac 60 tren man 120 -> 16,7 (khong K=2); PaintFps 120 nhung man da tut 60 Hz -> 16,7 (khong K=2)
	const double dMucTieu = (s_nTgPaintFps > 0) ? 1000.0 / (double)s_nTgPaintFps : 1000.0 / 60.0;
	const double dManHinh = Rep3_JxManHinhMs();
	double dChuKy = (dManHinh > dMucTieu) ? dManHinh : dMucTieu; if (dChuKy < 4.0) dChuKy = 4.0; if (dChuKy > 40.0) dChuKy = 40.0;
	const double dCachTB = s_uTgCachSo ? s_dTgCachTong / s_uTgCachSo : 0.0;
	const double dViecTB = s_uTgViecSo ? s_dTgViecTong / s_uTgViecSo : 0.0;
	s_dTgChuKy = dChuKy; s_dTgViecTB = dViecTB; s_dTgCachTB = dCachTB;
	// K = 2 chi khi the gioi van >= TheGioiToiThieuHz: 2 chu ky <= 1000 / ToiThieuHz (dung sai 3 %) - tuc PaintFps va man deu >= ~117 Hz
	const bool bManNhanh = s_nTgPaintFps > 0 && dManHinh > 0.0 && (2.0 * dChuKy) <= (1000.0 / (double)s_nTgToiThieuHz) * 1.03;
	if (s_nTgK == 1)
	{
		if (s_nTgBat && bManNhanh && s_uTgViecSo >= 30 && dViecTB > 0.70 * dChuKy && dCachTB > 1.12 * dChuKy)
		{
			s_nTgK = 2; s_nTgCuaSoNhe = 0; s_uTgDemDoiK++;
			Rep3Log("[TG] qua tai: viec/khung %.2f ms > 70%% chu ky %.2f (PaintFps %d, man %.2f ms), cach present TB %.2f -> the gioi cach khung (K=2, %d Hz)", dViecTB, dChuKy, s_nTgPaintFps, dManHinh, dCachTB, (int)(500.0 / dChuKy + 0.5));
		}
	}
	else
	{
		if (!s_nTgBat || !bManNhanh) { s_nTgK = 1; s_nTgCuaSoNhe = 0; s_uTgDemDoiK++; Rep3Log("[TG] ve K=1: chu ky %.2f ms (PaintFps %d, man %.2f) khong du nhanh (hoac tat)", dChuKy, s_nTgPaintFps, dManHinh); }
		else if (s_uTgViecSo >= 30 && dViecTB < 0.45 * dChuKy)
		{
			if (++s_nTgCuaSoNhe >= 2) { s_nTgK = 1; s_nTgCuaSoNhe = 0; s_uTgDemDoiK++; Rep3Log("[TG] het qua tai: viec/khung %.2f ms < 45%% chu ky %.2f trong 4 s -> K=1", dViecTB, dChuKy); }
		}
		else s_nTgCuaSoNhe = 0;
	}
	s_dTgMinCach = 1e9; s_dTgCachTong = s_dTgViecTong = 0.0; s_uTgCachSo = s_uTgViecSo = 0;
}

void KRepresentShell3::JxTheGioiKyIn()
{
	Rep3Log("[TG] the gioi RT: bat=%d ep=%d K=%d | khung: duong cu %u, ve vao RT %u, chi blit %u | doi K %u lan | cua so gan nhat: chu ky %.2f ms (PaintFps %d, man %.2f ms), viec/khung the gioi TB %.2f, cach present TB %.2f, min %.2f",
		s_nTgBat, s_nTgEp, s_nTgK, s_uTgDemThuong, s_uTgDemVe, s_uTgDemBlit, s_uTgDemDoiK, s_dTgChuKy, s_nTgPaintFps, Rep3_JxManHinhMs(), s_dTgViecTB, s_dTgCachTB, (s_dTgMinCach < 1e8) ? s_dTgMinCach : 0.0);
	s_uTgDemThuong = s_uTgDemVe = s_uTgDemBlit = s_uTgDemDoiK = 0;
}
#endif
void Rep3VeDem(const char* p)
{
	if (!p) { g_uRep3VeLoai[5]++; return; }
	if (*p == '\\' || *p == '/') p++;
	if (*p == '*' || *p == '_') { g_uRep3VeLoai[4]++; return; }
	if (_strnicmp(p, "spr\\npcres", 10) == 0 || _strnicmp(p, "spr/npcres", 10) == 0) { g_uRep3VeLoai[0]++; return; }
	if (_strnicmp(p, "spr\\skill", 9) == 0 || _strnicmp(p, "spr/skill", 9) == 0) { g_uRep3VeLoai[1]++; return; }
	if (_strnicmp(p, "spr\\ui", 6) == 0 || _strnicmp(p, "spr/ui", 6) == 0) { g_uRep3VeLoai[2]++; return; }
	if (_strnicmp(p, "maps", 4) == 0) { g_uRep3VeLoai[3]++; return; }
	g_uRep3VeLoai[5]++;
}
double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)
{
	static LARGE_INTEGER s_liTanSo = {0};
	if (!s_liTanSo.QuadPart) QueryPerformanceFrequency(&s_liTanSo);
	return s_liTanSo.QuadPart ? (1000.0 * (double)(b.QuadPart - a.QuadPart) / (double)s_liTanSo.QuadPart) : 0.0;
}

// [REP3 08/09 h] top ten anh bi bo qua (GetImage NULL hoac khung ngoai tam) trong ky thong ke
struct Rep3AnhNullMuc { char szTen[64]; int nKhung; unsigned uDem; };
static Rep3AnhNullMuc s_Rep3AnhNull[8];
static int s_nRep3AnhNull = 0;
static void Rep3AnhNullGhi(const char* szTen, int nKhung)
{
#ifdef JX_MOBILE
	if (g_nJxAnhBoVeNen) { g_nJxAnhBoVeNen = 0; return; }	// [VE 11/09] khung dang nap o luong nen: khong phai anh thieu
#endif
	if (!szTen) szTen = "?";
	int i;
	for (i = 0; i < s_nRep3AnhNull; i++)
		if (strncmp(s_Rep3AnhNull[i].szTen, szTen, 63) == 0 && s_Rep3AnhNull[i].nKhung == nKhung) { s_Rep3AnhNull[i].uDem++; return; }
	if (s_nRep3AnhNull < 8)
	{
		strncpy(s_Rep3AnhNull[s_nRep3AnhNull].szTen, szTen, 63); s_Rep3AnhNull[s_nRep3AnhNull].szTen[63] = 0;
		s_Rep3AnhNull[s_nRep3AnhNull].nKhung = nKhung; s_Rep3AnhNull[s_nRep3AnhNull].uDem = 1; s_nRep3AnhNull++;
	}
}
static void Rep3AnhNullIn()
{
	if (s_nRep3AnhNull <= 0) return;
	char sz[640]; int n = 0;
	for (int i = 0; i < s_nRep3AnhNull && n < 560; i++)
		n += sprintf(sz + n, "%s%s(k%d) x%u", i ? " ; " : "", s_Rep3AnhNull[i].szTen, s_Rep3AnhNull[i].nKhung, s_Rep3AnhNull[i].uDem);
	Rep3Log("[REP3] anh_null top: %s", sz);
	s_nRep3AnhNull = 0;
}
void Rep3Log(const char* fmt, ...)
{
	if (!g_nRep3Log)
		return;
	FILE* pLog = fopen("jx_rep3.log", "a");
	if (!pLog)
		return;
	char szBuf[1024];
	va_list va;
	va_start(va, fmt);
	_vsnprintf(szBuf, sizeof(szBuf) - 1, fmt, va);
	va_end(va);
	szBuf[sizeof(szBuf) - 1] = 0;
	fprintf(pLog, "[%u] %s\n", (unsigned int)GetTickCount(), szBuf);
	fclose(pLog);
}

bool Test3D()
{
#ifdef JX_NO_DIRECTX
	return true;	// [ANDROID 08/09] khong co DirectDraw7 de hoi VRAM
#else
	IDirectDraw7 *pDDraw7;
	DirectDrawCreateEx(NULL, (void**)&pDDraw7, IID_IDirectDraw7, NULL);
	DDCAPS hel_ddcaps, hal_ddcpas ;
	hel_ddcaps.dwSize = sizeof(DDCAPS);
	hal_ddcpas.dwSize = sizeof(DDCAPS);

	if(!pDDraw7)
		return false;

	pDDraw7->GetCaps(&hel_ddcaps, &hal_ddcpas);
	pDDraw7->Release();

	// Èç¹ûÏÔ´æÐ¡ÓÚ32Õ×Ôò·µ»Ø¼Ù
	if(hel_ddcaps.dwVidMemTotal < 33554432)
		return false;

	MEMORYSTATUS stat;
	GlobalMemoryStatus (&stat);
	// Èç¹ûÎïÀíÄÚ´æÐ¡ÓÚ128Õ×Ôò·µ»Ø¼Ù
	if(stat.dwTotalPhys < 134217728)
		return false;

	return true;
#endif
}

static inline void DOWRDToRGB(DWORD color, DWORD &a, DWORD &r, DWORD &g, DWORD &b)
{
	a = color >> 24;
	r = (color & 0x00ff0000) >> 16;
	g = (color & 0x0000ff00) >> 8;
	b = (color & 0x000000ff);
}

static inline DWORD ARGBToDWORD(DWORD a, DWORD r, DWORD g, DWORD b)
{
	return (a<<24) | (r<<16) | (g<<8) | b;
}

// ½«color1ÓÃcolor2×öÆ«É«
static inline DWORD ScaleColor(DWORD color1, DWORD color2)
{
	DWORD a1, r1, g1, b1, a2, r2, g2, b2;
	DOWRDToRGB(color1, a1, r1, g1, b1);
	DOWRDToRGB(color2, a2, r2, g2, b2);

	r1 = r1 * r2 / 256;     // 255
	g1 = g1 * g2 / 256;     // 255
	b1 = b1 * b2 / 256;     // 255

	return ARGBToDWORD(a1, r1, g1, b1);
}

static inline DWORD ScaleColor(DWORD color1, DWORD r, DWORD g, DWORD b)
{
	DWORD a1, r1, g1, b1;
	DOWRDToRGB(color1, a1, r1, g1, b1);
	r1 = r1 * r / 256;     // 255
	g1 = g1 * g / 256;     // 255
	b1 = b1 * b / 256;     // 255

	return ARGBToDWORD(a1, r1, g1, b1);
}

// inlineº¯Êý·ÅÔÚÎÄ¼þµÄÇ°²¿£¬¿ÉÒÔÊ¹µÃRelease°æ±¾Ð§ÂÊ¸ü¸ß
inline unsigned int KRepresentShell3::GetPoint3dLighting(D3DXVECTOR3& v)
{
	if(!m_bDoLighting)
		return 0xff404040;

	unsigned uX = (unsigned)(int)(((int)v.x) - m_nLightingAreaX);
    if (uX >= 1536)
        return pLightingArray[0];
    
	unsigned uY = (unsigned)(int)(((int)v.y) - m_nLightingAreaY);
    if (uY >= 3072)
        return pLightingArray[0];
        
	return pLightingArray[
		// ÕâÀï²»ÄÜÐ´³ÉuY * LIGHTING_GRID_WIDTH / LIGHTING_GRID_SIZEY,ÒòÎªuY / LIGHTING_GRID_SIZEYÕâÒ»²½ÉáÈ¥Ð¡Êý²¿·ÖÊÇ±ØÐëµÄ
        (uY / LIGHTING_GRID_SIZEY * LIGHTING_GRID_WIDTH) +
        (uX / LIGHTING_GRID_SIZEX)
    ];
}

void __fastcall KRepresentShell3::SetPoint3dLighting(VERTEX3D& pDes, VERTEX3D& pSrc, DWORD color)
{
	pDes = pSrc;
	pDes.color = GetPoint3dLighting(pDes.position);
	if(color != 0xffffffff)
		pDes.color = ScaleColor(pDes.color, color);
}


static WORD g_A8ToA4[256] =
{
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
    0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,0x1000,
    0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
    0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
    0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,
    0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,0x3000,
    0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
    0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,0x4000,
    0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,
    0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,0x5000,
    0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,
    0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,
    0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,
    0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,
    0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,
    0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,
    0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,
    0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,0x9000,
    0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,
    0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,0xa000,
    0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,
    0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,0xb000,
    0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,
    0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,0xc000,
    0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,
    0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,0xd000,
    0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,
    0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,0xe000,
    0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,
    0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000,0xf000
};

// ½«sprÊý¾Ý×ª»»µ½A4R4G4B4»º³åÇø
void RenderToA4R4G4B4(
	WORD* pDest, uint32 nPitch, PBYTE pData,
	RECT& rect, uint32 nWidth, uint32 nHeight,
	WORD* pPalette
)
{
	uint32 nPixelCount = 0;
	uint32 nTotlePixel = nWidth * nHeight;
	uint32 nNextWidth = nWidth;
	uint32 pixelNum;
	BYTE* pTexLine = (BYTE*)pDest;
	pTexLine += rect.top * nPitch + rect.left * 2;
	BYTE* pLine = pTexLine;

	for (;;)
	{
		pixelNum = *pData++;
		nPixelCount += pixelNum;

		uint32 alpha = *pData++;
		uint32 uAlpha = g_A8ToA4[alpha]; // 
		if (uAlpha == 0)
		{

			pLine += pixelNum * 2;
			if (alpha != 0)
				pData += pixelNum;
		}
		else
		{
			while (pixelNum--)
			{
				WORD* pCurrentPixel = (WORD*)pLine;


				uint32 destAlpha = (*pCurrentPixel) & 0xF000;
				uint32 destColor = (*pCurrentPixel) & 0x0FFF;


				uint32 srcColor = pPalette[*pData] & 0x0FFF;
				uint32 srcAlpha = uAlpha >> 12; //
				uint32 invAlpha = 15 - srcAlpha;


				uint32 destR = (destColor >> 8) & 0xF;
				uint32 destG = (destColor >> 4) & 0xF;
				uint32 destB = destColor & 0xF;

				uint32 srcR = (srcColor >> 8) & 0xF;
				uint32 srcG = (srcColor >> 4) & 0xF;
				uint32 srcB = srcColor & 0xF;

				uint32 blendedR, blendedG, blendedB;


				if (srcAlpha > 12)
				{
					blendedR = srcR;
					blendedG = srcG;
					blendedB = srcB;
				}
				else
				{

					blendedR = (srcR * srcAlpha + destR * invAlpha) / 15;
					blendedG = (srcG * srcAlpha + destG * invAlpha) / 15;
					blendedB = (srcB * srcAlpha + destB * invAlpha) / 15;


					blendedR = min(blendedR + 1, 15);
					blendedG = min(blendedG + 1, 15);
					blendedB = min(blendedB + 1, 15);
				}


				uint32 blendedColor = ((blendedR & 0xF) << 8) |
					((blendedG & 0xF) << 4) |
					(blendedB & 0xF);


				uint32 finalAlpha = (uAlpha > destAlpha) ? uAlpha : destAlpha;


				*pCurrentPixel = (finalAlpha & 0xF000) | blendedColor;

				pData++;
				pLine += 2;
			}
		}


		if (nPixelCount >= nNextWidth)
		{
			pLine = pTexLine = pTexLine + nPitch;
			nNextWidth += nWidth;
		}

		assert(nPixelCount <= nTotlePixel);
		if (nPixelCount >= nTotlePixel)
			break;
	}
}

// äÖÈ¾´°¿ÚµÄ´°¿Úº¯Êý
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

// ½«ÊÀ½ç×ø±ê×ª»»µ½ÆÁÄ»×ø±ê
D3DXVECTOR3* WorldToScreen
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DVIEWPORT9 *pViewport,
      CONST D3DXMATRIX *pProjection, CONST D3DXMATRIX *pView, CONST D3DXMATRIX *pWorld)
{
	D3DXMATRIX mat = (*pView) * (*pProjection);
	D3DXVec3TransformCoord(pOut, pV, &mat);
	pOut->x = (pOut->x + 1.0f) * pViewport->Width / 2;
	pOut->y = (1.0f - pOut->y) * pViewport->Height / 2;
	return pOut;
}

// fd2,fd2 target area, fs1,fs2 original area, the position of the point in the original area
// Return the position of the point in the target area
inline float ChaZhi(float fd1, float fd2, float fs1, float fs2, float fs)
{
	return (fs - fs1) / (fs2 - fs1) * (fd2 - fd1) + fd1;
}

// Difference v1 and v2 according to the position of fs relative to fs1 and fs2
inline D3DXVECTOR3 ChaZhi(D3DXVECTOR3 v1, D3DXVECTOR3 v2, float fs1, float fs2, float fs)
{
	return (v2 - v1) * (fs - fs1) / (fs2 - fs1) + v1;
}

//=========²âÊÔÄ£¿éÐÔÄÜ£¬ÅÐ¶ÏÊÇ·ñÍÆ¼öÊ¹ÓÃ===============
extern "C" __declspec(dllexport)
bool RepresentIsModuleRecommended()
{
	return Test3D();
}

//=========´´½¨Ò»¸öiRepresentShell½Ó¿ÚµÄÊµÀý===============
extern "C" __declspec(dllexport)
iRepresentShell* CreateRepresentShell()
{
	return (new KRepresentShell3);
}

// [NAPCHIEU 09/09] nap truoc anh: Core goi qua GetProcAddress("Rep3_NapTruoc2") de khong doi vtable iRepresentShell.
// [NAPNPC 09/09] nNguon 1 = anh chieu (goi 95, cong tac Rep3NapChieu), 2 = anh than NPC (SetSprFile, cong tac Rep3NapNpc).
int KRepresentShell3::NapTruoc(const char* pszImage, int nNguon)
{
	if (nNguon < 1 || nNguon > 2) return 0;
	if (nNguon == 1 && !g_nRep3NapChieu) return 0;	// cong tac tat: khong nap truoc, khong dem
	if (nNguon == 2 && !g_nRep3NapNpc) return 0;
	const int n = m_TextureResMgr.NapTruoc(pszImage, ISI_T_SPR, nNguon);
	g_uRep3NapTruoc[nNguon][(n >= 0 && n <= 2) ? n : 0]++;
	return n;
}
extern "C" __declspec(dllexport)
int Rep3_NapTruoc(const char* pszImage)
{
	if (!g_pRep3ShellDuyNhat || !pszImage)
		return 0;
	return g_pRep3ShellDuyNhat->NapTruoc(pszImage, 1);
}
extern "C" __declspec(dllexport)
int Rep3_NapTruoc2(const char* pszImage, int nNguon)	// [NAPNPC 09/09]
{
	if (!g_pRep3ShellDuyNhat || !pszImage)
		return 0;
	return g_pRep3ShellDuyNhat->NapTruoc(pszImage, nNguon);
}
#ifdef JX_MOBILE
// [NENTRUOC 13/09] nen dat: Core (KScenePlaceRegionC) goi qua GetProcAddress("Rep3_NenTruocKhung") - chuan bi khung o nen o luong nen truoc khi ghep vung.
int KRepresentShell3::JxNenTruocKhung(const char* pszImage, int nFrame)
{
	return m_TextureResMgr.JxNenTruocKhung(pszImage, nFrame);
}
extern "C" __declspec(dllexport)
int Rep3_NenTruocKhung(const char* pszImage, int nFrame)	// [NENTRUOC 13/09]
{
	if (!g_pRep3ShellDuyNhat || !pszImage)
		return 0;
	return g_pRep3ShellDuyNhat->JxNenTruocKhung(pszImage, nFrame);
}
#endif
#ifdef JX_MOBILE
// [TG 13/09] Wnds.cpp goi qua GetProcAddress (nhu Rep3_NapTruoc2, khong doi vtable); iOS dang ky trong ios/JxIosMain.cpp
extern "C" __declspec(dllexport)
int Rep3_JxTheGioi(int nLenh, int nThamSo)
{
	if (!g_pRep3ShellDuyNhat) return 0;
	return g_pRep3ShellDuyNhat->JxTheGioi(nLenh, nThamSo);
}
// [KHOIDUTRU 14/09] S3Client GameSpaceChangedNotify (doi map that) goi qua GetProcAddress; iOS dang ky trong ios/JxIosMain.cpp
void Rep3Gpu_KhoiDuTru(IDirect3DDevice9*, int);	// D3D9onGPUDev.cpp - khai bao NGOAI ham extern "C" (khai bao trong than ham extern "C" bi lien ket C -> undefined symbol)
extern "C" __declspec(dllexport)
int Rep3_KhoiDuTru(int nLopToiThieu)
{
	if (!g_pRep3ShellDuyNhat || !PD3DDEVICE) return 0;
	Rep3Gpu_KhoiDuTru(PD3DDEVICE, nLopToiThieu);
	return 1;
}
#endif

IInlinePicEngineSink* g_pIInlinePicSinkRP = NULL;	//Ç¶ÈëÊ½Í¼Æ¬µÄ´¦Àí½Ó¿Ú[wxb 2003-6-20]
long KRepresentShell3::AdviseRepresent(IInlinePicEngineSink* pSink)	// [ANDROID 08/09] khop 'long' cua iRepresentShell.h (LP64)
{
	assert(NULL == g_pIInlinePicSinkRP);	//Ò»°ã²»»á¹Ò½ÓÁ½´Î
	g_pIInlinePicSinkRP = pSink;
	return S_OK;
}
long KRepresentShell3::UnAdviseRepresent(IInlinePicEngineSink* pSink)	// [ANDROID 08/09] khop 'long' cua iRepresentShell.h (LP64)
{
	if (pSink == g_pIInlinePicSinkRP)
		g_pIInlinePicSinkRP = NULL;
	return S_OK;
}

KRepresentShell3::KRepresentShell3()
{
	g_pRep3ShellDuyNhat = this;	// [NAPCHIEU 09/09]
	m_nLeft = 0;
	m_nTop = 0;
#ifdef JX_MOBILE
	m_pTgTex = NULL; m_pTgSurf = NULL; m_pTgSurfCu = NULL; m_pTgSB = NULL; m_nTgW = m_nTgH = 0; m_nTgTrangThai = 0; m_nTgLeft = m_nTgTop = 0; m_dwTgMauXoa = 0xff000000;	// [TG 13/09]
	m_nTgZoom = 1000; m_nZoomDx = m_nZoomDy = 0; m_nTgKhungW = m_nTgKhungH = 0; m_nTgLeftKhung = m_nTgTopKhung = 0;	// [ZOOM 13/09]
	m_pTgTex2 = NULL; m_pTgSurf2 = NULL; m_nTg2W = m_nTg2H = 0;	// [ZOOM3D 14/09] RT2 phong to
	m_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; m_nTg2CapW = m_nTg2CapH = 0;	// [TGCAP 14/09]
	m_nTgCapW = m_nTgCapH = m_nTgCapKhungW = m_nTgCapKhungH = 0; m_nTg2CapW = m_nTg2CapH = 0; m_nTgZoomMin = 1000;	// [TGCAP 14/09]
	m_nTgPxW = m_nTgPxH = 0; m_bTgNac = 0;	// [TGNAC 14/09]
	m_nTgXoay = 0; m_nTgLe = 1000; m_nTgZoomRt = 1000;	// [LAC 14/09]
	m_nTgDoc = 1000;	// [LAC 14/09 b]
#endif
	m_pPreRenderTexture128 = NULL;
	m_pPreRenderTexture256 = NULL;
	m_pPreRenderTexture512 = NULL;
	m_pVB2D = NULL;
	m_pVB3D = NULL;
	m_bDeviceLost = false;
	m_bDoLighting = true;
	m_nBlendMode = 0;		// [REP3 03/09]
	m_dwLastPresent = 0;
	m_fFpsAvg = 0.0f;
	m_pJxReplay = NULL;
	m_nReplayTime = 0;
	m_nReplayStatus = -1;
	memset(m_FontTable, 0, sizeof(m_FontTable));

	g_fZoomFactor = 1.00f;
}

KRepresentShell3::~KRepresentShell3()
{
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].pFontObj)
		{
			m_FontTable[i].pFontObj->Release();
			m_FontTable[i].pFontObj = NULL;
			m_FontTable[i].nId = 0;
		}
	}
}

void KRepresentShell3::SetOption(RepresentOption eOption,	bool bOn)
{
	switch (eOption)
	{
	case DYNAMICLIGHT:
		if(bOn)
			m_bDoLighting = false;
		else
			m_bDoLighting = false;
		break;
	case TEXTBOLD:	// [CHUDAM 09/09] chu dam cho ten / danh hieu (KNpc::PaintInfo bat, ve xong tat)
		KFont3::SetBold(bOn);
		break;
	case PERSPECTIVE:
		{	if (g_nRep3Flat)			// [REP3 03/09] ve phang: khong doi sang 3D
				g_renderModel = RenderModel2D;
			else if(bOn)
				g_renderModel = RenderModel3DOrtho;
			else
				g_renderModel = RenderModel3DOrtho;

			SetUpProjectionMatrix();
		}
		break;
	}
}

void KRepresentShell3::SetUpProjectionMatrix()
{
	// Set the projection transformation matrix according to g_renderModel
	float fAspect = (float)(g_nScreenWidth / g_nScreenHeight * 1.37);
	if(g_renderModel == RenderModel3DOrtho)
		D3DXMatrixOrthoLH(&m_matProj, g_nScreenWidth *g_fZoomFactor, g_nScreenHeight *g_fZoomFactor, 1.0f, 20000.0f );
	else
		D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/24, fAspect, 1.0f, 20000.0f );
	PD3DDEVICE->SetTransform( D3DTS_PROJECTION, &m_matProj );
}

bool KRepresentShell3::Create(int nWidth, int nHeight, bool bFullScreen)
{
	// [REP3 03/09] doc cong tac [Client] trong config.ini
	g_nRep3Flat      = Rep3Ini("Rep3Flat", 1);
	g_nRep3Composite = Rep3Ini("Rep3Composite", 0);
	g_nRep3Tex32     = Rep3Ini("Rep3Tex32", 1);
	g_nRep3Npot      = Rep3Ini("Rep3Npot", 1);
	g_nRep3Vsync     = Rep3Ini("Rep3Vsync", 0);
	if (Rep3Ini("PaintVsync", 0) > 0) g_nRep3Vsync = 1;	// [NHIP 08/09] Game.exe ve theo vblank -> Present(1)
	g_nRep3CacheMB   = Rep3Ini("Rep3CacheMB", 0);
	g_nRep3Log       = Rep3Ini("Rep3Log", 1);
	g_nRep3Pool      = Rep3Ini("Rep3Pool", 1);		// [REP3 03/09 RAM]
	g_nRep3Api       = Rep3Ini("Rep3Api", 11);
#ifdef JX_MOBILE
	g_nRep3Api = 100;	// [ANDROID 11/09 c] Android chi co SDL_GPU; doc ini co luc hong (khoi dong lai ngay sau khi dong app) -> "Rep3Api=11 ... lui ve D3D9" -> GameInit that bai
#endif	// [D3D11 08/09] [NAP 08/09 #0] mac dinh 11, tu lui D3D9 khi may khong du
	g_nRep3Atlas     = Rep3Ini("Rep3Atlas", 1);	// [D3D11 08/09 d]
	g_nRep3Flip      = Rep3Ini("Rep3Flip", 1);	// [D3D11 08/09 f] bitblt DISCARD bi DWM ghep giua chung -> "gon song" khi di chuyen
	g_nRep3Tearing   = Rep3Ini("Rep3Tearing", 0);	// [D3D11 08/09 f]
	g_nRep3Batch     = Rep3Ini("Rep3Batch", 1);	// [D3D11 08/09 j]
	g_nRep3AtlasMang = Rep3Ini("Rep3AtlasMang", 1);	// [MANG 09/09]
	{ int nLat = Rep3Ini("Rep3Latency", -1); g_nRep3Latency = (nLat >= 0) ? nLat : (g_nRep3Vsync ? 1 : 3); }	// [NHIP 08/09] vsync: hang 1 khung (do tre thap); khong vsync: 3 nhu cu	// [D3D11 08/09 o]
	g_nRep3NoWait    = Rep3Ini("Rep3NoWait", 0);	// [D3D11 08/09 l]
	g_nRep3Buffers   = Rep3Ini("Rep3Buffers", 3);	// [D3D11 08/09 o]
	g_nRep3Waitable  = Rep3Ini("Rep3Waitable", 0);	// [D3D11 08/09 o]
	g_nRep3Pal       = Rep3Ini("Rep3Pal", 1);	// [D3D11 08/09 r]
#ifdef JX_PLATFORM_SDL
	g_nRep3AtlasGpu    = Rep3Ini("Rep3AtlasGpu", g_nRep3AtlasGpu) ? 1 : 0;	// [GPU 11/09 ATLAS]
	g_nRep3GpuBoBanCpu = Rep3Ini("Rep3GpuBoBanCpu", g_nRep3GpuBoBanCpu) ? 1 : 0;	// [GPU 11/09 BOCPU]
	g_nRep3GpuMailbox  = Rep3Ini("Rep3GpuMailbox", g_nRep3GpuMailbox) ? 1 : 0;	// [ANDROID 11/09 MAILBOX]
#endif
	g_nRep3LocMs     = Rep3Ini("Rep3LocMs", 0);	// [LOCTG 09/09] [CHUGIU] mac dinh TAT: chu che toi; giu lam cong tac
	g_nRep3ChuGiuMs  = Rep3Ini("Rep3ChuGiuMs", 12);	// [CHUGIU 09/09]
	g_nRep3NapChieu  = Rep3Ini("Rep3NapChieu", 1) ? 1 : 0;	// [NAPCHIEU 09/09 b]
	g_nRep3NapNpc    = Rep3Ini("Rep3NapNpc", 1) ? 1 : 0;	// [NAPNPC 09/09]
	if (g_nRep3ChuGiuMs < 0) g_nRep3ChuGiuMs = 0;
	if (g_nRep3ChuGiuMs > 100) g_nRep3ChuGiuMs = 100;
	if (g_nRep3LocMs < 0) g_nRep3LocMs = 0;
	if (g_nRep3LocMs > 100) g_nRep3LocMs = 100;
	g_nRep3LocKieu   = Rep3Ini("Rep3LocKieu", 1) ? 1 : 0;	// [LOCTG b]
	{ int nToi = Rep3Ini("Rep3LocToi", 128); if (nToi < 8) nToi = 8; if (nToi > 255) nToi = 255; g_fRep3LocK = 255.0f / (float)nToi; }
	g_nRep3Ex        = Rep3Ini("Rep3Ex", 0);		// [RAM 08/09]
	if (g_nRep3Ex)
		g_nRep3Pool = 1;	// D3D9Ex khong co POOL_MANAGED: bat buoc dem SYSTEMMEM + DEFAULT
	g_nRep3NapNen    = Rep3Ini("Rep3NapNen", 1);	// [NAP 08/09 b]
#ifdef JX_MOBILE
	g_nJxNapKhungNen   = Rep3Ini("NapKhungNen", 1);		// [VE 11/09] 1 = nap khung sprite o luong nen khi het ngan sach dong bo (0 = nhu cu)
	g_nJxNapKhungMs    = Rep3Ini("NapKhungMs", 3);		// ngan sach nap dong bo tren luong ve moi khung (ms); qua thi giao luong nen, bo ve khung nay
	g_nJxNapKhungTruoc = Rep3Ini("NapKhungTruoc", 2);	// so khung KE TIEP cung huong nap truoc o luong nen (0 = tat)
	g_nJxNapKhungApMs  = Rep3Ini("NapKhungApMs", 3);	// ngan sach tao texture tu ket qua luong nen moi khung (ms)
	g_nJxNapKhungKB    = Rep3Ini("NapKhungKB", 128);	// [TAI 14/09] ngan sach tai dan khung nap truoc len GPU (KB/khung); 0 = tat (tai ca khung luc ve nhu cu)
	g_nJxNapKhungToKB  = Rep3Ini("NapKhungToKB", 128);	// [NAPTO 14/09] khung chua rut co co nen >= KB nay -> giao luong nen ngay, bo ve 1-3 khung; 0 = tat (rut dong bo khi con ngan sach nhu cu)
	g_nJxKhoiTruocPal  = Rep3Ini("Rep3KhoiTruocPal", 4);	// [KHOITRUOC 14/09] so khoi R8G8 cap san (0 = tat)
	g_nJxKhoiTruoc32   = Rep3Ini("Rep3KhoiTruoc32", 1);	// so khoi BGRA8 cap san
	{ extern int g_nJxTaiDo; g_nJxTaiDo = Rep3Ini("TaiDo", 0); }	// [TAI-DO 14/09] 1 = do duong tai len GPU luc khoi dong thiet bi (chi log [TAI-DO], ~0,4 s); [DEM 14/09] mac dinh 0
	g_nJxVeGiatMs      = Rep3Ini("VeGiatMs", 20);		// ghi [VE-GIAT] khi ve CPU + trinh chieu (hoac nap ngoai luc ve) cua mot khung vuot nguong (ms); 0 = tat
	g_nJxHoiKhongDe    = Rep3Ini("NapHoiKhongDe", 1);	// [VE 11/09 d] 1 = hoi kich thuoc sprite NPC dang nap o luong nen -> tra 'chua co' (khong nap dong bo de len)
	g_nJxAtlasKe       = Rep3Ini("Rep3AtlasKe", 1) ? 1 : 0;	// [VE 11/09 e] 1 = atlas xep ke theo dinh dang (khung cung NPC cung trang -> gop lenh), 0 = trang theo bin cao nhu cu
	g_nJxAtlasTrang    = Rep3Ini("Rep3AtlasTrang", 2048);	// co trang atlas 1024 / 2048 / 4096
	if (g_nJxAtlasTrang != 1024 && g_nJxAtlasTrang != 2048 && g_nJxAtlasTrang != 4096) g_nJxAtlasTrang = 2048;
	g_nJxPalBuffer      = Rep3Ini("Rep3PalBuffer", 1) ? 1 : 0;	// [PALBUF 11/09] 1 = bang mau trong storage buffer (het khung chep 17-100 ms khi tai hang bang mau vao texture 256x8192); 0 = texture nhu cu
#ifdef JX_APPLE	// [MOBILE 13/09] Metal: bind ring dung duoc (drawPrimitives:vertexStart:), KHOI tu tat trong CreateShaders khi thieu JX_MSL_CO_KHOI;
	g_nJxAtlasMang = 0;	// texture mang chua co bien the MSL (Android cung mac dinh tat sau khi do Fold 7 16:22)
#endif	// [PALBUF 11/09] 1 = bang mau trong storage buffer (het khung chep 17-100 ms khi tai hang bang mau vao texture 256x8192); 0 = texture nhu cu
	g_nJxBoKhungGiong   = Rep3Ini("Rep3BoKhungGiong", 1);	// [BKG 11/09] 1 = khung giong het khung vua trinh chieu -> khong trinh chieu; 0 = chi dem [VE-BKG]; -1 = tat han (khong so sanh)
	g_nJxBoKhungGiongMs = Rep3Ini("Rep3BoKhungGiongMs", 250);	// toi da ms giua hai lan trinh chieu khi khung giong (0 = khong gioi han)
	if (g_nJxBoKhungGiong > 1) g_nJxBoKhungGiong = 1; if (g_nJxBoKhungGiong < -1) g_nJxBoKhungGiong = -1;
#ifdef JX_MOBILE
	{ extern int g_nJxONenLog; g_nJxONenLog = Rep3Ini("Rep3ONenLog", 0); if (g_nJxONenLog < 0) g_nJxONenLog = 0; if (g_nJxONenLog > 64) g_nJxONenLog = 64; }	// [ONEN 15/09]
#endif
	g_nJxPsBuffer       = Rep3Ini("Rep3PsBuffer", 1) ? 1 : 0;	// [GOP 11/09] 1 = trang thai tang texture qua storage buffer, chi so theo dinh (hai quad khac ps van gop duoc; bot 800 lan day uniform/khung)
	if (!g_nJxPalBuffer) g_nJxPsBuffer = 0;	// shader PC khong co buffer nao
	g_nJxAtlasMang      = Rep3Ini("Rep3AtlasMangGpu", 0) ? 1 : 0;	// [MANG 11/09] MAC DINH 0 sau khi do tren Fold 7 16:22: gop lenh CO giam (doi texture 1043 -> 508, lenh 2001 -> 1222)
			// nhung nop 1,42 -> 3,28 ms va ve CPU 2,96 -> 4,51 -> fps tut, giat khi di chuyen man hinh. Giu ma sau cong tac; 1 = bat lai de thu tiep
	if (!g_nJxPsBuffer) g_nJxAtlasMang = 0;	// lop di chung o PALROW voi chi so ps: can shader bien the pal+ps
	{ int n = Rep3Ini("Rep3AtlasLop", 8); if (n < 2) n = 2; if (n > 32) n = 32; g_nJxAtlasLop = n; }	// so lop toi da moi cum
	{ int n = Rep3Ini("Rep3AtlasCumMB", 64); if (n < 8) n = 8; if (n > 256) n = 256; g_nJxAtlasCumMB = n; }	// ngan sach byte moi cum
	g_nJxDoVeChiTiet    = Rep3Ini("Rep3DoVeChiTiet", 0) ? 1 : 0;	// [VECHITIET 11/09] 1 = tach 've CPU' thanh phan trong lop ve va phan dung canh cua game
	g_nJxAtlasKhoi      = Rep3Ini("Rep3AtlasKhoi", 0) ? 1 : 0;	// [KHOI 11/09] 1 = atlas theo khoi co dinh, moi khoi gan chet mot khe sampler (port buoc (f) cua [MANG 09/09]); MAC DINH TAT
	g_nJxAtlasKhoiLop   = Rep3Ini("Rep3AtlasKhoiLop", 8);	// [KHOI 11/09] so trang atlas moi khoi (1..8; 8 trang 2048 x 2 byte = 64 MB/khoi)
	g_nJxCullCpu        = Rep3Ini("Rep3CullCpu", 1) ? 1 : 0;	// [CULLCPU 11/09] 1 = cull tam giac 2D tren CPU de chu gop chung lo voi sprite (port buoc (e) cua [MANG 09/09]); 0 = nhu cu
	g_nJxAtlasManaged   = Rep3Ini("Rep3AtlasManaged", 1) ? 1 : 0;	// [CHUATLAS 11/09] 1 = texture MANAGED (chu, anh dung san) cung vao atlas (port buoc (d) cua [MANG 09/09]); 0 = nhu cu
	g_nJxBindRing       = Rep3Ini("Rep3BindRing", 1) ? 1 : 0;	// [GOP 11/09] 1 = bind ring dinh mot lan moi render pass, lenh ve dung first_vertex (bot 1 lenh Vulkan moi draw)
	g_nJxSwapchainLogic = Rep3Ini("Rep3SwapchainLogic", 100);	// [D1 11/09] swapchain = backbuffer x %/100 (100 = khung logic 1040x936/1436x616, GPU to it diem hon 3-4,4 lan; 150 = 1,5x; 0 = cua so nhu cu)
	if (g_nJxSwapchainLogic < 0) g_nJxSwapchainLogic = 0; if (g_nJxSwapchainLogic > 0 && g_nJxSwapchainLogic < 50) g_nJxSwapchainLogic = 50; if (g_nJxSwapchainLogic > 400) g_nJxSwapchainLogic = 400;
	Rep3Log("[VE] nap khung nen=%d, ngan sach %d ms/khung, nap truoc %d khung, ap %d ms/khung; nguong [VE-GIAT] %d ms", g_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_nJxVeGiatMs);
	Rep3Log("[VE] bang mau kieu %s (Rep3PalBuffer=%d); bo khung giong khung truoc: %d (Rep3BoKhungGiong; toi da %d ms giua hai lan trinh chieu); ps theo dinh=%d, bind ring mot lan=%d", g_nJxPalBuffer ? "storage buffer" : "texture 256x8192", g_nJxPalBuffer, g_nJxBoKhungGiong, g_nJxBoKhungGiongMs, g_nJxPsBuffer, g_nJxBindRing);	// [PALBUF 11/09] [BKG 11/09] [GOP 11/09]
	Rep3Log("[VE] atlas mang 2D=%d (Rep3AtlasMang; toi da %d lop/cum, ngan sach %d MB/cum)", g_nJxAtlasMang, g_nJxAtlasLop, g_nJxAtlasCumMB);	// [MANG 11/09]
#endif
	g_nRep3StatSec   = Rep3Ini("Rep3StatSec", 30);
#ifdef JX_MOBILE
	JxTheGioiDocIni();	// [TG 13/09]
#endif
	m_TextureResMgr.SetBudget();	// [REP3 03/09 RAM] doc Rep3CacheMB SAU khi doc ini (ctor chay truoc Create)
	g_bUse4444Texture = (g_nRep3Tex32 == 0);
	if (g_nRep3Flat)
		g_renderModel = RenderModel2D;
	Rep3Log("[REP3] Create %dx%d full=%d flat=%d composite=%d tex32=%d npot=%d vsync=%d cacheMB=%d pool=%s statSec=%d ex=%d",
		nWidth, nHeight, (int)bFullScreen, g_nRep3Flat, g_nRep3Composite, g_nRep3Tex32, g_nRep3Npot, g_nRep3Vsync, g_nRep3CacheMB,
		g_nRep3Pool ? "DEFAULT(VRAM)" : "MANAGED(RAM+VRAM)", g_nRep3StatSec, g_nRep3Ex);	// [REP3 03/09 RAM]
	if (!g_D3DShell.Create())
		return false;
	g_DebugLog("[D3DRender]g_D3DShell create ok!");

	g_hWnd = g_GetMainHWnd();
	if(!g_hWnd)
		return false;

	g_nScreenWidth = nWidth;
	g_nScreenHeight = nHeight;
	g_bRunWindowed = !bFullScreen;

	if(!g_bRunWindowed)
	{
		LONG winLong = GetWindowLong(g_hWnd, GWL_STYLE);
		winLong &= ~WS_BORDER;
		winLong &= ~WS_CAPTION;
		SetWindowLong(g_hWnd, GWL_STYLE, winLong);
	}


	if (g_bRunWindowed)
	{
		// [REP3 03/09] cua so phai dung nWidth x nHeight de Present 1:1 (nhu KDirectDraw::SetWindowStyle).
		// Game.exe cu (truoc 03/09) tu keo cao them 40 px va co toa do chuot theo 40/808:
		// gap dung dau hieu do thi GIU NGUYEN de chuot khop, chap nhan hinh bi keo 5%.
		RECT rcClient = {0, 0, 0, 0};
		GetClientRect(g_hWnd, &rcClient);
		int nCW = rcClient.right - rcClient.left;
		int nCH = rcClient.bottom - rcClient.top;
		if (nCW == nWidth && nCH == nHeight + 40)
		{
			Rep3Log("[REP3] cua so %dx%d = Game.exe cu (+40 px), giu nguyen", nCW, nCH);
		}
		else if (nCW != nWidth || nCH != nHeight)
		{
			RECT	rc = {0, 0, nWidth, nHeight};
			DWORD	dwStyle = WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX;
			SetWindowLong(g_hWnd, GWL_STYLE, dwStyle);
			AdjustWindowRectEx(&rc, dwStyle, GetMenu(g_hWnd) != NULL, GetWindowLong(g_hWnd, GWL_EXSTYLE));
			SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
				SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
			Rep3Log("[REP3] cua so %dx%d -> dat lai %dx%d", nCW, nCH, nWidth, nHeight);
		}
	}
	D3DAdapterInfo* pAdapterInfo	 = NULL;
	D3DDeviceInfo*  pDeviceInfo		 = NULL;
	D3DModeInfo*	pModeInfo		 = NULL;

	pDeviceInfo = g_D3DShell.PickDefaultDev(&pAdapterInfo);
	if (!pDeviceInfo)
	{
		// ÎÞ·¨ÕÒµ½ºÏÊÊµÄD3DÉè±¸
		D3DTerm();
		g_DebugLog("[D3DRender]Can't find any d3d devices to use!");
		return false; 
	}

	pModeInfo = g_D3DShell.PickDefaultMode(pDeviceInfo,DEFAULT_BITDEPTH);
	if (!pModeInfo)
	{
		// ÎÞ·¨ÕÒµ½ºÏÊÊµÄÏÔÊ¾Ä£Ê½
		D3DTerm();
		g_DebugLog("[D3DRender]Can't find an appropriate display mode!");
		return false;
	}

	// ´´½¨Éè±¸
	if (!g_Device.CreateDevice(pAdapterInfo,pDeviceInfo,pModeInfo))
	{
		D3DTerm();
		g_DebugLog("[D3DRender]Couldn't create D3D Device!");
		return false;
	}

	// ÉèÖÃÏÔÊ¾Ä£Ê½
	if (!Reset(nWidth, nHeight, bFullScreen))
		return false;
	g_DebugLog("[D3DRender]Device reset ok!");

	if (!RestoreDeviceObjects())
		return false;
	g_DebugLog("[D3DRender]RestoreDeviceObjects ok!");
	Rep3Log("[REP3] D3D9Ex: %s", g_nRep3ExOn ? "BAT (Rep3Ex=1, texture MANAGED -> DYNAMIC+DEFAULT)" : "tat");
	Rep3Log("[REP3] API: %s", g_nRep3ApiOn == 11 ? "Direct3D 11 (lop D3D9 tren D3D11, Rep3Api=11)" : "Direct3D 9");	// [D3D11 08/09]	// [RAM 08/09]
	m_TextureResMgr.CapBudgetByVram((unsigned __int64)(PD3DDEVICE->GetAvailableTextureMem() >> 20));	// [FX 08/09] kep ngan sach theo VRAM con

	if (!InitDeviceObjects())
		return false;
	g_DebugLog("[D3DRender]InitDeviceObjects ok!");

	PD3DDEVICE->SetDialogBoxMode(true);

	PD3DDEVICE->GetGammaRamp(0, &m_ramp);

	SetGamma(50);

	// ³õÊ¼»¯Gdi+
	InitGdiplus();

	return true;
}

bool KRepresentShell3::InitDeviceObjects()
{
	// ´´½¨Ô¤äÖÈ¾Ö÷½ÇµÄÌùÍ¼
	if (FAILED(PD3DDEVICE->CreateTexture(SPR_PRERENDER_TEXSIZE1, SPR_PRERENDER_TEXSIZE1, 1,
								REP3_USAGE_MANAGED, D3DFMT_A4R4G4B4, REP3_POOL_MANAGED, &m_pPreRenderTexture128, NULL)))
		return false;
	if (FAILED(PD3DDEVICE->CreateTexture(SPR_PRERENDER_TEXSIZE2, SPR_PRERENDER_TEXSIZE2, 1,
								REP3_USAGE_MANAGED, D3DFMT_A4R4G4B4, REP3_POOL_MANAGED, &m_pPreRenderTexture256, NULL)))
		return false;
	if (FAILED(PD3DDEVICE->CreateTexture(SPR_PRERENDER_TEXSIZE3, SPR_PRERENDER_TEXSIZE3, 1,
								REP3_USAGE_MANAGED, D3DFMT_A4R4G4B4, REP3_POOL_MANAGED, &m_pPreRenderTexture512, NULL)))
		return false;
	return true;
}

void KRepresentShell3::Release()
{
	DeleteDeviceObjects();
	D3DTerm();
	ShutdownGdiplus();
	delete this;
}

void KRepresentShell3::DeleteDeviceObjects()
{
#ifdef JX_MOBILE
	JxTheGioiHuy();	// [TG 13/09]
#endif

	SAFE_RELEASE( m_pPreRenderTexture128 );
	SAFE_RELEASE( m_pPreRenderTexture256 );
	SAFE_RELEASE( m_pPreRenderTexture512 );
	SAFE_RELEASE( m_pVB3D );
	SAFE_RELEASE( m_pVB2D );
	m_TextureResMgr.Free();
}

bool KRepresentShell3::InvalidateDeviceObjects()
{
#ifdef JX_MOBILE
	JxTheGioiHuy();	// [TG 13/09]
#endif
	SAFE_RELEASE( m_pVB2D );
	SAFE_RELEASE( m_pVB3D );

	if(!m_TextureResMgr.InvalidateDeviceObjects())
		return false;

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].pFontObj)
		{
			m_FontTable[i].pFontObj->InvalidateDeviceObjects();
		}
	}
	return true;
}

bool KRepresentShell3::RestoreDeviceObjects()
{
	int i;
	if(!m_pVB2D)
	{
		// ´´½¨·ÇÍ¸ÊÓ×´Ì¬Ê¹ÓÃµÄ¶¥µã»º³åÇø
		if( FAILED(PD3DDEVICE->CreateVertexBuffer( VERTEX_BUFFER_SIZE*sizeof(VERTEX2D),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_SOFTWAREPROCESSING, 
						D3DFVF_VERTEX2D, D3DPOOL_DEFAULT, &m_pVB2D, NULL)))
			return false;
	}

	if((g_renderModel == RenderModel3DOrtho || g_renderModel == RenderModel3DPerspective) && !m_pVB3D)
	{
		// ´´½¨Í¸ÊÓ×´Ì¬Ê¹ÓÃµÄ¶¥µã»º³åÇø
		if( FAILED(PD3DDEVICE->CreateVertexBuffer( VERTEX_BUFFER_SIZE*sizeof(VERTEX3D),
						D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_SOFTWAREPROCESSING, 
						D3DFVF_VERTEX3D, D3DPOOL_DEFAULT, &m_pVB3D, NULL)))
			return false;
	}

	for (i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].pFontObj)
		{
			m_FontTable[i].pFontObj->RestoreDeviceObjects();
		}
	}

	if(!m_TextureResMgr.RestoreDeviceObjects())
		return false;

	// ÉèÖÃäÖÈ¾×´Ì¬
	// Ê¹ÓÃAlpha»ìºÏ
	PD3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    PD3DDEVICE->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
    PD3DDEVICE->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
	// ¹Ø±ÕAlpha²âÊÔ
    PD3DDEVICE->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    PD3DDEVICE->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
    PD3DDEVICE->SetRenderState( D3DRS_CULLMODE,   D3DCULL_NONE );
    PD3DDEVICE->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    PD3DDEVICE->SetRenderState( D3DRS_CLIPPING,         FALSE );
    PD3DDEVICE->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
    PD3DDEVICE->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
    PD3DDEVICE->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
    PD3DDEVICE->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	PD3DDEVICE->SetRenderState( D3DRS_LIGHTING,FALSE );
	// ÉèÖÃÌùÍ¼äÖÈ¾½×¶Î0
	// ÉèÖÃÑÕÉ«»ìºÏÄ£Ê½
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	// ÉèÖÃAlpha»ìºÏÄ£Ê½
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	// ÉèÖÃ¹ýÂËÄ£Ê½
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	// [REP3 03/09] NPOT co dieu kien bat buoc CLAMP; UV cua ta luon trong [0,1] nen khong doi hinh
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	PD3DDEVICE->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    PD3DDEVICE->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	// ¹Ø±Õ0ÒÔÉÏµÄÌùÍ¼äÖÈ¾½×¶Î
    PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	g_Device.SetupViewport(0, g_nScreenWidth, 0, g_nScreenHeight, 0.0f, 1.0f);

	if(g_renderModel == RenderModel3DPerspective || g_renderModel == RenderModel3DOrtho)
	{
		// ÉèÖÃÊÀ½ç×ø±ê±ä»»¾ØÕó
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		PD3DDEVICE->SetTransform( D3DTS_WORLD, &matWorld );

		// ÉèÖÃÍ¶Ó°±ä»»¾ØÕó
		SetUpProjectionMatrix();
		PD3DDEVICE->SetTransform( D3DTS_PROJECTION, &m_matProj );
	}

	PD3DDEVICE->SetPixelShader( NULL );

	return true;
}

bool KRepresentShell3::Reset(int nWidth, int nHeight, bool bFullScreen)
{
	g_Device.ReleaseDevObjects();

	D3DAdapterInfo* pAdapterInfo	 = NULL;
	D3DDeviceInfo*  pDeviceInfo		 = NULL;
	D3DModeInfo*	pModeInfo		 = NULL;

	pAdapterInfo  = g_Device.GetAdapterInfo();
	pDeviceInfo	  = g_Device.GetDeviceInfo();
	pModeInfo	  = g_D3DShell.PickDefaultMode(pDeviceInfo,DEFAULT_BITDEPTH);

	if (!pModeInfo)
	{
		// ÎÞ·¨ÕÒµ½ºÏÊÊµÄÏÔÊ¾Ä£Ê½
		D3DTerm();
		g_DebugLog("[D3DRender]Can't find an appropriate display mode!");
		return false;
	}

	if (!g_Device.SetMode(pModeInfo))
	{
		// ÎÞ·¨ÉèÖÃºÏÊÊµÄÏÔÊ¾Ä£Ê½
		D3DTerm();
		g_DebugLog("[D3DRender]Can't find an appropriate display mode!");
		return false;
	}
	
	if((pModeInfo->Format == D3DFMT_X1R5G5B5))
		g_16BitFormat = D3DFMT_X1R5G5B5;
	else
		g_16BitFormat = D3DFMT_R5G6B5;
	
	g_Device.SetDefaultRenderStates();

	g_Device.RestoreDevObjects();

	return true;
}

bool KRepresentShell3::CreateAFont(const char* pszFontFile, CHARACTER_CODE_SET CharaSet, int nId)
{
	if(m_bDeviceLost)
		return false;

	int nFirstFree = -1;
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].pFontObj == NULL && nFirstFree == -1)
			nFirstFree = i;
		else if (m_FontTable[i].nId == nId)
		{
			nFirstFree = i;
			break;
		}	
	}
	
	if (nFirstFree == -1 || pszFontFile == NULL || nId == 0)
		return false;

	if (m_FontTable[nFirstFree].pFontObj)
	{
		m_FontTable[nFirstFree].pFontObj->Release();
		m_FontTable[nFirstFree].pFontObj = NULL;
	}

	if (pszFontFile[0] == '#')
	{
		//¹²ÏíÒÑ¾­´ò¿ªµÄ×Ö¿â
		int nShareWithId = atoi(pszFontFile + 1);
		for (int j = 0; j < RS2_MAX_FONT_ITEM_NUM; j++)
		{
			if (nFirstFree != j &&	m_FontTable[j].nId == nShareWithId &&
				m_FontTable[j].pFontObj)
			{
				m_FontTable[nFirstFree].nId = nId;
				m_FontTable[nFirstFree].pFontObj = (KFont3*)m_FontTable[j].pFontObj->Clone();
				return true;
			}
		}
		return false;
	}
	
	if ((m_FontTable[nFirstFree].pFontObj = new KFont3) == NULL)
		return false;

	m_FontTable[nFirstFree].pFontObj->Init(PD3DDEVICE);
	if (!m_FontTable[nFirstFree].pFontObj->Load((LPSTR)pszFontFile/*, CharaSet*/))
	{
		m_FontTable[nFirstFree].pFontObj->Release();
		m_FontTable[nFirstFree].pFontObj = NULL;
	}
	
	m_FontTable[nFirstFree].nId = nId;
#ifdef JX_POSIX	// [ANDROID 08/09] chan doan: font nao nap duoc
	Rep3Log("[FONT] CreateAFont id=%d tep=%s -> %s", nId, pszFontFile,
		m_FontTable[nFirstFree].pFontObj ? "OK" : "HONG");
#endif
	return (m_FontTable[nFirstFree].pFontObj != NULL);
}

unsigned int KRepresentShell3::CreateImage(const char* pszName, int nWidth, int nHeight, int nType)
{
	if(m_bDeviceLost)
		return 0;

	return m_TextureResMgr.CreateImage(pszName, nWidth, nHeight, nType);
}

// [VE 09/09 d] giu khoa TextureResMgr mot lan cho ca lo lenh (GetImage tren cung luong bo khoa; ~3 000 cap Enter/Leave/khung)
struct Rep3KhoaNgoai { TextureResMgr& m; Rep3KhoaNgoai(TextureResMgr& t) : m(t) { m.KhoaNgoaiVao(); } ~Rep3KhoaNgoai() { m.KhoaNgoaiRa(); } };
void KRepresentShell3::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)
{
	Rep3VeDpTimer veDp;	// [VE 08/09 b]
	Rep3KhoaNgoai khoaNgoai(m_TextureResMgr);	// [VE 09/09 d]
	if(!pPrimitives)
	{
		assert(pPrimitives);
		return;
	}

	if(m_bDeviceLost)
		return;

	int i = 0;
#ifdef JX_MOBILE
	int bLopChu = 0; if (bSinglePlaneCoord == 2) { bLopChu = 1; bSinglePlaneCoord = 0; }	// [CHUNET 14/09 d] KNpc::PaintInfo (mobile): icon canh ten = toa do the gioi + lop thong tin
	if (uGenre == RU_T_IMAGE && bLopChu && TgChuXep()) { for (i = 0; i < nPrimitiveCount; i++) TgChuThem(4, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, ((KRUImage*)pPrimitives) + i); return; }
	if (uGenre == RU_T_SHADOW && !bSinglePlaneCoord && nPrimitiveCount == 1 && TgChuXep()) { TgChuThem(3, 0, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, (KRUShadow*)pPrimitives, NULL); return; }	// [CHUNET 14/09] nen mo sau ten + thanh mau (KNpc VeNenChu / PaintInfo, luon 1 hat): ve sau blit cung chu; KWeather ve ca lo hat mua / tuyet -> khong xep (soi cheo)
#endif
	
	switch(uGenre)
	{
	case RU_T_IMAGE:
	case RU_T_IMAGE_4:
		{
			if( g_renderModel == RenderModel2D || bSinglePlaneCoord)
			{
				if (g_nRep3Composite)
					DrawImage2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord);
				else	// [REP3 03/09] ve tung sprite y nhu Represent2
					DrawImage2DFlat(nPrimitiveCount, pPrimitives, bSinglePlaneCoord, uGenre == RU_T_IMAGE_4);
			}
			else
				DrawImage3D(uGenre, nPrimitiveCount, pPrimitives, bSinglePlaneCoord);
		}
		break;
	case RU_T_IMAGE_STRETCH:
		if(bSinglePlaneCoord)
			DrawImage2DStretch(nPrimitiveCount, pPrimitives);
		break;
	case RU_T_IMAGE_PART:
		{
			KRUImagePart* pTemp = (KRUImagePart *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{				
				switch(pTemp->nType)
				{
				case ISI_T_SPR:
					{
						TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
							pTemp->szImage,	pTemp->uImage,
							pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
						if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
							{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
							  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); break; }
#else
							{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); break; }
#endif

						int nX = pTemp->oPosition.nX;
						int nY = pTemp->oPosition.nY;
						if (!bSinglePlaneCoord)
							CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
						if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
						{
							nX -= pSprite->GetCenterX();
							nY -= pSprite->GetCenterY();
						}

						if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
						{
							nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
							nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
						}

						// Clipper
						RECT rc;
						rc.left  = nX;
						rc.top   = nY;
						nX -= pTemp->oImgLTPos.nX;
						nY -= pTemp->oImgLTPos.nY;
						rc.right = nX + pTemp->oImgRBPos.nX;
						rc.bottom= nY + pTemp->oImgRBPos.nY;
						
						switch(pTemp->bRenderStyle)
						{
						case IMAGE_RENDER_STYLE_BORDER:				// [REP3 03/09]
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
						case IMAGE_RENDER_STYLE_ALPHA:
						case IMAGE_RENDER_STYLE_3LEVEL:
						case IMAGE_RENDER_STYLE_OPACITY:
						case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
						{
							DrawSpritePartAlpha(nX, nY, pSprite->m_pFrameInfo[pTemp->nFrame].nWidth, 
								pSprite->m_pFrameInfo[pTemp->nFrame].nHeight, pTemp->nFrame, pSprite, rc, pTemp->Color.Color_dw, pTemp->bRenderStyle);
							break;
						}
						}
					}
					break;
				case ISI_T_BITMAP16:
					{
						TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
						pTemp->szImage,	pTemp->uImage,
						pTemp->nISPosition, pTemp->nFrame, pTemp->nType);

						if (pBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
	
							RECT rc;
							rc.left  = pTemp->oImgLTPos.nX;
							rc.top   = pTemp->oImgLTPos.nY;
							rc.right = pTemp->oImgRBPos.nX;
							rc.bottom= pTemp->oImgRBPos.nY;
							DrawBitmap16Part(nX, nY,
								pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap, rc);
						}
					}
					break;
				}
			}
		}
		break;
	case RU_T_POINT:
		{	
			DrawPoint(nPrimitiveCount, pPrimitives, bSinglePlaneCoord);
		}
		break;
	case RU_T_LINE:
		{
			KRULine* pTemp = (KRULine *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				int	nX1 = pTemp->oPosition.nX;
				int nY1 = pTemp->oPosition.nY;
					
				int nX2 = pTemp->oEndPos.nX;
				int nY2 = pTemp->oEndPos.nY;
				if (!bSinglePlaneCoord)
				{
					CoordinateTransform(nX1, nY1, pTemp->oPosition.nZ);
					CoordinateTransform(nX2, nY2, pTemp->oEndPos.nZ);
				}
				DrawLine(nX1, nY1, nX2, nY2, D3DCOLOR_ARGB((DWORD)pTemp->Color.Color_b.a, (DWORD)pTemp->Color.Color_b.r, 
							(DWORD)pTemp->Color.Color_b.g, (DWORD)pTemp->Color.Color_b.b));
			}
		}
		break;
	case RU_T_RECT:
		{
			KRURect* pTemp = (KRURect *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				int	nX1 = pTemp->oPosition.nX;
				int nY1 = pTemp->oPosition.nY;
					
				int nX2 = pTemp->oEndPos.nX;
				int nY2 = pTemp->oEndPos.nY;
				if (!bSinglePlaneCoord)
				{
					CoordinateTransform(nX1, nY1, pTemp->oPosition.nZ);
					CoordinateTransform(nX2, nY2, pTemp->oEndPos.nZ);
				}
				DrawRectFrame(nX1, nY1, nX2, nY2, D3DCOLOR_ARGB(255, (DWORD)pTemp->Color.Color_b.r, 
							(DWORD)pTemp->Color.Color_b.g, (DWORD)pTemp->Color.Color_b.b));
			}
		}
		break;
	case RU_T_SHADOW:
		{
			KRUShadow* pTemp =(KRUShadow *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{				
				int nX1 = pTemp->oPosition.nX;
				int nY1 = pTemp->oPosition.nY;
				int nX2 = pTemp->oEndPos.nX;
				int	nY2 = pTemp->oEndPos.nY;
				if (!bSinglePlaneCoord)
				{
					CoordinateTransformX(nX1, nY1, pTemp->oPosition.nZ);
					CoordinateTransformX(nX2, nY2, pTemp->oEndPos.nZ);	// No nEndZ? Must be Single Plane Coord?
				}
				DrawRect(nX1, nY1, nX2 - nX1, nY2 - nY1, D3DCOLOR_ARGB(255 - (pTemp->Color.Color_b.a<<3), 
					(DWORD)pTemp->Color.Color_b.r, (DWORD)pTemp->Color.Color_b.g, (DWORD)pTemp->Color.Color_b.b));
			}
		}
		break;
	default:
		break;
	}
}

void KRepresentShell3::DrawImage2D(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord)
{
	// ÅÐ¶ÏÊÇ·ñÖ÷½ÇÀà
	if(nPrimitiveCount >= 4)
	{
		DrawPlayer2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord);
		return;
	}

	int i;
	KRUImage* pTemp = (KRUImage*)pPrimitives;

	for (i = 0; i < nPrimitiveCount; i++, pTemp++)
	{	
		switch(pTemp->nType)
		{
		case ISI_T_SPR:
			{
				TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
					{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
					  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#else
					{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#endif

				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
				
				if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
				{
#define CENTERX		160
#define	CENTERY		192
					int nCenterX = pSprite->GetCenterX();
					int nCenterY = pSprite->GetCenterY();
					if (nCenterX || nCenterY)
					{
						nX -= nCenterX;
						nY -= nCenterY;
					}
					else if (pSprite->GetWidth() > CENTERX)
					{
						nX -= CENTERX;
						nY -= CENTERY;
					}
				}
				
				if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
				{
					nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
					nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
				}
				DrawSpriteAlpha(nX, nY, pSprite->m_pFrameInfo[pTemp->nFrame].nWidth, 
									pSprite->m_pFrameInfo[pTemp->nFrame].nHeight, 
									pTemp->nFrame, pSprite, pTemp->Color.Color_dw, pTemp->bRenderStyle);
			}
			break;
		case ISI_T_BITMAP16:
			{
				TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				{	// [ANHNEN 10/09 d]
					static int s_nGhiBm = 0;	// [ANHNEN 10/09 e]
					if (s_nGhiBm < 10 && !strstr(pTemp->szImage, "Login") && !strstr(pTemp->szImage, "login"))
					{
						s_nGhiBm++;
						DWORD dwCull = 0, dwBlend = 0, dwSrc = 0, dwDst = 0, dwAT = 0, dwARef = 0, dwAFunc = 0, dwCOp = 0, dwCA1 = 0, dwCA2 = 0;
						PD3DDEVICE->GetRenderState(D3DRS_CULLMODE, &dwCull);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwBlend);
						PD3DDEVICE->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
						PD3DDEVICE->GetRenderState(D3DRS_DESTBLEND, &dwDst);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAT);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHAREF, &dwARef);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHAFUNC, &dwAFunc);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOp);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG2, &dwCA2);
						Rep3Log("[ANHNEN] bitmap16 %s: %s | tai %d,%d | co %dx%d | tex %dx%d | ptex %p | cull %u tron %u (%u/%u) at %u ham %u ref %u | op %u a1 %u a2 %u",
							pTemp->szImage, pBitmap ? "CO" : "NULL", pTemp->oPosition.nX, pTemp->oPosition.nY,
							pBitmap ? pBitmap->m_nWidth : 0, pBitmap ? pBitmap->m_nHeight : 0,
							pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nWidth : 0, pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nHeight : 0,
							pBitmap ? (void*)pBitmap->m_FrameInfo.texInfo[0].pTexture : NULL,
							(unsigned)dwCull, (unsigned)dwBlend, (unsigned)dwSrc, (unsigned)dwDst, (unsigned)dwAT, (unsigned)dwAFunc, (unsigned)dwARef,
							(unsigned)dwCOp, (unsigned)dwCA1, (unsigned)dwCA2);
					}
				}
				if (!pBitmap)
					break;
			
				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
				DrawBitmap16(nX, nY, pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap);
			}
			break;
		}
	}
}

// [REP3 03/09] ve tung sprite theo dung Represent2::DrawPrimitives (RU_T_IMAGE / RU_T_IMAGE_4):
//  - toa do: CoordinateTransform 2D, REF_SPOT, FRAME_DRAW y het;
//  - RU_T_IMAGE_4 (bClipRect): cat theo oImgLTPos/oImgRBPos nhu SetClipRect cua Represent2;
//  - kieu ve giao cho SetSpriteBlend (ALPHA/3LEVEL/NOT_BE_LIT = alpha; OPACITY = duc; BORDER = khong ve;
//    COLOR_ADJUST = nhan mau; spr moi (Reserved[1]) = SCREEN nhu DrawSpriteBlendColor(..., 1, TRUE)).
void KRepresentShell3::DrawImage2DFlat(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord, int bClipRect)
{
	int i;
	KRUImage* pTemp = (KRUImage*)pPrimitives;
	int nStep = bClipRect ? sizeof(KRUImage4) : sizeof(KRUImage);

	for (i = 0; i < nPrimitiveCount; i++, pTemp = (KRUImage*)((char*)pTemp + nStep))
	{
		switch(pTemp->nType)
		{
		case ISI_T_SPR:
			{
				if (pTemp->bRenderStyle == IMAGE_RENDER_STYLE_BORDER)	// Represent2 khong ve gi
					break;
				TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
					{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
					  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); break; }
#else
					{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); break; }
#endif

				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);

				if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
				{
					int nCenterX = pSprite->GetCenterX();
					int nCenterY = pSprite->GetCenterY();
					if (nCenterX || nCenterY)
					{
						nX -= nCenterX;
						nY -= nCenterY;
					}
					else if (pSprite->GetWidth() > 160)
					{
						nX -= 160;
						nY -= 192;
					}
				}
				if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
				{
					nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
					nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
				}
				int nW = pSprite->m_pFrameInfo[pTemp->nFrame].nWidth;
				int nH = pSprite->m_pFrameInfo[pTemp->nFrame].nHeight;
				if (bClipRect)
				{
					KRUImage4* p4 = (KRUImage4*)pTemp;
					RECT rc;
					rc.left  = nX;
					rc.top   = nY;
					nX -= p4->oImgLTPos.nX;
					nY -= p4->oImgLTPos.nY;
					rc.right = nX + p4->oImgRBPos.nX;
					rc.bottom= nY + p4->oImgRBPos.nY;
					if (rc.left < 0) rc.left = 0;
					if (rc.top < 0) rc.top = 0;
					if (rc.right > g_nScreenWidth) rc.right = g_nScreenWidth;
					if (rc.bottom > g_nScreenHeight) rc.bottom = g_nScreenHeight;
					DrawSpritePartAlpha(nX, nY, nW, nH, pTemp->nFrame, pSprite, rc, pTemp->Color.Color_dw, pTemp->bRenderStyle);
				}
				else
					DrawSpriteAlpha(nX, nY, nW, nH, pTemp->nFrame, pSprite, pTemp->Color.Color_dw, pTemp->bRenderStyle);
			}
			break;
		case ISI_T_BITMAP16:
			{
				TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				{	// [ANHNEN 10/09 d]
					static int s_nGhiBm = 0;	// [ANHNEN 10/09 e]
					if (s_nGhiBm < 10 && !strstr(pTemp->szImage, "Login") && !strstr(pTemp->szImage, "login"))
					{
						s_nGhiBm++;
						DWORD dwCull = 0, dwBlend = 0, dwSrc = 0, dwDst = 0, dwAT = 0, dwARef = 0, dwAFunc = 0, dwCOp = 0, dwCA1 = 0, dwCA2 = 0;
						PD3DDEVICE->GetRenderState(D3DRS_CULLMODE, &dwCull);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwBlend);
						PD3DDEVICE->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
						PD3DDEVICE->GetRenderState(D3DRS_DESTBLEND, &dwDst);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAT);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHAREF, &dwARef);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHAFUNC, &dwAFunc);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOp);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG2, &dwCA2);
						Rep3Log("[ANHNEN] bitmap16 %s: %s | tai %d,%d | co %dx%d | tex %dx%d | ptex %p | cull %u tron %u (%u/%u) at %u ham %u ref %u | op %u a1 %u a2 %u",
							pTemp->szImage, pBitmap ? "CO" : "NULL", pTemp->oPosition.nX, pTemp->oPosition.nY,
							pBitmap ? pBitmap->m_nWidth : 0, pBitmap ? pBitmap->m_nHeight : 0,
							pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nWidth : 0, pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nHeight : 0,
							pBitmap ? (void*)pBitmap->m_FrameInfo.texInfo[0].pTexture : NULL,
							(unsigned)dwCull, (unsigned)dwBlend, (unsigned)dwSrc, (unsigned)dwDst, (unsigned)dwAT, (unsigned)dwAFunc, (unsigned)dwARef,
							(unsigned)dwCOp, (unsigned)dwCA1, (unsigned)dwCA2);
					}
				}
				if (!pBitmap)
					break;

				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
				DrawBitmap16(nX, nY, pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap);
			}
			break;
		}
	}
}

// [REP3 03/09] trang thai tron mau theo kieu ve - doi chieu tung nhanh cua Represent2::DrawPrimitives
//   Represent2 (32 bit):  ALPHA/3LEVEL/NOT_BE_LIT -> DrawSpriteAlpha(alpha = run*Color.a)
//                         OPACITY -> DrawSprite (duc, bo Color.a)
//                         BORDER  -> khong ve (DrawSpriteBorder da bi chu thich)
//                         COLOR_ADJUST -> Color.rgb==0: nhu ALPHA; khac 0: g_BlendColor32b mode 0 = nhan mau
//                         spr moi Reserved[1] -> g_DrawSpriteScreen32b: d' = d + a*s*(1-d) (mode 1 doi mau ~ nhan mau)
void KRepresentShell3::SetSpriteBlend(int nRenderStyle, DWORD color, bool bNew, DWORD& vtxColor)
{
	DWORD a   = color >> 24;
	DWORD rgb = color & 0x00ffffff;
	DWORD tint = 0x00ffffff;
	m_nBlendMode = 0;

	if (nRenderStyle == IMAGE_RENDER_STYLE_OPACITY)
	{
		vtxColor = 0xffffffff;
		PD3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHAREF, 0x01 );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		m_nBlendMode = 1;
		return;
	}
	if ((bNew || nRenderStyle == IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST) && rgb != 0)
		tint = rgb;
	vtxColor = (a << 24) | tint;
	if (bNew)
	{
		// screen: SRC = tex*diffuse*(texA*diffA) [2 tang], out = SRC*(1-dst) + dst
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT | D3DTA_ALPHAREPLICATE );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
		PD3DDEVICE->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR );
		PD3DDEVICE->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_nBlendMode = 2;
	}
}

void KRepresentShell3::ResetSpriteBlend()
{
	switch (m_nBlendMode)
	{
	case 1:
		PD3DDEVICE->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		PD3DDEVICE->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		break;
	case 2:
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		PD3DDEVICE->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		PD3DDEVICE->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		PD3DDEVICE->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		break;
	}
	m_nBlendMode = 0;
}

void KRepresentShell3::DrawImage2DStretch(int nPrimitiveCount, KRepresentUnit* pPrimitives)
{
	int i;
	KRUImageStretch* pTemp = (KRUImageStretch*)pPrimitives;

	for (i = 0; i < nPrimitiveCount; i++, pTemp++)
	{	
		// Ö»´¦ÀíISI_T_BITMAP16Àà×ÊÔ´
		// [A31 04/09] SPRITE cung co gian duoc: DrawSpriteAlpha noi suy tung manh cua khung anh
		// vao khung dich (xem ChaZhi trong ham do) - dung cai ta can de ve vat pham nhieu o
		// thu nho ve MOT O. Truoc day ham nay chan ngay o dong duoi nen sprite khong qua duoc.
		if (pTemp->nType == ISI_T_BITMAP16)	// [ANHNEN 10/09 g] anh nen canh co gian cho phu kin khung ve
		{
			TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
				pTemp->szImage, pTemp->uImage, pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
			if (!pBitmap)
				continue;
			int nW = pTemp->oEndPos.nX - pTemp->oPosition.nX;
			int nH = pTemp->oEndPos.nY - pTemp->oPosition.nY;
			if (nW <= 0) nW = pBitmap->GetWidth();
			if (nH <= 0) nH = pBitmap->GetHeight();
			DrawBitmap16(pTemp->oPosition.nX, pTemp->oPosition.nY, nW, nH, pBitmap, true);
			continue;
		}
		if (pTemp->nType == ISI_T_SPR)
		{
			TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
				pTemp->szImage,	pTemp->uImage,
				pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
			if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
				{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
				  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#else
				{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#endif
			int nW = pTemp->oEndPos.nX - pTemp->oPosition.nX;
			int nH = pTemp->oEndPos.nY - pTemp->oPosition.nY;
			if (nW <= 0 || nH <= 0)
				continue;
			// [A33 04/09] PHAI dung DrawSpritePartAlpha, KHONG phai DrawSpriteAlpha.
			// DrawSpriteAlpha noi suy ChaZhi(fX1, fX2, 0, nWidth, nFrameX): mien nguon (0, nWidth)
			// va mien dich (fX1, fX1 + nWidth) nen ti le dung bang 1 - tham so nWidth BI TRIET TIEU,
			// tuc ham do KHONG BAO GIO co gian. DrawSpritePartAlpha lay mien nguon la kich thuoc
			// KHUNG ANH (m_pFrameInfo[nFrame].nWidth) nen moi co gian that.
			// rc de rong ca man hinh vi day khong phai viec cat vien.
			RECT rcFull;
			rcFull.left = 0;
			rcFull.top = 0;
			rcFull.right = g_nScreenWidth;
			rcFull.bottom = g_nScreenHeight;
#ifdef JX_MOBILE
			if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_CAT_KHUNG)
			{	// [KYNANG 12/09 TRON] ben goi cho khung cat rieng (KRUImagePart): dung de ve anh vuong thanh hinh tron
				KRUImagePart* pCat = (KRUImagePart*)pTemp;
				rcFull.left   = pCat->oImgLTPos.nX;
				rcFull.top    = pCat->oImgLTPos.nY;
				rcFull.right  = pCat->oImgRBPos.nX;
				rcFull.bottom = pCat->oImgRBPos.nY;
			}
#endif
			DrawSpritePartAlpha(pTemp->oPosition.nX, pTemp->oPosition.nY, nW, nH,
				pTemp->nFrame, pSprite, rcFull, pTemp->Color.Color_dw, pTemp->bRenderStyle);
			continue;
		}
		if(pTemp->nType != ISI_T_BITMAP16)
			continue;

		TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
			pTemp->szImage,	pTemp->uImage,
			pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
		if (!pBitmap)
			break;
	
		DrawBitmap16(pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->oEndPos.nX - pTemp->oPosition.nX, 
					pTemp->oEndPos.nY - pTemp->oPosition.nY, pBitmap, true);
	}
}

void KRepresentShell3::DrawPlayer2D(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord)
{
	RECT rcBound;
	RECTFLOAT rcBound1;

	// Get the outer rectangle of all parts of the player
	GetBoundBox2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord, rcBound);

	// If the pixel is outside the screen range, it will not be rendered
	if(rcBound.right < 0 || rcBound.left > g_nScreenWidth || rcBound.bottom < 0 || rcBound.top > g_nScreenHeight)
		return;

	rcBound1.left = (float)(rcBound.left);
	rcBound1.top = (float)(rcBound.top);
	rcBound1.right = (float)(rcBound.right);
	rcBound1.bottom = (float)(rcBound.bottom);

	// Draw the main character class
	if(rcBound.right - rcBound.left <= SPR_PRERENDER_TEXSIZE1 
		&& rcBound.bottom - rcBound.top <= SPR_PRERENDER_TEXSIZE1)
	{
		DrawSprOnTexture2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord, rcBound, rcBound1, SPR_PRERENDER_TEXSIZE1, false);
		g_ntest++;
	}
	else if(rcBound.right - rcBound.left <= SPR_PRERENDER_TEXSIZE2 
		&& rcBound.bottom - rcBound.top <= SPR_PRERENDER_TEXSIZE2)
	{
		DrawSprOnTexture2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord, rcBound, rcBound1, SPR_PRERENDER_TEXSIZE2, false);
		g_ntest++;
	}
}

void KRepresentShell3::GetBoundBox2D(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord, RECT &rcBound)
{
	int i;
	KRUImage* pTemp = (KRUImage*)pPrimitives;

	bool bFirstOne = true;
	// Compute the bounding rectangle of all pixels
	for (i = 0; i < nPrimitiveCount; i++, pTemp++)
	{
		TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
			pTemp->szImage,	pTemp->uImage,
			pTemp->nISPosition, pTemp->nFrame, pTemp->nType, false);
		if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
			{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
			  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#else
			{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#endif

		int nX = pTemp->oPosition.nX;
		int nY = pTemp->oPosition.nY;
		if (!bSinglePlaneCoord)
			CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
		
		if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
		{
#define CENTERX		160
#define	CENTERY		192
			int nCenterX = pSprite->GetCenterX();
			int nCenterY = pSprite->GetCenterY();
			if (nCenterX || nCenterY)
			{
				nX -= nCenterX;
				nY -= nCenterY;
			}
			else if (pSprite->GetWidth() > CENTERX)
			{
				nX -= CENTERX;
				nY -= CENTERY;
			}
		}
		
		if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
		{
			nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
			nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
		}

		if(bFirstOne)
		{
			bFirstOne = false;
			rcBound.left = nX, rcBound.top = nY;
			rcBound.right = nX + pSprite->m_pFrameInfo[pTemp->nFrame].nWidth;
			rcBound.bottom = nY + pSprite->m_pFrameInfo[pTemp->nFrame].nHeight;
		}
		else
		{
			if(nX < rcBound.left)
				rcBound.left = nX;
			if(nY < rcBound.top)
				rcBound.top = nY;
			if(nX + pSprite->m_pFrameInfo[pTemp->nFrame].nWidth > rcBound.right)
				rcBound.right = nX + (pSprite->m_pFrameInfo[pTemp->nFrame].nWidth);
			if(nY + pSprite->m_pFrameInfo[pTemp->nFrame].nHeight > rcBound.bottom)
				rcBound.bottom = nY + (pSprite->m_pFrameInfo[pTemp->nFrame].nHeight);
		}
	}
}

void KRepresentShell3::DrawSprOnTexture2D(int nPrimitiveCount, KRepresentUnit* pPrimitives,
	int bSinglePlaneCoord, RECT& rcBound, RECTFLOAT& rcRenderBound, int nTexSize, bool bLighting)
{
	int i;
	RECT rect;
	unsigned char cRenderStyle;
	KRUImage* pTemp = (KRUImage*)pPrimitives;
	uint32 nWidth = rcBound.right - rcBound.left;
	uint32 nHeight = rcBound.bottom - rcBound.top;
	unsigned int alpha = (((DWORD)pTemp->Color.Color_b.a) << 24);
	unsigned int color = 0x00404040 | alpha;
	//	unsigned int color = 0xff404040;
	bool bLightGet = false;
	bool sprNew = false;
	if (nWidth == 0 || nHeight == 0)
		return;

	D3DLOCKED_RECT LockedRect;
	rect.left = rect.top = 0;
	rect.right = nWidth;
	rect.bottom = nHeight;

	// ¸ù¾Ý»æÖÆÇøÓò²»Í¬´óÐ¡Ëø¶¨²»Í¬µÄÌùÍ¼
	if (nTexSize == SPR_PRERENDER_TEXSIZE1)
	{
		if (FAILED(m_pPreRenderTexture128->LockRect(0, &LockedRect, &rect, 0)))
			return;
	}
	else if (nTexSize == SPR_PRERENDER_TEXSIZE2)
	{
		if (FAILED(m_pPreRenderTexture256->LockRect(0, &LockedRect, &rect, 0)))
			return;
	}
	else {
		if (FAILED(m_pPreRenderTexture512->LockRect(0, &LockedRect, &rect, 0)))
			return;
	}

	// Çå¿ÕÌùÍ¼
	BYTE* p = (BYTE*)LockedRect.pBits;
	int nLen = (rect.right - rect.left) * 2;
	for (i = 0; i < rect.bottom - rect.top; i++)
	{
		memset(p, 0, nLen);
		p += LockedRect.Pitch;
	}

	for (i = 0; i < nPrimitiveCount; i++, pTemp++)
	{
		TextureResSpr* pSprite = (TextureResSpr*)m_TextureResMgr.GetImage(
			pTemp->szImage, pTemp->uImage,
			pTemp->nISPosition, pTemp->nFrame, pTemp->nType, false);
		if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
			{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
			  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#else
			{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#endif
		if (pSprite->m_bNew) {
			sprNew = true;
			PD3DDEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

			// Destination blend: D3DBLEND_ONE (add the destination color as is)
			PD3DDEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			// Blend operation: D3DBLENDOP_ADD (add the source and destination colors)
			PD3DDEVICE->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		}
		if (!bLightGet)
		{
			bLightGet = true;
			if (bLighting && m_bDoLighting)
			{
				D3DXVECTOR3 v;
				v.x = (float)(pTemp->oPosition.nX);
				v.y = (float)(pTemp->oPosition.nY);
				v.z = (float)(pTemp->oPosition.nZ);
				color = (GetPoint3dLighting(v) & 0x00ffffff) | alpha;
			}

			if (pTemp->bRenderStyle == IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST)
			{
				// ´¦ÀíÆ«É«
				color = ScaleColor(color, pTemp->Color.Color_b.r, pTemp->Color.Color_b.g, pTemp->Color.Color_b.b);
			}
		}

		cRenderStyle = pTemp->bRenderStyle;

		int nX = pTemp->oPosition.nX;
		int nY = pTemp->oPosition.nY;
		if (!bSinglePlaneCoord)
			CoordinateTransform(nX, nY, pTemp->oPosition.nZ);

		if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
		{
#define CENTERX		160
#define	CENTERY		192
			int nCenterX = pSprite->GetCenterX();
			int nCenterY = pSprite->GetCenterY();
			if (nCenterX || nCenterY)
			{
				nX -= nCenterX;
				nY -= nCenterY;
			}
			else if (pSprite->GetWidth() > CENTERX)
			{
				nX -= CENTERX;
				nY -= CENTERY;
			}
		}

		if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
		{
			nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
			nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
		}

		rect.left = nX - rcBound.left, rect.top = nY - rcBound.top;
		rect.right = rect.left + pSprite->m_pFrameInfo[pTemp->nFrame].nWidth;
		rect.bottom = rect.top + pSprite->m_pFrameInfo[pTemp->nFrame].nHeight;

		//assert(pSprite->m_pFrameInfo[pTemp->nFrame].pRawData);
		if (pSprite->m_pFrameInfo[pTemp->nFrame].pRawData)
		{
			// ½«sprÔ­Ê¼Êý¾ÝÖ±½Ó×ª»»µ½A4R4G4B4µÄÌùÍ¼ÉÏ
			RenderToA4R4G4B4((WORD*)LockedRect.pBits, LockedRect.Pitch,
				pSprite->m_pFrameInfo[pTemp->nFrame].pRawData, rect,
				pSprite->m_pFrameInfo[pTemp->nFrame].nWidth,
				pSprite->m_pFrameInfo[pTemp->nFrame].nHeight,
				pSprite->m_pPal16);
		}
	}

	if (nTexSize == SPR_PRERENDER_TEXSIZE1)
		m_pPreRenderTexture128->UnlockRect(0);
	else if (nTexSize == SPR_PRERENDER_TEXSIZE2)
		m_pPreRenderTexture256->UnlockRect(0);
	else
		m_pPreRenderTexture512->UnlockRect(0);

	if (FAILED(PD3DDEVICE->SetStreamSource(0, m_pVB2D, 0, sizeof(VERTEX2D))))
		return;
	PD3DDEVICE->SetFVF(D3DFVF_VERTEX2D);

	VERTEX2D* pvb;
	m_pVB2D->Lock(0, 4 * sizeof(VERTEX2D), (void**)&pvb, 0);

	float fX1, fY1, fX2, fY2;
	fX1 = rcRenderBound.left;
	fY1 = rcRenderBound.top;
	fX2 = rcRenderBound.right;
	fY2 = rcRenderBound.bottom;

	float fU2, fV2;
	if (nTexSize == SPR_PRERENDER_TEXSIZE1)
	{
		fU2 = (float)nWidth / (float)SPR_PRERENDER_TEXSIZE1;
		fV2 = (float)nHeight / (float)SPR_PRERENDER_TEXSIZE1;

		PD3DDEVICE->SetTexture(0, m_pPreRenderTexture128);
	}
	else if (nTexSize == SPR_PRERENDER_TEXSIZE2)
	{
		fU2 = (float)nWidth / (float)SPR_PRERENDER_TEXSIZE2;
		fV2 = (float)nHeight / (float)SPR_PRERENDER_TEXSIZE2;

		PD3DDEVICE->SetTexture(0, m_pPreRenderTexture256);
	}
	else {
		fU2 = (float)nWidth / (float)SPR_PRERENDER_TEXSIZE3;
		fV2 = (float)nHeight / (float)SPR_PRERENDER_TEXSIZE3;

		PD3DDEVICE->SetTexture(0, m_pPreRenderTexture512);
	}

	float ft1, ft2;
	ft1 = 0.5f / (float)SPR_PRERENDER_TEXSIZE2;
	ft2 = 0.5f / (float)SPR_PRERENDER_TEXSIZE2;

	pvb[0].position = D3DXVECTOR4(fX1, fY1, 100, 1);
	pvb[0].color = color;
	pvb[0].tu = 0.0f + ft1;
	pvb[0].tv = 0.0f + ft2;

	pvb[1].position = D3DXVECTOR4(fX2, fY1, 100, 1);
	pvb[1].color = color;
	pvb[1].tu = fU2 - ft1;
	pvb[1].tv = 0.0f + ft2;

	pvb[2].position = D3DXVECTOR4(fX1, fY2, 100, 1);
	pvb[2].color = color;
	pvb[2].tu = 0.0f + ft1;
	pvb[2].tv = fV2 - ft2;

	pvb[3].position = D3DXVECTOR4(fX2, fY2, 100, 1);
	pvb[3].color = color;
	pvb[3].tu = fU2 - ft1;
	pvb[3].tv = fV2 - ft2;

	m_pVB2D->Unlock();

	if (cRenderStyle == IMAGE_RENDER_STYLE_BORDER)
		PD3DDEVICE->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
	else
		PD3DDEVICE->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE4X);

	PD3DDEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	PD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	PD3DDEVICE->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	PD3DDEVICE->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	PD3DDEVICE->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	PD3DDEVICE->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	PD3DDEVICE->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	if (sprNew) {
		PD3DDEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		PD3DDEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		PD3DDEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
}

// ÎªºÍ2DÄ£Ê½´óÐ¡Ò»ÖÂ£¬ÌØ¶¨µÄsprËõ·ÅÒ»¸ö±ÈÀý
#define SCALE_RATE_SPRITE_WIDTH	 1.05f
#define SCALE_RATE_SPRITE_HEIGHT 1.12f

void KRepresentShell3::DrawImage3D(unsigned int uGenre, int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord)
{
	if(uGenre != RU_T_IMAGE && uGenre != RU_T_IMAGE_4)
		return;

	// ÅÐ¶ÏÊÇ·ñÖ÷½ÇÀà
	if(nPrimitiveCount >= 4)
	{
		DrawPlayer3D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord);
		return;
	}

	int i;
	RenderParam3D renderParam;
	KRUImage* pTemp = (KRUImage*)pPrimitives;

	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB3D, 0, sizeof(VERTEX3D) )))
		return;

	PD3DDEVICE->SetFVF( D3DFVF_VERTEX3D );
	// ÓÉÓÚÊÇÍ¸ÊÓÄ£Ê½£¬½«¹ýÂË·½Ê½ÉèÎªD3DTEXF_LINEAR·ÀÖ¹ÌùÍ¼¶¶¶¯
	if(g_renderModel == RenderModel3DPerspective)
	{
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	}

	for (i = 0; i < nPrimitiveCount; i++, pTemp++)
	{
		float fX1, fY1, fZ1, fX2, fY2, fZ2, fX3, fY3, fZ3, fX4, fY4, fZ4;
		fX1 = (float)pTemp->oPosition.nX;
		fY1 = (float)pTemp->oPosition.nY;
		fZ1 = (float)pTemp->oPosition.nZ;
		fX3 = (float)pTemp->oEndPos.nX;
		fY3 = (float)pTemp->oEndPos.nY;
		fZ3 = (float)pTemp->oEndPos.nZ;

		switch(pTemp->nType)
		{
		case ISI_T_SPR:
			{
				TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);

				if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
					{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
					  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); break; }
#else
					{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); break; }
#endif

				if(!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT) && (pTemp->oEndPos.nX == 0 || pTemp->oEndPos.nY == 0))
					break;

				if (pSprite->m_bNew) {
					PD3DDEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

					// Destination blend: D3DBLEND_ONE (add the destination color as is)
					PD3DDEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

					// Blend operation: D3DBLENDOP_ADD (add the source and destination colors)
					PD3DDEVICE->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				}
				if(uGenre != RU_T_IMAGE_4)
				{
					// ¾ØÐÎÍ¼ËØ
					if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
					{
#define CENTERX		160
#define	CENTERY		192
						int nCenterX = pSprite->GetCenterX();
						int nCenterY = pSprite->GetCenterY();
						if (nCenterX || nCenterY)
						{
							fX1 -= nCenterX * SCALE_RATE_SPRITE_WIDTH;
							fZ1 += nCenterY * SCALE_RATE_SPRITE_HEIGHT;
						}
						else if (pSprite->GetWidth() > CENTERX)
						{
							fX1 -= CENTERX * SCALE_RATE_SPRITE_WIDTH;
							fZ1 += CENTERY * SCALE_RATE_SPRITE_HEIGHT;
						}
						fX3 = fX1 + pSprite->m_pFrameInfo[pTemp->nFrame].nWidth * SCALE_RATE_SPRITE_WIDTH;
						fZ3 = fZ1 - pSprite->m_pFrameInfo[pTemp->nFrame].nHeight * SCALE_RATE_SPRITE_HEIGHT;
						fY3 = fY1;
					}
			
					float fXOff = 0;
					float fYOff = 0;
					if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
					{
						fXOff += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX * SCALE_RATE_SPRITE_WIDTH;
						fYOff += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY * SCALE_RATE_SPRITE_HEIGHT;
						if(fZ1 == fZ3)
						{
							fX1 += fXOff;
							fX3 += fXOff;
							fY1 += fYOff * 2;
							fY3 += fYOff * 2;
						}
						else
						{
							float fW,fH,fM;
							fW = (float)(fabs(fX3 - fX1));
							fH = (float)(fabs(fY3 - fY1));
							fM = (float)(sqrt(fW * fW + fH * fH));
							fW = fXOff * fW / fM;
							fH = fXOff * fH / fM;
							fX1 += fW;
							fX3 += fW;
							fY1 += fH * 2;
							fY3 += fH * 2;
							fZ1 -= fYOff;
							fZ3 -= fYOff;
						}
					}
					
					if(fZ1 == fZ3)
					{
						// Ë®Æ½Í¼ËØ
						renderParam.m_pos[0] = D3DXVECTOR3( fX1,fY1, fZ1 );
						renderParam.m_pos[1] = D3DXVECTOR3( fX3,fY1, fZ1 );
						renderParam.m_pos[2] = D3DXVECTOR3( fX3,fY3, fZ3 );
						renderParam.m_pos[3] = D3DXVECTOR3( fX1,fY3, fZ3 );
					}
					else
					{
						// ´¹Ö±Í¼ËØ
						renderParam.m_pos[0] = D3DXVECTOR3( fX1,fY1, fZ1 );
						renderParam.m_pos[1] = D3DXVECTOR3( fX3,fY3, fZ1 );
						renderParam.m_pos[2] = D3DXVECTOR3( fX3,fY3, fZ3 );
						renderParam.m_pos[3] = D3DXVECTOR3( fX1,fY1, fZ3 );
					}

					if(m_bDoLighting && pTemp->bRenderStyle != IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT)
						DrawSpriteAlpha3DLighting(renderParam, pTemp->nFrame, pSprite,
													pTemp->Color.Color_dw, pTemp->bRenderStyle, NULL);
					else
						DrawSpriteAlpha3D(renderParam, pTemp->nFrame, pSprite, pTemp->Color.Color_dw, 
													pTemp->bRenderStyle, NULL);
				}
				else
				{
					// Æ½ÐÐËÄ±ßÐÎÍ¼ËØ
					KRUImage4 *pTemp4 = (KRUImage4*)pTemp;
					fX2 = (float)pTemp4->oSecondPos.nX;
					fY2 = (float)pTemp4->oSecondPos.nY;
					fZ2 = (float)pTemp4->oSecondPos.nZ;
					fX4 = (float)pTemp4->oThirdPos.nX;
					fY4 = (float)pTemp4->oThirdPos.nY;
					fZ4 = (float)pTemp4->oThirdPos.nZ;
					
					renderParam.m_pos[0] = D3DXVECTOR3( fX1,fY1, fZ1 );
					renderParam.m_pos[1] = D3DXVECTOR3( fX2,fY2, fZ2 );
					renderParam.m_pos[2] = D3DXVECTOR3( fX3,fY3, fZ3 );
					renderParam.m_pos[3] = D3DXVECTOR3( fX4,fY4, fZ4 );

					// Clipper
					RECT rc;
					rc.left  = pTemp4->oImgLTPos.nX;
					rc.top   = pTemp4->oImgLTPos.nY;
					rc.right = pTemp4->oImgRBPos.nX;
					rc.bottom= pTemp4->oImgRBPos.nY;

					if(m_bDoLighting && pTemp->bRenderStyle != IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT)
						DrawSpriteAlpha3DLighting(renderParam, pTemp->nFrame, pSprite, 
												pTemp->Color.Color_dw, pTemp->bRenderStyle, &rc);
					else
						DrawSpriteAlpha3D(renderParam, pTemp->nFrame, pSprite, pTemp->Color.Color_dw, pTemp->bRenderStyle, &rc);
				}
				if(pSprite->m_bNew){
					PD3DDEVICE->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					PD3DDEVICE->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
					PD3DDEVICE->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				}
			}

			break;
		case ISI_T_BITMAP16:
			{
				TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
					pTemp->szImage,	pTemp->uImage,
					pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
				{	// [ANHNEN 10/09 d]
					static int s_nGhiBm = 0;	// [ANHNEN 10/09 e]
					if (s_nGhiBm < 10 && !strstr(pTemp->szImage, "Login") && !strstr(pTemp->szImage, "login"))
					{
						s_nGhiBm++;
						DWORD dwCull = 0, dwBlend = 0, dwSrc = 0, dwDst = 0, dwAT = 0, dwARef = 0, dwAFunc = 0, dwCOp = 0, dwCA1 = 0, dwCA2 = 0;
						PD3DDEVICE->GetRenderState(D3DRS_CULLMODE, &dwCull);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwBlend);
						PD3DDEVICE->GetRenderState(D3DRS_SRCBLEND, &dwSrc);
						PD3DDEVICE->GetRenderState(D3DRS_DESTBLEND, &dwDst);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAT);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHAREF, &dwARef);
						PD3DDEVICE->GetRenderState(D3DRS_ALPHAFUNC, &dwAFunc);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOp);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
						PD3DDEVICE->GetTextureStageState(0, D3DTSS_COLORARG2, &dwCA2);
						Rep3Log("[ANHNEN] bitmap16 %s: %s | tai %d,%d | co %dx%d | tex %dx%d | ptex %p | cull %u tron %u (%u/%u) at %u ham %u ref %u | op %u a1 %u a2 %u",
							pTemp->szImage, pBitmap ? "CO" : "NULL", pTemp->oPosition.nX, pTemp->oPosition.nY,
							pBitmap ? pBitmap->m_nWidth : 0, pBitmap ? pBitmap->m_nHeight : 0,
							pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nWidth : 0, pBitmap ? pBitmap->m_FrameInfo.texInfo[0].nHeight : 0,
							pBitmap ? (void*)pBitmap->m_FrameInfo.texInfo[0].pTexture : NULL,
							(unsigned)dwCull, (unsigned)dwBlend, (unsigned)dwSrc, (unsigned)dwDst, (unsigned)dwAT, (unsigned)dwAFunc, (unsigned)dwARef,
							(unsigned)dwCOp, (unsigned)dwCA1, (unsigned)dwCA2);
					}
				}
				if (!pBitmap)
					break;
				if(fZ1 == fZ3)
				{
					// Ë®Æ½Í¼ËØ
					renderParam.m_pos[0] = D3DXVECTOR3( fX1,fY1, fZ1 );
					renderParam.m_pos[1] = D3DXVECTOR3( fX3,fY1, fZ1 );
					renderParam.m_pos[2] = D3DXVECTOR3( fX3,fY3, fZ3 );
					renderParam.m_pos[3] = D3DXVECTOR3( fX1,fY3, fZ3 );
				}
				else
				{
					// ´¹Ö±Í¼ËØ
					renderParam.m_pos[0] = D3DXVECTOR3( fX1,fY1, fZ1 );
					renderParam.m_pos[1] = D3DXVECTOR3( fX3,fY3, fZ1 );
					renderParam.m_pos[2] = D3DXVECTOR3( fX3,fY3, fZ3 );
					renderParam.m_pos[3] = D3DXVECTOR3( fX1,fY1, fZ3 );
				}
				if(m_bDoLighting)
					DrawBitmap163DLighting(renderParam, pBitmap);
				else
					DrawBitmap163D(renderParam, pBitmap);
			}
			break;
		}
	}
	// »Ö¸´È±Ê¡¹ýÂËÄ£Ê½
	if(g_renderModel == RenderModel3DPerspective)
	{
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	}
}

void KRepresentShell3::DrawPlayer3D(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord)
{
	RECTFLOAT rcBound3D;
	RECT rcBound2D;

	// Calculate the bounding rectangle for non-perspective mode and perspective mode
	GetBoundBox2D(nPrimitiveCount, pPrimitives, bSinglePlaneCoord, rcBound2D);
	GetBoundBox3D(nPrimitiveCount, pPrimitives, rcBound3D);

	// If the pixel is outside the screen range, it will not be rendered
	if(rcBound3D.right < 0 || rcBound3D.left > g_nScreenWidth || rcBound3D.bottom < 0 || rcBound3D.top > g_nScreenHeight)
		return;

	if(rcBound3D.left - rcBound3D.right > 260 || rcBound3D.right - rcBound3D.left > 560)
		return;

	// Draw the main character class
	if(rcBound2D.right - rcBound2D.left <= SPR_PRERENDER_TEXSIZE1 
		&& rcBound2D.bottom - rcBound2D.top <= SPR_PRERENDER_TEXSIZE1)
	{
		DrawSprOnTexture2D(nPrimitiveCount, pPrimitives, false, rcBound2D, rcBound3D, SPR_PRERENDER_TEXSIZE1, true);
		g_ntest++;
	}
	else if(rcBound2D.right - rcBound2D.left <= SPR_PRERENDER_TEXSIZE2 
		&& rcBound2D.bottom - rcBound2D.top <= SPR_PRERENDER_TEXSIZE2)
	{
		DrawSprOnTexture2D(nPrimitiveCount, pPrimitives, false, rcBound2D, rcBound3D, SPR_PRERENDER_TEXSIZE2, true);
		g_ntest++;
	}
	else if (rcBound2D.right - rcBound2D.left <= SPR_PRERENDER_TEXSIZE3
		&& rcBound2D.bottom - rcBound2D.top <= SPR_PRERENDER_TEXSIZE3)
	{
		DrawSprOnTexture2D(nPrimitiveCount, pPrimitives, false, rcBound2D, rcBound3D, SPR_PRERENDER_TEXSIZE3, true);
		g_ntest++;
	}
}

void KRepresentShell3::GetBoundBox3D(int nPrimitiveCount, KRepresentUnit* pPrimitives, RECTFLOAT &rcBound)
{
	int i;
	KRUImage* pTemp = (KRUImage*)pPrimitives;

	bool bFirstOne = true;
	// ËùÓÐÍ¼ËØµÄ¼ÆËãÍâ°ü¾ØÐÎ
	for (i = 0; i < nPrimitiveCount; i++, pTemp++)
	{
		D3DXVECTOR3 v1,v2;
		v1.x = (float)pTemp->oPosition.nX;
		v1.y = (float)pTemp->oPosition.nY;
		v1.z = (float)pTemp->oPosition.nZ;
		v2.x = (float)pTemp->oEndPos.nX;
		v2.y = (float)pTemp->oEndPos.nY;
		v2.z = (float)pTemp->oEndPos.nZ;

		TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
			pTemp->szImage,	pTemp->uImage,
			pTemp->nISPosition, pTemp->nFrame, pTemp->nType, false);
		if (!pSprite || pTemp->nFrame >= pSprite->m_nFrameNum)
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09]
			{ if (!pSprite) g_uJxRongAnh++; else { g_uJxRongKhung++; if ((unsigned)pSprite->m_nFrameNum > g_uJxRongKhungMax) g_uJxRongKhungMax = (unsigned)pSprite->m_nFrameNum; }
			  g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#else
			{ g_uRep3FxAnhNull++; Rep3AnhNullGhi(pTemp->szImage, pTemp->nFrame); continue; }
#endif

		if(!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT) && (pTemp->oEndPos.nX == 0 || pTemp->oEndPos.nY == 0))
			continue;

		// ¾ØÐÎÍ¼ËØ
		if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
		{
#define CENTERX		160
#define	CENTERY		192
			int nCenterX = pSprite->GetCenterX();
			int nCenterY = pSprite->GetCenterY();
			if (nCenterX || nCenterY)
			{
				v1.x -= nCenterX * SCALE_RATE_SPRITE_WIDTH;
				v1.z += nCenterY * SCALE_RATE_SPRITE_HEIGHT;
			}
			else if (pSprite->GetWidth() > CENTERX)
			{
				v1.x -= CENTERX * SCALE_RATE_SPRITE_WIDTH;
				v1.z += CENTERY * SCALE_RATE_SPRITE_HEIGHT;
			}
			v2.x = v1.x + pSprite->m_pFrameInfo[pTemp->nFrame].nWidth * SCALE_RATE_SPRITE_WIDTH;
			v2.z = v1.z - pSprite->m_pFrameInfo[pTemp->nFrame].nHeight * SCALE_RATE_SPRITE_HEIGHT;
			v2.y = v1.y;
		}
			
		float fXOff = 0;
		float fYOff = 0;
		if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
		{
			fXOff += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX * SCALE_RATE_SPRITE_WIDTH;
			fYOff += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY * SCALE_RATE_SPRITE_HEIGHT;

			float fW,fH,fM;
			fW = (float)(fabs(v2.x - v1.x));
			fH = (float)(fabs(v2.y - v1.y));
			fM = (float)(sqrt(fW * fW + fH * fH));
			fW = fXOff * fW / fM;
			fH = fXOff * fH / fM;
			v1.x += fW;
			v2.x += fW;
			v1.y += fH * 2;
			v2.y += fH * 2;
			v1.z -= fYOff;
			v2.z -= fYOff;
		}

		// ½«×ø±ê×ª»¯µ½ÆÁÄ»¿Õ¼ä
		D3DXVECTOR3 vPos1, vPos2;
		D3DVIEWPORT9 viewportData = g_Device.GetViewport();
		
		D3DXVec3Project(&vPos1, &v1, &viewportData, &m_matProj, &m_matView, NULL);
		D3DXVec3Project(&vPos2, &v2, &viewportData, &m_matProj, &m_matView, NULL);

		if(bFirstOne)
		{
			bFirstOne = false;
			rcBound.left = vPos1.x, rcBound.top = vPos1.y;
			rcBound.right = vPos2.x;
			rcBound.bottom = vPos2.y;
		}
		else
		{
			if(vPos1.x < rcBound.left)
				rcBound.left = vPos1.x;
			if(vPos1.y < rcBound.top)
				rcBound.top = vPos1.y;
			if(vPos2.x > rcBound.right)
				rcBound.right = vPos2.x;
			if(vPos2.y > rcBound.bottom)
				rcBound.bottom = vPos2.y;
		}
	}
}

void KRepresentShell3::DrawPrimitivesOnImage(int nPrimitiveCount, KRepresentUnit* pPrimitives, 
        unsigned int uGenre, const char* pszImage, unsigned int uImage, short &nImagePosition)
{
	Rep3NapDongBo napDongBo(m_TextureResMgr);	// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo
#ifdef JX_MOBILE
	// [NENDO 13/09] do chi tiet: GetImage / RIO / doi dich ve -> [PGND-V] trong jx_rep3.log
	LARGE_INTEGER jxNd0, jxNd1, jxNdT0, jxNdF; QueryPerformanceFrequency(&jxNdF); QueryPerformanceCounter(&jxNdT0); const double jxNdK = jxNdF.QuadPart ? 1000.0 / (double)jxNdF.QuadPart : 0.0;
	s_dJxNenGetMs = 0.0; s_dJxNenRioMs = 0.0; s_dJxNenRtMs = 0.0; s_uJxNenGetLan = 0; s_uJxNenBoLan = 0; s_uJxNenNapLan = 0;
	const unsigned uJxNdNap0 = (unsigned)m_TextureResMgr.m_nLoadCount;
	extern int g_nJxONenLog;	// [ONEN 15/09] ghi chi tiet tung o cho N lan ghep nen dau tien
	const bool bJxONen = (g_nJxONenLog > 0 && uGenre == RU_T_IMAGE && pszImage && pszImage[0]);
	if (bJxONen) { g_nJxONenLog--; Rep3Log("[ONEN] === bat dau ghep %s: %d anh ===", pszImage, nPrimitiveCount); }
#endif
	if(!pPrimitives)
	{
		assert(pPrimitives);
		return;
	}

	if(!pszImage || !pszImage[0])
		return;

	if(m_bDeviceLost)
		return;

#ifdef JX_MOBILE
	QueryPerformanceCounter(&jxNd0);	// [NENDO 13/09]
#endif
	TextureResBmp* pDestBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
		pszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);
	if (pDestBitmap == NULL)
		return;

	if(!pDestBitmap->m_FrameInfo.texInfo[0].pTexture)
	{
		assert(pDestBitmap->m_FrameInfo.texInfo[0].pTexture);
		return;
	}

	IDirect3DSurface9 *pDesSurface, *pOldSurface;
	if (FAILED(PD3DDEVICE->GetRenderTarget( 0, &pOldSurface )))
		return;

	if (FAILED(pDestBitmap->m_FrameInfo.texInfo[0].pTexture->GetSurfaceLevel( 0, &pDesSurface)))
	{
		pOldSurface->Release();
		return;
	}

	if (FAILED(PD3DDEVICE->SetRenderTarget( 0, pDesSurface)))
	{
		pDesSurface->Release();
		pOldSurface->Release();
		return;
	}

	PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) );
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

#ifdef JX_MOBILE
	QueryPerformanceCounter(&jxNd1); s_dJxNenRtMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK;	// [NENDO 13/09] doi dich ve
#endif
	int i;
	switch(uGenre)
	{
	case RU_T_IMAGE:
		KRUImage* pTemp = (KRUImage*)pPrimitives;
		for (i = 0; i < nPrimitiveCount; i++, pTemp++)
		{			
			switch(pTemp->nType)
			{
			case ISI_T_SPR:
				{
#ifdef JX_MOBILE
					QueryPerformanceCounter(&jxNd0);	// [NENDO 13/09]
#endif
					TextureResSpr* pSprite = (TextureResSpr *)m_TextureResMgr.GetImage(
						pTemp->szImage, pTemp->uImage,
						pTemp->nISPosition, pTemp->nFrame, pTemp->nType);
#ifdef JX_MOBILE
					QueryPerformanceCounter(&jxNd1); s_dJxNenGetMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK; s_uJxNenGetLan++;	// [NENDO 13/09]
					if (bJxONen)
					{	// [ONEN 15/09] ghi TRUOC khi bo, de biet DUNG o nao va vi sao
						if (pSprite == NULL)
							Rep3Log("[ONEN] o %d/%d tai %d,%d khung %d : %s -> BO GetImage NULL", i, nPrimitiveCount, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->nFrame, pTemp->szImage);
						else if (pTemp->nFrame >= pSprite->m_nFrameNum)
							Rep3Log("[ONEN] o %d/%d tai %d,%d khung %d : %s -> BO khung %d >= so khung %d", i, nPrimitiveCount, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->nFrame, pTemp->szImage, pTemp->nFrame, pSprite->m_nFrameNum);
					}
					if (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum) { s_uJxNenBoLan++; break; }
#else
					if (pSprite == NULL || pTemp->nFrame >= pSprite->m_nFrameNum)
						break;
#endif

					int nX = pTemp->oPosition.nX;
					int nY = pTemp->oPosition.nY;

					if (!(pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW))
					{
						nX += pSprite->m_pFrameInfo[pTemp->nFrame].nOffX;
						nY += pSprite->m_pFrameInfo[pTemp->nFrame].nOffY;
						if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
						{
							nX -= pSprite->GetCenterX();
							nY -= pSprite->GetCenterY();
						}
					}

					switch(pTemp->bRenderStyle)
					{
					case IMAGE_RENDER_STYLE_ALPHA:
					case IMAGE_RENDER_STYLE_3LEVEL:
					case IMAGE_RENDER_STYLE_OPACITY:
					case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
#ifdef JX_MOBILE
						QueryPerformanceCounter(&jxNd0);
						{	// [ONEN 15/09] g_uRep3FxTexNull tang = RIO thoat vi texture rong -> o do DEN ma khong bi dem vao "bo"
							const unsigned uJxTN0 = g_uRep3FxTexNull;
							RIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY);
							if (bJxONen)
								Rep3Log("[ONEN] o %d/%d tai %d,%d khung %d : %s -> %s", i, nPrimitiveCount, pTemp->oPosition.nX, pTemp->oPosition.nY, pTemp->nFrame, pTemp->szImage,
									(g_uRep3FxTexNull != uJxTN0) ? "BO texture rong" : "VE");
						}
						QueryPerformanceCounter(&jxNd1); s_dJxNenRioMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK;	// [NENDO 13/09]
#else
						RIO_CopySprToBufferAlpha(pSprite, pTemp->nFrame, pDestBitmap, nX, nY);
#endif
						break;
					}						
				}
				break;
			case ISI_T_BITMAP16:
				{
					TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
						pTemp->szImage, pTemp->uImage, pTemp->nISPosition,
							pTemp->nFrame, pTemp->nType);
						if (!pBitmap)
							break;
						RIO_CopyBitmap16ToBuffer(pBitmap, pDestBitmap, pTemp->oPosition.nX, pTemp->oPosition.nY);
				}
				break;
			}
		}
		break;
	}

#ifdef JX_MOBILE
	QueryPerformanceCounter(&jxNd0);	// [NENDO 13/09]
#endif
	PD3DDEVICE->SetRenderTarget( 0, pOldSurface );
	pDesSurface->Release();
	pOldSurface->Release();
#ifdef JX_MOBILE
	QueryPerformanceCounter(&jxNd1); s_dJxNenRtMs += (double)(jxNd1.QuadPart - jxNd0.QuadPart) * jxNdK; s_uJxNenNapLan = (unsigned)m_TextureResMgr.m_nLoadCount - uJxNdNap0;	// [NENDO 13/09]
	{	// [NENDO 13/09] ghi khi >= 4 ms: 've len anh' cua mot vung = GetImage (tim/nap) + RIO (ghi lenh ve) + doi dich ve
		const double dNdTong = (double)(jxNd1.QuadPart - jxNdT0.QuadPart) * jxNdK;
		if (dNdTong >= 4.0)
			Rep3Log("[PGND-V] %s: %d anh, %.1f ms = GetImage %.1f (%u lan, nap dong bo %u, bo %u) + RIO %.1f + doi dich %.1f", pszImage, nPrimitiveCount, dNdTong,
				s_dJxNenGetMs, s_uJxNenGetLan, s_uJxNenNapLan, s_uJxNenBoLan, s_dJxNenRioMs, s_dJxNenRtMs);
	}
#endif
}

//## Çå³ýÍ¼ÐÎÊý¾Ý
void KRepresentShell3::ClearImageData(const char* pszImage, unsigned int uImage, short nImagePosition)
{
	Rep3NapDongBo napDongBo(m_TextureResMgr);	// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo
	if(!pszImage || !pszImage[0])
		return;

	if(m_bDeviceLost)
		return;

#ifdef JX_MOBILE
	LARGE_INTEGER jxG0, jxG1; QueryPerformanceCounter(&jxG0);	// [NENDO 13/09 b] do GetImage anh dich
#endif
	TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
		pszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);
#ifdef JX_MOBILE
	QueryPerformanceCounter(&jxG1);
#endif
	if (pBitmap)
	{
#ifdef JX_MOBILE
		// [XOANEN 13/09] anh nen vung (_*PlaceGround*_): xoa TREN GPU (Clear vao dich ve) thay vi LockData + memset + UpdateTexture.
		// UpdateSurface khoa DICH -> CTexGpu::LockRect thay render target da ve (m_bGpuNewer) -> ReadbackTexture = SubmitFrame + doc GPU
		// ve CPU DONG BO ~17 ms (Fold 7 13/09: 1002/1156 lan [PGND] o nhanh XA la cai nay, 've len anh' chi 0,6 ms). Ban CPU cua anh
		// nen khong ai doc. ScenePlaceMapC / UiPlayVideo giu duong cu (ten khac).
		if (strncmp(pszImage, "_*PlaceGround*_", 15) == 0 && pBitmap->m_FrameInfo.texInfo[0].pTexture)
		{
			LARGE_INTEGER jxX0, jxX1, jxXF; QueryPerformanceFrequency(&jxXF); QueryPerformanceCounter(&jxX0);
			IDirect3DSurface9 *pXDes = NULL, *pXOld = NULL;
			bool bXong = false;
			if (SUCCEEDED(PD3DDEVICE->GetRenderTarget(0, &pXOld)) && SUCCEEDED(pBitmap->m_FrameInfo.texInfo[0].pTexture->GetSurfaceLevel(0, &pXDes)))
			{
				{ extern void Rep3Gpu_DichSeXoa(IDirect3DDevice9*, int); Rep3Gpu_DichSeXoa(PD3DDEVICE, 1); }	// [XOANEN 13/09 b] khong doc nguoc GPU khi chuan bi dich
				const HRESULT hrXDes = PD3DDEVICE->SetRenderTarget(0, pXDes);
				{ extern void Rep3Gpu_DichSeXoa(IDirect3DDevice9*, int); Rep3Gpu_DichSeXoa(PD3DDEVICE, 0); }
				if (SUCCEEDED(hrXDes))
				{
					PD3DDEVICE->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0L);
					PD3DDEVICE->SetRenderTarget(0, pXOld);
					bXong = true;
				}
			}
			if (pXDes) pXDes->Release();
			if (pXOld) pXOld->Release();
			QueryPerformanceCounter(&jxX1);
			const double dXMs = jxXF.QuadPart ? (double)(jxX1.QuadPart - jxX0.QuadPart) * 1000.0 / (double)jxXF.QuadPart : 0.0;
			const double dGMs = jxXF.QuadPart ? (double)(jxG1.QuadPart - jxG0.QuadPart) * 1000.0 / (double)jxXF.QuadPart : 0.0;	// [NENDO 13/09 b]
			if (dXMs + dGMs >= 3.0) Rep3Log("[PGND-X] xoa nen %s tren GPU: %.1f ms (%s) | GetImage dich %.1f ms", pszImage, dXMs, bXong ? "xong" : "HONG -> memset", dGMs);
			if (bXong)
				return;
		}
		LARGE_INTEGER jxC0, jxC1, jxCF; QueryPerformanceFrequency(&jxCF); QueryPerformanceCounter(&jxC0);	// [XOANEN 13/09] do duong cu (LockData/UpdateTexture)
#endif
		byte* pBuffer;
		int32 nPitch;
		if( pBitmap->LockData((void**)(&pBuffer), nPitch) )
		{
			for(int i=0; i<pBitmap->GetHeight(); i++)
			{
				memset(pBuffer, 0, nPitch);
				pBuffer += nPitch;
			}
			pBitmap->UnLockData();
		}
#ifdef JX_MOBILE
		QueryPerformanceCounter(&jxC1);
		{ const double dCMs = jxCF.QuadPart ? (double)(jxC1.QuadPart - jxC0.QuadPart) * 1000.0 / (double)jxCF.QuadPart : 0.0; if (dCMs >= 4.0) Rep3Log("[PGND-X] xoa anh %s bang CPU (LockData/UpdateTexture): %.1f ms", pszImage, dCMs); }	// [XOANEN 13/09]
#endif
	}
}


void KRepresentShell3::FreeAllImage()
{
	m_TextureResMgr.Free();
}

void KRepresentShell3::FreeImage(const char* pszImage)
{
	if(!pszImage || !pszImage[0])
		return;

	m_TextureResMgr.FreeImage(pszImage);
}

void* KRepresentShell3::GetBitmapDataBuffer(const char* pszImage, KBitmapDataBuffInfo* pInfo, int nType)
{
	Rep3NapDongBo napDongBo(m_TextureResMgr);	// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo
	if(!pszImage || !pszImage[0])
		return NULL;

	if(m_bDeviceLost)
		return NULL;

	short nImagePosition;
	unsigned int uImage = 0;
	TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
							pszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);
	if (!pBitmap)
		return NULL;

	void *pData;
	int32 nPitch;
	if(!pBitmap->LockData(&pData, nPitch))
		return NULL;

	if(pInfo)
	{
		pInfo->nWidth = pBitmap->GetWidth();
		pInfo->nHeight = pBitmap->GetHeight();
		pInfo->nPitch = nPitch;
		pInfo->pData = pData;
		if(g_16BitFormat == D3DFMT_R5G6B5)
			pInfo->eFormat = BDBF_16BIT_565;
		else
			pInfo->eFormat = BDBF_16BIT_555;
	}

	return pData;
}

void KRepresentShell3::ReleaseBitmapDataBuffer(const char* pszImage, void* pBuffer)
{
	Rep3NapDongBo napDongBo(m_TextureResMgr);	// [NAP 08/09 d] ghep/ghi anh mot lan -> nap dong bo
	if(!pszImage || !pszImage[0])
		return;

	if(m_bDeviceLost)
		return;

	short nImagePosition;
	unsigned int uImage = 0;
	TextureResBmp* pBitmap = (TextureResBmp *)m_TextureResMgr.GetImage(
							pszImage, uImage, nImagePosition, 0, ISI_T_BITMAP16);
	if (!pBitmap)
		return;

	pBitmap->UnLockData();
}

bool KRepresentShell3::GetImageParam(const char* pszImage, KImageParam* pImageData, int nType)
{
	if(!pImageData || !pszImage || !pszImage[0])
		return false;

	return m_TextureResMgr.GetImageParam(pszImage, pImageData, nType);
}

bool KRepresentShell3::GetImageFrameParam( const char* pszImage, int nFrame,
										  KRPosition2* pOffset, KRPosition2* pSize, int nType)
{
	if(!pOffset || !pszImage || !pszImage[0])
		return false;

	return m_TextureResMgr.GetImageFrameParam(pszImage, nFrame, pOffset, pSize, nType);
}

int KRepresentShell3::GetImagePixelAlpha(const char* pszImage, int nFrame, int nX, int nY, int nType)
{
	if(!pszImage || !pszImage[0])
		return 0;

	if(m_bDeviceLost)
		return 0;

	return m_TextureResMgr.GetImagePixelAlpha(pszImage, nFrame, nX, nY, nType);
}

void KRepresentShell3::LookAt(int nX, int nY, int nZ)
{
	if(m_bDeviceLost)
		return;

	m_nLeft = nX - g_nScreenWidth / 2;
	m_nTop  = nY / 2 - ((nZ * 887) >> 10) - g_nScreenHeight / 2;

	if(g_renderModel == RenderModel3DOrtho || g_renderModel == RenderModel3DPerspective)
	{
		float fX = (float)nX;
		float fY = (float)nY;
		float fZ = (float)nZ;
		// ÉãÏñ»úºóÍË£¬Ì§¸ß³É30¶È½Ç
		m_vCamera1.x = fX;
		m_vCamera1.y = fY;
		m_vCamera1.z = fZ;
		m_vCamera.x = fX;
		m_vCamera.y = fY + 4000.0f* g_fZoomFactor;
		m_vCamera.z = (float)(fZ + 4000.0f*0.58* g_fZoomFactor);
		D3DXMatrixLookAtLH( &m_matView, &m_vCamera,
							&D3DXVECTOR3( fX, fY, fZ ), &D3DXVECTOR3( 0, 0, 1 ) );
		PD3DDEVICE->SetTransform( D3DTS_VIEW,  &m_matView );
		D3DXMatrixInverse( &m_matViewInverse, NULL, &m_matView );
	}
}

// [CHUGIU 09/09] Giu vi tri MAN HINH cua chu toi thieu Rep3ChuGiuMs ms (xem ReverseTools/goi_va_chugiu_0909.py).
// Chu A/B: giu 14 ms thi het am ma khong toi; giu ca the gioi thi giat => chi giu CHU. Theo thoi gian nen
// 60 Hz khong giu, 143 Hz giu 1 khung, 240 Hz giu 2 khung. Nhan dien dong chu = bam chuoi+font, va vi tri
// man hinh moi cach vi tri dang giu <= 24 px.
struct KRep3ChuGiu { unsigned uBam; int nX, nY; double dLuc; };
static KRep3ChuGiu s_ChuGiu[2048];	// [CHUGIU 09/09 b] bang bam mo, do tuyen tinh toi da 16 o (truoc: vong 512 o duyet tuyen tinh moi dong chu)
static int         s_nChuGiuKe = 0;
static double      s_dChuGiuF = 0.0;
static inline unsigned Rep3BamChu(const char* p, int n, int nFont)
{
	unsigned h = 2166136261u ^ (unsigned)nFont;
	if (n < 0) { for (; *p; p++) { h ^= (unsigned char)*p; h *= 16777619u; } }
	else       { for (int i = 0; i < n && p[i]; i++) { h ^= (unsigned char)p[i]; h *= 16777619u; } }
	return h;
}
// tra ve true neu (nX, nY) da duoc thay bang vi tri dang giu
static bool Rep3ChuGiu(const char* psText, int nCount, int nFont, int& nX, int& nY)
{
	if (g_nRep3ChuGiuMs <= 0) return false;
	if (s_dChuGiuF == 0.0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); s_dChuGiuF = (double)f.QuadPart / 1000.0; }
	// [CHUGIU 09/09 b] gio dau khung (RepresentBegin) - cung mot gia tri cho moi dong chu trong khung, khong QPC moi dong
	const double dNow = (double)g_liRep3VeBegin.QuadPart / s_dChuGiuF;
	const unsigned uBam = Rep3BamChu(psText, nCount, nFont);
	int nTrong = -1; int nCu = -1; double dCuNhat = 0.0;
	for (int k = 0; k < 16; k++)
	{
		const int i = (int)((uBam + (unsigned)k) & 2047);
		KRep3ChuGiu& e = s_ChuGiu[i];
		if (e.dLuc == 0.0) { if (nTrong < 0) nTrong = i; break; }	// o trong: chuoi do ket thuc
		if (nCu < 0 || e.dLuc < dCuNhat) { nCu = i; dCuNhat = e.dLuc; }
		if (e.uBam != uBam) continue;
		int dx = nX - e.nX; if (dx < 0) dx = -dx;
		int dy = nY - e.nY; if (dy < 0) dy = -dy;
		if (dx > 24 || dy > 24) continue;
		if (dNow - e.dLuc < (double)g_nRep3ChuGiuMs) { nX = e.nX; nY = e.nY; g_uRep3ChuGiu++; return true; }
		e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++; return false;
	}
	KRep3ChuGiu& e = s_ChuGiu[nTrong >= 0 ? nTrong : (nCu >= 0 ? nCu : (int)(uBam & 2047))];	// o trong, khong thi o cu nhat trong chuoi do
	e.uBam = uBam; e.nX = nX; e.nY = nY; e.dLuc = dNow; g_uRep3ChuVe++;
	return false;
}

void KRepresentShell3::OutputText(int nFontId, const char* psText, int nCount, int nX, 
								  int nY, unsigned int Color, int nLineWidth, int nZ, unsigned int BorderColor)
{
	int i;
	if(!psText || !psText[0])
		return;

	if(m_bDeviceLost)
		return;
#ifdef JX_MOBILE
	if (TgChuXep()) { TgChuThem(0, nFontId, psText, nCount, nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL, NULL); return; }	// [CHUNET 14/09] chu the gioi: ve sau blit
#endif
	
	Color |= 0xff000000;

	for (i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
#ifdef JX_POSIX	// [ANDROID 08/09] chan doan: chu bi bo vi thieu font
	if (i == RS2_MAX_FONT_ITEM_NUM || !m_FontTable[i].pFontObj)
	{
		static int s_nBoChu = 0;
		if (s_nBoChu < 20)
		{
			s_nBoChu++;
			Rep3Log("[FONT] BO chu (font id=%d chua nap): \"%.40s\"", nFontId, psText);
		}
	}
#endif
	if (i == RS2_MAX_FONT_ITEM_NUM)
		return;

	if (!m_FontTable[i].pFontObj)
		return;

	if(nZ != TEXT_IN_SINGLE_PLANE_COORD)
	{
		// ½«3D×ø±ê×ª»¯ÎªÆÁÄ»×ø±ê
		if(m_dwWindowStyle == RenderModel3DPerspective)
		{
			D3DXVECTOR3 vPos((float)(nX), (float)(nY), (float)(nZ));
			D3DVIEWPORT9 viewportData = g_Device.GetViewport();
			D3DXVec3Project(&vPos, &vPos, &viewportData, &m_matProj, &m_matView, NULL);
			nX = (int)vPos.x, nY = (int)vPos.y;
		}
		else {
			CoordinateTransform(nX, nY, nZ);
		}
	}
	
	if (nZ != TEXT_IN_SINGLE_PLANE_COORD)
		Rep3ChuGiu(psText, nCount, nFontId, nX, nY);	// [CHUGIU 09/09] chi chu neo vao the gioi
	m_FontTable[i].pFontObj->SetBorderColor(BorderColor);
	m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
	m_FontTable[i].pFontObj->OutputText(psText, nCount, nX, nY, Color, nLineWidth);
}

void KRepresentShell3::OutputVNText(int nFontId, char* psText, int nCount, int nX, 
								  int nY, unsigned int Color, int nLineWidth, int nZ, unsigned int BorderColor)
{
	if(!psText || !psText[0])
		return;

	if(m_bDeviceLost)
		return;
	
	int i = 0;
#ifdef JX_MOBILE
	if (TgChuXep()) { TgChuThem(1, nFontId, psText, nCount, nX, nY, nZ, Color, nLineWidth, BorderColor, NULL, NULL, NULL); return; }	// [CHUNET 14/09] chu VN the gioi: ve sau blit
#endif

	Color |= 0xff000000;

	for (i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i == RS2_MAX_FONT_ITEM_NUM)
		return;

	if (!m_FontTable[i].pFontObj)
		return;

	if(nZ != TEXT_IN_SINGLE_PLANE_COORD)
	{
		// ½«3D×ø±ê×ª»¯ÎªÆÁÄ»×ø±ê
		if(m_dwWindowStyle == RenderModel3DPerspective)
		{
			D3DXVECTOR3 vPos((float)(nX), (float)(nY), (float)(nZ));
			D3DVIEWPORT9 viewportData = g_Device.GetViewport();
			D3DXVec3Project(&vPos, &vPos, &viewportData, &m_matProj, &m_matView, NULL);
			nX = (int)vPos.x, nY = (int)vPos.y;
		}
		else {
			CoordinateTransform(nX, nY, nZ);
		}
	}
	
	m_FontTable[i].pFontObj->SetBorderColor(BorderColor);
	m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
	m_FontTable[i].pFontObj->OutputText(psText, nCount, nX, nY, Color, nLineWidth);
}

//## Êä³öÎÄ×Ö¡£
int KRepresentShell3::OutputRichText(int nFontId, KOutputTextParam* pParam, 
		const char* psText, int nCount, int nLineWidth)
{
	int i;
	if(!pParam || !psText || !psText[0])
		return 0;

	if(m_bDeviceLost)
		return 0;
#ifdef JX_MOBILE
	if (TgChuXep()) { TgChuThem(2, nFontId, psText, nCount, 0, 0, 0, 0, nLineWidth, 0, pParam, NULL, NULL); return 0; }	// [CHUNET 14/09] chat tren dau (PaintChat): ve sau blit
#endif

	for (i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		KTextProcess	tp(psText, nCount, nLineWidth * 2 / nFontId);
		if(pParam->nZ != TEXT_IN_SINGLE_PLANE_COORD)
		{
			int x, y, z;
			x = pParam->nX;
			y = pParam->nY;
			z = pParam->nZ;

			// ½«3D×ø±ê×ª»¯ÎªÆÁÄ»×ø±ê
			if(m_dwWindowStyle == RenderModel3DPerspective)
			{
				D3DXVECTOR3 vPos((float)x, (float)y, (float)z);
				D3DVIEWPORT9 viewportData = g_Device.GetViewport();
				D3DXVec3Project(&vPos, &vPos, &viewportData, &m_matProj, &m_matView, NULL);
				x = (int)vPos.x, y = (int)vPos.y;
			}
			else {
				CoordinateTransform(x, y, z);
			}

			pParam->nX = x;
			pParam->nY = y;
		}
		if (!pParam->BorderColor)
			m_FontTable[i].pFontObj->SetBorderColor(0xff000000);
		else
		m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
		return tp.DrawTextLine(m_FontTable[i].pFontObj, nFontId, pParam);
	}
	return 0;
}

int KRepresentShell3::LocateRichText(int nX, int nY,
					int nFontId, KOutputTextParam* pParam, 
					const char* psText, int nCount, int nLineWidth)
{
	int i;
	if(!pParam || !psText || !psText[0])
		return -1;

	if(m_bDeviceLost)
		return -1;

	for (i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		KTextProcess	tp(psText, nCount, nLineWidth * 2 / nFontId);
		if(pParam->nZ != TEXT_IN_SINGLE_PLANE_COORD)
		{
			int x, y, z;
			x = pParam->nX;
			y = pParam->nY;
			z = pParam->nZ;

			// ½«3D×ø±ê×ª»¯ÎªÆÁÄ»×ø±ê
			if(m_dwWindowStyle == RenderModel3DPerspective)
			{
				D3DXVECTOR3 vPos((float)x, (float)y, (float)z);
				D3DVIEWPORT9 viewportData = g_Device.GetViewport();
				D3DXVec3Project(&vPos, &vPos, &viewportData, &m_matProj, &m_matView, NULL);
				x = (int)vPos.x, y = (int)vPos.y;
			}
			else {
				CoordinateTransform(x, y, z);
			}

			pParam->nX = x;
			pParam->nY = y;
		}

		m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
		return tp.TransXYPosToCharOffset(nX, nY, m_FontTable[i].pFontObj, nFontId, pParam);
	}
	return -1;
}

void KRepresentShell3::ReleaseAFont(int nId)
{
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nId)
		{
			if (m_FontTable[i].pFontObj)
			{
				m_FontTable[i].pFontObj->Release();
				m_FontTable[i].pFontObj = NULL;
				m_FontTable[i].nId = 0;
				break;
			}
		}
	}
}

bool KRepresentShell3::SaveImage(const char* pszFile, const char* pszImage, int nFileType)
{
	if(!pszFile || !pszFile[0] || !pszImage || !pszImage[0])
		return false;

	if(m_bDeviceLost)
		return false;
	
	return true;
}

void KRepresentShell3::SetImageStoreBalanceParam(int nNumImage, unsigned int uCheckPoint)
{
	m_TextureResMgr.SetBalanceParam(nNumImage, uCheckPoint);
}

bool KRepresentShell3::CopyDeviceImageToImage(const char* pszName, int nDeviceX, int nDeviceY, int nImageX, int nImageY, int nWidth, int nHeight)
{
	if(!pszName || !pszName[0])
		return 0;

	if(m_bDeviceLost)
		return false;

	return true;
}

bool KRepresentShell3::RepresentBegin(int bClear, unsigned int Color)
{
	QueryPerformanceCounter(&g_liRep3VeBegin);	// [VE 08/09 b]
	{ static unsigned s_uKhung = 0; g_nRep3VeMau = ((++s_uKhung) & 7) == 0; }	// [VE 09/09 d]
	HRESULT hr;
	g_ntest = 0;
    // Test the cooperative level to see if it's okay to render
    if( FAILED( hr = PD3DDEVICE->TestCooperativeLevel() ) )
    {
		m_bDeviceLost = true;
        // If the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
        {
			return false;
		}

        // Check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr )
        {
            if( !InvalidateDeviceObjects() )
				return false;
			if( FAILED( g_Device.ResetDevice()) )
				return false;
			if( !RestoreDeviceObjects() )
				return false;
			m_bDeviceLost = false;
        }
		return false;
    }

	// Çå³ý±³¾°
	PD3DDEVICE->Clear( 0, NULL, D3DCLEAR_TARGET, bClear ? (0xff000000 | (Color & 0x00ffffff)) : D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );	// [REP3 03/09]
#ifdef JX_MOBILE
	m_dwTgMauXoa = bClear ? (0xff000000 | (Color & 0x00ffffff)) : D3DCOLOR_XRGB(0,0,0); s_uTgKhung++;	// [TG 13/09]
#endif

	// ¿ªÊ¼ÐÔÄÜÍ³¼Æ
	m_TextureResMgr.NapNenNhan();	// [NAP 08/09 b] nhan ket qua luong nen truoc khi ve
#ifdef JX_MOBILE
	m_TextureResMgr.JxNapKhungNhan();	// [VE 11/09] tao texture tu khung da giai ma o luong nen (theo ngan sach NapKhungApMs)
	m_TextureResMgr.JxNenTruocXuLy();	// [NENTRUOC 13/09] nen dat: muc cho tep -> giao khung
	{ extern void Rep3Gpu_TaiTruocChay(IDirect3DDevice9*, unsigned); if (g_nJxNapKhungKB > 0) Rep3Gpu_TaiTruocChay(PD3DDEVICE, (unsigned)g_nJxNapKhungKB << 10); }	// [TAI 14/09] tai dan khung nap truoc len GPU
	{ static int s_nJxKhoiTruoc = 0; if (!s_nJxKhoiTruoc) { s_nJxKhoiTruoc = 1; extern void Rep3Gpu_KhoiCapTruoc(IDirect3DDevice9*, int, int); Rep3Gpu_KhoiCapTruoc(PD3DDEVICE, g_nJxKhoiTruocPal, g_nJxKhoiTruoc32); } }	// [KHOITRUOC 14/09] khung dau: cap san khoi atlas + to 0
#endif
	m_TextureResMgr.m_bVeDangDien = true;
	m_TextureResMgr.StartProfile();

/*	float fAngleAdd = 0.03f;
	static float fAngel = 0.0f;
	fAngel += fAngleAdd;
	if(fAngel > 6.28f)
		fAngel = 0.0f;
	float fXAdd, fYAdd;
	fXAdd = sin(fAngel) * 50.0f;
	fYAdd = cos(fAngel) * 50.0f;

	static int c1 = 0x40, c2 = 0x60, c3 = 0x90;
	static int c1Add = 0x01, c2Add = 0x02, c3Add = 0x03;
	c1 += c1Add, c2 += c2Add, c3+= c3Add;
	if(c1 > 0xff)
		c1 = 0xff, c1Add = -0x01;
	if(c2 > 0xff)
		c2 = 0xff, c2Add = -0x02;
	if(c3 > 0xff)
		c3 = 0xff, c3Add = -0x04;
	if(c1 < 0)
		c1 = 0, c1Add = 0x01;
	if(c2 < 0)
		c2 = 0, c2Add = 0x01;
	if(c3 < 0)
		c3 = 0, c3Add = 0x01;

	static unsigned int dwTp[LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT];
	float fSenterX = LIGHTING_GRID_WIDTH * 16;
	float fSenterY = LIGHTING_GRID_HEIGHT * 16;
	float fx,fy,fd;
	for(int i=0; i<LIGHTING_GRID_HEIGHT; i++)
		for(int j=0; j<LIGHTING_GRID_WIDTH; j++)
		{
			fx = j * 32 - (fSenterX + fXAdd);
			fy = i * 32 - (fSenterY + fYAdd);
			fd = (float)(sqrt(fx * fx + fy * fy) / fSenterX);
			if(fd > 1.0f)
				fd = 1.0f;
			unsigned char c11,c22,c33;
			c11 = (1.0f - fd) * c1;
			c22 = (1.0f - fd) * c2;
			c33 = (1.0f - fd) * c3;
			dwTp[i * LIGHTING_GRID_WIDTH + j] = 0xff000000 | (c11<<16) | (c22<<8) | c33;
		}

//	SetLightInfo(112640, 197632, dwTp);
//	SetLightInfo(111104 + fXAdd, 195584 - 400 + fYAdd, dwTp);
*/
	if(FAILED(g_Device.Start3D()))
		return false;

	return true;
}

void KRepresentShell3::RepresentEnd()
{
	m_TextureResMgr.m_bVeDangDien = false;	// [NAP 08/09 b] ngoai luc ve: hoi anh nap ngay
	if (g_nRep3VeMau)	// [VE 09/09 d] chi khung mau: dem don vi + thoi gian DrawPrimitives + thoi gian khung (TB/khung khong doi, so khung in = khung mau)
	{
		g_uRep3VeKhung++;	// [VE 08/09 a]
		LARGE_INTEGER liNow; QueryPerformanceCounter(&liNow);	// [VE 08/09 b] CPU pass ve khung nay
		const double dKhung = g_liRep3VeBegin.QuadPart ? Rep3NapMs(g_liRep3VeBegin, liNow) : 0.0;
		g_dRep3VeKhungTong += dKhung; if (dKhung > g_dRep3VeKhungMax) g_dRep3VeKhungMax = dKhung;
		g_dRep3VeDpTong += g_dRep3VeDpKhung; if (g_dRep3VeDpKhung > g_dRep3VeDpMax) g_dRep3VeDpMax = g_dRep3VeDpKhung;
		g_dRep3VeDpKhung = 0.0;
	}
	{	// [NAP 08/09 a] tong ms nap trong khung nay -> max / dem khung nang
		if (g_dRep3NapKhung > g_dRep3NapKhungMax) g_dRep3NapKhungMax = g_dRep3NapKhung;
		if (g_dRep3NapKhung > 16.0) g_uRep3NapKhung16++; else if (g_dRep3NapKhung > 5.0) g_uRep3NapKhung5++;
#ifdef JX_MOBILE
		JxVeKhungChot();	// [VE 11/09] chot so nap + ve CPU cua khung (in [VE-GIAT] sau Present)
#endif
		g_dRep3NapKhung = 0.0;
	}
	if(m_bDeviceLost)
		return;

	char cc[200];

	// ÖÕÖ¹ÐÔÄÜÍ³¼Æ
	m_TextureResMgr.EndProfile();

	// Éú³É²¢ÏÔÊ¾Í³¼ÆÐÅÏ¢
	m_TextureResMgr.GetProfileString(cc, 200);
	KOutputTextParam param;
	param.Color = 0xffff;
	param.nNumLine = 4;
	param.nX = 0;
	param.nY = 30;

	int nFontId;

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId != 0)
		{
			nFontId = m_FontTable[i].nId;
			break;
		}
	}

//	OutputText(nFontId, cc, strlen(cc), 600, 20, 0xffffffff, 800);

	// Íê³É3DäÖÈ¾
	g_Device.End3D();
	// ½»»»Ò³Ãæ
#ifdef JX_MOBILE
	{ LARGE_INTEGER liJx0, liJx1; QueryPerformanceCounter(&liJx0); PD3DDEVICE->Present(NULL,NULL,NULL,NULL); QueryPerformanceCounter(&liJx1); JxVeGiatGhi(Rep3NapMs(liJx0, liJx1)); JxTheGioiCapNhat(Rep3NapMs(liJx0, liJx1)); }	// [VE 11/09] [TG 13/09]
#else
	PD3DDEVICE->Present(NULL,NULL,NULL,NULL);
#endif

	// [REP3 03/09] fps trung binh (EMA ~100 khung); chi don cache khi may khong dang chay cham (theo 2.0: >= 25 fps)
	DWORD dwNow = timeGetTime();
	if (m_dwLastPresent)
	{
		DWORD dwDt = dwNow - m_dwLastPresent;
		if (dwDt < 10) dwDt = 10;
		if (dwDt > 1000) dwDt = 1000;
		float fFps = 1000.0f / (float)dwDt;
		m_fFpsAvg = (m_fFpsAvg <= 0.0f) ? fFps : (m_fFpsAvg * 0.98f + fFps * 0.02f);
	}
	m_dwLastPresent = dwNow;
	if (m_fFpsAvg >= 25.0f)
		m_TextureResMgr.CheckBalanceFrame();
	{	// [REP3 08/09 q] moi 5 s: VRAM con thap -> ha ngan sach (che do tu dong)
		static DWORD s_dwLastVram = 0;
		if (s_dwLastVram == 0 || (dwNow - s_dwLastVram) >= 5000)
		{
			s_dwLastVram = dwNow;
			m_TextureResMgr.PressureByVram((unsigned __int64)(PD3DDEVICE->GetAvailableTextureMem() >> 20));
		}
	}

	// [REP3 03/09 RAM] thong ke dinh ky: RAM rieng tien trinh, VRAM con trong, cache texture (VRAM) + raw spr (RAM), so nap/bo, fps
	if (g_nRep3StatSec > 0)
	{
		static DWORD s_dwLastStat = 0;
		if (s_dwLastStat == 0 || (dwNow - s_dwLastStat) >= (DWORD)g_nRep3StatSec * 1000)
		{
			s_dwLastStat = dwNow;
			PROCESS_MEMORY_COUNTERS_EX pmc; memset(&pmc, 0, sizeof(pmc)); pmc.cb = sizeof(pmc);
			GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
			// [CPU 09/09 do] CPU % tien trinh va luong chinh trong cua so thong ke (cpu ms / ms that x 100; tien trinh co the > 100 % vi nhieu luong)
			static ULONGLONG s_uCpuTt = 0, s_uCpuLc = 0; static DWORD s_dwCpuMoc = 0; static int s_nNhan = 0;
			double dCpuTt = 0.0, dCpuLc = 0.0;
			{
				FILETIME ftT, ftX, ftK, ftU; ULONGLONG uTt = 0, uLc = 0;
				if (GetProcessTimes(GetCurrentProcess(), &ftT, &ftX, &ftK, &ftU)) uTt = (((ULONGLONG)ftK.dwHighDateTime << 32) | ftK.dwLowDateTime) + (((ULONGLONG)ftU.dwHighDateTime << 32) | ftU.dwLowDateTime);
				if (GetThreadTimes(GetCurrentThread(), &ftT, &ftX, &ftK, &ftU)) uLc = (((ULONGLONG)ftK.dwHighDateTime << 32) | ftK.dwLowDateTime) + (((ULONGLONG)ftU.dwHighDateTime << 32) | ftU.dwLowDateTime);
				if (s_nNhan == 0) { SYSTEM_INFO si; GetSystemInfo(&si); s_nNhan = (int)si.dwNumberOfProcessors; }
				if (s_dwCpuMoc != 0 && dwNow > s_dwCpuMoc) { const double dMs = (double)(dwNow - s_dwCpuMoc); dCpuTt = (double)(uTt - s_uCpuTt) / 10000.0 * 100.0 / dMs; dCpuLc = (double)(uLc - s_uCpuLc) / 10000.0 * 100.0 / dMs; }
				s_uCpuTt = uTt; s_uCpuLc = uLc; s_dwCpuMoc = dwNow;
			}
			uint32 uNodes = 0, uTexMB = 0, uRawMB = 0, uDrawMB = 0, uBudgetMB = 0;
			unsigned uVramUsed = 0, uVramBudget = 0; Rep3_D3D11VramInfo(&uVramUsed, &uVramBudget);	// [D3D11 08/09 f]
			m_TextureResMgr.GetStat(uNodes, uTexMB, uRawMB, uDrawMB, uBudgetMB);
			Rep3Log("[REP3] RAM rieng %u MB, WS %u MB | cpu tien trinh %.0f %% (luong chinh %.0f %%, may %d nhan) | VRAM con %u MB | cache %u muc: texture %u MB (ve khung nay %u MB, ngan sach %u MB), raw spr %u MB | nap %u, bo %u | fps TB %.0f | fx: tex_null %u anh_null %u tao_hong %u khung_khong_tex %u giai_ma %u khung %.1f ms | gpu tex %u (%u MB, %u trang %u MB) | vram %u/%u MB | d3d11: present TB %.2f ms bo %u, ve %u lenh %.1f us/lenh, gop %u quad -> %u Draw | pal %u hang",
				(unsigned)(pmc.PrivateUsage >> 20), (unsigned)(pmc.WorkingSetSize >> 20), dCpuTt, dCpuLc, s_nNhan, (unsigned)(PD3DDEVICE->GetAvailableTextureMem() >> 20),
				uNodes, uTexMB, uDrawMB, uBudgetMB, uRawMB, (unsigned)m_TextureResMgr.m_nLoadCount, (unsigned)m_TextureResMgr.m_nReleaseCount, m_fFpsAvg,
				g_uRep3FxTexNull, g_uRep3FxAnhNull, g_uRep3FxTaoHong, g_uRep3FxKhungKhongTex, g_uRep3FxGiaiMa, g_dRep3FxGiaiMaMs, g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20), g_uRep3AtlasPages, (unsigned)(g_uRep3AtlasBytes >> 20), uVramUsed, uVramBudget,
				g_uRep3Presents ? g_dRep3PresentMs / g_uRep3Presents : 0.0, g_uRep3PresentSkip, g_uRep3Draws, g_uRep3Draws ? g_dRep3DrawMs * 1000.0 / g_uRep3Draws : 0.0, g_uRep3BatchQuads, g_uRep3BatchDraws, g_uRep3PalRows);
#ifdef JX_APPLE	// [IOS-ANHRONG 12/09] dat o day vi JxVeKyIn (cho cu) KHONG chay tren iOS - log may thuc te khong co dong [VE-GOP]
			Rep3Log("[ANHRONG] anh chua co %u | khung vuot so khung %u (so khung lon nhat: %u)", g_uJxRongAnh, g_uJxRongKhung, g_uJxRongKhungMax);
			g_uJxRongAnh = 0; g_uJxRongKhung = 0; g_uJxRongKhungMax = 0;
#endif
			{	// [GOP 09/09 do]
				extern unsigned g_uRep3GopVo[12]; extern unsigned g_uRep3VeNgay[4]; extern unsigned g_uRep3CullGiu, g_uRep3CullBo; /* [MANG 09/09 e] */
				Rep3Log("[GOP] vo lo quad: doi trang atlas %u, texture rieng %u, srv1 %u, blend %u, sampler %u, ps st0 %u, ps st1 %u, alphatest %u, vs %u, layout %u, vp/scissor %u, day %u | ve ngay: fan %u, list %u, strip %u, khac %u | cull cpu: giu %u bo %u",
					g_uRep3GopVo[0], g_uRep3GopVo[1], g_uRep3GopVo[2], g_uRep3GopVo[3], g_uRep3GopVo[4], g_uRep3GopVo[5], g_uRep3GopVo[6], g_uRep3GopVo[7], g_uRep3GopVo[8], g_uRep3GopVo[9], g_uRep3GopVo[10], g_uRep3GopVo[11],
					g_uRep3VeNgay[0], g_uRep3VeNgay[1], g_uRep3VeNgay[2], g_uRep3VeNgay[3], g_uRep3CullGiu, g_uRep3CullBo);
				memset(g_uRep3GopVo, 0, sizeof(g_uRep3GopVo)); memset(g_uRep3VeNgay, 0, sizeof(g_uRep3VeNgay)); g_uRep3CullGiu = g_uRep3CullBo = 0;
			}
			Rep3Log("[LOCTG] tau=%d ms kieu=%d toi=%d | %u khung da tron", g_nRep3LocMs, g_nRep3LocKieu, (int)(255.0f / g_fRep3LocK + 0.5f), g_uRep3LocKhung);
			g_uRep3LocKhung = 0;
			Rep3Log("[CHUGIU] giu %d ms | dong chu giu %u, ve moi %u", g_nRep3ChuGiuMs, g_uRep3ChuGiu, g_uRep3ChuVe);
			g_uRep3ChuGiu = 0; g_uRep3ChuVe = 0;
			for (int ng = 1; ng <= 2; ng++)	// [NAPCHIEU 09/09 b] [NAPNPC 09/09] 1 = anh chieu (goi 95), 2 = anh than NPC (SetSprFile)
			{
				Rep3Log("%s nap truoc (bat=%d): goi %u | da co %u, giao nen %u, khong %u | lan dung dau: kip %u, tre %u", ng == 1 ? "[NAPCHIEU]" : "[NAPNPC]", ng == 1 ? g_nRep3NapChieu : g_nRep3NapNpc,
					g_uRep3NapTruoc[ng][0] + g_uRep3NapTruoc[ng][1] + g_uRep3NapTruoc[ng][2], g_uRep3NapTruoc[ng][1], g_uRep3NapTruoc[ng][2], g_uRep3NapTruoc[ng][0],
					m_TextureResMgr.m_nNapTruocKip[ng], m_TextureResMgr.m_nNapTruocTre[ng]);
				g_uRep3NapTruoc[ng][0] = g_uRep3NapTruoc[ng][1] = g_uRep3NapTruoc[ng][2] = 0; m_TextureResMgr.m_nNapTruocKip[ng] = 0; m_TextureResMgr.m_nNapTruocTre[ng] = 0;
			}
			g_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0; g_dRep3DrawMs = 0.0; g_uRep3Draws = 0; g_uRep3BatchQuads = 0; g_uRep3BatchDraws = 0;
			g_uRep3FxTexNull = 0; g_uRep3FxAnhNull = 0; g_uRep3FxTaoHong = 0; g_uRep3FxKhungKhongTex = 0; g_uRep3FxGiaiMa = 0; g_dRep3FxGiaiMaMs = 0.0;
			Rep3Log("[REP3-NAP] %ds tren luong ve: tep spr %u lan %.1f ms (max %.1f) | jpeg %u lan %.1f ms (max %.1f) | rut khung %u lan %.1f ms (max %.2f) | giai ma %u %.1f ms (max %.2f) | tao GPU %u %.1f ms (max %.2f) | khung co nap >5 ms: %u, >16 ms: %u, max %.1f ms/khung | nen: giao %u xong %u hong %u bo_ve %u | ve/khung: npc %.0f skill %.0f ui %.0f map %.0f tao %.0f khac %.0f (khung mau %u) | cpu ve: DrawPrimitives %.2f ms/khung (max %.1f), khung %.2f ms (max %.1f) | ring: %u vong, Map max %.1f ms | texture rieng tao %u",	// [NAP 08/09 a/b] [VE 08/09 a/b]
				g_nRep3StatSec, g_napSpr.n, g_napSpr.ms, g_napSpr.max, g_napJpeg.n, g_napJpeg.ms, g_napJpeg.max, g_napKhung.n, g_napKhung.ms, g_napKhung.max,
				g_napGiaiMa.n, g_napGiaiMa.ms, g_napGiaiMa.max, g_napGpu.n, g_napGpu.ms, g_napGpu.max, g_uRep3NapKhung5, g_uRep3NapKhung16, g_dRep3NapKhungMax,
				m_TextureResMgr.m_nNapNenGui, m_TextureResMgr.m_nNapNenXong, m_TextureResMgr.m_nNapNenHong, m_TextureResMgr.m_nNapNenBoVe,
				g_uRep3VeKhung ? (double)g_uRep3VeLoai[0] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[1] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[2] / g_uRep3VeKhung : 0.0,
				g_uRep3VeKhung ? (double)g_uRep3VeLoai[3] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[4] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[5] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung,
				g_uRep3VeKhung ? g_dRep3VeDpTong / g_uRep3VeKhung : 0.0, g_dRep3VeDpMax, g_uRep3VeKhung ? g_dRep3VeKhungTong / g_uRep3VeKhung : 0.0, g_dRep3VeKhungMax, g_uRep3RingVong, g_dRep3RingMapMax, g_uRep3TexRiengTao);
			g_dRep3VeDpTong = 0.0; g_dRep3VeDpMax = 0.0; g_dRep3VeKhungTong = 0.0; g_dRep3VeKhungMax = 0.0; g_uRep3RingVong = 0; g_dRep3RingMapMax = 0.0; g_uRep3TexRiengTao = 0; /* [MANG 09/09 c] */
			memset(g_uRep3VeLoai, 0, sizeof(g_uRep3VeLoai)); g_uRep3VeKhung = 0;
			m_TextureResMgr.m_nNapNenGui = 0; m_TextureResMgr.m_nNapNenXong = 0; m_TextureResMgr.m_nNapNenHong = 0; m_TextureResMgr.m_nNapNenBoVe = 0;
			memset(&g_napSpr, 0, sizeof(g_napSpr)); memset(&g_napJpeg, 0, sizeof(g_napJpeg)); memset(&g_napKhung, 0, sizeof(g_napKhung)); memset(&g_napGiaiMa, 0, sizeof(g_napGiaiMa)); memset(&g_napGpu, 0, sizeof(g_napGpu));
			g_dRep3NapKhungMax = 0.0; g_uRep3NapKhung5 = 0; g_uRep3NapKhung16 = 0;
#ifdef JX_MOBILE
			JxVeKyIn();	// [VE 11/09] [VE] + [VE-GOP] + [VE-NAP]
			JxTheGioiKyIn();	// [TG 13/09]
#endif
			Rep3AnhNullIn();	// [REP3 08/09 h]
		}
	}
}

void KRepresentShell3::ViewPortCoordToSpaceCoord(int& nX, int& nY, int nZ)
{
	if (g_renderModel == RenderModel3DPerspective || g_renderModel == RenderModel3DOrtho)
	{
		D3DVIEWPORT9 vp = g_Device.GetViewport();

		D3DXVECTOR3 screenNear((float)nX, (float)nY, 0.0f);
		D3DXVECTOR3 screenFar((float)nX, (float)nY, 1.0f);
		D3DXVECTOR3 worldNear, worldFar;

		D3DXVec3Unproject(&worldNear, &screenNear, &vp, &m_matProj, &m_matView, nullptr);
		D3DXVec3Unproject(&worldFar, &screenFar, &vp, &m_matProj, &m_matView, nullptr);

		D3DXVECTOR3 dir = worldFar - worldNear;

		if (fabs(dir.z) < 1e-6f)  // Avoid division by near-zero
			return;

		float t = (nZ - worldNear.z) / dir.z;
		D3DXVECTOR3 worldHit = worldNear + dir * t;

		nX = static_cast<int>(worldHit.x);
		nY = static_cast<int>(worldHit.y);
	}
	else
	{
		// Legacy 2D logic (if you're using map rendering or UI mode)
#ifdef JX_MOBILE
		if (m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000)	// [ZOOM3D 14/09] != 1000 (ca phong to)
		{	// [ZOOM 13/09] cham tren khung -> diem anh RT (nhan zoom, bo lui goc RT) -> the gioi theo goc KHUNG (m_nLeft dang lui neu dang ve RT)
			// [LAC 14/09] them M^-1 (x = c.q.x + 2s.q.y, y = -s.q.x/2 + c.q.y) quanh tam khung; goc 0 va le 1000 = cong thuc cu
			const int nGocX = (m_nTgTrangThai == 1) ? m_nTgLeftKhung : m_nLeft;
			const int nGocY = (m_nTgTrangThai == 1) ? m_nTgTopKhung : m_nTop;
			const int nKW = (m_nTgTrangThai == 1) ? m_nTgKhungW : g_nScreenWidth, nKH = (m_nTgTrangThai == 1) ? m_nTgKhungH : g_nScreenHeight;
			const float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc), fK = (float)m_nTgDoc / 1000.0f;	// [LAC 14/09 b] fK = co dan doc
			const float qx = (float)(nX - nKW / 2), qy = (float)(nY - nKH / 2), fZ = (float)m_nTgZoom / 1000.0f;
			const float px = (float)m_nTgW * 0.5f + (fC * qx + 2.0f * fS * qy) * fZ;
			const float py = (float)m_nTgH * 0.5f + (-0.5f * fS * qx + fC * qy) / fK * fZ;	// [LAC 14/09 b] chia co dan doc
			nX = (int)floorf(px + 0.5f) - m_nZoomDx + nGocX;
			nY = ((int)floorf(py + 0.5f) - m_nZoomDy + nGocY + ((nZ * 887) >> 10)) * 2;
			return;
		}
#endif
		nX = nX + m_nLeft;
		nY = (nY + m_nTop + ((nZ * 887) >> 10)) * 2;
	}
}


void KRepresentShell3::D3DTerm()
{
	g_Device.ReleaseDevObjects();

	// [09/09] D3DTerm con duoc goi tren duong LOI cua Create() (khong tim duoc card / che do man hinh),
	// luc do chua tao duoc thiet bi nen PD3DDEVICE la NULL -> dong duoi day sap thay vi bao loi tu te.
	if (PD3DDEVICE)
	PD3DDEVICE->SetGammaRamp(0, D3DSGR_CALIBRATE, &m_ramp);
	g_Device.FreeAll(); 					// Free the Device...
	g_D3DShell.FreeAll();					// Free the Shell...
}

void KRepresentShell3::CoordinateTransform( int& nX, int& nY, int nZ)
{
	if (g_renderModel == RenderModel2D)		// [REP3 03/09] cong thuc Represent2
	{
		nX = nX - m_nLeft;
		nY = nY / 2 - m_nTop - ((nZ * 887) >> 10);
#ifdef JX_MOBILE
		if ((m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)
		{	// [ZOOM 13/09] ve len khung (sau blit): diem anh RT = (diem - goc khung) + lui goc RT, roi thu nho theo zoom; [LAC 14/09] + xoay M quanh tam khung
			const float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc), fK = (float)m_nTgDoc / 1000.0f;	// [LAC 14/09 b] fK = co dan doc
			const float qx = ((float)(nX + m_nZoomDx) - (float)m_nTgW * 0.5f) * 1000.0f / (float)m_nTgZoom;
			const float qy = ((float)(nY + m_nZoomDy) - (float)m_nTgH * 0.5f) * 1000.0f / (float)m_nTgZoom;
			nX = (int)floorf((float)g_nScreenWidth * 0.5f + fC * qx - 2.0f * fS * fK * qy + 0.5f);
			nY = (int)floorf((float)g_nScreenHeight * 0.5f + 0.5f * fS * qx + fC * fK * qy + 0.5f);
		}
#endif
		return;
	}
	// 1) grab current viewport
	D3DVIEWPORT9 vp;
	PD3DDEVICE->GetViewport(&vp);

	// 2) build a world©\space vector for your point
	D3DXVECTOR3 worldPos((float)nX, (float)nY, (float)nZ);

	// 3) prepare an identity world matrix (no extra world transform)
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	// 4) project it into screen space
	D3DXVECTOR3 screenPos;
	D3DXVec3Project(
		&screenPos,
		&worldPos,
		&vp,
		&m_matProj,    // your projection matrix
		&m_matView,    // your view matrix (from LookAt)
		&matWorld      // identity world matrix
	);

	// 5) write back pixel coords (rounding for safety)
	nX = int(screenPos.x + 0.5f);
	nY = int(screenPos.y + 0.5f);
}

void KRepresentShell3::CoordinateTransformX(int& nX, int& nY, int nZ)
{
	if (g_renderModel == RenderModel2D)		// [REP3 03/09] cong thuc Represent2
	{
		nX = nX - m_nLeft;
		nY = nY / 2 - m_nTop - ((nZ * 887) >> 10);
#ifdef JX_MOBILE
		if ((m_nTgZoomRt != 1000 || m_nTgXoay || m_nTgDoc != 1000) && m_nTgTrangThai == 0)
		{	// [ZOOM 13/09] ve len khung (sau blit): diem anh RT = (diem - goc khung) + lui goc RT, roi thu nho theo zoom; [LAC 14/09] + xoay M quanh tam khung
			const float fGoc = (float)m_nTgXoay * 3.14159265f / 18000.0f, fC = cosf(fGoc), fS = sinf(fGoc), fK = (float)m_nTgDoc / 1000.0f;	// [LAC 14/09 b] fK = co dan doc
			const float qx = ((float)(nX + m_nZoomDx) - (float)m_nTgW * 0.5f) * 1000.0f / (float)m_nTgZoom;
			const float qy = ((float)(nY + m_nZoomDy) - (float)m_nTgH * 0.5f) * 1000.0f / (float)m_nTgZoom;
			nX = (int)floorf((float)g_nScreenWidth * 0.5f + fC * qx - 2.0f * fS * fK * qy + 0.5f);
			nY = (int)floorf((float)g_nScreenHeight * 0.5f + 0.5f * fS * qx + fC * fK * qy + 0.5f);
		}
#endif
		return;
	}
	// 1) grab current viewport
	D3DVIEWPORT9 vp;
	PD3DDEVICE->GetViewport(&vp);

	// 2) build a world©\space vector for your point
	D3DXVECTOR3 worldPos((float)nX, (float)nY, (float)nZ);

	// 3) prepare an identity world matrix (no extra world transform)
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	// 4) project it into screen space
	D3DXVECTOR3 screenPos;
	D3DXVec3Project(
		&screenPos,
		&worldPos,
		&vp,
		&m_matProj,    // your projection matrix
		&m_matView,    // your view matrix (from LookAt)
		&matWorld      // identity world matrix
	);

	// 5) write back pixel coords (rounding for safety)
	nX = int(screenPos.x + 0.5f);
	nY = int(screenPos.y + 0.5f);
}

void KRepresentShell3::DrawRect(int32 x1, int32 y1, int32 nWidth, int32 nHeight, DWORD color)
{
	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	VERTEX2D* pvb;
	m_pVB2D->Lock( 0, 4*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)x1;
	fY1 = (float)y1;
	fX2 = fX1 + (float)nWidth;
	fY2 = fY1 + (float)nHeight;

	pvb[0].position = D3DXVECTOR4( fX1,fY1, 100, 1 );
	pvb[0].color    = color;

	pvb[1].position = D3DXVECTOR4( fX2,fY1, 100, 1 );
    pvb[1].color    = color;

	pvb[2].position = D3DXVECTOR4( fX1,fY2, 100, 1 );
    pvb[2].color    = color;

	pvb[3].position = D3DXVECTOR4( fX2,fY2, 100, 1 );
    pvb[3].color    = color;

	m_pVB2D->Unlock();

	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
}

void KRepresentShell3::DrawPoint(int nPrimitiveCount, KRepresentUnit* pPrimitives, int bSinglePlaneCoord)
{
	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	VERTEX2D* pvb;
	if(FAILED(m_pVB2D->Lock( 0, nPrimitiveCount * sizeof(VERTEX2D), (void**)&pvb, 0 )))
		return;

	KRUPoint* pTemp = (KRUPoint *)pPrimitives;
	for (int i = 0; i < nPrimitiveCount; i++, pTemp++)
	{				
		int nX = pTemp->oPosition.nX;
		int nY = pTemp->oPosition.nY;
		if (!bSinglePlaneCoord)
			CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
		DWORD color = D3DCOLOR_ARGB(255, (DWORD)pTemp->Color.Color_b.r, 
					(DWORD)pTemp->Color.Color_b.g, (DWORD)pTemp->Color.Color_b.b);

		pvb[i].position = D3DXVECTOR4( (float)nX, (float)nY, 100, 1 );
		pvb[i].color    = color;
	}

	m_pVB2D->Unlock();

	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->DrawPrimitive( D3DPT_POINTLIST, 0, nPrimitiveCount );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
}

void KRepresentShell3::DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, DWORD color)
{
	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	VERTEX2D* pvb;
	m_pVB2D->Lock( 0, 2*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)x1;
	fY1 = (float)y1;
	fX2 = (float)x2;
	fY2 = (float)y2;

	pvb[0].position = D3DXVECTOR4( fX1,fY1, 100, 1 );
	pvb[0].color    = color;

	pvb[1].position = D3DXVECTOR4( fX2,fY2, 100, 1 );
    pvb[1].color    = color;

	m_pVB2D->Unlock();

	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->DrawPrimitive( D3DPT_LINELIST, 0, 1 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
}

void KRepresentShell3::DrawRectFrame(int32 x1, int32 y1, int32 x2, int32 y2, DWORD color)
{
	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	VERTEX2D* pvb;
	m_pVB2D->Lock( 0, 5*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)x1;
	fY1 = (float)y1;
	fX2 = (float)x2;
	fY2 = (float)y2;

	pvb[0].position = D3DXVECTOR4( fX1,fY1, 100, 1 );
	pvb[0].color    = color;

	pvb[1].position = D3DXVECTOR4( fX2,fY1, 100, 1 );
    pvb[1].color    = color;

	pvb[2].position = D3DXVECTOR4( fX2,fY2, 100, 1 );
    pvb[2].color    = color;

	pvb[3].position = D3DXVECTOR4( fX1,fY2, 100, 1 );
    pvb[3].color    = color;

	pvb[4].position = D3DXVECTOR4( fX1,fY1, 100, 1 );
	pvb[4].color    = color;

	m_pVB2D->Unlock();

	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
	PD3DDEVICE->DrawPrimitive( D3DPT_LINESTRIP, 0, 4 );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
}

void KRepresentShell3::DrawBitmap16(int32 nX, int32 nY, int32 nWidth,
									int32 nHeight, TextureResBmp* pBitmap, bool bStretch)
{
	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	VERTEX2D* pvb;
	m_pVB2D->Lock( 0, 4*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)nX;
	fY1 = (float)nY;
	fX2 = fX1 + (float)nWidth;
	fY2 = fY1 + (float)nHeight;

	float fU2, fV2;
	fU2 = (float)pBitmap->m_nWidth / (float)pBitmap->m_FrameInfo.texInfo[0].nWidth;
	fV2 = (float)pBitmap->m_nHeight / (float)pBitmap->m_FrameInfo.texInfo[0].nHeight;

	pvb[0].position = D3DXVECTOR4( fX1,fY1, 100, 1 );
	pvb[0].color    = 0xffffffff;
    pvb[0].tu       = 0.0f;
    pvb[0].tv       = 0.0f;

	pvb[1].position = D3DXVECTOR4( fX2,fY1, 100, 1 );
    pvb[1].color    = 0xffffffff;
    pvb[1].tu       = fU2;
    pvb[1].tv       = 0.0f;

	pvb[2].position = D3DXVECTOR4( fX1,fY2, 100, 1 );
    pvb[2].color    = 0xffffffff;
    pvb[2].tu       = 0.0f;
    pvb[2].tv       = fV2;

	pvb[3].position = D3DXVECTOR4( fX2,fY2, 100, 1 );
    pvb[3].color    = 0xffffffff;
    pvb[3].tu       = fU2;
    pvb[3].tv       = fV2;

	m_pVB2D->Unlock();

	if(!pBitmap->m_FrameInfo.texInfo[0].pTexture)
	{
		assert(pBitmap->m_FrameInfo.texInfo[0].pTexture);
		return;
	}
	PD3DDEVICE->SetTexture( 0, pBitmap->m_FrameInfo.texInfo[0].pTexture );
	if(bStretch)
	{
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	}
	else
		PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void KRepresentShell3::DrawSpriteAlpha(int32 nX, int32 nY, int32 nWidth, int32 nHeight,
									   int32 nFrame, TextureResSpr* pSprite, DWORD color, int32 nRenderStyle)
{
	int i;

	if(nFrame >= pSprite->m_nFrameNum)
		return;
	// [REP3 03/09] BORDER: Represent2 khong ve gi (DrawSpriteBorder da bi chu thich) - khong lam sang nhan vat nua
	if (nRenderStyle == IMAGE_RENDER_STYLE_BORDER)
		return;

	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)nX;
	fY1 = (float)nY;
	fX2 = fX1 + (float)nWidth;
	fY2 = fY1 + (float)nHeight;

	if(fX2 < 0 || fX1 > g_nScreenWidth || fY2 < 0 || fY1 > g_nScreenHeight)
		return;

	DWORD vtxColor = 0xffffffff;
	SetSpriteBlend(nRenderStyle, color, pSprite->m_bNew, vtxColor);

	VERTEX2D* pvb = NULL;
	if(FAILED(m_pVB2D->Lock( 0, 16*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD )))
	{
		ResetSpriteBlend();
		return;
	}

	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		float fU2, fV2;
		float x1, y1, x2, y2;
		fU2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		fV2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		x1 = ChaZhi(fX1, fX2, 0, (float)nWidth, (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		y1 = ChaZhi(fY1, fY2, 0, (float)nHeight, (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		x2 = ChaZhi(fX1, fX2, 0, (float)nWidth, (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX +
										pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		y2 = ChaZhi(fY1, fY2, 0, (float)nHeight, (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY +
										pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		x1 -= 0.5f,	y1 -= 0.5f,	x2 -= 0.5f,	y2 -= 0.5f;

		int nBase = i*4;
		pvb[nBase+0].position = D3DXVECTOR4( x1,y1, 100, 1 );
		pvb[nBase+0].color    = vtxColor;
		pvb[nBase+0].tu       = 0.0f;
		pvb[nBase+0].tv       = 0.0f;

		pvb[nBase+1].position = D3DXVECTOR4( x2,y1, 100, 1 );
		pvb[nBase+1].color    = vtxColor;
		pvb[nBase+1].tu       = fU2;
		pvb[nBase+1].tv       = 0.0f;

		pvb[nBase+2].position = D3DXVECTOR4( x1,y2, 100, 1 );
		pvb[nBase+2].color    = vtxColor;
		pvb[nBase+2].tu       = 0.0f;
		pvb[nBase+2].tv       = fV2;

		pvb[nBase+3].position = D3DXVECTOR4( x2,y2, 100, 1 );
		pvb[nBase+3].color    = vtxColor;
		pvb[nBase+3].tu       = fU2;
		pvb[nBase+3].tv       = fV2;
	}
	m_pVB2D->Unlock();

	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);
		if(!pTex)
		{
			g_uRep3FxTexNull++;	// [FX 07/09] truoc day bo qua im lang
			continue;
		}
		PD3DDEVICE->SetTexture( 0, pTex );
		PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );
	}
	ResetSpriteBlend();
}

void KRepresentShell3::DrawSpritePartAlpha(int32 nX, int32 nY, int32 nWidth, int32 nHeight,
									   int32 nFrame, TextureResSpr* pSprite, RECT &rect, DWORD color, int32 nRenderStyle)
{
	int i;

	if(nFrame >= pSprite->m_nFrameNum)
		return;
	if (nRenderStyle == IMAGE_RENDER_STYLE_BORDER)	// [REP3 03/09] Represent2 khong ve
		return;

	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)nX;
	fY1 = (float)nY;
	fX2 = fX1 + (float)nWidth;
	fY2 = fY1 + (float)nHeight;

	// Èç¹ûÍ¼ËØ³¬³öÆÁÄ»·¶Î§Ôò²»äÖÈ¾
	if(fX2 < 0 || fX1 > g_nScreenWidth || fY2 < 0 || fY1 > g_nScreenHeight)
		return;

	VERTEX2D* pvb = NULL;
	DWORD vtxColor = 0xffffffff;
	SetSpriteBlend(nRenderStyle, color, pSprite->m_bNew, vtxColor);	// [REP3 03/09]
	if(FAILED(m_pVB2D->Lock( 0, 16*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD )))
	{
		ResetSpriteBlend();
		return;
	}

	bool bDraw[4];

	// ¸ù¾ÝÌùÍ¼ÊýÄ¿°Ñ¾ØÐÎ²ð·Ö³É¶à¸öÐ¡¾ØÐÎ£¬¼ÆËã×ø±ê¼°ÎÆÀí
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		float fU1, fV1, fU2, fV2, u2, v2;
		float x1, y1, x2, y2, fRcX1, fRcY1, fRcX2, fRcY2;

		x1 = ChaZhi(fX1, fX2, 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
			(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		y1 = ChaZhi(fY1, fY2, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
			(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		x2 = ChaZhi(fX1, fX2, 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
			(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		y2 = ChaZhi(fY1, fY2, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
			(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		x1 -= 0.5f,	y1 -= 0.5f,	x2 -= 0.5f,	y2 -= 0.5f;

		fRcX1 = (float)rect.left, fRcY1 = (float)rect.top;
		fRcX2 = (float)rect.right, fRcY2 = (float)rect.bottom;
		if(fRcX1 > x2 || fRcX2 < x1 || fRcY1 > y2 || fRcY2 < y1)
		{
			// Èç¹ûÕâ¸öÃæÆ¬µÄ¼ô²Ê¾ØÐÎÔÚÍ¼ËØ¾ØÐÎÖ®ÍâÔò²»»­
			bDraw[i] = false;
			continue;
		}
		bDraw[i] = true;
		if(fRcX1 < x1) fRcX1 = x1;
		if(fRcY1 < y1) fRcY1 = y1;
		if(fRcX2 > x2) fRcX2 = x2;
		if(fRcY2 > y2) fRcY2 = y2;

		u2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth / 
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		v2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;
		fU1 = ChaZhi(0, u2, x1, x2, fRcX1);
		fV1 = ChaZhi(0, v2, y1, y2, fRcY1);
		fU2 = ChaZhi(0, u2, x1, x2, fRcX2);
		fV2 = ChaZhi(0, v2, y1, y2, fRcY2);

		int nBase = i*4;
		pvb[nBase+0].position = D3DXVECTOR4( fRcX1,fRcY1, 100, 1 );
		pvb[nBase+0].color    = vtxColor;
		pvb[nBase+0].tu       = fU1;
		pvb[nBase+0].tv       = fV1;

		pvb[nBase+1].position = D3DXVECTOR4( fRcX2,fRcY1, 100, 1 );
		pvb[nBase+1].color    = vtxColor;
		pvb[nBase+1].tu       = fU2;
		pvb[nBase+1].tv       = fV1;

		pvb[nBase+2].position = D3DXVECTOR4( fRcX1,fRcY2, 100, 1 );
		pvb[nBase+2].color    = vtxColor;
		pvb[nBase+2].tu       = fU1;
		pvb[nBase+2].tv       = fV2;

		pvb[nBase+3].position = D3DXVECTOR4( fRcX2,fRcY2, 100, 1 );
		pvb[nBase+3].color    = vtxColor;
		pvb[nBase+3].tu       = fU2;
		pvb[nBase+3].tv       = fV2;
	}

	m_pVB2D->Unlock();

	// »æÖÆ¶à±ßÐÎ
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);
		if(!pTex || !bDraw[i])
			continue;
		PD3DDEVICE->SetTexture( 0, pTex );
		PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );	
	}
	ResetSpriteBlend();
}

void KRepresentShell3::DrawBitmap16Part(int32 nX, int32 nY, int32 nWidth, int32 nHeight, TextureResBmp* pBitmap, RECT &rc)
{
	if(FAILED(PD3DDEVICE->SetStreamSource( 0, m_pVB2D, 0, sizeof(VERTEX2D) )))
		return;
	PD3DDEVICE->SetFVF( D3DFVF_VERTEX2D );

	VERTEX2D* pvb;
	m_pVB2D->Lock( 0, 4*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD );

	float fX1, fY1, fX2, fY2;
	fX1 = (float)nX;
	fY1 = (float)nY;
	fX2 = fX1 - (float)rc.left + (float)rc.right;
	fY2 = fY1 - (float)rc.top + (float)rc.bottom;

	float fU1, fV1, fU2, fV2, u2, v2;
	float fWidth = (float)pBitmap->GetWidth();
	float fHeight = (float)pBitmap->GetHeight();
	u2 = fWidth / (float)pBitmap->m_FrameInfo.texInfo[0].nWidth;
	v2 = fHeight / (float)pBitmap->m_FrameInfo.texInfo[0].nHeight;
	fU1 = ChaZhi(0, u2, 0, fWidth, (float)rc.left);
	fV1 = ChaZhi(0, v2, 0, fHeight, (float)rc.top);
	fU2 = ChaZhi(0, u2, 0, fWidth, (float)rc.right);
	fV2 = ChaZhi(0, v2, 0, fHeight, (float)rc.bottom);

	fX1 -= 0.5f,	fY1 -= 0.5f,	fX2 -= 0.5f,	fY2 -= 0.5f;

	pvb[0].position = D3DXVECTOR4( fX1,fY1, 100, 1 );
	pvb[0].color    = 0xffffffff;
    pvb[0].tu       = fU1;
    pvb[0].tv       = fV1;

	pvb[1].position = D3DXVECTOR4( fX2,fY1, 100, 1 );
    pvb[1].color    = 0xffffffff;
    pvb[1].tu       = fU2;
    pvb[1].tv       = fV1;

	pvb[2].position = D3DXVECTOR4( fX1,fY2, 100, 1 );
    pvb[2].color    = 0xffffffff;
    pvb[2].tu       = fU1;
    pvb[2].tv       = fV2;

	pvb[3].position = D3DXVECTOR4( fX2,fY2, 100, 1 );
    pvb[3].color    = 0xffffffff;
    pvb[3].tu       = fU2;
    pvb[3].tv       = fV2;

	m_pVB2D->Unlock();

	if(!pBitmap->m_FrameInfo.texInfo[0].pTexture)
	{
		assert(pBitmap->m_FrameInfo.texInfo[0].pTexture);
		return;
	}
	PD3DDEVICE->SetTexture( 0, pBitmap->m_FrameInfo.texInfo[0].pTexture );
	PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

void KRepresentShell3::DrawBitmap163D(RenderParam3D &param, TextureResBmp* pBitmap)
{
	float fU2, fV2;
	fU2 = (float)pBitmap->m_nWidth / (float)pBitmap->m_FrameInfo.texInfo[0].nWidth;
	fV2 = (float)pBitmap->m_nHeight / (float)pBitmap->m_FrameInfo.texInfo[0].nHeight;

	float ft1,ft2;
	ft1 = 0.5f / (float)pBitmap->m_FrameInfo.texInfo[0].nWidth;
	ft2 = 0.5f / (float)pBitmap->m_FrameInfo.texInfo[0].nHeight;

	VERTEX3D* pvb = NULL;
	if(FAILED(m_pVB3D->Lock( 0, 4*sizeof(VERTEX3D), (void**)&pvb, 0 )))
		return;

	pvb[0].position = param.m_pos[0];
	pvb[1].position = param.m_pos[1];
	pvb[2].position = param.m_pos[3];
	pvb[3].position = param.m_pos[2];

	pvb[0].color    = 0xffffffff;
    pvb[0].tu       = 0.0f + ft1;
    pvb[0].tv       = 0.0f + ft2;

    pvb[1].color    = 0xffffffff;
    pvb[1].tu       = fU2 - ft1;
    pvb[1].tv       = 0.0f + ft2;

    pvb[2].color    = 0xffffffff;
    pvb[2].tu       = 0.0f + ft1;
    pvb[2].tv       = fV2 - ft2;

    pvb[3].color    = 0xffffffff;
    pvb[3].tu       = fU2 - ft1;
    pvb[3].tv       = fV2 - ft2;

	m_pVB3D->Unlock();

	if(!pBitmap->m_FrameInfo.texInfo[0].pTexture)
	{
		assert(pBitmap->m_FrameInfo.texInfo[0].pTexture);
		return;
	}

	PD3DDEVICE->SetTexture( 0, pBitmap->m_FrameInfo.texInfo[0].pTexture );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
}

void KRepresentShell3::DrawBitmap163DLighting(RenderParam3D &param, TextureResBmp* pBitmap)
{
	float fU2, fV2;
	fU2 = (float)pBitmap->m_nWidth / (float)pBitmap->m_FrameInfo.texInfo[0].nWidth;
	fV2 = (float)pBitmap->m_nHeight / (float)pBitmap->m_FrameInfo.texInfo[0].nHeight;

	float ft1,ft2;
	ft1 = 0.5f / (float)pBitmap->m_FrameInfo.texInfo[0].nWidth;
	ft2 = 0.5f / (float)pBitmap->m_FrameInfo.texInfo[0].nHeight;

	VERTEX3D vbSrc[4];

	vbSrc[0].position = param.m_pos[0];
	vbSrc[1].position = param.m_pos[1];
	vbSrc[2].position = param.m_pos[2];
	vbSrc[3].position = param.m_pos[3];
	
	vbSrc[0].color    = 0xffffffff;
    vbSrc[0].tu       = 0.0f + ft1;
    vbSrc[0].tv       = 0.0f + ft2;

    vbSrc[1].color    = 0xffffffff;
    vbSrc[1].tu       = fU2 - ft1;
    vbSrc[1].tv       = 0.0f + ft2;

    vbSrc[2].color    = 0xffffffff;
	vbSrc[2].tu       = fU2 - ft1;
    vbSrc[2].tv       = fV2 - ft2;

    vbSrc[3].color    = 0xffffffff;
    vbSrc[3].tu       = 0.0f + ft1;
    vbSrc[3].tv       = fV2 - ft2;

	VERTEX3D* pvb = NULL;
	if(FAILED(m_pVB3D->Lock( 0, VERTEX_BUFFER_SIZE*sizeof(VERTEX3D), (void**)&pvb, 0 )))
		return;

	// Éú³ÉÍø¸ñ£¬¼ÆËãÁÁ¶È
	uint32 nCount  = BuildMesh3D(vbSrc, pvb, VERTEX_BUFFER_SIZE, 0xffffffff);

	m_pVB3D->Unlock();

	if(nCount <= 2)
		return;

	if(!pBitmap->m_FrameInfo.texInfo[0].pTexture)
	{
		assert(pBitmap->m_FrameInfo.texInfo[0].pTexture);
		return;
	}

	PD3DDEVICE->SetTexture( 0, pBitmap->m_FrameInfo.texInfo[0].pTexture );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE4X );
	PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, nCount - 2 );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
}

void KRepresentShell3::DrawSpriteAlpha3D(RenderParam3D &param, int32 nFrame, 
									TextureResSpr* pSprite, DWORD color, int32 nRenderStyle, RECT *rect)
{
	int i;
	if(nFrame >= pSprite->m_nFrameNum)
		return;

	//Check if the pixel is visible in screen space, and return if it is not visible
	//This can also be optimized
	D3DXVECTOR3 vPos1, vPos2, vPos3, vPos4;
	D3DVIEWPORT9 viewportData = g_Device.GetViewport();
	D3DXVec3Project(&vPos1, &param.m_pos[0], &viewportData, &m_matProj, &m_matView, NULL);
	D3DXVec3Project(&vPos2, &param.m_pos[1], &viewportData, &m_matProj, &m_matView, NULL);
	D3DXVec3Project(&vPos3, &param.m_pos[2], &viewportData, &m_matProj, &m_matView, NULL);
	D3DXVec3Project(&vPos4, &param.m_pos[3], &viewportData, &m_matProj, &m_matView, NULL);

	if(vPos2.x < 0 || vPos1.x > g_nScreenWidth || vPos3.x < 0 || vPos4.x > g_nScreenWidth)
		return;
	if(vPos3.y < 0 || vPos1.y > g_nScreenHeight || vPos4.y < 0 || vPos2.y > g_nScreenHeight)
		return;

	D3DXVECTOR3 v11, v22, v1, v2, v3, v4;

	VERTEX3D* pvb = NULL;
	if(FAILED(m_pVB3D->Lock( 0, 16*sizeof(VERTEX3D), (void**)&pvb, 0 )))
		return;

	bool bDraw[4];

	if(nRenderStyle != IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST)
	{
		// No color cast, change the color to white
		color = 0xffffffff;
	}

	// Split the rectangle into multiple small rectangles according to the number of textures, 
	// calculate the coordinates and texture
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		bDraw[i] = true;
		float fU1 = 0.0f, fV1 = 0.0f, fU2, fV2, fu2, fv2;
		fU2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth / 
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		fV2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		// If you only draw part of the pixel, 
		// you need to adjust the texture coordinates
		if(rect)
		{
			float fRcX1, fRcY1, fRcX2, fRcY2;
			float fFrameX1 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX;
			float fFrameY1 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY;
			float fFrameX2 = (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth);
			float fFrameY2 = (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight);
			fRcX1 = (float)rect->left, fRcY1 = (float)rect->top;
			fRcX2 = (float)rect->right, fRcY2 = (float)rect->bottom;
			if(fRcX1 > fFrameX2	|| fRcX2 < fFrameX1	|| fRcY1 > fFrameY2	|| fRcY2 < fFrameY1)
			{
				// If the cutout rectangle of this patch is outside 
				// the pixel rectangle, it will not be drawn.
				bDraw[i] = false;
				continue;
			}
			if(fRcX1 < fFrameX1) fRcX1 = fFrameX1;
			if(fRcY1 < fFrameY1) fRcY1 = fFrameY1;
			if(fRcX2 > fFrameX2) fRcX2 = fFrameX2;
			if(fRcY2 > fFrameY2) fRcY2 = fFrameY2;

			fu2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth / 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
			fv2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;
			fU1 = ChaZhi(0, fu2, fFrameX1, fFrameX2, fRcX1);
			fV1 = ChaZhi(0, fv2, fFrameY1, fFrameY2, fRcY1);
			fU2 = ChaZhi(0, fu2, fFrameX1, fFrameX2, fRcX2);
			fV2 = ChaZhi(0, fv2, fFrameY1, fFrameY2, fRcY2);
		}

		float ft1,ft2;
		ft1 = 0.5f / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		ft2 = 0.5f / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		v11 = ChaZhi(param.m_pos[0], param.m_pos[1], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth,
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		v22 = ChaZhi(param.m_pos[3], param.m_pos[2], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth,
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		v1 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		v4 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		v11 = ChaZhi(param.m_pos[0], param.m_pos[1], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		v22 = ChaZhi(param.m_pos[3], param.m_pos[2], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		v2 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		v3 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		int nBase = i*4;
		pvb[nBase+0].position = v1;
		pvb[nBase+0].color    = color;
		pvb[nBase+0].tu       = fU1+ft1;
		pvb[nBase+0].tv       = fV1+ft2;

		pvb[nBase+1].position = v2;
		pvb[nBase+1].color    = color;
		pvb[nBase+1].tu       = fU2-ft1;
		pvb[nBase+1].tv       = fV1+ft2;

		pvb[nBase+2].position = v4;
		pvb[nBase+2].color    = color;
		pvb[nBase+2].tu       = fU1+ft1;
		pvb[nBase+2].tv       = fV2-ft2;

		pvb[nBase+3].position = v3;
		pvb[nBase+3].color    = color;
		pvb[nBase+3].tu       = fU2-ft1;
		pvb[nBase+3].tv       = fV2-ft2;
	}

	m_pVB3D->Unlock();

	// Draw polygons
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);
		if(!pTex || !bDraw[i])
			continue;
		PD3DDEVICE->SetTexture( 0, pTex );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		if( nRenderStyle == IMAGE_RENDER_STYLE_BORDER )
		{
			// Select highlight effect
			PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
			PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );
			PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		}
		else
			PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );	
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	}
}

void KRepresentShell3::DrawSpriteAlpha3DLighting(RenderParam3D &param, int32 nFrame, 
											TextureResSpr* pSprite, DWORD color, int32 nRenderStyle, RECT *rect)
{
	int i;
	if(nFrame >= pSprite->m_nFrameNum)
		return;

	// ÔÚÆÁÄ»¿Õ¼ä¼ì²âÍ¼ËØÊÇ·ñ¿É¼û£¬Èç²»¿É¼ûÔò·µ»Ø
	// ÕâÀï»¹¿ÉÒÔÓÅ»¯
	D3DXVECTOR3 vPos1, vPos2, vPos3, vPos4;
	D3DVIEWPORT9 viewportData = g_Device.GetViewport();
	D3DXVec3Project(&vPos1, &param.m_pos[0], &viewportData, &m_matProj, &m_matView, NULL);
	D3DXVec3Project(&vPos2, &param.m_pos[1], &viewportData, &m_matProj, &m_matView, NULL);
	D3DXVec3Project(&vPos3, &param.m_pos[2], &viewportData, &m_matProj, &m_matView, NULL);
	D3DXVec3Project(&vPos4, &param.m_pos[3], &viewportData, &m_matProj, &m_matView, NULL);

	if(vPos2.x < 0 || vPos1.x > g_nScreenWidth || vPos3.x < 0 || vPos4.x > g_nScreenWidth)
		return;
	if(vPos3.y < 0 || vPos1.y > g_nScreenHeight || vPos4.y < 0 || vPos2.y > g_nScreenHeight)
		return;

	D3DXVECTOR3 v11, v22, v1, v2, v3, v4;
	VERTEX3D vbSrc[4];
	int nStripLen[4];

	VERTEX3D* pvb = NULL;
	if(FAILED(m_pVB3D->Lock( 0, 16*sizeof(VERTEX3D), (void**)&pvb, 0 )))
		return;

	bool bDraw[4];
	if(nRenderStyle != IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST)
	{
		// ²»Æ«É«£¬½«ÑÕÉ«¸ÄÎª°×
		color = 0xffffffff;
	}

	// ¸ù¾ÝÌùÍ¼ÊýÄ¿°Ñ¾ØÐÎ²ð·Ö³É¶à¸öÐ¡¾ØÐÎ£¬¼ÆËã×ø±ê¼°ÎÆÀí
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		bDraw[i] = true;
		float fU1 = 0.0f, fV1 = 0.0f, fU2, fV2, fu2, fv2;
		fU2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth / 
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		fV2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
				(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		// Èç¹ûÖ»»­Í¼ËØµÄÒ»²¿·Ö£¬ÔòÐèÒªµ÷ÕûÎÆÀí×ø±ê
		if(rect)
		{
			float fRcX1, fRcY1, fRcX2, fRcY2;
			float fFrameX1 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX;
			float fFrameY1 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY;
			float fFrameX2 = (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth);
			float fFrameY2 = (float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight);
			fRcX1 = (float)rect->left, fRcY1 = (float)rect->top;
			fRcX2 = (float)rect->right, fRcY2 = (float)rect->bottom;
			if(fRcX1 > fFrameX2	|| fRcX2 < fFrameX1	|| fRcY1 > fFrameY2	|| fRcY2 < fFrameY1)
			{
				// Èç¹ûÕâ¸öÃæÆ¬µÄ¼ô²Ê¾ØÐÎÔÚÍ¼ËØ¾ØÐÎÖ®ÍâÔò²»»­
				bDraw[i] = false;
				continue;
			}
			if(fRcX1 < fFrameX1) fRcX1 = fFrameX1;
			if(fRcY1 < fFrameY1) fRcY1 = fFrameY1;
			if(fRcX2 > fFrameX2) fRcX2 = fFrameX2;
			if(fRcY2 > fFrameY2) fRcY2 = fFrameY2;

			fu2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth / 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
			fv2 = (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight /
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;
			fU1 = ChaZhi(0, fu2, fFrameX1, fFrameX2, fRcX1);
			fV1 = ChaZhi(0, fv2, fFrameY1, fFrameY2, fRcY1);
			fU2 = ChaZhi(0, fu2, fFrameX1, fFrameX2, fRcX2);
			fV2 = ChaZhi(0, fv2, fFrameY1, fFrameY2, fRcY2);
		}

		float ft1,ft2;
		ft1 = 0.5f / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		ft2 = 0.5f / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		v11 = ChaZhi(param.m_pos[0], param.m_pos[1], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		v22 = ChaZhi(param.m_pos[3], param.m_pos[2], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX);
		v1 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		v4 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		v11 = ChaZhi(param.m_pos[0], param.m_pos[1], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		v22 = ChaZhi(param.m_pos[3], param.m_pos[2], 0, (float)pSprite->m_pFrameInfo[nFrame].nWidth, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth));
		v2 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight,
					(float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY);
		v3 = ChaZhi(v11, v22, 0, (float)pSprite->m_pFrameInfo[nFrame].nHeight, 
					(float)(pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight));

		vbSrc[0].position = v1;
		vbSrc[0].color    = color;
		vbSrc[0].tu       = fU1+ft1;
		vbSrc[0].tv       = fV1+ft2;

		vbSrc[1].position = v2;
		vbSrc[1].color    = color;
		vbSrc[1].tu       = fU2-ft1;
		vbSrc[1].tv       = fV1+ft2;

		vbSrc[2].position = v3;
		vbSrc[2].color    = color;
		vbSrc[2].tu       = fU2-ft1;
		vbSrc[2].tv       = fV2-ft2;

		vbSrc[3].position = v4;
		vbSrc[3].color    = color;
		vbSrc[3].tu       = fU1+ft1;
		vbSrc[3].tv       = fV2-ft2;

		// Éú³ÉÍø¸ñ£¬¼ÆËãÁÁ¶È
		nStripLen[i] = BuildMesh3D(vbSrc, pvb, VERTEX_BUFFER_SIZE, color);
		pvb += nStripLen[i];
	}

	m_pVB3D->Unlock();

	// »æÖÆ¶à±ßÐÎ
	int nBase = 0;
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE4X );
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);
		if(!pTex || !bDraw[i])
			continue;
		PD3DDEVICE->SetTexture( 0, pTex );
		
		if( nRenderStyle == IMAGE_RENDER_STYLE_BORDER )
		{
			// Ñ¡ÖÐ¼ÓÁÁÐ§¹û
			PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADD );
			PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, nBase, nStripLen[i] - 2 );
		}
		else
			PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, nBase, nStripLen[i] - 2 );	

		nBase += nStripLen[i];
	}
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	PD3DDEVICE->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	PD3DDEVICE->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
}

#define MESH_GRID_SIZE	32.0f
uint32 KRepresentShell3::BuildMesh3D(VERTEX3D *pvbSrc, VERTEX3D* pvbDes, uint32 nNumDes, DWORD color)
{
	uint32 nCount = 0;

	if(pvbSrc[0].position.z == pvbSrc[2].position.z)
	{
		VERTEX3D vt1 = pvbSrc[0];						// É¨ÃèµÄÇ°Ò»ÐÐ¶¥µã
		VERTEX3D vt2 = pvbSrc[0];						// É¨ÃèµÄºóÒ»ÐÐ¶¥µã
		float ftuAdd, ftvAdd;
		bool bAllDone = false;
		
		ftuAdd = (pvbSrc[1].tu - pvbSrc[0].tu) * MESH_GRID_SIZE / (pvbSrc[1].position.x - pvbSrc[0].position.x);
		ftvAdd = (pvbSrc[2].tv - pvbSrc[0].tv) * MESH_GRID_SIZE / (pvbSrc[2].position.y - pvbSrc[0].position.y);

		while(1)
		{
			vt1 = vt2;
			// ¼ÆËãºóÒ»ÐÐÆðµã
			vt2.position.y += MESH_GRID_SIZE;
			vt2.tv += ftvAdd;
			if(vt2.position.y >= pvbSrc[2].position.y)
			{
				vt2.position.y = pvbSrc[2].position.y;
				vt2.tv = pvbSrc[2].tv;
				bAllDone = true;
			}
			// ´Ó×óµ½ÓÒ´´½¨Ò»ÐÐÈý½ÇÐÎÌõ´ø
			while(1)
			{
				SetPoint3dLighting(pvbDes[nCount++], vt2, color);
				SetPoint3dLighting(pvbDes[nCount++], vt1, color);
				vt1.position.x += MESH_GRID_SIZE;
				vt2.position.x += MESH_GRID_SIZE;
				vt1.tu += ftuAdd;
				vt2.tu += ftuAdd;
				if(vt1.position.x >= pvbSrc[1].position.x)
				{
					vt1.position.x = pvbSrc[1].position.x;
					vt2.position.x = pvbSrc[1].position.x;
					vt1.tu = pvbSrc[1].tu;
					vt2.tu = pvbSrc[1].tu;
					SetPoint3dLighting(pvbDes[nCount++], vt2, color);
					SetPoint3dLighting(pvbDes[nCount++], vt1, color);
					break;
				}
			}
			if(bAllDone)
				break;
			
			vt1 = vt2;
			// ¼ÆËãºóÒ»ÐÐÆðµã
			vt2.position.y += MESH_GRID_SIZE;
			vt2.tv += ftvAdd;
			if(vt2.position.y >= pvbSrc[2].position.y)
			{
				vt2.position.y = pvbSrc[2].position.y;
				vt2.tv = pvbSrc[2].tv;
				bAllDone = true;
			}
			// ´ÓÓÒµ½×ó´´½¨Ò»ÐÐÈý½ÇÐÎÌõ´ø
			while(1)
			{
				SetPoint3dLighting(pvbDes[nCount++], vt1, color);
				SetPoint3dLighting(pvbDes[nCount++], vt2, color);
				vt1.position.x -= MESH_GRID_SIZE;
				vt2.position.x -= MESH_GRID_SIZE;
				vt1.tu -= ftuAdd;
				vt2.tu -= ftuAdd;
				if(vt1.position.x <= pvbSrc[0].position.x)
				{
					vt1.position.x = pvbSrc[0].position.x;
					vt2.position.x = pvbSrc[0].position.x;
					vt1.tu = pvbSrc[0].tu;
					vt2.tu = pvbSrc[0].tu;
					SetPoint3dLighting(pvbDes[nCount++], vt1, color);
					SetPoint3dLighting(pvbDes[nCount++], vt2, color);
					break;
				}
			}
			if(bAllDone)
				break;
		}
	}
	else
	{
		VERTEX3D vt1 = pvbSrc[0];						// É¨ÃèµÄÇ°Ò»ÐÐ¶¥µã
		VERTEX3D vt2 = pvbSrc[0];						// É¨ÃèµÄºóÒ»ÐÐ¶¥µã
		float ftuAdd, ftvAdd;
		D3DXVECTOR3 fvAdd;
		float fEndZ1, fEndZ2;
		bool bAllDone = false;
		
		ftuAdd = (pvbSrc[1].tu - pvbSrc[0].tu) * (MESH_GRID_SIZE / (pvbSrc[1].position.x - pvbSrc[0].position.x));
		ftvAdd = (pvbSrc[3].tv - pvbSrc[0].tv) * (MESH_GRID_SIZE / (pvbSrc[0].position.z - pvbSrc[3].position.z));
		fvAdd = (pvbSrc[1].position - pvbSrc[0].position) * (MESH_GRID_SIZE / (pvbSrc[1].position.x - pvbSrc[0].position.x));

		while(1)
		{
			vt1 = vt2;
			// ¼ÆËãºóÒ»ÐÐÆðµã
			vt2.position += fvAdd;
			vt2.tu += ftuAdd;
			if(abs((int)(vt2.position.x - pvbSrc[0].position.x)) >=
				abs((int)(pvbSrc[1].position.x - pvbSrc[0].position.x)))
			{
				vt2 = pvbSrc[1];
				bAllDone = true;
			}
			fEndZ1 = vt1.position.z - (pvbSrc[0].position.z - pvbSrc[3].position.z);
			fEndZ2 = vt2.position.z - (pvbSrc[0].position.z - pvbSrc[3].position.z);
			// ´ÓÉÏµ½ÏÂ´´½¨Ò»ÐÐÈý½ÇÐÎÌõ´ø
			while(1)
			{
				SetPoint3dLighting(pvbDes[nCount++], vt1, color);
				SetPoint3dLighting(pvbDes[nCount++], vt2, color);
				vt1.position.z -= MESH_GRID_SIZE;
				vt2.position.z -= MESH_GRID_SIZE;
				vt1.tv += ftvAdd;
				vt2.tv += ftvAdd;
				if(vt1.position.z <= fEndZ1)
				{
					vt1.position.z = fEndZ1;
					vt2.position.z = fEndZ2;
					vt1.tv = pvbSrc[3].tv;
					vt2.tv = pvbSrc[3].tv;
					SetPoint3dLighting(pvbDes[nCount++], vt1, color);
					SetPoint3dLighting(pvbDes[nCount++], vt2, color);
					break;
				}
			}
			if(bAllDone)
				break;
			
			vt1 = vt2;
			// ¼ÆËãºóÒ»ÐÐÆðµã
			vt2.position += fvAdd;
			vt2.tu += ftuAdd;
			if(abs((int)(vt2.position.x - pvbSrc[3].position.x)) >=
				abs((int)(pvbSrc[2].position.x - pvbSrc[3].position.x)))
			{
				vt2 = pvbSrc[2];
				bAllDone = true;
			}
			fEndZ1 = vt1.position.z + (pvbSrc[0].position.z - pvbSrc[3].position.z);
			fEndZ2 = vt2.position.z + (pvbSrc[0].position.z - pvbSrc[3].position.z);
			// ´ÓÓÒµ½×ó´´½¨Ò»ÐÐÈý½ÇÐÎÌõ´ø
			while(1)
			{
				SetPoint3dLighting(pvbDes[nCount++], vt2, color);
				SetPoint3dLighting(pvbDes[nCount++], vt1, color);
				vt1.position.z += MESH_GRID_SIZE;
				vt2.position.z += MESH_GRID_SIZE;
				vt1.tv -= ftvAdd;
				vt2.tv -= ftvAdd;
				if(vt1.position.z >= fEndZ1)
				{
					vt1.position.z = fEndZ1;
					vt2.position.z = fEndZ2;
					vt1.tv = pvbSrc[0].tv;
					vt2.tv = pvbSrc[0].tv;
					SetPoint3dLighting(pvbDes[nCount++], vt2, color);
					SetPoint3dLighting(pvbDes[nCount++], vt1, color);
					break;
				}
			}
			if(bAllDone)
				break;
		}
	}

	return nCount;
}


unsigned int KRepresentShell3::GetPoint3dLightingLerp(D3DXVECTOR3& v)
{
	DWORD dwLightColor;
	D3DXVECTOR3 vColor[4];
	KRPosition3 pt[4];
	float fOffX, fOffY;
	fOffX = (((int)v.x) % LIGHTING_GRID_SIZEX) / (float)LIGHTING_GRID_SIZEX;
	fOffY = (((int)v.y) % LIGHTING_GRID_SIZEY) / (float)LIGHTING_GRID_SIZEY;
	pt[0].nX = (int)v.x;
	pt[0].nY = (int)v.y;
	pt[0].nZ = (int)v.z;
	pt[1].nX = pt[0].nX + LIGHTING_GRID_SIZEX;
	pt[1].nY = pt[0].nY;
	pt[1].nZ = pt[0].nZ;
	pt[2].nX = pt[0].nX + LIGHTING_GRID_SIZEX;
	pt[2].nY = pt[0].nY + LIGHTING_GRID_SIZEY;
	pt[2].nZ = pt[0].nZ;
	pt[3].nX = pt[0].nX;
	pt[3].nY = pt[0].nY + LIGHTING_GRID_SIZEY;
	pt[3].nZ = pt[0].nZ;

	for(int i=0; i<4; i++)
	{
		if(pt[i].nX < m_nLightingAreaX || pt[i].nY < m_nLightingAreaY ||
					pt[i].nX >= m_nLightingAreaX + 1536 || pt[i].nY >= m_nLightingAreaY + 3072)
			dwLightColor = 0xff404040;
		else
		{
			pt[i].nX = (pt[i].nX - m_nLightingAreaX) / LIGHTING_GRID_SIZEX;
			pt[i].nY = (pt[i].nY - m_nLightingAreaY) / LIGHTING_GRID_SIZEY;
			dwLightColor = pLightingArray[pt[i].nY * LIGHTING_GRID_WIDTH + pt[i].nX];
		}
		vColor[i].x = (float)((dwLightColor & 0x00ff0000) >> 16);
		vColor[i].y = (float)((dwLightColor & 0x0000ff00) >> 8);
		vColor[i].z = (float)(dwLightColor & 0x000000ff);
	}

	vColor[0] = vColor[0] * fOffX + vColor[1] * (1.0f - fOffX);
	vColor[3] = vColor[3] * fOffX + vColor[2] * (1.0f - fOffX);
	vColor[0] = vColor[0] * fOffY + vColor[3] * (1.0f - fOffY);
	dwLightColor = 0xff000000 | (((DWORD) vColor[0].x)<<16) | (((DWORD) vColor[0].y)<<8) | ((DWORD) vColor[0].z);

	return dwLightColor;
}

void KRepresentShell3::RIO_CopyBitmap16ToBuffer(TextureResBmp* pBitmap, TextureResBmp* pDestBitmap, 
												int32 nX, int32 nY)
{
	return;
/*	int   nDestWidth  = pDestBitmap->GetWidth();
	int   nDestHeight = pDestBitmap->GetHeight();
	int   nSrcWidth  = pBitmap->GetWidth();
	int   nSrcHeight = pBitmap->GetHeight();

	if(!pBitmap->m_TextureInfo.pTexture)
	{
		assert(pBitmap->m_TextureInfo.pTexture);
		return;
	}

	if(!pDestBitmap->m_TextureInfo.pTexture)
	{
		assert(pDestBitmap->m_TextureInfo.pTexture);
		return;
	}

	IDirect3DSurface8* pSrcSurface, *pDesSurface;

	if ( pBitmap->m_TextureInfo.pTexture->GetSurfaceLevel( 0, &pSrcSurface) != D3D_OK )
		return;
	if ( pDestBitmap->m_TextureInfo.pTexture->GetSurfaceLevel( 0, &pDesSurface) != D3D_OK )
	{
		pSrcSurface->Release();
		return;
	}

	RECT SrcRect;
	POINT DesPoint;

	SrcRect.left = SrcRect.top = 0;
	DesPoint.x = nX;
	DesPoint.y = nY;


	if(nSrcWidth <= nDestWidth - nX)
		SrcRect.right = nSrcWidth;
	else
		SrcRect.right = nDestWidth - nX;

	if(nSrcHeight <= nDestHeight - nY)
		SrcRect.bottom = nSrcHeight;
	else
		SrcRect.bottom = nDestHeight - nY;

	PD3DDEVICE->CopyRects( pSrcSurface, &SrcRect, 1, pDesSurface, &DesPoint);
	pSrcSurface->Release();
	pDesSurface->Release();*/
}

inline WORD C32BitTo16Bit555(BYTE* pSrc)
{
	return (((WORD)(*pSrc))>>3) + ((((WORD)(*(pSrc+1)))<<2)&MASK_GREEN_555) + ((((WORD)(*(pSrc+2)))<<7)&MASK_RED_555);
}

inline void C32BitTo16Bit565(BYTE* pSrc, WORD *pDesColor)
{
	WORD rs, gs, bs, rd, gd, bd, as, ad;
	rs = (WORD)(*(pSrc+2));
	gs = (WORD)(*(pSrc+1));
	bs = (WORD)(*pSrc);
	as = (WORD)(*(pSrc+3));
	ad = 255 - as;
	rd = (*pDesColor & MASK_RED_565) >> 8;
	gd = (*pDesColor & MASK_GREEN_565) >> 3;
	bd = (*pDesColor & MASK_BLUE_565) << 3;

	*pDesColor = ((rs * as) & MASK_RED_565) + (((gs * as) >> 5) & MASK_GREEN_565) + ((bs * as) >> 11) +
					((rd * ad) & MASK_RED_565) + (((gd * ad) >> 5) & MASK_GREEN_565) + ((bd * ad) >> 11);

}

void KRepresentShell3::RIO_CopySprToBufferAlpha(TextureResSpr* pSprite, int32 nFrame, 
									TextureResBmp* pDestBitmap, int32 nX, int32 nY)
{
	if(nFrame >= pSprite->m_nFrameNum)
		return;

	int nx, ny, i;
	int32 nTargetWidth, nTargetHeight;

	VERTEX2D* pvb;
	if(FAILED(m_pVB2D->Lock( 0, 16*sizeof(VERTEX2D), (void**)&pvb, D3DLOCK_DISCARD )))
		return;

	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		int32 nDesWidth  = pDestBitmap->GetWidth();
		int32 nDesHeight = pDestBitmap->GetHeight();
		int32 nSrcWidth  = pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameWidth;
		int32 nSrcHeight = pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameHeight;
		nx = nX + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameX;
		ny = nY + pSprite->m_pFrameInfo[nFrame].texInfo[i].nFrameY;

		if(nx >= 0)
			nDesWidth -= nx;
		else
			nSrcWidth += nx;
		if(ny >= 0)
			nDesHeight -= ny;
		else
			nSrcHeight += ny;

		if(nSrcWidth <= nDesWidth)
			nTargetWidth = nSrcWidth;
		else
			nTargetWidth = nDesWidth;
		if(nSrcHeight <= nDesHeight)
			nTargetHeight = nSrcHeight;
		else
			nTargetHeight = nDesHeight;

		float fX1, fY1, fX2, fY2;
		fX1 = fY1 = 0.0f;
		if(nx >= 0)
			fX1 = (float)nx;
		if(ny >= 0)
			fY1 = (float)ny;
		fX2 = fX1 + (float)nTargetWidth;
		fY2 = fY1 + (float)nTargetHeight;

		float fU1, fV1, fU2, fV2;
		if(nx >= 0)
			fU1 = 0.0f;
		else
			fU1 = -(float)nx / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		if(ny >= 0)
			fV1 = 0.0f;
		else
			fV1 = -(float)ny / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;
		fU2 = fU1 + (float)nTargetWidth / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nWidth;
		fV2 = fV1 + (float)nTargetHeight / (float)pSprite->m_pFrameInfo[nFrame].texInfo[i].nHeight;

		fX1 -= 0.5f, fY1 -= 0.5f, fX2 -= 0.5f, fY2 -= 0.5f;

		int nBase = i*4;
		pvb[nBase+0].position = D3DXVECTOR4( fX1,fY1, 0, 1 );
		pvb[nBase+0].color    = 0xffffffff;
		pvb[nBase+0].tu       = fU1;
		pvb[nBase+0].tv       = fV1;

		pvb[nBase+1].position = D3DXVECTOR4( fX2,fY1, 0, 1 );
		pvb[nBase+1].color    = 0xffffffff;
		pvb[nBase+1].tu       = fU2;
		pvb[nBase+1].tv       = fV1;

		pvb[nBase+2].position = D3DXVECTOR4( fX1,fY2, 0, 1 );
		pvb[nBase+2].color    = 0xffffffff;
		pvb[nBase+2].tu       = fU1;
		pvb[nBase+2].tv       = fV2;

		pvb[nBase+3].position = D3DXVECTOR4( fX2,fY2, 0, 1 );
		pvb[nBase+3].color    = 0xffffffff;
		pvb[nBase+3].tu       = fU2;
		pvb[nBase+3].tv       = fV2;
	}

	m_pVB2D->Unlock();

	// »æÖÆ¶à±ßÐÎ
	for(i=0; i<pSprite->m_pFrameInfo[nFrame].nTexNum; i++)
	{
		LPDIRECT3DTEXTURE9 pTex = pSprite->GetTexture(nFrame, i);
		if(!pTex)
		{
			g_uRep3FxTexNull++;	// [FX 07/09]
			return;
		}
		PD3DDEVICE->SetTexture( 0, pTex );
		PD3DDEVICE->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );
	}
}

void KRepresentShell3::SetLightInfo(int nX, int nY, unsigned int *pLighting)
{
	if(!pLighting)
	{
		m_bDoLighting = false;
		return;
	}

	m_nLightingAreaX = nX;
	m_nLightingAreaY = nY;
	memcpy(pLightingArray, pLighting, LIGHTING_GRID_WIDTH * LIGHTING_GRID_HEIGHT * sizeof(DWORD));
}

bool KRepresentShell3::SaveScreenToFile(const char* pszName, ScreenFileType eType, unsigned int nQuality)
{
	if(!pszName || !pszName[0])
		return 0;

	if(m_bDeviceLost)
		return false;

	int nPicWidth, nPicHeight, nDesktopWidth, nDesktopHeight, nPicOffX, nPicOffY;
	if(g_bRunWindowed)
	{
		RECT rect;
		POINT ptLT, ptRB;
		GetClientRect(g_hWnd, &rect);
		ptLT.x = rect.left, ptLT.y = rect.top;
		ptRB.x = rect.right, ptRB.y = rect.bottom;
		ClientToScreen(g_hWnd, &ptLT);
		ClientToScreen(g_hWnd, &ptRB);

		D3DDISPLAYMODE mode;
		PD3DDEVICE->GetDisplayMode(D3DADAPTER_DEFAULT, &mode);
		nDesktopWidth = mode.Width;
		nDesktopHeight = mode.Height;

		// Èç¹û´°¿Ú¿Í»§Çø³¬³öÆÁÄ»Ôò·µ»Ø
		if(ptLT.x >= nDesktopWidth || ptLT.y >= nDesktopHeight || ptRB.x <= 0 || ptRB.y <= 0)
			return false;
		if(ptLT.x < 0)
			ptLT.x = 0;
		if(ptLT.y < 0)
			ptLT.y = 0;
		if(ptRB.x > nDesktopWidth)
			ptRB.x = nDesktopWidth - 1;
		if(ptRB.y > nDesktopHeight)
			ptRB.y = nDesktopHeight - 1;

		nPicOffX = ptLT.x;
		nPicOffY = ptLT.y;
		nPicWidth = ptRB.x - ptLT.x;
		nPicHeight = ptRB.y - ptLT.y;
	}
	else
	{
		nPicOffX = 0;
		nPicOffY = 0;
		nDesktopWidth = nPicWidth = g_nScreenWidth;
		nDesktopHeight = nPicHeight = g_nScreenHeight;
	}

	BYTE *pData;
	// ´´½¨ÓÃÓÚ´æ·Å½ØÍ¼µÄSurface
	IDirect3DSurface9* pSurface = NULL;
	if(FAILED(PD3DDEVICE->CreateOffscreenPlainSurface(nDesktopWidth, nDesktopHeight, 
												D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM , &pSurface, NULL)))
		goto error;

	// »ñÈ¡ÆÁÄ»½ØÍ¼
	if(FAILED(PD3DDEVICE->GetFrontBufferData(0, pSurface)))
    	goto error;

	D3DLOCKED_RECT lockedRect;
	if(FAILED(pSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY)))
	{
		pSurface->Release();
		goto error;
	}
	
	pData = ((BYTE*)lockedRect.pBits) + nPicOffY * nDesktopWidth * 4 + nPicOffX * 4;
	BOOL bRet;
	if(eType == SCRFILETYPE_BMP)
		// ±£´æ24Î»bmpÎÄ¼þ
		bRet = KBmpFile24::SaveBuffer32((char*)pszName, pData, nDesktopWidth*4, nPicWidth, nPicHeight);
	else
		// ±£´æ24Î»jpgÎÄ¼þ
		bRet = SaveBufferToJpgFile32((char*)pszName, pData, nDesktopWidth*4, nPicWidth, nPicHeight, nQuality);
	if(!bRet)
	{
		pSurface->UnlockRect();
		goto error;
	}

	pSurface->UnlockRect();
	SAFE_RELEASE(pSurface);
	return true;

error:
	SAFE_RELEASE(pSurface);
	return false;
}

void KRepresentShell3::SetGamma(int nGamma)
{
	if(nGamma < 0 || nGamma > 100)
		return;

	// °ÑnGamma±äÎª-100µ½100
	nGamma = nGamma * 2 - 100;

	D3DGAMMARAMP ramp;
	DWORD dwGama;
	int i;
	if(nGamma >= 0)
	{
		for(i=0; i<256; i++)
		{
			dwGama = i * 256 + i * nGamma;
			if(dwGama <= 65535)
				ramp.red[i] = ramp.green[i] = ramp.blue[i] = (WORD)dwGama;
			else
				ramp.red[i] = ramp.green[i] = ramp.blue[i] = 65535;
		}
	}
	else
	{
		for(i=0; i<256; i++)
		{
			dwGama = i * 256 + i * nGamma;
			if(dwGama <= 65535)
				ramp.red[i] = ramp.green[i] = ramp.blue[i] = (WORD)dwGama;
			else
				ramp.red[i] = ramp.green[i] = ramp.blue[i] = 0;
		}
	}
	PD3DDEVICE->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &ramp);
}

void KRepresentShell3::setZoomFactor(float zoomFactorDelta) { //set zoom factor
	if (zoomFactorDelta == 99) { //reset
		g_fZoomFactor = 1;
	}
	else if (g_fZoomFactor + zoomFactorDelta <= 5.10 && g_fZoomFactor + zoomFactorDelta >= 0.15) {
		g_fZoomFactor += zoomFactorDelta;
	}
	if (g_renderModel == RenderModel3DOrtho)
		SetUpProjectionMatrix();
	else {
		SetUpProjectionMatrix();

		m_vCamera.x = m_vCamera1.x;
		m_vCamera.y = m_vCamera1.y + 4000.0f * g_fZoomFactor;
		m_vCamera.z = (float)(m_vCamera1.z + 4000.0f * 0.58 * g_fZoomFactor);
		D3DXMatrixLookAtLH(&m_matView, &m_vCamera,
			&D3DXVECTOR3(m_vCamera1.x, m_vCamera1.y, m_vCamera1.z), &D3DXVECTOR3(0, 0, 1));
		PD3DDEVICE->SetTransform(D3DTS_VIEW, &m_matView);
		D3DXMatrixInverse(&m_matViewInverse, NULL, &m_matView);
	}
}