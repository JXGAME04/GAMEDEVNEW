/*****************************************************************************************
//	[UITOADO] Che do sua giao dien ngay trong game.
//
//	Bat che do sua -> keo / lan / bam chuot de dat lai giao dien -> tat che do
//	sua thi tu dong ghi ra  <thu muc game>\UserData\UiToaDo.ini .
//	Lan sau khoi dong, nhung gi ghi trong tep do duoc ap de len \Ui\ui3\*.ini.
//
//	Chuot trong che do sua:
//	  keo TRAI    : doi cho mot o
//	  keo PHAI    : doi cho ca khoi cua so
//	  LAN         : o duoi con tro to / nho lai (moi nac 5%, tu 30% den 300%)
//	  bam GIUA    : tren mot o = an han / hien lai o do
//	                tren nen trong = bat / tat bang danh sach cua so
//
//	Dinh danh moi o giao dien = "<ten lop C++ cua cua so goc>|<ten muc ini>"
//	  - ten lop lay bang RTTI ngay trong Wnd_AddWindow  -> khong phai sua
//	    152 cho goi.
//	  - ten muc ini do KWndWindow::Init nho lai vao m_szMucIni.
//
//	Tinh nang nay CHI bat duoc khi  config.ini  co  [Ui] SuaToaDo=1 .
*****************************************************************************************/
#pragma once

class KWndWindow;

//	config.ini [Ui] SuaToaDo = 1 thi moi cho bat che do sua (doc 1 lan).
bool	UiToaDo_ChoPhep();

//	Nap UserData\UiToaDo.ini vao bang trong bo nho.
void	UiToaDo_Nap();

//	Ap lai cho mot cua so goc va toan bo o con cua no.
void	UiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc);
#ifdef JX_ANDROID
void	UiToaDo_ApChoO(KWndWindow* pWnd);	// [UITOADO 12/09 NEO b] ap cho mot o ngay khi doc xong ini
void	UiToaDo_DumpThuc();	// [UITOADO 12/09 NEO d] nhat ky vi tri thuc moi cua so (mot lan)
void	UiToaDo_LayDich(const char* pszKhoa, int* pnDX, int* pnDY);	// [NHOMTREN 12/09] khoang bo neo da dich mot muc
#endif

//	Ap cho moi cua so dang co (dung sau khi nap lai giao dien).
void	UiToaDo_ApChoTatCa();

//	Duoc goi tu KWndWindow::Paint va PtInWindow nen phai that nhe.
bool	UiToaDo_DangSua();

//	Bat / tat che do sua. Tat = tu dong ghi tep.
void	UiToaDo_BatTat();

//	Xoa het nhung gi nguoi choi da dat + hoi phuc NGAY trong phien
//	(go co an, dua ti le ve 100%). Toa do thi phai khoi dong lai moi ve goc.
void	UiToaDo_XoaHet();

//	Nhan chuot khi dang o che do sua.
//	Tra ve true = da xu ly, khong chuyen tiep cho giao dien phia duoi nua.
bool	UiToaDo_NhanChuot(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);

//	Ve khung + dong huong dan + bang danh sach (goi o cuoi Wnd_RenderWindows).
void	UiToaDo_Ve();

//---------------------------------------------------------------------------
//	[UITOADO 09/09 E] O VE TAY (khong phai cua so KWnd)
//
//	Cum nut ky nang, can dieu khien... duoc ve thang bang DrawPrimitives nen he sua
//	giao dien khong nhin thay chung. Dang ky o day thi chung doi cho duoc y nhu moi
//	o giao dien khac, va vi tri cung duoc luu vao UserData\UiToaDo.ini.
//---------------------------------------------------------------------------
typedef bool (*PFN_UITOADO_TRUNG)(void* pNgu, int x, int y);		// (x,y) co trung o nay khong
typedef void (*PFN_UITOADO_LAYVITRI)(void* pNgu, int* px, int* py);	// vi tri hien tai
typedef void (*PFN_UITOADO_DATVITRI)(void* pNgu, int x, int y);		// dat vi tri moi

//	pszKhoa: ten rieng, dung lam khoa trong tep ini (vi du "CumKyNang").
//	[UITOADO 10/09 F] pNgu = ngu canh tuy y, duoc dua nguyen ve ca ba ham (vi du: so thu
//	tu cua nut, de mot bo ham dung chung cho nhieu nut).
void	UiToaDo_DangKyORieng(const char* pszKhoa, PFN_UITOADO_TRUNG pfnTrung,
			PFN_UITOADO_LAYVITRI pfnLay, PFN_UITOADO_DATVITRI pfnDat, void* pNgu);

//	[UITOADO 10/09 F] Dang sua va cong cu dang chon la "Doi khoi" (keo = ca khoi).
bool	UiToaDo_CongCuKhoi();

//	Cua so sap bi xoa - bo moi con tro dang giu toi no (goi tu Wnd_OnWindowDelete).
void	UiToaDo_QuenCuaSo(KWndWindow* pWnd);
