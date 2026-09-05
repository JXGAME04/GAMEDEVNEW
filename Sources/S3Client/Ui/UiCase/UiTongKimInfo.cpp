//////////////////////////////////////////////////////////////////////
// UiTongKimInfo.cpp - [TKINFO 05/09] cua so "thong tin tran" Tong Kim kieu Lien Dau Bang 2.0 (xem UiTongKimInfo.h)
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KIniFile.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../elem/wnds.h"		// mot lan duy nhat (khong co include guard): Wnd_AddWindow, WL_TOPMOST
#include "../Elem/WndMessage.h"	// WND_N_BUTTON_CLICK
#include "../UiBase.h"
#include "UiTongKimInfo.h"
#include "UiBattleReport.h"
#include <stdlib.h>
#include <string.h>

#define TKINFO_INI			"UiTongKimInfo.ini"
#define TKINFO_KIND_HEAD	7	// "phase|rest|tong|kim"
#define TKINFO_KIND_ROWS	8	// "n;hang|ten|phe|diem;..."
#define TKINFO_KIND_HIDE	9

extern int SCREEN_WIDTH;

KUiTongKimInfo* KUiTongKimInfo::ms_pSelf = NULL;

KUiTongKimInfo::KUiTongKimInfo()
{
	memset(m_szPhase, 0, sizeof(m_szPhase));
	memset(m_szCamp, 0, sizeof(m_szCamp));
	m_nRestSec = 0;
	m_uRestTick = 0;
	m_nLastShown = -1;
	m_bHaveData = 0;
}

KUiTongKimInfo* KUiTongKimInfo::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongKimInfo;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	return ms_pSelf;
}

void KUiTongKimInfo::CloseWindow(BOOL bDestroy)
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

void KUiTongKimInfo::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_StageLabel);
	AddChild(&m_Stage);
	AddChild(&m_CountLabel);
	AddChild(&m_Count);
	AddChild(&m_CountSuffix);
	AddChild(&m_HdRank);
	AddChild(&m_HdName);
	AddChild(&m_HdCamp);
	AddChild(&m_HdPoint);
	for (int i = 0; i < TKINFO_MAX_ROW; i++)
	{
		AddChild(&m_Rank[i]);
		AddChild(&m_Name[i]);
		AddChild(&m_Camp[i]);
		AddChild(&m_Point[i]);
	}
	AddChild(&m_BtnReport);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
	Hide();	// chi hien khi may chu gui du lieu tran
}

void KUiTongKimInfo::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf || !pScheme || !pScheme[0])
		return;
	char Buff[256];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, TKINFO_INI);
	if (!Ini.Load(Buff))
		return;
	if (SCREEN_WIDTH == 1024)
		ms_pSelf->Init(&Ini, "Main1024");
	else
		ms_pSelf->Init(&Ini, "Main");
	ms_pSelf->m_Title.Init(&Ini, "Title");
	ms_pSelf->m_StageLabel.Init(&Ini, "StageLabel");
	ms_pSelf->m_Stage.Init(&Ini, "Stage");
	ms_pSelf->m_CountLabel.Init(&Ini, "CountLabel");
	ms_pSelf->m_Count.Init(&Ini, "Count");
	ms_pSelf->m_CountSuffix.Init(&Ini, "CountSuffix");
	ms_pSelf->m_HdRank.Init(&Ini, "HdRank");
	ms_pSelf->m_HdName.Init(&Ini, "HdName");
	ms_pSelf->m_HdCamp.Init(&Ini, "HdCamp");
	ms_pSelf->m_HdPoint.Init(&Ini, "HdPoint");
	char szSec[32];
	for (int i = 0; i < TKINFO_MAX_ROW; i++)
	{
		sprintf(szSec, "Rank_%d", i);	ms_pSelf->m_Rank[i].Init(&Ini, szSec);
		sprintf(szSec, "Name_%d", i);	ms_pSelf->m_Name[i].Init(&Ini, szSec);
		sprintf(szSec, "Camp_%d", i);	ms_pSelf->m_Camp[i].Init(&Ini, szSec);
		sprintf(szSec, "Point_%d", i);	ms_pSelf->m_Point[i].Init(&Ini, szSec);
	}
	ms_pSelf->m_BtnReport.Init(&Ini, "BtnReport");
	for (int p = 0; p < 4; p++)
	{
		sprintf(szSec, "P%d", p);
		Ini.GetString("Phase", szSec, "", ms_pSelf->m_szPhase[p], sizeof(ms_pSelf->m_szPhase[p]));
		sprintf(szSec, "C%d", p);
		Ini.GetString("Camp", szSec, "", ms_pSelf->m_szCamp[p], sizeof(ms_pSelf->m_szCamp[p]));
	}
}

