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
#include "../../Core/src/GameDataDef.h"	// KSceneMapInfo, KUiSceneTimeInfoOften
#include "../../Represent/iRepresent/iRepresentShell.h"	// [CAMERA 13/09] OutputText
#include "KDebug.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern iCoreShell*	g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;	// [CAMERA 13/09]
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
// [ZOOM3D 14/09] chum hai ngon kieu game 3D (chu "lam 1-3" sau muc 11 mo zoom ban 3D): tuyen tinh theo px + chong rung + duoi ham mu
static int	s_nZoomKieu = 1;		// ZoomKieu: 1 = kieu 3D (zoom dich -= dpx x ZoomNhay/100, muot ZoomMuot), 0 = ti le khoang cach hai ngon (cu)
static int	s_nZoomNhay = 10;		// ZoomNhay: % zoom tren 100 px chum o muc "vua" (3D: 0,01 don vi/px tren dai 11 don vi ~ 9 %); Zoom nhanh x2, cham x0,5
static int	s_nZoomChongRung = 5;	// ZoomChongRung: khoang cach hai ngon doi it hon (px) thi bo qua (3D: 5 px)
static int	s_nZoomMuot = 16;		// ZoomMuot: zoom += (dich - zoom) x clamp01(dt x ZoomMuot) (3D: fDistanceSpeed = 16, 95 % sau ~0,18 s)
static int	s_nZoomToiThieu = 80;	// ZoomToiThieu: % nho nhat = PHONG TO (80 = to 1,25 lan; 3D cho toi 1,9 lan); 100 = tat phong to
static float	s_fChumDCu = 0.f;		// khoang cach hai ngon lan chap nhan gan nhat (kieu 3D)
static float	s_fZoomDich3D = 0.f;	// zoom dich lien tuc trong luc chum (kieu 3D)
static int	s_nNcZoomNhanh = 0, s_nNcZoomCham = 0, s_nNcLac = 1;	// cong tac Cai dat > Toi uu: Zoom nhanh / Zoom cham / Lac camera (JxLia_DatNhay)

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
// [CAMERA 13/09] theo map + nho + troi + chu bao
static int			s_nZoomToiDaCfg = 150, s_nXaNgangCfg = 60, s_nXaDocCfg = 60, s_nBatCfg = 1, s_nZoomBatCfg = 1;	// gia tri config.ini (map ghi de len ban dang dung)
static int			s_nZoomNho = 1;		// [Cham] ZoomNho: 1 = nho zoom nguoi choi qua UserData\CameraMobile.ini
static int			s_nZoomTocDo = 100;	// [Cham] ZoomTocDo: %/giay khi troi toi zoom cua map
static int			s_nZoomChu = 1;		// [Cham] ZoomChu: 1 = chu "Nhin rong NNN %" 1,5 s khi doi
static int			s_nZoomNguoiChoi = 0;	// zoom nguoi choi tu chum (0 = chua), nho qua tep
static int			s_nZoomDich = 0;		// zoom dang troi toi (0 = khong troi)
static float		s_fZoomTroi = 100.f;	// zoom lien tuc trong luc troi
static int			s_nCanDocMap = 1;		// 1 = vao map / doi map, can doc lai muc cua map
static int			s_nMapId = -1;			// map dang ap
static unsigned int	s_uZoomChuLuc = 0;		// luc zoom doi gan nhat (chu bao)
static int			s_nZoomChuTruoc = 100;	// zoom lan bao truoc (chi bao khi doi)
static char			s_szZoomChu[48] = "";
static void			ZoomAp(int nPhanTram);	// dinh nghia o khoi ZOOM cuoi tep
static void			Camera_DocMap();
// [CAMERA 13/09 TUYCHON] cong tac nguoi choi (Cai dat > Toi uu); hieu luc = config/map VA nguoi choi
static int			s_nNcLia = 1, s_nNcZoom = 1, s_nNcVeNhanh = 0;
static int ChoVeMs() { return s_nNcVeNhanh ? s_nChoVeMs / 3 : s_nChoVeMs; }	// ve nhanh: cho 1/3
static int VeTocDo() { return s_nNcVeNhanh ? s_nVeTocDo * 2 : s_nVeTocDo; }	// toc do x2
static int VeEm()    { return s_nNcVeNhanh ? s_nVeEm * 2 : s_nVeEm; }
// [LAC 14/09] camera lac nhe khi lia: goc = LiaLacDo x (lechX / lech ngang toi da), Represent3 lenh 6 (goc 0,01 do) / 8 (le RT phan nghin)
static int			s_nLacDo = 5;		// LiaLacDo: do xoay toi da (0 = tat)
static int			s_nLacLe = 112;		// LiaLacLe: % anh dem rong them khi dang lia (goc khung khong ho khi xoay)
static int			s_nLacThu = 0;		// LiaLacThu: GO LOI - giu goc co dinh (do) 3 s sau khi vao the gioi
static int			s_nLacThuXong = 0;
static int			s_nGocDaGui = 0, s_nLeDaGui = 1000;	// da gui cho Represent3
static int			s_nLacDoc = 6;		// [LAC 14/09 b] LiaLacDoc: % co/dan doc toi da khi keo doc toi LiaXaDoc (0 = tat)
static int			s_nDocDaGui = 1000;
static unsigned int	s_uLeChoTat = 0;	// [LAC 14/09 c] luc bat dau cho tat le RT sau khi het lia (giu 2 s)

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
	s_nZoomKieu      = GetPrivateProfileInt("Cham", "ZoomKieu", 1, szCfg);	// [ZOOM3D 14/09]
	s_nZoomNhay      = GetPrivateProfileInt("Cham", "ZoomNhay", 10, szCfg);
	s_nZoomChongRung = GetPrivateProfileInt("Cham", "ZoomChongRung", 5, szCfg);
	s_nZoomMuot      = GetPrivateProfileInt("Cham", "ZoomMuot", 16, szCfg);
	s_nZoomToiThieu  = GetPrivateProfileInt("Cham", "ZoomToiThieu", 80, szCfg);
	if (s_nZoomNhay < 1) s_nZoomNhay = 1; if (s_nZoomNhay > 100) s_nZoomNhay = 100;
	if (s_nZoomChongRung < 0) s_nZoomChongRung = 0; if (s_nZoomChongRung > 50) s_nZoomChongRung = 50;
	if (s_nZoomMuot < 1) s_nZoomMuot = 1; if (s_nZoomMuot > 100) s_nZoomMuot = 100;
	if (s_nZoomToiThieu < 50) s_nZoomToiThieu = 50; if (s_nZoomToiThieu > 100) s_nZoomToiThieu = 100;
	if (s_nZoomToiDa < 100) s_nZoomToiDa = 100;   if (s_nZoomToiDa > 300) s_nZoomToiDa = 300;
	if (s_nZoomBuoc < 1)    s_nZoomBuoc = 1;      if (s_nZoomBuoc > 50)   s_nZoomBuoc = 50;
	if (s_nZoomMacDinh < s_nZoomToiThieu) s_nZoomMacDinh = s_nZoomToiThieu; if (s_nZoomMacDinh > s_nZoomToiDa) s_nZoomMacDinh = s_nZoomToiDa;	// [ZOOM3D 14/09] cho phep < 100
	s_nZoom = s_nZoomMacDinh;
	s_nZoomToiDaCfg = s_nZoomToiDa; s_nXaNgangCfg = s_nXaNgang; s_nXaDocCfg = s_nXaDoc; s_nBatCfg = s_nBat; s_nZoomBatCfg = s_nZoomBat;	// [CAMERA 13/09]
	s_nZoomNho   = GetPrivateProfileInt("Cham", "ZoomNho", 1, szCfg);
	s_nZoomTocDo = GetPrivateProfileInt("Cham", "ZoomTocDo", 100, szCfg);
	s_nZoomChu   = GetPrivateProfileInt("Cham", "ZoomChu", 1, szCfg);
	s_nLacDo  = GetPrivateProfileInt("Cham", "LiaLacDo", 5, szCfg);	// [LAC 14/09]
	s_nLacLe  = GetPrivateProfileInt("Cham", "LiaLacLe", 112, szCfg);
	s_nLacThu = GetPrivateProfileInt("Cham", "LiaLacThu", 0, szCfg);
	s_nLacDoc = GetPrivateProfileInt("Cham", "LiaLacDoc", 6, szCfg);	// [LAC 14/09 b]
	if (s_nLacDoc < -15) s_nLacDoc = -15; if (s_nLacDoc > 15) s_nLacDoc = 15;
	if (s_nLacDo < -15) s_nLacDo = -15; if (s_nLacDo > 15) s_nLacDo = 15;
	if (s_nLacLe < 100) s_nLacLe = 100; if (s_nLacLe > 150) s_nLacLe = 150;
	if (s_nLacThu < -15) s_nLacThu = -15; if (s_nLacThu > 15) s_nLacThu = 15;
	if (s_nZoomTocDo < 10) s_nZoomTocDo = 10; if (s_nZoomTocDo > 1000) s_nZoomTocDo = 1000;
	if (s_nZoomNho)
	{	// [CAMERA 13/09] zoom nguoi choi da nho (nhu CameraSave cua game 3D): UserData\CameraMobile.ini dong "Zoom=NNN"
		char szNho[MAX_PATH]; char szDong[64]; FILE* pTep;
		GetCurrentDirectory(MAX_PATH, szNho); strcat(szNho, "\\UserData\\CameraMobile.ini");
		pTep = fopen(szNho, "rt");
		if (pTep)
		{
			while (fgets(szDong, sizeof(szDong), pTep))
				if (strncmp(szDong, "Zoom=", 5) == 0) s_nZoomNguoiChoi = atoi(szDong + 5);
			fclose(pTep);
			if (s_nZoomNguoiChoi < 50 || s_nZoomNguoiChoi > 300) s_nZoomNguoiChoi = 0;	// [ZOOM3D 14/09] 50..300
		}
	}
	s_fZoomTroi = (float)s_nZoom;
	if (s_nXaNgang < 0) s_nXaNgang = 0;   if (s_nXaNgang > 150) s_nXaNgang = 150;
	if (s_nXaDoc < 0)   s_nXaDoc = 0;     if (s_nXaDoc > 150)   s_nXaDoc = 150;
	if (s_nNguong < 1)  s_nNguong = 1;    if (s_nNguong > 40)   s_nNguong = 40;
	if (s_nChoVeMs < 0) s_nChoVeMs = 0;
	if (s_nVeTocDo < 50) s_nVeTocDo = 50;
	if (s_nVeEm < 0)    s_nVeEm = 0;      if (s_nVeEm > 30)     s_nVeEm = 30;
	g_DebugLog("[LIA] lia canh: bat=%d xa=%d%%x%d%% nguong=%d cho ve=%d ms toc do=%d px/s em=%d ve khi di=%d thu=%d,%d",
		s_nBat, s_nXaNgang, s_nXaDoc, s_nNguong, s_nChoVeMs, s_nVeTocDo, s_nVeEm, s_nVeKhiDi, s_nThuDx, s_nThuDy);
	g_DebugLog("[ZOOM] chum hai ngon: bat=%d toi da=%d%% buoc=%d%% mac dinh=%d%% thu=%d%%", s_nZoomBat, s_nZoomToiDa, s_nZoomBuoc, s_nZoomMacDinh, s_nZoomThu);
	g_DebugLog("[CAMERA] theo map: nho=%d (zoom nguoi choi da nho %d%%) toc do troi=%d%%/s chu=%d", s_nZoomNho, s_nZoomNguoiChoi, s_nZoomTocDo, s_nZoomChu);
	g_DebugLog("[LAC] camera lac nhe khi lia: ngang %d do, doc %d%%, le RT %d%%, thu %d do", s_nLacDo, s_nLacDoc, s_nLacLe, s_nLacThu);
	g_DebugLog("[ZOOM3D] chum kieu=%d nhay=%d%%/100px chong rung=%d px muot=%d toi thieu=%d%%", s_nZoomKieu, s_nZoomNhay, s_nZoomChongRung, s_nZoomMuot, s_nZoomToiThieu);
}

