/*****************************************************************************************
//	[UITOADO] Che do sua giao dien ngay trong game.
//
//	Bat che do sua -> keo chuot de dat lai vi tri bat ky o giao dien nao ->
//	tat che do sua thi tu dong ghi ra  <thu muc game>\UserData\UiToaDo.ini.
//	Lan sau khoi dong, toa do trong tep do duoc ap de len toa do trong
//	cac tep ini giao dien (\Ui\ui3\*.ini).
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

//	Ap toa do da luu cho mot cua so goc va toan bo o con cua no.
void	UiToaDo_ApChoCuaSo(KWndWindow* pCuaSoGoc);

//	Ap cho moi cua so dang co (dung sau khi nap lai giao dien).
void	UiToaDo_ApChoTatCa();

bool	UiToaDo_DangSua();

//	Bat / tat che do sua. Tat = tu dong ghi tep.
void	UiToaDo_BatTat();

//	Xoa het toa do da luu (giao dien tro lai dung ini goc sau khi khoi dong lai).
void	UiToaDo_XoaHet();

//	Nhan chuot khi dang o che do sua.
//	Tra ve true = da xu ly, khong chuyen tiep cho giao dien phia duoi nua.
bool	UiToaDo_NhanChuot(unsigned int uMsg, unsigned int uParam, int nParam);

//	Ve khung + dong huong dan (goi o cuoi Wnd_RenderWindows).
void	UiToaDo_Ve();
