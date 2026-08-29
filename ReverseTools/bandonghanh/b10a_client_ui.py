# -*- coding: utf-8 -*-
"""BAN DONG HANH - G4b: sinh 8 tep cua so client (S3Client\\Ui\\UiCase\\UiPartner*).

4 cua so theo dung 4 ini da rut tu ban Linux (b02, nam \\Ui\\Ui3\\):
  UiPartnerAttr  - "\\xCD\\xAC\\xB0\\xE9\\xCA\\xF4\\xD0\\xD4.ini"  (dong ho so 5 the)
  UiPartnerSkill - "\\xCD\\xAC\\xB0\\xE9\\xBC\\xBC\\xC4\\xDC.ini"
  UiPartnerBag   - "\\xCD\\xAC\\xB0\\xE9\\xB1\\xB3\\xB0\\xFC.ini"  (6x10)
  UiPartnerBar   - "\\xCD\\xAC\\xB0\\xE9\\xBF\\xEC\\xBD\\xDD\\xC0\\xB8.ini" (9 nut)

Du lieu: doc task value da dong bo (GDI_TASK_SAVE_VALUE, so do PTP_* trong
KPlayerPartner.h - hang nam NGOAI #ifdef _SERVER nen client include duoc).
Ghi de neu tep da ton tai (generator la nguon chan ly).
"""
import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

T = chr(9)
E = chr(13) + chr(10)
BS = chr(92)
Q = chr(34)

DST = r"D:\GAMEDEVNEW\Sources\S3Client\Ui\UiCase"

INI_ATTR = r"\xCD\xAC\xB0\xE9\xCA\xF4\xD0\xD4.ini"
INI_SKILL = r"\xCD\xAC\xB0\xE9\xBC\xBC\xC4\xDC.ini"
INI_BAG = r"\xCD\xAC\xB0\xE9\xB1\xB3\xB0\xFC.ini"
INI_BAR = r"\xCD\xAC\xB0\xE9\xBF\xEC\xBD\xDD\xC0\xB8.ini"


def ghi(name, s):
    p = os.path.join(DST, name)
    io.open(p, "w", encoding="latin-1", newline="").write(s.replace(chr(10), E).replace(E.replace(chr(10), ""), "").replace(chr(13) + chr(13), chr(13)))
    print("ghi", name, len(s), "ky tu")


# dung LF trong chuoi python roi doi sang CRLF mot lan
def crlf(s):
    return s.replace(chr(13), "").replace(chr(10), E)


def ghi2(name, s):
    p = os.path.join(DST, name)
    io.open(p, "w", encoding="latin-1", newline="").write(crlf(s))
    print("ghi", name)


# ===========================================================================
# 1. UiPartnerCommon.h - hang + ham doc task value dung chung (header-only)
# ===========================================================================
common_h = r'''/*******************************************************************************
File        : UiPartnerCommon.h
create data : 27-08-2026
Description : [BDH-G4] He BAN DONG HANH - phan dung chung cua 4 cua so client.
  Du lieu lay tu bang task value da duoc may chu dong bo tung o
  (KPlayer::SyncTaskValueToClient, id >= 256 qua kenh UI_TASKVALUE).
  So do o: KPlayerPartner.h (PTP_* / PTG_* nam ngoai #ifdef _SERVER).
*******************************************************************************/
#ifndef UIPARTNERCOMMON_H
#define UIPARTNERCOMMON_H

#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/KPlayerPartner.h"
#include "../../../core/src/KProtocol.h"    // keo theo Headers\KPartnerProtocol.h (PARTNER_OP_*)

extern iCoreShell* g_pCoreShell;

// doc mot o task value (ban sao phia client)
inline int PT_TaskVal(int nId)
{
    return g_pCoreShell ? g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, nId, 0) : 0;
}

// o rieng cua dong hanh nP (1-based)
inline int PT_P(int nP, int nOff)
{
    if (nP < 1 || nP > PARTNER_MAX_COUNT)
        return 0;
    return PT_TaskVal(PARTNER_TASK_P(nP - 1) + nOff);
}

inline int PT_Count()
{
    int n = 0;
    for (int i = 1; i <= PARTNER_MAX_COUNT; i++)
        if (PT_P(i, PTP_USED) > 0)
            n++;
    return n;
}

inline int PT_Cur()
{
    int n = PT_TaskVal(PTG_CURPARTNER);
    if (n < 0 || n > PARTNER_MAX_COUNT)
        n = 0;
    return n;
}

// ten 16 byte nam trong 4 o int
inline void PT_GetName(int nP, char* szOut17)
{
    int* p = (int*)szOut17;
    for (int i = 0; i < 4; i++)
        p[i] = PT_P(nP, PTP_NAME0 + i);
    szOut17[16] = 0;
}

// id trong mot o ky nang dang pack id*1000+level
inline int PT_SkillId(int nPack)  { return nPack / 1000; }
inline int PT_SkillLv(int nPack)  { return nPack % 1000; }

// mot cua so partner co dang xem du lieu -> bao refresh khi task value doi
void UiPartner_OnTaskValueChanged(int nTaskId);

// phim tat thao tac: 0 talk / 1 callout / 2 attack / 3 follow / 4 select next
void UiPartner_HotKey(int nWhat);

#endif // UIPARTNERCOMMON_H
'''

