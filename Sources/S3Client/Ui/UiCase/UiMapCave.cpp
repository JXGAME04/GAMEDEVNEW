/*****************************************************************************************
//	UiMapCave.cpp
//	Copyright : Kingsoft 2003
//	Author	  : Phong Kieu
//	CreateTime:	2020-12-01
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include <crtdbg.h>
//#include "../ShortcutKey.h"
#include "UiMapCave.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/GameDataDef.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

KUiMapCave* KUiMapCave::m_pSelf = NULL;

#define		SCHEME_INI_WORLD		"UiMapCave.ini"
#define		WORLD_MAP_INFO_FILE		"\\Settings\\MapList.ini"

void MapToggleStatus();

KUiMapCave* KUiMapCave::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMapCave;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->Show();
		m_pSelf->BringToTop();
		Wnd_SetExclusive(m_pSelf);
	}
	return m_pSelf;
}

void KUiMapCave::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ReleaseExclusive(m_pSelf);
		m_pSelf->Destroy();
		m_pSelf = NULL;
		MapToggleStatus();
	}
}

KUiMapCave* KUiMapCave::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}


void KUiMapCave::Initialize()
{
	AddChild(&m_Sign);

	char szBuffer[128];
	g_UiBase.GetCurSchemePath(szBuffer, sizeof(szBuffer));
	// [BANDO20 06/09] LOI CU: sprintf GHI DE duong dan giao dien vua lay -> nap "\UiMapCave.ini" (khong ton tai)
	// => cua so khong co Left/Top/Width/Height va mui ten [Sign] 'nguoi o day' khong bao gio hien.
	// Noi them nhu KUiWorldmap::Initialize.
	strcat(szBuffer, "\\");
	strcat(szBuffer, SCHEME_INI_WORLD);
	KIniFile	Ini;
	if (Ini.Load(szBuffer))
	{
		Init(&Ini, "WorldMap");
		m_Sign.Init(&Ini, "Sign");
	}

	Wnd_AddWindow(this, WL_TOPMOST);
	return;
}

int KUiMapCave::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int nResult = false;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:	// [BANDO20 06/09] bam dia diem -> Core tu chay bo toi map do; bam cho khac = dong (nhu cu)
		{
			int nCX = 0, nCY = 0;
			Wnd_GetCursorPos(&nCX, &nCY);
			int nGo = m_Locs.OnClick(nCX - m_nAbsoluteLeft, nCY - m_nAbsoluteTop);
			if (nGo == 0)	// trung dia diem nhung khong di duoc (da bao ly do) -> giu ban do mo
			{
				nResult = true;
				break;
			}
			CloseWindow();
			nResult = true;
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_KEYDOWN:
		CloseWindow();
		nResult = true;
		break;
	default:
		nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiMapCave::UpdateData()
{
	m_Sign.Hide();

	if (g_pCoreShell)
	{
		KIniFile	Ini;
		if (Ini.Load(WORLD_MAP_INFO_FILE))
		{
			m_Locs.Load(&Ini);	// [BANDO20 06/09] bang dia diem (ten Viet + toa do) de tro chuot / bam
			
			char	szBuffer[128];
			if (Ini.GetString("List", "CaveMapImage", "", szBuffer, sizeof(szBuffer)))
			{
				SetImage(ISI_T_SPR, szBuffer, true);//SetImage(ISI_T_BITMAP16, szBuffer, true);

				int nAreaX = -1, nAreaY = 0;
				KUiSceneTimeInfo Info;
				g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (KUPARAM)&Info, 0);
				sprintf(szBuffer, "%d_MapPos", Info.nSceneId);
				Ini.GetInteger2("List", szBuffer, &nAreaX, &nAreaY);
				if (nAreaX != -1)
				{
					int nWidth, nHeight;
					m_Sign.GetSize(&nWidth,  &nHeight);
					m_Sign.SetPosition((nAreaX - nWidth / 2) + 30,	// [BANDO20 06/09] cung do lech +30,+17 voi KUiWorldmap (cung anh)
						(nAreaY - nHeight / 2) + 17);
					m_Sign.Show();
				}
			}
		}
	}
}

void KUiMapCave::Breathe()
{
	if (m_Sign.IsVisible())
		m_Sign.NextFrame();
}

// [BANDO20 06/09] ve anh ban do son dong roi ten dia diem dang tro chuot
void KUiMapCave::PaintWindow()
{
	KWndImage::PaintWindow();
	m_Locs.PaintHover(m_nAbsoluteLeft, m_nAbsoluteTop, m_Width);
}
