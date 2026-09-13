//---------------------------------------------------------------------------
// [WAUTO 12/09] Bang phu cua khung WAuto - xem UiWAutoDsach.h. Chuoi tieng Viet: TCVN3 (sinh bang vn_edit.py trong
// android/va_nguon_android_wauto21.py, khong sua bang trinh soan thao thuong).
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_MOBILE
#include "KIniFile.h"
#include "KDebug.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;
#include "UiWAutoDsach.h"
#include "UiWAutoBang.h"
#include "../../Platform/JxWAutoNoiBo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int JxCore_WAutoViTri(int* pnMapId, char* szMap, int nMapMax, int* pnX, int* pnY);
extern "C" int JxCore_WAutoTenQuanhDay(char* pOut, int nMax);
extern "C" int JxCore_WAutoTenVatPham(char* pOut, int nMax);

#define WA_DS_SCHEME	"UiWAuto_Dsach.ini"
#define WA_DS_GHICHU	"UiWAuto_GhiChu.ini"	// [WAUTO 12/09] sinh boi android/sinh_ghichu_wauto.py tu s_aTips + s_aNote cua WAuto.cpp

// ---------------------------------------------------------------- ve nen (nhu UiWAutoTrang.cpp; de static cho khoi trung ten tren ELF)
static void WAD_Bong(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	KRUShadow S;
	S.oPosition.nX = x0;
	S.oPosition.nY = y0;
	S.oPosition.nZ = 0;
	S.oEndPos.nX = x1;
	S.oEndPos.nY = y1;
	S.oEndPos.nZ = 0;
	S.Color.Color_b.r = (unsigned char)r;
	S.Color.Color_b.g = (unsigned char)g;
	S.Color.Color_b.b = (unsigned char)b;
	S.Color.Color_b.a = (unsigned char)a;
	g_pRepresentShell->DrawPrimitives(1, &S, RU_T_SHADOW, true);
}

static void WAD_Vien(int x0, int y0, int x1, int y1, int r, int g, int b)
{
	KRURect R;
	R.oPosition.nX = x0;
	R.oPosition.nY = y0;
	R.oPosition.nZ = 0;
	R.oEndPos.nX = x1;
	R.oEndPos.nY = y1;
	R.oEndPos.nZ = 0;
	R.Color.Color_b.r = (unsigned char)r;
	R.Color.Color_b.g = (unsigned char)g;
	R.Color.Color_b.b = (unsigned char)b;
	R.Color.Color_b.a = 255;
	g_pRepresentShell->DrawPrimitives(1, &R, RU_T_RECT, true);
}

// ---------------------------------------------------------------- o danh sach
// [WAUTO 13/09] Ve co CHIEU SAU (chu: "tham my hon, co chieu sau hon"):
//   * bong do chech phai-duoi (hai lop mo dan) -> o noi len khoi nen khung
//   * long o LOM: nen toi, canh TREN-TRAI toi hon (bong hat vao), canh DUOI-PHAI sang hon (anh sang tu tren)
//   * vien ngoai hai lop: vien toi om ngoai, vien vang am ben trong
//   * dong chan xen ke mot lop sang rat nhe -> de dem dong, do moi mat
//   * dong dang chon: dai mau vang am ba lop (gia chuyen sac), vien sang, va VACH DANH DAU ben trai
//   * con noi dung o tren / o duoi: ve dai mo dan o mep de biet con phai cuon
//   * THANH CUON that o mep phai (ranh lom + con truot noi), chi hien khi noi dung dai hon o
static void WAD_Vach(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	WAD_Bong(x0, y0, x1, y1, r, g, b, a);
}

KWndDsachWA::KWndDsachWA()
{
	m_nSo = 0;
	m_nChon = -1;
	m_nDau = 0;
	m_szDong[0][0] = 0;
}

int KWndDsachWA::SoDongHien() const
{
	int n = (m_Height - 2 * WA_DS_LE) / WA_DS_DONG;
	return n > 0 ? n : 1;
}

const char* KWndDsachWA::Dong(int i) const
{
	if (i < 0 || i >= m_nSo)
		return "";
	return m_szDong[i];
}

void KWndDsachWA::XoaHet()
{
	m_nSo = 0;
	m_nChon = -1;
	m_nDau = 0;
}

int KWndDsachWA::Them(const char* sz)
{
	if (m_nSo >= WA_DS_TOI_DA)
		return -1;
	strncpy(m_szDong[m_nSo], sz ? sz : "", WA_DS_CHU - 1);
	m_szDong[m_nSo][WA_DS_CHU - 1] = 0;
	return m_nSo++;
}

void KWndDsachWA::DatChon(int n)
{
	m_nChon = (n >= 0 && n < m_nSo) ? n : -1;
	if (m_nChon >= 0)
	{
		int nHien = SoDongHien();
		if (m_nChon < m_nDau)
			m_nDau = m_nChon;
		else if (m_nChon >= m_nDau + nHien)
			m_nDau = m_nChon - nHien + 1;
	}
}

// nBuoc > 0 = xuong. Dung cho ca lan chuot (vuot ngon: KSdlApp doi thanh WM_MOUSEWHEEL) lan trang.
void KWndDsachWA::Cuon(int nDong)
{
	int nHien = SoDongHien();
	m_nDau += nDong;
	if (m_nDau > m_nSo - nHien)
		m_nDau = m_nSo - nHien;
	if (m_nDau < 0)
		m_nDau = 0;
}

