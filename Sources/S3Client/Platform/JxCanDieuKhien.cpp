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
#include "../Ui/Elem/UiToaDo.h"	// [UITOADO 09/09 E] dang ky o ve tay
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "../../Core/src/coreshell.h"
#include "../../Core/src/GameDataDef.h"
#include "../../Core/src/CoreObjGenreDef.h"	// [ANDROID 09/09 KYNANG] CGOG_NOTHING
#include "KDebug.h"
#include "../Ui/ShortcutKey.h"	// [ANDROID 10/09 OTRONG] mo bang ky nang khi cham o trong
#include "../Ui/UiCase/UiSkillsNew.h"	// [ANDROID 10/09 BANGCHON] tu dong bang chon khi cua so ky nang dong
#include "../Ui/UiCase/UiSkills.h"
#include "../Ui/Elem/MouseHover.h"	// [ANDROID 10/09 LUAN] dat 4 nut canh khung thong tin ky nang
#include <math.h>
#include <stdint.h>	// [UITOADO 10/09 F] intptr_t

extern iCoreShell*			g_pCoreShell;
extern iRepresentShell*		g_pRepresentShell;
extern int					SCREEN_WIDTH;
extern int					SCREEN_HEIGHT;
extern "C" int		JxCore_GotoHuong(int nDir, int mode, int nBuoc, int bEp);	// [ANDROID 11/09 CAN] CoreShell.cpp (chi Android)

// --- cai dat, doc mot lan tu config.ini [Cham] -----------------------------
static int	s_nDaDoc = 0;
static int	s_nBat = 1;			// 0 = tat han can dieu khien
static int	s_nVungRong = 45;	// % chieu ngang man hinh tinh tu bien TRAI
static int	s_nVungTren = 25;	// % chieu cao: tu day tro xuong moi la vung can
static int	s_nVungDuoi = 88;	// % chieu cao: qua day la thanh cong cu, khong lay
static int	s_nBanKinh = 90;		// ban kinh can, tinh theo khung ve
static int	s_nNguong = 14;		// lech qua bao nhieu diem anh thi bat dau di
static int	s_nBuocXa = 8;		// [ANDROID 11/09 CAN] dich xa bao nhieu buoc moi lan gui lenh di (Goto cu = 2); [Cham] CanBuocXa
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
static int			s_nKNSangPhai = 24;	// doi ca cum sang phai bao nhieu diem anh
static int			s_nKNCoIcon = 32;	// co THAT cua bieu tuong ky nang (DrawSkillIcon bo qua Width/Height)
//	[ANDROID 09/09 KYNANG F] keo het ban kinh nay = ngam xa bang dung TAM DANH cua ky nang
//	(ban tham khao: _Beilv = _BackAttackRadius / radius, KuiMyMenu.cpp:310, radius = 60).
static int			s_nKNBanKinhKeo = 60;
//	[ANDROID 09/09 KYNANG I] de nut = danh lien tuc; day la khoang cach giua hai phat.
static int			s_nKNNhip = 200;		// mili giay
static unsigned int	s_uKNDanhLuc = 0;	// luc danh phat gan nhat
//	[ANDROID 09/09 KYNANG G] noi rong cum nut: do lech goc tinh cho khung 70x70 nhan
//	ti le, con ta ve anh o co that (70/100/140) nen phai gian ra keo cac vong chong nhau.
static int			s_nKNGian = 100;	// phan tram (co nut da dung roi nen khong can gian)
// [ANDROID 10/09 GANTOADO] Do DOI ca cum so voi cho neo goc phai duoi cua ban tham khao.
// Am duong deu duoc (truoc day -1 = "chua dat" nen keo sang trai/len tren la mat).
// Che do sua giao dien keo cum di thi doi hai so nay, va luu vao UserData\UiToaDo.ini.
static int			s_nKNX   = 0;
static int			s_nKNY   = 0;
// [UITOADO 10/09 F] Do doi RIENG cua tung nut (0 = nut chinh, 1..8 = o phu 0..7,
// 9 = nut doi che do gan), cong them vao vi tri theo cung. Keo bang cong cu "Doi o".
static int			s_nKNDoiX[KYNANG_SO_PHU + 2];
static int			s_nKNDoiY[KYNANG_SO_PHU + 2];

//	Bo anh THAT cua VNKU - chu da chi san (\spr\Ui3\UiSkillControl)
//	[ANDROID 09/09 KYNANG H] CO THAT lay tu ban tham khao (KgameWorld.cpp:13821):
//	nen tron 40 diem anh, nhan 1.5 cho o 0..4 va 1.2 cho o 5..7 => 60 va 48.
//	Ve bang RU_T_IMAGE_STRETCH nen dat duoc dung co, khong phu thuoc co san cua tep anh;
//	nho vay chi can MOT tep khung cho ca ba cap.
#define	KYNANG_CO_CHINH	92	// nut danh chinh (mr-1_new nhan 1.2 cua ban tham khao)
#define	KYNANG_CO_TRONG	60	// o phu 0..4 = 40 * 1.5
#define	KYNANG_CO_NGOAI	48	// o phu 5..7 = 40 * 1.2

static char		s_szKNAnhChinh[128] = "\\spr\\Ui3\\UiSkillControl\\assign_skill_100x100.spr";
static char		s_szKNAnhPhu[128]   = "\\spr\\Ui3\\UiSkillControl\\assign_skill_100x100.spr";
static char		s_szKNAnhPhuNho[128]= "\\spr\\Ui3\\UiSkillControl\\assign_skill_100x100.spr";
static char		s_szKNAnhNgam[128]  = "\\spr\\Ui3\\UiSkillControl\\effect_skill.spr";
//	[ANDROID 09/09 NGHIENG] num chay trong long nut theo huong ngon tay
static char		s_szKNAnhNum[128]   = "\\spr\\Ui3\\UiSkillControl\\joystick_ctrl.spr";
//	[ANDROID 09/09 GAN] nut doi che do GAN ky nang vao o (anh cua VNKU, cung bo)
static char		s_szKNAnhGan[128]   = "\\spr\\Ui3\\UiSkillControl\\switch_assign_mode.spr";
#define	KYNANG_NUT_GAN		100		// ma tra ve cua JxKyNang_TrungNut cho nut doi che do
#define	KYNANG_GAN_DX		300		// cho dat nut doi che do, tinh tu goc phai duoi
#define	KYNANG_GAN_DY		30
#define	KYNANG_GAN_CO		40

static int			s_nKNCheDoGan = 0;	// 1 = dang o che do gan
static int			s_nKNOChon = -1;	// o dang cho gan (0..7), -1 = chua chon
static KUiGameObject s_KNGan[KYNANG_SO_PHU];	// ky nang nguoi choi tu gan cho tung o
static int			s_nKNDaDocGan = 0;
//	[ANDROID 10/09 BANGCHON] bang 3 nut khi cham mot ky nang trong bang ky nang; o CHINH cung gan duoc
#define	KYNANG_CHON_CHINH	100		// s_nKNOChon = dang chon O CHINH
//	[ANDROID 10/09 NUTVNKU] 4 nut anh cua VNKU (236x86, ve thu nua) thay cho chu tu ve - chu: "nhin khong ro chu"
//	[ANDROID 10/09 LUAN] nut nho (84x31), khong ten, khong nen, nam ngay canh khung thong tin ky nang
#define	BC_RONG				88
#define	BC_CAO_NUT			35
#define	BC_CAO_TEN			0
#define	BC_SO_NUT			4
#define	BC_NUT_RONG			84
#define	BC_NUT_CAO			31
static const char* s_szBCAnh[BC_SO_NUT] =
{
	"\\Spr\\UiNew\\UiSkills\\nut_phim_chinh.spr",
	"\\Spr\\UiNew\\UiSkills\\nut_phim_phu.spr",
	"\\Spr\\UiNew\\UiSkills\\nut_go_phim_chinh.spr",
	"\\Spr\\UiNew\\UiSkills\\nut_go_phim_phu.spr",
};
static int			s_nBCBat = 0;		// bang chon dang mo
static KUiGameObject s_BCKN;			// ky nang dang chon tren bang
static char			s_szBCTen[64];
static int			s_nBCX = 0, s_nBCY = 0;
static KUiGameObject s_KNChinhGan;		// ky nang nguoi choi gan cho O CHINH (Chinh= trong KyNangMobile.ini)
static int			s_nKNDaApChinh = 0;
static KUiGameObject s_KNCho;			// ky nang dang cho nguoi choi cham o phu de gan (tu bang chon)
static char			s_szKNBao[96];		// dong thong bao ngan tren man (3 giay)
static unsigned int	s_uKNBaoLuc = 0;
//	[ANDROID 10/09 LUAN] vong sang luan chuyen: 3 ky nang vong sang o o phu tu doi qua luan phien (chu, Nga My)
static int			s_nKNLuanMs = 500;			// [Cham] LuanChuyenMs - ban tham khao KuiAutoPlay.cpp:92 MAX_SKILLAURA_COUNT 15 nhip ~0,5 s
static unsigned int	s_uKNLuanLuc = 0;
static int			s_nKNLuanK = 0;
static int			s_nKNLuanLog = 0;	// [ANDROID 11/09 LUAN d] so lan da luan chuyen (de han che nhat ky)
static unsigned int	s_uKNAuraId[KYNANG_SO_PHU + 1];	// bo dem: ma ky nang cua o -> co phai vong sang
static int			s_nKNAuraLa[KYNANG_SO_PHU + 1];
static char			s_szKNAnhXoay[128] = "\\spr\\Ui3\\UiSkillControl\\vong_xoay.spr";	// 16 khung, lam_vong_xoay.py
#define	KYNANG_XOAY_KHUNG	16
//	[ANDROID 10/09 KHINHCONG b] xu ngua xong thi GIU LENH de tu danh khi da xuong (Core TIME_RIDE = 5 giay)
static unsigned int	s_uKNDoiNguaDen = 0;	// > 0: dang cho xuong/len ngua, thu lai toi luc nay
static int			s_nKNNgonGiu = 0;		// ngon tay dang that su de tren nut
static int			s_nKNGiuCanDiem = 0;	// [ANDROID 10/09 KHINHCONG c] nut dang giu la ky nang CAN DIEM -> chi ban luc nha ngon

