/*****************************************************************************************
//	[UITOADO] Che do sua giao dien ngay trong game - xem UiToaDo.h.
*****************************************************************************************/
#include "KWin32.h"
#include "KFilePath.h"
#include "KDebug.h"
#include "WndWindow.h"
#include "Wnds.h"
#include "UiToaDo.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"

#include <typeinfo>
#include <stdio.h>
#include <string.h>

extern iRepresentShell*	g_pRepresentShell;

#define	UITOADO_TEP			"\\UserData\\UiToaDo.ini"
#define	UITOADO_CAUHINH		"\\config.ini"
#define	UITOADO_MUC			"[Pos]"
#define	UITOADO_MAX			2048
#define	UITOADO_CO_KHOA		160			// ten lop 64 + '|' + muc ini 64, du cho
#define	UITOADO_CO_AN		1			// bit 1 cua truong "Co" = an han o nay

#define	UITOADO_TILE_MIN	300
#define	UITOADO_TILE_MAX	3000
#define	UITOADO_TILE_NAC	50			// moi nac lan chuot = 5%

#define	UITOADO_BANG_X		8
#define	UITOADO_BANG_Y		64
#define	UITOADO_BANG_CAO	14
#define	UITOADO_BANG_RONG	330
#define	UITOADO_BANG_MAXDONG 30

struct KOToaDo
{
	char	szKhoa[UITOADO_CO_KHOA];
	int		nLeft;
	int		nTop;
	int		nTiLe;			// phan nghin, 1000 = 100%
	int		nCo;			// bit UITOADO_CO_AN
};

static KOToaDo		s_Bang[UITOADO_MAX];
static int			s_nSo			= 0;
static int			s_bTran			= 0;		// bang day - phai bao, cam im lang

static bool			s_bDaDocCauHinh	= false;
static bool			s_bChoPhep		= false;
static bool			s_bDangSua		= false;
static int			s_nStyleCu		= 0;		// WND_SHOW_DEBUG_FRAME_TEXT truoc khi bat

static KWndWindow*	s_pKeo			= NULL;
static char			s_szKhoaKeo[UITOADO_CO_KHOA] = "";
static int			s_nKeoX			= 0;
static int			s_nKeoY			= 0;

//	cua so dang bi ep hien tam (chi mot cai mot luc)
static KWndWindow*	s_pEpHien		= NULL;
static unsigned int	s_uStyleEpHienCu = 0;

//	bang danh sach cua so
static bool			s_bHienBang		= false;
static KWndWindow*	s_pDongBang[UITOADO_BANG_MAXDONG];
static int			s_nSoDongBang	= 0;

static char			s_szThongBao[192] = "";
static unsigned int	s_uHetThongBao	= 0;


//--------------------------------------------------------------------------
//	[UITOADO 09/09] Thanh nut cham
//
//	Dien thoai khong co lan chuot / chuot phai / chuot giua / ban phim, ma ba
//	thu do lai chinh la duong duy nhat de to nho, doi ca khoi, an va luu.
//	Thanh nut nay lam duong THU HAI cho ca bon viec ay, chi can bam trai.
//	Moi phim chuot cu van giu nguyen cho ban PC.
//--------------------------------------------------------------------------
#define	UITOADO_NUT_CAO		34
#define	UITOADO_NUT_Y		4
#define	UITOADO_NUT_CACH	2
#define	UITOADO_MO_RONG		92		// nut "mo che do sua" luc chua sua
#define	UITOADO_MO_CAO		30

//	Cong cu dang chon: quyet dinh cham / keo vao mot o thi lam gi
enum
{
	CONGCU_DOI = 0,		// keo = doi cho MOT o   (= keo trai cu)
	CONGCU_KHOI,		// keo = doi cho CA KHOI (= keo phai cu)
	CONGCU_TO,			// cham = to them mot nac (= lan chuot len)
	CONGCU_NHO,			// cham = nho di mot nac  (= lan chuot xuong)
	CONGCU_AN,			// cham = an / hien lai   (= bam chuot giua)
	CONGCU_SO
};

//	Ma lenh cua nut khong phai cong cu
#define	NUT_BANG		100		// bat / tat bang danh sach cua so
#define	NUT_LUU			101		// luu va thoat (= Ctrl+U)
#define	NUT_XOAHET		102		// xoa het (= Ctrl+K)
#define	UITOADO_SO_NUT	8

static const struct
{
	const char*	pszTen;
	int			nMa;
} s_Nut[UITOADO_SO_NUT] =
{
	{ "Dêi «",	CONGCU_DOI },
	{ "Dêi khèi",	CONGCU_KHOI },
	{ "To h¬n",		CONGCU_TO },
	{ "Nhá l¹i",	CONGCU_NHO },
	{ "GiÊu/hiÖn",	CONGCU_AN },
	{ "Danh s¸ch",	NUT_BANG },
	{ "L­u",		NUT_LUU },
	{ "Xo¸ hÕt",	NUT_XOAHET },
};

static int			s_nCongCu		= CONGCU_DOI;
static bool			s_bDaDocViTriNut = false;
static int			s_nMoX			= -1;	// -1 = tu tinh (mep phai, giua man hinh)
static int			s_nMoY			= -1;

extern int SCREEN_WIDTH;		// S3Client.cpp / KSdlApp.cpp: co khung ve that
extern int SCREEN_HEIGHT;

//--------------------------------------------------------------------------
//	[UITOADO 09/09 E] Bang O VE TAY (khong phai cua so KWnd) - xem UiToaDo.h
//--------------------------------------------------------------------------
#define	UITOADO_ORIENG_MAX	8

struct KORieng
{
	char					szKhoa[64];
	PFN_UITOADO_TRUNG		pfnTrung;
	PFN_UITOADO_LAYVITRI	pfnLay;
	PFN_UITOADO_DATVITRI	pfnDat;
};

static KORieng		s_ORieng[UITOADO_ORIENG_MAX];
static int			s_nSoORieng = 0;
static int			s_nKeoORieng = -1;	// o rieng dang keo, -1 = khong

static void DatThongBao(const char* pszChu);