// hinh chu nhat dac co vien LOM (bong hat vao tu tren-trai) hoac NOI (anh sang tu tren-trai)
static void WAD_Khoi(int x0, int y0, int x1, int y1, int bNoi)
{
	if (bNoi)
	{
		WAD_Vach(x0, y0, x1, y0 + 1, 225, 205, 150, 10);			// mep tren sang
		WAD_Vach(x0, y0, x0 + 1, y1, 205, 185, 135, 12);			// mep trai sang
		WAD_Vach(x0, y1 - 1, x1, y1, 40, 28, 12, 6);				// mep duoi toi
		WAD_Vach(x1 - 1, y0, x1, y1, 55, 40, 18, 6);				// mep phai toi
	}
	else
	{
		WAD_Vach(x0, y0, x1, y0 + 2, 0, 0, 0, 8);					// bong hat vao tu tren
		WAD_Vach(x0, y0, x0 + 1, y1, 0, 0, 0, 12);					// va tu trai
		WAD_Vach(x0, y1 - 1, x1, y1, 130, 112, 76, 22);				// day hoi sang
		WAD_Vach(x1 - 1, y0, x1, y1, 120, 104, 72, 24);				// phai hoi sang
	}
}

void KWndDsachWA::PaintWindow()
{
	if (!g_pRepresentShell || !(m_Style & WND_S_VISIBLE))
		return;
	int x0 = m_nAbsoluteLeft, y0 = m_nAbsoluteTop, x1 = x0 + m_Width, y1 = y0 + m_Height;
	int nHien = SoDongHien();
	int bCuon = (m_nSo > nHien);
	int xNoiDung = x1 - WA_DS_LE - (bCuon ? WA_DS_THANH : 0);	// mep phai cua phan chu

	// 1. bong do chech phai - duoi
	WAD_Bong(x0 + 4, y0 + 4, x1 + 4, y1 + 4, 0, 0, 0, 20);
	WAD_Bong(x0 + 2, y0 + 2, x1 + 2, y1 + 2, 0, 0, 0, 14);
	// 2. long o
	WAD_Bong(x0, y0, x1, y1, 30, 26, 20, 2);		// [WAUTO 13/09] nen nau rat toi (truoc la 14,13,11 gan den:
												// bong den hat vao cua vien lom trung mau nen nen khong thay o lom xuong)
	WAD_Khoi(x0, y0, x1, y1, 0);
	// 3. hai lop vien
	WAD_Vien(x0 - 1, y0 - 1, x1, y1, 18, 14, 8);
	WAD_Vien(x0, y0, x1 - 1, y1 - 1, 150, 122, 72);

	// 4. cac dong
	for (int i = 0; i < nHien; i++)
	{
		int k = m_nDau + i;
		if (k >= m_nSo)
			break;
		int yd = y0 + WA_DS_LE + i * WA_DS_DONG;
		int yh = yd + WA_DS_DONG;
		if (k == m_nChon)
		{	// dai vang am ba lop: dam o giua, nhat dan ra hai mep -> nhin nhu co do cong
			WAD_Bong(x0 + 2, yd, xNoiDung, yh - 1, 108, 78, 24, 10);
			WAD_Bong(x0 + 2, yd + 2, xNoiDung, yh - 3, 146, 106, 30, 8);
			WAD_Bong(x0 + 2, yd + WA_DS_DONG / 2 - 2, xNoiDung, yd + WA_DS_DONG / 2 + 2, 178, 132, 40, 6);
			WAD_Vien(x0 + 2, yd, xNoiDung - 1, yh - 2, 214, 176, 96);
			WAD_Vach(x0 + 2, yd, x0 + 5, yh - 1, 255, 214, 120, 2);		// vach danh dau ben trai
		}
		else if (k & 1)
			WAD_Bong(x0 + 2, yd, xNoiDung, yh - 1, 120, 124, 132, 28);	// dong le: sang rat nhe
		if (i && k != m_nChon && (k - 1) != m_nChon)
			WAD_Vach(x0 + 6, yd - 1, xNoiDung - 4, yd, 0, 0, 0, 20);	// vach chan dong

		int n = (int)strlen(m_szDong[k]);
		int nMax = (xNoiDung - (x0 + WA_DS_CHU_L) - 4) / 6;
		if (nMax < 1)
			nMax = 1;
		if (n > nMax)
			n = nMax;
		g_pRepresentShell->OutputText(12, m_szDong[k], n, x0 + WA_DS_CHU_L, yd + (WA_DS_DONG - 12) / 2,
			(k == m_nChon) ? 0x00FFF2C8 : 0x00DCD8D0, 0, TEXT_IN_SINGLE_PLANE_COORD, 0x00000000);
	}

	if (!bCuon)
		return;

	// 5. dai mo dan o mep tren / duoi: bao con noi dung phia do
	if (m_nDau > 0)
		for (int q = 0; q < 5; q++)
			WAD_Vach(x0 + 2, y0 + 1 + q, xNoiDung, y0 + 2 + q, 0, 0, 0, 10 + q * 4);
	if (m_nDau + nHien < m_nSo)
		for (int q = 0; q < 5; q++)
			WAD_Vach(x0 + 2, y1 - 2 - q, xNoiDung, y1 - 1 - q, 0, 0, 0, 10 + q * 4);

	// 6. thanh cuon: ranh LOM + con truot NOI, cao theo ti le phan dang thay
	{
		int xt0 = x1 - WA_DS_LE - WA_DS_THANH + 2, xt1 = x1 - WA_DS_LE;
		int yt0 = y0 + WA_DS_LE, yt1 = y1 - WA_DS_LE;
		int nCaoRanh = yt1 - yt0;
		int nCao = nCaoRanh * nHien / m_nSo;
		int nToiDa = m_nSo - nHien;
		int yc0, yc1;
		if (nCao < WA_DS_THANH * 2)
			nCao = WA_DS_THANH * 2;
		if (nCao > nCaoRanh)
			nCao = nCaoRanh;
		yc0 = yt0 + (nToiDa > 0 ? (nCaoRanh - nCao) * m_nDau / nToiDa : 0);
		yc1 = yc0 + nCao;
		WAD_Bong(xt0, yt0, xt1, yt1, 8, 7, 6, 6);			// ranh
		WAD_Khoi(xt0, yt0, xt1, yt1, 0);
		WAD_Bong(xt0 + 1, yc0, xt1 - 1, yc1, 150, 118, 62, 6);	// con truot
		WAD_Bong(xt0 + 1, yc0 + 1, xt1 - 1, yc0 + nCao / 2, 186, 150, 84, 8);
		WAD_Khoi(xt0 + 1, yc0, xt1 - 1, yc1, 1);
	}
}

