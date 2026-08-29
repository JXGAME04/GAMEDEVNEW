/*******************************************************************************
File        : UiPartnerBag.cpp
create data : 27-08-2026
Description : [BDH-G4] TUI ban dong hanh (xem UiPartnerBag.h).
*******************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiPartnerCommon.h"
#include "UiPartnerBag.h"
#include "UiItem.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../../../core/src/GameDataDef.h"

#define SCHEME_INI_PARTNER_BAG "\xCD\xAC\xB0\xE9\xB1\xB3\xB0\xFC.ini"

KUiPartnerBag* KUiPartnerBag::ms_pSelf = NULL;

KUiPartnerBag::KUiPartnerBag()
{
}

KUiPartnerBag::~KUiPartnerBag()
{
}

KUiPartnerBag* KUiPartnerBag::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

KUiPartnerBag* KUiPartnerBag::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiPartnerBag;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        if (KUiItem::GetIfVisible() == NULL)
            KUiItem::OpenWindow();
        else
            UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->UpdateData();
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

void KUiPartnerBag::CloseWindow()
{
    if (ms_pSelf)
    {
        ms_pSelf->Hide();
        ms_pSelf = NULL;
    }
}

void KUiPartnerBag::Initialize()
{
    AddChild(&m_CloseBtn);
    AddChild(&m_TextCap);
    AddChild(&m_ItemBox);

    m_ItemBox.SetContainerId((int)UOC_PARTNER_BAG);

    char Scheme[256];
    KIniFile Ini;
    g_UiBase.GetCurSchemePath(Scheme, 256);
    strcat(Scheme, "\\" SCHEME_INI_PARTNER_BAG);
    if (Ini.Load(Scheme))
    {
        Init(&Ini, "Main");
        m_CloseBtn.Init(&Ini, "CloseBtn");
        m_ItemBox.Init(&Ini, "ItemBox");
        m_ItemBox.EnableTracePutPos(true);
        m_TextCap.Init(&Ini, "TextBtn");
        m_TextCap.SetTextColor(0xFFBEF17D);     // 190,241,125 nhu ini
    }
    Wnd_AddWindow(this);
}

void KUiPartnerBag::UpdateData()
{
    m_ItemBox.Clear();

    char szBuf[48];
    sprintf(szBuf, "CÊp tói: %d", PT_TaskVal(PTG_BAGLEVEL));
    m_TextCap.SetText(szBuf);

    int nCount = g_pCoreShell->GetGameData(GDI_ITEM_IN_PARTNER_BAG, 0, 0);
    if (nCount == 0)
        return;

    KUiObjAtRegion* pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount);
    if (pObjs)
    {
        g_pCoreShell->GetGameData(GDI_ITEM_IN_PARTNER_BAG, (unsigned int)pObjs, nCount);
        for (int i = 0; i < nCount; i++)
            UpdateItem(&pObjs[i], 1);
        free(pObjs);
    }
}

void KUiPartnerBag::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
    if (pItem)
    {
        if (pItem->Obj.uGenre != CGOG_MONEY)
        {
            KUiDraggedObject Obj;
            Obj.uGenre = pItem->Obj.uGenre;
            Obj.uId = pItem->Obj.uId;
            Obj.DataX = pItem->Region.h;
            Obj.DataY = pItem->Region.v;
            Obj.DataW = pItem->Region.Width;
            Obj.DataH = pItem->Region.Height;
            if (bAdd)
                m_ItemBox.AddObject(&Obj, 1);
            else
                m_ItemBox.RemoveObject(&Obj);
        }
    }
    else
        UpdateData();
}

int KUiPartnerBag::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_RIGHT_CLICK_ITEM:
    {
        // chuot phai: tra vat pham ve hanh trang (khuon UiExBox1)
        if (g_UiBase.GetStatus() == UIS_S_IDLE)
        {
            KUiDraggedObject* pItem = (KUiDraggedObject*)uParam;
            unsigned int uPr[2];
            uPr[0] = pItem->uId;
            uPr[1] = pos_partnerbag;
            g_pCoreShell->OperationRequest(GOI_EXCHANGEITEM,
                (unsigned int)&uPr, pos_equiproom);
        }
    }
    break;
    case WND_N_ITEM_PICKDROP:
        OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
        break;
    case WND_N_BUTTON_CLICK:
        if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
            CloseWindow();
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}

void KUiPartnerBag::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
    if (g_UiBase.GetStatus() != UIS_S_IDLE)
        return;
    KUiObjAtContRegion Pick, Drop;
    KUiDraggedObject   Obj;

    if (pPickPos)
    {
        _ASSERT(pPickPos->pWnd);
        ((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(Obj, pPickPos->h, pPickPos->v);
        Pick.Obj.uGenre = Obj.uGenre;
        Pick.Obj.uId = Obj.uId;
        Pick.Region.Width = Obj.DataW;
        Pick.Region.Height = Obj.DataH;
        Pick.Region.h = Obj.DataX;
        Pick.Region.v = Obj.DataY;
        Pick.eContainer = UOC_PARTNER_BAG;
    }
    if (pDropPos)
    {
        Wnd_GetDragObj(&Obj);
        Drop.Obj.uGenre = Obj.uGenre;
        Drop.Obj.uId = Obj.uId;
        Drop.Region.Width = Obj.DataW;
        Drop.Region.Height = Obj.DataH;
        Drop.Region.h = pDropPos->h;
        Drop.Region.v = pDropPos->v;
        Drop.eContainer = UOC_PARTNER_BAG;
    }

    g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
        pPickPos ? (unsigned int)&Pick : 0,
        pDropPos ? (int)&Drop : 0);
}
