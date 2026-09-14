//---------------------------------------------------------------------------
// [ZOOMTHANH 14/09] THANH KEO ZOOM cho ban mobile. Xem chu thich o UiZoomThanh.h.
// Chi bien dich khi JX_MOBILE (chi nam trong android/CMakeLists.txt + ios/CMakeLists.txt) - ban PC khong dinh gi.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_MOBILE
#include "UiZoomThanh.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "KIniFile.h"
#include "../UiBase.h"
#include "../../Platform/JxLiaCanh.h"
#include "KDebug.h"
#include <stdio.h>

#define	SCHEME_INI		"UiZoomThanh.ini"
#define	ZT_CHO_TROI_MS	700		// sau khi nguoi choi keo, cho bao lau roi moi de zoom troi (map / chum) keo nut theo

KUiZoomThanh* KUiZoomThanh::m_pSelf = NULL;
static int s_nThanhZoomBat = 1;	// [ZOOMTHANH 14/09 c] cong tac "Thanh zoom" (Cai dat > Toi uu), mac dinh BAT

// [ZOOMTHANH 14/09 c] UiOptions2: bat / tat thanh keo zoom (tat = khong ve, cham xuyen qua; van zoom duoc bang chum neu bat "Chum zoom")
extern "C" void JxZoomThanh_DatBat(int nBat)
{
	s_nThanhZoomBat = nBat ? 1 : 0;
}

KUiZoomThanh::KUiZoomThanh()
{
	m_nMin = 80; m_nMax = 150; m_nBuoc = 5;
	m_bDangDat = 0; m_bAn = 0; m_uKeoCuoi = 0; m_bNgang = 0;
	AddChild(&m_Thanh);
}

KUiZoomThanh* KUiZoomThanh::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiZoomThanh;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

void KUiZoomThanh::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiZoomThanh::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_NORMAL);
}

void KUiZoomThanh::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Thanh.Init(&Ini, "Thanh");
			{ int nKieu = 0; Ini.GetInteger("Thanh", "Type", 0, &nKieu); m_pSelf->m_bNgang = (nKieu == 0) ? 1 : 0; }	// [ZOOMTHANH 14/09 b] Type=0 ngang
		}
		else
			g_DebugLog("[ZOOMTHANH] khong nap duoc %s", Buff);
	}
}

// Moi khung ve: an / hien theo map + cong tac, dat lai khoang gia tri theo map, keo nut theo zoom dang troi (tru luc nguoi choi vua keo)
void KUiZoomThanh::CapNhat()
{
	int nMin = 0, nMax = 0, nBuoc = 5;
	int nBat = JxLia_ZoomGioiHan(&nMin, &nMax, &nBuoc) && s_nThanhZoomBat;	// [ZOOMTHANH 14/09 c] + cong tac Thanh zoom

	if (nBuoc <= 0)
		nBuoc = 5;
	if (!nBat || nMax <= nMin)
	{
		if (!m_bAn)
		{
			m_bAn = 1;
			m_Thanh.Hide();
		}
		return;
	}
	if (m_bAn)
	{
		m_bAn = 0;
		m_Thanh.Show();
	}
	if (nMin != m_nMin || nMax != m_nMax || nBuoc != m_nBuoc)
	{
		m_nMin = nMin; m_nMax = nMax; m_nBuoc = nBuoc;
		m_bDangDat = 1;
		m_Thanh.SetValueRange(0, (nMax - nMin) / nBuoc);
		m_bDangDat = 0;
	}
	int nGia = m_bNgang ? (m_nMax - JxLia_ZoomDich() + m_nBuoc / 2) / m_nBuoc : (JxLia_ZoomDich() - m_nMin + m_nBuoc / 2) / m_nBuoc;	// [ZOOMTHANH 14/09 b] ngang: 0 = nhin rong nhat (trai)
	if (nGia != m_Thanh.GetScrollPos() && (unsigned int)GetTickCount() - m_uKeoCuoi > ZT_CHO_TROI_MS)
	{
		m_bDangDat = 1;
		m_Thanh.SetScrollPos(nGia);
		m_bDangDat = 0;
	}
}

int KUiZoomThanh::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	if (uMsg == WND_N_SCORLLBAR_POS_CHANGED && uParam == (KUPARAM)(KWndWindow*)&m_Thanh)
	{
		if (!m_bDangDat)
		{
			m_uKeoCuoi = (unsigned int)GetTickCount();
			JxLia_ZoomDatMuot(m_bNgang ? m_nMax - (int)nParam * m_nBuoc : m_nMin + (int)nParam * m_nBuoc);	// [ZOOMTHANH 14/09 b]
		}
		return 0;
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}

void KUiZoomThanh::PaintWindow()
{
	CapNhat();
	KWndWindow::PaintWindow();
}

int KUiZoomThanh::PtInWindow(int x, int y)
{
	if (m_bAn)
		return 0;	// zoom tat: cham xuyen qua xuong ban do
	return KWndWindow::PtInWindow(x, y);
}
int KUiZoomThanh::TaiDiem(int x, int y)
{
	return (m_pSelf && m_pSelf->IsVisible() && m_pSelf->PtInWindow(x, y)) ? 1 : 0;	// PtInWindow tra 0 khi thanh dang an
}

// [ZOOMTHANH 14/09] KSdlApp.cpp: vuot DOC tren giao dien mac dinh la CUON danh sach; tren thanh keo zoom phai la KEO nut
extern "C" int JxZoomThanh_TaiDiem(int x, int y)
{
	return KUiZoomThanh::TaiDiem(x, y);
}
#endif	// JX_MOBILE