int KWndDsachWA::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{	// [WAUTO 13/09] VUOT DOC de cuon: KSdlApp doi vuot doc tren giao dien thanh WM_MOUSEWHEEL
		// (moi 28 px mot nac, [ANDROID 09/09 CUON]) - dung duong ma khung chat / thoai NPC dang dung.
		int zDelta = (int)(short)HIWORD(uParam);
		if (zDelta)
			Cuon(-zDelta / WHEEL_DELTA);
		return 1;
	}
	if (uMsg == WM_LBUTTONDOWN)
	{
		int y = (int)(short)HIWORD(nParam);
		int i = (y - m_nAbsoluteTop - WA_DS_LE) / WA_DS_DONG;
		int k = m_nDau + i;
		if (i >= 0 && i < SoDongHien() && k < m_nSo)
		{
			m_nChon = k;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (KUPARAM)(KWndWindow*)this, k);
		}
		return 1;
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}
// ---------------------------------------------------------------- cau hinh tung danh sach
// bNguon: 0 khong co / 1 bang thuoc tinh / 2 ten vat pham trong hanh trang / 3 ten nguoi choi quanh day
struct WADsCau
{
	int			nViec;
	const char*	szTieuDe;
	int			nToiDa;
	int			bNguon;
	int			bSo;		// co o nhap so di kem (loc thuoc tinh)
	int			bThuTu;		// co nut Len / Xuong
	int			bLay;		// co nut "Lay o day" (toa do dang dung)
	int			bXoa;		// co nut Xoa / Xoa het
	const char*	szHuong;
};

static const WADsCau s_aDsCau[] = {
	{ WA_V_DS_TOADO,     "Täa ®é ®i tuÇn",           24, 0, 0, 1, 1, 1,
	  "Auto ch¹y lÇn l­ît tõ trªn xuèng råi lÆp l¹i. §øng ®óng chç muèn ®i qua råi bÊm LÊy ë ®©y." },
	{ WA_V_DS_LOC,       "Läc thuéc tÝnh khi nhÆt",  40, 1, 1, 0, 0, 1,
	  "ChØ nhÆt trang bÞ cã dßng thuéc tÝnh nµy vµ ®¹t gi¸ trÞ tèi thiÓu. Chän dßng, gâ gi¸ trÞ råi bÊm Thªm." },
	{ WA_V_DS_KHONGNHAT, "Kh«ng nhÆt theo tªn",      60, 2, 0, 0, 0, 1,
	  "Danh s¸ch lÊy tõ c¸c mãn ®ang cã trong hµnh trang. Tªn trong danh s¸ch th× auto bá qua kh«ng nhÆt." },
	{ WA_V_DS_TODOI,     "Mêi / vµo nhãm theo tªn",  24, 3, 0, 1, 0, 1,
	  "ChØ mêi vµ chØ nhËn lêi mêi cña c¸c tªn trong danh s¸ch. BÊm Chän tªn ®Ó lÊy ng­êi ®ang ®øng quanh." },
	{ WA_V_DS_LIENDAU,   "B¹n diÔn Liªn ®Êu",         8, 3, 0, 0, 0, 1,
	  "Tèi ®a 8 tªn, auto chØ tæ ®éi víi ng­êi gÆp ®Çu tiªn. C¶ hai m¸y ®Òu ph¶i cã tªn cña nhau." },
	{ WA_V_DS_NGUHANH,   "Thø tù ­u tiªn ngò hµnh",   5, 0, 0, 1, 0, 0,
	  "M¸y PK chän môc tiªu theo thø tù hÖ nµy, tõ trªn xuèng. ChØ ®æi ®­îc thø tù, kh«ng thªm bít." },
};
#define WA_DS_SO_CAU	(sizeof(s_aDsCau) / sizeof(s_aDsCau[0]))

