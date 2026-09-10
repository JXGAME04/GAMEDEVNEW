//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B0] Ben gui WAuto trong tien trinh - xem JxWAutoNoiBo.h.
// Chu Viet trong chuoi: TCVN3 (sinh bang vn_edit.py trong android/va_nguon_android_wauto1.py), khong dung Edit/Write thuong.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_ANDROID
#include "JxWAutoNoiBo.h"
#include "KFile.h"
// [ANDROID 11/09 WAUTO B0 b] khong dung g_CreatePath (KFilePath.h): tren Android no khong doi dau '\\' truoc khi goi SDL_CreateDirectory
#include "KDebug.h"			// g_DebugLog
#include "../Ui/UiCase/UiMsgCentrePad.h"
#include <stdio.h>
#include <string.h>

#include "../../Core/src/coreshell.h"	// [ANDROID 11/09 WAUTO B2 i b] iCoreShell day du: LockSomeoneAction / LockObjectAction
extern iCoreShell*		g_pCoreShell;
extern SharedState*		g_pState;		// S3Client.cpp: hop thu lenh cua WAuto (Android: calloc trong tien trinh)
extern HANDLE			g_hEventRecv;	// S3Client.cpp: tin hieu "co lenh moi" (Android: pthread cond, auto-reset)
extern "C" int JxCore_WAutoNhanVat(unsigned int* puId, int* pnLifeMax, int* pnManaMax);	// CoreShell.cpp (chi Android)

#define WA_NHIP_MS		54				// = GAMELOOPINTV cua WAuto.exe
#define WA_THU_MUC		"\\APdata"		// cung thu muc voi WAuto.exe ben PC

static autoData		s_CauHinh;			// constructor = mac dinh cua struct (nhu WAuto.exe)
static int			s_nBat = 0;
static int			s_nDaDocCfg = 0;
static UINT			s_uNhipKe = 0;
static UINT			s_uBaoKe = 0;
static unsigned int	s_uId = 0;			// ma nhan vat cua cau hinh dang nap (0 = chua vao game)
static int			s_nTickDaBao = -1;	// trang thai da bao cho bo nao (PRT_TICKSTART): -1 chua, 0 tat, 1 bat
static UINT			s_uNhatDen = 0;		// [ANDROID 11/09 WAUTO B2 i] dot nhat ngay: gui nhip toi luc nay (0 = khong)
static int			s_nNhatMuonAuto = 0;	// auto dang tat luc bat dau dot -> het dot phai bao bo nao dung (ATYPE_CLEAR)

static void WA_DocConfig()
{
	char szCfg[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCfg);
	strcat(szCfg, "\\Config.ini");
	s_nBat = GetPrivateProfileInt("WAuto", "Bat", 0, szCfg) ? 1 : 0;
	g_DebugLog("[WAUTO] config.ini [WAuto] Bat=%d; sizeof(autoData)=%u", s_nBat, (unsigned)sizeof(autoData));
}

static void WA_TenTep(char* sz, int nMax, unsigned int uId)
{
	snprintf(sz, nMax, WA_THU_MUC "\\%u.dat", uId);
}

