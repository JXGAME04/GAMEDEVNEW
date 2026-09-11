//=========================================================================================
//	[SUAGD 13/09] PHAN MOBILE CUA HE SUA GIAO DIEN - duoc #include o CUOI UiToaDo.cpp (cung don vi dich, dung
//	chung s_Bang / s_ORieng / DatThongBao / OKhoi / VeChu...). Chuoi tieng Viet la TCVN3 tho -> chi sua bang
//	safe_edit.py / vn_edit.py (skill swordonline-dev). Xem PHANTICH_GIAODIEN_MOBILE_KEHOACH1209.md.
//
//	1. KHONG GIAN BO CUC = VUNG AN TOAN (JxSdl_LayVungAnToan, KSdlApp.cpp): toa do trong tep bo cuc la toa do
//	   TRONG vung an toan; dong ManHinh=W,H la co vung an toan luc luu. Cua so GOC (va o ve tay) cong goc vung
//	   an toan khi nap (DoiCaTepTheoNeo), tru di khi ghi (GhiTepVao). O con giu toa do tuong doi cha nhu cu.
//	2. LUOT KEP (SuaGd_Kep): o trong DANH SACH TRANG loi ra ngoai vung an toan thi day vao - sau khi cua so dung
//	   xong, sau khi thoat trinh chinh, sau khi vung an toan / nhan vat doi.
//	3. TRINH CHINH CHO NGUOI CHOI: cham chon (danh sach trang, dung sai 8 diem, chon o NHO NHAT chua diem cham),
//	   keo (bat dinh mep/tam + duong giong), chum hai ngon, bang mui ten 1 diem (giu = chay deu) + -/+ 5 %,
//	   giau/hien, mac dinh tung o, hoan tac, xong (= luu). Khung gioi han cha (KUiPlayerBar|Main...) KHONG nam
//	   trong danh sach -> cham nen khong con keo ca cum (goc loi 12/09: DummyWnd + Trans + anh nen).
//	4. HO SO THEO NHAN VAT: UserData\UiToaDo_<id>.ini; thieu thi UserData\UiToaDo.ini (luu ghi ca hai).
//	Chi bien dich khi JX_ANDROID; ban PC giu nguyen duong cu (Ctrl+U, thanh nut 09/09).
//=========================================================================================
#include <math.h>

extern "C" int  JxSdl_LayVungAnToan(int* pnL, int* pnT, int* pnW, int* pnH);	// KSdlApp.cpp
extern "C" int  JxSdl_VungAnToanDoi(void);
extern "C" int  JxSdl_HoKhung(void);
extern "C" int  JxCore_WAutoNhanVat(unsigned int* puId, int* pnLifeMax, int* pnManaMax);	// CoreShell.cpp
extern "C" void JxPlayerBar_NeoNhomTrenLai(void);	// UiPlayerBar.cpp

#define	SUAGD_TEP_DANHSACH	"\\Ui\\UiToaDo_DanhSach.ini"
#define	SUAGD_TEP_SOLIEU	"\\bocuc_thuc.txt"
#define	SUAGD_MAX_MUC		256		// [SUAGD 14/09 h] 96 -> 256: danh sach trang da 99 muc, hai icon Tong Kim / Kinh Mach (dong 98-99) bi rot khoi trinh chinh
#define	SUAGD_DUNGSAI		8		// cham cach mep o toi da bao nhieu diem van trung
#define	SUAGD_BATDINH		6		// bat dinh mep / tam trong khoang nay
#define	SUAGD_KEO_CHET		5		// [SUAGD 13/09 g] ngon phai di qua nguong nay moi bat dau doi (cham suong khong lam o nhich theo bat dinh)
#define	SUAGD_UNDO_MAX		64
#define	SUAGD_TILE_MIN		500		// nguoi choi: 50 %..200 %
#define	SUAGD_TILE_MAX		2000
#define	SUAGD_CO_KHONGPHONG	1		// co trong danh sach trang: khong cho to nho
#define	SUAGD_CO_KHONGAN	2		// khong cho giau
#define	SUAGD_CO_KHONGKEP	4		// khong kep vao vung an toan
#define	SUAGD_CO_GOPCON		8		// [SUAGD 13/09 f] hinh = gop ca o con dang hien (khung goc nho hon phan hien: muc tieu, thanh tren)
#define	SUAGD_CO_PHONGBANG	16		// [PHONGBANG 14/09] bang tinh nang: phong CA CAY theo man hinh luc mo (khong chum tay)
#define	SUAGD_CO_PHONGTRAN	32		// [PHONGBANG 14/09 h] kem 16: phong theo TRAN PhongBang, khong xet khung, can giua (khung dang nhap 800x600)
static int	s_nPhongBang = 1400;		// [Ui] PhongBang: tran phan nghin cua phep phong bang (0 = tat)
#define	SUAGD_PAD_O			64		// o nut cua bang chinh (mui ten, +, -); chu font 16 -> "Hoan tac" 8 chu x 8 px vua khit
#define	SUAGD_PAD_CACH		5
#define	SUAGD_PAD_TIEUDE	24		// hang tieu de (keo bang), chu 16
#define	SUAGD_PAD_INFO		18		// dong thong tin / dong so (chu 12)
#define	SUAGD_PAD_NUT		34		// hang nut chu (A / B), chu 16
#define	SUAGD_PAD_GON_RONG	44		// nut "gon" / "du" o goc phai tieu de
#define	SUAGD_LAP_CHO		400		// giu nut mui ten bao lau thi bat dau chay deu (ms)
#define	SUAGD_LAP_MOI		60		// ... roi moi bao lau mot buoc

#define	SUAGD_MAU_NUT		0xD0202838
#define	SUAGD_MAU_NUT_CHON	0xD02A6E2A
#define	SUAGD_MAU_NEN		0xB0141820
#define	SUAGD_MAU_CHU		0xFFF0F0F0
#define	SUAGD_MAU_CHU_MO	0xFFA0A0A0
#define	SUAGD_MAU_VANG		0xFFFFD24A
#define	SUAGD_MAU_KHUNG		0x9A5AD8FF
#define	SUAGD_MAU_KHUNG_AN	0xC0FF6A6A
#define	SUAGD_MAU_CHON		0xFFFFE040
#define	SUAGD_MAU_CHON_NEN	0x30FFE040
#define	SUAGD_MAU_GIONG		0xFFFF66FF
#define	SUAGD_MAU_ANTOAN	0x70FFFFFF

//---------------------------------------------------------------- tien ich ve
static void SuaGd_VeChuCo(int nCo, const char* pszChu, int x, int y, unsigned int uMau)
{
	int nDai = (int)strlen(pszChu);

	if (nDai <= 0 || g_pRepresentShell == NULL)
		return;
	g_pRepresentShell->OutputText(nCo, (char*)pszChu, nDai, x, y, uMau, 0, TEXT_IN_SINGLE_PLANE_COORD, 0xFF000000);	// vien DEN: vien trang cua VeChu lam chu nhoe
}

static void SuaGd_VeChuGiua(int nCo, const char* pszChu, int x, int y, int w, int h, unsigned int uMau)
{
	int nDai = (int)strlen(pszChu);

	SuaGd_VeChuCo(nCo, pszChu, x + (w - nDai * nCo / 2) / 2, y + (h - nCo) / 2, uMau);
}

static void SuaGd_VeKhung(int x0, int y0, int x1, int y1, unsigned int uMau)
{
	KRULine	L[4];
	int		i;

	if (g_pRepresentShell == NULL)
		return;
	for (i = 0; i < 4; i++)
		L[i].Color.Color_dw = uMau;
	L[0].oPosition.nX = x0; L[0].oPosition.nY = y0; L[0].oEndPos.nX = x1; L[0].oEndPos.nY = y0;
	L[1].oPosition.nX = x1; L[1].oPosition.nY = y0; L[1].oEndPos.nX = x1; L[1].oEndPos.nY = y1;
	L[2].oPosition.nX = x1; L[2].oPosition.nY = y1; L[2].oEndPos.nX = x0; L[2].oEndPos.nY = y1;
	L[3].oPosition.nX = x0; L[3].oPosition.nY = y1; L[3].oEndPos.nX = x0; L[3].oEndPos.nY = y0;
	g_pRepresentShell->DrawPrimitives(4, L, RU_T_LINE, true);
}

static void SuaGd_VeDuong(int x0, int y0, int x1, int y1, unsigned int uMau)
{
	KRULine	L;

	if (g_pRepresentShell == NULL)
		return;
	L.Color.Color_dw = uMau;
	L.oPosition.nX = x0; L.oPosition.nY = y0; L.oEndPos.nX = x1; L.oEndPos.nY = y1;
	g_pRepresentShell->DrawPrimitives(1, &L, RU_T_LINE, true);
}

static void SuaGd_VeNut(int x, int y, int w, int h, const char* pszChu, bool bChon, bool bMo)
{
	OKhoi(x, y, x + w, y + h, bChon ? SUAGD_MAU_NUT_CHON : SUAGD_MAU_NUT);
	SuaGd_VeKhung(x, y, x + w - 1, y + h - 1, bChon ? 0xFFAAFFAA : 0x80FFFFFF);
	SuaGd_VeChuGiua(16, pszChu, x, y, w, h, bMo ? SUAGD_MAU_CHU_MO : (bChon ? 0xFFAAFFAA : SUAGD_MAU_CHU));
}

//---------------------------------------------------------------- 1. khong gian bo cuc = vung an toan
static int	s_nKgL = 0, s_nKgT = 0, s_nKgW = 0, s_nKgH = 0;
//	(s_nGhiKgL / s_nGhiKgT - goc vung an toan luc GhiTepVao - khai bao trong UiToaDo.cpp vi GhiTepVao nam truoc tep nay)
static int	s_nKgDoiDaThay = -1;				// gia tri JxSdl_VungAnToanDoi da xu ly

static void UiToaDoM_KhongGian(int* pnL, int* pnT, int* pnW, int* pnH)
{
	int l = 0, t = 0, w = SCREEN_WIDTH, h = SCREEN_HEIGHT;

	JxSdl_LayVungAnToan(&l, &t, &w, &h);
	if (w <= 0 || h <= 0)
	{
		l = 0; t = 0; w = SCREEN_WIDTH; h = SCREEN_HEIGHT;
	}
	s_nKgL = l; s_nKgT = t; s_nKgW = w; s_nKgH = h;
	if (pnL) *pnL = l;
	if (pnT) *pnT = t;
	if (pnW) *pnW = w;
	if (pnH) *pnH = h;
}

//	Khoa la cua so GOC (hoac o ve tay - khong co '|'): toa do tuyet doi -> cong / tru goc vung an toan.
static bool UiToaDoM_LaGoc(const char* pszKhoa)
{
	char szCha[UITOADO_CO_KHOA];

	return KhoaCuaCha(pszKhoa, szCha, sizeof(szCha)) == false;
}

static int UiToaDoM_HoKhung()
{
	return JxSdl_HoKhung();
}

static bool UiToaDoM_CoTep(const char* pszTep)
{
	char	szDuongDan[MAX_PATH];
	FILE*	p;

	g_GetFullPath(szDuongDan, (char*)pszTep);
	p = fopen(szDuongDan, "rt");
	if (p == NULL)
		return false;
	fclose(p);
	return true;
}

//---------------------------------------------------------------- lop MAC DINH (chup sau khi nap tep mac dinh) + toa do GOC tu ini
static KOToaDo	s_BangMacDinh[UITOADO_MAX];
static int		s_nSoMacDinh = 0;

static void UiToaDoM_SauMacDinh()
{
	if (s_nSo > 0)
		memcpy(s_BangMacDinh, s_Bang, sizeof(KOToaDo) * s_nSo);
	s_nSoMacDinh = s_nSo;
}

static int UiToaDoM_TimMacDinh(const char* pszKhoa)
{
	int i;

	for (i = 0; i < s_nSoMacDinh; i++)
		if (strcmp(s_BangMacDinh[i].szKhoa, pszKhoa) == 0)
			return i;
	return -1;
}

struct KSuaGdGoc
{
	char	szKhoa[UITOADO_CO_KHOA];
	int		nLeft;
	int		nTop;
};
static KSuaGdGoc	s_Goc[SUAGD_MAX_MUC];
static int			s_nGoc = 0;
static unsigned int	s_uCanKepLuc = 0;		// hen luot kep (0 = khong)

static int  SuaGd_TimMuc(const char* pszKhoa);
static void SuaGd_NapDanhSach();

static int SuaGd_TimGoc(const char* pszKhoa)
{
	int i;

	for (i = 0; i < s_nGoc; i++)
		if (strcmp(s_Goc[i].szKhoa, pszKhoa) == 0)
			return i;
	return -1;
}

//	Goi tu UiToaDo_ApChoO (KWndWindow::Init, ngay sau SetPosition(ini)) - chi nho o trong danh sach trang.
static void UiToaDoM_NhoGoc(const char* pszKhoa, KWndWindow* pWnd)
{
	SuaGd_NapDanhSach();
	if (SuaGd_TimMuc(pszKhoa) < 0)
		return;
	s_uCanKepLuc = GetTickCount() + 400;		// mot o HUD vua dung -> hen kep sau khi ca cum dung xong
	if (SuaGd_TimGoc(pszKhoa) >= 0 || s_nGoc >= SUAGD_MAX_MUC)
		return;
	strncpy(s_Goc[s_nGoc].szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);
	s_Goc[s_nGoc].szKhoa[UITOADO_CO_KHOA - 1] = 0;
	pWnd->GetPosition(&s_Goc[s_nGoc].nLeft, &s_Goc[s_nGoc].nTop);
	s_nGoc++;
}