static void KyNang_Bao(const char* pszChu)
{
	strncpy(s_szKNBao, pszChu, sizeof(s_szKNBao) - 1);
	s_szKNBao[sizeof(s_szKNBao) - 1] = 0;
	s_uKNBaoLuc = (unsigned int)GetTickCount();
}
//	[ANDROID 09/09 KYNANG H] direction_arrow.spr la huong DI CHUYEN cua nhan vat -
//	khong phai cai nay. Mui ten dinh huong DANH la attack_direction.spr (nhieu khung
//	theo huong) - chon khung theo goc ngam thi mui ten quay dung phia dang chi.
static char		s_szKNAnhTen[128]   = "\\spr\\npcres\\attack_direction.spr";
//	[ANDROID 09/09 HUONGDI] vong ELIP xanh chi TAM DANH, ve tren mat dat o diem ngam
//	(ban tham khao: JX1M_AimVeVong, KuiMyMenu.cpp:912).
static char		s_szKNAnhTam[128]   = "\\spr\\attack_radius.spr";
//	[ANDROID 09/09 HUONGDI] mui ten nho duoi chan nhan vat khi dang di chuyen.
//	Chinh chu chi ro day la anh "huong di chuyen cua player".
static char		s_szHuongDiAnh[128] = "\\spr\\npcres\\direction_arrow.spr";
static int			s_nHuongDiBat = 1;
static int			s_nHuongDiCoAnh = -1;
static int			s_nHuongDiThap = 10;	// chan nhan vat thap hon giua man hinh bao nhieu
//	[ANDROID 09/09 NGHIENG] do duoc: day 38 thi mui ten van roi NGAY TREN nguoi
//	(tam (522,301) trong khi giua man hinh la (520,302)). Day xa hon.
static int			s_nHuongDiXa = 56;		// day mui ten ra phia truoc bao nhieu diem anh

//	Diem NGAM trong the gioi (toa do VE) khi dang keo - de ve mui ten va de danh.
static int			s_nKNNgamX = 0, s_nKNNgamY = 0;
static int			s_nKNCoNgam = 0;	// 1 = dang co diem ngam
static int			s_nKNCoAnh = -1;	// -1 = chua kiem

static KUiSkillData	s_KNBang[KYNANG_DS_TOI_DA];
static int			s_nKNCo1 = 0;		// so ky nang doc duoc
static unsigned int	s_uKNDocLuc = 0;
static KUiGameObject s_KNChinh;		// ky nang danh TRAI dang hien tren thanh trang thai
static KUiGameObject s_KNPhai;		// [ANDROID 10/09 LUAN c] ky nang danh PHAI = vong sang dang bat (neu la aura)
static int			s_nKNDangCam = -1;	// 0 = nut chinh, 1..8 = o phu; -1 = khong giu
static int			s_nKNNgonX = 0, s_nKNNgonY = 0;
static int			s_nKNDichIdx = 0;	// chi so NPC dang ngam
static int			s_nKNDichX = 0, s_nKNDichY = 0;	// vi tri VE cua no

static bool	s_bCam = false;
static int	s_nTamX = 0, s_nTamY = 0;	// tam can (cho dat ngon)
static int	s_nHuongGui = -1;		// [ANDROID 11/09 CAN] huong da gui may chu; -1 = chua gui / da gui lenh dung
static unsigned int	s_uCanGuiLuc = 0;	// [ANDROID 11/09 CAN] luc gui gan nhat (doi huong lien tuc cung chi gui 1 lan / 110 ms)
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
	s_nBuocXa   = GetPrivateProfileInt("Cham", "CanBuocXa", 8, szCfg);	// [ANDROID 11/09 CAN]
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
	s_nKNSangPhai = GetPrivateProfileInt("Cham", "KyNangSangPhai", 24, szCfg);
	s_nKNCoIcon  = GetPrivateProfileInt("Cham", "KyNangCoIcon", 32, szCfg);
	s_nKNBanKinhKeo = GetPrivateProfileInt("Cham", "KyNangBanKinhKeo", 60, szCfg);
	s_nKNNhip = GetPrivateProfileInt("Cham", "KyNangNhip", 200, szCfg);
	s_nKNLuanMs = GetPrivateProfileInt("Cham", "LuanChuyenMs", 500, szCfg);	// [ANDROID 10/09 LUAN] 0,5 s nhu ban tham khao
	if (s_nKNLuanMs < 100) s_nKNLuanMs = 100;
	GetPrivateProfileString("Cham", "KyNangAnhXoay", s_szKNAnhXoay, s_szKNAnhXoay, sizeof(s_szKNAnhXoay), szCfg);
	s_nHuongDiBat  = GetPrivateProfileInt("Cham", "HuongDi", 1, szCfg);
	s_nHuongDiThap = GetPrivateProfileInt("Cham", "HuongDiThap", 10, szCfg);
	s_nHuongDiXa   = GetPrivateProfileInt("Cham", "HuongDiXa", 56, szCfg);
	GetPrivateProfileString("Cham", "HuongDiAnh", s_szHuongDiAnh, s_szHuongDiAnh,
		sizeof(s_szHuongDiAnh), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhTam", s_szKNAnhTam, s_szKNAnhTam,
		sizeof(s_szKNAnhTam), szCfg);
	if (s_nKNNhip < 60) s_nKNNhip = 60;
	s_nKNGian = GetPrivateProfileInt("Cham", "KyNangGian", 100, szCfg);
	if (s_nKNGian < 50)  s_nKNGian = 50;
	if (s_nKNGian > 300) s_nKNGian = 300;
	if (s_nKNBanKinhKeo < 20) s_nKNBanKinhKeo = 20;
	if (s_nKNCoIcon < 8) s_nKNCoIcon = 8;
	s_nKNX       = GetPrivateProfileInt("Cham", "KyNangX", 0, szCfg);
	s_nKNY       = GetPrivateProfileInt("Cham", "KyNangY", 0, szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhChinh", s_szKNAnhChinh, s_szKNAnhChinh,
		sizeof(s_szKNAnhChinh), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhPhuNho", s_szKNAnhPhuNho, s_szKNAnhPhuNho,
		sizeof(s_szKNAnhPhuNho), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhTen", s_szKNAnhTen, s_szKNAnhTen,
		sizeof(s_szKNAnhTen), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhPhu", s_szKNAnhPhu, s_szKNAnhPhu,
		sizeof(s_szKNAnhPhu), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhNum", s_szKNAnhNum, s_szKNAnhNum,
		sizeof(s_szKNAnhNum), szCfg);
	GetPrivateProfileString("Cham", "KyNangAnhNgam", s_szKNAnhNgam, s_szKNAnhNgam,
		sizeof(s_szKNAnhNgam), szCfg);
	if (s_nVungRong < 10) s_nVungRong = 10;
	if (s_nVungRong > 100) s_nVungRong = 100;
	if (s_nBanKinh < 30) s_nBanKinh = 30;
	if (s_nNguong < 4) s_nNguong = 4;
	if (s_nBuocXa < 2) s_nBuocXa = 2;	// [ANDROID 11/09 CAN] 2 = dich gan nhu Goto cu (thi phai gui lien tuc moi khong dung)
	if (s_nBuocXa > 30) s_nBuocXa = 30;
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

// [ANDROID 11/09 CAN] Dung lai: gui MOT lenh dich gan (2 buoc, dung nhu Goto() cu) roi thoi -> nhan vat dung trong ~0,1 s.
static void JxCan_DungLai()
{
	if (s_nHuongGui < 0)
		return;
	if (g_pCoreShell)
		JxCore_GotoHuong(s_nHuongGui, 0, 2, 1);
	s_nHuongGui = -1;
	s_uCanGuiLuc = (unsigned int)GetTickCount();
}

void JxCan_Nha()
{
	s_bCam = false;
	JxCan_DungLai();
}

void JxCan_Nhip()
{
	if (!s_bCam || !g_pCoreShell)
		return;
	int dx = s_nNgonX - s_nTamX;
	int dy = s_nNgonY - s_nTamY;
	if (dx * dx + dy * dy < s_nNguong * s_nNguong)
	{
		JxCan_DungLai();	// ngon tay ve giua can -> dung lai
		return;
	}
	// [ANDROID 11/09 CAN] Truoc: g_pCoreShell->Goto(s_nHuong, 0) MOI VONG LAP (1-8 ms) = moi lan mot goi c2s_npcwalk khong cong gac,
	// may chu tim duong + phat s2c_npcwalk cho ca vung => hang tram goi/giay chi de di bo (PC giu chuot: 1 goi / 5 tick).
	// Nay: doi huong -> gui ngay (toi da 1 lan / 110 ms); cung huong -> gui lai khi cong gac 5 tick mo (JxCore_GotoHuong);
	// dich xa s_nBuocXa buoc nen nhan vat khong dung giua hai lan gui. Cam giac di chuyen giu nguyen.
	unsigned int uNay = (unsigned int)GetTickCount();
	int bEp = (s_nHuong != s_nHuongGui && (s_nHuongGui < 0 || uNay - s_uCanGuiLuc >= 110)) ? 1 : 0;
	if (JxCore_GotoHuong(s_nHuong, 0, s_nBuocXa, bEp))
	{
		s_nHuongGui = s_nHuong;
		s_uCanGuiLuc = uNay;
	}
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

//	[ANDROID 09/09 HUONGDI] Ve anh vao mot khung chu nhat (tam nX,nY; rong nR, cao nC).
//	Dung cho vong elip tam danh - rong gap doi cao vi goc nhin nghieng.
static void VeAnhKhung(const char* pszAnh, int nX, int nY, int nR, int nC, int nKhung)
{
	KRUImage a;

	if (g_pRepresentShell == NULL || nR < 2 || nC < 2)
		return;
	memset(&a, 0, sizeof(a));
	a.nType = ISI_T_SPR;
	a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	a.Color.Color_dw = 0xffffffff;
	a.nISPosition = IMAGE_IS_POSITION_INIT;
	a.nFrame = nKhung;
	strncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);
	a.oPosition.nX = nX - nR / 2;
	a.oPosition.nY = nY - nC / 2;
	a.oPosition.nZ = 0;
	a.oEndPos.nX = a.oPosition.nX + nR;
	a.oEndPos.nY = a.oPosition.nY + nC;
	a.oEndPos.nZ = 0;
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);
}

