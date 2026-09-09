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
// [ANDROID 09/09 KYNANG C] Nut danh chinh + 8 o ky nang phu xep thanh CUNG
// quanh no - bo cuc lay dung cua ban JX1 Mobile tham khao (KgameWorld.cpp:13877,
// KgameWorldVN.cpp:1688 "vong tron skill quanh mainskill").
#define	KYNANG_SO_PHU		8		// ban tham khao: MAX_FUZHUSKILL_COUNT = 8
#define	KYNANG_DS_TOI_DA	65		// GDI_LEFT_ENABLE_SKILLS tra ve toi da 65 muc
#define	KYNANG_LAM_MOI_MS	2000	// bao lau doc lai danh sach ky nang mot lan
#define	KYNANG_NGUONG_NGAM	18		// keo qua bao nhieu diem anh moi tinh la ngam

//	Do lech tam tung o, tinh tu goc PHAI DUOI man hinh (chua cong ban kinh o).
//	Lay dung so cua ban tham khao: KgameWorld.cpp:13877 (o 0..4, cung trong)
//	va :13908 (o 5..7, cung ngoai).
static const int	s_nKNDX[KYNANG_SO_PHU] = { 215, 195, 160, 100,  35, 255, 220, 155 };
static const int	s_nKNDY[KYNANG_SO_PHU] = {  -5,  60, 115, 145, 115,  40, 115, 180 };
#define	KYNANG_CHINH_DX		105		// nut danh chinh: KgameWorldVN.cpp:804
#define	KYNANG_CHINH_DY		45

static int			s_nKNBat = 1;
static int			s_nKNCoChinh = 0;	// 0 = lay theo co that cua anh
static int			s_nKNCoPhu   = 0;
// [ANDROID 09/09 KYNANG D] Ban tham khao thay HAN thanh cong cu duoi day nen dat
// cum sat goc phai duoi duoc. Ban nay CON GIU thanh cong cu + qua cau Bao Vat +
// cot icon phai cua ban PC, nen phai day cum len va sang phai cho khoi de len.
static int			s_nKNLenTren = 80;	// nang ca cum len bao nhieu diem anh
static int			s_nKNSangPhai = 60;	// doi ca cum sang phai bao nhieu diem anh
static int			s_nKNCoIcon = 32;	// co THAT cua bieu tuong ky nang (DrawSkillIcon bo qua Width/Height)
static int			s_nKNX   = -1;		// -1 = neo goc phai duoi nhu ban tham khao
static int			s_nKNY   = -1;

//	Bo anh THAT cua VNKU - chu da chi san (\spr\Ui3\UiSkillControl)
static char		s_szKNAnhChinh[128] = "\\spr\\Ui3\\UiSkillControl\\assign_skill_100x100.spr";
static char		s_szKNAnhPhu[128]   = "\\spr\\Ui3\\UiSkillControl\\assign_skill_70x70.spr";
static char		s_szKNAnhNgam[128]  = "\\spr\\Ui3\\UiSkillControl\\effect_skill.spr";
static int			s_nKNCoAnh = -1;	// -1 = chua kiem

static KUiSkillData	s_KNBang[KYNANG_DS_TOI_DA];
static int			s_nKNCo1 = 0;		// so ky nang doc duoc
static unsigned int	s_uKNDocLuc = 0;
static KUiGameObject s_KNChinh;		// ky nang danh TRAI dang hien tren thanh trang thai
static int			s_nKNDangCam = -1;	// 0 = nut chinh, 1..8 = o phu; -1 = khong giu
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
	// [ANDROID 09/09 KYNANG C]
	s_nKNBat     = GetPrivateProfileInt("Cham", "KyNang", 1, szCfg);
	s_nKNCoChinh = GetPrivateProfileInt("Cham", "KyNangCo", 0, szCfg);
	s_nKNCoPhu   = GetPrivateProfileInt("Cham", "KyNangCoPhu", 0, szCfg);
	s_nKNLenTren = GetPrivateProfileInt("Cham", "KyNangLenTren", 80, szCfg);
	s_nKNSangPhai = GetPrivateProfileInt("Cham", "KyNangSangPhai", 60, szCfg);
	s_nKNCoIcon  = GetPrivateProfileInt("Cham", "KyNangCoIcon", 32, szCfg);
	if (s_nKNCoIcon < 8) s_nKNCoIcon = 8;
	s_nKNX       = GetPrivateProfileInt("Cham", "KyNangX", -1, szCfg);
	s_nKNY       = GetPrivateProfileInt("Cham", "KyNangY", -1, szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhChinh", s_szKNAnhChinh, s_szKNAnhChinh,
		sizeof(s_szKNAnhChinh), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhPhu", s_szKNAnhPhu, s_szKNAnhPhu,
		sizeof(s_szKNAnhPhu), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhNgam", s_szKNAnhNgam, s_szKNAnhNgam,
		sizeof(s_szKNAnhNgam), szCfg);
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
// [ANDROID 09/09 KYNANG C] NUT DANH CHINH + 8 O KY NANG PHU XEP THANH CUNG
//
// Bo cuc va anh lay dung cua ban JX1 Mobile tham khao - xem android/va_nguon_android_38.py.
//---------------------------------------------------------------------------

