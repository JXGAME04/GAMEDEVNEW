/*******************************************************************************
File        : UiPet.h
create data : 28-08-2026
Description : [PETSYS] Cua so "Ban Dong Hanh" ban PC - dung 100% bo cuc
  \Ui\Ui3\pet_main.ini (rut nguyen ban tu client VLTK, tieng Viet san trong ini).
  Du lieu tu task value PET_TV_* (KPlayerPet.h) da duoc server tu dong bo.
  Nut gui GOI_PARTNER_OP(PARTNER_OP_PETSYS, PET_OPERATION_*).
*******************************************************************************/
#if !defined(AFX_UIPET_H__PS100001__INCLUDED_)
#define AFX_UIPET_H__PS100001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndbutton.h"
#include "../elem/wndlabeledbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../elem/wndobjcontainer.h"

#define PET_UI_BTN_NUM      8
#define PET_UI_TXT_NUM      12
#define PET_UI_EXTSKILL_NUM 4
#define PET_UI_EQUIP_NUM    5

class KUiPet : public KWndImage
{
public:
    KUiPet();
    virtual ~KUiPet();

    static KUiPet*  OpenWindow();
    static KUiPet*  GetIfVisible();
    static void     CloseWindow();
    static void     OnTaskValueChanged(int nTaskId);

private:
    static KUiPet*  ms_pSelf;

    void          Initialize();
    void          UpdateData();
    void          SendOp(int nOp);
    virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
    KWndButton        m_CloseBtn;
    // 8 nut chuc nang (thu tu khop s_szBtnSec)
    KWndLabeledButton m_Btn[PET_UI_BTN_NUM];
    KWndLabeledButton m_CompanionBtn;   // "Duc lai" - he trang bi pet (dot sau)
    // 12 nhan tinh (Text= trong ini) + 12 o gia tri
    KWndText32        m_Txt[PET_UI_TXT_NUM];
    KWndText32        m_Val[PET_UI_TXT_NUM];
    KWndObjectBox     m_Skill1;
    KWndObjectBox     m_ExtSkill[PET_UI_EXTSKILL_NUM];
    KWndImage         m_Equip[PET_UI_EQUIP_NUM];   // khung 40x40 (hang 1)
    KWndImage         m_Appearance;   // [PETSYS] hinh pet dung (npcres _st01)
};

#endif
