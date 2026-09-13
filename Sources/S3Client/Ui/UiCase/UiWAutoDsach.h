//---------------------------------------------------------------------------
// [WAUTO 12/09] BANG PHU cua khung WAuto trong game: sua 6 danh sach cua WAuto va thay MENU cho cac hop chon DAI.
//
// Ban PC de bon o danh sach ngay trong the (toa do di tuan, loc thuoc tinh, moi/vao nhom, ban dien Lien dau) va hai
// hop thoai con (IDD_NOPICK_DIALOG "khong nhat theo ten", IDD_SETSERIES_DIALOG "thu tu ngu hanh"). Tren dien thoai
// mot o danh sach 3 dong trong the 720x432 thi khong cham noi, nen ca sau cai dung CHUNG bang phu nay: the chi con
// mot dong tom tat + nut "Sua".
//
// Bang phu cung thay KPopupMenu cho hop chon dai: KPopupMenu khong cuon (PopupMenu.cpp:171 chieu cao = so dong x cao
// dong ~26 px) nen tren man 604 px, tu dong 24 tro di la cham khong toi - danh sach chieu co the toi 72 dong
// (defSKILLNUMGET) va hop "Di Xa Phu" co 31 dong.
//
// Chi bien dich khi JX_MOBILE; khong nam trong vcxproj nao (them thang vao target main cua android/CMakeLists.txt).
//---------------------------------------------------------------------------
#ifndef UiWAutoDsach_H
#define UiWAutoDsach_H
#ifdef JX_MOBILE
#include "../Elem/WndWindow.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndText.h"
#include "UiWAutoTrang.h"	// KWndNhapWA
#include "../../../Core/Src/ipc_shared.h"

#define WA_DS_DONG		22		// cao mot dong danh sach (px) - du cho ngon tay
#define WA_DS_LE		4		// [WAUTO 13/09] le trong long o (de vien lom khong an vao chu)
#define WA_DS_THANH		10		// be ngang thanh cuon o mep phai (chi ve khi noi dung dai hon o)
#define WA_DS_CHU_L		10		// chu bat dau cach mep trai bao nhieu px
#define WA_DS_TOI_DA	240		// so dong toi da (trang huong dan cua mot the dai nhat khoang 200 dong)
#define WA_DS_CHU		112		// [WAUTO 13/09] do dai moi dong (o rong 660 px, font 12 = 6 px/byte -> 108 byte)
#define WA_DS_NUT		6		// Them / Len / Xuong / Xoa / Xoa het / Lay o day

// O danh sach cham duoc: ve nen toi + tung dong, dong dang chon co vach sang. Cuon bang KUiWAutoDsach (hai nut).
class KWndDsachWA : public KWndWindow
{
public:
	KWndDsachWA();
	virtual void	PaintWindow();
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
	void	XoaHet();
	int		Them(const char* sz);				// -1 = day
	int		So() const			{ return m_nSo; }
	int		Chon() const		{ return m_nChon; }
	void	DatChon(int n);
	void	Cuon(int nBuoc);					// +1 / -1 trang
	int		SoDongHien() const;
	int		Dau() const			{ return m_nDau; }
	const char*	Dong(int i) const;
private:
	char	m_szDong[WA_DS_TOI_DA][WA_DS_CHU];
	int		m_nSo;
	int		m_nChon;
	int		m_nDau;								// dong dau dang hien (cuon)
};

class KUiWAutoDsach : public KWndWindow
{
public:
	KUiWAutoDsach();
	void	KhoiTao(KWndWindow* pTrang);		// AddChild kho widget; pTrang = trang noi dung (an di khi bang phu mo)
	void	NapBoCuc();							// doc uiwauto_dsach.ini (goi sau khi cha da co scheme)
	// che do 0: sua mot trong sau danh sach (nViec = WA_V_DS_*)
	static int	MoSua(int nViec);
	// che do 2: chon mot muc trong danh sach DAI; chon xong bao ve pGoi bang WND_M_MENUITEM_SELECTED (nhu KPopupMenu)
	static int	MoChonMuc(KWndWindow* pGoi, int nKhe, const char* szTieuDe, const char* const* pDong, int nDong, int nChon);
	// [WAUTO 12/09] Dong chu tom tat cua mot danh sach (trang hien ngay canh nut "Sua"): "3 muc: 1/2, 3/4, ..."
	static void	TomTat(int nViec, char* sz, int nMax);
	// [WAUTO 12/09] che do 3: hien HUONG DAN cua the dang mo (trang note + ghi chu tung o), doc uiwauto_ghichu.ini
	static int	MoGhiChu(int nTab, const char* szTenThe);
	static int	DangMo();
	static void	DongLai();
	virtual void	PaintWindow();
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
private:
	static KUiWAutoDsach*	s_pSelf;
	void	Mo(int nCheDo);
	void	Dong();
	void	NapDanhSach();						// che do 0: dien lai o danh sach tu autoData
	void	NapNguon();							// che do 1: dien nguon (thuoc tinh / ten vat pham / ten quanh day)
	void	CapNhatNut();
	void	ChonNguon(int nDong);
	void	Them();
	void	Doi(int nBuoc);						// doi cho dong dang chon len / xuong
	void	NapGhiChu(int nTab);					// [WAUTO 12/09] che do 3
	void	XuongDong(const char* szChu, const char* szDau);	// cat chu dai thanh nhieu dong vua be ngang o
	autoData*	CauHinh();
	KWndText80			m_TieuDe;
	KWndText256			m_Huong;	// [WAUTO 13/09] cau huong dan dai 90-102 byte, KWndText80 cat con 79
	KWndText80			m_NhanSo;
	KWndDsachWA			m_Dsach;
	KWndLabeledButton	m_Nguon;
	KWndNhapWA			m_So;
	KWndLabeledButton	m_Nut[WA_DS_NUT];
	KWndLabeledButton	m_Dong;
	KWndWindow*			m_pTrang;
	int					m_nCheDo;				// 0 sua danh sach / 1 chon nguon / 2 chon mot muc
	int					m_nViec;				// WA_V_DS_* (che do 0 / 1)
	int					m_nNguonChon;			// chi so dong nguon dang chon (-1 = chua)
	char				m_szNguon[WA_DS_CHU];	// chu cua dong nguon dang chon
	KWndWindow*			m_pGoi;					// che do 2: ai goi
	int					m_nKhe;					// che do 2: khe hop chon cua ben goi
	const char* const*	m_pDong;				// che do 2: cac dong
	int					m_nDong;
};
#endif // JX_MOBILE
#endif