static void UiToaDoM_NhoGocORieng(int i)
{
	s_ORieng[i].nGocX = s_ORieng[i].nGocY = 0;
	s_ORieng[i].pfnLay(s_ORieng[i].pNgu, &s_ORieng[i].nGocX, &s_ORieng[i].nGocY);
}

//	Ap co rieng (nut ky nang...) tu truong TiLe cua bang: 1000 = theo config.
static void UiToaDoM_ApCoORieng(int i, int nMuc)
{
	int nCo = 0, nGoc = 0;

	if (i < 0 || i >= s_nSoORieng || s_ORieng[i].pfnLayCo == NULL || s_ORieng[i].pfnDatCo == NULL)
		return;
	s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
	if (nGoc <= 0)
		return;
	if (nMuc >= 0 && s_Bang[nMuc].nTiLe > 0 && s_Bang[nMuc].nTiLe != 1000)
		s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu, nGoc * s_Bang[nMuc].nTiLe / 1000);
	else
		s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu, 0);
}

void UiToaDo_DangKyORiengCo(const char* pszKhoa, PFN_UITOADO_LAYCO pfnLayCo, PFN_UITOADO_DATCO pfnDatCo)
{
	int i;

	if (pszKhoa == NULL || pfnLayCo == NULL || pfnDatCo == NULL)
		return;
	for (i = 0; i < s_nSoORieng; i++)
	{
		if (strcmp(s_ORieng[i].szKhoa, pszKhoa) == 0)
		{
			s_ORieng[i].pfnLayCo = pfnLayCo;
			s_ORieng[i].pfnDatCo = pfnDatCo;
			UiToaDoM_ApCoORieng(i, TimKhoa(pszKhoa));
			return;
		}
	}
}

void UiToaDo_DangKyORiengHinh(const char* pszKhoa, PFN_UITOADO_LAYHINH pfnLayHinh)
{
	int i;

	if (pszKhoa == NULL || pfnLayHinh == NULL)
		return;
	for (i = 0; i < s_nSoORieng; i++)
		if (strcmp(s_ORieng[i].szKhoa, pszKhoa) == 0)
		{
			s_ORieng[i].pfnLayHinh = pfnLayHinh;
			return;
		}
}

int UiToaDo_CoKhoa(const char* pszKhoa)
{
	return (pszKhoa && pszKhoa[0] && TimKhoa(pszKhoa) >= 0) ? 1 : 0;
}

//---------------------------------------------------------------- ho so theo nhan vat
static char			s_szTepNguoiChoi[MAX_PATH];
static unsigned int	s_uNhanVatNap = 0;		// ma nhan vat cua ho so dang nap (0 = tep chung)

static unsigned int UiToaDoM_MaNhanVat()
{
	unsigned int u = 0;

	if (!JxCore_WAutoNhanVat(&u, NULL, NULL))
		return 0;
	return u;
}

static const char* UiToaDoM_TepNguoiChoi()
{
	unsigned int u = UiToaDoM_MaNhanVat();

	s_uNhanVatNap = u;
	if (u)
	{
		_snprintf(s_szTepNguoiChoi, sizeof(s_szTepNguoiChoi), "\\UserData\\UiToaDo_%u.ini", u);
		s_szTepNguoiChoi[sizeof(s_szTepNguoiChoi) - 1] = 0;
		if (UiToaDoM_CoTep(s_szTepNguoiChoi))
			return s_szTepNguoiChoi;
	}
	return UITOADO_TEP;
}

static int UiToaDoM_GhiTep()
{
	int			 n = GhiTepVao(UITOADO_TEP);
	unsigned int u = UiToaDoM_MaNhanVat();

	if (u)
	{
		char szTep[MAX_PATH];

		_snprintf(szTep, sizeof(szTep), "\\UserData\\UiToaDo_%u.ini", u);
		szTep[sizeof(szTep) - 1] = 0;
		GhiTepVao(szTep);
		s_uNhanVatNap = u;
	}
	return n;
}

static void UiToaDoM_XoaHet()
{
	int i;

	for (i = 0; i < s_nSoORieng; i++)
	{
		s_ORieng[i].pfnDat(s_ORieng[i].pNgu, s_ORieng[i].nGocX, s_ORieng[i].nGocY);
		if (s_ORieng[i].pfnDatCo)
			s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu, 0);
	}
	if (s_uNhanVatNap)
	{
		char szTep[MAX_PATH], szDuongDan[MAX_PATH];

		_snprintf(szTep, sizeof(szTep), "\\UserData\\UiToaDo_%u.ini", s_uNhanVatNap);
		szTep[sizeof(szTep) - 1] = 0;
		g_GetFullPath(szDuongDan, szTep);
		remove(szDuongDan);
	}
}

//---------------------------------------------------------------- 2. danh sach trang + o da tim thay
struct KSuaGdMuc
{
	char	szKhoa[UITOADO_CO_KHOA];
	char	szTen[64];		// ten hien thi (TCVN3)
	int		nCo;			// SUAGD_CO_*
};
static KSuaGdMuc	s_DS[SUAGD_MAX_MUC];
static int			s_nDS = 0;
static int			s_nDSTinh = 0;		// [SUAGD 13/09 d] so muc TINH (tep / ma); sau do la cua so dang mo, them lai moi lan lam moi
static bool			s_bDSDaNap = false;
static char			s_szChonKhoa[UITOADO_CO_KHOA] = "";	// khoa cua o dang chon (chi so co the doi khi cua so mo / dong)
static int			s_nChon = -1;			// chi so danh sach dang chon (-1 = chua)

struct KSuaGdO
{
	KWndWindow*	pWnd;		// o KWnd (NULL neu la o ve tay)
	int			nORieng;	// chi so s_ORieng (-1 neu la KWnd)
	int			nL, nT, nW, nH;	// hinh chu nhat tuyet doi
	bool		bCo;		// tim thay trong cay cua so / bang o ve tay
	bool		bHien;		// dang hien (ca cha)
};
static KSuaGdO		s_O[SUAGD_MAX_MUC];
static unsigned int	s_uLamMoiLuc = 0;
static bool			s_bTrongGame = false;

static int SuaGd_TimMuc(const char* pszKhoa)
{
	int i;

	for (i = 0; i < s_nDS; i++)
		if (s_DS[i].szKhoa[0] == pszKhoa[0] && strcmp(s_DS[i].szKhoa, pszKhoa) == 0)
			return i;
	return -1;
}

static void SuaGd_ThemMuc(const char* pszKhoa, const char* pszTen, int nCo)
{
	if (s_nDS >= SUAGD_MAX_MUC || pszKhoa == NULL || pszKhoa[0] == 0 || SuaGd_TimMuc(pszKhoa) >= 0)
		return;
	strncpy(s_DS[s_nDS].szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);
	s_DS[s_nDS].szKhoa[UITOADO_CO_KHOA - 1] = 0;
	strncpy(s_DS[s_nDS].szTen, (pszTen && pszTen[0]) ? pszTen : pszKhoa, sizeof(s_DS[s_nDS].szTen) - 1);
	s_DS[s_nDS].szTen[sizeof(s_DS[s_nDS].szTen) - 1] = 0;
	s_DS[s_nDS].nCo = nCo;
	s_nDS++;
}

//	[SUAGD 13/09 e] Che do "hien cac o da giau" trong trinh chinh: mac dinh CAT (khong ve, khong chon duoc - chu 13/09
//	"xoa cac icon da an"); bam nut Giau khi KHONG chon o -> bat / tat. KWndWindow::Paint / PtInWindow (Android) hoi qua
//	UiToaDo_HienOAn() de chi ve / bat o da giau khi nay. Tat trinh chinh = cat lai.
static bool			s_bHienOAn = false;

bool UiToaDo_HienOAn()
{
	return s_bDangSua && s_bHienOAn;
}

//	Danh sach trong ma - chi dung khi thieu tep Ui\UiToaDo_DanhSach.ini (ten hien thi = khoa).
static const struct { const char* pszKhoa; int nCo; } s_DSMa[] =
{
	{ "KUiPlayerBar|Status", 0 }, { "KUiPlayerBar|Items", 0 }, { "KUiPlayerBar|Skills", 0 },
	{ "KUiPlayerBar|Faction", 0 }, { "KUiPlayerBar|Team", 0 }, { "KUiPlayerBar|Friend", 0 },
	{ "KUiPlayerBar|Options", 0 }, { "KUiPlayerBar|Market", 0 }, { "KUiPlayerBar|TraceBtn", 0 }, { "KUiPlayerBar|AutoPlay", 0 },
	{ "KUiPlayerBar|SpringGame", 0 },
	{ "KUiPlayerBar|HideChat", 0 }, { "KUiPlayerBar|ChannelBtn", 0 },
	{ "KUiPlayerBar|InputBack", SUAGD_CO_KHONGPHONG }, { "KUiPlayerBar|Face", 0 }, { "KUiPlayerBar|SendBtn", 0 },
	{ "KUiPlayerBar|HideIcons", 0 }, { "KUiPlayerBar|Item_0", 0 }, { "KUiPlayerBar|Item_1", 0 },
	{ "KUiPlayerBar|Item_2", 0 }, { "KUiPlayerBar|Item_3", 0 }, { "KUiPlayerBar|DateTime", SUAGD_CO_KHONGPHONG },
	{ "KUiPlayerBar|WifiStatus", 0 },
	{ "KUiPlayerBar|HideWindow", 0 }, { "KUiPlayerBar|PartnerIcon", 0 }, { "KUiPlayerBar|ImediaLeftSkill", 0 },
	{ "KUiPlayerBar|ImediaRightSkill", 0 }, { "KUiToolsControlBar|Rec", 0 },	// [SUAGD 13/09 e] giau san trong mac dinh
	{ "KUiToolsControlBar|Run", 0 }, { "KUiToolsControlBar|Sit", 0 }, { "KUiToolsControlBar|Horse", 0 },
	{ "KUiToolsControlBar|Exchange", 0 }, { "KUiToolsControlBar|PK", 0 }, { "KUiToolsControlBar|WAuto", 0 }, { "KUiToolsControlBar|NhatDo", 0 },
	{ "KUiMiniMap|MiniMap", SUAGD_CO_KHONGPHONG }, { "KUiMiniMap|WorldMapBtn", 0 }, { "KUiMiniMap|BtnFlag", 0 },
	{ "KUiMiniMap|SwitchBtn", 0 }, { "KUiMiniMap|CaveMapBtn", 0 }, { "KUiMiniMap|LockBtn", 0 },
	{ "KUiMiniMapTo|MiniMap", SUAGD_CO_KHONGPHONG }, { "KUiMiniMapToEx|MiniMap", SUAGD_CO_KHONGPHONG }, { "KUiMiniMapKhongAnh|MiniMap", SUAGD_CO_KHONGPHONG },	// [UITOADO 14/09 LOPPHU]
	{ "KUiCLIcon|Main", 0 }, { "KUiAuctionIcon|Main", 0 }, { "KUiMailIcon|Main", 0 },
	{ "KUiChienLenh|Main", SUAGD_CO_KHONGPHONG }, { "KUiTaskTrace|Main", SUAGD_CO_KHONGPHONG },
	{ "KUiMsgCentrePad|Main", SUAGD_CO_KHONGPHONG | SUAGD_CO_KHONGKEP }, { "KSysMsgCentrePad|SysRoom", SUAGD_CO_KHONGPHONG | SUAGD_CO_KHONGKEP },
	{ "KUiSysMsgCentre|Main", SUAGD_CO_KHONGPHONG | SUAGD_CO_KHONGKEP }, { "KUiHeaderControlBar|Main", SUAGD_CO_KHONGPHONG | SUAGD_CO_GOPCON },
	{ "KUiTargetInfo|Main", SUAGD_CO_KHONGPHONG | SUAGD_CO_GOPCON }, { "ThanhBuff", SUAGD_CO_KHONGPHONG },	// [SUAGD 13/09 f]
	{ "KUiTongKimScore|Main", SUAGD_CO_KHONGPHONG }, { "KUiTongKimInfo|Main", SUAGD_CO_KHONGPHONG },	// [SUAGD 13/09 g]
	{ "KyNang0", 0 }, { "KyNang1", 0 }, { "KyNang2", 0 }, { "KyNang3", 0 }, { "KyNang4", 0 },
	{ "KyNang5", 0 }, { "KyNang6", 0 }, { "KyNang7", 0 }, { "KyNang8", 0 }, { "KyNangGan", 0 },
	{ "IconNpc", SUAGD_CO_KHONGPHONG }, { "ThongTinGoc", SUAGD_CO_KHONGPHONG },
	{ NULL, 0 }
};

static void SuaGd_NapDanhSach()
{
	char	szDuongDan[MAX_PATH];
	char	szDong[320];
	FILE*	pTep;
	int		i;

	if (s_bDSDaNap)
		return;
	s_bDSDaNap = true;
	s_nDS = 0;
	g_GetFullPath(szDuongDan, (char*)SUAGD_TEP_DANHSACH);
	pTep = fopen(szDuongDan, "rt");
	if (pTep)
	{
		while (fgets(szDong, sizeof(szDong), pTep))
		{
			char*	p = szDong;
			char*	pBang;
			char*	pTen;
			char*	pPhay;
			char*	e;
			int		n, nCo = 0;

			while (*p == ' ' || *p == '\t')
				p++;
			if (*p == ';' || *p == '[' || *p == '\r' || *p == '\n' || *p == 0)
				continue;
			pBang = strchr(p, '=');
			if (pBang == NULL)
				continue;
			*pBang = 0;
			n = (int)strlen(p);
			while (n > 0 && (p[n - 1] == ' ' || p[n - 1] == '\t'))
				p[--n] = 0;
			if (n == 0)
				continue;
			pTen = pBang + 1;
			pPhay = strchr(pTen, ',');
			if (pPhay)
			{
				*pPhay = 0;
				nCo = atoi(pPhay + 1);
			}
			e = pTen + strlen(pTen);
			while (e > pTen && (e[-1] == '\r' || e[-1] == '\n' || e[-1] == ' ' || e[-1] == '\t'))
				*--e = 0;
			SuaGd_ThemMuc(p, pTen, nCo);
		}
		fclose(pTep);
		g_DebugLog("[SUAGD] danh sach trang: %d muc tu %s", s_nDS, szDuongDan);
	}
	if (s_nDS == 0)
	{
		for (i = 0; s_DSMa[i].pszKhoa; i++)
			SuaGd_ThemMuc(s_DSMa[i].pszKhoa, NULL, s_DSMa[i].nCo);
		g_DebugLog("[SUAGD] khong co %s -> dung danh sach trong ma (%d muc)", szDuongDan, s_nDS);
	}
	s_nDSTinh = s_nDS;
}