// Mac dinh LAN DAU y het WAuto.exe (LoadRoleData khi chua co tep .dat): danh + nhat + uong thuoc theo 2/3 va 1/3 mau.
// Cung dung lam nen cho tep CU ngan hon struct: phan duoi tep khong phu toi giu gia tri nay (WAuto.exe lam y vay o
// cac nhanh nang cap theo kich thuoc tep).
static void WA_MacDinhLanDau(autoData& ap, int nLifeMax, int nManaMax)
{
	ap.nIlifeCell1 = nLifeMax / 3 * 2;
	ap.nIlifeCell2 = nLifeMax / 3;
	ap.nIlifeCell3 = 1000;
	ap.nImanaCell1 = nManaMax / 3 * 2;
	ap.nImanaCell2 = nManaMax / 3;
	ap.nImanaCell3 = 1000;
	ap.bCheckiLife = 1;
	ap.bCheckiMana = 1;
	ap.bFight = 1;
	ap.nVision = 1000;
	ap.nNearDist = 75;
	ap.nFBVision = 600;
	ap.nSLSPerc = 50;
	ap.nSMSPerc = 50;
	ap.nSkillCSec = 1;
	ap.uFKey = 0x20 /* VK_SPACE */;
	ap.nPKVision = 800;
	ap.nPKNearDist = 75;
	ap.bPKFollowTG = 1;
	ap.bPKPlayer = 1;
	ap.bPKNpc = 1;
	ap.bPickUp = 1;
	ap.bFollowPick = 1;
	ap.nPickVision = 400;
	ap.nLeavePtMem = 5;
	ap.nLeavePtMin = 10;
	ap.nRemovePtMin = 10;
	ap.bRepair = 1;
	ap.bFRepair = 1;
	ap.nWDMoney = 20;
	ap.nBLNum = 15;
	ap.nBMNum = 10;
	ap.nBPNum = 5;
	ap.nBTPNum = 1;
	ap.nBuyLifeSel = 9;
	ap.nBuyManaSel = 4;
	ap.nBuyPoisSel = 4;
	ap.bSaveItem = 1;
	ap.nFollowDist = 100;
	ap.bDTType[0] = 1;
	ap.bDTType[1] = 1;
	ap.bDTType[2] = 1;
	ap.bDTType[3] = 1;
	ap.bDTType[4] = 1;
	ap.bDTType[5] = 1;
	ap.bDTUseBox = 1;
	ap.nDTReward1 = 0;
	ap.nDTReward2 = 2;
	ap.nDTWDMoney = 50;
	ap.bDTLenhBai = 1;
	ap.bDTMuaSap = 1;
	ap.nDTMaxMua = 200;
	ap.bTongKim = 0;
	ap.bTKGio[0] = 1;
	ap.bTKGio[1] = 1;
	ap.bTKGio[2] = 1;
	ap.bTKGio[3] = 1;
	ap.nTKSom = 2;
	ap.nTKLech = 0;
	ap.nTKPhe = 2;
	ap.nTKMuaMau = 0;
	ap.nTKSoBinh = 20;
	ap.bTKThuoc = 1;
	ap.nTKThuocSel = 0;
	ap.nTKUuTien = 1;
	ap.bTKVeCho = 1;
	ap.nTKVeThanh = 0;
	ap.bTKRuong = 0;
	ap.nTKRuongHuong = 5;
	ap.bTKRuongCat = 1;
	ap.bPKBoQuaKhien = 1;
	ap.bTimAcChinh = 0;
	ap.nAcChinhKC = 200;
	ap.bAcChinhThanh = 0;
	ap.bCungMucTieu = 0;
	ap.bAcChinhVaoMap = 1;
	ap.szAcChinhTen[0] = 0;
	ap.bCongThanh = 0;
	ap.nCTGio = 20;
	ap.nCTPhut = 0;
	ap.nCTLech = 0;
	ap.nCTSom = 3;
	ap.nCTCua = 95;
	ap.nCTThanh = 7;
	ap.nCTPhe = 0;
	ap.nCTCong = 0;
	ap.nCTThu = 0;
	ap.nCTVe = 7;
	ap.bCTLoa = 1;
	ap.bCTMua = 1;
	ap.nCTSoBinh = 10;
	ap.nCTCapBinh = 5;
	ap.bWANpcTheSame = 0;
	ap.bWAMissle = 0;
	ap.nWAMissleIndex = 1;
	ap.bLienDau = 0;
	ap.bLDKhung[0] = 1;
	ap.bLDKhung[1] = 1;
	ap.nLDGio[0] = 18;
	ap.nLDPhut[0] = 0;
	ap.nLDGio[1] = 20;
	ap.nLDPhut[1] = 0;
	ap.nLDLuot = 4;
	ap.nLDPhutLuot = 15;
	ap.nLDSom = 5;
	ap.nLDLech = 0;
	ap.bLDTuLap = 1;
	ap.szLDTen[0] = 0;
	ap.bLDCatDoCam = 1;
	ap.bLDNhanThuong = 1;
	ap.nLDVeThanh = 7;
	ap.bLDPhuVe = 0;
	ap.bLDParty = 0;
	ap.nLDThanhBD = 7;
	ap.nLDPtCount = 0;
	ap.bHDBachNhan = 0;
	ap.nHDBNGio = 12;
	ap.nHDBNPhut = 5;
	ap.nHDBNCay = 0;
	ap.nHDBNMode = 0;
	ap.nHDBNDai = 0;
	ap.bHDBNBuff = 1;
	ap.bHDBangChien = 0;
	ap.nHDBCGio = 20;
	ap.nHDBCPhut = 25;
	ap.nHDLech = 0;
	ap.bHDTinSu = 0;
	ap.nHDTSGio = 9;
	ap.nHDTSPhut = 30;
	ap.nHDTSLuot = 2;
	ap.bCombo = 1;
	ap.bComboNoUT = 1;
	ap.bTienChieu = 0;
	ap.nTCKieu = 0;
	ap.nTCMs = 8000;
	ap.nTCDist = 120;
	ap.nTCHoi = 6;
}

