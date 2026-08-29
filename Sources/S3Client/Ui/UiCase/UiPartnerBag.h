/*******************************************************************************
File        : UiPartnerBag.h
create data : 27-08-2026
Description : [BDH-G4] TUI ban dong hanh 6x10 (\xCD\xAC\xB0\xE9\xB1\xB3\xB0\xFC.ini).
  Vat pham nam pos_partnerbag (dong bo ITEM_SYNC binh thuong);
  keo tha qua GOI_SWITCH_OBJECT voi eContainer = UOC_PARTNER_BAG,
  chuot phai = tra ve hanh trang (GOI_EXCHANGEITEM) - y khuon UiExBox1.
  O khoa theo cap tui do MAY CHU chan (Partner_BagCellActive).
*******************************************************************************/
#if !defined(AFX_UIPARTNERBAG_H__BD100003__INCLUDED_)
#define AFX_UIPARTNERBAG_H__BD100003__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"
#include "../elem/wndobjcontainer.h"

struct ITEM_PICKDROP_PLACE;
struct KUiObjAtRegion;

class KUiPartnerBag : public KWndImage
{
public:
    KUiPartnerBag();
    virtual ~KUiPartnerBag();

    static KUiPartnerBag* OpenWindow();
    static KUiPartnerBag* GetIfVisible();
    static void           CloseWindow();

    void          UpdateData();
    void          UpdateItem(KUiObjAtRegion* pItem, int bAdd);

private:
    static KUiPartnerBag* ms_pSelf;

    void          Initialize();
    void          OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
    virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
    KWndButton       m_CloseBtn;
    KWndText32       m_TextCap;      // dong "cap tui"
    KWndObjectMatrix m_ItemBox;
};

#endif
