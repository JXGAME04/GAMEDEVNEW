/*****************************************************************************************
// [DUNGLUYEN 01/09] Box DUNG LUYEN VAN CUONG - theo dung 3 ini client JX2/VLTK:
//   smeltsystem\smeltsystem.ini : khung cha (nen, tieu de, nut dong, 2 the), [RuleInfo] Smelt/UnSmelt
//   smeltsystem\smelt.ini       : the 1 "Dung luyen Van Cuong" - BigBox trang bi, SmallBox Van Cuong,
//                                  GuideList + thanh cuon, nut Dung luyen / Huy, hieu ung, [ReturnInfo] 1..24
//   smeltsystem\distill.ini     : the 2 "Van Cuong Thi Luyen" - SmallBox = Huyen Hoa Than, nut Tinh Che / Huy
// Co che du lieu: give-box UOC_AFFAIR_ITEM (S2C_GIVE_BOX nType=5). Region.h = 0 trang bi | 1 nguyen lieu.
// Nut -> GOI_ADD_UI_CMD_SCRIPT(1, "doSmeltBox" | "doUnSmeltBox"); Huy/dong -> GOI_RECOVERY_BOX_COMMAND.
// Ket qua server bao bang Msg2Player (JX1 khong co goi s2c 0xDC cua Linux); client chi kiem so bo 2 o.
// Thong tin mon dat vao o: GDI_FUSION_INFO (KItem::FUS_BuildInfo) in vao GuideList.
*****************************************************************************************/
#pragma once

#include "../elem/wndpage.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../Elem/WndLabeledButton.h"
#include "../elem/wndscrollbar.h"
#include "../elem/wndimage.h"
#include "../Elem/WndMessageListBox.h"
#include "../../../core/src/gamedatadef.h"

struct KUiObjAtRegion;
class KUiSmelt;

// Mot the (dung luyen / thi luyen) - cung bo cuc, khac ini va ten nut
class KUiSmeltPage : public KWndPage
{
public:
	KUiSmeltPage();
	void	Initialize();
	void	LoadScheme(const char* pScheme, const char* pIni, int nMode, const char* pszRule);
	void	ThemGuide(const char* pszText, bool bClear);
	void	HienRuleInfo();
	void	HienReturnInfo(int nKey);		// [ReturnInfo] nKey (1-based, = ma ket qua Linux + 1)
	void	HienThongTinMon(unsigned uId);	// GDI_FUSION_INFO
	void	BatHieuUng();
	void	Breathe();
private:
	friend class KUiSmelt;
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	int			m_nMode;			// 0 dung luyen | 1 thi luyen
	char		m_szIni[64];
	char		m_szRule[1024];
	int			m_nEffect;
	KWndObjectBox		m_BoxEquip;		// BigBox
	KWndObjectBox		m_BoxMat;		// SmallBox
	KWndMessageListBox	m_Guide;
	KWndScrollBar		m_Scroll;
	KWndLabeledButton	m_BtnDo;		// SmeltBtn | UnSmeltBtn
	KWndLabeledButton	m_BtnCancel;
	KWndImage			m_Effect;
	KWndText32			m_Pos1;
	KWndText32			m_Pos2;
};

class KUiSmelt : public KWndPageSet
{
public:
	static KUiSmelt*	OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc);
	static KUiSmelt*	GetIfVisible();
	static KUiSmelt*	Instance() { return m_pSelf; }
	static void		LoadScheme(const char* pScheme);
	static void		CloseWindow(bool bDestroy);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateData();
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	Breathe();
private:
	KUiSmelt();
	~KUiSmelt() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnDo(int nMode);
private:
	static KUiSmelt*	m_pSelf;
	KWndText32			m_Title;
	KWndButton			m_Close;
	KWndLabeledButton	m_TabBtn[2];
	KUiSmeltPage		m_Page[2];
	char				m_szFunc[32];
	unsigned			m_uIdEquip;		// uId mon o o trang bi (0 = trong)
	unsigned			m_uIdMat;		// uId mon o o nguyen lieu
};