//	[SUAGD 13/09 d] lop cua so KHONG cho chon tam (nen dang nhap, khung HUD chua con, dong thong bao)
static bool SuaGd_LopKhongDong(const char* pszLop)
{
	static const char* s_aLop[] = { "KUiLoginBackGround", "KUiPlayerBar", "KUiToolsControlBar", "KUiMsgCentrePad", "KSysMsgCentrePad",
		"KUiNewsMessage", "KUiFlashMessage", "KUiSysMsgCentre", "KUiHeaderControlBar", "KUiTongKimScore", "KUiTongKimInfo", NULL };
	int i;

	for (i = 0; s_aLop[i]; i++)
		if (strncmp(pszLop, s_aLop[i], strlen(s_aLop[i])) == 0)
			return true;
	return false;
}

static void SuaGd_Chon(int m)
{
	s_nChon = m;
	if (m >= 0 && m < s_nDS)
	{
		strncpy(s_szChonKhoa, s_DS[m].szKhoa, UITOADO_CO_KHOA - 1);
		s_szChonKhoa[UITOADO_CO_KHOA - 1] = 0;
	}
	else
		s_szChonKhoa[0] = 0;
}

//	Gop hinh chu nhat cua cac con DANG HIEN (de quy). GetAllChildLayoutRect gop ca con an - khung chat co 6 phong
//	chat an nam o rel -300 -> hinh rong 685 lech ra ngoai man, luot kep day ca khung chat sang phai 305 diem (do 13/09).
static void SuaGd_GopConHien(KWndWindow* pWnd, int* pl, int* pt, int* pr, int* pb, bool* pbCo)
{
	KWndWindow* p;

	for (p = pWnd->GetFirstChild(); p; p = p->GetNextWnd())
	{
		int l = 0, t = 0, w = 0, h = 0;

		if (!p->IsVisible())
			continue;
		p->GetAbsolutePos(&l, &t);
		p->GetSize(&w, &h);
		//	con nam HOAN TOAN ngoai khung = bi "giau" kieu cu (khung chat day phong khong dung ra x = -300): bo qua
		if (w > 0 && h > 0 && (l + w <= 0 || t + h <= 0 || l >= SCREEN_WIDTH || t >= SCREEN_HEIGHT))
			continue;
		if (w > 0 && h > 0 && (p->GetStyle() & WND_S_SIZE_WITH_ALL_CHILD) == 0)
		{
			if (!*pbCo || l < *pl)		*pl = l;
			if (!*pbCo || t < *pt)		*pt = t;
			if (!*pbCo || l + w > *pr)	*pr = l + w;
			if (!*pbCo || t + h > *pb)	*pb = t + h;
			*pbCo = true;
		}
		if (p->GetFirstChild())
			SuaGd_GopConHien(p, pl, pt, pr, pb, pbCo);
	}
}

static void SuaGd_HinhKWnd(KWndWindow* pWnd, int* pl, int* pt, int* pw, int* ph, int nCo = 0)
{
	int l = 0, t = 0, w = 0, h = 0;

	pWnd->GetAbsolutePos(&l, &t);
	pWnd->GetSize(&w, &h);
	if ((pWnd->GetStyle() & WND_S_SIZE_WITH_ALL_CHILD) || w <= 0 || h <= 0)
	{
		int  L = 0, T = 0, R = 0, B = 0;
		bool bCo = false;

		SuaGd_GopConHien(pWnd, &L, &T, &R, &B, &bCo);
		if (bCo)
		{
			l = L; t = T; w = R - L; h = B - T;
		}
		else
			w = h = 0;		// khong con nao hien -> khong chon, khong kep
	}
	else if (nCo & SUAGD_CO_GOPCON)
	{	//	[SUAGD 13/09 f] khung goc co kich thuoc nhung phan hien nam o cac o con (khung muc tieu 27x23 + hop ben duoi, thanh tren 17 px + dong so)
		int  L = l, T = t, R = l + w, B = t + h;
		bool bCo = true;

		SuaGd_GopConHien(pWnd, &L, &T, &R, &B, &bCo);
		l = L; t = T; w = R - L; h = B - T;
	}
	*pl = l; *pt = t; *pw = w; *ph = h;
}

static void SuaGd_DuyetCay(KWndWindow* pWnd, bool bChaHien)
{
	char szKhoa[UITOADO_CO_KHOA];

	while (pWnd)
	{
		bool bHien = bChaHien && (pWnd->IsVisible() != 0) && (s_bHienOAn || !pWnd->UiDangAn());	// [SUAGD 13/09 e] o da giau: chi khi dang "hien o da giau"

		if (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)))
		{
			int m = SuaGd_TimMuc(szKhoa);

			if (m >= 0 && !s_O[m].bCo)
			{
				s_O[m].bCo = true;
				s_O[m].pWnd = pWnd;
				s_O[m].nORieng = -1;
				s_O[m].bHien = bHien;
				SuaGd_HinhKWnd(pWnd, &s_O[m].nL, &s_O[m].nT, &s_O[m].nW, &s_O[m].nH, s_DS[m].nCo);
			}
		}
		if (pWnd->GetFirstChild())
			SuaGd_DuyetCay(pWnd->GetFirstChild(), bHien);
		pWnd = pWnd->GetNextWnd();
	}
}

//	Dang o trong the gioi: co thanh cong cu (KUiToolsControlBar) - nhu KyNang_TrongGame.
static bool SuaGd_TrongGame()
{
	int nLop;

	for (nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow* pGoc = Wnd_GetLayerRoot(nLop);
		KWndWindow* p;
		char		szLop[64];

		if (pGoc == NULL)
			continue;
		for (p = pGoc->GetNextWnd(); p; p = p->GetNextWnd())
		{
			LayTenLop(p, szLop, sizeof(szLop));
			if (strcmp(szLop, "KUiToolsControlBar") == 0)
				return true;
		}
	}
	return false;
}

//	Tim lai moi o cua danh sach trang trong cay cua so + bang o ve tay (con tro co the doi khi cua so dung lai).
static void SuaGd_LamMoi()
{
	int nLop, i;

	SuaGd_NapDanhSach();
	s_nDS = s_nDSTinh;		// [SUAGD 13/09 d] bo cac cua so dang mo cua lan truoc, them lai ben duoi
	for (i = 0; i < SUAGD_MAX_MUC; i++)
	{
		s_O[i].bCo = false;
		s_O[i].pWnd = NULL;
		s_O[i].nORieng = -1;
		s_O[i].bHien = false;
	}
	for (nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow* pGoc = Wnd_GetLayerRoot(nLop);

		if (pGoc)
			SuaGd_DuyetCay(pGoc->GetNextWnd(), true);
	}
	s_bTrongGame = SuaGd_TrongGame();
	for (i = 0; i < s_nSoORieng; i++)
	{
		int m = SuaGd_TimMuc(s_ORieng[i].szKhoa);
		int x = 0, y = 0, nCo = 40, nGoc = 40;

		if (m < 0 || s_O[m].bCo)
			continue;
		s_ORieng[i].pfnLay(s_ORieng[i].pNgu, &x, &y);
		if (s_ORieng[i].pfnLayCo)
			s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
		if (nCo <= 0)
			nCo = 40;
		s_O[m].bCo = true;
		s_O[m].nORieng = i;
		s_O[m].bHien = s_bTrongGame;
		if (s_ORieng[i].pfnLayHinh)
		{	//	o khong theo quy uoc "tam + co" (dong FPS neo mep phai-tren): lay hinh chu nhat that
			int l = 0, t = 0, w = 0, h = 0;

			s_ORieng[i].pfnLayHinh(s_ORieng[i].pNgu, &l, &t, &w, &h);
			s_O[m].nL = l; s_O[m].nT = t; s_O[m].nW = w; s_O[m].nH = h;
		}
		else
		{
			s_O[m].nL = x - nCo / 2;
			s_O[m].nT = y - nCo / 2;
			s_O[m].nW = nCo;
			s_O[m].nH = nCo;
		}
	}
	//	[SUAGD 13/09 d] cua so DANG MO (hop thoai mo tu icon, man dang nhap...) chua co trong danh sach: them tam, chi doi cho.
	//	Chu: "cac giao dien khi bam vao icon chua chinh sua toa do duoc", "cac phan luc dang nhap vao game chua tu can chinh duoc".
	for (nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow* pGoc = Wnd_GetLayerRoot(nLop);
		KWndWindow* p;

		for (p = pGoc ? pGoc->GetNextWnd() : NULL; p; p = p->GetNextWnd())
		{
			char szKhoa[UITOADO_CO_KHOA], szLop[64];
			int  l, t, w, h, m;

			if (!p->IsVisible() || (p->GetStyle() & WND_S_SIZE_WITH_ALL_CHILD))
				continue;
			if (!TaoKhoaTuOCon(p, szKhoa, sizeof(szKhoa)) || SuaGd_TimMuc(szKhoa) >= 0)
				continue;
			LayTenLop(p, szLop, sizeof(szLop));
			if (SuaGd_LopKhongDong(szLop))
				continue;
			SuaGd_HinhKWnd(p, &l, &t, &w, &h);
			if (w <= 0 || h <= 0 || (long long)w * h * 10 > (long long)SCREEN_WIDTH * SCREEN_HEIGHT * 9)
				continue;	//	khong co hinh / phu gan het man (nen)
			if (s_nDS >= SUAGD_MAX_MUC)
				break;
			SuaGd_ThemMuc(szKhoa, (strncmp(szLop, "KUi", 3) == 0) ? szLop + 3 : szLop,
				SUAGD_CO_KHONGPHONG | SUAGD_CO_KHONGAN | SUAGD_CO_KHONGKEP);
			m = s_nDS - 1;
			if (m < 0 || strcmp(s_DS[m].szKhoa, szKhoa) != 0)
				continue;
			s_O[m].bCo = true;
			s_O[m].pWnd = p;
			s_O[m].nORieng = -1;
			s_O[m].bHien = true;
			s_O[m].nL = l; s_O[m].nT = t; s_O[m].nW = w; s_O[m].nH = h;
		}
	}
	//	o dang chon: tim lai theo khoa (chi so doi khi cua so mo / dong)
	if (s_nChon >= 0 || s_szChonKhoa[0])
		s_nChon = s_szChonKhoa[0] ? SuaGd_TimMuc(s_szChonKhoa) : -1;
	s_uLamMoiLuc = GetTickCount();
}

static void UiToaDoM_QuenCuaSo(KWndWindow* pWnd)
{
	int i;

	for (i = 0; i < s_nDS; i++)
		if (s_O[i].pWnd == pWnd)
		{
			s_O[i].pWnd = NULL;
			s_O[i].bCo = false;
		}
}

//	O NHO NHAT chua diem cham (cong dung sai). -1 = khong.
static int SuaGd_TimODuoi(int x, int y)
{
	int i, nTot = -1, nDT = 0;

	for (i = 0; i < s_nDS; i++)
	{
		int l, t, w, h, dt;

		if (!s_O[i].bCo || !s_O[i].bHien)
			continue;
		l = s_O[i].nL - SUAGD_DUNGSAI;
		t = s_O[i].nT - SUAGD_DUNGSAI;
		w = s_O[i].nW + 2 * SUAGD_DUNGSAI;
		h = s_O[i].nH + 2 * SUAGD_DUNGSAI;
		if (x < l || y < t || x >= l + w || y >= t + h)
		{	//	o ve tay: ham do trung cua chinh no la chuan (dong FPS, icon NPC...) - khong chi dua vao hinh chu nhat
			if (s_O[i].nORieng < 0 || s_nCongCu == CONGCU_KHOI
				|| !s_ORieng[s_O[i].nORieng].pfnTrung(s_ORieng[s_O[i].nORieng].pNgu, x, y))
				continue;
		}
		dt = w * h;
		if (nTot < 0 || dt < nDT)
		{
			nTot = i;
			nDT = dt;
		}
	}
	return nTot;
}

static KWndWindow* SuaGd_TimKWnd_De(KWndWindow* pWnd, const char* pszKhoa)
{
	char szKhoa[UITOADO_CO_KHOA];

	while (pWnd)
	{
		if (TaoKhoaTuOCon(pWnd, szKhoa, sizeof(szKhoa)) && strcmp(szKhoa, pszKhoa) == 0)
			return pWnd;
		if (pWnd->GetFirstChild())
		{
			KWndWindow* p = SuaGd_TimKWnd_De(pWnd->GetFirstChild(), pszKhoa);

			if (p)
				return p;
		}
		pWnd = pWnd->GetNextWnd();
	}
	return NULL;
}

static KWndWindow* SuaGd_TimKWnd(const char* pszKhoa)
{
	int nLop;

	for (nLop = 0; nLop < 3; nLop++)
	{
		KWndWindow* pGoc = Wnd_GetLayerRoot(nLop);
		KWndWindow* p = pGoc ? SuaGd_TimKWnd_De(pGoc->GetNextWnd(), pszKhoa) : NULL;

		if (p)
			return p;
	}
	return NULL;
}

