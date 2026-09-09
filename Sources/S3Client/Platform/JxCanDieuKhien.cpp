//---------------------------------------------------------------------------
// [ANDROID 09/09 CAN] Can dieu khien ao - xem JxCanDieuKhien.h.
//
// Cach lam lay theo ban JX1 Mobile cua chu (D:\USVOLAM\Jx1mClientMobile, lop HRocker):
//   - moi khung, khi ngon tay lech khoi tam qua mot nguong nho, goi iCoreShell::Goto(nDir, 0);
//   - nDir la 1 trong 8 huong cua vong 64 huong:
//         24  32  40         (trai-len, len, phai-len)
//         16      48         (trai,          phai)
//          8   0  56         (trai-xuong, xuong, phai-xuong)
//   - luc bat dau keo thi go muc tieu dang khoa (LockSomeoneAction/LockObjectAction) de nhan vat
//     chiu roi cho ma di, neu khong dang danh thi no cu dung yen.
//
// Khac ban Mobile: ho co anh rieng cho can, ban nay ve bang KRUShadow (o mau trong suot) nen
// KHONG can them tep anh nao - hop voi mot ban port giu nguyen giao dien PC.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_ANDROID
#include "JxCanDieuKhien.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "../../Core/src/coreshell.h"
#include "KDebug.h"
#include <math.h>

extern iCoreShell*			g_pCoreShell;
extern iRepresentShell*		g_pRepresentShell;
extern int					SCREEN_WIDTH;
extern int					SCREEN_HEIGHT;

// --- cai dat, doc mot lan tu config.ini [Cham] -----------------------------
static int	s_nDaDoc = 0;
static int	s_nBat = 1;			// 0 = tat han can dieu khien
static int	s_nVungRong = 45;	// % chieu ngang man hinh tinh tu bien TRAI
static int	s_nVungTren = 25;	// % chieu cao: tu day tro xuong moi la vung can
static int	s_nVungDuoi = 88;	// % chieu cao: qua day la thanh cong cu, khong lay
static int	s_nBanKinh = 90;		// ban kinh can, tinh theo khung ve
static int	s_nNguong = 14;		// lech qua bao nhieu diem anh thi bat dau di

// --- trang thai --------------------------------------------------------------
static bool	s_bCam = false;
static int	s_nTamX = 0, s_nTamY = 0;	// tam can (cho dat ngon)
static int	s_nNgonX = 0, s_nNgonY = 0;	// cho ngon dang o
static int	s_nHuong = 0;				// nDir 0..63

static void DocCaiDat()
{
	if (s_nDaDoc)
		return;
	s_nDaDoc = 1;
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	s_nBat      = GetPrivateProfileInt("Cham", "CanDieuKhien", 1, szCfg);
	s_nVungRong = GetPrivateProfileInt("Cham", "CanVungRong", 45, szCfg);
	s_nVungTren = GetPrivateProfileInt("Cham", "CanVungTren", 25, szCfg);
	s_nVungDuoi = GetPrivateProfileInt("Cham", "CanVungDuoi", 88, szCfg);
	s_nBanKinh  = GetPrivateProfileInt("Cham", "CanBanKinh", 90, szCfg);
	s_nNguong   = GetPrivateProfileInt("Cham", "CanNguong", 14, szCfg);
	if (s_nVungRong < 10) s_nVungRong = 10;
	if (s_nVungRong > 100) s_nVungRong = 100;
	if (s_nBanKinh < 30) s_nBanKinh = 30;
	if (s_nNguong < 4) s_nNguong = 4;
	g_DebugLog("[CAN] can dieu khien: bat=%d vung=%d%% x %d..%d%% ban kinh=%d nguong=%d",
		s_nBat, s_nVungRong, s_nVungTren, s_nVungDuoi, s_nBanKinh, s_nNguong);
}

bool JxCan_TrongVung(int x, int y)
{
	DocCaiDat();
	if (!s_nBat)
		return false;
	return x >= 0 && x < SCREEN_WIDTH * s_nVungRong / 100
		&& y >= SCREEN_HEIGHT * s_nVungTren / 100
		&& y <  SCREEN_HEIGHT * s_nVungDuoi / 100;
}

