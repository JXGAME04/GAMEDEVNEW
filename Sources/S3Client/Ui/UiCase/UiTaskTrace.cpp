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
#include "UiPlayerBar.h"
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
		m_pSelf->SnapToButton();	// moi lan bung ra deu ngang hang nut theo doi
		m_pSelf->UpdateView();
		m_pSelf->Show();
	}
	return m_pSelf;
}

// dat khung sat ben trai + NGANG HANG nut theo doi tren thanh PlayerBar
void KUiTaskTrace::SnapToButton()
{
	int nAX = 0, nAY = 0;
	if (KUiPlayerBar::GetTraceBtnPos(nAX, nAY))
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

	SnapToButton();

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
