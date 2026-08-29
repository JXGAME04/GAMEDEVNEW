/*******************************************************************************
File        : UiPartnerSkill.cpp
create data : 27-08-2026
Description : [BDH-G4] Cua so KY NANG ban dong hanh (xem UiPartnerSkill.h).
*******************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiPartnerCommon.h"
#include "UiPartnerSkill.h"
#include "UiPartnerAttr.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"

#define SCHEME_INI_PARTNER_SKILL "\xCD\xAC\xB0\xE9\xBC\xBC\xC4\xDC.ini"

KUiPartnerSkill* KUiPartnerSkill::ms_pSelf = NULL;

KUiPartnerSkill::KUiPartnerSkill()
{
    m_nView = 0;
}

KUiPartnerSkill::~KUiPartnerSkill()
{
}

KUiPartnerSkill* KUiPartnerSkill::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

KUiPartnerSkill* KUiPartnerSkill::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiPartnerSkill;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->m_nView = PT_Cur();
        if (ms_pSelf->m_nView == 0)
            ms_pSelf->m_nView = 1;
        ms_pSelf->UpdateData();
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

void KUiPartnerSkill::CloseWindow()
{
    if (ms_pSelf)
    {
        ms_pSelf->Hide();
        ms_pSelf = NULL;
    }
}

void KUiPartnerSkill::OnTaskValueChanged()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        ms_pSelf->UpdateData();
}

void KUiPartnerSkill::SetView(int nP)
{
    if (nP >= 1 && nP <= PARTNER_MAX_COUNT)
    {
        m_nView = nP;
        UpdateData();
    }
}

void KUiPartnerSkill::Initialize()
{
    int i, j;
    AddChild(&m_Title);
    AddChild(&m_PagePartner);
    for (i = 0; i < PARTNER_UI_SLOT; i++)
        AddChild(&m_BtnTab[i]);
    AddChild(&m_Forget);
    AddChild(&m_SaveBtn);
    AddChild(&m_DeleteBtn);
    AddChild(&m_CloseBtn);
    AddChild(&m_SwitchBtn);
    for (i = 0; i < PARTNER_UI_SLOT; i++)
    {
        for (j = 0; j < PARTNER_SKILL_GIVEN_COL; j++)
            AddChild(&m_Given[i][j]);
        for (j = 0; j < PARTNER_SKILL_GRID_COL; j++)
            AddChild(&m_Learnt[i][j]);
    }

    char Scheme[256];
    KIniFile Ini;
    g_UiBase.GetCurSchemePath(Scheme, 256);
    strcat(Scheme, "\\" SCHEME_INI_PARTNER_SKILL);
    if (Ini.Load(Scheme))
    {
        Init(&Ini, "Main");
        m_Title.Init(&Ini, "Title");
        m_PagePartner.Init(&Ini, "PagePartner");
        char szSec[32];
        for (i = 0; i < PARTNER_UI_SLOT; i++)
        {
            sprintf(szSec, "BtnPartner_%d", i);
            m_BtnTab[i].Init(&Ini, szSec);
        }
        m_Forget.Init(&Ini, "Forget");
        m_SaveBtn.Init(&Ini, "SaveBtn");
        m_DeleteBtn.Init(&Ini, "DeleteBtn");
        m_CloseBtn.Init(&Ini, "CloseBtn");
        m_SwitchBtn.Init(&Ini, "SwitchBtn");

        // 3 cot bam sinh: [KongfuIcon][ToughIcon][MissleIcon]
        // XStart/YStart/YDistance/Width/Height (XDistance = 0 - cot doc)
        static const char* szCol[PARTNER_SKILL_GIVEN_COL] =
            { "KongfuIcon", "ToughIcon", "MissleIcon" };
        int nX, nY, nDY, nW, nH, nDX;
        for (j = 0; j < PARTNER_SKILL_GIVEN_COL; j++)
        {
            nX = nY = nDY = 0; nW = nH = 36;
            Ini.GetInteger(szCol[j], "XStart", 0, &nX);
            Ini.GetInteger(szCol[j], "YStart", 0, &nY);
            Ini.GetInteger(szCol[j], "YDistance", 51, &nDY);
            Ini.GetInteger(szCol[j], "Width", 36, &nW);
            Ini.GetInteger(szCol[j], "Height", 36, &nH);
            for (i = 0; i < PARTNER_UI_SLOT; i++)
            {
                m_Given[i][j].SetPosition(nX, nY + nDY * i);
                m_Given[i][j].SetSize(nW, nH);
            }
        }
        // luoi tu hoc [SkillsIcon]: XStart/XDistance/YStart/YDistance
        nX = nY = 0; nDX = 39; nDY = 51; nW = nH = 36;
        Ini.GetInteger("SkillsIcon", "XStart", 208, &nX);
        Ini.GetInteger("SkillsIcon", "YStart", 68, &nY);
        Ini.GetInteger("SkillsIcon", "XDistance", 39, &nDX);
        Ini.GetInteger("SkillsIcon", "YDistance", 51, &nDY);
        Ini.GetInteger("SkillsIcon", "Width", 36, &nW);
        Ini.GetInteger("SkillsIcon", "Height", 36, &nH);
        for (i = 0; i < PARTNER_UI_SLOT; i++)
            for (j = 0; j < PARTNER_SKILL_GRID_COL; j++)
            {
                m_Learnt[i][j].SetPosition(nX + nDX * j, nY + nDY * i);
                m_Learnt[i][j].SetSize(nW, nH);
            }
    }
    Wnd_AddWindow(this);
}

void KUiPartnerSkill::UpdateData()
{
    int i, j;
    for (i = 0; i < PARTNER_UI_SLOT; i++)
        m_BtnTab[i].CheckButton(i == m_nView - 1);

    for (i = 0; i < PARTNER_UI_SLOT; i++)
    {
        int nP = i + 1;
        BOOL bCo = (nP <= PARTNER_MAX_COUNT && PT_P(nP, PTP_USED) > 0);

        // 3 ky nang bam sinh (SK_GIVEN 1..3)
        for (j = 0; j < PARTNER_SKILL_GIVEN_COL; j++)
        {
            int nPack = bCo ? PT_P(nP, PTP_SK_GIVEN0 + j) : 0;
            if (nPack > 0)
                m_Given[i][j].HoldObject(CGOG_SKILL_FIGHT,
                    PT_SkillId(nPack), PT_SkillLv(nPack), 0);
            else
                m_Given[i][j].HoldObject(CGOG_NOTHING, 0, 0, 0);
        }
        // 6 o dau cua ky nang tu hoc (SK_LEARNT 1..6)
        for (j = 0; j < PARTNER_SKILL_GRID_COL; j++)
        {
            int nPack = bCo ? PT_P(nP, PTP_SK_LEARNT0 + j) : 0;
            if (nPack > 0)
                m_Learnt[i][j].HoldObject(CGOG_SKILL_FIGHT,
                    PT_SkillId(nPack), PT_SkillLv(nPack), 0);
            else
                m_Learnt[i][j].HoldObject(CGOG_NOTHING, 0, 0, 0);
        }
    }
}

int KUiPartnerSkill::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
        {
            CloseWindow();
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
        {
            CloseWindow();
            KUiPartnerAttr::OpenWindow();
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_SaveBtn)
        {
            if (m_nView >= 1 && m_nView != PT_Cur() && PT_P(m_nView, PTP_USED) > 0)
                g_pCoreShell->OperationRequest(GOI_PARTNER_OP,
                    PARTNER_OP_SELECT, m_nView);
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_DeleteBtn)
        {
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_DELETE, m_nView);
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_Forget)
        {
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_FORGETSKILL, m_nView);
        }
        else
        {
            for (int i = 0; i < PARTNER_UI_SLOT; i++)
            {
                if (uParam == (unsigned int)(KWndWindow*)&m_BtnTab[i])
                {
                    m_nView = i + 1;
                    UpdateData();
                    break;
                }
            }
        }
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}