static int SuaGd_TimORieng(const char* pszKhoa)
{
	int i;

	for (i = 0; i < s_nSoORieng; i++)
		if (strcmp(s_ORieng[i].szKhoa, pszKhoa) == 0)
			return i;
	return -1;
}

//---------------------------------------------------------------- [PHONGBANG 14/09] phong bang theo man hinh
//	Chu 14/09: 'cac o item trong hanh trang - ruong - Auto - cac bang tinh nang ... giu nguyen hien tai, chi phong to theo kich
//	thuoc moi dien thoai ... chum 2 ngon rat bat tien'. Bang co SUAGD_CO_PHONGBANG (danh sach trang ,23) luc duoc ap bo cuc:
//	k = min(khung rong / rong goc, khung cao / cao goc) x 0,96, chan tran [Ui] PhongBang -> KWndWindow::UiPhongCay(k), roi dich
//	bang de giu tam nhu truoc khi phong va kep trong khung. Vi tri trong BANG BO CUC = vi tri o co GOC (lech giu tam ghi vao
//	m_nUiPhongLech, GhiLaiO cong lai) nen ap lai / keo / kep vung an toan khong bi troi. Tinh tu GOC nen ap lai van mot ket qua.
static void SuaGd_DocCauHinh();
static int UiToaDoM_PhongBang(KWndWindow* p, const char* pszKhoa)
{
	int i, nCo, w0, h0, k, kW, kH, l, t, wS, hS, nLechX, nLechY;
	SuaGd_DocCauHinh();
	i = SuaGd_TimMuc(pszKhoa);
	nCo = (i >= 0) ? s_DS[i].nCo : 0;
	if (!(nCo & SUAGD_CO_PHONGBANG) || s_nPhongBang < 1000)
		return 0;
	w0 = p->UiLayGocW();
	h0 = p->UiLayGocH();
	if (w0 <= 0 || h0 <= 0 || SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0)
		return 0;
	if (nCo & SUAGD_CO_PHONGTRAN)	// [h] cua so to hon khung nhung noi dung nam giua (khung dang nhap): phong dung tran
		k = s_nPhongBang;
	else
	{
		kW = SCREEN_WIDTH * 1000 / w0;
		kH = SCREEN_HEIGHT * 1000 / h0;
		k = (kW < kH ? kW : kH) * 96 / 100;
		if (k > s_nPhongBang)
			k = s_nPhongBang;
	}
	k = p->UiKhopTiLeCay(k);	// [KHOPO 14/09] bang co luoi vat pham: chon k de o luoi = so nguyen, khop anh nen gian
	if (k < 1020)
		k = 1000;
	if (k == 1000 && p->UiLayPhongCay() == 1000)
		return 0;
	p->GetPosition(&l, &t);
	p->UiPhongCay(k);
	p->GetSize(&wS, &hS);
	nLechX = (wS - w0) / 2;	// giu tam nhu truoc khi phong
	nLechY = (hS - h0) / 2;
	if (nCo & SUAGD_CO_PHONGTRAN)
	{	// [h] can giua khung, cho phep toa do am (phan ngoai khung la nen trong suot), khong kep
		l = (SCREEN_WIDTH - wS) / 2;
		t = (SCREEN_HEIGHT - hS) / 2;
	}
	else
	{
		if (p->UiLayViTriTuBang())	// [g] vi tri vua dat tu bang bo cuc = vi tri o co GOC -> dich giu tam; con lai (chinh PhongBang da dat,
		{					// hay ma bang tu dat voi co da phong, vd Auto can giua) giu nguyen, chi kep trong khung
			l -= nLechX;
			t -= nLechY;
		}
		if (l + wS > SCREEN_WIDTH)
			l = SCREEN_WIDTH - wS;
		if (t + hS > SCREEN_HEIGHT)
			t = SCREEN_HEIGHT - hS;
		if (l < 0)
			l = 0;
		if (t < 0)
			t = 0;
	}
	p->SetPosition(l, t);
	p->UiDatPhongLech(nLechX, nLechY);
	p->UiGhiPhongDat(l, t);	// [c]
	p->UiXoaViTriTuBang();
	p->UiNhoViTri();
	g_DebugLog("[PHONGBANG] %s goc %dx%d khung %dx%d -> k=%d -> %dx%d tai %d,%d (lech %d,%d)", pszKhoa, w0, h0, SCREEN_WIDTH, SCREEN_HEIGHT, k, wS, hS, l, t, nLechX, nLechY);
	return 1;
}
//	o CON nam trong mot bang da phong ca cay -> ti le cay cua to tien gan nhat (1000 = khong)
static int UiToaDoM_KCayCha(KWndWindow* p)
{
	for (KWndWindow* q = p ? p->GetParent() : NULL; q; q = q->GetParent())
		if (q->UiLayPhongCay() != 1000)
			return q->UiLayPhongCay();
	return 1000;
}
//---------------------------------------------------------------- ap / ghi bang
static void SuaGd_ApKWnd(KWndWindow* p, int nLeft, int nTop, int nTiLe, int nAn)
{
	int nKCay = UiToaDoM_KCayCha(p);	// [PHONGBANG 14/09] o con cua bang da phong: toa do / ti le trong ho so hoa so theo cay
	if (nKCay != 1000)
	{
		p->UiDatGocViTri(nLeft, nTop);
		p->SetPosition(nLeft * nKCay / 1000, nTop * nKCay / 1000);
		p->UiNhoViTri();
		p->UiPhongCay(nKCay * ((!CamCoGian(p) && nTiLe > 0) ? nTiLe : 1000) / 1000);	// [d] phong ca cay con cua o
		p->UiDatAn(nAn ? 1 : 0);
		return;
	}
	p->SetPosition(nLeft, nTop);
	p->UiDatPhongLech(0, 0);	// [PHONGBANG 14/09 b] vi tri tu bang bo cuc = vi tri o co goc
	p->UiDanhDauViTriBang();	// [g]
	p->UiNhoViTri();
	if (!CamCoGian(p) && nTiLe > 0 && (nTiLe != 1000 || p->UiLayTiLe() != 1000))
		p->UiDatTiLe(nTiLe);
	p->UiDatAn(nAn ? 1 : 0);
	{	// [PHONGBANG 14/09] bang: phong ca cay theo man hinh (ke ca khi trinh chinh tra mac dinh / doi ho so)
		char szKhoaPB[UITOADO_CO_KHOA];
		if (TaoKhoaTuOCon(p, szKhoaPB, sizeof(szKhoaPB)))
			UiToaDoM_PhongBang(p, szKhoaPB);
	}
}

static void SuaGd_GhiBangORieng(int i)
{
	int x = 0, y = 0, nCo = 0, nGoc = 0, nTiLe = 1000;

	s_ORieng[i].pfnLay(s_ORieng[i].pNgu, &x, &y);
	if (s_ORieng[i].pfnLayCo)
	{
		s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
		if (nGoc > 0 && nCo > 0)
			nTiLe = nCo * 1000 / nGoc;
	}
	DatKhoa(s_ORieng[i].szKhoa, x, y, nTiLe, 0);
}

static void SuaGd_XoaKhoa(const char* pszKhoa)
{
	int i = TimKhoa(pszKhoa);

	if (i < 0)
		return;
	s_nSo--;
	if (i != s_nSo)
	{
		s_Bang[i] = s_Bang[s_nSo];
		s_aTepNap[i] = s_aTepNap[s_nSo];
	}
}

//	Ti le hien tai cua mot o trong danh sach (phan nghin).
static int SuaGd_TiLeCua(int m)
{
	if (m < 0 || !s_O[m].bCo)
		return 1000;
	if (s_O[m].nORieng >= 0)
	{
		int i = s_O[m].nORieng, nCo = 0, nGoc = 0;

		if (s_ORieng[i].pfnLayCo)
		{
			s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
			if (nGoc > 0 && nCo > 0)
				return nCo * 1000 / nGoc;
		}
		return 1000;
	}
	return s_O[m].pWnd->UiLayTiLe();
}

//---------------------------------------------------------------- hoan tac
struct KSuaGdUndo
{
	char	szKhoa[UITOADO_CO_KHOA];
	int		nNhom;			// cac muc cung mot thao tac co cung so nhom -> hoan tac mot lan
	int		nORieng;		// -1 = KWnd
	int		nLeft, nTop;	// KWnd: tuong doi cha; ORieng: tam
	int		nTiLe;			// KWnd: phan nghin; ORieng: co tuyet doi (0 = theo config)
	int		nAn;
	int		nCoBang;		// bang co muc nay khong (de xoa lai khi hoan tac)
	KOToaDo	oBang;
};
static KSuaGdUndo	s_Undo[SUAGD_UNDO_MAX];
static int			s_nUndo = 0;
static int			s_nUndoNhom = 0;

static void SuaGd_UndoNhomMoi()
{
	s_nUndoNhom++;
}

static KSuaGdUndo* SuaGd_UndoMoi(const char* pszKhoa)
{
	KSuaGdUndo* u;
	int			n;

	if (s_nUndo >= SUAGD_UNDO_MAX)
	{
		memmove(&s_Undo[0], &s_Undo[1], sizeof(KSuaGdUndo) * (SUAGD_UNDO_MAX - 1));
		s_nUndo = SUAGD_UNDO_MAX - 1;
	}
	u = &s_Undo[s_nUndo++];
	memset(u, 0, sizeof(*u));
	strncpy(u->szKhoa, pszKhoa, UITOADO_CO_KHOA - 1);
	u->nNhom = s_nUndoNhom;
	n = TimKhoa(pszKhoa);
	if (n >= 0)
	{
		u->nCoBang = 1;
		u->oBang = s_Bang[n];
	}
	return u;
}

static void SuaGd_GhiUndoKWnd(KWndWindow* p, const char* pszKhoa)
{
	KSuaGdUndo* u = SuaGd_UndoMoi(pszKhoa);

	u->nORieng = -1;
	p->GetPosition(&u->nLeft, &u->nTop);
	u->nTiLe = p->UiLayTiLe();
	u->nAn = p->UiDangAn() ? 1 : 0;
}

static void SuaGd_GhiUndoORieng(int i)
{
	KSuaGdUndo* u = SuaGd_UndoMoi(s_ORieng[i].szKhoa);
	int nCo = 0, nGoc = 0;

	u->nORieng = i;
	s_ORieng[i].pfnLay(s_ORieng[i].pNgu, &u->nLeft, &u->nTop);
	if (s_ORieng[i].pfnLayCo)
		s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
	u->nTiLe = (nCo > 0 && nCo != nGoc) ? nCo : 0;
}

static void SuaGd_GhiUndoMuc(int m)
{
	if (m < 0 || !s_O[m].bCo)
		return;
	if (s_O[m].nORieng >= 0)
		SuaGd_GhiUndoORieng(s_O[m].nORieng);
	else
		SuaGd_GhiUndoKWnd(s_O[m].pWnd, s_DS[m].szKhoa);
}

static void SuaGd_HoanTac()
{
	int nNhom;

	if (s_nUndo == 0)
	{
		DatThongBao("Kh´ng cﬂn g◊ Æ” hoµn t∏c");
		return;
	}
	nNhom = s_Undo[s_nUndo - 1].nNhom;
	while (s_nUndo > 0 && s_Undo[s_nUndo - 1].nNhom == nNhom)
	{
		KSuaGdUndo* u = &s_Undo[--s_nUndo];

		if (u->nORieng >= 0)
		{
			int i = SuaGd_TimORieng(u->szKhoa);

			if (i >= 0)
			{
				s_ORieng[i].pfnDat(s_ORieng[i].pNgu, u->nLeft, u->nTop);
				if (s_ORieng[i].pfnDatCo)
					s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu, u->nTiLe);
			}
		}
		else
		{
			KWndWindow* p = SuaGd_TimKWnd(u->szKhoa);

			if (p)
				SuaGd_ApKWnd(p, u->nLeft, u->nTop, u->nTiLe, u->nAn);
		}
		if (u->nCoBang)
		{
			int n = TimKhoa(u->szKhoa);

			if (n < 0)
				n = DatKhoa(u->szKhoa, u->oBang.nLeft, u->oBang.nTop, u->oBang.nTiLe, u->oBang.nCo);
			if (n >= 0)
				s_Bang[n] = u->oBang;
		}
		else
			SuaGd_XoaKhoa(u->szKhoa);
	}
	DatThongBao("ß∑ hoµn t∏c");
}

//---------------------------------------------------------------- 3. thao tac: keo, to nho, giau, mac dinh
static bool	s_bKeoChon = false;		// dang keo o
static KWndWindow* s_pKeoM = NULL;	// o KWnd dang keo (co the la CHA khi "ca cum")
static char	s_szKeoMKhoa[UITOADO_CO_KHOA];
static int	s_nKeoMORieng = -1;
static bool	s_bKeoDaDi = false;	// [SUAGD 13/09 g] ngon da di qua SUAGD_KEO_CHET ke tu luc cham
static int	s_nKeoNgonX = 0, s_nKeoNgonY = 0;	// cho dat ngon
static int	s_nKeoGocX = 0, s_nKeoGocY = 0;		// vi tri o luc bat dau (KWnd: tuong doi; ORieng: tam)
static int	s_nKeoAbsDX = 0, s_nKeoAbsDY = 0;	// hinh tuyet doi = vi tri + (dx, dy)
static int	s_nKeoW = 0, s_nKeoH = 0;
static int	s_nGiongX = -1, s_nGiongY = -1;		// duong giong dang bat dinh (-1 = khong)
static int	s_nPadX = -1, s_nPadY = -1;			// bang mui ten (-1 = cho mac dinh)
static bool	s_bKeoPad = false;
static int	s_nKeoPadDX = 0, s_nKeoPadDY = 0;
static int	s_nNutGiu = -1;						// nut mui ten dang giu (chay deu)
static unsigned int s_uNutGiuKe = 0;
static int	s_nBatDinh = 1;						// [Ui] BatDinh
static int	s_bChu = 0;							// [Ui] SuaToaDoChu: nut "Luu mac dinh"
static int	s_nNhatKyBoCucM = -1;				// [Ui] NhatKyBoCuc (doc mot lan)
static bool	s_bDaGhiSoLieu = false;
static unsigned int s_uVaoGameLuc = 0;