//	[ANDROID 09/09 KYNANG H] nhu VeAnh nhung chon duoc KHUNG (anh nhieu huong).
static void VeAnh2(const char* pszAnh, int nX, int nY, int nKhung)
{
	KRUImage a;
	KRPosition2 oOff = { 0, 0 }, oCo = { 0, 0 };

	if (g_pRepresentShell == NULL)
		return;
	memset(&a, 0, sizeof(a));
	a.nType = ISI_T_SPR;
	a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	a.Color.Color_dw = 0xffffffff;
	a.nISPosition = IMAGE_IS_POSITION_INIT;
	a.nFrame = nKhung;
	strncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);
	if (g_pRepresentShell->GetImageFrameParam(a.szImage, nKhung, &oOff, &oCo, a.nType)
		&& oCo.nX > 0)
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

//	[ANDROID 10/09 GANTOADO] Ve mot dong chu (toa do man hinh) - de huong dan ngay tren man.
static void KyNang_VeChu(const char* pszChu, int nX, int nY, unsigned int uMau)
{
	int nDai = (int)strlen(pszChu);

	if (nDai <= 0 || g_pRepresentShell == NULL)
		return;
	if (nX < 8) nX = 8;
	g_pRepresentShell->OutputText(12, (char*)pszChu, nDai, nX, nY, uMau,
		0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);
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

//	[ANDROID 09/09 GAN] Doc / ghi bang gan ky nang: UserData\KyNangMobile.ini
//	Moi dong: O<so>=<loai>,<ma ky nang>
static void KyNang_DuongTepGan(char* pszRa, int nCo)
{
	pszRa[0] = 0;
	GetCurrentDirectory(nCo, pszRa);
	strncat(pszRa, "\\UserData\\KyNangMobile.ini", nCo - strlen(pszRa) - 1);
}

static void KyNang_DocGan()
{
	char szTep[MAX_PATH];
	char szDong[128];
	FILE* pTep;

	if (s_nKNDaDocGan)
		return;
	s_nKNDaDocGan = 1;
	memset(s_KNGan, 0, sizeof(s_KNGan));
	memset(&s_KNChinhGan, 0, sizeof(s_KNChinhGan));	// [ANDROID 10/09 BANGCHON]
	KyNang_DuongTepGan(szTep, sizeof(szTep));
	pTep = fopen(szTep, "rt");
	if (pTep == NULL)
		return;
	while (fgets(szDong, sizeof(szDong), pTep))
	{
		int nO = 0, nLoai = 0, nMa = 0;

		if (sscanf(szDong, "O%d=%d,%d", &nO, &nLoai, &nMa) == 3
			&& nO >= 0 && nO < KYNANG_SO_PHU)
		{
			s_KNGan[nO].uGenre = (unsigned int)nLoai;
			s_KNGan[nO].uId    = (unsigned int)nMa;
		}
		else if (sscanf(szDong, "Chinh=%d,%d", &nLoai, &nMa) == 2)
		{	// [ANDROID 10/09 BANGCHON] ky nang gan cho O CHINH
			s_KNChinhGan.uGenre = (unsigned int)nLoai;
			s_KNChinhGan.uId    = (unsigned int)nMa;
		}
	}
	fclose(pTep);
}

static void KyNang_GhiGan()
{
	char szTep[MAX_PATH];
	FILE* pTep;
	int i;

	KyNang_DuongTepGan(szTep, sizeof(szTep));
	pTep = fopen(szTep, "wt");
	if (pTep == NULL)
	{
		g_DebugLog("[KYNANG] khong ghi duoc %s", szTep);
		return;
	}
	fprintf(pTep, "; [ANDROID] Ky nang nguoi choi tu gan cho tung o tren dien thoai.\n");
	fprintf(pTep, "; Moi dong: O<so o 0..7>=<loai>,<ma ky nang>. Xoa tep = ve mac dinh.\n");
	for (i = 0; i < KYNANG_SO_PHU; i++)
	{
		if (s_KNGan[i].uId)
			fprintf(pTep, "O%d=%u,%u\n", i, s_KNGan[i].uGenre, s_KNGan[i].uId);
	}
	if (s_KNChinhGan.uId)	// [ANDROID 10/09 BANGCHON]
		fprintf(pTep, "Chinh=%u,%u\n", s_KNChinhGan.uGenre, s_KNChinhGan.uId);
	fclose(pTep);
}

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

//	[ANDROID 09/09 KYNANG E] O 0..4 nam cung TRONG nen TO hon o 5..7 cung ngoai -
//	dung ti le rieng cua ban tham khao (1.5 so voi 1.2).
static const char* KyNang_AnhCuaNut(int nNut)
{
	if (nNut <= 0)		return s_szKNAnhChinh;
	if (nNut <= 5)		return s_szKNAnhPhu;		// o phu 0..4
	return s_szKNAnhPhuNho;						// o phu 5..7
}

static int KyNang_CoNut(int nNut)
{
	if (nNut <= 0)
		return s_nKNCoChinh > 0 ? s_nKNCoChinh : KYNANG_CO_CHINH;
	if (nNut <= 5)
		return s_nKNCoPhu > 0 ? s_nKNCoPhu : KYNANG_CO_TRONG;	// o phu 0..4
	return KYNANG_CO_NGOAI;									// o phu 5..7
}

//	[ANDROID 09/09 KYNANG H] Ve mot anh .spr vao DUNG o vuong (tam nX,nY canh nCo).
//	Dung RU_T_IMAGE_STRETCH nhu KItem.cpp:1991 - nho vay co nut khong bi buoc theo
//	co san cua tep anh, va man hinh nao cung dat duoc dung co mong muon.
static void VeAnhCo(const char* pszAnh, int nX, int nY, int nCo, int nKhung)
{
	KRUImage a;

	if (g_pRepresentShell == NULL || nCo < 2)
		return;
	memset(&a, 0, sizeof(a));
	a.nType = ISI_T_SPR;
	a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	a.Color.Color_dw = 0xffffffff;
	a.nISPosition = IMAGE_IS_POSITION_INIT;
	a.nFrame = nKhung;
	strncpy(a.szImage, pszAnh, sizeof(a.szImage) - 1);
	a.oPosition.nX = nX - nCo / 2;
	a.oPosition.nY = nY - nCo / 2;
	a.oPosition.nZ = 0;
	a.oEndPos.nX = a.oPosition.nX + nCo;
	a.oEndPos.nY = a.oPosition.nY + nCo;
	a.oEndPos.nZ = 0;
	g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE_STRETCH, true);
}

//	So khung cua mot anh .spr (mui ten danh co mot khung cho moi huong).
static int KyNang_SoKhung(const char* pszAnh)
{
	KImageParam oTs;

	memset(&oTs, 0, sizeof(oTs));
	if (g_pRepresentShell && g_pRepresentShell->GetImageParam(pszAnh, &oTs, ISI_T_SPR)
		&& oTs.nNumFrames > 0)
		return (int)oTs.nNumFrames;
	return 1;
}

//	[ANDROID 09/09 GAN] Tam nut doi che do gan.
static void KyNang_TamNutGan(int* px, int* py)
{
	int nR = KYNANG_GAN_CO / 2;

	*px = SCREEN_WIDTH  - (nR + KYNANG_GAN_DX * s_nKNGian / 100)
		+ s_nKNSangPhai + s_nKNX + s_nKNDoiX[KYNANG_SO_PHU + 1];
	*py = SCREEN_HEIGHT - (nR + KYNANG_GAN_DY * s_nKNGian / 100)
		- s_nKNLenTren + s_nKNY + s_nKNDoiY[KYNANG_SO_PHU + 1];
	if (*px < nR + 2) *px = nR + 2;
	if (*py > SCREEN_HEIGHT - nR - 2) *py = SCREEN_HEIGHT - nR - 2;
}

//	Tam cua mot o, toa do MAN HINH. nNut: 0 = nut danh chinh, 1..8 = o phu.
static void KyNang_TamNut(int nNut, int* px, int* py)
{
	int nR, nDX, nDY;

	nR = KyNang_CoNut(nNut) / 2;
	if (nNut <= 0)
	{
		nDX = KYNANG_CHINH_DX;
		nDY = KYNANG_CHINH_DY;
	}
	else
	{
		nDX = s_nKNDX[nNut - 1];
		nDY = s_nKNDY[nNut - 1];
	}
	// Neo goc PHAI DUOI y nhu ban tham khao. KyNangX/Y (neu dat) doi ca cum di.
	// [ANDROID 09/09 KYNANG G] gian do lech ra cho vua bo anh (giu nguyen hinh cung)
	nDX = nDX * s_nKNGian / 100;
	nDY = nDY * s_nKNGian / 100;
	*px = SCREEN_WIDTH  - (nR + nDX) + s_nKNSangPhai + s_nKNX + s_nKNDoiX[nNut < 0 ? 0 : nNut];
	*py = SCREEN_HEIGHT - (nR + nDY) - s_nKNLenTren + s_nKNY + s_nKNDoiY[nNut < 0 ? 0 : nNut];
	// [ANDROID 09/09 KYNANG E] giu han trong khung ve: da do thay o ngoai cung bi cat
	// mat mot nua khi doi cum sang phai. Cung la de man hinh co nao cung khong loi o.
	if (*px > SCREEN_WIDTH  - nR - 2)	*px = SCREEN_WIDTH  - nR - 2;
	if (*px < nR + 2)					*px = nR + 2;
	if (*py > SCREEN_HEIGHT - nR - 2)	*py = SCREEN_HEIGHT - nR - 2;
	if (*py < nR + 2)					*py = nR + 2;
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
	s_KNPhai  = oTay.IMmediaSkill[1];	// [ANDROID 10/09 LUAN c]
	// [ANDROID 10/09 BANGCHON] o chinh nguoi choi tu gan (Chinh= trong KyNangMobile.ini): ap MOT lan khi da vao game
	// (co danh sach ky nang) va ky nang danh trai hien tai khac.
	KyNang_DocGan();
	if (!s_nKNDaApChinh && s_nKNCo1 > 0 && s_KNChinhGan.uId)
	{
		s_nKNDaApChinh = 1;
		if (s_KNChinh.uId != s_KNChinhGan.uId)
		{
			g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNChinhGan, 0);
			g_DebugLog("[KYNANG] ap ky nang o chinh da gan: %u", s_KNChinhGan.uId);
		}
	}
}