//--------------------------------------------------------------------------
//	Bang toa do trong bo nho
//--------------------------------------------------------------------------
static int TimKhoa(const char* pszKhoa)
{
	for (int i = 0; i < s_nSo; i++)
	{
		if (strcmp(s_Bang[i].szKhoa, pszKhoa) == 0)
			return i;
	}
	return -1;
}

static int DatKhoa(const char* pszKhoa, int nLeft, int nTop, int nTiLe, int nCo)
{
	int i = TimKhoa(pszKhoa);
	if (i < 0)
	{
		if (s_nSo >= UITOADO_MAX)
		{
			if (s_bTran == 0)
			{
				s_bTran = 1;
				DatThongBao("§Çy b¶ng, kh«ng nhí thªm ®­îc « nµo n÷a");
				g_DebugLog("[UITOADO] BANG DAY %d muc, khong ghi them duoc", UITOADO_MAX);
			}
			return -1;
		}
		i = s_nSo++;
		strncpy(s_Bang[i].szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);
		s_Bang[i].szKhoa[UITOADO_CO_KHOA - 1] = 0;
	}
	s_Bang[i].nLeft = nLeft;
	s_Bang[i].nTop  = nTop;
	s_Bang[i].nTiLe = nTiLe;
	s_Bang[i].nCo   = nCo;
	return i;
}

//--------------------------------------------------------------------------
//	Dinh danh cua mot o giao dien
//--------------------------------------------------------------------------
//	typeid tra ve "class KUiItem" -> chi lay "KUiItem"
static void LayTenLop(KWndWindow* pWnd, char* pszRa, int nCo)
{
	pszRa[0] = 0;
	if (pWnd == NULL)
		return;

	const char*	pszTen	= typeid(*pWnd).name();
	const char*	p		= strrchr(pszTen, ' ');

	if (p)
		pszTen = p + 1;				// MSVC: "class KUiFoo" -> "KUiFoo"

	//	[UITOADO 09/09 C] GCC / Itanium ABI (Android) tra ten MA HOA dang
	//	<do dai><ten>, vi du "16KUiMailManager" - khong co dau cach de cat.
	//	Bo cac chu so dau thi khoa luu giong het ban PC, nho vay tep
	//	UserData\UiToaDo.ini dung chung duoc cho ca hai ban.
	while (*pszTen >= '0' && *pszTen <= '9')
		pszTen++;
	strncpy(pszRa, pszTen, nCo - 1);
	pszRa[nCo - 1] = 0;
}

//	Tra ve false neu o nay khong dat ten duoc (chua bao gio Init tu ini).
static bool TaoKhoa(const char* pszLop, KWndWindow* pWnd, char* pszRa, int nCo)
{
	pszRa[0] = 0;
	if (pszLop == NULL || pszLop[0] == 0 || pWnd == NULL)
		return false;
	if (pWnd->GetMucIni()[0] == 0)
		return false;

	_snprintf(pszRa, nCo, "%s|%s", pszLop, pWnd->GetMucIni());
	pszRa[nCo - 1] = 0;
	return true;
}

//	[UITOADO 09/09 F] So thu tu cua mot o trong danh sach con cua cha no (-1 = khong ro).
static int SoThuTuCon(KWndWindow* pWnd)
{
	KWndWindow*	pCha;
	KWndWindow*	p;
	int			i = 0;

	if (pWnd == NULL)
		return -1;
	pCha = pWnd->GetParent();
	if (pCha == NULL)
		return -1;
	for (p = pCha->GetFirstChild(); p; p = p->GetNextWnd(), i++)
	{
		if (p == pWnd)
			return i;
	}
	return -1;
}

static bool TaoKhoaTuOCon(KWndWindow* pWnd, char* pszRa, int nCo)
{
	char szLop[64];

	pszRa[0] = 0;
	if (pWnd == NULL)
		return false;
	LayTenLop(pWnd->GetOwner(), szLop, sizeof(szLop));
	if (TaoKhoa(szLop, pWnd, pszRa, nCo))
		return true;

	//	[UITOADO 09/09 F] O nay khong nap tu ini nen khong co ten muc. Truoc day tra
	//	false -> ben goi LEO LEN CHA -> keo mot icon thanh keo CA CUM, va nhieu icon
	//	khong keo rieng duoc. Nay dat ten thay the theo VI TRI trong cay cua so:
	//		<ten lop cua so goc>|#<thu tu cua cha>.<thu tu cua no>
	//	Trinh tu dung giao dien co dinh moi lan chay nen khoa nay on dinh.
	if (szLop[0])
	{
		int nToi = SoThuTuCon(pWnd);
		int nCha = SoThuTuCon(pWnd->GetParent());

		if (nToi >= 0)
		{
			_snprintf(pszRa, nCo, "%s|#%d.%d", szLop, nCha, nToi);
			pszRa[nCo - 1] = 0;
			return true;
		}
	}
	return false;
}

//	Co nhung lop TUYET DOI khong duoc goi SetSize: chinh tac gia goc da canh bao
//	trong WndWindow.cpp, va KWndMessageListBox::SetSize con tinh lai so dong.
static bool CamCoGian(KWndWindow* pWnd)
{
	char szLop[64];

	LayTenLop(pWnd, szLop, sizeof(szLop));
	return (strstr(szLop, "ScrollBar") != NULL ||
			strstr(szLop, "MessageListBox") != NULL ||
			strstr(szLop, "List") != NULL);
}

//	Cam an nut dong / huy: an nham nut dong cua mot hop thoai doc quyen thi
//	chuot lan ban phim deu chet, Ctrl+K cung khong cuu duoc.
static bool CamAn(KWndWindow* pWnd)
{
	char	szMuc[80];
	int		i;

	strncpy(szMuc, pWnd->GetMucIni(), sizeof(szMuc) - 1);
	szMuc[sizeof(szMuc) - 1] = 0;
	for (i = 0; szMuc[i]; i++)
	{
		if (szMuc[i] >= 'A' && szMuc[i] <= 'Z')
			szMuc[i] = (char)(szMuc[i] + ('a' - 'A'));
	}
	return (strstr(szMuc, "close") != NULL ||
			strstr(szMuc, "cancel") != NULL ||
			strstr(szMuc, "exit") != NULL);
}