// [CAMERA 13/09] ghi zoom nguoi choi: UserData\CameraMobile.ini (nhu KyNangMobile.ini: GetCurrentDirectory + duong tuong doi)
static void Camera_GhiNho()
{
	char szNho[MAX_PATH]; FILE* pTep;

	if (!s_nZoomNho || s_nZoomNguoiChoi <= 0)
		return;
	GetCurrentDirectory(MAX_PATH, szNho); strcat(szNho, "\\UserData\\CameraMobile.ini");
	pTep = fopen(szNho, "wt");
	if (!pTep)
		return;
	fprintf(pTep, "; [CAMERA 13/09] zoom nguoi choi tu chum (%%); xoa tep = ve ZoomMacDinh cua map\nZoom=%d\n", s_nZoomNguoiChoi);
	fclose(pTep);
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

// [LAC 14/09] goc xoay theo do lech ngang (dang lia) -> Represent3 lenh 6; le RT (lenh 8) + vung truy van vat the rong theo khi dang lia
static void ApXoay()
{
	int nGoc = 0, nLe = 1000, nDoc = 1000;

	if (s_nLacThu && s_nLacThuXong)
		return;	// go loi: giu goc co dinh
	if (s_nNcLac && (s_nLacDo != 0 || s_nLacDoc != 0) && s_nTrangThai != LIA_KHONG)	// [ZOOM3D 14/09] cong tac Lac camera
	{
		const float fMaxX = (float)(SCREEN_WIDTH * s_nXaNgang / 100) * (float)s_nZoom / 100.f;
		const float fMaxY = (float)(SCREEN_HEIGHT * s_nXaDoc / 100 * 2) * (float)s_nZoom / 100.f;	// [LAC 14/09 b] Y the gioi = 2 x px
		float fTi = (fMaxX > 1.f) ? s_fLechX / fMaxX : 0.f;
		float fTiY = (fMaxY > 1.f) ? s_fLechY / fMaxY : 0.f;
		if (fTi > 1.f) fTi = 1.f; if (fTi < -1.f) fTi = -1.f;
		if (fTiY > 1.f) fTiY = 1.f; if (fTiY < -1.f) fTiY = -1.f;
		nGoc = (int)(fTi * (float)s_nLacDo * 100.f);
		nDoc = 1000 + (int)(fTiY * (float)s_nLacDoc * 10.f);	// keo xuong (lech am) -> k < 1: canh det nhu camera ngang hon; keo len -> k > 1
		nLe = s_nLacLe * 10;
	}
	if (nLe == 1000 && s_nLeDaGui != 1000)
	{	// [LAC 14/09 c] het lia: giu le RT them 2 s (keo lien tiep khong phai cap lai RT); JxLia_Nhip goi lai ApXoay trong luc cho
		const unsigned int uLuc = (unsigned int)GetTickCount();
		if (!s_uLeChoTat) s_uLeChoTat = uLuc;
		if (uLuc - s_uLeChoTat < 2000) nLe = s_nLeDaGui;
		else s_uLeChoTat = 0;
	}
	else s_uLeChoTat = 0;
	if (nLe != s_nLeDaGui)
	{
		Rep3TheGioi(8, nLe); s_nLeDaGui = nLe;
		if (g_pCoreShell) g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100 * nLe / 1000, SCREEN_HEIGHT * s_nZoom / 100 * nLe / 1000);
	}
	if (nGoc != s_nGocDaGui) { Rep3TheGioi(6, nGoc); s_nGocDaGui = nGoc; }
	if (nDoc != s_nDocDaGui) { Rep3TheGioi(9, nDoc); s_nDocDaGui = nDoc; }	// [LAC 14/09 b]
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
	ApXoay();	// [LAC 14/09]
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
	ApXoay();	// [LAC 14/09] goc ve 0, bo le RT
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
	return s_nBat && s_nNcLia;	// [CAMERA 13/09 TUYCHON]
}