//	Co THAT cua mot anh .spr (de neo o theo dung ban kinh cua no).
static int KyNang_CoAnh(const char* pszAnh, int nMacDinh)
{
	KRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };

	if (g_pRepresentShell
		&& g_pRepresentShell->GetImageFrameParam((char*)pszAnh, 0, &oOff, &oCo, ISI_T_SPR)
		&& oCo.nX > 0)
		return oCo.nX;
	return nMacDinh;
}

static int KyNang_CoChinh()
{
	return s_nKNCoChinh > 0 ? s_nKNCoChinh : KyNang_CoAnh(s_szKNAnhChinh, 100);
}

static int KyNang_CoPhu()
{
	return s_nKNCoPhu > 0 ? s_nKNCoPhu : KyNang_CoAnh(s_szKNAnhPhu, 70);
}

//	Tam cua mot o, toa do MAN HINH. nNut: 0 = nut danh chinh, 1..8 = o phu.
static void KyNang_TamNut(int nNut, int* px, int* py)
{
	int nR, nDX, nDY;

	if (nNut <= 0)
	{
		nR = KyNang_CoChinh() / 2;
		nDX = KYNANG_CHINH_DX;
		nDY = KYNANG_CHINH_DY;
	}
	else
	{
		nR = KyNang_CoPhu() / 2;
		nDX = s_nKNDX[nNut - 1];
		nDY = s_nKNDY[nNut - 1];
	}
	// Neo goc PHAI DUOI y nhu ban tham khao. KyNangX/Y (neu dat) doi ca cum di.
	*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + (s_nKNX >= 0 ? s_nKNX : 0);
	*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + (s_nKNY >= 0 ? s_nKNY : 0);
}

//	Doc lai danh sach ky nang danh + ky nang danh trai dang dung.
//	Khong doc moi khung: GDI nay quet ca cay vo cong nen goi lien tuc la phi.
static void KyNang_DocBang()
{
	unsigned int uNay = (unsigned int)GetTickCount();
	KUiPlayerImmedItemSkill oTay;

	if (g_pCoreShell == NULL)
		return;
	if (s_uKNDocLuc && uNay - s_uKNDocLuc < KYNANG_LAM_MOI_MS)
		return;
	s_uKNDocLuc = uNay;

	memset(s_KNBang, 0, sizeof(s_KNBang));
	s_nKNCo1 = g_pCoreShell->GetGameData(GDI_LEFT_ENABLE_SKILLS, (KUPARAM)&s_KNBang, 0);
	if (s_nKNCo1 < 0)				s_nKNCo1 = 0;
	if (s_nKNCo1 > KYNANG_DS_TOI_DA)	s_nKNCo1 = KYNANG_DS_TOI_DA;

	// Nut danh chinh dung ky nang danh TRAI dang hien tren thanh trang thai -
	// dung thu ma nut mainskill cua ban tham khao dung.
	memset(&oTay, 0, sizeof(oTay));
	g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (KNPARAM)&oTay, 0);
	s_KNChinh = oTay.IMmediaSkill[0];
}

//	Ky nang gan cho mot o. Tra ve false neu o trong.
static bool KyNang_CuaNut(int nNut, KUiGameObject* pRa)
{
	if (nNut <= 0)
	{
		*pRa = s_KNChinh;
		return (pRa->uGenre != CGOG_NOTHING && pRa->uId != 0);
	}
	if (nNut - 1 >= s_nKNCo1)
	{
		memset(pRa, 0, sizeof(*pRa));
		return false;
	}
	pRa->uGenre = s_KNBang[nNut - 1].uGenre;
	pRa->uId    = s_KNBang[nNut - 1].uId;
	return (pRa->uGenre != CGOG_NOTHING && pRa->uId != 0);
}

int JxKyNang_TrungNut(int x, int y)
{
	int i, nX, nY, nR;

	DocCaiDat();
	if (!s_nKNBat)
		return 0;
	KyNang_DocBang();
	// Xet o phu TRUOC roi moi den nut chinh: cung o phu vong sat nut chinh, uu tien
	// o nho de cham vao ria cung khong bi nut to nuot mat.
	for (i = KYNANG_SO_PHU; i >= 0; i--)
	{
		KUiGameObject o;

		if (!KyNang_CuaNut(i, &o))
			continue;		// o trong thi khong bat cham
		KyNang_TamNut(i, &nX, &nY);
		nR = (i <= 0 ? KyNang_CoChinh() : KyNang_CoPhu()) / 2;
		// khung tron -> do theo BAN KINH, khong phai hinh vuong
		if ((x - nX) * (x - nX) + (y - nY) * (y - nY) <= nR * nR)
			return i + 1;
	}
	return 0;
}