# ===========================================================================
# 2. UiPartnerAttr.h
# ===========================================================================
attr_h = r'''/*******************************************************************************
File        : UiPartnerAttr.h
create data : 27-08-2026
Description : [BDH-G4] Cua so THUOC TINH ban dong hanh (trang "dong ho so").
  Bo cuc: __INI_ATTR__ (rut tu ban Linux, da Viet hoa).
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
'''.replace("__INI_ATTR__", INI_ATTR)

# ===========================================================================
# 3. UiPartnerAttr.cpp
# ===========================================================================
S_SERIES = "{ %s }" % ", ".join('"%s"' % vn(x) for x in
                                ["Kim", "Mộc", "Thủy", "Hỏa", "Thổ"])
S_CHARS = "{ %s }" % ", ".join('"%s"' % vn(x) for x in
                               ["-", "Dũng mãnh", "Vệ chủ", "Lưu manh", "Nhu nhược"])

attr_cpp = r'''/*******************************************************************************
File        : UiPartnerAttr.cpp
create data : 27-08-2026
Description : [BDH-G4] Cua so THUOC TINH ban dong hanh.
  27 truong hien thi bam dung ten muc trong __INI_ATTR__ cua ban Linux
  (m_editExp/m_editLife/... - o day dung KWndText32 vi chi hien, rieng
  m_editName la KWndEdit32 nhap duoc, Type=2 trong ini).
  m_editEnergy / m_editFire ban goc mobile chua ro nguon du lieu -> hien "-"
  (ghi trong BANGIAO_BANDONGHANH_2708.md).
*******************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "KTabFile.h"
#include "../elem/wnds.h"
#include "UiPartnerCommon.h"
#include "UiPartnerAttr.h"
#include "UiPartnerSkill.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"

#define SCHEME_INI_PARTNER_ATTR "__INI_ATTR__"

KUiPartnerAttr* KUiPartnerAttr::ms_pSelf = NULL;

// bang nguong exp CONG DON (level_exp.txt - da chep sang client): dat nguong
// hang LEVEL=N thi len N+1. Doc 1 lan, khuon y het KPartnerTables::Load.
static int  s_nLevelExp[PARTNER_MAX_LEVEL + 2];
static BOOL s_bLevelExpLoaded = FALSE;

static void sLoadLevelExp()
{
    if (s_bLevelExpLoaded)
        return;
    s_bLevelExpLoaded = TRUE;
    memset(s_nLevelExp, 0, sizeof(s_nLevelExp));
    KTabFile Tab;
    if (Tab.Load((LPSTR)"\\settings\\partner\\level_exp.txt"))
    {
        for (int r = 2; r <= Tab.GetHeight(); r++)
        {
            int nLv = 0, nExp = 0;
            Tab.GetInteger(r, 1, 0, &nLv);
            Tab.GetInteger(r, 2, 0, &nExp);
            if (nLv >= 1 && nLv <= PARTNER_MAX_LEVEL)
                s_nLevelExp[nLv] = nExp;
        }
    }
}

// ten 27 muc trong ini, THU TU CO DINH - khop mang m_Field
static const char* s_szFieldSec[PARTNER_ATTR_FIELD_NUM] =
{
    "m_editExp",            // 0
    "m_editEnergy",         // 1  (chua ro nguon - hien "-")
    "m_editFire",           // 2  (chua ro nguon - hien "-")
    "m_editPufang",         // 3  khang vat ly  (skill 549)
    "m_editBingfang",       // 4  khang bang    (550)
    "m_editLeifang",        // 5  khang loi     (551)
    "m_editHuofang",        // 6  khang hoa     (552)
    "m_editDufang",         // 7  khang doc     (553)
    "m_editLevel",          // 8
    "m_editCharacter",      // 9
    "m_editLife",           // 10
    "m_editLifePotential",  // 11
    "m_editPower",          // 12
    "m_editPowerPotential", // 13
    "m_editHitRate",        // 14
    "m_editHitRatePotential", // 15
    "m_editDuobi",          // 16
    "m_editDuobiPotential", // 17
    "m_editRunV",           // 18
    "m_editRunVPotential",  // 19
    "m_editLuck",           // 20
    "m_editLuckPotential",  // 21
    "m_editWuxing",         // 22 ngo tinh = trung binh 6 tu chat
    "m_editQinHe",          // 23 than mat (emotion 0..100)
    "m_editExp",            // 24 (du phong - trung 0, khong dung)
    "m_editExp",            // 25
    "m_editExp",            // 26
};

static const char* s_szSeries[PARTNER_SERIES_NUM] = __S_SERIES__;
static const char* s_szCharacter[PARTNER_CHARACTER_NUM + 1] = __S_CHARS__;

KUiPartnerAttr::KUiPartnerAttr()
{
    m_nView = 0;
}

KUiPartnerAttr::~KUiPartnerAttr()
{
}

KUiPartnerAttr* KUiPartnerAttr::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

KUiPartnerAttr* KUiPartnerAttr::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiPartnerAttr;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->m_nView = PT_Cur();
        if (ms_pSelf->m_nView == 0)
            ms_pSelf->m_nView = 1;
        ms_pSelf->UpdateData();
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

void KUiPartnerAttr::CloseWindow()
{
    if (ms_pSelf)
    {
        ms_pSelf->Hide();
        ms_pSelf = NULL;
    }
}

void KUiPartnerAttr::OnTaskValueChanged()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        ms_pSelf->UpdateData();
}

void KUiPartnerAttr::Initialize()
{
    int i, j;
    sLoadLevelExp();
    AddChild(&m_Title);
    AddChild(&m_PagePartner);
    for (i = 0; i < PARTNER_UI_SLOT; i++)
        AddChild(&m_BtnTab[i]);
    AddChild(&m_Forget);
    AddChild(&m_SaveBtn);
    AddChild(&m_DeleteBtn);
    AddChild(&m_CloseBtn);
    AddChild(&m_SwitchBtn);
    AddChild(&m_editName);
    for (i = 0; i < PARTNER_ATTR_FIELD_NUM; i++)
        AddChild(&m_Field[i]);
    for (i = 0; i < PARTNER_UI_SLOT; i++)
        for (j = 0; j < PARTNER_ATTR_GIVEN_COL; j++)
            AddChild(&m_GivenIcon[i][j]);

    char Scheme[256];
    KIniFile Ini;
    g_UiBase.GetCurSchemePath(Scheme, 256);
    strcat(Scheme, "\\" SCHEME_INI_PARTNER_ATTR);
    if (Ini.Load(Scheme))
    {
        Init(&Ini, "Main");
        m_Title.Init(&Ini, "Title");
        m_PagePartner.Init(&Ini, "PagePartner");
        char szSec[32];
        for (i = 0; i < PARTNER_UI_SLOT; i++)
        {
            sprintf(szSec, "BtnPartner_%d", i);
            m_BtnTab[i].Init(&Ini, szSec);
        }
        m_Forget.Init(&Ini, "Forget");
        m_SaveBtn.Init(&Ini, "SaveBtn");
        m_DeleteBtn.Init(&Ini, "DeleteBtn");
        m_CloseBtn.Init(&Ini, "CloseBtn");
        m_SwitchBtn.Init(&Ini, "SwitchBtn");
        m_editName.Init(&Ini, "m_editName");
        for (i = 0; i < PARTNER_ATTR_FIELD_NUM; i++)
        {
            m_Field[i].Init(&Ini, s_szFieldSec[i]);
            // muc ini chi co Left/Top/Width/Height/Font/Color; KWndText32
            // sau Init o mau den alpha 0 se KHONG VE (bai hoc UiDiceItem) ->
            // mau ARGB alpha 0xFF, lay dung mau xanh la 0,255,0 trong ini.
            if (i >= 3 && i <= 7)
                m_Field[i].SetTextColor(0xFFFFD94E);    // 255,217,78 nhu ini
            else
                m_Field[i].SetTextColor(0xFF00FF00);
        }
        // 3 muc trung ten m_editExp o duoi (24..26) la du phong - an han
        for (i = 24; i < PARTNER_ATTR_FIELD_NUM; i++)
            m_Field[i].Hide();

        // 3 cot icon bam sinh canh 5 the (nhu trang ky nang)
        static const char* szCol[PARTNER_ATTR_GIVEN_COL] =
            { "KongfuIcon", "ToughIcon", "MissleIcon" };
        int nX, nY, nDY, nW, nH;
        for (j = 0; j < PARTNER_ATTR_GIVEN_COL; j++)
        {
            nX = nY = nDY = 0; nW = nH = 36;
            Ini.GetInteger(szCol[j], "XStart", 0, &nX);
            Ini.GetInteger(szCol[j], "YStart", 0, &nY);
            Ini.GetInteger(szCol[j], "YDistance", 51, &nDY);
            Ini.GetInteger(szCol[j], "Width", 36, &nW);
            Ini.GetInteger(szCol[j], "Height", 36, &nH);
            for (i = 0; i < PARTNER_UI_SLOT; i++)
            {
                m_GivenIcon[i][j].SetPosition(nX, nY + nDY * i);
                m_GivenIcon[i][j].SetSize(nW, nH);
            }
        }
    }
    Wnd_AddWindow(this);
}

void KUiPartnerAttr::UpdateData()
{
    int i, j;
    int nP = m_nView;
    BOOL bCo = (nP >= 1 && nP <= PARTNER_MAX_COUNT && PT_P(nP, PTP_USED) > 0);

    for (i = 0; i < PARTNER_UI_SLOT; i++)
        m_BtnTab[i].CheckButton(i == nP - 1);

    // 3 cot theo anh nen goc: [Vo cong = ky nang bam sinh] [Khang tinh = 5 skill
    // khang 549..553] [O ra chieu = ky nang tu hoc dang co] - moi cot doc theo
    // buoc YDistance, hien cua CON DANG XEM (m_nView), toi da PARTNER_UI_SLOT o.
    for (i = 0; i < PARTNER_UI_SLOT; i++)
    {
        int nPackVo = 0, nPackKhang = 0, nPackChieu = 0;
        if (bCo)
        {
            if (i < 8)
                nPackVo = PT_P(nP, PTP_SK_GIVEN0 + i);
            if (i < PARTNER_RESIST_NUM)
                nPackKhang = PT_P(nP, PTP_SK_RESIST0 + i);
            nPackChieu = PT_P(nP, PTP_SK_LEARNT0 + i);
        }
        if (nPackVo > 0)
            m_GivenIcon[i][0].HoldObject(CGOG_SKILL_FIGHT,
                PT_SkillId(nPackVo), PT_SkillLv(nPackVo), 0);
        else
            m_GivenIcon[i][0].HoldObject(CGOG_NOTHING, 0, 0, 0);
        if (nPackKhang > 0)
            m_GivenIcon[i][1].HoldObject(CGOG_SKILL_FIGHT,
                PT_SkillId(nPackKhang), PT_SkillLv(nPackKhang), 0);
        else
            m_GivenIcon[i][1].HoldObject(CGOG_NOTHING, 0, 0, 0);
        if (nPackChieu > 0)
            m_GivenIcon[i][2].HoldObject(CGOG_SKILL_FIGHT,
                PT_SkillId(nPackChieu), PT_SkillLv(nPackChieu), 0);
        else
            m_GivenIcon[i][2].HoldObject(CGOG_NOTHING, 0, 0, 0);
    }

    char szBuf[64];
    if (!bCo)
    {
        m_editName.SetText("-");
        for (i = 0; i < 24; i++)
            m_Field[i].SetText("-");
        return;
    }

    char szName[20];
    PT_GetName(nP, szName);
    m_editName.SetText(szName[0] ? szName : "-");

    int nLevel = PT_P(nP, PTP_LEVEL);
    if (nLevel < 1) nLevel = 1;

    // exp CONG DON "hien tai/nguong len cap ke" (bang level_exp.txt:
    // dat nguong hang LEVEL=N thi len N+1 - do thuc 5000->lv7, 10000->lv10)
    if (nLevel < PARTNER_MAX_LEVEL && s_nLevelExp[nLevel] > 0)
        sprintf(szBuf, "%d/%d", PT_P(nP, PTP_EXP), s_nLevelExp[nLevel]);
    else
        sprintf(szBuf, "%d", PT_P(nP, PTP_EXP));
    m_Field[0].SetText(szBuf);
    m_Field[1].SetText("-");
    m_Field[2].SetText("-");

    // 5 khang = cap ky nang khang 549..553 (pack id*1000+lv)
    for (i = 0; i < PARTNER_RESIST_NUM; i++)
    {
        int nPack = PT_P(nP, PTP_SK_RESIST0 + i);
        if (nPack > 0)
            sprintf(szBuf, "%d", PT_SkillLv(nPack));
        else
            strcpy(szBuf, "0");
        m_Field[3 + i].SetText(szBuf);
    }

    sprintf(szBuf, "%d", nLevel);
    m_Field[8].SetText(szBuf);

    int nChar = PT_P(nP, PTP_CHARACTER);
    if (nChar < 1 || nChar > PARTNER_CHARACTER_NUM) nChar = 0;
    m_Field[9].SetText(s_szCharacter[nChar]);

    // 6 thuoc tinh: init + inc * (lv - 1), fixed-point x10000.
    // Cot ben phai (m_editXxxPotential) theo anh nen goc la "TU CHAT" (apt 1..10)
    // cua tung thuoc tinh - KHONG phai muc tang/cap.
    for (i = 0; i < PARTNER_APT_NUM; i++)
    {
        double dInit = PT_P(nP, PTP_ATTRINIT0 + i) / 10000.0;
        double dInc  = PT_P(nP, PTP_ATTRINC0 + i) / 10000.0;
        double dVal  = dInit + dInc * (nLevel - 1);
        if (i == 0)
            sprintf(szBuf, "%d", (int)dVal);        // sinh luc so nguyen
        else
            sprintf(szBuf, "%.1f", dVal);
        m_Field[10 + i * 2].SetText(szBuf);
        sprintf(szBuf, "%d", PT_P(nP, PTP_APT0 + i));
        m_Field[11 + i * 2].SetText(szBuf);
    }

    // o "Wuxing" theo anh nen goc la NGU HANH (he Kim/Moc/Thuy/Hoa/Tho)
    int nSeries = PT_P(nP, PTP_SERIES);
    if (nSeries < 0 || nSeries >= PARTNER_SERIES_NUM) nSeries = 0;
    m_Field[22].SetText(s_szSeries[nSeries]);

    sprintf(szBuf, "%d/100", PT_P(nP, PTP_EMOTION));
    m_Field[23].SetText(szBuf);
}

int KUiPartnerAttr::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
        {
            CloseWindow();
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
        {
            int nView = m_nView;
            CloseWindow();
            KUiPartnerSkill* pSkill = KUiPartnerSkill::OpenWindow();
            if (pSkill)
                pSkill->SetView(nView);
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_SaveBtn)
        {
            // luu ten (neu doi) + chon lam duong nhiem
            if (m_nView >= 1 && PT_P(m_nView, PTP_USED) > 0)
            {
                char szMoi[32], szCu[20];
                memset(szMoi, 0, sizeof(szMoi));
                m_editName.GetText(szMoi, 17, false);
                PT_GetName(m_nView, szCu);
                if (szMoi[0] && strcmp(szMoi, szCu) != 0)
                {
                    if (m_nView != PT_Cur())
                        g_pCoreShell->OperationRequest(GOI_PARTNER_OP,
                            PARTNER_OP_SELECT, m_nView);
                    g_pCoreShell->OperationRequest(GOI_PARTNER_OP,
                        PARTNER_OP_RENAME, (int)szMoi);
                }
                else if (m_nView != PT_Cur())
                {
                    g_pCoreShell->OperationRequest(GOI_PARTNER_OP,
                        PARTNER_OP_SELECT, m_nView);
                }
            }
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_DeleteBtn)
        {
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_DELETE, m_nView);
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_Forget)
        {
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_FORGETSKILL, m_nView);
        }
        else
        {
            for (int i = 0; i < PARTNER_UI_SLOT; i++)
            {
                if (uParam == (unsigned int)(KWndWindow*)&m_BtnTab[i])
                {
                    m_nView = i + 1;
                    UpdateData();
                    break;
                }
            }
        }
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}
'''.replace("__INI_ATTR__", INI_ATTR).replace("__S_SERIES__", S_SERIES).replace("__S_CHARS__", S_CHARS)

