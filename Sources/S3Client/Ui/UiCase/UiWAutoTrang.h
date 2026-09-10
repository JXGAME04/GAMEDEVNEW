//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B2] TRANG NOI DUNG cua khung WAuto trong game - chay theo BANG (UiWAutoBang.h, sinh bang may tu WAuto.rc).
// Mot kho widget dung chung cho ca 15 tab; NapTab(n) doc uiwauto_tabN.ini de dat lai vi tri / nhan, roi dien gia tri doc thang
// tu autoData (JxWAuto_CauHinh) theo offset trong bang. Doi gi -> ghi vao autoData + luu APdata\<id>.dat ngay.
// Chi bien dich khi JX_ANDROID; khong nam trong vcxproj nao. Xem LOTRINH_WAUTO_MOBILE_1109.md §B2.
//---------------------------------------------------------------------------
#ifndef UiWAutoTrang_H
#define UiWAutoTrang_H
#ifdef JX_ANDROID
#include "../Elem/WndWindow.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndText.h"
#include "../../../Core/Src/ipc_shared.h"	// IPCSkillInfo, defSKILLNUMGET, autoData

#define WA_TR_TICK	22
#define WA_TR_NHAP	12
#define WA_TR_CHON	12
#define WA_TR_NHAN	32
#define WA_TR_NUT	12
#define WA_TR_SO_TAB	15
#define WA_TR_HOP	8
#define WA_TR_AN_TOI_DA	(WA_TR_TICK + WA_TR_NHAP + WA_TR_CHON + WA_TR_NHAN + WA_TR_NUT)

struct WAUiMuc;
struct WAUiTab;

// O nhap cua trang: KWndEdit chi ve chu, tren nen khung khong nhin ra o nhap -> ve them nen toi + vien truoc chu
class KWndNhapWA : public KWndEdit512
{
public:
	virtual void	PaintWindow();
};

// O NHOM (chu 11/09: "phai ke o de phan biet tung nhom", "co mau phan biet cac nhom chuc nang"): nen toi mo + vien + tieu de
// (TieuDe= trong ini, TCVN3); Mau= 0..5 chon bo mau vien / nen tieu de (bang s_aMauHop trong UiWAutoTrang.cpp)
class KWndHopNhomWA : public KWndWindow
{
public:
	KWndHopNhomWA();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);
	virtual void	PaintWindow();
private:
	char	m_szTieuDe[64];
	int		m_nMau;
};

class KUiWAutoTrang : public KWndWindow
{
public:
	KUiWAutoTrang();
	void	KhoiTao();					// AddChild ca kho widget (goi mot lan truoc khi cha AddChild(this))
	void	NapTab(int nTab);			// 0..14: doc uiwauto_tabN.ini, dat lai widget, dien gia tri
	void	DienGiaTri();				// dien lai tu autoData (mo lai cua so, doi nhan vat)
	int		TabDangHien()	{ return m_nTab; }
	void	Breathe();					// cha goi moi nhip (LetMeBreathe chi toi cua so goc)
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
private:
	KWndHopNhomWA		m_Hop[WA_TR_HOP];		// ve truoc (AddChild dau tien) de nam duoi cac o
	KWndButton			m_Tick[WA_TR_TICK];
	KWndNhapWA			m_Nhap[WA_TR_NHAP];
	KWndLabeledButton	m_Chon[WA_TR_CHON];
	KWndText80			m_Nhan[WA_TR_NHAN];
	KWndLabeledButton	m_Nut[WA_TR_NUT];
	int					m_nTab;
	const WAUiTab*		m_pTab;
	IPCSkillInfo		m_aChieu[defSKILLNUMGET];
	int					m_nChieu;				// -1 = chua nap
	int					m_bDangDien;			// dang dien tu autoData -> bo qua WND_N_EDIT_CHANGE (SetIntText cung ban)
	KWndWindow*			m_apAnTam[WA_TR_AN_TOI_DA];	// widget tam an khi menu chon dang mo (trinh chieu Android ve anh + chu SAU bong menu)
	int					m_nAnTam;
	void	AnHet();
	const WAUiMuc*	Muc(int nLoai, int nKhe);
	void*	DiaChi(const WAUiMuc* p);
	int		LayInt(const WAUiMuc* p);
	void	DatInt(const WAUiMuc* p, int v);
	void	DienChon(const WAUiMuc* p);
	void	NapChieu();
	void	MoMenuChon(int nKhe);
	void	ChonMenu(int nKhe, int nMuc);
	void	AnDuoiMenu(int bAn);
	void	ThuAn(KWndWindow* p, int x0, int y0, int x1, int y1);
	void	DaDoi();
};
#endif // JX_ANDROID
#endif
