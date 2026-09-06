//////////////////////////////////////////////////////////////////////
// UiTongKimInfo.h - [TKINFO 05/09] CUA SO "THONG TIN TRAN" Tong Kim kieu "Lien Dau Bang" cua client 2.0.
//
// Chu 05/09 (anh chup 2.0): tieu de, "Giai doan: X", "Con: N giay" (dem lui), bang Hang | Ten | Phe | Diem,
// dong "Nhan xem Chien Bao". Mo 2.0 (slistcl.pak): cua so do la UI Lua greatteamfight (fieldreport.lua co dung
// mau chu "<color=yellow>Giai doan: <color>%s" / "Con: <color=green>%d<color> giay"), ini/lua cua chinh cua so
// khong duoc dong goi trong 13 pak -> dung lai bang C++ theo anh, nen tu ve theo phong cach nen 2.0
// (than gan den, dai tieu de nau vang, vien vang manh): spr\Ui3\UiGameMain\UiTongKim\thongtin20.spr.
//
// Du lieu: kenh S2C_BATTLE_BOX co san (khong doi giao thuc):
//   kind 7 = "phase|rest_giay|tong|kim"      -> giai doan (1 bao danh, 2 chien dau), giay con lai
//   kind 8 = "n;hang|ten|phe|diem;..."        -> toi da TKINFO_MAX_ROW hang (top tich luy tu m_MissionLadder may chu)
//   kind 9 = an (dung chung voi bang diem KUiTongKimScore)
// May chu: ScriptFuns.cpp UpdateBattleInfo / UpdateBattleInfoAll (Lua lib_tktc.lua TK_GuiThongTin*).
// Nut "Nhan xem Chien Bao" mo/dong bang chien bao co san KUiBattleReport (top 10 day du).
//
// [TKINFO 06/09] Chu bao ba loi sau khi choi that:
//   1. "con thieu hien ten cac cot"  -> 8 NHAN TINH khai bang KWndText TRAN nen khong bao gio hien:
//      KWndText khong co bo dem chu (m_pText = NULL), KWndText::Init chi doc khoa Text= khi DA co bo dem
//      (WndText.cpp:163) va PaintWindow thoat ngay khi khong co (WndText.cpp:202). Phai dung KWndText32/80.
//      (Cung bay da ghi o UiTongKimScore.h va UiDiceItem.h - lap lai lan thu ba.)
//   2. "nut an vao"                  -> them nut THU GON [BtnFold]: thu con dai tieu de (doi anh nen sang
//      ImageFold vi KWndImage::PaintWindow ve NGUYEN tam SPR, KHONG cat theo m_Height), bam lan nua thi mo ra.
//      Nho m_bUserFold nen may chu gui du lieu moi (3 giay/lan) KHONG tu bung ra lai; het tran thi xoa nho.
//   3. "chien bao mo ra tat di khong duoc" -> nut thanh CONG TAC (GetIfVisible -> CloseWindow), va bang chien
//      bao duoc them nut "Dong" cua rieng no (UiBattleReport.*) + tu dong khi het tran (kind 9).
//   Kem: PtInWindow chi bat chuot o dai tieu de + hai nut, con lai cho xuyen xuong the gioi game (truoc day
//   cua so TOPMOST 221x268 nuot moi cu bam vao goc trai man hinh suot tran).
//////////////////////////////////////////////////////////////////////
#ifndef UITONGKIMINFO_H
#define UITONGKIMINFO_H

// Wnds.h KHONG co include guard -> include tung header elem co guard
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"

#define TKINFO_MAX_ROW	5

class KUiTongKimInfo : public KWndImage
{
public:
	KUiTongKimInfo();
	static KUiTongKimInfo*	OpenWindow();			// tao (an) - goi luc vao game
	static void		CloseWindow(BOOL bDestroy);
	static void		LoadScheme(const char* pScheme);
	// tu GameSpaceChangedNotify: goi S2C_BATTLE_BOX (szDesc, nType = nKind 7/8/9)
	static void		OnBattleBox(const char* pszDesc, int nType);
	// tu GameSpaceChangedNotify: doi map that (GDCNI_SWITCHING_MAPMODE) -> an
	static void		OnSwitchMap(int bLoading);
private:
	static KUiTongKimInfo* ms_pSelf;
	void	Initialize();
	void	SetHeader(int nPhase, int nRestSec, int nTong, int nKim);
	void	SetRows(const char* pszRows);
	void	RefreshCountdown();
	void	SetFold(int bFold);				// [TKINFO 06/09] thu gon con dai tieu de / mo ra
	virtual void Breathe();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual int  PtInWindow(int x, int y);	// [TKINFO 06/09] chi bat chuot o dai tieu de + hai nut

	// [TKINFO 06/09] MOI o chu deu phai la KWndText32/80 (co bo dem trong lop). KWndText TRAN im lang:
	// khong nap Text= tu ini, SetText khong lam gi, PaintWindow khong ve.
	KWndText32	m_Title;			// [Title]       Text= (TCVN3 trong ini)
	KWndText32	m_StageLabel;		// [StageLabel]  Text=Giai doan:
	KWndText32	m_Stage;			// [Stage]       ten giai doan (ini [Phase] P1..P3)
	KWndText32	m_CountLabel;		// [CountLabel]  Text=Con:
	KWndText32	m_Count;			// [Count]       so giay (client tu dem lui)
	KWndText32	m_CountSuffix;		// [CountSuffix] Text=giay
	KWndText32	m_HdRank;			// [HdRank]  Text=Hang
	KWndText32	m_HdName;			// [HdName]  Text=Ten
	KWndText32	m_HdCamp;			// [HdCamp]  Text=Phe
	KWndText32	m_HdPoint;			// [HdPoint] Text=Diem
	KWndText32	m_Rank[TKINFO_MAX_ROW];		// [Rank_i]
	KWndText80	m_Name[TKINFO_MAX_ROW];		// [Name_i]  ten 12 ky tu TCVN3 co the toi 24 byte -> 80 cho thoai mai
	KWndText32	m_Camp[TKINFO_MAX_ROW];		// [Camp_i]
	KWndText32	m_Point[TKINFO_MAX_ROW];	// [Point_i]
	KWndLabeledButton m_BtnReport;	// [BtnReport] Label=Nhan xem Chien Bao (nut chu) -> bat/tat KUiBattleReport
	KWndButton	m_BtnFold;			// [BtnFold]   nut thu gon o dai tieu de (switch.spr 15x11, 3 khung)
	char	m_szPhase[4][32];		// [Phase] P0..P3 (TCVN3)
	char	m_szCamp[4][16];		// [Camp]  C0..C3 (TCVN3: Tong / Kim)
	char	m_szImgFull[128];		// [Main] Image=      nen day du 221x268
	char	m_szImgFold[128];		// [Main] ImageFold=  nen khi thu gon 221x27
	int		m_nHeightFull;			// chieu cao khi mo ([Main] Height)
	int		m_nHeightFold;			// chieu cao khi thu gon ([Main] HeightFold, mac dinh 27)
	int		m_bFold;				// dang thu gon?
	int		m_bUserFold;			// nguoi choi CHU DONG thu gon -> khong tu bung ra khi co du lieu moi
	int		m_nRestSec;				// giay con lai luc nhan goi
	unsigned int m_uRestTick;		// IR_GetCurrentTime() luc nhan goi
	int		m_nLastShown;			// so giay dang hien (tranh SetText moi khung)
	int		m_bHaveData;
};

#endif // UITONGKIMINFO_H
