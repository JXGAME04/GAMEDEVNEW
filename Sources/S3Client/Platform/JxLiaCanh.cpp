//---------------------------------------------------------------------------
// [LIA 13/09] Lia canh (pan) cho ban mobile - xem JxLiaCanh.h.
//
// Trang thai: KHONG -> KEO (ngon dang keo) -> CHO_VE (da nha, dem LiaChoVeMs) -> VE (troi ve 0) -> KHONG.
// Moi lan doi do lech: g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, lechX, lechY) sau khi da
// bat GSMOI_SCENE_FOLLOW_WITH_MAP mot lan. Ket thuc: FOLLOW_WITH_MAP=0 roi FOCUS_OFFSET=0 (dung thu tu cua
// KUiMiniMap::MapMoveBack). Do lech giu bang float de troi ve muot; gui xuong Core bang int.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_MOBILE
#include "JxLiaCanh.h"
#include "JxCanDieuKhien.h"		// JxCan_TrongVung / JxCan_DangCam / JxKyNang_TrungNut
#include "../Ui/Elem/UiToaDo.h"	// UiToaDo_DangSua
#include "../Ui/UiCase/UiToolsControlBar.h"	// thanh cong cu = dau hieu da vao the gioi (nhu JxCanDieuKhien)
#include "../../Core/src/coreshell.h"
#include "../../Core/src/GameDataDef.h"	// KSceneMapInfo
#include "KDebug.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern iCoreShell*	g_pCoreShell;
extern int			SCREEN_WIDTH;
extern int			SCREEN_HEIGHT;
extern "C" int		JxUi_CoGiaoDienTaiDiem(int x, int y);	// Wnds.cpp

// --- cai dat ([Cham] trong config.ini, doc mot lan) -----------------------------
static int	s_nDaDoc = 0;
static int	s_nBat = 1;			// LiaCanh: 0 = tat han
static int	s_nXaNgang = 60;	// LiaXaNgang: lech toi da ngang, % be rong khung ve
static int	s_nXaDoc = 60;		// LiaXaDoc: lech toi da doc, % chieu cao khung ve
static int	s_nNguong = 3;		// LiaNguong: tay di it hon bao nhieu px thi chua tinh (rung tay)
static int	s_nChoVeMs = 1000;	// LiaChoVeMs: nha tay bao lau thi bat dau ve (game 3D: 1 s)
static int	s_nVeTocDo = 700;	// LiaVeTocDo: toc do ve toi thieu, px khung ve / giay (game 3D: 40 do/s = nhe nhang)
static int	s_nVeEm = 3;		// LiaVeEm: moi giay ve bao nhieu lan phan con lai (em cuoi; 0 = di deu)
static int	s_nVeKhiDi = 1;		// LiaVeKhiDi: 1 = nhan vat buoc di / cam can -> ve ngay, khong cho
static int	s_nNhatKy = 0;		// LiaNhatKy: 1 = ghi [LIA] ra nhat ky
static int	s_nThuDx = 0, s_nThuDy = 0;	// LiaThu=dx,dy: GO LOI - 3 s sau khi vao the gioi tu keo (dx,dy) roi tu ve
// [ZOOM 13/09]
static int	s_nZoomBat = 1;		// ZoomCanh: 0 = tat chum hai ngon
static int	s_nZoomToiDa = 150;	// ZoomToiDa: % nhin rong toi da (150 = rong gap 1,5 lan moi chieu)
static int	s_nZoomBuoc = 5;	// ZoomBuoc: nac zoom, %
static int	s_nZoomMacDinh = 100;	// ZoomMacDinh: zoom luc vao game, %
static int	s_nZoomThu = 0;		// ZoomThu: GO LOI - % zoom tu dat 3 s sau khi vao the gioi (may ao khong co hai ngon)