static const WADsCau* WA_DsCau(int nViec)
{
	for (int i = 0; i < (int)WA_DS_SO_CAU; i++)
		if (s_aDsCau[i].nViec == nViec)
			return &s_aDsCau[i];
	return NULL;
}

static const char* const s_aNguHanh[5] = { "Kim", "Méc", "Thñy", "Háa", "Thæ" };

static const char* WA_TenMagic(int nId)
{
	for (int i = 0; i < WA_SO_MAGIC; i++)
		if (s_WAMagic[i].nId == nId)
			return s_WAMagic[i].szTen;
	return "?";
}

// ---------------------------------------------------------------- doc / sua du lieu tung danh sach
static int WA_DsSo(int nViec, autoData* p)
{
	if (!p)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:		return p->nCoordCount;
	case WA_V_DS_LOC:		return p->nFtMaCount;
	case WA_V_DS_KHONGNHAT:	return p->nNOPCount;
	case WA_V_DS_TODOI:		return p->nIJPtCount;
	case WA_V_DS_LIENDAU:	return p->nLDPtCount;
	case WA_V_DS_NGUHANH:	return 5;
	}
	return 0;
}

static void WA_DsDong(int nViec, autoData* p, int i, char* sz, int nMax)
{
	sz[0] = 0;
	if (!p)
		return;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
		snprintf(sz, nMax, "%d. %d / %d", i + 1, p->sMoveCoord[i].x, p->sMoveCoord[i].y);
		break;
	case WA_V_DS_LOC:
		snprintf(sz, nMax, "%s >= %d", WA_TenMagic(p->nFtMagic[i][0]), p->nFtMagic[i][1]);
		break;
	case WA_V_DS_KHONGNHAT:
		snprintf(sz, nMax, "%s", p->szNOPName[i]);
		break;
	case WA_V_DS_TODOI:
		snprintf(sz, nMax, "%s", p->szIJPtName[i]);
		break;
	case WA_V_DS_LIENDAU:
		snprintf(sz, nMax, "%s", p->szLDPtName[i]);
		break;
	case WA_V_DS_NGUHANH:
	{
		int h = p->nSerPy[i];
		snprintf(sz, nMax, "%d. %s", i + 1, (h >= 0 && h < 5) ? s_aNguHanh[h] : "?");
		break;
	}
	}
}

// tra 1 neu da doi du lieu
static int WA_DsThem(int nViec, autoData* p, int nNguon, const char* szTen, int nSo)
{
	int i;
	if (!p)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
	{
		int nMapId = 0, x = 0, y = 0;
		char szMap[32] = { 0 };
		if (p->nCoordCount >= 24 || !JxCore_WAutoViTri(&nMapId, szMap, sizeof(szMap), &x, &y))
			return 0;
		p->sMoveCoord[p->nCoordCount].x = x;
		p->sMoveCoord[p->nCoordCount].y = y;
		p->nCoordCount++;
		p->nMoveMapId = nMapId;		// nhu nut "Lay" cua ban PC: toa do di kem ban do dang dung
		strncpy(p->szMoveMap, szMap, sizeof(p->szMoveMap) - 1);
		p->szMoveMap[sizeof(p->szMoveMap) - 1] = 0;
		return 1;
	}
	case WA_V_DS_LOC:
	{
		if (nNguon < 0 || nNguon >= WA_SO_MAGIC)
			return 0;
		int nId = s_WAMagic[nNguon].nId;
		for (i = 0; i < p->nFtMaCount; i++)
			if (p->nFtMagic[i][0] == nId)
			{
				p->nFtMagic[i][1] = nSo;		// da co dong nay -> chi doi gia tri (giong CBN_SELCHANGE ben PC)
				return 1;
			}
		if (p->nFtMaCount >= 40)
			return 0;
		p->nFtMagic[p->nFtMaCount][0] = nId;
		p->nFtMagic[p->nFtMaCount][1] = nSo;
		p->nFtMaCount++;
		return 1;
	}
	case WA_V_DS_KHONGNHAT:
		if (!szTen || !szTen[0] || p->nNOPCount >= 60)
			return 0;
		for (i = 0; i < p->nNOPCount; i++)
			if (!strcmp(p->szNOPName[i], szTen))
				return 0;
		strncpy(p->szNOPName[p->nNOPCount], szTen, 79);
		p->szNOPName[p->nNOPCount][79] = 0;
		p->nNOPCount++;
		return 1;
	case WA_V_DS_TODOI:
		if (!szTen || !szTen[0] || p->nIJPtCount >= 24)
			return 0;
		for (i = 0; i < p->nIJPtCount; i++)
			if (!strcmp(p->szIJPtName[i], szTen))
				return 0;
		strncpy(p->szIJPtName[p->nIJPtCount], szTen, 31);
		p->szIJPtName[p->nIJPtCount][31] = 0;
		p->nIJPtCount++;
		return 1;
	case WA_V_DS_LIENDAU:
		if (!szTen || !szTen[0] || p->nLDPtCount >= 8)
			return 0;
		for (i = 0; i < p->nLDPtCount; i++)
			if (!strcmp(p->szLDPtName[i], szTen))
				return 0;
		strncpy(p->szLDPtName[p->nLDPtCount], szTen, 31);
		p->szLDPtName[p->nLDPtCount][31] = 0;
		p->nLDPtCount++;
		return 1;
	}
	return 0;
}

