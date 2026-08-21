//---------------------------------------------------------------------------
// File     : UiJxrPlayer.h
// Muc dich : Thanh dieu khien PHAT LAI ban dien .jxr.
//            Dung lai theo lop KUiJxrPlayer cua ban tham chieu
//            (VFTABLE 0x006A3C3C, the hien toan cuc 0x00758C10).
//
// Bo cuc lay tu tep ini GBK "<luc tuong><thao tac gioi dien>.ini" cua ban
// tham chieu: khung 168x26 tai (313,486), 6 nut o Top=2 va Left lan luot
// GoOn=26, Pause=49, SpeedUp=72, SlowDown=95, Stop=118, Open=141.
//---------------------------------------------------------------------------
#pragma once
#include "../Elem/WndPage.h"
#include "../Elem/WndButton.h"

class KUiJxrPlayer : public KWndPage
{
public:
	static KUiJxrPlayer*	OpenWindow();
	static void				CloseWindow();
	static KUiJxrPlayer*	GetIfVisible();

	// May bom khung khi dang phat lai. Goi tu vong lap chinh.
	static void				PlayPump();

private:
	KUiJxrPlayer();
	~KUiJxrPlayer();

	static KUiJxrPlayer*	m_pSelf;

	void					Initialize();
	void					LoadScheme(const char* pScheme);
	virtual int				WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	KWndButton	m_btnPause;
	KWndButton	m_btnGoOn;
	KWndButton	m_btnSpeedUp;
	KWndButton	m_btnSlowDown;
	KWndButton	m_btnStop;
	KWndButton	m_btnOpen;

	// Moc thoi gian khung cuoi + khoang cach giua hai khung (ms).
	// Ban tham chieu dat mac dinh 0x60 = 96 ms  (~10.4 khung/giay), khop dung
	// luoi thoi gian 3 tick cua dinh dang .jxr o nhip 30 tick/giay.
	DWORD		m_dwLastTick;
	DWORD		m_dwInterval;
};
