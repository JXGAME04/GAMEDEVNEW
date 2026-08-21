//---------------------------------------------------------------------------
// File     : UiJxrPlayer.cpp
// Muc dich : Thanh dieu khien PHAT LAI ban dien .jxr (xem UiJxrPlayer.h)
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "KWin32Wnd.h"
#include "UiJxrPlayer.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../../JxReplay.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/iJxReplay.h"

extern struct iRepresentShell* g_pRepresentShell;

// Ten tep ini cua ban tham chieu, ma GBK: C2 BC CF F1 B2 D9 D7 F7 BD E7 C3 E6
// (= "<luc tuong><thao tac gioi dien>.ini"). Viet bang thoat BAT PHAN de khong
// phu thuoc bang ma cua trinh bien dich.
#define SCHEME_INI_JXRPLAYER	"\302\274\317\361\262\331\327\367\275\347\303\346.ini"

KUiJxrPlayer* KUiJxrPlayer::m_pSelf = NULL;

KUiJxrPlayer::KUiJxrPlayer()
{
	m_dwLastTick = 0;
	m_dwInterval = 0x60;		// 96 ms, dung bang ban tham chieu
}

KUiJxrPlayer::~KUiJxrPlayer()
{
}

KUiJxrPlayer* KUiJxrPlayer::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiJxrPlayer;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->BringToTop();
		m_pSelf->Show();
		m_pSelf->m_dwLastTick = 0;
		m_pSelf->m_dwInterval = 0x60;
	}
	return m_pSelf;
}

void KUiJxrPlayer::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiJxrPlayer* KUiJxrPlayer::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiJxrPlayer::Initialize()
{
	AddChild(&m_btnGoOn);
	AddChild(&m_btnPause);
	AddChild(&m_btnSpeedUp);
	AddChild(&m_btnSlowDown);
	AddChild(&m_btnStop);
	AddChild(&m_btnOpen);

	char szScheme[128];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	LoadScheme(szScheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiJxrPlayer::LoadScheme(const char* pScheme)
{
	char		szBuff[MAX_PATH];
	KIniFile	Ini;
	_snprintf(szBuff, sizeof(szBuff), "%s\\%s", pScheme, SCHEME_INI_JXRPLAYER);
	szBuff[sizeof(szBuff) - 1] = 0;
	if (Ini.Load(szBuff))
	{
		KWndPage::Init(&Ini, "Main");
		m_btnPause.		Init(&Ini, "Pause");
		m_btnGoOn.		Init(&Ini, "GoOn");
		m_btnSpeedUp.	Init(&Ini, "SpeedUp");
		m_btnSlowDown.	Init(&Ini, "SlowDown");
		m_btnStop.		Init(&Ini, "Stop");
		m_btnOpen.		Init(&Ini, "Open");
	}
}

int KUiJxrPlayer::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_btnGoOn)
		{
			JxReplay_DoVerb("pause");	// bam lai de chay tiep khi dang tam dung
			IJXReplay* p = JxReplay_Get();
			if (p)
				p->Resume();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnPause)
		{
			JxReplay_DoVerb("pause");
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnSpeedUp)
		{
			// LUU Y: o vtable nay la HAM RONG trong jxreplay.dll ban tham chieu.
			JxReplay_DoVerb("speedup");
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnSlowDown)
		{
			// LUU Y: cung la ham rong trong jxreplay.dll.
			JxReplay_DoVerb("slowdown");
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnStop)
		{
			JxReplay_DoVerb("stop");
			CloseWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_btnOpen)
		{
			// KHAC ban tham chieu: o do nut nay bi Enable(0) vo hieu hoa vi
			// nguoi choi mo tep tu man hinh dang nhap. Client cua ta khong co
			// nut [OpenRep] o man dang nhap nen giu nut nay SONG, neu khong se
			// khong con duong nao mo mot ban dien de xem.
			JxReplay_OpenFileAndPlay();
		}
		return 0;
	}
	return KWndPage::WndProc(uMsg, uParam, nParam);
}

//---------------------------------------------------------------------------
// May bom khung khi dang phat lai.
// Cau truc lay dung tu KUiJxrPlayer::Breathe cua ban tham chieu (0x0052D590):
//    neu chua den ky  -> Sleep(1) roi thoi
//    RepresentBegin -> PlayTick -> ve UI de len -> RepresentEnd
//    PlayTick tra 0 = het ban dien -> dong thanh dieu khien
//---------------------------------------------------------------------------
void KUiJxrPlayer::PlayPump()
{
	IJXReplay* pReplay = JxReplay_Get();
	if (pReplay == NULL || g_pRepresentShell == NULL)
		return;

	KUiJxrPlayer* pSelf = m_pSelf;
	DWORD dwInterval = pSelf ? pSelf->m_dwInterval : 0x60;
	DWORD dwNow = timeGetTime();

	if (pSelf && (dwNow - pSelf->m_dwLastTick) <= dwInterval)
	{
		Sleep(1);
		return;
	}

	if (g_pRepresentShell->RepresentBegin(true, 0) == false)
		return;

	if (pSelf)
		pSelf->m_dwLastTick = timeGetTime();

	int nAlive = pReplay->PlayTick();

	// Ve toan bo cua so UI de len khung vua duoc phat lai.
	Wnd_RenderWindows();
	g_pRepresentShell->RepresentEnd();

	if (nAlive == 0)
	{
		// Het ban dien hoac loi doc: dong thanh dieu khien lai.
		Sleep(10);
		CloseWindow();
	}
}