//	Hoi Core: con dich hop nhat theo huong ngam (0,0 = khong ngam -> gan nhat).
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
	s_nKNDangCam = nNut - 1;	// 0 = nut chinh, 1..8 = o phu
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
	KyNang_TamNut(s_nKNDangCam, &nX, &nY);
	dx = x - nX;
	dy = y - nY;
	// Keo chua du xa = chua ngam (tranh rung tay lam doi muc tieu). Ban tham khao
	// cung co vung chet nay (KuiMyMenu m_ptJXTouch0 / m_bJXAimKeo).
	if (dx * dx + dy * dy < KYNANG_NGUONG_NGAM * KYNANG_NGUONG_NGAM)
	{
		s_nKNDichIdx = 0;
		return;
	}
	s_nKNDichIdx = KyNang_TimDich(dx, dy, &s_nKNDichX, &s_nKNDichY);
}

bool JxKyNang_Nha()
{
	int nNut = s_nKNDangCam;
	int nDich = s_nKNDichIdx;
	KUiGameObject o;

	s_nKNDangCam = -1;
	s_nKNDichIdx = 0;
	if (nNut < 0 || g_pCoreShell == NULL)
		return false;
	if (!KyNang_CuaNut(nNut, &o))
		return false;

	// O phu: dat luon lam ky nang danh TRAI (GOI_SET_IMMDIA_SKILL, y het
	// UiSkillTree.cpp:156) de o tren thanh trang thai doi theo va lan sau cham
	// thang vao dich la danh bang dung ky nang nay. Nut chinh thi khong can -
	// no VON dung ky nang danh trai.
	if (nNut > 0)
		g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);

	// Khong ngam duoc con nao trong huong keo thi danh con gan nhat (= kieu 0
	// cua ban tham khao: cham la danh, tu chon muc tieu).
	if (nDich == 0)
		nDich = KyNang_TimDich(0, 0, NULL, NULL);
	if (nDich == 0)
		return true;		// khong co dich: van coi la da xu ly
	g_pCoreShell->LockSomeoneUseSkill(nDich, (int)o.uId);
	return true;
}

//	Vong tron duoi chan con dang ngam - dung anh vong DICH nhu JxVongChon_Ve.
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
	int i, nX, nY, nR, nIcon;
	KUiGameObject o;

	DocCaiDat();
	if (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	if (s_nKNCoAnh < 0)
	{
		s_nKNCoAnh = (CoAnh(s_szKNAnhChinh) && CoAnh(s_szKNAnhPhu)) ? 1 : 0;
		g_DebugLog("[KYNANG] anh nut: %s -> co anh=%d", s_szKNAnhChinh, s_nKNCoAnh);
	}
	KyNang_DocBang();

	// Ve tu o phu ra ngoai roi den nut chinh, de nut chinh nam TREN CUNG.
	for (i = KYNANG_SO_PHU; i >= 0; i--)
	{
		bool bCo = KyNang_CuaNut(i, &o);

		if (!bCo && i > 0)
			continue;		// o phu trong thi khong ve gi ca
		KyNang_TamNut(i, &nX, &nY);
		nR = (i <= 0 ? KyNang_CoChinh() : KyNang_CoPhu()) / 2;

		// khung tron cua VNKU; thieu anh thi lui ve o mau cho van dung duoc
		if (s_nKNCoAnh)
			VeAnh(i <= 0 ? s_szKNAnhChinh : s_szKNAnhPhu, nX, nY);
		else
			OVuong(nX, nY, nR, (i == s_nKNDangCam) ? 0xB03A8A3A : 0x80202020);

		// [ANDROID 09/09 KYNANG D] KSkill::DrawSkillIcon (KSkills.cpp:2861) BO QUA
		// Width/Height - no ve anh o co THAT, lay (x, y) lam goc TRAI TREN. Nen phai
		// tu canh giua theo co that (KyNangCoIcon), khong the nho ham do co lai.
		if (bCo)
		{
			nIcon = s_nKNCoIcon;
			g_pCoreShell->DrawGameObj(o.uGenre, o.uId,
				nX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);
		}
	}

	// Dang giu mot nut: vong sang ngam + vach chi huong + vong duoi chan con dich.
	if (s_nKNDangCam >= 0)
	{
		KRULine oVach;

		KyNang_TamNut(s_nKNDangCam, &nX, &nY);
		if (CoAnh(s_szKNAnhNgam))
			VeAnh(s_szKNAnhNgam, nX, nY);	// effect_skill.spr cua VNKU

		oVach.oPosition.nX = nX;
		oVach.oPosition.nY = nY;
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