// [CAMERA 13/09] diem (x,y) nam tren ban do (khong dinh giao dien / can / nut ky nang) - dung chung cho lia mot ngon va chum hai ngon
static bool TrenBanDo(int x, int y)
{
	if (!g_pCoreShell || !TrongGame())
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

bool JxLia_DuocBatDau(int x, int y)
{
	DocCaiDat();
	return s_nBat && s_nNcLia && TrenBanDo(x, y);	// [CAMERA 13/09 TUYCHON] nguoi choi tat lia -> khong lia
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
	s_nZoomDaAp = -1; s_nCanDocMap = 1; s_nZoomDich = 0;	// [CAMERA 13/09] doi map: 1 s sau doc muc cua map, ap lai vung truy van, troi toi zoom dich
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
	// [CAMERA 13/09] vao the gioi / doi map: 1 s sau doc muc cua map (settings\camera_mobile.ini) roi TROI zoom toi dich theo ZoomTocDo
	if (!s_uVaoGameLuc)
		s_uVaoGameLuc = uNay;
	else if (uNay - s_uVaoGameLuc >= 1000 && s_nCanDocMap)
		Camera_DocMap();
	if (s_nLacThu && !s_nLacThuXong && uNay - s_uVaoGameLuc >= 3000)
	{	// [LAC 14/09] GO LOI: giu goc co dinh de chup anh / cham thu (may ao khong keo tay duoc)
		s_nLacThuXong = 1;
		Rep3TheGioi(8, s_nLacLe * 10); s_nLeDaGui = s_nLacLe * 10;
		Rep3TheGioi(6, s_nLacThu * 100); s_nGocDaGui = s_nLacThu * 100;
		if (g_pCoreShell) g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100 * s_nLeDaGui / 1000, SCREEN_HEIGHT * s_nZoom / 100 * s_nLeDaGui / 1000);
		g_DebugLog("[LAC] LiaLacThu: giu goc %d do, le RT %d%%", s_nLacThu, s_nLacLe);
	}
	if (s_nZoomDich > 0 && (!s_nChum || s_nZoomKieu))	// [ZOOM3D 14/09] kieu 3D: troi ca trong luc chum (dich do ChumKeo dat)
	{
		if (s_nZoomKieu)
		{	// [ZOOM3D 14/09] duoi ham mu nhu GameCamera 3D: t = clamp01(dt x ZoomMuot)
			float t = dt * (float)s_nZoomMuot; if (t > 1.f) t = 1.f;
			s_fZoomTroi += ((float)s_nZoomDich - s_fZoomTroi) * t;
			if (s_fZoomTroi > (float)s_nZoomDich - 0.5f && s_fZoomTroi < (float)s_nZoomDich + 0.5f) s_fZoomTroi = (float)s_nZoomDich;
		}
		else
		{
		float fBuoc = (float)s_nZoomTocDo * dt;
		if (s_fZoomTroi < (float)s_nZoomDich) { s_fZoomTroi += fBuoc; if (s_fZoomTroi > (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }
		else { s_fZoomTroi -= fBuoc; if (s_fZoomTroi < (float)s_nZoomDich) s_fZoomTroi = (float)s_nZoomDich; }
		}
		ZoomAp((int)(s_fZoomTroi + 0.5f));
		if ((int)(s_fZoomTroi + 0.5f) == s_nZoomDich)
		{
			ZoomAp(s_nZoomDich);
			s_nZoomDich = 0;
		}
	}
	if (!s_nBat || !s_nNcLia)
		return;	// [CAMERA 13/09] LiaCanh=0 (config hay map) hay nguoi choi tat: van doc map + troi zoom o tren, khong lia
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
			s_uNhaLuc = uNay;	// [LAC 14/09] keo gia lap: luc nha = luc nhip (khong thi unsigned am -> ve ngay)
		}
	}
	if (s_nTrangThai == LIA_KHONG && s_nLeDaGui != 1000)
		ApXoay();	// [LAC 14/09 c] dem 2 s roi tat le RT
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
		if ((s_nVeKhiDi && (bDi || JxCan_DangCam())) || uNay - s_uNhaLuc >= (unsigned int)ChoVeMs())
		{
			s_nTrangThai = LIA_VE;
			if (s_nNhatKy)
				g_DebugLog("[LIA] bat dau ve (%s)", (uNay - s_uNhaLuc >= (unsigned int)ChoVeMs()) ? "het cho" : "nhan vat di");
		}
		else
		{
			ApLech();
			return;
		}
	}
	// LIA_VE: troi do lech ve 0 - buoc = max(toc do toi thieu, phan con lai x LiaVeEm) moi giay
	fLen = sqrtf(s_fLechX * s_fLechX + (s_fLechY * 0.5f) * (s_fLechY * 0.5f));	// do dai theo px khung ve
	fBuoc = (float)VeTocDo() * dt;	// [CAMERA 13/09 TUYCHON] ve nhanh: x2
	fEm = fLen * (float)VeEm() * dt;
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

