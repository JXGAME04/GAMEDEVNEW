//---------------------------------------------------------------------------
// [VATPHAM 12/09] DAI NUT THAO TAC VAT PHAM cho ban Android (chu 11:10 + 11:55).
//
// Chu: "kich vao item trong ruong se hien thong tin item thay vi kich len tay - hien thong tin item kem cai nut
// co ben du an usvolam: trang bi - rao - nem - dinh - thao"; roi: "THEM NUT DUOI bang hien thong tin item thoi".
// -> Bang thong tin la CAI CO SAN cua game (chu giai chuot, KMouseOver). Cua so nay chi la DAI NUT dat ngay
// DUOI khung chu giai do (g_MouseOver.JxLayKhung). Cham vao o vat pham thi hien dai nut, khong nhac len tay.
//
// Bo nut theo ngu canh roi moi theo loai vat pham (nhu ban tham khao USVOLAM - KuiItemdescVN::addDialogData):
//   - Dang mo RUONG:        do trong ruong -> "Lay ra";  do trong tui -> "Cat ruong"
//   - O trang bi tren nguoi: "Thao", "Rao"
//   - Cua hang / giao dich:  "Chon" (chuyen tiep dung cai bam cu cua cua so chu), "Rao"
//   - Con lai (tui):         "Dung"/"Trang bi", "Rao", "Nem" (neu chua dinh); trang bi thi "Dinh" / "Thao dinh";
//                            mon xep chong thi "Tach". Luon co "Dong".
// Moi nut goi lenh SAN CO cua ban PC (g_pCoreShell->OperationRequest), khong tu che giao thuc moi.
// Anh nut lay tu kho VNKU (android/anh_vatpham_vnku.py -> \spr\ui3\uivatpham\nut_vp_<ten>.spr).
//
// Chi bien dich khi JX_ANDROID va chi nam trong android/CMakeLists.txt - hai chuoi Windows khong dinh gi.
//---------------------------------------------------------------------------
#ifndef UiVatPham_H
#define UiVatPham_H
#ifdef JX_ANDROID

#include "../Elem/WndWindow.h"
#include "../Elem/WndPureTextBtn.h"
#include "../../../Core/src/GameDataDef.h"

#define VP_MAX_NUT		9

struct KUiDraggedObject;

class KUiVatPham : public KWndWindow
{
public:
	//	Cham vao mot o vat pham: hien dai nut. Tra ve false neu khong nhan (ben goi giu thao tac cu).
	static bool			Mo(const KUiDraggedObject* pItem, UIOBJECT_CONTAINER eCont, int nX, int nY, KWndWindow* pChu);
	static void			Dong();
	static bool			DangMo();
	static void			Huy();			// luc thoat game
	static KUiVatPham*	GetIfVisible();

private:
	KUiVatPham();
	void	Initialize();
	void	DungDaiNut(const KUiDraggedObject* pItem, UIOBJECT_CONTAINER eCont, int nX, int nY);
	void	ThemNut(int nMa, const char* pszTen);
	void	XepNut();
	void	LamNut(int nMa);
	void	PaintWindow();
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
	bool	LayObj(KUiObjAtContRegion* pObj);

	static KUiVatPham*	m_pSelf;

	KUiGameObject		m_Obj;			// the loai + ma vat pham dang xem
	int					m_nDataX, m_nDataY, m_nDataW, m_nDataH;
	UIOBJECT_CONTAINER	m_eCont;		// ngan chua no
	KWndWindow*			m_pChu;			// cua so chua o (chuyen tiep cai bam cu)
	int					m_nGenre;		// loai vat pham (item_equip / item_medicine / ...)
	int					m_bKhoa;		// 1 = da dinh (khoa)
	int					m_bChong;		// 1 = mon xep chong (tach duoc)

	KWndPureTextBtn		m_Nut[VP_MAX_NUT];
	int					m_anMaNut[VP_MAX_NUT];
	char				m_szAnhNut[VP_MAX_NUT][96];
	int					m_nSoNut;
};

#endif	// JX_ANDROID
#endif