# ===========================================================================
# 4. UiPartnerSkill.h
# ===========================================================================
skill_h = r'''/*******************************************************************************
File        : UiPartnerSkill.h
create data : 27-08-2026
Description : [BDH-G4] Cua so KY NANG ban dong hanh.
  Bo cuc __INI_SKILL__: 5 hang (1 hang / the dong hanh);
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
'''.replace("__INI_SKILL__", INI_SKILL)

# ===========================================================================
# 5. UiPartnerSkill.cpp
# ===========================================================================
skill_cpp = r'''/*******************************************************************************
File        : UiPartnerSkill.cpp
create data : 27-08-2026
Description : [BDH-G4] Cua so KY NANG ban dong hanh (xem UiPartnerSkill.h).
*******************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiPartnerCommon.h"
#include "UiPartnerSkill.h"
#include "UiPartnerAttr.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"

#define SCHEME_INI_PARTNER_SKILL "__INI_SKILL__"

KUiPartnerSkill* KUiPartnerSkill::ms_pSelf = NULL;

KUiPartnerSkill::KUiPartnerSkill()
{
    m_nView = 0;
}

KUiPartnerSkill::~KUiPartnerSkill()
{
}

KUiPartnerSkill* KUiPartnerSkill::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

KUiPartnerSkill* KUiPartnerSkill::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiPartnerSkill;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->m_nView = PT_Cur();
        if (ms_pSelf->m_nView == 0)
            ms_pSelf->m_nView = 1;
        ms_pSelf->UpdateData();
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

void KUiPartnerSkill::CloseWindow()
{
    if (ms_pSelf)
    {
        ms_pSelf->Hide();
        ms_pSelf = NULL;
    }
}

void KUiPartnerSkill::OnTaskValueChanged()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        ms_pSelf->UpdateData();
}

void KUiPartnerSkill::SetView(int nP)
{
    if (nP >= 1 && nP <= PARTNER_MAX_COUNT)
    {
        m_nView = nP;
        UpdateData();
    }
}

void KUiPartnerSkill::Initialize()
{
    int i, j;
    AddChild(&m_Title);
    AddChild(&m_PagePartner);
    for (i = 0; i < PARTNER_UI_SLOT; i++)
        AddChild(&m_BtnTab[i]);
    AddChild(&m_Forget);
    AddChild(&m_SaveBtn);
    AddChild(&m_DeleteBtn);
    AddChild(&m_CloseBtn);
    AddChild(&m_SwitchBtn);
    for (i = 0; i < PARTNER_UI_SLOT; i++)
    {
        for (j = 0; j < PARTNER_SKILL_GIVEN_COL; j++)
            AddChild(&m_Given[i][j]);
        for (j = 0; j < PARTNER_SKILL_GRID_COL; j++)
            AddChild(&m_Learnt[i][j]);
    }

    char Scheme[256];
    KIniFile Ini;
    g_UiBase.GetCurSchemePath(Scheme, 256);
    strcat(Scheme, "\\" SCHEME_INI_PARTNER_SKILL);
    if (Ini.Load(Scheme))
    {
        Init(&Ini, "Main");
        m_Title.Init(&Ini, "Title");
        m_PagePartner.Init(&Ini, "PagePartner");
        char szSec[32];
        for (i = 0; i < PARTNER_UI_SLOT; i++)
        {
            sprintf(szSec, "BtnPartner_%d", i);
            m_BtnTab[i].Init(&Ini, szSec);
        }
        m_Forget.Init(&Ini, "Forget");
        m_SaveBtn.Init(&Ini, "SaveBtn");
        m_DeleteBtn.Init(&Ini, "DeleteBtn");
        m_CloseBtn.Init(&Ini, "CloseBtn");
        m_SwitchBtn.Init(&Ini, "SwitchBtn");

        // 3 cot bam sinh: [KongfuIcon][ToughIcon][MissleIcon]
        // XStart/YStart/YDistance/Width/Height (XDistance = 0 - cot doc)
        static const char* szCol[PARTNER_SKILL_GIVEN_COL] =
            { "KongfuIcon", "ToughIcon", "MissleIcon" };
        int nX, nY, nDY, nW, nH, nDX;
        for (j = 0; j < PARTNER_SKILL_GIVEN_COL; j++)
        {
            nX = nY = nDY = 0; nW = nH = 36;
            Ini.GetInteger(szCol[j], "XStart", 0, &nX);
            Ini.GetInteger(szCol[j], "YStart", 0, &nY);
            Ini.GetInteger(szCol[j], "YDistance", 51, &nDY);
            Ini.GetInteger(szCol[j], "Width", 36, &nW);
            Ini.GetInteger(szCol[j], "Height", 36, &nH);
            for (i = 0; i < PARTNER_UI_SLOT; i++)
            {
                m_Given[i][j].SetPosition(nX, nY + nDY * i);
                m_Given[i][j].SetSize(nW, nH);
            }
        }
        // luoi tu hoc [SkillsIcon]: XStart/XDistance/YStart/YDistance
        nX = nY = 0; nDX = 39; nDY = 51; nW = nH = 36;
        Ini.GetInteger("SkillsIcon", "XStart", 208, &nX);
        Ini.GetInteger("SkillsIcon", "YStart", 68, &nY);
        Ini.GetInteger("SkillsIcon", "XDistance", 39, &nDX);
        Ini.GetInteger("SkillsIcon", "YDistance", 51, &nDY);
        Ini.GetInteger("SkillsIcon", "Width", 36, &nW);
        Ini.GetInteger("SkillsIcon", "Height", 36, &nH);
        for (i = 0; i < PARTNER_UI_SLOT; i++)
            for (j = 0; j < PARTNER_SKILL_GRID_COL; j++)
            {
                m_Learnt[i][j].SetPosition(nX + nDX * j, nY + nDY * i);
                m_Learnt[i][j].SetSize(nW, nH);
            }
    }
    Wnd_AddWindow(this);
}

void KUiPartnerSkill::UpdateData()
{
    int i, j;
    for (i = 0; i < PARTNER_UI_SLOT; i++)
        m_BtnTab[i].CheckButton(i == m_nView - 1);

    for (i = 0; i < PARTNER_UI_SLOT; i++)
    {
        int nP = i + 1;
        BOOL bCo = (nP <= PARTNER_MAX_COUNT && PT_P(nP, PTP_USED) > 0);

        // 3 ky nang bam sinh (SK_GIVEN 1..3)
        for (j = 0; j < PARTNER_SKILL_GIVEN_COL; j++)
        {
            int nPack = bCo ? PT_P(nP, PTP_SK_GIVEN0 + j) : 0;
            if (nPack > 0)
                m_Given[i][j].HoldObject(CGOG_SKILL_FIGHT,
                    PT_SkillId(nPack), PT_SkillLv(nPack), 0);
            else
                m_Given[i][j].HoldObject(CGOG_NOTHING, 0, 0, 0);
        }
        // 6 o dau cua ky nang tu hoc (SK_LEARNT 1..6)
        for (j = 0; j < PARTNER_SKILL_GRID_COL; j++)
        {
            int nPack = bCo ? PT_P(nP, PTP_SK_LEARNT0 + j) : 0;
            if (nPack > 0)
                m_Learnt[i][j].HoldObject(CGOG_SKILL_FIGHT,
                    PT_SkillId(nPack), PT_SkillLv(nPack), 0);
            else
                m_Learnt[i][j].HoldObject(CGOG_NOTHING, 0, 0, 0);
        }
    }
}

int KUiPartnerSkill::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
        {
            CloseWindow();
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
        {
            CloseWindow();
            KUiPartnerAttr::OpenWindow();
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_SaveBtn)
        {
            if (m_nView >= 1 && m_nView != PT_Cur() && PT_P(m_nView, PTP_USED) > 0)
                g_pCoreShell->OperationRequest(GOI_PARTNER_OP,
                    PARTNER_OP_SELECT, m_nView);
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_DeleteBtn)
        {
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_DELETE, m_nView);
        }
        else if (uParam == (unsigned int)(KWndWindow*)&m_Forget)
        {
            g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_FORGETSKILL, m_nView);
        }
        else
        {
            for (int i = 0; i < PARTNER_UI_SLOT; i++)
            {
                if (uParam == (unsigned int)(KWndWindow*)&m_BtnTab[i])
                {
                    m_nView = i + 1;
                    UpdateData();
                    break;
                }
            }
        }
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}
'''.replace("__INI_SKILL__", INI_SKILL)