// [CAMERA 13/09] ap zoom that (Represent3 + vung truy van); ZoomDat = nguoi choi dat (huy troi, nho lai), troi theo map goi thang ZoomAp
static void ZoomAp(int nPhanTram)
{
	DocCaiDat();
	if (nPhanTram < s_nZoomToiThieu) nPhanTram = s_nZoomToiThieu;	// [ZOOM3D 14/09] < 100 = phong to
	if (nPhanTram > s_nZoomToiDa) nPhanTram = s_nZoomToiDa;
	nPhanTram = (nPhanTram + s_nZoomBuoc / 2) / s_nZoomBuoc * s_nZoomBuoc;
	if (nPhanTram < s_nZoomToiThieu) nPhanTram = s_nZoomToiThieu;
	s_nZoom = nPhanTram;
	if (s_nZoomDaAp == s_nZoom || !g_pCoreShell)
		return;
	s_nZoomDaAp = s_nZoom;
	if (s_nZoom != s_nZoomChuTruoc)
	{	// chu bao "Nhin rong NNN %" (JxLia_Ve) 1,5 s
		s_nZoomChuTruoc = s_nZoom;
		s_uZoomChuLuc = (unsigned int)GetTickCount();
		if (s_nZoom < 100) sprintf(s_szZoomChu, "Phãng to %d%%", 10000 / s_nZoom);	// [ZOOM3D 14/09] 80 % = to 125 %
		else sprintf(s_szZoomChu, "Nh×n réng %d%%", s_nZoom);
	}
	// Represent3: RT = khung x zoom, thu nho khi blit; Core: vung truy van vat the = khung x zoom (khong thi nguoi o ria khong duoc ve)
	Rep3TheGioi(4, s_nZoom * 10);
	g_pCoreShell->SetRepresentAreaSize(SCREEN_WIDTH * s_nZoom / 100 * s_nLeDaGui / 1000, SCREEN_HEIGHT * s_nZoom / 100 * s_nLeDaGui / 1000);	// [LAC 14/09] + le RT khi dang lia
	if (s_nNhatKy || s_nZoomThu)
		g_DebugLog("[ZOOM] zoom %d%% (Rep3 %d, vung truy van %dx%d)", s_nZoom, Rep3TheGioi(5, 0), SCREEN_WIDTH * s_nZoom / 100, SCREEN_HEIGHT * s_nZoom / 100);
}