static int WA_DsXoa(int nViec, autoData* p, int k)
{
	int i;
	if (!p || k < 0 || k >= WA_DsSo(nViec, p))
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
		for (i = k; i < p->nCoordCount - 1; i++)
			p->sMoveCoord[i] = p->sMoveCoord[i + 1];
		p->nCoordCount--;
		return 1;
	case WA_V_DS_LOC:
		for (i = k; i < p->nFtMaCount - 1; i++)
		{
			p->nFtMagic[i][0] = p->nFtMagic[i + 1][0];
			p->nFtMagic[i][1] = p->nFtMagic[i + 1][1];
		}
		p->nFtMaCount--;
		return 1;
	case WA_V_DS_KHONGNHAT:
		for (i = k; i < p->nNOPCount - 1; i++)
			memcpy(p->szNOPName[i], p->szNOPName[i + 1], 80);
		p->nNOPCount--;
		return 1;
	case WA_V_DS_TODOI:
		for (i = k; i < p->nIJPtCount - 1; i++)
			memcpy(p->szIJPtName[i], p->szIJPtName[i + 1], 32);
		p->nIJPtCount--;
		return 1;
	case WA_V_DS_LIENDAU:
		for (i = k; i < p->nLDPtCount - 1; i++)
			memcpy(p->szLDPtName[i], p->szLDPtName[i + 1], 32);
		p->nLDPtCount--;
		return 1;
	}
	return 0;
}

static int WA_DsXoaHet(int nViec, autoData* p)
{
	if (!p)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:		p->nCoordCount = 0; return 1;
	case WA_V_DS_LOC:		p->nFtMaCount = 0;  return 1;
	case WA_V_DS_KHONGNHAT:	p->nNOPCount = 0;   return 1;
	case WA_V_DS_TODOI:		p->nIJPtCount = 0;  return 1;
	case WA_V_DS_LIENDAU:	p->nLDPtCount = 0;  return 1;
	}
	return 0;
}

static int WA_DsHoanDoi(int nViec, autoData* p, int a, int b)
{
	int n = WA_DsSo(nViec, p);
	if (!p || a < 0 || b < 0 || a >= n || b >= n)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
	{
		autoCoord t = p->sMoveCoord[a];
		p->sMoveCoord[a] = p->sMoveCoord[b];
		p->sMoveCoord[b] = t;
		return 1;
	}
	case WA_V_DS_TODOI:
	{
		char t[32];
		memcpy(t, p->szIJPtName[a], 32);
		memcpy(p->szIJPtName[a], p->szIJPtName[b], 32);
		memcpy(p->szIJPtName[b], t, 32);
		return 1;
	}
	case WA_V_DS_NGUHANH:
	{
		short t = p->nSerPy[a];
		p->nSerPy[a] = p->nSerPy[b];
		p->nSerPy[b] = t;
		return 1;
	}
	}
	return 0;
}

// ---------------------------------------------------------------- bang phu
KUiWAutoDsach* KUiWAutoDsach::s_pSelf = NULL;

KUiWAutoDsach::KUiWAutoDsach()
{
	m_pTrang = NULL;
	m_nCheDo = 0;
	m_nViec = 0;
	m_nNguonChon = -1;
	m_szNguon[0] = 0;
	m_pGoi = NULL;
	m_nKhe = 0;
	m_pDong = NULL;
	m_nDong = 0;
}

void KUiWAutoDsach::KhoiTao(KWndWindow* pTrang)
{
	int i;
	m_pTrang = pTrang;
	AddChild(&m_TieuDe);
	AddChild(&m_Huong);
	AddChild(&m_NhanSo);
	AddChild(&m_Dsach);
	AddChild(&m_Nguon);
	AddChild(&m_So);
	for (i = 0; i < WA_DS_NUT; i++)
		AddChild(&m_Nut[i]);
	AddChild(&m_Dong);
	s_pSelf = this;
	Hide();
}

void KUiWAutoDsach::NapBoCuc()
{
	char Scheme[128];
	char Buff[256];
	char szMuc[32];
	KIniFile Ini;
	int i;
	g_UiBase.GetCurSchemePath(Scheme, 128);
	sprintf(Buff, "%s\\%s", Scheme, WA_DS_SCHEME);
	if (!Ini.Load(Buff))
	{
		g_DebugLog("[WAUTO-DS] khong doc duoc bo cuc %s", Buff);
		return;
	}
	Init(&Ini, "Bang");
	m_TieuDe.SetText("");
	m_TieuDe.Init(&Ini, "TieuDe");
	m_Huong.SetText("");
	m_Huong.Init(&Ini, "Huong");
	m_NhanSo.SetText("");
	m_NhanSo.Init(&Ini, "NhanSo");
	m_Dsach.Init(&Ini, "Dsach");
	m_Nguon.Init(&Ini, "Nguon");
	m_So.Init(&Ini, "So");
	for (i = 0; i < WA_DS_NUT; i++)
	{
		sprintf(szMuc, "Nut%d", i);
		m_Nut[i].Init(&Ini, szMuc);
	}
	m_Dong.Init(&Ini, "Dong");
	Hide();
}

autoData* KUiWAutoDsach::CauHinh()
{
	autoData* p = JxWAuto_CauHinh();
	if (!p || sizeof(autoData) != WA_SIZEOF_AUTODATA)
		return NULL;
	return p;
}