//	Ky nang gan cho mot o. Tra ve false neu o trong.
static bool KyNang_CuaNut(int nNut, KUiGameObject* pRa)
{
	if (nNut <= 0)
	{
		*pRa = s_KNChinh;
		return (pRa->uGenre != CGOG_NOTHING && pRa->uId != 0);
	}
	// [ANDROID 09/09 GAN] Uu tien ky nang NGUOI CHOI TU GAN cho o nay.
	KyNang_DocGan();
	if (s_KNGan[nNut - 1].uId)
	{
		*pRa = s_KNGan[nNut - 1];
		return true;
	}
	// Chua gan thi lay theo danh sach ky nang danh, de dung duoc ngay khong phai gan tay.
	if (nNut - 1 >= s_nKNCo1)
	{
		memset(pRa, 0, sizeof(*pRa));
		return false;
	}
	pRa->uGenre = s_KNBang[nNut - 1].uGenre;
	pRa->uId    = s_KNBang[nNut - 1].uId;
	return (pRa->uGenre != CGOG_NOTHING && pRa->uId != 0);
}

//---------------------------------------------------------------------------
//	[UITOADO 10/09 F] TUNG NUT ky nang doi cho RIENG duoc trong che do sua giao dien.
//	Chu: "nut ky nang phai dieu chinh tung nut duoc".
//	  - cong cu "Doi o"   : keo mot nut -> CHI nut do dich (do doi rieng s_nKNDoiX/Y[i]),
//	                        luu khoa KyNang0 (nut chinh), KyNang1..8 (o phu 0..7), KyNangGan.
//	  - cong cu "Doi khoi": keo bat ky nut nao -> CA CUM dich (s_nKNX/Y), luu khoa CumKyNang.
//	Nut chua keo rieng bao gio thi van nam theo hinh cung cua ban tham khao.
//---------------------------------------------------------------------------
#define	KYNANG_ORIENG_GAN	(KYNANG_SO_PHU + 1)	// chi so cua nut doi che do trong bang do doi

static void KyNang_TamORieng(int i, int* px, int* py, int* pR)
{
	if (i == KYNANG_ORIENG_GAN)
	{
		KyNang_TamNutGan(px, py);
		*pR = KYNANG_GAN_CO / 2;
	}
	else
	{
		KyNang_TamNut(i, px, py);
		*pR = KyNang_CoNut(i) / 2;
	}
}

static bool KyNang_ORiengTrungMot(void* pNgu, int x, int y)
{
	int i = (int)(intptr_t)pNgu;
	int nX, nY, nR;

	if (UiToaDo_CongCuKhoi())
		return false;			// dang "Doi khoi" thi de muc CumKyNang bat
	KyNang_TamORieng(i, &nX, &nY, &nR);
	return (x - nX) * (x - nX) + (y - nY) * (y - nY) <= nR * nR;
}

static void KyNang_ORiengLayMot(void* pNgu, int* px, int* py)
{
	int nR;

	KyNang_TamORieng((int)(intptr_t)pNgu, px, py, &nR);
}

static void KyNang_ORiengDatMot(void* pNgu, int x, int y)
{
	int i = (int)(intptr_t)pNgu;
	int nX = 0, nY = 0, nR;

	KyNang_TamORieng(i, &nX, &nY, &nR);
	s_nKNDoiX[i] += (x - nX);
	s_nKNDoiY[i] += (y - nY);
}

//	Ca cum - chi bat khi dang dung cong cu "Doi khoi".
static bool KyNang_ORiengTrungCum(void* pNgu, int x, int y)
{
	int i;

	if (!UiToaDo_CongCuKhoi())
		return false;
	for (i = 0; i <= KYNANG_ORIENG_GAN; i++)
	{
		int nX, nY, nR;

		KyNang_TamORieng(i, &nX, &nY, &nR);
		if ((x - nX) * (x - nX) + (y - nY) * (y - nY) <= nR * nR)
			return true;
	}
	return false;
}

static void KyNang_ORiengLayCum(void* pNgu, int* px, int* py)
{
	KyNang_TamNut(0, px, py);	// lay tam nut danh chinh lam moc cua ca cum
}

static void KyNang_ORiengDatCum(void* pNgu, int x, int y)
{
	int nX = 0, nY = 0;

	KyNang_TamNut(0, &nX, &nY);
	// s_nKNX/Y la do DOI cua ca cum so voi cho neo goc phai duoi; am duong deu duoc.
	s_nKNX += (x - nX);
	s_nKNY += (y - nY);
}

static void KyNang_DangKySuaToaDo()
{
	static bool s_bDaDangKy = false;
	static const char* s_szKhoa[KYNANG_SO_PHU + 2] =
	{
		"KyNang0", "KyNang1", "KyNang2", "KyNang3", "KyNang4",
		"KyNang5", "KyNang6", "KyNang7", "KyNang8", "KyNangGan"
	};
	int i;

	if (s_bDaDangKy)
		return;
	s_bDaDangKy = true;
	// Ca cum dang ky TRUOC: vi tri da luu cua cum ap truoc, roi moi den do doi rieng tung
	// nut (do doi rieng tinh so voi cum) - thu tu nay phai giu.
	UiToaDo_DangKyORieng("CumKyNang", KyNang_ORiengTrungCum,
		KyNang_ORiengLayCum, KyNang_ORiengDatCum, NULL);
	for (i = 0; i < KYNANG_SO_PHU + 2; i++)
		UiToaDo_DangKyORieng(s_szKhoa[i], KyNang_ORiengTrungMot,
			KyNang_ORiengLayMot, KyNang_ORiengDatMot, (void*)(intptr_t)i);
}
int JxKyNang_TrungNut(int x, int y)
{
	int i, nX, nY, nR;

	DocCaiDat();
	if (!s_nKNBat)
		return 0;
	KyNang_DocBang();
	// [ANDROID 09/09 GAN] nut doi che do gan
	{
		int nGX, nGY, nGR = KYNANG_GAN_CO / 2;

		KyNang_TamNutGan(&nGX, &nGY);
		if ((x - nGX) * (x - nGX) + (y - nGY) * (y - nGY) <= nGR * nGR)
			return KYNANG_NUT_GAN;
	}
	// Xet o phu TRUOC roi moi den nut chinh: cung o phu vong sat nut chinh, uu tien
	// o nho de cham vao ria cung khong bi nut to nuot mat.
	for (i = KYNANG_SO_PHU; i >= 0; i--)
	{
		KUiGameObject o;

		// Trong che do gan thi o TRONG cung phai bat duoc cham, khong thi khong gan
		// duoc vao o trong.
		// [ANDROID 10/09 OTRONG] o trong cung bat duoc cham (cham = vao che do gan cho dung o do)
		KyNang_CuaNut(i, &o);
		KyNang_TamNut(i, &nX, &nY);
		nR = KyNang_CoNut(i) / 2;
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

//	[ANDROID 09/09 KYNANG F] Hoi Core truoc khi danh: ky nang nay dung duoc luc dang
//	cuoi ngua khong, tam danh bao nhieu, co phai ky nang tro khong.
//	Tra ve true = danh tiep duoc; false = lan nay thoi (vua gui lenh len/xuong ngua,
//	hoac ky nang khong dung duoc luc nay).
static bool KyNang_HoiCore(int nSkillId, int* pTamDanh, int* pLaAura, int* pTuDung = NULL, char* pszTen = NULL,
	int* pCanDiem = NULL, int bChiHoi = 0, int* pHuong = NULL, int* pTamCap = NULL)
{
	KJxKyNangHoi oHoi;

	if (pTamDanh) *pTamDanh = 0;
	if (pLaAura)  *pLaAura  = 0;
	if (pTuDung)  *pTuDung  = 0;	// [ANDROID 10/09 BUFF]
	if (pszTen)   pszTen[0] = 0;
	if (pCanDiem) *pCanDiem = 0;	// [ANDROID 10/09 KHINHCONG]
	if (pHuong)   *pHuong   = 0;	// [ANDROID 10/09 KHINHCONG b]
	if (pTamCap)  *pTamCap  = 0;
	if (g_pCoreShell == NULL || nSkillId <= 0)
		return false;
	memset(&oHoi, 0, sizeof(oHoi));
	oHoi.nSkillId = nSkillId;
	if (!g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oHoi, bChiHoi ? 1 : 0))	// [ANDROID 10/09 LUAN]
		return true;		// khong hoi duoc thi cu danh, dung nhu truoc
	if (pTamDanh) *pTamDanh = oHoi.nTamDanh;
	if (pLaAura)  *pLaAura  = oHoi.nLaAura;
	if (pTuDung)  *pTuDung  = oHoi.nTuDung;	// [ANDROID 10/09 BUFF]
	if (pCanDiem) *pCanDiem = oHoi.nCanDiem;	// [ANDROID 10/09 KHINHCONG]
	if (pHuong)   *pHuong   = oHoi.nHuong;		// [ANDROID 10/09 KHINHCONG b]
	if (pTamCap)  *pTamCap  = oHoi.nTamCap;
	if (pszTen)
	{
		strncpy(pszTen, oHoi.szTen, 63);
		pszTen[63] = 0;
	}
	if (oHoi.nNgua != 0)
	{
		g_DebugLog("[KYNANG] ky nang %d: xu ngua = %d -> lan nay chua danh",
			nSkillId, oHoi.nNgua);
		// [ANDROID 10/09 KHINHCONG b] giu lenh toi 6 giay: JxKyNang_Nhip thu lai moi nhip, xuong ngua xong la danh
		if (!bChiHoi && s_uKNDoiNguaDen == 0)
			s_uKNDoiNguaDen = (unsigned int)GetTickCount() + 6000;
		return false;
	}
	return true;
}

void JxKyNang_BatDau(int nNut, int x, int y)
{
	// [ANDROID 09/09 GAN] nut doi che do
	if (nNut == KYNANG_NUT_GAN)
	{
		s_nKNCheDoGan = !s_nKNCheDoGan;
		s_nKNOChon = -1;
		s_nKNDangCam = -1;
		g_DebugLog("[KYNANG] che do gan = %d", s_nKNCheDoGan);
		return;
	}
	// Dang o che do gan: cham o la CHON o do de gan, khong danh.
	if (s_nKNCheDoGan)
	{
		// [ANDROID 10/09 BANGCHON] o CHINH cung gan duoc (chu: "o ky nang chinh khong doi ky nang duoc")
		s_nKNOChon = (nNut == 1) ? KYNANG_CHON_CHINH : (nNut - 2);
		s_nKNDangCam = -1;
		// dang co ky nang cho san (bam "Gan o phu" tren bang chon luc het o trong): gan ngay vao o vua cham
		if (s_KNCho.uId)
		{
			KUiGameObject oCho = s_KNCho;

			memset(&s_KNCho, 0, sizeof(s_KNCho));
			JxKyNang_GanKyNang(oCho.uGenre, oCho.uId);
		}
		return;
	}
	// [ANDROID 10/09 OTRONG] cham o phu TRONG = chon o do de gan va mo bang ky nang luon (mot cham thay vi ba).
	if (nNut > 1)
	{
		KUiGameObject oCo;

		if (!KyNang_CuaNut(nNut - 1, &oCo))
		{
			s_nKNCheDoGan = 1;
			s_nKNOChon = nNut - 2;
			s_nKNDangCam = -1;
			g_DebugLog("[KYNANG] cham o trong %d -> che do gan, mo bang ky nang", nNut - 1);
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_SKILLSNEW);
			return;
		}
	}
	s_nKNDangCam = nNut - 1;	// 0 = nut chinh, 1..8 = o phu
	s_nKNNgonGiu = 1;			// [ANDROID 10/09 KHINHCONG b]
	s_uKNDoiNguaDen = 0;
	// [ANDROID 10/09 KHINHCONG c] ky nang can diem (khinh cong): khong ban luc dat ngon, doi nha ngon de con chon huong
	s_nKNGiuCanDiem = 0;
	{
		KUiGameObject oCD;
		int nCD = 0;

		if (KyNang_CuaNut(nNut - 1, &oCD))
			KyNang_HoiCore((int)oCD.uId, NULL, NULL, NULL, NULL, &nCD, 1);
		s_nKNGiuCanDiem = nCD;
	}
	s_nKNNgonX = x;
	s_nKNNgonY = y;
	s_nKNDichIdx = 0;
	s_nKNCoNgam = 0;
	s_uKNDanhLuc = 0;		// = danh ngay phat dau o nhip ke tiep
}