//--------------------------------------------------------------------------
//	config.ini [Ui] SuaToaDo
//--------------------------------------------------------------------------
void UiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,
		PFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat)
{
	int i;

	if (pszKhoa == NULL || pszKhoa[0] == 0 || pfnTrung == NULL
		|| pfnLay == NULL || pfnDat == NULL)
		return;
	for (i = 0; i < s_nSoORieng; i++)
	{
		if (strcmp(s_ORieng[i].szKhoa, pszKhoa) == 0)
			return;		// da dang ky roi
	}
	if (s_nSoORieng >= UITOADO_ORIENG_MAX)
		return;
	i = s_nSoORieng++;
	strncpy(s_ORieng[i].szKhoa, pszKhoa, sizeof(s_ORieng[i].szKhoa) - 1);
	s_ORieng[i].szKhoa[sizeof(s_ORieng[i].szKhoa) - 1] = 0;
	s_ORieng[i].pfnTrung = pfnTrung;
	s_ORieng[i].pfnLay   = pfnLay;
	s_ORieng[i].pfnDat   = pfnDat;

	//	Co vi tri da luu thi ap lai ngay
	{
		int n = TimKhoa(pszKhoa);
		if (n >= 0)
			pfnDat(s_Bang[n].nLeft, s_Bang[n].nTop);
	}
}

bool UiToaDo_ChoPhep()
{
	if (s_bDaDocCauHinh == false)
	{
		char szDuongDan[MAX_PATH];

		s_bDaDocCauHinh = true;
		g_GetFullPath(szDuongDan, (char*)UITOADO_CAUHINH);
		s_bChoPhep = (GetPrivateProfileInt("Ui", "SuaToaDo", 0, szDuongDan) != 0);
	}
	return s_bChoPhep;
}

//--------------------------------------------------------------------------
//	Doc / ghi UserData\UiToaDo.ini
//--------------------------------------------------------------------------
//	Mot dong:  <khoa> = Left , Top [, TiLe [, Co]]
//	Thieu TiLe -> 1000 (giu nguyen co).  Thieu Co -> 0.
void UiToaDo_Nap()
{
	char	szDuongDan[MAX_PATH];
	char	szDong[320];
	FILE*	pTep;

	s_nSo   = 0;
	s_bTran = 0;
	g_GetFullPath(szDuongDan, (char*)UITOADO_TEP);
	pTep = fopen(szDuongDan, "rt");
	if (pTep == NULL)
		return;

	while (fgets(szDong, sizeof(szDong), pTep))
	{
		char*	pBang;
		char*	p;
		char*	pSo[4];
		int		nSoTruong = 0;
		int		nGiaTri[4];
		int		i;

		//	bo khoang trang dau dong
		p = szDong;
		while (*p == ' ' || *p == '\t')
			p++;
		if (*p == ';' || *p == '[' || *p == '\r' || *p == '\n' || *p == 0)
			continue;

		pBang = strchr(p, '=');
		if (pBang == NULL)
			continue;
		*pBang = 0;

		//	tach toi da 4 truong ngan cach bang dau phay
		pSo[nSoTruong++] = pBang + 1;
		for (char* q = pBang + 1; *q && nSoTruong < 4; q++)
		{
			if (*q == ',')
			{
				*q = 0;
				pSo[nSoTruong++] = q + 1;
			}
		}
		for (i = 0; i < 4; i++)
			nGiaTri[i] = (i < nSoTruong) ? atoi(pSo[i]) : 0;
		if (nSoTruong < 3 || nGiaTri[2] <= 0)
			nGiaTri[2] = 1000;					// khong co / rong -> giu nguyen co
		if (nGiaTri[2] < UITOADO_TILE_MIN)
			nGiaTri[2] = UITOADO_TILE_MIN;
		if (nGiaTri[2] > UITOADO_TILE_MAX)
			nGiaTri[2] = UITOADO_TILE_MAX;

		//	cat khoang trang cuoi khoa
		{
			int n = strlen(p);
			while (n > 0 && (p[n - 1] == ' ' || p[n - 1] == '\t'))
				p[--n] = 0;
			if (n == 0)
				continue;
		}
		DatKhoa(p, nGiaTri[0], nGiaTri[1], nGiaTri[2], nGiaTri[3]);
	}
	fclose(pTep);
	g_DebugLog("[UITOADO] nap %d muc tu %s", s_nSo, szDuongDan);
}

static int GhiTep()
{
	char	szDuongDan[MAX_PATH];
	FILE*	pTep;
	int		i;

	g_GetFullPath(szDuongDan, (char*)UITOADO_TEP);
	pTep = fopen(szDuongDan, "wt");
	if (pTep == NULL)
	{
		g_DebugLog("[UITOADO] KHONG ghi duoc %s", szDuongDan);
		return -1;
	}

	fprintf(pTep, "; [UITOADO] Vi tri / co / an o giao dien do nguoi choi tu dat.\n");
	fprintf(pTep, "; Moi dong:  <ten lop cua so>|<ten muc ini> = Left,Top,TiLe,Co\n");
	fprintf(pTep, ";   TiLe : phan nghin, 1000 = 100%%  (%d..%d)\n",
		UITOADO_TILE_MIN, UITOADO_TILE_MAX);
	fprintf(pTep, ";   Co   : bit 1 = an han o nay\n");
	fprintf(pTep, "; Xoa het tep nay = tra giao dien ve dung \\Ui\\ui3\\*.ini goc.\n");
	fprintf(pTep, "%s\n", UITOADO_MUC);
	for (i = 0; i < s_nSo; i++)
	{
		fprintf(pTep, "%s=%d,%d,%d,%d\n", s_Bang[i].szKhoa,
			s_Bang[i].nLeft, s_Bang[i].nTop, s_Bang[i].nTiLe, s_Bang[i].nCo);
	}
	fclose(pTep);
	g_DebugLog("[UITOADO] ghi %d muc vao %s", s_nSo, szDuongDan);
	return s_nSo;
}