// --- trang thai --------------------------------------------------------------
enum { LIA_KHONG = 0, LIA_KEO, LIA_CHO_VE, LIA_VE };
static int			s_nTrangThai = LIA_KHONG;
static float		s_fLechX = 0.f, s_fLechY = 0.f;	// do lech tieu diem, don vi THE GIOI (X = px, Y = 2 x px)
static int			s_nLechX = 0, s_nLechY = 0;		// ban int da gui xuong Core
static int			s_nTayX = 0, s_nTayY = 0;		// cho ngon dang o (khung ve)
static unsigned int	s_uNhaLuc = 0;		// luc nha ngon
static unsigned int	s_uNhipTruoc = 0;	// luc nhip truoc (tinh dt)
static int			s_nGocX = 0, s_nGocY = 0;		// tieu diem goc (vi tri nhan vat) o nhip truoc
static int			s_nCoGoc = 0;
static int			s_nDaBatCo = 0;		// da gui FOLLOW_WITH_MAP=1 chua
static unsigned int	s_uVaoGameLuc = 0;	// LiaThu: luc thay thanh cong cu
static int			s_nThuXong = 0;
static unsigned int	s_uKeoDem = 0;		// thong ke: so lan keo trong dot nay
// [ZOOM 13/09]
static int			s_nZoom = 100;		// % dang ap (100 = 1:1)
static int			s_nZoomDaAp = -1;	// % da gui cho Represent3 / Core (-1 = chua)
static int			s_nChum = 0;		// 1 = dang chum
static float		s_fChumD0 = 1.f;	// khoang cach hai ngon luc bat dau chum
static int			s_nChumZoom0 = 100;	// zoom luc bat dau chum
static int			s_nZoomThuXong = 0;

static void DocCaiDat()
{
	if (s_nDaDoc)
		return;
	s_nDaDoc = 1;
	char szCfg[MAX_PATH] = { 0 };
	char szThu[64] = "0,0";
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	s_nBat     = GetPrivateProfileInt("Cham", "LiaCanh", 1, szCfg);
	s_nXaNgang = GetPrivateProfileInt("Cham", "LiaXaNgang", 60, szCfg);
	s_nXaDoc   = GetPrivateProfileInt("Cham", "LiaXaDoc", 60, szCfg);
	s_nNguong  = GetPrivateProfileInt("Cham", "LiaNguong", 3, szCfg);
	s_nChoVeMs = GetPrivateProfileInt("Cham", "LiaChoVeMs", 1000, szCfg);
	s_nVeTocDo = GetPrivateProfileInt("Cham", "LiaVeTocDo", 700, szCfg);
	s_nVeEm    = GetPrivateProfileInt("Cham", "LiaVeEm", 3, szCfg);
	s_nVeKhiDi = GetPrivateProfileInt("Cham", "LiaVeKhiDi", 1, szCfg);
	s_nNhatKy  = GetPrivateProfileInt("Cham", "LiaNhatKy", 0, szCfg);
	GetPrivateProfileString("Cham", "LiaThu", szThu, szThu, sizeof(szThu), szCfg);
	sscanf(szThu, "%d,%d", &s_nThuDx, &s_nThuDy);
	s_nZoomBat     = GetPrivateProfileInt("Cham", "ZoomCanh", 1, szCfg);	// [ZOOM 13/09]
	s_nZoomToiDa   = GetPrivateProfileInt("Cham", "ZoomToiDa", 150, szCfg);
	s_nZoomBuoc    = GetPrivateProfileInt("Cham", "ZoomBuoc", 5, szCfg);
	s_nZoomMacDinh = GetPrivateProfileInt("Cham", "ZoomMacDinh", 100, szCfg);
	s_nZoomThu     = GetPrivateProfileInt("Cham", "ZoomThu", 0, szCfg);
	if (s_nZoomToiDa < 100) s_nZoomToiDa = 100;   if (s_nZoomToiDa > 300) s_nZoomToiDa = 300;
	if (s_nZoomBuoc < 1)    s_nZoomBuoc = 1;      if (s_nZoomBuoc > 50)   s_nZoomBuoc = 50;
	if (s_nZoomMacDinh < 100) s_nZoomMacDinh = 100; if (s_nZoomMacDinh > s_nZoomToiDa) s_nZoomMacDinh = s_nZoomToiDa;
	s_nZoom = s_nZoomMacDinh;
	if (s_nXaNgang < 0) s_nXaNgang = 0;   if (s_nXaNgang > 150) s_nXaNgang = 150;
	if (s_nXaDoc < 0)   s_nXaDoc = 0;     if (s_nXaDoc > 150)   s_nXaDoc = 150;
	if (s_nNguong < 1)  s_nNguong = 1;    if (s_nNguong > 40)   s_nNguong = 40;
	if (s_nChoVeMs < 0) s_nChoVeMs = 0;
	if (s_nVeTocDo < 50) s_nVeTocDo = 50;
	if (s_nVeEm < 0)    s_nVeEm = 0;      if (s_nVeEm > 30)     s_nVeEm = 30;
	g_DebugLog("[LIA] lia canh: bat=%d xa=%d%%x%d%% nguong=%d cho ve=%d ms toc do=%d px/s em=%d ve khi di=%d thu=%d,%d",
		s_nBat, s_nXaNgang, s_nXaDoc, s_nNguong, s_nChoVeMs, s_nVeTocDo, s_nVeEm, s_nVeKhiDi, s_nThuDx, s_nThuDy);
	g_DebugLog("[ZOOM] chum hai ngon: bat=%d toi da=%d%% buoc=%d%% mac dinh=%d%% thu=%d%%", s_nZoomBat, s_nZoomToiDa, s_nZoomBuoc, s_nZoomMacDinh, s_nZoomThu);
}

