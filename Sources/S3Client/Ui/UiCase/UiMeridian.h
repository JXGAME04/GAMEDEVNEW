#pragma once
#include "../Elem/WndToolBar.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"
#include <GameDataDef.h>
#include "../Elem/WndLine.h"


enum {
	normalway = 0,
	enhancedway,
	protectedway,
};
class KUiMeridian : public KWndToolBar
{
public:
	static KUiMeridian* OpenWindow();
	static void				CloseWindow();
	static KUiMeridian* GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void				DefaultScheme(const char* pScheme);
	static KUiMeridian* GetSelf() { return m_pSelf; }
	void UpdateMeridianLevel();
	void UpdateMeridian();
private:
	~KUiMeridian() {}
	void	Initialize();							
	void	Breathe();
	void	LoadScheme(KIniFile* pIni);
	void	RebuildPage();							// [KM 27/08] dung lai trang mach theo ban chuan
	void	CapNhatNguyenLuc();						// [KM 27/08] 2 dong Chan Nguyen / Huyen Nguyen
	void	DungChuKhiDoanh(KIniFile* pIni);			// [KM 27/08] 3 dong mo ta trang Khi Doanh
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	int btnNo = 1;
private:
	static KUiMeridian* m_pSelf;
	KWndText80	m_LevelText;
	KWndText80	m_RankWorldText;

	KWndLabeledButton	m_Btn[MAX_MERIDIAN+1];
	KWndButton			m_Points[MAX_MERIDIAN_LEVEL];
	KWndButton	m_Close;
	KWndLabeledButton	m_BtnBreath[3];			// [KM 27/08] 1 / 7 / 30 ngay
	KWndLabeledButton	m_BtnViewBuff;			// [KM 27/08] Xem thuoc tinh tang bac
	KWndText80			m_txtCurZY;				// [KM 27/08] Chan Nguyen hien tai
	KWndText80			m_txtCurXY;				// [KM 27/08] Huyen Nguyen hien tai
	KWndText256			m_BreathBuffInfo;		// [KM 27/08]
	char				m_szBreathInfo[256];		// [KM 27/08] bo dem 3 dong trang Khi Doanh
	KWndText80	m_MerLevelLabel[MAX_MERIDIAN_LEVEL];
	KWndImage	m_Background;
	BYTE			m_MeridianLevel[MAX_MERIDIAN];
	KWndLine	m_Connections[15];
	KWndLine	m_NameLine[MAX_MERIDIAN_LEVEL / 2];	// [KM 27/08] duong ngang tu huyet ra cot ten
	BYTE			m_LastLevel[MAX_MERIDIAN];			// [KM 27/08] de biet luc nao phai dung lai trang
	int				m_nLastBtn = 0;						// [KM 27/08]
	KIniFile*		m_pIni = NULL;						// [KM 27/08] nap ini MOT LAN roi dung lai
	int				m_nLastBuild = 0;					// [KM 27/08] moc gio lan dung lai gan nhat
	int				m_nTinDenHan = 0;					// [KM 27/08b] moc gio het hien thong bao tam
	KIniFile*		LayIni();							// [KM 27/08]
	int lastTick = 0;
};

// [KM 27/08] Cua so "Xem thuoc tinh tang bac Kinh Mach" - bo cuc + noi dung theo
// qiyingdantian_buff.lua cua ban chuan (da trich tu slistcache.pak).
// [KM 27/08] o chu 1024 byte theo dung khuon KWndText256 (SetTextPtr la protected)
class KWndText1024KM : public KWndText
{
private:
	char			m_Text[1024];
public:
	KWndText1024KM() { m_Text[0] = m_Text[1023] = 0; SetTextPtr(m_Text, 1023); }
};

class KUiMeridianBuff : public KWndToolBar
{
public:
	static KUiMeridianBuff* OpenWindow();
	static void				CloseWindow();
	static KUiMeridianBuff* GetIfVisible();
private:
	KUiMeridianBuff() {}
	~KUiMeridianBuff() {}
	void	Initialize();
	void	DungNoiDung(KIniFile* pIni);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	static KUiMeridianBuff* m_pSelf;
	KWndImage	m_Nen;
	KWndText80	m_TieuDe;
	KWndText80	m_DauCot;
	KWndText1024KM	m_DanhSach;
	KWndButton	m_Dong;
	char		m_szDanhSach[1024];
	BYTE		m_CapMach[MAX_MERIDIAN];
};

class KUiMeridianConfirm : public KWndToolBar
{
public:
	static KUiMeridianConfirm* OpenWindow();
	static void				CloseWindow();
	static KUiMeridianConfirm* GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void				DefaultScheme(const char* pScheme);
	static KUiMeridianConfirm* GetSelf() { return m_pSelf; }
	void				SetMeridian(int Type, int Level);
	void	SetTitle(char* title);
	void	SetInfo();
private:
	KUiMeridianConfirm() {}
	~KUiMeridianConfirm() {}
	void	Initialize();
	void	Breathe();
	void	LoadScheme(KIniFile* pIni);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	static KUiMeridianConfirm* m_pSelf;

	// [KM 27/08] Bo dieu khien theo tep chuan: 3 o chon + 2 nut Xung Huyet + bang mo ta.
	KWndLabeledButton	m_btnOk;				// [btnOk]
	KWndLabeledButton	m_btnOkOne;				// [btnOk_OnlyOneWay] - khi chi co mot cach
	KWndLabeledButton	m_cbWay[3];				// Pho thong / Huyet vi bao ve / Long Hon Ho The
	KWndButton			m_btnClose;
	KWndText256			Title;					// [txtTitle]
	KWndText1024KM		m_Info;					// [ListInfo] - bang mo ta nhieu dong
	int m_nType;
	int m_nLevel;
	int m_nWay;									// 0 thuong / 1 bao ve / 2 Long Hon / 3 Dai Ho Mach
	int m_TyLeLuot[2][33][6];					// [TyLeLuot0/1]: [0]=so muc, [1..5]=ty le theo so lan hong
	KWndImage			m_imgWay[3];			// [KM 27/08b] o tick 3 khung nhu ban chuan
	int	m_nTickCo;								// StatusEnableFrame - dang chon
	int	m_nTickKhong;							// StatusDisableFrame - chua chon
	int	m_nTickXam;								// StatusInvalidFrame - cach vo hieu
	int m_HuyetLong[9][17][7];					// [HuyetLong1..8]
	int m_DaiHoMach[17][3];						// [DaiHoMach]
};