//--------------------------------------------------------------------------
//	Ap lai cho tung o
//--------------------------------------------------------------------------
static void ApMotO(KWndWindow* pWnd, int nMuc)
{
	if (nMuc < 0)
		return;
	pWnd->SetPosition(s_Bang[nMuc].nLeft, s_Bang[nMuc].nTop);
	//	KWndMovingImage chup vi tri goc cua hoat hinh mo cua so luc Init, tuc
	//	TRUOC luc nay => phai bao no chup lai, khong thi mo/dong lai la nhay ve cho cu
	pWnd->UiNhoViTri();
	if (s_Bang[nMuc].nTiLe > 0 && s_Bang[nMuc].nTiLe != 1000 && !CamCoGian(pWnd))
		pWnd->UiDatTiLe(s_Bang[nMuc].nTiLe);
	pWnd->UiDatAn(s_Bang[nMuc].nCo & UITOADO_CO_AN);
}

static void ApChoCay(const char* pszLop, KWndWindow* pWnd)
{
	char szKhoa[UITOADO_CO_KHOA];

	while (pWnd)
	{
		if (TaoKhoa(pszLop, pWnd, szKhoa, sizeof(szKhoa)))
			ApMotO(pWnd, TimKhoa(szKhoa));
		ApChoCay(pszLop, pWnd->GetFirstChild());
		pWnd = pWnd->GetNextWnd();
	}
}

void UiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc)
{
	char szLop[64];

	if (pCuaSoGoc == NULL || s_nSo == 0)
		return;

	LayTenLop(pCuaSoGoc, szLop, sizeof(szLop));
	if (szLop[0] == 0)
		return;

	//	chi cua so goc nay, khong lan sang anh em cua no
	{
		char szKhoa[UITOADO_CO_KHOA];

		if (TaoKhoa(szLop, pCuaSoGoc, szKhoa, sizeof(szKhoa)))
			ApMotO(pCuaSoGoc, TimKhoa(szKhoa));
	}
	ApChoCay(szLop, pCuaSoGoc->GetFirstChild());
}

//	[UITOADO 09/09 E] Ap lai vi tri da luu cho cac o ve tay.
static void ApChoORieng()
{
	for (int i = 0; i < s_nSoORieng; i++)
	{
		int n = TimKhoa(s_ORieng[i].szKhoa);
		if (n >= 0)
			s_ORieng[i].pfnDat(s_Bang[n].nLeft, s_Bang[n].nTop);
	}
}

void UiToaDo_ApChoTatCa()
{
	ApChoORieng();	// [UITOADO 09/09 E] ca cac o ve tay
	if (s_nSo == 0)
		return;

	for (int nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow*	pGoc = Wnd_GetLayerRoot(nLop);

		if (pGoc == NULL)
			continue;
		for (KWndWindow* pWnd = pGoc->GetNextWnd(); pWnd; pWnd = pWnd->GetNextWnd())
			UiToaDo_ApChoCuaSo(pWnd);
	}
}

//--------------------------------------------------------------------------
//	Ep hien tam mot cua so dang an
//--------------------------------------------------------------------------
//	Bat CA HAI bit WND_S_VISIBLE va WND_S_DISABLE:
//	  Paint / PtInWindow chi doi VISIBLE  -> ve duoc, bam duoc
//	  LetMeBreathe doi dung (VISIBLE|DISABLE) == VISIBLE -> Breathe() KHONG chay
//	=> khong kich hoat logic game trong Breathe cua cac hop thoai giao dich.
static void TraCoEpHien()
{
	if (s_pEpHien)
	{
		s_pEpHien->SetStyle(s_uStyleEpHienCu);
		s_pEpHien = NULL;
	}
}

static void EpHien(KWndWindow* pWnd)
{
	if (pWnd == NULL)
		return;
	if (s_pEpHien == pWnd)			// bam lai lan nua = thoi ep hien
	{
		TraCoEpHien();
		return;
	}
	TraCoEpHien();
	s_pEpHien = pWnd;
	s_uStyleEpHienCu = (unsigned int)pWnd->GetStyle();
	pWnd->SetStyle(s_uStyleEpHienCu | WND_S_VISIBLE | WND_S_DISABLE);
	pWnd->BringToTop();
}

void UiToaDo_QuenCuaSo(KWndWindow* pWnd)
{
	int i;

	if (s_pKeo == pWnd)
		s_pKeo = NULL;
	if (s_pEpHien == pWnd)
		s_pEpHien = NULL;			// da bi xoa roi, khong tra co nua
	for (i = 0; i < s_nSoDongBang; i++)
	{
		if (s_pDongBang[i] == pWnd)
			s_pDongBang[i] = NULL;
	}
}

//--------------------------------------------------------------------------
//	Bat / tat che do sua
//--------------------------------------------------------------------------
bool UiToaDo_DangSua()
{
	return s_bDangSua;
}

static void DatThongBao(const char* pszChu)
{
	strncpy(s_szThongBao, pszChu, sizeof(s_szThongBao) - 1);
	s_szThongBao[sizeof(s_szThongBao) - 1] = 0;
	s_uHetThongBao = GetTickCount() + 5000;
}

void UiToaDo_BatTat()
{
	char szChu[192];

	if (UiToaDo_ChoPhep() == false)
	{
		DatThongBao("Ch­a më: thªm [Ui] SuaToaDo=1 vµo config.ini");
		return;
	}

	if (s_bDangSua)
	{
		int nSo;

		TraCoEpHien();
		s_bDangSua  = false;
		s_pKeo      = NULL;
		s_bHienBang = false;
		WND_SHOW_DEBUG_FRAME_TEXT = s_nStyleCu;

		nSo = GhiTep();
		if (nSo < 0)
			DatThongBao("Lçi: kh«ng ghi ®­îc UserData\\UiToaDo.ini");
		else
		{
			_snprintf(szChu, sizeof(szChu), "§· l­u %d môc vµo UserData\\UiToaDo.ini", nSo);
			szChu[sizeof(szChu) - 1] = 0;
			DatThongBao(szChu);
		}
	}
	else
	{
		// [ANDROID 09/09 PHIMMA] ghi vet: che do nay tung TU BAT tren Android
		g_DebugLog("[UITOADO] BAT che do sua giao dien");
		s_bDangSua = true;
		s_pKeo     = NULL;
		s_nStyleCu = WND_SHOW_DEBUG_FRAME_TEXT;
		WND_SHOW_DEBUG_FRAME_TEXT = 1;
		s_szThongBao[0] = 0;
		//	co nhung o con duoc AddChild SAU khi Wnd_AddWindow da chay
		//	=> ap lai mot lan o day cho chac
		UiToaDo_ApChoTatCa();
		DatThongBao("§· vµo chÕ ®é söa giao diÖn");
	}
}

