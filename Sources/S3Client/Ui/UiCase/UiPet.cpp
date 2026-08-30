/*******************************************************************************
File        : UiPet.cpp
create data : 28-08-2026
Description : [PETSYS] Cua so "Ban Dong Hanh" ban PC (xem UiPet.h).
*******************************************************************************/
#include "KWin32.h"
#include "../Elem/MouseHover.h"	// [30/08] tooltip o trang bi
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
    PET_OP_CHANGE_FEATURE, PET_OP_DELETE, PET_OP_LEVEL_UP, 8,	// 8 = XIUZHEN_POINT (ban private VLTK)
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
// [29/08] tra ImageName cua item theo ParticularType (bang magicscript client)
static void sPetItemImg(int nParticular, char* szOut, int nOutLen)
{
    szOut[0] = 0;
    static KTabFile s_ItemTab;
    static int s_bLoaded = 0;
    if (!s_bLoaded)
    {
        s_bLoaded = 1;
        s_ItemTab.Load((LPSTR)"\\settings\\item\\magicscript.txt");
    }
    char szNum[16];
    int nRow = s_ItemTab.GetHeight();
    for (int r = 2; r <= nRow; r++)
    {
        s_ItemTab.GetString(r, 4, (LPSTR)"", szNum, sizeof(szNum));
        if (atoi(szNum) == nParticular)
        {
            s_ItemTab.GetString(r, 5, (LPSTR)"", szOut, nOutLen);
            return;
        }
    }
}

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
    // [29/08] dung spr TINH offset-0 sinh rieng cho khung (spr npcres
    // goc co offset tam-chan lon -> KWndImage ve lech ra ngoai control)
    _snprintf(szOut, nOutLen - 1, "\\spr\\Ui3\\pet\\face\\%s.spr", szRes);
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
    // [30/08 phan bien] dai cu +39 cat mat o trang bi 8/9/10 (5150..5152)
    // va o bo (5163) -> mo rong +59
    if (nTaskId < PET_TV_BASE || nTaskId > PET_TV_BASE + 59)
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
    // [30/08] khung anh pet (40,303 260x120) TRUM len nut "Duc lai"
    // (235,390) - control them SAU nam TREN va nuot cu bam. Them
    // m_Appearance TRUOC de nut nam tren cung.
    AddChild(&m_Appearance);
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
            sprintf(szSec, "PetEquip_%d", i + 1);	// [29/08] 7 o hang TREN
            m_Equip[i].Init(&Ini, szSec);
        }
        m_Appearance.Init(&Ini, "AppearanceImg");
    }
    Wnd_AddWindow(this);
}

// [30/08] ten mon theo id (cot 1 bang magicscript)
static void sPetItemName(int nParticular, char* szOut, int nOutLen)
{
	szOut[0] = 0;
	static KTabFile s_TenTab;
	static int s_bLoaded = 0;
	if (!s_bLoaded)
	{
		s_bLoaded = 1;
		s_TenTab.Load((LPSTR)"\\settings\\item\\magicscript.txt");
	}
	// bang JX1: record index = particular -> hang = particular + 2
	s_TenTab.GetString(nParticular + 2, 1, (LPSTR)"", szOut, nOutLen);
}

// [30/08] Tooltip o trang bi: ten mon + 3 thuoc tinh (ma tu
// settings\petsys\equipattrib.txt, ten tu attribname.txt, gia tri tu
// task 5170 + slot*3). Dung g_MouseOver nhu UiPlayerBar.cpp:2429.
static KTabFile s_AttNameTab;
static int      s_bAttNameLoaded = 0;

static void sPetAttName(int nMa, char* szOut, int nOutLen)
{
	szOut[0] = 0;
	if (!s_bAttNameLoaded)
	{
		s_bAttNameLoaded = 1;
		s_AttNameTab.Load((LPSTR)"\\settings\\petsys\\attribname.txt");
	}
	char szNum[16];
	int nRow = s_AttNameTab.GetHeight();
	for (int r = 2; r <= nRow; r++)
	{
		s_AttNameTab.GetString(r, 1, (LPSTR)"", szNum, sizeof(szNum));
		if (atoi(szNum) == nMa)
		{
			s_AttNameTab.GetString(r, 2, (LPSTR)"", szOut, nOutLen);
			return;
		}
	}
	_snprintf(szOut, nOutLen - 1, "thuoc tinh %d", nMa);
}