// "phase|rest|tong|kim"
void KUiTongKimInfo::SetHeader(int nPhase, int nRestSec, int nTong, int nKim)
{
	if (nPhase < 0 || nPhase > 3)
		nPhase = 0;
	m_Stage.SetText(m_szPhase[nPhase]);
	m_nRestSec = nRestSec < 0 ? 0 : nRestSec;
	m_uRestTick = IR_GetCurrentTime();
	m_nLastShown = -1;
	m_bHaveData = 1;
	RefreshCountdown();
	if (!IsVisible())
	{
		Show();
		BringToTop();
	}
}

// "n;hang|ten|phe|diem;hang|ten|phe|diem..." (hang thieu -> xoa trang)
void KUiTongKimInfo::SetRows(const char* pszRows)
{
	char szTmp[160];
	strncpy(szTmp, pszRows ? pszRows : "", sizeof(szTmp) - 1);
	szTmp[sizeof(szTmp) - 1] = 0;
	int nRow = 0;
	char* pCtx = NULL;
	char* pRec = strtok(szTmp, ";");	// phan tu dau = so hang (tham khao)
	if (pRec)
		pRec = strtok(NULL, ";");
	while (pRec && nRow < TKINFO_MAX_ROW)
	{
		// hang|ten|phe|diem
		char szRank[16] = "", szName[32] = "", szPoint[16] = "";
		int nCamp = 0;
		char* p = pRec;
		char* q = strchr(p, '|');
		if (q) { *q = 0; strncpy(szRank, p, 15); p = q + 1; }
		q = strchr(p, '|');
		if (q) { *q = 0; strncpy(szName, p, 31); p = q + 1; }
		q = strchr(p, '|');
		if (q) { *q = 0; nCamp = atoi(p); p = q + 1; }
		strncpy(szPoint, p, 15);
		if (nCamp < 0 || nCamp > 3) nCamp = 0;
		m_Rank[nRow].SetText(szRank);
		m_Name[nRow].SetText(szName);
		m_Camp[nRow].SetText(m_szCamp[nCamp]);
		m_Point[nRow].SetText(szPoint);
		nRow++;
		pRec = strtok(NULL, ";");
	}
	for (; nRow < TKINFO_MAX_ROW; nRow++)
	{
		m_Rank[nRow].SetText(" ");
		m_Name[nRow].SetText(" ");
		m_Camp[nRow].SetText(" ");
		m_Point[nRow].SetText(" ");
	}
	if (m_bHaveData && !IsVisible())
	{
		Show();
		BringToTop();
	}
}

void KUiTongKimInfo::RefreshCountdown()
{
	int nElapsed = (int)((IR_GetCurrentTime() - m_uRestTick) / 1000);
	int nRest = m_nRestSec - nElapsed;
	if (nRest < 0)
		nRest = 0;
	if (nRest != m_nLastShown)
	{
		m_nLastShown = nRest;
		char szBuf[32];
		sprintf(szBuf, "%d", nRest);
		m_Count.SetText(szBuf);
	}
}

void KUiTongKimInfo::Breathe()
{
	// KWndWindow::Breathe la private (khong goi lop cha - nhu KUiFlashMessage)
	if (m_bHaveData && IsVisible())
		RefreshCountdown();
}

void KUiTongKimInfo::OnBattleBox(const char* pszDesc, int nType)
{
	if (nType != TKINFO_KIND_HEAD && nType != TKINFO_KIND_ROWS && nType != TKINFO_KIND_HIDE)
		return;
	KUiTongKimInfo* p = OpenWindow();
	if (!p)
		return;
	if (nType == TKINFO_KIND_HIDE)
	{
		p->m_bHaveData = 0;
		p->Hide();
		return;
	}
	if (!pszDesc)
		return;
	if (nType == TKINFO_KIND_ROWS)
	{
		p->SetRows(pszDesc);
		return;
	}
	char szTmp[128];
	strncpy(szTmp, pszDesc, sizeof(szTmp) - 1);
	szTmp[sizeof(szTmp) - 1] = 0;
	int nVal[4] = { 0, 0, 0, 0 };
	char* pTok = strtok(szTmp, "|");
	for (int i = 0; i < 4 && pTok; i++)
	{
		nVal[i] = atoi(pTok);
		pTok = strtok(NULL, "|");
	}
	p->SetHeader(nVal[0], nVal[1], nVal[2], nVal[3]);
}

void KUiTongKimInfo::OnSwitchMap(int bLoading)
{
	if (ms_pSelf && bLoading)
	{
		ms_pSelf->m_bHaveData = 0;
		ms_pSelf->Hide();
	}
}

int KUiTongKimInfo::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnReport)
		{
			// "Nhan xem Chien Bao": mo bang chien bao day du (top 10) co san
			KUiBattleReport::OpenWindow();
			return 0;
		}
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
