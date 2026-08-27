/*******************************************************************************
File        : UiDiceItem.cpp
create data : 26-08-2026
Description : He XUC XAC chia do (DICEITEM) - cua so ben client. Xem UiDiceItem.h
*******************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/KProtocol.h"     // keo theo Headers\KDiceProtocol.h
#include "UiDiceItem.h"
#include "UiSysMsgCentre.h"

extern iCoreShell*      g_pCoreShell;

// Ten hai tep bo cuc la chu Trung - phai la BYTE GBK THO trong ma nguon,
// y het KUiTeamApply (UiTeamApply.cpp:19). Khong ky tu hex nao theo ngay sau
// cac chuoi thoat nen khong bi nuot chu so.
#define DICE_FRAME_INI  "\xCD\xB6\xC9\xAB\xD7\xD3-\xB9\xAB\xD3\xC3.ini"  // khung 4 o
#define DICE_ROW_INI    "\xCD\xB6\xC9\xAB\xD7\xD3.ini"                   // noi dung 1 o

#define DICE_FRAME_PER_SECOND   18      // = KUiTimeBox: 1 giay = 18 khung hinh

KUiDiceItem* KUiDiceItem::ms_pSelf = NULL;

// [Img3] o dinh khung, [Img0] o day - giu dung thu tu cua tep .ini goc
static const char* s_szRowSection[DICE_ROW_COUNT] =
{
    "Img3", "Img2", "Img1", "Img0",
};

KUiDiceItem::KUiDiceItem()
{
    for (int i = 0; i < DICE_ROW_COUNT; i++)
    {
        m_nDiceId[i]  = 0;
        m_nItemIdx[i] = 0;
        m_nFrame[i]   = 0;
    }
    strcpy(m_szTimeFmt, "%d");
}

KUiDiceItem::~KUiDiceItem()
{
}

//------------------------------------------------------------------
KUiDiceItem* KUiDiceItem::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiDiceItem;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

KUiDiceItem* KUiDiceItem::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

void KUiDiceItem::CloseWindow(bool bDestory)
{
    if (ms_pSelf)
    {
        // PHAI nha het vat pham dung tam, khong thi ro mang Item[] cua client
        // (y het KSellItem.cpp:57 lam khi dong cua so mua ban)
        for (int i = 0; i < DICE_ROW_COUNT; i++)
            ms_pSelf->ClearRow(i);

        ms_pSelf->Hide();
        if (bDestory)
        {
            ms_pSelf->Destroy();
            ms_pSelf = NULL;
        }
    }
}

//------------------------------------------------------------------
void KUiDiceItem::Initialize()
{
    for (int i = 0; i < DICE_ROW_COUNT; i++)
    {
        AddChild(&m_Row[i]);

        // Toa do trong DICE_ROW_INI la TUONG DOI so voi mot o, nen cac dieu khien
        // phai la CON cua o - khoi phai tu cong offset theo hang.
        m_ItemBox[i].SetObjectGenre(CGOG_PLAYERSELLITEM);
        m_Row[i].AddChild(&m_ItemBox[i]);
        m_ItemBox[i].SetContainerId(0);
        // vat pham nay NGUOI CHOI CHUA SO HUU - cam nhac/tha
        m_ItemBox[i].EnablePickPut(false);

        m_Row[i].AddChild(&m_Time[i]);
        m_Row[i].AddChild(&m_BtnNeed[i]);
        m_Row[i].AddChild(&m_BtnGiveUp[i]);
    }

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    for (int j = 0; j < DICE_ROW_COUNT; j++)
        m_Row[j].Hide();

    Wnd_AddWindow(this);
}

void KUiDiceItem::LoadScheme(const char* pScheme)
{
    if (!ms_pSelf)
        return;

    char        Buff[256];
    KIniFile    IniFrame;
    KIniFile    IniRow;

    sprintf(Buff, "%s\\%s", pScheme, DICE_FRAME_INI);
    if (IniFrame.Load(Buff))
    {
        ms_pSelf->Init(&IniFrame, "Main");
        for (int i = 0; i < DICE_ROW_COUNT; i++)
            ms_pSelf->m_Row[i].Init(&IniFrame, s_szRowSection[i]);
    }

    sprintf(Buff, "%s\\%s", pScheme, DICE_ROW_INI);
    if (IniRow.Load(Buff))
    {
        for (int i = 0; i < DICE_ROW_COUNT; i++)
        {
            ms_pSelf->m_ItemBox[i].Init(&IniRow, "objItem");
            ms_pSelf->m_Time[i].Init(&IniRow, "TimeInfo");
            // [TimeInfo] trong .ini CHI co Left/Top/Text - khong Width/Height/
            // Color, nen sau Init o chu rong 0x0 va mau den => KHONG VE GI.
            // Phai tu dat. O bat dau tai x=80 trong hang rong 312 nen con 224px.
            ms_pSelf->m_Time[i].SetSize(224, 16);
            // MAU LA ARGB, alpha PHAI la 0xFF. GetColor() cua engine dung
            // 0xFF000000 + (R<<16) + (G<<8) + B (WndWindow.cpp). Ban truoc toi
            // truyen 0x00FFFCB2 -> alpha = 0 -> chu TRONG SUOT, khong thay gi.
            // 255,252,178 = mau vang nhat, y het nhan nut trong .ini.
            ms_pSelf->m_Time[i].SetTextColor(0xFFFFFCB2);
            // Ban goc co HAI muc nut chong nhau o cung cho: [BtnNeed] nhan "Can"
            // (3 chu, vua khung 63px) va [BtnGreed] nhan "Tham du nhan" (dai gap
            // doi, bi cat cut). Anh nen "3 chu" nen [BtnNeed] moi la y do goc.
            ms_pSelf->m_BtnNeed[i].Init(&IniRow, "BtnNeed");     // nhan "Can"
            ms_pSelf->m_BtnGiveUp[i].Init(&IniRow, "BtnClose");
            // "Huy bo nhan" cung qua dai cho khung 63px -> rut con "Bo qua".
            // Chuoi la byte TCVN3 (B\xE1 qua), giong moi tep giao dien khac.
            ms_pSelf->m_BtnGiveUp[i].SetLabel("B\xE1 qua");
        }
        // "Text=Thoi gian con lai %d giay" - KWndText::Init dat nguyen chuoi co %d,
        // ta phai giu lai lam DINH DANG roi tu sprintf moi giay.
        char szFmt[128];
        szFmt[0] = 0;
        IniRow.GetString("TimeInfo", "Text", "", szFmt, sizeof(szFmt) - 1);
        if (szFmt[0])
        {
            strncpy(ms_pSelf->m_szTimeFmt, szFmt, sizeof(ms_pSelf->m_szTimeFmt) - 1);
            ms_pSelf->m_szTimeFmt[sizeof(ms_pSelf->m_szTimeFmt) - 1] = 0;
        }
    }
}

//------------------------------------------------------------------
int KUiDiceItem::FindRow(int nDiceId)
{
    if (nDiceId <= 0)
        return -1;
    for (int i = 0; i < DICE_ROW_COUNT; i++)
    {
        if (m_nDiceId[i] == nDiceId)
            return i;
    }
    return -1;
}

int KUiDiceItem::FindFreeRow()
{
    for (int i = 0; i < DICE_ROW_COUNT; i++)
    {
        if (m_nDiceId[i] == 0)
            return i;
    }
    return -1;
}

int KUiDiceItem::VisibleRowCount()
{
    int n = 0;
    for (int i = 0; i < DICE_ROW_COUNT; i++)
    {
        if (m_nDiceId[i] != 0)
            n++;
    }
    return n;
}

void KUiDiceItem::ClearRow(int nRow)
{
    if (nRow < 0 || nRow >= DICE_ROW_COUNT)
        return;
    if (m_nItemIdx[nRow] > 0)
    {
        // uParam = false -> nhanh ItemSet.Remove cua GDI_ITEM_CHAT (CoreShell.cpp:2296)
        g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, m_nItemIdx[nRow]);
        m_nItemIdx[nRow] = 0;
    }
    m_nDiceId[nRow] = 0;
    m_nFrame[nRow]  = 0;
    m_ItemBox[nRow].HoldObject(CGOG_NOTHING, 0, 0, 0);
    m_Row[nRow].Hide();
}

void KUiDiceItem::ShowRow(int nRow, int nDiceId, void* pChatItem, int nTimeLeft)
{
    if (nRow < 0 || nRow >= DICE_ROW_COUNT)
        return;

    ClearRow(nRow);

    // Dung lai vat pham trong mang Item[] cua CLIENT tu ban mo ta - dung duong
    // co san GDI_ITEM_CHAT (CoreShell.cpp:2226). Nho vay bieu tuong va CHU GIAI
    // hoat dong nhu vat pham binh thuong, khong phai viet them gi.
    int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)pChatItem);
    if (nIdx <= 0)
        return;

    m_nDiceId[nRow]  = nDiceId;
    m_nItemIdx[nRow] = nIdx;
    m_nFrame[nRow]   = nTimeLeft * DICE_FRAME_PER_SECOND;
    if (m_nFrame[nRow] < 0)
        m_nFrame[nRow] = 0;

    // 1x1: o [objItem] chi rong 38x38 = MOT o luoi. DataW/DataH la so o luoi
    // (WndObjContainer.cpp:512 nhan voi m_nUnitWidth), de kich thuoc that cua
    // vat pham thi vat pham 2x3 se ve tran ra ngoai o.
    m_ItemBox[nRow].HoldObject(CGOG_PLAYERSELLITEM, (unsigned int)nIdx, 1, 1);

    m_BtnNeed[nRow].Enable(TRUE);
    m_BtnGiveUp[nRow].Enable(TRUE);
    RefreshTime(nRow);
    m_Row[nRow].Show();
}

void KUiDiceItem::RefreshTime(int nRow)
{
    char Buff[160];
    int nGiay = m_nFrame[nRow] / DICE_FRAME_PER_SECOND;
    _snprintf(Buff, sizeof(Buff) - 1, m_szTimeFmt, nGiay);
    Buff[sizeof(Buff) - 1] = 0;
    m_Time[nRow].SetText(Buff);
}

void KUiDiceItem::SendChoice(int nRow, int nChoice)
{
    if (nRow < 0 || nRow >= DICE_ROW_COUNT || m_nDiceId[nRow] == 0)
        return;
    // Truyen thang hai so, khong truyen con tro: khoi lo vong doi bo nho.
    g_pCoreShell->OperationRequest(GOI_DICE_CHOICE,
                                   (unsigned int)m_nDiceId[nRow], nChoice);
    // Khoa nut ngay; may chu se gui DICE_ACT_CLOSE khi chot xong.
    m_BtnNeed[nRow].Enable(FALSE);
    m_BtnGiveUp[nRow].Enable(FALSE);
}

//------------------------------------------------------------------
void KUiDiceItem::OnDiceMsg(void* pSync)
{
    if (!pSync)
        return;
    DICE_ITEM_SYNC* p = (DICE_ITEM_SYNC*)pSync;

    if (p->m_btAction == DICE_ACT_OPEN)
    {
        KUiDiceItem* pWnd = OpenWindow();
        if (!pWnd)
            return;
        int nRow = pWnd->FindRow(p->m_nDiceId);
        if (nRow < 0)
            nRow = pWnd->FindFreeRow();
        if (nRow < 0)
            return;     // day 4 o - bo qua, giong ban goc chi co 4 khung
        pWnd->ShowRow(nRow, p->m_nDiceId, &p->m_Item, p->m_nTimeLeft);
        return;
    }

    if (!ms_pSelf)
        return;

    if (p->m_btAction == DICE_ACT_CLOSE)
    {
        int nRow = ms_pSelf->FindRow(p->m_nDiceId);
        if (nRow >= 0)
            ms_pSelf->ClearRow(nRow);
        if (ms_pSelf->VisibleRowCount() == 0)
            CloseWindow();
        return;
    }

    if (p->m_btAction == DICE_ACT_NUMBER)
    {
        char Buff[192];
        if (p->m_nNumber > 0)
            _snprintf(Buff, sizeof(Buff) - 1, "%s: %d", p->m_szName, p->m_nNumber);
        else
            _snprintf(Buff, sizeof(Buff) - 1, "%s", p->m_szName);
        Buff[sizeof(Buff) - 1] = 0;
        // in ra trung tam tin he thong - y het KUiTrembleItem.cpp:207
        KSystemMessage Msg;
        Msg.byConfirmType = SMCT_NONE;
        Msg.eType         = SMT_NORMAL;
        Msg.byPriority    = 1;
        Msg.byParamSize   = 0;
        strncpy(Msg.szMessage, Buff, sizeof(Msg.szMessage) - 1);
        Msg.szMessage[sizeof(Msg.szMessage) - 1] = 0;
        KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
        return;
    }
}

//------------------------------------------------------------------
int KUiDiceItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        for (int i = 0; i < DICE_ROW_COUNT; i++)
        {
            if (uParam == (unsigned int)(KWndWindow*)&m_BtnNeed[i])
            {
                SendChoice(i, DICE_CHOICE_NEED);
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_BtnGiveUp[i])
            {
                SendChoice(i, DICE_CHOICE_GIVEUP);
                return 1;
            }
        }
        break;
    default:
        // y het KUiTrembleItem::WndProc: chi uy thac o nhanh default
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 1;
}

void KUiDiceItem::Breathe()
{
    for (int i = 0; i < DICE_ROW_COUNT; i++)
    {
        if (m_nDiceId[i] == 0)
            continue;
        if (m_nFrame[i] > 0)
        {
            m_nFrame[i]--;
            if ((m_nFrame[i] % DICE_FRAME_PER_SECOND) == 0)
                RefreshTime(i);
        }
        else
        {
            // Het gio o phia client: khoa nut cho do bam nham. KHONG tu dong o -
            // may chu moi la ben chot, no se gui DICE_ACT_CLOSE.
            m_BtnNeed[i].Enable(FALSE);
            m_BtnGiveUp[i].Enable(FALSE);
        }
    }
    // KHONG goi KWndImage::Breathe(): trong KWndWindow no la thanh vien PRIVATE
    // (WndWindow.h:119, than rong). KUiTrembleItem cung khong goi.
}
