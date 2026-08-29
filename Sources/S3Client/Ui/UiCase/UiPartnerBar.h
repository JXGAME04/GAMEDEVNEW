/*******************************************************************************
File        : UiPartnerBar.h
create data : 27-08-2026
Description : [BDH-G4] THANH NHANH ban dong hanh 9 nut (\xCD\xAC\xB0\xE9\xBF\xEC\xBD\xDD\xC0\xB8.ini).
  Chara/Skill/Item mo 3 cua so; cac nut con lai gui GOI_PARTNER_OP.
  Nut Remind la checkbox phia client (tam thoi chua noi gi).
*******************************************************************************/
#if !defined(AFX_UIPARTNERBAR_H__BD100004__INCLUDED_)
#define AFX_UIPARTNERBAR_H__BD100004__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"

#define PARTNER_BAR_BTN_NUM 9

class KUiPartnerBar : public KWndImage
{
public:
    KUiPartnerBar();
    virtual ~KUiPartnerBar();

    static KUiPartnerBar* OpenWindow();
    static KUiPartnerBar* GetIfVisible();
    static void           CloseWindow();
    // tu mo khi dang nhap xong ma nguoi choi da co dong hanh
    static void           OnTaskValueChanged(int nTaskId);

private:
    static KUiPartnerBar* ms_pSelf;

    void          Initialize();
    virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
    KWndButton    m_Btn[PARTNER_BAR_BTN_NUM];
};

#endif