static void SuaGd_DocCauHinh()
{
	static bool s_bDaDoc = false;
	char szDuongDan[MAX_PATH];

	if (s_bDaDoc)
		return;
	s_bDaDoc = true;
	g_GetFullPath(szDuongDan, (char*)UITOADO_CAUHINH);
	s_nBatDinh = GetPrivateProfileInt("Ui", "BatDinh", 1, szDuongDan);
	s_nPhongBang = GetPrivateProfileInt("Ui", "PhongBang", 1400, szDuongDan);	// [PHONGBANG 14/09]
	s_bChu = GetPrivateProfileInt("Ui", "SuaToaDoChu", 0, szDuongDan);
	s_nNhatKyBoCucM = GetPrivateProfileInt("Ui", "NhatKyBoCuc", 0, szDuongDan);
}

//	Bat dinh: doi hinh chu nhat (tuyet doi) dang keo vao mep / tam cua o khac va mep vung an toan.
static void SuaGd_BatDinh(int m, int* pL, int* pT, int w, int h)
{
	int i, nTotX = SUAGD_BATDINH + 1, nTotY = SUAGD_BATDINH + 1, nMoiL = *pL, nMoiT = *pT;

	s_nGiongX = s_nGiongY = -1;
#define	SUAGD_THU_X(cand, giong)	{ int d = (cand) - *pL; if (d < 0) d = -d; if (d < nTotX) { nTotX = d; nMoiL = (cand); s_nGiongX = (giong); } }
#define	SUAGD_THU_Y(cand, giong)	{ int d = (cand) - *pT; if (d < 0) d = -d; if (d < nTotY) { nTotY = d; nMoiT = (cand); s_nGiongY = (giong); } }
	SUAGD_THU_X(s_nKgL, s_nKgL);
	SUAGD_THU_X(s_nKgL + s_nKgW - w, s_nKgL + s_nKgW);
	SUAGD_THU_X(s_nKgL + (s_nKgW - w) / 2, s_nKgL + s_nKgW / 2);
	SUAGD_THU_Y(s_nKgT, s_nKgT);
	SUAGD_THU_Y(s_nKgT + s_nKgH - h, s_nKgT + s_nKgH);
	SUAGD_THU_Y(s_nKgT + (s_nKgH - h) / 2, s_nKgT + s_nKgH / 2);
	for (i = 0; i < s_nDS; i++)
	{
		if (i == m || !s_O[i].bCo || !s_O[i].bHien || s_O[i].nW <= 0)
			continue;
		SUAGD_THU_X(s_O[i].nL, s_O[i].nL);
		SUAGD_THU_X(s_O[i].nL + s_O[i].nW - w, s_O[i].nL + s_O[i].nW);
		SUAGD_THU_X(s_O[i].nL + (s_O[i].nW - w) / 2, s_O[i].nL + s_O[i].nW / 2);
		SUAGD_THU_Y(s_O[i].nT, s_O[i].nT);
		SUAGD_THU_Y(s_O[i].nT + s_O[i].nH - h, s_O[i].nT + s_O[i].nH);
		SUAGD_THU_Y(s_O[i].nT + (s_O[i].nH - h) / 2, s_O[i].nT + s_O[i].nH / 2);
	}
#undef SUAGD_THU_X
#undef SUAGD_THU_Y
	*pL = nMoiL;
	*pT = nMoiT;
}

static void SuaGd_BatDauKeo(int m, int x, int y)
{
	KSuaGdO* o = &s_O[m];

	if (m < 0 || !o->bCo)
		return;
	s_nKeoNgonX = x;
	s_nKeoNgonY = y;
	s_nKeoW = o->nW;
	s_nKeoH = o->nH;
	s_bKeoChon = true;
	s_bKeoDaDi = false;	// [SUAGD 13/09 g]
	SuaGd_UndoNhomMoi();
	if (o->nORieng >= 0)
	{
		s_nKeoMORieng = o->nORieng;
		s_pKeoM = NULL;
		s_ORieng[o->nORieng].pfnLay(s_ORieng[o->nORieng].pNgu, &s_nKeoGocX, &s_nKeoGocY);
		//	hinh tuyet doi = vi tri + (dx, dy): o "tam + co" -> -co/2; o co pfnLayHinh (dong FPS neo mep phai-tren) -> hinh - vi tri
		s_nKeoAbsDX = o->nL - s_nKeoGocX;
		s_nKeoAbsDY = o->nT - s_nKeoGocY;
		SuaGd_GhiUndoORieng(o->nORieng);
	}
	else
	{
		KWndWindow* p = o->pWnd;
		int al, at, aw, ah;

		if (s_nCongCu == CONGCU_KHOI)
			p = p->GetOwner();
		if (!TaoKhoaTuOCon(p, s_szKeoMKhoa, sizeof(s_szKeoMKhoa)))
			strncpy(s_szKeoMKhoa, s_DS[m].szKhoa, sizeof(s_szKeoMKhoa) - 1);
		s_szKeoMKhoa[sizeof(s_szKeoMKhoa) - 1] = 0;
		s_pKeoM = p;
		s_nKeoMORieng = -1;
		p->GetPosition(&s_nKeoGocX, &s_nKeoGocY);
		SuaGd_HinhKWnd(p, &al, &at, &aw, &ah, (p == o->pWnd) ? s_DS[m].nCo : 0);
		s_nKeoAbsDX = al - s_nKeoGocX;
		s_nKeoAbsDY = at - s_nKeoGocY;
		s_nKeoW = aw;
		s_nKeoH = ah;
		SuaGd_GhiUndoKWnd(p, s_szKeoMKhoa);
	}
}

static void SuaGd_Keo(int x, int y)
{
	int nL = s_nKeoGocX + (x - s_nKeoNgonX);
	int nT = s_nKeoGocY + (y - s_nKeoNgonY);
	int nAbsL = nL + s_nKeoAbsDX, nAbsT = nT + s_nKeoAbsDY;

	if (s_nBatDinh && s_nCongCu != CONGCU_KHOI && s_nChon >= 0)
		SuaGd_BatDinh(s_nChon, &nAbsL, &nAbsT, s_nKeoW, s_nKeoH);
	else
		s_nGiongX = s_nGiongY = -1;
	nL = nAbsL - s_nKeoAbsDX;
	nT = nAbsT - s_nKeoAbsDY;
	if (s_nKeoMORieng >= 0)
		s_ORieng[s_nKeoMORieng].pfnDat(s_ORieng[s_nKeoMORieng].pNgu, nL, nT);
	else if (s_pKeoM)
	{
		s_pKeoM->SetPosition(nL, nT);
		s_pKeoM->UiNhoViTri();
	}
}

static void SuaGd_KetKeo()
{
	if (!s_bKeoChon)
		return;
	if (s_nKeoMORieng >= 0)
		SuaGd_GhiBangORieng(s_nKeoMORieng);
	else if (s_pKeoM && s_szKeoMKhoa[0])
		GhiLaiO(s_pKeoM, s_szKeoMKhoa);
	s_bKeoChon = false;
	s_pKeoM = NULL;
	s_nKeoMORieng = -1;
	s_nGiongX = s_nGiongY = -1;
}

//	Dich o dang chon di (dx, dy) diem - khong bat dinh (dung cho mui ten).
static void SuaGd_Dich(int m, int dx, int dy)
{
	KSuaGdO* o = &s_O[m];

	if (m < 0 || !o->bCo)
		return;
	if (o->nORieng >= 0)
	{
		int x = 0, y = 0;

		s_ORieng[o->nORieng].pfnLay(s_ORieng[o->nORieng].pNgu, &x, &y);
		s_ORieng[o->nORieng].pfnDat(s_ORieng[o->nORieng].pNgu, x + dx, y + dy);
		SuaGd_GhiBangORieng(o->nORieng);
	}
	else
	{
		int l, t;

		o->pWnd->GetPosition(&l, &t);
		o->pWnd->SetPosition(l + dx, t + dy);
		o->pWnd->UiNhoViTri();
		GhiLaiO(o->pWnd, s_DS[m].szKhoa);
	}
	o->nL += dx;
	o->nT += dy;
}

//	Phong to / thu nho MOT o (quanh tam). Cua so goc co con (icon ghep dau gia / thu / CL) thi phong ca con.
static void SuaGd_DatTiLe(int m, int nTiLe, bool bUndo)
{
	KSuaGdO* o = &s_O[m];
	char	 szChu[192];

	if (m < 0 || !o->bCo)
		return;
	if (nTiLe < SUAGD_TILE_MIN) nTiLe = SUAGD_TILE_MIN;
	if (nTiLe > SUAGD_TILE_MAX) nTiLe = SUAGD_TILE_MAX;
	if (s_DS[m].nCo & SUAGD_CO_KHONGPHONG)
	{
		DatThongBao("§ nµy kh´ng co gi∑n Æ≠Óc");
		return;
	}
	if (o->nORieng >= 0)
	{
		int i = o->nORieng, nCo = 0, nGoc = 0;

		if (s_ORieng[i].pfnLayCo == NULL || s_ORieng[i].pfnDatCo == NULL)
		{
			DatThongBao("§ nµy kh´ng co gi∑n Æ≠Óc");
			return;
		}
		if (bUndo)
		{
			SuaGd_UndoNhomMoi();
			SuaGd_GhiUndoORieng(i);
		}
		s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
		if (nGoc <= 0)
			return;
		s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu, (nTiLe == 1000) ? 0 : nGoc * nTiLe / 1000);
		SuaGd_GhiBangORieng(i);
	}
	else
	{
		KWndWindow* p = o->pWnd;
		int			nCu = p->UiLayTiLe();

		if (CamCoGian(p))
		{
			DatThongBao("§ nµy kh´ng co gi∑n Æ≠Óc");
			return;
		}
		if (nCu <= 0)
			nCu = 1000;
		if (bUndo)
			SuaGd_UndoNhomMoi();
		if (bUndo)
			SuaGd_GhiUndoKWnd(p, s_DS[m].szKhoa);
		if (p->GetParent() == NULL && p->GetFirstChild())
		{	//	icon ghep: phong ca con, con dat lai theo ti le so voi goc
			KWndWindow* pCon;

			for (pCon = p->GetFirstChild(); pCon; pCon = pCon->GetNextWnd())
			{
				char szKhoaCon[UITOADO_CO_KHOA];
				int  l, t;

				if (CamCoGian(pCon))
					continue;
				if (bUndo && TaoKhoaTuOCon(pCon, szKhoaCon, sizeof(szKhoaCon)))
					SuaGd_GhiUndoKWnd(pCon, szKhoaCon);
				pCon->GetPosition(&l, &t);
				pCon->SetPosition(l * nTiLe / nCu, t * nTiLe / nCu);
				pCon->UiNhoViTri();
				pCon->UiDatTiLe(nTiLe);
				if (TaoKhoaTuOCon(pCon, szKhoaCon, sizeof(szKhoaCon)))
					GhiLaiO(pCon, szKhoaCon);
			}
		}
		p->UiDatTiLeQuanhTam(nTiLe);
		GhiLaiO(p, s_DS[m].szKhoa);
	}
	SuaGd_LamMoi();		// hinh chu nhat cua o vua doi (ca o ve tay)
	_snprintf(szChu, sizeof(szChu), "%s  %d%%", s_DS[m].szTen, nTiLe / 10);
	szChu[sizeof(szChu) - 1] = 0;
	DatThongBao(szChu);
}

static void SuaGd_GiauHien(int m)
{
	KSuaGdO* o = &s_O[m];
	char	 szChu[192];

	if (m < 0 || !o->bCo)
		return;
	if (o->nORieng >= 0 || (s_DS[m].nCo & SUAGD_CO_KHONGAN))
	{
		DatThongBao("§ nµy kh´ng gi u Æ≠Óc");
		return;
	}
	if (o->pWnd->UiDangAn() == 0 && CamAn(o->pWnd))
	{
		DatThongBao("Kh´ng cho gi u nÛt Æ„ng / hu˚");
		return;
	}
	SuaGd_UndoNhomMoi();
	SuaGd_GhiUndoKWnd(o->pWnd, s_DS[m].szKhoa);
	o->pWnd->UiDatAn(o->pWnd->UiDangAn() ? 0 : 1);
	GhiLaiO(o->pWnd, s_DS[m].szKhoa);
	_snprintf(szChu, sizeof(szChu), "%s  %s", s_DS[m].szTen, o->pWnd->UiDangAn() ? "Æ∑ gi u" : "hi÷n lπi");
	szChu[sizeof(szChu) - 1] = 0;
	DatThongBao(szChu);
}

