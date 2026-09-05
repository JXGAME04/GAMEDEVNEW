// UiChienLenh.h - [CL 04/09 DOT2] cua so CHIEN LENH (battle pass) ve theo DUNG bo cuc cua VLTK 2.0:
// tep ini rut tu client 2.0 (\Ui\ui3_800\season_ticket_2026.ini, 56 muc, 595x474) + 30 sprite goc.
// Khung ma va cach nhan lenh y het UiAuction.cpp: Core -> GDCNI_CHIENLENH_UI (KUiChienLenh_OnCoreCmd),
// UI -> GOI_CHIENLENH_UI (KChienLenhUiDef.h). ini: \ui\Ui3\chienlenh.ini.
//
// 2.0 dung UiManage (Lua) NHAN BAN widget tu ini luc chay: 9 cot moc x 22 + 20 dong nhiem vu x 9 = 378.
// Du an khong co bo nap ini tong quat nen moi widget khai tay o day, Init tu CUNG muc ini roi dat vi tri
// theo chi so cot/hang - ket qua ve y het.
//
// BON BAY DA VA TRONG INI (xem ini_ui.py): ten muc > 31 ky tu (KIniFile cat luc nap, tra bang ten du ->
// widget 0x0 khong bao gi), o thuong la WndImage (khong phat click) -> doi WndButton, Enable=0 -> Disable=1,
// Over=N -> Over=1 + OverFrame=N.
#ifndef UICHIENLENH_H
#define UICHIENLENH_H

#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../elem/wndbutton.h"
#include "../elem/WndLabeledButton.h"
#include "../elem/WndScrollBar.h"
#include "../elem/WndImagePart.h"
#include "../elem/WndObjContainer.h"
#include "../../../core/src/KChienLenhUiDef.h"

class KIniFile;

// khung chuyen tiep: KWndButton/KWndScrollBar chi bao cho cha TRUC TIEP, cay o day sau 3-4 tang
class KUiCLScrollWnd : public KWndImage
{
public:
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
};

// ---- MOT O THUONG (nhanh Thuong hoac Hao Hoa) trong mot cot ----
class KUiCLAwardCell : public KUiCLScrollWnd
{
public:
	KUiCLAwardCell();
	void	Build();
	void	LoadScheme(KIniFile* pIni, int bVip);
	void	Fill(const KCLUiAward* p, int nScore, int nGot, int bVipOk);
	void	ClearCell();
	void	ReleaseItem();
	void	Blink(int nFrame);
	KCLUiAward			m_Data;						// nIdx = 0: trong
	int					m_bClaimable;
	KWndButton			m_Light;					// [ElemImg*AwardLight]  - nut bam nhan (WndButton)
	KWndImage			m_Effect;					// [ElemImg*AwardEffect]
	KWndImage			m_IconBg;					// [ElemImg*AwardIconBG]
	KWndButton			m_Icon;						// [ElemImg*AwardIcon]   - anh mac dinh, an khi co vat pham that
	KWndObjectBox		m_Box;						// vat pham THAT (GDI_ITEM_CHAT), nhu hop thu
	int					m_nTempItemIdx;
	KWndText32			m_Num;						// [ElemTxt*AwardNum]
	KWndImage			m_Gray;						// [ElemImg*AwardGray]
	KWndImage			m_Finish;					// [ElemImg*AwardFinish]
	KWndImage			m_Lock;						// [ElemImgVipAwardLock] (chi nhanh VIP)
	int					m_bVip;
};

// ---- MOT COT MOC (ElemImgRankAwardInfoBG 57x175): thanh tien do + so cap + 2 o thuong ----
class KUiCLAwardCol : public KUiCLScrollWnd
{
public:
	KUiCLAwardCol();
	void	Build();
	void	LoadScheme(KIniFile* pIni, int nCol);
	void	Fill(const KCLUiAward* p, const KCLUiInfo* pInf, int nPrevNeed);
	void	ClearCol();
	void	ReleaseItems();
	int					m_nIdx;						// moc dang hien (0 = trong)
	KWndImage			m_ProgBg;					// [ElemImgProgressBg]
	KWndImagePart		m_Prog;						// [ElemImgProgress]
	KWndImage			m_RankBg;					// [ElemImgAwardRankBG]
	KWndText32			m_Rank;						// [ElemTxtAwardRank]
	KUiCLAwardCell		m_Low;						// [ElemImgLowAwardBG]
	KUiCLAwardCell		m_Vip;						// [ElemImgVipAwardBG]
};

