/*****************************************************************************************
// Panel TAY LUYEN thuoc tinh an cua Phi Phong (Long Ngam+).
// Port bo cuc tu client VLTK (box "洗炼" trong nhom Ngu Hanh An): dat phi phong vao o,
// bam "Tay luyen" -> server roll 2 dong an MOI (ghi tam vao item + sync). Cot TRAI hien
// 2 dong TRUOC (snapshot luc mo / lan tay truoc), cot PHAI hien 2 dong SAU. Bam
// "Ap dung" giu dong moi; "Giu nguyen" khoi phuc dong cu.
// Date: 2026-09-01. Dung lai co che give-box (UOC_AFFAIR_ITEM, nType=4) nhu box kham.
*****************************************************************************************/
#pragma once

#include "../Elem/WndMessageListBox.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../Elem/WndLabeledButton.h"
#include "../elem/wndimage.h"
#include "../../../core/src/gamedatadef.h"

struct KUiObjAtRegion;

#define PF_WASH_LINE	2	// so dong an hien thi moi cot (2 dong: nhom 1 + nhom 2)

class KUiMantleWash : protected KWndImage
{
public:
	static KUiMantleWash*	OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc);
	static KUiMantleWash*	GetIfVisible();
	static void		LoadScheme(const char* pScheme);
	static void		CloseWindow(bool bDestroy);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateData();
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
private:
	KUiMantleWash();
	~KUiMantleWash() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnWash();		// bam "Tay luyen": roll dong moi
	void	OnKeepOld();	// "Giu nguyen": khoi phuc dong cu
	void	OnApplyNew();	// "Ap dung": giu dong moi + dong box
	// Doc 2 dong an (khe 6-7) cua phi phong dang trong o, dinh dang ra text.
	int		LayDongAn(char szDong[PF_WASH_LINE][128]);
	void	VeHaiCot();		// ve lai 2 cot truoc/sau
private:
	static KUiMantleWash*	m_pSelf;
	KWndImage		m_ImgBg;
	KWndImage		m_ImgArrow;
	KWndImage		m_ImgItemFrame;
	KWndObjectBox	m_ItemSlot;			// o dat phi phong
	KWndText32		m_Title;
	KWndMessageListBox	m_Desc;			// mo ta huong dan
	KWndText32		m_TxtBeforeHdr;
	KWndText32		m_TxtAfterHdr;
	KWndText32		m_TxtBefore[PF_WASH_LINE];
	KWndText32		m_TxtAfter[PF_WASH_LINE];
	KWndText32		m_TxtConsume;
	KWndButton		m_Close;
	KWndLabeledButton	m_BtnWash;		// BtnKeepOn = "Tay luyen"
	KWndLabeledButton	m_BtnKeep;		// BtnRemain = "Giu nguyen"
	KWndLabeledButton	m_BtnApply;		// BtnApply  = "Ap dung"
	char			m_szFunc[32];		// ham nop do (doWashRoll)
	// snapshot 2 dong TRUOC lan tay gan nhat (client tu nho de ve cot trai)
	char			m_szTruoc[PF_WASH_LINE][128];
	bool			m_bDaTay;			// da bam Tay it nhat 1 lan
};
