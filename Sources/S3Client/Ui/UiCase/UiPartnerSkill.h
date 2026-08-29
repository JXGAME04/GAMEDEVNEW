/*******************************************************************************
File        : UiPartnerSkill.h
create data : 27-08-2026
Description : [BDH-G4] Cua so KY NANG ban dong hanh.
  Bo cuc \xCD\xAC\xB0\xE9\xBC\xBC\xC4\xDC.ini: 5 hang (1 hang / the dong hanh);
  moi hang 3 o ky nang bam sinh (KongfuIcon/ToughIcon/MissleIcon) +
  luoi SkillsIcon 6 cot ky nang tu hoc. Icon lay tu bang skills.txt client
  qua KWndObjectBox::HoldObject nhu cua so ky nang nguoi choi.
*******************************************************************************/
#if !defined(AFX_UIPARTNERSKILL_H__BD100002__INCLUDED_)
#define AFX_UIPARTNERSKILL_H__BD100002__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndobjcontainer.h"
#include "../../../core/src/KPlayerPartner.h"   // PARTNER_UI_SLOT (hang ngoai _SERVER)

#define PARTNER_SKILL_GIVEN_COL 3
#define PARTNER_SKILL_GRID_COL  6

class KUiPartnerSkill : public KWndImage
{
public:
    KUiPartnerSkill();
    virtual ~KUiPartnerSkill();

    static KUiPartnerSkill* OpenWindow();
    static KUiPartnerSkill* GetIfVisible();
    static void             CloseWindow();
    static void             OnTaskValueChanged();

    void          SetView(int nP);

private:
    static KUiPartnerSkill* ms_pSelf;

    void          Initialize();
    void          UpdateData();
    virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
    KWndImage     m_Title;
    KWndImage     m_PagePartner;
    KWndButton    m_BtnTab[PARTNER_UI_SLOT];
    KWndButton    m_Forget;
    KWndButton    m_SaveBtn;
    KWndButton    m_DeleteBtn;
    KWndButton    m_CloseBtn;
    KWndButton    m_SwitchBtn;

    // hang i = dong hanh i+1 (3 hang dau co du lieu, 2 hang cuoi trong)
    KWndObjectBox m_Given[PARTNER_UI_SLOT][PARTNER_SKILL_GIVEN_COL];
    KWndObjectBox m_Learnt[PARTNER_UI_SLOT][PARTNER_SKILL_GRID_COL];

    int           m_nView;
};

#endif