void UiToaDo_XoaHet()
{
	char szDuongDan[MAX_PATH];
	int	 nLop;

	//	hoi phuc NGAY trong phien: go co an, dua ti le ve 100%.
	//	(toa do thi phai khoi dong lai moi ve dung ini goc)
	for (nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow*	pGoc = Wnd_GetLayerRoot(nLop);

		if (pGoc == NULL)
			continue;
		for (KWndWindow* pWnd = pGoc->GetNextWnd(); pWnd; pWnd = pWnd->GetNextWnd())
		{
			//	duyet ca cay con bang de quy nho
			KWndWindow*	pNgan[64];
			int			nDinh = 0;

			pNgan[nDinh++] = pWnd;
			while (nDinh > 0)
			{
				KWndWindow* p = pNgan[--nDinh];

				p->UiDatAn(0);
				if (p->UiLayTiLe() != 1000)
					p->UiDatTiLe(1000);
				for (KWndWindow* pCon = p->GetFirstChild();
					 pCon && nDinh < 64; pCon = pCon->GetNextWnd())
					pNgan[nDinh++] = pCon;
			}
		}
	}

	s_nSo   = 0;
	s_bTran = 0;
	g_GetFullPath(szDuongDan, (char*)UITOADO_TEP);
	remove(szDuongDan);
	DatThongBao("§· xo¸ hÕt. Cì vµ « Èn vÒ nguyªn tr¹ng; to¹ ®é vÒ gèc sau khi më l¹i game");
	g_DebugLog("[UITOADO] da xoa %s va hoi phuc trong phien", szDuongDan);
}

//--------------------------------------------------------------------------
//	Tim o giao dien duoi con tro
//--------------------------------------------------------------------------
//	Tra ve cua so GOC (ca khoi) dung duoi diem (x, y), NULL neu khong co.
static KWndWindow* TimCuaSoGoc(int x, int y)
{
	KWndWindow*	pTrung = NULL;

	//	ve theo thu tu Low -> Normal -> Top, ve sau thi nam tren
	for (int nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow*	pGoc = Wnd_GetLayerRoot(nLop);

		if (pGoc == NULL)
			continue;
		for (KWndWindow* pWnd = pGoc->GetNextWnd(); pWnd; pWnd = pWnd->GetNextWnd())
		{
			if (pWnd->IsVisible() && pWnd->PtInWindow(x, y))
				pTrung = pWnd;
		}
	}
	return pTrung;
}

//	O nho nhat duoi con tro co dat ten duoc; lui dan len cha neu o do khong co ten.
static KWndWindow* TimODuoiChuot(int x, int y, bool bCaKhoi, char* pszKhoa, int nCo)
{
	KWndWindow*	pGoc = TimCuaSoGoc(x, y);
	KWndWindow*	pChon;

	pszKhoa[0] = 0;
	if (pGoc == NULL)
		return NULL;

	if (bCaKhoi)
		pChon = pGoc;
	else
	{
		pChon = pGoc->TopChildFromPoint(x, y);
		if (pChon == NULL)
			pChon = pGoc;
	}
	while (pChon && TaoKhoaTuOCon(pChon, pszKhoa, nCo) == false)
		pChon = pChon->GetParent();
	if (pChon == NULL || pszKhoa[0] == 0)
		return NULL;
	return pChon;
}

//	Ghi lai trang thai hien tai cua mot o vao bang
static void GhiLaiO(KWndWindow* pWnd, const char* pszKhoa)
{
	int nLeft, nTop;

	if (pWnd == NULL || pszKhoa == NULL || pszKhoa[0] == 0)
		return;
	pWnd->GetPosition(&nLeft, &nTop);
	DatKhoa(pszKhoa, nLeft, nTop, pWnd->UiLayTiLe(),
		pWnd->UiDangAn() ? UITOADO_CO_AN : 0);
}

//--------------------------------------------------------------------------
//	Bang danh sach cua so
//--------------------------------------------------------------------------
static void DungBang()
{
	s_nSoDongBang = 0;
	for (int nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow*	pGoc = Wnd_GetLayerRoot(nLop);

		if (pGoc == NULL)
			continue;
		for (KWndWindow* pWnd = pGoc->GetNextWnd();
			 pWnd && s_nSoDongBang < UITOADO_BANG_MAXDONG; pWnd = pWnd->GetNextWnd())
		{
			s_pDongBang[s_nSoDongBang++] = pWnd;
		}
	}
}

static bool TrongBang(int x, int y)
{
	return (s_bHienBang &&
			x >= UITOADO_BANG_X && x < UITOADO_BANG_X + UITOADO_BANG_RONG &&
			y >= UITOADO_BANG_Y &&
			y < UITOADO_BANG_Y + s_nSoDongBang * UITOADO_BANG_CAO);
}

//--------------------------------------------------------------------------
//	[UITOADO 09/09] Thanh nut cham - hinh hoc, do trung, ve
//--------------------------------------------------------------------------
//	Khoi mau dac. Dung KRUShadow y nhu JxCanDieuKhien.cpp - o day khong co san
//	nguyen thuy ve hinh nao khac ngoai OutputText.
static void OKhoi(int x0, int y0, int x1, int y1, unsigned int uMau)
{
	KRUShadow o;

	if (g_pRepresentShell == NULL)
		return;
	o.oPosition.nX = x0;
	o.oPosition.nY = y0;
	o.oEndPos.nX   = x1;
	o.oEndPos.nY   = y1;
	o.Color.Color_dw = uMau;
	g_pRepresentShell->DrawPrimitives(1, &o, RU_T_SHADOW, true);
}