// [ZOOM 13/09] Represent3 (libRepresent3.so / Represent3.dll) qua GetProcAddress nhu Wnds.cpp JxUi_TheGioi: libmain.so khong link Represent3;
// iOS tra bang ky hieu tinh (Rep3_JxTheGioi da dang ky trong ios/JxIosMain.cpp). Khong co ham (ban cu) -> 0.
typedef int (*PFN_Rep3JxTheGioi)(int, int);
static int Rep3TheGioi(int nLenh, int nThamSo)
{
	static PFN_Rep3JxTheGioi s_pfn = NULL; static int s_nThu = 0;
	if (!s_pfn && s_nThu < 8) { s_nThu++; HMODULE h = GetModuleHandleA("Represent3.dll"); if (h) s_pfn = (PFN_Rep3JxTheGioi)GetProcAddress(h, "Rep3_JxTheGioi"); }
	return s_pfn ? s_pfn(nLenh, nThamSo) : 0;
}

// Dang trong the gioi (co thanh cong cu) - o menu / dang nhap / chon nhan vat thi khong lia.
static bool TrongGame()
{
	return KUiToolsControlBar::GetSelf() != NULL;
}

// Thong tin tieu diem: nOrigFocus = vi tri nhan vat (luon co), bien ban do (chi khi tra ve != 0).
static int LayThongTin(KSceneMapInfo* p)
{
	memset(p, 0, sizeof(*p));
	if (!g_pCoreShell)
		return 0;
	return g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (KUPARAM)p, 0);
}

static void ApLech()
{
	if (!g_pCoreShell)
		return;
	if (!s_nDaBatCo)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 1);
		s_nDaBatCo = 1;
	}
	// FOCUS_OFFSET: CoreShell doc (int)uParam -> ep qua unsigned int de so am di dung (nhu UiMiniMap::MapScroll)
	g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (KUPARAM)(unsigned int)s_nLechX, s_nLechY);
}

// Ket thuc: ve dung vi tri nhan vat, tat co (thu tu nhu KUiMiniMap::MapMoveBack).
static void KetThuc(const char* szLyDo)
{
	if (s_nNhatKy)
		g_DebugLog("[LIA] ket thuc (%s): lech cuoi %d,%d, %u lan keo", szLyDo, s_nLechX, s_nLechY, s_uKeoDem);
	s_fLechX = s_fLechY = 0.f;
	s_nLechX = s_nLechY = 0;
	s_uKeoDem = 0;
	if (g_pCoreShell && s_nDaBatCo)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
	}
	s_nDaBatCo = 0;
	s_nTrangThai = LIA_KHONG;
}

