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

KUiTaskTrace::KUiTaskTrace()
{
	// KWndMovingImage KHONG khoi tao m_oFixPos trong constructor: neu tasktrace.ini
	// khong nap duoc thi Init() khong chay va Show() se nhay den toa do RAC.
	m_oFixPos.x = 0;
	m_oFixPos.y = 0;
	m_nLineCount = 0;	// [C33]
	memset(m_anLineTask, 0, sizeof(m_anLineTask));
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

// Dat khung sat ben trai + NGANG HANG nut theo doi tren thanh PlayerBar.
//
// BAT BUOC cap nhat CA m_oFixPos: KWndShowAnimate::Show() (WndShowAnimate.cpp:60-64)
// khi khong co StartPos/EndPos trong ini se goi SetPosition(m_oFixPos) - tuc RESET
// ve toa do ini - de len moi lenh SetPosition goi TRUOC do. Chi SetPosition khong
// thoi thi khung luon bung ra tai toa do [Main] cua tasktrace.ini, khong ngang nut.
void KUiTaskTrace::SnapToButton()
{
	int nX = 0, nY = 0;
	int nAX = 0, nAY = 0;
	if (KUiPlayerBar::GetTraceBtnPos(nAX, nAY))
	{
		nX = nAX - m_Width - 2;
		nY = nAY;
	}
	else
	{
		int nSW = 0, nSH = 0;
		Wnd_GetScreenSize(nSW, nSH);
		if (nSW <= 0 || nSH <= 0)
			return;
		nX = nSW - m_Width - 2;
		nY = nSH * 2 / 5;
	}
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	m_oFixPos.x = nX;		// diem "dung yen" ma Show()/Hide() se quay ve
	m_oFixPos.y = nY;
	SetPosition(nX, nY);
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
}

KUiTaskTrace* KUiTaskTrace::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

// Trang thai "dang theo doi" = cua so co dang hien hay khong (mot nguon su that duy
// nhat) - khong the lech voi thuc te du bi an tu duong khac (ESC, giao dich, co bac).
bool KUiTaskTrace::IsTraced()
{
	return GetIfVisible() != NULL;
}

void KUiTaskTrace::SetTraced(bool bTraced)
{
	if (bTraced)
		OpenWindow();
	else
		CloseWindow(false);
}

void KUiTaskTrace::OnTaskValueChanged(int nTaskId)
{
	if (m_pSelf && m_pSelf->IsVisible())
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
	m_nLineCount = 0;

	// [C33] liet ke MOI he dang theo doi (khung dung KScrollMessageListBox nen
	// co san thanh cuon); moi he 2 dong: ten he (vang) + ban rut gon.
	int anTask[16];
	int nTask = KUiTaskGuide::GetTracedList(anTask, 16);
	pList->SetCapability(nTask > 0 ? nTask * 2 + 2 : 4);

	char szLine[512];
	for (int i = 0; i < nTask; i++)
	{
		strncpy(szLine, KUiTaskGuide::GetTaskTitle(anTask[i]), sizeof(szLine) - 1);
		szLine[sizeof(szLine) - 1] = 0;
		int nLen = TEncodeText(szLine, strlen(szLine));
		pList->AddOneMessage(szLine, nLen);
		if (m_nLineCount < 32) m_anLineTask[m_nLineCount++] = anTask[i];

		KUiTaskGuide::BuildTraceLineOf(anTask[i], szLine, sizeof(szLine));
		if (szLine[0])
		{
			nLen = TEncodeText(szLine, strlen(szLine));
			pList->AddOneMessage(szLine, nLen);
			if (m_nLineCount < 32) m_anLineTask[m_nLineCount++] = anTask[i];
		}
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