void JxKyNang_Keo(int x, int y)
{
	int nX, nY, dx, dy, nTam = 0;
	KUiGameObject o;

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
		s_nKNCoNgam = 0;
		return;
	}
	s_nKNDichIdx = KyNang_TimDich(dx, dy, &s_nKNDichX, &s_nKNDichY);

	// [ANDROID 09/09 KYNANG F] DIEM NGAM tren man hinh, dung phep cua ban tham khao:
	//   diem = giua man hinh + do_lech_keo * (tam_danh / ban_kinh_keo)
	// truc Y ep con MOT NUA vi goc nhin nghieng (isometric).
	// Nhan vat luon o giua khung ve nen giua man hinh = cho nhan vat dung.
	if (KyNang_CuaNut(s_nKNDangCam, &o))
	{
		KJxKyNangHoi oH;

		memset(&oH, 0, sizeof(oH));
		oH.nSkillId = (int)o.uId;
		if (g_pCoreShell && g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oH, 1))
			nTam = oH.nTamDanh;
	}
	if (nTam < 40)
		nTam = 40;		// ky nang khong co tam (danh gan) van phai ngam duoc mot doan
	{
		int nDai = (int)sqrt((double)(dx * dx + dy * dy));
		int nXa  = nDai * nTam / s_nKNBanKinhKeo;

		if (nXa > nTam)
			nXa = nTam;		// keo qua xa cung chi toi duoc tam danh
		if (nDai < 1)
			nDai = 1;
		s_nKNNgamX = SCREEN_WIDTH  / 2 + dx * nXa / nDai;
		s_nKNNgamY = SCREEN_HEIGHT / 2 + dy * nXa / nDai / 2;	// ep Y con mot nua
		s_nKNCoNgam = 1;
	}
}

//	[ANDROID 09/09 KYNANG I] Danh MOT phat bang nut dang giu. Dung chung cho nhip
//	(dang de nut) va cho luc nha ngon.
static void KyNang_DanhMotPhat()
{
	int nNut = s_nKNDangCam;
	int nDich = s_nKNDichIdx;
	int bCoNgam = s_nKNCoNgam;
	int nNgamX = s_nKNNgamX, nNgamY = s_nKNNgamY;
	int nTam = 0, nAura = 0;
	KUiGameObject o;

	if (nNut < 0 || g_pCoreShell == NULL)
		return;
	if (!KyNang_CuaNut(nNut, &o))
		return;

	// Khau kiem tra ngua - dung nhu he tu danh lam. Neu ky nang chi dung duoc duoi
	// ngua ma dang cuoi thi Core da gui lenh xuong ngua, phat nay khong danh.
	int nTuDung = 0, nCanDiem = 0, nHuong = 0, nTamCap = 0;

	if (!KyNang_HoiCore((int)o.uId, &nTam, &nAura, &nTuDung, NULL, &nCanDiem, 0, &nHuong, &nTamCap))
		return;
	s_uKNDoiNguaDen = 0;	// [ANDROID 10/09 KHINHCONG b] hoi duoc = khong con vuong ngua
	if (!nAura && nTuDung)
	{
		// [ANDROID 10/09 BUFF] ky nang TU DUNG (buff / hoi phuc len minh, dong doi): cham la dung NGAY tai
		// cho nhan vat (nhan vat luon o giua khung ve), khong can keo ngam, khong can co ke dich - chu:
		// "cac o phu bo cac ky nang buff thi chi can kich vao se tu su dung". Dung mot lan moi cham.
		// [ANDROID 10/09 LUAN] KHONG SetRightSkill nua: KPlayer::SetRightSkill(ky nang thuong) goi SetAuraSkill(0) = tat
		// vong sang moi lan dung; UseSkill nhan ma ky nang truc tiep nen khong can.
		g_pCoreShell->UseSkill(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (int)o.uId);
		s_nKNDangCam = -1;		// khong lap lai khi con giu
		return;
	}
	if (nAura)
	{
		// [ANDROID 09/09 AURA] Ky nang TRO (noi cong / trang thai) khong phai danh, ma
		// la BAT len. Duong co san: dat lam ky nang danh PHAI (nParam = 1) ->
		// KPlayer::SetRightSkill (KPlayer.cpp:4444) tu goi SetAuraSkill cho ky nang aura.
		// (Ban truoc dat nham vao o danh TRAI nen khong bat gi ca.)
		g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 1);
		s_KNPhai = o;	// [ANDROID 11/09 LUAN d] vong xoay chuyen ngay sang o vua cham
		return;
	}

	if (nCanDiem && !bCoNgam)
	{
		// [ANDROID 10/09 KHINHCONG b] LAM DUNG NHU BAN THAM KHAO (KgameWorld.cpp:4387, R158/R171): huong = m_Dir cua nhan vat
		// (0 xuong, 16 trai, 32 len, 48 phai, nguoc chieu kim), goc = (-90 - 5,625*dir) do; tam theo CAP DA
		// HOC, it nhat 60; diem = (W/2 + cos*0,9*tam, H/2 - sin*0,45*tam) (ep dan isometric). Dang keo can
		// thi lay huong can. (Truoc: huong can + khong ep dan -> (520,662) ngoai man, NewJump fail im lang.)
		int nDir = JxCan_DangCam() ? s_nHuong : nHuong;
		double fA = (-90.0 - 5.625 * (double)nDir) * 3.14159265358979 / 180.0;
		double fR = (double)((nTamCap > 0) ? nTamCap : nTam);

		if (fR < 60.0) fR = 60.0;
		nNgamX = SCREEN_WIDTH / 2 + (int)(cos(fA) * 0.9 * fR);
		nNgamY = SCREEN_HEIGHT / 2 - (int)(sin(fA) * 0.45 * fR);
		bCoNgam = 1;
		g_DebugLog("[KYNANG] ky nang can diem %u: huong %d tam %d -> (%d,%d)", o.uId, nDir, (int)fR, nNgamX, nNgamY);
	}
	if (nNut > 0)
	{
		// NUT PHU: truoc day dat lam ky nang danh PHAI roi danh theo diem (nhu ban tham khao).
		// [ANDROID 10/09 LUAN] BO buoc SetRightSkill: KPlayer::SetRightSkill(ky nang thuong) goi SetAuraSkill(0)
		// = TAT vong sang moi lan danh. UseSkill / LockSomeoneUseSkill nhan ma ky nang truc tiep
		// (SetActiveSkill theo ma) nen nut phu van khong dam chan nut chinh.
		if (bCoNgam)
		{
			g_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);	// tha ky nang o cho o xanh
			if (nCanDiem)
				s_nKNDangCam = -1;	// [ANDROID 10/09 KHINHCONG b] nhay MOT lan moi cham; lap lai 200 ms se cat ngang cu nhay
			return;
		}
	}
	else if (bCoNgam)
	{
		g_pCoreShell->UseSkill(nNgamX, nNgamY, (int)o.uId);	// nut chinh, danh theo huong
		if (nCanDiem)
			s_nKNDangCam = -1;	// [ANDROID 10/09 KHINHCONG b]
		return;
	}

	// Khong ngam: de nut la danh con gan nhat (kieu 0 cua ban tham khao).
	if (nDich == 0)
		nDich = KyNang_TimDich(0, 0, NULL, NULL);
	if (nDich == 0)
		return;
	g_pCoreShell->LockSomeoneUseSkill(nDich, (int)o.uId);
}

