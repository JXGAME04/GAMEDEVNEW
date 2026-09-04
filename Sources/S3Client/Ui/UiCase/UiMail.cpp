/*******************************************************************************
File        : UiMail.cpp
create data : 03-09-2026
Description : [MAIL 03/09] Cua so HOP THU. Xem UiMail.h.
*******************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/PopupMenu.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "UiInformation.h"
#include "UiMail.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern iCoreShell*      g_pCoreShell;

#define MAIL_MANAGER_INI    "mail\\mail_manager.ini"
#define MAIL_LIST_INI       "mail\\mail_list.ini"
#define MAIL_HEADER_INI     "mail\\mail_header.ini"
#define MAIL_DETAIL_INI     "mail\\mail_detail.ini"
#define MAIL_AWARD_INI      "mail\\mail_award_item.ini"
#define MAIL_ICON_INI       "mail\\mail_icon.ini"

#define MAILUI_ROW_HEIGHT       48
#define MAILUI_MENU_FILTER      0x0311      // ma menu bo loc (HIWORD nParam cua WND_M_MENUITEM_SELECTED)
#define MAILUI_CONFIRM_PARAM    0x4D41494C  // 'MAIL' - uParam cua WND_M_OTHER_WORK_RESULT tu hop xac nhan
extern int SCREEN_WIDTH;    // S3Client.cpp: 800 / 1024 (UiPlayerBar neo cot icon phai theo no)

// [MAIL 03/09 D4] nhat ky chan doan cua so thu: jx_mail.log canh Game.exe (cung tep voi KScriptProtocol.cpp)
static void sMailLog(const char* szFmt, ...)
{
    if (!szFmt)
        return;
    FILE* f = fopen("jx_mail.log", "a");
    if (!f)
        return;
    time_t t = time(NULL);
    struct tm* p = localtime(&t);
    if (p)
        fprintf(f, "%02d:%02d:%02d ", p->tm_hour, p->tm_min, p->tm_sec);
    va_list va;
    va_start(va, szFmt);
    vfprintf(f, szFmt, va);
    va_end(va);
    fprintf(f, "\n");
    fclose(f);
}

#define MAILUI_ICON_FRAME_NORMAL 0          // [D8] mail_icon.ini [MailBtn] Up=0
#define MAILUI_ICON_FRAME_HOT    2          // [D8] khung Over=2 = con thu chua doc
#define MAILUI_BLINK_TOTAL      (18 * 6)    // [D8] thu moi: nhay 6 giay roi giu khung HOT
#define MAILUI_BLINK_FRAMES     9           // 18 khung = 1 giay -> nhay 2 lan/giay

KUiMailManager* KUiMailManager::ms_pSelf = NULL;
KUiMailIcon*    KUiMailIcon::ms_pSelf = NULL;

// [MAIL 03/09 D4] goi thu luc dang nhap (playerlogin.lua o dau KPlayer::LaunchPlayer) den TRUOC s2c_syncend =
// GDCNI_GAME_START -> hoan hien bieu tuong / mo cua so toi khi vao game; thoat game thi don sach.
static int s_bMailGameStarted = 0;
static int s_bMailPendingOpen = 0;
static int s_nMailPendingIcon = -1;

static void sSendOp(int nOp, int nExtra, int nParam)
{
    if (g_pCoreShell)
        g_pCoreShell->OperationRequest(GOI_MAIL_UI, MAILUI_OP_MAKE(nOp, nExtra), nParam);
}

static void sCopyStr(char* pDst, int nSize, const char* pSrc)
{
    if (!pDst || nSize <= 0)
        return;
    if (!pSrc)
    {
        pDst[0] = 0;
        return;
    }
    strncpy(pDst, pSrc, nSize - 1);
    pDst[nSize - 1] = 0;
}

//------------------------------------------------------------------
KWndTextMail::KWndTextMail()
{
    m_Text[0] = m_Text[MAILUI_CONTENT_LEN - 1] = 0;
    SetTextPtr(m_Text, MAILUI_CONTENT_LEN - 1);
}

//==================================================================
// KUiMailRow
//==================================================================
KUiMailRow::KUiMailRow()
{
    m_nId = 0;
}

void KUiMailRow::Build()
{
    // thu tu AddChild = thu tu ve va uu tien bat chuot (con them SAU nam TREN):
    // nen chon -> gach -> chu -> bieu tuong -> nut phu ca hang -> o chon
    AddChild(&m_Selection);
    AddChild(&m_Divider);
    AddChild(&m_Sender);
    AddChild(&m_Title);
    AddChild(&m_Date);
    AddChild(&m_ReadIcon);
    AddChild(&m_UnreadIcon);
    AddChild(&m_Attach);
    AddChild(&m_Click);
    AddChild(&m_Check);
}

void KUiMailRow::LoadScheme(KIniFile* pIni)
{
    Init(pIni, "Main");
    m_Selection.Init(pIni, "SelectionImg");
    m_Divider.Init(pIni, "DividingLine");
    m_Sender.Init(pIni, "MailSenderLable");
    m_Title.Init(pIni, "MailTitleLable");
    m_Date.Init(pIni, "MailDateLable");
    m_ReadIcon.Init(pIni, "MailHaveReadIcon");
    m_UnreadIcon.Init(pIni, "MailNotReadIcon");
    m_Attach.Init(pIni, "AttachmentImg");
    m_Click.Init(pIni, "ForClickImg");
    m_Check.Init(pIni, "CheckBox");
    ClearRow();
}

void KUiMailRow::Fill(const KMailUiHeader* p, const char* szDayUnit, int bSelected, int bChecked)
{
    if (!p)
    {
        ClearRow();
        return;
    }
    m_nId = p->nId;
    m_Sender.SetText(p->szSender);
    m_Title.SetText(p->szTitle);

    char szDate[32];
    szDate[0] = 0;
    if (p->nExpiredTime > 0)
    {
        int nLeft = p->nExpiredTime - (int)time(NULL);
        int nDay = (nLeft + 86399) / 86400;
        if (nDay < 1)
            nDay = 1;
        if (nDay > 9999)
            nDay = 9999;
        sprintf(szDate, "%d %s", nDay, szDayUnit ? szDayUnit : "");
    }
    m_Date.SetText(szDate);

    if (p->bRead)
    {
        m_ReadIcon.Show();
        m_UnreadIcon.Hide();
    }
    else
    {
        m_ReadIcon.Hide();
        m_UnreadIcon.Show();
    }
    if (p->bHasAward)
        m_Attach.Show();
    else
        m_Attach.Hide();
    if (bSelected)
        m_Selection.Show();
    else
        m_Selection.Hide();
    m_Check.CheckButton(bChecked ? 1 : 0);
    m_Check.Show();
    m_Click.Show();
    m_Divider.Show();
    m_Sender.Show();
    m_Title.Show();
    m_Date.Show();
    Show();
}

void KUiMailRow::ClearRow()
{
    m_nId = 0;
    m_Sender.SetText("");
    m_Title.SetText("");
    m_Date.SetText("");
    m_Selection.Hide();
    m_ReadIcon.Hide();
    m_UnreadIcon.Hide();
    m_Attach.Hide();
    m_Check.CheckButton(0);
    Hide();
}

//==================================================================
// KUiMailDetail
//==================================================================
KUiMailDetail::KUiMailDetail()
{
    m_nAwardLeft = 300;
    m_nAwardTop = 315;
    m_nAwardInterval = 28;
    m_nAwardW = 26;
    m_nAwardH = 26;
    for (int i = 0; i < MAILUI_MAX_AWARD; i++)
        m_nAwardItemIdx[i] = 0;
}

void KUiMailDetail::Build()
{
    AddChild(&m_BigTitle);
    AddChild(&m_SendTime);
    AddChild(&m_SenderLabel);
    AddChild(&m_TitleLabel);
    AddChild(&m_SenderValue);
    AddChild(&m_TitleValue);
    AddChild(&m_Divider);
    AddChild(&m_ContentLabel);
    AddChild(&m_Content);
    for (int i = 0; i < MAILUI_MAX_AWARD; i++)
    {
        AddChild(&m_AwardBg[i]);
        AddChild(&m_AwardSpr[i]);
        // vat pham tam: y het KUiDiceItem (o vat pham nguoi choi CHUA so huu - cam nhac/tha)
        m_AwardBox[i].SetObjectGenre(CGOG_PLAYERSELLITEM);
        AddChild(&m_AwardBox[i]);
        m_AwardBox[i].SetContainerId(0);
        m_AwardBox[i].EnablePickPut(false);
        AddChild(&m_AwardCount[i]);
    }
}

// Nha vat pham tam cua o i (neu co)
static void sReleaseAwardItem(int* pnIdx)
{
    if (pnIdx && *pnIdx > 0 && g_pCoreShell)
    {
        g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, *pnIdx);
        *pnIdx = 0;
    }
}

void KUiMailDetail::LoadScheme(const char* pScheme)
{
    char        Buff[256];
    KIniFile    Ini;
    KIniFile    IniAward;

    sprintf(Buff, "%s\\%s", pScheme, MAIL_DETAIL_INI);
    if (Ini.Load(Buff))
    {
        Init(&Ini, "Main");
        m_BigTitle.Init(&Ini, "MailBigTitleValue");
        m_SendTime.Init(&Ini, "MailSendTime");
        m_SenderLabel.Init(&Ini, "MailSenderLable");
        m_TitleLabel.Init(&Ini, "MailTitleLable");
        m_SenderValue.Init(&Ini, "MailSenderValue");
        m_TitleValue.Init(&Ini, "MailTitleValue");
        m_Divider.Init(&Ini, "DividingLine");
        m_ContentLabel.Init(&Ini, "MailContentLable");
        m_Content.Init(&Ini, "MailContentValue");
        Ini.GetInteger("MailAwardPos", "Left", 300, &m_nAwardLeft);
        Ini.GetInteger("MailAwardPos", "Top", 315, &m_nAwardTop);
        Ini.GetInteger("MailAwardPos", "Interval", 28, &m_nAwardInterval);
        if (m_nAwardInterval <= 0)
            m_nAwardInterval = 28;
    }

    sprintf(Buff, "%s\\%s", pScheme, MAIL_AWARD_INI);
    if (IniAward.Load(Buff))
    {
        IniAward.GetInteger("Main", "Width", 26, &m_nAwardW);
        IniAward.GetInteger("Main", "Height", 26, &m_nAwardH);
        for (int i = 0; i < MAILUI_MAX_AWARD; i++)
        {
            int l, t;
            // toa do trong mail_award_item.ini la tuong doi voi mot o; o thu i xep sang TRAI
            int nBaseX = m_nAwardLeft - i * m_nAwardInterval;
            int nBaseY = m_nAwardTop;
            m_AwardBg[i].Init(&IniAward, "MailAwardBgSpr");
            m_AwardBg[i].GetPosition(&l, &t);
            m_AwardBg[i].SetPosition(nBaseX + l, nBaseY + t);
            m_AwardSpr[i].Init(&IniAward, "MailAwardItemSpr");
            m_AwardSpr[i].GetPosition(&l, &t);
            m_AwardSpr[i].SetPosition(nBaseX + l, nBaseY + t);
            m_AwardBox[i].Init(&IniAward, "MailAwardItemSpr");
            m_AwardBox[i].GetPosition(&l, &t);
            m_AwardBox[i].SetPosition(nBaseX + l, nBaseY + t);
            m_AwardCount[i].Init(&IniAward, "MailAwardItemCount");
            m_AwardCount[i].GetPosition(&l, &t);
            m_AwardCount[i].SetPosition(nBaseX + l, nBaseY + t);
        }
    }
    Clean();
}

void KUiMailDetail::Update(const KMailUiDetail* p)
{
    if (!p)
    {
        Clean();
        return;
    }
    m_BigTitle.SetText(p->szTitle);
    m_SendTime.SetText(p->szTime);
    m_SenderValue.SetText(p->szSender);
    m_TitleValue.SetText(p->szTitle);

    // noi dung: 2.0 xuong dong bang <enter>; KWndText (OutputRichText) dung '\n'
    static char szContent[MAILUI_CONTENT_LEN];
    int n = 0;
    const char* s = p->szContent;
    while (*s && n < MAILUI_CONTENT_LEN - 1)
    {
        if (strncmp(s, "<enter>", 7) == 0)
        {
            szContent[n++] = '\n';
            s += 7;
        }
        else
            szContent[n++] = *s++;
    }
    szContent[n] = 0;
    m_Content.SetText(szContent);
    m_Content.SetTopLine(0);

    for (int i = 0; i < MAILUI_MAX_AWARD; i++)
    {
        // nha vat pham tam cu truoc khi ve o moi
        sReleaseAwardItem(&m_nAwardItemIdx[i]);
        m_AwardBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
        m_AwardBox[i].Hide();
        m_AwardSpr[i].Hide();

        if (i >= p->nAwardCount)
        {
            m_AwardBg[i].Hide();
            m_AwardCount[i].Hide();
            continue;
        }
        const KMailUiAward* pA = &p->Award[i];
        m_AwardBg[i].Show();
        // [MAIL 04/09 D13] o vat pham that (KWndObjectBox) da duoc ENGINE ve san so chong o goc,
        // nen KHONG ve them nhan so nua (truoc day hien hai so chong nhau: "500" va "500").
        int bBoxShown = 0;
        if (pA->nKind == MAILAWARD_ITEM)
        {
            // dung lai vat pham trong Item[] cua client tu ChatItem (CoreShell.cpp GDI_ITEM_CHAT)
            int nIdx = g_pCoreShell ? g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&pA->Item) : 0;
            if (nIdx > 0)
            {
                m_nAwardItemIdx[i] = nIdx;
                m_AwardBox[i].HoldObject(CGOG_PLAYERSELLITEM, (unsigned int)nIdx, 1, 1);
                m_AwardBox[i].Show();
                bBoxShown = 1;
            }
        }
        else if (pA->szIcon[0])
        {
            m_AwardSpr[i].SetImage(ISI_T_SPR, pA->szIcon);
            m_AwardSpr[i].Show();
            char szTip[256];
            if (pA->szDesc[0])
                _snprintf(szTip, sizeof(szTip) - 1, "%s: %s", pA->szName, pA->szDesc);
            else
                _snprintf(szTip, sizeof(szTip) - 1, "%s", pA->szName);
            szTip[sizeof(szTip) - 1] = 0;
            m_AwardSpr[i].SetToolTipInfo(szTip, sizeof(szTip));
        }
        // [A8 04/09] o vat pham chi ve duoc so <= 255 (m_bStack la BYTE); tren nguong do
        // KMailClient dat stack = 1 va ta tu ve nhan so that.
        if (pA->nCount > 1 && (!bBoxShown || pA->nCount > 255))
            m_AwardCount[i].SetIntText(pA->nCount);
        else
            m_AwardCount[i].SetText("");
        m_AwardCount[i].Show();
    }
}

void KUiMailDetail::Clean()
{
    m_BigTitle.SetText("");
    m_SendTime.SetText("");
    m_SenderValue.SetText("");
    m_TitleValue.SetText("");
    m_Content.SetText("");
    for (int i = 0; i < MAILUI_MAX_AWARD; i++)
    {
        sReleaseAwardItem(&m_nAwardItemIdx[i]);
        m_AwardBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
        m_AwardBox[i].Hide();
        m_AwardBg[i].Hide();
        m_AwardSpr[i].Hide();
        m_AwardCount[i].Hide();
    }
}

//==================================================================
// KUiMailList (trang InBox)
//==================================================================
KUiMailList::KUiMailList()
{
    m_nCount = 0;
    m_nTop = 0;
    m_nSelId = 0;
    m_nFilterCount = 0;
    m_nFilterCur = 1;
    memset(m_bChecked, 0, sizeof(m_bChecked));
    memset(m_szFilter, 0, sizeof(m_szFilter));
    strcpy(m_szDayUnit, "");
}

// [MAIL 03/09 D7] khung cuon chuyen tiep thong bao cua hang / thanh cuon len KUiMailList
int KUiMailScrollWnd::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    if ((uMsg == WND_N_BUTTON_CLICK || uMsg == WND_N_SCORLLBAR_POS_CHANGED) && m_pParentWnd)
        return m_pParentWnd->WndProc(uMsg, uParam, nParam);
    return KWndImage::WndProc(uMsg, uParam, nParam);
}

void KUiMailList::Build()
{
    AddChild(&m_ScrollWnd);
    int i;
    for (i = 0; i < MAILUI_ROW_COUNT; i++)
    {
        m_Row[i].Build();
        m_ScrollWnd.AddChild(&m_Row[i]);
    }
    m_ScrollWnd.AddChild(&m_Scroll);
    m_Detail.Build();
    AddChild(&m_Detail);
    AddChild(&m_BtnAccept);
    AddChild(&m_BtnDelOne);
    AddChild(&m_BtnDelAll);
    AddChild(&m_AutoDelLabel);
    AddChild(&m_AutoDelChk);
    AddChild(&m_SelAllLabel);
    AddChild(&m_SelAllChk);
    AddChild(&m_CountLabel);
    AddChild(&m_CountValue);
    AddChild(&m_FilterTitle);
    AddChild(&m_FilterBg);
    AddChild(&m_FilterBtn);
}

void KUiMailList::LoadScheme(const char* pScheme)
{
    char        Buff[256];
    KIniFile    Ini;
    KIniFile    IniHeader;
    int         i;

    sprintf(Buff, "%s\\%s", pScheme, MAIL_LIST_INI);
    if (Ini.Load(Buff))
    {
        Init(&Ini, "Main");
        m_BtnAccept.Init(&Ini, "ItemAccept_Btn");
        m_BtnDelOne.Init(&Ini, "DeleteMail_Btn");
        m_BtnDelAll.Init(&Ini, "DeleteAllMail_Btn");
        m_AutoDelLabel.Init(&Ini, "AutoDeleteLable");
        m_AutoDelChk.Init(&Ini, "AutoDelete_Btn");
        m_SelAllLabel.Init(&Ini, "SelectAllMailLable");
        m_SelAllChk.Init(&Ini, "SelectAllMail_Btn");
        m_CountLabel.Init(&Ini, "MailCountLable");
        m_CountValue.Init(&Ini, "MailCountValue");
        m_FilterTitle.Init(&Ini, "MailFilterTitle");
        m_FilterBg.Init(&Ini, "MailFilterBg");
        m_FilterBtn.Init(&Ini, "MailFilterBtn");
        m_ScrollWnd.Init(&Ini, "MailListScroll");
        m_Scroll.Init(&Ini, "MailListScroll_ScrollBar");

        Ini.GetInteger("MailFilterMenu", "MenuItemCount", 0, &m_nFilterCount);
        if (m_nFilterCount > MAILUI_FILTER_MAX)
            m_nFilterCount = MAILUI_FILTER_MAX;
        if (m_nFilterCount < 0)
            m_nFilterCount = 0;
        for (i = 0; i < m_nFilterCount; i++)
        {
            char szKey[32];
            sprintf(szKey, "MenuText%d", i + 1);
            m_szFilter[i][0] = 0;
            Ini.GetString("MailFilterMenu", szKey, "", m_szFilter[i], sizeof(m_szFilter[i]) - 1);
        }
        if (m_nFilterCount > 0)
            m_FilterBtn.SetText(m_szFilter[0]);
    }

    sprintf(Buff, "%s\\%s", pScheme, MAIL_HEADER_INI);
    if (IniHeader.Load(Buff))
    {
        // "Text=30 ngay" -> giu lai DON VI sau con so
        char szText[64];
        szText[0] = 0;
        IniHeader.GetString("MailDateLable", "Text", "", szText, sizeof(szText) - 1);
        const char* q = szText;
        while (*q && ((*q >= '0' && *q <= '9') || *q == ' '))
            q++;
        sCopyStr(m_szDayUnit, sizeof(m_szDayUnit), q);

        for (i = 0; i < MAILUI_ROW_COUNT; i++)
        {
            int l, t;
            m_Row[i].LoadScheme(&IniHeader);
            m_Row[i].GetPosition(&l, &t);
            m_Row[i].SetPosition(l, i * MAILUI_ROW_HEIGHT);
        }
    }

    m_Detail.LoadScheme(pScheme);
    m_Scroll.SetValueRange(0, 0);
    UpdateCount();
    Refresh();
}

int KUiMailList::FindHeader(int nId)
{
    for (int i = 0; i < m_nCount; i++)
    {
        if (m_Header[i].nId == nId)
            return i;
    }
    return -1;
}

int KUiMailList::RowOf(KWndWindow* pWnd, int* pnKind)
{
    for (int i = 0; i < MAILUI_ROW_COUNT; i++)
    {
        if (pWnd == (KWndWindow*)&m_Row[i].m_Click)
        {
            if (pnKind) *pnKind = 0;
            return i;
        }
        if (pWnd == (KWndWindow*)&m_Row[i].m_Check)
        {
            if (pnKind) *pnKind = 1;
            return i;
        }
    }
    return -1;
}

void KUiMailList::SendOp(int nOp, int nExtra, int nParam)
{
    sSendOp(nOp, nExtra, nParam);
}

void KUiMailList::Refresh()
{
    int nMax = m_nCount - MAILUI_ROW_COUNT;
    if (nMax < 0)
        nMax = 0;
    if (m_nTop > nMax)
        m_nTop = nMax;
    if (m_nTop < 0)
        m_nTop = 0;
    m_Scroll.SetValueRange(0, nMax);
    if (m_Scroll.GetScrollPos() != m_nTop)
        m_Scroll.SetScrollPos(m_nTop);

    for (int i = 0; i < MAILUI_ROW_COUNT; i++)
    {
        int nIdx = m_nTop + i;
        if (nIdx >= 0 && nIdx < m_nCount)
        {
            m_Row[i].Fill(&m_Header[nIdx], m_szDayUnit,
                          m_Header[nIdx].nId == m_nSelId, m_bChecked[nIdx]);
        }
        else
            m_Row[i].ClearRow();
    }
}

void KUiMailList::AddHeader(const KMailUiHeader* p)
{
    if (!p)
        return;
    int nIdx = FindHeader(p->nId);
    if (nIdx >= 0)
    {
        m_Header[nIdx] = *p;
    }
    else
    {
        if (m_nCount >= MAILUI_MAX_MAIL)
            return;
        m_Header[m_nCount] = *p;
        m_bChecked[m_nCount] = 0;
        m_nCount++;
    }
    UpdateCount();
    Refresh();
}

void KUiMailList::SetHeader(const KMailUiHeader* p)
{
    if (!p)
        return;
    int nIdx = FindHeader(p->nId);
    if (nIdx < 0)
    {
        AddHeader(p);
        return;
    }
    m_Header[nIdx] = *p;
    Refresh();
}

void KUiMailList::DelOne(int nId)
{
    int nIdx = FindHeader(nId);
    if (nIdx < 0)
        return;
    for (int i = nIdx; i + 1 < m_nCount; i++)
    {
        m_Header[i] = m_Header[i + 1];
        m_bChecked[i] = m_bChecked[i + 1];
    }
    m_nCount--;
    if (m_nSelId == nId)
        m_nSelId = 0;
    UpdateCount();
    Refresh();
}

void KUiMailList::CleanList()
{
    m_nCount = 0;
    m_nTop = 0;
    m_nSelId = 0;
    memset(m_bChecked, 0, sizeof(m_bChecked));
    m_SelAllChk.CheckButton(0);
    UpdateCount();
    Refresh();
}

void KUiMailList::UpdateCount()
{
    char szBuf[32];
    sprintf(szBuf, "%d/%d", m_nCount, MAILUI_MAX_MAIL);
    m_CountValue.SetText(szBuf);
}

void KUiMailList::SetBtnStatus(const KMailUiBtnStatus* p)
{
    if (!p)
        return;
    if (p->nAcceptShow) m_BtnAccept.Show(); else m_BtnAccept.Hide();
    m_BtnAccept.Enable(p->nAcceptEnable ? TRUE : FALSE);
    if (p->nDelOneShow) m_BtnDelOne.Show(); else m_BtnDelOne.Hide();
    m_BtnDelOne.Enable(p->nDelOneEnable ? TRUE : FALSE);
    if (p->nDelAllShow) m_BtnDelAll.Show(); else m_BtnDelAll.Hide();
    m_BtnDelAll.Enable(p->nDelAllEnable ? TRUE : FALSE);
}

void KUiMailList::Select(int nId)
{
    m_nSelId = nId;
    // cuon toi hang dang chon neu no nam ngoai vung nhin
    int nIdx = FindHeader(nId);
    if (nIdx >= 0)
    {
        if (nIdx < m_nTop)
            m_nTop = nIdx;
        else if (nIdx >= m_nTop + MAILUI_ROW_COUNT)
            m_nTop = nIdx - MAILUI_ROW_COUNT + 1;
    }
    Refresh();
}

void KUiMailList::SetFilterText(int nIndex)
{
    if (nIndex < 1 || nIndex > m_nFilterCount)
        return;
    m_nFilterCur = nIndex;
    m_FilterBtn.SetText(m_szFilter[nIndex - 1]);
}

void KUiMailList::PopupFilter()
{
    if (m_nFilterCount <= 0)
        return;
    KPopupMenuData* pMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(m_nFilterCount));
    if (!pMenu)
        return;
    KPopupMenu::InitMenuData(pMenu, m_nFilterCount);
    pMenu->nNumItem = 0;
    pMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
    for (int i = 0; i < m_nFilterCount; i++)
    {
        strncpy(pMenu->Items[i].szData, m_szFilter[i], 63);
        pMenu->Items[i].szData[63] = 0;
        pMenu->Items[i].uDataLen = strlen(pMenu->Items[i].szData);
        pMenu->nNumItem++;
    }
    int l, t, w, h;
    m_FilterBtn.GetAbsolutePos(&l, &t);
    m_FilterBtn.GetSize(&w, &h);
    pMenu->nX = l;
    pMenu->nY = t + h;
    KPopupMenu::Popup(pMenu, this, MAILUI_MENU_FILTER);
}

int KUiMailList::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        {
            int nKind = 0;
            int nRow = RowOf((KWndWindow*)uParam, &nKind);
            if (nRow >= 0)
            {
                int nIdx = m_nTop + nRow;
                if (nIdx < 0 || nIdx >= m_nCount)
                    return 1;
                if (nKind == 0)
                {
                    m_nSelId = m_Header[nIdx].nId;
                    Refresh();
                    SendOp(MAILUI_OP_SELECT, 0, m_nSelId);
                    SendOp(MAILUI_OP_UPDATE, 0, 0);
                }
                else
                {
                    int bOn = m_Row[nRow].m_Check.IsButtonChecked() ? 1 : 0;
                    m_bChecked[nIdx] = (unsigned char)bOn;
                    // extra: bit0 = trang thai, bit1 = nguoi choi tu bam
                    SendOp(MAILUI_OP_CHECK, bOn | 2, m_Header[nIdx].nId);
                    SendOp(MAILUI_OP_UPDATE, 0, 0);
                }
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_BtnAccept)
            {
                if (m_nSelId > 0)
                    SendOp(MAILUI_OP_ACCEPT, 0, m_nSelId);
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_BtnDelOne)
            {
                SendOp(MAILUI_OP_DELETE_ONE, 0, 0);
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_BtnDelAll)
            {
                SendOp(MAILUI_OP_DELETE_CHECKED, 0, 0);
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_SelAllChk)
            {
                int bOn = m_SelAllChk.IsButtonChecked() ? 1 : 0;
                for (int i = 0; i < m_nCount; i++)
                {
                    m_bChecked[i] = (unsigned char)bOn;
                    SendOp(MAILUI_OP_CHECK, bOn, m_Header[i].nId);
                }
                Refresh();
                SendOp(MAILUI_OP_UPDATE, 0, 0);
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_AutoDelChk)
            {
                SendOp(MAILUI_OP_AUTO_DELETE, 0, m_AutoDelChk.IsButtonChecked() ? 1 : 0);
                return 1;
            }
            if (uParam == (unsigned int)(KWndWindow*)&m_FilterBtn)
            {
                PopupFilter();
                return 1;
            }
        }
        break;
    case WND_N_SCORLLBAR_POS_CHANGED:
        if (uParam == (unsigned int)(KWndWindow*)&m_Scroll)
        {
            m_nTop = nParam;
            Refresh();
            return 1;
        }
        break;
    case WND_M_MENUITEM_SELECTED:
        if (HIWORD(nParam) == MAILUI_MENU_FILTER && (short)LOWORD(nParam) >= 0)
        {
            int nIdx = (short)LOWORD(nParam);
            SetFilterText(nIdx + 1);
            SendOp(MAILUI_OP_FILTER, 0, nIdx);
            return 1;
        }
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return KWndImage::WndProc(uMsg, uParam, nParam);
}

//==================================================================
// KUiMailManager
//==================================================================
KUiMailManager::KUiMailManager()
{
}

KUiMailManager::~KUiMailManager()
{
}

static KUiMailManager* sEnsureManager()
{
    KUiMailManager* p = KUiMailManager::GetSelf();
    if (!p)
    {
        p = KUiMailManager::OpenWindow();
        if (p)
            p->Hide();
    }
    return p;
}

KUiMailManager* KUiMailManager::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiMailManager;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->m_InBox.CheckButton(1);
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
        int nX = 0, nY = 0;
        ms_pSelf->GetPosition(&nX, &nY);
        sMailLog("[UI] OpenWindow: cua so tai (%d,%d) visible=%d", nX, nY, ms_pSelf->IsVisible() ? 1 : 0);
        sSendOp(MAILUI_OP_UPDATE, 0, 0);
    }
    else
        sMailLog("[UI] OpenWindow: khong tao duoc cua so");
    return ms_pSelf;
}

KUiMailManager* KUiMailManager::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

void KUiMailManager::CloseWindow(bool bDestory)
{
    if (ms_pSelf)
    {
        if (ms_pSelf->IsVisible())
            UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->Hide();
        sSendOp(MAILUI_OP_CLOSE, 0, 0);
        if (bDestory)
        {
            ms_pSelf->Destroy();
            ms_pSelf = NULL;
        }
    }
}

void KUiMailManager::Toggle()
{
    if (GetIfVisible())
        CloseWindow(false);
    else
        OpenWindow();
}

void KUiMailManager::Confirm(const KMailUiConfirm* p)
{
    if (!p)
        return;
    UIMessageBox(p->szText, ms_pSelf ? (KWndWindow*)ms_pSelf : NULL,
                 p->szYes[0] ? p->szYes : "OK",
                 p->szNo[0] ? p->szNo : NULL,
                 MAILUI_CONFIRM_PARAM);
}

void KUiMailManager::Initialize()
{
    AddChild(&m_Title);
    AddChild(&m_InBox);
    AddChild(&m_Close);
    m_List.Build();
    AddChild(&m_List);

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    Wnd_AddWindow(this);
}

void KUiMailManager::LoadScheme(const char* pScheme)
{
    if (!ms_pSelf)
        return;
    char        Buff[256];
    KIniFile    Ini;
    sprintf(Buff, "%s\\%s", pScheme, MAIL_MANAGER_INI);
    if (Ini.Load(Buff))
    {
        ms_pSelf->Init(&Ini, "Main");
        ms_pSelf->m_Title.Init(&Ini, "Title");
        ms_pSelf->m_InBox.Init(&Ini, "InBox");
        ms_pSelf->m_Close.Init(&Ini, "Close");
        sMailLog("[UI] da nap %s", Buff);
    }
    else
        sMailLog("[UI] KHONG nap duoc %s", Buff);
    ms_pSelf->m_List.LoadScheme(pScheme);
}

int KUiMailManager::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        if (uParam == (unsigned int)(KWndWindow*)&m_Close)
        {
            CloseWindow(false);
            return 1;
        }
        if (uParam == (unsigned int)(KWndWindow*)&m_InBox)
        {
            m_InBox.CheckButton(1);     // chi co mot tab
            return 1;
        }
        break;
    case WND_M_OTHER_WORK_RESULT:
        if (uParam == MAILUI_CONFIRM_PARAM)
        {
            // nut dau (dong y) = WND_OPER_RESULT(0)
            sSendOp(MAILUI_OP_CONFIRM_RESULT, 0, (nParam == WND_OPER_RESULT(0)) ? 1 : 0);
            return 1;
        }
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return KWndImage::WndProc(uMsg, uParam, nParam);
}

//==================================================================
// KUiMailIcon
//==================================================================
KUiMailIcon::KUiMailIcon()
{
    m_bBlink = 0;
    m_nFrame = 0;
    m_bUnread = 0;
    m_nBlinkLeft = 0;
}

void KUiMailIcon::Initialize()
{
    AddChild(&m_Btn);
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);
    Wnd_AddWindow(this);
}

void KUiMailIcon::LoadScheme(const char* pScheme)
{
    if (!ms_pSelf)
        return;
    char        Buff[256];
    KIniFile    Ini;
    sprintf(Buff, "%s\\%s", pScheme, MAIL_ICON_INI);
    if (Ini.Load(Buff))
    {
        ms_pSelf->Init(&Ini, "Main");
        ms_pSelf->m_Btn.Init(&Ini, "MailBtn");
        // [MAIL 03/09 D4] duoi bieu tuong Bau Cua: ini [Main] = vi tri 800x600 (UiPlayerBar [SpringGame] 765,243 50x50);
        // 1024 -> neo cot icon phai nhu UiPlayerBar.cpp (x = 1024 - 30), giu Top.
        int nX = 0, nY = 0;
        ms_pSelf->GetPosition(&nX, &nY);
        if (SCREEN_WIDTH == 1024)
        {
            nX = SCREEN_WIDTH - 30;
            ms_pSelf->SetPosition(nX, nY);
        }
        sMailLog("[UI] bieu tuong thu tai (%d,%d), man hinh rong %d", nX, nY, SCREEN_WIDTH);
    }
    else
        sMailLog("[UI] KHONG nap duoc %s", Buff);
}

// [MAIL 03/09 D4] bieu tuong thu LUON hien sau khi may chu gui trang header dau (dang nhap), nhu bieu tuong Bau Cua;
// bVisible (= con thu chua doc) chi bat/tat nhap nhay. Bam vao -> uimail.lua mo hop thu ngay (khong qua Tin Su).
void KUiMailIcon::SetVisible(int bVisible)
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiMailIcon;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (!ms_pSelf)
        return;
    ms_pSelf->m_Btn.Show();
    ms_pSelf->Show();
    // [D8] khong an/hien nut nua (an = bam xuyen qua, chu bao 'bam la mat, kho trung'): con thu chua doc = khung HOT
    ms_pSelf->m_bUnread = bVisible ? 1 : 0;
    if (!ms_pSelf->m_bBlink)
        ms_pSelf->m_Btn.SetFrame(ms_pSelf->m_bUnread ? MAILUI_ICON_FRAME_HOT : MAILUI_ICON_FRAME_NORMAL);
}

// [D8] thu moi: doi khung 0 <-> 2 moi 9 khung trong 6 giay, roi giu khung HOT (nut luon ve, luon bam duoc)
void KUiMailIcon::Blink()
{
    SetVisible(1);
    if (ms_pSelf)
    {
        ms_pSelf->m_bBlink = 1;
        ms_pSelf->m_nFrame = 0;
        ms_pSelf->m_nBlinkLeft = MAILUI_BLINK_TOTAL;
    }
}

void KUiMailIcon::Breathe()
{
    if (!m_bBlink)
        return;
    m_nFrame++;
    if (m_nBlinkLeft > 0)
        m_nBlinkLeft--;
    if (m_nBlinkLeft <= 0)
    {
        m_bBlink = 0;
        m_Btn.SetFrame(m_bUnread ? MAILUI_ICON_FRAME_HOT : MAILUI_ICON_FRAME_NORMAL);
        return;
    }
    if (m_nFrame >= MAILUI_BLINK_FRAMES)
    {
        m_nFrame = 0;
        m_Btn.SetFrame(m_Btn.GetCurrentFrame() == MAILUI_ICON_FRAME_HOT ? MAILUI_ICON_FRAME_NORMAL : MAILUI_ICON_FRAME_HOT);
    }
}

int KUiMailIcon::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    if (uMsg == WND_N_BUTTON_CLICK && uParam == (unsigned int)(KWndWindow*)&m_Btn)
    {
        m_bBlink = 0;
        m_Btn.Show();
        m_Btn.SetFrame(m_bUnread ? MAILUI_ICON_FRAME_HOT : MAILUI_ICON_FRAME_NORMAL);
        sSendOp(MAILUI_OP_ICON_CLICK, 0, 0);
        return 1;
    }
    return KWndImage::WndProc(uMsg, uParam, nParam);
}

//==================================================================
// Core -> UI
//==================================================================
// [MAIL 03/09 D4] GDCNI_GAME_START: bieu tuong thu luon hien (nhap nhay neu da co thu chua doc) + mo hop thu neu may chu da bao
void KUiMail_OnGameStart()
{
    s_bMailGameStarted = 1;
    KUiMailIcon::SetVisible(s_nMailPendingIcon > 0 ? 1 : 0);
    if (s_bMailPendingOpen)
        KUiMailManager::OpenWindow();
    sMailLog("[UI] GAME_START: icon=%d mo=%d", s_nMailPendingIcon, s_bMailPendingOpen);
    s_bMailPendingOpen = 0;
    s_nMailPendingIcon = -1;
}

// [MAIL 03/09 D4] GDCNI_EXIT_GAME: huy cua so + bieu tuong, xoa danh sach thu trong state Lua (doi nhan vat)
void KUiMail_OnGameExit()
{
    s_bMailGameStarted = 0;
    s_bMailPendingOpen = 0;
    s_nMailPendingIcon = -1;
    KUiMailManager::CloseWindow(true);
    KUiMailIcon::Release();
    sSendOp(MAILUI_OP_RESET, 0, 0);
    sMailLog("[UI] EXIT_GAME: da don hop thu");
}

void KUiMailIcon::Release()
{
    if (ms_pSelf)
    {
        ms_pSelf->Destroy();
        ms_pSelf = NULL;
    }
}

void KUiMail_OnCoreCmd(unsigned int uCmd, int nParam)
{
    KUiMailManager* pMgr;
    sMailLog("[UI] cmd=%u param=%d", uCmd, nParam);
    if (!s_bMailGameStarted)
    {
        // [D4] chua GDCNI_GAME_START: chi ghi nho; lenh sua danh sach (cua so an) van chay binh thuong
        switch (uCmd)
        {
        case MAILUI_CMD_OPEN:
        case MAILUI_CMD_SWITCH_MANAGER:
            s_bMailPendingOpen = 1;
            return;
        case MAILUI_CMD_ICON_VISIBLE:
            s_nMailPendingIcon = nParam;
            return;
        case MAILUI_CMD_NEW_MAIL_BLINK:
            s_nMailPendingIcon = 1;
            return;
        default:
            break;
        }
    }
    switch (uCmd)
    {
    case MAILUI_CMD_ICON_VISIBLE:
        KUiMailIcon::SetVisible(nParam);
        break;
    case MAILUI_CMD_NEW_MAIL_BLINK:
        KUiMailIcon::Blink();
        break;
    case MAILUI_CMD_OPEN:
        KUiMailManager::OpenWindow();
        break;
    case MAILUI_CMD_SWITCH_MANAGER:
        KUiMailManager::Toggle();
        break;
    case MAILUI_CMD_CLOSE:
        KUiMailManager::CloseWindow(false);
        break;
    case MAILUI_CMD_CONFIRM:
        KUiMailManager::Confirm((const KMailUiConfirm*)nParam);
        break;
    case MAILUI_CMD_ADD_HEADER:
        if ((pMgr = sEnsureManager()) != NULL)
            pMgr->m_List.AddHeader((const KMailUiHeader*)nParam);
        break;
    case MAILUI_CMD_SET_HEADER:
        if ((pMgr = sEnsureManager()) != NULL)
            pMgr->m_List.SetHeader((const KMailUiHeader*)nParam);
        break;
    case MAILUI_CMD_DEL_ONE:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
            pMgr->m_List.DelOne(nParam);
        break;
    case MAILUI_CMD_CLEAN_LIST:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
            pMgr->m_List.CleanList();
        break;
    case MAILUI_CMD_CLEAN_DETAIL:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
            pMgr->m_List.m_Detail.Clean();
        break;
    case MAILUI_CMD_CLEAN_ALL:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
        {
            pMgr->m_List.CleanList();
            pMgr->m_List.m_Detail.Clean();
        }
        break;
    case MAILUI_CMD_UPDATE_COUNT:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
            pMgr->m_List.UpdateCount();
        break;
    case MAILUI_CMD_BTN_STATUS:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
            pMgr->m_List.SetBtnStatus((const KMailUiBtnStatus*)nParam);
        break;
    case MAILUI_CMD_UPDATE_DETAIL:
        if ((pMgr = sEnsureManager()) != NULL)
            pMgr->m_List.m_Detail.Update((const KMailUiDetail*)nParam);
        break;
    case MAILUI_CMD_SELECT:
        if ((pMgr = KUiMailManager::GetSelf()) != NULL)
            pMgr->m_List.Select(nParam);
        break;
    case MAILUI_CMD_SET_FILTER_TEXT:
        if ((pMgr = sEnsureManager()) != NULL)
            pMgr->m_List.SetFilterText(nParam);
        break;
    default:
        break;
    }
}
