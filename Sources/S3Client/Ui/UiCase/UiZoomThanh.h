//---------------------------------------------------------------------------
// [ZOOMTHANH 14/09] THANH KEO ZOOM cho ban mobile: nut keo doc o mep man choi de doi muc zoom (phong to <-> nhin rong) thay cho
// chum hai ngon (chu: "lam nut keo dieu chinh zoom cho khoi anh huong toi luc kich vao man choi"). Chum hai ngon van con nhung mac
// dinh TAT (cong tac "Chum zoom" trong Cai dat > Toi uu, JxLia_DatChum). Dau tren = phong to (ZoomToiThieu), dau duoi = nhin rong
// (ZoomToiDa cua map); bam vao thanh = nhay mot nac (ZoomBuoc 5 %), keo nut = lien tuc; zoom troi muot (JxLia_ZoomDatMuot). Thanh
// tu an (khong ve, cham xuyen qua) khi map / nguoi choi tat zoom. Bo cuc ui\ui3\uizoomthanh.ini (lop ghi de), anh
// spr\ui3\uizoomthanh\ (android/anh_zoomthanh.py). Nguoi choi doi cho / giau bang "Chinh giao dien" (khoa KUiZoomThanh|Main).
// Chi bien dich khi JX_MOBILE (android/CMakeLists.txt, ios/CMakeLists.txt) - ban PC khong dinh gi.
//---------------------------------------------------------------------------
#ifndef UIZOOMTHANH_H
#define UIZOOMTHANH_H
#ifdef JX_MOBILE
#include "../Elem/WndWindow.h"
#include "../Elem/WndScrollBar.h"

class KUiZoomThanh : public KWndWindow
{
public:
	static KUiZoomThanh*	OpenWindow();
	static void				CloseWindow();
	static void				LoadScheme(const char* pScheme);
	static KUiZoomThanh*	GetSelf() { return m_pSelf; }
	static int			TaiDiem(int x, int y);	// 1 = diem (toa do man) nam tren thanh dang hien (KSdlApp: vuot doc o day = keo nut, khong cuon)

private:
	KUiZoomThanh();
	void	Initialize();
	void	CapNhat();
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
	void	PaintWindow();
	int		PtInWindow(int x, int y);

	static KUiZoomThanh*	m_pSelf;
	KWndScrollBar	m_Thanh;		// thanh doc: gia tri 0 = phong to nhat (ZoomToiThieu) ... max = nhin rong nhat (ZoomToiDa cua map)
	int				m_nMin, m_nMax, m_nBuoc;	// % zoom
	int				m_bDangDat;		// 1 = dang tu dat lai vi tri nut theo zoom (bo qua thong bao cua thanh)
	int				m_bAn;			// 1 = zoom tat (map / nguoi choi) -> khong ve, cham xuyen qua
	int				m_bNgang;		// [ZOOMTHANH 14/09 b] 1 = thanh NGANG ([Thanh] Type=0): trai = nhin rong (-), phai = phong to (+); 0 = doc: tren = phong to
	unsigned int	m_uKeoCuoi;		// luc nguoi choi doi gan nhat (GetTickCount); 700 ms sau moi cho zoom troi keo nut theo
};
#endif	// JX_MOBILE
#endif
