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
//   kind 7 = "phase|rest_giay|tong|kim"      -> giai doan (1 bao danh, 2 chien dau, 3 ket thuc), giay con lai
//   kind 8 = "n;hang|ten|phe|diem;..."        -> toi da TKINFO_MAX_ROW hang (top tich luy tu m_MissionLadder may chu)
//   kind 9 = an (dung chung voi bang diem KUiTongKimScore)
// May chu: ScriptFuns.cpp UpdateBattleInfo / UpdateBattleInfoAll (Lua lib_tktc.lua TK_GuiThongTin*).
// Nut "Nhan xem Chien Bao" mo bang chien bao co san KUiBattleReport (top 10 day du).
//////////////////////////////////////////////////////////////////////
#ifndef UITONGKIMINFO_H
#define UITONGKIMINFO_H

// Wnds.h KHONG co include guard -> include tung header elem co guard
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
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
	virtual void Breathe();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	KWndText	m_Title;			// [Title]       Text= (TCVN3 trong ini)
	KWndText	m_StageLabel;		// [StageLabel]  Text=Giai doan:
	KWndText32	m_Stage;			// [Stage]       ten giai doan (ini [Phase] P1..P3)
	KWndText	m_CountLabel;		// [CountLabel]  Text=Con:
	KWndText32	m_Count;			// [Count]       so giay (client tu dem lui)
	KWndText	m_CountSuffix;		// [CountSuffix] Text=giay
	KWndText	m_HdRank;			// [HdRank]  Text=Hang
	KWndText	m_HdName;			// [HdName]  Text=Ten
	KWndText	m_HdCamp;			// [HdCamp]  Text=Phe
	KWndText	m_HdPoint;			// [HdPoint] Text=Diem
	KWndText32	m_Rank[TKINFO_MAX_ROW];		// [Rank_i]
	KWndText32	m_Name[TKINFO_MAX_ROW];		// [Name_i]
	KWndText32	m_Camp[TKINFO_MAX_ROW];		// [Camp_i]
	KWndText32	m_Point[TKINFO_MAX_ROW];	// [Point_i]
	KWndLabeledButton m_BtnReport;	// [BtnReport] Label=Nhan xem Chien Bao (nut chu trong suot) -> KUiBattleReport
	char	m_szPhase[4][32];		// [Phase] P0..P3 (TCVN3)
	char	m_szCamp[4][16];		// [Camp]  C0..C3 (TCVN3: Tong / Kim)
	int		m_nRestSec;				// giay con lai luc nhan goi
	unsigned int m_uRestTick;		// IR_GetCurrentTime() luc nhan goi
	int		m_nLastShown;			// so giay dang hien (tranh SetText moi khung)
	int		m_bHaveData;
};

#endif // UITONGKIMINFO_H
