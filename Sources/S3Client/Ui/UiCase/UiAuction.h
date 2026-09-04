// UiAuction.h - [DAUGIA 04/09 A3] cua so DAU GIA (viet lai theo VLTK 2.0: KUiAuctionManager + 3 tab
// Bang hoi / The gioi / Ca nhan, danh sach phien ben trai, danh sach vat pham ben phai (kieu Anh / Ha Lan),
// danh sach thanh vien phien bang hoi, bieu tuong canh bieu tuong thu).
// Khung ma va cach nhan lenh y het UiMail.cpp: Core -> GDCNI_AUCTION_UI (KUiAuction_OnCoreCmd),
// UI -> GOI_AUCTION_UI (KAuctionUiDef.h). ini: \ui\Ui3\auction\*.ini (rut tu client JX1 cu).
#ifndef UIAUCTION_H
#define UIAUCTION_H

#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../elem/wndedit.h"
#include "../elem/wndbutton.h"
#include "../elem/WndLabeledButton.h"
#include "../elem/WndScrollBar.h"
#include "../elem/WndObjContainer.h"
#include "../../../core/src/KAuctionUiDef.h"

class KIniFile;

// ---- mot hang PHIEN (auction_activity_header.ini) ----
class KUiAuctionActRow : public KWndImage
{
public:
	KUiAuctionActRow();
	void	Build();
	void	LoadScheme(KIniFile* pIni);
	void	Fill(const KAucUiActivity* p, int bSelected);
	void	ClearRow();
	char				m_szName[AUCUI_NAME_LEN];	// rong = hang trong
	KWndImage			m_Selection;				// [SelectionImg]
	KWndText80			m_Name;						// [ActivityNameLable]
	KWndButton			m_Click;					// [ForClickImg]
};

// ---- mot hang VAT PHAM (auction_item_english_header.ini + auction_item_dutch_header.ini) ----
class KUiAuctionItemRow : public KWndImage
{
public:
	KUiAuctionItemRow();
	void	Build();
	void	LoadScheme(KIniFile* pEng, KIniFile* pDutch, KIniFile* pMine, KIniFile* pIcon);
	void	Fill(const KAucUiItem* p);
	void	ClearRow();
	void	RefreshTime();							// moi giay
	void	ReleaseItem();							// nha vat pham tam
	KAucUiItem			m_Data;						// nId = 0: hang trong
	int					m_nOffer;					// gia minh dinh tra (kieu Anh)
	// chung
	KWndText256			m_Name;						// [NameOfItem]
	KWndImage			m_IconBg;					// [MailAwardBgSpr] (auction_item_icon.ini)
	KWndObjectBox		m_IconBox;					// [MailAwardItemSpr]
	KWndText32			m_IconCount;				// [MailAwardItemCount]
	int					m_nTempItemIdx;
	// kieu Anh
	KWndText80			m_EngT1, m_EngV1;			// GuaranteedPrice
	KWndText80			m_EngT2, m_EngV2;			// RangePerOffer
	KWndText80			m_EngT3, m_EngV3;			// CurrencyType
	KWndText80			m_EngT4, m_EngV4;			// MaxPrice
	KWndText80			m_EngT5, m_EngV5;			// SelfPrice
	KWndText80			m_EngT6, m_EngV6;			// RemainingTime
	KWndButton			m_BtnAddPrice;				// [btnAddPrice]
	KWndLabeledButton	m_BtnOffer;					// [btnOfferPrice]
	KWndLabeledButton	m_BtnGiveUp;				// [btnGiveUp]
	KWndImage			m_ImgSuccess;				// [imgFinishSuccess]
	KWndImage			m_ImgFail;					// [imgFinishFail]
	KWndImage			m_ImgInput;					// [imgInPutPrice]
	KWndText80			m_TxtOffer;					// [txtJinPai]
	// kieu Ha Lan / ky gui
	KWndText80			m_DutT1, m_DutV1;			// CurrentPrice
	KWndText80			m_DutT2, m_DutV2;			// BasePrice
	KWndText80			m_DutT3, m_DutV3;			// CurrencyType
	KWndText80			m_DutT4, m_DutV4;			// FreshRemainingTime
	KWndText80			m_DutT5, m_DutV5;			// NextPrice
	KWndText80			m_DutT6, m_DutV6;			// TotalRemainingTime
	KWndLabeledButton	m_BtnBid;					// [btnBid]
	KWndLabeledButton	m_BtnGetBack;				// [btnGetBackItem] (auction_item_personal_myself_header.ini)
private:
	void	ShowEnglish(int bShow);
	void	ShowDutch(int bShow);
};

// ---- mot hang THANH VIEN (auction_member_header.ini) ----
class KUiAuctionMemberRow : public KWndImage
{
public:
	KUiAuctionMemberRow();
	void	Build();
	void	LoadScheme(KIniFile* pIni);
	void	Fill(const KAucUiMember* p);
	void	ClearRow();
	KWndText80			m_Name;						// [RoleName1]
	KWndText80			m_Position;					// [Position1]
	KWndText80			m_Level;					// [Level1]
};

// khung cuon: chuyen tiep thong bao nut/thanh cuon cho cha (KWndPage/KWndScrollBar chi bao cha TRUC TIEP)
class KUiAuctionScrollWnd : public KWndImage
{
public:
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
};

#define AUCUI_MEMBER_ROW_COUNT	16

