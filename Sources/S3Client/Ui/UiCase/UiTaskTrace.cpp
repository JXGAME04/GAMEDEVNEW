/*
 * File:     UiTaskTrace.cpp
 * Desc:     Khung "Theo doi nhiem vu" - xem UiTaskTrace.h.
 *           Giao dien nap tu <scheme>\uitaskguide\tasktrace.ini (ban song do ta
 *           dung lai tu ban comment cua ban tham chieu; anh nen co san trong pak).
 * Creation: 2026/08/19
 */
#include "KWin32.h"
#include "KIniFile.h"
#include <stdio.h>
#include <string.h>
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiTaskTrace.h"
#include "UiTaskGuide.h"
#include "UiTaskGuideStr.h"
#include "../UiBase.h"
#include "../../../Engine/src/Text.h"

#define SCHEME_INI_TASKTRACE	"uitaskguide\\tasktrace.ini"

KUiTaskTrace*	KUiTaskTrace::m_pSelf = NULL;
bool			KUiTaskTrace::m_bTraced = false;

KUiTaskTrace::KUiTaskTrace()
{
}

KUiTaskTrace::~KUiTaskTrace()
{
}

KUiTaskTrace* KUiTaskTrace::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTaskTrace;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->UpdateView();
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiTaskTrace::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
	if (bDestroy)
		m_bTraced = false;	// logout / ve man hinh chinh: bo theo doi
}

KUiTaskTrace* KUiTaskTrace::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

bool KUiTaskTrace::IsTraced()
{
	return m_bTraced;
}

void KUiTaskTrace::SetTraced(bool bTraced)
{
	m_bTraced = bTraced;
	if (bTraced)
		OpenWindow();
	else
		CloseWindow(false);
}

void KUiTaskTrace::OnTaskValueChanged(int nTaskId)
{
	if (m_bTraced && m_pSelf && m_pSelf->IsVisible())
		m_pSelf->UpdateView();
}

void KUiTaskTrace::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_List);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
	LoadSchemeSelf(Scheme);

	// mo ra NGANG HANG voi nut icon (vi tri Zalo cu), khung nam sat ben trai nut;
	// neu chua co nut (khong the xay ra vi icon mo cung HUD) thi neo 2/5 man hinh
	int nAX = 0, nAY = 0;
	if (KUiTaskTraceIcon::GetAnchor(nAX, nAY))
	{
		SetPosition(nAX - m_Width - 2, nAY);
	}
	else
	{
		int nSW = 0, nSH = 0;
		Wnd_GetScreenSize(nSW, nSH);
		if (nSW > 0 && nSH > 0)
			SetPosition(nSW - m_Width - 2, nSH * 2 / 5);
	}

	Wnd_AddWindow(this);
}

void KUiTaskTrace::LoadSchemeSelf(const char* pScheme)
{
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_TASKTRACE);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_List.Init(&Ini, "TaskInfo");
	}
}

void KUiTaskTrace::UpdateView()
{
	KWndMessageListBox* pList = m_List.GetMessageListBox();
	pList->Clear();
	pList->SetCapability(4);

	char szLine[512];

	// dong 1: ten nhiem vu (mau vang)
	strncpy(szLine, DTG_TRACE_TITLE, sizeof(szLine) - 1);
	szLine[sizeof(szLine) - 1] = 0;
	int nLen = TEncodeText(szLine, strlen(szLine));
	pList->AddOneMessage(szLine, nLen);

	// dong 2: ban rut gon trang thai hien tai
	KUiTaskGuide::BuildBriefLine(szLine, sizeof(szLine));
	if (szLine[0])
	{
		nLen = TEncodeText(szLine, strlen(szLine));
		pList->AddOneMessage(szLine, nLen);
	}
}

int KUiTaskTrace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_LIST_ITEM_SEL:
	case WND_N_LIST_ITEM_D_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_List)
		{
			// nhu ClickTraceItemFunc cua ban goc: mo bang Chi nam dung nhiem vu
			KUiTaskGuide::OpenWindow();
			return true;
		}
		break;
	}
	return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
}

//////////////////////////////////////////////////////////////////////////
// KUiTaskTraceIcon - nut icon nho mep phai man hinh
//////////////////////////////////////////////////////////////////////////

KUiTaskTraceIcon*	KUiTaskTraceIcon::m_pSelf = NULL;
int					KUiTaskTraceIcon::ms_nAnchorX = 0;
int					KUiTaskTraceIcon::ms_nAnchorY = 0;
bool				KUiTaskTraceIcon::ms_bAnchor = false;

bool KUiTaskTraceIcon::GetAnchor(int& nX, int& nY)
{
	nX = ms_nAnchorX;
	nY = ms_nAnchorY;
	return ms_bAnchor;
}

KUiTaskTraceIcon::KUiTaskTraceIcon()
{
}

KUiTaskTraceIcon::~KUiTaskTraceIcon()
{
}

KUiTaskTraceIcon* KUiTaskTraceIcon::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTaskTraceIcon;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

void KUiTaskTraceIcon::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiTaskTraceIcon::Initialize()
{
	AddChild(&m_Btn);

	char		Scheme[256];
	char		Buff[256];
	KIniFile	Ini;
	g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI_TASKTRACE);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "OpenIcon");
		m_Btn.Init(&Ini, "OpenIconBtn");
	}

	// dat DUNG vi tri nut Zalo cu: X = mep phai - 30 (nhu UiPlayerBar cu),
	// Y = Top doc tu ini [OpenIcon] (= 185, bang [Zalo] cua UiPlayerBar.ini)
	int nX = 0, nY = 0;
	GetPosition(&nX, &nY);
	int nSW = 0, nSH = 0;
	Wnd_GetScreenSize(nSW, nSH);
	if (nSW > 0)
		nX = nSW - 30;
	SetPosition(nX, nY);
	ms_nAnchorX = nX;
	ms_nAnchorY = nY;
	ms_bAnchor = true;

	Wnd_AddWindow(this);
}

int KUiTaskTraceIcon::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK &&
		uParam == (unsigned int)(KWndWindow*)&m_Btn)
	{
		// bat/tat khung theo doi nhiem vu
		KUiTaskTrace::SetTraced(!KUiTaskTrace::IsTraced());
		KUiTaskGuide::RefreshButtons();
		return true;
	}
	return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
}
