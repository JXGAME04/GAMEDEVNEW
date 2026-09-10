//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B1] KHUNG AUTO trong game cho ban Android (buoc B1 cua LOTRINH_WAUTO_MOBILE_1109.md).
// Khung anh nen kho VNKU + dai nut NHOM / TAB CON (dung s_aNhomTab cua WAuto.exe, bo Ac chinh va D.nhap) + nut BAT/TAT
// (JxWAuto_Bat cua B0) + dong trang thai (WA_HoatDong cua CoreShell qua JxCore_WAutoHoatDong). Noi dung tab: B2+.
// Anh: android/anh_wauto_vnku.py (thu nho tu kho VNKU); bo cuc: ui\ui3\uiwauto.ini. Mo/dong bang icon Auto tren thanh
// cong cu (Player_WAuto trong UiShell.cpp). Chi bien dich khi JX_ANDROID; khong nam trong vcxproj nao.
//---------------------------------------------------------------------------
#ifndef UiWAuto_H
#define UiWAuto_H
#ifdef JX_ANDROID
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndText.h"

#define WA_UI_SO_NHOM		4
#define WA_UI_TAB_MOI_NHOM	8

class KUiWAuto : protected KWndShowAnimate
{
public:
	static KUiWAuto*	OpenWindow();
	static KUiWAuto*	GetIfVisible();
	static void			CloseWindow();		// an di (giu doi tuong de mo lai nhanh)
	static void			BatTatCuaSo();		// icon Auto: dang hien thi an, dang an thi mo
	static void			HuyCuaSo();			// luc thoat game
private:
	static KUiWAuto*	m_pSelf;
	KUiWAuto();
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
	void	Breathe();
	void	ChonNhom(int nNhom);
	void	ChonTab(int nTab);
	void	CapNhatBatTat();
	void	CapNhatTrangThai(int bEp);

	KWndPureTextBtn	m_Nhom[WA_UI_SO_NHOM];
	KWndPureTextBtn	m_Tab[WA_UI_TAB_MOI_NHOM];
	KWndPureTextBtn	m_Dong;
	KWndButton		m_BatTat;
	KWndText80		m_TrangThai;
	KWndText80		m_TenTab;		// (B1) ten tab dang chon, giua vung noi dung con trong
	int				m_nNhom;
	int				m_nTab;
	int				m_nBatCu;
	unsigned int	m_uTrangThaiKe;
};
#endif // JX_ANDROID
#endif
