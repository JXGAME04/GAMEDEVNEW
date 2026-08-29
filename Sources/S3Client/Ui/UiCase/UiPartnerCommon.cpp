/*******************************************************************************
File        : UiPartnerCommon.cpp
create data : 27-08-2026
Description : [BDH-G4] dispatcher: task value doi -> bao cac cua so partner.
*******************************************************************************/
#include "KWin32.h"
#include "UiPartnerCommon.h"
#include "UiPartnerAttr.h"
#include "UiPartnerSkill.h"
#include "UiPartnerBag.h"
#include "UiPartnerBar.h"

void UiPartner_OnTaskValueChanged(int nTaskId)
{
    if (nTaskId < PARTNER_TASK_BASE || nTaskId > PARTNER_TASK_END)
        return;
    KUiPartnerBar::OnTaskValueChanged(nTaskId);
    KUiPartnerAttr::OnTaskValueChanged();
    KUiPartnerSkill::OnTaskValueChanged();
    if (nTaskId == PTG_BAGLEVEL)
    {
        KUiPartnerBag* pBag = KUiPartnerBag::GetIfVisible();
        if (pBag)
            pBag->UpdateData();
    }
}

void UiPartner_HotKey(int nWhat)
{
    if (!g_pCoreShell)
        return;
    switch (nWhat)
    {
    case 0:
        g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_TALK, 0);
        break;
    case 1:
        g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_CALLOUT, 0);
        break;
    case 2:
        g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_ATTACK, 0);
        break;
    case 3:
        g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_FOLLOW, 0);
        break;
    case 4:
    {
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
