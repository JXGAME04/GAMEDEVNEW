/*******************************************************************************
File        : UiPartnerAttr.cpp
create data : 27-08-2026
Description : [BDH-G4] Cua so THUOC TINH ban dong hanh.
  27 truong hien thi bam dung ten muc trong \xCD\xAC\xB0\xE9\xCA\xF4\xD0\xD4.ini cua ban Linux
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

#define SCHEME_INI_PARTNER_ATTR "\xCD\xAC\xB0\xE9\xCA\xF4\xD0\xD4.ini"

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

static const char* s_szSeries[PARTNER_SERIES_NUM] = { "Kim", "Méc", "Thñy", "Háa", "Thæ" };
static const char* s_szCharacter[PARTNER_CHARACTER_NUM + 1] = { "-", "Dòng m·nh", "VÖ chñ", "L­u manh", "Nhu nh­îc" };

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