void JxLia_ZoomDat(int nPhanTram)
{	// nguoi choi dat (chum): ap ngay, huy troi, nho lai
	ZoomAp(nPhanTram);
	s_nZoomDich = 0;
	s_fZoomTroi = (float)s_nZoom;
	s_nZoomNguoiChoi = s_nZoom;
}

// [CAMERA 13/09] doc muc cua map dang dung (settings\camera_mobile.ini) roi dat zoom dich
static void Camera_DocMap()
{
	KUiSceneTimeInfoOften o; char szTep[MAX_PATH]; char szMuc[32]; int nZoomMD, nDich;

	if (!g_pCoreShell)
		return;
	memset(&o, 0, sizeof(o));
	g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO_OFTEN, (KUPARAM)&o, 0);
	if (o.nSceneId <= 0)
		return;	// chua co map: thu lai nhip sau
	s_nCanDocMap = 0;
	s_nMapId = o.nSceneId;
	GetCurrentDirectory(MAX_PATH, szTep); strcat(szTep, "\\settings\\camera_mobile.ini");
	sprintf(szMuc, "Map_%d", o.nSceneId);
	// [MacDinh] ghi de config.ini; [Map_<id>] ghi de [MacDinh]
	nZoomMD      = GetPrivateProfileInt(szMuc, "ZoomMacDinh", GetPrivateProfileInt("MacDinh", "ZoomMacDinh", s_nZoomMacDinh, szTep), szTep);
	s_nZoomToiDa = GetPrivateProfileInt(szMuc, "ZoomToiDa",   GetPrivateProfileInt("MacDinh", "ZoomToiDa",   s_nZoomToiDaCfg, szTep), szTep);
	s_nXaNgang   = GetPrivateProfileInt(szMuc, "LiaXaNgang",  GetPrivateProfileInt("MacDinh", "LiaXaNgang",  s_nXaNgangCfg, szTep), szTep);
	s_nXaDoc     = GetPrivateProfileInt(szMuc, "LiaXaDoc",    GetPrivateProfileInt("MacDinh", "LiaXaDoc",    s_nXaDocCfg, szTep), szTep);
	s_nBat       = GetPrivateProfileInt(szMuc, "LiaCanh",     GetPrivateProfileInt("MacDinh", "LiaCanh",     s_nBatCfg, szTep), szTep);
	s_nZoomBat   = GetPrivateProfileInt(szMuc, "ZoomCanh",    GetPrivateProfileInt("MacDinh", "ZoomCanh",    s_nZoomBatCfg, szTep), szTep);
	if (s_nZoomToiDa < 100) s_nZoomToiDa = 100; if (s_nZoomToiDa > 300) s_nZoomToiDa = 300;
	if (s_nXaNgang < 0) s_nXaNgang = 0; if (s_nXaNgang > 150) s_nXaNgang = 150;
	if (s_nXaDoc < 0) s_nXaDoc = 0; if (s_nXaDoc > 150) s_nXaDoc = 150;
	// zoom dich: ZoomThu (go loi) > zoom nguoi choi da nho > ZoomMacDinh cua map; kep theo ZoomToiDa cua map; map tat zoom -> 100
	if (s_nZoomThu >= 50 && s_nZoomThu != 100 && !s_nZoomThuXong) { nDich = s_nZoomThu; s_nZoomThuXong = 1; }	// [ZOOM3D 14/09] ZoomThu < 100 = thu phong to
	else if (s_nZoomNho && s_nZoomNguoiChoi > 0) nDich = s_nZoomNguoiChoi;
	else nDich = nZoomMD;
	if (!s_nZoomBat || !s_nNcZoom) nDich = 100;	// [CAMERA 13/09 TUYCHON] map tat / nguoi choi tat -> 100 %
	if (nDich < s_nZoomToiThieu) nDich = s_nZoomToiThieu;	// [ZOOM3D 14/09]
	if (nDich > s_nZoomToiDa) nDich = s_nZoomToiDa;
	s_nZoomDich = nDich;
	s_fZoomTroi = (float)s_nZoom;
	Rep3TheGioi(11, (s_nZoomBat && s_nNcZoom) ? s_nZoomToiThieu * 10 : 1000);	// [TGCAP 14/09 b] bao Represent3 co phong to hay khong (< 1000) -> cap san RT2 o khung dau
	if (s_nZoomDaAp < 0)
		ZoomAp(s_nZoom);	// doi map: ap lai vung truy van (Represent3 van giu zoom) truoc khi troi
	g_DebugLog("[CAMERA] map %d (%s): zoom mac dinh %d%%, toi da %d%%, lia %d%%x%d%%, lia=%d zoom=%d | dang %d%% -> troi toi %d%% (nguoi choi nho %d%%)",
		o.nSceneId, o.szSceneName, nZoomMD, s_nZoomToiDa, s_nXaNgang, s_nXaDoc, s_nBat, s_nZoomBat, s_nZoom, nDich, s_nZoomNguoiChoi);
}