int JxWAuto_NapCauHinh()
{
	unsigned int uId = 0;
	int nLifeMax = 0, nManaMax = 0;
	// [ANDROID 11/09 WAUTO B0 c] Ban mac dinh lay tu doi tuong TINH: bo nho tinh duoc xoa trang truoc khi constructor chay, nen cac
	// mang chuoi (szIJPtName, szNOPName, szLDPtName...) la 0. Bien tam tren ngan xep thi constructor KHONG xoa mang -> rac
	// ('r8v') lot vao tep .dat (thay o lan thu B0 thu hai).
	static const autoData s_MacDinh;
	s_CauHinh = s_MacDinh;					// ve mac dinh cua struct truoc
	s_nTickDaBao = -1;
	if (!JxCore_WAutoNhanVat(&uId, &nLifeMax, &nManaMax) || !uId)
	{
		s_uId = 0;
		return 0;
	}
	s_uId = uId;
	WA_MacDinhLanDau(s_CauHinh, nLifeMax, nManaMax);
	char sz[128];
	WA_TenTep(sz, sizeof(sz), uId);
	KFile f;
	if (!f.Open(sz))
	{
		g_DebugLog("[WAUTO] chua co %s -> mac dinh lan dau (danh + nhat + uong thuoc), mau %d / noi luc %d; ghi ra tep", sz, nLifeMax, nManaMax);
		JxWAuto_LuuCauHinh();
		return 0;
	}
	DWORD dwCo = f.Size();
	if (dwCo > sizeof(autoData))
		dwCo = (DWORD)sizeof(autoData);
	DWORD dwDoc = f.Read(&s_CauHinh, dwCo);		// tep cu ngan hon struct: phan duoi giu mac dinh lan dau
	f.Close();
	g_DebugLog("[WAUTO] nap %s: %u/%u byte; fight=%d vis=%d pick=%d life=%d TK=%d DT=%d", sz, (unsigned)dwDoc, (unsigned)sizeof(autoData),
		s_CauHinh.bFight, s_CauHinh.nVision, s_CauHinh.bPickUp, s_CauHinh.bCheckiLife, s_CauHinh.bTongKim, s_CauHinh.bDaTau);
	return 1;
}

int JxWAuto_LuuCauHinh()
{
	if (!s_uId)
		return 0;
	// [ANDROID 11/09 WAUTO B0 b] Dung lop gia lap CreateDirectory (KPosixWin32.cpp): JxPathPosix doi '\\' -> '/', ghep thu muc du lieu,
	// ha chu thuong (=> <du lieu>/apdata) - dung duong ma KFile::Create dung cho tep ben trong. Da co thi tra FALSE, vo hai.
	CreateDirectory(WA_THU_MUC, NULL);
	char sz[128];
	WA_TenTep(sz, sizeof(sz), s_uId);
	KFile f;
	if (!f.Create(sz))
	{
		g_DebugLog("[WAUTO] KHONG ghi duoc %s", sz);
		return 0;
	}
	DWORD dwGhi = f.Write(&s_CauHinh, sizeof(autoData));
	f.Close();
	g_DebugLog("[WAUTO] ghi %s: %u byte", sz, (unsigned)dwGhi);
	return dwGhi == sizeof(autoData) ? 1 : 0;
}

