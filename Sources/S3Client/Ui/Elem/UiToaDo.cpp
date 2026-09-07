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
#define	UITOADO_CO_KHOA		112

struct KOToaDo
{
	char	szKhoa[UITOADO_CO_KHOA];
	int		nLeft;
	int		nTop;
};

static KOToaDo		s_Bang[UITOADO_MAX];
static int			s_nSo			= 0;

static bool			s_bDaDocCauHinh	= false;
static bool			s_bChoPhep		= false;
static bool			s_bDangSua		= false;
static int			s_nStyleCu		= 0;		// WND_SHOW_DEBUG_FRAME_TEXT truoc khi bat

static KWndWindow*	s_pKeo			= NULL;
static char			s_szKhoaKeo[UITOADO_CO_KHOA] = "";
static int			s_nKeoX			= 0;
static int			s_nKeoY			= 0;

static char			s_szThongBao[160] = "";
static unsigned int	s_uHetThongBao	= 0;

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

static void DatKhoa(const char* pszKhoa, int nLeft, int nTop)
{
	int i = TimKhoa(pszKhoa);
	if (i < 0)
	{
		if (s_nSo >= UITOADO_MAX)
			return;
		i = s_nSo++;
		strncpy(s_Bang[i].szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);
		s_Bang[i].szKhoa[UITOADO_CO_KHOA - 1] = 0;
	}
	s_Bang[i].nLeft = nLeft;
	s_Bang[i].nTop  = nTop;
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
void UiToaDo_Nap()
{
	char	szDuongDan[MAX_PATH];
	char	szDong[256];
	FILE*	pTep;

	s_nSo = 0;
	g_GetFullPath(szDuongDan, (char*)UITOADO_TEP);
	pTep = fopen(szDuongDan, "rt");
	if (pTep == NULL)
		return;

	while (fgets(szDong, sizeof(szDong), pTep))
	{
		char*	pBang;
		char*	pPhay;
		char*	p;
		int		nLeft, nTop;

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

		pPhay = strchr(pBang + 1, ',');
		if (pPhay == NULL)
			continue;
		*pPhay = 0;

		nLeft = atoi(pBang + 1);
		nTop  = atoi(pPhay + 1);

		//	cat khoang trang cuoi khoa
		{
			int n = strlen(p);
			while (n > 0 && (p[n - 1] == ' ' || p[n - 1] == '\t'))
				p[--n] = 0;
			if (n == 0)
				continue;
		}
		DatKhoa(p, nLeft, nTop);
	}
	fclose(pTep);
	g_DebugLog("[UITOADO] nap %d vi tri tu %s", s_nSo, szDuongDan);
}

static int GhiTep()
{
	char	szDuongDan[MAX_PATH];
	FILE*	pTep;

	g_GetFullPath(szDuongDan, (char*)UITOADO_TEP);
	pTep = fopen(szDuongDan, "wt");
	if (pTep == NULL)
	{
		g_DebugLog("[UITOADO] KHONG ghi duoc %s", szDuongDan);
		return -1;
	}

	fprintf(pTep, "; [UITOADO] Vi tri o giao dien do nguoi choi tu dat.\n");
	fprintf(pTep, "; Moi dong:  <ten lop cua so>|<ten muc ini> = Left,Top\n");
	fprintf(pTep, "; Xoa het tep nay = tra giao dien ve dung \\Ui\\ui3\\*.ini goc.\n");
	fprintf(pTep, "%s\n", UITOADO_MUC);
	for (int i = 0; i < s_nSo; i++)
	{
		fprintf(pTep, "%s=%d,%d\n", s_Bang[i].szKhoa,
			s_Bang[i].nLeft, s_Bang[i].nTop);
	}
	fclose(pTep);
	g_DebugLog("[UITOADO] ghi %d vi tri vao %s", s_nSo, szDuongDan);
	return s_nSo;
}

void UiToaDo_XoaHet()
{
	char szDuongDan[MAX_PATH];

	s_nSo = 0;
	g_GetFullPath(szDuongDan, (char*)UITOADO_TEP);
	remove(szDuongDan);
	g_DebugLog("[UITOADO] da xoa %s", szDuongDan);
}

//--------------------------------------------------------------------------
//	Ap toa do da luu
//--------------------------------------------------------------------------
static void ApChoCay(const char* pszLop, KWndWindow* pWnd)
{
	char szKhoa[UITOADO_CO_KHOA];

	while (pWnd)
	{
		if (TaoKhoa(pszLop, pWnd, szKhoa, sizeof(szKhoa)))
		{
			int i = TimKhoa(szKhoa);
			if (i >= 0)
				pWnd->SetPosition(s_Bang[i].nLeft, s_Bang[i].nTop);
		}
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
		{
			int i = TimKhoa(szKhoa);
			if (i >= 0)
				pCuaSoGoc->SetPosition(s_Bang[i].nLeft, s_Bang[i].nTop);
		}
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
	s_uHetThongBao = GetTickCount() + 4000;
}

void UiToaDo_BatTat()
{
	char szChu[160];

	if (UiToaDo_ChoPhep() == false)
	{
		DatThongBao("Ch­a më: thªm [Ui] SuaToaDo=1 vµo config.ini");
		return;
	}

	if (s_bDangSua)
	{
		int nSo;

		s_bDangSua = false;
		s_pKeo     = NULL;
		WND_SHOW_DEBUG_FRAME_TEXT = s_nStyleCu;

		nSo = GhiTep();
		if (nSo < 0)
			DatThongBao("Lçi: kh«ng ghi ®­îc UserData\UiToaDo.ini");
		else
		{
			_snprintf(szChu, sizeof(szChu), "§· l­u %d vÞ trÝ vµo UserData\UiToaDo.ini", nSo);
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
	}
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

//--------------------------------------------------------------------------
//	Nhan chuot khi dang sua
//--------------------------------------------------------------------------
bool UiToaDo_NhanChuot(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	x, y;

	if (s_bDangSua == false)
		return false;

	if (uMsg == WM_MOUSEWHEEL)
		return true;			// nuot, khoi lo cuon nham

	x = (short)LOWORD(nParam);
	y = (short)HIWORD(nParam);

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			KWndWindow*	pGoc = TimCuaSoGoc(x, y);
			KWndWindow*	pChon = NULL;

			s_pKeo = NULL;
			s_szKhoaKeo[0] = 0;
			if (pGoc == NULL)
				break;

			//	chuot trai = o nho nhat duoi con tro; chuot phai = ca khoi
			if (uMsg == WM_LBUTTONDOWN)
			{
				pChon = pGoc->TopChildFromPoint(x, y);
				if (pChon == NULL)
					pChon = pGoc;
			}
			else
				pChon = pGoc;

			//	o khong co ten muc ini thi lui dan len cha cho toi khi co
			while (pChon && TaoKhoaTuOCon(pChon, s_szKhoaKeo, sizeof(s_szKhoaKeo)) == false)
				pChon = pChon->GetParent();

			if (pChon == NULL || s_szKhoaKeo[0] == 0)
			{
				DatThongBao("¤ nµy kh«ng ®Æt tªn ®­îc (kh«ng n¹p tõ ini)");
				break;
			}
			s_pKeo  = pChon;
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
			s_nKeoX = x;
			s_nKeoY = y;
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if (s_pKeo && s_szKhoaKeo[0])
		{
			int nLeft, nTop;

			s_pKeo->GetPosition(&nLeft, &nTop);
			DatKhoa(s_szKhoaKeo, nLeft, nTop);
		}
		s_pKeo = NULL;
		break;
	}
	return true;
}

//--------------------------------------------------------------------------
//	Ve dong huong dan
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
	char szChu[224];

	if (g_pRepresentShell == NULL)
		return;

	if (s_bDangSua)
	{
		VeChu("Söa giao diÖn: kÐo chuét tr¸i = mét «, kÐo chuét ph¶i = c¶ khèi, bÊm l¹i phÝm t¾t ®Ó l­u", 8, 8, 0xFFFFD24A);

		if (s_pKeo && s_szKhoaKeo[0])
		{
			int nLeft, nTop;

			s_pKeo->GetPosition(&nLeft, &nTop);
			_snprintf(szChu, sizeof(szChu), "%s  =  %d,%d",
				s_szKhoaKeo, nLeft, nTop);
			szChu[sizeof(szChu) - 1] = 0;
			VeChu(szChu, 8, 24, 0xFF66FF66);
		}
	}

	if (s_szThongBao[0] && GetTickCount() < s_uHetThongBao)
		VeChu(s_szThongBao, 8, 40, 0xFFFF8080);
	else
		s_szThongBao[0] = 0;
}
