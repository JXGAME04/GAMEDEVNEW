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
#include "../../Core/src/CoreObjGenreDef.h"	// [ANDROID 09/09 KYNANG] CGOG_NOTHING
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
// [ANDROID 09/09 KYNANG] bang nut ky nang
#define	KYNANG_TOI_DA		12		// so nut nhieu nhat cho hien
#define	KYNANG_DS_TOI_DA	65		// bang GDI_LEFT_ENABLE_SKILLS tra ve toi da 65 muc
#define	KYNANG_CACH			6		// khe ho giua hai nut
#define	KYNANG_LAM_MOI_MS	2000	// bao lau doc lai danh sach ky nang mot lan

static int			s_nKNBat = 1;
static int			s_nKNSo  = 6;
static int			s_nKNCot = 2;
static int			s_nKNCo  = 56;
static int			s_nKNX   = -1;		// -1 = tu tinh
static int			s_nKNY   = -1;

static KUiSkillData	s_KNBang[KYNANG_DS_TOI_DA];
static int			s_nKNCo1 = 0;		// so ky nang doc duoc
static unsigned int	s_uKNDocLuc = 0;
static int			s_nKNDangCam = -1;	// nut dang giu (0-based), -1 = khong
static int			s_nKNNgonX = 0, s_nKNNgonY = 0;
static int			s_nKNDichIdx = 0;	// chi so NPC dang ngam
static int			s_nKNDichX = 0, s_nKNDichY = 0;	// vi tri VE cua no

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
	// [ANDROID 09/09 KYNANG]
	s_nKNBat = GetPrivateProfileInt("Cham", "KyNang", 1, szCfg);
	s_nKNSo  = GetPrivateProfileInt("Cham", "KyNangSo", 6, szCfg);
	s_nKNCot = GetPrivateProfileInt("Cham", "KyNangCot", 2, szCfg);
	s_nKNCo  = GetPrivateProfileInt("Cham", "KyNangCo", 56, szCfg);
	s_nKNX   = GetPrivateProfileInt("Cham", "KyNangX", -1, szCfg);
	s_nKNY   = GetPrivateProfileInt("Cham", "KyNangY", -1, szCfg);
	if (s_nKNSo  < 1)  s_nKNSo  = 1;
	if (s_nKNSo  > KYNANG_TOI_DA) s_nKNSo = KYNANG_TOI_DA;
	if (s_nKNCot < 1)  s_nKNCot = 1;
	if (s_nKNCo  < 24) s_nKNCo  = 24;
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
//---------------------------------------------------------------------------
// [ANDROID 09/09 KYNANG] NUT CHON KY NANG DANH
//---------------------------------------------------------------------------

// Doc lai danh sach ky nang danh trai. Khong doc moi khung: GDI nay quet ca cay
// vo cong nen goi lien tuc la phi.
static void KyNang_DocBang()
{
	unsigned int uNay = (unsigned int)GetTickCount();

	if (g_pCoreShell == NULL)
		return;
	if (s_uKNDocLuc && uNay - s_uKNDocLuc < KYNANG_LAM_MOI_MS)
		return;
	s_uKNDocLuc = uNay;
	memset(s_KNBang, 0, sizeof(s_KNBang));
	s_nKNCo1 = g_pCoreShell->GetGameData(GDI_LEFT_ENABLE_SKILLS, (KUPARAM)&s_KNBang, 0);
	if (s_nKNCo1 < 0)
		s_nKNCo1 = 0;
	if (s_nKNCo1 > KYNANG_DS_TOI_DA)
		s_nKNCo1 = KYNANG_DS_TOI_DA;
}

