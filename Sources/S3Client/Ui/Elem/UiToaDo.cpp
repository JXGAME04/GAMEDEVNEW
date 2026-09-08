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
		pszTen = p + 1;
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

static bool TaoKhoaTuOCon(KWndWindow* pWnd, char* pszRa, int nCo)
{
	char szLop[64];

	pszRa[0] = 0;
	if (pWnd == NULL)
		return false;
	LayTenLop(pWnd->GetOwner(), szLop, sizeof(szLop));
	return TaoKhoa(szLop, pWnd, pszRa, nCo);
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

void UiToaDo_ApChoTatCa()
{
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
//	Nhan chuot khi dang sua
//--------------------------------------------------------------------------
bool UiToaDo_NhanChuot(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int		x, y;
	char	szKhoa[UITOADO_CO_KHOA];
	char	szChu[192];

	if (s_bDangSua == false)
		return false;

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

			s_pKeo = NULL;
			s_szKhoaKeo[0] = 0;

			//	bam vao bang danh sach = ep hien cua so o dong do
			if (uMsg == WM_LBUTTONDOWN && TrongBang(x, y))
			{
				int i = (y - UITOADO_BANG_Y) / UITOADO_BANG_CAO;

				if (i >= 0 && i < s_nSoDongBang && s_pDongBang[i])
					EpHien(s_pDongBang[i]);
				break;
			}

			//	chuot trai = o nho nhat duoi con tro; chuot phai = ca khoi
			pO = TimODuoiChuot(x, y, uMsg == WM_RBUTTONDOWN, szKhoa, sizeof(szKhoa));
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
		VeChu("Söa giao diÖn: kÐo tr¸i = mét «, kÐo ph¶i = c¶ khèi, l¨n = to nhá, bÊm gi÷a = xo¸ / hiÖn l¹i", 8, 8, 0xFFFFD24A);
		VeChu("BÊm gi÷a lªn chç trèng = b¶ng danh s¸ch cöa sæ. Ctrl+U = l­u vµ tho¸t. Ctrl+K = xo¸ hÕt", 8, 24, 0xFFFFD24A);

		if (s_pKeo && s_szKhoaKeo[0])
		{
			int nLeft, nTop, nRong, nCao;

			s_pKeo->GetPosition(&nLeft, &nTop);
			s_pKeo->GetSize(&nRong, &nCao);
			_snprintf(szChu, sizeof(szChu), "%s  =  %d,%d  %dx%d  %d%%",
				s_szKhoaKeo, nLeft, nTop, nRong, nCao, s_pKeo->UiLayTiLe() / 10);
			szChu[sizeof(szChu) - 1] = 0;
			VeChu(szChu, 8, 40, 0xFF66FF66);
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

	if (s_szThongBao[0] && GetTickCount() < s_uHetThongBao)
		VeChu(s_szThongBao, 8, 56, 0xFFFF8080);
	else
		s_szThongBao[0] = 0;
}