// [CAMERA 13/09] chu "Nhin rong NNN %" 1,5 s sau khi zoom doi - giua man, 1/3 tren (nhu KyNang_VeChu: OutputText toa do man hinh, co 16)
// [CAMERA 13/09 TUYCHON] cong tac nguoi choi tu Cai dat > Toi uu (KUiOptions2): goi luc vao the gioi (LoadSetting) va moi lan bam
void JxLia_DatTuyChon(int nLia, int nZoom, int nVeNhanh)
{
	int nZoomCu = s_nNcZoom;

	if (nLia >= 0) s_nNcLia = nLia ? 1 : 0;
	if (nZoom >= 0) s_nNcZoom = nZoom ? 1 : 0;
	if (nVeNhanh >= 0) s_nNcVeNhanh = nVeNhanh ? 1 : 0;
	if (!s_nNcLia && (s_nTrangThai != LIA_KHONG || s_nDaBatCo))
		KetThuc("nguoi choi tat lia");
	if (s_nNcZoom != nZoomCu)
	{	// tat -> troi ve 100 %; bat lai -> doc lai muc map (zoom da nho / mac dinh map)
		if (!s_nNcZoom) { s_nZoomDich = 100; s_fZoomTroi = (float)s_nZoom; }
		else s_nCanDocMap = 1;
	}
	g_DebugLog("[CAMERA] tuy chon nguoi choi: lia=%d nhin rong=%d ve nhanh=%d", s_nNcLia, s_nNcZoom, s_nNcVeNhanh);
}