//	[ANDROID 09/09 GAN] Bang ky nang goi vao day khi nguoi choi cham mot ky nang.
//	Tra ve true = da gan vao o dang cho, ben goi khoi lam viec cua no nua.
bool JxKyNang_GanKyNang(unsigned int uGenre, unsigned int uId)
{
	if (!s_nKNCheDoGan || uId == 0)
		return false;
	// [ANDROID 10/09 BANGCHON] o CHINH: dat lam ky nang danh TRAI cua Core va nho lai de mo game van con
	if (s_nKNOChon == KYNANG_CHON_CHINH)
	{
		KUiGameObject o;

		memset(&o, 0, sizeof(o));
		o.uGenre = uGenre;
		o.uId    = uId;
		KyNang_DocGan();
		s_KNChinhGan = o;
		KyNang_GhiGan();
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);
		KyNang_Bao("§· g¾n vµo « chÝnh");
		g_DebugLog("[KYNANG] gan ky nang %u vao O CHINH", uId);
		s_nKNOChon = -1;
		s_nKNCheDoGan = 0;
		return true;
	}
	if (s_nKNOChon < 0 || s_nKNOChon >= KYNANG_SO_PHU)
		return false;
	KyNang_DocGan();
	s_KNGan[s_nKNOChon].uGenre = uGenre;
	s_KNGan[s_nKNOChon].uId    = uId;
	KyNang_GhiGan();
	g_DebugLog("[KYNANG] gan ky nang %u vao o %d", uId, s_nKNOChon);
	s_nKNOChon = -1;
	s_nKNCheDoGan = 0;		// gan xong thi ra khoi che do gan luon
	return true;
}

//	[ANDROID 09/09 KYNANG I] Goi moi vong lap game: con de nut thi cu danh tiep.
//	[ANDROID 10/09 LUAN] o phu i (1..8) dang giu VONG SANG (aura)? Bo dem theo ma ky nang; hoi Core kieu "chi hoi".
static int KyNang_OLaAura(int i, KUiGameObject* pRa)
{
	if (i < 1 || i > KYNANG_SO_PHU || !KyNang_CuaNut(i, pRa))
		return 0;
	if (s_uKNAuraId[i] != pRa->uId)
	{
		// [ANDROID 11/09 LUAN d] Hoi THANG Core va CHI ghi bo dem khi Core TRA LOI DUOC (GetGameData tra 1).
		// Truoc: KyNang_HoiCore tra ve (nAura = 0) ca khi Core chua tra loi duoc (chua vao game: Player.m_nIndex = 0
		// -> Core break, nRet 0) -> 0 bi ghi vao bo dem theo MA KY NANG cho ca phien -> o vong sang bi coi la
		// "khong phai vong sang" -> khong luan chuyen, khong vong xoay. JxKyNang_Nhip chay tu vong lap KSdlApp::Run
		// ngay tu man hinh dang nhap (KyNangMobile.ini da co) nen bo dem luon bi doc sai truoc khi vao game.
		KJxKyNangHoi oHoi;

		if (g_pCoreShell == NULL)
			return 0;
		memset(&oHoi, 0, sizeof(oHoi));
		oHoi.nSkillId = (int)pRa->uId;
		if (!g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oHoi, 1))
			return 0;		// chua tra loi duoc: KHONG ghi bo dem, lan sau hoi lai
		s_uKNAuraId[i] = pRa->uId;
		s_nKNAuraLa[i] = oHoi.nLaAura;
	}
	return s_nKNAuraLa[i];
}

//	[ANDROID 10/09 LUAN] Vong sang luan chuyen: cac o phu dang giu vong sang thay nhau bat, moi s_nKNLuanMs.
//	Mot vong sang: chi giu cho no bat (bi tat thi bat lai). Chu: "3 ky nang vong sang o o phu se tu doi
//	qua luan phien lien tuc" (Nga My). Ban tham khao: AUTO goi lai nut vong sang lien tuc.
static void KyNang_LuanChuyen()
{
	KUiGameObject aAura[KYNANG_SO_PHU];
	KUiGameObject oMuon;
	KUiPlayerImmedItemSkill oTay;
	unsigned int uNay;
	int i, n = 0;

	if (g_pCoreShell == NULL)
		return;
	// [ANDROID 11/09 LUAN d] moi s_nKNLuanMs chi lam MOT lan (truoc: quet 8 o + hoi Core moi vong lap ~1 ms)
	uNay = (unsigned int)GetTickCount();
	if (s_uKNLuanLuc && uNay - s_uKNLuanLuc < (unsigned int)s_nKNLuanMs)
		return;
	s_uKNLuanLuc = uNay;
	memset(&oTay, 0, sizeof(oTay));
	g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (KNPARAM)&oTay, 0);
	if (oTay.IMmediaSkill[0].uId == 0)
		return;		// [ANDROID 11/09 LUAN d] chua vao game (chua co ky nang danh trai): khong hoi, khong doi gi
	s_KNPhai = oTay.IMmediaSkill[1];	// [ANDROID 11/09 LUAN d] vong xoay bam dung o dang bat, khong doi KyNang_DocBang (2 s)
	for (i = 1; i <= KYNANG_SO_PHU; i++)
	{
		KUiGameObject o;

		if (KyNang_OLaAura(i, &o))
			aAura[n++] = o;
	}
	if (n == 0)
		return;
	oMuon = aAura[s_nKNLuanK % n];
	s_nKNLuanK++;
	if (n == 1 && oTay.IMmediaSkill[1].uId == oMuon.uId)
		return;		// mot vong sang va dang bat dung no
	g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&oMuon, 1);	// SetRightSkill -> SetAuraSkill
	s_KNPhai = oMuon;	// [ANDROID 11/09 LUAN d] vong xoay chuyen NGAY sang o vua bat
	if (s_nKNLuanLog < 20 || (s_nKNLuanLog % 200) == 0)	// [ANDROID 11/09 LUAN d] 20 dong dau, sau do moi 200 lan mot dong
		g_DebugLog("[KYNANG] luan chuyen vong sang -> %u (%d/%d) lan %d", oMuon.uId, (s_nKNLuanK - 1) % n + 1, n, s_nKNLuanLog);
	s_nKNLuanLog++;
}

void JxKyNang_Nhip()
{
	unsigned int uNay;

	KyNang_LuanChuyen();	// [ANDROID 10/09 LUAN] chay moi khung, khong phu thuoc dang giu nut hay khong
	if (s_nKNDangCam < 0)
		return;
	if (s_nKNGiuCanDiem && s_nKNNgonGiu)
		return;		// [ANDROID 10/09 KHINHCONG c] con de ngon: chua nhay, doi nha ngon (JxKyNang_Nha ban)
	uNay = (unsigned int)GetTickCount();
	if (s_uKNDanhLuc && uNay - s_uKNDanhLuc < (unsigned int)s_nKNNhip)
		return;
	s_uKNDanhLuc = uNay;
	KyNang_DanhMotPhat();
	// [ANDROID 10/09 KHINHCONG b] ngon da nha: het cho ngua (danh xong hoac qua 6 giay) thi tha lenh
	if (s_uKNDoiNguaDen && uNay >= s_uKNDoiNguaDen)
		s_uKNDoiNguaDen = 0;
	if (!s_nKNNgonGiu && !s_uKNDoiNguaDen)
	{
		s_nKNDangCam = -1;
		s_nKNDichIdx = 0;
		s_nKNCoNgam = 0;
		s_uKNDanhLuc = 0;
	}
}