static void sPetEquipTip(int nSlot, char* szOut, int nOutLen)
{
	szOut[0] = 0;
	int nId = sPetTV(5143 + nSlot);
	if (nId < 4907 || nId > 4926)
		return;
	char szTen[64];
	szTen[0] = 0;
	sPetItemName(nId, szTen, sizeof(szTen));
	int nLen = _snprintf(szOut, nOutLen - 1, "%s", szTen);
	if (nLen < 0) nLen = 0;
	// 3 thuoc tinh
	static KTabFile s_EqAttTab;
	static int s_bLoaded = 0;
	if (!s_bLoaded)
	{
		s_bLoaded = 1;
		s_EqAttTab.Load((LPSTR)"\\settings\\petsys\\equipattrib.txt");
	}
	char szNum[16];
	int nRow = s_EqAttTab.GetHeight();
	for (int r = 2; r <= nRow; r++)
	{
		s_EqAttTab.GetString(r, 1, (LPSTR)"", szNum, sizeof(szNum));
		if (atoi(szNum) != nId) continue;
		for (int c = 0; c < 3; c++)
		{
			s_EqAttTab.GetString(r, 2 + c, (LPSTR)"", szNum, sizeof(szNum));
			int nMa = atoi(szNum);
			int nVal = sPetTV(5170 + nSlot * 3 + c);
			if (nMa > 0 && nVal > 0 && nLen < nOutLen - 40)
			{
				char szTenAtt[48];
				sPetAttName(nMa, szTenAtt, sizeof(szTenAtt));
				nLen += _snprintf(szOut + nLen, nOutLen - nLen - 1,
					"\n%s +%d", szTenAtt, nVal);
			}
		}
		break;
	}
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

    // [29/08] 4 ky nang bi dong da hoc (task 5139..5142)
    // [29/08] 10 o trang bi Dong Hanh (task 5143..5152 = ParticularType).
    // Anh lay tu bang item (cot ImageName) - bang goc VLTK da port sang JX1.
    for (i = 0; i < PET_UI_EQUIP_NUM && i < 10; i++)
    {
        int nP = sPetTV(5143 + i);
        char szImg[128];
        szImg[0] = 0;
        // [30/08] CHI ve id trong DAI trang bi Dong Hanh (4907..4926 - bang
        // goc VLTK da port). O con giu id he tu che cu (4881..) thi bo qua,
        // neu khong se ve nham anh cua item JX1 khac trung id.
        if (nP >= 4907 && nP <= 4926)
            sPetItemImg(nP, szImg, sizeof(szImg));
        if (szImg[0])
        {
            m_Equip[i].SetImage(ISI_T_SPR, szImg);
            m_Equip[i].Show();
        }
        else
        {
            m_Equip[i].SetImage(ISI_T_SPR, (char*)"");
            m_Equip[i].Hide();
        }
    }

    for (i = 0; i < PET_UI_EXTSKILL_NUM && i < 4; i++)
    {
        int nSk = sPetTV(5139 + i);
        if (nSk > 0)
            m_ExtSkill[i].HoldObject(CGOG_SKILL_FIGHT, nSk, 1, 0);
        else
            m_ExtSkill[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
    }
}

// [29/08] framework goi Breathe() cho cua so visible (WndWindow.cpp:367)
// -> quay frame hinh pet ~180ms de dung tho nhu ngoai map
void KUiPet::Breathe()
{
    unsigned int uNow = IR_GetCurrentTime();
    if (uNow - m_uLastFrameTime < 180)
        return;
    m_uLastFrameTime = uNow;
    if (m_Appearance.IsVisible())
        m_Appearance.NextFrame();
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
        if (uParam == (unsigned int)(KWndWindow*)&m_CompanionBtn)
            SendOp(10);	// menu trang bi Dong Hanh (server)
        break;
    case WM_MOUSEMOVE:
    {
        // [30/08] tooltip o trang bi: toa do 10 o trong INI (x 328 + k*30,
        // y 82, 24x24) - uParam/nParam la toa do chuot tuyet doi
        int nX = (int)uParam - m_nAbsoluteLeft;
        int nY = nParam - m_nAbsoluteTop;
        int nO = -1;
        if (nY >= 82 && nY <= 106)
        {
            for (int k = 0; k < 10; k++)
            {
                int nL = 328 + k * 30;
                if (nX >= nL && nX <= nL + 24)
                {
                    nO = k;
                    break;
                }
            }
        }
        if (nO >= 0 && sPetTV(5143 + nO) >= 4907)
        {
            char szTip[256];
            sPetEquipTip(nO, szTip, sizeof(szTip));
            if (szTip[0])
            {
                g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, nO,
                    (int)uParam, nParam, false, true);
                g_MouseOver.SetMouseHoverTitle(szTip, strlen(szTip), 0xffffffff);
            }
        }
        else if (g_MouseOver.IsMoseHoverWndObj((void*)(KWndWindow*)this, -1) == 0)
        {
            g_MouseOver.CancelMouseHoverInfo();
        }
        break;
    }
    default:
        return KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}