// [WAUTO 12/09] Dong tom tat cho trang: so muc + vai muc dau. Khong can bang phu dang mo.
void KUiWAutoDsach::TomTat(int nViec, char* sz, int nMax)
{
	char szD[WA_DS_CHU];
	autoData* p = JxWAuto_CauHinh();
	int n, i, nDai;
	if (!sz || nMax <= 0)
		return;
	sz[0] = 0;
	if (!p || sizeof(autoData) != WA_SIZEOF_AUTODATA)
		return;
	n = WA_DsSo(nViec, p);
	if (n <= 0)
	{
		snprintf(sz, nMax, "(ch­a cã môc nµo)");
		return;
	}
	snprintf(sz, nMax, "%d môc: ", n);
	for (i = 0; i < n; i++)
	{
		nDai = (int)strlen(sz);
		if (nDai + 8 >= nMax)
			break;
		WA_DsDong(nViec, p, i, szD, sizeof(szD));
		if (i)
			strncat(sz, ", ", nMax - nDai - 1);
		strncat(sz, szD, nMax - (int)strlen(sz) - 1);
	}
}

int KUiWAutoDsach::DangMo()
{
	return (s_pSelf && s_pSelf->IsVisible()) ? 1 : 0;
}

void KUiWAutoDsach::DongLai()
{
	if (s_pSelf)
		s_pSelf->Dong();
}

// [WAUTO 12/09] Cat mot doan chu dai thanh nhieu dong vua be ngang o danh sach (cat o dau cach, khong cat giua tu).
void KUiWAutoDsach::XuongDong(const char* szChu, const char* szDau)
{
	char sz[WA_DS_CHU];
	int nRong, nMax, nDauDai, i, k, nCat;
	if (!szChu || !szChu[0])
		return;
	m_Dsach.GetSize(&nRong, &i);
	nMax = (nRong - 12) / 6;				// font 12: 6 px moi byte TCVN3
	if (nMax > WA_DS_CHU - 1)
		nMax = WA_DS_CHU - 1;
	if (nMax < 8)
		nMax = 8;
	nDauDai = szDau ? (int)strlen(szDau) : 0;
	i = 0;
	while (szChu[i])
	{
		int nCho = nMax - (i == 0 ? nDauDai : 2);	// dong sau lui vao 2 ky tu
		if (nCho < 8)
			nCho = 8;
		nCat = 0;
		for (k = 0; k < nCho && szChu[i + k]; k++)
			if (szChu[i + k] == ' ')
				nCat = k;
		if (!szChu[i + k])
			nCat = k;						// het chu
		else if (nCat <= 0)
			nCat = nCho;					// mot tu dai hon ca dong: cat cung
		sz[0] = 0;
		if (i == 0 && nDauDai)
			strncpy(sz, szDau, WA_DS_CHU - 1);
		else if (i)
			strcpy(sz, "  ");
		k = (int)strlen(sz);
		if (nCat > WA_DS_CHU - 1 - k)
			nCat = WA_DS_CHU - 1 - k;
		memcpy(sz + k, szChu + i, nCat);
		sz[k + nCat] = 0;
		if (m_Dsach.Them(sz) < 0)
			return;
		i += nCat;
		while (szChu[i] == ' ')
			i++;
	}
}

// [WAUTO 12/09] Trang huong dan cua mot the: trang note cua the (dau || la xuong doan) roi ghi chu tung o.
void KUiWAutoDsach::NapGhiChu(int nTab)
{
	char Scheme[128];
	char Buff[256];
	char szKhoa[8];
	static char s_szChu[4096];
	static char s_szNhan[128];
	KIniFile Ini;
	int i;
	m_Dsach.XoaHet();
	g_UiBase.GetCurSchemePath(Scheme, 128);
	sprintf(Buff, "%s\\%s", Scheme, WA_DS_GHICHU);
	if (!Ini.Load(Buff))
	{
		m_Dsach.Them("Ch­a cã tÖp ghi chó (uiwauto_ghichu.ini).");
		return;
	}
	snprintf(szKhoa, sizeof(szKhoa), "%d", nTab);
	s_szChu[0] = 0;
	if (Ini.GetString("The", szKhoa, "", s_szChu, sizeof(s_szChu)) && s_szChu[0])
	{	// dau || = xuong doan
		char* p = s_szChu;
		while (p && *p)
		{
			char* q = strstr(p, "||");
			if (q)
				*q = 0;
			XuongDong(p, "");
			if (!q)
				break;
			p = q + 2;
		}
		m_Dsach.Them("");
	}
	if (nTab < 0 || nTab >= WA_TR_SO_TAB)
		return;
	m_Dsach.Them("--- Tõng « mét ---");
	{
		const WAUiTab* pT = &s_WAUiTab[nTab];
		for (i = 0; i < pT->nMuc; i++)
		{
			const WAUiMuc& m = pT->pMuc[i];
			if (!m.szIdc)
				continue;
			s_szChu[0] = 0;
			if (!Ini.GetString("O", m.szIdc, "", s_szChu, sizeof(s_szChu)) || !s_szChu[0])
				continue;
			s_szNhan[0] = 0;
			Ini.GetString("Nhan", m.szIdc, "", s_szNhan, sizeof(s_szNhan));
			if (s_szNhan[0])
			{
				char szDau[144];
				snprintf(szDau, sizeof(szDau), "%s: ", s_szNhan);
				XuongDong(s_szChu, szDau);
			}
			else
				XuongDong(s_szChu, "- ");
		}
	}
}

