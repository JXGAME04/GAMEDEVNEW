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
#include "../../Core/src/GameDataDef.h"
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
static int	s_nDaBaoAnh = 0;
static int	s_nCoAnh = -1;		// -1 = chua kiem, 0 = khong co anh (ve o mau), 1 = co anh
static char	s_szAnhNen[128] = "\\spr\\Ui3\\UiSkillControl\\joystick_bg.spr";
static char	s_szAnhNum[128] = "\\spr\\Ui3\\UiSkillControl\\joystick_ctrl.spr";
static int	s_nVongBat = 1;
static int	s_nVongCoAnh = -1;
static char	s_szVongAnh[128]    = "\\spr\\npcres\\focused_non_enemy_circle.spr";
static char	s_szVongAnhDich[128] = "\\spr\\npcres\\focused_enemy_circle.spr";
static int	s_nIconBat = 1;
static int	s_nIconCoAnh = -1;
static int	s_nIconCao = 62;	// icon cao hon chan NPC bao nhieu diem anh
static char	s_szIconAnh[128] = "\\spr\\obj\\box\\YellowPoint.spr";

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
	GetPrivateProfileString("Cham", "CanAnhNen", s_szAnhNen, s_szAnhNen, sizeof(s_szAnhNen), szCfg);
	GetPrivateProfileString("Cham", "CanAnhNum", s_szAnhNum, s_szAnhNum, sizeof(s_szAnhNum), szCfg);
	s_nVongBat   = GetPrivateProfileInt("Cham", "VongChon", 1, szCfg);
	GetPrivateProfileString("Cham", "VongChonAnh", s_szVongAnh, s_szVongAnh, sizeof(s_szVongAnh), szCfg);
	GetPrivateProfileString("Cham", "VongChonAnhDich", s_szVongAnhDich, s_szVongAnhDich, sizeof(s_szVongAnhDich), szCfg);
	s_nIconBat   = GetPrivateProfileInt("Cham", "IconNpc", 1, szCfg);
	s_nIconCao   = GetPrivateProfileInt("Cham", "IconNpcCao", 62, szCfg);
	GetPrivateProfileString("Cham", "IconNpcAnh", s_szIconAnh, s_szIconAnh, sizeof(s_szIconAnh), szCfg);
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
// Ve can dieu khien.
//
// Uu tien ANH THAT: chu da co san bo anh giao dien mobile VNKU, trong do co
//     \spr\Ui3\UiSkillControl\joystick_bg.spr    (than can)
//     \spr\Ui3\UiSkillControl\joystick_ctrl.spr  (num)
// Anh o dung dinh dang .spr cua engine nay nen ve thang duoc, khong phai doi gi.
// Neu thieu anh (chua chep vao thu muc du lieu) thi lui ve ve o mau trong suot -
// van dung duoc, chi la xau hon.
//
// Doi anh khac: config.ini [Cham] CanAnhNen / CanAnhNum (duong dan .spr).
//---------------------------------------------------------------------------
static void VeAnh(const char* pszAnh, int nX, int nY)
{
	KRUImage a;
	memset(&a, 0, sizeof(a));
	a.nType = ISI_T_SPR;
	a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	a.Color.Color_dw = 0xffffffff;
	a.nISPosition = IMAGE_IS_POSITION_INIT;
	a.nFrame = 0;
	strncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);
	// Anh .spr co diem neo rieng; lay co khung de dat tam anh vao dung cho ngon tay.
	KRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };
	if (g_pRepresentShell->GetImageFrameParam(a.szImage, 0, &oOff, &oCo, a.nType) && oCo.nX > 0)
	{
		a.oPosition.nX = nX - oCo.nX / 2;
		a.oPosition.nY = nY - oCo.nY / 2;
	}
	else
	{
		a.oPosition.nX = nX;
		a.oPosition.nY = nY;
	}
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE, true);
}

