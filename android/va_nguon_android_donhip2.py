# -*- coding: utf-8 -*-
#
# [FPS 12/09] + [THONGTIN 12/09] - chu 12/09: "co thanh chinh FPS o cai dat va hien thong tin FPS hien tai CPU - GPU - pin o goc
# phai man hinh dang hien thi nhu nhung game mobile khac chu khong phai che do PerfHud". CHI JX_ANDROID.
#   UiOptions.h / .cpp        : thanh cuon [Fps] trong cua so Cai dat (muc 0 tu dong theo man hinh, 1..5 = 30/45/60/90/120),
#                               luu UserData\UiCommon.ini [Options] FpsMuc, ap khi mo game (LoadSetting) va khi keo.
#                               Hang "Bo phim tat" (khong dung tren dien thoai) nhuong cho cho thanh FPS.
#   JxPerfHudAndroid.cpp      : JxNhip_DatMuc / JxNhip_ChuMuc / JxNhip_VeNen; thong tin goc phai "60 FPS | CPU | GPU | Pin | ms"
#                               ([Client] ThongTinGoc=0 de tat; keo duoc trong Sua giao dien, khoa ThongTinGoc).
# Di kem: android/sinh_uioptions_fps.py (ini lop ghi de co muc [Fps]). Chay SAU va_nguon_android_donhip1.py.
# Nguon co byte cao (TCVN3): doc/ghi latin-1, so byte cao truoc/sau bang nhau, moi moc khop dung 1 cho.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[FPS 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def cao(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def khoi(txt):
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


def sau(L, sub, them, ten, loc=None):
    i = mot(L, sub, ten, loc)
    return L[:i + 1] + khoi(them) + L[i + 1:]


def va(p, ham, dau=DAU):
    s = doc(p)
    if dau in s:
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


# ================================================================ UiOptions.h
def va_h(L):
    L = sau(L, "void				SetBrightness(int);", r'''
#ifdef JX_ANDROID
	void				SetFpsMuc(int);		// [FPS 12/09] muc khung hinh/giay: 0 tu dong (theo man hinh), 1..5 = 30/45/60/90/120
#endif
''', "SetBrightness")
    L = sau(L, "KWndScrollBar	m_SoundValue;", r'''
#ifdef JX_ANDROID
	KWndScrollBar	m_FpsScroll;		// [FPS 12/09] thanh khung hinh/giay (ini muc [Fps], lop ghi de Android)
	int				m_nFpsMuc;
#endif
''', "m_SoundValue")
    return L


