#pragma once

#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndScrollBar.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../../../core/src/gamedatadef.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndEdit.h"

struct KUiObjAtRegion;

class KUiAffairItem : protected KWndImage
{
public:
	static KUiAffairItem*	OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc1);
	static KUiAffairItem*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);
	static void			CloseWindow(bool bDestroy);	
	// [DAUGIA 04/09 A6] che do KY GUI: hien them o nhap gia + nut doi loai tien ngay trong hop nay
	static void			SetAuctionMode(int bOn);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateData();	
private:
	KUiAffairItem();
	~KUiAffairItem() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
private:
	static KUiAffairItem*	m_pSelf;
	KWndText32			m_Title;
	KWndMessageListBox	m_ContentList;
	KWndScrollBar		m_ContentScroll;	
	KWndObjectMatrix	m_ItemBox;
	KWndLabeledButton	m_OkBtn;
	KWndLabeledButton	m_CancelBtn;
	KWndText32			m_AucLabel;		// [A6] "Gia ban"
	KWndEdit32			m_AucPrice;		// [A6] o nhap gia
	KWndLabeledButton	m_AucCur;		// [A6] nut doi Ngan luong <-> Xu
	KWndText32			m_AucUnit;		// [A7] don vi sau o nhap: "van" / "Xu"
	KWndText32			m_AucLabel2;	// [A20] "Gia co ban"
	KWndEdit32			m_AucBase;		// [A20] o nhap gia co ban (khoi diem, phai thap hon gia mua ngay)
	KWndText32			m_AucUnit2;		// [A20] don vi cua o gia co ban
	int					m_bAucMode;
	int					m_nAucCur;		// 1 Ngan luong, 2 Xu
	char 				szFunc1[32];	
	char 				szFunc2[32];		
};

// [BOXMAU 01/09] Ma hoa the <color=...> cho o mo ta cac box giao nop (AffairItem/
// MantleInlay/MantleWash dung chung). Chuoi Lua den con THO; KWndMessageListBox::
// AddOneMessage khong tu ma hoa (WndMessageListBox.cpp:121 chi memcpy) nen the mau
// hien nguyen van. Tra ve so byte sau ma hoa (dung lam tham so AddOneMessage).
int UiAffair_EncodeDesc(const char* pszSrc, char* pszDst, int nDstCap);

