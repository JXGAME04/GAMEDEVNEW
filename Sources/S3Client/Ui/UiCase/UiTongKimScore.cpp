//////////////////////////////////////////////////////////////////////
// UiTongKimScore.cpp - [TKDIEM 04/09] BANG DIEM "Tong VS Kim" (xem UiTongKimScore.h)
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"		// mot lan duy nhat (khong co include guard): Wnd_AddWindow, WL_TOPMOST
#include "../UiBase.h"
#include "UiTongKimScore.h"
#include <stdlib.h>
#include <string.h>

#define TKSCORE_INI		"UiTongKimScore.ini"
#define TKSCORE_KIND_UPDATE	6	// UpdateBattleBox(..., 6): "tong|kim|diem"
#define TKSCORE_KIND_HIDE	9	// UpdateBattleBox(..., 9): an bang

extern int SCREEN_WIDTH;

KUiTongKimScore* KUiTongKimScore::ms_pSelf = NULL;

KUiTongKimScore::KUiTongKimScore()
{
	m_nBarLeft = m_nBarTop = 0;
	m_nBarWidth = m_nBarHeight = 0;
	m_nTong = m_nKim = 0;
	m_bHaveData = 0;
}

KUiTongKimScore* KUiTongKimScore::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongKimScore;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	return ms_pSelf;
}

void KUiTongKimScore::CloseWindow(BOOL bDestroy)
{
	if (ms_pSelf)
	{
		if (bDestroy)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
		else
			ms_pSelf->Hide();
	}
}

void KUiTongKimScore::Initialize()
{
	AddChild(&m_BarTong);
	AddChild(&m_BarKim);
	AddChild(&m_TongPoint);
	AddChild(&m_KimPoint);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
	Hide();	// chi hien khi co diem tu may chu
}

void KUiTongKimScore::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf || !pScheme || !pScheme[0])
		return;
	char Buff[256];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, TKSCORE_INI);
	if (!Ini.Load(Buff))
		return;
	// [Main] / [Main1024] nhu cac cua so khac
	if (SCREEN_WIDTH == 1024)
		ms_pSelf->Init(&Ini, "Main1024");
	else
		ms_pSelf->Init(&Ini, "Main");
	ms_pSelf->m_TongPoint.Init(&Ini, "TongPoint");
	ms_pSelf->m_KimPoint.Init(&Ini, "KimPoint");
	ms_pSelf->m_BarTong.Init(&Ini, "BarTong");
	ms_pSelf->m_BarKim.Init(&Ini, "BarKim");
	// [Bar]: vung thanh can bang (toa do trong cua so); hai KWndShadow chia nhau theo %
	Ini.GetInteger("Bar", "Left", 0, &ms_pSelf->m_nBarLeft);
	Ini.GetInteger("Bar", "Top", 0, &ms_pSelf->m_nBarTop);
	Ini.GetInteger("Bar", "Width", 0, &ms_pSelf->m_nBarWidth);
	Ini.GetInteger("Bar", "Height", 0, &ms_pSelf->m_nBarHeight);
	// dat giua man hinh theo chieu ngang (ini co the ghi Left = -1 de tu can)
	int nX = 0, nY = 0, nW = 0, nH = 0;
	ms_pSelf->GetPosition(&nX, &nY);
	ms_pSelf->GetSize(&nW, &nH);
	if (nX < 0)
		ms_pSelf->SetPosition((SCREEN_WIDTH - nW) / 2, nY);
	ms_pSelf->ApplyBar();
}

// chia thanh can bang: phan Tong = tong*100/(tong+kim) %, bang nhau (hoac 0-0) = 50 %  (dung luat KUiHuaihehepan 2.0)
void KUiTongKimScore::ApplyBar()
{
	if (m_nBarWidth <= 0 || m_nBarHeight <= 0)
	{
		m_BarTong.Hide();
		m_BarKim.Hide();
		return;
	}
	int nPct = 50;
	if (m_nTong != m_nKim && (m_nTong + m_nKim) > 0)
		nPct = (int)((__int64)m_nTong * 100 / (m_nTong + m_nKim));
	if (nPct < 3) nPct = 3;		// luon thay mot mau
	if (nPct > 97) nPct = 97;
	int nWTong = m_nBarWidth * nPct / 100;
	m_BarTong.SetPosition(m_nBarLeft, m_nBarTop);
	m_BarTong.SetSize(nWTong, m_nBarHeight);
	m_BarKim.SetPosition(m_nBarLeft + nWTong, m_nBarTop);
	m_BarKim.SetSize(m_nBarWidth - nWTong, m_nBarHeight);
	m_BarTong.Show();
	m_BarKim.Show();
}

void KUiTongKimScore::SetScore(int nTong, int nKim)
{
	if (nTong < 0) nTong = 0;
	if (nKim < 0) nKim = 0;
	m_nTong = nTong;
	m_nKim = nKim;
	m_bHaveData = 1;
	m_TongPoint.SetIntText(nTong);
	m_KimPoint.SetIntText(nKim);
	ApplyBar();
	if (!IsVisible())
		Show();
	BringToTop();
}

// szDesc = "tong|kim|diem_toi" (LuaUpdateBattleBox 5 tham so); nType = nKind
void KUiTongKimScore::OnBattleBox(const char* pszDesc, int nType)
{
	if (nType != TKSCORE_KIND_UPDATE && nType != TKSCORE_KIND_HIDE)
		return;
	KUiTongKimScore* p = OpenWindow();
	if (!p)
		return;
	if (nType == TKSCORE_KIND_HIDE)
	{
		p->m_bHaveData = 0;
		p->Hide();
		return;
	}
	if (!pszDesc)
		return;
	char szTmp[128];
	strncpy(szTmp, pszDesc, sizeof(szTmp) - 1);
	szTmp[sizeof(szTmp) - 1] = 0;
	int nVal[2] = { 0, 0 };
	char* pTok = strtok(szTmp, "|");
	for (int i = 0; i < 2 && pTok; i++)
	{
		nVal[i] = atoi(pTok);
		pTok = strtok(NULL, "|");
	}
	p->SetScore(nVal[0], nVal[1]);
}

// bat dau nap map moi (roi tran, ve thanh, mat ket noi...) -> an; map moi co diem thi may chu gui lai (kind 6)
void KUiTongKimScore::OnSwitchMap(int bLoading)
{
	if (ms_pSelf && bLoading)
	{
		ms_pSelf->m_bHaveData = 0;
		ms_pSelf->Hide();
	}
}

int KUiTongKimScore::IsShowing()
{
	return (ms_pSelf && ms_pSelf->IsVisible()) ? 1 : 0;
}

// khong bat chuot: de nguoi choi bam xuyen qua bang (nhu KUiFlashMessage)
int KUiTongKimScore::PtInWindow(int x, int y)
{
	return 0;
}

int KUiTongKimScore::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		return 0;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