//	Tra MOT o ve mac dinh cua game (lop mac dinh), khong co thi ve toa do goc trong ini / trong ma.
static void SuaGd_MacDinh(int m)
{
	KSuaGdO*	o = &s_O[m];
	const char*	pszKhoa = s_DS[m].szKhoa;
	int			nMD = UiToaDoM_TimMacDinh(pszKhoa);
	char		szChu[192];

	if (m < 0 || !o->bCo)
		return;
	SuaGd_UndoNhomMoi();
	if (o->nORieng >= 0)
	{
		int i = o->nORieng;

		SuaGd_GhiUndoORieng(i);
		if (nMD >= 0)
		{
			s_ORieng[i].pfnDat(s_ORieng[i].pNgu, s_BangMacDinh[nMD].nLeft, s_BangMacDinh[nMD].nTop);
			if (s_ORieng[i].pfnDatCo)
			{
				int nCo = 0, nGoc = 0;

				s_ORieng[i].pfnLayCo(s_ORieng[i].pNgu, &nCo, &nGoc);
				s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu,
					(s_BangMacDinh[nMD].nTiLe > 0 && s_BangMacDinh[nMD].nTiLe != 1000 && nGoc > 0)
						? nGoc * s_BangMacDinh[nMD].nTiLe / 1000 : 0);
			}
			{
				int n = DatKhoa(pszKhoa, s_BangMacDinh[nMD].nLeft, s_BangMacDinh[nMD].nTop, s_BangMacDinh[nMD].nTiLe, 0);

				if (n >= 0)
				{
					s_Bang[n].nNeoX = s_BangMacDinh[nMD].nNeoX;
					s_Bang[n].nNeoY = s_BangMacDinh[nMD].nNeoY;
				}
			}
		}
		else
		{
			s_ORieng[i].pfnDat(s_ORieng[i].pNgu, s_ORieng[i].nGocX, s_ORieng[i].nGocY);
			if (s_ORieng[i].pfnDatCo)
				s_ORieng[i].pfnDatCo(s_ORieng[i].pNgu, 0);
			SuaGd_XoaKhoa(pszKhoa);
		}
	}
	else
	{
		KWndWindow* p = o->pWnd;

		SuaGd_GhiUndoKWnd(p, pszKhoa);
		if (nMD >= 0)
		{
			int n;

			SuaGd_ApKWnd(p, s_BangMacDinh[nMD].nLeft, s_BangMacDinh[nMD].nTop, s_BangMacDinh[nMD].nTiLe,
				s_BangMacDinh[nMD].nCo & UITOADO_CO_AN);
			n = DatKhoa(pszKhoa, s_BangMacDinh[nMD].nLeft, s_BangMacDinh[nMD].nTop, s_BangMacDinh[nMD].nTiLe, s_BangMacDinh[nMD].nCo);
			if (n >= 0)
			{
				s_Bang[n].nNeoX = s_BangMacDinh[nMD].nNeoX;
				s_Bang[n].nNeoY = s_BangMacDinh[nMD].nNeoY;
			}
		}
		else
		{
			int g = SuaGd_TimGoc(pszKhoa);

			if (g >= 0)
				SuaGd_ApKWnd(p, s_Goc[g].nLeft, s_Goc[g].nTop, 1000, 0);
			else
			{
				int l, t;

				p->GetPosition(&l, &t);		// khong biet goc: giu cho, chi tra ti le / giau
				SuaGd_ApKWnd(p, l, t, 1000, 0);
			}
			SuaGd_XoaKhoa(pszKhoa);
		}
	}
	SuaGd_LamMoi();
	_snprintf(szChu, sizeof(szChu), "%s  %s", s_DS[m].szTen, "v“ m∆c Æﬁnh");
	szChu[sizeof(szChu) - 1] = 0;
	DatThongBao(szChu);
}

//---------------------------------------------------------------- luot kep vao vung an toan
static int SuaGd_Kep(bool bGhiBang)
{
	int i, nDem = 0, kgL, kgT, kgW, kgH;

	UiToaDoM_KhongGian(&kgL, &kgT, &kgW, &kgH);
	SuaGd_LamMoi();
	for (i = 0; i < s_nDS; i++)
	{
		KSuaGdO* o = &s_O[i];
		int		 dx = 0, dy = 0;

		if (!o->bCo || !o->bHien || (s_DS[i].nCo & SUAGD_CO_KHONGKEP))
			continue;
		if (o->nW <= 0 || o->nH <= 0 || o->nW > kgW || o->nH > kgH)
			continue;
		if (o->nL < kgL)						dx = kgL - o->nL;
		else if (o->nL + o->nW > kgL + kgW)	dx = kgL + kgW - (o->nL + o->nW);
		if (o->nT < kgT)						dy = kgT - o->nT;
		else if (o->nT + o->nH > kgT + kgH)	dy = kgT + kgH - (o->nT + o->nH);
		if (dx == 0 && dy == 0)
			continue;
		if (o->nORieng >= 0)
		{
			int x = 0, y = 0;

			s_ORieng[o->nORieng].pfnLay(s_ORieng[o->nORieng].pNgu, &x, &y);
			s_ORieng[o->nORieng].pfnDat(s_ORieng[o->nORieng].pNgu, x + dx, y + dy);
			if (bGhiBang)
				SuaGd_GhiBangORieng(o->nORieng);
		}
		else
		{
			int l, t;

			o->pWnd->GetPosition(&l, &t);
			o->pWnd->SetPosition(l + dx, t + dy);
			o->pWnd->UiNhoViTri();
			if (bGhiBang)
				GhiLaiO(o->pWnd, s_DS[i].szKhoa);
		}
		o->nL += dx;
		o->nT += dy;
		g_DebugLog("[SUAGD] kep %s vao vung an toan %d,%d %dx%d: dich %d,%d", s_DS[i].szKhoa, kgL, kgT, kgW, kgH, dx, dy);
		nDem++;
	}
	return nDem;
}