# ===========================================================================
# 6. UiPartnerBag.h
# ===========================================================================
bag_h = r'''/*******************************************************************************
File        : UiPartnerBag.h
create data : 27-08-2026
Description : [BDH-G4] TUI ban dong hanh 6x10 (__INI_BAG__).
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
'''.replace("__INI_BAG__", INI_BAG)

# ===========================================================================
# 7. UiPartnerBag.cpp
# ===========================================================================
CAP_FMT = vn("Cấp túi: %d")
bag_cpp = r'''/*******************************************************************************
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

#define SCHEME_INI_PARTNER_BAG "__INI_BAG__"

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
    sprintf(szBuf, "__CAP_FMT__", PT_TaskVal(PTG_BAGLEVEL));
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
'''.replace("__INI_BAG__", INI_BAG).replace("__CAP_FMT__", CAP_FMT)

# ===========================================================================
# 8. UiPartnerBar.h
# ===========================================================================
bar_h = r'''/*******************************************************************************
File        : UiPartnerBar.h
create data : 27-08-2026
Description : [BDH-G4] THANH NHANH ban dong hanh 9 nut (__INI_BAR__).
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
'''.replace("__INI_BAR__", INI_BAR)

# ===========================================================================
# 9. UiPartnerBar.cpp
# ===========================================================================
bar_cpp = r'''/*******************************************************************************
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

#define SCHEME_INI_PARTNER_BAR "__INI_BAR__"

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
'''.replace("__INI_BAR__", INI_BAR)

# ===========================================================================
# 10. UiPartnerCommon.cpp - dispatcher refresh
# ===========================================================================
common_cpp = r'''/*******************************************************************************
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
'''

ghi2("UiPartnerCommon.h", common_h)
ghi2("UiPartnerCommon.cpp", common_cpp)
ghi2("UiPartnerAttr.h", attr_h)
ghi2("UiPartnerAttr.cpp", attr_cpp)
ghi2("UiPartnerSkill.h", skill_h)
ghi2("UiPartnerSkill.cpp", skill_cpp)
ghi2("UiPartnerBag.h", bag_h)
ghi2("UiPartnerBag.cpp", bag_cpp)
ghi2("UiPartnerBar.h", bar_h)
ghi2("UiPartnerBar.cpp", bar_cpp)
print("XONG b10a: 10 tep")