// Kep do lech: theo % khung ve (LiaXaNgang/LiaXaDoc) va theo bien ban do (nFocusMin/Max cua Core, neu co).
static void KepLech()
{
	KSceneMapInfo o;
	float fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100) * (float)s_nZoom / 100.f;	// [ZOOM 13/09] nhin rong thi lia xa hon theo ti le
	float fMaxY = (float)(SCREEN_HEIGHT * s_nXaDoc / 100 * 2) * (float)s_nZoom / 100.f;	// Y the gioi = 2 x px

	if (s_fLechX >  fMaxX) s_fLechX =  fMaxX;
	if (s_fLechX < -fMaxX) s_fLechX = -fMaxX;
	if (s_fLechY >  fMaxY) s_fLechY =  fMaxY;
	if (s_fLechY < -fMaxY) s_fLechY = -fMaxY;
	if (LayThongTin(&o))
	{
		float fMin = (float)(o.nFocusMinH - o.nOrigFocusH), fMax = (float)(o.nFocusMaxH - o.nOrigFocusH);
		if (fMin <= fMax)	// nhan vat dang dung ngoai khoang (sat mep ban do) thi khong kep truc nay
		{
			if (s_fLechX < fMin) s_fLechX = fMin;
			if (s_fLechX > fMax) s_fLechX = fMax;
		}
		fMin = (float)(o.nFocusMinV - o.nOrigFocusV); fMax = (float)(o.nFocusMaxV - o.nOrigFocusV);
		if (fMin <= fMax)
		{
			if (s_fLechY < fMin) s_fLechY = fMin;
			if (s_fLechY > fMax) s_fLechY = fMax;
		}
	}
	s_nLechX = (int)floorf(s_fLechX + 0.5f);
	s_nLechY = (int)floorf(s_fLechY + 0.5f);
}

bool JxLia_Bat()
{
	DocCaiDat();
	return s_nBat != 0;
}

bool JxLia_DuocBatDau(int x, int y)
{
	DocCaiDat();
	if (!s_nBat || !g_pCoreShell || !TrongGame())
		return false;
	if (UiToaDo_DangSua())
		return false;
	if (JxUi_CoGiaoDienTaiDiem(x, y))
		return false;
	if (JxCan_TrongVung(x, y) || JxCan_DangCam())
		return false;
	if (JxKyNang_TrungNut(x, y) > 0)
		return false;
	return true;
}

void JxLia_BatDau(int x, int y)
{
	DocCaiDat();
	s_nTayX = x;
	s_nTayY = y;
	if (s_nTrangThai == LIA_KHONG)
	{
		s_fLechX = s_fLechY = 0.f;
		s_nLechX = s_nLechY = 0;
		s_uKeoDem = 0;
	}
	// dang cho ve / dang ve: giu do lech hien co, keo tiep tu do
	s_nTrangThai = LIA_KEO;
	ApLech();
	if (s_nNhatKy)
		g_DebugLog("[LIA] bat dau tai %d,%d (lech dang co %d,%d)", x, y, s_nLechX, s_nLechY);
}

void JxLia_Keo(int x, int y)
{
	int dx, dy;

	if (s_nTrangThai != LIA_KEO)
		return;
	dx = x - s_nTayX;
	dy = y - s_nTayY;
	if (abs(dx) < s_nNguong && abs(dy) < s_nNguong)
		return;	// rung tay: giu moc cu de cong don, khong troi
	s_nTayX = x;
	s_nTayY = y;
	s_fLechX -= (float)dx;			// keo sang phai = xem phan ben trai
	s_fLechY -= (float)(dy * 2);	// Y the gioi = 2 x px
	KepLech();
	ApLech();
	s_uKeoDem++;
}

void JxLia_Nha()
{
	if (s_nTrangThai != LIA_KEO)
		return;
	s_uNhaLuc = (unsigned int)GetTickCount();
	s_nTrangThai = LIA_CHO_VE;
	if (s_nNhatKy)
		g_DebugLog("[LIA] nha tay: lech %d,%d -> cho %d ms roi ve", s_nLechX, s_nLechY, s_nChoVeMs);
}