// Goc trai tren cua bang nut. Mac dinh: goc phai duoi, chua len tren thanh cong cu.
static void KyNang_GocBang(int* px, int* py)
{
	int nHang = (s_nKNSo + s_nKNCot - 1) / s_nKNCot;
	int nRong = s_nKNCot * s_nKNCo + (s_nKNCot - 1) * KYNANG_CACH;
	int nCao  = nHang * s_nKNCo + (nHang - 1) * KYNANG_CACH;

	// SCREEN_WIDTH / SCREEN_HEIGHT = co khung ve that (KSdlApp dat), dung nhu JxCan_TrongVung.
	*px = (s_nKNX >= 0) ? s_nKNX : (SCREEN_WIDTH - nRong - 12);
	*py = (s_nKNY >= 0) ? s_nKNY : (SCREEN_HEIGHT - nCao - 96);
}

static void KyNang_HinhNut(int i, int* px, int* py)
{
	int nX0, nY0;

	KyNang_GocBang(&nX0, &nY0);
	*px = nX0 + (i % s_nKNCot) * (s_nKNCo + KYNANG_CACH);
	*py = nY0 + (i / s_nKNCot) * (s_nKNCo + KYNANG_CACH);
}

// So nut thuc su ve ra: khong nhieu hon so ky nang dang co.
static int KyNang_SoNutHien()
{
	int n = s_nKNSo;

	if (n > s_nKNCo1)
		n = s_nKNCo1;
	return n;
}

int JxKyNang_TrungNut(int x, int y)
{
	int i, nX, nY, n;

	DocCaiDat();
	if (!s_nKNBat)
		return 0;
	KyNang_DocBang();
	n = KyNang_SoNutHien();
	for (i = 0; i < n; i++)
	{
		KyNang_HinhNut(i, &nX, &nY);
		if (x >= nX && x < nX + s_nKNCo && y >= nY && y < nY + s_nKNCo)
			return i + 1;
	}
	return 0;
}

// Hoi Core: con dich hop nhat theo huong ngam (0,0 = khong ngam -> gan nhat).
// Tra ve chi so NPC, 0 neu khong co con nao.
static int KyNang_TimDich(int nHuongX, int nHuongY, int* pVeX, int* pVeY)
{
	KUiTargetDetailInfo tt;

	if (g_pCoreShell == NULL)
		return 0;
	memset(&tt, 0, sizeof(tt));
	tt.nViTriVeX = nHuongX;
	tt.nViTriVeY = nHuongY;
	if (!g_pCoreShell->GetGameData(NPC_OI_TARGET_INFO, (KUPARAM)&tt, 2))
		return 0;
	if (pVeX) *pVeX = tt.nViTriVeX;
	if (pVeY) *pVeY = tt.nViTriVeY;
	return tt.nChiSoNpc;
}

void JxKyNang_BatDau(int nNut, int x, int y)
{
	s_nKNDangCam = nNut - 1;
	s_nKNNgonX = x;
	s_nKNNgonY = y;
	s_nKNDichIdx = 0;
}

void JxKyNang_Keo(int x, int y)
{
	int nX, nY, dx, dy;

	if (s_nKNDangCam < 0)
		return;
	s_nKNNgonX = x;
	s_nKNNgonY = y;
	KyNang_HinhNut(s_nKNDangCam, &nX, &nY);
	dx = x - (nX + s_nKNCo / 2);
	dy = y - (nY + s_nKNCo / 2);
	// keo chua du xa thi coi nhu chua ngam - tranh rung tay lam doi muc tieu
	if (dx * dx + dy * dy < 18 * 18)
	{
		s_nKNDichIdx = 0;
		return;
	}
	s_nKNDichIdx = KyNang_TimDich(dx, dy, &s_nKNDichX, &s_nKNDichY);
}