static bool CoAnh(const char* pszAnh)
{
	if (!pszAnh || !pszAnh[0] || !g_pRepresentShell)
		return false;
	KRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };
	return g_pRepresentShell->GetImageFrameParam((char*)pszAnh, 0, &oOff, &oCo, ISI_T_SPR) && oCo.nX > 0;
}

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


//---------------------------------------------------------------------------
// [ANDROID 09/09 VONG] VONG CHON DUOI CHAN MUC TIEU
//
// Chu: "kich vao doi tuong la co vong tron duoi chan doi tuong nham co dinh lai de hien thong tin".
//
// Ve bang DrawPrimitives voi bSinglePlaneCoord = FALSE, tuc la dua TOA DO THE GIOI - Represent3 tu
// doi sang toa do man hinh, nen vong luon nam dung duoi chan du man hinh cuon di dau.
// Vi tri do Core tra ve trong KUiTargetDetailInfo (NPC_OI_TARGET_INFO), xem dot va 20.
//
// config.ini [Cham]:
//   VongChon=1                 ; 0 = tat
//   VongChonAnh=\spr\npcres\focused_non_enemy_circle.spr
//   VongChonAnhDich=\spr\npcres\focused_enemy_circle.spr   (dung khi muc tieu dang bi khoa danh)
//---------------------------------------------------------------------------

void JxVongChon_Ve()
{
	DocCaiDat();
	if (!s_nVongBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	KUiTargetDetailInfo tt;
	memset(&tt, 0, sizeof(tt));
	if (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&tt, 0))
		return;
	if (tt.sTargetName[0] == 0)
		return;
	if (s_nVongCoAnh < 0)
	{
		s_nVongCoAnh = (CoAnh(s_szVongAnh) && CoAnh(s_szVongAnhDich)) ? 1 : 0;
		g_DebugLog("[VONG] anh vong chon: %s -> co anh=%d", s_szVongAnh, s_nVongCoAnh);
	}
	if (!s_nVongCoAnh)
		return;
	static KRUImage s_Vong, s_VongDich;
	KRUImage& a = tt.nDangKhoa ? s_VongDich : s_Vong;
	const char* pszAnh = tt.nDangKhoa ? s_szVongAnhDich : s_szVongAnh;
	if (a.szImage[0] == 0)
	{
		memset(&a, 0, sizeof(a));
		a.nType = ISI_T_SPR;
		a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		a.Color.Color_dw = 0xffffffff;
		a.nISPosition = IMAGE_IS_POSITION_INIT;
		a.nFrame = 0;
		strncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);
	}
	// Anh .spr ve tu goc tren-trai cua no nen phai lui lai nua khung, khong thi vong nam lech
	// xuong duoi ben phai chan doi tuong (da nhin tan mat o ban dung dau tien).
	KRPosition2 oOffV = { 0, 0 }, oCoV = { 0, 0 };
	int nLuiX = 0, nLuiY = 0;
	if (g_pRepresentShell->GetImageFrameParam(a.szImage, 0, &oOffV, &oCoV, a.nType) && oCoV.nX > 0)
	{
		nLuiX = oCoV.nX / 2;
		nLuiY = oCoV.nY / 2;
	}
	a.oPosition.nX = tt.nViTriVeX - nLuiX;
	a.oPosition.nY = tt.nViTriVeY - nLuiY;
	// FALSE = toa do THE GIOI (khong phai toa do man hinh) -> Represent3 tu dat dung cho
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE, false);
}