void JxLia_DatLai()
{
	if (s_nTrangThai != LIA_KHONG || s_nDaBatCo)
		KetThuc("dat lai");
	s_nCoGoc = 0;
	s_uVaoGameLuc = 0;
	s_nChum = 0;
	s_nZoomThuXong = 0; s_nZoomDaAp = -1;	// [ZOOM 13/09] doi map: 3 s sau ap lai zoom (vung truy van vat the) - Represent3 van giu zoom
}

int JxLia_DangLia()
{
	return s_nTrangThai != LIA_KHONG;
}

void JxLia_Nhip()
{
	KSceneMapInfo o;
	unsigned int uNay;
	float dt, fLen, fBuoc, fEm;
	bool bDi = false;

	DocCaiDat();
	if (!s_nBat)
		return;
	uNay = (unsigned int)GetTickCount();
	dt = s_uNhipTruoc ? (float)(uNay - s_uNhipTruoc) / 1000.f : 0.f;
	if (dt > 0.1f) dt = 0.1f;	// khung dung lau (nap map) khong nhay mot cuc
	s_uNhipTruoc = uNay;
	if (!TrongGame())
	{
		if (s_nTrangThai != LIA_KHONG || s_nDaBatCo)
			KetThuc("roi the gioi");
		s_nCoGoc = 0;
		s_uVaoGameLuc = 0;
		return;
	}
	// [ZOOM 13/09] zoom mac dinh / ZoomThu (go loi): ap sau khi vao the gioi 3 s (Represent3 va Core da san sang)
	if (!s_uVaoGameLuc)
		s_uVaoGameLuc = uNay;
	else if (uNay - s_uVaoGameLuc >= 3000 && !s_nZoomThuXong)
	{
		s_nZoomThuXong = 1;
		if (s_nZoomThu > 100) { g_DebugLog("[ZOOM] ZoomThu: tu dat %d%%", s_nZoomThu); JxLia_ZoomDat(s_nZoomThu); }
		else if (s_nZoom != 100 || s_nZoomDaAp != s_nZoom) JxLia_ZoomDat(s_nZoom);
	}
	// LiaThu (go loi, thu tren may ao khong co ngon tay): 3 s sau khi vao the gioi tu keo mot lan roi tu ve
	if ((s_nThuDx || s_nThuDy) && !s_nThuXong)
	{
		if (!s_uVaoGameLuc)
			s_uVaoGameLuc = uNay;
		else if (uNay - s_uVaoGameLuc >= 3000 && s_nTrangThai == LIA_KHONG)
		{
			int cx = SCREEN_WIDTH / 2, cy = SCREEN_HEIGHT / 2;
			s_nThuXong = 1;
			g_DebugLog("[LIA] LiaThu: gia lap keo %d,%d tu giua khung ve", s_nThuDx, s_nThuDy);
			JxLia_BatDau(cx, cy);
			JxLia_Keo(cx + s_nThuDx, cy + s_nThuDy);
			JxLia_Nha();
		}
	}
	if (s_nTrangThai == LIA_KHONG)
		return;
	// nhan vat co dang di khong: tieu diem goc (nOrigFocus) doi giua hai nhip
	LayThongTin(&o);
	if (s_nCoGoc && (o.nOrigFocusH != s_nGocX || o.nOrigFocusV != s_nGocY))
		bDi = true;
	s_nGocX = o.nOrigFocusH;
	s_nGocY = o.nOrigFocusV;
	s_nCoGoc = 1;
	if (s_nTrangThai == LIA_KEO)
	{
		ApLech();	// bay FollowWithMap: ap lai moi khung de khung van bam nhan vat (kem do lech) khi nhan vat di
		return;
	}
	if (s_nTrangThai == LIA_CHO_VE)
	{
		if ((s_nVeKhiDi && (bDi || JxCan_DangCam())) || uNay - s_uNhaLuc >= (unsigned int)s_nChoVeMs)
		{
			s_nTrangThai = LIA_VE;
			if (s_nNhatKy)
				g_DebugLog("[LIA] bat dau ve (%s)", (uNay - s_uNhaLuc >= (unsigned int)s_nChoVeMs) ? "het cho" : "nhan vat di");
		}
		else
		{
			ApLech();
			return;
		}
	}
	// LIA_VE: troi do lech ve 0 - buoc = max(toc do toi thieu, phan con lai x LiaVeEm) moi giay
	fLen = sqrtf(s_fLechX * s_fLechX + (s_fLechY * 0.5f) * (s_fLechY * 0.5f));	// do dai theo px khung ve
	fBuoc = (float)s_nVeTocDo * dt;
	fEm = fLen * (float)s_nVeEm * dt;
	if (fEm > fBuoc)
		fBuoc = fEm;
	if (fLen <= fBuoc || fLen < 2.f)
	{
		KetThuc("ve xong");
		return;
	}
	if (fBuoc > 0.f)
	{
		float k = 1.f - fBuoc / fLen;
		s_fLechX *= k;
		s_fLechY *= k;
		s_nLechX = (int)floorf(s_fLechX + 0.5f);
		s_nLechY = (int)floorf(s_fLechY + 0.5f);
	}
	ApLech();
}

