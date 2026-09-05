//////////////////////////////////////////////////////////////////////
// UiTongKimScore.h - [TKDIEM 04/09] BANG DIEM "Tong VS Kim" giua-tren man hinh trong tran Tong Kim.
//
// Chu 04/09: "mo ban 2.0 lay cai hien so diem nhu tren hinh gan vao tong kim cua du an".
// Mo gamecl.exe 2.0: lop KUiHuaihehepan (ini GBK "淮河河畔.ini", nhan Label_songPoint / Label_jinPoint,
// thanh can bang "%s_fence" dat theo % = tong*100/(tong+kim), = 50 khi bang nhau; Lua OpenHHHPShiQiZhiUI(bHien,
// nTong, nKim) + UpdateHHHPUiData(nTong, nKim)). Tai nguyen ini/spr cua no KHONG duoc dong goi trong 13 pak
// cua client 2.0 (da quet theo ten bam + theo noi dung) nen dung lai CAU TRUC do voi anh tu ve cua du an.
//
// Du lieu: kenh S2C_BATTLE_BOX co san (Lua UpdateBattleBox 5 tham so -> "tong|kim|diem_toi", nKind):
//   nKind 6 = cap nhat diem (script TK gui sau moi lan giet, khi ra khoi hau doanh) -> hien bang;
//   nKind 9 = an bang (ket thuc tran / roi tran).
// Tu an khi client bat dau nap map moi (GDCNI_SWITCHING_SCENEPLACE, nParam = 1).
//////////////////////////////////////////////////////////////////////
#ifndef UITONGKIMSCORE_H
#define UITONGKIMSCORE_H

// Wnds.h KHONG co include guard (bay da ghi o dot dau gia) -> include tung header elem co guard
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../Elem/WndShadow.h"

class KUiTongKimScore : public KWndImage
{
public:
	KUiTongKimScore();
	static KUiTongKimScore*	OpenWindow();			// tao (an) - goi luc vao game
	static void		CloseWindow(BOOL bDestroy);
	static void		LoadScheme(const char* pScheme);
	// tu GameSpaceChangedNotify: goi S2C_BATTLE_BOX (szDesc = "tong|kim|diem", nType = nKind)
	static void		OnBattleBox(const char* pszDesc, int nType);
	// tu GameSpaceChangedNotify: bat dau nap map moi -> an
	static void		OnSwitchMap(int bLoading);
	static int		IsShowing();
private:
	static KUiTongKimScore* ms_pSelf;
	void	Initialize();
	void	SetScore(int nTong, int nKim);
	void	ApplyBar();
	virtual int  WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual int  PtInWindow(int x, int y);	// 0 = trong suot voi chuot (bam xuyen qua nhu KUiFlashMessage)

	KWndText	m_TongPoint;	// [TongPoint]
	KWndText	m_KimPoint;		// [KimPoint]
	KWndShadow	m_BarTong;		// [BarTong] phan trai thanh can bang (mau Tong)
	KWndShadow	m_BarKim;		// [BarKim]  phan phai (mau Kim)
	int		m_nBarLeft, m_nBarTop, m_nBarWidth, m_nBarHeight;	// [Bar]
	int		m_nTong, m_nKim;
	int		m_bHaveData;
};

#endif // UITONGKIMSCORE_H
