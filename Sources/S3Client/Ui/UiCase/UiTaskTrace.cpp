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
	m_bDaNeo = false;	// [NEOTRACE 10/09] chua neo duoc vao nut theo doi
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
	// [NEOTRACE 10/09] GetTraceBtnPos chi bao PlayerBar CO TON TAI; vi tri tuyet doi cua nut
	// chi dung SAU khi thanh duoc dat cho (AbsoluteMove), nen phai loai truong hop
	// con (0,0) - nut that nam o goc duoi-phai, khong bao gio o (0,0).
	if (KUiPlayerBar::GetTraceBtnPos(nAX, nAY) && (nAX > 0 || nAY > 0))
	{
		nX = nAX - m_Width - 2;
		nY = nAY;
		m_bDaNeo = true;
	}
	else
	{
		int nSW = 0, nSH = 0;
		Wnd_GetScreenSize(nSW, nSH);
		if (nSW <= 0 || nSH <= 0)
			return;
		nX = nSW - m_Width - 2;
		nY = nSH * 2 / 5;
		m_bDaNeo = false;	// [NEOTRACE 10/09] du phong -> Breathe se thu neo lai
	}
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	m_oFixPos.x = nX;		// diem "dung yen" ma Show()/Hide() se quay ve
	m_oFixPos.y = nY;
	SetPosition(nX, nY);
}

//	[NEOTRACE 10/09] + [TASKTRACE 12/09] GIU CA HAI: Breathe la duong chung, thu neo lai moi khung cho den
//	khi co nut. NeoLaiKhiCoThanh van giu vi ban Android goi thang tu KUiPlayerBar ngay sau Wnd_AddWindow -
//	neo duoc ngay trong khung do, khong phai doi them mot nhip ve.
// [NEOTRACE 10/09] Chay moi khung. Luc dang nhap, khung nay duoc bat len tu goi TASK VALUE
// (GDCNI_TASK_VALUE_UPDATE -> KUiTaskGuide::AutoTraceOnTask -> SetTraced(true)),
// goi do ve TRUOC khi UiStartGame() (UiShell.cpp) tao KUiPlayerBar => lan neo dau
// khong co nut, khung roi vao nhanh du phong (mep phai, 2/5 chieu cao) va NAM LUON
// o do ca phien = "moi lan dang nhap bi lech". Nay thu neo lai den khi duoc.
void KUiTaskTrace::Breathe()
{
	// Khong goi ban lop cha: KWndWindow::Breathe la virtual RONG va de o muc private
	// (C2248 neu goi thang); ca KWndMovingImage/KWndImage/KWndShowAnimate deu khong cai dat.
	if (!m_bDaNeo && IsVisible())
		SnapToButton();
}
#ifdef JX_ANDROID
// [TASKTRACE 12/09] tu dang nhap vao thang game: khung mo TRUOC khi KUiPlayerBar co -> SnapToButton roi vao nhanh
// 'mep phai, 40 % chieu cao' va m_oFixPos giu luon cho do (de len cot icon phai). PlayerBar goi ham nay sau Wnd_AddWindow.
void KUiTaskTrace::NeoLaiKhiCoThanh()
{
	if (m_pSelf)
		m_pSelf->SnapToButton();
}
#endif

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


int KUiTaskTrace::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	switch (uMsg)
	{
	case WND_N_LIST_ITEM_SEL:
	case WND_N_LIST_ITEM_D_CLICK:
		if (uParam == (KUPARAM)(KWndWindow*)&m_List)
		{
			// nhu ClickTraceItemFunc cua ban goc: mo bang Chi nam dung nhiem vu
			KUiTaskGuide::OpenWindow();
			return true;
		}
		break;
	}
	return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
}
