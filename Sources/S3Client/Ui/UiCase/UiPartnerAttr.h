/*******************************************************************************
File        : UiPartnerAttr.h
create data : 27-08-2026
Description : [BDH-G4] Cua so THUOC TINH ban dong hanh (trang "dong ho so").
  Bo cuc: \xCD\xAC\xB0\xE9\xCA\xF4\xD0\xD4.ini (rut tu ban Linux, da Viet hoa).
  5 the doc (BtnPartner_0..4) - ban goc ve 5, he chi dung 3 (PARTNER_MAX_COUNT).
*******************************************************************************/
#if !defined(AFX_UIPARTNERATTR_H__BD100001__INCLUDED_)
#define AFX_UIPARTNERATTR_H__BD100001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../elem/WndEdit.h"
#include "../elem/wndobjcontainer.h"
#include "../../../core/src/KPlayerPartner.h"   // PARTNER_UI_SLOT (hang ngoai _SERVER)

#define PARTNER_ATTR_FIELD_NUM  27
#define PARTNER_ATTR_GIVEN_COL  3

class KUiPartnerAttr : public KWndImage
{
public:
    KUiPartnerAttr();
    virtual ~KUiPartnerAttr();

    static KUiPartnerAttr* OpenWindow();
    static KUiPartnerAttr* GetIfVisible();
    static void            CloseWindow();
    static void            OnTaskValueChanged();   // task value ve -> ve lai

private:
    static KUiPartnerAttr* ms_pSelf;

    void          Initialize();
    void          UpdateData();
    virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
    KWndImage     m_Title;
    KWndImage     m_PagePartner;
    KWndButton    m_BtnTab[PARTNER_UI_SLOT];
    KWndButton    m_Forget;
    KWndButton    m_SaveBtn;      // "chon" = luu ten + chon lam duong nhiem
    KWndButton    m_DeleteBtn;    // giai tan (mo doi thoai xac nhan phia server)
    KWndButton    m_CloseBtn;
    KWndButton    m_SwitchBtn;    // chuyen sang trang ky nang
    KWndEdit32    m_editName;
    KWndText32    m_Field[PARTNER_ATTR_FIELD_NUM];
    // 3 cot icon ky nang bam sinh canh 5 the (KongfuIcon/ToughIcon/MissleIcon)
    KWndObjectBox m_GivenIcon[PARTNER_UI_SLOT][PARTNER_ATTR_GIVEN_COL];

    int           m_nView;        // dong hanh dang xem (1-based)
};

#endif
