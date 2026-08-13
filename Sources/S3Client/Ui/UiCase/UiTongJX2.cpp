/*******************************************************************************
File        : UiTongJX2.cpp
Description : Cua so bang hoi kieu JX2 - hien du lieu ban sao GS (goi
              TONG_JX2_*_SYNC) va gui thao tac (GTOI_TONG_JX2_VIEW/OP).
              Mo tu script server bang OpenTongJX2() (UI_OPENTONGJX2).
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "KPlayerDef.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"

#include "../UiBase.h"

#include "UiTongJX2.h"

#include "../../Core/Src/KProtocol.h"
#include "../../../Headers/KProtocolDef.h"

#define TONG_JX2_INI	"UiTongJX2.ini"

extern iCoreShell* g_pCoreShell;
extern int SCREEN_WIDTH;

KUiTongJX2* KUiTongJX2::ms_pSelf = NULL;

static const char* s_szTabName[TJX2_UI_TABS] =
{
	"Thong tin", "Thanh vien", "Quyen han", "Tac phuong", "Thong bao",
};

// ten 12 quyen theo thu tu mat na (KProtocol.h)
static const char* s_szRightName[12] =
{
	"1000 giao quyen", "1003 doi phe", "1101 quan ly", "1901 duoi nguoi",
	"1902 thoai an", "2001 nang cap", "2004 lanh dia", "2005 muc tieu tuan",
	"2006 tuyet ky", "2007 tuyen chien", "3001 quy bang", "9001 tac phuong",
};
static const DWORD s_dwRightId[12] =
{
	1000, 1003, 1101, 1901, 1902, 2001, 2004, 2005, 2006, 2007, 3001, 9001,
};

static const char* s_szWsName[8] =
{
	"", "Binh Giap", "Thien Cong", "Mat Na", "Thi Luyen", "Thien Y", "Le Vat", "Hoat Dong",
};

static const char* s_szFigure[5] =
{
	"Bang chu", "Truong lao", "Doi truong", "Bang chung", "An si",
};

//////////////////////////////////////////////////////////////////////

KUiTongJX2::KUiTongJX2()
{
	m_nPage = defTONG_JX2_PAGE_INFO;
	m_nStart = 0;
	m_nSel = 0;
	m_bHasInfo = 0;
	m_bHasMember = 0;
	m_bHasWs = 0;
	memset(m_byInfo, 0, sizeof(m_byInfo));
	memset(m_byMember, 0, sizeof(m_byMember));
	memset(m_byWs, 0, sizeof(m_byWs));
}

KUiTongJX2::~KUiTongJX2()
{
}

KUiTongJX2* KUiTongJX2::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongJX2;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->Show();
		ms_pSelf->SwitchPage(defTONG_JX2_PAGE_INFO);
	}
	return ms_pSelf;
}

KUiTongJX2* KUiTongJX2::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiTongJX2::CloseWindow(bool bDestroy)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestroy)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiTongJX2::Initialize()
{
	int i;
	for (i = 0; i < TJX2_UI_TABS; i++)
		AddChild(&m_BtnTab[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_Row[i]);
	for (i = 0; i < TJX2_UI_ACTS; i++)
		AddChild(&m_BtnAct[i]);
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_BtnClose);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	for (i = 0; i < TJX2_UI_TABS; i++)
		m_BtnTab[i].SetLabel(s_szTabName[i]);

	Wnd_AddWindow(this);
}

void KUiTongJX2::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[128];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, TONG_JX2_INI);
	if (!Ini.Load(Buff))
		return;

	if (SCREEN_WIDTH == 1024)
		ms_pSelf->Init(&Ini, "Main1024");
	else
		ms_pSelf->Init(&Ini, "Main");

	char szSec[32];
	int i;
	for (i = 0; i < TJX2_UI_TABS; i++)
	{
		sprintf(szSec, "Tab%d", i);
		ms_pSelf->m_BtnTab[i].Init(&Ini, szSec);
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		sprintf(szSec, "Row%d", i);
		ms_pSelf->m_Row[i].Init(&Ini, szSec);
	}
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		sprintf(szSec, "Act%d", i);
		ms_pSelf->m_BtnAct[i].Init(&Ini, szSec);
	}
	ms_pSelf->m_BtnPrev.Init(&Ini, "BtnPrev");
	ms_pSelf->m_BtnNext.Init(&Ini, "BtnNext");
	ms_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::RequestPage(int nPage, int nStart)
{
	if (g_pCoreShell)
		g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, (unsigned int)nPage, nStart);
}

void KUiTongJX2::SendOp(int nOp, unsigned long dwTarget, int nP1, int nP2, const char* pszText)
{
	if (!g_pCoreShell)
		return;
	KUiTongJX2Op sOp;
	memset(&sOp, 0, sizeof(sOp));
	sOp.nOp = nOp;
	sOp.dwTarget = dwTarget;
	sOp.nParam1 = nP1;
	sOp.nParam2 = nP2;
	if (pszText)
		strncpy(sOp.szText, pszText, sizeof(sOp.szText) - 1);
	g_pCoreShell->TongOperation(GTOI_TONG_JX2_OP, (unsigned int)&sOp, 0);
	// xin lai trang sau khi thao tac (relay echo ve GS truoc khi minh hoi lai)
	RequestPage(m_nPage, m_nStart);
}

void KUiTongJX2::DataArrive(unsigned char* pData, int nLen)
{
	if (nLen < 0)
	{
		// lenh mo cua so tu server (OpenTongJX2)
		OpenWindow();
		return;
	}
	if (!ms_pSelf || !pData || nLen < 5)
		return;

	int nPage = pData[4];	// {BYTE ProtocolType; WORD wLength; BYTE btMsgId;} + btPage
	switch (nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		if (nLen <= (int)sizeof(ms_pSelf->m_byInfo))
		{
			memcpy(ms_pSelf->m_byInfo, pData, nLen);
			ms_pSelf->m_bHasInfo = 1;
			if (ms_pSelf->m_nPage == defTONG_JX2_PAGE_INFO)
				ms_pSelf->RenderInfo();
		}
		break;
	case defTONG_JX2_PAGE_MEMBER:
	case defTONG_JX2_PAGE_RIGHT:
		if (nLen <= (int)sizeof(ms_pSelf->m_byMember))
		{
			memcpy(ms_pSelf->m_byMember, pData, nLen);
			ms_pSelf->m_bHasMember = 1;
			if (ms_pSelf->m_nPage == defTONG_JX2_PAGE_MEMBER ||
				ms_pSelf->m_nPage == defTONG_JX2_PAGE_RIGHT)
				ms_pSelf->RenderMembers();
		}
		break;
	case defTONG_JX2_PAGE_WS:
		if (nLen <= (int)sizeof(ms_pSelf->m_byWs))
		{
			memcpy(ms_pSelf->m_byWs, pData, nLen);
			ms_pSelf->m_bHasWs = 1;
			if (ms_pSelf->m_nPage == defTONG_JX2_PAGE_WS)
				ms_pSelf->RenderWorkshop();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::ClearRows()
{
	for (int i = 0; i < TJX2_UI_ROWS; i++)
		m_Row[i].SetText("");
}

void KUiTongJX2::SwitchPage(int nPage)
{
	m_nPage = nPage;
	m_nStart = 0;
	m_nSel = (nPage == defTONG_JX2_PAGE_WS) ? 1 : 0;
	ClearRows();
	SetupActions();
	if (nPage == 4)
		RenderAnnounce();	// trang thong bao dung du lieu INFO
	RequestPage(nPage == 4 ? defTONG_JX2_PAGE_INFO : nPage, 0);
}

void KUiTongJX2::SetupActions()
{
	int i;
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		m_BtnAct[i].SetLabel("");
		m_BtnAct[i].Enable(false);
	}
	switch (m_nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		m_BtnAct[0].SetLabel("Gop 10 van");
		m_BtnAct[1].SetLabel("Gop 100 van");
		m_BtnAct[2].SetLabel("NANG CAP bang");
		m_BtnAct[3].SetLabel("Ha cap bang");
		m_BtnAct[4].SetLabel("Tuyet ky 1");
		m_BtnAct[5].SetLabel("Lam moi");
		break;
	case defTONG_JX2_PAGE_MEMBER:
		m_BtnAct[0].SetLabel("Duoi nguoi chon");
		m_BtnAct[1].SetLabel("Phat 10 cong hien");
		m_BtnAct[5].SetLabel("Chon >>");
		break;
	case defTONG_JX2_PAGE_RIGHT:
		m_BtnAct[0].SetLabel("Cap 1901");
		m_BtnAct[1].SetLabel("Cap 3001");
		m_BtnAct[2].SetLabel("Cap 9001");
		m_BtnAct[3].SetLabel("Cap 2006");
		m_BtnAct[4].SetLabel("Thu 2006");
		m_BtnAct[5].SetLabel("Chon >>");
		break;
	case defTONG_JX2_PAGE_WS:
		m_BtnAct[0].SetLabel("Lap khu chon");
		m_BtnAct[1].SetLabel("Mo / Dong");
		m_BtnAct[2].SetLabel("Nang cap khu");
		m_BtnAct[5].SetLabel("Chon khu >>");
		break;
	case 4:	// thong bao
		m_BtnAct[0].SetLabel("Dat TB mau");
		m_BtnAct[1].SetLabel("Xoa thong bao");
		m_BtnAct[5].SetLabel("Lam moi");
		break;
	}
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		char szLabel[8];
		if (m_BtnAct[i].GetLabel(szLabel, sizeof(szLabel)) > 0)
			m_BtnAct[i].Enable(true);
	}
}

void KUiTongJX2::RenderInfo()
{
	if (!m_bHasInfo)
		return;
	TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
	char sz[120];
	ClearRows();
	sprintf(sz, "Bang: %s   Bang chu: %s", p->m_szTongName, p->m_szMaster);
	m_Row[0].SetText(sz);
	sprintf(sz, "Cap bang: %d   Kinh nghiem: %d   Thanh vien: %d", p->m_nLevel, p->m_nExp, (int)p->m_wMemberTotal);
	m_Row[1].SetText(sz);
	sprintf(sz, "Ngan quy: %.0f luong", (double)p->m_nMoney);
	m_Row[2].SetText(sz);
	sprintf(sz, "Quy kien thiet: %u   (tuan nay %u / tran %u)", p->m_dwBuildFund, p->m_dwWeekBuild, p->m_dwWeekUpper);
	m_Row[3].SetText(sz);
	sprintf(sz, "Quy chien bi: %u   Duy tri/ngay: %u", p->m_dwWarFund, p->m_dwMaintain);
	m_Row[4].SetText(sz);
	sprintf(sz, "Quy du tru: %u   Kien thiet du tru: %u", p->m_dwStoredOffer, p->m_dwStoredBuild);
	m_Row[5].SetText(sz);
	sprintf(sz, "Tro cap moi nguoi: %u", p->m_dwPerStand);
	m_Row[6].SetText(sz);
	sprintf(sz, "Ngay hoat dong: %d   Tuan: %d", p->m_nDay, p->m_nWeek);
	m_Row[7].SetText(sz);
	if (p->m_dwStuntID && p->m_dwStuntID < 8)
		sprintf(sz, "Tuyet ky: %s (%s)", s_szWsName[0] == NULL ? "" : "", "");
	if (p->m_dwStuntID)
		sprintf(sz, "Tuyet ky ID %u (%s)", p->m_dwStuntID, p->m_dwStuntOn ? "hieu luc" : "thieu chien bi");
	else
		sprintf(sz, "Tuyet ky: chua dat");
	m_Row[8].SetText(sz);
	sprintf(sz, "Chuc vu cua ban: %s   Cong hien: %u",
		p->m_btMyFigure < 5 ? s_szFigure[p->m_btMyFigure] : "?", p->m_dwMyOffer);
	m_Row[9].SetText(sz);
	if (p->m_szAnnounce[0])
	{
		sprintf(sz, "Thong bao: %.90s", p->m_szAnnounce);
		m_Row[10].SetText(sz);
	}
}

void KUiTongJX2::RenderMembers()
{
	if (!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* p = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	char sz[120];
	ClearRows();
	sprintf(sz, "%s (%d-%d / tong %d)",
		m_nPage == defTONG_JX2_PAGE_RIGHT ? "Bang chu + truong lao" : "Thanh vien",
		(int)p->m_wStart + 1, (int)p->m_wStart + p->m_btCount, (int)p->m_wTotal);
	m_Row[0].SetText(sz);
	int i;
	for (i = 0; i < (int)p->m_btCount && i + 1 < TJX2_UI_ROWS; i++)
	{
		TONG_JX2_ONE_MEMBER* pM = &p->m_sMember[i];
		if (m_nPage == defTONG_JX2_PAGE_RIGHT)
		{
			// liet ke quyen dang co theo mat na
			char szR[64];
			szR[0] = 0;
			int nR = 0;
			for (int b = 0; b < 12 && nR < 4; b++)
			{
				if (pM->m_wRights & (1 << b))
				{
					char szOne[12];
					sprintf(szOne, "%u ", s_dwRightId[b]);
					strcat(szR, szOne);
					nR++;
				}
			}
			sprintf(sz, "%s%-16s %-10s quyen: %s", (i == m_nSel) ? "> " : "  ",
				pM->m_szName, pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?",
				szR[0] ? szR : "(khong)");
		}
		else
		{
			sprintf(sz, "%s%-16s %-10s ch:%u %s", (i == m_nSel) ? "> " : "  ",
				pM->m_szName, pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?",
				pM->m_dwOffer, pM->m_btOnline ? "[ON]" : "");
		}
		m_Row[i + 1].SetText(sz);
	}
}

void KUiTongJX2::RenderWorkshop()
{
	if (!m_bHasWs)
		return;
	TONG_JX2_WS_SYNC* p = (TONG_JX2_WS_SYNC*)m_byWs;
	char sz[120];
	ClearRows();
	m_Row[0].SetText("Tac phuong (7 khu):");
	for (int t = 1; t <= 7; t++)
	{
		TONG_JX2_ONE_WS* pW = &p->m_sWs[t];
		if (pW->btExist)
			sprintf(sz, "%s%d. %-10s cap %d  [%s]  san luong %u", (t == m_nSel) ? "> " : "  ",
				t, s_szWsName[t], (int)pW->wLevel, pW->btOpen ? "MO" : "DONG", pW->dwOutput);
		else
			sprintf(sz, "%s%d. %-10s (chua lap)", (t == m_nSel) ? "> " : "  ", t, s_szWsName[t]);
		m_Row[t].SetText(sz);
	}
}

void KUiTongJX2::RenderAnnounce()
{
	ClearRows();
	m_Row[0].SetText("Thong bao bang:");
	if (m_bHasInfo)
	{
		TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
		char sz[120];
		if (p->m_szAnnounce[0])
		{
			sprintf(sz, "%.100s", p->m_szAnnounce);
			m_Row[2].SetText(sz);
		}
		else
			m_Row[2].SetText("(chua co thong bao)");
	}
	m_Row[5].SetText("Chi bang chu duoc dat / xoa thong bao.");
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::OnAction(int nIdx)
{
	TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	DWORD dwTarget = 0;
	if (m_bHasMember && m_nSel < (int)pM->m_btCount)
		dwTarget = pM->m_sMember[m_nSel].m_dwNameID;

	switch (m_nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		if (nIdx == 0)
			SendOp(defTONG_JX2_COP_DONATE, 0, 10, 0, NULL);
		else if (nIdx == 1)
			SendOp(defTONG_JX2_COP_DONATE, 0, 100, 0, NULL);
		else if (nIdx == 2)
			SendOp(defTONG_JX2_COP_UPGRADE, 0, 0, 0, NULL);
		else if (nIdx == 3)
			SendOp(defTONG_JX2_COP_DEGRADE, 0, 0, 0, NULL);
		else if (nIdx == 4)
			SendOp(defTONG_JX2_COP_SETSTUNT, 0, 1, 0, NULL);
		else if (nIdx == 5)
			RequestPage(m_nPage, m_nStart);
		break;

	case defTONG_JX2_PAGE_MEMBER:
		if (nIdx == 0 && dwTarget)
			SendOp(defTONG_JX2_COP_KICK, dwTarget, 0, 0, NULL);
		else if (nIdx == 1 && dwTarget)
			SendOp(defTONG_JX2_COP_GRANT, dwTarget, 10, 0, NULL);
		else if (nIdx == 5)
		{
			if (m_bHasMember && pM->m_btCount)
				m_nSel = (m_nSel + 1) % pM->m_btCount;
			RenderMembers();
		}
		break;

	case defTONG_JX2_PAGE_RIGHT:
		if (nIdx == 5)
		{
			if (m_bHasMember && pM->m_btCount)
				m_nSel = (m_nSel + 1) % pM->m_btCount;
			RenderMembers();
		}
		else if (dwTarget)
		{
			if (nIdx == 0)
				SendOp(defTONG_JX2_COP_ADDRIGHT, dwTarget, 1901, 0, NULL);
			else if (nIdx == 1)
				SendOp(defTONG_JX2_COP_ADDRIGHT, dwTarget, 3001, 0, NULL);
			else if (nIdx == 2)
				SendOp(defTONG_JX2_COP_ADDRIGHT, dwTarget, 9001, 0, NULL);
			else if (nIdx == 3)
				SendOp(defTONG_JX2_COP_ADDRIGHT, dwTarget, 2006, 0, NULL);
			else if (nIdx == 4)
				SendOp(defTONG_JX2_COP_DELRIGHT, dwTarget, 2006, 0, NULL);
		}
		break;

	case defTONG_JX2_PAGE_WS:
		if (nIdx == 0)
			SendOp(defTONG_JX2_COP_WS_ADD, 0, m_nSel, 0, NULL);
		else if (nIdx == 1)
		{
			TONG_JX2_WS_SYNC* pW = (TONG_JX2_WS_SYNC*)m_byWs;
			if (m_bHasWs && m_nSel >= 1 && m_nSel <= 7)
				SendOp(pW->m_sWs[m_nSel].btOpen ? defTONG_JX2_COP_WS_CLOSE : defTONG_JX2_COP_WS_OPEN,
					0, m_nSel, 0, NULL);
		}
		else if (nIdx == 2)
			SendOp(defTONG_JX2_COP_WS_UP, 0, m_nSel, 0, NULL);
		else if (nIdx == 5)
		{
			m_nSel = m_nSel % 7 + 1;
			RenderWorkshop();
		}
		break;

	case 4:	// thong bao
		if (nIdx == 0)
			SendOp(defTONG_JX2_COP_SETANN, 0, 0, 0,
				"Chao mung den voi bang hoi! Online deu de nhan tro cap.");
		else if (nIdx == 1)
			SendOp(defTONG_JX2_COP_SETANN, 0, 0, 0, "");
		else if (nIdx == 5)
			RequestPage(defTONG_JX2_PAGE_INFO, 0);
		break;
	}
}

int KUiTongJX2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow(false);
			return 1;
		}
		{
			int i;
			for (i = 0; i < TJX2_UI_TABS; i++)
			{
				if (uParam == (unsigned int)&m_BtnTab[i])
				{
					SwitchPage(i == 4 ? 4 : i);
					return 1;
				}
			}
			for (i = 0; i < TJX2_UI_ACTS; i++)
			{
				if (uParam == (unsigned int)&m_BtnAct[i])
				{
					OnAction(i);
					return 1;
				}
			}
		}
		if (uParam == (unsigned int)&m_BtnPrev)
		{
			if (m_nStart >= defTONG_JX2_VIEW_MEMBERS)
				m_nStart -= defTONG_JX2_VIEW_MEMBERS;
			else
				m_nStart = 0;
			m_nSel = 0;
			RequestPage(m_nPage, m_nStart);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnNext)
		{
			TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
			if (m_bHasMember && m_nStart + defTONG_JX2_VIEW_MEMBERS < (int)pM->m_wTotal)
			{
				m_nStart += defTONG_JX2_VIEW_MEMBERS;
				m_nSel = 0;
				RequestPage(m_nPage, m_nStart);
			}
			return 1;
		}
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