//---------------------------------------------------------------------------
// [ANDROID 09/09 ICON] ICON "NOI CHUYEN" TREN DAU NPC DOI THOAI GAN NHAT
//
// Chu: "toi gan npc nao phai hien icon de kich vao chon doi thoai hay khong". Cham vao NPC thi thoai
// da mo duoc san, nen icon nay de nguoi choi BIET cho nao cham duoc - va vi no nam ngay tren dau NPC
// nen cham vao icon cung la cham trung NPC.
//
// Core tra vi tri NPC theo toa do THE GIOI (NPC_OI_TARGET_INFO voi nParam = 1); doi sang toa do man
// hinh bang CoordinateTransform roi ve cao hon dau mot chut. NPC ra ngoai khung ve thi khong ve -
// tuc la chi hien cho NPC dang nhin thay.
//
// config.ini [Cham]: IconNpc=1 / IconNpcAnh / IconNpcCao (cao hon chan NPC bao nhieu diem anh)
//---------------------------------------------------------------------------
void JxIconNpc_Ve()
{
	DocCaiDat();
	if (!s_nIconBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	KUiTargetDetailInfo gan;
	memset(&gan, 0, sizeof(gan));
	if (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&gan, 1))
		return;
	if (gan.sTargetName[0] == 0)
		return;
	if (s_nIconCoAnh < 0)
	{
		s_nIconCoAnh = CoAnh(s_szIconAnh) ? 1 : 0;
		g_DebugLog("[ICON] anh icon NPC: %s -> co anh=%d", s_szIconAnh, s_nIconCoAnh);
	}
	if (!s_nIconCoAnh)
		return;
	int x = gan.nViTriVeX, y = gan.nViTriVeY;
	g_pRepresentShell->CoordinateTransform(x, y, 0);	// the gioi -> man hinh
	y -= s_nIconCao;
	if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
		return;		// NPC ra ngoai khung ve
	static KRUImage s_Icon;
	if (s_Icon.szImage[0] == 0)
	{
		memset(&s_Icon, 0, sizeof(s_Icon));
		s_Icon.nType = ISI_T_SPR;
		s_Icon.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		s_Icon.Color.Color_dw = 0xffffffff;
		s_Icon.nISPosition = IMAGE_IS_POSITION_INIT;
		s_Icon.nFrame = 0;
		strncpy(s_Icon.szImage, s_szIconAnh, sizeof(s_Icon.szImage) - 1);
	}
	KRPosition2 oOffI = { 0, 0 }, oCoI = { 0, 0 };
	int nLui = 0;
	if (g_pRepresentShell->GetImageFrameParam(s_Icon.szImage, 0, &oOffI, &oCoI, s_Icon.nType) && oCoI.nX > 0)
		nLui = oCoI.nX / 2;
	s_Icon.oPosition.nX = x - nLui;
	s_Icon.oPosition.nY = y;
	g_pRepresentShell->DrawPrimitives(1, &s_Icon, RU_T_IMAGE, true);	// true = toa do MAN HINH
}

void JxCan_Ve()
{
	if (!s_bCam || g_pRepresentShell == NULL)
		return;
	// num keo theo ngon tay nhung khong ra khoi ban kinh
	int dx = s_nNgonX - s_nTamX, dy = s_nNgonY - s_nTamY;
	double d = sqrt((double)(dx * dx + dy * dy));
	if (d > s_nBanKinh / 2)
	{
		dx = (int)(dx * (s_nBanKinh / 2) / d);
		dy = (int)(dy * (s_nBanKinh / 2) / d);
	}
	if (s_nCoAnh < 0)
		s_nCoAnh = (CoAnh(s_szAnhNen) && CoAnh(s_szAnhNum)) ? 1 : 0;
	if (s_nCoAnh >= 0 && !s_nDaBaoAnh) { s_nDaBaoAnh = 1; g_DebugLog("[CAN] anh nen=%s num=%s -> dung anh=%d", s_szAnhNen, s_szAnhNum, s_nCoAnh); }
	if (s_nCoAnh)
	{
		VeAnh(s_szAnhNen, s_nTamX, s_nTamY);
		VeAnh(s_szAnhNum, s_nTamX + dx, s_nTamY + dy);
		return;
	}
	OVuong(s_nTamX, s_nTamY, s_nBanKinh / 2, 0x30202020);
	OVuong(s_nTamX + dx, s_nTamY + dy, s_nBanKinh / 6, 0x60d0c090);
}

#endif // JX_ANDROID
