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

//	Cua so sap bi xoa - bo moi con tro dang giu toi no (goi tu Wnd_OnWindowDelete).
void	UiToaDo_QuenCuaSo(KWndWindow* pWnd);