autoData* JxWAuto_CauHinh()			{ return &s_CauHinh; }
unsigned int JxWAuto_IdNhanVat()	{ return s_uId; }
int JxWAuto_DangBat()				{ return s_nBat; }
int JxWAuto_DangNhat()				{ return s_uNhatDen != 0; }	// [ANDROID 11/09 WAUTO B2 i]
void JxWAuto_NhatNgay(int nMs)
{
	if (nMs <= 0)
	{
		s_uNhatDen = 0;
		return;
	}
	if (!s_uNhatDen)
	{
		s_nNhatMuonAuto = !s_nBat;
		if (g_pCoreShell)			// dang khoa muc tieu (danh ai) thi go de nhan vat chiu roi cho di nhat
		{
			g_pCoreShell->LockSomeoneAction(0);
			g_pCoreShell->LockObjectAction(0);
		}
	}
	s_uNhatDen = timeGetTime() + (UINT)nMs;
	if (!s_uNhatDen)
		s_uNhatDen = 1;
	s_uNhipKe = 0;					// gui nhip dau ngay khung nay
	g_DebugLog("[WAUTO] NHAT NGAY %d ms (auto dang %s)", nMs, s_nBat ? "bat" : "tat");
}
int JxWAuto_Bat(int bBat)			{ s_nBat = bBat ? 1 : 0; return s_nBat; }

// Nap goi vao hop thu cua ProcIpcCommand y nhu AppLoop cua WAuto.exe: [so goi][goi 1][goi 2]... roi SetEvent.
static void WA_GuiGoi(const void* p, unsigned int n, unsigned int uSo)
{
	if (!g_pState || !g_hEventRecv || sizeof(UINT) + n > SHARED_SIZE)
		return;
	*(UINT*)g_pState = uSo;
	memcpy((BYTE*)g_pState + sizeof(UINT), p, n);
	SetEvent(g_hEventRecv);
}

// Bao bo nao bat/tat (PRT_TICKSTART -> ATYPE_CLEAR: xoa trang thai auto; tat thi dung nhan vat lai) + mot dong o khung chat.
static void WA_BaoTick(int bBat, int bIm = 0)	// [ANDROID 11/09 WAUTO B2 i] bIm = 1: chi bao bo nao (het dot nhat), khong ghi chat
{
	IPCHideGame s;
	s.CmdID = PRT_TICKSTART;
	s.Size = sizeof(IPCHideGame);
	s.bHide = bBat;
	WA_GuiGoi(&s, sizeof(s), 1);
	if (bIm)
	{
		g_DebugLog("[WAUTO] het dot NHAT NGAY (PRT_TICKSTART -> ATYPE_CLEAR, im lang)");
		return;
	}
	const char* sz = bBat ? "BËt auto trong game (WAuto)" : "T¾t auto trong game (WAuto)";
	KUiMsgCentrePad::SystemMessageArrival(sz, (unsigned short)strlen(sz));
	g_DebugLog("[WAUTO] %s (PRT_TICKSTART -> ATYPE_CLEAR)", bBat ? "BAT" : "TAT");
}