// ================= [ZOOM 13/09] zoom nhin rong + chum hai ngon =================
int JxLia_ZoomLay()
{
	return s_nZoom;
}

void JxLia_ZoomDat(int nPhanTram)
{
	DocCaiDat();
	if (nPhanTram < 100) nPhanTram = 100;
	if (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;
	nPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;
	if (nPhanTram < 100) nPhanTram = 100;
	s_nZoom = nPhanTram;
	if (s_nZoomDaAp == s_nZoom || !g_pCoreShell)
		return;
	s_nZoomDaAp = s_nZoom;
	// Represent3: RT = khung x zoom, thu nho khi blit; Core: vung truy van vat the = khung x zoom (khong thi nguoi o ria khong duoc ve)
	Rep3TheGioi(4, s_nZoom * 10);
	g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);
	if (s_nNhatKy || s_nZoomThu)
		g_DebugLog("[ZOOM] zoom %d%% (Rep3 %d, vung truy van %dx%d)", s_nZoom, Rep3TheGioi(5, 0), SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);
}

bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2)
{
	DocCaiDat();
	if (!s_nZoomBat)
		return false;
	return JxLia_DuocBatDau(x1, y1) && JxLia_DuocBatDau(x2, y2);
}

void JxLia_ChumBatDau(int x1, int y1, int x2, int y2)
{
	float dx = (float)(x2 - x1), dy = (float)(y2 - y1);

	s_fChumD0 = sqrtf(dx * dx + dy * dy);
	if (s_fChumD0 < 10.f)
		s_fChumD0 = 10.f;
	s_nChumZoom0 = s_nZoom;
	s_nChum = 1;
	JxLia_BatDau((x1 + x2) / 2, (y1 + y2) / 2);	// lia theo tam hai ngon (dang lech thi giu lech, neo lai tu tam)
	if (s_nNhatKy)
		g_DebugLog("[ZOOM] chum bat dau: d0=%.0f zoom=%d%%", s_fChumD0, s_nZoom);
}

void JxLia_ChumKeo(int x1, int y1, int x2, int y2)
{
	float dx = (float)(x2 - x1), dy = (float)(y2 - y1);
	float d = sqrtf(dx * dx + dy * dy);

	if (!s_nChum)
		return;
	if (d < 10.f)
		d = 10.f;
	JxLia_ZoomDat((int)((float)s_nChumZoom0 * s_fChumD0 / d + 0.5f));	// hai ngon gan nhau (d < d0) = zoom lon = thay rong hon
	JxLia_Keo((x1 + x2) / 2, (y1 + y2) / 2);
}

void JxLia_ChumNha()
{
	if (!s_nChum)
		return;
	s_nChum = 0;
	if (s_nNhatKy)
		g_DebugLog("[ZOOM] chum nha: zoom=%d%%", s_nZoom);
	JxLia_Nha();
}

#endif // JX_MOBILE