bool JxKyNang_Nha()
{
	bool bCo = (s_nKNDangCam >= 0);

	s_nKNNgonGiu = 0;
	// [ANDROID 10/09 KHINHCONG c] ky nang can diem: NHA ngon moi nhay - co keo ngam thi theo diem ngam, khong thi theo huong nhin
	if (bCo && s_nKNGiuCanDiem && s_uKNDoiNguaDen == 0)
	{
		s_uKNDanhLuc = (unsigned int)GetTickCount();
		KyNang_DanhMotPhat();	// ben trong tu dat s_nKNDangCam = -1 khi nhay xong
	}
	// [ANDROID 10/09 KHINHCONG b] dang cho xuong/len ngua: giu lenh, JxKyNang_Nhip thu lai roi tu nha
	if (bCo && s_uKNDoiNguaDen && (unsigned int)GetTickCount() < s_uKNDoiNguaDen)
		return bCo;
	s_nKNDangCam = -1;
	s_nKNDichIdx = 0;
	s_nKNCoNgam = 0;
	s_uKNDanhLuc = 0;
	return bCo;
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

//---------------------------------------------------------------------------
// [ANDROID 09/09 HUONGDI] MUI TEN NHO DUOI CHAN THEO HUONG DI CHUYEN
//
// Chu: "nguoi choi khi di chuyen duoi chan se co mui ten nho theo huong di chuyen".
// Ban tham khao co role_dir rieng cho viec nay (KgameWorldVN.cpp:826).
// Nhan vat luon o giua khung ve nen ve ngay duoi giua man hinh.
//---------------------------------------------------------------------------
void JxHuongDi_Ve()
{
	int nSo, nKhung;

	DocCaiDat();
	if (!s_nHuongDiBat || !s_bCam || g_pRepresentShell == NULL)
		return;		// chi hien khi dang cam can dieu khien (tuc dang di)
	if (s_nHuongDiCoAnh < 0)
	{
		s_nHuongDiCoAnh = CoAnh(s_szHuongDiAnh) ? 1 : 0;
		g_DebugLog("[HUONGDI] anh mui ten: %s -> co anh=%d",
			s_szHuongDiAnh, s_nHuongDiCoAnh);
	}
	if (!s_nHuongDiCoAnh)
		return;
	// s_nHuong la 0..63 theo loi cua game (0 = xuong, 16 = trai, 32 = len, 48 = phai).
	nSo = KyNang_SoKhung(s_szHuongDiAnh);
	nKhung = (nSo > 1) ? (s_nHuong * nSo / 64) % nSo : 0;
	// [ANDROID 09/09 HUONGDI B] Mui ten phai nam PHIA TRUOC nhan vat, khong phai mot
	// cho co dinh: day no ra theo dung huong dang di, chia doi truc Y vi goc nhin
	// nghieng. (Truoc day ve o mot cho nen di len thi mui ten nam sau lung.)
	{
		double fGocD = (48.0 - (double)s_nHuong) * 5.625 * 3.14159265358979 / 180.0;
		int nDay = s_nHuongDiXa;
		int nX = SCREEN_WIDTH  / 2 + (int)(cos(fGocD) * nDay);
		// [ANDROID 09/09 NGHIENG B] KHONG ep truc Y con mot nua nua. Ep 1/2 la dung ve
		// hinh hoc cho vat nam tren MAT DAT, nhung lam dau hieu len/xuong gan nhu khong
		// nhin thay: do duoc khi di LEN mui ten van roi ngay tren nguoi (tam (521,301)
		// trong khi giua man hinh la (520,302)) - dung nhu chu ta "van nam sau dit".
		int nY = SCREEN_HEIGHT / 2 + s_nHuongDiThap - (int)(sin(fGocD) * nDay);

		// Ve theo KHUNG co dinh thay vi de anh tu neo: anh .spr nay co khung rong hon
		// phan nhin thay nen neo theo co khung lam mui ten roi thap hon chan (da do).
		VeAnhKhung(s_szHuongDiAnh, nX, nY, 34, 38, nKhung);
	}
}

//---------------------------------------------------------------------------
// [ANDROID 10/09 BANGCHON] BANG 3 NUT khi cham mot ky nang trong bang ky nang (chu: "mo bang ky nang ra bam vao
// ky nang nao thi hien thong tin ky nang va kem 3 nut gan ky nang chinh - ky nang phu - go ky nang khoi o").
// Thong tin ky nang: cua so ky nang tu hien khi con tro dung tren o (cham = con tro toi do, KSdlApp).
static void HopChuNhat(int x0, int y0, int x1, int y1, unsigned int uMau)
{
	KRUShadow o;

	o.oPosition.nX = x0; o.oPosition.nY = y0;
	o.oEndPos.nX = x1;   o.oEndPos.nY = y1;
	o.Color.Color_dw = uMau;
	g_pRepresentShell->DrawPrimitives(1, &o, RU_T_SHADOW, true);
}

void JxKyNang_MoBangChon(unsigned int uGenre, unsigned int uId, int x, int y)
{
	int nCao = BC_CAO_TEN + BC_CAO_NUT * BC_SO_NUT;

	if (uId == 0)
		return;
	memset(&s_BCKN, 0, sizeof(s_BCKN));
	s_BCKN.uGenre = uGenre;
	s_BCKN.uId    = uId;
	KyNang_HoiCore((int)uId, NULL, NULL, NULL, s_szBCTen);
	s_nBCX = x + 6;
	s_nBCY = y - 8;
	if (s_nBCX + BC_RONG > SCREEN_WIDTH - 4)	s_nBCX = x - 50 - BC_RONG;
	if (s_nBCX < 4)							s_nBCX = 4;
	if (s_nBCY + nCao > SCREEN_HEIGHT - 4)	s_nBCY = SCREEN_HEIGHT - 4 - nCao;
	if (s_nBCY < 4)							s_nBCY = 4;
	s_nBCBat = 1;
	g_DebugLog("[KYNANG] bang chon: ky nang %u (%s) tai %d,%d", uId, s_szBCTen, s_nBCX, s_nBCY);
}

//	Go ky nang khoi moi o phu dang giu no. O chinh khong go duoc (Core luon can mot ky nang danh trai).
static void KyNang_GoKhoiO(const KUiGameObject* p)
{
	int i, nGo = 0;

	KyNang_DocGan();
	for (i = 0; i < KYNANG_SO_PHU; i++)
	{
		if (s_KNGan[i].uId == p->uId && s_KNGan[i].uGenre == p->uGenre)
		{
			memset(&s_KNGan[i], 0, sizeof(s_KNGan[i]));
			nGo++;
		}
	}
	if (nGo)
	{
		int nAura = 0;

		KyNang_GhiGan();
		memset(s_uKNAuraId, 0, sizeof(s_uKNAuraId));	// [ANDROID 10/09 LUAN] bo dem aura cua cac o phai tinh lai
		// go mot vong sang -> tat no: dat ky nang phai = ky nang danh chinh (SetRightSkill -> SetAuraSkill(0))
		KyNang_HoiCore((int)p->uId, NULL, &nAura, NULL, NULL, NULL, 1);
		if (nAura && s_KNChinh.uId && g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&s_KNChinh, 1);
			s_KNPhai = s_KNChinh;	// [ANDROID 11/09 LUAN d]
		}
		KyNang_Bao("§· gì khái « phô");
	}
	else if (s_KNChinh.uId == p->uId)
		KyNang_Bao("¤ chÝnh chØ thay ®­îc, kh«ng gì ®­îc");
	else
		KyNang_Bao("Kü n¨ng nµy ch­a n»m ë « nµo");
	g_DebugLog("[KYNANG] go ky nang %u khoi %d o", p->uId, nGo);
}

//	[ANDROID 10/09 NUTVNKU] "Go phim chinh": tra o chinh ve ky nang dau danh sach danh trai (ky nang co ban),
//	va quen "Chinh=" da nho. (Core luon can mot ky nang danh trai nen khong "go" thanh trong duoc.)
static void KyNang_GoChinh()
{
	KyNang_DocGan();
	memset(&s_KNChinhGan, 0, sizeof(s_KNChinhGan));
	KyNang_GhiGan();
	if (g_pCoreShell && s_nKNCo1 > 0 && s_KNBang[0].uId && s_KNBang[0].uId != s_KNChinh.uId)
	{
		KUiGameObject o;

		memset(&o, 0, sizeof(o));
		o.uGenre = s_KNBang[0].uGenre;
		o.uId    = s_KNBang[0].uId;
		g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (KUPARAM)&o, 0);
	}
	KyNang_Bao("¤ chÝnh vÒ kü n¨ng c¬ b¶n");
}

int JxKyNang_ChamBangChon(int x, int y)
{
	int nCao = BC_CAO_TEN + BC_CAO_NUT * BC_SO_NUT;
	int nNut;
	KUiGameObject o;

	if (!s_nBCBat)
		return 0;
	if (x < s_nBCX || x >= s_nBCX + BC_RONG || y < s_nBCY || y >= s_nBCY + nCao)
	{
		s_nBCBat = 0;		// cham ra ngoai: dong bang, cu cham di tiep
		return 0;
	}
	nNut = (y - s_nBCY - BC_CAO_TEN) / BC_CAO_NUT;	// < 0 = dong ten
	o = s_BCKN;
	s_nBCBat = 0;
	if (y - s_nBCY < BC_CAO_TEN)
		return 1;
	if (nNut == 0)
	{	// gan o CHINH
		s_nKNCheDoGan = 1;
		s_nKNOChon = KYNANG_CHON_CHINH;
		JxKyNang_GanKyNang(o.uGenre, o.uId);
	}
	else if (nNut == 1)
	{	// gan o PHU: o da chon truoc (cham o trong) -> gan luon; khong thi o trong dau tien; het o thi cho cham
		int i;

		KyNang_DocGan();
		if (s_nKNCheDoGan && s_nKNOChon >= 0 && s_nKNOChon < KYNANG_SO_PHU)
			JxKyNang_GanKyNang(o.uGenre, o.uId);
		else
		{
			for (i = 0; i < KYNANG_SO_PHU; i++)
			{
				if (s_KNGan[i].uId == 0)
					break;
			}
			if (i < KYNANG_SO_PHU)
			{
				char szBao[96];

				s_nKNCheDoGan = 1;
				s_nKNOChon = i;
				JxKyNang_GanKyNang(o.uGenre, o.uId);
				_snprintf(szBao, sizeof(szBao), "%s %d", "§· g¾n vµo « phô", i + 1);
				szBao[sizeof(szBao) - 1] = 0;
				KyNang_Bao(szBao);
			}
			else
			{
				s_KNCho = o;
				s_nKNCheDoGan = 1;
				s_nKNOChon = -1;
				KyNang_Bao("HÕt « trèng: ch¹m « phô muèn thay");
			}
		}
	}
	else if (nNut == 2)
		KyNang_GoChinh();		// [ANDROID 10/09 NUTVNKU]
	else
		KyNang_GoKhoiO(&o);
	return 1;
}