int KUiWAutoDsach::MoGhiChu(int nTab, const char* szTenThe)
{
	char sz[96];
	if (!s_pSelf)
		return 0;
	snprintf(sz, sizeof(sz), "H­íng dÉn: %s", szTenThe ? szTenThe : "");
	s_pSelf->m_TieuDe.SetText(sz);
	s_pSelf->NapGhiChu(nTab);
	s_pSelf->Mo(3);
	return 1;
}

int KUiWAutoDsach::MoSua(int nViec)
{
	if (!s_pSelf || !WA_DsCau(nViec))
		return 0;
	s_pSelf->m_nViec = nViec;
	s_pSelf->m_nNguonChon = -1;
	s_pSelf->m_szNguon[0] = 0;
	s_pSelf->Mo(0);
	return 1;
}

int KUiWAutoDsach::MoChonMuc(KWndWindow* pGoi, int nKhe, const char* szTieuDe, const char* const* pDong, int nDong, int nChon)
{
	if (!s_pSelf || !pDong || nDong <= 0)
		return 0;
	s_pSelf->m_pGoi = pGoi;
	s_pSelf->m_nKhe = nKhe;
	s_pSelf->m_pDong = pDong;
	s_pSelf->m_nDong = nDong;
	s_pSelf->m_TieuDe.SetText(szTieuDe ? szTieuDe : "");
	s_pSelf->m_Dsach.XoaHet();
	for (int i = 0; i < nDong && i < WA_DS_TOI_DA; i++)
		s_pSelf->m_Dsach.Them(pDong[i]);
	s_pSelf->m_Dsach.DatChon(nChon);
	s_pSelf->Mo(2);
	return 1;
}

void KUiWAutoDsach::Mo(int nCheDo)
{
	m_nCheDo = nCheDo;
	if (nCheDo == 0)
	{
		const WADsCau* c = WA_DsCau(m_nViec);
		m_TieuDe.SetText(c ? c->szTieuDe : "");
		m_Huong.SetText(c ? c->szHuong : "");
		NapDanhSach();
	}
	else if (nCheDo == 1)
	{
		NapNguon();
	}
	else if (nCheDo == 2)
	{
		m_Huong.SetText("Ch¹m mét dßng ®Ó chän.");
	}
	else
	{	// [WAUTO 12/09] che do 3: chi doc, cuon bang hai nut ben phai
		m_Huong.SetText("Vuèt lªn xuèng trong « ®Ó cuén. BÊm §ãng ®Ó quay l¹i.");
	}
	CapNhatNut();
	if (m_pTrang)
		m_pTrang->Hide();		// trang khong nhan cham nua (PtInWindow doi WND_S_VISIBLE)
	Show();
}

void KUiWAutoDsach::Dong()
{
	if (m_nCheDo == 1)
	{	// dang chon nguon -> ve lai che do sua danh sach
		Mo(0);
		return;
	}
	Hide();
	m_pGoi = NULL;
	m_pDong = NULL;
	m_nDong = 0;
	if (m_pTrang)
		m_pTrang->Show();
}

void KUiWAutoDsach::NapDanhSach()
{
	char sz[WA_DS_CHU];
	autoData* p = CauHinh();
	int n = WA_DsSo(m_nViec, p);
	int nCu = m_Dsach.Chon();
	m_Dsach.XoaHet();
	for (int i = 0; i < n && i < WA_DS_TOI_DA; i++)
	{
		WA_DsDong(m_nViec, p, i, sz, sizeof(sz));
		m_Dsach.Them(sz);
	}
	if (nCu >= 0)
		m_Dsach.DatChon(nCu < n ? nCu : n - 1);
}

void KUiWAutoDsach::NapNguon()
{
	const WADsCau* c = WA_DsCau(m_nViec);
	int i, n;
	m_Dsach.XoaHet();
	if (!c)
		return;
	if (c->bNguon == 1)
	{
		m_TieuDe.SetText("Chän dßng thuéc tÝnh");
		for (i = 0; i < WA_SO_MAGIC; i++)
			m_Dsach.Them(s_WAMagic[i].szTen);
	}
	else if (c->bNguon == 2)
	{
		static char s_szTen[60][80];
		m_TieuDe.SetText("Chän mãn trong hµnh trang");
		n = JxCore_WAutoTenVatPham((char*)s_szTen, 60);
		for (i = 0; i < n; i++)
			m_Dsach.Them(s_szTen[i]);
		if (n <= 0)
			m_Huong.SetText("Hµnh trang ®ang trèng - bá vµo tói mét mãn råi më l¹i.");
	}
	else if (c->bNguon == 3)
	{
		static char s_szTen2[100][32];
		m_TieuDe.SetText("Chän tªn ng­êi ch¬i quanh ®©y");
		n = JxCore_WAutoTenQuanhDay((char*)s_szTen2, 100);
		for (i = 0; i < n; i++)
			m_Dsach.Them(s_szTen2[i]);
		if (n <= 0)
			m_Huong.SetText("Quanh ®©y ch­a thÊy ng­êi ch¬i nµo.");
	}
}