// [ZOOM3D 14/09] cong tac Cai dat > Toi uu: Zoom nhanh / Zoom cham (loai tru nhau, ca hai tat = vua) / Lac camera
void JxLia_DatNhay(int nZoomNhanh, int nZoomCham, int nLac)
{
	if (nZoomNhanh >= 0) s_nNcZoomNhanh = nZoomNhanh ? 1 : 0;
	if (nZoomCham >= 0) s_nNcZoomCham = nZoomCham ? 1 : 0;
	if (nLac >= 0) s_nNcLac = nLac ? 1 : 0;
	g_DebugLog("[ZOOM3D] tuy chon: zoom nhanh=%d cham=%d lac camera=%d", s_nNcZoomNhanh, s_nNcZoomCham, s_nNcLac);
}
void JxLia_Ve()
{
	unsigned int uNay = (unsigned int)GetTickCount();
	int nDai;

	if (!s_nZoomChu || !s_uZoomChuLuc || uNay - s_uZoomChuLuc > 1500 || g_pRepresentShell == NULL || !TrongGame())
		return;
	nDai = (int)strlen(s_szZoomChu);
	if (nDai <= 0)
		return;
	g_pRepresentShell->OutputText(12, s_szZoomChu, nDai, SCREEN_WIDTH / 2 - nDai * 3, SCREEN_HEIGHT / 5, 0xFFFFD040, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xFF000000);	// [CAMERA 13/09 b] nhu PerfHud_Chu: co 12, vang, vien den
}

