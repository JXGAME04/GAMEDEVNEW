//////////////////////////////////////////////////////////////////////
// UiTongKimInfo.cpp - [TKINFO 05/09 + 06/09] cua so "thong tin tran" Tong Kim (xem UiTongKimInfo.h)
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KIniFile.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../UiBase.h"
#include "UiTongKimInfo.h"
#include "../Elem/WndMessage.h"	// WND_N_BUTTON_CLICK
#include "../elem/wnds.h"		// KHONG co include guard -> de CUOI CUNG: Wnd_AddWindow, WL_TOPMOST
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
	memset(m_szImgFull, 0, sizeof(m_szImgFull));
	memset(m_szImgFold, 0, sizeof(m_szImgFold));
	m_nWidthFull = 0;
	m_nWidthFold = 27;
	m_nHeightFull = 0;
	m_nHeightFold = 27;
	m_bFold = 0;
	m_bUserFold = 0;
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
	AddChild(&m_BtnFold);	// [TKINFO 06/09] them SAU CUNG: TopChildFromPoint duyet nguoc nen nut nay o tren cung
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
	_snprintf(Buff, sizeof(Buff) - 1, "%s\\%s", pScheme, TKINFO_INI);
	Buff[sizeof(Buff) - 1] = 0;
	if (!Ini.Load(Buff))
		return;
	const char* pszSec = (SCREEN_WIDTH == 1024) ? "Main1024" : "Main";
	ms_pSelf->Init(&Ini, pszSec);
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
	ms_pSelf->m_BtnFold.Init(&Ini, "BtnFold");
	for (int p = 0; p < 4; p++)
	{
		sprintf(szSec, "P%d", p);
		Ini.GetString("Phase", szSec, "", ms_pSelf->m_szPhase[p], sizeof(ms_pSelf->m_szPhase[p]));
		sprintf(szSec, "C%d", p);
		Ini.GetString("Camp", szSec, "", ms_pSelf->m_szCamp[p], sizeof(ms_pSelf->m_szCamp[p]));
	}
	// [TKINFO 06/09] hai anh nen cho hai trang thai. KWndImage::PaintWindow ve NGUYEN tam SPR va KHONG cat
	// theo m_Height, nen "thu gon" bat buoc phai DOI ANH chu khong the chi SetSize.
	Ini.GetString(pszSec, "Image", "", ms_pSelf->m_szImgFull, sizeof(ms_pSelf->m_szImgFull));
	Ini.GetString(pszSec, "ImageFold", "", ms_pSelf->m_szImgFold, sizeof(ms_pSelf->m_szImgFold));
	if (!ms_pSelf->m_szImgFold[0])
		strcpy(ms_pSelf->m_szImgFold, ms_pSelf->m_szImgFull);
	Ini.GetInteger(pszSec, "HeightFold", 27, &ms_pSelf->m_nHeightFold);
	if (ms_pSelf->m_nHeightFold <= 0)
		ms_pSelf->m_nHeightFold = 27;
	// [TKINFO 06/09 toi] thu gon con dung O NUT -> phai thu ca BE NGANG, khong chi chieu cao
	Ini.GetInteger(pszSec, "WidthFold", 27, &ms_pSelf->m_nWidthFold);
	if (ms_pSelf->m_nWidthFold <= 0)
		ms_pSelf->m_nWidthFold = 27;
	int nW = 0, nH = 0;
	ms_pSelf->GetSize(&nW, &nH);
	ms_pSelf->m_nWidthFull = nW;
	ms_pSelf->m_nHeightFull = nH;
	ms_pSelf->m_bFold = 0;
}

// [TKINFO 06/09 toi] thu gon con DUNG O NUT 27x27 / mo ra. Chi nut thu gon la luon hien.
void KUiTongKimInfo::SetFold(int bFold)
{
	m_bFold = bFold ? 1 : 0;
	int i;
	for (i = 0; i < TKINFO_MAX_ROW; i++)
	{
		if (m_bFold) { m_Rank[i].Hide(); m_Name[i].Hide(); m_Camp[i].Hide(); m_Point[i].Hide(); }
		else		 { m_Rank[i].Show(); m_Name[i].Show(); m_Camp[i].Show(); m_Point[i].Show(); }
	}
	if (m_bFold)
	{
		// [TKINFO 06/09 toi] an ca TIEU DE: o nut chi rong 27 px, chu tieu de can 221 px se tran ra ngoai
		m_Title.Hide();
		m_StageLabel.Hide(); m_Stage.Hide(); m_CountLabel.Hide(); m_Count.Hide(); m_CountSuffix.Hide();
		m_HdRank.Hide(); m_HdName.Hide(); m_HdCamp.Hide(); m_HdPoint.Hide();
	}
	else
	{
		m_Title.Show();
		m_StageLabel.Show(); m_Stage.Show(); m_CountLabel.Show(); m_Count.Show(); m_CountSuffix.Show();
		m_HdRank.Show(); m_HdName.Show(); m_HdCamp.Show(); m_HdPoint.Show();
	}
	if (m_szImgFull[0])
		SetImage(ISI_T_SPR, m_bFold ? m_szImgFold : m_szImgFull);
	SetSize(m_bFold ? m_nWidthFold : m_nWidthFull,
			m_bFold ? m_nHeightFold : m_nHeightFull);
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
	// [TKINFO 06/09] ton trong lua chon cua nguoi choi: da thu gon thi khong tu bung ra khi co du lieu moi
	if (m_bFold != m_bUserFold)
		SetFold(m_bUserFold);
	if (!IsVisible())
	{
		Show();
		BringToTop();
	}
}