void KUiWAutoDsach::CapNhatNut()
{
	const WADsCau* c = WA_DsCau(m_nViec);
	int bSua = (m_nCheDo == 0 && c != NULL);
	int i;
	for (i = 0; i < WA_DS_NUT; i++)
		m_Nut[i].Hide();
	m_Nguon.Hide();
	m_So.Hide();
	m_NhanSo.Hide();
	if (!bSua)
		return;
	if (c->bNguon)
	{
		m_Nguon.SetLabel(m_szNguon[0] ? m_szNguon : "(chän)");
		m_Nguon.Show();
		m_Nut[0].Show();			// Them
	}
	if (c->bSo)
	{
		m_So.Show();
		m_NhanSo.Show();
	}
	if (c->bThuTu)
	{
		m_Nut[1].Show();			// Len
		m_Nut[2].Show();			// Xuong
	}
	if (c->bXoa)
	{
		m_Nut[3].Show();			// Xoa
		m_Nut[4].Show();			// Xoa het
	}
	if (c->bLay)
		m_Nut[5].Show();			// Lay o day
}

void KUiWAutoDsach::ChonNguon(int nDong)
{
	const char* s = m_Dsach.Dong(nDong);
	m_nNguonChon = nDong;
	strncpy(m_szNguon, s ? s : "", WA_DS_CHU - 1);
	m_szNguon[WA_DS_CHU - 1] = 0;
	Mo(0);							// ve che do sua danh sach
}

void KUiWAutoDsach::Them()
{
	const WADsCau* c = WA_DsCau(m_nViec);
	autoData* p = CauHinh();
	if (!c || !p)
		return;
	if (c->bNguon && m_nNguonChon < 0)
	{
		m_Huong.SetText("Chän mét dßng ë « bªn d­íi tr­íc ®·.");
		return;
	}
	if (WA_DsThem(m_nViec, p, m_nNguonChon, m_szNguon, c->bSo ? m_So.GetIntNumber() : 0))
	{
		JxWAuto_LuuCauHinh();
		NapDanhSach();
	}
	else
		m_Huong.SetText("Kh«ng thªm ®­îc: ®· ®ñ sè dßng tèi ®a hoÆc tªn ®· cã trong danh s¸ch.");
}

void KUiWAutoDsach::Doi(int nBuoc)
{
	autoData* p = CauHinh();
	int k = m_Dsach.Chon();
	if (!p || k < 0)
		return;
	if (WA_DsHoanDoi(m_nViec, p, k, k + nBuoc))
	{
		JxWAuto_LuuCauHinh();
		NapDanhSach();
		m_Dsach.DatChon(k + nBuoc);
	}
}

void KUiWAutoDsach::PaintWindow()
{
	if (!g_pRepresentShell || !(m_Style & WND_S_VISIBLE))
		return;
	int x0 = m_nAbsoluteLeft, y0 = m_nAbsoluteTop, x1 = x0 + m_Width, y1 = y0 + m_Height;
	WAD_Bong(x0, y0, x1, y1, 6, 8, 12, 1);			// gan nhu dac: che han trang phia duoi
	WAD_Bong(x0, y0, x1, y0 + 3, 150, 110, 60, 5);
	WAD_Vien(x0, y0, x1 - 1, y1 - 1, 190, 160, 90);
}

int KUiWAutoDsach::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int i;
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (KUPARAM)(KWndWindow*)&m_Dong)
		{
			Dong();
			return 1;
		}
		if (uParam == (KUPARAM)(KWndWindow*)&m_Nguon)
		{
			Mo(1);
			return 1;
		}
		for (i = 0; i < WA_DS_NUT; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nut[i])
			{
				autoData* p = CauHinh();
				switch (i)
				{
				case 0:		// Them (tu nguon dang chon)
					Them();
					break;
				case 1:		// Len
					Doi(-1);
					break;
				case 2:		// Xuong
					Doi(1);
					break;
				case 3:		// Xoa
					if (p && WA_DsXoa(m_nViec, p, m_Dsach.Chon()))
					{
						JxWAuto_LuuCauHinh();
						NapDanhSach();
					}
					break;
				case 4:		// Xoa het
					if (p && WA_DsXoaHet(m_nViec, p))
					{
						JxWAuto_LuuCauHinh();
						NapDanhSach();
					}
					break;
				case 5:		// Lay o day (toa do dang dung)
					if (p && WA_DsThem(m_nViec, p, -1, NULL, 0))
					{
						JxWAuto_LuuCauHinh();
						NapDanhSach();
					}
					else
						m_Huong.SetText("Kh«ng lÊy ®­îc: ®· ®ñ 24 täa ®é, hoÆc ch­a vµo game.");
					break;
				}
				return 1;
			}
	}
	else if (uMsg == WND_N_LIST_ITEM_SEL)
	{
		if (uParam == (KUPARAM)(KWndWindow*)&m_Dsach)
		{
			if (m_nCheDo == 1)
				ChonNguon((int)nParam);
			else if (m_nCheDo == 2)
			{
				KWndWindow* pGoi = m_pGoi;
				int nKhe = m_nKhe;
				int nMuc = (int)nParam;
				Dong();
				if (pGoi)
					pGoi->WndProc(WND_M_MENUITEM_SELECTED, (KUPARAM)pGoi, MAKELONG(nMuc, nKhe));
			}
			return 1;
		}
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}
#endif // JX_MOBILE