// ---- MOT DONG NHIEM VU (ElemImgMissionInfoBG 563x38) ----
class KUiCLMissionRow : public KUiCLScrollWnd
{
public:
	KUiCLMissionRow();
	void	Build();
	void	LoadScheme(KIniFile* pIni, int nRow);
	void	Fill(const KCLUiMission* p);
	void	ClearRow();
	KCLUiMission		m_Data;						// nId = 0: trong
	KWndImage			m_BgFinish;					// [ElemImgMissionInfoBG_Finish]
	KWndImage			m_IconBg;					// [ElemImgMissionAwardIconBG]
	KWndImage			m_Icon;						// [ElemImgMissionAwardIcon] (bieu tuong EXP)
	KWndText32			m_Num;						// [ElemTxtMissionAwardIconNum]
	KWndText256			m_Info;						// [ElemTxtMissionInfo]
	KWndLabeledButton	m_BtnGoto;					// [ElemBtnGoToMissionArea]  "Den"
	KWndLabeledButton	m_BtnGet;					// [ElemBtnGetMissionAward]  "Nhan"
	KWndLabeledButton	m_BtnGot;					// [ElemBtnHaveGetAward]     "Da nhan"
};

// ---- CUA SO CHINH (chienlenh.ini [Main] 595x474) ----
class KUiChienLenh : public KWndImage
{
public:
	KUiChienLenh();
	virtual ~KUiChienLenh();
	static KUiChienLenh*	OpenWindow();
	static KUiChienLenh*	Prepare();					// tao ngam (an, khong tieng) de nhan du lieu truoc khi mo
	static KUiChienLenh*	GetIfVisible();
	static KUiChienLenh*	GetSelf() { return ms_pSelf; }
	static void				CloseWindow(bool bDestory = false);
	static void				LoadScheme(const char* pScheme);
	// du lieu tu Core
	void	SetInfo(const KCLUiInfo* p);
	void	SetAward(const KCLUiAward* p);
	void	SetMission(const KCLUiMission* p);
	void	ClearData();
	void	Refresh();
	void	ShowMsg(const char* szMsg);
private:
	static KUiChienLenh* ms_pSelf;
	void	Initialize();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void Breathe();
	void	RefreshAwards();
	void	RefreshMissions();
	void	RefreshRemain();
	int		MissionCount(int nKind);
	const KCLUiMission* MissionAt(int nKind, int nPos);
	// thanh tieu de
	KWndText80			m_Title;					// [TxtTitle]
	KWndButton			m_Close;					// [BtnClose]
	KWndButton			m_Help;						// [BtnHelp]
	KWndText80			m_Remain;					// [TxtRemainTime]
	KWndLabeledButton	m_BtnBuy;					// [BtnBuyVipCard]
	// khu moc thuong
	KWndImage			m_AwardBg;					// [ImgAwardListBG] 580x176
	KWndImage			m_RankBox;					// [ImgPlayerRankBG] hop nen cl_04 duoi "Cap/LVn" - [CL 04/09 DOT2f] (2.0 ve qua Image= cua WndText)
	KWndText32			m_RankTitle;				// [TxtPlayerRankTitle]
	KWndText32			m_Rank;						// [TxtPlayerRank] "LV30"
	KWndImage			m_LowTitle;					// [ImgLowAwardTitle]
	KWndImage			m_VipTitle;					// [ImgVipAwardTitle]
	KUiCLAwardCol		m_Col[CLUI_COL_COUNT];
	KWndLabeledButton	m_Prev;						// [BtnPrevPage]
	KWndLabeledButton	m_Next;						// [BtnNextPage]
	// khu nhiem vu
	KWndButton			m_DayTabBg;					// [ImgDailyMissionBG]  - the bam duoc (2.0 bat click tren anh)
	KWndText80			m_DayTab;					// [TxtDailyMission]
	KWndButton			m_WeekTabBg;				// [ImgWeeklyMissionBG]
	KWndText80			m_WeekTab;					// [TxtWeeklyMission]
	KWndImage			m_ListBg;					// [DlyList] 582x173
	KWndScrollBar		m_Scroll;					// [DlyScr]
	KUiCLMissionRow		m_Row[CLUI_ROW_COUNT];
	KWndText80			m_Msg;						// thong bao ngan (dung [TxtRemainTime] lam mau)
	// du lieu
	KCLUiInfo			m_Info;
	KCLUiAward			m_Award[CLUI_MAX_AWARD];
	int					m_nAward;
	KCLUiMission		m_Mission[CLUI_MAX_MISSION + 1];
	int					m_nPage;					// 0-based
	int					m_nTab;						// CLUI_KIND_DAY / WEEK
	int					m_nScrollTop;
	int					m_nBlinkFrame;
	int					m_nLastRemainSec;
	int					m_nMsgLeft;
	char				m_szScheme[256];
};

// ---- bieu tuong tren HUD (ini rieng: chienlenh_icon.ini) ----
class KUiCLIcon : public KWndImage
{
public:
	KUiCLIcon();
	static void	SetVisible(int bVisible);
	static void	LoadScheme(const char* pScheme);
	static void	Release();
private:
	static KUiCLIcon* ms_pSelf;
	void	Initialize();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	KWndButton			m_Btn;						// [BtnIcon]
};

void KUiChienLenh_OnCoreCmd(unsigned int uCmd, int nParam);
void KUiChienLenh_OnGameStart();
void KUiChienLenh_OnGameExit();

#endif // UICHIENLENH_H