static void KyNang_VeBangChon()
{
	int i, nY, nL, nT, nW, nH;

	if (!s_nBCBat || g_pRepresentShell == NULL)
		return;
	// cua so ky nang dong roi thi bang cung dong
	if (KUiSkillsNew::GetIfVisible() == NULL && KUiSkills::GetIfVisible() == NULL)
	{
		s_nBCBat = 0;
		return;
	}
	// [ANDROID 10/09 LUAN] dat NGAY CANH khung thong tin ky nang dang hien (ben phai; het cho thi ben trai)
	if (g_MouseOver.JxLayKhung(&nL, &nT, &nW, &nH))
	{
		s_nBCX = nL + nW + 4;
		if (s_nBCX + BC_RONG > SCREEN_WIDTH - 2)
			s_nBCX = nL - 4 - BC_RONG;
		s_nBCY = nT;
		if (s_nBCX < 2)											s_nBCX = 2;
		if (s_nBCY + BC_CAO_NUT * BC_SO_NUT > SCREEN_HEIGHT - 2)	s_nBCY = SCREEN_HEIGHT - 2 - BC_CAO_NUT * BC_SO_NUT;
		if (s_nBCY < 2)											s_nBCY = 2;
	}
	nY = s_nBCY + BC_CAO_TEN;
	// 4 nut anh cua VNKU (khung 0 = binh thuong), 236x86 -> 84x31
	for (i = 0; i < BC_SO_NUT; i++, nY += BC_CAO_NUT)
	{
		if (CoAnh(s_szBCAnh[i]))
			VeAnhKhung(s_szBCAnh[i], s_nBCX + BC_RONG / 2, nY + BC_CAO_NUT / 2, BC_NUT_RONG, BC_NUT_CAO, 0);
		else
			HopChuNhat(s_nBCX + 2, nY + 2, s_nBCX + BC_RONG - 2, nY + BC_CAO_NUT - 2, 0xC0304868);
	}
}
void JxKyNang_Ve()
{
	int i, nX, nY, nR, nIcon;
	KUiGameObject o;

	DocCaiDat();
	if (!s_nKNBat || g_pCoreShell == NULL || g_pRepresentShell == NULL)
		return;
	KyNang_DangKySuaToaDo();	// [UITOADO 09/09 E] cho doi cho duoc trong che do sua
	if (s_nKNCoAnh < 0)
	{
		s_nKNCoAnh = (CoAnh(s_szKNAnhChinh) && CoAnh(s_szKNAnhPhu)
			&& CoAnh(s_szKNAnhPhuNho)) ? 1 : 0;
		g_DebugLog("[KYNANG] anh nut: %s -> co anh=%d", s_szKNAnhChinh, s_nKNCoAnh);
	}
	KyNang_DocBang();

	// Ve tu o phu ra ngoai roi den nut chinh, de nut chinh nam TREN CUNG.
	for (i = KYNANG_SO_PHU; i >= 0; i--)
	{
		bool bCo = KyNang_CuaNut(i, &o);

		// [ANDROID 10/09 OTRONG] o phu TRONG van ve khung tron (VNKU ve vong "+"), chu: "mat mot so nut ky nang phu".
		// (Truoc: khong o che do sua thi bo qua -> tuong mat nut.)
		KyNang_TamNut(i, &nX, &nY);
		nR = KyNang_CoNut(i) / 2;

		// khung tron cua VNKU; thieu anh thi lui ve o mau cho van dung duoc
		if (s_nKNCoAnh)
			VeAnhCo(KyNang_AnhCuaNut(i), nX, nY, nR * 2, 0);
		else
			OVuong(nX, nY, nR, (i == s_nKNDangCam) ? 0xB03A8A3A : 0x80202020);

		// [ANDROID 09/09 GAN] o dang cho gan: to nen sang cho de nhan
		if (s_nKNCheDoGan && ((i > 0 && (i - 1) == s_nKNOChon) || (i == 0 && s_nKNOChon == KYNANG_CHON_CHINH)))
			OVuong(nX, nY, nR, 0x80FFD24A);

		// [ANDROID 09/09 KYNANG D] KSkill::DrawSkillIcon (KSkills.cpp:2861) BO QUA
		// Width/Height - no ve anh o co THAT, lay (x, y) lam goc TRAI TREN. Nen phai
		// tu canh giua theo co that (KyNangCoIcon), khong the nho ham do co lai.
		if (bCo)
		{
			nIcon = s_nKNCoIcon;
			g_pCoreShell->DrawGameObj(o.uGenre, o.uId,
				nX - nIcon / 2, nY - nIcon / 2, nIcon, nIcon, 0);
		}
		// [ANDROID 10/09 LUAN] o phu giu VONG SANG: vong xoay (16 khung, ~1 vong/giay) nhu ban tham khao (CCRotateBy 360/1s)
		// [ANDROID 10/09 LUAN c] chi o vong sang DANG BAT (= ky nang phai) moi co vong xoay - KgameWorldVN.cpp:8642
		if (bCo && i > 0 && KyNang_OLaAura(i, &o) && o.uId == s_KNPhai.uId && CoAnh(s_szKNAnhXoay))
			VeAnhCo(s_szKNAnhXoay, nX, nY, nR * 2 + 12, (int)(((unsigned int)GetTickCount() / 62) % KYNANG_XOAY_KHUNG));
	}

	// [ANDROID 09/09 GAN] nut doi che do gan - ve sau cung de nam tren
	if (CoAnh(s_szKNAnhGan))
	{
		int nGX, nGY;

		KyNang_TamNutGan(&nGX, &nGY);
		if (s_nKNCheDoGan)
		{
			OVuong(nGX, nGY, KYNANG_GAN_CO / 2, 0x90FFD24A);
			// [ANDROID 10/09 GANTOADO] huong dan ngay tren man hinh, tung buoc mot
			KyNang_VeChu(s_nKNOChon < 0
				? "ChÕ ®é g¸n: ch¹m vµo « kü n¨ng muèn ®æi"
				: "Giê më b¶ng kü n¨ng, ch¹m mét kü n¨ng ®Ó g¸n vµo « ®ang s¸ng",
				nGX - 300, nGY - 38, 0xFFFFD24A);
		}
		VeAnhCo(s_szKNAnhGan, nGX, nGY, KYNANG_GAN_CO, 0);
	}

	// Dang giu mot nut: vong sang ngam + vach chi huong + vong duoi chan con dich.
	if (s_nKNDangCam >= 0)
	{
		KyNang_TamNut(s_nKNDangCam, &nX, &nY);
		if (CoAnh(s_szKNAnhNgam))	// vong sang effect_skill.spr cua VNKU, vua khit nut
			VeAnhCo(s_szKNAnhNgam, nX, nY, KyNang_CoNut(s_nKNDangCam) * 3 / 2, 0);

		// [ANDROID 09/09 NGHIENG] Khong ve vach nua. Thay bang NUM chay trong ban kinh
		// nut theo huong ngon tay - dung cach cua ban tham khao (KuiMyMenu jsSprite,
		// setSpritePiont): nut "nghieng" ve phia dang chi. Anh num la joystick_ctrl.spr
		// cua VNKU, cung bo voi khung nut.
		{
			int dx = s_nKNNgonX - nX;
			int dy = s_nKNNgonY - nY;
			int nDai = (int)sqrt((double)(dx * dx + dy * dy));
			int nBK = KyNang_CoNut(s_nKNDangCam) / 2;	// num chay trong long nut
			int nNumX = nX, nNumY = nY;

			if (nDai > nBK && nDai > 0)
			{
				nNumX = nX + dx * nBK / nDai;
				nNumY = nY + dy * nBK / nDai;
			}
			else
			{
				nNumX = nX + dx;
				nNumY = nY + dy;
			}
			if (CoAnh(s_szKNAnhNum))
				VeAnhCo(s_szKNAnhNum, nNumX, nNumY, nBK, 0);
		}

		if (s_nKNDichIdx)
			KyNang_VeVongDich(s_nKNDichX, s_nKNDichY);

		// [ANDROID 09/09 HUONGDI] VONG TAM DANH tren mat dat o diem ngam - ban tham
		// khao ve bang JX1M_AimVeVong(_BackAttackRadius, moveEndPoint). Anh la vong ELIP
		// (det theo truc Y) vi goc nhin nghieng, nen ve rong gap doi chieu cao.
		if (s_nKNCoNgam && CoAnh(s_szKNAnhTam))
		{
			KUiGameObject oT;
			int nTamVe = 120;

			if (KyNang_CuaNut(s_nKNDangCam, &oT))
			{
				KJxKyNangHoi oH;
				memset(&oH, 0, sizeof(oH));
				oH.nSkillId = (int)oT.uId;
				if (g_pCoreShell->GetGameData(GDI_KYNANG_MOBILE, (KUPARAM)&oH, 1)
					&& oH.nTamDanh > 0)
					nTamVe = oH.nTamDanh;
			}
			if (nTamVe < 48)  nTamVe = 48;
			if (nTamVe > 400) nTamVe = 400;
			VeAnhKhung(s_szKNAnhTam, s_nKNNgamX, s_nKNNgamY, nTamVe, nTamVe / 2, 0);
		}

		// [ANDROID 09/09 KYNANG H] MUI TEN DINH HUONG DANH tai vi tri dang ngam.
		// attack_direction.spr co mot khung cho moi huong - chon khung theo goc ngam
		// thi mui ten quay dung phia dang chi.
		if (s_nKNCoNgam && CoAnh(s_szKNAnhTen))
		{
			int nSo = KyNang_SoKhung(s_szKNAnhTen);
			int nKhung = 0;

			if (nSo > 1)
			{
				int dx = s_nKNNgamX - SCREEN_WIDTH / 2;
				int dy = s_nKNNgamY - SCREEN_HEIGHT / 2;
				// goc 0 = sang phai, tang nguoc chieu kim dong ho (truc Y man hinh huong xuong)
				double fGoc = atan2((double)(-dy), (double)dx) * 180.0 / 3.14159265358979;
				if (fGoc < 0) fGoc += 360.0;
				nKhung = (int)((fGoc * nSo + 180.0) / 360.0) % nSo;
			}
			VeAnh2(s_szKNAnhTen, s_nKNNgamX, s_nKNNgamY, nKhung);
		}
	}

	// [ANDROID 10/09 BANGCHON] bang 3 nut + dong thong bao ngan
	KyNang_VeBangChon();
	if (s_szKNBao[0])
	{
		if ((unsigned int)GetTickCount() - s_uKNBaoLuc < 3000)
			KyNang_VeChu(s_szKNBao, SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 70, 0xFFFFD24A);
		else
			s_szKNBao[0] = 0;
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
