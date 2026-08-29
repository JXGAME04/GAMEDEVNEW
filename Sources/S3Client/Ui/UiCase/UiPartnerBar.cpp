/*******************************************************************************
File        : UiPartnerBar.cpp
create data : 27-08-2026
Description : [BDH-G4] THANH NHANH ban dong hanh (xem UiPartnerBar.h).
*******************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiPartnerCommon.h"
#include "UiPartnerBar.h"
#include "UiPartnerAttr.h"
#include "UiPartnerSkill.h"
#include "UiPartnerBag.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"

#define SCHEME_INI_PARTNER_BAR "\xCD\xAC\xB0\xE9\xBF\xEC\xBD\xDD\xC0\xB8.ini"

KUiPartnerBar* KUiPartnerBar::ms_pSelf = NULL;

// thu tu 9 muc trong ini
static const char* s_szBtnSec[PARTNER_BAR_BTN_NUM] =
{
    "Chara",            // 0 cua so thuoc tinh
    "Skill",            // 1 cua so ky nang
    "Item",             // 2 tui dong hanh
    "TalkToPartner",    // 3 doi thoai
    "Attack",           // 4 chu dong cong kich
    "Follow",           // 5 chi di theo
    "CallForPartner",   // 6 goi ra / thu ve
    "Remind",           // 7 nhac nho (checkbox, chua noi)
    "SelectPartner",    // 8 doi con duong nhiem
};

KUiPartnerBar::KUiPartnerBar()
{
}

KUiPartnerBar::~KUiPartnerBar()
{
}

KUiPartnerBar* KUiPartnerBar::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

KUiPartnerBar* KUiPartnerBar::OpenWindow()
{
    // [29/08] he partner mobile DA GO theo yeu cau chu (dung he PET PC)
    return NULL;
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiPartnerBar;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

void KUiPartnerBar::CloseWindow()
{
    if (ms_pSelf)
    {
        ms_pSelf->Hide();
        ms_pSelf = NULL;
    }
}

void KUiPartnerBar::OnTaskValueChanged(int nTaskId)
{
    // dang nhap xong: may chu dong bo PTG_VERSION = 1 -> tu hien thanh nhanh
    if (nTaskId == PTG_VERSION && PT_TaskVal(PTG_VERSION) > 0)
    {
        if (ms_pSelf == NULL)
            OpenWindow();
    }
}

void KUiPartnerBar::Initialize()
{
    int i;
    for (i = 0; i < PARTNER_BAR_BTN_NUM; i++)
        AddChild(&m_Btn[i]);

    char Scheme[256];
    KIniFile Ini;
    g_UiBase.GetCurSchemePath(Scheme, 256);
    strcat(Scheme, "\\" SCHEME_INI_PARTNER_BAR);
    if (Ini.Load(Scheme))
    {
        Init(&Ini, "Main");
        for (i = 0; i < PARTNER_BAR_BTN_NUM; i++)
            m_Btn[i].Init(&Ini, s_szBtnSec[i]);
    }
    Wnd_AddWindow(this);
}

int KUiPartnerBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
    {
        int nBtn = -1;
        for (int i = 0; i < PARTNER_BAR_BTN_NUM; i++)
            if (uParam == (unsigned int)(KWndWindow*)&m_Btn[i])
            {
                nBtn = i;
                break;
            }
        switch (nBtn)
        {
        case 0:
            if (KUiPartnerAttr::GetIfVisible())
                KUiPartnerAttr::CloseWindow();
            else
                KUiPartnerAttr::OpenWindow();
            break;
        case 1:
            if (KUiPartnerSkill::GetIfVisible())
                KUiPartnerSkill::CloseWindow();
            else
                KUiPartnerSkill::OpenWindow();
            break;
        case 2:
            if (KUiPartnerBag::GetIfVisible())
                KUiPartnerBag::CloseWindow();
            else
                KUiPartnerBag::OpenWindow();
            break;
        case 3:
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_TALK, 0);
            break;
        case 4:
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_ATTACK, 0);
            break;
        case 5:
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_FOLLOW, 0);
            break;
        case 6:
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_CALLOUT, 0);
            break;
        case 7:
            // Remind: checkbox phia client, chua noi chuc nang
            break;
        case 8:
        {
            // doi sang con ke tiep dang co
            int nCur = PT_Cur();
            for (int k = 1; k <= PARTNER_MAX_COUNT; k++)
            {
                int nNext = (nCur + k - 1) % PARTNER_MAX_COUNT + 1;
                if (PT_P(nNext, PTP_USED) > 0 && nNext != nCur)
                {
                    g_pCoreShell->OperationRequest(GOI_PARTNER_OP,
                        PARTNER_OP_SELECT, nNext);
                    break;
                }
            }
        }
        break;
        default:
            break;
        }
    }
    break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}