//---------------------------------------------------------------- so lieu may that (bocuc_thuc.txt)
static void SuaGd_GhiSoLieu()
{
	char	szTep[MAX_PATH];
	FILE*	f;
	int		i, kgL, kgT, kgW, kgH;

	UiToaDoM_KhongGian(&kgL, &kgT, &kgW, &kgH);
	SuaGd_LamMoi();
	g_GetFullPath(szTep, (char*)SUAGD_TEP_SOLIEU);
	f = fopen(szTep, "w");
	if (f == NULL)
		return;
	fprintf(f, "khung ve %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
	fprintf(f, "ho khung %d (1 = dien thoai cao co dinh, 2 = may tinh bang rong co dinh)\n", UiToaDoM_HoKhung());
	fprintf(f, "vung an toan %d,%d %dx%d (inset trai %d phai %d tren %d duoi %d)\n", kgL, kgT, kgW, kgH,
		kgL, SCREEN_WIDTH - kgL - kgW, kgT, SCREEN_HEIGHT - kgT - kgH);
	fprintf(f, "nhan vat %u, tep bo cuc %s, bang %d muc\n", UiToaDoM_MaNhanVat(), s_szTepNguoiChoi[0] ? s_szTepNguoiChoi : UITOADO_TEP, s_nSo);
	fprintf(f, "\n[DanhSachTrang] khoa = abs L,T WxH hien\n");
	for (i = 0; i < s_nDS; i++)
	{
		if (!s_O[i].bCo)
			continue;
		fprintf(f, "%s = %d,%d %dx%d %s%s%s\n", s_DS[i].szKhoa, s_O[i].nL, s_O[i].nT, s_O[i].nW, s_O[i].nH,
			s_O[i].bHien ? "hien" : "an",
			(s_O[i].nL < kgL || s_O[i].nT < kgT || s_O[i].nL + s_O[i].nW > kgL + kgW || s_O[i].nT + s_O[i].nH > kgT + kgH) ? " NGOAI-VUNG-AN-TOAN" : "",
			s_O[i].nORieng >= 0 ? " (ve tay)" : "");
	}
	fprintf(f, "\n[CayCuaSo] khoa = abs x,y rel x,y WxH\n");
	s_fDumpThuc = f;
	DumpCayThuc(Wnd_GetLayerRoot(WL_LOWEST)->GetNextWnd());
	DumpCayThuc(Wnd_GetLayerRoot(WL_NORMAL)->GetNextWnd());
	DumpCayThuc(Wnd_GetLayerRoot(WL_TOPMOST)->GetNextWnd());
	s_fDumpThuc = NULL;
	fclose(f);
	g_DebugLog("[SUAGD] da ghi so lieu bo cuc: %s", szTep);
}

//---------------------------------------------------------------- 4. BANG CHINH - mot bang noi, keo duoc, thu gon duoc
//	Chu 13/09: "lam lai giao dien chinh cho gon de khong che het cac icon". Truoc: thanh nut kin be ngang + dai chu
//	+ bang mui ten rieng -> che hang icon tren. Nay MOT bang 225 px, mac dinh nam GIUA man (vung the gioi, khong co
//	HUD o do), keo bang tieu de; nut "gon" thu lai chi con hai hang nut. Bo cuc:
//	  tieu de   | Chinh giao dien - keo bang                 [gon]
//	  thong tin | <ten o>  x,y  WxH  %
//	  hang A    | [Doi o / Doi cum] [Giau] [Goc o] [Hoan tac]
//	  bang mui ten 4 cot x 3 hang (an khi gon): (1,0) len (0,1) trai (2,1) phai (1,2) xuong (3,0) + (3,1) % (3,2) -
//	  dong so   | 1060x616 DT | an toan 0,0 1060x616         (an khi gon)
//	  hang B    | [Xoa het] [Luu MD - chi chu] [Xong - luu]
enum { SG_NUT_CHEDO = 0, SG_NUT_GIAU, SG_NUT_MACDINH, SG_NUT_HOANTAC, SG_NUT_XOAHET, SG_NUT_LUUMD, SG_NUT_XONG, SG_NUT_GON, SG_NUT_SO };
enum { SG_PAD_LEN = 0, SG_PAD_XUONG, SG_PAD_TRAI, SG_PAD_PHAI, SG_PAD_TO, SG_PAD_NHO, SG_PAD_SO };
#define	SG_TRUNG_TIEUDE		(-2)
#define	SG_TRUNG_THAN		(-3)
#define	SG_TRUNG_PAD		100

static bool	s_bPadGon = false;

static int SuaGd_BangRong()
{
	return SUAGD_PAD_CACH + 4 * (SUAGD_PAD_O + SUAGD_PAD_CACH);
}

static int SuaGd_BangCao()
{
	int h = SUAGD_PAD_TIEUDE + SUAGD_PAD_INFO + SUAGD_PAD_CACH + SUAGD_PAD_NUT + SUAGD_PAD_CACH;	// tieu de + thong tin + hang A

	if (!s_bPadGon)
		h += 3 * (SUAGD_PAD_O + SUAGD_PAD_CACH) + SUAGD_PAD_INFO + SUAGD_PAD_CACH;	// bang mui ten + dong so
	h += SUAGD_PAD_NUT + SUAGD_PAD_CACH;	// hang B
	return h;
}

static void SuaGd_BangGoc(int* px, int* py, int* pw, int* ph)
{
	*pw = SuaGd_BangRong();
	*ph = SuaGd_BangCao();
	if (s_nPadX < 0 || s_nPadY < 0)
	{	// mac dinh: giua man (vung the gioi), khong de len HUD o cac mep
		s_nPadX = (SCREEN_WIDTH - *pw) / 2;
		s_nPadY = (SCREEN_HEIGHT - *ph) / 2;
	}
	if (s_nPadX > SCREEN_WIDTH - *pw)	s_nPadX = SCREEN_WIDTH - *pw;
	if (s_nPadY > SCREEN_HEIGHT - *ph)	s_nPadY = SCREEN_HEIGHT - *ph;
	if (s_nPadX < 0) s_nPadX = 0;
	if (s_nPadY < 0) s_nPadY = 0;
	*px = s_nPadX;
	*py = s_nPadY;
}

static int SuaGd_YHangA(int gy)	{ return gy + SUAGD_PAD_TIEUDE + SUAGD_PAD_INFO + SUAGD_PAD_CACH; }
static int SuaGd_YPad(int gy)	{ return SuaGd_YHangA(gy) + SUAGD_PAD_NUT + SUAGD_PAD_CACH; }
static int SuaGd_YDongSo(int gy)	{ return SuaGd_YPad(gy) + 3 * (SUAGD_PAD_O + SUAGD_PAD_CACH); }
static int SuaGd_YHangB(int gy)	{ return s_bPadGon ? SuaGd_YPad(gy) : (SuaGd_YDongSo(gy) + SUAGD_PAD_INFO + SUAGD_PAD_CACH); }

static int SuaGd_DanhSachNutA(int* aNut)
{
	aNut[0] = SG_NUT_CHEDO;
	aNut[1] = SG_NUT_GIAU;
	aNut[2] = SG_NUT_MACDINH;
	aNut[3] = SG_NUT_HOANTAC;
	return 4;
}

static int SuaGd_DanhSachNutB(int* aNut)
{
	int n = 0;

	aNut[n++] = SG_NUT_XOAHET;
	if (s_bChu)
		aNut[n++] = SG_NUT_LUUMD;
	aNut[n++] = SG_NUT_XONG;
	return n;
}

static const char* SuaGd_TenNut(int nMa)
{
	switch (nMa)
	{
	case SG_NUT_CHEDO:		return (s_nCongCu == CONGCU_KHOI) ? "DÍi cÙm" : "DÍi ´";
	case SG_NUT_GIAU:
		if (s_nChon >= 0 && s_O[s_nChon].bCo)
			return (s_O[s_nChon].pWnd && s_O[s_nChon].pWnd->UiDangAn()) ? "Hi÷n lπi" : "Gi u";
		return s_bHienOAn ? "C t ´ gi u" : "§ Æ∑ gi u";	// [SUAGD 13/09 e] khong chon o: bat / tat "hien cac o da giau"
	case SG_NUT_MACDINH:	return "GËc ´";
	case SG_NUT_HOANTAC:	return "Hoµn t∏c";
	case SG_NUT_XOAHET:		return "Xo∏ h’t";
	case SG_NUT_LUUMD:		return "L≠u Mß";
	case SG_NUT_XONG:		return "Xong - l≠u";
	case SG_NUT_GON:		return s_bPadGon ? "ÆÒ" : "g‰n";
	}
	return "";
}

//	Hinh chu nhat cua mot nut chu theo ma (w = 0 neu nut khong hien).
static void SuaGd_NutHinh(int nMa, int* px, int* py, int* pw, int* ph)
{
	int gx, gy, gw, gh, aNut[4], n, k;

	SuaGd_BangGoc(&gx, &gy, &gw, &gh);
	*pw = *ph = 0;
	*px = *py = 0;
	if (nMa == SG_NUT_GON)
	{
		*pw = SUAGD_PAD_GON_RONG;
		*ph = SUAGD_PAD_TIEUDE - 4;
		*px = gx + gw - *pw - 3;
		*py = gy + 2;
		return;
	}
	n = SuaGd_DanhSachNutA(aNut);
	for (k = 0; k < n; k++)
		if (aNut[k] == nMa)
		{
			*pw = (gw - (n + 1) * SUAGD_PAD_CACH) / n;
			*ph = SUAGD_PAD_NUT;
			*px = gx + SUAGD_PAD_CACH + k * (*pw + SUAGD_PAD_CACH);
			*py = SuaGd_YHangA(gy);
			return;
		}
	n = SuaGd_DanhSachNutB(aNut);
	for (k = 0; k < n; k++)
		if (aNut[k] == nMa)
		{
			*pw = (gw - (n + 1) * SUAGD_PAD_CACH) / n;
			*ph = SUAGD_PAD_NUT;
			*px = gx + SUAGD_PAD_CACH + k * (*pw + SUAGD_PAD_CACH);
			*py = SuaGd_YHangB(gy);
			return;
		}
}

static void SuaGd_PadO(int nCot, int nHang, int* px, int* py)
{
	int gx, gy, gw, gh;

	SuaGd_BangGoc(&gx, &gy, &gw, &gh);
	*px = gx + SUAGD_PAD_CACH + nCot * (SUAGD_PAD_O + SUAGD_PAD_CACH);
	*py = SuaGd_YPad(gy) + nHang * (SUAGD_PAD_O + SUAGD_PAD_CACH);
}

static void SuaGd_PadNutHinh(int nMa, int* px, int* py)
{
	static const int aCot[SG_PAD_SO] = { 1, 1, 0, 2, 3, 3 };
	static const int aHang[SG_PAD_SO] = { 0, 2, 1, 1, 0, 2 };

	SuaGd_PadO(aCot[nMa], aHang[nMa], px, py);
}

//	-1 = ngoai bang; 0..SG_NUT_SO-1 = nut chu; SG_TRUNG_PAD + i = nut mui ten; SG_TRUNG_TIEUDE = keo bang; SG_TRUNG_THAN = than bang
static int SuaGd_BangTrung(int x, int y)
{
	int gx, gy, gw, gh, i;

	SuaGd_BangGoc(&gx, &gy, &gw, &gh);
	if (x < gx || y < gy || x >= gx + gw || y >= gy + gh)
		return -1;
	for (i = 0; i < SG_NUT_SO; i++)
	{
		int nx, ny, nw, nh;

		SuaGd_NutHinh(i, &nx, &ny, &nw, &nh);
		if (nw > 0 && x >= nx && y >= ny && x < nx + nw && y < ny + nh)
			return i;
	}
	if (!s_bPadGon)
	{
		for (i = 0; i < SG_PAD_SO; i++)
		{
			int nx, ny;

			SuaGd_PadNutHinh(i, &nx, &ny);
			if (x >= nx && y >= ny && x < nx + SUAGD_PAD_O && y < ny + SUAGD_PAD_O)
				return SG_TRUNG_PAD + i;
		}
	}
	if (y < gy + SUAGD_PAD_TIEUDE)
		return SG_TRUNG_TIEUDE;
	return SG_TRUNG_THAN;
}

static void SuaGd_BamNut(int nMa)
{
	switch (nMa)
	{
	case SG_NUT_CHEDO:
		s_nCongCu = (s_nCongCu == CONGCU_KHOI) ? CONGCU_DOI : CONGCU_KHOI;
		DatThongBao((s_nCongCu == CONGCU_KHOI) ? "K–o = dÍi c∂ cÙm: khung cha ho∆c c∂ cÙm k¸ n®ng" : "K–o = dÍi tıng ´");
		break;
	case SG_NUT_GIAU:
		if (s_nChon < 0 || !s_O[s_nChon].bCo)
		{	//	[SUAGD 13/09 e] khong chon o: bat / tat "hien cac o da giau" (khung do) de chon roi bam Hien lai
			s_bHienOAn = !s_bHienOAn;
			SuaGd_LamMoi();
			DatThongBao(s_bHienOAn ? "ßang hi÷n c∏c ´ Æ∑ gi u - khung Æ·: chπm ´ rÂi b m Hi÷n lπi" : "ß∑ c t c∏c ´ Æ∑ gi u");
		}
		else
		{
			SuaGd_GiauHien(s_nChon);
			SuaGd_LamMoi();
		}
		if (!s_bHienOAn && s_nChon >= 0 && s_O[s_nChon].bCo && s_O[s_nChon].pWnd && s_O[s_nChon].pWnd->UiDangAn())
			SuaGd_Chon(-1);	//	o vua giau bi cat khoi man -> bo chon
		break;
	case SG_NUT_MACDINH:
		if (s_nChon < 0 || !s_O[s_nChon].bCo)
			DatThongBao("Chπm vµo mÈt ´ tr≠Ìc");
		else
			SuaGd_MacDinh(s_nChon);
		break;
	case SG_NUT_HOANTAC:
		SuaGd_HoanTac();
		SuaGd_LamMoi();
		break;
	case SG_NUT_XOAHET:
		SuaGd_Chon(-1);
		s_nUndo = 0;
		UiToaDo_XoaHet();
		SuaGd_LamMoi();
		break;
	case SG_NUT_LUUMD:
		if (GhiTepVao((UiToaDoM_HoKhung() == 1 && UiToaDoM_CoTep(UITOADO_TEP_MACDINH_RONG))
				? UITOADO_TEP_MACDINH_RONG : UITOADO_TEP_MACDINH) >= 0)
		{
			UiToaDoM_SauMacDinh();
			DatThongBao("ß∑ l≠u bË cÙc hi÷n tπi lµm m∆c Æﬁnh cÒa game cho h‰ khung nµy");
		}
		else
			DatThongBao("LÁi: kh´ng ghi Æ≠Óc t÷p m∆c Æﬁnh");
		break;
	case SG_NUT_XONG:
		UiToaDo_BatTat();
		break;
	case SG_NUT_GON:
		s_bPadGon = !s_bPadGon;
		break;
	}
}

static void SuaGd_BamPad(int nMa)
{
	if (s_nChon < 0 || !s_O[s_nChon].bCo)
	{
		DatThongBao("Chπm vµo mÈt ´ tr≠Ìc rÂi mÌi chÿnh");
		return;
	}
	switch (nMa)
	{
	case SG_PAD_LEN:	SuaGd_Dich(s_nChon, 0, -1); break;
	case SG_PAD_XUONG:	SuaGd_Dich(s_nChon, 0, 1); break;
	case SG_PAD_TRAI:	SuaGd_Dich(s_nChon, -1, 0); break;
	case SG_PAD_PHAI:	SuaGd_Dich(s_nChon, 1, 0); break;
	case SG_PAD_TO:		SuaGd_DatTiLe(s_nChon, SuaGd_TiLeCua(s_nChon) + UITOADO_TILE_NAC, false); break;
	case SG_PAD_NHO:	SuaGd_DatTiLe(s_nChon, SuaGd_TiLeCua(s_nChon) - UITOADO_TILE_NAC, false); break;
	}
}

//---------------------------------------------------------------- 5. nhan chuot / cham
static bool UiToaDoM_NhanChuot(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int x, y;

	if (s_bDangSua == false)
	{
		if (uMsg != WM_LBUTTONDOWN)
			return false;
		Wnd_GetCursorPos(&x, &y);
		if (TrongNutMo(x, y) == false)
			return false;	// [SUAGD 13/09 d] mo duoc o moi man (ca dang nhap)
		UiToaDo_BatTat();
		return true;
	}
	Wnd_GetCursorPos(&x, &y);
	if (GetTickCount() - s_uLamMoiLuc > 100)
		SuaGd_LamMoi();

	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		{	//	may tinh / may ao: lan chuot = to nho o dang chon (hoac o duoi con tro)
			int nDelta = (int)(short)HIWORD(uParam);
			int m = SuaGd_TimODuoi(x, y);

			if (m < 0)
				m = s_nChon;
			if (m >= 0 && s_O[m].bCo)
			{
				SuaGd_Chon(m);
				SuaGd_DatTiLe(m, SuaGd_TiLeCua(m) + ((nDelta > 0) ? UITOADO_TILE_NAC : -UITOADO_TILE_NAC), true);
			}
		}
		break;

	case WM_MBUTTONDOWN:
		{
			int m = SuaGd_TimODuoi(x, y);

			if (m >= 0)
			{
				SuaGd_Chon(m);
				SuaGd_GiauHien(m);
			}
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			int nTrung, m;

			s_nGiongX = s_nGiongY = -1;
			nTrung = SuaGd_BangTrung(x, y);
			if (nTrung >= SG_TRUNG_PAD)
			{
				int nPad = nTrung - SG_TRUNG_PAD;

				if (s_nChon >= 0 && s_O[s_nChon].bCo)
				{
					SuaGd_UndoNhomMoi();
					SuaGd_GhiUndoMuc(s_nChon);
				}
				s_nNutGiu = nPad;
				s_uNutGiuKe = GetTickCount() + SUAGD_LAP_CHO;
				SuaGd_BamPad(nPad);
				break;
			}
			if (nTrung >= 0)
			{
				SuaGd_BamNut(nTrung);
				break;
			}
			if (nTrung == SG_TRUNG_TIEUDE)
			{
				int gx, gy, gw, gh;

				SuaGd_BangGoc(&gx, &gy, &gw, &gh);
				s_bKeoPad = true;
				s_nKeoPadDX = x - gx;
				s_nKeoPadDY = y - gy;
				break;
			}
			if (nTrung == SG_TRUNG_THAN)
				break;
			//	"ca cum" voi o ve tay: cum ky nang (TrungCum chi bat khi dang CONGCU_KHOI)
			if (s_nCongCu == CONGCU_KHOI)
			{
				int k;

				for (k = 0; k < s_nSoORieng; k++)
				{
					if (s_ORieng[k].pfnTrung(s_ORieng[k].pNgu, x, y))
					{
						s_bKeoChon = true;
						s_bKeoDaDi = false;	// [SUAGD 13/09 g]
						s_nKeoMORieng = k;
						s_pKeoM = NULL;
						s_nKeoNgonX = x;
						s_nKeoNgonY = y;
						s_ORieng[k].pfnLay(s_ORieng[k].pNgu, &s_nKeoGocX, &s_nKeoGocY);
						s_nKeoAbsDX = s_nKeoAbsDY = 0;
						SuaGd_UndoNhomMoi();
						SuaGd_GhiUndoORieng(k);
						break;
					}
				}
				if (k < s_nSoORieng)
					break;
			}
			m = SuaGd_TimODuoi(x, y);
			if (m < 0 && s_nChon >= 0 && s_O[s_nChon].bCo && s_O[s_nChon].bHien)
				m = s_nChon;	//	keo o dang chon tu bat ky dau (icon nho hon dau ngon)
			if (m < 0)
			{
				DatThongBao("Chπm vµo mÈt icon Æ” ch‰n n„");
				break;
			}
			SuaGd_Chon(m);
			SuaGd_BatDauKeo(m, x, y);
		}
		break;

	case WM_MOUSEMOVE:
		if (s_bKeoPad)
		{
			s_nPadX = x - s_nKeoPadDX;
			s_nPadY = y - s_nKeoPadDY;
			break;
		}
		if (s_bKeoChon)
		{	//	[SUAGD 13/09 g] cham suong: chua di qua nguong thi khong doi, khong bat dinh (truoc day cham la o nhich toi 6 diem)
			int ddx = x - s_nKeoNgonX, ddy = y - s_nKeoNgonY;

			if (!s_bKeoDaDi && ddx * ddx + ddy * ddy < SUAGD_KEO_CHET * SUAGD_KEO_CHET)
				break;
			s_bKeoDaDi = true;
			SuaGd_Keo(x, y);
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		s_nNutGiu = -1;
		s_bKeoPad = false;
		if (s_bKeoChon)
		{
			SuaGd_KetKeo();
			SuaGd_LamMoi();
		}
		s_nGiongX = s_nGiongY = -1;
		break;
	}
	return true;
}

//	Chum hai ngon (KSdlApp): to nho o dang chon theo khoang cach hai ngon, lam tron 5 %.
static int		s_nChumMuc = -1;
static float	s_fChumD0 = 1.0f;
static int		s_nChumTiLe0 = 1000;

int UiToaDo_ChumBatDau(int x1, int y1, int x2, int y2)
{
	float dx = (float)(x2 - x1), dy = (float)(y2 - y1);

	if (!s_bDangSua || s_nChon < 0 || !s_O[s_nChon].bCo)
		return 0;
	if (s_bKeoChon)
	{
		SuaGd_KetKeo();
		SuaGd_LamMoi();
	}
	s_nNutGiu = -1;
	s_fChumD0 = (float)sqrt((double)(dx * dx + dy * dy));
	if (s_fChumD0 < 10.0f)
		s_fChumD0 = 10.0f;
	s_nChumTiLe0 = SuaGd_TiLeCua(s_nChon);
	s_nChumMuc = s_nChon;
	SuaGd_UndoNhomMoi();
	SuaGd_GhiUndoMuc(s_nChon);
	return 1;
}

void UiToaDo_ChumKeo(int x1, int y1, int x2, int y2)
{
	float dx = (float)(x2 - x1), dy = (float)(y2 - y1);
	float d;
	int	  nTiLe;

	if (s_nChumMuc < 0 || !s_bDangSua)
		return;
	d = (float)sqrt((double)(dx * dx + dy * dy));
	nTiLe = (int)((float)s_nChumTiLe0 * d / s_fChumD0 + 0.5f);
	nTiLe = (nTiLe + UITOADO_TILE_NAC / 2) / UITOADO_TILE_NAC * UITOADO_TILE_NAC;
	if (nTiLe < SUAGD_TILE_MIN) nTiLe = SUAGD_TILE_MIN;
	if (nTiLe > SUAGD_TILE_MAX) nTiLe = SUAGD_TILE_MAX;
	if (nTiLe != SuaGd_TiLeCua(s_nChumMuc))
		SuaGd_DatTiLe(s_nChumMuc, nTiLe, false);
}

void UiToaDo_ChumNha()
{
	s_nChumMuc = -1;
	SuaGd_LamMoi();
}

//---------------------------------------------------------------- 6. ve
static void UiToaDoM_Ve()
{
	char szChu[256];
	int  i;

	if (g_pRepresentShell == NULL)
		return;
	if (s_bDangSua == false)
	{
		if (UiToaDo_ChoPhep())
		{
			int nX, nY, nW, nH;

			NutMoHinh(&nX, &nY, &nW, &nH);
			OKhoi(nX, nY, nX + nW, nY + nH, 0xA0202020);
			SuaGd_VeChuCo(12, "Chÿnh giao di÷n", nX + 6, nY + 8, SUAGD_MAU_VANG);
		}
		if (s_szThongBao[0] && GetTickCount() < s_uHetThongBao)
			SuaGd_VeChuCo(12, s_szThongBao, 8, 60, 0xFFFF8080);
		else
			s_szThongBao[0] = 0;
		return;
	}
	if (GetTickCount() - s_uLamMoiLuc > 100)
		SuaGd_LamMoi();
	SuaGd_DocCauHinh();

	//	vung an toan
	UiToaDoM_KhongGian(NULL, NULL, NULL, NULL);
	if (s_nKgL > 0 || s_nKgT > 0 || s_nKgW < SCREEN_WIDTH || s_nKgH < SCREEN_HEIGHT)
		SuaGd_VeKhung(s_nKgL, s_nKgT, s_nKgL + s_nKgW - 1, s_nKgT + s_nKgH - 1, SUAGD_MAU_ANTOAN);

	//	khung moi o trong danh sach trang; o dang chon: vang + nen mo
	for (i = 0; i < s_nDS; i++)
	{
		unsigned int uMau;

		if (!s_O[i].bCo || !s_O[i].bHien || s_O[i].nW <= 0)
			continue;
		if (i == s_nChon)
		{
			OKhoi(s_O[i].nL, s_O[i].nT, s_O[i].nL + s_O[i].nW, s_O[i].nT + s_O[i].nH, SUAGD_MAU_CHON_NEN);
			uMau = SUAGD_MAU_CHON;
		}
		else
			uMau = (s_O[i].pWnd && s_O[i].pWnd->UiDangAn()) ? SUAGD_MAU_KHUNG_AN : SUAGD_MAU_KHUNG;
		SuaGd_VeKhung(s_O[i].nL, s_O[i].nT, s_O[i].nL + s_O[i].nW - 1, s_O[i].nT + s_O[i].nH - 1, uMau);
		if (i == s_nChon)
			SuaGd_VeKhung(s_O[i].nL - 1, s_O[i].nT - 1, s_O[i].nL + s_O[i].nW, s_O[i].nT + s_O[i].nH, uMau);
	}
	//	duong giong khi bat dinh
	if (s_bKeoChon && s_nGiongX >= 0)
		SuaGd_VeDuong(s_nGiongX, 0, s_nGiongX, SCREEN_HEIGHT - 1, SUAGD_MAU_GIONG);
	if (s_bKeoChon && s_nGiongY >= 0)
		SuaGd_VeDuong(0, s_nGiongY, SCREEN_WIDTH - 1, s_nGiongY, SUAGD_MAU_GIONG);

	//	BANG CHINH (mot bang noi, keo duoc, thu gon duoc - xem muc 4)
	{
		int gx, gy, gw, gh, k, n, aNut[4], nx, ny, nw, nh;
		static const char* s_aChuPad[SG_PAD_SO] = { "^", "v", "<", ">", "+", "-" };
		bool bChon = (s_nChon >= 0 && s_O[s_nChon].bCo);

		SuaGd_BangGoc(&gx, &gy, &gw, &gh);
		OKhoi(gx, gy, gx + gw, gy + gh, SUAGD_MAU_NEN);
		SuaGd_VeKhung(gx, gy, gx + gw - 1, gy + gh - 1, 0x80FFFFFF);
		//	tieu de (keo bang) + nut gon/du
		OKhoi(gx, gy, gx + gw, gy + SUAGD_PAD_TIEUDE, 0xD0303848);
		SuaGd_VeChuCo(16, "Chÿnh giao di÷n - k–o b∂ng", gx + 6, gy + 4, SUAGD_MAU_VANG);
		SuaGd_NutHinh(SG_NUT_GON, &nx, &ny, &nw, &nh);
		SuaGd_VeNut(nx, ny, nw, nh, SuaGd_TenNut(SG_NUT_GON), false, false);
		//	dong thong tin: o dang chon
		if (bChon)
		{
			_snprintf(szChu, sizeof(szChu), "%.18s  %d,%d  %dx%d  %d%%%s", s_DS[s_nChon].szTen,
				s_O[s_nChon].nL, s_O[s_nChon].nT, s_O[s_nChon].nW, s_O[s_nChon].nH, SuaGd_TiLeCua(s_nChon) / 10,
				(s_O[s_nChon].pWnd && s_O[s_nChon].pWnd->UiDangAn()) ? " - gi u" : "");
			szChu[sizeof(szChu) - 1] = 0;
			SuaGd_VeChuCo(12, szChu, gx + 6, gy + SUAGD_PAD_TIEUDE + 3, 0xFF80FF80);
		}
		else
			SuaGd_VeChuCo(12, "Chπm mÈt icon Æ” ch‰n, k–o Æ” dÍi", gx + 6, gy + SUAGD_PAD_TIEUDE + 2, SUAGD_MAU_CHU_MO);
		//	hang A
		n = SuaGd_DanhSachNutA(aNut);
		for (k = 0; k < n; k++)
		{
			SuaGd_NutHinh(aNut[k], &nx, &ny, &nw, &nh);
			SuaGd_VeNut(nx, ny, nw, nh, SuaGd_TenNut(aNut[k]),
				(aNut[k] == SG_NUT_CHEDO && s_nCongCu == CONGCU_KHOI) || (aNut[k] == SG_NUT_GIAU && s_bHienOAn && !bChon),
				(aNut[k] == SG_NUT_HOANTAC && s_nUndo == 0) || (aNut[k] == SG_NUT_MACDINH && !bChon));	// [SUAGD 13/09 e] Giau luon bam duoc
		}
		if (!s_bPadGon)
		{	//	bang mui ten + dong so
			for (k = 0; k < SG_PAD_SO; k++)
			{
				SuaGd_PadNutHinh(k, &nx, &ny);
				SuaGd_VeNut(nx, ny, SUAGD_PAD_O, SUAGD_PAD_O, s_aChuPad[k], s_nNutGiu == k, !bChon);
			}
			SuaGd_PadO(1, 1, &nx, &ny);
			SuaGd_VeChuGiua(12, "1 Æi”m", nx, ny, SUAGD_PAD_O, SUAGD_PAD_O, SUAGD_MAU_CHU_MO);
			SuaGd_PadO(3, 1, &nx, &ny);
			if (bChon)
				_snprintf(szChu, sizeof(szChu), "%d%%", SuaGd_TiLeCua(s_nChon) / 10);
			else
				strcpy(szChu, "--");
			SuaGd_VeChuGiua(16, szChu, nx, ny, SUAGD_PAD_O, SUAGD_PAD_O, SUAGD_MAU_CHU);
			_snprintf(szChu, sizeof(szChu), "%dx%d %s | %s %d,%d %dx%d", SCREEN_WIDTH, SCREEN_HEIGHT,
				(UiToaDoM_HoKhung() == 1) ? "ßT" : "b∂ng", "an toµn", s_nKgL, s_nKgT, s_nKgW, s_nKgH);
			szChu[sizeof(szChu) - 1] = 0;
			SuaGd_VeChuCo(12, szChu, gx + 6, SuaGd_YDongSo(gy) + 1, SUAGD_MAU_CHU_MO);
		}
		//	hang B
		n = SuaGd_DanhSachNutB(aNut);
		for (k = 0; k < n; k++)
		{
			SuaGd_NutHinh(aNut[k], &nx, &ny, &nw, &nh);
			SuaGd_VeNut(nx, ny, nw, nh, SuaGd_TenNut(aNut[k]), false, false);
		}
		//	thong bao: ngay duoi bang (khong du cho thi tren bang)
		if (s_szThongBao[0] && GetTickCount() < s_uHetThongBao)
			SuaGd_VeChuCo(12, s_szThongBao, gx, (gy + gh + 18 < SCREEN_HEIGHT) ? gy + gh + 4 : gy - 16, 0xFFFF8080);
		else
			s_szThongBao[0] = 0;
	}
}

//---------------------------------------------------------------- 7. bat / tat + nhip
static void UiToaDoM_KhiBat()
{
	SuaGd_DocCauHinh();
	SuaGd_NapDanhSach();
	s_nChon = -1;
	s_bKeoChon = false;
	s_bKeoPad = false;
	s_nNutGiu = -1;
	s_nUndo = 0;
	s_nChumMuc = -1;
	s_nCongCu = CONGCU_DOI;
	SuaGd_Chon(-1);
	s_nPadX = s_nPadY = -1;		// bang chinh ve giua man moi lan mo
	s_bPadGon = false;
	s_bHienOAn = false;	// [SUAGD 13/09 e] mo trinh chinh: o da giau dang cat
	SuaGd_LamMoi();
	DatThongBao("Chπm vµo mÈt icon Æ” ch‰n, k–o Æ” dÍi, chÙm hai ng„n Æ” to nh·");
}

static void UiToaDoM_TruocKhiTat()
{
	if (s_bKeoChon)
		SuaGd_KetKeo();
	s_nChumMuc = -1;
	s_nNutGiu = -1;
	s_bKeoPad = false;
	SuaGd_Chon(-1);
	s_nCongCu = CONGCU_DOI;
	s_bHienOAn = false;	// [SUAGD 13/09 e]
	SuaGd_Kep(true);
}

void UiToaDo_NhipMobile()
{
	static unsigned int s_uKiem = 0;
	unsigned int uNay = GetTickCount();
	bool bTrongGame;

	if (s_bDangSua)
	{
		if (s_nNutGiu >= 0 && uNay >= s_uNutGiuKe)
		{
			SuaGd_BamPad(s_nNutGiu);
			s_uNutGiuKe = uNay + SUAGD_LAP_MOI;
		}
		if (uNay - s_uLamMoiLuc > 100)
			SuaGd_LamMoi();
	}
	if (uNay - s_uKiem < 250)
		return;
	s_uKiem = uNay;
	SuaGd_DocCauHinh();
	bTrongGame = SuaGd_TrongGame();
	if (!bTrongGame)
		s_uVaoGameLuc = 0;
	else if (s_uVaoGameLuc == 0)
	{
		s_uVaoGameLuc = uNay;
		s_bDaGhiSoLieu = false;
	}

	//	vung an toan doi (Android bao inset muon) hoac ho so nhan vat doi -> nap lai + ap lai + neo nhom tren + kep
	if (!s_bDangSua)
	{
		bool bNapLai = false;

		if (JxSdl_VungAnToanDoi() != s_nKgDoiDaThay)
		{
			s_nKgDoiDaThay = JxSdl_VungAnToanDoi();
			bNapLai = true;
			g_DebugLog("[SUAGD] vung an toan doi -> nap lai bo cuc");
		}
		if (bTrongGame && UiToaDoM_MaNhanVat() != s_uNhanVatNap)
		{
			bNapLai = true;
			g_DebugLog("[SUAGD] nhan vat %u -> nap ho so bo cuc", UiToaDoM_MaNhanVat());
		}
		if (bNapLai)
		{
			UiToaDo_Nap();
			UiToaDo_ApChoTatCa();
			JxPlayerBar_NeoNhomTrenLai();
			s_uCanKepLuc = uNay + 200;
		}
		if (s_uCanKepLuc && uNay >= s_uCanKepLuc)
		{
			s_uCanKepLuc = 0;
			SuaGd_Kep(true);
		}
	}
	//	so lieu may that: 3 giay sau khi vao the gioi, [Ui] NhatKyBoCuc >= 1
	if (bTrongGame && !s_bDaGhiSoLieu && s_nNhatKyBoCucM >= 1 && uNay - s_uVaoGameLuc > 3000)
	{
		s_bDaGhiSoLieu = true;
		SuaGd_GhiSoLieu();
	}
}
//	[SUAGD 13/09 g] muc nhat ky bo cuc cho UiToaDo.cpp (ApMotO) - khai bao truoc o dau UiToaDo.cpp
static int UiToaDoM_NhatKy(void)
{
	SuaGd_DocCauHinh();
	return s_nNhatKyBoCucM;
}