# ================================================================ UiOptions.cpp
def va_cpp(L):
    L = sau(L, "extern iCoreShell*	g_pCoreShell;", r'''
#ifdef JX_ANDROID
#include "../../../Represent/iRepresent/KRepresentUnit.h"	// [FPS 12/09] TEXT_IN_SINGLE_PLANE_COORD
// [FPS 12/09] Platform/JxPerfHudAndroid.cpp: thanh chinh khung hinh/giay trong Cai dat (chu: "co thanh chinh FPS o cai dat")
void JxNhip_DatMuc(int nMuc);							// ap muc: PaintFps + xin tan so man hinh
void JxNhip_ChuMuc(int nMuc, char* sz, int n);			// chu canh thanh: "Tu dong (120)" / "60"
void JxNhip_VeNen(int nX, int nY, int nRong, int nCao);	// nen mo (che nhan "Phim tat" ve san trong anh nen)
#endif
''', "extern g_pCoreShell")
    L = sau(L, "	m_nMusicValue = 100;", r'''
#ifdef JX_ANDROID
	m_nFpsMuc = 0;	// [FPS 12/09]
#endif
''', "ctor m_nMusicValue")
    L = sau(L, "	KWndImage::PaintWindow();", r'''
#ifdef JX_ANDROID
	if (m_FpsScroll.IsVisible() && g_pRepresentShell)
	{	// [FPS 12/09] nhan "FPS" (che nhan "Phim tat" ve san trong anh nen) + gia tri muc ben phai thanh
		int nX = 0, nY = 0, nRong = 0, nCao = 0;
		char sz[48];
		m_FpsScroll.GetPosition(&nX, &nY);
		m_FpsScroll.GetSize(&nRong, &nCao);
		JxNhip_VeNen(m_nAbsoluteLeft + 4, m_nAbsoluteTop + nY - 3, nX - 8, nCao + 6);
		g_pRepresentShell->OutputText(12, (char*)"FPS", 3, m_nAbsoluteLeft + 8, m_nAbsoluteTop + nY, 0xFFFEFFC3, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xFF000000);
		JxNhip_ChuMuc(m_nFpsMuc, sz, sizeof(sz));
		g_pRepresentShell->OutputText(12, sz, (int)strlen(sz), m_nAbsoluteLeft + nX + nRong + 4, m_nAbsoluteTop + nY, 0xFFFFE28A, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xFF000000);
	}
#endif
''', "PaintWindow")
    L = sau(L, "	AddChild(&m_SoundValue);", r'''
#ifdef JX_ANDROID
	AddChild(&m_FpsScroll);	// [FPS 12/09]
#endif
''', "AddChild m_SoundValue")
    L = sau(L, '	m_Scroll.Init(pIni, "Scroll");', r'''
#ifdef JX_ANDROID
	if (!m_FpsScroll.Init(pIni, "Fps"))	// [FPS 12/09] ini chua co muc [Fps] (thieu lop ghi de) -> an thanh
		m_FpsScroll.Hide();
	m_ShortcutSetView.Hide();	// [FPS 12/09] bo phim tat khong dung tren dien thoai; hang nay danh cho thanh FPS
#endif
''', "m_Scroll.Init")
    L = sau(L, "		SetBrightness(nPos);", r'''
#ifdef JX_ANDROID
	else if (pWnd == (KWndWindow*)&m_FpsScroll)
		SetFpsMuc(nPos);	// [FPS 12/09]
#endif
''', "OnScrollBarPosChanged Brightness")
    L = sau(L, "	int nSettingSet  = 0;", r'''
#ifdef JX_ANDROID
	int nFpsMuc = 0;	// [FPS 12/09]
#endif
''', "LoadSetting nSettingSet")
    L = sau(L, "		nSettingSet  = m_pSelf->m_nShortcutSet;", r'''
#ifdef JX_ANDROID
		nFpsMuc = m_pSelf->m_nFpsMuc;	// [FPS 12/09]
#endif
''', "LoadSetting tu m_pSelf")
    L = sau(L, '			pSetting->GetInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", 0, &nSettingSet);', r'''
#ifdef JX_ANDROID
			pSetting->GetInteger(OPTIONS_SAVE_SECTION, "FpsMuc", 0, &nFpsMuc);	// [FPS 12/09]
#endif
''', "LoadSetting doc ShortcutSet")
    i = mot(L, "		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_WEATHER, bOptionsEnable[OPTION_I_WEATHER]);", "ap WEATHER")
    if L[i + 1].strip() != "}":
        raise SystemExit("sau dong ap WEATHER khong phai '}': " + L[i + 1])
    L = L[:i + 2] + khoi(r'''
#ifdef JX_ANDROID
	if (bUpdateOption)
		JxNhip_DatMuc(nFpsMuc);	// [FPS 12/09] ap muc khung hinh nguoi choi da chon (luc mo game va moi lan nap lai)
#endif
''') + L[i + 2:]
    L = sau(L, "			m_pSelf->m_SoundValue.SetScrollPos(nSoundValue);", r'''
#ifdef JX_ANDROID
			m_pSelf->m_nFpsMuc = nFpsMuc;
			m_pSelf->m_FpsScroll.SetScrollPos(nFpsMuc);	// [FPS 12/09]
#endif
''', "LoadSetting SetScrollPos Sound")
    L = sau(L, '		pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", m_nShortcutSet);', r'''
#ifdef JX_ANDROID
		pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "FpsMuc", m_nFpsMuc);	// [FPS 12/09]
#endif
''', "StoreSetting")
    L = L + khoi(r'''

#ifdef JX_ANDROID
// [FPS 12/09] thanh khung hinh/giay: doi muc -> ap ngay (PaintFps + xin tan so man hinh); luu khi dong cua so (StoreSetting)
void KUiOptions::SetFpsMuc(int n)
{
	if (m_nFpsMuc != n)
	{
		m_nFpsMuc = n;
		JxNhip_DatMuc(n);
	}
}
#endif
''')
    return L


# ================================================================ JxPerfHudAndroid.cpp
HUD_KHOI = r'''
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
'''


def va_hud(L):
    L = sau(L, "#include <unistd.h>", r'''
#include "../Ui/Elem/UiToaDo.h"	// [THONGTIN 12/09] thong tin goc phai keo duoc trong Sua giao dien
''', "include unistd")
    i = mot(L, "void PerfHud_SetEnable(int nOn)", "PerfHud_SetEnable")
    L = L[:i] + khoi(HUD_KHOI) + [""] + L[i:]
    L = sau(L, "	DoNhip_VeNhan();	// [DONHIP 12/09]", r'''
	ThongTin_Ve(nPaintFps, dwPing);	// [THONGTIN 12/09] FPS | CPU | GPU | Pin | ping o goc phai (khong phu thuoc PerfHud)
''', "goi DoNhip_VeNhan")
    return L


va(os.path.join("Sources", "S3Client", "Ui", "UiCase", "UiOptions.h"), va_h)
va(os.path.join("Sources", "S3Client", "Ui", "UiCase", "UiOptions.cpp"), va_cpp)
va(os.path.join("Sources", "S3Client", "Platform", "JxPerfHudAndroid.cpp"), va_hud)
