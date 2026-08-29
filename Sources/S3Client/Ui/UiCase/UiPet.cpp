/*******************************************************************************
File        : UiPet.cpp
create data : 28-08-2026
Description : [PETSYS] Cua so "Ban Dong Hanh" ban PC (xem UiPet.h).
*******************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/KPlayerPet.h"
#include "../../../core/src/KProtocol.h"    // PARTNER_OP_* (KPartnerProtocol.h)
#include "UiPet.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"

extern iCoreShell* g_pCoreShell;

#define SCHEME_INI_PET  "pet_main.ini"

// PET_OPERATION_* (script\petsys\common.lua cua ban Linux)
#define PET_OP_DELETE           1
#define PET_OP_SUMMON           2
#define PET_OP_UNSUMMON         3
#define PET_OP_LEVEL_UP         4
#define PET_OP_CHANGE_NAME      5
#define PET_OP_CHANGE_FEATURE   6
#define PET_OP_TAME             7

KUiPet* KUiPet::ms_pSelf = NULL;

// 8 nut: section -> operation (0 = chua mo)
static const char* s_szBtnSec[PET_UI_BTN_NUM] =
{
    "TameBtn", "SummonBtn", "ChangeNameBtn", "UnSummonBtn",
    "ChangeFeatureBtn", "DeleteBtn", "LevelUpBtn", "XiuzhenBtn",
};
static const int s_nBtnOp[PET_UI_BTN_NUM] =
{
    PET_OP_TAME, PET_OP_SUMMON, PET_OP_CHANGE_NAME, PET_OP_UNSUMMON,
    PET_OP_CHANGE_FEATURE, PET_OP_DELETE, PET_OP_LEVEL_UP, 0,
};

// 12 nhan + 12 o gia tri (Txt co Text= san trong ini - tieng Viet nguyen ban)
static const char* s_szTxtSec[PET_UI_TXT_NUM] =
{
    "UpgradeTxt", "GrownTxt", "TameTxt", "XiuzhenTxt", "NameTxt", "LevelTxt",
    "StrTxt", "DexTxt", "VitTxt", "EngTxt", "LifeTxt", "ManaTxt",
};
static const char* s_szValSec[PET_UI_TXT_NUM] =
{
    "UpgradeValueTxt", "GrownValueTxt", "TameValueTxt", "XiuzhenValueTxt",
    "NameValueTxt", "LevelValueTxt",
    "StrValueTxt", "DexValueTxt", "VitValueTxt", "EngValueTxt",
    "LifeValueTxt", "ManaValueTxt",
};

// tra duong anh DUNG (spr/npcres/<nhom>/<res>/<res>_st01.spr) theo template.
// Doc settings/npcs.txt (bang client) cot 12 NpcResType; nhom = phan CHU dau
// cua res (boss018 -> boss).
#include "KTabFile.h"
static void sPetResPath(int nTpl, char* szOut, int nOutLen)
{
    szOut[0] = 0;
    static KTabFile s_NpcTab;
    static BOOL s_bLoaded = FALSE;
    if (!s_bLoaded)
    {
        s_bLoaded = TRUE;
        s_NpcTab.Load((LPSTR)"\\settings\\npcs.txt");
    }
    char szRes[64];
    szRes[0] = 0;
    // data dau cua KTabFile = row 2 (row 1 la header) => id N = row N + 2
    s_NpcTab.GetString(nTpl + 2, 12, (LPSTR)"", szRes, sizeof(szRes));
    if (!szRes[0] || szRes[0] == '0')
        return;
    char szNhom[64];
    int i = 0;
    while (szRes[i] && (szRes[i] < '0' || szRes[i] > '9') && i < 60)
    {
        szNhom[i] = szRes[i];
        i++;
    }
    szNhom[i] = 0;
    if (!szNhom[0])
        return;
    _snprintf(szOut, nOutLen - 1, "\\spr\\npcres\\%s\\%s\\%s_st01.spr",
        szNhom, szRes, szRes);
    szOut[nOutLen - 1] = 0;
}

static int sPetTV(int nId)
{
    return g_pCoreShell ? g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, nId, 0) : 0;
}

KUiPet::KUiPet()
{
}

KUiPet::~KUiPet()
{
}

KUiPet* KUiPet::GetIfVisible()
{
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

KUiPet* KUiPet::OpenWindow()
{
    if (ms_pSelf == NULL)
    {
        ms_pSelf = new KUiPet;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf)
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->UpdateData();
        ms_pSelf->BringToTop();
        ms_pSelf->Show();
    }
    return ms_pSelf;
}

void KUiPet::CloseWindow()
{
    if (ms_pSelf)
    {
        ms_pSelf->Hide();
        ms_pSelf = NULL;
    }
}

void KUiPet::OnTaskValueChanged(int nTaskId)
{
    if (nTaskId < PET_TV_BASE || nTaskId > PET_TV_BASE + 39)
        return;
    if (ms_pSelf && ms_pSelf->IsVisible())
        ms_pSelf->UpdateData();
}

void KUiPet::Initialize()
{
    int i;
    AddChild(&m_CloseBtn);
    for (i = 0; i < PET_UI_BTN_NUM; i++)
        AddChild(&m_Btn[i]);
    AddChild(&m_CompanionBtn);
    for (i = 0; i < PET_UI_TXT_NUM; i++)
    {
        AddChild(&m_Txt[i]);
        AddChild(&m_Val[i]);
    }
    AddChild(&m_Skill1);
    for (i = 0; i < PET_UI_EXTSKILL_NUM; i++)
        AddChild(&m_ExtSkill[i]);
    for (i = 0; i < PET_UI_EQUIP_NUM; i++)
        AddChild(&m_Equip[i]);
    AddChild(&m_Appearance);

    char Scheme[256];
    KIniFile Ini;
    g_UiBase.GetCurSchemePath(Scheme, 256);
    strcat(Scheme, "\\" SCHEME_INI_PET);
    if (Ini.Load(Scheme))
    {
        Init(&Ini, "Main");
        m_CloseBtn.Init(&Ini, "CloseBtn");
        for (i = 0; i < PET_UI_BTN_NUM; i++)
            m_Btn[i].Init(&Ini, s_szBtnSec[i]);
        m_CompanionBtn.Init(&Ini, "CompanionBtn");
        char szSec[32];
        for (i = 0; i < PET_UI_TXT_NUM; i++)
        {
            m_Txt[i].Init(&Ini, s_szTxtSec[i]);
            m_Txt[i].SetTextColor(0xFFFFFFFF);
            m_Val[i].Init(&Ini, s_szValSec[i]);
            m_Val[i].SetSize(120, 16);
            m_Val[i].SetTextColor(0xFFFFF07D);
        }
        m_Skill1.Init(&Ini, "Skill_1");
        for (i = 0; i < PET_UI_EXTSKILL_NUM; i++)
        {
            sprintf(szSec, "Ext_Skill_%d", i + 1);
            m_ExtSkill[i].Init(&Ini, szSec);
        }
        for (i = 0; i < PET_UI_EQUIP_NUM; i++)
        {
            sprintf(szSec, "Equip_%d", i + 1);
            m_Equip[i].Init(&Ini, szSec);
        }
        m_Appearance.Init(&Ini, "AppearanceImg");
    }
    Wnd_AddWindow(this);
}

void KUiPet::UpdateData()
{
    char szBuf[64];
    int bCo = (sPetTV(PET_TV_CREATE) == 1);

    // 4 diem
    static const int nPointId[4] =
        { PET_TV_UPGRADE, PET_TV_GROWN, PET_TV_TAME, PET_TV_XIUZHEN };
    int i;
    for (i = 0; i < 4; i++)
    {
        sprintf(szBuf, "%d", sPetTV(nPointId[i]));
        m_Val[i].SetText(szBuf);
    }

    if (!bCo)
    {
        m_Val[4].SetText("-");
        for (i = 5; i < PET_UI_TXT_NUM; i++)
            m_Val[i].SetText("-");
        m_Skill1.HoldObject(CGOG_NOTHING, 0, 0, 0);
        m_Appearance.Hide();
        return;
    }

    // hinh pet dung trong khung AppearanceImg (theo ngoai quan hien tai)
    {
        char szSpr[256];
        sPetResPath(sPetTV(PET_TV_FEATURE), szSpr, sizeof(szSpr));
        if (szSpr[0])
        {
            m_Appearance.SetImage(ISI_T_SPR, szSpr);
            m_Appearance.Show();
        }
        else
            m_Appearance.Hide();
    }

    // ten 16B trong 4 int
    char szName[20];
    int* p = (int*)szName;
    for (i = 0; i < 4; i++)
        p[i] = sPetTV(PET_TV_NAME0 + i);
    szName[16] = 0;
    m_Val[4].SetText(szName[0] ? szName : "-");

    int nLevel = sPetTV(PET_TV_LEVEL);
    sprintf(szBuf, "%d", nLevel);
    m_Val[5].SetText(szBuf);

    // 6 thuoc tinh: STR DEX VIT ENG HP MP
    for (i = 0; i < 6; i++)
    {
        sprintf(szBuf, "%d", sPetTV(PET_TV_ATTRIB0 + i));
        m_Val[6 + i].SetText(szBuf);
    }

    // icon aura: loai 1..4 -> skill 1600..1603, cap = cap pet
    int nKind = sPetTV(PET_TV_SKILL0);
    if (nKind >= 1 && nKind <= 4 && nLevel >= 1)
        m_Skill1.HoldObject(CGOG_SKILL_FIGHT, PET_AURA_SKILL0 + nKind - 1,
            nLevel, 0);
    else
        m_Skill1.HoldObject(CGOG_NOTHING, 0, 0, 0);
}

void KUiPet::SendOp(int nOp)
{
    if (nOp <= 0)
        return;
    g_pCoreShell->OperationRequest(GOI_PARTNER_OP, PARTNER_OP_PETSYS, nOp);
}

int KUiPet::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
    switch (uMsg)
    {
    case WND_N_BUTTON_CLICK:
        if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
        {
            CloseWindow();
            break;
        }
        for (int i = 0; i < PET_UI_BTN_NUM; i++)
        {
            if (uParam == (unsigned int)(KWndWindow*)&m_Btn[i])
            {
                SendOp(s_nBtnOp[i]);
                break;
            }
        }
        // m_CompanionBtn ("Duc lai") - he trang bi pet chua mo (dot sau)
        break;
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}