//	Be rong mot nut: chia deu ca hang theo be ngang man hinh, co chan tren duoi
//	de man hinh rat hep van bam duoc, man hinh rong khong bi nut dai ngoang.
static int NutRong()
{
	int nRong = (SCREEN_WIDTH > 0 ? SCREEN_WIDTH : 1024);

	nRong = (nRong - 2 * UITOADO_NUT_CACH) / UITOADO_SO_NUT - UITOADO_NUT_CACH;
	if (nRong < 56)		nRong = 56;
	if (nRong > 124)	nRong = 124;
	return nRong;
}

static void NutHinh(int i, int* px, int* py, int* pw, int* ph)
{
	int nRong = NutRong();

	*pw = nRong;
	*ph = UITOADO_NUT_CAO;
	*px = UITOADO_NUT_CACH + i * (nRong + UITOADO_NUT_CACH);
	*py = UITOADO_NUT_Y;
}

//	-1 = khong trung nut nao
static int NutTrung(int x, int y)
{
	int i, nX, nY, nW, nH;

	for (i = 0; i < UITOADO_SO_NUT; i++)
	{
		NutHinh(i, &nX, &nY, &nW, &nH);
		if (x >= nX && x < nX + nW && y >= nY && y < nY + nH)
			return i;
	}
	return -1;
}

//	Nut "mo che do sua" - chi co khi config cho phep va dang khong sua.
//	Vi tri lay tu config.ini [Ui] SuaToaDoNutX / SuaToaDoNutY, de chu doi cho
//	neu no che mat thu gi tren man hinh cua minh.
static void NutMoHinh(int* px, int* py, int* pw, int* ph)
{
	if (s_bDaDocViTriNut == false)
	{
		char szDuongDan[MAX_PATH];

		s_bDaDocViTriNut = true;
		g_GetFullPath(szDuongDan, (char*)UITOADO_CAUHINH);
		s_nMoX = GetPrivateProfileInt("Ui", "SuaToaDoNutX", -1, szDuongDan);
		s_nMoY = GetPrivateProfileInt("Ui", "SuaToaDoNutY", -1, szDuongDan);
	}
	*pw = UITOADO_MO_RONG;
	*ph = UITOADO_MO_CAO;
	//	[UITOADO 09/09 D] Mac dinh CU la mep phai GIUA man hinh - dung ngay vung
	//	ngon cai phai, va tu 09/09 cum nut ky nang cung nam do. Do tren may ao da
	//	cham nham that: che do sua bat len giua tran danh. Doi xuong ngay duoi tieu
	//	ban do (goc phai tren), cho do khong co nut nao khac.
	*px = (s_nMoX >= 0) ? s_nMoX
		: (SCREEN_WIDTH > 0 ? SCREEN_WIDTH : 1024) - UITOADO_MO_RONG - 6;
	*py = (s_nMoY >= 0) ? s_nMoY : 172;
}

static bool TrongNutMo(int x, int y)
{
	int nX, nY, nW, nH;

	if (s_bDangSua || UiToaDo_ChoPhep() == false)
		return false;
	NutMoHinh(&nX, &nY, &nW, &nH);
	return (x >= nX && x < nX + nW && y >= nY && y < nY + nH);
}

//--------------------------------------------------------------------------
//	Nhan chuot khi dang sua
//--------------------------------------------------------------------------
//	[UITOADO 09/09 B] Cham vao mot o khi dang chon cong cu to / nho / giau.
//	Lam dung viec ma lan chuot va chuot giua van lam, chi khac duong vao.
static void CongCuChamO(int x, int y)
{
	char		szKhoa[UITOADO_CO_KHOA];
	char		szChu[192];
	KWndWindow*	pO = TimODuoiChuot(x, y, false, szKhoa, sizeof(szKhoa));

	if (pO == NULL)
	{
		DatThongBao("¤ nµy kh«ng ®Æt tªn ®­îc (kh«ng n¹p tõ ini)");
		return;
	}

	if (s_nCongCu == CONGCU_AN)
	{
		if (pO->UiDangAn() == 0 && CamAn(pO))
		{
			DatThongBao("Kh«ng cho giÊu nót ®ãng / huû, giÊu xong sÏ kÑt cöa sæ");
			return;
		}
		pO->UiDatAn(pO->UiDangAn() ? 0 : 1);
		GhiLaiO(pO, szKhoa);
		_snprintf(szChu, sizeof(szChu), "%s  %s", szKhoa,
			pO->UiDangAn() ? "®· giÊu" : "hiÖn l¹i");
		szChu[sizeof(szChu) - 1] = 0;
		DatThongBao(szChu);
		return;
	}

	//	CONGCU_TO / CONGCU_NHO
	if (CamCoGian(pO))
	{
		DatThongBao("¤ nµy kh«ng co gi·n ®­îc (« danh s¸ch / thanh cuén)");
		return;
	}
	pO->UiDatTiLe(pO->UiLayTiLe()
		+ ((s_nCongCu == CONGCU_TO) ? UITOADO_TILE_NAC : -UITOADO_TILE_NAC));
	GhiLaiO(pO, szKhoa);
	_snprintf(szChu, sizeof(szChu), "%s  %d%%", szKhoa, pO->UiLayTiLe() / 10);
	szChu[sizeof(szChu) - 1] = 0;
	DatThongBao(szChu);
}

//	[UITOADO 09/09 B] Bam mot nut tren thanh. Tra ve true = da xu ly.
static void BamNutThanh(int i)
{
	int nMa = s_Nut[i].nMa;
	char szChu[192];

	if (nMa < CONGCU_SO)
	{
		s_nCongCu = nMa;
		_snprintf(szChu, sizeof(szChu), "C«ng cô: %s", s_Nut[i].pszTen);
		szChu[sizeof(szChu) - 1] = 0;
		DatThongBao(szChu);
		return;
	}
	switch (nMa)
	{
	case NUT_BANG:
		s_bHienBang = !s_bHienBang;
		if (s_bHienBang)
			DungBang();
		break;
	case NUT_LUU:
		UiToaDo_BatTat();		// dang sua -> tat = tu dong ghi tep
		break;
	case NUT_XOAHET:
		UiToaDo_XoaHet();
		break;
	}
}