// "n;hang|ten|phe|diem;hang|ten|phe|diem..." (hang thieu -> xoa trang)
void KUiTongKimInfo::SetRows(const char* pszRows)
{
	// [TKINFO 06/09] dung dung co truong S2C_BATTLE_BOX::szBattleDesc[128] (KProtocol.h) - truoc day de 160
	char szTmp[128];
	strncpy(szTmp, pszRows ? pszRows : "", sizeof(szTmp) - 1);
	szTmp[sizeof(szTmp) - 1] = 0;
	int nRow = 0;
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
		if (q) { *q = 0; strncpy(szRank, p, sizeof(szRank) - 1); p = q + 1; }
		q = strchr(p, '|');
		if (q) { *q = 0; strncpy(szName, p, sizeof(szName) - 1); p = q + 1; }
		q = strchr(p, '|');
		if (q) { *q = 0; nCamp = atoi(p); p = q + 1; }
		strncpy(szPoint, p, sizeof(szPoint) - 1);
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
	if (m_bHaveData && IsVisible() && !m_bFold)
		RefreshCountdown();
}

void KUiTongKimInfo::OnBattleBox(const char* pszDesc, int nType)
{
	if (nType != TKINFO_KIND_HEAD && nType != TKINFO_KIND_ROWS && nType != TKINFO_KIND_HIDE)
		return;
	if (nType == TKINFO_KIND_HIDE)
	{
		// [TKINFO 06/09] KHONG tao cua so chi de an no (truoc day goi OpenWindow o day co the sinh lai
		// cua so ngay sau khi UiShell da CloseWindow(TRUE) luc thoat game).
		if (ms_pSelf)
		{
			ms_pSelf->m_bHaveData = 0;
			ms_pSelf->m_bUserFold = 0;	// tran sau mo lai binh thuong
			ms_pSelf->Hide();
		}
		return;
	}
	KUiTongKimInfo* p = OpenWindow();
	if (!p || !pszDesc)
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
		ms_pSelf->m_bUserFold = 0;
		ms_pSelf->Hide();
	}
}

// [TKINFO 06/09] Cua so nam o lop WL_TOPMOST: neu bat chuot tren ca 221x232 thi suot tran nguoi choi khong
// bam duoc xuong dat/muc tieu o goc trai man hinh (Wnds.cpp Wnd_GetActive chon cua so theo PtInWindow).
// [TKINFO 06/09 toi] Da bo nut Chien Bao nen khi MO chi con NUT THU GON bat chuot (ca dai tieu de lan than
// cua so deu cho xuyen xuong the gioi game); khi THU GON thi ca o nut 27x27 deu bat cho de bam trung.
int KUiTongKimInfo::PtInWindow(int x, int y)
{
	if (!IsVisible())
		return 0;
	int nW = m_bFold ? m_nWidthFold : m_nWidthFull;
	int nH = m_bFold ? m_nHeightFold : m_nHeightFull;
	if (x < m_nAbsoluteLeft || x >= m_nAbsoluteLeft + nW)
		return 0;
	if (y < m_nAbsoluteTop || y >= m_nAbsoluteTop + nH)
		return 0;
	if (m_bFold)
		return 1;
	return m_BtnFold.PtInWindow(x, y) ? 1 : 0;
}

int KUiTongKimInfo::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (KUPARAM)(KWndWindow*)&m_BtnFold)
		{
			// [TKINFO 06/09 toi] "nut an vao": thu con dung o nut 27x27, bam lan nua thi mo ra
			m_bUserFold = m_bFold ? 0 : 1;
			SetFold(m_bUserFold);
			return 0;
		}
		// [TKINFO 06/09 toi] chu 06/09 toi: "nhan xem chien bao chua hien thong tin len do nen bo luon phan xem
		// chien bao di" -> nut [BtnReport] da bi xoa han khoi cua so nay.
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