void JxWAuto_NhipVongLap()
{
	if (!s_nDaDocCfg)
	{
		s_nDaDocCfg = 1;
		WA_DocConfig();
	}
	if (!g_pCoreShell || !g_pState)
		return;
	unsigned int uId = 0;
	int nL = 0, nM = 0;
	if (!JxCore_WAutoNhanVat(&uId, &nL, &nM) || !uId)
	{
		if (s_uId)
		{
			s_uId = 0;			// vua roi map / dang xuat: lan vao lai se nap lai cau hinh
			s_nTickDaBao = -1;
		}
		return;
	}
	if (uId != s_uId)
		JxWAuto_NapCauHinh();
	if (s_nTickDaBao != s_nBat)
	{
		s_nTickDaBao = s_nBat;
		WA_BaoTick(s_nBat);
		return;					// khung nay hop thu da co goi; goi vong lap gui tu khung sau
	}
	UINT uNow = timeGetTime();
	int bNhat = (s_uNhatDen != 0);		// [ANDROID 11/09 WAUTO B2 i] dot nhat ngay (nut ban tay)
	if (bNhat && (int)(uNow - s_uNhatDen) >= 0)
	{
		s_uNhatDen = 0;
		bNhat = 0;
		if (s_nNhatMuonAuto)			// auto von tat: bao bo nao xoa trang thai, dung nhan vat
			WA_BaoTick(0, 1);
		s_nNhatMuonAuto = 0;
	}
	if (!s_nBat && !bNhat)
		return;
	if (uNow < s_uNhipKe)
		return;
	s_uNhipKe = uNow + WA_NHIP_MS;
	static BYTE s_Goi[sizeof(IPCGameLoop) + sizeof(IPCHienThi)];
	IPCGameLoop* pGL = (IPCGameLoop*)s_Goi;
	pGL->CmdID = PRT_GAMELOOP;
	pGL->Size = sizeof(IPCGameLoop);
	memcpy(&pGL->setting, &s_CauHinh, sizeof(autoData));
	// Mot may = mot nhan vat: khong co ac chinh / ac phu (the Ac chinh khong dung tren mobile - ma va truong van giu nguyen).
	pGL->setting.nACLaChinh = 0;
	pGL->setting.szAcChinhTen[0] = 0;
	pGL->setting.bTimAcChinh = 0;
	pGL->setting.bCungMucTieu = 0;
	pGL->setting.bAcChinhVaoMap = 0;
	pGL->setting.uACTuoi = 0;
	if (bNhat)
	{
		// [ANDROID 11/09 WAUTO B2 i] dot nhat ngay: KHONG danh (bFight / bOnPK = 0), NHAT + chay toi (bFollowPick), tam nhat >= 800
		pGL->setting.bFight = 0;
		pGL->setting.bOnPK = 0;
		pGL->setting.bPickUp = 1;
		pGL->setting.bFollowPick = 1;
		if (pGL->setting.nPickVision < 800)
			pGL->setting.nPickVision = 800;
		pGL->setting.bCityPick = 1;	// [ANDROID 11/09 WAUTO B2 i d] ATYPE_PICKUP bo qua khi khong o the chien dau tru khi bCityPick
		pGL->setting.bTongKim = 0;	// cac may hoat dong cung nghi trong dot
		pGL->setting.bDaTau = 0;
	}
	IPCHienThi* pHT = (IPCHienThi*)(s_Goi + sizeof(IPCGameLoop));
	pHT->CmdID = PRT_HIENTHI;
	pHT->Size = sizeof(IPCHienThi);
	pHT->bNpcTheSame = s_CauHinh.bWANpcTheSame;
	pHT->nMissleIndex = s_CauHinh.bWAMissle ? s_CauHinh.nWAMissleIndex : 0;
	WA_GuiGoi(s_Goi, sizeof(s_Goi), 2);
	if (uNow >= s_uBaoKe)
	{
		s_uBaoKe = uNow + 5000;
		g_DebugLog("[WAUTO] nhip: id=%u fight=%d vis=%d pick=%d life=%d/%d TK=%d DT=%d", s_uId, s_CauHinh.bFight, s_CauHinh.nVision,
			s_CauHinh.bPickUp, s_CauHinh.bCheckiLife, s_CauHinh.nIlifeCell1, s_CauHinh.bTongKim, s_CauHinh.bDaTau);
	}
}
#endif // JX_ANDROID