bool UiToaDo_NhanChuot(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int		x, y;
	char	szKhoa[UITOADO_CO_KHOA];
	char	szChu[192];

	//	[UITOADO 09/09 B] Chua sua: chi bat cu bam vao nut "mo che do sua".
	//	Dien thoai khong co ban phim nen khong bam Ctrl+U duoc.
	if (s_bDangSua == false)
	{
		if (uMsg != WM_LBUTTONDOWN)
			return false;
		Wnd_GetCursorPos(&x, &y);
		if (TrongNutMo(x, y) == false)
			return false;
		UiToaDo_BatTat();
		return true;
	}

	//	Dung Wnd_GetCursorPos cho MOI thong diep: voi WM_MOUSEWHEEL thi nParam
	//	la toa do MAN HINH chu khong phai toa do cua so, lay LOWORD se tro nham o.
	Wnd_GetCursorPos(&x, &y);

	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		{
			KWndWindow*	pO = TimODuoiChuot(x, y, false, szKhoa, sizeof(szKhoa));
			int			nDelta;
			int			nTiLe;

			if (pO == NULL)
				break;
			if (CamCoGian(pO))
			{
				DatThongBao("¤ nµy kh«ng co gi·n ®­îc (« danh s¸ch / thanh cuén)");
				break;
			}
			nDelta = (int)(short)HIWORD(uParam);
			nTiLe  = pO->UiLayTiLe() + ((nDelta > 0) ? UITOADO_TILE_NAC : -UITOADO_TILE_NAC);
			pO->UiDatTiLe(nTiLe);
			GhiLaiO(pO, szKhoa);
			_snprintf(szChu, sizeof(szChu), "%s  %d%%", szKhoa, pO->UiLayTiLe() / 10);
			szChu[sizeof(szChu) - 1] = 0;
			DatThongBao(szChu);
		}
		break;

	case WM_MBUTTONDOWN:
		{
			KWndWindow*	pO;

			if (TrongBang(x, y))
				break;
			pO = TimODuoiChuot(x, y, false, szKhoa, sizeof(szKhoa));
			if (pO == NULL)
			{
				//	bam giua tren nen trong = bat / tat bang danh sach
				s_bHienBang = !s_bHienBang;
				if (s_bHienBang)
					DungBang();
				break;
			}
			if (pO->UiDangAn() == 0 && CamAn(pO))
			{
				DatThongBao("Kh«ng cho Èn nót ®ãng / huû, Èn xong sÏ kÑt cöa sæ");
				break;
			}
			pO->UiDatAn(pO->UiDangAn() ? 0 : 1);
			GhiLaiO(pO, szKhoa);
			_snprintf(szChu, sizeof(szChu), "%s  %s", szKhoa,
				pO->UiDangAn() ? "®· Èn" : "hiÖn l¹i");
			szChu[sizeof(szChu) - 1] = 0;
			DatThongBao(szChu);
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			KWndWindow*	pO;
			int			nNut;

			s_pKeo = NULL;
			s_szKhoaKeo[0] = 0;

			//	[UITOADO 09/09 B] thanh nut an tren het
			nNut = (uMsg == WM_LBUTTONDOWN) ? NutTrung(x, y) : -1;
			if (nNut >= 0)
			{
				BamNutThanh(nNut);
				break;
			}

			//	[UITOADO 09/09 B] cong cu to / nho / giau: cham la lam ngay,
			//	khong bat dau keo.
			if (uMsg == WM_LBUTTONDOWN && s_nCongCu >= CONGCU_TO
				&& TrongBang(x, y) == false)
			{
				CongCuChamO(x, y);
				break;
			}

			//	bam vao bang danh sach = ep hien cua so o dong do
			if (uMsg == WM_LBUTTONDOWN && TrongBang(x, y))
			{
				int i = (y - UITOADO_BANG_Y) / UITOADO_BANG_CAO;

				if (i >= 0 && i < s_nSoDongBang && s_pDongBang[i])
					EpHien(s_pDongBang[i]);
				break;
			}

			//	chuot trai = o nho nhat duoi con tro; chuot phai = ca khoi
			//	[UITOADO 09/09 E] O VE TAY xet TRUOC: chung nam tren cung man hinh nen
			//	phai duoc uu tien, khong thi cua so phia duoi cuop mat cu keo.
			s_nKeoORieng = -1;
			{
				int k;
				for (k = 0; k < s_nSoORieng; k++)
				{
					if (s_ORieng[k].pfnTrung(x, y))
					{
						s_nKeoORieng = k;
						s_nKeoX = x;
						s_nKeoY = y;
						break;
					}
				}
			}
			if (s_nKeoORieng >= 0)
				break;

			//	[UITOADO 09/09 B] bam trai + cong cu "Doi khoi" = y nhu bam phai
			pO = TimODuoiChuot(x, y,
				uMsg == WM_RBUTTONDOWN || s_nCongCu == CONGCU_KHOI,
				szKhoa, sizeof(szKhoa));
			if (pO == NULL)
			{
				DatThongBao("¤ nµy kh«ng ®Æt tªn ®­îc (kh«ng n¹p tõ ini)");
				break;
			}
			strncpy(s_szKhoaKeo, szKhoa, sizeof(s_szKhoaKeo) - 1);
			s_szKhoaKeo[sizeof(s_szKhoaKeo) - 1] = 0;
			s_pKeo  = pO;
			s_nKeoX = x;
			s_nKeoY = y;
		}
		break;

	case WM_MOUSEMOVE:
		//	[UITOADO 09/09 E] dang keo mot o ve tay
		if (s_nKeoORieng >= 0)
		{
			int nX = 0, nY = 0;

			s_ORieng[s_nKeoORieng].pfnLay(&nX, &nY);
			s_ORieng[s_nKeoORieng].pfnDat(nX + (x - s_nKeoX), nY + (y - s_nKeoY));
			s_nKeoX = x;
			s_nKeoY = y;
			break;
		}
		if (s_pKeo)
		{
			int nLeft, nTop;

			s_pKeo->GetPosition(&nLeft, &nTop);
			s_pKeo->SetPosition(nLeft + (x - s_nKeoX), nTop + (y - s_nKeoY));
			s_pKeo->UiNhoViTri();
			s_nKeoX = x;
			s_nKeoY = y;
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//	[UITOADO 09/09 E] o ve tay: ghi vi tri moi vao bang de luc luu co trong tep
		if (s_nKeoORieng >= 0)
		{
			int nX = 0, nY = 0;
			char szB[192];

			s_ORieng[s_nKeoORieng].pfnLay(&nX, &nY);
			DatKhoa(s_ORieng[s_nKeoORieng].szKhoa, nX, nY, 1000, 0);
			_snprintf(szB, sizeof(szB), "%s  =  %d,%d",
				s_ORieng[s_nKeoORieng].szKhoa, nX, nY);
			szB[sizeof(szB) - 1] = 0;
			DatThongBao(szB);
			s_nKeoORieng = -1;
			break;
		}
		if (s_pKeo && s_szKhoaKeo[0])
			GhiLaiO(s_pKeo, s_szKhoaKeo);
		s_pKeo = NULL;
		break;
	}
	return true;
}

