/*****************************************************************************************
// Panel kham Tinh Than Thach len Phi Phong
// Port tu ban Linux (Kiem The): bo cuc lay nguyen tu tep ini goc trong Ui\Ui3
// Date: 2026-08-29
------------------------------------------------------------------------------------------
// Vi sao ke thua khuon KUiAffairItem: duong THU DO va NOP DO cua hop dat do da chay
// on dinh (UOC_AFFAIR_ITEM + GOI_ADD_UI_CMD_SCRIPT), va phia server doMantleMosaicStoneBox
// da nhan dung khuon do. Chi thay bo cuc bang bo cuc goc cua ban Linux.
*****************************************************************************************/
#pragma once

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../Elem/WndLabeledButton.h"
#include "../elem/wndimage.h"
#include "../../../core/src/gamedatadef.h"

struct KUiObjAtRegion;

//////////////////////////////////////////////////////////////////////////
// RUOT cua bang kham. Phai la lop rieng chu khong dung KWndImage tran:
// KWndObjectBox::DropObject gui WND_N_ITEM_PICKDROP toi m_pParentWnd, ma cha
// cua cac o chinh la ruot nay -> phai CHUYEN TIEP len cua so ngoai, khong thi
// tha do va bam nut deu roi vao hu vo.
//////////////////////////////////////////////////////////////////////////
class KUiMantleInlayPad : public KWndImage
{
protected:
	int WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
	{
		if (m_pParentWnd)
			return m_pParentWnd->WndProc(uMsg, uParam, nParam);
		return 0;
	}
};

// Tep ini goc co obj_0..obj_13 = 14 o (3 o cuoi la cho dat PHI PHONG),
// kem imgBottom_0..13 va imgUp_0..12 la anh KHUNG cua tung o.
#define PF_UI_SLOT_COUNT	14
#define PF_UI_IMGBOTTOM		14
#define PF_UI_IMGUP			13

// Vai tro o CO DINH THEO CHI SO, chot tu ma server (CheckInlayStarStone):
//   Region.h 0..4 = Tinh Than Thach, Region.h 5 = Phi Phong (tbItemIdx[6]).
// Cac o con lai (hoa van ngoi sao) chi de HIEN THI, khong nhan tha do.
#define PF_UI_INPUT_COUNT	6

class KUiMantleInlay : protected KWndImage
{
public:
	static KUiMantleInlay*	OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc);
	static KUiMantleInlay*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);
	static void			CloseWindow(bool bDestroy);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateData();
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
private:
	KUiMantleInlay();
	~KUiMantleInlay() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
private:
	static KUiMantleInlay*	m_pSelf;
	KWndMessageListBox	m_Guide;
	KWndScrollBar		m_GuideScroll;
	// --- lop NGOAI (tep starstone-inlay-INTERFACE.ini): vien, nut dong, tieu de, the ---
	KUiMantleInlayPad	m_Pad;			// RUOT -- chuyen tiep thong diep len cua so ngoai
	KWndButton			m_Close;
	KWndText32			m_Title;
	KWndLabeledButton	m_TabStone;
	// --- cac thanh phan ben trong RUOT ---
	KWndImage			m_ImgBottom[PF_UI_IMGBOTTOM];	// khung o (ve duoi cung)
	KWndImage			m_ImgUp[PF_UI_IMGUP];			// vien o (ve tren khung)
	KWndObjectBox		m_Obj[PF_UI_SLOT_COUNT];		// o dat do (ve tren cung)
	KWndLabeledButton	m_OkBtn;
	KWndLabeledButton	m_CancelBtn;
	KWndText32			m_Nhan[PF_UI_INPUT_COUNT];	// nhan ghi ro o nao bo gi
	char				m_szFunc[32];
};