// ---- cua so danh sach thanh vien (auction_memberlist_info.ini) ----
class KUiAuctionMemberWnd : public KWndImage
{
public:
	KUiAuctionMemberWnd();
	void	Build();
	void	LoadScheme(const char* pScheme);
	virtual int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	AddMember(const KAucUiMember* p);
	void	Clear();
	void	SetSalary(const KAucUiSalary* p);
	void	Refresh();
private:
	KWndText80			m_Title;					// [Title]
	KWndText80			m_Col1, m_Col2, m_Col3;		// [ColumnName1..3]
	KWndText80			m_TotalTitle, m_TotalValue;	// [TotalMemberTitle/Value]
	KWndText80			m_SalTitle, m_SalValue;		// [AverageSalaryTitle/Value]
	KUiAuctionScrollWnd	m_ScrollWnd;				// [MemberListScroll]
	KWndScrollBar		m_Scroll;					// [MemberListScroll_ScrollBar]
	KWndButton			m_Close;					// [Close]
	KUiAuctionMemberRow	m_Row[AUCUI_MEMBER_ROW_COUNT];
	KAucUiMember		m_Member[AUCUI_MAX_MEMBER];
	int					m_nCount;
	int					m_nTop;
	int					m_nRowH;
};

// ---- trang (auction_page_{tong,world,personal}.ini) ----
class KUiAuctionPage : public KWndImage
{
public:
	KUiAuctionPage();
	void	Build();
	void	LoadScheme(const char* pScheme, int nType);
	virtual int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void Breathe();
	// phien
	void	AddActivity(const KAucUiActivity* p);
	void	DelActivity(const char* szName);
	void	ClearActivity();
	void	SelectActivity(const char* szName);
	// vat pham
	void	AddItem(const KAucUiItem* p);
	void	SetItem(const KAucUiItem* p);
	void	EndItem(int nId);
	void	ClearItem();
	void	SetPageText(const char* sz);
	void	SetMoney(int nMoney, int nXu);
	void	RefreshAct();
	void	RefreshItem();
	int					m_nType;
private:
	int		ActRowOf(KWndWindow* pWnd);
	int		ItemRowOf(KWndWindow* pWnd, int* pnBtn);	// 1 add, 2 offer, 3 giveup, 4 bid, 5 getback
	int		FindItem(int nId);
	KWndText80			m_Title;					// [Title]
	KWndText80			m_GoldTitle, m_GoldValue;	// [BindingGoldTitle/Value]  -> Xu
	KWndText80			m_SilverTitle, m_SilverValue;	// [BindingSilverTitle/Value] -> Ngan luong
	KWndLabeledButton	m_BtnTip;					// [AuctionPersonalTip]/[AuctionWorldTip]/[seeMemberList]
	KWndText80			m_PageText;					// [CurrentPage]
	KWndLabeledButton	m_BtnPrev;					// [btnPrevPage]
	KWndLabeledButton	m_BtnNext;					// [btnNextPage]
	KUiAuctionScrollWnd	m_ActScrollWnd;				// [ActivityListScroll]
	KWndScrollBar		m_ActScroll;				// [ActivityListScroll_ScrollBar]
	KUiAuctionScrollWnd	m_ItemScrollWnd;			// [ItemListScroll]
	KWndScrollBar		m_ItemScroll;				// [ItemListScroll_ScrollBar]
	KUiAuctionActRow	m_ActRow[AUCUI_ACT_ROW_COUNT];
	KUiAuctionItemRow	m_ItemRow[AUCUI_ROW_COUNT];
	KAucUiActivity		m_Act[AUCUI_MAX_ACTIVITY];
	int					m_nActCount;
	int					m_nActTop;
	char				m_szSelAct[AUCUI_NAME_LEN];
	KAucUiItem			m_Item[AUCUI_MAX_ITEM];
	int					m_nItemCount;
	int					m_nItemTop;
	int					m_nActRowH;
	int					m_nItemRowH;
	int					m_nBreath;
};

// ---- cua so chinh (auction_manager.ini) ----
class KUiAuctionManager : public KWndImage
{
public:
	KUiAuctionManager();
	virtual ~KUiAuctionManager();
	static KUiAuctionManager*	OpenWindow();
	static KUiAuctionManager*	GetIfVisible();
	static KUiAuctionManager*	GetSelf() { return ms_pSelf; }
	static void					CloseWindow(bool bDestory = false);
	static void					LoadScheme(const char* pScheme);
	void	Switch(int nType);
	KUiAuctionPage		m_Page;
	KUiAuctionMemberWnd	m_Members;
private:
	static KUiAuctionManager* ms_pSelf;
	void	Initialize();
	virtual int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	KWndText80			m_Title;					// [Title]
	KWndLabeledButton	m_TabTong;					// [AuctionTong]
	KWndLabeledButton	m_TabWorld;					// [AuctionWorld]
	KWndLabeledButton	m_TabPersonal;				// [AuctionPersonal]
	KWndButton			m_Close;					// [Close]
	char				m_szScheme[256];
};

// ---- bieu tuong (auction_icon.ini) ----
class KUiAuctionIcon : public KWndImage
{
public:
	KUiAuctionIcon();
	static void	SetVisible(int bVisible);
	static void	Blink();
	static void	LoadScheme(const char* pScheme);
	static void	Release();
private:
	static KUiAuctionIcon* ms_pSelf;
	void	Initialize();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void Breathe();
	KWndButton			m_Btn;						// [AuctionBtn]
	int					m_bBlink;
	int					m_nFrame;
	int					m_nBlinkLeft;
};

void KUiAuction_OnCoreCmd(unsigned int uCmd, int nParam);
void KUiAuction_OnGameStart();
void KUiAuction_OnGameExit();

#endif // UIAUCTION_H
