/*****************************************************************************************
// Panel "Tay luyen Phi Phong" - dung LAI theo dung client VLTK (lop KUiMantleImplicit):
//   KHUNG CHA  (mantleimplicit_main.ini)       : nen, tieu de, nut dong, 2 tab
//   TRANG 1    (mantleimplicit_pageshift.ini)  : Tay thuoc tinh an - cot TRUOC / mui ten / cot SAU,
//                                                o dat phi phong, Giu nguyen / Gia tri moi / Tay luyen
//   TRANG 2    (mantleimplicit_pageactivate.ini): Kich hoat thuoc tinh an - VLTK kich hoat co thoi han
//                                                14 ngay; JX1 dong an luon hieu luc nen trang nay chi
//                                                HIEN dong an (nen "da kich hoat"), nut Kich hoat tat.
// 3 ini + 12 spr rut nguyen ban tu pak client VLTK (spr doi sang duong dan ASCII
// \spr\vng\ui\mantleimplicit\ de khong phu thuoc codepage).
// Co che du lieu giu nguyen: give-box UOC_AFFAIR_ITEM (nType=4), doWashRoll / doWashKeep / doWashApply.
// Date: 2026-09-01 (lam lai theo yeu cau chu game: "phai nhu tren hinh, co san o client VLTK").
*****************************************************************************************/
#pragma once

#include "../elem/wndpage.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../Elem/WndLabeledButton.h"
#include "../elem/wndimage.h"
#include "../Elem/WndMessageListBox.h"
#include "../../../core/src/gamedatadef.h"

struct KUiObjAtRegion;

#define PF_WASH_LINE	3	// VLTK hien toi da 3 dong an; JX1 hien co 2 (khe 6-7), dong 3 de trong

class KUiMantleWash;

// ---------------- TRANG 1: Tay thuoc tinh an ----------------
class KUiMantleWashPageShift : public KWndPage
{
public:
	KUiMantleWashPageShift();
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	VeHaiCot(const char szTruoc[PF_WASH_LINE][128], const char szSau[PF_WASH_LINE][128]);
private:
	friend class KUiMantleWash;
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	KWndImage		m_ImgBg;
	KWndText80		m_DescTitle;
	KWndMessageListBox	m_Desc;
	KWndText80		m_TxtBeforeHdr;
	KWndText80		m_TxtAfterHdr;
	KWndImage		m_BarBefore[PF_WASH_LINE];
	KWndImage		m_BarAfter[PF_WASH_LINE];
	KWndText256		m_TxtBefore[PF_WASH_LINE];
	KWndText256		m_TxtAfter[PF_WASH_LINE];
	KWndImage		m_ImgArrow;
	KWndImage		m_ImgItemFrame;
	KWndObjectBox	m_ItemSlot;
	KWndText256		m_TxtConsume;
	KWndLabeledButton	m_BtnKeep;		// BtnRemain  = "Giu nguyen gia tri ban dau"
	KWndLabeledButton	m_BtnApply;		// BtnApply   = "Gia tri moi tuong ung"
	KWndLabeledButton	m_BtnWash;		// BtnKeepOn  = "Tay luyen"
};

// ---------------- TRANG 2: Kich hoat thuoc tinh an ----------------
class KUiMantleWashPageActivate : public KWndPage
{
public:
	KUiMantleWashPageActivate();
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	VeDongAn(const char szDong[PF_WASH_LINE][128], int nCo);
private:
	friend class KUiMantleWash;
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	KWndImage		m_ImgBg;
	KWndText80		m_DescTitle;
	KWndMessageListBox	m_Desc;
	KWndImage		m_AttrBgActive[PF_WASH_LINE];
	KWndImage		m_AttrBgInactive[PF_WASH_LINE];
	KWndText256		m_AttrTxt[PF_WASH_LINE];
	KWndImage		m_ImgItemFrame;
	KWndObjectBox	m_ItemSlot;
	KWndText256		m_TxtActivate;
	KWndText256		m_TxtConsume;
	KWndLabeledButton	m_BtnActivate;
};

// ---------------- KHUNG CHA ----------------
class KUiMantleWash : public KWndPageSet
{
public:
	static KUiMantleWash*	OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc);
	static KUiMantleWash*	GetIfVisible();
	static KUiMantleWash*	Instance() { return m_pSelf; }
	static void		LoadScheme(const char* pScheme);
	static void		CloseWindow(bool bDestroy);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateData();
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	// doc dong an (khe 6-7) cua phi phong dang trong o -> text; tra so dong
	static int	LayDongAn(KWndObjectBox& Slot, char szDong[PF_WASH_LINE][128]);
private:
	KUiMantleWash();
	~KUiMantleWash() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnWash();		// "Tay luyen": chup dong hien tai -> cot TRUOC, goi Lua roll
	void	OnKeepOld();	// "Giu nguyen": Lua doWashKeep
	void	OnApplyNew();	// "Gia tri moi": Lua doWashApply + dong
	void	VeLai();
private:
	static KUiMantleWash*	m_pSelf;
	KWndText32		m_Title;
	KWndButton		m_Close;
	KWndLabeledButton	m_TabBtn[2];
	KUiMantleWashPageShift		m_PageShift;
	KUiMantleWashPageActivate	m_PageActivate;
	char			m_szFunc[32];
	char			m_szTruoc[PF_WASH_LINE][128];	// snapshot dong TRUOC lan tay gan nhat
	bool			m_bDaTay;
};
