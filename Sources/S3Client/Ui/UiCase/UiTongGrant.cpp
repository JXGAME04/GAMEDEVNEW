// ---------------------------------------------------------------------------
// Cua so PHAT NGAN LUONG / DIEM CONG HIEN theo CHUC VU - xem UiTongGrant.h.
// Vong doi (OpenWindow/LoadScheme/Wnd_AddWindow) theo dung khuon
// UiTongAssignBox.cpp; ini UiTongGrant.ini la ban port blueprint goc
// (da sua loi duong sprite cua [ImgTitle] - thieu thu muc con, xem commit).
// ---------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "../elem/wnds.h"
#include "../elem/wndmessage.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../Core/Src/KProtocol.h"
#include "UiTongGrant.h"
#include "UiTongJX2.h"

#define TONG_GRANT_INI "UiTongGrant.ini"

KUiTongGrant* KUiTongGrant::ms_pSelf = NULL;

KUiTongGrant::KUiTongGrant()
{
	m_bMoney = true;
	m_szForOnline[0] = 0;
	m_szForWhole[0] = 0;
	m_szImgMoney[0] = 0;
	m_szImgContribution[0] = 0;
}

KUiTongGrant::~KUiTongGrant()
{
}

KUiTongGrant* KUiTongGrant::OpenWindow(bool bMoney)
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongGrant;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->m_bMoney = bMoney;
		ms_pSelf->ApplyMode();
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}

KUiTongGrant* KUiTongGrant::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiTongGrant::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiTongGrant::Initialize()
{
	AddChild(&m_ImgTitle);
	AddChild(&m_TxtDirector);
	AddChild(&m_TxtManager);
	AddChild(&m_TxtMember);
	AddChild(&m_EditDirector);
	AddChild(&m_EditManager);
	AddChild(&m_EditMember);
	AddChild(&m_BtnDirectorTotal);
	AddChild(&m_BtnManagerTotal);
	AddChild(&m_BtnMemberTotal);
	AddChild(&m_BtnTotalMember);
	AddChild(&m_TextDescription);
	AddChild(&m_BtnConfirm);
	AddChild(&m_BtnCancel);

	char szBuff[128];
	g_UiBase.GetCurSchemePath(szBuff, sizeof(szBuff));
	LoadScheme(szBuff);
	Wnd_AddWindow(this, WL_TOPMOST);
	Hide();
}

void KUiTongGrant::LoadScheme(const char* pScheme)
{
	if (ms_pSelf == NULL || pScheme == NULL)
		return;
	char szBuff[256];
	sprintf(szBuff, "%s\\%s", pScheme, TONG_GRANT_INI);
	KIniFile Ini;
	if (!Ini.Load(szBuff))
		return;
	ms_pSelf->Init(&Ini, "Main");
	ms_pSelf->m_ImgTitle.Init(&Ini, "ImgTitle");
	ms_pSelf->m_TxtDirector.Init(&Ini, "TxtDirector");
	ms_pSelf->m_TxtManager.Init(&Ini, "TxtManager");
	ms_pSelf->m_TxtMember.Init(&Ini, "TxtMember");
	ms_pSelf->m_EditDirector.Init(&Ini, "EditDirector");
	ms_pSelf->m_EditManager.Init(&Ini, "EditManager");
	ms_pSelf->m_EditMember.Init(&Ini, "EditMember");
	ms_pSelf->m_BtnDirectorTotal.Init(&Ini, "BtnDirectorTotal");
	ms_pSelf->m_BtnManagerTotal.Init(&Ini, "BtnManagerTotal");
	ms_pSelf->m_BtnMemberTotal.Init(&Ini, "BtnMemberTotal");
	ms_pSelf->m_BtnTotalMember.Init(&Ini, "BtnTotalMember");
	ms_pSelf->m_TextDescription.Init(&Ini, "TextDescription");
	ms_pSelf->m_BtnConfirm.Init(&Ini, "BtnConfirm");
	ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
	// chuoi 2 che do cua blueprint
	Ini.GetString("Settings", "ForOnline", "", ms_pSelf->m_szForOnline,
		sizeof(ms_pSelf->m_szForOnline));
	Ini.GetString("Settings", "ForWhole", "", ms_pSelf->m_szForWhole,
		sizeof(ms_pSelf->m_szForWhole));
	Ini.GetString("ImgTitle", "ImageMoneyFund", "", ms_pSelf->m_szImgMoney,
		sizeof(ms_pSelf->m_szImgMoney));
	Ini.GetString("ImgTitle", "ImageContributionFund", "",
		ms_pSelf->m_szImgContribution, sizeof(ms_pSelf->m_szImgContribution));
}

void KUiTongGrant::ApplyMode()
{
	const char* szImg = m_bMoney ? m_szImgMoney : m_szImgContribution;
	if (szImg[0])
		m_ImgTitle.SetImage(ISI_T_SPR, (char*)szImg, false);
	m_TextDescription.SetText(m_BtnTotalMember.IsButtonChecked() ?
		m_szForOnline : m_szForWhole);
	m_EditDirector.SetText("0");
	m_EditManager.SetText("0");
	m_EditMember.SetText("0");
}

void KUiTongGrant::OnConfirm()
{
	char szE[16], szC[16], szM[16], szText[64];
	m_EditDirector.GetText(szE, sizeof(szE), false);
	m_EditManager.GetText(szC, sizeof(szC), false);
	m_EditMember.GetText(szM, sizeof(szM), false);
	int nE = atoi(szE);
	int nC = atoi(szC);
	int nM = atoi(szM);
	if (nE < 0) nE = 0;
	if (nC < 0) nC = 0;
	if (nM < 0) nM = 0;
	if (nE == 0 && nC == 0 && nM == 0)
		return;
	int nFlags = 0;
	if (m_BtnDirectorTotal.IsButtonChecked()) nFlags |= 1;
	if (m_BtnManagerTotal.IsButtonChecked()) nFlags |= 2;
	if (m_BtnMemberTotal.IsButtonChecked()) nFlags |= 4;
	if (m_BtnTotalMember.IsButtonChecked()) nFlags |= 8;
	if (m_bMoney) nFlags |= 16;
	sprintf(szText, "%d|%d|%d", nE, nC, nM);
	KUiTongJX2::SendOpStatic(defTONG_JX2_COP_GRANT_GROUP, 0, nFlags, 0, szText);
	CloseWindow();
}

int KUiTongGrant::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)&m_BtnConfirm)
		{
			OnConfirm();
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnCancel)
		{
			CloseWindow();
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnDirectorTotal ||
			uParam == (unsigned int)&m_BtnManagerTotal ||
			uParam == (unsigned int)&m_BtnMemberTotal)
		{
			KWndButton* pBtn = (KWndButton*)uParam;
			pBtn->CheckButton(!pBtn->IsButtonChecked());
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnTotalMember)
		{
			m_BtnTotalMember.CheckButton(!m_BtnTotalMember.IsButtonChecked());
			m_TextDescription.SetText(m_BtnTotalMember.IsButtonChecked() ?
				m_szForOnline : m_szForWhole);
			return 1;
		}
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