//--------------------------------------------------------------------------
//	Ve lop phu
//--------------------------------------------------------------------------
static void VeChu(const char* pszChu, int x, int y, unsigned int uMau)
{
	int nDai = strlen(pszChu);

	if (nDai <= 0 || g_pRepresentShell == NULL)
		return;
	g_pRepresentShell->OutputText(12, (char*)pszChu, nDai, x, y, uMau,
		0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);
}

void UiToaDo_Ve()
{
	char szChu[256];

	if (g_pRepresentShell == NULL)
		return;

	if (s_bDangSua)
	{
		int i, nX, nY, nW, nH;

		//	[UITOADO 09/09 B] thanh nut cham - ve truoc, chu huong dan tut xuong duoi
		for (i = 0; i < UITOADO_SO_NUT; i++)
		{
			bool bDangChon = (s_Nut[i].nMa < CONGCU_SO && s_Nut[i].nMa == s_nCongCu);

			NutHinh(i, &nX, &nY, &nW, &nH);
			OKhoi(nX, nY, nX + nW, nY + nH,
				bDangChon ? 0xC02A6E2A : 0xC0202020);
			VeChu(s_Nut[i].pszTen, nX + 5, nY + 10,
				bDangChon ? 0xFFAAFFAA : 0xFFDDDDDD);
		}

		VeChu("Chän c«ng cô ë trªn råi ch¹m vµo « cÇn söa. KÐo = dêi chç.", 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 4, 0xFFFFD24A);
		VeChu("Chuét: kÐo ph¶i = c¶ khèi, l¨n = to nhá, bÊm gi÷a = giÊu / b¶ng. Ctrl+U l­u, Ctrl+K xo¸ hÕt", 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 18, 0xFFFFD24A);

		if (s_pKeo && s_szKhoaKeo[0])
		{
			int nLeft, nTop, nRong, nCao;

			s_pKeo->GetPosition(&nLeft, &nTop);
			s_pKeo->GetSize(&nRong, &nCao);
			_snprintf(szChu, sizeof(szChu), "%s  =  %d,%d  %dx%d  %d%%",
				s_szKhoaKeo, nLeft, nTop, nRong, nCao, s_pKeo->UiLayTiLe() / 10);
			szChu[sizeof(szChu) - 1] = 0;
			VeChu(szChu, 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 32, 0xFF66FF66);
		}

		if (s_bHienBang)
		{
			int i;

			for (i = 0; i < s_nSoDongBang; i++)
			{
				char		szLop[64];
				const char*	pszTrangThai;
				unsigned	uMau;

				if (s_pDongBang[i] == NULL)
					continue;
				LayTenLop(s_pDongBang[i], szLop, sizeof(szLop));
				if (s_pDongBang[i] == s_pEpHien)
				{
					pszTrangThai = "[®ang Ðp hiÖn]";
					uMau = 0xFFFFFF66;
				}
				else if (s_pDongBang[i]->UiDangAn())
				{
					pszTrangThai = "[®· Èn]";
					uMau = 0xFFFF7777;
				}
				else if (s_pDongBang[i]->IsVisible())
				{
					pszTrangThai = "[®ang hiÖn]";
					uMau = 0xFF99FF99;
				}
				else
				{
					pszTrangThai = "[®ang ®ãng]";
					uMau = 0xFFAAAAAA;
				}
				_snprintf(szChu, sizeof(szChu), "%2d. %-34s %s",
					i + 1, szLop, pszTrangThai);
				szChu[sizeof(szChu) - 1] = 0;
				VeChu(szChu, UITOADO_BANG_X,
					UITOADO_BANG_Y + i * UITOADO_BANG_CAO, uMau);
			}
			if (s_nSoDongBang >= UITOADO_BANG_MAXDONG)
			{
				VeChu("... cßn n÷a, b¶ng chØ hiÖn ®­îc bÊy nhiªu dßng", UITOADO_BANG_X,
					UITOADO_BANG_Y + s_nSoDongBang * UITOADO_BANG_CAO, 0xFFFF7777);
			}
		}
	}

	//	[UITOADO 09/09 B] Chua sua ma config da cho phep: hien nut nho de mo.
	//	Dien thoai khong co Ctrl+U. Doi cho bang [Ui] SuaToaDoNutX / SuaToaDoNutY.
	if (s_bDangSua == false && UiToaDo_ChoPhep())
	{
		int nX, nY, nW, nH;

		NutMoHinh(&nX, &nY, &nW, &nH);
		OKhoi(nX, nY, nX + nW, nY + nH, 0xA0202020);
		VeChu("Söa giao diÖn", nX + 6, nY + 8, 0xFFFFD24A);
	}

	if (s_szThongBao[0] && GetTickCount() < s_uHetThongBao)
		VeChu(s_szThongBao, 8, UITOADO_NUT_Y + UITOADO_NUT_CAO + 46, 0xFFFF8080);
	else
		s_szThongBao[0] = 0;
}
