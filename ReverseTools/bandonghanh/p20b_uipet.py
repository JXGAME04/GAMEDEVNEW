# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] Cua so UiPet client (C++) - dung 100% bo cuc pet.ini VLTK.

Sinh S3Client\Ui\UiCase\UiPet.h/.cpp + dang ky:
 - ShortcutKey: "petmain" (case 40); autoexec Y -> petmain
 - UiPlayerBar PartnerIcon -> mo UiPet (thay bo mobile)
 - GameSpaceChangedNotify: refresh khi task value PET_TV_* ve
 - UiShell dong khi logout; vcxproj + filters
Du lieu: task value PET_TV_* (KPlayerPet.h) da tu sync. Nut gui
GOI_PARTNER_OP(PARTNER_OP_PETSYS, PET_OPERATION_*).
"""
import io
import os
import shutil

CR = chr(13)
LF = chr(10)
T = chr(9)
E = CR + LF
BS = chr(92)

UI = r"D:\GAMEDEVNEW\Sources\S3Client\Ui"
DST = os.path.join(UI, "UiCase")


def crlf(s):
    return s.replace(CR, "").replace(LF, E)


def ghi2(name, s):
    io.open(os.path.join(DST, name), "w", encoding="latin-1", newline="").write(crlf(s))
    print("ghi", name)


# ============================================================== UiPet.h
h = r'''/*******************************************************************************
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
};

#endif
'''

# ============================================================== UiPet.cpp
cpp = r'''/*******************************************************************************
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
        return;
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
'''

ghi2("UiPet.h", h)
ghi2("UiPet.cpp", cpp)


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, bak=".truoc_petsys"):
    if not os.path.exists(p + bak):
        shutil.copyfile(p, p + bak)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def patch(p, neo, moi, marker):
    s = doc(p)
    if marker in s:
        print("  da co:", os.path.basename(p), marker[:36])
        return
    assert s.count(neo) == 1, "anchor %d lan: %r" % (s.count(neo), neo[:60])
    ghi(p, s.replace(neo, moi, 1))
    print("  VA:", os.path.basename(p), marker[:36])


# ---- ShortcutKey: "petmain" case 40 ----
p = os.path.join(UI, "ShortcutKey.cpp")
patch(p,
      '#include "UiCase/UiPartnerCommon.h"' + T + "// [BDH-G4]" + E,
      '#include "UiCase/UiPartnerCommon.h"' + T + "// [BDH-G4]" + E +
      '#include "UiCase/UiPet.h"' + T + "// [PETSYS]" + E,
      'UiPet.h"' + T + "// [PETSYS]")
patch(p,
      T + '"partnerselect",	//39 [BDH-G4] doi con duong nhiem' + E,
      T + '"partnerselect",	//39 [BDH-G4] doi con duong nhiem' + E +
      T + '"petmain",	//40 [PETSYS] cua so Ban Dong Hanh ban PC' + E,
      '"petmain",')
patch(p,
      T + T + "case 39:" + E + T + T + T + "UiPartner_HotKey(4);" + E +
      T + T + T + "break;" + E,
      T + T + "case 39:" + E + T + T + T + "UiPartner_HotKey(4);" + E +
      T + T + T + "break;" + E +
      T + T + "case 40:	// [PETSYS]" + E +
      T + T + T + "if (KUiPet::GetIfVisible())" + E +
      T + T + T + T + "KUiPet::CloseWindow();" + E +
      T + T + T + "else" + E +
      T + T + T + T + "KUiPet::OpenWindow();" + E +
      T + T + T + "break;" + E,
      "case 40:	// [PETSYS]")

# ---- UiPlayerBar icon -> UiPet ----
p = os.path.join(UI, "UiCase", "UiPlayerBar.cpp")
s = doc(p)
if 'UiPet.h"' not in s:
    neo = '#include "UiPartnerBar.h"' + T + "// [BDH-G4]"
    assert s.count(neo) == 1
    s = s.replace(neo, neo + E + '#include "UiPet.h"' + T + "// [PETSYS]", 1)
    ghi(p, s)
    print("  VA: UiPlayerBar include UiPet")
s = doc(p)
cu = (T + T + T + "// [BDH-G4] mo/dong bo giao dien dong hanh (thanh nhanh + thuoc tinh)" + E +
      T + T + T + "if (KUiPartnerAttr::GetIfVisible() || KUiPartnerBar::GetIfVisible())" + E +
      T + T + T + "{" + E +
      T + T + T + T + "KUiPartnerAttr::CloseWindow();" + E +
      T + T + T + T + "KUiPartnerBar::CloseWindow();" + E +
      T + T + T + "}" + E +
      T + T + T + "else" + E +
      T + T + T + "{" + E +
      T + T + T + T + "KUiPartnerBar::OpenWindow();" + E +
      T + T + T + T + "KUiPartnerAttr::OpenWindow();" + E +
      T + T + T + "}" + E)
moi = (T + T + T + "// [PETSYS] icon mo cua so Ban Dong Hanh ban PC (UiPet)" + E +
       T + T + T + "if (KUiPet::GetIfVisible())" + E +
       T + T + T + T + "KUiPet::CloseWindow();" + E +
       T + T + T + "else" + E +
       T + T + T + T + "KUiPet::OpenWindow();" + E)
if cu in s:
    ghi(p, s.replace(cu, moi, 1))
    print("  VA: PartnerIcon -> UiPet")
else:
    print("  da doi icon (hoac anchor khac)" if "[PETSYS] icon" in s else "  !! icon anchor khong khop")

# ---- GameSpaceChangedNotify refresh ----
p = os.path.join(UI, "GameSpaceChangedNotify.cpp")
patch(p,
      T + T + "UiPartner_OnTaskValueChanged((int)uParam);" + T + "// [BDH-G4]" + E,
      T + T + "UiPartner_OnTaskValueChanged((int)uParam);" + T + "// [BDH-G4]" + E +
      T + T + "KUiPet::OnTaskValueChanged((int)uParam);" + T + "// [PETSYS]" + E,
      "KUiPet::OnTaskValueChanged")
s = doc(p)
if '#include "UiCase/UiPet.h"' not in s:
    neo = '#include "UiCase/UiPartnerBar.h"' + E
    assert s.count(neo) == 1
    ghi(p, s.replace(neo, neo + '#include "UiCase/UiPet.h"' + T + "// [PETSYS]" + E, 1))
    print("  VA: notify include UiPet")

# ---- UiShell dong khi logout ----
p = os.path.join(UI, "UiShell.cpp")
patch(p,
      T + "KUiPartnerBar::CloseWindow();" + E,
      T + "KUiPartnerBar::CloseWindow();" + E +
      T + "KUiPet::CloseWindow();" + T + "// [PETSYS]" + E,
      "KUiPet::CloseWindow();")
s = doc(p)
if '#include "UiCase/UiPet.h"' not in s:
    neo = '#include "UiCase/UiPartnerBar.h"' + E
    assert s.count(neo) == 1
    ghi(p, s.replace(neo, neo + '#include "UiCase/UiPet.h"' + T + "// [PETSYS]" + E, 1))
    print("  VA: UiShell include UiPet")

# ---- vcxproj + filters ----
p = r"D:\GAMEDEVNEW\Sources\S3Client\S3Client.vcxproj"
patch(p,
      '    <ClCompile Include="ui\\uicase\\UiPartnerBar.cpp" />' + E,
      '    <ClCompile Include="ui\\uicase\\UiPartnerBar.cpp" />' + E +
      '    <ClCompile Include="ui\\uicase\\UiPet.cpp" />' + E,
      "UiPet.cpp")
patch(p,
      '    <ClInclude Include="ui\\uicase\\UiPartnerBar.h" />' + E,
      '    <ClInclude Include="ui\\uicase\\UiPartnerBar.h" />' + E +
      '    <ClInclude Include="ui\\uicase\\UiPet.h" />' + E,
      "UiPet.h")
p = r"D:\GAMEDEVNEW\Sources\S3Client\S3Client.vcxproj.filters"
patch(p,
      '    <ClCompile Include="ui\\uicase\\UiPartnerBar.cpp">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClCompile>" + E,
      '    <ClCompile Include="ui\\uicase\\UiPartnerBar.cpp">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClCompile>" + E +
      '    <ClCompile Include="ui\\uicase\\UiPet.cpp">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClCompile>" + E,
      'uicase\\UiPet.cpp">')
patch(p,
      '    <ClInclude Include="ui\\uicase\\UiPartnerBar.h">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClInclude>" + E,
      '    <ClInclude Include="ui\\uicase\\UiPartnerBar.h">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClInclude>" + E +
      '    <ClInclude Include="ui\\uicase\\UiPet.h">' + E +
      "      <Filter>BASE_ELEMENT_CASE\\UICase</Filter>" + E +
      "    </ClInclude>" + E,
      'uicase\\UiPet.h">')

# ---- autoexec: Y -> petmain ----
p = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\autoexec.lua"
s = doc(p)
if "petmain" not in s:
    cu = 'AddCommand("Y", "", "Open([[partner]])")'
    assert s.count(cu) == 1
    ghi(p, s.replace(cu, 'AddCommand("Y", "", "Open([[petmain]])")' +
                     ' -- [PETSYS] he Ban Dong Hanh PC', 1), ".truoc_pet_y")
    print("  VA: autoexec Y -> petmain")
else:
    print("  da co: autoexec petmain")

print("XONG p20b")