bool JxKyNang_Nha()
{
	int nChon = s_nKNDangCam;
	int nDich = s_nKNDichIdx;

	s_nKNDangCam = -1;
	s_nKNDichIdx = 0;
	if (nChon < 0 || nChon >= s_nKNCo1 || g_pCoreShell == NULL)
		return false;
	if (s_KNBang[nChon].uGenre == CGOG_NOTHING)
		return false;

	// Chon lam ky nang danh TRAI - y het ban PC (UiSkillTree.cpp:156), nho vay o
	// ky nang tren thanh trang thai cung doi theo va lan sau cham thang vao dich
	// la danh bang dung ky nang nay.
	g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNBang[nChon], 0);

	// Khong ngam duoc con nao trong huong keo thi danh con gan nhat.
	if (nDich == 0)
		nDich = KyNang_TimDich(0, 0, NULL, NULL);
	if (nDich == 0)
		return true;		// khong co dich: van coi la da xu ly (da doi ky nang)
	g_pCoreShell->LockSomeoneUseSkill(nDich, (int)s_KNBang[nChon].uId);
	return true;
}

// Vong tron duoi chan con dang ngam - dung anh vong DICH nhu JxVongChon_Ve.
static void KyNang_VeVongDich(int nVeX, int nVeY)
{
	static KRUImage s_VongNgam;
	KRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };
	int nLuiX = 0, nLuiY = 0;

	if (s_nVongCoAnh < 0)
		s_nVongCoAnh = (CoAnh(s_szVongAnh) && CoAnh(s_szVongAnhDich)) ? 1 : 0;
	if (!s_nVongCoAnh)
		return;
	if (s_VongNgam.szImage[0] == 0)
	{
		memset(&s_VongNgam, 0, sizeof(s_VongNgam));
		s_VongNgam.nType = ISI_T_SPR;
		s_VongNgam.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		s_VongNgam.Color.Color_dw = 0xffffffff;
		s_VongNgam.nISPosition = IMAGE_IS_POSITION_INIT;
		s_VongNgam.nFrame = 0;
		strncpy(s_VongNgam.szImage, s_szVongAnhDich, sizeof(s_VongNgam.szImage) - 1);
	}
	if (g_pRepresentShell->GetImageFrameParam(s_VongNgam.szImage, 0, &oOff, &oCo, s_VongNgam.nType)
		&& oCo.nX > 0)
	{
		nLuiX = oCo.nX / 2;
		nLuiY = oCo.nY / 2;
	}
	s_VongNgam.oPosition.nX = nVeX - nLuiX;
	s_VongNgam.oPosition.nY = nVeY - nLuiY;
	// FALSE = toa do THE GIOI
	g_pRepresentShell->DrawPrimitives(1, &s_VongNgam, RU_T_IMAGE, false);
}

void JxKyNang_Ve()
{
	int i, n, nX, nY;

	DocCaiDat();
	if (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	KyNang_DocBang();
	n = KyNang_SoNutHien();
	for (i = 0; i < n; i++)
	{
		KyNang_HinhNut(i, &nX, &nY);
		// nen mo cho thay ranh o nut, dam hon khi dang giu
		OVuong(nX + s_nKNCo / 2, nY + s_nKNCo / 2, s_nKNCo / 2,
			(i == s_nKNDangCam) ? 0xB03A8A3A : 0x80202020);
		g_pCoreShell->DrawGameObj(s_KNBang[i].uGenre, s_KNBang[i].uId,
			nX, nY, s_nKNCo, s_nKNCo, 0);
	}

	// Dang ngam: vach chi huong tu nut toi ngon tay + vong tron duoi chan con dich.
	if (s_nKNDangCam >= 0)
	{
		KRULine oVach;

		KyNang_HinhNut(s_nKNDangCam, &nX, &nY);
		oVach.oPosition.nX = nX + s_nKNCo / 2;
		oVach.oPosition.nY = nY + s_nKNCo / 2;
		oVach.oEndPos.nX   = s_nKNNgonX;
		oVach.oEndPos.nY   = s_nKNNgonY;
		oVach.Color.Color_dw = s_nKNDichIdx ? 0xFFFF6666 : 0xA0FFFFFF;
		g_pRepresentShell->DrawPrimitives(1, &oVach, RU_T_LINE, true);

		if (s_nKNDichIdx)
			KyNang_VeVongDich(s_nKNDichX, s_nKNDichY);
	}
}

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