bool JxLia_ChumDuoc(int x1, int y1, int x2, int y2)
{
	DocCaiDat();
	if (!s_nZoomBat || !s_nNcZoom)
		return false;	// [CAMERA 13/09 TUYCHON]
	return TrenBanDo(x1, y1) && TrenBanDo(x2, y2);	// [CAMERA 13/09] chum khong can LiaCanh (map tat lia van chum duoc)
}

void JxLia_ChumBatDau(int x1, int y1, int x2, int y2)
{
	float dx = (float)(x2 - x1), dy = (float)(y2 - y1);

	s_fChumD0 = sqrtf(dx * dx + dy * dy);
	if (s_fChumD0 < 10.f)
		s_fChumD0 = 10.f;
	s_nChumZoom0 = s_nZoom;
	s_nChum = 1;
	s_fChumDCu = s_fChumD0; s_fZoomDich3D = (float)((s_nZoomDich > 0) ? s_nZoomDich : s_nZoom);	// [ZOOM3D 14/09]
	if (s_nBat && s_nNcLia)	// [CAMERA 13/09] LiaCanh=0 / nguoi choi tat lia thi chum chi zoom, khong lia
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
	if (s_nZoomKieu)
	{	// [ZOOM3D 14/09] kieu 3D: dich -= dpx x nhay (hai ngon dang ra = phong to); dpx < ZoomChongRung bo qua; ap muot o JxLia_Nhip
		float fD = d - s_fChumDCu, fNhay;
		if (fD <= -(float)s_nZoomChongRung || fD >= (float)s_nZoomChongRung)
		{
			s_fChumDCu = d;
			fNhay = (float)s_nZoomNhay / 100.f; if (s_nNcZoomNhanh) fNhay *= 2.f; else if (s_nNcZoomCham) fNhay *= 0.5f;
			s_fZoomDich3D -= fD * fNhay;
			if (s_fZoomDich3D < (float)s_nZoomToiThieu) s_fZoomDich3D = (float)s_nZoomToiThieu;
			if (s_fZoomDich3D > (float)s_nZoomToiDa) s_fZoomDich3D = (float)s_nZoomToiDa;
			s_nZoomDich = (int)(s_fZoomDich3D + 0.5f);
			if (s_nNhatKy) g_DebugLog("[ZOOM3D] chum d=%.0f dpx=%.0f -> dich %d%% (dang %d%%)", d, fD, s_nZoomDich, s_nZoom);
		}
	}
	else
		JxLia_ZoomDat((int)((float)s_nChumZoom0 * s_fChumD0 / d + 0.5f));	// hai ngon gan nhau (d < d0) = zoom lon = thay rong hon
	if (s_nBat && s_nNcLia)
		JxLia_Keo((x1 + x2) / 2, (y1 + y2) / 2);
}

void JxLia_ChumNha()
{
	if (!s_nChum)
		return;
	s_nChum = 0;
	if (s_nZoomKieu && s_nZoomDich > 0) s_nZoomNguoiChoi = s_nZoomDich;	// [ZOOM3D 14/09] nho dich (Nhip van troi tiep toi do)
	Camera_GhiNho();	// [CAMERA 13/09] nho zoom nguoi choi
	if (s_nNhatKy)
		g_DebugLog("[ZOOM] chum nha: zoom=%d%%", s_nZoom);
	if (s_nBat && s_nNcLia)
		JxLia_Nha();
}

#endif // JX_MOBILE