bool JxCan_DangCam()
{
	return s_bCam;
}

// Goc tren man hinh (do, 0 = sang PHAI, tang nguoc chieu kim dong ho) -> nDir 0..63.
// Bang huong lay dung cua ban JX1 Mobile (HRocker::onTouchMoved).
static int GocSangHuong(float fGoc)
{
	if (fGoc < 0) fGoc += 360.0f;
	if (fGoc >= 337.5f || fGoc <  22.5f) return 48;	// phai
	if (fGoc <  67.5f)                   return 40;	// phai len
	if (fGoc < 112.5f)                   return 32;	// len
	if (fGoc < 157.5f)                   return 24;	// trai len
	if (fGoc < 202.5f)                   return 16;	// trai
	if (fGoc < 247.5f)                   return 8;	// trai xuong
	if (fGoc < 292.5f)                   return 0;	// xuong
	return 56;										// phai xuong
}

static void TinhHuong()
{
	int dx = s_nNgonX - s_nTamX;
	int dy = s_nNgonY - s_nTamY;		// khung ve: y TANG khi xuong duoi
	// atan2 muon y TANG khi len tren -> dao dau dy
	float fGoc = (float)(atan2((double)(-dy), (double)dx) * 180.0 / 3.14159265358979);
	s_nHuong = GocSangHuong(fGoc);
}

void JxCan_BatDau(int x0, int y0, int x, int y)
{
	DocCaiDat();
	if (!s_nBat)
		return;
	s_bCam = true;
	s_nTamX = x0; s_nTamY = y0;
	s_nNgonX = x; s_nNgonY = y;
	TinhHuong();
	// Go muc tieu dang khoa: dang danh ai thi nhan vat khong chiu roi cho, phai go moi di duoc.
	// (Ban JX1 Mobile lam dung cho nay trong HRocker::onTouchMoved.)
	if (g_pCoreShell)
	{
		g_pCoreShell->LockSomeoneAction(0);
		g_pCoreShell->LockObjectAction(0);
	}
}

void JxCan_Keo(int x, int y)
{
	if (!s_bCam)
		return;
	s_nNgonX = x; s_nNgonY = y;
	TinhHuong();
}

void JxCan_Nha()
{
	s_bCam = false;
}

void JxCan_Nhip()
{
	if (!s_bCam || !g_pCoreShell)
		return;
	int dx = s_nNgonX - s_nTamX;
	int dy = s_nNgonY - s_nTamY;
	if (dx * dx + dy * dy < s_nNguong * s_nNguong)
		return;		// ngon tay con o giua can -> dung yen
	g_pCoreShell->Goto(s_nHuong, 0);	// mode 0 = tu chon di bo / chay nhu ban PC
}

//---------------------------------------------------------------------------
// Ve can: mot o vuong to (than can) + mot o vuong nho (num) o phia ngon tay.
// Dung KRUShadow (o mau trong suot) nen khong can tep anh nao.
//---------------------------------------------------------------------------
static void OVuong(int nX, int nY, int nNua, unsigned int uMau)
{
	KRUShadow o;
	o.oPosition.nX = nX - nNua;
	o.oPosition.nY = nY - nNua;
	o.oEndPos.nX   = nX + nNua;
	o.oEndPos.nY   = nY + nNua;
	o.Color.Color_dw = uMau;
	g_pRepresentShell->DrawPrimitives(1, &o, RU_T_SHADOW, true);
}

void JxCan_Ve()
{
	if (!s_bCam || g_pRepresentShell == NULL)
		return;
	// than can
	OVuong(s_nTamX, s_nTamY, s_nBanKinh / 2, 0x30202020);
	// num: keo theo ngon tay nhung khong ra khoi ban kinh
	int dx = s_nNgonX - s_nTamX, dy = s_nNgonY - s_nTamY;
	double d = sqrt((double)(dx * dx + dy * dy));
	if (d > s_nBanKinh / 2)
	{
		dx = (int)(dx * (s_nBanKinh / 2) / d);
		dy = (int)(dy * (s_nBanKinh / 2) / d);
	}
	OVuong(s_nTamX + dx, s_nTamY + dy, s_nBanKinh / 6, 0x60d0c090);
}

#endif // JX_ANDROID
